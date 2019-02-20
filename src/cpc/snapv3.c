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
/* Snapshot V3 format as used by No$CPC v1.8 */

#include "cpcglob.h"
#include "snapv3.h"
#include "riff.h"
#include "cpc.h"
#include "z80/z80.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* start of chunk */
static	unsigned long SnapshotV3_ChunkLength;
static	RIFF_CHUNK	*pRiffChunk;

/* begin a chunk */
unsigned char *SnapshotV3_BeginChunk(unsigned char *buffer, unsigned long ChunkName)
{
	RIFF_CHUNK ChunkHeader;

	/* setup initial header */
	ChunkHeader.ChunkName = ChunkName;
	ChunkHeader.ChunkLength = 0;

	/* write into buffer */
	pRiffChunk = (RIFF_CHUNK *)buffer;
	memcpy(buffer, (unsigned char *)&ChunkHeader, sizeof(RIFF_CHUNK));
	buffer+=sizeof(RIFF_CHUNK);

	/* reset chunk length */
	SnapshotV3_ChunkLength = 0;

	return buffer;
}

/* write data to chunk */
unsigned char *SnapshotV3_WriteDataToChunk(unsigned char *buffer, unsigned char *pData, unsigned long Length)
{
	/* write data to buffer and update length */
	memcpy(buffer, pData, Length);
	SnapshotV3_ChunkLength+=Length;

	buffer+=Length;

	return buffer;
}

/* end a chunk */
void SnapshotV3_EndChunk(void)
{
	/* calc pointer to header */
	RIFF_CHUNK *pHeader = (RIFF_CHUNK *)(pRiffChunk);

	/* write length into header */
	Riff_SetChunkLength(pHeader, SnapshotV3_ChunkLength);
}

unsigned char *SnapshotV3_WriteByte(unsigned char *buffer, unsigned char ByteData)
{
	return SnapshotV3_WriteDataToChunk(buffer, &ByteData, 1);
}

unsigned char *SnapshotV3_WriteWord(unsigned char *buffer, unsigned short WordData)
{
	unsigned char ByteData;

	ByteData = WordData & 0x0ff;
	buffer = SnapshotV3_WriteByte(buffer, ByteData);
	ByteData = (WordData>>8) & 0x0ff;
	buffer = SnapshotV3_WriteByte(buffer,ByteData);

	return buffer;
}



