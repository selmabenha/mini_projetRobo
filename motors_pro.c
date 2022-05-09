/*
 * motors_pro.c
 *
 *  Created on: May 15, 2022
 *      Author: mehdiamor
 */

#include <ch.h>
#include <hal.h>
#include "motors_pro.h"

#define MOTOR_TIMER_FREQ 100000 // [Hz]
#define THRESV 650 // This is the speed under which the power save feature is active.
#define PI                  3.1415926536f
//TO ADJUST IF NECESSARY. NOT ALL THE E-PUCK2 HAVE EXACTLY THE SAME WHEEL DISTANCE
#define WHEEL_DISTANCE      5.35f    //cm
#define PERIMETER_EPUCK     (PI * WHEEL_DISTANCE)
#define NSTEP_ONE_TURN      1000 // number of step for 1 turn of the motor
#define NSTEP_ONE_EL_TURN   4  //number of steps to do 1 electrical turn
#define NB_OF_PHASES        4  //number of phases of the motors
#define WHEEL_PERIMETER     13 // [cm]
#define SPEED				600

void motors_go(void) {
	left_motor_set_speed(SPEED);
	right_motor_set_speed(SPEED);
}

void motors_stop(void) {
	left_motor_set_speed(0);
	right_motor_set_speed(0);
}


void motors_turn_right(void){

	motors_init_pos();
	while(left_motor_get_pos() < (PERIMETER_EPUCK/4)*(NSTEP_ONE_TURN/WHEEL_PERIMETER)){
		left_motor_set_speed(SPEED);
		right_motor_set_speed(-SPEED);
	}

	motors_stop();
}

void motors_turn_left(void){

	motors_init_pos();
	while(right_motor_get_pos() < (PERIMETER_EPUCK/4)*(NSTEP_ONE_TURN/WHEEL_PERIMETER)){
		left_motor_set_speed(-SPEED);
		right_motor_set_speed(SPEED);
	}

	motors_stop();
}

void motors_init_pos(void) {
	left_motor_set_pos(0);
	right_motor_set_pos(0);
}

void motors_u_turn(void) {
	motors_turn_right();
	motors_turn_right();
}

void motors_spin(void) {
	motors_u_turn();
	motors_u_turn();
}
