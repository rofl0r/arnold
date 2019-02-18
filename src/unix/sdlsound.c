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
#ifdef HAVE_SDL

#include "sdlsound.h"
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
#include "../cpc/messages.h"

static SOUND_PLAYBACK_FORMAT SoundFormat;

const int audio_NumberOfChannels = 2;
//static const int audio_NumberOfChannels = 1;
//static const int audio_BitsPerSample = 16;
static const int audio_BitsPerSample = 8;
static const int audio_Frequency = 44100;
//const int audio_Frequency = 22050;
//const int audio_bufsize = 131072;
//const int audio_bufsize = 16384;
//static const int audio_bufsize = 16384;
static const int audio_bufsize = 8192;
//static const int audio_bufsize = 4096;
//const int audio_bufsize = 1024;
//static const int audio_callbacksize = 4096;
//static const int audio_callbacksize = 1024;
//static const int audio_callbacksize = 2048;
static const int audio_callbacksize = AUDIO_WATERMARK/2;
static SDL_AudioSpec audioSpec;
static BOOL audio_open = FALSE;
static Uint8 *audio_chunk;
//Uint32 chunk_len;
static Uint32 audio_len;
static Uint8 *audio_pos;
static Uint8 *audio_rec;
static void *(*samplecpy)(void *dest, const void *src, size_t n);

BOOL	sdl_open_audio(SDL_AudioSpec *audioSpec) {
	BOOL status;
	SDL_AudioSpec desired;
	memcpy(&desired, audioSpec, sizeof(SDL_AudioSpec));
	audio_open = FALSE;
	status = SDL_OpenAudio(&desired, audioSpec);
	if ( status < 0 ){
		fprintf(stderr, Messages[82], SDL_GetError());
		return FALSE;
	}
	fprintf(stderr, "Opened Audio device: %i/%0x/%i\n",
		audioSpec->freq, audioSpec->format, audioSpec->samples);
	if (audio_chunk != NULL) free(audio_chunk);
	audio_chunk = malloc(audio_bufsize);
	if (audio_chunk == NULL) {
		fprintf(stderr,Messages[83],
			audio_bufsize);
		exit(1);
	}
	memset(audio_chunk, 0, audio_bufsize);
	audio_pos = audio_chunk;
	audio_rec = audio_chunk;
	audio_len = audio_bufsize;
	audio_waterlevel = 0;
	SDL_PauseAudio(0);
	audio_open = TRUE;
	return TRUE;
}

void	sdl_close_audio(void) {
	SDL_CloseAudio();
	audio_open = FALSE;
}

void	*halfcpy(void *s1, const void *s2, size_t len) {
	Uint8 *dst = s1;
	const Uint8 *src = s2;
	while(len-- > 0) {
		*dst++ = *src++/2;
	}
	return dst;
}

void	fill_audio(void *userdata, Uint8 *stream, int len) {
	static int debugcounter = 0;
	//static SDL_AudioSpec wav_spec;
	//static Uint32 wav_length;
	//static Uint8 *wav_buffer = NULL;
	//static Uint8 *p = NULL;
	int i, j;
	int remain;
	//fprintf(stderr,"fill_audio()\n");
	//fprintf(stderr,".%x",len);
	//fprintf(stderr,".%x:%x:%x",(debugcounter+=len),len,
		//audio_pos-audio_chunk);
#if 0
	if ( wav_buffer == NULL ) {
		SDL_LoadWAV("test.wav", &wav_spec, &wav_buffer, &wav_length);
		p = wav_buffer;
	}
	memcpy(stream, p, len);
	//if (debugcounter < 0x5000) {
	if (debugcounter < wav_spec.samples*16*2*2) {
		fprintf(stderr,"_");
		p += len;
	}
#endif
	audio_waterlevel -= len;	// FIXME
	if ( audio_pos + len < audio_chunk + audio_bufsize ) {
		//memcpy(stream, audio_pos, len);
		//halfcpy(stream, audio_pos, len);
		samplecpy(stream, audio_pos, len);
		audio_pos += len;
		//fprintf(stderr,",");
	} else {
		remain = (audio_chunk + audio_bufsize) - audio_pos;
		//memcpy(stream, audio_pos, remain);
		//halfcpy(stream, audio_pos, remain);
		samplecpy(stream, audio_pos, remain);
		//memcpy(stream + remain, audio_chunk, len - remain);
		//halfcpy(stream + remain, audio_chunk, len - remain);
		samplecpy(stream + remain, audio_chunk, len - remain);
		audio_pos = audio_chunk + len - remain;
		//fprintf(stderr,"'");
	}

	//memset(stream, 0, len);

#if 0
	/* Only play if we have data left */
	if ( audio_len == 0 ) {
		//return;
		audio_pos = audio_chunk;
		audio_len = audio_bufsize;
		fprintf(stderr,",");
	}
	/* Mix as much data as possible */
	len = ( len > audio_len ? audio_len : len );
	//SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
	memcpy(stream, audio_pos, len);
	audio_pos += len;
	audio_len -= len;
#endif
}

