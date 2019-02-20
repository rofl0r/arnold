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

#include "sdlsound.h"
#include "osssound.h"
#include "alsasound.h"
#include "alsasound-mmap.h"
#include "alsasound-common.h"
#include "global.h"
#include "sound.h"

#ifdef HAVE_SDL
//#define USE_SDL_SOUND
//#define USE_ALSA_SOUND
//#define USE_OSS_SOUND
#endif

#ifdef HAVE_ALSA
# define SOUND_PLUGIN_DEFAULT SOUND_PLUGIN_ALSA
#else
# define SOUND_PLUGIN_DEFAULT SOUND_PLUGIN_SDL
#endif

int sound_plugin = SOUND_PLUGIN_DEFAULT;

static GRAPHICS_BUFFER_INFO BufferInfo;
static GRAPHICS_BUFFER_COLOUR_FORMAT BufferColourFormat;

void Host_HandlePrinterOutput(void)
{
}


BOOL	Host_SetDisplay(int Type, int Width, int Height, int Depth)
{
#ifdef HAVE_SDL
	if (Type == DISPLAY_TYPE_WINDOWED)
	{
		sdl_SetDisplayWindowed(Width, Height, Depth);
	}
	else
	{
		sdl_SetDisplayFullscreen(Width, Height, Depth);
	}
#else
	if (Type == DISPLAY_TYPE_WINDOWED)
	{
		XWindows_SetDisplayWindowed(Width, Height, Depth);
	}
	else
	{
		XWindows_SetDisplayWindowed(Width, Height, Depth);
	}
#endif

	return TRUE;
}


BOOL	Host_LockGraphicsBuffer(void);	
GRAPHICS_BUFFER_INFO	*Host_GetGraphicsBufferInfo(void);
void	Host_UnlockGraphicsBuffer(void);
void	Host_SetPaletteEntry(int, unsigned char, unsigned char, unsigned char);
BOOL	Host_SetDisplay(int Type, int Width, int Height, int Depth);


GRAPHICS_BUFFER_COLOUR_FORMAT *Host_GetGraphicsBufferColourFormat()
{
#ifdef HAVE_SDL
	sdl_GetGraphicsBufferColourFormat(&BufferColourFormat);
#else
	XWindows_GetGraphicsBufferColourFormat(&BufferColourFormat);
#endif

	return &BufferColourFormat;
}

GRAPHICS_BUFFER_INFO *Host_GetGraphicsBufferInfo()
{
#ifdef HAVE_SDL
	sdl_GetGraphicsBufferInfo(&BufferInfo);
#else
	XWindows_GetGraphicsBufferInfo(&BufferInfo);
#endif

	return &BufferInfo;
}

BOOL	Host_LockGraphicsBuffer(void)
{
	//printf("buffer lock\r\n");
#ifdef HAVE_SDL
	return sdl_LockGraphicsBuffer();
#else
	return TRUE;
#endif
}


void	Host_UnlockGraphicsBuffer(void)
{
	//printf("buffer unlock\r\n");
#ifdef HAVE_SDL
	sdl_UnlockGraphicsBuffer();
#endif
}

void	Host_SwapGraphicsBuffers(void)
{
#ifdef HAVE_SDL
	sdl_SwapGraphicsBuffers();
#else
	XWindows_SwapGraphicsBuffers();
#endif
}

void	Host_SetPaletteEntry(int Index, unsigned char R, unsigned char G, unsigned char B)
{
}

void	Host_WriteDataToSoundBuffer(unsigned char *pData, unsigned long Length)
{
	fprintf(stderr,".\n");
}		

/*
BOOL	Host_open_audio(SDL_AudioSpec *audioSpec) {
#ifdef USE_SDL_SOUND
	return sdl_open_audio(audioSpec);
#else
	return FALSE;
#endif
}
*/

void	Host_close_audio(void) {
	switch(sound_plugin) {
		case SOUND_PLUGIN_OSS:
			oss_close_audio();
			break;
		case SOUND_PLUGIN_ALSA:
			alsa_close_audio();
			break;
		case SOUND_PLUGIN_ALSA_MMAP:
			alsa_mmap_close_audio();
			break;
		case SOUND_PLUGIN_SDL:
			sdl_close_audio();
			break;
	}
}

BOOL	Host_AudioPlaybackPossible(void)
{
	switch(sound_plugin) {
		case SOUND_PLUGIN_OSS:
			return oss_AudioPlaybackPossible();
			break;
		case SOUND_PLUGIN_ALSA:
			return alsa_AudioPlaybackPossible();
			alsa_close_audio();
			break;
		case SOUND_PLUGIN_ALSA_MMAP:
			return alsa_mmap_AudioPlaybackPossible();
			alsa_mmap_close_audio();
			break;
		case SOUND_PLUGIN_SDL:
			return sdl_AudioPlaybackPossible();
			sdl_close_audio();
			break;
		default:
			return FALSE;
			break;
	}
}

