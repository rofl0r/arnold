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
#ifndef __PSG_PLAY_HEADER_INCLUDED__
#define __PSG_PLAY_HEADER_INCLUDED__


#include "cpcglob.h"

typedef struct
{
	unsigned char VolA;
	unsigned char VolB;
	unsigned char VolC;
	unsigned char pad0;
} PSG_OUTPUT_VOLUME; 

typedef struct
{
	union 
	{
		signed long	L;

#ifdef CPC_LSB_FIRST
		struct
		{
			unsigned short Fraction;
			signed short Int;
		} W;
#else
		struct
		{
			signed short Int;
			unsigned short Fraction;
		} W;
#endif

	} FixedPoint;
} FIXED_POINT16;


PSG_OUTPUT_VOLUME	PSG_UpdateChannels(FIXED_POINT16 *pPeriodUpdate );

void	PSG_UpdateState(unsigned long Reg, unsigned long Data);

char	*PSG_GetRegisterName(int Index);

void	PSG_InitialiseToneUpdates(FIXED_POINT16 *pUpdate);


void PSGPlay_Write(int Register, int Data);
void PSGPlay_Reset(void);
void PSGPlay_Initialise(void);


#endif
