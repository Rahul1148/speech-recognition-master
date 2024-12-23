/*

 */

#ifndef __SPLIT_H__
#define __SPLIT_H__

#include "frame.h"
#include <sys/types.h>

/*
 * Structure that records a speech signal by storing its PCM data
 * (int16_t *) buffer	The PCM values ​​of the signal
 * (unsigned int) n	The number of values


 */
typedef
	struct
	{
		int16_t *buffer;
		unsigned int number;
	} voice_signal;

/*
 * <<<INPUT>>>
 * (int16_t *) buffer		The PCM values ​​of the signals
 * (unsigned int) n		The number of values
 * (voice_signal **) signals	Address of a voice_signal pointer so that memory can be reserved and filled with the PCM values ​​of recognized sounds and words
 * <<<OUTPUT>>>
 * (int)			The number of recognized words, i.e. the length of the result vector
 */
int split(int16_t *buffer, unsigned int n, voice_signal **signals);

#endif
