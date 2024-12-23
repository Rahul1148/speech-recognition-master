/*
 */

#ifndef __COMPARE_H__
#define __COMPARE_H__

#include "frame.h"

/*

 * Algorithm Dynamic-Time-Warping benutzt.
 * <<<INPUT>>>
 * (mfcc_frame) mfcc_frames1	The first vector with the MFCC features
 * (unsigned int) n1		The length of the first vector
 * (mfcc_frame) mfcc_frames2	The second vector with the MFCC features
 * (unsigned int) n2		The length of the second vector
 * <<<OUTPUT>>>
 * (double)			Value indicating the correlation of the two vectors
 */
double compare(mfcc_frame *mfcc_frames1, unsigned int n1, mfcc_frame *mfcc_frames2, unsigned int n2);

#endif
