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
 /*
        CPC+ ASIC CODE


  1. DCSR bit 7 set for normal ints.


  2. Analogue channels report 0x03f when read if nothing attached.
  (From demo by Simon Matthews)

  3. When colours are written to ASIC ram, the data is not latched.
         The effect we see is RB being loaded followed by G being loaded.
         One NOP is required for each.

         So changing colour from Black to white and back again 
         we see: Magenta, white...., green


  4. ASIC ints:

  addr + 0: Specifies DMA channel  or non-raster line ints

*/

/* 9:3:1 summing, green:red:blue */

#include "asic.h"
#include "render.h"
#include "cpcglob.h"    
#include "crtc.h"
#include "multface.h"
#include "cpc.h"
#include "host.h"
#include "yiq.h"
#include "z80/z80.h"
#include "cpcdefs.h"
#include "endian.h"
#include "riff.h"


static ASIC_DATA ASIC_Data;

static void    ASIC_TriggerInterrupt(void);
void    ASIC_DMA_DisableChannels(unsigned char Data);

#define ASIC_UNUSED_RAM_DATA 0x0b0


#include <string.h>
#include "debugmain.h"

#ifdef ASIC_DEBUGGING
extern char DebugString[256];
extern DEBUG_HANDLE ASIC_Debug;

static BOOL             ASIC_DebugEnable = TRUE;

void    ASIC_SetDebug(BOOL State)
{
        Debug_Enable(ASIC_Debug, State);

        ASIC_DebugEnable = Debug_Active(ASIC_Debug);
}
#endif

/* for colour/greyscale monitor support */
static RGBCOLOUR ASIC_DisplayColours[4096];

static RGBCOLOUR ASIC_PackedRGB[4096];
static RGBCOLOUR ASIC_PackedRGBColour[4096];
static RGBCOLOUR ASIC_PackedRGBGreyScale[4096];

/*#define DO_MONITOR_COLOUR */

static int CurrentLine; 


/* byte mask, which has bits set according to the lines occupied by this sprite */

/* cartridge data */
static  unsigned char   *pCartridge=NULL;

extern char *pLowerRom;
extern int LowerRomIndex;

unsigned char ASIC_GetPRI(void)
{
	return (unsigned char)ASIC_Data.ASIC_RasterInterruptLine;
}

unsigned char ASIC_GetSPLT(void)
{
	return (unsigned char)ASIC_Data.ASIC_RasterSplitLine;
}

unsigned long ASIC_GetSSA(void)
{
	return ASIC_Data.ASIC_SecondaryScreenAddress.Addr_W;
}

unsigned char ASIC_GetSSCR(void)
{
	return (unsigned char)ASIC_Data.ASIC_SoftScroll;
}

unsigned char ASIC_GetIVR(void)
{
	return (unsigned char)ASIC_Data.ASIC_InterruptVector;
}


void ASIC_SetPRI(unsigned char PRI)
{
	ASIC_Data.ASIC_RasterInterruptLine = PRI;
}

void	ASIC_SetSPLT(unsigned char SPLT)
{
	ASIC_Data.ASIC_RasterSplitLine = SPLT;
}

void ASIC_SetSSA(unsigned long SSA)
{
	ASIC_Data.ASIC_SecondaryScreenAddress.Addr_W = (unsigned short)SSA;
}

void ASIC_SetSSCR(unsigned char SSCR)
{
	ASIC_Data.ASIC_SoftScroll = SSCR;
}

void	ASIC_SetIVR(unsigned char IVR)
{
	ASIC_Data.ASIC_InterruptVector = IVR;
}

/* byte 0: upper nibble is red, lower nibble is blue.
   byte 1: lower nibble is green */

#ifdef CPC_LSB_FIRST
/* value is 0GRB for writing LSB first */
#define CPC_TO_ASIC_COLOUR(R,G,B) ((G<<8) | (R<<4) | (B))
#else
/* value is RB0G for writing MSB first */
#define CPC_TO_ASIC_COLOUR(R,G,B) ((R<<12) | (B<<8) | (G))
#endif

/* ASIC RGB of CPC hw colour. These values were obtained by 
sending the CPC colour and reading back the RGB from the ASIC palette,
using a test program */

static unsigned short	CPCToASICColours[32]=
{
        CPC_TO_ASIC_COLOUR(0x06,0x06,0x06),                     /* White */
        CPC_TO_ASIC_COLOUR(0x06,0x06,0x06),                     /* White */
        CPC_TO_ASIC_COLOUR(0x00,0x0f,0x06),                     /* Sea Green */
        CPC_TO_ASIC_COLOUR(0x0f,0x0f,0x06),                     /* Pastel yellow */
        CPC_TO_ASIC_COLOUR(0x00,0x00,0x06),                     /* Blue */
        CPC_TO_ASIC_COLOUR(0x0f,0x00,0x06),                     /* Purple */
        CPC_TO_ASIC_COLOUR(0x00,0x06,0x06),                     /* Cyan */
        CPC_TO_ASIC_COLOUR(0x0f,0x06,0x06),                     /* Pink */
        CPC_TO_ASIC_COLOUR(0x0f,0x00,0x06),                     /* Purple */
        CPC_TO_ASIC_COLOUR(0x0f,0x0f,0x06),                    /* Pastel yellow */
        CPC_TO_ASIC_COLOUR(0x0f,0x0f,0x00),                     /* Bright Yellow */
        CPC_TO_ASIC_COLOUR(0x0f,0x0f,0x0f),                     /* Bright White */
        CPC_TO_ASIC_COLOUR(0x0f,0x00,0x00),                     /* Bright Red */
        CPC_TO_ASIC_COLOUR(0x0f,0x00,0x0f),                     /* Bright Magenta */
        CPC_TO_ASIC_COLOUR(0x0f,0x06,0x00),                     /* Orange */
        CPC_TO_ASIC_COLOUR(0x0f,0x06,0x0f),                     /* Pastel Magenta */
        CPC_TO_ASIC_COLOUR(0x00,0x00,0x06),                     /* Blue */
        CPC_TO_ASIC_COLOUR(0x00,0x0f,0x06),                     /* Sea Green */
        CPC_TO_ASIC_COLOUR(0x00,0x0f,0x00),                     /* Bright green */
        CPC_TO_ASIC_COLOUR(0x00,0x0f,0x0f),                     /* Bright Cyan */
        CPC_TO_ASIC_COLOUR(0x00,0x00,0x00),                     /* Black */
        CPC_TO_ASIC_COLOUR(0x00,0x00,0x0f),                     /* Bright Blue */
        CPC_TO_ASIC_COLOUR(0x00,0x06,0x00),                     /* Green */
        CPC_TO_ASIC_COLOUR(0x00,0x06,0x0f),                     /* Sky Blue */
        CPC_TO_ASIC_COLOUR(0x06,0x00,0x06),                     /* Magenta */
        CPC_TO_ASIC_COLOUR(0x06,0x0f,0x06),                     /* Pastel green */
        CPC_TO_ASIC_COLOUR(0x06,0x0f,0x00),                     /* Lime */
        CPC_TO_ASIC_COLOUR(0x06,0x0f,0x0f),                     /* Pastel cyan */
        CPC_TO_ASIC_COLOUR(0x06,0x00,0x00),                     /* Red */
        CPC_TO_ASIC_COLOUR(0x06,0x00,0x0f),                     /* Mauve */
        CPC_TO_ASIC_COLOUR(0x06,0x06,0x00),                     /* Yellow */
        CPC_TO_ASIC_COLOUR(0x06,0x06,0x0f)                      /* Pastel blue */
};

static unsigned char    *CartridgePages[32];                    /* pointer to cartridge pages */


static const int ASIC_EnableSequence[]=
{
        0x0ff,0x077,0x0b3,0x051,0x0a8,0x0d4,0x062,0x039,
        0x09c,0x046,0x02b,0x015,0x08a,0x0cd
};

