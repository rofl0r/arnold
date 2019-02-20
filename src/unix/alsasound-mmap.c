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

#include "alsasound-mmap.h"
#include "alsasound-common.h"
#include "display.h"
#include "../cpc/host.h"

void	alsa_mmap_close_audio(void)
{
	alsa_close_audio();
}

#ifndef HAVE_ALSA

BOOL    alsa_mmap_AudioPlaybackPossible(void) { return FALSE; }
SOUND_PLAYBACK_FORMAT *alsa_mmap_GetSoundPlaybackFormat(void) { return NULL; }
BOOL	alsa_mmap_LockAudioBuffer(unsigned char **pBlock1, unsigned long
*pBlock1Size, unsigned char **pBlock2, unsigned long *pBlock2Size, int
AudioBufferSize) { return FALSE; }
void	alsa_mmap_UnLockAudioBuffer(void) {}

#else

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

static int commitBufferSize;
static int skipfirst = 1;
static signed short dummybuffer[1760*2];

BOOL	alsa_mmap_AudioPlaybackPossible(void)
{
	int err;
	err = alsa_open_audio(TRUE);	// use mmap
	fprintf(stderr,"alsa_mmap_AudioPlaybackPossible(void): %i\n", err);
	return err;
}

SOUND_PLAYBACK_FORMAT *alsa_mmap_GetSoundPlaybackFormat(void)
{
	fprintf(stderr,"alsa_GetSoundPlaybackFormat(void)\n");
	SoundFormat.NumberOfChannels = channels;
	SoundFormat.BitsPerSample = 8;
	if (format != SND_PCM_FORMAT_S8 && format != SND_PCM_FORMAT_U8)
		SoundFormat.BitsPerSample = 16;
	SoundFormat.Frequency = rate;
	return &SoundFormat;

}

#if 0
BOOL	alsa_mmap_LockAudioBuffer(unsigned char **pBlock1, unsigned long
*pBlock1Size, unsigned char **pBlock2, unsigned long *pBlock2Size, int
AudioBufferSize)
{	
	int err;
	const snd_pcm_channel_area_t *my_areas;
	snd_pcm_uframes_t offset, frames;

	fprintf(stderr,"alsa_mmap_LockAudioBuffer: %x, %x\n",
		(unsigned int) *pBlock1Size, (unsigned int) *pBlock2Size);
	/*if (*pBlock1Size >= 10) fprintf(stderr,"%x %x %x %x %x %x %x %x %x %x\n", *pBlock1[0], *pBlock1[1], *pBlock1[2], *pBlock1[3], *pBlock1[4], *pBlock1[5], *pBlock1[6], *pBlock1[7], *pBlock1[8], *pBlock1[9]);
	if ((err = snd_pcm_writei (playback_handle, *pBlock1, *pBlock1Size)) != *pBlock1Size) {
		fprintf (stderr, "write to audio interface failed (%s)\n",
			snd_strerror (err));
			//exit (1);
	}*/
	alsa_mmap_pSize = pBlock1Size;
	snd_pcm_avail_update(playback_handle);	// FIXME: process return value
	//err = snd_pcm_mmap_begin(playback_handle, &my_areas, &offset, pBlock1Size);
	err = snd_pcm_mmap_begin(playback_handle, &my_areas, &offset, &frames);
	if (err < 0) {
		//if ((err = xrun_recovery(playback_handle, err)) < 0) {
			printf("MMAP begin avail error: %s\n", snd_strerror(err));
			exit(1);
		//}
	}
	*pBlock1 = my_areas->addr;
	*pBlock2 = NULL;
	*pBlock2Size = 0;
	return TRUE;
}
#endif

