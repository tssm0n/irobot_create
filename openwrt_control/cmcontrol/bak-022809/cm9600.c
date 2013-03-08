//#define CM_LOOPBACK_TEST
/*-------------------------------------------------------------------
 * cm9600.c / cm9600.h
 * Designed to run on Create Command Module wih ATMega168 MCU
 *
 * John A. Qualtrough
 *-------------------------------------------------------------------
 * Transmit data via simulated UART port using Port B discrete
 * Data sent in 10-bit format 1 Start,8 Data,No Parity,1 Stop
 * Process uses TIMER 2 and Port B bit 1, 2, or 3
 *-------------------------------------------------------------------
 *
 *
 * ====================== REVISION HISTORY ==========================
 *
 * 1.00	06-08-2007	John Qualtrough Created
 *
 * Added send_cm_line to transmit char array in background.
 * Added predefined PORT and BAUD settings to cm9600.h
 *-------------------------------------------------------------------
 *
 * 2.00 06-10-2007  John Qualtrough Update
 *
 * Interrupt turned on/off as needed for transmission
 * Ring values checked/reset in done-bit state.
 *-------------------------------------------------------------------
 *
 * 2.10 06-11-2007  John Qualtrough Update
 *
 * Clear Timer 2 count register after enabling interrupts.
 *-------------------------------------------------------------------
 *
 * 2.11 06-11-2007  John Qualtrough Update
 *
 * Added local start_ctc2() and stop_ctc2() functions.
 *-------------------------------------------------------------------
 *
 * 2.12 06-12-2007  John Qualtrough Update
 *
 * Expanded line buffer from 32 to 48 bytes
 * Removed redundant state from transmit state machine
 * Simplified cm_UART_busy() logic
 *-------------------------------------------------------------------
 *
 * 3.00 06-16-2007  John Qualtrough Update
 *
 * Added full-duplex (receive) capability on ePort pin 2 (PortC)
 *-------------------------------------------------------------------
 *
 * 3.10 06-16-2007  John Qualtrough Update
 *
 * Added receive ring buffer.
 *-------------------------------------------------------------------
 *
 * 3.11 06-18-2007  John Qualtrough Update
 *
 * Transmitter bug fix
 *-------------------------------------------------------------------
 *
 * 3.20 06-20-2007  John Qualtrough Update
 *
 * Changed receiver sample time to startbit interrupt+1
 * Added cm_rcv_enable() and cm_rcv_disable() functions.
 * Eliminated call to strlen()
 *-------------------------------------------------------------------
 *
 * 4.0  06-22-2007  John Qualtrough Update
 *
 * Added capability to have 3 ports open at once
 * Changed function names to use "cm_" prefix
 *-------------------------------------------------------------------
 *
 * 4.1 07-11-2007  John Qualtrough Update
 *
 * Changed gXring to array of pointers.
 * Pointers are to user allocated char arrays which removed the
 * 48 char limit, and saves SRAM.  User must not change the array
 * until it is fully transmitted.
 *-------------------------------------------------------------------
 */

//-----------------
//    Includes
//-----------------
#include <avr/interrupt.h>
#include <string.h>
#include "cm9600.h"

//-----------------
//    Constants
//-----------------
// Simulated UART States
enum{ SEND_DONE, SEND_START, SEND_DATA, SEND_STOP, STATE_INIT };
enum{ RCV_IDLE,  RCV_RDY };

//------------------------------
//   DEFINE ePORT SETTINGS
//------------------------------
#define CM_BIT_CARGO  0
#define CM_BIT_CENTER 1
#define CM_BIT_RIGHT  2
#define CM_BIT_LEFT   3

//------------------------------
//  DEFINE BAUD RATE SETTINGS
//  W/RCVR INTS RUN 3 x BAUD
//------------------------------
#define CM_TIMER_RATE  9
#define MAX_CM_RBUF   16
#define CM_PORTS       3

//-----------------
//     Common
//-----------------
static volatile uint8_t gBaudRate[CM_PORTS] = {0,0,0};
static volatile uint8_t gPortBit[CM_PORTS]  = {0,0,0};
static volatile uint8_t gPortsOpen = 0;   // Number of open ports 0-3

