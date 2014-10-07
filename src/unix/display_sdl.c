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
/* display_sdl */
#ifdef HAVE_SDL
#include "display.h"
#include "sdlsound.h"
#include <string.h>
#include "../cpc/messages.h"

SDL_Surface *screen;
BOOL fullscreen = FALSE;	//FIXME
int scale = 1;
//int mode = SDL_SWSURFACE;
int mode = SDL_HWSURFACE|SDL_DOUBLEBUF;

static INLINE void debug(char *s) {
	fprintf(stderr,"%s\n",s);
}

void sdl_InitialiseKeyboardMapping(void);
void sdl_InitialiseJoysticks(void);

void sdl_SetDisplay(int Width, int Height, int Depth, BOOL fullscreen) {

	fprintf(stderr, Messages[106],
		Width, Height, Depth);
	if ( fullscreen ) mode |= SDL_FULLSCREEN;
	else mode &= ~SDL_FULLSCREEN;
	screen = SDL_SetVideoMode(Width, Height, Depth, mode);
	if ( screen == NULL ) {
		fprintf(stderr, Messages[107],
			Width, Height, Depth, SDL_GetError());
		exit(1);
	}

}

void sdl_SetDisplayWindowed(int Width, int Height, int Depth) {

	sdl_SetDisplay(Width, Height, Depth, FALSE);
	SDL_ShowCursor(SDL_ENABLE);

}

void sdl_SetDisplayFullscreen(int Width, int Height, int Depth) {

	sdl_SetDisplay(Width, Height, Depth, TRUE);
	SDL_ShowCursor(SDL_DISABLE);

}

void sdl_SetDoubled( BOOL doubled ) {

	int w, h;

	scale = doubled ? 2 : 1;
	if ( doubled ) {
		w = screen->w * 2;
		h = screen->h * 2;
	} else {
		w = screen->w / 2;
		h = screen->h / 2;
	}
	sdl_SetDisplay( w, h, screen->format->BitsPerPixel, fullscreen);

}

static INLINE unsigned long CalcBPPFromMask(unsigned long Mask)
{
	unsigned long LocalShift = 0;
	unsigned long LocalBPP = 0;
	unsigned long LocalMask = Mask;

	if (LocalMask!=0)
	{
		do
		{
			if ((LocalMask & 0x01)!=0)
				break;

			LocalMask = LocalMask >>1;
			LocalShift++;
		}
		while (1==1);

		do
		{
			if ((LocalMask & 0x01)!=1)
				break;

			LocalMask = LocalMask>>1;
			LocalBPP++;
		}
		while (1==1);
	}

	return LocalBPP;
}

void sdl_GetGraphicsBufferColourFormat(GRAPHICS_BUFFER_COLOUR_FORMAT *pFormat) {
	SDL_PixelFormat *format = screen->format;

	pFormat->Red.Mask = format->Rmask;
	pFormat->Red.BPP = CalcBPPFromMask(format->Rmask);
	pFormat->Red.Shift = format->Rshift;

	pFormat->Green.Mask = format->Gmask;
	pFormat->Green.BPP = CalcBPPFromMask(format->Gmask);
	pFormat->Green.Shift = format->Gshift;

	pFormat->Blue.Mask = format->Bmask;
	pFormat->Blue.BPP = CalcBPPFromMask(format->Bmask);
	pFormat->Blue.Shift = format->Bshift;

	pFormat->BPP = format->BitsPerPixel;

	printf("BPP: %d\r\n",pFormat->BPP);
	printf("Red: M: %08x B: %d S: %d\r\n",pFormat->Red.Mask, pFormat->Red.BPP, pFormat->Red.Shift);
	printf("Green: M: %08x B: %d S: %d\r\n", pFormat->Green.Mask, pFormat->Green.BPP, pFormat->Green.Shift);
	printf("Blue: M: %08x B: %d S: %d\r\n",pFormat->Blue.Mask, pFormat->Blue.BPP, pFormat->Blue.Shift);
}

int sdl_CheckDisplay(void) {
	return 0;
}

void sdl_GetGraphicsBufferInfo(GRAPHICS_BUFFER_INFO *pBufferInfo) {
	pBufferInfo->pSurface = screen->pixels;
	pBufferInfo->Width = screen->w;
	pBufferInfo->Height = screen->h;
	pBufferInfo->Pitch = screen->pitch;

	//printf("get buffer info\r\n");
	//printf("W: %d H: %d P: %d\r\n",pBufferInfo->Width, pBufferInfo->Height,
		//pBufferInfo->Pitch);
}

BOOL sdl_LockGraphicsBuffer(void) {
	if (SDL_LockSurface(screen) == 0) return TRUE;
	else return FALSE;
}

void sdl_UnlockGraphicsBuffer(void) {
	SDL_UnlockSurface(screen);
}

