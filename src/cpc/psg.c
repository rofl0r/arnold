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
#include "cpcdefs.h"
#include "cpc.h"
#include "psg.h"
#include "cpcglob.h"


#ifdef AY_OUTPUT
#include "dumpym.h"
#endif


/* these are anded with the data when a read is done */
static const int		PSG_ReadAndMask[16] =
{
	0x0ff,	/* channel A tone fine */
	0x00f,	/* channel A tone coarse */
	0x0ff,	/* channel B tone fine */
	0x00f,	/* channel B tone coarse */
	0x0ff,	/* channel C tone fine */
	0x00f,	/* channel C tone coarse */
	0x01f,	/* noise */
	0x0ff,	/* mixer */
	0x01f,	/* volume A */
	0x01f,	/* volume B */
	0x01f,	/* volume C */
	0x0ff,	/* hardware envelope duration fine */
	0x0ff,	/* hardware envelope duration coarse */
	0x00f,	/* hardware envelope */
	0x0ff,	/* I/O port A */
	0x0ff	/* I/O port B */
};

typedef struct
{
	/* stores current selected register */
	int		PSG_SelectedRegister;
	/* stores current register data */
	int		PSG_Registers[16];
	/* io mask for port A and B */
	/* when 0x0ff will return input's, when 0x00 will return state of output latch */
	int		io_mask[2];
} AY_3_8912;

AY_3_8912 ay;

void	PSG_Init(void)
{
	PSGPlay_Initialise();
}


/* reset PSG and put it into it's initial state */
void	PSG_Reset(void)
{
	int i;

	/* reset all registers to 0 - as per the Ay-3-8912 spec */
	for (i=0; i<15; i++)
	{
		PSG_RegisterSelect(i);
		PSG_WriteData(0);
	}
	
	PSGPlay_Reset();
}

unsigned int		PSG_ReadData(void)
{
	switch (ay.PSG_SelectedRegister)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			return ay.PSG_Registers[ay.PSG_SelectedRegister] & PSG_ReadAndMask[ay.PSG_SelectedRegister];

		/* port A */
		case 14:
		{
			unsigned char KeyboardData;

			/* get keyboard data */
			KeyboardData = Keyboard_Read();
	
			/* output has a AND mask of 0, input has a AND mask of 0x0ff */

			/* if port A is set to input, a read will return keyboard line data */
			/* if port A is set to output, a read will return output latch ANDed with port input */
			return ((KeyboardData & ay.io_mask[0]) | (ay.PSG_Registers[14] & KeyboardData & (~ay.io_mask[0])));
		}
		break;

		/* if port B is set to input, a read will return 0x0ff */
		/* if port B is set to output, a read will return output latch ANDed with port input (always 0x0ff) */
		case 15:
			return ((0x0ff & ay.io_mask[1]) | (ay.PSG_Registers[15] & (~ay.io_mask[1])));
		default:
			break;
	}

	return 0x0ff;
}

void	PSG_WriteData(unsigned int Data )
{
	/* if port A or port B is set to output, writing to the port register will store the value */
	/* it can be read again as soon as the port is set to output */
	/* write data to register */
	ay.PSG_Registers[ay.PSG_SelectedRegister] = Data;

	/* setup I/O mask for reading/writing register depending on input/output status */
	if (ay.PSG_SelectedRegister==7)
	{
		ay.io_mask[0] = (ay.io_mask[1] = 0x0ff);

		if (Data & (1<<7))
		{
			/* port B is output mode */
			ay.io_mask[1] = !ay.io_mask[1];
		}

		if (Data & (1<<6))
		{
			ay.io_mask[0] = !ay.io_mask[0];
		}
	}

#ifdef AY_OUTPUT
	YMOutput_StoreRegData(ay.PSG_SelectedRegister, Data);
#endif

	/* write register for audio playback */
	PSGPlay_Write(ay.PSG_SelectedRegister, Data);
}


void	PSG_RegisterSelect(unsigned int Data)
{
	ay.PSG_SelectedRegister = Data & 0x0f;
}

/* for debugging */
int		PSG_GetSelectedRegister(void)
{
	return ay.PSG_SelectedRegister;
}

/* for debugging; not correct for port A and port B */
int		PSG_GetRegisterData(int RegisterIndex)
{
	RegisterIndex &= 0x0f;

	return ay.PSG_Registers[RegisterIndex];
}
