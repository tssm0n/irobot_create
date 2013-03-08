
#include<iostream>
#include<iomanip>
#include<string>
#include "irobotserial.h"
#include "controlprogram.h"
#include "remotecontrol.h"
#include "irobotcommandhandler.h"
#include "customirobotserial.h"


using namespace std;

int idCommand(char* command);
int numberOfParams(int instruction);
bool hasFlag(int argc, char** argv, const char* flag);

const char* DAEMON_FLAG = "-d";
const char* LOW_SPEED_FLAG = "-l";

int main(int argc, char** argv){
	bool daemonFlag = false;
	bool lowSpeedFlag = false;
	int numFlags = 0;
	char* device = NULL;
	if(argc > 1){
		if(hasFlag(argc, argv, DAEMON_FLAG)){
			daemonFlag = true;
			numFlags++;
		}
		if(hasFlag(argc, argv, LOW_SPEED_FLAG)){
			lowSpeedFlag = true;
			numFlags++;
		}

		if(argc-1 > numFlags){
			device = argv[argc-1];
		}
	} 


	CustomIrobotSerial* robot;
	if(!daemonFlag){
		cout << "device: " << device << endl;
		if(device != NULL){
			robot = new CustomIrobotSerial(device, lowSpeedFlag);
		} else {
			robot = new CustomIrobotSerial(lowSpeedFlag);
		}
	} else {
		RemoteControl* rc = new RemoteControl(device, lowSpeedFlag);
		delete rc;
		return 0;
	}

	robot->initialize();

	short done = 0;
	char command[40];
	int params[4];
	int instruction;
	IRobotCommandHandler* handler = new IRobotCommandHandler();
	while(!done){
		cout << "Available Commands: " << endl;
		cout << "(E) Exit" << endl;
		cout << "(L) Led (0/1) (0/1)" << endl;
		cout << "(C) CM-Led" << endl;
		cout << "(D) Drive (velocity) (radius)" << endl;
		cout << "(R) Drive Direct (right) (left)" << endl;
		cout << "(S) Stop" << endl;
		cout << endl;
		cout << "(1) Run 'Drive' Program" << endl;
		cout << "(2) Run 'Follow Wall' Program" << endl;
		cout << endl;
		cout << "(N) Sensors (packet type)" << endl;
		cout << "(I) Ir Sensor" << endl;

		cin >> command;

		if(strcasecmp(command, "E") == 0){
			cout << "Exiting" << endl;
			delete robot;
			return 0;
		}

		instruction = idCommand(command);
		
		int pCount = numberOfParams(instruction);

		if(pCount >= 1){
			cin >> params[0];
		}
		if(pCount >= 2){
			cin >> params[1];
		}

		handler->runCommand(robot, instruction, params, pCount);
	}

	delete handler;
	delete robot;
	
	return 0;
}

bool hasFlag(int argc, char** argv, const char* flag){
	char* temp;
	for(int count = 1; count < argc; count++){
		temp = argv[count];
		if(strcmp(temp, flag) == 0){
			return true;
		}
	}
	return false;
}

int numberOfParams(int instruction){
	if(instruction == LED || instruction == DRIVE || instruction == DRIVEDIRECT || 
	   instruction == DRIVE_PROGRAM_WITH_SPEED_CONTROL){
		return 2;
	}

	if(instruction == SENSORS){
		return 1;
	}
	return 0;
}

int idCommand(char* command){
	if(strcasecmp(command, "L") == 0){
		return LED;
	}
	if(strcasecmp(command, "D") == 0){
		return DRIVE;
	}
	if(strcasecmp(command, "R") == 0){
		return DRIVEDIRECT;
	}
	if(strcasecmp(command, "S") == 0){
		return STOP;
	}
	if(strcasecmp(command, "N") == 0){
		return SENSORS;
	}
	if(strcasecmp(command, "1") == 0){
		return DRIVE_PROGRAM;
	}
	if(strcasecmp(command, "2") == 0){
		return FOLLOW_WALL;
	}
	if(strcasecmp(command, "C") == 0){
		return CMLED_ON;
	}
	if(strcasecmp(command, "I") == 0){
		return IR_SENSOR;
	}
}

