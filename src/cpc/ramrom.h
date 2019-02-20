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
#ifndef __RAM_ROM_HEADER_INCLUDED__
#define __RAM_ROM_HEADER_INCLUDED__

#include "cpc.h"

/* Inicron RAM-ROM */

#define RAM_ROM_FLAGS_RAM_ON	0x0001
#define RAM_ROM_FLAGS_RAM_WRITE_ENABLE	0x0002
#define RAM_ROM_FLAGS_EPROM_ON	0x0004

void	RAM_ROM_Initialise(int NumBlocks);
void	RAM_ROM_Finish(void);
void	RAM_ROM_SetBankEnable(int Bank, BOOL State);
BOOL	RAM_ROM_GetBankEnableState(int Bank);
void	RAM_ROM_RethinkMemory(unsigned char **, unsigned char **);
BOOL	RAM_ROM_IsRamOn(void);
BOOL	RAM_ROM_IsRamWriteEnabled(void); 
void	RAM_ROM_SetRamOnState(BOOL);
void	RAM_ROM_SetRamWriteEnableState(BOOL);
void	RAM_ROM_SetEPROMOnState(BOOL);
BOOL	RAM_ROM_IsEPROMOn(void);
void	RAM_ROM_SetupTable(void);

#endif
