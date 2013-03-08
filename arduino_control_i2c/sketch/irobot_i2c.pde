#include <Servo.h> 
#include <Wire.h>
#include "cmcommon.h"
#include "oi.h"
#include "robotconstants.h"

// Servo - Pin 9 - Connect red to power, black to ground, other to arduino
// Analog - Analog 0-3
// I2C Slave
// I2C to eeprom  -- Analog 4 and 5 are i2c 
// Digital out - Pin 7 - Connect to pin B1 on robot

#define SERIAL_SPEED 19200

#define IR_SENSOR 0

#define I2C_BUFFER_SIZE 40

// A0-A2 are tied to ground
#define EEPROM_ADDRESS 0x50

// Status pin is tied to B1 on the CM
// Top center, pin 3
#define STATUS_PIN_1 7

Servo servo;

int customSensors[CUSTOM_SENSORS_BUFFER];
byte sensors_in[Sen6Size];
byte sensors[Sen6Size];

int servoPos = 0;

byte i2cBuffer[I2C_BUFFER_SIZE];
byte i2cReadPos = 0;
byte i2cWritePos = 0;
boolean i2cRequest = false;
boolean receiving = false;
byte sensorIndex = 0;

int distance = 0; 
int angle = 0;

byte statusPin1Value = LOW; 

struct cm_state {
        uint16_t left;
	uint16_t right;
	uint8_t spin;
	uint16_t spinSpeed;
};

struct cm_state cmState;

void setup(){
    Serial.begin(SERIAL_SPEED);
    
    randomSeed(analogRead(3));    
   
    Wire.begin(0x02);
    Wire.onRequest(requestEvent);
    Wire.onReceive(receiveEvent); 
    
    pinMode(STATUS_PIN_1, OUTPUT);
    
    moveServo(SERVO_CENTER_POS);
    
    statusPin1Value = LOW; 

    Wire.send(NO_COMMAND);  
    
    Serial.println("Ready...");
}

void loop(){
  digitalWrite(STATUS_PIN_1, statusPin1Value);
  
  checkIr(&cmState);
  if(isBumped()){
    Serial.println("Bumped"); 
    addToI2cBuffer(LED2_ON);
    driveDirect(-200,-200);
    delay(1000);
    driveDirect(0,0);
  }
  
  readSensors();
  //Serial.print("I: ");
  //Serial.println(customSensors[IR_SENSOR]);
  
  delay(200);
  
  /*for(int i = 0; i < 10; i++){
    readSensors();
    delay(500);
    Serial.println((int)sensorValues[0]);
  }
  
  testServo();
  testI2cEeprom(EEPROM_ADDRESS);*/

}

void requestEvent(){
	i2cRequest = true;
        sendFromI2cBuffer();
}

void receiveEvent(int numBytes){
	if(receiving){
          sensors_in[sensorIndex++] = Wire.receive();
          if(sensorIndex == Sen6Size){
            receiving = false;
            //addToI2cBuffer(SENSOR_DATA_ACK);            
            for(byte pos = 0; pos < Sen6Size; pos++)
              sensors[pos] = sensors_in[pos];
              distance += (int)((sensors[SenDist1] << 8) | sensors[SenDist0]);
              angle += (int)((sensors[SenAng1] << 8) | sensors[SenAng0]);              
          }
        } else {
	  byte next = Wire.receive();
	  handleRequest(next);
	}
}

void handleRequest(byte type){
    switch(type){
      case SENSOR_DATA_START:
        receiveSensorData();
        break;
      case REQUEST_ANY_COMMAND:
        if(i2cReadPos == i2cWritePos){
          addToI2cBuffer(NO_COMMAND);
        } 
        break;
      default:
        break;
    };
}

void receiveSensorData(){
    receiving = true;
    sensorIndex = 0;    
}

void readSensors(){
    customSensors[IR_SENSOR] = analogRead(IR_SENSOR);
}

void drive(int inSpeed, int angle){
   addToI2cBuffer(DO_DRIVE);
   addToI2cBuffer(highByte(inSpeed));
   addToI2cBuffer(lowByte(inSpeed));
   addToI2cBuffer(highByte(angle));
   addToI2cBuffer(lowByte(angle));
}

void driveDirect(int right, int left){
   addToI2cBuffer(DO_DRIVE_DIRECT);
   addToI2cBuffer(highByte(right));
   addToI2cBuffer(lowByte(right));
   addToI2cBuffer(highByte(left));
   addToI2cBuffer(lowByte(left));
}

void moveServo(byte pos){
  servo.attach(SERVO_PIN);
  servo.write(pos);
  delay(SERVO_MOVE_DELAY);
  servo.detach();
}

int glance(byte pos){
   moveServo(pos);
   readSensors();
   moveServo(SERVO_CENTER_POS); 
   return customSensors[IR_CUSTOM_SENSOR];
}

boolean isBumped(void){
	return (sensors[SenCliffL] ||
           sensors[SenCliffFL] ||
           sensors[SenCliffFR] ||
           sensors[SenCliffR] ||
           (sensors[SenBumpDrop] & BumpEither));
}

void addToI2cBuffer(byte val){
  i2cBuffer[i2cWritePos++] = val;   
  if(i2cWritePos >= I2C_BUFFER_SIZE)
	i2cWritePos = 0;
}

