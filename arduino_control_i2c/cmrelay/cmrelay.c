
// Includes
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include "oi.h"
#include "cm9600.h"

#define RECEIVE_FROM_BOT_BUFFER 64

// Global variables
volatile uint16_t timer_cnt = 0;
volatile uint8_t timer_on = 0;
volatile uint8_t receiveIndex = 0;
volatile uint8_t receiveBuffer[RECEIVE_FROM_BOT_BUFFER];
volatile uint8_t myport = 0;
volatile char prevVal = 255;
volatile uint8_t localAckIndex = 0;
volatile char localAck = 0;
volatile uint8_t localSensorCheck = 0;
volatile uint8_t localSensorData = 0;
volatile uint8_t localSensorCounter = 0;

// Functions
void byteTx(uint8_t value);
void delayMs(uint16_t time_ms);
void initialize(void);
void powerOnRobot(void);
void baud(uint8_t baud_code);
void relayCommand(void);
void checkIr(void);
uint8_t checkLocalAck(uint8_t nextChar);
void handleLocalCommand(uint8_t value);
uint16_t readIRSensor(void);
void writeChar(char c, uint8_t com);
uint8_t getSerialDestination(void);
void setSerial(uint8_t com) ;


int main (void) 
{
	initialize();
	// Needed for IR Remote
  powerOnRobot();
  byteTx(CmdStart);
  byteTx(CmdSafe);	
    // End
	
	LEDBothOff;
	LED1On;
	for(;;){
		// Relay commands
		relayCommand();

		if(UserButtonPressed){
			//Stop
			byteTx(145);
			byteTx(0);
			byteTx(0);
			byteTx(0);
			byteTx(0);
			//Passive Mode
			byteTx(128);
		}
			
		//checkIr(); // Check for input from IR remote control
	}
}

void relayCommand(){
	char inval;
	while(cm_char_rdy(myport)){
		inval = cm_read_char(myport);
		if(inval != 0xFF || prevVal >= 0x80){
			if(!localAck){
				byteTx(inval);		
				//cm_ByteXmt(myport, inval);
			} else {
				handleLocalCommand(inval);
			}
		}
		prevVal = inval;

		if(checkLocalAck(inval)){
			localAck = 1;
		}
	}

	if(receiveIndex > 0){
		uint8_t txIndex = 0;
		for(txIndex = 0; txIndex < receiveIndex; txIndex++)
			cm_ByteXmt(myport, receiveBuffer[txIndex]);
		receiveIndex = 0;
	}
}

// Timer 1 interrupt to time delays in ms
SIGNAL(SIG_OUTPUT_COMPARE1A)
{
  if(timer_cnt)
    timer_cnt--;
  else
    timer_on = 0;
}

// Serial receive interrupt to store sensor values
SIGNAL(SIG_USART_RECV)
{
  uint8_t temp;

  temp = UDR0;

  if(localSensorCheck){
  	localSensorData = temp;
	localSensorCheck = 0;
  } else {
  	if(receiveIndex < RECEIVE_FROM_BOT_BUFFER)
  		receiveBuffer[receiveIndex++] = temp;
  }
}


// Transmit a byte over the serial port
void byteTx(uint8_t value)
{
  while(!(UCSR0A & _BV(UDRE0))) ;
  UDR0 = value;
}


// Check to see if this character is the next in the local ACK sequence
uint8_t checkLocalAck(uint8_t nextChar)
{
  if((localAckIndex % 2 == 0 && nextChar == 151) ||
     (localAckIndex % 2 == 1 && nextChar == 137))
  {
	localAckIndex++;
  } else {
  	localAckIndex = 0;
  }

  if(localAckIndex == 6){
  	return 1;
  } else {
  	return 0;
  }
}

void handleLocalCommand(uint8_t value){
	uint16_t irResult;
	uint8_t irMsb;
	uint8_t irLsb;

	switch(value){
		case 1:
			LED2On;
			break;
		case 2:
			LED2Off;
			break;
		case 3:
			// Read the IR sensor
			irResult = readIRSensor();
			irMsb = irResult >> 8;
			irLsb = irResult & 0x00FF;
			cm_ByteXmt(myport, irMsb);
			cm_ByteXmt(myport, irLsb);
			break;
		}

	// Turn localAck off so that commands are relayed again
	localAck = 0;
}

