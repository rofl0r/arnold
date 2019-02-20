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
#ifndef __TZX_TAPE_IMAGE_HEADER_INCLUDED__
#define __TZX_TAPE_IMAGE_HEADER_INCLUDED__

/* insert and validate a tape image */
BOOL	TapeImage_Insert(char *);
/* remove a tape image */
void	TapeImage_Remove(void);
/* rewind tape to start */
void	TapeImage_RewindToStart(void);

/* get current bit of data */
int		TapeImage_GetBit(int);
void	TapeImage_Update(int);
void	TapeImage_Init(void);
void	TapeImage_PlayStatus(BOOL);

void	TZX_Write_Initialise(char *);
void	TZX_Write(unsigned long, unsigned long);
void	TZX_Write_End();

#endif