BOOL	alsa_mmap_LockAudioBuffer(unsigned char **pBlock1, unsigned long
*pBlock1Size, unsigned char **pBlock2, unsigned long *pBlock2Size, int
AudioBufferSize)
{	
	const snd_pcm_channel_area_t *my_areas;
	snd_pcm_uframes_t frames;
	unsigned char *samples[channels];
	int steps[channels];
	snd_pcm_sframes_t avail;
	snd_pcm_state_t state;
	int err, first = 1;

	//fprintf(stderr,"alsa_mmap_LockAudioBuffer: %i\n", AudioBufferSize);
	if (!alsa_audiodev_is_open) {
		return FALSE;
	}

restart:
	state = snd_pcm_state(playback_handle);
	if (state == SND_PCM_STATE_XRUN) {
		err = xrun_recovery(playback_handle, -EPIPE);
		if (err < 0) {
			printf("XRUN recovery failed: %s\n", snd_strerror(err));
			return err;
		}
		first = 1;
	} else if (state == SND_PCM_STATE_SUSPENDED) {
		err = xrun_recovery(playback_handle, -ESTRPIPE);
		if (err < 0) {
			printf("SUSPEND recovery failed: %s\n", snd_strerror(err));
			return err;
		}
	}
	avail = snd_pcm_avail_update(playback_handle);
	//printf("avail: %i period_size: %i\n", (int) avail, (int) period_size);
	if (avail < 0) {
		err = xrun_recovery(playback_handle, avail);
		if (err < 0) {
			printf("avail update failed: %s\n", snd_strerror(err));
			return err;
		}
		first = 1;
		goto restart;
	}
	if (avail < period_size) {
	//if (avail < AudioBufferSize) {
		if (first) {
			first = 0;
			err = snd_pcm_start(playback_handle);
			if (err < 0) {
				//FIXME: Why do we get tons of these errors?
				//printf("Start error: %s\n", snd_strerror(err));
				//exit(EXIT_FAILURE);
			}
		} else {
			err = snd_pcm_wait(playback_handle, -1);
			if (err < 0) {
				if ((err = xrun_recovery(playback_handle, err)) < 0) {
					printf("snd_pcm_wait error: %s\n", snd_strerror(err));
					exit(EXIT_FAILURE);
				}
				first = 1;
			}
		}
		goto restart;
	}
	/*while (avail < AudioBufferSize) {
		printf("%i ", avail);
		usleep(100);
		avail = snd_pcm_avail_update(playback_handle);
	}*/
	//printf("%i\n", avail);
	//size = period_size;
	frames = AudioBufferSize/channels;

	// FIXME: Quickhack to get rid of click sound when playing
	// unitialized auio buffer on first entry.
	if (skipfirst) {
		*pBlock1 = dummybuffer;
		*pBlock1Size = AudioBufferSize;
		*pBlock2 = NULL;
		*pBlock2Size = 0;
		commitBufferSize = AudioBufferSize/channels;
		return TRUE;
	} else {
		err = snd_pcm_mmap_begin(playback_handle, &my_areas, &offset, &frames);
	}
	//printf("frames: %i\n", frames);
	if (err < 0) {
		if ((err = xrun_recovery(playback_handle, err)) < 0) {
			printf("MMAP begin avail error: %s\n", snd_strerror(err));
			exit(1);
		}
	}
	for (chn = 0; chn < channels; chn++) {
		if ((my_areas[chn].first % 8) != 0) {
			printf("areas[%i].first == %i, aborting...\n", chn, my_areas[chn].first);
			exit(EXIT_FAILURE);
		}
		samples[chn] = /*(signed short *)*/(((unsigned char *)my_areas[chn].addr) + (my_areas[chn].first / 8));
		if ((my_areas[chn].step % 16) != 0) {
			printf("my_areas[%i].step == %i, aborting...\n", chn, my_areas[chn].step);
			exit(EXIT_FAILURE);
		}
		steps[chn] = my_areas[chn].step / 8;
		samples[chn] += offset * steps[chn];
	}
	//*pBlock1 = my_areas->addr;
	*pBlock1 = samples[0];
	*pBlock1Size = AudioBufferSize;
	*pBlock2 = NULL;
	*pBlock2Size = 0;
	commitBufferSize = AudioBufferSize/channels;
	return TRUE;
}

void	alsa_mmap_UnLockAudioBuffer(void)
{
	//fprintf(stderr,"alsa_mmap_UnLockAudioBuffer\n");
	if (!alsa_audiodev_is_open) {
		return;
	}
	// FIXME: Quickhack to get rid of click sound when playing
	// unitialized auio buffer on first entry.
	if (skipfirst) {
		skipfirst--;
		return;
	}
	snd_pcm_mmap_commit(playback_handle, offset, commitBufferSize);
}

#endif	/* HAVE_ALSA */

