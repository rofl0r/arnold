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
#ifndef __DD_HEADER_INCLUDED__
#define __DD_HEADER_INCLUDED__

#include <windows.h>
#include <ddraw.h>

BOOL	DD_Init(void);
void	DD_Close(void);

BOOL	DD_SetupDD(HWND,void *);
void	DD_CloseDD(void);

void	DD_Flip(void);
void	DD_ClearScreen(void);

void	DD_ResizeWindow(HWND);

BOOL	DD_GetSurfacePtr(DDSURFACEDESC *);
void	DD_ReturnSurfacePtr(DDSURFACEDESC *);

BOOL	DD_SetVideoMode(int Type,int Width,int Height,int Depth);

BOOL		DirectX_CheckComponentsArePresent(void);

void	DD_FlipWindowed(void);

void	DD_SetPaletteEntry(int index, unsigned char r, unsigned char g, unsigned char b);
void	DD_BuildModeMenu(void (*pCallback)(int, int,int));
void	DD_RestoreSurfaces(void);
void	DD_ClearBothSurfaces(void);
void	DD_ShutDownWindowed(void);
void	DD_ReleaseSurfacesAndRestoreVideoMode(void);

#endif
