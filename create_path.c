#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors.h>
#include <create_path.h>
#include <process_image.h>

#define SPEED			600
bool freeze = 0;

//decides whether the motors stop or go depending on information from the TOF sensor
void init_path(int16_t index) {

	if(freeze){
		left_motor_set_speed(0);
		right_motor_set_speed(0);
		process_path(index);
	} else {
		left_motor_set_speed(SPEED);
		right_motor_set_speed(SPEED);
	}

}

//decides in which direction the e-puck should turn depending on information from the microphones and camera sensors
void process_path(int16_t index) {
	//turn left
	if(index >= FREQ_LEFT_L && index <= FREQ_LEFT_H){
		left_motor_set_speed(-SPEED);
		right_motor_set_speed(SPEED);
	}
	//turn right
	else if(index >= FREQ_RIGHT_L && index <= FREQ_RIGHT_H){
		left_motor_set_speed(SPEED);
		right_motor_set_speed(-SPEED);
	}
	//is a line found?
	else if(get_pathFound()) {
		left_motor_set_speed(-SPEED);
		right_motor_set_speed(SPEED);
		//turn around 180 degrees?
	}
}
