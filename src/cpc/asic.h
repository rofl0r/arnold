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
#ifndef __ASIC_HEADER_INCLUDED__
#define __ASIC_HEADER_INCLUDED__

#include <stdio.h>
#include <stdlib.h>
#include "cpcglob.h"    
#include "garray.h"
#include "cpcglob.h"
#include "cpcdefs.h"

typedef struct 
{
        BOOL    PauseActive;
        int             PauseCount;                     /* pause current count */
        int             PrescaleCount;                  /* channel prescalar current count */
        int             LoopStart;                      /* reload address for loop */
        int             RepeatCount;            /* number of times to repeat the loop */
} ASIC_DMA_CHANNEL;

BOOL    ASIC_Initialise(void);
void    ASIC_Finish(void);

void    ASIC_InitCart(void);
void    ASIC_Reset(void);
void    ASIC_EnableDisable(int);
int		ASIC_DMA_GetChannelAddr(int);
int		ASIC_DMA_GetChannelPrescale(int);
unsigned char   *ASIC_GetCartPage(int);
void    ASIC_SetRasterInterrupt(void);
void    ASIC_ClearRasterInterrupt(void);

/*char    *ASIC_DebugDMACommand(int,int); */

unsigned char *ASIC_GetRamPtr(void);


unsigned long ASIC_BuildDisplayReturnMaskWithPixels(int Line, int HCount, /*int MonitorHorizontalCount, int ActualY,*/ int *pPixels);

void    ASIC_DoDMA(void);

void    ASIC_HSync(int,int);

/* asic functions to be executed when Htot reached */
void    ASIC_HTot(int);

int             ASIC_CalculateInterruptVector(void);

/* get lock state of ASIC (features locked/unlocked) for snapshot */
BOOL	ASIC_GetUnLockState(void);
/* set lock state of ASIC (features locked/unlocked) for snapshot */
void	ASIC_SetUnLockState(BOOL);

void	ASIC_SetSecondaryRomMapping(unsigned char Data);
int ASIC_GetSecondaryRomMapping(void);

/* reset gate array in ASIC */
void    ASIC_GateArray_Reset(void);

/* trap writes to asic ram */
void    ASIC_WriteRam(int Addr,int Data);

/* used when setting up ASIC in reset or from snapshots */
void	ASIC_WriteRamFull(int Addr, int Data);

BOOL    Cartridge_Load(char *);
void    Cartridge_Remove(void);

BOOL    ASIC_RasterIntEnabled(void);

void    ASIC_DoRomSelection(void);
void    ASIC_AcknowledgeInterrupt(void);

void    ASIC_DMA_EnableChannels(unsigned char);

/* debugger */
unsigned char   ASIC_GetRed(int);
unsigned char ASIC_GetGreen(int);
unsigned char ASIC_GetBlue(int);
unsigned char ASIC_GetSpritePixel(int SpriteIndex, int X, int Y);

typedef struct 
{
        unsigned int    XMagShift,YMagShift;
        signed short    SpriteMinXPixel, SpriteMinYPixel;
        signed short    SpriteMaxXPixel, SpriteMaxYPixel;
        signed short    SpriteXColumnMin, SpriteXColumnMax;
		signed short	pad[2];
} ASIC_SPRITE_RENDER_INFO;

#define ASIC_RAM_ENABLED	0x0002
#define ASIC_ENABLED		0x0001

/* this structure represents what is stored in internal ASIC registers */
typedef struct 
{
        union
        {
                signed short    SpriteX_W;

#ifdef CPC_LSB_FIRST        

                struct 
                {
                        unsigned char l;
                        signed char h;
                } SpriteX_B;
#else
	  struct
	  {
	    signed char h;
	    unsigned char l;
	  } SpriteX_B;
#endif

        } SpriteX;

        union
        {
                signed short    SpriteY_W;
#ifdef CPC_LSB_FIRST
                struct
                {
                        unsigned char l;
                        signed char h;
                } SpriteY_B;
#else
	    struct
	    {
	      signed char h;
	      unsigned char l;
	    } SpriteY_B;
#endif

      } SpriteY;
        
        unsigned char   SpriteMag;

		unsigned char	pad[3];
} ASIC_SPRITE_INFO;

