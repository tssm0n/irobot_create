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
#ifndef IROBOTCOMMANDHANDLER_H
#define IROBOTCOMMANDHANDLER_H

#include <iostream>
#include "irobotserial.h"
#include "controlprogram.h"
#include "customirobotserial.h"

#define LED 1
#define DRIVE 2
#define DRIVEDIRECT 3
#define STOP 4
#define SENSORS 5
#define DRIVE_PROGRAM 6
#define FOLLOW_WALL 7
#define CMLED_ON 8
#define EXIT_PROGRAM 9
#define DRIVE_PROGRAM_WITH_SPEED_CONTROL 10
#define CMLED_OFF 11
#define IR_SENSOR 12

/**
	@author Kevin Shaw <tss@cornbread.com>
*/
class IRobotCommandHandler{
public:
    IRobotCommandHandler();

    void runCommand(CustomIrobotSerial* robot, int instruction, int* params, int numParams);
    void runControlProgram(IrobotSerial* robot, int type);
    void runControlProgram(IrobotSerial* robot, int type, int* params, int numParams);
    void readSensors(IrobotSerial* robot, int packetType);
    void readIrSensor(CustomIrobotSerial* robot);

    ~IRobotCommandHandler();

private:

    ControlProgram* cp;
};

#endif
