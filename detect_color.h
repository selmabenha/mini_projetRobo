/*
 * detect_color.h
 *
 *  Created on: Apr 30, 2022
 *      Author: mehdiamor
 */

#ifndef DETECT_COLOR_H_
#define DETECT_COLOR_H_

//start and end of image capture & processing threads
void detect_color_start(void);
void detect_color_end(void);

//setter and getter of static bool variables
bool get_pathFound(void);
void set_pathFound(bool path);
bool get_impasseFound(void);
void set_impasseFound(bool imp);

//checks that all values of an array are true
bool read_table(bool table[]);

//applies a red and blue mask to the image and places the  result in static arrays
void filter_image(uint8_t *img_ptr);

//function that detects whether a colored bar is present on a white background
//white includes red, green, and blue pixels
//a colored bar would indicate a lack of red, green, or blue pixels
bool verify_line_color(uint8_t *buffer);

#endif /* DETECT_COLOR_H_ */
