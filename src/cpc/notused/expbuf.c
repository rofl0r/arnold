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
/* a simple expanding buffer used by snapshot write routine */
#include "headers.h"
#include "cpcdefs.h"
#include "expbuf.h"

/* initialise the buffer */
void	simple_expanding_buffer_init(simple_expanding_buffer *buffer,int Increment)
{
	buffer->Capacity = 0;
	buffer->Used = 0;
	buffer->ExpansionIncrement = Increment;
	buffer->pBuffer = NULL;
}

/* free the buffer */
void	simple_expanding_buffer_free(simple_expanding_buffer *buffer)
{
	if (buffer->pBuffer!=NULL)
	{
		free(buffer->pBuffer);
		buffer->pBuffer = NULL;
	}
}

/* write data to buffer, if data will not fit expand buffer */
int	simple_expanding_buffer_write(simple_expanding_buffer *buffer, unsigned char *pData, unsigned long DataSize)
{
	/* actual length to copy to buffer */
	int CopyLength;

	/* calculate space remaining */
	int SpaceRemaining = buffer->Capacity - buffer->Used;

	/* enough space to fit data into buffer? */
	if (SpaceRemaining<DataSize)
	{
		unsigned char *pBuffer;

		/* no, increase size of buffer */
		int ActualIncrement;

		ActualIncrement = buffer->ExpansionIncrement;

		/* is data larger than space remaining and increment? */
		if (DataSize>(buffer->ExpansionIncrement+SpaceRemaining))
		{
			/* yes, so increase buffer by datasize - spaceremaining */
			ActualIncrement = DataSize-SpaceRemaining;
		}
	
		/* alloc and copy data to new buffer */
		pBuffer = (unsigned char *)realloc(buffer->pBuffer, buffer->Capacity+ActualIncrement);

		if (pBuffer!=NULL)
		{
			buffer->pBuffer = pBuffer;
			buffer->Capacity += ActualIncrement;
			SpaceRemaining = buffer->Capacity - buffer->Used;
		}
	}

	/* if allocate failed, don't let memcpy go over end of buffer */
	if (SpaceRemaining<DataSize)
	{
		CopyLength = SpaceRemaining;
	}
	else
	{
		CopyLength = DataSize;
	}

	if (CopyLength!=0)
	{
		/* copy data */
		memcpy(buffer->pBuffer + buffer->Used, pData, CopyLength);
		/* update offset */
		buffer->Used += CopyLength;
	}

	return CopyLength;
}

