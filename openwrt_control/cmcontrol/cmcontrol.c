
#define DEBUG_MODE 1 // 1 to enable debug

// Includes
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/twi.h>
#include <stdlib.h>
#include "oi.h"
#include "cm9600.h"
#include "cmcontrol.h"
#include "sensormod.h"

#define RECEIVE_FROM_BOT_BUFFER 64
#define MANUAL_INTERVAL 25

#define I2C_SLAVE 0x04

#define sensorModAvailable() (PINB & 0b00000010)

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
void delayAndUpdateSensors(unsigned int time_ms);
void debug(char* debugStr);
void debugInt(int outInt);
void initSensorMod(void);
uint16_t readSensorInt(char type);
void turnServo(char position);
uint16_t readIRSensor(void);

unsigned char i2cReadByte(int address);
int i2cWriteByte(int address, unsigned char data);
void i2cStop(void);

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

	struct cm_state cmState;

	// Initialize the Sensor module
	// Wait for the digital pin to go low

	while(!sensorModAvailable()){
		delayMs(500);
	}
	LED1On;

	initSensorMod();
	
	LED2On;

	int debugCount = 0;
	for(;;){
		debugCount++;
		delayAndUpdateSensors(5000);
		if(debugCount == 90){
			turnServo(SM_SERVO_CENTER);
			delayMs(1000);
			turnServo(SM_SERVO_LEFT);
			delayMs(1000);
			turnServo(SM_SERVO_RIGHT);
			debugCount = 0;
		}
	}
/*	for(;;){
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
	}*/
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
      
      // TODO: Read Custom Sensor Values
      //customSensors[IR_CUSTOM_SENSOR] = readIRSensor();
	  customSensors[IR_CUSTOM_SENSOR] = readSensorInt(SM_SENSOR_1);
     
	//  debugInt(customSensors[IR_CUSTOM_SENSOR]);
      customSensors[PROXIMITY_SENSOR] = -1; // Disable proximity sensor for now

#if DEBUG_MODE > 0  
/*	  if(debugCount >= 120){
		debug("IR Sensor: ");
		debugInt(customSensors[IR_CUSTOM_SENSOR]);
		debug("\r\n");
	  
		debug("Proximity Sensor: ");
		debugInt(customSensors[PROXIMITY_SENSOR]);
		debug("\r\n");	  
		debugCount = 0;
	  }
	  debugCount++;*/
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
/*		debugInt(sensors[SenCliffL] ||
           sensors[SenCliffFL] ||
           sensors[SenCliffFR] ||
           sensors[SenCliffR]);
		debug(" ");
		debugInt(sensors[SenBumpDrop] & BumpEither);
		debug(" ");
		debugInt(customSensors[IR_CUSTOM_SENSOR]);
		debug(" ");		
		debugInt(customSensors[PROXIMITY_SENSOR]);
		debug("\r\n");		*/
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
	/*uint16_t irResult;
	uint8_t irMsb;
	uint8_t irLsb;*/

	switch(value){
		case 1:
			LED2On;
			break;
		case 2:
			LED2Off;
			break;
		case 3:
			// TODO: Send the IR Sensor value
			// Read the IR sensor
			/*irResult = readIRSensor();
			irMsb = irResult >> 8;
			irLsb = irResult & 0x00FF;
			cm_ByteXmt(myport, irMsb);
			cm_ByteXmt(myport, irLsb);*/
			break;
		}

	// Turn localAck off so that commands are relayed again
	localAck = 0;
}

void initSensorMod(void){
	char sensorAck = 0;
	while(sensorAck != SM_ACK){
		i2cWriteByte(I2C_SLAVE, SM_INITIALIZE);
		sensorAck = i2cReadByte(I2C_SLAVE);
		debug("I: ");
		debugInt(sensorAck);
	}

/*	unsigned char result = 0;
	for(unsigned char i = 1; i < 20; i++){
		i2cWriteByte(I2C_SLAVE, i);
		result = i2cReadByte(I2C_SLAVE);
		debug("R: ");
		debugInt(result);
		if(PINB & 0b00000010)
			LED2On;
		else
			LED2Off;
	}*/
}
 
int i2cWriteByte(int address, unsigned char data){
  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* send start condition */
  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */

  TWDR = address | TW_WRITE; // send address with WRITE
  TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
	if( !(TW_STATUS&TW_MT_SLA_ACK))
		goto error;
		
  TWDR = data; // Data to send
  TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
	if( !(TW_STATUS&TW_MT_DATA_ACK))
		goto error;

  return 1;

  error:
	debug("i2c send error");
	return 0;
}

unsigned char i2cReadByte(int address){
  unsigned char result = 0;

  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* send (repeat) start condition */
  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */

  TWDR = 0x04| TW_READ; // send address with READ
  TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
	if(!(TW_STATUS&TW_MR_SLA_ACK))
		goto error;

  TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA); 
  while ((TWCR & _BV(TWINT)) == 0) ; 
	result = TWDR;

  i2cStop();

  return result;

  error:
	debug("i2c read error");
	return 0;
}

void i2cStop(void){
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* send stop condition */
}


uint16_t readSensorInt(char type){
	unsigned char byte1 = 0;
	unsigned char byte2 = 0;
	uint16_t result = 0;
	
	i2cWriteByte(I2C_SLAVE, type);

	byte1 = i2cReadByte(I2C_SLAVE);
	byte2 = i2cReadByte(I2C_SLAVE);
	
	result = ((byte1 << 8) | byte2);	

	debug("---");
	debugInt((int)byte1);
	debugInt((int)byte2);
	debugInt((int)result);
	
	if(result < 250){
		LED2Off;
	} else {
		LED2On;
	}
	return result;
}

void turnServo(char position){
	servoTurned = position;
	if(position == SM_SERVO_CENTER)
		servoTurned = 0;
	i2cWriteByte(I2C_SLAVE, position);
}

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
  DDRC = 0x02;
  PORTC = 0xFF;
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

// TWI
  /* initialize I2C (TWI) clock: 100 kHz clock, TWPS = 0 => prescaler = 1 */
  TWSR = 0;
  TWBR = (F_CPU / 100000UL - 16);

// Digital read on B1
  DDRB &=0b11111101; // B1 is an input
  PORTB |= 0b00000010; // Turn on pull up

  // Turn on interrupts
  sei();

 
  // Set up the A/D converter
/*  DIDR0 |= 0x04;  // disable digital input on C2
  PRR &= ~_BV(PRADC); // Turn off ADC power save
  ADCSRA = (_BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0)); // Enabled, prescaler = 128
  ADMUX = (_BV(REFS0) | 0x02); // set voltage reference, select channel C2
  
  // Set up the A/D converter C1
  DIDR0 |= 0x02;  // disable digital input on C1 */
  

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