//-----------------
//   Transmitter
//-----------------
static volatile char    *gXring[CM_PORTS];
static volatile uint8_t gUartXmt[CM_PORTS];
static volatile uint8_t gXmtState[CM_PORTS]  = {SEND_DONE,SEND_DONE,SEND_DONE};
static volatile uint8_t gBitnum[CM_PORTS]    = {1,1,1};
static volatile uint8_t gXringNdx[CM_PORTS]  = {0,0,0};

//-----------------
//    Receiver
//-----------------
static volatile char    gRring[CM_PORTS][MAX_CM_RBUF];
static volatile uint8_t gRhead[CM_PORTS] = {0,0,0};
static volatile uint8_t gRtail[CM_PORTS] = {0,0,0};
static volatile uint8_t gRcvrEnabled[CM_PORTS] = {1,1,1};

//-------------------------------------------------
//       Init timer 2 and port B output pin
//     for user selected baud aux UART output
//  NOTE: See port and baud settings in cm9600.h
//-------------------------------------------------
uint8_t cm_init_UART(uint8_t port, uint8_t brate)
{  
//-----------------------
// Init State Variables
//-----------------------
  switch(port) // Get actual bit position based on index
  {
    case CM_EPORT_LEFT:
      gPortBit[gPortsOpen] = CM_BIT_LEFT;
      break;

    case CM_EPORT_RIGHT:
      gPortBit[gPortsOpen] = CM_BIT_RIGHT;
      break;

    case CM_EPORT_CARGO:
      gPortBit[gPortsOpen] = CM_BIT_CARGO;
      break;

    default:
      gPortBit[gPortsOpen] = CM_BIT_CENTER;
      break;
  }

  gBaudRate[gPortsOpen] = brate;      // Modulo of 57600
  gXringNdx[gPortsOpen] = 0;
  gXmtState[gPortsOpen] = SEND_DONE;  // ISR state maching init
  gBitnum[gPortsOpen]   = 0x01;       // Prepare for MSB first order
  gRhead[gPortsOpen]    = 0;
  gRtail[gPortsOpen]    = 0;
  cm_rcv_enable(gPortsOpen);          // Start with receiver enabled.
  
//----------------------------------
// Port transmit line configuration
//----------------------------------  
  cli();  // Disable ints while we configure the hardware
  
  DDRB  |= _BV(gPortBit[gPortsOpen]);  // setup desired PORTB bit as output
  PORTB |= _BV(gPortBit[gPortsOpen]);  // Set logic level to high "Mark"

//----------------------------------
// Port receive line configuration
//----------------------------------   
  DIDR0 &= (0x3F^_BV(gPortBit[gPortsOpen])); // Enable PORTC input buffer
  DDRC  &= ~_BV(gPortBit[gPortsOpen]);       // Set PORTC bit for input.
  PORTC |= _BV(gPortBit[gPortsOpen]);        // Enable pull-up
  
//----------------------------------------------------
//   Set up timer 2 for 57600 interrupt rate
//   Lower bauds may then be derived from that rate
//      CTC mode with divide by 32 prescaler
//   Atmel Doc Rev. 2545K–AVR–04/07 pages 153-158
//----------------------------------------------------
  if (gPortsOpen==0) // Only setup timer once
  {
    TCCR2A = _BV(WGM21);        // Configure CTC mode
    OCR2A  = CM_TIMER_RATE;     // 18432000/(32*57600) = 10 (So OCR2A = 9)
   //-----------------
   // Enable Timer 2
   //-----------------  
    TCNT2  =  0;                      // Start 1 bit period after enabled.
    TCCR2B = _BV(CS21) | _BV(CS20);   // Start clock divide by 32
    TIMSK2 = _BV(OCIE2A);             // Enable terminal count irq.
  }
  
  gPortsOpen++;
  
  sei();  // Re-enable interrupts, and we're done.
  
  return(gPortsOpen-1); // Return handle (index) to this port
}

