/*
 * motors_pro.h
 *
 *  Created on: May 15, 2022
 *      Author: mehdiamor
 */

#ifndef MOTORS_PRO_H_
#define MOTORS_PRO_H_


#include <stdint.h>
#include <hal.h>

#define MOTOR_SPEED_LIMIT 1100 // [step/s]

void motors_init_pos(void);
void motors_go(void);
void motors_stop(void);
void motors_turn_right(void);
void motors_turn_left(void);
void motors_u_turn(void);
void motors_spin(void);


#endif /* MOTORS_PRO_H_ */