SOUND_PLAYBACK_FORMAT *Host_GetSoundPlaybackFormat(void)
{
	switch(sound_plugin) {
		case SOUND_PLUGIN_OSS:
			return oss_GetSoundPlaybackFormat();
			break;
		case SOUND_PLUGIN_ALSA:
			return alsa_GetSoundPlaybackFormat();
			alsa_close_audio();
			break;
		case SOUND_PLUGIN_ALSA_MMAP:
			return alsa_mmap_GetSoundPlaybackFormat();
			alsa_mmap_close_audio();
			break;
		case SOUND_PLUGIN_SDL:
			return sdl_GetSoundPlaybackFormat();
			sdl_close_audio();
			break;
		default:
			return NULL;
			break;
	}
}

BOOL XWindows_ProcessSystemEvents();


BOOL	Host_ProcessSystemEvents(void)
{	
	/* Always break out of main loop when using GTK+, because GTK+ has it's
	 * own event loop. */
#ifdef HAVE_GTK
#ifdef HAVE_SDL
	sdl_ProcessSystemEvents();		/* SDL    /    GTK+ */
#else
	XWindows_ProcessSystemEvents();		/* no SDL /    GTK+ */
#endif
	return TRUE;	/* always break if we use GTK+ */
#elif HAVE_SDL 
	return sdl_ProcessSystemEvents();	/* SDL    / no GTK+ */
#else
	return XWindows_ProcessSystemEvents();  /* no SDL / no GTK+ */
#endif

}

/* copied from /usr/include/linux/kd.h */
#define KDGETLED	0x4B31	/* return current led state */
#define KDSETLED	0x4B32	/* set led state [lights, not flags] */
#define LED_SCR		0x01	/* scroll lock led */
#define LED_CAP		0x04	/* caps lock led */
#define LED_NUM		0x02	/* num lock led */

char	*fn_console = "/dev/console";
int		fd_console;		/* File descriptor for console tty */
long	led_save;

void	Host_InitDriveLEDIndicator()
{
		fd_console = open(fn_console, O_RDONLY);
		if(fd_console < 0) {
			fprintf(stderr, Messages[84], fn_console);
			perror(NULL);
			fprintf(stderr, "%s", Messages[85]);
		}
		ioctl(fd_console,KDGETLED,&led_save);
}

void	Host_FreeDriveLEDIndicator()
{
		if (fd_console < 0) return;

		ioctl(fd_console,KDSETLED,led_save);
		close(fd_console);
}

void	Host_DoDriveLEDIndicator(int Drive, BOOL State)
{
		long led;
		long indicator;

		if (fd_console < 0) return;

		led = ioctl(fd_console,KDGETLED,&led);
		indicator = (Drive == 0) ? LED_SCR : LED_CAP;
		if(State) led = led | indicator;
		else led = led & ~indicator;
		ioctl(fd_console,KDSETLED,led);
}


void	Host_SetDirectory(char *Directory)
{
	/* fprintf(stderr,"Host_SetDirectory(%s)\n",Directory); */
	strncpy(currentDir, Directory, MAXCURDIR);
	chdir(Directory);
}

void	Host_Throttle(void)
{
#ifdef HAVE_SDL
	sdl_Throttle();
#endif
}

BOOL	Host_LockAudioBuffer(unsigned char **pBlock1, unsigned long
*pBlock1Size, unsigned char **pBlock2, unsigned long *pBlock2Size, int
AudioBufferSize)
{	
	switch(sound_plugin) {
		case SOUND_PLUGIN_OSS:
			return oss_LockAudioBuffer(pBlock1, pBlock1Size,
				pBlock2, pBlock2Size, AudioBufferSize);
			break;
		case SOUND_PLUGIN_ALSA:
			return alsa_LockAudioBuffer(pBlock1, pBlock1Size,
				pBlock2, pBlock2Size, AudioBufferSize);
			alsa_close_audio();
			break;
		case SOUND_PLUGIN_ALSA_MMAP:
			return alsa_mmap_LockAudioBuffer(pBlock1, pBlock1Size,
				pBlock2, pBlock2Size, AudioBufferSize);
			alsa_mmap_close_audio();
			break;
		case SOUND_PLUGIN_SDL:
			return sdl_LockAudioBuffer(pBlock1, pBlock1Size,
				pBlock2, pBlock2Size, AudioBufferSize);
			sdl_close_audio();
			break;
		default:
			return FALSE;
			break;
	}
}

void	Host_UnLockAudioBuffer(void)
{
	switch(sound_plugin) {
		case SOUND_PLUGIN_OSS:
			oss_UnLockAudioBuffer();
			break;
		case SOUND_PLUGIN_ALSA:
			alsa_UnLockAudioBuffer();
			break;
		case SOUND_PLUGIN_ALSA_MMAP:
			alsa_mmap_UnLockAudioBuffer();
			break;
		case SOUND_PLUGIN_SDL:
			sdl_UnLockAudioBuffer();
			break;
		default:
			break;
	}
}

void	quit(void)
{
#ifdef HAVE_GTK
	gtk_main_quit();
#endif
}

