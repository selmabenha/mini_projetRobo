#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors_pro.h>
#include <create_path.h>
#include <process_image.h>

static bool freeze = 0;

//decides whether the motors stop or go depending on information from the TOF sensor
void init_path(int16_t index) {

	if(freeze){
		motors_stop();
		process_path(index);
	} else {
		motors_go();
	}

}

//decides in which direction the e-puck should turn depending on information from the microphones and camera sensors
void process_path(int16_t index) {
	//turn left
	if(index >= FREQ_LEFT_L && index <= FREQ_LEFT_H){
		motors_turn_left();
	}
	//turn right
	else if(index >= FREQ_RIGHT_L && index <= FREQ_RIGHT_H){
		motors_turn_right();
	}
	//is a line found?
	else if(get_pathFound()) {
		motors_turn_left();
		//turn around 180 degrees?
	}
}
