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
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<pthread.h>
#include<iostream>
#include "controlprogram.h"

// Constants
#define RESET_SONG 0
#define START_SONG 1
#define BUMP_SONG  2
#define END_SONG   3

#define ADJUSTMENT_RADIUS -25000
#define LEFT_ADJUSTMENT_ANGLE 35
#define RIGHT_ADJUSTMENT_ANGLE 15
#define DRIVE_SLIGHT_LEFT 30000

using namespace std;

ControlProgram::ControlProgram()
{
	init();
}

ControlProgram::~ControlProgram()
{
}

void ControlProgram::init()
{
	interrupted = false;
	complete = false;
}

void* doRunProgramInBackground(void* arg){
	ControlProgram* prog = (ControlProgram*) arg;
	prog->runProgram(prog->getIrobot());
	prog->setComplete(true);
	pthread_exit(NULL);
} 

void ControlProgram::runProgramInBackground(IrobotSerial* irobotSerial)
{
	pthread_t pthreadt;
	irobot = irobotSerial;
	void* arg = (void*) (this);
	int rc = pthread_create(&pthreadt, NULL, doRunProgramInBackground, arg);
}

IrobotSerial* ControlProgram::getIrobot()
{
	return irobot;
}

void ControlProgram::interrupt()
{
	interrupted = true;
}

void ControlProgram::setComplete(bool value)
{
	complete = value;
}

bool ControlProgram::isComplete()
{
	return complete;
}

// DriveControl

DriveControl::DriveControl() : ControlProgram() {
	speed = 200;
	useCustomSensors = false;
	distanceThreshold = 200;
}

DriveControl::DriveControl(int* params, int numParams) : ControlProgram() {
	initializeParams(params, numParams);
}

void DriveControl::runProgram(IrobotSerial* irobotSerial){
	irobot = irobotSerial;
	initialize();
	run();
}

void DriveControl::initialize(){

	sensorData = NULL;
	distance = 0;
	angle = 0;
	srand(time(NULL));
	defineSongs();
}

void DriveControl::initializeParams(int* params, int numParams){
	cout << "Initialize numParams: " << numParams << " Param1: " << params[0] << " Param2: " << params[1] << endl;
	if(params != NULL && numParams > 0){
		speed = params[0];
		if(numParams > 1 && params[1] != 0){
			useCustomSensors = true;
		}
	}
	cout << "Drive Program:  Speed: " << speed << " Custom Sensors: " << useCustomSensors << endl;
}

