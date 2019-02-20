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
#ifndef __MULTIFACE_HEADER_INCLUDED__
#define __MULTIFACE_HEADER_INCLUDED__

#include "cpcglob.h"	

typedef enum
{
	MULTIFACE_ROM_CPC_VERSION = 0,
	MULTIFACE_ROM_CPCPLUS_VERSION
} MULTIFACE_ROM_TYPE;

typedef enum
{
	MULTIFACE_CPC_MODE = 0,
	MULTIFACE_CPCPLUS_MODE
} MULTIFACE_MODE;

/* indicates the multiface ram and rom are paged into the address space */
#define MULTIFACE_FLAGS_RAM_ENABLED			0x0001
/* indicates a rom has been loaded for CPC mode */
#define	MULTIFACE_FLAGS_CPC_ROM_LOADED		0x0002
/* indicates a rom has been loaded for PLUS mode */
#define MULTIFACE_FLAGS_CPCPLUS_ROM_LOADED	0x0004
/* current state of multiface */
#define MULTIFACE_IS_DETECTABLE				0x0008
/* user setting: multiface is detectable after a reset */
#define MULTIFACE_IS_DETECTABLE_AFTER_RESET	0x0010
/* Multiface is enabled - rom loaded and no enable override */
#define MULTIFACE_IS_ENABLED				0x0020
/* user setting: Override enable is that the hardware can be disabled/enabled */
#define MULTIFACE_ENABLE_OVERRIDE			0x0040
/* stop button has been pressed */
#define MULTIFACE_STOP_BUTTON_PRESSED		0x0080
/* called when Multiface stop button is pressed */
void	Multiface_Stop(void);

/* called to initialise emulation */
void	Multiface_Initialise(void);

/* called to load a rom */
BOOL	Multiface_LoadRom(MULTIFACE_ROM_TYPE, unsigned char *);

/* set multiface mode - so correct rom can be used */
void	Multiface_SetMode(MULTIFACE_MODE);

/* called to finish emulation */
void	Multiface_Finish(void);

/* called to set memory read/write pointers when Multiface RAM/ROM is paged
in and out of Z80 address space */
void	Multiface_SetMemPointers(unsigned char **, unsigned char **);

/* called when a machine reset is done */
void	Multiface_Reset(void);

/* TRUE if multiface emulation enabled, false otherwise */
BOOL	Multiface_Enabled(void);

/* set enabled/disabled emulation state */ 
void	Multiface_EnableEmulation(BOOL State);

void	Multiface_WriteIO(unsigned short, unsigned char);

BOOL	Multiface_IsRomLoaded(MULTIFACE_ROM_TYPE RomType);
BOOL	Multiface_IsEmulationEnabled(void);

#endif
