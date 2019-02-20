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
        MULTIFACE II EMULATION

  1. Multiface is enabled when CPC has been reset.

  2. When Multiface is enabled, OUT &FEE8 and &FEEA are active. The
  ram/rom can be paged in and out of memory space (&0000-&3fff)

  3. On the original Multiface device there was two coloured buttons
  on top. The first was yellow, which reset the CPC, and a second
  was red which stopped the computer and gave control to the multiface.
  The little red button is the Multiface Stop Button. The multiface
  can have control regardless of whether it is enabled or not.

  4. When the stop button is pressed, the multiface ram/rom is paged
  into the memory address space and a NMI is caused.

  5. The Multiface had 3 PAL (Programmable Address Logic) IC's on the circuit
  board, these converted OUT style commands to writes
  into the Multiface RAM. Therefore when the Multiface was stopped
  it had a record of the h/w state in it's ram, and could restore
  it when it gave back control.

*/

#include "multface.h"
#include "cpcglob.h"
#include "headers.h"
#include "garray.h"
#include "z80/z80.h"
#include "cpc.h"


static MULTIFACE_MODE Multiface_StoredMode = MULTIFACE_CPC_MODE;

static void    Multiface_SetRamState(int RamState);

/* flags */
static unsigned long Multiface_Flags = 0;

/* multiface occupies memory &0000-&4000. &0000-&1fff is ROM, 0x02000-0x03fff is RAM */
/* lower 8k is multiface rom */
static unsigned char *MultifaceRom = NULL;

/* CPC version */
static unsigned char *MultifaceRom_CPC = NULL;
/* CPC Plus version */
static unsigned char *MultifaceRom_CPCPLUS = NULL;

/* upper 8k is multiface ram */
static unsigned char *MultifaceRam = NULL;

/* when a write to the I/O is performed, this is called. The I/O writes are therefore
trapped by the Multiface 2 */
void	Multiface_WriteIO(unsigned short Port, unsigned char Data)
{

	if (!(Multiface_Flags & MULTIFACE_IS_ENABLED))
		return;

	if ((Port & 0x0fffd) == 0x0fee8)
	{
		/* set ram state */
		Multiface_SetRamState((((~Port) & 0x02)>>1));
	}

	{
		unsigned char PortHighByte = (unsigned char)(Port>>8);


		/* Gate Array Write */
		if (PortHighByte==(unsigned char)0x07f)
		{
			/* write gate array data - pen select, colour data, rom/mode, ram config*/
			if ((Data & 0x0c0)==0x040)
			{
				/* get selected pen */
				int PenIndex = MultifaceRam[0x01fcf];

				/* write to border or pen array */
				MultifaceRam[0x01f90 | ((PenIndex & 0x010)<<2) | (PenIndex & 0x0f)] = Data;
			}
			else
			{
				MultifaceRam[0x01fcf | ((Data & 0x0c0)>>2)] = Data;
			}
		}

		/* CRTC write */
		if (PortHighByte==(unsigned char)0x0bc)
		{
			/* store reg index */
			MultifaceRam[0x01cff] = Data;
		}
		
		if (PortHighByte==(unsigned char)0x0bd)
		{
			int CRTCRegIndex;

			/* get reg index */
			CRTCRegIndex = MultifaceRam[0x01cff];
			/* write reg data */
			MultifaceRam[(0x01db0 + (CRTCRegIndex & 0x0f))] = Data;
		}

		if (PortHighByte==(unsigned char)0x0f7)
		{
			MultifaceRam[0x017ff] = Data;
		}

		if (PortHighByte==(unsigned char)0x0df)
		{
			MultifaceRam[0x01aac] = Data;
		}
	}
}


/* call here when Multiface stop button is pressed */
void    Multiface_Stop(void)
{
	/* multiface enabled? */
    if (!(Multiface_Flags & MULTIFACE_IS_ENABLED))
            return;

	/* stop button pressed already? */
	if (Multiface_Flags & MULTIFACE_STOP_BUTTON_PRESSED)
		return;

    /* no, but it is pressed now, and enable ram too */
    Multiface_Flags |= MULTIFACE_STOP_BUTTON_PRESSED | MULTIFACE_FLAGS_RAM_ENABLED;
	
    /* setup Z80 emulation for NMI */
    Z80_NMI();

    /* enable lower rom */
    GateArray_Write(0x089);
}

