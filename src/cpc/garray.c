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
#include "garray.h"
#include "asic.h"
#include "render.h"
#include "multface.h"
#include "crtc.h"
#include "cpc.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

/* these point to the actual ram/rom that the CPU can see 
for reading and writing operations */
unsigned char   *pReadRamPtr[8];
unsigned char   *pWriteRamPtr[8];

GATE_ARRAY_STATE	GateArray_State;


extern unsigned char *RamConfigurationTable[64*4];
extern unsigned char *pUpperRom;
extern int LowerRomIndex;
extern char *pLowerRom;

/* actual colours to display */
RGBCOLOUR       DisplayColours[32];

/* base hardware colours */
static RGBCOLOUR        HardwareColours[32] =
{
		{0x060,0x060,0x060,0},                     /* r1 g1 b1     White */
        {0x060,0x060,0x060,0},                     /* -------- ** White ** */
        {0x00,0x0ff,0x060,0},                    /* r0 g2 b1 Sea Green */
        {0x0ff,0x0ff,0x060,0},                   /* r2 g2 b1 Pastel yellow */
        {0x00,0x00,0x060,0},                     /* r0 g0 b1 Blue */
        {0x0ff,0x00,0x060,0},                    /* r2 g0 b1 Purple */
        {0x00,0x060,0x060,0},                     /* r0 g1 b1 Cyan */
        {0x0ff,0x060,0x060,0},                    /* r2 g1 b1 Pink */
        {0x0ff,0x00,0x060,0},                    /* -------- ** Purple ** */
        {0x0ff,0x0ff,0x060,0},                   /* -------- ** Pastel yellow ** */
        {0x0ff,0x0ff,0x00,0},                   /* r2 g2 b0 Bright Yellow */
        {0x0ff,0x0ff,0x0ff,0},                  /* r2 g2 b2 Bright White */
        {0x0ff,0x00,0x00,0},                    /* r2 g0 b0 Bright Red */
        {0x0ff,0x00,0x0ff,0},                   /* r2 g0 b2 Bright Magenta */
        {0x0ff,0x060,0x00,0},                    /* r2 g1 b0 Orange */
        {0x0ff,0x060,0x0ff,0},                   /* r2 g1 b2 Pastel Magenta */
        {0x00,0x00,0x060,0},                     /* -------- ** Blue ** */
        {0x00,0x0ff,0x060,0},                    /* -------- ** Sea Green ** */
        {0x00,0x0ff,0x00,0},                    /* r0 g2 b0 Bright green */
        {0x00,0x0ff,0x0ff,0},                   /* r0 g2 b2 Bright Cyan */
        {0x00,0x00,0x00,0},                     /* r0 g0 b0 Black */
        {0x00,0x00,0x0ff,0},                    /* r0 g0 b2 Bright Blue */
        {0x00,0x060,0x00,0},                     /* r0 g1 b0 Green */
        {0x00,0x060,0x0ff,0},                    /* r0 g1 b2 Sky Blue */
        {0x060,0x00,0x060,0},                     /* r1 g0 b1 Magenta */
        {0x060,0x0ff,0x060,0},                    /* r1 g2 b1 Pastel green */
         {0x060,0x0ff,0x00,0},                    /* r1 g2 b0 Lime */
        {0x060,0x0ff,0x0ff,0},                   /* r1 g2 b2 Pastel cyan */
        {0x060,0x00,0x00,0},                     /* r1 g0 b0 Red */
        {0x060,0x00,0x0ff,0},                    /* r1 g0 b2 Mauve */
        {0x060,0x060,0x00,0},                     /* r1 g1 b0 Yellow */
        {0x060,0x060,0x0ff,0}                             /* r1 g1 b2 Pastel blue */
};

/* the green brightness is equal to the firmware colour index */
static unsigned char FirmwareColourIndex[32] =
{
        13,
        13,
        19,
        25,
        1,
        7,
        10,
        16,
        7,
        25,
        24,
        26,
        6,
        8,
        15,
        17,
        1,
        19,
        18,
        20,
        0,
        2,
        9,
        11,
        4,
        22,
        21,
        23,
        3,
        5,
        12,
        14
};

#ifdef DEBUG_MODE
char *HardwareColourNames[32] =
{
        "White",
        "White",
        "Sea Green",
        "Pastel Yellow",
        "Blue",
        "Purple",
        "Cyan",
        "Pink",
        "Purple",
        "Pastel Yellow",
        "Bright Yellow",
        "Bright White",
        "Bright Red",
        "Bright Magenta",
        "Orange",
        "Pastel Magenta",
        "Blue",
        "Sea Green",
        "Bright Green",
        "Bright Cyan",
        "Black",
        "Bright Blue",
        "Green",
        "Sky Blue",
        "Magenta",
        "Pastel Green",
        "Lime",
        "Pastel Cyan",
        "Red",
        "Mauve",
        "Yellow",
        "Pastel Blue"
};
#endif

