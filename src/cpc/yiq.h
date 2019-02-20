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
#ifndef __YIQ_HEADER_INCLUDED__
#define __YIQ_HEADER_INCLUDED__


typedef struct RGB_CHAR
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char pad0;
} RGB_CHAR;

void	BrightnessControl_GenerateGreyScaleFromColour(RGB_CHAR *pSourceRGB, RGB_CHAR *pDestRGB);
void	BrightnessControl_AdjustBrightness(RGB_CHAR *pSourceRGB, RGB_CHAR *pDestRGB, int Brightness);
void	BrightnessControl_Initialise(void);


#endif
