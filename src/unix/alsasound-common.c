/* 
 *  Arnold emulator (c) Copyright, Kevin Thacker 1995-2001
 *  
 *  This file is part of the Arnold emulator source code distribution.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#define HAVE_ALSA 1
#ifdef HAVE_ALSA

#define __ALSASOUND_COMMON_C__ 1

#include "../cpc/host.h"
#include "display.h"
#include "gtkui.h"
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include "../cpc/messages.h"
#include "sound.h"
#include "alsasound-common.h"

snd_pcm_t *playback_handle;
signed short *samples;
unsigned int chn;
snd_pcm_channel_area_t *areas;
//int commitBufferSize;

char *device = "plughw:0,0";                     /* playback device */
//char *device = "hw:0,0";                       /* playback device */
snd_pcm_format_t format_16 = SND_PCM_FORMAT_S16; /* sample format 16 */
snd_pcm_format_t format_8 = SND_PCM_FORMAT_U8;   /* sample format 8 */
snd_pcm_format_t format;                         /* sample format */
unsigned int rate = 44100;                       /* stream rate */
unsigned int channels = 2;                       /* count of channels */
snd_pcm_uframes_t arnold_frame_size = 880;	 /* period size in samples */
snd_pcm_uframes_t period_size = 880*2;		 /* period size in samples */
unsigned int periods = 2;			 /* number of periods */

snd_output_t *output = NULL;
snd_pcm_uframes_t offset;

BOOL alsa_audiodev_is_open = FALSE;

int set_hwparams(snd_pcm_t *handle,
                        snd_pcm_hw_params_t *params,
                        snd_pcm_access_t access)
{
        unsigned int rrate;
        int err, dir;

	printf("set_hwparams\n");
        /* choose all parameters */
        err = snd_pcm_hw_params_any(handle, params);
        if (err < 0) {
                printf("Broken configuration for playback: no configurations available: %s\n", snd_strerror(err));
                return err;
        }
        /* set the interleaved read/write format */
        err = snd_pcm_hw_params_set_access(handle, params, access);
        if (err < 0) {
                printf("Access type not available for playback: %s\n", snd_strerror(err));
                return err;
        }
        /* set the sample format */
        err = snd_pcm_hw_params_set_format(handle, params, format);
        if (err < 0) {
                printf("Sample format not available for playback: %s\n", snd_strerror(err));
                return err;
        }
        /* set the count of channels */
        err = snd_pcm_hw_params_set_channels(handle, params, channels);
        if (err < 0) {
                printf("Channels count (%i) not available for playbacks: %s\n", channels, snd_strerror(err));
                return err;
        }
        /* set the stream rate */
        rrate = rate;
        err = snd_pcm_hw_params_set_rate_near(handle, params, &rrate, 0);
        if (err < 0) {
                printf("Rate %iHz not available for playback: %s\n", rate, snd_strerror(err));
                return err;
        }
        if (rrate != rate) {
                printf("Rate doesn't match (requested %iHz, get %iHz)\n", rate, err);
                return -EINVAL;
        }
	dir = 0;
        err = snd_pcm_hw_params_set_period_size_near(handle, params, &period_size, &dir);
        if (err < 0) {
                printf("Unable to set period size %i for playback: %s\n", (int)period_size, snd_strerror(err));
                return err;
        }
        err = snd_pcm_hw_params_set_periods(handle, params, periods, 0);
        if (err < 0) {
                printf("Unable to set periods %i for playback: %s\n", periods, snd_strerror(err));
                return err;
        }
        /* write the parameters to device */
        err = snd_pcm_hw_params(handle, params);
        if (err < 0) {
                printf("Unable to set hw params for playback: %s\n", snd_strerror(err));
                return err;
        }

        err = snd_pcm_hw_params_get_periods(params, &periods, &dir);
        if (err < 0) {
                printf("Unable to get periods for playback: %s\n", snd_strerror(err));
                return err;
        }
        err = snd_pcm_hw_params_get_period_size(params, &period_size, &dir);
        if (err < 0) {
                printf("Unable to get period size for playback: %s\n", snd_strerror(err));
                return err;
        }
        return 0;
}