static RGBCOLOUR        HardwareColoursGreen[32];

static RGBCOLOUR        HardwareColoursGreyScale[32];

static RGBCOLOUR        HardwareColoursBrightnessAdjusted[32];


#include "yiq.h"

/* adjust the colours depending on the brightness setting of the
monitor */
void	GateArray_AdjustMonitorBrightness(void)
{
	int i;
/*    int Brightness = CPC_GetMonitorBrightness(); */

	/* colour monitor brightness adjust */
    for (i=0; i<32; i++)
    {
		unsigned char Red, Green, Blue;
	   RGB_CHAR SourceRGB, DestRGB;
 
		/* R,G,B that would be represented by this colour index  */
		Red = HardwareColours[i].u.element.Red;
		Green = HardwareColours[i].u.element.Green;
		Blue = HardwareColours[i].u.element.Blue;

		SourceRGB.R = Red;
		SourceRGB.G = Green;
		SourceRGB.B = Blue;

        /* adjust it's brightness */
/*		BrightnessControl_AdjustBrightness(&SourceRGB, &DestRGB, Brightness); */

		DestRGB.R = SourceRGB.R;
		DestRGB.G = SourceRGB.G;
		DestRGB.B = SourceRGB.B;

		HardwareColoursBrightnessAdjusted[i].u.element.Red = DestRGB.R;
		HardwareColoursBrightnessAdjusted[i].u.element.Green = DestRGB.G;
		HardwareColoursBrightnessAdjusted[i].u.element.Blue = DestRGB.B;
    }
}


/* set monitor colour mode */
void    GateArray_SetMonitorColourMode(MONITOR_COLOUR_MODE      MonitorMode)
{
        int i;

        switch (MonitorMode)
        {
                case MONITOR_MODE_COLOUR:
                {
						GateArray_AdjustMonitorBrightness();
				
						memcpy(&DisplayColours, &HardwareColoursBrightnessAdjusted, sizeof(RGBCOLOUR)*32);

				} 
                break;

                case MONITOR_MODE_GREEN_SCREEN:
                {
					memcpy(&DisplayColours, &HardwareColoursGreen, sizeof(RGBCOLOUR)*32);
                }
                break;

                case MONITOR_MODE_GREY_SCALE:
                {
					memcpy(&DisplayColours, &HardwareColoursGreyScale, sizeof(RGBCOLOUR)*32);
                }
                break;

        }

        /* update all colours in correct monitor mode. These will
        take effect at next rendering. */
        for (i=0; i<17; i++)
        {
                int Red, Green,Blue;
                int HwColourIndex;

                /* get hardware colour index for this pen */
                HwColourIndex = GateArray_State.PenColour[i];

                /* get R,G,B from either colour, green screen etc colour table */
                Red = DisplayColours[HwColourIndex].u.element.Red;
                Green = DisplayColours[HwColourIndex].u.element.Green;
                Blue = DisplayColours[HwColourIndex].u.element.Blue;

                /* set colour */
                Render_SetColour(&DisplayColours[HwColourIndex],/*Red,Green,Blue,*/i);
        }
}

void    GateArray_Initialise(void)
{
        int i;

        unsigned long GreenFraction = (255<<16)/26;

        /* setup green screen colours from hardware colours */
        for (i=0; i<32; i++)
        {
                /* green colour intensity is the same as the firmware colour
                index */
                int GreenIntensityInt = FirmwareColourIndex[i];
                
                HardwareColoursGreen[i].u.element.Red = 0;
                HardwareColoursGreen[i].u.element.Blue = 0;
                HardwareColoursGreen[i].u.element.Green = (char)((GreenIntensityInt*GreenFraction)>>16);
        }

        {
                #include "yiq.h"

                for (i=0; i<32; i++)
                {
                        RGB_CHAR SourceColour;
                        RGB_CHAR GreyScaleColour;

                        SourceColour.R = HardwareColours[i].u.element.Red;
                        SourceColour.G = HardwareColours[i].u.element.Green;
                        SourceColour.B = HardwareColours[i].u.element.Blue;

                        BrightnessControl_GenerateGreyScaleFromColour(&SourceColour, &GreyScaleColour);
                        
                        HardwareColoursGreyScale[i].u.element.Red = GreyScaleColour.R;
                        HardwareColoursGreyScale[i].u.element.Green = GreyScaleColour.G;
                        HardwareColoursGreyScale[i].u.element.Blue = GreyScaleColour.B;
                }
        }

        GateArray_State.pChosenRamConfig = &RamConfigurationTable[0];


}

