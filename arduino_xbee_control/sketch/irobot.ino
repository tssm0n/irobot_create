#include <Servo.h> 
#include <Wire.h>
#include <SoftwareSerial.h> 
#include "cmcommon.h"
#include "oi.h"
#include "robotconstants.h"
#include"xbeecom.h"
#include"commands.h"

// Servo - Pin 9 - Connect red to power, black to ground, other to arduino
// Analog - Analog 0-3
// I2C Master
// I2C to eeprom  -- Analog 4 and 5 are i2c 
// Digital out - Pin 7 - Connect to pin B1 on robot
// Xbee pins 11 and 12
// Robot TX - Pin 0, Robot RX - Pin 1
// Power toggle - Pin 4

#define SERIAL_SPEED 57600

#define IR_SENSOR 0

// A0-A2 are tied to ground
#define EEPROM_ADDRESS 0x50

// Status pin is tied to B1 on the CM
// Top center, pin 3
#define STATUS_PIN_1 7

#define POWER_PIN 4

// TX pin to the Xbee's RX
#define XBEE_RX 11
#define XBEE_TX 12

#define XBEE_ADDRESS 2
#define XBEE_MASTER_ADDRESS 0

// In milliseconds
#define TICK_RATE 400

#define STOP() driveDirect(0,0,0,0)

#define DATA1_LENGTH (Sen0Size + 2)

#define byteTx Serial.write

Servo servo;

XbeeCom xbeeCom;
SoftwareSerial xbee(XBEE_RX, XBEE_TX);

int customSensors[CUSTOM_SENSORS_BUFFER];
byte sensors_in[Sen0Size];
byte sensors[Sen0Size];
byte sensorsPointer = 0;

int servoPos = 0;

boolean receiving = false;
byte sensorIndex = 0;

byte outgoingBuffer[XBEE_OUTGOING_BUFFER];

unsigned long time;

boolean emergencyStop = false;

boolean statusLED = false;

byte powerOnCount = 0;
byte powerOffCount = 0;

byte sensorReadCount = 0;

void setup(){
    Serial.begin(SERIAL_SPEED);
    
    randomSeed(analogRead(3));    
   
    Wire.begin();
    
    pinMode(STATUS_PIN_1, OUTPUT);
    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, LOW);
    
    moveServo(SERVO_CENTER_POS);

    Wire.write(NO_COMMAND);  
    
    xbee.begin(9600);
    
    Serial.write(CmdStart);
    Serial.write(CmdFull);
}

void loop(){
  while (xbee.available())
     xbeeCom.xbeeReceiveByte(xbee.read());
    
   if(xbeeCom.xbeeIsPacketAvailable())
     handleXbeeReceive();
    
   if(xbeeCom.bufferError){
     xbeeCom.bufferError = false;
     byte data[] = { NACK };
     transmit(XBEE_MASTER_ADDRESS, 1, data);
   } 
   
   while(Serial.available()){
     sensors_in[sensorsPointer++] = Serial.read();
     if(sensorsPointer >= Sen0Size){
        for(sensorsPointer = 0; sensorsPointer < Sen0Size; sensorsPointer++){
          sensors[sensorsPointer] = sensors_in[sensorsPointer];
        }
        sensorsPointer = 0;
        sensorReadCount++;
     }
     
   }
     
   unsigned long newTime = millis();
   if((newTime >= (time + TICK_RATE)) ||
      (time > newTime)){
       
        time = newTime;    
       
        clockTick();
            
   }
  
}

