/*
 * tof.h
 *
 *  Created on: Apr 23, 2022
 * 		Author: mehdiamor
 */

#ifndef TOF_H_
#define TOF_H_

void start_tof(void);
bool get_freeze(void);
void set_freeze(bool frozen);
void wait_detectStart(void);


#endif /* TOF_H_ */
