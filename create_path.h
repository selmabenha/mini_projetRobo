/*
 * create_path.h
 *
 *  Created on: May 5, 2022
 *      Author: selmabenhassine
 */

#ifndef CREATE_PATH_H_
#define CREATE_PATH_H_

#include <sys/_stdint.h>
void init_path(int16_t index);
void process_path(int16_t index);
void control_mov_start(void);
_Bool time_path(void);

_Bool get_end_of_path(void);
void set_end_of_path(_Bool end);
_Bool get_turn_around(void);
void set_turn_around(_Bool turn);

#endif /* CREATE_PATH_H_ */
