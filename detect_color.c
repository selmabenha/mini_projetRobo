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
#define WIDTH_SLOPE_RED				500 //experimental value
#define MIN_LINE_WIDTH_RED			200 //experimental value
#define WIDTH_SLOPE_GREEN				500 //experimental value
#define MIN_LINE_WIDTH_GREEN			200 //experimental value
#define WIDTH_SLOPE_BLUE				500 //experimental value
#define MIN_LINE_WIDTH_BLUE			200 //experimental value

#define PXTOCM					1570.0f //experimental value
#define GOAL_DISTANCE 			10.0f
#define MAX_DISTANCE 			25.0f
#define DETECT_NUM				5    //experimental value
#define BLUE_FILTER				0xF0 //experimental value //0xF0
#define RED_FILTER				0x1E //experimental value //0x1F

static bool pathFound = 0;
static bool impasseFound = 0;

//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);

/*
 *  Returns the line's width extracted from the image buffer given
 *  Returns 0 if line not found
 */
bool verify_line_color(uint8_t *buffer, uint16_t width, uint16_t slope) {
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

		//detection process is verified in an array of size DETECT_NUM to help eliminate noise
		for(int i = 0; i < DETECT_NUM; i++) {
			//waits until an image has been captured
			chBSemWait(&image_ready_sem);
			//gets the pointer to the array filled with the last image in RGB565
			img_buff_ptr = dcmi_get_last_image_ptr();

			//filters the image into the blue and red tables
			filter_image(img_buff_ptr);

			//search for a line in the image
			red_lineCheck[i] = verify_line_color(red_image);
			blue_lineCheck[i] = verify_line_color(blue_image);
		}

		//finalizes the detection of the red or blue line so that they can be read from the ControlMovement thread
		if(send_to_computer){
			set_led(1,1);
		}
		send_to_computer = !send_to_computer;
		for(int i = 0; i < DETECT_NUM; i++) {
			red_lineCheck[i] = 0;
			blue_lineCheck[i] = 0;
		}

	}
}

void filter_image(uint8_t *img_ptr) {
	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2){
		//Extracts only the red pixels - fills the blue table
		blue_image[i/2] = (uint8_t)img_ptr[i]&BLUE_FILTER;

		//Extracts only the blue pixels - fills the red table
		red_image[i/2] = (uint8_t)(img_ptr[i + 1]&RED_FILTER);
	}
}

bool read_table(bool table[]) {
	for(uint8_t i = 0; i < DETECT_NUM; i++) {
		if(!table[i]) {
			return false;
		}
	}
	return true;
}

void process_image_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO+1, ProcessImage, NULL);
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO+1, CaptureImage, NULL);
}

bool get_pathFound(void) {
	return pathFound;
}
void set_pathFound(bool path) {
	pathFound = path;
}
