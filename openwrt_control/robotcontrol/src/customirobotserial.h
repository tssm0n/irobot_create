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
#ifndef CUSTOMIROBOTSERIAL_H
#define CUSTOMIROBOTSERIAL_H

#include "irobotserial.h"

#define LED_ON 1
#define LED_OFF 2
#define READ_IR_SENSOR 3

/**
	@author Kevin Shaw <tss@cornbread.com>
*/
class CustomIrobotSerial : public IrobotSerial
{
public:
    	CustomIrobotSerial();
	CustomIrobotSerial(char* deviceFile);
	CustomIrobotSerial(bool lowSpeed);
	CustomIrobotSerial(char* deviceFile, bool lowSpeed);

    	~CustomIrobotSerial();

	void cmLedsOn();
	void cmLedsOff();
	int readIrSensor();
	IRobotSensorData* readCustomSensors(IRobotSensorData* result);

private:
	void sendInitialSequence();
};

#endif