/* handle chunk on reading */
void	SnapshotV3_HandleChunk(RIFF_CHUNK *pCurrentChunk,unsigned long Size)
{
	unsigned long ChunkName = Riff_GetChunkName(pCurrentChunk);

	switch (ChunkName)
	{
		case RIFF_FOURCC_CODE('C','P','C','+'):
		{
			unsigned char *pChunkData = Riff_GetChunkDataPtr(pCurrentChunk);
			unsigned long ASICRamOffset;
			int i;

			/* sprite data */
			ASICRamOffset = 0x04000;

			/* each sprite is 16x16 and there are 16 of these */
			/* in the snapshot the x pixels are compressed so there are 2 pixels per byte,
			therefore each sprite takes up 8*16 bytes. */
			for (i=0; i<((16*16*16)>>1); i++)
			{
				unsigned char PackedPixels;

				PackedPixels = pChunkData[0];

				ASIC_WriteRamFull(ASICRamOffset, (PackedPixels>>4) & 0x0f);
				ASICRamOffset++;
				ASIC_WriteRamFull(ASICRamOffset, PackedPixels & 0x0f);
				ASICRamOffset++;
				pChunkData++;
			}

			/* sprite attributes */
			ASICRamOffset = 0x06000;

			for (i=0; i<16; i++)
			{
				/* x */
				ASIC_WriteRamFull(ASICRamOffset, pChunkData[0]);
				pChunkData++;
				ASICRamOffset++;

				ASIC_WriteRamFull(ASICRamOffset, pChunkData[0]);
				pChunkData++;
				ASICRamOffset++;

				/* y */
				ASIC_WriteRamFull(ASICRamOffset, pChunkData[0]);
				pChunkData++;
				ASICRamOffset++;

				ASIC_WriteRamFull(ASICRamOffset, pChunkData[0]);
				pChunkData++;
				ASICRamOffset++;

				/* mag */
				ASIC_WriteRamFull(ASICRamOffset, pChunkData[0]);
				pChunkData++;
				ASICRamOffset++;

				/* unused */
				pChunkData+=3;
				ASICRamOffset+=3;
			}

			/* palettes */
			ASICRamOffset = 0x06400;

			for (i=0; i<32; i++)
			{
				ASIC_WriteRamFull(ASICRamOffset, pChunkData[0]);
				pChunkData++;
				ASICRamOffset++;

				ASIC_WriteRamFull(ASICRamOffset, pChunkData[0]);
				pChunkData++;
				ASICRamOffset++;
			}

			/* misc */
			ASIC_WriteRamFull(0x06800, pChunkData[0]);
			pChunkData++;

			ASIC_WriteRamFull(0x06801, pChunkData[0]);
			pChunkData++;

			ASIC_WriteRamFull(0x06802, pChunkData[0]);
			pChunkData++;

			ASIC_WriteRamFull(0x06803, pChunkData[0]);
			pChunkData++;

			ASIC_WriteRamFull(0x06804, pChunkData[0]);
			pChunkData++;

			ASIC_WriteRamFull(0x06805, pChunkData[0]);
			pChunkData++;

			/* skip unused */
			pChunkData+=2;

			/* analogue inputs */
			for (i=0; i<8; i++)
			{
				ASIC_SetAnalogueInput(i,pChunkData[0]);
				pChunkData++;
			}
			
			/* dma */
			ASICRamOffset = 0x06c00;

			for (i=0; i<3; i++)
			{
				/* address */
				ASIC_WriteRamFull(ASICRamOffset, pChunkData[0]);
				pChunkData++;
				ASICRamOffset++;

				ASIC_WriteRamFull(ASICRamOffset, pChunkData[0]);
				pChunkData++;
				ASICRamOffset++;

				/* prescale */
				ASIC_WriteRamFull(ASICRamOffset, pChunkData[0]);
				pChunkData++;
				ASICRamOffset++;

				/* unused */
				pChunkData++;
				ASICRamOffset++;
			}

			pChunkData+=3;

			/* DCSR */
			ASIC_WriteRamFull(0x06c0f, pChunkData[0]);
			pChunkData++;

			/* TO BE COMPLETED! */
			pChunkData+=(3*7);


			/* set secondary rom mapping */
			ASIC_SetSecondaryRomMapping(pChunkData[0]);
			pChunkData++;

			/* set lock state */
			if (pChunkData[0]&0x01)
			{
				/* unlock */
				ASIC_SetUnLockState(TRUE);
			}
			else
			{
				/* lock */
				ASIC_SetUnLockState(FALSE);
			}
			
			/* position in lock? */
		}
		break;
	}
}

/* calculate size of CPC Plus chunk */
unsigned long SnapshotV3_CPCPlus_CalculateOutputSize()
{
	unsigned long nChunkLength;

	/* chunk header */
	nChunkLength = sizeof(RIFF_CHUNK);

	/* sprite data */
	nChunkLength += ((16*16*16)>>1);

	/* sprite attributes */
	nChunkLength += (8*16);

	/* palettes */
	nChunkLength += (32*2);

	/* misc */
	nChunkLength += 6;

	/* unused */
	nChunkLength +=2;

	/* analogue inputs */
	nChunkLength+=8;

	/* dma */
	nChunkLength+=(3*4);

	/* unused */
	nChunkLength+=3;

	/* dcsr */
	nChunkLength++;

	/* dma internal */
	nChunkLength +=(3*7);

	/* secondary rom mapping */
	nChunkLength ++;

	/* lock state */
	nChunkLength ++;

	return nChunkLength;
}

