#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors_pro.h>
#include <create_path.h>
#include <process_image.h>

static THD_WORKING_AREA(theControlMovement, 256);
static THD_FUNCTION(ControlMovement, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;

	while(!get_pathFound()) {
		time = chVTGetSystemTime();
		waitDetectStart();
		init_path(get_max_norm_index());
	}
}

void control_mov_start(void) {
	chThdCreateStatic(theControlMovement, sizeof(theControlMovement), NORMALPRIO, ControlMovement, NULL);
}

void control_mov_end(void) {
	chThdTerminate(theControlMovement);
}

//decides whether the motors stop or go depending on information from the TOF sensor
void init_path(int16_t index) {

	if(get_freeze()){
		motors_stop();
		process_path(index);
	} else if(get_pathFound()){
		motors_turn_right();
		motors_turn_right();
		motors_go();
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

void time_path(void) {
	if(!get_pathFound()) {
		if(!chVTIsSystemTimeWithinX(0, GAME_OVER_TIME) && game_over == 0) {
			set_led(1,1);
			playNote(NOTE_C5, WARN_TIME);
			game_over++;
		}
		if(!chVTIsSystemTimeWithinX(0, GAME_OVER_TIME*2) && game_over == 1) {
			set_led(3,1);
			playNote(NOTE_C5, WARN_TIME);
			game_over++;
		}
		if(!chVTIsSystemTimeWithinX(0, GAME_OVER_TIME*3) && game_over == 2) {
			set_led(5,1);
			playNote(NOTE_C5, WARN_TIME);
			game_over++;
		}
		if(!chVTIsSystemTimeWithinX(0, GAME_OVER_TIME*4) && game_over == 3) {
			set_led(8,1);
			playMelody(MARIO_DEATH, 0, ML_FORCE_CHANGE);
			motors_stop();
			waitMelodyHasFinished();
			return true;
		}
		return false;
	}
	clear_leds();
	return false;
}