void sdl_DoubleGraphicsBuffer32(void) {
	int x, y;
	Uint16 pitch = screen->pitch;
	Uint32 *pixels32 = (Uint32 *) screen->pixels;
	int w = screen->w;
	int h = screen->h;
	int bpp = screen->format->BytesPerPixel;
	int ll = pitch/bpp;
	Uint32 *src, *dst, *dst2, pel;
	int i;

	SDL_LockSurface(screen);
	// works, but VERY slow
	/*for ( y = h-1; y >= 0; y-- ) {
		for ( x = w-1; x >= 0; x-- ) {
			*(pixels32+y*ll+x) = *(pixels32+y/2*ll+x/2);
		}
	}*/
	// Faster? Probably, but only a bit.
	for ( y = h-1; y >= 0; y-=2 ) {
		dst = pixels32+y*ll+w-1;
		dst2 = pixels32+(y-1)*ll+w-1;
		src = pixels32+y/2*ll+w/2-1;
		for ( x = w/2-1; x >= 0; x-- ) {
			pel = *src--;
			*dst-- = pel;
			*dst-- = pel;
			*dst2-- = pel;
			*dst2-- = pel;
		}
	}
	SDL_UnlockSurface(screen);
}

void sdl_DoubleGraphicsBuffer16(void) {
	int x, y;
	Uint16 pitch = screen->pitch;
	Uint16 *pixels16 = (Uint16 *) screen->pixels;
	int w = screen->w;
	int h = screen->h;
	int bpp = screen->format->BytesPerPixel;
	int ll = pitch/bpp;
	Uint16 *src, *dst, *dst2, pel;
	int i;

	SDL_LockSurface(screen);
	for ( y = h-1; y >= 0; y-=2 ) {
		dst = pixels16+y*ll+w-1;
		dst2 = pixels16+(y-1)*ll+w-1;
		src = pixels16+y/2*ll+w/2-1;
		for ( x = w/2-1; x >= 0; x-- ) {
			pel = *src--;
			*dst-- = pel;
			*dst-- = pel;
			*dst2-- = pel;
			*dst2-- = pel;
		}
	}
	SDL_UnlockSurface(screen);
}

void sdl_DoubleGraphicsBuffer8(void) {
	int x, y;
	Uint16 pitch = screen->pitch;
	Uint8 *pixels8 = (Uint8 *) screen->pixels;
	int w = screen->w;
	int h = screen->h;
	int bpp = screen->format->BytesPerPixel;
	int ll = pitch/bpp;
	Uint8 *src, *dst, *dst2, pel;
	int i;

	SDL_LockSurface(screen);
	for ( y = h-1; y >= 0; y-=2 ) {
		dst = pixels8+y*ll+w-1;
		dst2 = pixels8+(y-1)*ll+w-1;
		src = pixels8+y/2*ll+w/2-1;
		for ( x = w/2-1; x >= 0; x-- ) {
			pel = *src--;
			*dst-- = pel;
			*dst-- = pel;
			*dst2-- = pel;
			*dst2-- = pel;
		}
	}
	SDL_UnlockSurface(screen);
}

void sdl_DoubleGraphicsBuffer(void) {

	int bpp = screen->format->BytesPerPixel;
	switch( bpp ) {
		case 1:
			sdl_DoubleGraphicsBuffer8();
			break;
		case 2:
			sdl_DoubleGraphicsBuffer16();
			break;
		case 4:
			sdl_DoubleGraphicsBuffer32();
			break;
		default:	/* unsupported */
			break;
	}
}

void sdl_SwapGraphicsBuffers(void) {
	if ( scale == 2 ) sdl_DoubleGraphicsBuffer();
	//SDL_UpdateRects(screen,1,&screen->clip_rect);
	SDL_Flip( screen );
}

/* Some comments about throttling on Linux/Unix:
 * Originally I used the nice and portable SDL_GetTicks() and SDL_Delay()
 * functions. Unfortunately the resolution is bad and the result is very
 * dodgy. Therefore I used the gettimeofday() and usleep() functions. This is
 * POSIX, but not SDL and therefore not directly portable to non-POSIX SDL
 * Targets.
 * I left in the SDL in case someone wants to try.
 * FIXME: Maybe we can get rid of floating point here?
 */
double delta_time()
{
	static struct timeval t1, t2;
	double dt;
	gettimeofday(&t1,NULL);
	dt=(t1.tv_sec - t2.tv_sec)+(t1.tv_usec - t2.tv_usec)/1000000.0; /* 1000000 microseconds in a second... */
	memcpy( &t2, &t1, sizeof(t2) );
	return dt;
}

#define FRAMES_PER_SEC 50
int sdl_LockSpeed = TRUE;

void sdl_Throttle(void) {
	if (sdl_LockSpeed)
	{
#if 0
		static Uint32 next_tick = 0;
		Uint32 this_tick;

		/* Wait for the next frame */
		this_tick = SDL_GetTicks(); 
		if ( this_tick < next_tick ) {
			SDL_Delay(next_tick-this_tick);
		}
		next_tick = this_tick + (1000/FRAMES_PER_SEC);
#endif
		long delay;
		delay = 10000/FRAMES_PER_SEC - delta_time();
		if (delay>0 && audio_waterlevel > AUDIO_WATERMARK )
			usleep(delay);	// FIXME
	}

	CPC_UpdateAudio();
}

#include "keyboard_sdl.c"
#endif		/* HAVE_SDL */

