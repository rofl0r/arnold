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
#define HAVE_OSS 1
#ifdef HAVE_OSS

#include "osssound.h"
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

//#define FORMAT AFMT_S16_NE
//#define FORMAT AFMT_U16_LE
#define FORMAT AFMT_U8
//#define FORMAT AFMT_S8

int fd_dsp;
static signed short backbuffer[1760*2];
static int commitBufferSize;
static BOOL oss_audiodev_is_open = FALSE;

static unsigned int rate = 44100;                       /* stream rate */
static unsigned int channels = 2;                       /* count of channels */

int frag = 0x00200002;	/* 2 fragments of 2^11=2048 bytes */

BOOL	oss_open_audio() {
	int tmp;

	if (oss_audiodev_is_open) return TRUE;

	if ((fd_dsp = open ("/dev/dsp", O_WRONLY, 0)) == -1) {
		perror ("/dev/dsp");
		exit (-1);
	}

	if (ioctl (fd_dsp, SNDCTL_DSP_SETFRAGMENT, &frag) == -1)
		perror ("SNDCTL_DSP_SETFRAGMENT");

	tmp = FORMAT;		/* Native 16 bits */
	if (ioctl (fd_dsp, SNDCTL_DSP_SETFMT, &tmp) == -1) {
		perror ("SNDCTL_DSP_SETFMT");
		exit (-1);
	}

	if (tmp != FORMAT) {
		fprintf (stderr,
				"The device doesn't support the 16 bit sample format.\n");
		exit (-1);
	}

	tmp = channels;
	if (ioctl (fd_dsp, SNDCTL_DSP_CHANNELS, &tmp) == -1) {
		perror ("SNDCTL_DSP_CHANNELS");
		exit (-1);
	}

	if (tmp != channels) {
		fprintf (stderr, "The device doesn't support stereo mode.\n");
		exit (-1);
	}

	if (ioctl (fd_dsp, SNDCTL_DSP_SPEED, &rate) == -1) {
		perror ("SNDCTL_DSP_SPEED");
		exit (-1);
	}

	oss_audiodev_is_open = TRUE;

	return TRUE;
}

void	oss_close_audio(void) {
	if (!oss_audiodev_is_open) return;
	close(fd_dsp);
	//free(areas);
	//free(samples);
	oss_audiodev_is_open = FALSE;
}

BOOL	oss_AudioPlaybackPossible(void)
{
	int err;
	err = oss_open_audio();
	fprintf(stderr,"oss_AudioPlaybackPossible(void): %i\n", err);
	return err;
}

SOUND_PLAYBACK_FORMAT *oss_GetSoundPlaybackFormat(void)
{
	SoundFormat.NumberOfChannels = channels;
	SoundFormat.BitsPerSample = 8;
	/*if (format != SND_PCM_FORMAT_S8 && format != SND_PCM_FORMAT_U8)
		SoundFormat.BitsPerSample = 16;*/
	SoundFormat.Frequency = rate;
	fprintf(stderr,"oss_GetSoundPlaybackFormat channels:%i, BitsPerSample: %i, Frequency: %i\n", SoundFormat.NumberOfChannels, SoundFormat.BitsPerSample, SoundFormat.Frequency);
	return &SoundFormat;
}

BOOL	oss_LockAudioBuffer(unsigned char **pBlock1, unsigned long
*pBlock1Size, unsigned char **pBlock2, unsigned long *pBlock2Size, int
AudioBufferSize)
{
	//printf("oss_LockAudioBuffer audiobuffersize: %i\n", AudioBufferSize);
	*pBlock1 = (unsigned char *) backbuffer;
	*pBlock1Size = AudioBufferSize;
	*pBlock2 = NULL;
	*pBlock2Size = 0;
	// in ALSA we measure this in samples, so we devide through channels
	// in OSS we measure in bytes, so we do not device
	//commitBufferSize = AudioBufferSize/channels;
	commitBufferSize = AudioBufferSize;
	return TRUE;
}

void	oss_UnLockAudioBuffer(void)
{
	signed short *ptr;
	int err, cptr;
	static int skipfirst = 1;

	//fprintf(stderr,"oss_UnLockAudioBuffer commitBufferSize: %i\n", commitBufferSize);
	// FIXME: Quickhack to get rid of click sound when playing
	// unitialized auio buffer on first entry.
	if (skipfirst) {
		skipfirst--;
		return;
	}
	if (!oss_audiodev_is_open) {
		return;
	}
	ptr = backbuffer;
	cptr = commitBufferSize;
	//if (format != SND_PCM_FORMAT_U8 && format != SND_PCM_FORMAT_S8) {
	//	convert8to16bit(ptr, cptr);
	//}
	while (cptr > 0) {
		err = write(fd_dsp, ptr, cptr);
		//fprintf(stderr, " %i<%i>", err, cptr);
		if (err < 0) {
			perror("Audio write");
			exit(1);
		}
		ptr += err * channels;
		cptr -= err;
	}
}

BOOL	oss_Throttle(void)
{
	if (!oss_audiodev_is_open) return FALSE;
	return TRUE;
}

#endif	/* HAVE_OSS */

