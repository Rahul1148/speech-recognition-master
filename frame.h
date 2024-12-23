/*

 */

#ifndef __FRAME_H__
#define __FRAME_H__

#include <sys/types.h>

#define N 512
#define N_FILTER 26
#define N_MFCC N_FILTER 

typedef
	struct
	{
		double magnitudes[N];
	} frame;

typedef
	struct
	{
		double features[N_MFCC];
	} mfcc_frame;

unsigned int make_frames_hamming(int16_t *buffer, unsigned int n, frame **frames);
void mfcc_features(frame *frames, unsigned int n, mfcc_frame *mfcc_frames);

#endif