#define ASIC_EnableSequenceLength (sizeof(ASIC_EnableSequence)/sizeof(const int))

typedef enum
{
        SEQUENCE_SYNCHRONISE_FIRST_BYTE = 1,
        SEQUENCE_SYNCHRONISE_SECOND_BYTE,
        SEQUENCE_RECOGNISE,
        SEQUENCE_GET_LOCK_STATUS,
        SEQUENCE_SYNCHRONISE_THIRD_BYTE
} SEQUENCE_STATE;

static SEQUENCE_STATE   RecogniseSequenceState;
static int      CurrentSequencePos;

void	ASIC_SetUnLockState(BOOL State)
{
	if (State)
	{
		ASIC_Data.Flags |= ASIC_ENABLED;	
		CRTC_SetRenderFunction(RENDER_MODE_ASIC_FEATURES);
	}
	else
	{
		ASIC_Data.Flags &=~ASIC_ENABLED;
		CRTC_SetRenderFunction(RENDER_MODE_STANDARD);
	}
}

BOOL	ASIC_GetUnLockState(void)
{
	return ((ASIC_Data.Flags & ASIC_ENABLED)==ASIC_ENABLED);
}


/* when an out to 0x0bc is done, call this code
 this code enables or disables the ASIC depending on the un-locking/locking
 sequence */
void    ASIC_EnableDisable(int Data)
{
	switch (RecogniseSequenceState)
	{
			case    SEQUENCE_SYNCHRONISE_FIRST_BYTE:
			{
					/* we are waiting for the first byte of synchronisation */
					if (Data!=0)
							RecogniseSequenceState = SEQUENCE_SYNCHRONISE_SECOND_BYTE;
			}
			break;
			
			case    SEQUENCE_SYNCHRONISE_SECOND_BYTE:
			{
					/* at this point we already have a non-zero byte to start the synchronisation.

					 we are waiting for the second byte of synchronisation

					 this byte must be zero for synchronisation. If it is non-zero,
					 then it can still count as part of the synchronisation. */

					if (Data==0x00)
					{
							/* got zero. We are now waiting for the first byte of the sequence */
							RecogniseSequenceState = SEQUENCE_SYNCHRONISE_THIRD_BYTE;
					}
			}
			break;

            case SEQUENCE_SYNCHRONISE_THIRD_BYTE:
            {
                    /* we are waiting for the first data byte of the sequence. To get here
                     we must have had a non-zero byte followed by a zero byte */

                    /* have we got the first byte of the sequence? */
                    if (Data==0x0ff)
                    {
                            /* first byte of sequence, get ready to recognise the sequence. */
                            RecogniseSequenceState = SEQUENCE_RECOGNISE;
                            CurrentSequencePos = 1;
                    }
                    else
                    {
                            if (Data!=0)
                            {
                                    /* we got a non-zero byte, and it wasn't the first part of the sequence.
                                    / this could act as the first byte of synchronisation ready for a zero
                                    / to follow. */
                                    RecogniseSequenceState = SEQUENCE_SYNCHRONISE_SECOND_BYTE;
                            }
                    
                            /* if we got a zero, we are still synchronised. We are still waiting for
                            the first byte of sequence. */
                    }

            }
            break;

            case    SEQUENCE_RECOGNISE:
            {
                    /* we want to recognise the sequence. We already recognise the first byte. */

                    if (Data == 0x000)
                    {
                            RecogniseSequenceState = SEQUENCE_SYNCHRONISE_THIRD_BYTE;
                    }
                    else
                    {
                            if (Data==ASIC_EnableSequence[CurrentSequencePos])
                            {
                                    /* data byte the same as sequence */

                                    /* ready for next char in sequence */
                                    CurrentSequencePos++;

                                    if (CurrentSequencePos==ASIC_EnableSequenceLength)
                                    {

                                            /* needs this for KLAX (it unlocks by not sending last 0x0ee) */

                                            /* disable asic. If next byte is 0x0ee, then asic will be enabled */
                                            ASIC_Data.Flags &=~ASIC_ENABLED;	
											ASIC_Data.ASIC_RamMask = 0;

                                            CRTC_SetRenderFunction(RENDER_MODE_STANDARD);

                                            /* sequence is almost complete. If next byte sent is
                                             a 0x0ee then the asic will be enabled.  */
                                            RecogniseSequenceState = SEQUENCE_GET_LOCK_STATUS;
                                            break;                  
                                    }
                            }
                            else
                            {
                                    CurrentSequencePos = 0;
                                    RecogniseSequenceState = SEQUENCE_SYNCHRONISE_SECOND_BYTE;
		                    }
					}
            }
			break;

            case    SEQUENCE_GET_LOCK_STATUS:
            {
                    /* the sequence has been correct up to this point, therefore
                     we want to lock or un-lock the ASIC */

					if (Data==0x0ee)
                    {
                            /* unlock asic */

#ifdef ASIC_DEBUGGING
		              if (ASIC_DebugEnable)
			          {
				                Debug_WriteString(ASIC_Debug,"Asic enabled\r\n");
					  }
#endif
						ASIC_Data.Flags |= ASIC_ENABLED;	

						CRTC_SetRenderFunction(RENDER_MODE_ASIC_FEATURES);

                    }

                    if (Data!=0)
                    {
                            RecogniseSequenceState = SEQUENCE_SYNCHRONISE_SECOND_BYTE;
                    }
                    else
                    {
                            RecogniseSequenceState = SEQUENCE_SYNCHRONISE_FIRST_BYTE;
                    }
	        }
			break;
	}
}

unsigned char   *ASIC_GetCartPage(int Page)
{
        return CartridgePages[Page & 0x01f];
}

                
/* initialise ASIC emulation */
BOOL    ASIC_Initialise(void)
{
		ASIC_Data.ASIC_Ram = NULL;

		/* allocate ASIC Ram */
        ASIC_Data.ASIC_Ram = (unsigned char *)malloc(16384);

        if (ASIC_Data.ASIC_Ram==NULL)
                return FALSE;

		memset(ASIC_Data.ASIC_Ram, ASIC_UNUSED_RAM_DATA, 16384);

		ASIC_Data.ASIC_Ram_Adjusted = ASIC_Data.ASIC_Ram-(unsigned long)0x04000;

		/* Initialise cartridge pages */
        ASIC_InitCart();

        /* initialise a table of colours and grey-scales. Given a ASIC
        colour, this will give the new colour to pass to the render part
        to give the appearance of a colour or grey-scale/paper-white display */
        ASIC_InitialiseMonitorColourModes();

		/* initial analogue inputs */
		ASIC_Data.AnalogueInputs[0] = 0x03f;
		ASIC_Data.AnalogueInputs[1] = 0x03f;
		ASIC_Data.AnalogueInputs[2] = 0x03f;
		ASIC_Data.AnalogueInputs[3] = 0x03f;
		ASIC_Data.AnalogueInputs[4] = 0x03f;
		ASIC_Data.AnalogueInputs[5] = 0x000;
		ASIC_Data.AnalogueInputs[6] = 0x03f;
		ASIC_Data.AnalogueInputs[7] = 0x000;

        return TRUE;
}

void	ASIC_SetAnalogueInput(int InputID, unsigned char Data)
{
	ASIC_Data.AnalogueInputs[InputID] = Data;
}

unsigned char ASIC_GetAnalogueInput(int InputID)
{
	return ASIC_Data.AnalogueInputs[InputID];
}

void	ASIC_WriteRamFull(int Addr, int Data)
{
	ASIC_Data.ASIC_Ram[Addr & 0x03fff] = Data;
	ASIC_WriteRam(Addr,Data);
}

void    ASIC_Finish(void)
{
        if (ASIC_Data.ASIC_Ram!=NULL)
        {
                free(ASIC_Data.ASIC_Ram);
        }

        ASIC_Data.ASIC_Ram = NULL;

        Cartridge_Remove();
}

