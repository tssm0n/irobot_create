
#define DEBUG_MODE 1 // 1 to enable debug

// Includes
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include "oi.h"
#include "cm9600.h"
#include "cmcontrol.h"

#define RECEIVE_FROM_BOT_BUFFER 64
#define MANUAL_INTERVAL 25

// Global variables
volatile uint16_t timer_cnt = 0;
volatile uint8_t timer_on = 0;
volatile uint8_t receiveIndex = 0;
volatile uint8_t receiveBuffer[RECEIVE_FROM_BOT_BUFFER];
volatile uint8_t myport = 0;
volatile char prevVal = 255;
volatile uint8_t localAckIndex = 0;
volatile char localAck = 0;
volatile uint8_t doRelay = 0;
volatile int16_t distance = 0;
volatile int16_t angle = 0;
volatile int8_t debugCount = 0;
volatile uint8_t servoTurned = 0;

// Sensors
volatile uint8_t sensors_flag = 0;
volatile uint8_t sensors_index = 0;
volatile uint8_t sensors_in[Sen6Size];
volatile uint8_t sensors[Sen6Size];
volatile uint16_t customSensors[CUSTOM_SENSORS_BUFFER];

struct cm_state {
        uint16_t left;
	uint16_t right;
	uint8_t spin;
	uint16_t spinSpeed;
};


// Functions
void byteTx(uint8_t value);
void delayMs(uint16_t time_ms);
void delay100us(uint16_t time_ms);
void initialize(void);
void powerOnRobot(void);
void baud(uint8_t baud_code);
void relayCommand(void);
uint8_t checkLocalAck(uint8_t nextChar);
void handleLocalCommand(uint8_t value);
void writeChar(char c, uint8_t com);
uint8_t getSerialDestination(void);
void setSerial(uint8_t com) ;
uint16_t readIRSensor(void);
uint16_t readProximitySensor(void);
void delayAndUpdateSensors(unsigned int time_ms);
uint16_t glance(short position);
void turnServo(short position);
void debug(char* debugStr);
void debugInt(int outInt);

// Manual Drive Functions
void checkIr(struct cm_state* cmState);
void checkBump(struct cm_state* cmState);
uint8_t isBumped(void);
void driveDirect(uint16_t left, uint16_t right);
void drive(uint16_t speed, uint16_t angle);
void doDrive(void);
void doFollow(void);

