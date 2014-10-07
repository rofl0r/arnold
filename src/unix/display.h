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
#include "../cpc/host.h"
#include <SDL.h>
#include <unistd.h>
#include <sys/time.h>
#include "../cpc/cpc.h"
void sdl_SetDisplayWindowed(int Width, int Height, int Depth);
void sdl_SetDisplayFullscreen(int Width, int Height, int Depth);
void sdl_SetDoubled( BOOL doubled );
void sdl_GetGraphicsBufferColourFormat(GRAPHICS_BUFFER_COLOUR_FORMAT *pFormat);
int sdl_CheckDisplay(void);
void sdl_GetGraphicsBufferInfo(GRAPHICS_BUFFER_INFO *pBufferInfo);
BOOL sdl_LockGraphicsBuffer(void);
void sdl_UnlockGraphicsBuffer(void);
void sdl_SwapGraphicsBuffers(void);
BOOL sdl_ProcessSystemEvents(void);
void sdl_Throttle(void);
extern int sdl_LockSpeed;
#define QWERTY 0
#define QWERTZ 1
#define AZERTY 2
void sdl_InitialiseKeyboardMapping(int layout);
#else
#include "../cpc/host.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include "../cpc/cpc.h"
void XWindows_SetDisplayWindowed(int Width, int Height, int Depth);
void XWindows_GetGraphicsBufferColourFormat(GRAPHICS_BUFFER_COLOUR_FORMAT *pFormat);
int XWindows_CheckDisplay(void);
void XWindows_GetGraphicsBufferInfo(GRAPHICS_BUFFER_INFO *pBufferInfo);
void XWindows_SwapGraphicsBuffers(void);
#endif
