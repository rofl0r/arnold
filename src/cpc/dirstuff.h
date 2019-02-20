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
#ifndef __DIRSTUFF_HEADER_INCLUDED__
#define __DIRSTUFF_HEADER_INCLUDED__


typedef enum
{
	EMULATOR_DIR = 0,
	EMULATOR_ROM_DIR,
	EMULATOR_ROM_CPCAMSDOS_DIR,
	EMULATOR_ROM_CPC464_DIR,
	EMULATOR_ROM_CPC664_DIR,
	EMULATOR_ROM_CPC6128_DIR,
	EMULATOR_ROM_CPCPLUS_DIR,
	EMULATOR_ROM_KCCOMPACT_DIR,
	LAST_DIR
} LOCATION_ID;

void	DirStuff_Initialise(void);
void	DirStuff_Finish(void);
void	SetDirectoryForLocation(LOCATION_ID, unsigned char *);
void	ChangeToLocationDirectory(LOCATION_ID);

#endif
