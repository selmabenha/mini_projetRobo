#ifndef CREATE_PATH_H_
#define CREATE_PATH_H_

#include <sys/_stdint.h>
void init_path(int16_t index);
void process_path(int16_t index);

void control_mov_start(void);
void control_mov_end(void);

void time_path(void);

#endif /* CREATE_PATH_H_ */