//-------------------------------------------------
//        Send single byte via "soft" UART
//-------------------------------------------------
void cm_ByteXmt(uint8_t port, uint8_t value)
{
  sei(); // Be sure interrupts are enabled
  while(cm_UART_busy(port)) // Wait while xmit in progress.
  {
    asm("nop"); // I don't like empty wait loops.
  }
  
  cli();
  gUartXmt[port]  = value;        // Set data byte to be sent.
  gXmtState[port] = SEND_START;   // Activate transmitter state machine.
  sei();
}

//---------------------------------------------------------
//  Send user NUL terminated (ASCIZ) string out aux UART
//---------------------------------------------------------
void cm_send_line(uint8_t port, char *ibuf)
{
  uint8_t len=0;
  
  sei();
  while(cm_UART_busy(port))  // Wait if string is in queue.
  {
    asm("nop");
  }
  
  gXring[port]=&ibuf[1]; // 1st char is sent below.
  while(*(gXring[port]+len))                       // Equivalent to strlen()
  {
    len++;
  }
  cli();
  gXringNdx[port] = len;                          // Pass length to ISR
  gUartXmt[port]  = ibuf[0];                      // Queue up first character.
  gXmtState[port] = SEND_START;                   // Activate transmit state machine.
  sei();
}

//----------------------------------------------------
//   Return busy status of aux UART transmitter
//----------------------------------------------------
uint8_t cm_UART_busy(uint8_t port)
{
  return(gXmtState[port] != SEND_DONE);
}

//----------------------------------------------------
//    Returns number of chars waiting to be read
//----------------------------------------------------
uint8_t cm_char_rdy(uint8_t port)
{
  if (gRhead[port] >= gRtail[port])
     return (gRhead[port]-gRtail[port]);
  else
     return (MAX_CM_RBUF - (gRtail[port]-gRhead[port]));  
}

//----------------------------------------------------
//          Return next char in rcv FIFO
//----------------------------------------------------
char cm_read_char(uint8_t port)
{
  char temp;
  
  temp = gRring[port][gRtail[port]++];
  if (gRtail[port] >= MAX_CM_RBUF) gRtail[port]=0;
  return temp;
}
    
//----------------------------------------------------
//          Enable Receiver
//----------------------------------------------------
void cm_rcv_enable(uint8_t port)
{
  gRcvrEnabled[port] = 1;
}
    
//----------------------------------------------------
//          Disable Receiver
//----------------------------------------------------
void cm_rcv_disable(uint8_t port)
{
  gRcvrEnabled[port] = 0;
}
    
