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
#ifndef __CPC_RENDER_HEADER_INCLUDED__
#define __CPC_RENDER_HEADER_INCLUDED__

#include "cpcdefs.h"
#include "cpcglob.h"

/* NOTE: all graphics are rendered as if they are in mode 2 resolution */

/* pixels in mode 2 size. 
Used to define the position of each pixel for each graphical mode */
typedef struct  PIXEL_DATA
{
	unsigned int     Pixel[8];
} PIXEL_DATA;

/* these values take into account the horizontal and vertical retrace times, the actual
visible portion is somewhat smaller */
#define		NOPS_PER_LINE	64			/* NOP time for a single scan line */
#define		BYTES_PER_NOP	2			/* number of bytes in a NOP time unit */	
#define		LINES_PER_SCREEN	39*8	/* number of scan-lines on a monitor screen */
#define		BITS_PER_LINE	(NOPS_PER_LINE*BYTES_PER_NOP*8)	/* total number of bits on a line */
#define		NOPS_PER_FRAME	(NOPS_PER_LINE*LINES_PER_SCREEN)

#define		SCREEN_DATA_SIZE ((BITS_PER_LINE)*(LINES_PER_SCREEN+2))

void    CPC_BuildModeRenderTables(void);
PIXEL_DATA *CPC_GetModePixelData(int ModeIndex);
unsigned long *CPC_GetModePackedPixelData(int ModeIndex);

#define	X_CRTC_CHAR_OFFSET	8
#ifdef SIMPLE_MONITOR_EMULATION	
#define Y_CRTC_LINE_OFFSET	(4*8)
#else
#define Y_CRTC_LINE_OFFSET	0
#endif
#define X_CRTC_CHAR_WIDTH	48
#define Y_CRTC_LINE_HEIGHT	(34*8)

/* the following is used in paletted modes */

/* palette entry has been allocated a colour */
#define PALETTE_ENTRY_USED		0x0001

/* palette entry should not be removed. This is used in windows mode,
where some colours are reserved for drawing the window borders etc */
/* any reserved colours like this should have this bit set in the flags,
the colour information may then represent a index into a system palette,
or an actual colour */
#define PALETTE_ENTRY_DO_NOT_REMOVE	0x0002

#define PALETTE_ENTRY_DO_NOT_MATCH	0x0004

typedef struct PALETTE_ENTRY
{
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;
	unsigned char Flags;
} PALETTE_ENTRY;


typedef struct RGBCOLOUR
{
	union
	{
		struct 
		{
			unsigned char	Red;
			unsigned char	Green;
			unsigned char	Blue;
			unsigned char	pad0;
		} element;

		unsigned long PackedColour;

	} u;
} RGBCOLOUR;


typedef struct PALETTE_ENTRY_RGB888
{
	union
	{
		unsigned long PackedElements;

		struct RGBCOLOUR SeperateElements;
	} RGB;
} PALETTE_ENTRY_RGB888;

typedef struct RENDER_BUFFER_INFO
{
	unsigned char *pSurface;
	int		Width;
	int		Height;
	int		Pitch;
} RENDER_BUFFER_INFO;




void	Render_Initialise(void);
void	Render_DoScreen(void);
void	Render_ClearScreen(void);
void	Render_Flip(void);
void	Render_SetColourTable(RGBCOLOUR *pColours, int NoOfColours);

void	Render_SetPixelTranslation(int Mode);
void	Render_PutData(int HorizontalCount,unsigned long Data, int Offset,int InBorder,int,int);

void	Render_PutDataWord(int HorizontalCount,unsigned long GraphicsData, int Line);
void	Render_PutDataWordPLUS(int HorizontalCount,unsigned long GraphicsData,int Line);
void	Render_PutBorder(int HorizontalCount, int Line);

void	Render_DumpLine(int LineNum);

void	Render_SetColour(RGBCOLOUR *pColour,/*int,int,int,*/ int Index);

void	Render_DumpDisplay(void);

void	Render_PutPixel(int X, int Y, int Pixel);

void	Render_GetPixelRGBAtXY(int X,int Y, unsigned char *r, unsigned char *g, unsigned char *b);

void	Render_FirstLine(void);
void	Render_NextLine(void);

void	Render_SetHorizontalPixelScroll(int PixelScroll);

void    Render_PlotText(char *pString, int X, int Y);
void	Render_Put8Pixels(int X, int Y, unsigned long  PackedPixels);

void	Render_SetPixelPos(int X, int Y);
void	Render_PutPixelToPos(int Pixel);
void	Render_PutDataWordPLUSMask(int HorizontalCount,unsigned long GraphicsData, int Line, unsigned long Mask);

int	Render_SetDisplayWindowed(void);
int	Render_SetDisplayFullScreen(int, int,int);
void	Render_DumpLine(int);
void	Render_Finish(void);

void	Render_PutPixels(int X, int Y, int *pPixels, unsigned long Mask);
void	Render_PutDataWordPLUSMaskWithPixels(int HorizontalCount,unsigned long GraphicsData, int Line, unsigned long Mask, int *pPixels);
void	Render_ClearDisplay(void);
BOOL	Render_IsRendererActive(void);

void	Render_MarkPaletteEntryForHostUse(int Index);

/* TrueColour */
void Render_TrueColourRGB_PutSync(int,int);
void Render_TrueColourRGB_PutBorder(int,int);
void Render_TrueColourRGB_PutDataWord(int,unsigned long,int);
void    Render_TrueColourRGB_PutDataWordPLUS(int HorizontalCount,unsigned long GraphicsData, int Line, unsigned long Mask, int *pPixels);

void Render_Paletted_PutSync(int,int);
void Render_Paletted_PutBorder(int,int);
void Render_Paletted_PutDataWord(int,unsigned long,int);
void Render_Paletted_PutDataWordPLUS(int HorizontalCount,unsigned long GraphicsData, int Line, unsigned long Mask, int *pPixels);

#define RENDERING_ACCURACY_LOW  0x0001
#define RENDERING_ACCURACY_HIGH 0x0002
#define RENDERING_ACCURACY_HIGHER 0x0004
void    Render_SetRenderingAccuracyForWindowedMode(int Accuracy);
void    Render_TrueColourRGB_PutBorderCycles(int HorizontalCount, int Line, int Cycles);


#endif
