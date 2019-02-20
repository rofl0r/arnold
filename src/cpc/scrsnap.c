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
#include "render.h"
#include "bmp.h"
#include "cpcglob.h"

extern int PIXEL_STEP_SHIFT;

void	Screen_SaveSnapshot(char *pFilename)
{
	int ScreenWidth = (X_CRTC_CHAR_WIDTH<<(1+3));
	int ScreenHeight = Y_CRTC_LINE_HEIGHT;
	int x,y;
	BMP_HEADER *pBitmap;
	
	ScreenWidth = ScreenWidth>>PIXEL_STEP_SHIFT;

	/* create a bitmap to hold the screen image */
	pBitmap = BitmapUtil_New(24, ScreenWidth, ScreenHeight);

	if (pBitmap!=NULL)
	{
		/* copy data from stored screen buffer to bitmap */
		for (y=0; y<ScreenHeight; y++)
		{
			for (x=0; x<ScreenWidth; x++)
			{
				unsigned char r,g,b;

				/* get pixel from graphics buffer */
				Render_GetPixelRGBAtXY(x,y, &r, &g, &b);
		
				/* write to bitmap */
				BitmapUtil_WritePixelRGB(pBitmap, x, y, r,g,b);

			}
		}

		/* write bitmap image file to disk */
		BitmapUtil_Write(pBitmap,pFilename);

		/* free temporary memory allocated for screen image */
		BitmapUtil_Delete(pBitmap);
	}
}