void    GateArray_Reset(void)
{
		/* CPC+ or KC Compact do not set colours to black on reset */
        if ((CPC_GetHardware()!=CPC_HW_CPCPLUS) && (CPC_GetHardware()!=CPC_HW_KCCOMPACT))
		{
			int     i;

			/* set colours to black, including border */
			for (i=0; i<17; i++)
			{
					GateArray_State.PenColour[i] = 20;
					Render_SetColour(&HardwareColours[20],i);
			}
		}

	    /* setup main ram */
        GateArray_Write(0x0c0);

        /* set mode and rom enable register to zero, enabling both
        halves of the rom */
        GateArray_Write(0x080 | 0x00);

        Render_SetPixelTranslation(0);
}

void	Vortex_RethinkMemory(void);

void	GateArray_RethinkMemory(void)
{
	unsigned char RomConfiguration = GateArray_State.RomConfiguration;

    pWriteRamPtr[0] = pWriteRamPtr[1] = pReadRamPtr[0] = pReadRamPtr[1] = GateArray_State.pChosenRamConfig[0];
	pWriteRamPtr[2] = pWriteRamPtr[3] = pReadRamPtr[2] = pReadRamPtr[3] = GateArray_State.pChosenRamConfig[1];
	pWriteRamPtr[4] = pWriteRamPtr[5] = pReadRamPtr[4] = pReadRamPtr[5] = GateArray_State.pChosenRamConfig[2];

	{
		unsigned char *pAddrC000 = GateArray_State.pChosenRamConfig[3];

		pWriteRamPtr[6] = pWriteRamPtr[7] = pAddrC000;

		if (!(RomConfiguration & 0x008))
		{
			pAddrC000 = pUpperRom;
			pReadRamPtr[6] = pReadRamPtr[7] = pAddrC000;

			RAM_ROM_RethinkMemory(pReadRamPtr, pWriteRamPtr);
		}
		else
		{
			pReadRamPtr[6] = pReadRamPtr[7] = pAddrC000;
		}

	}

	if (CPC_GetHardware()==CPC_HW_CPCPLUS)
	{
		ASIC_GateArray_RethinkMemory(pReadRamPtr,pWriteRamPtr);
	}

    if (!(RomConfiguration & 0x004))
    {
  		pReadRamPtr[LowerRomIndex] = pReadRamPtr[LowerRomIndex+1] = (unsigned char *)pLowerRom;
    }


#ifdef MULTIFACE
    Multiface_SetMemPointers(pReadRamPtr, pWriteRamPtr);
#endif

/*	Vortex_RethinkMemory(); */


}

int             GateArray_GetPaletteColour(int PenIndex)
{
        return GateArray_State.PenColour[PenIndex];
}

int             GateArray_GetSelectedPen(void)
{
        return GateArray_State.PenSelection;
}

int             GateArray_GetRamConfiguration(void)
{
        return GateArray_State.RamConfiguration;
}

int             GateArray_GetMultiConfiguration(void)
{
        return GateArray_State.RomConfiguration;
}



void    GateArray_ClearInterruptCount(void)
{
    /* reset interrupt line count */
    GateArray_State.InterruptLineCount = 0;
}

void	GateArray_SetInterruptLineCount(int Count)
{
	GateArray_State.InterruptLineCount = Count;
}

int		GateArray_GetInterruptLineCount(void)
{
	return GateArray_State.InterruptLineCount;
}


/* This is called when a OUT to Gate Array rom configuration/mode
select register, bit 4 is set */
void    GateArray_ClearInterrupt(void)
{
		Z80_ClearInterruptRequest();
	
		GateArray_ClearInterruptCount();

		ASIC_ClearRasterInterrupt();

}

/*---------------------------------------------------------------------------*/

/* called when Z80 acknowledges interrupt */
void    GateArray_AcknowledgeInterrupt(void)
{
        /* reset top bit of interrupt line counter */
        /* this ensures that the next interrupt is no closer than 32 lines */
        GateArray_State.InterruptLineCount &= 0x01f;
}

void	GateArray_Interrupt(void)
{
	if (CPC_GetHardware()==CPC_HW_CPCPLUS)
	{
		/* CPC+ mode */

		/* if ASIC raster interrupts are not enabled... */
		if (!ASIC_RasterIntEnabled())
		{
			/* CPC+ mode. Mark this interrupt as a Raster int */
			ASIC_SetRasterInterrupt();
                        
			/* generate a interrupt */
			Z80_SetInterruptRequest();
		}
		return;
	}

	/* CPC Mode. Generate a interrupt */
	Z80_SetInterruptRequest();
}

void	GateArray_TriggerVsyncSynchronisation(void)
{
	GateArray_State.InterruptSyncCount = 2;
}

