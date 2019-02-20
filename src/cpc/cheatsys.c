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
/* THIS IS NOT YET COMPLETE!!! */
#include "cheatsys.h"
#include "z80/z80.h"


#define MAX_NUM_OCCURANCES 65536
static int CheatSystem_Stage;

static char SearchByte;
static int Addressess[MAX_NUM_OCCURANCES];

static int		CountOccurances(void)
{
	int Count = 0;
	int i;

	for (i=0; i<MAX_NUM_OCCURANCES; i++)
	{
		if (Addressess[i] != -1)
		{
			Count++;
		}
	}

	return Count;
}



void	CheatSystem_Initialise(void)
{
	int i;

	for (i=0; i<MAX_NUM_OCCURANCES; i++)
	{
		Addressess[i] = -1;
	}

	CheatSystem_Stage = 0;
}



static void AddOccuranceToList(int ThisAddr)
{
	int i;

	for (i=0; i<MAX_NUM_OCCURANCES; i++)
	{
		if (Addressess[i]==-1)
		{
			Addressess[i] = ThisAddr;
			break;
		}
	}
}




static void	AddOccurancesToList(void)
{
	int i;

	for (i=0; i<65536; i++)
	{
		unsigned char ThisByte;

		ThisByte = (unsigned char)Z80_RD_MEM((Z80_WORD)i);

		if (ThisByte == SearchByte)
		{
			AddOccuranceToList(i);
		}
	}
}

static void CheckOccurancesInList(void)
{
	int i;

	for (i=0; i<MAX_NUM_OCCURANCES; i++)
	{
		if (Addressess[i]!=-1)
		{
			unsigned char ThisByte;

			ThisByte = (unsigned char)Z80_RD_MEM((Z80_WORD)Addressess[i]);

			if (ThisByte != (SearchByte-1))
			{
				Addressess[i] = -1;
			}
		}
	}

	SearchByte--;
}

void	CheatSystem_Update(void)
{
	switch (CheatSystem_Stage)
	{
		case 0:
		{
			SearchByte = 5;
			AddOccurancesToList();
			
			/* as long as there are a few.. continue */
			if (CountOccurances()>1)
			{
				CheatSystem_Stage = 1;

			}
		}
		break;

		case 1:
		{

			/* update these occurances. If they have changed by the correct
			 amount, then they are not removed. otherwise they are removed			CheckOccurancesInList();
*/
			CheckOccurancesInList();

			/* if there is one remaining, want to check it one more time..
			if (CountOccurances()<=1)
			{
				printf("found!");
			}
	*/
		}
		break;
	}
}

