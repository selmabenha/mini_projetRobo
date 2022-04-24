/*
 * tof.c
 *
 *  Created on: Apr 28, 2022
 *      Author: mehdiamor
 */

#define stopDistance 	70

#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>

#include <leds.h>
#include <audio_processing.h>
#include <sensors/VL53L0X/VL53L0X.h>
#include <tof.h>
#include <arm_math.h>

static bool freeze = false;

static BSEMAPHORE_DECL(startDetect_sem, TRUE);

static THD_WORKING_AREA(theTimeOfFlight, 256);
static THD_FUNCTION(TimeOfFlight, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    systime_t time;

    while(1){
        time = chVTGetSystemTime();

        if(VL53L0X_get_dist_mm() < stopDistance) {
        	chBSemSignal(&startDetect_sem);
        	freeze = true;
        } else {
        	freeze = false;
        }

        //100Hz
        chThdSleepUntilWindowed(time, time + MS2ST(10));
    }
}

void start_tof(void){
	chThdCreateStatic(theTimeOfFlight, sizeof(theTimeOfFlight), NORMALPRIO + 1, TimeOfFlight, NULL);
}

void wait_detectStart(void) {
	chBSemWait(&startDetect_sem);
}

bool get_freeze(void) {
	return freeze;
}

void set_freeze(bool frozen) {
	freeze = frozen;
}


