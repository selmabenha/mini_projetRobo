/*
 * motors_pro.c
 *
 *  Created on: May 15, 2022
 *      Author: mehdiamor
 */

#include <ch.h>
#include <hal.h>
#include "motors_pro.h"

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

	left_motor_set_speed(SPEED);
	right_motor_set_speed(-SPEED);

}

void motors_turn_left(void){

	left_motor_set_speed(-SPEED);
	right_motor_set_speed(SPEED);

}
