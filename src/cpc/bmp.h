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
#ifndef __BMP_UTIL_HEADER_INCLUDED__
#define __BMP_UTIL_HEADER_INCLUDED__

#ifdef _WINDOWS
#pragma pack(1)
#endif

typedef struct BMP_RGBQUAD
{
	unsigned char Blue;
	unsigned char Green;
	unsigned char Red;
	unsigned char Reserved;
} BMP_RGBQUAD;

typedef struct BMP_HEADER
{
	unsigned short	Type;
	unsigned long	Size;
	unsigned short	Reserved1;
	unsigned short	Reserved2;
	unsigned long	OffsetBits;
} BMP_HEADER;

typedef struct BMP_INFO_HEADER
{
	unsigned long Size;
	unsigned long Width;
	signed long Height;
	unsigned short Planes;
	unsigned short BitCount;
	unsigned long Compression;
	unsigned long SizeImage;
	unsigned long XPelsPerMeter;
	unsigned long YPelsPerMeter;
	unsigned long ColoursUsed;
	unsigned long ColoursImportant;
} BMP_INFO_HEADER;

#ifdef _WINDOWS
#pragma pack()
#endif

BMP_HEADER *BitmapUtil_New(int Depth, int Width, int Height);
void	BitmapUtil_Delete(BMP_HEADER *);
void	BitmapUtil_Write(BMP_HEADER *, char *);
void	BitmapUtil_WritePixelRGB(BMP_HEADER *pBMP, unsigned int X, unsigned int Y, unsigned char r, unsigned char g, unsigned char b);


#endif
