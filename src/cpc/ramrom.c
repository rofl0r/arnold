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
#include "ramrom.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

/*************************/
/**** INICRON RAM-ROM ****/

/* buffer to hold ram contents; split into banks of 16k */
static unsigned char	*RAM_ROM_RAM = NULL;
/* an array of bits, a bit will be 1 if the corresponding bank is enabled,
and 0 if the corresponding bank is disabled */
static unsigned char	*RAM_ROM_BankEnables = NULL;
/* rom select index for eprom onboard ram-rom (not emulated) */
static unsigned char	RAM_ROM_EPROM_Bank = 0;
/* flags about the ram-rom status */
static unsigned char	RAM_ROM_Flags = 0;
/* number of banks the ram-rom holds */
static unsigned long	RAM_ROM_NumBlocks;
/* the mask will be 0 if ram is write disabled and 0x0ffffffff if the ram is write enabled */
static unsigned long	RAM_ROM_WriteMask;

BOOL	RAM_ROM_IsRamOn(void)
{
	return ((RAM_ROM_Flags & RAM_ROM_FLAGS_RAM_ON)!=0);
}

/* set if ram-rom ram is enabled and therefore if roms can be seen */
void	RAM_ROM_SetRamOnState(BOOL State)
{
	if (State)
	{
		RAM_ROM_Flags |= RAM_ROM_FLAGS_RAM_ON;
	}
	else
	{
		RAM_ROM_Flags &= ~RAM_ROM_FLAGS_RAM_ON;
	}
	
	ExpansionROM_RefreshTable();
}

/* set read/write state for whole ram */
void	RAM_ROM_SetRamWriteEnableState(BOOL State)
{
	if (State)
	{
		RAM_ROM_Flags |= RAM_ROM_FLAGS_RAM_WRITE_ENABLE;
	}
	else
	{
		RAM_ROM_Flags &= ~RAM_ROM_FLAGS_RAM_WRITE_ENABLE;
	}
	ExpansionROM_RefreshTable();
}

/* set if eprom on ram-rom is visible */
void	RAM_ROM_SetEPROMOnState(BOOL State)
{
	if (State)
	{
		RAM_ROM_Flags |= RAM_ROM_FLAGS_EPROM_ON;
	}
	else
	{
		RAM_ROM_Flags &= ~RAM_ROM_FLAGS_EPROM_ON;
	}

	ExpansionROM_RefreshTable();
}

/* true if ram is write enabled, false if ram is write disabled */
BOOL	RAM_ROM_IsRamWriteEnabled(void)
{
	return ((RAM_ROM_Flags & RAM_ROM_FLAGS_RAM_WRITE_ENABLE)!=0);
}

/* true if rom is on and visible, false if off */
BOOL	RAM_ROM_IsEPROMOn(void)
{
	return ((RAM_ROM_Flags & RAM_ROM_FLAGS_EPROM_ON)!=0);
}

/* get selection value for rom */
int		RAM_ROM_GetEPROMBank(void)
{
	return RAM_ROM_EPROM_Bank;
}

/* initialise, allocate memory and setup */
void	RAM_ROM_Initialise(int NumBlocks)
{
	int Size;

	RAM_ROM_NumBlocks = NumBlocks;

	Size = NumBlocks*16*1024;

	RAM_ROM_RAM = malloc(Size);

	if (RAM_ROM_RAM!=NULL)
	{
		memset(RAM_ROM_RAM, 0, Size);
	}
	
	Size = NumBlocks>>3;

	
	RAM_ROM_BankEnables = malloc(Size);

	if (RAM_ROM_BankEnables!=NULL)
	{
		memset(RAM_ROM_BankEnables, 0x0ff, Size);
	}
		
	RAM_ROM_WriteMask = 0;
	RAM_ROM_Flags = 0;

}

void	RAM_ROM_Finish(void)
{
	if (RAM_ROM_RAM!=NULL)
	{
		free(RAM_ROM_RAM);
		RAM_ROM_RAM = NULL;
	}

	if (RAM_ROM_BankEnables!=NULL)
	{
		free(RAM_ROM_BankEnables);
		RAM_ROM_BankEnables = NULL;
	}
}

/* called when ram-rom has changed state and some tables need to be re-setup */
void	RAM_ROM_SetupTable(void)
{
	int i;

	RAM_ROM_WriteMask = 0;

	if (RAM_ROM_RAM==NULL)
		return;

	if ((RAM_ROM_Flags & RAM_ROM_FLAGS_RAM_ON)==0)
		return;

	if (RAM_ROM_Flags & RAM_ROM_FLAGS_RAM_WRITE_ENABLE)
	{
		RAM_ROM_WriteMask = ~0;
	}

	for (i=0; i<16; i++)
	{
		unsigned long BankByte;
		unsigned char BankBit;
		unsigned long BankSelected;

		if (i<RAM_ROM_NumBlocks)
		{
			BankSelected = i;

			BankByte = BankSelected>>3;
			BankBit = BankSelected & 0x07;

			/* if enabled... */
			if (RAM_ROM_BankEnables[BankByte] & (1<<BankBit))
			{
				unsigned char	*RAM_ROM_BankPtr;

				/* get pointer to data */
				RAM_ROM_BankPtr = (unsigned char *)((unsigned long)(RAM_ROM_RAM + (BankSelected<<14)) - (unsigned long)0x0c000);

				/* setup entry in table */
//				ExpansionROMTable[i] = RAM_ROM_BankPtr;

				ExpansionROM_SetTableEntry(i, RAM_ROM_BankPtr);
			}
		}
	}
}

/* set bank enabled  state - if bank is enabled it is visible */
void	RAM_ROM_SetBankEnable(int Bank, BOOL State)
{
	unsigned long BankByte;
	unsigned char BankBit;

	if (RAM_ROM_BankEnables==NULL)
		return;

	BankByte = Bank>>3;
	BankBit = Bank & 0x07;

	if (State)
	{
		RAM_ROM_BankEnables[BankByte] |= (1<<BankBit);
	}
	else
	{
		RAM_ROM_BankEnables[BankByte] &= ~(1<<BankBit);
	}

	ExpansionROM_RefreshTable();
}

/* true if bank is enabled, false otherwise */
BOOL	RAM_ROM_GetBankEnableState(int Bank)
{
	unsigned long BankByte;
	unsigned char BankBit;

	if (RAM_ROM_BankEnables==NULL)
		return FALSE;

	BankByte = Bank>>3;
	BankBit = Bank & 0x07;

	return ((RAM_ROM_BankEnables[BankByte] & (1<<BankBit))!=0);
}

void	RAM_ROM_RethinkMemory(unsigned char **pReadPtr, unsigned char **pWritePtr)
{	
	unsigned char *pPtr;
	unsigned long Mask = RAM_ROM_WriteMask;

	/* this will be pointer to rom or ram in ram/rom */
	
	/* if mask is all 1's, then pPtr = pReadPtr[6] 
	if mask is all 0's then pPtr = pWritePtr[6]; */
	pPtr = (unsigned char *)(((unsigned long)pReadPtr[6]&Mask)|((unsigned long)pWritePtr[6]&(~Mask)));

	pWritePtr[6] = pWritePtr[7] = pPtr;
}

void	RAM_ROM_Install(void)
{
	RAM_ROM_Initialise(16);
}

void	RAM_ROM_DeInstall(void)
{
	RAM_ROM_Finish();
}
