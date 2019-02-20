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
/* THIS IS NOT COMPLETE YET! */
#include "maketrk.h"


unsigned short CRC = 0x0ffff;

/* x^16 + x^12 + x^5 + 1 */


void	FDC_CRC_Init(void)
{
	CRC = 0x0ffff;
}

#if 0
void	FDC_CRC_Update(char *Byte)
{

	
	l = value ^ (*crc >> 8);
	*crc = (*crc & 0xff) | (l << 8);
	l >>= 4;
	l ^= (*crc >> 8);
	*crc <<= 8;
	*crc = (*crc & 0xff00) | l;
	l = (l << 4) | (l >> 4);
	h = l;
	l = (l << 2) | (l >> 6);
	l &= 0x1f;
	*crc = *crc ^ (l << 8);
	l = h & 0xf0;
	*crc = *crc ^ (l << 8);
	l = (h << 1) | (h >> 7);
	l &= 0xe0;
	*crc = *crc ^ l;
}




unsigned int CRCupdate(unsigned int CRC, unsigned char new)
{
 unsigned int aux = CRC ^ (new << 8);
 int i;

 for(i=0; i<8; i++)
   if (aux & 0x8000)
       aux = (aux <<= 1) ^ kCRCpoly;
     else
       aux <<= 1;

 return(aux);
}
#endif


void	MakeTrack_AddSync(char **pTrackData)
{
	int i;
	unsigned char *pLocalTrackData = (unsigned char *)*pTrackData;

	/* sync */
	for (i=12-1; i>=0; i--)
	{
		pLocalTrackData[0] = 0x0;
		pLocalTrackData++;
	}

	/* write sync bytes */
	pLocalTrackData[0] = 0x0a1;
	pLocalTrackData++;
	pLocalTrackData[0] = 0x0a1;
	pLocalTrackData++;
	pLocalTrackData[0] = 0x0a1;
	pLocalTrackData++;

	*pTrackData = (char *)pLocalTrackData;
}

void	MakeTrack(char *pTrackData)
{
	int i;
	int CRC = 0;
	int N = 2;
	int NCnt;
	int C = 0;
	int H = 0;
	int R = 0x041;
	int s;
	int GAP3 = 30;

	for (i=12-1; i>=0; i--)
	{
		pTrackData[0] = 0x00;
		pTrackData++;
	}

	/* index address mark */
	pTrackData[0] = 0x0c2;
	pTrackData++;
	pTrackData[0] = 0x0c2;
	pTrackData++;
	pTrackData[0] = 0x0c2;
	pTrackData++;
	pTrackData[0] = 0x0fc;


	for (i=50-1; i>=0; i--)
	{
		pTrackData[0] = 0x04e;
		pTrackData++;
	}


	for (s=0; s<9; s++)
	{
			
		/* id field */
		MakeTrack_AddSync(&pTrackData);

		pTrackData[0] = 0x0fe;
		pTrackData++;

		pTrackData[0] = C;
		pTrackData++;

		pTrackData[0] = H;
		pTrackData++;
		
		pTrackData[0] = R;
		pTrackData++;
		
		pTrackData[0] = N;
		pTrackData++;
	
		pTrackData[0] = CRC;
		pTrackData++;
		pTrackData[0] = CRC>>8;
		pTrackData++;

		/* GAP 2 */
		for (i=22-1; i>=0; i--)
		{
			pTrackData[0] = 0x04e;
			pTrackData++;
		}
		
		MakeTrack_AddSync(&pTrackData);
		pTrackData[0] = 0x0fb;
		pTrackData++;

		for (NCnt=512-1; NCnt>=0; NCnt--)
		{
			pTrackData[0] = 0x0e5;
			pTrackData++;
		}

		pTrackData[0] = CRC;
		pTrackData++;
		pTrackData[1] = CRC;
		pTrackData++;

		for (i=GAP3-1; i>=0; i--)
		{
			pTrackData[0] = 0x04e;
			pTrackData++;
		}
	}
}
