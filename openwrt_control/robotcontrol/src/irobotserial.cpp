/***************************************************************************
 *   Copyright (C) 2007 by Kevin Shaw   *
 *   tss@cornbread.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <cstdlib>
#include <iostream>
#include <string>
#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "irobotserial.h"

#define INCLUDE_DELAY 1

const char* DEFAULT_PORT = "/dev/ttyS0";
const int DEFAULT_BAUD = B57600;
const int LOW_BAUD = B19200;

using namespace std;

void splitTwoBytes(int val, short* bytes);

IrobotSerial::IrobotSerial()
{	
	internalInit(DEFAULT_PORT, false);
}

IrobotSerial::IrobotSerial(char* deviceFile)
{
	internalInit(deviceFile, false);
}

IrobotSerial::IrobotSerial(bool lowSpeed)
{	
	internalInit(DEFAULT_PORT, lowSpeed);
}

IrobotSerial::IrobotSerial(char* deviceFile, bool lowSpeed)
{
	internalInit(deviceFile, lowSpeed);
}

void IrobotSerial::internalInit(const char* deviceFile, bool lowSpeed)
{	
	cout << "Initializing " << deviceFile << endl;
	if(!lowSpeed)
		baud_rate = DEFAULT_BAUD;
	else
		baud_rate = LOW_BAUD;
	dataBits = CS8;
	//stopBits = CSTOPB;
	stopBits = 0;
	parityOn = 0;
 	parity = 0;
	fd = -1;

	device = new char[strlen(deviceFile)+1];
	strcpy(device, deviceFile);	
	openDeviceFile();
	setupPortOptions();

	sensorDataHelper = new SensorDataHelper();
}


IrobotSerial::~IrobotSerial()
{	
	writeToPort(131); // Put it in Safe mode
	closeConnection();

	delete device;
	delete sensorDataHelper;
}

void IrobotSerial::setupPortOptions(){
	struct termios termio;  // Holds settings for serial port

      //termio.c_cflag = baud_rate | CRTSCTS | dataBits | stopBits | parityOn | parity | CLOCAL | CREAD;
      termio.c_cflag = baud_rate | dataBits | stopBits | parityOn | parity | CLOCAL | CREAD;
      termio.c_iflag = IGNPAR;
      termio.c_oflag = 0;
      termio.c_lflag = 0;       //ICANON;
      termio.c_cc[VMIN]=1;
      termio.c_cc[VTIME]=0;
	// cfmakeraw(&termio);
      tcflush(fd, TCIFLUSH);
      tcsetattr(fd,TCSANOW,&termio);


}

int IrobotSerial::openDeviceFile(){
	cout << "Opening Device " << device << endl;
	if(device == NULL){
		return -1;
	}
      fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
      if (fd < 0)
      {
         perror(device);
         exit(-1);
      }

      return fd;
}

void IrobotSerial::closeConnection() {
	if(fd != -1)
		close(fd);
}

void IrobotSerial::writeToPort(short value){
	if(INCLUDE_DELAY > 0){
		usleep(INCLUDE_DELAY * 500);
	} 
	write(fd,&value,1); 
}

// TODO: Add a writeStringToPort function

char* IrobotSerial::readFromPort(int bytes, char* buf){
	int numRead = read(fd, buf, bytes);
	// TODO: Check that numRead == bytes, make it fail if not
}

void IrobotSerial::initialize() {
	usleep(2000); // Delay 2 milliseconds
	
	writeToPort(128); // Start
	
	writeToPort(132); // Full mode

	// Blink LEDs
	changeLEDs(10, 128);
	usleep(500000);
	changeLEDs(0, 128);
	usleep(500000);
	changeLEDs(10, 128);
}

void IrobotSerial::changeLEDs(short byte1, short byte2){
	changeLEDs(byte1, byte2, 128);
}

void IrobotSerial::changeLEDs(short byte1, short byte2, short powerLed){
	writeToPort(139);
	writeToPort(byte1);
	writeToPort(byte2);
	writeToPort(powerLed);
}

int IrobotSerial::drive(int velocity, int radius){
	// TODO: Verify those values
	if(velocity < -500 || velocity > 500){
		return 0;
	}

	short vel[2];
	short rad[2];

	splitTwoBytes(velocity, vel);
	splitTwoBytes(radius, rad);

	writeToPort(137);
	writeToPort(vel[0]);
	writeToPort(vel[1]);
	writeToPort(rad[0]);
	writeToPort(rad[1]);

	return 1;
}

int IrobotSerial::driveDirect(int rightWheel, int leftWheel){
	if(rightWheel < -500 || rightWheel > 500 ||
	   leftWheel < -500 || leftWheel > 500){
		return 0;
	}

	short right[2];
	short left[2];
	splitTwoBytes(rightWheel, right);
	splitTwoBytes(leftWheel, left);

	writeToPort(145);
	writeToPort(right[0]);	
	writeToPort(right[1]);
	writeToPort(left[0]);
	writeToPort(left[1]);

	return 1;
}

void IrobotSerial::writeSong(short songNum, short songLength, char* notes){
	// Only songs 0-15 are allowed
	if(songNum < 0 || songNum > 15){
		return;
	}
	writeToPort(140);
	writeToPort(songNum);
	writeToPort(songLength);

	for(int i = 0; i < songLength*2; i++){
		writeToPort(notes[i]);
	}
}

void IrobotSerial::playSong(short songNum){
	// Only songs 0-15 are allowed
	if(songNum < 0 || songNum > 15){
		return;
	}
	writeToPort(141);
	writeToPort(songNum);
}

IRobotSensorData* IrobotSerial::sensors(int packetType, IRobotSensorData* result){
	writeToPort(142);
	writeToPort(packetType);
	
	int bytes = sensorDataHelper->findPacketSize(packetType);

	char* buf = new char[bytes];
	readFromPort(bytes, buf);

	sensorDataHelper->parseSensorData(packetType, buf, result);

	delete buf;
	return result;
	
}

IRobotSensorData* IrobotSerial::readCustomSensors(IRobotSensorData* result){
	// Override this in a sub-class
	cout << "Not Reading Custom Sensors" << endl;
}

IRobotSensorData* IrobotSerial::readAllSensors(IRobotSensorData* result){
	sensors(6, result);
	readCustomSensors(result);

	return result;
}


// byte[0] = MSB, byte[1] = LSB
void splitTwoBytes(int val, short* bytes){
	int r1 = 0;
	int r2 = 0;
	r1 = val >> 8;
	r2 = val & 255;

	bytes[0] = (short) r1;
	bytes[1] = (short) r2;
}

