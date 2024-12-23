/*
 */

#include "fft.h"

#include <math.h>
#include <stdlib.h>

void fft(comp *time, comp *freq, unsigned int n)
{
	double pi = 4 * atan(1.0);
	unsigned int i = 0, j = 0, k = 0;
	unsigned int bits = 0;

	if (n == 1)
	{
		freq[0] = time[0];
		return;
	}

	//Count the bits needed to represent n in binary form

	for (i = n; i > 1; i /= 2)
		bits++;

	//Change the indexing to split into halves
	for (i = 0; i < n; i++)
	{
		int reversed = 0;
		for (j = 0; j < bits; j++)
			reversed |= (((i >> j) % 2) << (bits - j - 1));
		freq[reversed] = time[i];
	}

	
	for (i = bits; i > 0; i--)
	{
		unsigned int length_groups = (n >> (i - 1));
		unsigned int num_groups = n / length_groups;
		//
		for (j = 0; j < num_groups; j++)
		{
			for (k = 0; k < length_groups / 2; k++)
			{
				//number in the first half
				unsigned int off1 = length_groups * j + k;
				//Number in the second half
				unsigned int off2 = off1 + length_groups / 2;
				comp off1_val, off2_val;
				double cosine = cos((-2 * pi * k) / length_groups);
				double sine = sin((-2 * pi * k) / length_groups);
				//Multiply the complex numbers
				off1_val.real = freq[off1].real + (freq[off2].real * cosine - freq[off2].imag * sine);
				off1_val.imag = freq[off1].imag + (freq[off2].imag * cosine + freq[off2].real * sine);
				off2_val.real = freq[off1].real - (freq[off2].real * cosine - freq[off2].imag * sine);
				off2_val.imag = freq[off1].imag - (freq[off2].imag * cosine + freq[off2].real * sine);
				freq[off1] = off1_val;
				freq[off2] = off2_val;
			}
		}
	}
	//Normalize the result
	for (i = 0; i < n; i++)
	{
		freq[i].real /= n;
		freq[i].imag /= n;
		freq[i].imag *= -1;
	}
}