void handleXbeeReceive(){
  struct XbeePacket packet = xbeeCom.xbeeNextPacket();
  if(packet.dataLength == 0 || packet.destination != XBEE_ADDRESS){
    return; 
  }

  byte command = packet.data[0];
   
  switch(command){
    case PING: {
      byte data[] = { ACK };
      transmit(packet.source, 1, data);
      break;
    }
    case ACK:
    case STATUS:
      // No op...
      break;
    case STATUSREQ:
      updateAndTransmitSensors();
      break;
    case STOP_DRIVE:
      STOP();
      break;
    case DRIVE: {
      driveDirect(packet.data[1], packet.data[2], packet.data[3], packet.data[4]);
      break;
    }
    case TOGGLE_STATUS_LED: 
      // TODO
     break; 
    case RAW:
      {
        byte x = 1;
        for(x = 0; x < packet.dataLength; x++){
           Serial.write(packet.data[x]);
        }
      }
      break;
    case POWER_ON:
      digitalWrite(POWER_PIN, HIGH);
      powerOnCount = 3;
      break;
    case POWER_OFF:
      digitalWrite(POWER_PIN, HIGH);
      powerOffCount = 3;
      break;
    default:
      byte data[] = { NACK };
      transmit(packet.source, 1, data);
     break; 
  }
  
}

void clockTick(){
   boolean powerToggle = false;
   if(powerOnCount){
      powerOnCount--;
      powerToggle = true;
      
      Serial.write(CmdStart);
      Serial.write(CmdFull);
   }
   if(powerOffCount){
      powerOffCount--;
      powerToggle = true;
   }
   if(powerToggle && powerOnCount + powerOffCount == 0){
      digitalWrite(POWER_PIN, LOW);
   }
  
   //updateAndTransmitSensors();
  
   Serial.write(CmdSensors);
   Serial.write((uint8_t)0); 
}

void transmit(byte destination, byte length, byte* data){
    byte len = xbeeCom.xbeeSend(XBEE_ADDRESS, destination, length, data, outgoingBuffer);
    
    byte count;
    for(count = 0; count < len; count++){
      xbee.write(outgoingBuffer[count]); 
   }
}

void updateAndTransmitSensors(){
  // Safety
  if(isBumped()){
    if(!emergencyStop){
      STOP();
      emergencyStop = true;
    }
  } else {
    emergencyStop = false;
  }
  
  byte data1[DATA1_LENGTH];
  data1[0] = STATUS;
  data1[1] = 0;
  for(byte i = 0; i < Sen0Size; i++){
    data1[i+2] = sensors[i];  
  }
  transmit(XBEE_MASTER_ADDRESS, DATA1_LENGTH, data1);
  
  readSensors();
  byte data2[] = { STATUS, 1, customSensors[IR_SENSOR], 128,sensorReadCount  };
  transmit(XBEE_MASTER_ADDRESS, 5, data2);
}

void readXbee(){
  byte nextVal = 0;
  if (xbee.available())
    nextVal = xbee.read();
    xbeeCom.xbeeReceiveByte(nextVal);
}

void readSensors(){
    customSensors[IR_SENSOR] = analogRead(IR_SENSOR);
}

void drive(uint8_t speedMsb, uint8_t speedLsb, uint8_t angleMsb, uint8_t angleLsb){
    byteTx(CmdDrive);
    byteTx(speedMsb);
    byteTx(speedLsb);
    byteTx(angleMsb);
    byteTx(angleLsb);
}

void driveDirect(uint8_t rightMsb, uint8_t rightLsb, uint8_t leftMsb, uint8_t leftLsb){
    byteTx(CmdDriveWheels);
    byteTx(rightMsb);
    byteTx(rightLsb);
    byteTx(leftMsb);
    byteTx(leftLsb);
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



// I2C Code

void WireEepromRead(int theDeviceAddress, unsigned int theMemoryAddress, int theByteCount, byte* theByteArray) {
  for (int theByteIndex = 0; theByteIndex < theByteCount; theByteIndex++) {
    Wire.beginTransmission(theDeviceAddress);
    Wire.write((byte)((theMemoryAddress + theByteIndex) >> 8));
    Wire.write((byte)((theMemoryAddress + theByteIndex) >> 0));
    Wire.endTransmission();
    delay(5);
    Wire.requestFrom(theDeviceAddress, sizeof(byte));
    theByteArray[theByteIndex] = Wire.read();
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
    Wire.write((byte)((theMemoryAddress + theByteIndex) >> 8));
    Wire.write((byte)((theMemoryAddress + theByteIndex) >> 0));
    Wire.write(theByteArray[theByteIndex]);
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
