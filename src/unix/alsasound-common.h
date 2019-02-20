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
#ifndef __ALSASOUND_COMMON_HEADER_INCLUDED__
#define __ALSASOUND_COMMON_HEADER_INCLUDED__
#ifdef HAVE_ALSA

#include "../cpc/host.h"
#include <alsa/asoundlib.h>

extern snd_pcm_t *playback_handle;
extern signed short *samples;
extern unsigned int chn;
extern snd_pcm_channel_area_t *areas;
//extern int commitBufferSize;

extern char *device;                                    /* playback device */
extern snd_pcm_format_t format_16;                      /* sample format 16 */
extern snd_pcm_format_t format_8;                       /* sample format 8 */
extern snd_pcm_format_t format;                         /* sample format */
extern unsigned int rate;                               /* stream rate */
extern unsigned int channels;                           /* count of channels */
extern snd_pcm_uframes_t arnold_frame_size;             /* period size in samples */
extern snd_pcm_uframes_t period_size;                   /* period size in samples */
extern unsigned int periods;                            /* number of periods */

extern snd_output_t *output;
extern snd_pcm_uframes_t offset;


int set_hwparams(snd_pcm_t *handle,
                        snd_pcm_hw_params_t *params,
                        snd_pcm_access_t access);

int set_swparams(snd_pcm_t *handle, snd_pcm_sw_params_t *swparams);

int xrun_recovery(snd_pcm_t *handle, int err);

#endif	/* HAVE_ALSA */

extern BOOL alsa_audiodev_is_open;

BOOL	alsa_open_audio(BOOL use_mmap);

void	alsa_close_audio(void);

BOOL	alsa_AudioPlaybackPossible(void);

SOUND_PLAYBACK_FORMAT *alsa_GetSoundPlaybackFormat(void);

BOOL	alsa_LockAudioBuffer(unsigned char **pBlock1, unsigned long
*pBlock1Size, unsigned char **pBlock2, unsigned long *pBlock2Size, int
AudioBufferSize);

void	alsa_UnLockAudioBuffer(void);

BOOL	alsa_Throttle(void);

#endif  /* __ALSASOUND_COMMON_HEADER_INCLUDED__ */