void    ASIC_Reset(void)
{
        int     i;

#ifdef ASIC_DEBUGGING
        if (ASIC_DebugEnable)
        {
                Debug_WriteString(ASIC_Debug,"Doing Reset\r\n");
        }
#endif

        ASIC_Data.Flags |= (ASIC_ENABLED);
		
        /* disable all sprites */
        for (i=0; i<16; i++)
        {
                ASIC_WriteRamFull(0x06000 + (i<<3) + 4,0);
        }

        ASIC_Data.SpriteEnableMask = 0;

        /* no scan-line interrupt */
        ASIC_Data.ASIC_RasterInterruptLine = 0;
       
        /* no split */
        ASIC_Data.ASIC_RasterSplitLine = 0;
        
        /* no automatic clearing of DMA ints */
		/* bits 7-1 are undefined at reset */
		/* bit 0 set to 1 at reset */
        ASIC_Data.ASIC_InterruptVector |= 1;

        /* set soft scroll register */
        ASIC_Data.ASIC_SoftScroll = 0;

        /* DCSR */
        ASIC_WriteRamFull(0x06c0f,0);

        /* reset analogue input channels */
        for (i=0; i<8; i++)
        {
            ASIC_WriteRamFull(0x06808+i,0x0ff);
        }


        /* disable asic - requires unlocking sequence to use features */
        ASIC_Data.Flags &= ~(ASIC_ENABLED);
		
		ASIC_Data.ASIC_RamMask = 0;
        /* disable access to asic ram */
        CPC_EnableASICRamWrites(FALSE);

        CRTC_SetRenderFunction(RENDER_MODE_STANDARD);

        /* reset unlock sequence state-machine */
        RecogniseSequenceState = SEQUENCE_SYNCHRONISE_FIRST_BYTE;
        
        ASIC_Data.InternalDCSR = 0;

        ASICCRTC_SetSoftScroll(0);

/* CHECK! */
/*		ASIC_SetSecondaryRomMapping(0x0a0); */
/* ram state on reset */
#ifdef ASIC_DEBUGGING
        if (ASIC_DebugEnable)
        {
                Debug_WriteString(ASIC_Debug,"End Of Reset\r\n");
        }
#endif
}


/* initialise cartridge pointers */
void    ASIC_InitCart(void)
{
        int     i;

        for (i=0; i<32; i++)
        {
                CartridgePages[i] = NULL;
        }
}

/* check if cartridge file is valid, return TRUE if it is, else
false */
BOOL    Cartridge_ValidateCartridge(unsigned char *pData, int FileSize)
{
        RIFF_CHUNK *pHeader = (RIFF_CHUNK *)pData;
        unsigned char *pChunkData;

        /* check header of file has 'RIFF' */
        if (pHeader->ChunkName == RIFF_FOURCC_CODE('R','I','F','F'))
        {
                /* file is a RIFF */

                int FileLength;

                /* get size of file from header chunk */
                FileLength = Riff_GetChunkLength(pHeader); 
                
                if (FileLength==(int)(FileSize-sizeof(RIFF_CHUNK)))
                {
                        /* size ok */

						if (Riff_CheckChunkSizesAreValid((unsigned char *)pHeader, FileSize))
						{


							/* get pointer to chunk data */
							pChunkData = Riff_GetChunkDataPtr(pHeader);

							/* is RIFF of type 'AMS!'? */
							if ((pChunkData[0] == 'A') &&
									(pChunkData[1] == 'M') &&
									(pChunkData[2] == 'S') &&
									(pChunkData[3] == '!'))
							{
									/* RIFF is a AMS! */

									/* check each cart block is of the correct size */

									int i;
                                
									for (i=0; i<32; i++)
									{
											RIFF_CHUNK      *pChunk;
											unsigned long CartBlockChunkName;
											int                     ChunkSize;

											/* create chunk name */
											CartBlockChunkName = RIFF_FOURCC_CODE('c','b',((i/10)+'0'),((i % 10)+'0'));

											/* find chunk */
											pChunk = Riff_FindNamedSubChunk(pHeader, CartBlockChunkName);

											if (pChunk!=NULL)
											{
													/* get chunk size */
													ChunkSize = Riff_GetChunkLength(pChunk);
                                        
													/* block size should be 16384 */
													if (ChunkSize!=16384)
															break;
											}
									}

									/* if i is 32, then all blocks are correct size */
									if (i==32)
											return TRUE;
							}
					}
				}
        }

        return FALSE;
}

/* calc size of cart in blocks */
int             Cartridge_CalcCartSizeInBlocks(int NoOfBlocks)
{
        int i;
        int Temp = NoOfBlocks;
        int BitCount = 32;

        /* get highest bit in number */

        if (Temp==0)
                return 0;

        for (i=32; i>0; i++)
        {
                /* check highest bit */
                if ((Temp & 0x080000000)!=0)
                {
                        /* if it is set we have found the highest bit in the number */
                        break;
                }

                BitCount--;

                /* shift it up */
                Temp = Temp<<1;
        }

        /* calculate no of blocks so that size is a power of two */
        NoOfBlocks = 0x0ffffffff & (~(0x0ffffffff<<BitCount));

        return NoOfBlocks+1;
}

BOOL    Cartridge_Load(char *Filename)
{
        unsigned long CartridgeLength;
        unsigned char *pNewCartridge;

        /* load cartridge data */
        Host_LoadFile(Filename, &pNewCartridge, &CartridgeLength);
        
        if (pNewCartridge!=NULL)
        {
                /* check cartridge is valid */
                if (Cartridge_ValidateCartridge(pNewCartridge, CartridgeLength))
                {
                        /* cartridge is valid */
                        int HighestBlockIndex = 0;
                        int CartSizeInBlocks = 0;

                        /* get cartridge data blocks */
                        RIFF_CHUNK *pHeader = (RIFF_CHUNK *)pNewCartridge;
                        int i;

                        /* remove old one if it is present */   
                        Cartridge_Remove();

                        for (i=0; i<32; i++)
                        {
                                unsigned long CartBlockChunkName = RIFF_FOURCC_CODE('c','b',((i/10)+'0'),((i % 10)+'0'));
                                RIFF_CHUNK *pChunk;

                                /* find cart block chunk */
                                pChunk = Riff_FindNamedSubChunk(pHeader, CartBlockChunkName);

                                if (pChunk!=NULL)
                                {
                                        /* found cart block chunk */

                                        unsigned char *pChunkData = Riff_GetChunkDataPtr(pChunk);
                                        
                                        CartridgePages[i] = pChunkData;
        
                                        HighestBlockIndex  = i;
                                }
                                else
                                {
                                        CartridgePages[i] = NULL;
                                }
                        }

                        /* the following code works out the size of the cart,
                        and then fills in the NULL pointers with other pointers
                        to repeat the data */

                        CartSizeInBlocks = Cartridge_CalcCartSizeInBlocks(HighestBlockIndex);

                        /* convert highest block index, into highest block index
                        that is a power of 2 */
                        
                        /* fill in missing cart pages */
                        for (i=0; i<32; i++)
                        {
                                if (CartridgePages[i] == NULL)
                                {
                                        CartridgePages[i] = CartridgePages[i & (CartSizeInBlocks-1)];
                                }
                        }

                        pCartridge = pNewCartridge;

                        /* cartridge loaded ok */

                        /* are we in CPC+ mode? */
                        if (CPC_GetHardware()!=CPC_HW_CPCPLUS)
                        {
                            /* no. set type to CPC+ (6128), and reset it */
                            CPC_SetCPCType(CPC_TYPE_6128PLUS);
                        }
						
                        /* already in CPC+ mode, just reset */
                        CPC_Reset();

                        return TRUE;    
                }
                else
                {
                        free(pNewCartridge);
                }
        }

        return FALSE;
}

void    Cartridge_Remove(void)
{
        if (pCartridge!=NULL)
        {
                free(pCartridge);
                pCartridge = NULL;
        }
}

