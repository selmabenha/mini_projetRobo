#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H

uint16_t get_line_position(void);
void process_image_start(void);
bool verify_line_color(uint8_t *buffer);

bool get_pathFound(void);
void set_pathFound(bool path);

#endif /* PROCESS_IMAGE_H */
