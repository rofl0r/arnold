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

#include "../cpc/host.h"
#include "alsasound.h"
#include "alsasound-common.h"

#ifndef HAVE_ALSA

BOOL	alsa_AudioPlaybackPossible(void) { return FALSE; }
BOOL	alsa_LockAudioBuffer(unsigned char **pBlock1, unsigned long
*pBlock1Size, unsigned char **pBlock2, unsigned long *pBlock2Size, int
AudioBufferSize) { return FALSE; }
void	alsa_UnLockAudioBuffer(void) {}

#else

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

//#define DSPDEVICE "plughw:0,0"

static signed short backbuffer[1760*2];
static int commitBufferSize;

BOOL	alsa_AudioPlaybackPossible(void)
{
	int err;
	err = alsa_open_audio(FALSE);	// no mmap
	fprintf(stderr,"alsa_AudioPlaybackPossible(void): %i\n", err);
	return err;
}

BOOL	alsa_LockAudioBuffer(unsigned char **pBlock1, unsigned long
*pBlock1Size, unsigned char **pBlock2, unsigned long *pBlock2Size, int
AudioBufferSize)
{
	//printf("alsa_LockAudioBuffer audiobuffersize: %i\n",
	//	AudioBufferSize);
	*pBlock1 = (unsigned char *) backbuffer;
	*pBlock1Size = AudioBufferSize;
	*pBlock2 = NULL;
	*pBlock2Size = 0;
	commitBufferSize = AudioBufferSize/channels;
	return TRUE;
}

void	alsa_UnLockAudioBuffer(void)
{
	signed short *ptr;
	int err, cptr;
	static int skipfirst = 1;

	//fprintf(stderr,"alsa_UnLockAudioBuffer_writei commitBufferSize: %i\n", commitBufferSize);
	// FIXME: Quickhack to get rid of click sound when playing
	// unitialized auio buffer on first entry.
	if (skipfirst) {
		skipfirst--;
		return;
	}
	if (!alsa_audiodev_is_open) {
		return;
	}
	ptr = backbuffer;
	cptr = commitBufferSize;
	if (format != SND_PCM_FORMAT_U8 && format != SND_PCM_FORMAT_S8) {
		convert8to16bit(ptr, cptr);
	}
	while (cptr > 0) {
		err = snd_pcm_writei(playback_handle, ptr, cptr);
		//fprintf(stderr,"err: %i\n", err);
		if (err == -EAGAIN)
			continue;
		if (err < 0) {
			if (xrun_recovery(playback_handle, err) < 0) {
				printf("Write error: %s\n", snd_strerror(err));
				//exit(EXIT_FAILURE);
				return;
			}
			break;  /* skip one period */
		}
		ptr += err * channels;
		cptr -= err;
	}
	//return TRUE;
}

#endif	/* HAVE_ALSA */