INLINE static void    ASIC_SetMemPointers(unsigned char **pReadRamPtr, unsigned char **pWriteRamPtr)
{
	unsigned long Mask = ASIC_Data.ASIC_RamMask;
	unsigned char *pPtr;
	
	pPtr = (unsigned char *)(((unsigned long)ASIC_Data.ASIC_Ram_Adjusted & Mask) | ((unsigned long)pReadRamPtr[2] & (~Mask)));

	pReadRamPtr[2] = pWriteRamPtr[2] = pReadRamPtr[3] = pWriteRamPtr[3] = pPtr;	
}

void    ASIC_GateArray_RethinkMemory(unsigned char **pReadRamPtr, unsigned char **pWriteRamPtr)
{
    ASIC_SetMemPointers(pReadRamPtr, pWriteRamPtr);
}

/* initialise expansion rom table */
void	ASIC_RefreshExpansionROMTable(unsigned char **pExpansionROMTable)
{
	int i;
	unsigned char *pExpansionROM;

	/* setup cartridge selections */
	for (i=128; i<256; i++)
	{
		pExpansionROM = (unsigned char *)((unsigned long)ASIC_GetCartPage(i & 0x01f) - (unsigned long)0x0c000);
		pExpansionROMTable[i] = pExpansionROM;
	}


	/* basic */
	pExpansionROM = (unsigned char *)((unsigned long)ASIC_GetCartPage(1) - (unsigned long)0x0c000);

	for (i=0; i<128; i++)
	{
		pExpansionROMTable[i] = pExpansionROM;
	}

	/* amsdos */
	pExpansionROM = (unsigned char *)((unsigned long)ASIC_GetCartPage(3) - (unsigned long)0x0c000);
	pExpansionROMTable[7] = pExpansionROM;
}

int	ASIC_GetSecondaryRomMapping(void)
{
	return ASIC_Data.SecondaryRomMapping;
}

void     ASIC_SetSecondaryRomMapping(unsigned char Data)
{
        unsigned char ASIC_State;

		/* store it for snapshot */
		ASIC_Data.SecondaryRomMapping = Data;
        ASIC_State = (unsigned char)(((Data>>3) & 3));

        if (ASIC_State==0x03)
        {
                /* ASIC Ram enabled, lower rom 0x0000-0x03fff */
                LowerRomIndex = 0;
				ASIC_Data.ASIC_RamMask = (unsigned long)(~0);
                CPC_EnableASICRamWrites(TRUE);

        }
        else
        {
                /* 0, 1 or 2 */
                LowerRomIndex = ASIC_State;
				ASIC_Data.ASIC_RamMask = 0;
                CPC_EnableASICRamWrites(FALSE);

        }

        pLowerRom = (char *)ASIC_GetCartPage(Data & 0x07);

		/* initialise pointers for memory "re-think" */
		pLowerRom = (char *)((unsigned long)pLowerRom - (unsigned long)(LowerRomIndex<<14));

		LowerRomIndex = LowerRomIndex<<1;

		GateArray_RethinkMemory();
}


/* update asic ram with chosen Gate Array colour */
void	ASIC_GateArray_UpdatePaletteRam(unsigned char PenIndex, unsigned char Colour)
{								  
    /* write ASIC RGB version of CPC hardware colour
    to ASIC colour registers */
    ((unsigned short *)(ASIC_Data.ASIC_Ram + 0x02400 + (PenIndex<<1)))[0] = CPCToASICColours[Colour];
}

/* check if secondary rom mapping was chosen, and if so execute it */
BOOL	ASIC_GateArray_CheckForSecondaryRomMapping(unsigned char Function)
{
	if (ASIC_Data.Flags & ASIC_ENABLED)	
	{
		if ((Function & 0x0e0)==0x0a0)
        {
           /* function 101xxxxx */
           ASIC_SetSecondaryRomMapping(Function);
           return TRUE;
        }
	}

	return FALSE;
}


unsigned char *ASIC_GetRamPtr(void)
{
        return ASIC_Data.ASIC_Ram;
}


unsigned char   ASIC_GetRed(int ColourIndex)
{
        return (unsigned char)((ASIC_Data.ASIC_Ram[0x02400 + (ColourIndex<<1)] & 0x0f0)>>4);
}

unsigned char ASIC_GetGreen(int ColourIndex)
{
        return (unsigned char)((ASIC_Data.ASIC_Ram[0x02400 + (ColourIndex<<1)+1] & 0x0f));
}


unsigned char ASIC_GetBlue(int ColourIndex)
{
        return (unsigned char)((ASIC_Data.ASIC_Ram[0x02400 + (ColourIndex<<1)] & 0x0f));
}

unsigned char ASIC_GetSpritePixel(int SpriteIndex, int X, int Y)
{
        unsigned char *pSpriteData = &ASIC_Data.ASIC_Ram[(SpriteIndex<<8)];

        return (unsigned char)(pSpriteData[(Y<<4) + X] & 0x0f);
}

 unsigned long ASIC_BuildDisplayReturnMaskWithPixels(int Line, int HCount, /*int MonitorHorizontalCount,*/ /*int ActualY,*/ int *pPixels)
{
        /* based on line we can find which HW sprites are here */

        int     i;
		unsigned char   *pSpriteGraphics;

        unsigned long GraphicsMask = 0;

        if ((ASIC_Data.SpriteEnableMask!=0) && (ASIC_Data.SpriteEnableMaskOnLine!=0))
        {
                /* HCount * 16 */
                int Column = HCount<<4;

                {
                        /* do each sprite in turn (from highest to lowest priority) */
                        for (i=0; i<16; i++)
                        {
                                if (ASIC_Data.SpriteEnableMaskOnLine & (1<<i))
                                {
                                        pSpriteGraphics = ASIC_Data.ASIC_Ram + (i<<8);

                                        {
                                                if ((Column>=ASIC_Data.SpriteInfo[i].SpriteXColumnMin) && (Column<=ASIC_Data.SpriteInfo[i].SpriteXColumnMax))
                                                {
                                                        /* column and line is within sprite range */

                                                        /* need to work out X and Y pixel position of sprite */

                                                        int                     SpriteX,SpriteY;
                                                        int                     j, SprY;
                                                        int                     XStart;

                                                        SpriteY = Line - ASIC_Data.SpriteInfo[i].SpriteMinYPixel;

                                                        if (Column == ASIC_Data.SpriteInfo[i].SpriteXColumnMin)
                                                        {
                                                                XStart = ASIC_Data.SpriteInfo[i].SpriteMinXPixel & 0x0f;
                                                                SpriteX = 0;
                                                        }
                                                        else
                                                        {
                                                                SpriteX = Column - ASIC_Data.SpriteInfo[i].SpriteMinXPixel;
                                                                XStart = 0;
                                                        }

                                                        SprY = SpriteY>>ASIC_Data.SpriteInfo[i].YMagShift;


                                                        {
                                                                /* rendering 16 pixels in one go = 1 CRTC char width */
                                                                for (j=XStart; j<16; j++)
                                                                {
                                                                        /* if no pixel rendered here */
                                                                        if ((GraphicsMask & (1<<j))==0)
                                                                        {
                                                                                signed short    SprX;

                                                                        
                                                                                SprX = (signed short)((SpriteX+(j-XStart))>>ASIC_Data.SpriteInfo[i].XMagShift);
                                                                                
                                                                                /* if removed mainly renders but on last char appears to leave garbage */
                                                                                if (SprX<16)
                                                                                {
                                                                                        int     Colour;

                                                                                        Colour = (pSpriteGraphics[(SprY<<4) | SprX] & 0x0f);
                                                                                
                                                                                        if (Colour!=0)
                                                                                        {
                                                                                            
                                                                                                pPixels[j] = Colour+16;
                                                                                                /* mark this pixel as used */
                                                                                                GraphicsMask |= (1<<j);
                                                                                        }
                                                                                }
                                                                        }
                                                                }

                                                                /* if all are used, quit */
                                                                if (GraphicsMask == 0x00000ffff)
                                                                        break;
                                                        }
                                                }
                                        }
                                }
                                
           				}
                }
        }

        return (GraphicsMask ^ 0x0ffff);
}