void DriveControl::run() {
  int leds_cnt = 99;
  int leds_state = 0;
  int leds_on = 1;

  int turn_angle = 0;
  int turn_dir = 1;
  int turning = 0;
  int backing_up = 0;
  bool irSensorBump = false;

  int turnSpeed = (speed / 3) * 2;
  // Stop just as a precaution
  drive(0, DRIVE_STRAIGHT);

  // Play the reset song and wait while it plays
  irobot->playSong(RESET_SONG);
  delayAndUpdateSensors(750);
  delayAndUpdateSensors(20);

  cout << "Starting " << endl;
  cout << "Speed: " << speed << " Custom Sensors: " << useCustomSensors << endl;

  //for(;;)
  //{

    if(++leds_cnt >= 100)
    {
      leds_cnt = 0;
      leds_on = !leds_on;

      if(leds_on)
      {
        irobot->changeLEDs(128,255);
      }
      else
      {
        irobot->changeLEDs(0,0);
      }
    }

    delayAndUpdateSensors(10);

    //if(UserButtonPressed)
    //{
      // Play start song and wait
      irobot->playSong(START_SONG);
      delayAndUpdateSensors(2813);


      // Drive around until a button or unsafe condition is detected
      while(!interrupted && 
		!sensorData->getCliffLeft() && 
		!sensorData->getCliffFrontLeft() &&
		!sensorData->getCliffFrontRight() &&
		!sensorData->getCliffRight() &&
		!sensorData->getChargingSourcesAvailable())
	{
	//cout << interrupted << endl;

	//cout << "Left: " << sensorData->bumpLeft << endl;
	//cout << "Right: " << sensorData->bumpRight << endl;

      		irSensorBump = false;
		cout << "useCustomSensors " << useCustomSensors << endl;
      		if(useCustomSensors){
			int* customSensorData = sensorData->getCustomSensorData();
			if(customSensorData != NULL){
				cout << "IR: " << customSensorData[0] << endl;
				if(customSensorData[0] >= distanceThreshold){
					irSensorBump = true;
				}
			} else {
				cout << "Null custom sensors" << endl;
			}
      		}

        // Keep turning until the specified angle is reached
        if(turning)
        {
          if(backing_up)
          {
            if((-distance) > 5)
              backing_up = 0;
            drive(-1*turnSpeed, DRIVE_STRAIGHT);
          }
          else
          {
		//cout << "Turning: " << angle << " " << turn_angle << endl;
            if(turn_dir)
            {
              if(angle > turn_angle)
                turning = 0;
              drive(turnSpeed, DRIVE_CCW);
            }
            else
            {
              if((-1*angle) > turn_angle)
                turning = 0;
              drive(turnSpeed, DRIVE_CW);
            }
          }
        }
        else if(sensorData->bumpLeft || sensorData->bumpRight || irSensorBump)  // Check for a bump
        {
	//cout << "Bump" << endl;
          // Set the turn parameters and reset the angle
          if(sensorData->bumpLeft) {
            turn_dir = 0;
          } else {
	    if(irSensorBump && !sensorData->getWall()){
		turn_dir = 0;
	    } else {
            	turn_dir = 1;
	    }
	  }

          backing_up = 1;
          turning = 1;
          distance = 0;
          angle = 0;
          turn_angle = randomAngle();
	  cout << "Turn Angle: " << turn_angle << endl;

          // Play the bump song
          irobot->playSong(BUMP_SONG);
        }
        else
        {
		//cout << "Straight" << endl;
          // Otherwise, drive straight
          drive(speed, DRIVE_STRAIGHT);
        }


        // Flash the leds in sequence
        if(++leds_cnt >= 10)
        {
          leds_cnt = 0;
          if(turning)
          {
            // Flash backward while turning
            if(leds_state == 0)
              leds_state = 4;
            else
              leds_state--;
          }
          else
          {
            if(leds_state == 4)
              leds_state = 0;
            else
              leds_state++;
          }

          if(leds_state == 0)
          {
            // robot Power LED Amber
	    irobot->changeLEDs(0, 128, 255);
          }
          else if(leds_state == 1 || leds_state == 3)
          {
            // Play LED on
	    irobot->changeLEDs(1, 0, 0);
          }
          else if(leds_state == 2 || leds_state == 4)
          {
            // Advance LED on
	    irobot->changeLEDs(4, 0, 0);
          }
          /*else if(leds_state == 3)
          {
            // Robot LEDs off, CM left LED on
            byteTx(CmdLeds);
            byteTx(0x00);
            byteTx(0);
            byteTx(0);
            LED2On;
            LED1Off;
          }
          else if(leds_state == 4)
          {
            // Robot LEDs off, CM right LED on
            byteTx(CmdLeds);
            byteTx(0x00);
            byteTx(0);
            byteTx(0);
            LED1On;
            LED2Off;
          }*/
        }

        // wait a little more than one robot tick for sensors to update
	//cout << "daus" << endl;
        delayAndUpdateSensors(35);
      }
	cout << "Done" << endl;
	cout << !sensorData->getCliffLeft() << 
		!sensorData->getCliffFrontLeft() <<
		!sensorData->getCliffFrontRight() <<
		!sensorData->getCliffRight() <<
		!sensorData->getChargingSourcesAvailable() << endl;
	
      // Stop driving
      drive(0, DRIVE_STRAIGHT);

      // Play end song and wait
      delayAndUpdateSensors(500);
      //irobot->playSong(END_SONG);
      delayAndUpdateSensors(2438);

    //}
  //}
}

// Send Create drive commands in terms of velocity and radius
void ControlProgram::drive(int velocity, int radius)
{
  irobot->drive(velocity, radius);
}



void ControlProgram::delayAndUpdateSensors(int ms){
	usleep(ms * 1000);

	if(sensorData != NULL)
		delete sensorData;

	sensorData = new IRobotSensorData();
	sensorData = irobot->sensors(6, sensorData);
	sensorData = irobot->readCustomSensors(sensorData);

	// Update running totals of distance and angle
	distance += sensorData->getDistance();
	angle += sensorData->getAngle();

	cout << sensorData->displayValues() << endl;
}

int DriveControl::randomAngle(){
	int angle = 53 + ((rand() &0x00FF) >> 1);
	cout << "random angle " << endl;
	return angle;
}

