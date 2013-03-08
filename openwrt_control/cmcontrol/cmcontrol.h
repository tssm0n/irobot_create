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

#define CUSTOM_SENSORS_BUFFER 2

#define IR_SENSOR_BUMP 300 
#define PROXIMITY_BUMP 13

// Servo positions
#define LOOK_CENTER 15
#define LOOK_MAX_RIGHT 5
#define LOOK_MAX_LEFT 21
#define LOOK_RIGHT 9
#define LOOK_LEFT 21
#define LOOK_MID_RIGHT 12
#define LOOK_MID_LEFT 18

#define SERVO_DELAY 50

enum {
	SPIN_NONE,
	SPIN_LEFT,
	SPIN_RIGHT
};

#endif
