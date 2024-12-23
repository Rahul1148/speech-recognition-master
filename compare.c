/*

 */

#include "compare.h"

#include <math.h>
#include <stdlib.h>

double compare(mfcc_frame *mfcc_frames1, unsigned int n1, mfcc_frame *mfcc_frames2, unsigned int n2)
{
	double distances[n1 + 1][n2 + 1];
	unsigned int i = 0, j = 0, k = 0;

	
	for (i = 0; i < n1; i++)
	{
		for (j = 0; j < n2; j++)
		{
			distances[i + 1][j + 1] = 0;
			for (k = 0; k < N_MFCC; k++)
				distances[i + 1][j + 1] += pow(mfcc_frames1[i].features[k] - mfcc_frames2[j].features[k], 2);
			distances[i + 1][j + 1] = sqrt(distances[i + 1][j + 1]);
		}
	}

	
	for (i = 0; i <= n1; i++)
		distances[i][0] = atof("Inf");
	for (i = 0; i <= n2; i++)
		distances[0][i] = atof("Inf");

	distances[0][0] = 0;

	
	for (i = 1; i <= n1; i++)
		for (j = 1; j <= n2; j++)
		{
			
			double prev_min = distances[i - 1][j];
			if (distances[i - 1][j - 1] < prev_min)
				prev_min = distances[i - 1][j - 1];
			if (distances[i][j - 1] < prev_min)
				prev_min = distances[i][j - 1];
			
			distances[i][j] += prev_min;
		}

	
	return distances[n1][n2] / sqrt(pow(n1, 2) + pow(n2, 2));
}
