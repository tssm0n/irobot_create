#include "oi.h"
#include "robotconstants.h"

#define FOLLOW_SPEED 300

#define WALL_DISTANCE (IR_SENSOR_BUMP - 100)
#define WALL_DISTANCE_THRESHOLD 40
#define WALL_LOST_THRESHOLD 150
#define WALL_TURN_ANGLE 90

enum followStates {wallNotFound, wallFound, wallFoundTurning, wallFollowing};

int doFollow(){
  int state = wallNotFound;
  int speed = FOLLOW_SPEED;

  // Stop just as a precaution
  drive(0, RadStraight);

  delay(500);

  Serial.println("Follow Wall Starting");

  readSensors();

  for(;;)
  {
    // Drive around until a button or unsafe condition is detected
    while((!sensors[SenCliffL])
      && (!sensors[SenCliffFL])
      && (!sensors[SenCliffFR])
      && (!sensors[SenCliffR])
      && (!sensors[SenChAvailable])
      && (sensors[SenIRChar] != IR_PAUSE))
    {

      if(isBumped()){
	// TODO: Make this not relay on delay
	drive(-1*speed, RadStraight);
	delay(500);
	drive(0, (RadStraight/2));

      } else if(state == wallNotFound) {
	if(customSensors[IR_CUSTOM_SENSOR] >= WALL_DISTANCE){
	   drive(0, RadStraight);
	   state = wallFound;
	} else {
	   drive(FOLLOW_SPEED, RadStraight);
	}

      } else if(state == wallFound) {
	moveServo(SERVO_RIGHT);
	angle = 0;
	state = wallFoundTurning;

      } else if(state == wallFoundTurning) {
	if(angle < WALL_TURN_ANGLE){
	   drive(FOLLOW_SPEED, RadCCW);		
	} else {
	   drive(0, RadStraight);
	   state = wallFollowing;
	}

      } else if(state == wallFollowing) {


      }
 



      // wait a little more than one robot tick for sensors to update
      delay(50);

      readSensors();

    }

    Serial.println("Follow Wall Done");
    // Stop driving
    drive(0, RadStraight);

    if(sensors[SenIRChar] == IR_PAUSE){
      return 0;
    }
  }
}


void findWall(){

   drive(FOLLOW_SPEED, RadStraight);


}


void followWall(){


}
