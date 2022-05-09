/*
 * detect_color.c
 *
 *  Created on: Apr 30, 2022
 *      Author: mehdiamor
 */

#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>

#include <main.h>
#include <camera/po8030.h>
#include <motors_pro.h>
#include <detect_color.h>
#include <audio/audio_thread.h>
#include <audio/play_melody.h>

#define IMAGE_BUFFER_SIZE		640
#define WIDTH_SLOPE				10
#define MIN_LINE_WIDTH			50
#define PXTOCM					1570.0f //experimental value
#define GOAL_DISTANCE 			10.0f
#define MAX_DISTANCE 			25.0f
#define DETECT_NUM				2
#define BLUE_FILTER				0xF0
#define RED_FILTER				0x1E

static bool pathFound = 0;
static bool impasseFound = 0;

//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);

/*
 *  Returns the line's width extracted from the image buffer given
 *  Returns 0 if line not found
 */
bool verify_line_color(uint8_t *buffer) {
	uint16_t i = 0, begin = 0, end = 0;
	uint8_t stop = 0;
	uint32_t mean = 0;

	//performs an average
	for(uint16_t i = 0 ; i < IMAGE_BUFFER_SIZE ; i++){
		mean += buffer[i];
	}
	mean /= IMAGE_BUFFER_SIZE;

	//search for a begin
	while(stop == 0 && i < (IMAGE_BUFFER_SIZE - WIDTH_SLOPE))
	{
		//the WIDTH_SLOPE must at least be "WIDTH_SLOPE" wide and is compared
		//to the mean of the image
		if(buffer[i] > (mean) && buffer[i+WIDTH_SLOPE] < (mean))
		{
			begin = i;
			stop = 1;
		}
		i++;
	}
	//if a begin was found, search for an end
	if (i < (IMAGE_BUFFER_SIZE - WIDTH_SLOPE) && begin)
	{
		stop = 0;

		while(stop == 0 && i < IMAGE_BUFFER_SIZE)
		{
			if(buffer[i] > (mean) && buffer[i-WIDTH_SLOPE] < (mean))
			{
				end = i;
				stop = 1;
			}
			i++;
		}
	}

	//return conditions
	if(end == 0 || begin == 0) {
		return false;
	} else if((end-begin) < MIN_LINE_WIDTH){
		return false;
	} else if(end != 0 && begin != 0){
		return true;
	} else {
		return false;
	}
}

static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	//Takes pixels 0 to IMAGE_BUFFER_SIZE of the line 10 + 11 (minimum 2 lines because reasons)
	po8030_advanced_config(FORMAT_RGB565, 0, 10, IMAGE_BUFFER_SIZE, 2, SUBSAMPLING_X1, SUBSAMPLING_X1);
	dcmi_enable_double_buffering();
	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
	dcmi_prepare();

    while(1){
        //starts a capture
		dcmi_capture_start();
		//waits for the capture to be done
		wait_image_ready();
		//signals an image has been captured
		chBSemSignal(&image_ready_sem);
    }
}


static THD_WORKING_AREA(waProcessImage, 1024);
static THD_FUNCTION(ProcessImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	uint8_t *img_buff_ptr;
	bool red_lineCheck[DETECT_NUM] = {0};
	bool blue_lineCheck[DETECT_NUM] = {0};
	set_pathFound(0);

	bool send_to_computer = true;

    while(1){
    	//waits until an image has been captured
        chBSemWait(&image_ready_sem);
		//gets the pointer to the array filled with the last image in RGB565
		img_buff_ptr = dcmi_get_last_image_ptr();

		//Extracts only the red pixels
		for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2){
			//extracts first 5bits of the first byte
			//takes nothing from the second byte
			//extracts only red
			red_image[i/2] = (uint8_t)img_buff_ptr[i]&0xF8;

			//extracts green
			green_image[i/2] = ((uint8_t)((img_buff_ptr[i]&(0x07)<<3)+ ((uint8_t)img_buff_ptr[i + 1]&(0xE0)>>5))); //green

			//extracts blue
			blue_image[i/2] = (uint8_t)(img_buff_ptr[i + 1]&0x1F);
		}

		//search for a line in the image
		if(verify_line_color(red_image)){
			set_pathFound(true);
		} else {
			set_pathFound(false);
		}

		if(send_to_computer){
			//sends to the computer the image
			SendUint8ToComputer(image, IMAGE_BUFFER_SIZE);
		}
		//invert the bool
		send_to_computer = !send_to_computer;
    }
}

uint16_t get_line_position(void){
	return line_position;
}

void process_image_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
}

bool get_pathFound(void) {
	return pathFound;
}
void set_pathFound(bool path) {
	pathFound = path;
}
