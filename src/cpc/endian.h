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
#ifndef __ENDIAN_HEADER_INCLUDED__
#define __ENDIAN_HEADER_INCLUDED__

/* swap endian */
static unsigned long SwapEndianLong(unsigned long Data)
{
	unsigned char A,B,C,D;

	A = (unsigned char)((Data>>24) & 0x0ff);
	B = (unsigned char)((Data>>16) & 0x0ff);
	C = (unsigned char)((Data>>8) & 0x0ff);
	D = (unsigned char)(Data & 0x0ff);

	return (unsigned long)(A | (B<<8) | (C<<16) | (D<<24));
}

static unsigned short SwapEndianWord(unsigned short Data)
{
	unsigned char A,B;

	A = (unsigned char)((Data>>8) & 0x0ff);
	B = (unsigned char)(Data & 0x0ff);
	
	return (unsigned short)((unsigned short)A | ((unsigned short)B<<8));
}

#endif
