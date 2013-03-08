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
#ifndef IROBOTSERIAL_H
#define IROBOTSERIAL_H

/**
	@author Kevin Shaw <tss@cornbread.com>
*/
#include <sstream>

#define DRIVE_STRAIGHT 32768
#define DRIVE_CW -1
#define DRIVE_CCW 1

using namespace std;

class IRobotSensorData;
class SensorDataHelper;

class IrobotSerial{
public:
	IrobotSerial();
	IrobotSerial(char* deviceFile);
	IrobotSerial(bool lowSpeed);
	IrobotSerial(char* deviceFile, bool lowSpeed);

	virtual ~IrobotSerial();

	void initialize();
	void writeToPort(short value);
	char* readFromPort(int bytes, char* buf);

	// Commands
	void changeLEDs(short byte1, short byte2);
	void changeLEDs(short byte1, short byte2, short powerLed);
	int drive(int velocity, int radius);
	int driveDirect(int rightWheel, int leftWheel);
	IRobotSensorData* sensors(int packetType, IRobotSensorData* result);
	IRobotSensorData* sensors(int numPackets, int* packetTypes, IRobotSensorData* result);
	IRobotSensorData* readAllSensors(IRobotSensorData* result);
	virtual IRobotSensorData* readCustomSensors(IRobotSensorData* result);

	void writeSong(short songNum, short songLength, char* notes);
	void playSong(short songNum);
private:
	void internalInit(const char* deviceFile, bool lowSpeed);
	char* device;
	SensorDataHelper* sensorDataHelper;
	int fd;
	long baud_rate;
	long dataBits;
	long stopBits;
	long parityOn;
 	long parity;

	int openDeviceFile();
	void setupPortOptions(); // Opens the serial port
	void closeConnection();

};

class SensorDataHelper {
public:
	IRobotSensorData* parseSensorData(int packetType, char* sensorData, IRobotSensorData* result);
	int findPacketSize(int packetType);
};

class IRobotSensorData {
public:
	IRobotSensorData();
	~IRobotSensorData();
	void setData(int position, int value);
	int setByPacketType(int packetType, char* data); // Returns number of bytes set
	void setFlags();

	string displayValues();
	
	int getBumpsAndWheelDrops();
	int getWall();
	int getCliffLeft();
	int getCliffFrontLeft();
	int getCliffFrontRight();
	int getCliffRight();
	int getVirtualWall();
	int getOvercurrents();
	int getIrByte();
	int getButtons();
	int getDistance();
	int getAngle();
	int getChargingState();
	int getVoltage();
	int getCurrent();
	int getBatteryTemperature();
	int getBatteryCharge();
	int getBatteryCapacity();

	int getWallSignal();
	int getCliffLeftSignal();
	int getCliffFrontLeftSignal();
	int getCliffFrontRightSignal();
	int getCliffRightSignal();
	int getUserDigitalInputs();
	int getUserAnalogInput();
	int getChargingSourcesAvailable();
	int getOIMode();
	int getSongNumber();
	int getSongPlaying();
	int getNumberOfStreamPackets();
	int getVelocity();
	int getRadius();
	int getRightVelocity();
	int getLeftVelocity();

	// Bumps and Wheel Drops
	bool wheelDropCastor;
	bool wheelDropLeft;
	bool wheelDropRight;
	bool bumpLeft;
	bool bumpRight;

	// Low Side Driver and Wheel Overcurrents
	bool leftWheel;
	bool rightWheel;
	bool lowSideDriver2;
	bool lowSideDriver0;
	bool lowSideDriver1;

	// Buttons
	bool advanceButton;
	bool playButton;

	// Cargo bay digital inputs
	bool baudRateChange;
	bool digitalInput3;
	bool digitalInput2;
	bool digitalInput1;
	bool digialInput0;

	// Charging Sources
	bool homeBase;
	bool internalCharger;

	static int packet0[];
	static int packet1[];
	static int packet2[];
	static int packet3[];
	static int packet4[];
	static int packet5[];
	static int packet6[];

	void setCustomSensorData(int* sensorValues, int size);
	int* getCustomSensorData();
private:
	int sensorData[36];  // Sensor Data from the Create
	int* customSensorData; // Custom made sensors, populated seperately

	void splitBumpsAndWheelDrops(char data);
	void splitDriverAndWheelOvercurrents(char data);
	void splitButtons(char data);
	void splitCargoBayInputs(char data);
	void splitCharging(char data);
};

#endif