int	GateArray_GetVsyncSynchronisationCount(void)
{
	return 2-GateArray_State.InterruptSyncCount;
}

void	GateArray_SetVsyncSynchronisationCount(int Count)
{
	GateArray_State.InterruptSyncCount = 2-Count;
}


void	GateArray_Update(void)
{
    /* set pixel translation table which is dependant on mode */
    Render_SetPixelTranslation(GateArray_State.RomConfiguration & 0x03);

	/* increment interrupt line count */
	GateArray_State.InterruptLineCount++;

	if (GateArray_State.InterruptSyncCount==0)
	{
		/* if line == 52 then interrupt should be triggered */
		if (GateArray_State.InterruptLineCount==52)
		{
			/* clear counter. */
			GateArray_State.InterruptLineCount = 0;

			GateArray_Interrupt();

		
		}
	}
	else
	{
		GateArray_State.InterruptSyncCount--;

		/* "2 scans into the VSYNC signal..." */
		if (GateArray_State.InterruptSyncCount==0)
		{
			/* the case where InterruptLineCount == 0, should be covered by */
			/* code above */

			/* has interrupt line counter overflowed? */
			if (GateArray_State.InterruptLineCount>=32)
			{
					/* following might not be required, because it is probably */
					/* set by the code above */
                
					GateArray_Interrupt();
			}

			/* reset interrupt line count */
			GateArray_State.InterruptLineCount = 0; 
		}
	}
}



void    GateArray_Write(int Function)
{
    switch (Function & 0x0c0)
    {
		case 0x000:
		{
			/* function 00xxxxxx */
			GateArray_State.PenSelection = (unsigned char)Function;
			
			if (Function & 0x010)
			{
				GateArray_State.PenIndex = (unsigned char)16;
			}
			else
			{
				GateArray_State.PenIndex = (unsigned char)(Function & 0x0f);
			}
		}
		return;

		case 0x040:
		{
				
			/* function 01xxxxxx */

			int     PenIndex;
			int		ColourIndex;
			
			GateArray_State.ColourSelection = (unsigned char)Function;

			ColourIndex = Function & 0x01f;
			
			PenIndex = GateArray_State.PenIndex;

			GateArray_State.PenColour[PenIndex] = (unsigned char)ColourIndex;

			/* update ASIC palette ram */
			ASIC_GateArray_UpdatePaletteRam((unsigned char)PenIndex, (unsigned char)ColourIndex);

			Render_SetColour(&DisplayColours[ColourIndex],PenIndex);
		}
		return;

		case 0x080:
        {
			/* function 10xxxxxx */

			/* check for secondary rom mapping */				
            if (CPC_GetHardware()==CPC_HW_CPCPLUS)
			{
				if (ASIC_GateArray_CheckForSecondaryRomMapping((unsigned char)Function))
				{
					return;
				}
            }

			GateArray_State.RomConfiguration = (unsigned char)Function;

			/* clear interrupt counter? */
            if (Function & 0x010)
            {
                GateArray_ClearInterrupt();
            }
		}
		break;

		case 0x0c0:
        {
            /* function 11xxxxxx */
            int     Config;

            GateArray_State.RamConfiguration = (unsigned char)Function;

            Config = Function & 0x03f;
            Config = Config<<2;

            GateArray_State.pChosenRamConfig = &RamConfigurationTable[Config];
			break;
		}
	}

	GateArray_RethinkMemory();
}


/* return red colour component for palette index specified */
int             GateArray_GetRed(int PaletteIndex)
{
        int HwColourIndex = GateArray_State.PenColour[PaletteIndex & 0x01f];

        return HardwareColours[HwColourIndex].u.element.Red;
}


/* return green colour component for palette index specified */
int             GateArray_GetGreen(int PaletteIndex)
{
        int HwColourIndex = GateArray_State.PenColour[PaletteIndex & 0x01f];

        return HardwareColours[HwColourIndex].u.element.Green;
}

/* return blue colour component for palette index specified */
int             GateArray_GetBlue(int PaletteIndex)
{
        int HwColourIndex = GateArray_State.PenColour[PaletteIndex & 0x01f];
        
        return HardwareColours[HwColourIndex].u.element.Blue;
}

void	GateArray_UpdateColours(void)
{
	int i;

	for (i=0; i<17; i++)
	{
        int Red,Green,Blue;
		int HwColourIndex;

		HwColourIndex = GateArray_State.PenColour[i & 0x01f];

        Red = DisplayColours[HwColourIndex].u.element.Red;
        Green = DisplayColours[HwColourIndex].u.element.Green;
        Blue = DisplayColours[HwColourIndex].u.element.Blue;

        Render_SetColour(&DisplayColours[HwColourIndex],/*Red,Green,Blue,*/i);
	}
}