BOOL    ASIC_RasterSplitLineMatch(int CRTC_LineCounter, int CRTC_RasterCounter)
{
        unsigned char SplitLine;

        if ((ASIC_Data.Flags & ASIC_ENABLED)==0)	
                return FALSE;

        SplitLine = (unsigned char)((((CRTC_LineCounter & 0x01f)<<3) | (CRTC_RasterCounter & 0x07)));

        if ((ASIC_Data.ASIC_RasterSplitLine==0) || (SplitLine==0))
                return FALSE;

        if (SplitLine == ASIC_Data.ASIC_RasterSplitLine)
        {
                return TRUE;
        }

        return FALSE;
}

                        

void    ASIC_HSync(int CRTC_LineCounter, int CRTC_RasterCounter)
{
        /* if ASIC is not enabled, then functions not
         available */
        if ((ASIC_Data.Flags & ASIC_ENABLED)==0)	
                return;

        /* is raster interrupt line active? */
        if (ASIC_Data.ASIC_RasterInterruptLine!=0)
        {
                if ((CRTC_LineCounter == ((ASIC_Data.ASIC_RasterInterruptLine>>3) & 0x01f)) &&
                        (CRTC_RasterCounter == (ASIC_Data.ASIC_RasterInterruptLine & 0x07)))
                {
                        /* raster interrupt occured */
                        ASIC_SetRasterInterrupt();
                }
        }

        /* do a dma cycle and maybe set a dma interrupt */
        ASIC_DoDMA();

        /* trigger int if any sources require an interrupt */
        ASIC_TriggerInterrupt();
}

static void     ASIC_UpdateRAMWithInternalDCSR(void)
{
        int i;
        unsigned char thisDCSR = (unsigned char)((ASIC_Data.InternalDCSR & 0x07f) | ASIC_Data.ASIC_DCSR2);
		unsigned char *pAddr = &ASIC_Data.ASIC_Ram[0x02c00];

        for (i=15; i>=0; i--)
        {
			pAddr[i] = (unsigned char)thisDCSR;
        }
}


/* trigger a ASIC generated interrupt (raster or dma) */
static void     ASIC_TriggerInterrupt(void)
{
        /* any interrupting sources? */
        if (ASIC_Data.InternalDCSR & 0x0f0)
        {
                int Vector;

                /* calculate mode 2 interrupt vector based on interrupt that is enabled */
                Vector = ASIC_CalculateInterruptVector();

                /* set Z80 vector base for mode 2 */
                Z80_SetInterruptVector(Vector);

                /* trigger the interrupt */
                Z80_SetInterruptRequest();
        }
}

/* calculate a interrupt vector to supply in IM2 or ignore in IM1 based on interrupts active. */
int     ASIC_CalculateInterruptVector(void)
{
        unsigned char      Vector = 0;

        /* lowest priority to highest priority */

        /* is DMA channel 0 interrupt triggered? */
        if (ASIC_Data.InternalDCSR & 0x040)
        {
                Vector = 0x04;	
        }

        /* is DMA channel 1 interrupt triggered? */
        if (ASIC_Data.InternalDCSR & 0x020)
        {
                Vector = 0x02;	
        }

        /* is DMA channel 2 interrupt triggered? */
        if (ASIC_Data.InternalDCSR & 0x010)
        {
                Vector = 0x00;	
        }

        /* is raster interrupt triggered */
        if (ASIC_Data.InternalDCSR & 0x080)
        {
                /* raster int line specified */
                Vector = 0x06;	
        }

        return ((ASIC_Data.ASIC_InterruptVector & 0x0f8) | Vector);
}

/* clear dma ints by a manual write to DCSR */
void    ASIC_ClearDMAInterruptsManual(int Data)
{
	ASIC_Data.InternalDCSR = (unsigned char)(ASIC_Data.InternalDCSR & ~(Data & (0x07<<4)));
}

static void ASIC_ClearDMAInterruptsAutomatic(void)
{
        /* is DMA channel 2 interrupt triggered? */
        if (ASIC_Data.InternalDCSR & 0x010)
        {
                /* clear interrupt */
                ASIC_Data.InternalDCSR = (unsigned char)(ASIC_Data.InternalDCSR & (~0x010));
                return;
        }

        /* is DMA channel 1 interrupt triggered? */
        if (ASIC_Data.InternalDCSR & 0x020)
        {
                /* clear interrupt */
                ASIC_Data.InternalDCSR = (unsigned char)(ASIC_Data.InternalDCSR & (~0x020));
                return;
        }
                

        /* is DMA channel 0 interrupt triggered? */
        if (ASIC_Data.InternalDCSR & 0x040)
        {
                /* clear interrupt */
                ASIC_Data.InternalDCSR = (unsigned char)(ASIC_Data.InternalDCSR & (~0x040));
                return;
        }
}

/* clear dma interrupts automatically when ints are done, only clears */
/* highest priority DMA int active, so other ints may be done too */
void    ASIC_ClearDMAInterrupts()
{
        /* clear dma interrupts in order of priority */
        ASIC_ClearDMAInterruptsAutomatic();

        /* update asic ram with DCSR value */
        ASIC_UpdateRAMWithInternalDCSR();
}

/* set raster interrupt */
void    ASIC_SetRasterInterrupt(void)
{
        ASIC_Data.ASIC_DCSR2 = 0x080;
        ASIC_Data.InternalDCSR|=0x080;
}

/* clear raster interrupt */
void    ASIC_ClearRasterInterrupt(void)
{
        ASIC_Data.InternalDCSR&=0x07f;    

		ASIC_TriggerInterrupt();
}

/* this function is called whenever the Z80 acknowledges a interrupt */
void    ASIC_AcknowledgeInterrupt()
{
        if ((ASIC_Data.InternalDCSR & 0x080)==0)
        {
                /* not a raster interrupt. Is it a DMA interrupt ? */

                ASIC_Data.ASIC_DCSR2 = 0x00;

                /* if vector supplied had bit 0 not set,
                then we automatically clear dma interrupts on a acknowledge */
                if ((ASIC_Data.ASIC_InterruptVector & 0x001)==0)
                {
                        /* clear the dma interrupt */
                        ASIC_ClearDMAInterrupts();
                }
        }
        else
        {
                /* clear raster int */
                ASIC_ClearRasterInterrupt();
        }

        /* write InternalDCSR to ram */
        ASIC_UpdateRAMWithInternalDCSR();

        ASIC_Data.ASIC_DCSR2 = 0x00;
}



/******************************************/
/* ASIC handling code */



#ifdef ASIC_DEBUGGING
void    ASIC_WriteRamDebug(int Addr,int Data)
{
}
#endif

