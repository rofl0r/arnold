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
#ifndef __HOST_INTERFACE_HEADER_INCLUDED__
#define __HOST_INTERFACE_HEADER_INCLUDED__

#include "cpcglob.h"	

#define HOST_FILE_ACCESS_READ 0x0001
#define HOST_FILE_ACCESS_WRITE 0x0002

typedef unsigned long HOST_FILE_HANDLE;

typedef struct GRAPHICS_BUFFER_INFO
{
	unsigned char *pSurface;		/* pointer to top-left for rendering */
	int			Width;				/* width of graphics image */
	int			Height;				/* height of graphics image */
	int			Pitch;				/* number of bytes in one line of graphics buffer including padding. */
} GRAPHICS_BUFFER_INFO;

typedef struct GRAPHICS_BUFFER_COLOUR_ELEMENT
{
	int BPP;						/* bits per colour */
	int Mask;						/* mask */
	int Shift;						/* shift */
} GRAPHICS_BUFFER_COLOUR_ELEMENT;

typedef struct GRAPHICS_BUFFER_COLOUR_FORMAT
{
	int BPP;								/* bits per pixel (when R,G,B are combined) */
	GRAPHICS_BUFFER_COLOUR_ELEMENT Red;		/* information about red */
	GRAPHICS_BUFFER_COLOUR_ELEMENT Green;	/* information about green */
	GRAPHICS_BUFFER_COLOUR_ELEMENT Blue;	/* information about blue */
} GRAPHICS_BUFFER_COLOUR_FORMAT;

typedef struct SOUND_PLAYBACK_FORMAT
{
	int NumberOfChannels;	
	int BitsPerSample;
	int Frequency;
} SOUND_PLAYBACK_FORMAT;				

/* windowed display, bit depth is dependant on window manager settings */
#define DISPLAY_TYPE_WINDOWED	0x0001
/* full-screen display, bit depth can be set independently of window manager settings */
#define DISPLAY_TYPE_FULLSCREEN	0x0002

BOOL	Host_LockGraphicsBuffer(void);	
GRAPHICS_BUFFER_INFO	*Host_GetGraphicsBufferInfo(void);
void	Host_UnlockGraphicsBuffer(void);
void	Host_SwapGraphicsBuffers(void);
void	Host_SetPaletteEntry(int, unsigned char, unsigned char, unsigned char);
BOOL	Host_SetDisplay(int Type, int Width, int Height, int Depth);
GRAPHICS_BUFFER_COLOUR_FORMAT *Host_GetGraphicsBufferColourFormat(void);
void	Host_WriteDataToSoundBuffer(unsigned char *pData, unsigned long Length);
BOOL	Host_AudioPlaybackPossible(void);
SOUND_PLAYBACK_FORMAT *Host_GetSoundPlaybackFormat(void);
BOOL	Host_ProcessSystemEvents(void);
void	Host_DoDriveLEDIndicator(int Drive, BOOL State);
BOOL	Host_SaveFile(char *Filename, unsigned char *, unsigned long);
BOOL	Host_LoadFile(char *Filename, unsigned char **pLocation, unsigned long *pLength);

HOST_FILE_HANDLE Host_OpenFile(char *Filename, int Access);
void	Host_CloseFile(HOST_FILE_HANDLE);
int		Host_GetFileSize(HOST_FILE_HANDLE);
void	Host_WriteData(HOST_FILE_HANDLE, unsigned char *, unsigned long);
void	Host_ReadData(HOST_FILE_HANDLE, unsigned char *, unsigned long);
void	Host_SetDirectory(char *);
unsigned long	Host_GetCurrentTimeInMilliseconds(void);

BOOL Host_LockAudioBuffer(unsigned char **, unsigned long *, unsigned char **, unsigned long *, int);
void	Host_UnLockAudioBuffer(void);
void	Host_Throttle(void);

#endif