void sendFromI2cBuffer(){
  if(i2cRequest && (i2cReadPos != i2cWritePos)){
	i2cRequest = false;
	byte nextVal = i2cBuffer[i2cReadPos++];
	if(i2cReadPos >= I2C_BUFFER_SIZE)
		i2cReadPos = 0;
        /*if(nextVal != NO_COMMAND){
          Serial.print((int)i2cReadPos);
          Serial.print(" - ");
          Serial.print((int)i2cWritePos);
          Serial.print("S: ");
          Serial.println((int)nextVal);
        }*/
	Wire.send(nextVal);
  }
}

void testServo(){
  servo.write(SERVO_MAX_POS/2);
  delay(2000);
  servo.write(0);
  delay(2000);
  servo.write(SERVO_MAX_POS);
  delay(2000);
  for(servoPos = 0; servoPos < SERVO_MAX_POS; servoPos += 15){
    servo.write(servoPos);
    delay(500);
  }
  for(servoPos = SERVO_MAX_POS; servoPos >= 0; servoPos -= 15){
    servo.write(servoPos);
    delay(500);   
  } 
  
}

void testI2cEeprom(int theDeviceAddress) {
  Wire.begin();
  
  for (unsigned int theMemoryAddress = 0; theMemoryAddress < 256; theMemoryAddress++) {
    WireEepromWriteByte(theDeviceAddress, theMemoryAddress, (byte)theMemoryAddress);
  }
  for (unsigned int theMemoryAddress = 0; theMemoryAddress < 256; theMemoryAddress++) {
    Serial.print("Byte: ");
    Serial.print(theMemoryAddress);
    Serial.print(", ");
    Serial.print((int)WireEepromReadByte(theDeviceAddress, theMemoryAddress));
    Serial.println(".");
  }
/*  for (unsigned int theMemoryAddress = 0; theMemoryAddress < 1024; theMemoryAddress++) {
   WireEepromWriteInt(theDeviceAddress, theMemoryAddress * 2, (int)theMemoryAddress);
   }
   for (unsigned int theMemoryAddress = 0; theMemoryAddress < 1024; theMemoryAddress++) {
   Serial.print("Int: ");
   Serial.print(theMemoryAddress);
   Serial.print(", ");
   Serial.print(WireEepromReadInt(theDeviceAddress, theMemoryAddress * 2));
   Serial.println(".");
   }*/
}

void printDebugMenu(){
    Serial.println("\n");
    Serial.println("A) nalog Debug");
    Serial.println("S) ervo Test");  
    Serial.println("P) rogram EEPROM");
    Serial.println("I) /O Debug");
}

void handleDebugCommand(byte input){
   switch(input){
      case 'A':
      case 'a':
         break; 
      case 'B':
      case 'b':
         break; 
      case 'C':
      case 'c':
         break; 
      case 'D':
      case 'd':
         break; 
   }      
}

void debugMenu(){
   byte input;
    if(!Serial.available()){
         return;
    }
    
    input = Serial.read();

    handleDebugCommand(input);
}



// I2C Code

void WireEepromRead(int theDeviceAddress, unsigned int theMemoryAddress, int theByteCount, byte* theByteArray) {
  for (int theByteIndex = 0; theByteIndex < theByteCount; theByteIndex++) {
    Wire.beginTransmission(theDeviceAddress);
    Wire.send((byte)((theMemoryAddress + theByteIndex) >> 8));
    Wire.send((byte)((theMemoryAddress + theByteIndex) >> 0));
    Wire.endTransmission();
    delay(5);
    Wire.requestFrom(theDeviceAddress, sizeof(byte));
    theByteArray[theByteIndex] = Wire.receive();
  }
}

byte WireEepromReadByte(int theDeviceAddress, unsigned int theMemoryAddress) {
  byte theByteArray[sizeof(byte)];
  WireEepromRead(theDeviceAddress, theMemoryAddress, sizeof(byte), theByteArray);
  return (byte)(((theByteArray[0] << 0)));
}

int WireEepromReadInt(int theDeviceAddress, unsigned int theMemoryAddress) {
  byte theByteArray[sizeof(int)];
  WireEepromRead(theDeviceAddress, theMemoryAddress, sizeof(int), theByteArray);
  return (int)(((theByteArray[0] << 8)) | (int)((theByteArray[1] << 0)));
}

void WireEepromWrite(int theDeviceAddress, unsigned int theMemoryAddress, int theByteCount, byte* theByteArray) {
  for (int theByteIndex = 0; theByteIndex < theByteCount; theByteIndex++) {
    Wire.beginTransmission(theDeviceAddress);
    Wire.send((byte)((theMemoryAddress + theByteIndex) >> 8));
    Wire.send((byte)((theMemoryAddress + theByteIndex) >> 0));
    Wire.send(theByteArray[theByteIndex]);
    Wire.endTransmission();
    delay(5);
  }
}

void WireEepromWriteByte(int theDeviceAddress, unsigned int theMemoryAddress, byte theByte) {
  byte theByteArray[sizeof(byte)] = {(byte)(theByte >> 0)};
  WireEepromWrite(theDeviceAddress, theMemoryAddress, sizeof(byte), theByteArray);
}

void WireEepromWriteInt(int theDeviceAddress, unsigned int theMemoryAddress, int theInt) {
  byte theByteArray[sizeof(int)] = {(byte)(theInt >> 8), (byte)(theInt >> 0)};
  WireEepromWrite(theDeviceAddress, theMemoryAddress, sizeof(int), theByteArray);
}