static void ASIC_SetupSpriteRenderInfo(int SpriteIndex)
{
        /* write sprite info */

        unsigned char SpriteMag;
        ASIC_SPRITE_RENDER_INFO *pRenderInfo;
        
        pRenderInfo = &ASIC_Data.SpriteInfo[SpriteIndex];

        SpriteMag = ASIC_Data.Sprites[SpriteIndex].SpriteMag;

        /* is sprite renderable ? */
		/* is XMag!=0 and YMag!=0. For both to be not equal to zero, then they must be
		greater or equal to %0101! */

        if (SpriteMag>=5)		
        {

                /* sprite is renderable */
		        unsigned int    XMag,YMag;

				/* get X and Y mag */
		        XMag = (SpriteMag>>2) & 0x003;
				YMag = SpriteMag & 0x003;

                /* enable rendering of sprite */

                pRenderInfo->XMagShift = XMag-1;
                pRenderInfo->YMagShift = YMag-1;

                /* get sprite min coordinates */

                /* get sprite data - intel only */
                pRenderInfo->SpriteMinXPixel = (signed short)(ASIC_Data.Sprites[SpriteIndex].SpriteX.SpriteX_W);
                pRenderInfo->SpriteMinYPixel = (signed short)(ASIC_Data.Sprites[SpriteIndex].SpriteY.SpriteY_W);
                
                /* get sprite max coordinates */
                pRenderInfo->SpriteMaxXPixel = (short)(pRenderInfo->SpriteMinXPixel + (1<<(XMag-1+4)));
                pRenderInfo->SpriteMaxYPixel = (short)(pRenderInfo->SpriteMinYPixel + (1<<(YMag-1+4)));	

                /* each column is actually a CRTC char wide - 16 pixels */                      
                pRenderInfo->SpriteXColumnMin = (short)(pRenderInfo->SpriteMinXPixel & 0x0fff0);
                pRenderInfo->SpriteXColumnMax = (short)(pRenderInfo->SpriteMaxXPixel & 0x0fff0);

                ASIC_Data.SpriteEnableMask |= (1<<SpriteIndex);
                
                /* active on this line? */
                if ((CurrentLine>=pRenderInfo->SpriteMinYPixel) && (CurrentLine<pRenderInfo->SpriteMaxYPixel))
                {
                        /* say it's active in the mask */
                        ASIC_Data.SpriteEnableMaskOnLine |= (1<<SpriteIndex);

                }
        }
        else
        {

                ASIC_Data.SpriteEnableMask &= ~(1<<SpriteIndex);

                /* active on this line */
                if ((CurrentLine>=pRenderInfo->SpriteMinYPixel) && (CurrentLine<pRenderInfo->SpriteMaxYPixel))
                {
                        /* say it's active in the mask */
                        ASIC_Data.SpriteEnableMaskOnLine &= ~(1<<SpriteIndex);
                }

        }
}


unsigned char ASIC_GetDCSR(void)
{
	return (unsigned char)((ASIC_Data.InternalDCSR & 0x07f) | ASIC_Data.ASIC_DCSR2);
}


unsigned short ASIC_GetSpriteX(int SpriteIndex)
{
	return (unsigned short)ASIC_Data.Sprites[SpriteIndex].SpriteX.SpriteX_W;
}

unsigned short ASIC_GetSpriteY(int SpriteIndex)
{
	return (unsigned short)ASIC_Data.Sprites[SpriteIndex].SpriteY.SpriteY_W;

}

unsigned char ASIC_GetSpriteMagnification(int SpriteIndex)
{
	return ASIC_Data.Sprites[SpriteIndex].SpriteMag;
}


/* data will have already been poked into ram */
void    ASIC_WriteRam(int Addr,int Data)
{
        if ((Addr & 0x0c000)!=0x04000)
                return;
        
#ifdef ASIC_DEBUGGING
        if (ASIC_DebugEnable)
        {
                ASIC_WriteRamDebug(Addr,Data);
        }
#endif

		Addr = Addr & 0x03fff;

        if ((Addr & 0x0f000)==0x00000)	
        {
                /* write to sprite ram */

                /* remove upper nibble from sprite data */
                ASIC_Data.ASIC_Ram[Addr/* & 0x0fff*/] = (unsigned char)(Data & 0x0f);
                return;
        }

        if ((Addr & 0x03f80)==0x02000)  
        {
                int SpriteIndex = (Addr & 0x078)>>3;
                
                switch (Addr & 0x07)
                {
                        case 0:
                        {
							if (ASIC_Data.Sprites[SpriteIndex].SpriteX.SpriteX_B.l == Data)
							{
								return;
							}
                                
							/* set X coordinate low byte */
                            ASIC_Data.Sprites[SpriteIndex].SpriteX.SpriteX_B.l = (unsigned char)Data;
                                
							/* mirror */
							ASIC_Data.ASIC_Ram[Addr+4] = Data; 
                        }
                        break;

                        case 1:
                        {
							if (ASIC_Data.Sprites[SpriteIndex].SpriteX.SpriteX_B.h == Data)
							{
								return;
							}
							{
//								unsigned char LocalData = Data;	// & 0x03;


//								if (LocalData==3)
//								{
//									LocalData=0x0ff;
//								}

								/* set X coordinate high byte */
								ASIC_Data.Sprites[SpriteIndex].SpriteX.SpriteX_B.h = (signed char)Data;   
								
								/* change value in ram and mirror */
								ASIC_Data.ASIC_Ram[Addr] = (ASIC_Data.ASIC_Ram[Addr+4] = Data);

							}
                        }
                        break;

                        case 2:
                        {
							if (ASIC_Data.Sprites[SpriteIndex].SpriteY.SpriteY_B.l == Data)
							{
								return;
							}

                            /* set Y coordinate low byte */
                            ASIC_Data.Sprites[SpriteIndex].SpriteY.SpriteY_B.l = (unsigned char)Data;
							
							/* mirror */
							ASIC_Data.ASIC_Ram[Addr+4] = Data; 
                        }
                        break;

                        case 3:
                        {
							if (ASIC_Data.Sprites[SpriteIndex].SpriteY.SpriteY_B.h == Data)
							{
								return;
							}
							{
//								unsigned char LocalData = Data;	// & 0x01;

                            
//								/* change value and mirror */
//								if (LocalData==1)
//								{
//									LocalData=0x0ff;
//								}

								/* set Y coordinate high byte */
								ASIC_Data.Sprites[SpriteIndex].SpriteY.SpriteY_B.h = (signed char)Data;   


								ASIC_Data.ASIC_Ram[Addr] = (ASIC_Data.ASIC_Ram[Addr+4] = Data);
							}
                        }
                        break;


						default:
                        {
							if (ASIC_Data.Sprites[SpriteIndex].SpriteMag==(Data & 0x0f))
							{
	                            /* offset 4, mirrors offset 0, 
		                        offset 3, mirrors offset 1.. */
			                    ASIC_Data.ASIC_Ram[Addr] = ASIC_Data.ASIC_Ram[Addr & 0x03ffb];
								return;
							}

                            /* store sprite magnification */
                            ASIC_Data.Sprites[SpriteIndex].SpriteMag = (unsigned char)(Data & 0x0f);

                            /* offset 4, mirrors offset 0, 
                            offset 3, mirrors offset 1.. */
                            ASIC_Data.ASIC_Ram[Addr] = ASIC_Data.ASIC_Ram[Addr & 0x03ffb];
                        }
                        break;
                
                
                }

                /* update sprite render information */
                ASIC_SetupSpriteRenderInfo(SpriteIndex);
        
                return;
        }

		if ((Addr & 0x0fff8) == 0x02800)
		{
			switch (Addr & 0x07)
			{
				case 0:
				{
					ASIC_Data.ASIC_RasterInterruptLine = (unsigned char)Data;
					return;
				}

				case 1:
				{
					ASIC_Data.ASIC_RasterSplitLine = (unsigned char)Data;

					/* split line */
					ASICCRTC_ScreenSplit();
					return;
				}

		
				case 2:
				{
					ASIC_Data.ASIC_SecondaryScreenAddress.Addr_B.h = (unsigned char)Data;
					return;
				}

				case 3:
				{
					ASIC_Data.ASIC_SecondaryScreenAddress.Addr_B.l = (unsigned char)Data;
					return;
				}

				case 4:
				{
					ASIC_Data.ASIC_SoftScroll = (unsigned char)Data;

					Render_SetHorizontalPixelScroll(Data & 0x0f);
                
					ASICCRTC_SetSoftScroll(Data);
					return;
				}

				
				case 5:
				{
	                /* interrupt vector supplied by ASIC */
		            ASIC_Data.ASIC_InterruptVector = (unsigned char)Data;    
			        return;
				}

				default:
					return;

			}
		}
			
        /* analogue input channels */
        if ((Addr & 0x03ff8)==0x02808)  
        {
			ASIC_Data.ASIC_Ram[Addr] = ASIC_Data.AnalogueInputs[Addr & 0x07];
			return;
        }


        /* write colour palette */
        if ((Addr & 0x0ffc0)==0x02400)	
        {
                int             Index;

                Addr=Addr & 0x03ffe;
                Index= (Addr & 0x003f)>>1;

                /* Cliff's Test code checks for this */
                ASIC_Data.ASIC_Ram[Addr+1] &= 0x00f;
                        
                {
                        unsigned long PackedRGBLookup;

//#ifdef CPC_LSB_FIRST
//						PackedRGBLookup = ((unsigned long *)(ASIC_Data.ASIC_Ram + Addr))[0];
//#else
						PackedRGBLookup = ASIC_Data.ASIC_Ram[Addr] | ((ASIC_Data.ASIC_Ram[Addr+1])<<8);
//#endif
//						PackedRGBLookup = PackedRGBLookup & 0x0fff;
				
						Render_SetColour(&ASIC_DisplayColours[PackedRGBLookup], Index);

                }
                return;
        }

		if ((Addr & 0x0fff0)==0x02c00)
        {
			if (Addr==0x02c0f)
			{
                /* writing 1 to DMA int bits */

                /* clear dma interrupts */
                ASIC_ClearDMAInterruptsManual(Data);

                if ((Data & 0x07)!=0x07)
                {
						/* writing 0's to DMA enable bits */

                        /* disable channel(s) */

                        ASIC_DMA_DisableChannels((unsigned char)Data);
                }

                /* keep info about channels enabled and the channels interrupting */
                ASIC_Data.InternalDCSR = (unsigned char)((ASIC_Data.InternalDCSR & 0x070) | (Data & 0x07));

                ASIC_UpdateRAMWithInternalDCSR();


				return;
			}
			else
			{
				int ChannelIndex = (Addr & 0x0f)>>2;
        
                switch (Addr & 0x03)
                {
                        case 0:
                        {
                                ASIC_Data.DMA[ChannelIndex].Addr.Addr_B.l = (unsigned char)(Data & 0x0fe);
                        }
                        break;

                        case 1:
                        {
                                ASIC_Data.DMA[ChannelIndex].Addr.Addr_B.h = (unsigned char)Data;
                        }
                        break;

                        case 2:
                        {
                                ASIC_Data.DMA[ChannelIndex].Prescale = (unsigned char)Data;
                        }
                        break;
                }

                ASIC_UpdateRAMWithInternalDCSR();
        
	            return;
			}
		}

		/* write data for unused ram so that if a write and then a read is dopne the unused ram data byte will 
		be returned */
		ASIC_Data.ASIC_Ram[Addr] = ASIC_UNUSED_RAM_DATA;
}


