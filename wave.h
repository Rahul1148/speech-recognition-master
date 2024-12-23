/*
 */

#ifndef __WAVE_H__
#define __WAVE_H__

#define WAVE_FORMAT 0x001 //PCM
#define WAVE_CHANNELS 1
#define WAVE_SAMPLE_RATE 44100
#define WAVE_BITS_PER_SAMPLE 16
#define WAVE_FRAME_SIZE ((WAVE_BITS_PER_SAMPLE + 7) >> 3 * WAVE_CHANNELS) //(bits/sample + 7) / 8 * channels
#define WAVE_BYTES_PER_SECOND (WAVE_SAMPLE_RATE * WAVE_FRAME_SIZE) //framesize * samplerate

int open_wave(const char *path);
unsigned int read_pcm(void **buffer);
int write_pcm(const void *buffer, const unsigned int size, const char *path);
void close_wave(void);

#endif
