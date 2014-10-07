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
/* SPO-256 speech chip emulation */
/* on CPC input/output port is 0x0fbfe */
/* appears that when writing data lines hold address */
/* when reading bit 7 contains LRQ */

static int SPO_LRQ = 0;		/* 0 when value can be loaded, 1 when value cannot be loaded */
static int SPO_SBY = 1;		/* 1 indicates SPO is inactive, 0 indictates it is active */

#include "spo256.h"

unsigned char buffer[256];



void	SPO256_Initialise(void)
{
/*	FILE *fh;

	fh = fopen("currah.dat","r");

	fgets(buffer, sizeof(buffer), f);

	fgetc(buffer, sizeof(char), fh);


	if (fscanf(fin,"%D %D %D %D %D %d\n",
						&(allos[i].offset),
						&(allos[i].relrestart),
						&(allos[i].length),
						&(allos[i].pause),
						&(allos[i].playlength),
						&(allos[i].simple)
					  )!=6) {
*/


}

#if 0
static void	SPO256_SetAddress(unsigned char Addr)
{
	/* SPO address is 9-bits. Lower bit is always
	zero because word's are acccessed. Each word
	contains a coefficient value */
}
#endif

/* CPC interface to SPO 256 */
void	CPC_SPO256_WriteData(unsigned char Data)
{
/*	SPO256_SetAddress(Data); */

	/* accepted command, no others can be loaded */
	SPO_LRQ = 0x01;
}

/* CPC interface to SPO 256 - works for SSA-1! */
unsigned char CPC_SPO256_ReadData(void)
{
	unsigned char Data;

	Data = 0;

	Data = (unsigned char)(Data | ((SPO_LRQ^0x01)<<7) | ((SPO_SBY^0x01)<<6));

	/* for now we will assume that it completes it immediatly */
	if (SPO_LRQ == 0x01)
	{
		SPO_LRQ = 0x00;
	}

        return Data;
}