BOOL	sdl_AudioPlaybackPossible(void)
{
	fprintf(stderr,"sdl_AudioPlaybackPossible(void)\n");
return FALSE;
	if (audio_open) return TRUE;
	audioSpec.freq = audio_Frequency;
	if (audio_BitsPerSample == 16) {
		audioSpec.format = AUDIO_S16;
		samplecpy = memcpy;
	} else {
		audioSpec.format = AUDIO_S8;
		samplecpy = halfcpy;
	}
	audioSpec.channels = audio_NumberOfChannels;
	//audioSpec.samples = audio_BitsPerSample;
	//audioSpec.samples = 3520;
	//audioSpec.samples = audio_bufsize/4;	//FIXME, only 16 bit Stereo
	//audioSpec.samples = audio_callbacksize/4;//FIXME, only 16 bit Stereo
	audioSpec.samples = audio_callbacksize;
	audioSpec.callback = fill_audio;
	audioSpec.userdata = NULL;
	return sdl_open_audio(&audioSpec);
}

SOUND_PLAYBACK_FORMAT *sdl_GetSoundPlaybackFormat(void)
{
	fprintf(stderr,"sdl_GetSoundPlaybackFormat(void)\n");
	if (!audio_open) sdl_AudioPlaybackPossible();
	SoundFormat.NumberOfChannels = audioSpec.channels;
	//SoundFormat.BitsPerSample = audioSpec.samples;
	if (audioSpec.format == AUDIO_S8) {
		SoundFormat.BitsPerSample = 8;
	} else {
		SoundFormat.BitsPerSample = 16;
	}
	SoundFormat.Frequency = audioSpec.freq;
	return &SoundFormat;
}

BOOL	sdl_LockAudioBuffer(unsigned char **pBlock1, unsigned long
*pBlock1Size, unsigned char **pBlock2, unsigned long *pBlock2Size, int
AudioBufferSize)
{	
	static int debugcounter = 0;
	int remain;
	int n;
	//fprintf(stderr,"sdl_LockAudioBuffer %i %i %i %i %i\n",
		//*pBlock1, *pBlock1Size, *pBlock2, *pBlock2Size, AudioBufferSize);
	SDL_LockAudio();
	//fprintf(stderr,"<");
	/*if (chunk_len != AudioBufferSize) {
		fprintf(stderr,"?");
		chunk_len = AudioBufferSize;
		//chunk_len = audio_bufsize;
		if (audio_chunk != NULL) free(audio_chunk);
		audio_chunk = malloc(chunk_len);
		if (audio_chunk == NULL) {
			fprintf(stderr,"Error: Unable to allocate %i bytes of memory\n",
				chunk_len);
			exit(1);
		}
		memset(audio_chunk, 0, chunk_len);
		*pBlock1 = audio_chunk;
		*pBlock1Size = chunk_len;
		*pBlock2 = NULL;
		*pBlock2Size = 0;
		audio_pos = audio_chunk;
		audio_len = chunk_len;
	} else {
	}*/
	//memset(audio_chunk, 0, chunk_len);
	/**pBlock1 = audio_chunk;
	*pBlock1Size = audio_bufsize;
	*pBlock2 = NULL;
	*pBlock2Size = 0;
	audio_pos = audio_chunk;
	audio_len = audio_bufsize;*/
	/*if((audio_rec - audio_chunk) + AudioBufferSize < audio_bufsize) {
		fprintf(stderr,"-");
		*pBlock1 = audio_rec;
		*pBlock1Size = AudioBufferSize;
		*pBlock2 = NULL;
		*pBlock2Size = 0;
		audio_rec += AudioBufferSize;
	} else {
		fprintf(stderr,"+");
		*pBlock1 = audio_rec;
		*pBlock1Size = audio_bufsize - (audio_rec - audio_chunk);
		*pBlock2 = audio_chunk;
		*pBlock2Size = audio_bufsize;	// FIXME
		audio_rec = audio_chunk;	// FIXME
	}*/
	remain = audio_bufsize - (audio_rec - audio_chunk);
	//fprintf(stderr,"%i %i",remain,audio_rec - audio_chunk);
	//fprintf(stderr,"%x %x", AudioBufferSize, audio_rec - audio_chunk);
	//fprintf(stderr,"%x", (debugcounter+=AudioBufferSize));
	//fprintf(stderr,"%x:%x:%x", (debugcounter+=AudioBufferSize),
		//AudioBufferSize,audio_rec - audio_chunk);
	if(remain > AudioBufferSize) {
		//fprintf(stderr,"-");
		//fprintf(stderr,"-(%0x %0x)", audio_rec-audio_chunk, audio_pos-audio_chunk);
		/*n = 10;
		while( audio_pos > audio_rec && n-->0 ) {
			fprintf(stderr,".(%0x %0x)", audio_rec-audio_chunk, audio_pos-audio_chunk);
			usleep(10);
		}*/
		*pBlock1 = audio_rec;
		*pBlock1Size = AudioBufferSize;
		*pBlock2 = NULL;
		*pBlock2Size = 0;
		audio_rec += AudioBufferSize;
	} else {
		//fprintf(stderr,"+");
		*pBlock1 = audio_rec;
		*pBlock1Size = remain;
		*pBlock2 = audio_chunk;
		*pBlock2Size = AudioBufferSize - remain;
		audio_rec = audio_chunk + *pBlock2Size;
		//sleep(10);
	}
	audio_waterlevel += AudioBufferSize;		// FIXME
	//fprintf(stderr," %i ", audio_waterlevel);	// FIXME
	//usleep(10);
	return TRUE;
}

void	sdl_UnLockAudioBuffer(void)
{
	//fprintf(stderr,">");
	SDL_UnlockAudio();
	//fprintf(stderr,"sdl_UnLockAudioBuffer\n");
}

#endif	/* HAVE_SDL */

