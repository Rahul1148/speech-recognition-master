/*
*/

#ifndef __FFT_H__
#define __FFT_H__

typedef
	struct
	{
		double real;
		double imag;
	}
comp;

void fft(comp *time, comp *freq, unsigned int n);

#endif
