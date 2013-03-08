#ifndef CMCONTROL_H
#define CMCONTROL_H

#define IR_LEFT 129
#define IR_FORWARD 130
#define IR_RIGHT 131
#define IR_SPOT 132
#define IR_MAX 133
#define IR_CLEAN 136
#define IR_PAUSE 137
#define IR_POWER 138

// Custom Sensors
#define IR_CUSTOM_SENSOR 0
#define PROXIMITY_SENSOR 1

#define CUSTOM_SENSORS_BUFFER 4

#define IR_SENSOR_BUMP 320
#define PROXIMITY_BUMP 13

#define MANUAL_INTERVAL 40

// Servo positions
#define SERVO_PIN 9
#define SERVO_MAX_POS 160
#define SERVO_MIN_POS 0
#define SERVO_CENTER_POS SERVO_MAX_POS/2
#define SERVO_MOVE_DELAY 400

#define SERVO_HALF_LEFT (SERVO_MAX_POS - 40)
#define SERVO_HALF_RIGHT (SERVO_MIN_POS + 40)
#define SERVO_LEFT (SERVO_MAX_POS - 20)
#define SERVO_RIGHT (SERVO_MIN_POS + 20)

/*#define LOOK_CENTER 15
#define LOOK_MAX_RIGHT 5
#define LOOK_MAX_LEFT 21
#define LOOK_RIGHT 9
#define LOOK_LEFT 21
#define LOOK_MID_RIGHT 12
#define LOOK_MID_LEFT 18*/

#define SERVO_DELAY 50

enum {
        SPIN_NONE,
        SPIN_LEFT,
        SPIN_RIGHT
};

#endif
