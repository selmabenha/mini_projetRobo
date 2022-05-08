#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <motors.h>
#include <camera/po8030.h>
#include <chprintf.h>

#include <pi_regulator.h>
#include <process_image.h>

void SendUint8ToComputer(uint8_t* data, uint16_t size) 
{
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)"START", 5);
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)&size, sizeof(uint16_t));
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)data, size);
}

static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}

int main(void)
{

	halInit();
	chSysInit();
	mpu_init();

	//initializes melody
	playMelodyStart();

	//starts the serial communication
	serial_start();
	//starts the USB communication
	usb_start();
	//inits the motors
	motors_init();
	//starts the TOF sensors
	VL53L0X_start();
	//start the TOF thread
	start_tof();

	//starts the camera
	dcmi_start();
	po8030_start();
	po8030_set_awb(0);
	//starts the threads for the processing of the image
	detect_color_start();
	dac_start();

	//starts the threads for the control of the motors and audio processing
	control_mov_start();
	control_audio_start();

	/* Infinite loop. */
	while (1) {
		wait_send_to_computer();
	}
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
