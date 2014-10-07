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
#ifndef __PSG_HEADER_INCLUDED__
#define __PSG_HEADER_INCLUDED__

#include "cpcglob.h"
#include "psgplay.h"

#define PSG_MIXER_IO_B_INPUT_ENABLE 0x080
#define PSG_MIXER_IO_A_INPUT_ENABLE 0x040
#define PSG_MIXER_NOISE_C_ENABLE 0x20
#define PSG_MIXER_NOISE_B_ENABLE 0x10
#define PSG_MIXER_NOISE_A_ENABLE 0x08
#define PSG_MIXER_TONE_C_ENABLE 0x04
#define PSG_MIXER_TONE_B_ENABLE	0x02
#define PSG_MIXER_TONE_A_ENABLE	0x01

#define PSG_ENVELOPE_HOLD		0x01
#define PSG_ENVELOPE_ALTERNATE	0x02
#define PSG_ENVELOPE_ATTACK		0x04
#define PSG_ENVELOPE_CONTINUE	0x08

/* reset PSG */
void	PSG_Reset(void);

/* read data from selected register */
unsigned int	PSG_ReadData(void);

/* write data to selected register */
void	PSG_WriteData(unsigned int);

/* select register */
void	PSG_RegisterSelect(unsigned int);

/* get selected register - for snapshot and multiface, and ASIC */
int		PSG_GetSelectedRegister(void);
/* get register data - for snapshot and multiface */
int		PSG_GetRegisterData(int);

void	PSG_Init();

#endif
