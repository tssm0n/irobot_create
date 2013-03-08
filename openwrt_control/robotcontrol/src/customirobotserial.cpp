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
#include <iostream>
#include "customirobotserial.h"

using namespace std;

int combineTwoBytes(char msb, char lsb);

CustomIrobotSerial::CustomIrobotSerial()
 : IrobotSerial()
{
}

CustomIrobotSerial::CustomIrobotSerial(char* deviceFile)
 : IrobotSerial(deviceFile)
{
}

CustomIrobotSerial::CustomIrobotSerial(bool lowSpeed)
 : IrobotSerial(lowSpeed)
{
}

CustomIrobotSerial::CustomIrobotSerial(char* deviceFile, bool lowSpeed)
 : IrobotSerial(deviceFile, lowSpeed)
{
}


CustomIrobotSerial::~CustomIrobotSerial()
{
}

void CustomIrobotSerial::sendInitialSequence()
{
	short i;
	for(i = 0; i < 3; i++){
		// Write the sequence 3 times
		writeToPort(151);
		writeToPort(137);
	}
}

void CustomIrobotSerial::cmLedsOn()
{
	sendInitialSequence();
	writeToPort(LED_ON);
}

void CustomIrobotSerial::cmLedsOff()
{
	sendInitialSequence();
	writeToPort(LED_OFF);
}

int CustomIrobotSerial::readIrSensor()
{
	sendInitialSequence();
	writeToPort(READ_IR_SENSOR);
	
	char* buf = new char[2];
	readFromPort(2, buf);
	int result = combineTwoBytes(buf[0], buf[1]);

	int i1 = int(buf[0] & 0x00FF);
	int i2 = int(buf[1] & 0x00FF);
	cout << "IR Result: " << i1 << " " << i2 << endl;
	delete buf;
	return result;
}

IRobotSensorData* CustomIrobotSerial::readCustomSensors(IRobotSensorData* result){
	cout << "Reading custom sensors" << endl;
	int customSensors[1];
	customSensors[0] = readIrSensor();
	cout << "customSensors[0] " << customSensors[0] << endl;

	result->setCustomSensorData(customSensors, 1);

	return result;
}

