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
#ifndef __GRAPHICS_LIB__
#define __GRAPHICS_LIB__

#include <windows.h>
#include <ddraw.h>

//#ifndef TRUE
//#define TRUE (1==1)
//#endif

//#ifndef FALSE
//#define FALSE (1==0)
//#endif

//#ifndef _BOOL
//#define _BOOL
//typedef int BOOL;
//#endif

typedef struct 
{
	int	BPP;
	int RedBPP, RedMask, RedShift;
	int GreenBPP, GreenMask, GreenShift;
	int BlueBPP, BlueMask, BlueShift;
} MODE_DETAILS;

#define GRAPHICS_FULLSCREEN	0x0002
#define GRAPHICS_WINDOWED	0x0001

int		UserInitialise(void);
void	MainLoop(void);
void	UserExit(void);

BOOL	SetVideoMode(int,int,int,int);
void	ClearScreen(void);
void	Flip(void);
void	ShowString(int,int,char *);
BOOL	IsKeyPressed(int);
BOOL    DebounceIsKeyPressed(int);
BOOL	SetupDirect3D();
BOOL	DoRender3D(int RenderHeight,int RenderWidth);
BOOL	CreateTextureMap();
void	SetUserMessageHandler(BOOL (*)(HWND,UINT,WPARAM,LPARAM));

//void	PlotPixel16(DDSURFACEDESC *pSurface, int X,int Y, int Colour);

void	RegisterDialog(HWND handleDialog);

void	ExamineMode(MODE_DETAILS *pModeDetails);


#endif