
// Includes
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include "oi.h"
#include "cmcontrol.h"

#define AdjustRadius 1600

// Global variables
extern uint16_t timer_cnt;
extern uint8_t timer_on;
extern uint8_t sensors_flag;
extern uint8_t sensors_index;
extern uint8_t sensors_in[Sen6Size];
extern uint8_t sensors[Sen6Size];
extern int16_t distance;
extern int16_t angle;
extern uint16_t customSensors[];


// Functions
void byteTx(uint8_t value);
void delayMs(uint16_t time_ms);
void delayAndUpdateSensors(unsigned int time_ms);
void initialize(void);
void powerOnRobot(void);
void baud(uint8_t baud_code);
void drive(int16_t velocity, int16_t radius);
uint8_t isBumped(void);

void doFollow (void) 
{
  int16_t turn_angle = 0;
  uint8_t turn_dir = 1;
  uint8_t turning = 0;
  uint8_t backing_up = 0;
  uint8_t bump = 0;

  // Stop just as a precaution
  drive(0, RadStraight);

  delayAndUpdateSensors(750);


  for(;;)
  {

    delayAndUpdateSensors(10);

    delayAndUpdateSensors(500);

      // Drive around until a button or unsafe condition is detected
      while(!(UserButtonPressed)
            && (!sensors[SenCliffL])
            && (!sensors[SenCliffFL])
            && (!sensors[SenCliffFR])
            && (!sensors[SenCliffR])
            && (!sensors[SenChAvailable])
	    && sensors[SenIRChar] != IR_PAUSE
        )
      {

        // Keep turning until the specified angle is reached
        if(turning)
        {
          if(backing_up)
          {
            if((-distance) > 5)
              backing_up = 0;
            drive(-200, RadStraight);
          }
          else
          {
            if(turn_dir)
            {
              if(angle > turn_angle)
                turning = 0;
              drive(200, RadCCW);
            }
            else
            {
              if((-angle) > turn_angle)
                turning = 0;
              drive(200, RadCW);
            }
          }
        }
        else if(bump)  // Check for a bump
        {
          // Set the turn parameters and reset the angle
          turn_dir = 1;
          if(sensors[SenBumpDrop] & BumpEither){  
		backing_up = 1;
	  }
          turning = 1;
          distance = 0;
          angle = 0;
          turn_angle = 30;

        }
	else if(customSensors[IR_CUSTOM_SENSOR] > 250)
	{
	  if(customSensors[IR_CUSTOM_SENSOR] > 475){
       	         // Wall too close
		drive(300, AdjustRadius);
          } else if(customSensors[IR_CUSTOM_SENSOR] > 375){
                 // Wall found
		drive(300, RadStraight);
          } else {
                  // Wall Far 
		drive(300, -AdjustRadius);
          }
	}
        else
        {
          // Otherwise, drive straight
          drive(300, RadStraight);
        }

        // wait a little more than one robot tick for sensors to update
        delayAndUpdateSensors(50);

        bump = 0;
        // Check to see if there is an object in front
	if((sensors[SenBumpDrop] & BumpEither) ||
           customSensors[PROXIMITY_SENSOR] < 13){
		bump = 1;
	}
      }	
      // Stop driving
      drive(0, RadStraight);

      if(sensors[SenIRChar] == IR_PAUSE){
		return;
      }

      // Wait
      delayAndUpdateSensors(2438);

    }
}

