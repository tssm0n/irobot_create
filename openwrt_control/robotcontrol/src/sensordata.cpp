
#include "irobotserial.h"

using namespace std;

int combineTwoBytes(char msb, char lsb);

int IRobotSensorData::packet0[] = {7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
int IRobotSensorData::packet1[] = {7,8,9,10,11,12,13,14,15,16};
int IRobotSensorData::packet2[] = {17,18,19,20};
int IRobotSensorData::packet3[] = {21,22,23,24,25,26};
int IRobotSensorData::packet4[] = {27,28,29,30,31,32,33,34};
int IRobotSensorData::packet5[] = {35,36,37,38,39,40,41,42};
int IRobotSensorData::packet6[] = {7,8,9,10,11,12,13,14,16,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42};

int SensorDataHelper::findPacketSize(int packetType){
	if(packetType > 42){
		return 0;
	}

	switch(packetType){
		case 0:
			return 26;
		case 1:
			return 10;
		case 2:
			return 6;
		case 3:
			return 10;
		case 4:
			return 14;
		case 5:
			return 12;
		case 6:
			return 52;
		case 19:
		case 20:
		case 22:
		case 23:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
		case 33:
		case 39:
		case 40:
		case 41:
		case 42:
			return 2;
		default:
			return 1;
	}

	return 1;
}

IRobotSensorData* SensorDataHelper::parseSensorData(int packetType, char* sensorData, IRobotSensorData* result){
	
	int* types;
	int numTypes;
	int nextData = 0;
	
	switch(packetType){
		case 0:
			types = IRobotSensorData::packet0;
			numTypes = 20;
			break;
		case 1:
			types = IRobotSensorData::packet1;
			numTypes = 10;
			break;
		case 2:
			types = IRobotSensorData::packet2;
			numTypes = 4;
			break;
		case 3:
			types = IRobotSensorData::packet3;
			numTypes = 6;
			break;
		case 4:
			types = IRobotSensorData::packet4;
			numTypes = 8;
			break;
		case 5:
			types = IRobotSensorData::packet5;
			numTypes = 8;
			break;
		case 6:
			types = IRobotSensorData::packet6;
			numTypes = 36;
			break;
		default:
			types = new int[1];
			types[0] = packetType;
			numTypes = 1;
	}

	for(int i = 0; i < numTypes; i++){
		nextData += result->setByPacketType(types[i], sensorData + nextData);
	}
	result->setFlags();
	return result;

}

int combineTwoBytes(char msb, char lsb){
	int result = 0;
	result |= (lsb & 0x00FF);
	result |= ((msb << 8) & 0x00FF00);

	return result;
}

// IRobotSensorData

IRobotSensorData::IRobotSensorData(){
	for(int i = 0; i < 36; i++){
		sensorData[i] = 0;
	}
	customSensorData = NULL;
}

IRobotSensorData::~IRobotSensorData(){
	if(customSensorData != NULL){
		delete customSensorData;
	}
}

void IRobotSensorData::setData(int position, int value){
	if(position > 16){
		return;
	}
	sensorData[position] = value;
}

int IRobotSensorData::setByPacketType(int packetType, char* data){
	int dataBytes = 1;
	int intData = 0;
	if(packetType == 19
		|| packetType == 20
		|| packetType == 22
		|| packetType == 23
		|| (packetType >= 25 && packetType <=31)
		|| packetType == 33
		|| packetType >= 39){

		dataBytes = 2;
		intData = combineTwoBytes(data[0], data[1]);
	} else {
		intData = data[0];
	}

	setData(packetType-7, intData);

	return dataBytes;
}

void IRobotSensorData::setFlags(){
	splitBumpsAndWheelDrops(getBumpsAndWheelDrops());
	splitDriverAndWheelOvercurrents(getOvercurrents());
	splitButtons(getButtons());
	splitCargoBayInputs(getUserDigitalInputs());
	splitCharging(getChargingSourcesAvailable());
}

void IRobotSensorData::splitBumpsAndWheelDrops(char data){
	wheelDropCastor = (data & 0x10) > 0;
	wheelDropLeft = (data & 0x08) > 0;
	wheelDropRight = (data & 0x04) > 0;
	bumpLeft = (data & 0x02) > 0;
	bumpRight = (data & 0x01) > 0;
}
void IRobotSensorData::splitDriverAndWheelOvercurrents(char data){
	leftWheel = (data & 0x10) > 0;
	rightWheel = (data & 0x08) > 0;
	lowSideDriver2 = (data & 0x04) > 0;
	lowSideDriver0 = (data & 0x02) > 0;
	lowSideDriver1 = (data & 0x01) > 0;
}
void IRobotSensorData::splitButtons(char data){
	advanceButton = (data & 0x04) > 0;
	playButton = (data & 0x01) > 0;
}
void IRobotSensorData::splitCargoBayInputs(char data){
	baudRateChange = (data & 0x10) > 0;
	digitalInput3 = (data & 0x08) > 0;
	digitalInput2 = (data & 0x04) > 0;
	digitalInput1 = (data & 0x02) > 0;
	digialInput0 = (data & 0x01) > 0;
}
void IRobotSensorData::splitCharging(char data){
	homeBase = (data & 0x02) > 0;
	internalCharger = (data & 0x01) > 0;
}

	// TODO: Add more to this, for debugging purposes
string IRobotSensorData::displayValues(){
	stringstream display;
	display << "Cliff Left: " << getCliffLeft() << "\r\n"
		<< "Cliff Front Left: " << getCliffFrontLeft() << "\r\n"
		<< "Cliff Front Right: " << getCliffFrontRight() << "\r\n"
		<< "Cliff Right: " << getCliffRight() << "\r\n"
		<< "Charging Sources: " << getChargingSourcesAvailable() << "\r\n"
		<< "Angle: " << getAngle() << endl;
	return display.str();
}

// TODO: Find the sensors that are 2 bytes and can be negative and cast them to shorts so the conversion works correctly

int IRobotSensorData::getBumpsAndWheelDrops(){
	return sensorData[0];
}
int IRobotSensorData::getWall(){
	return sensorData[1];
}
int IRobotSensorData::getCliffLeft(){
	return sensorData[2];
}
int IRobotSensorData::getCliffFrontLeft(){
	return sensorData[3];
}
int IRobotSensorData::getCliffFrontRight(){
	return sensorData[4];
}
int IRobotSensorData::getCliffRight(){
	return sensorData[5];
}
int IRobotSensorData::getVirtualWall(){
	return sensorData[6];
}
int IRobotSensorData::getOvercurrents(){
	return sensorData[7];
}
int IRobotSensorData::getIrByte(){
	return sensorData[10];
}
int IRobotSensorData::getButtons(){
	return sensorData[11];
}
int IRobotSensorData::getDistance(){
	return short(sensorData[12]);
}
int IRobotSensorData::getAngle(){
	return short(sensorData[13]);
}
int IRobotSensorData::getChargingState(){
	return sensorData[14];
}
int IRobotSensorData::getVoltage(){
	return sensorData[15];
}
int IRobotSensorData::getCurrent(){
	return sensorData[16];
}
int IRobotSensorData::getBatteryTemperature(){
	return sensorData[17];
}
int IRobotSensorData::getBatteryCharge(){
	return sensorData[18];
}
int IRobotSensorData::getBatteryCapacity(){
	return sensorData[19];
}
int IRobotSensorData::getWallSignal(){
	return sensorData[20];
}
int IRobotSensorData::getCliffLeftSignal(){
	return sensorData[21];
}
int IRobotSensorData::getCliffFrontLeftSignal(){
	return sensorData[22];
}
int IRobotSensorData::getCliffFrontRightSignal(){
	return sensorData[23];
}
int IRobotSensorData::getCliffRightSignal(){
	return sensorData[24];
}
int IRobotSensorData::getUserDigitalInputs(){
	return sensorData[25];
}
int IRobotSensorData::getUserAnalogInput(){
	return sensorData[26];
}
int IRobotSensorData::getChargingSourcesAvailable(){
	return sensorData[27];
}
int IRobotSensorData::getOIMode(){
	return sensorData[28];
}
int IRobotSensorData::getSongNumber(){
	return sensorData[29];
}
int IRobotSensorData::getSongPlaying(){
	return sensorData[30];
}
int IRobotSensorData::getNumberOfStreamPackets(){
	return sensorData[31];
}
int IRobotSensorData::getVelocity(){
	return sensorData[32];
}
int IRobotSensorData::getRadius(){
	return sensorData[33];
}
int IRobotSensorData::getRightVelocity(){
	return sensorData[34];
}
int IRobotSensorData::getLeftVelocity(){
	return sensorData[35];
}

void IRobotSensorData::setCustomSensorData(int* sensorValues, int size){
	if(customSensorData != NULL){
		delete customSensorData;
	}
	if(sensorValues == NULL || size == 0){
		customSensorData = NULL;
	} else {
		customSensorData = new int[size];
		int sdCount;
		for(sdCount = 0; sdCount < size; sdCount++){
			customSensorData[sdCount] = sensorValues[sdCount];
		}
	}
}

int* IRobotSensorData::getCustomSensorData(){
	return customSensorData;
}