int set_swparams(snd_pcm_t *handle, snd_pcm_sw_params_t *swparams)
{
        int err;

        /* get the current swparams */
        err = snd_pcm_sw_params_current(handle, swparams);
        if (err < 0) {
                printf("Unable to determine current swparams for playback: %s\n", snd_strerror(err));
                return err;
        }
        /* start the transfer when the buffer is almost full: */
        /* (buffer_size / avail_min) * avail_min */
        //err = snd_pcm_sw_params_set_start_threshold(handle, swparams, (buffer_size / period_size) * period_size);
        err = snd_pcm_sw_params_set_start_threshold(handle, swparams, period_size * periods - arnold_frame_size);
        if (err < 0) {
                printf("Unable to set start threshold mode for playback: %s\n", snd_strerror(err));
                return err;
        }
        /* allow the transfer when at least period_size samples can be processed */
        err = snd_pcm_sw_params_set_avail_min(handle, swparams, arnold_frame_size);
        if (err < 0) {
                printf("Unable to set avail min for playback: %s\n", snd_strerror(err));
                return err;
        }
        /* write the parameters to the playback device */
        err = snd_pcm_sw_params(handle, swparams);
        if (err < 0) {
                printf("Unable to set sw params for playback: %s\n", snd_strerror(err));
                return err;
        }
        return 0;
}

/*
 *   Underrun and suspend recovery
 */
 
int xrun_recovery(snd_pcm_t *handle, int err)
{
	printf("xrun_recovery\n");
        if (err == -EPIPE) {    /* under-run */
                err = snd_pcm_prepare(handle);
                if (err < 0)
                        printf("Can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
                return 0;
        } else if (err == -ESTRPIPE) {
                while ((err = snd_pcm_resume(handle)) == -EAGAIN)
                        sleep(1);       /* wait until the suspend flag is released */
                if (err < 0) {
                        err = snd_pcm_prepare(handle);
                        if (err < 0)
                                printf("Can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
                }
                return 0;
        }
        return err;
}

BOOL	alsa_open_audio(BOOL use_mmap) {
	snd_pcm_hw_params_t *hwparams;
	snd_pcm_sw_params_t *swparams;
	int err;
	snd_pcm_access_t access = (use_mmap)?(SND_PCM_ACCESS_MMAP_INTERLEAVED)
		:(SND_PCM_ACCESS_RW_INTERLEAVED);
	format = (use_mmap)?(format_8):(format_16);

	if (alsa_audiodev_is_open) return TRUE;
	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_sw_params_alloca(&swparams);

	err = snd_output_stdio_attach(&output, stdout, 0);
	if (err < 0) {
		printf("Output failed: %s\n", snd_strerror(err));
		return 0;
	}

	if ((err = snd_pcm_open(&playback_handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		return 0;
	}

	if ((err = set_hwparams(playback_handle, hwparams, access)) < 0) {
		printf("Setting of hwparams failed: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}
	if ((err = set_swparams(playback_handle, swparams)) < 0) {
		printf("Setting of swparams failed: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}
	snd_pcm_dump(playback_handle, output);

	samples = malloc((period_size * channels * snd_pcm_format_width(format)) / 8);
	if (samples == NULL) {
		printf("No enough memory\n");
		exit(EXIT_FAILURE);
	}

	areas = calloc(channels, sizeof(snd_pcm_channel_area_t));
	if (areas == NULL) {
		printf("No enough memory\n");
		exit(EXIT_FAILURE);
	}
	for (chn = 0; chn < channels; chn++) {
		areas[chn].addr = samples;
		areas[chn].first = chn * snd_pcm_format_width(format);
		areas[chn].step = channels * snd_pcm_format_width(format);
	}

	alsa_audiodev_is_open = TRUE;

	//err = snd_pcm_writei(playback_handle, samples, (period_size * channels * snd_pcm_format_width(format)) / 8);

	return TRUE;
}

void	alsa_close_audio(void) {
	if (!alsa_audiodev_is_open) return;
	snd_pcm_close (playback_handle);
	free(areas);
	free(samples);
	alsa_audiodev_is_open = FALSE;
}

SOUND_PLAYBACK_FORMAT *alsa_GetSoundPlaybackFormat(void)
{
	SoundFormat.NumberOfChannels = channels;
	SoundFormat.BitsPerSample = 8;
	/*if (format != SND_PCM_FORMAT_S8 && format != SND_PCM_FORMAT_U8)
		SoundFormat.BitsPerSample = 16;*/
	SoundFormat.Frequency = rate;
	fprintf(stderr,"alsa_GetSoundPlaybackFormat channels:%i, BitsPerSample: %i, Frequency: %i\n", SoundFormat.NumberOfChannels, SoundFormat.BitsPerSample, SoundFormat.Frequency);
	return &SoundFormat;
}

BOOL	alsa_Throttle(void)
{
	if (!alsa_audiodev_is_open) return FALSE;
	return TRUE;
}

#endif	/* HAVE_ALSA */

