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
#ifndef __RIFF_HEADER_INCLUDED__
#define __RIFF_HEADER_INCLUDED__

#include "cpcdefs.h"

#ifdef CPC_LSB_FIRST
#define RIFF_FOURCC_CODE(a,b,c,d) \
        (unsigned long)(((unsigned char)a)		| \
                       (((unsigned char)b)<<8)		| \
                       (((unsigned char)c)<<16)		| \
                       (((unsigned char)d)<<24))
#else
#define RIFF_FOURCC_CODE(a,b,c,d) \
        (unsigned long)((((unsigned char)a)<<24)	| \
                       (((unsigned char)b)<<16)		| \
                       (((unsigned char)c)<<8)		| \
                       (((unsigned char)d)))
#endif


/* structure defining RIFF_CHUNK header. Chunk data follows header. */
typedef struct 
{
        unsigned long ChunkName;
        unsigned long ChunkLength;
} RIFF_CHUNK;

unsigned long Riff_GetChunkName(RIFF_CHUNK *pChunk);
int Riff_GetChunkLength(RIFF_CHUNK *pChunk);
void	Riff_SetChunkLength(RIFF_CHUNK *pChunk,unsigned long);
unsigned char   *Riff_GetChunkDataPtr(RIFF_CHUNK *pChunk);
RIFF_CHUNK      *Riff_GetNextChunk(RIFF_CHUNK *pChunk);
RIFF_CHUNK      *Riff_FindNamedSubChunk(RIFF_CHUNK *pHeader, unsigned long ChunkName);
RIFF_CHUNK		*Riff_GetFirstChunk(unsigned char *pFileStart);
BOOL	Riff_CheckChunkSizesAreValid(unsigned char *pRiffFile, unsigned long RiffFileSize);

#endif
