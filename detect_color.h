/*
 * detect_color.h
 *
 *  Created on: Apr 30, 2022
 *      Author: mehdiamor
 */

#ifndef DETECT_COLOR_H_
#define DETECT_COLOR_H_

void process_image_start(void);
bool verify_line_color(uint8_t *buffer);

bool get_pathFound(void);
void set_pathFound(bool path);

void filter_image(uint8_t *img_ptr);

bool read_table(bool table[]);

#endif /* DETECT_COLOR_H */