/* call here when memory paging is done */
void    Multiface_SetMemPointers(unsigned char **pReadRamPtr, unsigned char **pWriteRamPtr)
{
    /* is multiface ram/rom enabled? */
    if (Multiface_Flags & MULTIFACE_FLAGS_RAM_ENABLED)
    {
		if ((MultifaceRam[(0x01fcf | (0x080>>2))] & (1<<2))==0)
		{
			/* put in multiface rom/ram pointer in memory space */
			pReadRamPtr[1] = pWriteRamPtr[1] = MultifaceRam-8192;
			pReadRamPtr[0] = MultifaceRom;
		}
    }
}

#if 0
/* call here when Z80 PC = 0x0065 and Multiface Stop has been pressed */
/* first time in, enables multiface, next time in disables, next time
enables etc */
void    Multiface_ToggleState(void)
{
        /* do we want to disable this time? */
        if (!(Multiface_ToggleStateDisable))
        {
                /* enable it */
                Multiface_EnabledFlag = TRUE;
                /* want to disable it next time around */
                Multiface_ToggleStateDisable = TRUE;
        }
        else
        {
                /* disable it */
                Multiface_EnabledFlag = FALSE;
                /* want to enable it next time around */
                Multiface_ToggleStateDisable = TRUE;
        }
}
#endif

/* call here when a out 0x0fee8/0x0feea is done */
void    Multiface_SetRamState(int RamState)
{
	if (RamState!=0)
	{
		/* enable ram */
		Multiface_Flags |= MULTIFACE_FLAGS_RAM_ENABLED;
	}
	else
	{
		Multiface_Flags &= ~MULTIFACE_FLAGS_RAM_ENABLED;
	}

	/* re-setup memory pointers */
	GateArray_RethinkMemory();
}

void	Multiface_Initialise(void)
{
	/* the ram is only 8k in size, but we allocate an extra 8k for "under the rom",
	so that writes to the rom area are ignored and do not write to the ram underneath */
	MultifaceRam = malloc(16384);

	if (MultifaceRam!=NULL)
	{
		memset(MultifaceRam, 0, 16384);
	}

	MultifaceRom = MultifaceRom_CPC = MultifaceRom_CPCPLUS = NULL;
	Multiface_Flags = 0;
}

CPCPortWrite multifacePortWrite={0x0000,0x0000, Multiface_WriteIO};


void	Multiface_Install(void)
{

	Multiface_Initialise();

	/* install function to capture I/O port writes */
	CPC_InstallWritePort(&multifacePortWrite);
	/* install function to reset multiface at system reset */
	CPC_InstallResetFunction(Multiface_Reset);
}


void	Multiface_DeInstall(void)
{
	Multiface_Finish();
}

static void Multiface_FreeCPCMultifaceROM(void)
{
    if (MultifaceRom_CPC!=NULL)
    {
        /* free memory */
        free(MultifaceRom_CPC);
        
        MultifaceRom_CPC = NULL;
    }

	Multiface_Flags &= ~MULTIFACE_FLAGS_CPC_ROM_LOADED;

}

static void Multiface_FreeCPCPLUSMultifaceROM(void)
{
	if (MultifaceRom_CPCPLUS!=NULL)
	{
		/* free rom */
		free(MultifaceRom_CPCPLUS);
		MultifaceRom_CPCPLUS = NULL;
	}

	Multiface_Flags &= ~MULTIFACE_FLAGS_CPCPLUS_ROM_LOADED;

}

/* loads a rom for multiface emulation */
int Multiface_SetRomData(const MULTIFACE_ROM_TYPE RomType, const unsigned char *pRomData, const unsigned long RomDataSize)
{
	int State;
	unsigned long CopyLength;

	/* support AMSDOS header */

	CopyLength = 8192;
	if (RomDataSize<CopyLength)
		CopyLength = RomDataSize;

	if (pRomData!=NULL)
	{
		switch (RomType)
		{
			/* CPC version */
			case MULTIFACE_ROM_CPC_VERSION:
			{
				/* free old rom data if it exists */
				Multiface_FreeCPCMultifaceROM();
				
				MultifaceRom_CPC = (unsigned char *)malloc(8192);

				if (MultifaceRom_CPC!=NULL)
				{
					memcpy(MultifaceRom_CPC, pRomData, CopyLength);
				
					Multiface_Flags |= MULTIFACE_FLAGS_CPC_ROM_LOADED;

					State = ARNOLD_STATUS_OK;
				}
				else
				{
					State = ARNOLD_STATUS_OUT_OF_MEMORY;
				}
			}
			break;

			/* CPC Plus version */
			case MULTIFACE_ROM_CPCPLUS_VERSION:
			{
				/* free old rom data if it exists */
				Multiface_FreeCPCPLUSMultifaceROM();

				MultifaceRom_CPCPLUS = (unsigned char *)malloc(8192);

				if (MultifaceRom_CPCPLUS!=NULL)
				{
					memcpy(MultifaceRom_CPCPLUS, pRomData, CopyLength);
					
					Multiface_Flags |= MULTIFACE_FLAGS_CPCPLUS_ROM_LOADED;
					
					State = ARNOLD_STATUS_OK;
				}
				else
				{
					State = ARNOLD_STATUS_OUT_OF_MEMORY;
				}
			}
			break;

			default:
				break;
		}
	}

	Multiface_SetMode(Multiface_StoredMode);

	return State;
}

