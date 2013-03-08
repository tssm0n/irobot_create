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
#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include "irobotserial.h"
#include "customirobotserial.h"
#include "irobotcommandhandler.h"

#define LISTEN_PORT 5012

class ControlPacket;

/**
	@author Kevin Shaw <tss@cornbread.com>
*/

class RemoteControl{
public:
	RemoteControl(char* deviceFile, bool lowSpeed);

	~RemoteControl();

	void receiveCommand(ControlPacket* controlPacket);

private:
	CustomIrobotSerial* robot;
	int connectSocket;
	IRobotCommandHandler* handler;

	int doListen();
	bool processCommand(ControlPacket* packet);
	void readSensors(int param);
	void readIrSensors();
};

class ControlPacket {
public:
	ControlPacket(char* packet, int size);
	~ControlPacket();

	char* getPacketData();
	char getPacketType();
	int getIntParam(int index); // Index starting at 0
	int* getIntParams(int* params, int startIndex, int endIndex); 

private:
	char* packetData;
	int dataLength;
	int combineBytes(short b1, short b2); // b1 = MSB, b2 = LSB
};


#endif
