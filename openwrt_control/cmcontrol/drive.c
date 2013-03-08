/* drive.c
 * Designed to run on Create Command Module
 *
 * The basic architecture of this program can be re-used to easily 
 * write a wide variety of Create control programs.  All sensor values
 * are polled in the background (using the serial rx interrupt) and 
 * stored in the sensors array as long as the function 
 * delayAndUpdateSensors() is called periodically.  Users can send commands
 * directly a byte at a time using byteTx() or they can use the 
 * provided functions, such as baud() and drive().
 */



// Includes
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include "oi.h"
#include "cmcontrol.h"

#define PROXIMITY_WARN 10
#define IR_WARN 90 

// Global variables
extern uint16_t timer_cnt;
extern uint8_t timer_on;
extern uint8_t sensors_flag;
extern uint8_t sensors_index;
extern uint8_t sensors_in[Sen6Size];
extern uint8_t sensors[Sen6Size];
extern int16_t distance;
extern int16_t angle;
extern uint16_t customSensors[CUSTOM_SENSORS_BUFFER];

// Functions
void byteTx(uint8_t value);
void delayMs(uint16_t time_ms);
void delayAndUpdateSensors(unsigned int time_ms);
void initialize(void);
void powerOnRobot(void);
void baud(uint8_t baud_code);
void drive(int16_t velocity, int16_t radius);
uint16_t randomAngle(void);
uint8_t isBumped(void);

int doDrive (void) 
{
  int16_t turn_angle = 0;
  uint8_t turn_dir = 1;
  uint8_t turning = 0;
  uint8_t backing_up = 0;
  uint8_t bump = 0;

  uint16_t speed = 300;

  // Stop just as a precaution
  drive(0, RadStraight);

  delayAndUpdateSensors(750);


  for(;;)
  {

    delayAndUpdateSensors(10);

    delayAndUpdateSensors(2813);

      // Drive around until a button or unsafe condition is detected
      while(!(UserButtonPressed)
            && (!sensors[SenCliffL])
            && (!sensors[SenCliffFL])
            && (!sensors[SenCliffFR])
            && (!sensors[SenCliffR])
            && (!sensors[SenChAvailable])
	    && (sensors[SenIRChar] != IR_PAUSE))
      {

	    if(sensors[SenIRChar] == IR_FORWARD){
		  speed += 100;
		  if(speed > 500)
			speed = 500;
	    }

        // Keep turning until the specified angle is reached
        if(turning)
        {
          if(backing_up)
          {
            if((-distance) > 5)
              backing_up = 0;
            drive(-1 * (speed), RadStraight);
          }
          else
          {
            if(turn_dir)
            {
              if(angle > turn_angle)
                turning = 0;
              drive(speed, RadCCW);
            }
            else
            {
              if((-angle) > turn_angle)
                turning = 0;
              drive(speed, RadCW);
            }
          }
        }
        else if(bump)  // Check for a bump
        {
          // Set the turn parameters and reset the angle
          if(sensors[SenBumpDrop] & BumpLeft)
            turn_dir = 0;
          else
            turn_dir = 1;
          backing_up = 1;
          turning = 1;
          distance = 0;
          angle = 0;
          turn_angle = randomAngle();

        } else if(customSensors[IR_CUSTOM_SENSOR] > 
			(IR_SENSOR_BUMP + IR_WARN))
	    {
			turning = 1;
			/*if(!lookAround()){
				turn_dir = 0;
			} else {
				turn_dir = 1;
			}*/
			distance = 0;
			angle = 0;
			turn_angle = randomAngle();
		}
        else
        {
          // Otherwise, drive straight
          drive(speed, RadStraight);
        }


        // wait a little more than one robot tick for sensors to update
        delayAndUpdateSensors(50);

		bump = 0;
		if(isBumped()){
			bump = 1;
		}
    }


      // Stop driving
      drive(0, RadStraight);
 
      if(sensors[SenIRChar] == IR_PAUSE){
		return 0;
      }

      // Play end song and wait
      delayAndUpdateSensors(500);
      delayAndUpdateSensors(2438);

    }


}

/*short lookAround(){
	uint16_t left = glance(LOOK_MAX_LEFT);
	uint16_t right = glance(LOOK_MAX_RIGHT);
	return left < right;
}*/

uint16_t randomAngle(void)
{
     
     // 20 - 180 degree angle
     return (random() % 160) + 21;

    // uncomment for 53 - 180 degree angle
    //return (53 + ((uint16_t)(random() & 0xFF) >> 1));
}