int main (void) 
{
	initialize();
  	
	powerOnRobot();
  	byteTx(CmdStart);
  	byteTx(CmdFull);	
	
	LEDBothOff;
	LED1On;

	struct cm_state cmState;

	for(;;){
		// Relay commands
		if(doRelay){
			relayCommand();
		} else {
			delayAndUpdateSensors(15);
			checkIr(&cmState);
			checkBump(&cmState);
		}

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

  if(doRelay){
	if(receiveIndex < RECEIVE_FROM_BOT_BUFFER)
  		receiveBuffer[receiveIndex++] = temp;
  } else {
	  if(sensors_flag)
	  {
 	   sensors_in[sensors_index++] = temp;
  	 	 if(sensors_index >= Sen6Size)
   		   sensors_flag = 0;
  	  }
  }
}

// Delay for the specified time in ms and update sensor values
void delayAndUpdateSensors(uint16_t time_ms)
{
  uint8_t temp;

  timer_on = 1;
  timer_cnt = time_ms * 10;
  while(timer_on)
  {
    if(!sensors_flag)
    {
      for(temp = 0; temp < Sen6Size; temp++)
        sensors[temp] = sensors_in[temp];

      // Update running totals of distance and angle
      distance += (int)((sensors[SenDist1] << 8) | sensors[SenDist0]);
      angle += (int)((sensors[SenAng1] << 8) | sensors[SenAng0]);

      byteTx(CmdSensors);
      byteTx(6);
      sensors_index = 0;
      sensors_flag = 1;
      //if(!servoTurned){ // Only set the proximity sensor value if the servo is pointed straight
      customSensors[IR_CUSTOM_SENSOR] = readIRSensor();
      //}
      
	customSensors[PROXIMITY_SENSOR] = -1; // Disable proximit sensor for now
//	 customSensors[PROXIMITY_SENSOR] = readProximitySensor();

#if DEBUG_MODE > 0  
	  if(debugCount >= 120){
		debug("IR Sensor: ");
		debugInt(customSensors[IR_CUSTOM_SENSOR]);
		debug("\r\n");
	  
		debug("Proximity Sensor: ");
		debugInt(customSensors[PROXIMITY_SENSOR]);
		debug("\r\n");	  
		debugCount = 0;
	  }
	  debugCount++;
#endif
    }
  }
}

void checkIr(struct cm_state* cmState){
	short manualDriveChanged = 0;	
	switch(sensors[SenIRChar]){
		case 144:
			LEDBothOn;
			break;
		case IR_MAX:
			LEDBothOff;
			/*for(short i = 1; i < 2; i++){			
			
				for(i=1;i<50;i++)   //servo gets to position 600us for 1 sec
				{
					PORTB |= _BV(PB3);
					delay100us(6);
					PORTB &= ~_BV(PB3);
					delayMs(20);
				}			
				
				for(i=1;i<50;i++)   //servo gets to position 1200us for 1 sec
				{
					PORTB |= _BV(PB3);
					delay100us(12);
					PORTB &= ~_BV(PB3);
					delayMs(20);
				}


				for(i=1;i<50;i++)    //servo gets to position 1800us for 1 sec
				{
					PORTB |= _BV(PB3);
					delay100us(18);
					PORTB &= ~_BV(PB3);
					delayMs(20);
				} 			
			}*/
				turnServo(LOOK_LEFT);
				delayMs(500);
				turnServo(LOOK_MID_LEFT);							
				delayMs(500);
				turnServo(LOOK_CENTER);
				delayMs(500);
				turnServo(LOOK_MID_RIGHT);
				delayMs(500);
				turnServo(LOOK_RIGHT);				
				delayMs(500);
				turnServo(LOOK_CENTER);			
			
			//LED2On;
			//PORTB |= _BV(PB2);
			break;
		case IR_CLEAN:
			doDrive();
			break;
		case IR_SPOT:
			doFollow();
			break;
		case IR_RIGHT:
			if(cmState->left == 0 && cmState->right == 0){
				if(cmState->spin == SPIN_RIGHT){
					cmState->spinSpeed += MANUAL_INTERVAL;
				} else {
					cmState->spinSpeed = MANUAL_INTERVAL;
					cmState->spin = SPIN_RIGHT;
				}
			} else {
				if(cmState->left == 500){
					cmState->right -= MANUAL_INTERVAL;
				} else {
					cmState->left += MANUAL_INTERVAL;
				}
			}
			manualDriveChanged = 1;
			break;
		case IR_LEFT:
			if(cmState->left == 0 && cmState->right == 0){
				if(cmState->spin == SPIN_LEFT){
					cmState->spinSpeed += MANUAL_INTERVAL;
				} else {
					cmState->spinSpeed = MANUAL_INTERVAL;
					cmState->spin = SPIN_LEFT;
				}
			} else {
				if(cmState->right == 500){
					cmState->left -= MANUAL_INTERVAL;
				} else {
					cmState->right += MANUAL_INTERVAL;
				}
			}
			manualDriveChanged = 1;
			break;
		case IR_FORWARD:
			cmState->spin = SPIN_NONE;
			cmState->spinSpeed = 0;
			cmState->left += MANUAL_INTERVAL;
			cmState->right += MANUAL_INTERVAL;
			manualDriveChanged = 1;
			break;
		case IR_PAUSE:
			cmState->left = 0;
			cmState->right = 0;
			manualDriveChanged = 1;
			break;
	}
	if(manualDriveChanged){
		if(cmState->left > 500){
			cmState->left = 500;
		}
		if(cmState->right > 500){
			cmState->right = 500;
		}
		switch(cmState->spin){
			case SPIN_NONE:
				driveDirect(cmState->right, cmState->left);
				break;
			case SPIN_LEFT:
				drive(cmState->spinSpeed, 0x0001);
				break;
			case SPIN_RIGHT:
				drive(cmState->spinSpeed, 0xFFFF);
				break;
		}
	}
}

void checkBump(struct cm_state* cmState){
	// Check to see if it's moving
	if(cmState->right == 0 &&
	   cmState->left == 0){ 
	   	return;
	}

	if(isBumped()){

		cmState->right = 0;
		cmState->left = 0;
		cmState->spin = SPIN_NONE;
		cmState->spinSpeed = 0;

		driveDirect(-100, -100);
		delayMs(1000);
		driveDirect(0, 0);
#if DEBUG_MODE > 0
		debug("Bump found: Cliff, SenBumpDrop, IR, Proximity: \r\n");
		debugInt(sensors[SenCliffL] ||
           sensors[SenCliffFL] ||
           sensors[SenCliffFR] ||
           sensors[SenCliffR]);
		debug(" ");
		debugInt(sensors[SenBumpDrop] & BumpEither);
		debug(" ");
		debugInt(customSensors[IR_CUSTOM_SENSOR]);
		debug(" ");		
		debugInt(customSensors[PROXIMITY_SENSOR]);
		debug("\r\n");		
#endif
	   }

}

uint8_t isBumped(void){
	return (sensors[SenCliffL] ||
           sensors[SenCliffFL] ||
           sensors[SenCliffFR] ||
           sensors[SenCliffR] ||
           (sensors[SenBumpDrop] & BumpEither) ||
           customSensors[IR_CUSTOM_SENSOR] > IR_SENSOR_BUMP ||
           (customSensors[PROXIMITY_SENSOR] < PROXIMITY_BUMP && customSensors[PROXIMITY_SENSOR] != -1));
}

void drive(uint16_t speed, uint16_t angle){
	byteTx(CmdDrive);
	byteTx((uint8_t)((speed & 0xFF00) >> 8));
	byteTx((uint8_t)(speed & 0x00FF));
	byteTx((uint8_t)((angle & 0xFF00) >> 8));
	byteTx((uint8_t)(angle & 0x00FF));
}

void driveDirect(uint16_t right, uint16_t left){
    byteTx(CmdDriveWheels);
	byteTx((uint8_t)((right & 0xFF00) >> 8));
	byteTx((uint8_t)(right & 0x00FF));
	byteTx((uint8_t)((left & 0xFF00) >> 8));
	byteTx((uint8_t)(left & 0x00FF));
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

// TODO: Combine these next two functions
uint16_t readIRSensor(void){

	uint16_t meas_c5;
	
	//ADMUX &= ~0x0F; // Clear the ADC channel
	//ADMUX |= 0x05; // Set the ADC channel to C5
	ADMUX = (_BV(REFS0) | 0x02); // set voltage reference, select channel C2
	ADCSRA |= 0x40; // Start the measurement
	while(ADCSRA &0x40); // Wait until it's done
	meas_c5 = ADC; // Save the result

	return meas_c5;
}

uint16_t readProximitySensor(void){

	uint16_t meas_c1;
	
	//ADMUX &= ~0x0F; // Clear the ADC channel
	//ADMUX |= 0x05; // Set the ADC channel to C5
	ADMUX = (_BV(REFS0) | 0x01); // set voltage reference, select channel C1
	ADCSRA |= 0x40; // Start the measurement
	while(ADCSRA &0x40); // Wait until it's done
	meas_c1 = ADC; // Save the result

	return meas_c1;
}

uint16_t glance(short position){
	turnServo(position);
	uint16_t result =  readIRSensor();
	turnServo(LOOK_CENTER);
	return result;
}

void turnServo(short position){
	short waitTime = 200 - position;
	if(position != LOOK_CENTER){
		servoTurned = 1;
	}
	for(int i=0; i<SERVO_DELAY; i++) 
	{
		PORTB |= _BV(PB3);
		delay100us(position);
		PORTB &= ~_BV(PB3);
		delay100us(waitTime);
	}
	servoTurned = (position == LOOK_CENTER);
}

// Delay for the specified time in ms without updating sensor values
void delayMs(uint16_t time_ms)
{
   for(uint16_t i = 0; i < time_ms; i++){
	  delay100us(10);
   }
}

void delay100us(uint16_t time_ms){
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
  //PORTC = 0xFF;
  PORTC &= ~0x06; 
  DDRD = 0xE6;
  PORTD = 0x7D;

  // Set B2 and B3 as an output
  DDRB |= _BV(PB2) | _BV(PB3);
  PORTB &= ~(_BV(PB2) | _BV(PB3)) ;
  
  // Set up timer 1 to generate an interrupt every 1 ms
  /*TCCR1A = 0x00;  // Normal port operation, OC1A/OC1B disconnected
  TCCR1B = (_BV(WGM12) | _BV(CS12));  //WGM12 - CTC mode, max from OCR1A
				      // CS12 - clk/256
  OCR1A = 71;*/
  
  TCCR1A = 0x00;  // Normal port operation, OC1A/OC1B disconnected
  TCCR1B = (_BV(WGM12) | _BV(CS11));  //WGM12 - CTC mode, max from OCR1A
				      // CS11 - clk/8
  OCR1A = 229;  
  TIMSK1 = _BV(OCIE1A); // Enable Timer1 Output Compare A Match interrupt 

  // Set up timer 2 to generate an interrupt every .1 ms
  // try: prescaler 8, OCRA = 229
  //TCCR2A = _BV(WGM21); // CTC mode
  //TCCR2B = _BV(CS21);  // Prescaler 8
  //OCR1A = 229;
  //TIMSK2 = _BV(OCIE2A); // Enable interrupt

  // Set up the serial port with rx interrupt
  UBRR0 = 19;
  UCSR0B = (_BV(RXCIE0) | _BV(TXEN0) | _BV(RXEN0));
  UCSR0C = (_BV(UCSZ00) | _BV(UCSZ01));
  
  myport = cm_init_UART( CM_EPORT_CARGO,  CM_BAUD_19200 ); 

  // Turn on interrupts
  sei();

 
  // Set up the A/D converter
  DIDR0 |= 0x04;  // disable digital input on C2
  PRR &= ~_BV(PRADC); // Turn off ADC power save
  ADCSRA = (_BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0)); // Enabled, prescaler = 128
  ADMUX = (_BV(REFS0) | 0x02); // set voltage reference, select channel C2
  
  // Set up the A/D converter C1
  DIDR0 |= 0x02;  // disable digital input on C1
  //PRR &= ~_BV(PRADC); // Turn off ADC power save
  //ADCSRA = (_BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0)); // Enabled, prescaler = 128
  //ADMUX = (_BV(REFS0) | 0x01); // set voltage reference, select channel C1  

/*  DIDR0 |= 0x20; // Disable digital input on C5
  PRR &= ~0x01; // Disable ADC power reduction
  ADCSRA = 0x87; // Enable ADC, prescale = 128
  ADMUX = 0x40; // Set volage reference */

  turnServo(LOOK_CENTER);

  delayMs(250);

  debug("CMControl Initialized\r\n");

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


void debug(char* debugStr){
#if DEBUG_MODE > 0
	cm_send_line(myport, debugStr);
#endif
}

void debugInt(int outInt){
#if DEBUG_MODE > 0
	char intStr[16];
	itoa(outInt, intStr, 10);
	cm_send_line(myport, intStr);
	
	cm_send_line(myport, " ");
#endif
}
