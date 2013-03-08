
#include "oi.h"
#include "robotconstants.h"

#define PROXIMITY_WARN 10
#define IR_WARN 160
#define IR_WARN_STOP 110

#define CAUTION_SPEED 50

#define MILLI_SECONDS_BETWEEN_BUMPS 1800
#define MAX_BUMPS 4

boolean backing_up = false;

int doDrive (void) 
{
  int16_t turn_angle = 0;
  uint8_t turn_dir = 1;
  uint8_t turning = 0;
  uint8_t bump = 0;
  uint8_t forceBump = 0;
  
  unsigned long lastBump = 0;
  short bumpCount = 0;

  uint16_t speed = 300;

  // Stop just as a precaution
  drive(0, RadStraight);

  delay(750);

  Serial.println("Drive Starting");


  for(;;)
  {

    delay(500);
    
    lastBump = millis();

    // Drive around until a button or unsafe condition is detected
    while((!sensors[SenCliffL])
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
        Serial.println("Turning");
        Serial.print("Distance: " );
        Serial.print(distance);
        Serial.print(" - Angle: ");
        Serial.println(angle);
        if(backing_up)
        {
          Serial.println("Backing up");
          if((-distance) > 5)
            backing_up = false;
          drive(-1 * (speed), RadStraight);
        }
        else
        {
          Serial.println("Forward");
          if(isBumped()){
             forceBump = 1;
             turning = 0;
          }
          if(turn_dir)
          {
            Serial.println("Dir 1");
            if(angle > turn_angle)
              turning = 0;
            drive(speed, RadCCW);
          }
          else
          {
            Serial.println("dir2");
            if((-angle) > turn_angle)
              turning = 0;
            drive(speed, RadCW);
          }
        }
      }
      else if(bump)  // Check for a bump
      {
        Serial.println("Handle Bump");
        // Set the turn parameters and reset the angle
        if(sensors[SenBumpDrop] & BumpLeft)
          turn_dir = 0;
        else
          turn_dir = 1;
        backing_up = true;
        turning = 1;
        distance = 0;
        angle = 0;
        turn_angle = randomAngle();

      } 
      else if(customSensors[IR_CUSTOM_SENSOR] > 
        (IR_SENSOR_BUMP - IR_WARN))
      {
        Serial.println("Look around...");
        if(customSensors[IR_CUSTOM_SENSOR] > (IR_SENSOR_BUMP - IR_WARN_STOP)){
          driveDirect(0,0);
        } else {
          drive(CAUTION_SPEED, RadStraight);
        }
        
        unsigned long bumpTime = millis(); 
        if(bumpTime - lastBump <= MILLI_SECONDS_BETWEEN_BUMPS){
          // Flag this as a bump so that the bot will back up
          lastBump = bumpTime;
          bumpCount++;
          if(bumpCount >= MAX_BUMPS){
            bumpCount = 0;
            forceBump = 1;
          }          
        } else {
          bumpCount = 0;
          // Otherwise, just turn
          turning = 1;
          if(!lookAround()){
            turn_dir = 0;
          } 
          else {
            turn_dir = 1;
          }
          distance = 0;
          angle = 0;
          turn_angle = randomAngle();  
        }
      }
      else
      {
        // Otherwise, drive straight
        drive(speed, RadStraight);
      }


      // wait a little more than one robot tick for sensors to update
      delay(50);
      
      readSensors();


      bump = 0;
      if(forceBump || isBumped() || customSensors[IR_CUSTOM_SENSOR] > IR_SENSOR_BUMP){
        bump = 1;
        bumpCount++;
        forceBump = 0;
      }
    }

    Serial.println("Drive Done");
    // Stop driving
    drive(0, RadStraight);

    if(sensors[SenIRChar] == IR_PAUSE){
      return 0;
    }

    // Play end song and wait
    delay(2500);

  }


}

short lookAround(){
  int left = glance(SERVO_HALF_LEFT);
  if(isBumped()){
    return left > (IR_SENSOR_BUMP - IR_WARN);
  }
  int right = glance(SERVO_HALF_RIGHT);
  return left < right;
}

uint16_t randomAngle(void)
{

  // 20 - 180 degree angle
  //return (random() % 160) + 21;
  random(20,180);

  // uncomment for 53 - 180 degree angle
  //return (53 + ((uint16_t)(random() & 0xFF) >> 1));
}




