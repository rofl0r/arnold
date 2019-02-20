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
#ifndef __CRTC_HEADER_INCLUDED__
#define __CRTC_HEADER_INCLUDED__

#include "cpcglob.h"

#define RENDER_MODE_STANDARD		0x001
#define RENDER_MODE_ASIC_FEATURES	0x002

#define CRTC_VS_FLAG	0x001	/* Vsync active */	
#define CRTC_HS_FLAG	0x002	/* Hsync active */
#define CRTC_HDISP_FLAG	0x004	/* Horizontal Display Timing */
#define CRTC_VDISP_FLAG	0x008	/* Vertical Display Timing */
#define CRTC_HTOT_FLAG	0x010	/* HTot reached */
#define CRTC_VTOT_FLAG	0x020	/* VTot reached */
#define CRTC_MR_FLAG	0x040	/* Max Raster reached */
#define CRTC_VADJ_FLAG	0x080
#define CRTC_R8DT_FLAG	0x100
#define CRTC_VSCNT_FLAG	0x200
#define CRTC_HSCNT_FLAG 0x400

#define GA_HSYNC_FLAG	0x0001
#define GA_VSYNC_FLAG	0x0002
#define GA_INT_SYNC_FLAG	0x0003

typedef struct
{
	unsigned char GA_Flags;
	unsigned char GA_HSyncWidth;
	unsigned char  GA_HSyncCount;
	unsigned char GA_VSyncWidth;
	unsigned char GA_VSyncCounter;
	
	unsigned long	CRTC_PreviousVsyncState;

} GA_INTERNAL_STATE;


#define MONITOR_IN_VSYNC	0x0001
#define MONITOR_VSYNC_SYNC	0x0002
#define MONITOR_IN_HSYNC	0x0003

typedef struct
{
	/* number of scan-lines after start of VSYNC */
	int LinesAfterVsync;
	/* number of chars after start of HSYNC */
	int CharsAfterHsync;
	int				MonitorScanLineCount;

	int				VSyncRetraceCount;
	int				HSyncRetraceCount;

	/* these are used to render black on the screen, if a sync
	falls within screen area */
	unsigned char MonitorVsyncCount;
	/* "monitor" counts */
	unsigned char MonitorFlags;
	unsigned char pad;
	unsigned char MonitorHorizontalCount;
} MONITOR_INTERNAL_STATE;

/* used by the code */
typedef struct
{
	unsigned long CRTC_Flags;
	unsigned long CRTC_HalfHtotFlags;
	unsigned long CRTC_FlagsAtLastHsync;
	unsigned long CRTC_FlagsAtLastHtot;
	/* horizontal count */
	unsigned char HCount;
	/* start and end of line in char positions */
	unsigned char HStart, HEnd;
	/* Horizontal sync width */
	unsigned char HorizontalSyncWidth;
	/* horizontal sync width counter */
	unsigned char HorizontalSyncCount;

	/* raster counter (RA) */
	unsigned char RasterCounter;
	/* line counter */
	unsigned char LineCounter;
	/* Vertical sync width */
	unsigned char VerticalSyncWidth;
	/* vertical sync width counter */
	unsigned char VerticalSyncCount;

	unsigned char CRTC_HSyncTable[16];
	unsigned char GA_HsyncTable[16];
	unsigned char CRTC_WriteMaskTable[32];
	unsigned char CRTC_ReadMaskTable[32];

	/* INTERLACE STUFF */
	/* interlace and video mode number 0,1,2,3 */
	unsigned char InterlaceAndVideoMode;
	/* frame - odd or even - used in interlace */
	unsigned char Frame;
	/* Vert Adjust Counter */
	unsigned char VertAdjustCount;
		/* delay for start and end of line defined by reg 8 */
	unsigned char HDelayReg8;



	unsigned long CRTC_RenderType;
	/* type index of CRTC */
	unsigned char CRTC_Type;
	/* index of current register selected */
	unsigned char CRTC_Reg;
	unsigned short HDispAdd;



	unsigned long Reg9Mask;
	unsigned long RasterMA;
	/* MA (memory address base) */
	int MA;
	/* MA of current line we are rendering (character line) */
	int MALine;
	unsigned long MAStore;
	/* render graphics */
	void (*pCRTC_RenderGraphicsFunction)(void);
	/* rendering function */
	void (*pCRTC_RenderFunction)(void);
	void (*pCRTC_RenderSync)(void);
	void (*pCRTC_RenderBorder)(void);

	void (*pCRTC_GetGraphicsDataCPC)(void);
	void (*pCRTC_GetGraphicsDataPLUS)(void);

	/* line function */
	void (*pCRTC_DoLine)(void);
  void (*pCRTC_UpdateState)(void);
	MONITOR_INTERNAL_STATE Monitor_State;
	GA_INTERNAL_STATE GA_State;
	BOOL DontRender;
	unsigned long GraphicsLong;
	int Pixels[16];

} CRTC_INTERNAL_STATE;



void CRTC_DoCycle();
void CRTC_DoCycles(int);

void	CRTC_SetMA(int NewMA);
void	CRTC_AcknowledgeInterrupt();
void	CRTC_ClearInterrupt();

void	CRTC_SetRenderFunction(int RenderMode);
void	CRTC_SetRenderFunctionPLUS(unsigned long SpriteMask);



void	CRTC_SetDebug(BOOL);
void	CRTC_SetCPCMode(int NewCPCMode);

/*---------------------------------------------------------------------------*/
/* reset CRTC */
void	CRTC_Reset(void);

/* select CRTC type emulated */
void	CRTC_SetType(unsigned int);

/* select register */
void	CRTC_RegisterSelect(unsigned int);

/* read data from selected register */
unsigned char CRTC_ReadData(void);

/* write data to selected register */
void	CRTC_WriteData(unsigned int);

/* read status register */
unsigned int CRTC_ReadStatusRegister(void);

/* get selected CRTC register - for snapshot or multiface */
int		CRTC_GetSelectedRegister(void);
/* get register data - for Snapshot or Multiface */
unsigned char		CRTC_GetRegisterData(int RegisterIndex);

CRTC_INTERNAL_STATE *CRTC_GetInternalState(void);


void	ASICCRTC_SetSoftScroll(int SoftScroll);
void	CRTC_Initialise();
void	ASICCRTC_ScreenSplit(void);
void	CRTC_SetRenderState(BOOL State);

void	CRTC_SetTrueColourRender(BOOL State);

enum
{
	CRTC_RENDER_GRAPHICS = 1,
	CRTC_RENDER_BORDER,
	CRTC_RENDER_SYNC
} CRTC_RENDER_TYPE;

#define CRTC_RENDER_STAGE_START_NEW_LINE 0x0001
#define CRTC_RENDER_STAGE_START_NEW_FRAME 0x0002
typedef struct
{
	unsigned long MA;
	unsigned long Count;
	unsigned long RenderType;
	unsigned long Flags;
} CRTC_RENDER_STAGE;


void	CRTC_LightPen_Trigger(unsigned long X, unsigned long Y);
void	CRTC_LightPen_Update(unsigned long Cycles);

#endif