/********************/
/* ASIC SOUND "DMA" */
/********************/

INLINE static Z80_WORD ASIC_DMA_GetOpcode(Z80_WORD Addr)
{
        return Z80_RD_BASE_WORD(Addr);
}

int ASIC_DMA_GetChannelPrescale(int ChannelIndex)
{
        /* get pre-scalar */
        return (ASIC_Data.DMA[ChannelIndex].Prescale);
}

int ASIC_DMA_GetChannelAddr(int ChannelIndex)
{
        return (ASIC_Data.DMA[ChannelIndex].Addr.Addr_W);
}

INLINE static void ASIC_DMA_WriteChannelAddr(int ChannelIndex, int Addr)
{
        ASIC_Data.DMA[ChannelIndex].Addr.Addr_W = (unsigned short)Addr;
}

void    ASIC_DMA_ExecuteCommand(int ChannelIndex)
{
        Z80_WORD        Command;
        int CommandOpcode;
        int     Addr;
        ASIC_DMA_CHANNEL        *pChannel = &ASIC_Data.DMAChannel[ChannelIndex];
	
        Addr = ASIC_Data.DMA[ChannelIndex].Addr.Addr_W;
		
        Command = ASIC_DMA_GetOpcode((Z80_WORD)(Addr & 0x0fffe));
        
        CommandOpcode = (Command & 0x07000)>>12;

        Addr = Addr+2;

        if (CommandOpcode == 0)
        {
                /* LOAD R,D */

                int     Register;
                int     Data;

                int PreviousRegisterSelected;

                /* PSG register */
                Register = (Command>>8) & 0x0f;
                /* data to write */
                Data = (Command & 0x0ff);

                /* store current selected PSG address register */
                PreviousRegisterSelected = PSG_GetSelectedRegister();

                /* select new register */
                PSG_RegisterSelect(Register);

                /* write data */
                PSG_WriteData(Data);

                /* select previous register again - not to upset other code */
                PSG_RegisterSelect(PreviousRegisterSelected);
        }
        else
        {
                if (CommandOpcode & 0x01)
                {
                        /* PAUSE n */

                        /* pause of 0 is equivalent to a NOP */
                        int PauseCount = Command & 0x0fff;

                        if (PauseCount-1!=0)
                        {
							pChannel->PauseCount = PauseCount-1;
                            pChannel->PrescaleCount = ASIC_Data.DMA[ChannelIndex].Prescale;
                            pChannel->PauseActive = TRUE;
                        }

                }

                if (CommandOpcode & 0x02)
                {
                        /* REPEAT n */

                        /* store repeat count */
                        pChannel->RepeatCount = Command & 0x0fff;

                        /* set next instruction as loop start */
                        pChannel->LoopStart = Addr & 0x0ffff;
                }

                if (CommandOpcode & 0x04)
                {
                        /* NOP, LOOP, INT, STOP */

                        if (Command & 0x0001)
                        {
                                /* LOOP */

                                /* if loop count is 0, this acts like a NOP */

                                /* check repeat count */
                                if (pChannel->RepeatCount!=0)
                                {
                                        /* decrement count */
                                        pChannel->RepeatCount--;
                                        
                                        /* reload channel addr from stored loop start */
                                        Addr = pChannel->LoopStart;
                                }

                        
                        }

                        if (Command & 0x0010)
                        {
                                /* INT */

                                /* set channel interrupt */
                                ASIC_Data.InternalDCSR|=1<<(6-ChannelIndex);
                        }

                        if (Command & 0x0020)
                        {
                                /* STOP */

                                /* stop channel */
                                ASIC_Data.InternalDCSR&=~(1<<ChannelIndex);
                        }
                }
        }

		ASIC_DMA_WriteChannelAddr(ChannelIndex, Addr);

/*        ASIC_Data.DMA[ChannelIndex].Addr.Addr_W=(unsigned short)Addr; */

        ASIC_UpdateRAMWithInternalDCSR();

}


void    ASIC_DMA_HandleChannel(int ChannelIndex)
{
        ASIC_DMA_CHANNEL        *pChannel = &ASIC_Data.DMAChannel[ChannelIndex];

        if (pChannel->PauseActive)
        {
                /* is prescale count = 0? i.e. count has finished? */
                if (pChannel->PrescaleCount==0)
                {
                        /* channel prescale count finished */

                        /* get new prescale */
                        pChannel->PrescaleCount = ASIC_Data.DMA[ChannelIndex].Prescale;

                        /* is there a pause active? */
                        if (pChannel->PauseCount == 0)
                        {
                                pChannel->PauseActive = FALSE;
                        }
                        else
                        {
                                pChannel->PauseCount--;
                        }
                }
                else
                {
                        /* update pre-scale count */
                        pChannel->PrescaleCount--;
                }
        }
	
        if (!(pChannel->PauseActive))
        {
                /* execute DMA command */
                ASIC_DMA_ExecuteCommand(ChannelIndex);
        }
}

#if 0
void    ASIC_DMA_EnableChannel(int ChannelIndex)
{
}
#endif

static void    ASIC_DMA_DisableChannel(int ChannelIndex)
{
        ASIC_DMA_CHANNEL        *pChannel = &ASIC_Data.DMAChannel[ChannelIndex];

        /* set pause and prescale to 0 */
        /* at the end of the next HSYNC, the first opcode in the list
        will be executed for this channel, and the prescalar will be re-loaded */
        pChannel->PrescaleCount = 0; 
        pChannel->PauseCount = 0;
        pChannel->PauseActive = FALSE;
        pChannel->RepeatCount = 0;
}



