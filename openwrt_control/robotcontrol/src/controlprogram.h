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
#ifndef CONTROLPROGRAM_H
#define CONTROLPROGRAM_H

#include "irobotserial.h"

/**
	@author Kevin Shaw <tss@cornbread.com>
*/

class ControlProgram{
public:
	ControlProgram();
	~ControlProgram();

	virtual void runProgram(IrobotSerial* irobotSerial) = 0;
	void runProgramInBackground(IrobotSerial* irobotSerial);
	IrobotSerial* getIrobot();
	void interrupt();

	bool isComplete();
	void setComplete(bool value);
protected:
	IrobotSerial* irobot;

	IRobotSensorData* sensorData;

	int distance;
	int angle;
	bool interrupted;
	bool complete;
	
	void defineSongs(); // Some default songs
	void drive(int velocity, int radius);
	void delayAndUpdateSensors(int ms);
	void init();
};

// Port of the Drive example program
class DriveControl: public ControlProgram{
public:
	DriveControl();
	DriveControl(int* params, int numParams);

	void runProgram(IrobotSerial* irobotSerial);
private:
	void initialize();
	void run();
	int randomAngle();
	void initializeParams(int* params, int numParams);

	int speed;
	bool useCustomSensors;
	int distanceThreshold;
};

class FollowWallControl : public ControlProgram {
public:
	void runProgram(IrobotSerial* irobotSerial);
private:
	void initialize();
	void run();
};	

#endif
