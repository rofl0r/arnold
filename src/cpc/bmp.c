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
/* BMP creator */
#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "cpcglob.h"
#include "endian.h"
#include "cpcdefs.h"
#include "host.h"
#include <string.h>

BMP_HEADER *BitmapUtil_New(int Depth, int Width, int Height)
{
        BMP_HEADER *pBMP;
        BMP_INFO_HEADER *pBMPInfo;
        int     LinePitch;
        int ImageSize;
        int BmpFileSize;

        /* pitch is width of image * number of bytes per pixel + padding
        to make line length divisible by 4 */
        LinePitch = Width*(Depth>>3);
        LinePitch += ((4-(LinePitch & 0x03)) & 0x03);

        /* calculate size of image data */
        ImageSize = LinePitch * Height;

        /* calculate size of bmp file - including headers and image data */
        BmpFileSize = sizeof(BMP_HEADER) + sizeof(BMP_INFO_HEADER) + ImageSize;
        
        if (Depth==8)
        {
                BmpFileSize += sizeof(BMP_RGBQUAD)*256;
        }

        /* allocate memory for bitmap - headers, colour table and image data */
        pBMP = (BMP_HEADER *)malloc(BmpFileSize);

        if (pBMP!=NULL)
        {
                unsigned char *pID;

                memset(pBMP, 0, BmpFileSize);

                pBMPInfo = (BMP_INFO_HEADER *)((unsigned char *)pBMP + sizeof(BMP_HEADER));
                
                pID = (unsigned char *)pBMP;

                /* initialise main header */
                pID[0] = 'B';
                pID[1] = 'M';
        
                pBMP->Size = BmpFileSize;
                pBMP->Reserved1 = 0;
                pBMP->Reserved2 = 0;
                pBMP->OffsetBits = sizeof(BMP_HEADER) + sizeof(BMP_INFO_HEADER);

                if (Depth==8)
                {
                        pBMP->OffsetBits += sizeof(BMP_RGBQUAD)*256;
                }


                /* initialise info header */
                pBMPInfo->Size = sizeof(BMP_INFO_HEADER);
                pBMPInfo->Width = Width;
                pBMPInfo->Height = Height;
                pBMPInfo->Planes = 1;
                pBMPInfo->BitCount = (unsigned short)Depth;
                pBMPInfo->SizeImage = ImageSize;
                pBMPInfo->XPelsPerMeter = 1280;
                pBMPInfo->YPelsPerMeter = 1024;

        }

        return pBMP;
}

void    BitmapUtil_Write(BMP_HEADER *pBMP, char *pFilename)
{
#ifndef CPC_LSB_FIRST
                /* big endian stuff - swap before writing data */
                BMP_INFO_HEADER *pBMPInfo = (BMP_INFO_HEADER *)((unsigned char *)pBMP + sizeof(BMP_HEADER));
                
                /* big endian conversion for bmp header */
                pBMP->Size = SwapEndianLong(pBMP->Size);
                pBMP->OffsetBits = SwapEndianLong(pBMP->OffsetBits);

                /* big endian conversion for info header */
                pBMPInfo->Size = SwapEndianLong(pBMPInfo->Size);
                pBMPInfo->Width = SwapEndianLong(pBMPInfo->Width);
                pBMPInfo->Height = SwapEndianLong(pBMPInfo->Height);
                pBMPInfo->Planes = SwapEndianWord(pBMPInfo->Planes);
                pBMPInfo->BitCount = SwapEndianWord(pBMPInfo->BitCount);
                pBMPInfo->SizeImage  = SwapEndianLong(pBMPInfo->SizeImage);
                pBMPInfo->XPelsPerMeter = SwapEndianLong(pBMPInfo->XPelsPerMeter);
                pBMPInfo->YPelsPerMeter = SwapEndianLong(pBMPInfo->YPelsPerMeter);
#endif


        /* write bitmap to disc */
        Host_SaveFile(pFilename, (void *)pBMP, pBMP->Size);

#ifndef CPC_LSB_FIRST

                /* Swap back again */

                /* big endian conversion to swap back bmp header */
                pBMP->Size = SwapEndianLong(pBMP->Size);
                pBMP->OffsetBits = SwapEndianLong(pBMP->OffsetBits);

                /* big endian conversion to swap back info header*/
                pBMPInfo->Size = SwapEndianLong(pBMPInfo->Size);
                pBMPInfo->Width = SwapEndianLong(pBMPInfo->Width);
                pBMPInfo->Height = SwapEndianLong(pBMPInfo->Height);
                pBMPInfo->Planes = SwapEndianWord(pBMPInfo->Planes);
                pBMPInfo->BitCount = SwapEndianWord(pBMPInfo->BitCount);
                pBMPInfo->SizeImage  = SwapEndianLong(pBMPInfo->SizeImage);
                pBMPInfo->XPelsPerMeter = SwapEndianLong(pBMPInfo->XPelsPerMeter);
                pBMPInfo->YPelsPerMeter = SwapEndianLong(pBMPInfo->YPelsPerMeter);
#endif


}

void    BitmapUtil_Delete(BMP_HEADER *pBMP)
{
        /* free memory used by a bitmap created with BitmapUtil_New */
        if (pBMP!=NULL)
        {
                free(pBMP);
        }
}

/* write a RGB pixel to a RGB based Bitmap */
void    BitmapUtil_WritePixelRGB(BMP_HEADER *pBMP, unsigned int X, unsigned int Y, unsigned char r, unsigned char g, unsigned char b)
{
        unsigned char *pPixel; 
        BMP_INFO_HEADER *pBMPInfo;
        int LinePitch;
        
        pBMPInfo = (BMP_INFO_HEADER *)((unsigned char *)pBMP + sizeof(BMP_HEADER));

        if ((X<pBMPInfo->Width) && (Y<(unsigned long)(pBMPInfo->Height & 0x07fffffff)))
        {
                LinePitch = pBMPInfo->Width*(pBMPInfo->BitCount>>3);
                LinePitch += (4-(LinePitch & 0x03)) & 0x03;
                
                pPixel = (unsigned char *)((unsigned char *)pBMP + pBMP->OffsetBits + (X*(pBMPInfo->BitCount>>3)));

                if (pBMPInfo->Height>0)
                {
                        pPixel += ((pBMPInfo->Height)-1-Y)*LinePitch;
                }
                else
                {
                        pPixel += Y*LinePitch;
                }

                
                switch (pBMPInfo->BitCount)
                {
                        case 24:
                        {
                                pPixel[0] = b;
                                pPixel[1] = g;
                                pPixel[2] = r;
                        }
                        break;
                }
        }
}