void ControlProgram::defineSongs(){
  char notes[15];
  int it = 0;
  short songNum;
 // Reset song
  songNum = RESET_SONG;
  notes[it++] = 60;
  notes[it++] = 6;
  notes[it++] = 72;
  notes[it++] = 6;
  notes[it++] = 84;
  notes[it++] = 6;
  notes[it++] = 96;
  notes[it++] = 6;

  irobot->writeSong(songNum, it/2, notes);
  it = 0;

  // Start song
  songNum = START_SONG;
  notes[it++] = 69;
  notes[it++] = 18;
  notes[it++] = 72;
  notes[it++] = 12;
  notes[it++] = 74;
  notes[it++] = 12;
  notes[it++] = 72;
  notes[it++] = 12;
  notes[it++] = 69;
  notes[it++] = 12;
  notes[it++] = 77;
  notes[it++] = 24;
  irobot->writeSong(songNum, it/2, notes);
  it = 0;

  // Bump song
  songNum = BUMP_SONG;
  notes[it++] = 74;
  notes[it++] = 12;
  notes[it++] = 59;
  notes[it++] = 24;

  irobot->writeSong(songNum, it/2, notes);
  it = 0;

  // End song
  songNum = END_SONG;
  notes[it++] = 77;
  notes[it++] = 18;
  notes[it++] = 74;
  notes[it++] = 12;
  notes[it++] = 72;
  notes[it++] = 12;
  notes[it++] = 69;
  notes[it++] = 12;
  notes[it++] = 65;
  notes[it++] = 24;

  irobot->writeSong(songNum, it/2, notes);
  it = 0;

}


void FollowWallControl::runProgram(IrobotSerial* irobotSerial){
	irobot = irobotSerial;
	initialize();
	run();
}

void FollowWallControl::initialize(){

	sensorData = NULL;
	srand(time(NULL));
	defineSongs();
}

void FollowWallControl::run() {

  int turn_angle = 0;
  int turn_dir = 1;
  int turning = 0;
  int backing_up = 0;

  int lastSeenWall = 0;

  // Stop just as a precaution
  drive(0, DRIVE_STRAIGHT);

  // Play the reset song and wait while it plays
  //irobot->playSong(RESET_SONG);
  delayAndUpdateSensors(750);


  //for(;;)
  //{
    //delayAndUpdateSensors(10);

    //irobot->playSong(START_SONG);
    delayAndUpdateSensors(2813);
    cout << "Starting " << sensorData << endl;

      // Drive around until an unsafe condition is detected
      while(!interrupted && 
		!sensorData->getCliffLeft() && 
		!sensorData->getCliffFrontLeft() &&
		!sensorData->getCliffFrontRight() &&
		!sensorData->getCliffRight() &&
		!sensorData->getChargingSourcesAvailable())
	{

        // Keep turning until the specified angle is reached
        if(turning)
        {
          if(backing_up)
          {
            if((-distance) > 5)
              backing_up = 0;
            drive(-200, DRIVE_STRAIGHT);
          }
          else
          {
            if(turn_dir)
            {
              if(angle > turn_angle)
                turning = 0;
              drive(200, DRIVE_CCW);
            }
            else
            {
              if(angle > (360-turn_angle))
                turning = 0;
              drive(200, DRIVE_CW);
            }
          }
        }
        else if(sensorData->bumpLeft || sensorData->bumpRight)  // Check for a bump
        {
          // Set the turn parameters and reset the angle
          if(sensorData->bumpLeft)
            turn_dir = 0;
          else
            turn_dir = 1;
          backing_up = 1;
          turning = 1;
          distance = 0;
          angle = 0;

	  if(turn_dir == 0)
          	turn_angle = LEFT_ADJUSTMENT_ANGLE;
	  else 
		turn_angle = RIGHT_ADJUSTMENT_ANGLE;
          // Play the bump song
          //irobot->playSong(BUMP_SONG);
        }
	else if(sensorData->getWall() == 1)
	{
		cout << "Found wall" << endl;
		if(lastSeenWall + 3 < 20000)
			lastSeenWall += 3;
		drive(100, DRIVE_SLIGHT_LEFT);
	}
	else if(sensorData->getWall() == 0 && lastSeenWall > 0)
	{
		lastSeenWall--;
		drive(100, ADJUSTMENT_RADIUS);
	}
        else
        {
          // Otherwise, drive straight
          drive(100, DRIVE_STRAIGHT);
        }

        // wait a little more than one robot tick for sensors to update
        delayAndUpdateSensors(20);
      }

	cout << "Done" << endl;
	cout << !sensorData->getCliffLeft() << 
		!sensorData->getCliffFrontLeft() <<
		!sensorData->getCliffFrontRight() <<
		!sensorData->getCliffRight() <<
		!sensorData->getChargingSourcesAvailable() << endl;

      // Stop driving
      drive(0, DRIVE_STRAIGHT);

      // Play end song and wait
      delayAndUpdateSensors(500);
      //irobot->playSong(END_SONG);
      delayAndUpdateSensors(2438);

    //}
  //}
}





