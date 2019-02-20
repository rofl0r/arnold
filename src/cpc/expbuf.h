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
#ifndef __SIMPLE_EXPANDING_BUFFER__
#define __SIMPLE_EXPANDING_BUFFER__

typedef struct 
{
	/* the actual buffer */
	unsigned char *pBuffer;
	/* actual size of allocated data */
	unsigned long Capacity;
	/* the actual amount of data that has been used */
	unsigned long Used;
	/* size to increase buffer by if we run out of space; note if size of data being added to buffer
	is bigger than this increment, the buffer will actually be increased by the size of the data*/
	unsigned long ExpansionIncrement;
} simple_expanding_buffer;


/* initialise the buffer */
void	simple_expanding_buffer_init(simple_expanding_buffer *buffer,int Increment);
/* free the buffer */
void	simple_expanding_buffer_free(simple_expanding_buffer *buffer);

/* write data to buffer, if data will not fit expand buffer */
int	simple_expanding_buffer_write(simple_expanding_buffer *buffer, unsigned char *pData, unsigned long DataSize);


#endif

