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
#include "irobotcommandhandler.h"

IRobotCommandHandler::IRobotCommandHandler()
{
}


IRobotCommandHandler::~IRobotCommandHandler()
{
}

void IRobotCommandHandler::runCommand(CustomIrobotSerial* robot, int instruction, int* params, int numParams){
	short larg2 = 128;
	short larg1 = 0;
	switch(instruction){
		case LED:
			if(params[0] > 0){
				larg1 |= 8;
			}
			if(params[1] > 0){
				larg1 |= 2;
			}

			robot->changeLEDs(larg1, larg2);
			break;
		case DRIVE:
			robot->drive(params[0], params[1]);
			break;
		case DRIVEDIRECT:
			robot->driveDirect(params[0], params[1]);
			break;
		case STOP:
			cout << "stop" << endl;
			if(cp != NULL){
				cp->interrupt();
				while(!cp->isComplete()){}
				delete cp;
				cp = NULL;
			}
			robot->driveDirect(0,0);
			break;
		case SENSORS:
			readSensors(robot, params[0]);
			break;
		case DRIVE_PROGRAM:
			runControlProgram(robot, DRIVE_PROGRAM);
			break;
		case DRIVE_PROGRAM_WITH_SPEED_CONTROL:
			runControlProgram(robot, DRIVE_PROGRAM, params, numParams);
			break;
		case FOLLOW_WALL:
			runControlProgram(robot, FOLLOW_WALL);
			break;
		case CMLED_ON:
			robot->cmLedsOn();
			break;
		case CMLED_OFF:
			robot->cmLedsOff();
			break;
		case IR_SENSOR:
			readIrSensor(robot);
			break;
		default:
			cout << "Unknown Command" << endl;
	}
}

void IRobotCommandHandler::readSensors(IrobotSerial* robot, int packetType){
	IRobotSensorData* sensorData = new IRobotSensorData();

	robot->sensors(packetType, sensorData);

	cout << "Sensor Data:" << endl;
	cout << "Velocity: " << sensorData->getVelocity() << endl;
	cout << "Distance: " << sensorData->getDistance() << endl;
	cout << "Play Button: " << sensorData->playButton << endl;
	cout << "Bump Left: " << sensorData->bumpLeft << endl;
	cout << "Bump Right: " << sensorData->bumpRight << endl;
	cout << "Wall Sensor: " << sensorData->getWall() << endl;
	cout << "Cliff: " << sensorData->getCliffLeft() << 
			sensorData->getCliffFrontLeft() <<
			sensorData->getCliffFrontRight() <<
			sensorData->getCliffRight() <<
			sensorData->getChargingSourcesAvailable() << endl;
	cout << "Angle: " << sensorData->getAngle() << endl;
	cout << endl;

	delete sensorData;
}

void IRobotCommandHandler::readIrSensor(CustomIrobotSerial* robot){
	int result = robot->readIrSensor();
	cout << "IR Sensor: " << result << endl;
}

void IRobotCommandHandler::runControlProgram(IrobotSerial* robot, int type){
	runControlProgram(robot, type, NULL, 0);
}

void IRobotCommandHandler::runControlProgram(IrobotSerial* robot, int type, int* params, int numParams){
	switch(type){
		case DRIVE_PROGRAM:
			if(params == NULL){
				cp = new DriveControl();
			} else {
				cp = new DriveControl(params, numParams);
			}
			break;
		case FOLLOW_WALL:
			cp = new FollowWallControl();
			break;
	}
	cp->runProgramInBackground(robot);
}