uint16_t readIRSensor(){

	uint16_t meas_c5;
	
	//ADMUX &= ~0x0F; // Clear the ADC channel
	//ADMUX |= 0x05; // Set the ADC channel to C5
	ADMUX = (_BV(REFS0) | 0x05); // set voltage reference, select channel C5
	ADCSRA |= 0x40; // Start the measurement
	while(ADCSRA &0x40); // Wait until it's done
	meas_c5 = ADC; // Save the result

	if(meas_c5 > 200){
		LED1On;
	}

	return meas_c5;
}

void checkIr(){
		// 133 Is the text value...
	if(localSensorData == 133){
		LED2On;
	}

		// Only check every 30 iterations
	if(localSensorCounter++ == 30){
		localSensorCheck = 1;
		byteTx(142);
		byteTx(17);
		localSensorCounter = 0;
	}

}

// Delay for the specified time in ms without updating sensor values
void delayMs(uint16_t time_ms)
{
  timer_on = 1;
  timer_cnt = time_ms;
  while(timer_on) ;
}


// Initialize the Mind Control's ATmega168 microcontroller
void initialize(void)
{
  cli();

  // Set I/O pins
  DDRB = 0x10;
  PORTB = 0xCF;
  DDRC = 0x00;
  PORTC = 0xFF;
  DDRD = 0xE6;
  PORTD = 0x7D;

  // Set up the A/D converter
  DIDR0 |= 0x20;  // disable digital input on C5
  PRR &= ~_BV(PRADC); // Turn off ADC power save
  ADCSRA = (_BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0)); // Enabled, prescaler = 128
  ADMUX = (_BV(REFS0) | 0x05); // set voltage reference, select channel C5

/*  DIDR0 |= 0x20; // Disable digital input on C5
  PRR &= ~0x01; // Disable ADC power reduction
  ADCSRA = 0x87; // Enable ADC, prescale = 128
  ADMUX = 0x40; // Set volage reference */

  // Set up timer 1 to generate an interrupt every 1 ms
  TCCR1A = 0x00;
  TCCR1B = (_BV(WGM12) | _BV(CS12));
  OCR1A = 71;
  TIMSK1 = _BV(OCIE1A);

  // Set up the serial port with rx interrupt
  UBRR0 = 19;
  UCSR0B = (_BV(RXCIE0) | _BV(TXEN0) | _BV(RXEN0));
  UCSR0C = (_BV(UCSZ00) | _BV(UCSZ01));

  // Turn on interrupts
  sei();
  
  myport = cm_init_UART( CM_EPORT_CARGO,  CM_BAUD_19200 ); 
}


void powerOnRobot(void)
{
  // If Create's power is off, turn it on
  if(!RobotIsOn)
  {
      while(!RobotIsOn)
      {
          RobotPwrToggleLow;
          delayMs(500);  // Delay in this state
          RobotPwrToggleHigh;  // Low to high transition to toggle power
          delayMs(100);  // Delay in this state
          RobotPwrToggleLow;
      }
      delayMs(3500);  // Delay for startup
  }
}


// Switch the baud rate on both Create and module
void baud(uint8_t baud_code)
{
  if(baud_code <= 11)
  {
    byteTx(CmdBaud);
    UCSR0A |= _BV(TXC0);
    byteTx(baud_code);
    // Wait until transmit is complete
    while(!(UCSR0A & _BV(TXC0))) ;

    cli();

    // Switch the baud rate register
    if(baud_code == Baud115200)
      UBRR0 = Ubrr115200;
    else if(baud_code == Baud57600)
      UBRR0 = Ubrr57600;
    else if(baud_code == Baud38400)
      UBRR0 = Ubrr38400;
    else if(baud_code == Baud28800)
      UBRR0 = Ubrr28800;
    else if(baud_code == Baud19200)
      UBRR0 = Ubrr19200;
    else if(baud_code == Baud14400)
      UBRR0 = Ubrr14400;
    else if(baud_code == Baud9600)
      UBRR0 = Ubrr9600;
    else if(baud_code == Baud4800)
      UBRR0 = Ubrr4800;
    else if(baud_code == Baud2400)
      UBRR0 = Ubrr2400;
    else if(baud_code == Baud1200)
      UBRR0 = Ubrr1200;
    else if(baud_code == Baud600)
      UBRR0 = Ubrr600;
    else if(baud_code == Baud300)
      UBRR0 = Ubrr300;

    sei();

    delayMs(100);
  }
}