typedef struct 
{
        union
        {
                unsigned short Addr_W;
#ifdef CPC_LSB_FIRST
                struct
                {
                        unsigned char l;
                        unsigned char h;
                } Addr_B;
#else
	  struct
	  {
	    unsigned char h;
	    unsigned char l;
	  } Addr_B;
#endif

        } Addr;

        unsigned char Prescale;
		unsigned char pad;
} ASIC_DMA_INFO;

typedef struct 
{
	/* status flags */
	unsigned long	Flags;
	/* pointer to asic ram */
	unsigned char    *ASIC_Ram;
	/* pointer to asic ram for "re-thinking memory" */
	unsigned char	 *ASIC_Ram_Adjusted;
	/* a mask used for memory paging */
	unsigned long	ASIC_RamMask;

	/* SPRITES */
	unsigned long SpriteEnableMask;
	unsigned long SpriteEnableMaskOnLine;
	ASIC_SPRITE_INFO Sprites[16];
	ASIC_SPRITE_RENDER_INFO SpriteInfo[16];

	/* DMA */
	unsigned long	DMAPauseActive;
	ASIC_DMA_INFO    DMA[3];
	ASIC_DMA_CHANNEL DMAChannel[3];

	/* interrupt vector */
	unsigned char ASIC_InterruptVector;
	/* raster interrupt line */
	unsigned char ASIC_RasterInterruptLine;
	/* soft scroll */
	unsigned char ASIC_SoftScroll;
	/* raster split line */
	unsigned char ASIC_RasterSplitLine;

	/* Secondary Screen Address */
    union
    {
        unsigned short Addr_W;
#ifdef CPC_LSB_FIRST
        struct
        {
                unsigned char l;
                unsigned char h;
        } Addr_B;
#else
	  struct
	  {
	    unsigned char h;
	    unsigned char l;
	  } Addr_B;
#endif
    } ASIC_SecondaryScreenAddress;

	/* bit 7 = 1 if raster interrupt triggered */
	/* bit 6 = 1 if DMA channel 0 interrupt triggered */
	/* bit 5 = 1 if DMA channel 1 interrupt triggered */
	/* bit 4 = 1 if DMA channel 2 interrupt triggered */ 
	unsigned char InternalDCSR;
	unsigned char ASIC_DCSR2;

	unsigned char SecondaryRomMapping;

	unsigned char AnalogueInputs[8];

} ASIC_DATA;

BOOL    ASIC_SpritesActive(void);

void ASIC_GenerateSpriteActiveMaskForLine(int Line);

BOOL    ASIC_RasterSplitLineMatch(int,int);

void    ASIC_SetMonitorColourMode(MONITOR_COLOUR_MODE Mode);
void    ASIC_InitialiseMonitorColourModes(void);

void    ASIC_SetDebug(BOOL);
void	ASIC_UpdateColours(void);

unsigned char ASIC_GetDCSR(void);
unsigned char ASIC_GetPRI(void);
unsigned char ASIC_GetSPLT(void);
unsigned long ASIC_GetSSA(void);
unsigned char ASIC_GetSSCR(void);
unsigned char ASIC_GetIVR(void);

void	ASIC_SetPRI(unsigned char);
void	ASIC_SetSPLT(unsigned char);
void	ASIC_SetSSA(unsigned long);
void	ASIC_SetSSCR(unsigned char);
void	ASIC_SetIVR(unsigned char);

unsigned short ASIC_GetSpriteX(int SpriteIndex);
unsigned short ASIC_GetSpriteY(int SpriteIndex);
unsigned char ASIC_GetSpriteMagnification(int SpriteIndex);

/* update palette ram */
void	ASIC_GateArray_UpdatePaletteRam(unsigned char PaletteIndex, unsigned char Colour);

/* set secondary rom mapping? */
BOOL	ASIC_GateArray_CheckForSecondaryRomMapping(unsigned char Function);

/* initialise Expansion ROM Table with cartridge pages */
void	ASIC_RefreshExpansionROMTable(unsigned char **pExpansionROMTable);

void	ASIC_GateArray_RethinkMemory(unsigned char **pReadRamPtr, unsigned char **pWriteRamPtr);
void	ASIC_SetAnalogueInput(int InputID, unsigned char Data);
unsigned char ASIC_GetAnalogueInput(int InputID);

#endif
