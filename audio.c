
#include "audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <alsa/asoundlib.h>



#define FRAMES_IN_BLOCK 0x1000

#define BLOCKSIZE (FRAMES_IN_BLOCK * (WAVE_BITS_PER_SAMPLE / 8))


#define PCM_FORMAT SND_PCM_FORMAT_S16_LE


struct block
{
	void *buf;
	struct block *next;
};


struct block *head = NULL;

snd_pcm_t *handle = NULL;

int finish = 2;

unsigned int blocknumber = 0;

void capture(void);

int capture_start(const char *device)
{
	unsigned int freq = WAVE_SAMPLE_RATE;
	int freq_adjust_direction = 1;
	snd_pcm_hw_params_t *hw_params = NULL;
	pthread_t t;

	
	if (snd_pcm_open(&handle, !device ? "default" : device, SND_PCM_STREAM_CAPTURE, 0) < 0)
	{
		fprintf(stderr, "Could device %s don't open\n", !device ? "default" : device);
		return -1;
	}

	if (snd_pcm_hw_params_malloc(&hw_params) < 0)
	{
		fprintf(stderr, "Error reserving memory for hardware parameters\n");
		return -2;
	}
	
	if (snd_pcm_hw_params_any(handle, hw_params) < 0)
	{
		fprintf(stderr, "Error getting the Standard-Hardware-Parameter\n");
		return -3;
	}
	
	if (snd_pcm_hw_params_set_access(handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
	{
		fprintf(stderr, "Error setting read\\write access\n");
		return -4;
	}
	if (snd_pcm_hw_params_set_format(handle, hw_params, PCM_FORMAT) < 0)
	{
		fprintf(stderr, "Error setting the PCM format\n");
		return -5;
	}
	if (snd_pcm_hw_params_set_rate_near(handle, hw_params, &freq, &freq_adjust_direction) < 0)
	{
		fprintf(stderr, "Error setting the Frequenz\n");
		return -6;
	}
	if (snd_pcm_hw_params_set_channels(handle, hw_params, WAVE_CHANNELS) < 0)
	{
		fprintf(stderr, "Error setting the number of channels\n");
		return -7;
	}
	if (snd_pcm_hw_params(handle, hw_params) < 0)
	{
		fprintf(stderr, "Error setting the Hardwareparameter\n");
		return -8;
	}
	
	snd_pcm_hw_params_free(hw_params);

	if (snd_pcm_prepare(handle) < 0)
	{
		fprintf(stderr, "Error setting the Handles\n");
		return -9;
	}

	finish = 0;
	if (pthread_create(&t, NULL, (void *)capture, NULL) < 0)
	{
		fprintf(stderr, "Error starting recording thread\n");
		return -10;
	}
	return 0;
}

void capture_stop(unsigned int *size, void **buffer)
{
	unsigned int i = 0;
	struct block *now = NULL;
	
	void **adresses = NULL;

	
	finish = 1;
	while (finish != 2);
	
	snd_pcm_close(handle);
	
	*buffer = malloc(BLOCKSIZE * blocknumber);
	*size = BLOCKSIZE * blocknumber;
       	adresses = malloc(blocknumber * sizeof(void *));
	
	now = head;
	for (i = 0; i < blocknumber; i++)
	{
		now = now->next;
		adresses[i] = now;
		memcpy(*buffer + (i * BLOCKSIZE), now->buf, BLOCKSIZE);
		free(now->buf);
	}

	for (i = 0; i < blocknumber; i++)
		free(adresses[i]);
	free(head);
	free(adresses);
}

int play(const char *device, const void *buffer, const unsigned int size)
{
	int r = 0;
	unsigned int freq = WAVE_SAMPLE_RATE;
	unsigned int framenumber = size / (WAVE_BITS_PER_SAMPLE / 8);
	snd_pcm_hw_params_t *hw_params = NULL;

	
	if (snd_pcm_open(&handle, !device ? "default" : device, SND_PCM_STREAM_PLAYBACK, 0) < 0)
	{
		fprintf(stderr, "Konnte Geraet %s nicht oeffnen\n", !device ? "default" : device);
		return -1;
	}
	
	if (snd_pcm_hw_params_malloc(&hw_params) < 0)
	{
		fprintf(stderr, "Error reserving memory for hardware parameters\n");
		return -2;
	}

	if (snd_pcm_hw_params_any(handle, hw_params) < 0)
	{
		fprintf(stderr, "Error getting default hardware parameters\n");
		return -3;
	}
	//Setting the hardware parameters within the structure
	if (snd_pcm_hw_params_set_access(handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
	{
		fprintf(stderr, "Error setting read\\write access\n");
		return -4;
	}
	if (snd_pcm_hw_params_set_format(handle, hw_params, PCM_FORMAT) < 0)
	{
		fprintf(stderr, "Error setting the PCM-Formates\n");
		return -5;
	}
	if (snd_pcm_hw_params_set_rate_near(handle, hw_params, &freq, 0) < 0)
	{
		fprintf(stderr, "Error setting the Frequenz\n");
		return -6;
	}
	if (snd_pcm_hw_params_set_channels(handle, hw_params, WAVE_CHANNELS) < 0)
	{
		fprintf(stderr, "Error setting the number of channels\n");
		return -7;
	}
	
	{
		fprintf(stderr, "Error setting the Hardwareparameter\n");
		return -8;
	}
	
	snd_pcm_hw_params_free(hw_params);
	//Setting the  Hardware-Parameter Handle
	if (snd_pcm_prepare(handle) < 0)
	{
		fprintf(stderr, "Error setting the number of Handles\n");
		return -9;
	}
	//Playing the data in the buffer
	do
	{
		r = snd_pcm_writei(handle, buffer + (size - framenumber * (WAVE_BITS_PER_SAMPLE / 8)), framenumber);
		if (r < 0)
			fprintf(stderr, "WARNING: Error writing Audio\n");
		else
			framenumber -= r;
	} while (framenumber > 0);
	
	snd_pcm_close(handle);
	return 0;
}

void get_device_list(char ****devices, unsigned int *number)
{
	unsigned int i = 0;
	void **hints = NULL;
	//Hole die Liste der PCM-Geraete
	snd_device_name_hint(-1, "pcm", &hints);
	//Anzahl der Geraete ermitteln
	while (hints[i])
		i++;
	*number = i;
	//Speicher reservieren
	*devices = malloc(sizeof(char **) * i);
	//Eigenschaften der einzelnen Geraete ermitteln
	i = 0;
	while (hints[i])
	{
		char *ioid = NULL;
		(*devices)[i] = malloc(sizeof(char *) * 3);
		//Name holen
		(*devices)[i][0] = snd_device_name_get_hint(hints[i], "NAME");
		//Beschreibung holen
		(*devices)[i][1] = snd_device_name_get_hint(hints[i], "DESC");
		//Typ holen
		ioid = snd_device_name_get_hint(hints[i], "IOID");
		//Input oder Output
		if (ioid)
			(*devices)[i][2] = ioid;
		//Beides
		else
		{
			(*devices)[i][2] = malloc(sizeof(char) * 7);
			strncpy((*devices)[i][2], "Beides\0", 7);
		}
		i++;
	}
	//Speicher freigeben
	snd_device_name_free_hint(hints);
}

void capture(void)
{
	struct block *now = NULL;
	int r;

	//Initialisieren
	blocknumber = 0;
	head = malloc(sizeof(struct block));
	head->buf = NULL;
	head->next = NULL;
	
	now = head;
	while (!finish)
	{
	
		unsigned int framenumber = FRAMES_IN_BLOCK;
		blocknumber++;
		now->next = malloc(sizeof(struct block));
		now = now->next;
		now->buf = malloc(BLOCKSIZE);
		now->next = NULL;
		
		do
		{
			r = snd_pcm_readi(handle, now->buf + (FRAMES_IN_BLOCK - framenumber) * (WAVE_BITS_PER_SAMPLE / 8), framenumber);
			if (r < 0)
				fprintf(stderr, "WARNING: Fehler beim Lesen von Audio\n");
			else
				framenumber -= r;
		} while (framenumber > 0);
	}

	finish = 2;
}