unsigned char *SnapshotV3_CPCPlus_WriteChunk(unsigned char *buffer)
{
	unsigned char *pASICRamPtr;
	unsigned char *pPtr;
	int i;

	pASICRamPtr = ASIC_GetRamPtr();
	buffer = SnapshotV3_BeginChunk(buffer, RIFF_FOURCC_CODE('C','P','C','+'));

	pPtr = &pASICRamPtr[0x04000-0x04000];

	/* sprite data */
	for (i=0; i<((16*16*16)>>1); i++)
	{
		unsigned char PackedPixels;

		/* generate the packed pixel from 2 sprite pixels */
		PackedPixels = (pPtr[0] & 0x0f);
		PackedPixels = PackedPixels<<4;
		PackedPixels |= (pPtr[1] & 0x0f);
		pPtr+=2;
		
		/* write to snapshot */
		buffer = SnapshotV3_WriteByte(buffer, PackedPixels);
	}

	/* sprite attributes */
	for (i=0; i<16; i++)
	{
		buffer = SnapshotV3_WriteWord(buffer, ASIC_GetSpriteX(i));
		buffer = SnapshotV3_WriteWord(buffer, ASIC_GetSpriteY(i));
		buffer = SnapshotV3_WriteByte(buffer, ASIC_GetSpriteMagnification(i));
		buffer = SnapshotV3_WriteByte(buffer, 0);
		buffer = SnapshotV3_WriteByte(buffer, 0);
		buffer = SnapshotV3_WriteByte(buffer, 0);
	}

	/* palettes */
	pPtr = &pASICRamPtr[0x06400-0x04000];
	buffer = SnapshotV3_WriteDataToChunk(buffer, pPtr, (32*2));

	/* misc */
	buffer = SnapshotV3_WriteByte(buffer, ASIC_GetPRI());
	buffer = SnapshotV3_WriteByte(buffer, ASIC_GetSPLT());
	buffer = SnapshotV3_WriteWord(buffer, ASIC_GetSSA());
	buffer = SnapshotV3_WriteByte(buffer, ASIC_GetSSCR());
	buffer = SnapshotV3_WriteByte(buffer, ASIC_GetIVR());

	/* unused */
	buffer = SnapshotV3_WriteWord(buffer, 0);

	/* analogue inputs */
	for (i=0; i<8; i++)
	{
		buffer = SnapshotV3_WriteByte(buffer, ASIC_GetAnalogueInput(i));
	}

	/* DMA */
	for (i=0; i<3; i++)
	{
		buffer = SnapshotV3_WriteWord(buffer, ASIC_DMA_GetChannelAddr(i));
		buffer = SnapshotV3_WriteByte(buffer, ASIC_DMA_GetChannelPrescale(i));
		buffer = SnapshotV3_WriteByte(buffer, 0);
	}
	/* unused */
	buffer = SnapshotV3_WriteWord(buffer, 0);
	buffer = SnapshotV3_WriteByte(buffer, 0);

	/* DCSR */
	buffer = SnapshotV3_WriteByte(buffer, ASIC_GetDCSR());

	/* DMA internal */
	for (i=0; i<3*7; i++)
	{
		buffer = SnapshotV3_WriteByte(buffer, 0);
	}

	/* secondary rom mapping */
	buffer = SnapshotV3_WriteByte(buffer, (ASIC_GetSecondaryRomMapping() & 0x01f)|0x0a0);
	
	/* lock state */
	if (ASIC_GetUnLockState())
	{
		/* unlocked */
		buffer = SnapshotV3_WriteByte(buffer, 1);
	}
	else
	{
		buffer = SnapshotV3_WriteByte(buffer, 0);
	}

	SnapshotV3_EndChunk();

	return buffer;
}

