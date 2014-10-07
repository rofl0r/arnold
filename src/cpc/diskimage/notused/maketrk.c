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

	unsigned char *pTrackPtr;
	unsigned char *pTrackStart;
	unsigned char *pTrackEnd;



char TrackBuffer[7*1024];

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

void	FDD_WriteBytesToTrack(char Data, unsigned long Count)
{
	unsigned long BytesToTrackEnd;

	/* calculate number of bytes to track end */
	BytesToTrackEnd = pTrackEnd - pTrackPtr;

	/* goes over end of track */
	if (BytesToTrackEnd>Count)
	{
		int i;

		/* doesn't go over end of track */
		unsigned char *pTrack;

		pTrack = pTrackPtr;

		for (i=0; i<Count; i++)
		{
			pTrack[0] = Data;
			pTrack++;
		}
	
		pTrackPtr = pTrack;
	}
	else
	{
		/* goes over end of track */
		int i;

		unsigned char *pTrack;

		/* write bytes to end of track */
		pTrack = pTrackPtr;

		for (i=0; i<BytesToTrackEnd; i++)
		{
			pTrack[0] = Data;
			pTrack++;
		}

		pTrack = pTrackStart;

		for (i=0; i<(Count - BytesToTrackEnd); i++)
		{
			pTrack[0] = Data;
			pTrack++;
		}

		pTrackPtr = pTrack;
	}
}

void	FDD_WriteByteToTrack(char Data)
{
	unsigned char *pTrack;

	pTrack = pTrackPtr;

	pTrack[0] = Data;
	pTrack++;

	if (pTrack>=pTrackEnd)
	{
		pTrack = pTrackStart;
	}

	pTrackPtr = pTrack;
}




#define TRACK_SIZE	6656

#define MAX_MARKS			20

#define FDC_ID_MARK				0x0fe
#define FDC_DATA_MARK			0x0fb
#define FDC_DELETED_DATA_MARK	0x0f8

typedef struct 
{
	unsigned long Positions[MAX_MARKS];
	unsigned long Count;
} FDC_MFM_MARK;

FDC_MFM_MARK	MFM_ID;
FDC_MFM_MARK	Data;


void	FDC_InitialiseIDAndDataPositions(void)
{
	int i;
	unsigned char *pDataPtr;
	unsigned char *pDataStart;

	MFM_ID.Count = 0;
	Data.Count = 0;

	pDataPtr = (pDataStart = (unsigned char *)TrackBuffer);

	for (i=0; i<TRACK_SIZE; i++)
	{
		switch (pDataPtr[0])
		{
			case FDC_ID_MARK:
			{
				MFM_ID.Positions[MFM_ID.Count] = (unsigned long)pDataPtr - (unsigned long)pDataStart;
				MFM_ID.Count++;
			}
			break;


			case FDC_DATA_MARK:
			case FDC_DELETED_DATA_MARK:
			{
				Data.Positions[Data.Count] = (unsigned long)pDataPtr - (unsigned long)pDataStart;
				Data.Count++;
			}
			break;
		}
	
		pDataPtr++;
	}
}




/* get closest mark to the current position, marks points to either ID or data marks */
unsigned long	FDC_GetClosestMarkToCurrentPosition(unsigned long PositionInTrack, FDC_MFM_MARK *marks)
{
	int i;

	/* simple search through list for now */
	for (i=0; i<marks->Count; i++)
	{
		if (marks->Positions[i]>=PositionInTrack)
		{
			return marks->Positions[i];
		}
	}

	/* if no mark is close enough, then the next closest mark is at the beginning of the track */
	return marks->Positions[0];
}


/* gets the number of bytes from the current position to the next mark position */
unsigned long	FDC_GetBytesToNextMark(unsigned long PositionInTrack, FDC_MFM_MARK *marks)
{
	if (marks->Count==0)
	{
		/* time to get to index from current position + time to see next index after that. */
		return (TRACK_SIZE - PositionInTrack) + TRACK_SIZE;
	}

	{
		unsigned long ClosestMarkPosition = FDC_GetClosestMarkToCurrentPosition(PositionInTrack, marks);

		if (ClosestMarkPosition<PositionInTrack)
		{
			return (TRACK_SIZE - PositionInTrack)+ClosestMarkPosition;
		}

		return ClosestMarkPosition - PositionInTrack;
	}
}

static unsigned long LastPositionInTrack = 0;
static unsigned long NopFraction = 0;
static unsigned long PreviousNopCount = 0;

void	FDD_SetNopCount(void)
{
	PreviousNopCount = CPC_GetNopCount();
}


unsigned long	FDD_CalculatePositionInTrack(void)
{
	unsigned long NopCount;
	unsigned long BytesPassed;
	unsigned long CurrentNopCount;
	unsigned long NopDifference;
	unsigned long Offset;

	CurrentNopCount = CPC_GetNopCount();
	NopDifference = CurrentNopCount - PreviousNopCount;
	NopDifference = NopDifference+NopFraction;

	/* calculate new nop fraction */
	NopFraction = NopDifference & 31;
	/* calculate bytes passed since last time */
	BytesPassed = NopDifference>>5;

	PreviousNopCount = CurrentNopCount;

	/* calculate new position from old position and number of bytes
	passed, and make sure it is within the track size */
	Offset = (LastPositionInTrack + BytesPassed) % TRACK_SIZE;

	/* store position for next time */
	LastPositionInTrack = Offset;

	return Offset;
}

/* get NOPS to ID position */
unsigned long	FDC_GetNopsToNextID(void)
{
	unsigned long Position;
	unsigned long BytesToNextMark;

	/* calculate current position in track */
	Position = FDD_CalculatePositionInTrack();

	/* get number of bytes to next mark */
	BytesToNextMark = FDC_GetBytesToNextMark(Position, &MFM_ID);

	return BytesToNextMark<<5;
}


unsigned long FDC_GetNopsToNextIndex(void)
{
	unsigned long PositionInTrack;

	PositionInTrack = FDD_CalculatePositionInTrack();

	return ((TRACK_SIZE - PositionInTrack)<<5);
}

void	FDC_InitialiseIDAndDataPositions(void);
void	FDD_SetNopCount(void);