//-------------------------------------------------
//    Timer 2 ISR runs at 3X the Baud Rate
// Transmits character in 10-bit UART protocol
//      1 Start, 8 data, No parity, 1 stop.
//-------------------------------------------------
ISR(TIMER2_COMPA_vect)
{  
  uint8_t i=0;
  static volatile uint8_t ringNdx[CM_PORTS] = {0,0,0};
  static volatile uint8_t irqcnt [CM_PORTS] = {0,0,0};
  static volatile int8_t  rcving [CM_PORTS] = {0,0,0};
  static volatile uint8_t bittime[CM_PORTS] = {0,0,0};
  static volatile uint8_t rcvbyte[CM_PORTS] = {0,0,0};


  for(i=0; i<gPortsOpen; i++)
  {
    if (++irqcnt[i] >= gBaudRate[i]) irqcnt[i]=0; // Faster than modulo
    
//==============
//   RECEIVE
//==============
    if (gRcvrEnabled[i]) // Has user disabled receiver?
    {
      if (rcving[i])
      {
        if (irqcnt[i] == bittime[i])
        {
          rcvbyte[i] >>= 1;
          if (PINC & _BV(gPortBit[i])) rcvbyte[i] |= 0x80;
          rcving[i]++;
        
          if (rcving[i] >= 10) // Discard the start-bit
          {
            rcving[i] = 0;
            gRring[i][gRhead[i]++] = rcvbyte[i];
            if (gRhead[i] >= MAX_CM_RBUF) gRhead[i] = 0;
          }
        }
      }
      else if ((PINC & _BV(gPortBit[i])) == 0) // Start bit?
      {
        bittime[i] = irqcnt[i]+(gBaudRate[i]>>1); // When to sample subsequent bits
        while(bittime[i]>=gBaudRate[i])           // Same as a modulo "%"
          bittime[i] -= gBaudRate[i];
        rcving[i]  = 1;         // Boolean and bit count
        rcvbyte[i] = 0;         // Reset received byte
      }
    }
    else // Keep receiver state var reset while disabled.
    {
      rcving[i]  = 0;
    }

//==============
//   TRANSMIT
//==============
    if (irqcnt[i]==0)  // Only process transmit at Baud rate.
    {
      switch (gXmtState[i])
      {
        case SEND_START:    // Send single low start bit (Space)
           PORTB &= ~_BV(gPortBit[i]);
           gXmtState[i] = SEND_DATA;
           gBitnum[i] = 0x01;  // Data will be LSB first.
           break;

        case SEND_DATA:
           if (gUartXmt[i] & gBitnum[i]) // Send a 1 or a 0?
             PORTB |= _BV(gPortBit[i]);
           else
             PORTB &= ~_BV(gPortBit[i]);

           if (gBitnum[i] & 0x80) // If the MSB was sent, next is Stop bit.
             gXmtState[i] = SEND_STOP;
           else
             gBitnum[i] <<= 1; // Otherwise, send next bit at next bit time.
           break;

        case SEND_STOP: // Stop bit : "Marking" state for 1 bit time.
           PORTB |= _BV(gPortBit[i]);
           gBitnum[i] = 0x01;
           if (gXringNdx[i])  // See if a char array has been queued.
           {
             gUartXmt[i]  = *(gXring[i]+ringNdx[i]); // Set data byte to be sent.
             ringNdx[i]++;
             gXmtState[i] = SEND_START;        // Activate transmitter state machine.
             gXringNdx[i]--;                   // Decrement char count.
           }
           else // All done. Clean up and disable timer2 interrupts.
           {
             ringNdx[i] = 0;
             gXmtState[i] = SEND_DONE;
           }
           break;

        default: // Unknown state and STATE_INIT. Set vars to init values.
          break;
      }//end switch
    }// end if irqcnt==0

  }// end for i<gPortsOpen

}

//========================================= 
//              LOCAL FUNCTIONS 
//=========================================
// NONE.

/*__________________________________________________________________________________________________________________

                              >>><<<  GENERAL UART PROTOCOL TECHNICAL NOTES  >>><<<<
____________________________________________________________________________________________________________________

In Async UART protocol a value of 1 is called a Mark and a value of 0 is called a Space.
When a serial line is idle, the line is said to be “Marking”, or transmitting continuous 1 values.
The Start bit always has a value of 0 (a Space). A start bit is sent before each character.
After the Start Bit, the individual bits of the byte of data are sent, LSB first.
After 8 data bits are transmitted, the Stop Bit is sent. Stop bit always has a value of 1 (a Mark).
This means that there will always be a Mark (1) to Space (0) transition on the line at the start of every byte.
This guarantees that sender and receiver can resynchronize their clocks regardless of the data being transmitted.
____________________________________________________________________________________________________________________*/
#ifdef CM_LOOPBACK_TEST

#include <stdio.h>
#include "..\creator\oi.h"
int main(void)
{
   volatile uint32_t counter;
   volatile uint8_t outc = 64;
   volatile uint8_t inch = '0';
   volatile uint8_t buf[32];
   
   cm_init_UART(CM_EPORT_CENTER,CM_BAUD_19200);
   while(1)
   {
       cm_rcv_enable(CM_EPORT_CENTER);
       counter = 100000;
       outc++;
       cm_ByteXmt(CM_EPORT_CENTER,outc);
       
       while(counter != 0) // Delay
       {
          counter--;
       }
       while(cm_char_rdy(CM_EPORT_CENTER))
       {
         inch = cm_read_char(CM_EPORT_CENTER);
       }
       cm_rcv_disable(CM_EPORT_CENTER);
       sprintf((char*)buf,"?fSENT: %c  REC: %c",outc,inch);
       cm_send_line(CM_EPORT_CENTER,(char*)buf);       
       counter = 100000;
       while(counter != 0) // Delay
       {
          counter--;
       }
       if (outc>='Z') outc='A'-1;
   }
}
#endif
