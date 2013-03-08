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
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<iostream>
#include<sstream>
#include<string>
#include "remotecontrol.h"

#define MAX_MSG 100
#define LINE_ARRAY_SIZE (MAX_MSG+1)

using namespace std;

extern int numberOfParams(int instruction);

RemoteControl::RemoteControl(char* deviceFile, bool lowSpeed)
{
	if(deviceFile != NULL)
		robot = new CustomIrobotSerial(deviceFile, lowSpeed);
	else
		robot = new CustomIrobotSerial(lowSpeed);
	handler = new IRobotCommandHandler();
	doListen();
}


RemoteControl::~RemoteControl()
{
	delete handler;
}

int RemoteControl::doListen(){
	int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(listenSocket < 0){
		// Error
		return listenSocket;
	}

	struct sockaddr_in clientAddress, serverAddress;
	socklen_t clientAddressLength;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(LISTEN_PORT);
	char line[LINE_ARRAY_SIZE];

	if(bind(listenSocket, (struct sockaddr*) &serverAddress,
		sizeof(serverAddress)) < 0) {
			cout << "Error while binding to port" << endl;	
			return -1;
	}

	listen(listenSocket, 5);

	bool done = false;

	while(!done){
		cout << "Listening on port: " << LISTEN_PORT << endl;

    		clientAddressLength = sizeof(clientAddress);
        	connectSocket = accept(listenSocket,
	            (struct sockaddr *) &clientAddress,
	             &clientAddressLength);
	        if (connectSocket < 0) {
		        cerr << "cannot accept connection ";
			return connectSocket;
		}

		cout << "  connected to " << inet_ntoa(clientAddress.sin_addr) << endl;
		cout << "Initializing" << endl;
		robot->initialize();
		if (send(connectSocket, "Connected", 9, 0) < 0)
		    	cerr << "Error: cannot send modified data";
		
		// TODO: Remove debug code below
		ControlPacket* cp;
    		memset(line, 0x0, LINE_ARRAY_SIZE);
		int mLength = 0;
        	while ((mLength = recv(connectSocket, line, MAX_MSG, 0)) > 0) {
	      		cout << mLength << ":  --  " << line << "\n";
			
		   // Convert to a ControlPacket and process it
			cp = new ControlPacket(line, mLength);
			done = processCommand(cp);
			delete cp;
		        memset(line, 0x0, LINE_ARRAY_SIZE);  // set line to all zeroes
		}

	}		
}

bool RemoteControl::processCommand(ControlPacket* packet){
	int instruction = int(packet->getPacketType());
	cout << "Found Command " << instruction << endl;

	int numParams = numberOfParams(instruction);
	int* params = new int[numParams];
	if(numParams > 0){
		packet->getIntParams(params, 0, numParams-1);
	}

	switch(instruction){
		case SENSORS:
			readSensors(6);
			break;	
		case IR_SENSOR:
			readIrSensors();
			break;
		case EXIT_PROGRAM:
			return true;
			break;
		default:
			// For commands with no feedback
			handler->runCommand(robot, instruction, params, numParams);
	}

	delete params;
	return false;
}

void RemoteControl::readSensors(int param){
	cout << "readSensors() " << robot << endl;
	IRobotSensorData* sensorData = new IRobotSensorData();

	robot->sensors(param, sensorData);
	cout << "read" << endl;

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
	cout << endl;

// Format:  Velocity, distance, playbutton, bumpleft, bumpright, wall,
//          cliffleft, clifffrontleft, clifffrontright, cliffright, chargingSourcesAvailable
	string resultMsg;
	if(sensorData == NULL){
		resultMsg = "No Sensor Data Available";
	} else {
		stringstream tmpStream;
		tmpStream << "[";
		tmpStream <<  sensorData->getVelocity() << ",";
		tmpStream << sensorData->getDistance() << ",";
		tmpStream <<  sensorData->playButton << ",";
		tmpStream <<  sensorData->bumpLeft << ",";
		tmpStream <<  sensorData->bumpRight << ",";
		tmpStream <<  sensorData->getWall() << ",";
		tmpStream << sensorData->getCliffLeft() << "," ;
		tmpStream << sensorData->getCliffFrontLeft() << "," ;
		tmpStream << sensorData->getCliffFrontRight() << "," ;
		tmpStream << sensorData->getCliffRight() << "," ;
		tmpStream << sensorData->getChargingSourcesAvailable() << "]";
		resultMsg = tmpStream.str();
		cout << resultMsg << endl;
	}
	if (send(connectSocket, resultMsg.c_str(), strlen(resultMsg.c_str()),  0) < 0)
	    	cerr << "Error: cannot send sensor data";

	delete sensorData;	
}

void RemoteControl::readIrSensors(){
	int result = robot->readIrSensor();
	stringstream tmpStream;
	tmpStream << "{" << result << "}";
	string resultMsg = tmpStream.str();
	if (send(connectSocket, resultMsg.c_str(), strlen(resultMsg.c_str()),  0) < 0)
	    	cerr << "Error: cannot send sensor data";
}

ControlPacket::ControlPacket(char* packet, int size){
	int dataLength = size;
	packetData = new char[dataLength+1];
	for(int i = 0; i < dataLength; i++){
		packetData[i] = packet[i];
	}
}

ControlPacket::~ControlPacket(){
	delete packetData;
}

char* ControlPacket::getPacketData(){
	return packetData;
}

char ControlPacket::getPacketType(){
	return packetData[0];
}

int ControlPacket::getIntParam(int index){
	short b1 = short(packetData[index*2 + 1]);
	short b2 = 0x00FF & short(packetData[index*2 + 2]);
	return combineBytes(b1, b2);
}

int ControlPacket::combineBytes(short b1, short b2){
	int result = 0;
	result |= b2;
	result |= (b1 << 8);
	return result;
}

int* ControlPacket::getIntParams(int* params, int startIndex, int endIndex){
	int pos = 0;
	for(int index = startIndex; index <= endIndex; index++){
		params[pos++] = getIntParam(index);
	}

	return params;
}
