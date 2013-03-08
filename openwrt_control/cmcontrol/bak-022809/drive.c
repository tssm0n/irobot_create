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

// Global variables
extern uint16_t timer_cnt;
extern uint8_t timer_on;
extern uint8_t sensors_flag;
extern uint8_t sensors_index;
extern uint8_t sensors_in[Sen6Size];
extern uint8_t sensors[Sen6Size];
extern int16_t distance;
extern int16_t angle;


// Functions
void byteTx(uint8_t value);
void delayMs(uint16_t time_ms);
void delayAndUpdateSensors(unsigned int time_ms);
void initialize(void);
void powerOnRobot(void);
void baud(uint8_t baud_code);
void drive(int16_t velocity, int16_t radius);
uint16_t randomAngle(void);

int doDrive (void) 
{
  uint8_t leds_cnt = 99;
  uint8_t leds_state = 0;
  uint8_t leds_on = 1;

  int16_t turn_angle = 0;
  uint8_t turn_dir = 1;
  uint8_t turning = 0;
  uint8_t backing_up = 0;

  // Stop just as a precaution
  drive(0, RadStraight);

  delayAndUpdateSensors(750);


  for(;;)
  {

    delayAndUpdateSensors(10);

    if(++leds_cnt >= 100)
    {
      leds_cnt = 0;
      leds_on = !leds_on;

      if(leds_on)
      {
        byteTx(CmdLeds);
        byteTx(LEDsBoth);
        byteTx(128);
        byteTx(255);
        LEDBothOff;
      }
      else
      {
        byteTx(CmdLeds);
        byteTx(0x00);
        byteTx(0);
        byteTx(0);
        LEDBothOn;
      }
    }


    delayAndUpdateSensors(2813);

      // Drive around until a button or unsafe condition is detected
      while(!(UserButtonPressed)
            && (!sensors[SenCliffL])
            && (!sensors[SenCliffFL])
            && (!sensors[SenCliffFR])
            && (!sensors[SenCliffR])
            && (!sensors[SenChAvailable])
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
        else if(sensors[SenBumpDrop] & BumpEither)  // Check for a bump
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

        }
        else
        {
          // Otherwise, drive straight
          drive(300, RadStraight);
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
            byteTx(CmdLeds);
            byteTx(0x00);
            byteTx(128);
            byteTx(255);
            LEDBothOff;
          }
          else if(leds_state == 1)
          {
            // Play LED on
            byteTx(CmdLeds);
            byteTx(LEDPlay);
            byteTx(0);
            byteTx(0);
            LEDBothOff;
          }
          else if(leds_state == 2)
          {
            // Advance LED on
            byteTx(CmdLeds);
            byteTx(LEDAdvance);
            byteTx(0);
            byteTx(0);
            LEDBothOff;
          }
          else if(leds_state == 3)
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
          }
        }

        // wait a little more than one robot tick for sensors to update
        delayAndUpdateSensors(20);
      }

      // Stop driving
      drive(0, RadStraight);

      // Play end song and wait
      delayAndUpdateSensors(500);
      delayAndUpdateSensors(2438);

    }
}

uint16_t randomAngle(void)
{
    return (53 + ((uint16_t)(random() & 0xFF) >> 1));
}