#if 0
void    ASIC_DMA_EnableChannels(unsigned char Data)
{
        if (Data & 0x01)
        {
                ASIC_DMA_EnableChannel(0);
        }

        if (Data & 0x02)
        {
                ASIC_DMA_EnableChannel(1);
        }

        if (Data & 0x04)
        {
                ASIC_DMA_EnableChannel(2);
        }
}
#endif

void    ASIC_DMA_DisableChannels(unsigned char Data)
{
        if ((Data & 0x01)==0)
        {
                ASIC_DMA_DisableChannel(0);
        }

        if ((Data & 0x02)==0)
        {
                ASIC_DMA_DisableChannel(1);
        }

        if ((Data & 0x04)==0)
        {
                ASIC_DMA_DisableChannel(2);
        }
}



void    ASIC_DoDMA()
{
	if ((ASIC_Data.InternalDCSR & (0x001|0x002|0x04))==0)
		return;

        if (ASIC_Data.InternalDCSR & 0x001)
        {
                /* channel 0 is enabled */
                ASIC_DMA_HandleChannel(0);
        }

        if (ASIC_Data.InternalDCSR & 0x002)
        {
                /* channel 1 is enabled */
                ASIC_DMA_HandleChannel(1);
        }

        if (ASIC_Data.InternalDCSR & 0x004)
        {
                /* channel 2 is enabled */
                ASIC_DMA_HandleChannel(2);
        }
}

/* return TRUE if raster ints are enabled, FALSE if not */
BOOL    ASIC_RasterIntEnabled()
{
        if (ASIC_Data.ASIC_RasterInterruptLine!=0)        
                return TRUE;
        else
                return FALSE;
}


BOOL    ASIC_SpritesActive()
{
        if (ASIC_Data.SpriteEnableMask!=0)
        {
                return TRUE;
        }
        else
        {
                return FALSE;
        }
}

void	ASIC_GenerateSpriteActiveMaskForLine(int Line)
{
        unsigned long SpriteActiveMask = 0;
        int i;
		unsigned long EnableTestMask = (1<<15);

        if ((ASIC_Data.Flags & ASIC_ENABLED)==0)	
                return;

        CurrentLine = Line;

        /* do each sprite in turn */
        for (i=15; i>=0; i--)
        {
                /* enabled ? */
                if (ASIC_Data.SpriteEnableMask & EnableTestMask)	
                {
                        /* active on this line */
                        if ((Line>=ASIC_Data.SpriteInfo[i].SpriteMinYPixel) && (Line<ASIC_Data.SpriteInfo[i].SpriteMaxYPixel))
                        {
                                /* say it's active in the mask */
                                SpriteActiveMask |= EnableTestMask;
                        }
                }
        
			EnableTestMask = EnableTestMask>>1;
		}

        /* return mask */
        ASIC_Data.SpriteEnableMaskOnLine = SpriteActiveMask;

/*		return SpriteActiveMask; */
}


#include "yiq.h"


void    ASIC_InitialiseMonitorColourModes()
{
        int i;

        for (i=0; i<4096; i++)
        {
                unsigned char Red, Green, Blue;
/*                unsigned long PackedRGB; */


                /* R,G,B that would be represented by this colour index */
                Red = (unsigned char)((i>>4) & 0x0f);
                Green = (unsigned char)((i>>8) & 0x0f);
                Blue = (unsigned char)((i) & 0x0f);


                /* generate a 8:8:8 packed RGB */
                /* store colour as is without conversion */
                ASIC_PackedRGBColour[i].u.element.Red = Red;
				ASIC_PackedRGBColour[i].u.element.Green = Green;
				ASIC_PackedRGBColour[i].u.element.Blue = Blue;


                {
                        RGB_CHAR        SourceRGB;
                        RGB_CHAR        GreyScaleRGB;

                        SourceRGB.R = (unsigned char)(Red<<4);
                        SourceRGB.G = (unsigned char)(Green<<4);
                        SourceRGB.B = (unsigned char)(Blue<<4);

                        /* generate grey scale for this colour */
                        BrightnessControl_GenerateGreyScaleFromColour(&SourceRGB, &GreyScaleRGB);
#if 0                        
                        /* store grey scale */
                        ASIC_PackedRGBGreyScale[i] = (((GreyScaleRGB.R & 0x0ff)<<16) |
                                                     ((GreyScaleRGB.G & 0x0ff)<<8) |
                                                     (GreyScaleRGB.B & 0x0ff));
#endif
					ASIC_PackedRGBGreyScale[i].u.element.Red = GreyScaleRGB.R;
					ASIC_PackedRGBGreyScale[i].u.element.Green = GreyScaleRGB.G;
					ASIC_PackedRGBGreyScale[i].u.element.Blue = GreyScaleRGB.B;

				}
        }
}


void    ASIC_SetMonitorColourMode(MONITOR_COLOUR_MODE MonitorMode)
{
        int i;

        switch (MonitorMode)
        {
                case MONITOR_MODE_COLOUR:
                {
                        /* int Brightness = CPC_GetMonitorBrightness(); */
					
                        for (i=0; i<4096; i++)
                        {
                                unsigned char Red, Green, Blue;
/*                                unsigned long PackedRGB; */
                                RGB_CHAR SourceRGB, DestRGB;


                /* R,G,B that would be represented by this colour index */
                Red = (unsigned char)((i>>4) & 0x0f);
                Green = (unsigned char)((i>>8) & 0x0f);
                Blue = (unsigned char)((i) & 0x0f);


                                /* R,G,B that would be represented by this colour index */

                                /* as 8:8:8 */
                                SourceRGB.R = (unsigned char)(Red<<4);
                                SourceRGB.G = (unsigned char)(Green<<4);
                                SourceRGB.B = (unsigned char)(Blue<<4);

                              /* adjust it's brightness */
/*                              BrightnessControl_AdjustBrightness(&SourceRGB, &DestRGB, Brightness); */

								DestRGB.R = SourceRGB.R;
								DestRGB.G = SourceRGB.G;
								DestRGB.B = SourceRGB.B;

							ASIC_PackedRGBColour[i].u.element.Red = DestRGB.R;
							ASIC_PackedRGBColour[i].u.element.Green = DestRGB.G;
							ASIC_PackedRGBColour[i].u.element.Blue = DestRGB.B;
                        }

						memcpy(&ASIC_DisplayColours, &ASIC_PackedRGBColour, 4096*sizeof(unsigned long));

              }
                break;
                
                case MONITOR_MODE_GREEN_SCREEN:
                case MONITOR_MODE_GREY_SCALE:
                {
              			memcpy(&ASIC_DisplayColours, &ASIC_PackedRGBGreyScale, 4096*sizeof(unsigned long));

                }
                break;
        }

	ASIC_UpdateColours();
}

void	ASIC_UpdateColours(void)
{        
    if (ASIC_Data.ASIC_Ram!=NULL)
    {
		int i;

        /* update colours using colour scale */
        for (i=0; i<32; i++)
        {

            unsigned long PackedRGBLookup;

#ifdef CPC_LSB_FIRST
						PackedRGBLookup = ((unsigned long *)(ASIC_Data.ASIC_Ram + 0x02400 + (i<<1)))[0];
#else
						PackedRGBLookup = ASIC_Data.ASIC_Ram[0x02400 + (i<<1)] | ((ASIC_Data.ASIC_Ram[0x02400 + (i<<1) + 1])<<8);
#endif
						PackedRGBLookup = PackedRGBLookup & 0x0fff;

						/* set the colour */
            Render_SetColour(&ASIC_DisplayColours[PackedRGBLookup],/*Red,Green,Blue,*/ i);
        }
    }
}

