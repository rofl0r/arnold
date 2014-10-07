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
#include "scrsnap.h"
#include "../cpc/render.h"
#include "../cpc/bmp.h"
#include "../cpc/cpcglob.h"

/* PNG support added by Troels K. 2003 
- libpng Copyright (c) 1998-2002 Glenn Randers-Pehrson
- zlib   Copyright (C) 1995-1998 Jean-loup Gailly and Mark Adler
*/

extern int PIXEL_STEP_SHIFT;
extern int ScanLines;
extern int FillScanLines;

void	Screen_SaveSnapshot(const TCHAR *pFilename)
{
	int ScreenWidth = (X_CRTC_CHAR_WIDTH<<(1+3));
	int ScreenHeight = Y_CRTC_LINE_HEIGHT;
	int x,y;
	BMP_HEADER *pBitmap;
	unsigned long BmpFileSize;
	
	ScreenWidth = ScreenWidth>>PIXEL_STEP_SHIFT;
	
	if ((ScanLines) || (FillScanLines))
	{
		ScreenHeight = ScreenHeight<<1;
	}

	BmpFileSize = BMP_CalculateAllocSize(ScreenWidth, ScreenHeight);
	
	pBitmap = (BMP_HEADER *)malloc(BmpFileSize);

	if (pBitmap)
	{
		BMP_Setup(pBitmap, ScreenWidth, ScreenHeight);

		/* copy data from stored screen buffer to bitmap */
		for (y=0; y<ScreenHeight; y++)
		{
			if ((ScanLines) && (!FillScanLines) && ((y & 1)!=0))
			{
				for (x=0; x<ScreenWidth; x++)
				{
					BitmapUtil_WritePixelRGB(pBitmap, x, y, 0,0,0);
				}
			}
			else
			{
				for (x=0; x<ScreenWidth; x++)
				{
					unsigned char r,g,b;

					/* get pixel from graphics buffer */
					if ((FillScanLines) || (ScanLines))
					{
						Render_GetPixelRGBAtXY(x,(y>>1), &r, &g, &b);
					}
					else
					{
						Render_GetPixelRGBAtXY(x,y, &r, &g, &b);
					}

					/* write to bitmap */
					BitmapUtil_WritePixelRGB(pBitmap, x, y, r,g,b);

				}
			}

		}

		BMP_SetupForSave(pBitmap);

		SaveFile(pFilename, pBitmap, BmpFileSize);
		
		free(pBitmap);
	}
}
