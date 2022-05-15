/*
 * return_path.c
 *
 *  Created on: May 5, 2022
 *      Author: selmabenhassine
 */

#include <create_path.h>

#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>

#include <leds.h>
#include <audio/play_melody.h>
#include <audio_processing.h>
#include <detect_color.h>
#include <tof.h>
#include <motors_pro.h>
#include <arm_math.h>

#define FREQ_RIGHT	 	12.81	//200Hz
#define FREQ_LEFT	  	38.43	//600Hz
#define FREQ_LEFT_L			(FREQ_LEFT-3)
#define FREQ_LEFT_H			(FREQ_LEFT+3)
#define FREQ_RIGHT_L		(FREQ_RIGHT-3)
#define FREQ_RIGHT_H		(FREQ_RIGHT+3)
#define GAME_OVER_TIME		15000	//15 seconds
#define WARN_TIME			100

static bool end_of_path = 0;
static bool turn_around = 0;
static uint8_t game_over = 0;

static THD_WORKING_AREA(theControlMovement, 256);
static THD_FUNCTION(ControlMovement, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;

	while(!get_end_of_path()){
		time = chVTGetSystemTime();
		//once a red or blue line has been detected, the corresponding bool value must stay true until the action has been completed at
		//the wall, where the line may not be detectable anymore
		if(!get_end_of_path()) {set_end_of_path(get_pathFound());}
		if(!get_turn_around()) {set_turn_around(get_impasseFound());}


		init_path(get_max_norm_index());
		//100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10));

		//defines the end of the program once the countdown runs to the end
		set_end_of_path(time_path());

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
		motors_spin();
		playMelody(MARIO_FLAG, 0, NULL);
		waitMelodyHasFinished();
		set_led(1,1);
	} else if(get_impasseFound()) {
		motors_u_turn();
		playMelody(WALKING, 0, NULL);
		waitMelodyHasFinished();
		set_led(3,1);
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
