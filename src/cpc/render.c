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
#include "render.h"

static PIXEL_DATA      Mode0PixelData[256];
static PIXEL_DATA      Mode1PixelData[256];
static PIXEL_DATA      Mode2PixelData[256];
static PIXEL_DATA          Mode3PixelData[256];

static unsigned		Mode0PackedPixels[256];
static unsigned 		Mode1PackedPixels[256];
static unsigned 		Mode2PackedPixels[256];
static unsigned		Mode3PackedPixels[256];

unsigned		Mode0PenIndex[256];
unsigned		Mode1PenIndex[256];
unsigned		Mode2PenIndex[256];
unsigned		Mode3PenIndex[256];

#define PACK8_PIXELS(a,b,c,d,e,f,g,h)	((a<<0) | (b<<4) | (c<<8) | (d<<12) | (e<<16) | (f<<20) | (g<<24) | (h<<28))

unsigned *CPC_GetModePenIndexTable(int ModeIndex)
{
    switch (ModeIndex)
    {
            case 0:
            {
                    return &Mode0PenIndex[0];
            }
            break;

            case 1:
            {
                    return &Mode1PenIndex[0];
            }
            break;

            case 2:
            {
                    return &Mode2PenIndex[0];
            }
            break;

            case 3:
            {
                    return &Mode3PenIndex[0];
            }
            break;
    }

    return &Mode0PenIndex[0];
}


PIXEL_DATA *CPC_GetModePixelData(int ModeIndex)
{
        switch (ModeIndex)
        {
                case 0:
                {
                        return &Mode0PixelData[0];
                }
                break;

                case 1:
                {
                        return &Mode1PixelData[0];
                }
                break;

                case 2:
                {
                        return &Mode2PixelData[0];
                }
                break;

                case 3:
                {
                        return &Mode3PixelData[0];
                }
                break;
        }

        return &Mode0PixelData[0];
}


unsigned *CPC_GetModePackedPixelData(int ModeIndex)
{
        switch (ModeIndex)
        {
                case 0:
                {
                        return &Mode0PackedPixels[0];
                }
                break;

                case 1:
                {
                        return &Mode1PackedPixels[0];
                }
                break;

                case 2:
                {
                        return &Mode2PackedPixels[0];
                }
                break;

                case 3:
                {
                        return &Mode3PackedPixels[0];
                }
                break;
        }

        return &Mode0PackedPixels[0];
}


void    CPC_BuildModeRenderTables(void)
{
    int i;

    /* mode 0 */
    for (i=0; i<256; i++)
    {
        int Pixel1;
        int Pixel2;
      

        Pixel1 = ((i & 0x02)<<2) | ((i & 0x020)>>(5-2)) | ((i & 0x08)>>(3-1)) | ((i & 0x080)>>(7-0));

        Pixel2 = ((i & 0x01)<<3) | ((i & 0x010)>>(4-2)) | ((i & 0x04)>>(2-1)) | ((i & 0x040)>>(6-0));

        Mode0PixelData[i].Pixel[0] = Pixel1;  
        Mode0PixelData[i].Pixel[1] = Pixel1;
        Mode0PixelData[i].Pixel[2] = Pixel1;
        Mode0PixelData[i].Pixel[3] = Pixel1;
        Mode0PixelData[i].Pixel[4] = Pixel2;
        Mode0PixelData[i].Pixel[5] = Pixel2;
        Mode0PixelData[i].Pixel[6] = Pixel2;
        Mode0PixelData[i].Pixel[7] = Pixel2;

		Mode0PackedPixels[i] = PACK8_PIXELS(Pixel1,Pixel1,Pixel1,Pixel1,Pixel2,Pixel2,Pixel2,Pixel2);

		Mode0PenIndex[i] = Pixel1;
    }

    /* mode 1 */
    for (i=0; i<256; i++)
    {
        int Pixel1;
        int Pixel2;
        int Pixel3;
        int Pixel4;

        Pixel1 = ((i & 0x08)>>(3-1)) | ((i & 0x080)>>7);
        Pixel2 = ((i & 0x04)>>(2-1)) | ((i & 0x040)>>6);
        Pixel3 = ((i & 0x02)>>(1-1)) | ((i & 0x020)>>5);
        Pixel4 = ((i & 0x01)<<1) | ((i & 0x010)>>4);

        Mode1PixelData[i].Pixel[0] = Pixel1;  
        Mode1PixelData[i].Pixel[1] = Pixel1;
        Mode1PixelData[i].Pixel[2] = Pixel2;
        Mode1PixelData[i].Pixel[3] = Pixel2;
        Mode1PixelData[i].Pixel[4] = Pixel3;
        Mode1PixelData[i].Pixel[5] = Pixel3;
        Mode1PixelData[i].Pixel[6] = Pixel4;
        Mode1PixelData[i].Pixel[7] = Pixel4;

		
		Mode1PackedPixels[i] = PACK8_PIXELS(Pixel1,Pixel1,Pixel2,Pixel2,Pixel3,Pixel3,Pixel4,Pixel4);
    
		Mode1PenIndex[i] = Pixel1;
	}

    
    /* mode 2 */
    for (i=0; i<256; i++)
    {
        int Pixel1,Pixel2,Pixel3,Pixel4,Pixel5,Pixel6,Pixel7,Pixel8;
        
        Pixel1 = (i & 0x080)>>7;
        Pixel2 = (i & 0x040)>>6;
        Pixel3 = (i & 0x020)>>5;
        Pixel4 = (i & 0x010)>>4;
        Pixel5 = (i & 0x008)>>3;
        Pixel6 = (i & 0x004)>>2;
        Pixel7 = (i & 0x002)>>1;
        Pixel8 = (i & 0x001)>>0;

        Mode2PixelData[i].Pixel[0] = Pixel1;  
        Mode2PixelData[i].Pixel[1] = Pixel2;
        Mode2PixelData[i].Pixel[2] = Pixel3;
        Mode2PixelData[i].Pixel[3] = Pixel4;
        Mode2PixelData[i].Pixel[4] = Pixel5;
        Mode2PixelData[i].Pixel[5] = Pixel6;
        Mode2PixelData[i].Pixel[6] = Pixel7;
        Mode2PixelData[i].Pixel[7] = Pixel8;

		
		Mode2PackedPixels[i] = PACK8_PIXELS(Pixel1,Pixel2,Pixel3,Pixel4,Pixel5,Pixel6,Pixel7,Pixel8);
   
		Mode2PenIndex[i] = Pixel1;
	}

        /* mode 3 */
        for (i=0; i<256; i++)
        {
                int Pixel1,Pixel2;

        Pixel1 = ((i & 0x08)>>(3-1)) | ((i & 0x080)>>7);
        Pixel2 = ((i & 0x04)>>(2-1)) | ((i & 0x040)>>6);

                Mode3PixelData[i].Pixel[0] = Pixel1;
                Mode3PixelData[i].Pixel[1] = Pixel1;
                Mode3PixelData[i].Pixel[2] = Pixel1;
                Mode3PixelData[i].Pixel[3] = Pixel1;
                Mode3PixelData[i].Pixel[4] = Pixel2;
                Mode3PixelData[i].Pixel[5] = Pixel2;
                Mode3PixelData[i].Pixel[6] = Pixel2;
                Mode3PixelData[i].Pixel[7] = Pixel2;
        
		
		Mode3PackedPixels[i] = PACK8_PIXELS(Pixel1,Pixel1,Pixel1,Pixel1,Pixel2,Pixel2,Pixel2,Pixel2);

		Mode3PenIndex[i] = Pixel1;
		}


}
