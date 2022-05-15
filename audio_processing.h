#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H


#define FFT_SIZE 	1024


void control_audio_start(void);

void processAudioData(int16_t *data, uint16_t num_samples);

/*
*	put the invoking thread into sleep until it can process the audio data
*/
void wait_send_to_computer(void);


//setter and getter for static variable
int16_t get_max_norm_index(void);
void set_max_norm_index(int16_t index);


#endif /* AUDIO_PROCESSING_H */
