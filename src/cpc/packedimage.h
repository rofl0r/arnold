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
#ifndef __PACKED_IMAGE_HEADER_INCLUDED__
#define __PACKED_IMAGE_HEADER_INCLUDED__

typedef struct
{
        unsigned long                             BPP;                    /* Bits per pixel for this element */
        unsigned long   Mask;                   /* Mask */
        unsigned long                             Shift;                  /* Shift */
} GRAPHICS_ELEMENT_FORMAT;

typedef struct
{
        int BPP;                                                /* Bits Per Pixel in Resolution */
        GRAPHICS_ELEMENT_FORMAT Red;    /* Format info for Red */
        GRAPHICS_ELEMENT_FORMAT Green;  /* Format info for Green */
        GRAPHICS_ELEMENT_FORMAT Blue;   /* Format info for Blue */
        GRAPHICS_ELEMENT_FORMAT Alpha;  /* Format info for Alpha */
} GRAPHICS_FORMAT;

/* element in range 0..255 */
static INLINE unsigned long PackElementIntoDestinationImageFormat(unsigned
long Element, GRAPHICS_ELEMENT_FORMAT *pFormat)
{
        return  (((Element>>(8-pFormat->BPP))<<pFormat->Shift) & pFormat->Mask);
}

static INLINE unsigned long UnPackElementFromSourceImageFormat(unsigned
long PackedImageData, GRAPHICS_ELEMENT_FORMAT *pFormat)
{
        return ((PackedImageData & pFormat->Mask)>>pFormat->Shift)<<(8-pFormat->BPP);
}

/* writes out packed image data low byte first up to high byte */
static INLINE void WritePackedImageData(unsigned char *pDest, unsigned
long PackedImageData, int NoOfBytes)
{
        int Shift = 0;

        int i;

        for (i=0; i<NoOfBytes; i++)
        {
                pDest[i] = (unsigned char)(PackedImageData>>Shift);             

                Shift = Shift+8;
        }
}

static INLINE unsigned long ReadPackedImageData(unsigned char *pSource,
int NoOfBytes)
{
        unsigned long PackedImageData=0;
        int Shift = 0;
        int i;

        for (i=0; i<NoOfBytes; i++)
        {
                unsigned long   PackedImageByte;

                PackedImageByte = pSource[i] & 0x0ff;

                PackedImageData = PackedImageData | (PackedImageByte<<Shift);

                Shift = Shift + 8;
        }

        return PackedImageData;
}


/* r,g,b are in range 0..255 */
static INLINE unsigned long PackRGBIntoDestinationImageFormat(unsigned
long r, unsigned long g, unsigned long b, GRAPHICS_FORMAT *pFormat) 
{
        return (PackElementIntoDestinationImageFormat(r, &pFormat->Red) |
                        PackElementIntoDestinationImageFormat(g, &pFormat->Green) |
                        PackElementIntoDestinationImageFormat(b, &pFormat->Blue));
}

static INLINE void     GetRGBFromSourceImageFormat(unsigned long
PackedImageData, unsigned long *r, unsigned long *g, unsigned long *b, GRAPHICS_FORMAT *pFormat)
{
        *r = UnPackElementFromSourceImageFormat(PackedImageData, &pFormat->Red);
        *g = UnPackElementFromSourceImageFormat(PackedImageData, &pFormat->Green);
        *b = UnPackElementFromSourceImageFormat(PackedImageData, &pFormat->Blue);
}

static INLINE void CalcShiftAndBPPFromMask(unsigned long Mask, unsigned
long *BPP, unsigned long *Shift)
{
        unsigned long LocalShift = 0;
        unsigned long LocalBPP = 0;
        unsigned long LocalMask = Mask;

        if (LocalMask!=0)
        {
                while ((LocalMask & 0x01)==0)
                {
                        LocalMask >>= 1;
                        LocalShift++;
                }

                while ((LocalMask & 0x01)!=0)
                {
                
                        LocalMask >>= 1;
                        LocalBPP++;
                }
        }

        *Shift = LocalShift;
        *BPP = LocalBPP;
}


static INLINE void     BuildFormatInfoFromColourMasks(unsigned long
RedMask,
unsigned long GreenMask, unsigned long BlueMask, unsigned long AlphaMask, GRAPHICS_FORMAT *pFormat)
{
        int TotalBPP;
        int BPP;

        pFormat->Red.Mask = RedMask;
        CalcShiftAndBPPFromMask(RedMask, &pFormat->Red.BPP, &pFormat->Red.Shift);
        pFormat->Green.Mask = GreenMask;
        CalcShiftAndBPPFromMask(BlueMask, &pFormat->Blue.BPP, &pFormat->Blue.Shift);
        pFormat->Blue.Mask = BlueMask;
        CalcShiftAndBPPFromMask(GreenMask, &pFormat->Green.BPP, &pFormat->Green.Shift);
        pFormat->Alpha.Mask = AlphaMask;
        CalcShiftAndBPPFromMask(AlphaMask, &pFormat->Alpha.BPP, &pFormat->Alpha.Shift);
        
        TotalBPP = pFormat->Red.BPP + pFormat->Green.BPP + pFormat->Blue.BPP;

        BPP = 8;

        if (TotalBPP>8)
        {
                BPP = 16;
        }

        if (TotalBPP>16)
        {
                BPP = 24;
        }

        if (TotalBPP>32)
        {
                BPP = 32;
        }

        pFormat->BPP = BPP;
}


#endif
