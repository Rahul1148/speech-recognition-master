/*
 */

#include "frame.h"
#include "fft.h"
#include "wave.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//Overlapping of frames
#define OVERLAP (N / 8)

//List of medium frequencies; Bandwidth from the center frequency adjacent to the left to the center frequency adjacent to the right
static int mel_filters[N_FILTER] = {150, 200, 250, 300, 350, 400, 450,	//Linear
				490, 560, 640, 730, 840, 960, 1100,	//500-1000Hz Logarithm
				1210, 1340, 1480, 1640, 1810, 2000,	//1000-2000Hz Logarithm
				2250, 2520, 2840, 3190, 3580, 4020};	//2000-4000Hz Logarithm

unsigned int make_frames_hamming(int16_t *buffer, unsigned int n, frame **frames)
{
	//The frames partially overlap
	unsigned int frame_number = (n / (N - OVERLAP)) - 1;
	comp *result = malloc(sizeof(comp) * frame_number * N);
	comp *data = malloc(sizeof(comp) * frame_number * N);
	unsigned int pos = 0, i = 0, j = 0;
	double pi = 4 * atan(1.0);

	*frames = malloc(sizeof(frame) * frame_number);
	//Create the frames
	for (i = 0; i < frame_number; i++)
	{
		pos = (i + 1) * (N - 64);
		for (j = 0; j < N; j++)
		{
			data[i * N + j].real = buffer[pos + j] * (0.54 + 0.46 * cos(2 * pi * (j - N / 2) / N));
			data[i * N + j].imag = 0;
		}
	}
	//Transform each frame
	for (i = 0; i < frame_number; i++)
		fft(data + i * N, result + i * N, N);
	//Calculate the intensities and ignore the phase shift
	for (i = 0; i < frame_number; i++)
		for (j = 0; j < N; j++)
			(*frames)[i].magnitudes[j] = sqrt(pow(result[i * N + j].real, 2) + pow(result[i * N + j].imag, 2));
	
	for (i = 0; i < frame_number; i++)
	{
		double mean = 0;
		for (j = 0; j < N; j++)
			mean += (*frames)[i].magnitudes[j];
		mean /= N;
		for (j = 0; j < N; j++)
			(*frames)[i].magnitudes[j] /= mean;
	}
	free(data);
	free(result);
	return frame_number;
}

void mfcc_features(frame *frames, unsigned int n, mfcc_frame *mfcc_frames)
{

	unsigned int i = 0, j = 0, k = 0;
	double filterOutput[N_FILTER];
	double filterSpectrum[N_FILTER][N] = {{0}};
	double c0 = sqrt(1.0 / N_FILTER);
	double cn = sqrt(2.0 / N_FILTER);
	double pi = 4 * atan(1.0);

	
	for (i = 0; i < N_FILTER; i++)
	{
		double maxFreq = 0, minFreq = 0, centerFreq = 0;

		if (i == 0)
			minFreq = mel_filters[0] - (mel_filters[1] - mel_filters[0]);
		else
			minFreq = mel_filters[i - 1];
		centerFreq = mel_filters[i];

		if (i == N_FILTER - 1)
			maxFreq = mel_filters[N_FILTER - 1] + (mel_filters[N_FILTER - 1] - mel_filters[N_FILTER - 2]);
		else
			maxFreq = mel_filters[i + 1];

		for (j = 0; j < N; j++)
		{
			double freq = 1.0 * j * WAVE_SAMPLE_RATE / N;
			if (freq > minFreq && freq < maxFreq)
				
				if (freq < centerFreq)
					filterSpectrum[i][j] = 1.0 * (freq - minFreq) / (centerFreq - minFreq);
				else
					filterSpectrum[i][j] = 1 - 1.0 * (freq - centerFreq) / (maxFreq - centerFreq);
			else
				filterSpectrum[i][j] = 0;
		}
	}

	//Calculate the MFCC features for each frame.

	for (i = 0; i < n; i++)
	{
		
		for (j = 0; j < N_FILTER; j++)
		{
			filterOutput[j] = 0;
			for (k = 0; k < N; k++)
				filterOutput[j] += frames[i].magnitudes[k] * filterSpectrum[j][k];
		}
		
		for (j = 0; j < N_MFCC; j++)
		{
			mfcc_frames[i].features[j] = 0;

			for (k = 0; k < N_FILTER; k++)
				mfcc_frames[i].features[j] += log(fabs(filterOutput[k]) + 1e-10) * cos((pi * (2 * k + 1) * j) / (2 * N_FILTER));

			if (j)
				mfcc_frames[i].features[j] *= cn;
			else
				mfcc_frames[i].features[j] *= c0;
		}
	}
}