/* finish multiface emulation */
void    Multiface_Finish(void)
{
	/* free rom loaded for CPC version */
	Multiface_FreeCPCMultifaceROM();

	/* free rom loaded for CPC PLUS version */
	Multiface_FreeCPCPLUSMultifaceROM();

	/* free multiface ram */
	if (MultifaceRam!=NULL)
	{
		/* free ram */
		free(MultifaceRam);
		MultifaceRam = NULL;
	}

	Multiface_Flags = 0;
}

/* As with a real Multiface, when the CPC is reset, the multiface
is enabled. If the user has set the Multiface emulation to be active,
then the multiface will be enabled when it is reset, otherwise it will
not */

void    Multiface_Reset(void)
{
	/* stop button not pressed */
	Multiface_Flags &= ~MULTIFACE_STOP_BUTTON_PRESSED;

	/* disable the ram */
	Multiface_SetRamState(0x0);
}

/* true if emulation is enabled/disabled */
BOOL	Multiface_IsEmulationEnabled(void)
{
	return ((Multiface_Flags & MULTIFACE_ENABLE_OVERRIDE)!=0);
}

/* user control to enable or disable multiface being emulated */
void    Multiface_EnableEmulation(BOOL State)
{
	/* override enable settings */
    if (State)
    {
		/* enable multiface */
		Multiface_Flags |= MULTIFACE_ENABLE_OVERRIDE;
	}
	else
	{
		/* disablemultiface */
		Multiface_Flags &= ~MULTIFACE_ENABLE_OVERRIDE;
	}

	Multiface_SetMode(Multiface_StoredMode);
}

/* setup correct rom for multiface emulation depending on CPC type chosen */
void	Multiface_SetMode(MULTIFACE_MODE Mode)
{
	unsigned char *pRom;

	pRom = NULL;

	Multiface_StoredMode = Mode;

	/* user has chosen to disable multiface even though a rom has been loaded
	and the hardware could be used, or the ram has not been allocated */
	if (
		((Multiface_Flags & MULTIFACE_ENABLE_OVERRIDE)!=0) && 
		(MultifaceRam!=NULL)
		)
	{
		switch (Mode)
		{
			/* cpc mode */
			case MULTIFACE_CPC_MODE:
			{
				pRom = MultifaceRom_CPC;
			}
			break;

			/* cpc+ mode */
			case MULTIFACE_CPCPLUS_MODE:
			{
				pRom = MultifaceRom_CPCPLUS;
			}
			break;

			default:
				break;
		}
	}

	/* rom loaded, or enable override? */
	if (pRom==NULL)
	{
		/* multiface h/w is not enabled */
		Multiface_Flags &=~MULTIFACE_IS_ENABLED;
	}
	else
	{
		/* rom loaded, and not overriding enable */
		Multiface_Flags |=MULTIFACE_IS_ENABLED;
		MultifaceRom = pRom;
	}
}

/* returns true if the specified rom is loaded */
BOOL	Multiface_IsRomLoaded(MULTIFACE_ROM_TYPE RomType)
{
	switch (RomType)
	{
		case MULTIFACE_ROM_CPC_VERSION:
			return ((Multiface_Flags & MULTIFACE_FLAGS_CPC_ROM_LOADED)!=0);

		case MULTIFACE_ROM_CPCPLUS_VERSION:
			return ((Multiface_Flags & MULTIFACE_FLAGS_CPCPLUS_ROM_LOADED)!=0);

		default:
			break;
	}

	return FALSE;
}
