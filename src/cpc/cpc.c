/* 
 *  Arnold emulator (c) Copyright, Kevin Thacker 1997-2001
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
/*********************************

  CPC Hardware code 
(c) Kevin Thacker 1997-2001

  Ram Sizes that are valid:

  64,
  64 + 64k expansion (CPC464 + 64k ram expansion/CPC6128),
  64 + 256k expansion (extra 256k expansion)

**********************************/

#include "cpc.h"
#include "audioevent.h"
//#include "debugmain.h"
#include "yiq.h"
#include "z8536.h"
#ifdef SPEECH
#include "spo256.h"
#endif
#include "fdi.h"
#include "fdd.h"
#include "amsdos.h"
#include "pal.h"
#include "fdc.h"
#include "crtc.h"
#include "garray.h"
#include "asic.h"
#include "z80/z80.h"
#include "cpcglob.h"    
#include "diskimage/diskimg.h"
#include "printer.h"
#include "cheatdb.h"
#include "kempston.h"
#include "memory.h"

/* Hardware we are emulating */
/* CPC, PLUS or KC Compact */
static int CPC_Hardware = CPC_HW_CPC;

static BOOL Amstrad_DiscInterface_Enabled = FALSE;
static BOOL Amstrad_RamExpansion_Enabled = FALSE;

void Amstrad_DiscInterface_Install(void)
{
	Amstrad_DiscInterface_Enabled = TRUE;
}

void Amstrad_DiscInterface_DeInstall(void)
{
	Amstrad_DiscInterface_Enabled = FALSE;
}

void	Amstrad_RamExpansion_Install(void)
{
	Amstrad_RamExpansion_Enabled = TRUE;
}

void	Amstrad_RamExpansion_DeInstall(void)
{
	Amstrad_RamExpansion_Enabled = FALSE;
}

static int NumReadPorts = 0;
static CPCPortRead	readPorts[32];
static int NumWritePorts = 0;
static CPCPortWrite writePorts[32];
static int NumResetFunctions = 0;
static CPC_RESET_FUNCTION resetFunctions[32];


void CPC_InstallResetFunction(CPC_RESET_FUNCTION resetFunction)
{
	resetFunctions[NumResetFunctions] = resetFunction;	
	NumResetFunctions++;
}

void CPC_InstallReadPort(CPCPortRead *readPort)
{
	memcpy(&readPorts[NumReadPorts], readPort, sizeof(CPCPortRead));
	NumReadPorts++;
}

void CPC_InstallWritePort(CPCPortWrite *writePort)
{
	memcpy(&writePorts[NumWritePorts], writePort, sizeof(CPCPortWrite));
	NumWritePorts++;
}

static unsigned long   NopCount = 0;

//static unsigned char *pCPCAmsdos;       

/* if CPC464/664/6128 or KC Compact selected these pointers are defined */
/* DOS rom is NULL if no DOS is on the motherboard */
/* basic rom */
static const unsigned char *pBasic;

/* os rom */
const unsigned char *pOS;

/* dos rom */
static const unsigned char *pDOS;

void CPC_SetOSRom(const unsigned char *pOSRom)
{
	pOS = pOSRom;
}

void CPC_SetBASICRom(const unsigned char *pBASICROM)
{
	pBasic = pBASICROM;
}

void CPC_SetDOSRom(const unsigned char *pDOSRom)
{
	pDOS = pDOSRom;
}


/******************/
/* Expansion ROMs */

/* in the CPC, the BASIC rom is visible in all rom-slots unless there is a */
/* rom in a rom-board at the same slot. In this case the rom on the romboard */
/* overrides the BASIC rom */

/* in the CPC+, the rom data from the cartridge can be overriden by a rom */
/* in a rom-board */

/* this table holds the pointers for the active expansion ROM selections */
static unsigned char *ExpansionROMTable[256];

/* this is the index of the currently selected rom as written by the ROM select I/O */
static int     RomSelection;   

/* selected expansion rom */
unsigned char   *pUpperRom;

/* 16k position in read memory map of rom */
unsigned long	LowerRomIndex;
/* lower rom */
const unsigned char	*pLowerRom;

/********************/

/* keyboard data */
static int              SelectedKeyboardLine;
static BOOL				KeyboardScanned;

unsigned char KeyboardData[16]=
{
        0x0ff,0x0ff,0x0ff,0x0ff,
        0x0ff,0x0ff,0x0ff,0x0ff,
        0x0ff,0x0ff,0x0ff,0x0ff,
        0x0ff,0x0ff,0x0ff,0x0ff,
};

void (*pWriteMemory)(Z80_WORD Addr,Z80_BYTE Data);
void CPC_WriteMemory(Z80_WORD, Z80_BYTE);
void CPCPlus_WriteMemoryWithASICRamEnabled(Z80_WORD,Z80_BYTE);

static unsigned long CPC_HardwareConnectedToJoystickPort = JOYSTICK_HARDWARE_JOYSTICK;

/* Vector these routines to change between CPC and PLUS mode */

extern unsigned char    *pReadRamPtr[8];
extern unsigned char    *pWriteRamPtr[8];

extern  unsigned char *Z80MemoryBase;

extern CRTC_INTERNAL_STATE CRTC_InternalState;

BOOL Keyboard_HasBeenScanned()
{
	return KeyboardScanned;
}


unsigned char Joystick_ReadJoystickPort(void)
{
	return KeyboardData[9];
}

unsigned char Keyboard_Read(void)
{
    if (SelectedKeyboardLine!=9)
    {
		KeyboardScanned = TRUE;
		return KeyboardData[SelectedKeyboardLine];
    }
    else
    {
		unsigned char Data = 0x07f;

#if 0
		switch (CPC_HardwareConnectedToJoystickPort)
		{
			case JOYSTICK_HARDWARE_AMX_MOUSE:
				Data = AmxMouse_ReadJoystickPort();
				break;

			case JOYSTICK_HARDWARE_SPANISH_LIGHTGUN:
				Data = SpanishLightGun_ReadJoystickPort();
				break;

			case JOYSTICK_HARDWARE_WESTPHASER:
				Data = WestPhaser_ReadJoystickPort();
				break;
			default:
			case JOYSTICK_HARDWARE_JOYSTICK:
#endif
				Data = Joystick_ReadJoystickPort();
#if 0
				break;
		}		
#endif
		return (unsigned char)((Data & 0x07f) | (KeyboardData[9] & 0x080));

	}
}

void	CPC_SetHardwareConnectedToJoystickPort(JOYSTICK_HARDWARE_ID HardwareID)
{
	CPC_HardwareConnectedToJoystickPort = HardwareID;
}

JOYSTICK_HARDWARE_ID CPC_GetHardwareConnectedToJoystickPort(void)
{
	return CPC_HardwareConnectedToJoystickPort;
}


void    CPC_ClearKeyboard(void)
{
        int     i;

		/* set all keys to not be pressed */
        for (i=0; i<16; i++)
                KeyboardData[i] = 0x0ff;
}

void CPC_SetKey(int KeyID)
{
	if (KeyID!=CPC_KEY_NULL)
	{
		int Line = KeyID>>3;
		int Bit = KeyID & 0x07;

		KeyboardData[Line] &= ~(1<<Bit);
	}
}

void CPC_ClearKey(int KeyID)
{
	if (KeyID!=CPC_KEY_NULL)
	{
		int Line = KeyID>>3;
		int Bit = KeyID & 0x07;

		KeyboardData[Line] |= (1<<Bit);
	}
}

#if 0
void	CPC_GenerateKeyboardClash(void)
{
	int i,j;

	for (i=0; i<9; i++)
	{
		unsigned char Line1;

		Line1 = KeyboardData[i];

		if (Line1 != 0x0ff)
		{
			/* key(s) pressed in this row */

			for (j=0; j<9; j++)
			{
				unsigned char Line2;

				Line2 = KeyboardData[j];

				if ((Line1 | Line2)!=0x0ff)
				{
					/* common key(s) pressed in these two lines */

					Line2 = Line2 & (~(Line1 ^ Line2));
				
					KeyboardData[j] = Line2;
				}
			}
		}
	}
}
#endif

/***************************************************************************

 ROM CODE
 
 **************************************************************************/

/* expansion roms that are installed */
static unsigned char *ExpansionRomData[17];
/* true/false depending on if expansion rom is enabled or not */
static BOOL ExpansionRomActive[17];


/* remove expansion rom */
void    ExpansionRom_Remove(int RomIndex)
{
        if (ExpansionRomData[RomIndex]!=NULL)
        {
                free(ExpansionRomData[RomIndex]);
                ExpansionRomData[RomIndex] = NULL;
        }
}


void    ExpansionRom_Init(void)
{
        int i;

        for (i=0; i<17; i++)
        {
                ExpansionRomData[i] = NULL;
                ExpansionRomActive[i] = FALSE;
        }
}

/* finish expansion roms. Delete all roms */
void    ExpansionRom_Finish(void)
{
        int i;

        for (i=0; i<17; i++)
        {
                ExpansionRom_Remove(i);
        }
}

/* get a word of data from the expansion rom selected */
unsigned short ExpansionRom_GetWord(const unsigned char *pAddr, int Offset)
{
        unsigned short WordData;

        WordData = (unsigned short)((pAddr[(Offset & 0x03fff)]) | ((pAddr[(Offset+1) & 0x03fff])<<8));

        return WordData;
}

/* check address lies within expansion rom address space */
BOOL    ExpansionRom_CheckRomAddrValid(unsigned short Addr)
{
        /* if addr is not in range 0x0c000-0x0ffff then addr
        is invalid */
        if ((Addr & 0x0c000)!=0x0c000)
                return FALSE;

        return TRUE;
}

/* Byte 0: ROM Type 
                0 = Foreground, 1 = Background, 2 = Extension 
   Byte 1: ROM Mark
   Byte 2: ROM Version
   Byte 3: ROM Modification
   Byte 4,5: External Name Table */

BOOL    ExpansionRom_Validate(const unsigned char *pData, unsigned long DataSize)
{
        unsigned char RomType = (unsigned char)(pData[0] & 0x07f);
        unsigned short NameTableAddr;


        /* not a foreground, background or extension rom */
        if ((RomType!=0) && (RomType!=1) && (RomType!=2))
                return FALSE;

        /* rom size must be >0k and less than 16k */
        if ((DataSize==0) || (DataSize>16384))
                return FALSE;

        /* get name table addr */
        NameTableAddr = ExpansionRom_GetWord(pData,4);

        /* check it is valid */
        if (ExpansionRom_CheckRomAddrValid(NameTableAddr))
        {
                /* it is valid */
                const unsigned char   *pNames = pData + (NameTableAddr & 0x03fff);
                int             NameCount = 0;

                while (pNames[0]!=0x00)
                {
                    /* the last character in each name has bit 7 set */
                    if (pNames[0] & 0x080)
                    {
                            NameCount++;
                    }

                    /* a 0 indicates the end of the name table */
                    if (pNames[0]!=0x00)
					{                
                        pNames++;

                        /* if we were reading through rom data and ran out of
                        space the name table is damaged */
                        if (pNames>=(pData+0x04000))
                                return FALSE;
					}
				}

                /* must have at least one name = name for startup of rom */
                if (NameCount==0)
                        return FALSE;
        }

        return TRUE;
}

/* return TRUE if slot is in-use, RomName will hold a pointer to the name */
/* return FALSE if slot is not-used */
BOOL ExpansionRom_GetRomName(const int RomIndex, char **RomName)
{
        unsigned char *pRomData = ExpansionRomData[RomIndex];
        
		*RomName = NULL;
        if (pRomData!=NULL)
        {
                unsigned short NameTableAddr;
                unsigned char *pName;
                unsigned int RomMark = pRomData[1] & 0x0ff;
                unsigned int RomVersion = pRomData[2] & 0x0ff;
                unsigned int RomModification = pRomData[3] & 0x0ff;
				int NameLength;
				char *pNameBuffer;

                /* get rom name */

                /* get address of name table */
                NameTableAddr = ExpansionRom_GetWord(pRomData, 4);

                /* get pointer to name */
                pName = pRomData + (NameTableAddr & 0x03fff);

				/* the last character of the name is marked with bit 7 set */
                NameLength = 0;
				while ((pName[NameLength]&0x080)==0)
                {
					/* count this character */
                    NameLength++;
                
					/* if this character has bit 7 set, then it is the last
					character in the name. If this character doesn't have bit 7 set
					then this is not the last character */
				}
				NameLength++;

				/* allocate buffer to hold name */
				pNameBuffer = malloc(NameLength+8+1);

				if (pNameBuffer!=NULL)
				{
					int Count;

					/* fill in buffer */
					for (Count=0; Count<NameLength; Count++)
					{
						pNameBuffer[Count] = pName[Count]&0x07f;
					}

					pNameBuffer[Count]=' ';
					Count++;
					sprintf(&pNameBuffer[Count],"[v%1x.%01x%01x]",RomMark, RomVersion, RomModification);

					*RomName = pNameBuffer;
					return TRUE;
				}
        }

        return FALSE;
}
 

/* insert a expansion rom; returns status code */
int    ExpansionRom_SetRomData(const unsigned char *RomData, const unsigned long RomDataSize, const int RomIndex)
{
	const unsigned char *pRomData;
	unsigned long RomLength;
	int Status = ARNOLD_STATUS_ERROR;
	
    /* remove an existing rom */
    if (ExpansionRomData[RomIndex]!=NULL)
    {
        ExpansionRom_Remove(RomIndex);
    }
    
	pRomData = RomData;
	RomLength = RomDataSize;

    if (pRomData!=NULL)
    {
        /* as long as rom data is large enough to hold a amsdos header, we can check it
        for one */
        if (RomLength>128)
        {
            /* does rom data have a amsdos header? */
            if (AMSDOS_HasAmsdosHeader(pRomData))
            {
				/* yes */

				/* adjust pointer */
				pRomData = pRomData+128;
				/* adjust length */
				RomLength = RomLength-128;
			}
        }

		/* is rom length valid? */
        if (RomLength>0)
        {
			/* validate */
            if (ExpansionRom_Validate(pRomData,RomLength))
            {
                /* it is valid */

                /* we want to check if a foreground rom data file is 
                loaded into any slot above 0, or a non-foreground not
                loaded into slot 0 */

                /* store rom pointer in expansion rom table */


                /* The following is done, so that roms with less
                than 16k can be supported. We allocate 16k, so that
                if a memory access is done which would be outside the rom
                data area, it doesn't cause an invalid memory access */

                /* allocate 16384 bytes */
                ExpansionRomData[RomIndex] = (unsigned char *)malloc(16384);

                if (ExpansionRomData[RomIndex]!=NULL)
                {
                    /* copy rom data into allocated block */
                    memcpy(ExpansionRomData[RomIndex],pRomData,RomLength);
    
                    /* mark it as active */
                    ExpansionRom_SetActiveState(RomIndex, TRUE);
                 
					/* ok! */
					Status = ARNOLD_STATUS_OK;
                }
				else
				{
					Status = ARNOLD_STATUS_OUT_OF_MEMORY;
				}
            }
			else
			{
				/* invalid */
				Status = ARNOLD_STATUS_INVALID;
			}
        }
		else
		{
			/* the length is invalid */
			Status = ARNOLD_STATUS_INVALID_LENGTH;
		}
    }

	/* return status */
    return Status;
}

unsigned char *ExpansionRom_Get(int RomIndex)
{
        return ExpansionRomData[RomIndex];
}

int             ROM_GetSelectedROM(void)
{
        return RomSelection;
}

BOOL    ExpansionRom_IsActive(int RomIndex)
{
        return ExpansionRomActive[RomIndex];
}

void    ExpansionRom_SetActiveState(int RomIndex, BOOL State)
{
        ExpansionRomActive[RomIndex] = State;

		/* refresh table */
		ExpansionROM_RefreshTable();
}

/* this function is called to override the ExpansionROMTable entries with */
/* any expansion ROMs that are selected */
void	ExpansionROM_SetupTable(void)
{
	int i;

	for (i=0; i<16; i++)
	{
		unsigned char *pExpansionRom;

		/* get rom at rom index specified. An expansion rom
		in the same position as a on-board rom will be selected in
		preference to the on-board rom */
		pExpansionRom = ExpansionRom_Get(i);

		/* is it active ? */
		if ((pExpansionRom!=NULL) && (!ExpansionRom_IsActive(i)))
		{
			/* no, not active */
			pExpansionRom = NULL;
		}

		if (pExpansionRom==NULL)
		{
			switch (i)
			{
				default:
				case 0:
				{
					/* basic selected - get rom at slot 0 */
					pExpansionRom = ExpansionROMTable[0];
				}
				break;

				case 7:
				{
					pExpansionRom = ExpansionROMTable[7];
				}
				break;
   			}
		}
		else
		{
			pExpansionRom = (unsigned char *)((unsigned long)pExpansionRom - (unsigned long)0x0c000);
		}
		
		ExpansionROMTable[i] = pExpansionRom;
	
	}
}


void	ExpansionROM_RefreshTable(void)
{
	if (CPC_GetHardware()==CPC_HW_CPCPLUS)
	{
		/* initialise table with cartridge pages */
		ASIC_RefreshExpansionROMTable(ExpansionROMTable);
	}
	else
	{
		int i;
		unsigned char *pExpansionROM;
				
		pExpansionROM = (unsigned char *)((unsigned long)pBasic - (unsigned long)0x0c000);

		/* for CPC and KC Compact, fill all entries with BASIC */
		for (i=0; i<256; i++)
		{
			ExpansionROMTable[i] = pExpansionROM;
		}

		/* is dos present? */
		if (pDOS!=NULL)
		{
			pExpansionROM = (unsigned char *)((unsigned long)pDOS - (unsigned long)0x0c000);
	
			/* yes */
			ExpansionROMTable[7] = pExpansionROM;
		}	
	}

	/* now override with expansion roms that are enabled */
	ExpansionROM_SetupTable();

	/* now override with roms from a ram-rom */
	/* does ram/rom override rom-board settings? */
//	RAM_ROM_SetupTable();

	GateArray_RethinkMemory();
}

void	ExpansionROM_SetTableEntry(int Index, unsigned char *pRomData)
{
	ExpansionROMTable[Index] = pRomData;
}


/* this is called when a ROM select OUT is performed */
void    ROM_Select(int Selection)
{
    RomSelection = Selection;

	pUpperRom = ExpansionROMTable[RomSelection & 0x0ff];

    GateArray_RethinkMemory();

}



/**************************************************************************/
/* PPI */

/* port address: &F4xx
  Function: PSG data (bits 7-0)

 port address: &F5xx (various status information)
 PPI port B bit usage

 Bit            Function

 7              Datarecorder cassette read data
 6              Centronics Busy                 (implement using Amiga
                                                 printer interface)
 5              Not expansion port active signal (device connected to
                                                 expansion port detect)
 4              Screen refresh (50Hz/60Hz)
 3..1           Machine name on power-up
 0              Frame Flyback pulse

 port address: &F6xx
 PPI port C bit usage

 Bit            Function

 7,6            PSG Control
 5              Datarecorder write      (implement using serial port??)
 4              Datarecorder motor on   (implement using serial port??)
 3..0           Keyboard row select     

 port address: &F7xx
 PPI_control bit usage

 Bit            Function

 7              Function to perform (1: mode set, 0: bit set/reset)
 6...0          Used depending on function

 Bit            Function

 7              1: mode set
 6,5            Mode selection for Port A and Port C (upper)
 4              Port A control (1: input, 0:output)
 3              Port C (upper) control (1: input, 0:output)
 2              Mode selection for Port B and Port C (lower)
 1              Port B control (1: input, 0:output)
 0              Port C (lower) control (1: input, 0:output)

 Mode selection for Port A and Port C (upper)

 Bit 6          bit 5

 0              0               Mode 0
 0              1               Mode 1
 1              0               Mode 2
 1              1               Mode 2

 Mode selection for Port B and Port C (lower)

 0      Mode 0
 1      Mode 1

 Bit            Function

 7              0: bit set/reset flag
 6..4           Not used (doesnt matter what value is)
 3..1           Bit to set/reset
 0              Bit function (1:set, 0:reset)
*/

/* Note: When a port is set to output, when data is read from it the result is 0's */

#define VSYNC_ACTIVE    0x001
#define VSYNC_INACTIVE  0x0fe

#define PPI_CONTROL_BYTE_FUNCTION               0x080
#define PPI_CONTROL_PORT_A_CU_MODE1             0x040
#define PPI_CONTROL_PORT_A_CU_MODE0             0x020
#define PPI_CONTROL_PORT_A_STATUS               0x010
#define PPI_CONTROL_PORT_C_UPPER_STATUS 0x008
#define PPI_CONTROL_PORT_B_CL_MODE              0x004
#define PPI_CONTROL_PORT_B_STATUS               0x002
#define PPI_CONTROL_PORT_C_LOWER_STATUS 0x001

#define PPI_TAPE_READ_DATA                              (1<<7)
#define PPI_CENTRONICS_BUSY                             (1<<6)
#define PPI_EXPANSION_PORT                              (1<<5)

#define PPI_SCREEN_REFRESH_50HZ                 (0x001<<4)
#define PPI_SCREEN_REFRESH_60HZ                 (0x000<<4)

/* computer names */
#define PPI_COMPUTER_NAME_ISP                   0x000
#define PPI_COMPUTER_NAME_TRIUMPH               0x001
#define PPI_COMPUTER_NAME_SAISHO                0x002
#define PPI_COMPUTER_NAME_SOLAVOX               0x003
#define PPI_COMPUTER_NAME_AWA                   0x004
#define PPI_COMPUTER_NAME_SCHNEIDER             0x005
#define PPI_COMPUTER_NAME_ORION                 0x006
#define PPI_COMPUTER_NAME_AMSTRAD               0x007

static unsigned char ComputerNameIndex = PPI_COMPUTER_NAME_AMSTRAD;
static unsigned char PPI_PortBInputData;

void	PPI_UpdatePortBInputData(void)
{
        unsigned char Data = 0;

		if (CPC_GetHardware()==CPC_HW_KCCOMPACT)
		{
			Data |= (1<<4) | (1<<3) | (1<<2) | (1<<1);
		}
		else
		{
			/* set computer name */
			Data |= ((ComputerNameIndex & 0x07)<<1);        

			/* set screen refresh */
			Data |= PPI_SCREEN_REFRESH_50HZ;
        
			/* on CPC464+, /EXP is set to 0 */
		}


		PPI_PortBInputData = Data;
}


void      CPC_SetComputerNameIndex(int Index)
{
        if ((Index<0) || (Index>7))
        {
                Index = 7;
        }

        ComputerNameIndex = (unsigned char)Index;

		PPI_UpdatePortBInputData();
}

int CPC_GetComputerNameIndex(void)
{
        return ComputerNameIndex;
}

/* when port is set to input, the output of that port is 0x0ff */

typedef struct
{
	/* latched data written to outputs */
	unsigned char latched_outputs[4];

	unsigned char final_outputs[4];

	/* current inputs */
	unsigned char	inputs[4];
	
	/* masks for input/output. 0x0ff = keep input, 0x00 = keep output */
	unsigned char	io_mask[4];
	/* control information */
	unsigned char	control;
} PPI_8255;

static PPI_8255	ppi8255;

static unsigned long PortBRead_PreviousNopCount;
static unsigned long PortCWrite_PreviousNopCount;


void    PPI_Reset(void)
{
        /*  as per MTM docs. */
        PPI_WriteControl(0x09b);
		
		PPI_UpdatePortBInputData();

		/* inputs on CPC */
		ppi8255.inputs[1] = PPI_PortBInputData;
		ppi8255.inputs[2] =  (0x02<<4) | 0x0f;

		PortBRead_PreviousNopCount = 0;
		PortCWrite_PreviousNopCount = 0;

}

static void     UpdatePSG(void)
{
	/* psg databus at high impedance */
	ppi8255.inputs[0] = 0x0ff;

    switch ((ppi8255.final_outputs[2]>>6) & 0x03)
    {
		default:
        case 0:
			break;
        
        case 1:
        {
            /* PSG Read Data - psg data bus holds data */
			ppi8255.inputs[0] = (unsigned char)PSG_ReadData();
        }
        break;
        
        case 2:
        {
            /* PSG Write Data */
            PSG_WriteData(ppi8255.final_outputs[0]);
	    }
        break;
        
        case 3:
        {
            /* PSG Register Select */
            PSG_RegisterSelect(ppi8255.final_outputs[0]);
        }
        break;
    }
}


static BOOL             TapeMotorActive = FALSE;
/*static int PreviousNopCountReadBit;
static unsigned char CassetteReadBit;
static unsigned char PreviousData = 0x00;
static int PPI_PortC;*/

/* previous port C written */
static unsigned char PPI_PreviousPortC = 0;
/* current port C written */
static unsigned char PPI_CurrentPortC = 0;
 

void PPI_DoPortBInput(void)
{
	unsigned long CurrentNopCount;
	unsigned char Data = PPI_PortBInputData;

	/* get current nop count */
	CurrentNopCount = CPC_GetNopCount();


        /* set state of vsync bit */
        if (CRTC_InternalState.CRTC_Flags & CRTC_VS_FLAG)                    
        {
                Data |= VSYNC_ACTIVE;
        }

		/**** TAPE ****/
		/* tape motor is on? */
		if (PPI_CurrentPortC & 0x010)
		{
			unsigned char CassetteReadBit;
			unsigned long NopsPassed;

			NopsPassed = CurrentNopCount - PortBRead_PreviousNopCount;

			CassetteReadBit = Cassette_Read(NopsPassed);

	        Data |= (CassetteReadBit<<7);
		
			/* store previous port B read nop count */
			PortBRead_PreviousNopCount = CurrentNopCount;
		}
        

        ppi8255.inputs[1] = Data;

}


/* called when port A, port B or port C output has changed */
void	Amstrad_PPI_Refresh(void)
{
	unsigned char Data;
	unsigned long CurrentNopCount;

	/* get current nop count */
	CurrentNopCount = CPC_GetNopCount();

	/* get output from PPI */
	Data = ppi8255.final_outputs[2];

	/* store previous value */
	PPI_PreviousPortC = PPI_CurrentPortC;
	/* store current value */
	PPI_CurrentPortC = Data;

	/* has write bit state changed? */
	/* tape-volume when writing! */
	if (((PPI_PreviousPortC^PPI_CurrentPortC)&(1<<5))!=0)
	{
		if (PPI_CurrentPortC & 0x010)
		{
			unsigned long NopsPassed;

			NopsPassed = CurrentNopCount - PortCWrite_PreviousNopCount;

			Cassette_Write(NopsPassed, (Data>>5) & 0x01);

			PortCWrite_PreviousNopCount = CurrentNopCount;
		}
	}

	SelectedKeyboardLine = Data & 0x0f; 

    UpdatePSG();

	/* when the keyboard is checked, the I/O status of the ports forces the outputs to zeros!
	causing the motor to be switched on and off quickly, should this be correct? */
	
	/* tape motor */
	if (((PPI_PreviousPortC^PPI_CurrentPortC)&0x010)!=0)
	{
		/* changed state! */

		if (PPI_CurrentPortC & 0x010)
		{
			/* tape motor has been switched on */
			PortBRead_PreviousNopCount = CurrentNopCount;
			PortCWrite_PreviousNopCount = CurrentNopCount;

			// switched on
			Cassette_Write(0,(Data>>5) & 0x01);
		}
		else
		{
			// switched off
			Cassette_Write((CurrentNopCount - PortCWrite_PreviousNopCount), (Data>>5) & 0x01);
		}
	}


/*		if (Data & 0x010)
		{
			TapeImage_PlayStatus(TRUE);
		}
		else
		{
			TapeImage_PlayStatus(FALSE);
		}
*/
}


INLINE static int ppi_read_port(int port_index)
{
	return ((ppi8255.inputs[port_index] & ppi8255.io_mask[port_index]) | 
	        (ppi8255.latched_outputs[port_index] & (~ppi8255.io_mask[port_index])));
}

INLINE static void ppi_write_port(int port_index, int data)
{
    ppi8255.latched_outputs[port_index] = data;
    
    ppi8255.final_outputs[port_index] = ((ppi8255.latched_outputs[port_index] & (~ppi8255.io_mask[port_index])) |
                                (0x0ff & ppi8255.io_mask[port_index]));
}




void    PPI_WritePortA(int Data)
{
	ppi_write_port(0, Data);

	Amstrad_PPI_Refresh();
}

void    PPI_WritePortB(int Data)
{
	ppi_write_port(1, Data);

	Amstrad_PPI_Refresh();
}

void	PPI_WritePortC(int Data)
{
	ppi_write_port(2, Data);

	Amstrad_PPI_Refresh();
}


void    PPI_WriteControl(int Data)
{
    if (Data & PPI_CONTROL_BYTE_FUNCTION)
    {
        /* Configuration control byte */
        unsigned char PPI_PortControl = (unsigned char)Data;

        if (CPC_GetHardware()!=CPC_HW_CPCPLUS)
        {
            /* on CPC and KCC, the PPI resets the port values when the
            configuration is set. On CPC+ it doesn't do this. */
            ppi8255.latched_outputs[0] = ppi8255.latched_outputs[1] = ppi8255.latched_outputs[2] = 0;

			ppi8255.control = PPI_PortControl;

			if (Data & PPI_CONTROL_PORT_A_STATUS)
			{
				/* port A is input */
				ppi8255.io_mask[0] = 0x0ff;
			}
			else
			{
				/* port A is output */
				ppi8255.io_mask[0] = 0x000;
			}


			/* PORT B */
			if (Data & PPI_CONTROL_PORT_B_STATUS)
			{
				/* port B is in input mode */
				ppi8255.io_mask[1] = 0x0ff;
			}
			else
			{
				/* port B is in output mode, return data written to it */
				ppi8255.io_mask[1] = 0x000;
			}

			/* PORT C */
			ppi8255.io_mask[2] = 0x0ff;

			if ((Data & PPI_CONTROL_PORT_C_LOWER_STATUS)==0)
			{
				/* lower part of port is in output mode */
				
				ppi8255.io_mask[2] &= 0x0f0;
			}

			if ((Data & PPI_CONTROL_PORT_C_UPPER_STATUS)==0)
			{
				/* upper part of port is in output mode */
				ppi8255.io_mask[2] &= 0x00f;
			}

			ppi_write_port(0, ppi8255.latched_outputs[0]);
			ppi_write_port(1, ppi8255.latched_outputs[1]);
			ppi_write_port(2, ppi8255.latched_outputs[2]);

			Amstrad_PPI_Refresh();
		}
		else
		{
			/* CPC type is CPC+ */

            /* on CPC+ port B is always in input mode and
            port C is always in output mode */
			ppi8255.io_mask[1] = 0x0ff;
			ppi8255.io_mask[2] = 0x000;

			Data |= PPI_CONTROL_PORT_B_STATUS;
            Data &= ~(PPI_CONTROL_PORT_C_LOWER_STATUS | PPI_CONTROL_PORT_C_UPPER_STATUS);
			
			ppi8255.control = Data;

			/* on CPC+ port A can be programmed as input or output */
			if (Data & PPI_CONTROL_PORT_A_STATUS)
			{
				/* port A is input */
				ppi8255.io_mask[0] = 0x0ff;
			}
			else
			{
				/* port A is output */
				ppi8255.io_mask[0] = 0x000;
			}

			ppi_write_port(0, ppi8255.latched_outputs[0]);
	
			Amstrad_PPI_Refresh();
		}

	}
    else
    {
        /* Bit Set/Reset control bit */

        int     BitIndex = (Data>>1) & 0x07;

        if (Data & 1)
        {
			/* set bit */

			int     OrData;

			OrData = (1<<BitIndex);

			ppi8255.latched_outputs[2]|=OrData;
        }
        else
        {
			/* clear bit */

			int     AndData;

			AndData = (~(1<<BitIndex));

			ppi8255.latched_outputs[2]&=AndData;
        }

		ppi_write_port(2, ppi8255.latched_outputs[2]);

		Amstrad_PPI_Refresh();
    }
}

int PPI_ReadControl(void)
{
    if (CPC_GetHardware()==CPC_HW_CPCPLUS)
    {
        /* for CPC+, this is the result of reading PPI Control port, value
        returned is based on value last written. */
                		
		/* in ranges:

			0x080-0x08f, 0x0a0-0x0af,0x0c0-0x0cf,0x0e0-0x0ef? 
		*/
		if  ((ppi8255.latched_outputs[3] & 0x090)==0x080)
		{
			/* yes */
			return 0x0;
		}
    }

	/* default to value returned by CPC */
	return 0x0ff;
}



int PPI_GetControlForSnapshot(void)
{
    return ppi8255.control;
}

void    PPI_SetPortADataFromSnapshot(int Data)
{
	/* CPCEMU stores the inputs to this port regardless of it's input/output setting */
	ppi8255.inputs[0] = Data;

	/* TODO: There is a possibility that some bits are reported incorrectly, especially
	if this snapshot has been loaded from another emulator */
}

int PPI_GetPortADataForSnapshot(void)
{
	return ppi8255.inputs[0];
}

void    PPI_SetPortBDataFromSnapshot(int Data)
{
	/* CPCEMU stores the inputs to this port regardless of it's input/output setting */
	ppi8255.inputs[1] = Data;

	/* TODO: There is a possibility that some bits are reported incorrectly, especially
	if this snapshot has been loaded from another emulator */
}

	
int PPI_GetPortBDataForSnapshot(void)
{
	/* CPCEMU stores the inputs to this port regardless of it's input/output setting */
	/* therefore get inputs */
	return ppi8255.inputs[1];
}

void    PPI_SetPortCDataFromSnapshot(int Data)
{
	/* CPCEMU stores the outputs from this port regardless of it's input/output setting */
	ppi8255.final_outputs[2] = Data;

	Amstrad_PPI_Refresh();
}

int PPI_GetPortCDataForSnapshot(void)
{
	/* CPCEMU stores the outputs from this port regardless of it's input/output setting */
	return ppi8255.final_outputs[2];
}




unsigned int            PPI_ReadPortA(void)
{
    UpdatePSG();

	/* on CPC, KC Compact and CPC+ port A can be input or output, return appropiate data */
	return ppi_read_port(0);
}


unsigned int            PPI_ReadPortB(void)
{
    PPI_DoPortBInput();

	return ppi_read_port(1);
}

unsigned int            PPI_ReadPortC(void)
{
	return ppi_read_port(2);
}

void    Z80_Reti(void)
{
}

/*------------------------------------------------------------------------*/
/* set the CPC machine type */

#if 0
static int CurrentCPCType;

void    CPC_SetCPCType(CPC_TYPE_ID Type)
{
        /* ensure type is valid */
        if (Type>CPC_TYPE_KCCOMPACT)
        {
                Type = CPC_TYPE_CPC464;
        }

        CurrentCPCType = Type;

        /* setup OS, Basic and DOS rom pointers */
        switch(Type)
        {
                case CPC_TYPE_CPC464:
                {
//                        pOS = Roms464.pOs;
  //                      pBasic = Roms464.pBasic;
    //                    pDOS = pCPCAmsdos;

						/* if CPC, do not allow type 3 to be selected */
						if (CPC_GetCRTCType()==3)
						{
							CPC_SetCRTCType(0);
						}
				}
                break;

                case CPC_TYPE_CPC664:
                {
      //                  pOS = Roms664.pOs;
        //                pBasic = Roms664.pBasic;
          //              pDOS = pCPCAmsdos;
         
						/* if CPC, do not allow type 3 to be selected */
						if (CPC_GetCRTCType()==3)
						{
							CPC_SetCRTCType(0);
						}

				}
                break;

                case CPC_TYPE_CPC6128:
                {
            //            pOS = Roms6128.pOs;
              //          pBasic = Roms6128.pBasic;
                //        pDOS = pCPCAmsdos;
        
						/* if CPC, do not allow type 3 to be selected */
						if (CPC_GetCRTCType()==3)
						{
							CPC_SetCRTCType(0);
						}

						{
							unsigned long RamConfig;
														
							/* ensure extra 64k is present */
							RamConfig = CPC_GetRamConfig();
							CPC_SetRamConfig(CPC_RAM_CONFIG_64K_RAM | RamConfig);	
						}


                }
                break;

				case CPC_TYPE_KCCOMPACT:
				{
//					pOS = RomsKCC.pOs;
//					pBasic = RomsKCC.pBasic;
//					pDOS = pCPCAmsdos;
        
					/* force type 0, my KC Compact has a HD6845S */
					/* this is likely to be the only type of CRTC */
					/* used by this system */
					CPC_SetCRTCType(0);
				}
				break;

                case CPC_TYPE_464PLUS:
                case CPC_TYPE_6128PLUS:
                {
  //                      pOS = ASIC_GetCartPage(0);
    //                    pBasic = ASIC_GetCartPage(1);
      //                  pDOS = ASIC_GetCartPage(3);
         
						/* if plus, force crtc type 3 */
						CPC_SetCRTCType(3);

						if (Type==CPC_TYPE_6128PLUS)
						{
							unsigned long RamConfig;
														
							/* ensure extra 64k is present */
							RamConfig = CPC_GetRamConfig();
							CPC_SetRamConfig(CPC_RAM_CONFIG_64K_RAM | RamConfig);	
						}
				}
                break;
        }

        /* reset the cpc */
        CPC_Reset();
}

int CPC_GetCPCType(void)
{
        return CurrentCPCType;
}
#endif

/*--------------------------------------------------------------------------*/

static int CurrentMonitorType;

void    CPC_SetMonitorType(CPC_MONITOR_TYPE_ID MonitorType)
{
        /* force value to be valid */
        if ((MonitorType != CPC_MONITOR_COLOUR) && (MonitorType != CPC_MONITOR_GREEN_SCREEN)
                && (MonitorType != CPC_MONITOR_GREY_SCALE))
        {
                MonitorType = CPC_MONITOR_COLOUR;
        }

        /* if we are in CPC plus mode, and Green is specified, use Grey Scale instead */        
        if (CPC_GetHardware()==CPC_HW_CPCPLUS)
        {
                if (MonitorType == CPC_MONITOR_GREEN_SCREEN)
                        MonitorType = CPC_MONITOR_GREY_SCALE;
        }

        /* set monitor type */
        CurrentMonitorType = MonitorType;

        switch (CurrentMonitorType)
        {
                case CPC_MONITOR_COLOUR:
                {
                        /* update gate array colours */
                        GateArray_SetMonitorColourMode(MONITOR_MODE_COLOUR);
                        /* update ASIC colours */
                        ASIC_SetMonitorColourMode(MONITOR_MODE_COLOUR);
                }
                break;

                case CPC_MONITOR_GREEN_SCREEN:
                {
                        /* update gate array colours */
                        GateArray_SetMonitorColourMode(MONITOR_MODE_GREEN_SCREEN);
                }
                break;

                case CPC_MONITOR_GREY_SCALE:
                {
                        /* update gate array colours */
                        GateArray_SetMonitorColourMode(MONITOR_MODE_GREY_SCALE);
                        /* update asic colours */
                        ASIC_SetMonitorColourMode(MONITOR_MODE_GREY_SCALE);
                }
                break;

        }
}

int             CPC_GetMonitorType(void)
{
        return CurrentMonitorType;
}

/*-----------------------------------------------------------------------*/

static int MonitorBrightness;

void    CPC_SetMonitorBrightness(int BrightnessSetting)
{

        if (BrightnessSetting<MONITOR_BRIGHTNESS_MIN)
        {
                BrightnessSetting = MONITOR_BRIGHTNESS_MIN;
        }
        
        if (BrightnessSetting>MONITOR_BRIGHTNESS_MAX)
        {
                BrightnessSetting = MONITOR_BRIGHTNESS_MAX;
        }
        
        MonitorBrightness = BrightnessSetting;

        /* update monitor settings */
        CPC_SetMonitorType(CPC_GetMonitorType());
}

int     CPC_GetMonitorBrightness(void)
{
        return MonitorBrightness;
}



/*-----------------------------------------------------------------------*/

static int CurrentCRTCType;

/* set the CRTC emulation type */
void    CPC_SetCRTCType(unsigned int Type)
{
        if (Type>NUM_CRTC_TYPES)
        {
                Type = 0;
        }

		/* force type 0 for KC Compact */
		if (CPC_GetHardware()==CPC_HW_KCCOMPACT)
		{
			Type = 0;
		}
		else
		/* force type 3 for CPC+ */
		if (CPC_GetHardware()==CPC_HW_CPCPLUS)
		{
			Type = 3;
		}
		else
		if (CPC_GetHardware()==CPC_HW_CPC)
		{
			if (Type==3)
				Type = 0;
		}



        CurrentCRTCType = Type;

        CRTC_SetType(Type);
}

int CPC_GetCRTCType(void)
{
        return CurrentCRTCType;
}

/*---------------------------------------------------------------------------*/
/* do a reset of CPC/CPC+ system */
void    CPC_Reset(void)
{
	int i;

	/* reset keyboard scanned flag; used by auto-type feature */
	KeyboardScanned = FALSE;

	/* reset lower rom index */
	LowerRomIndex = 0;


	/* refresh vector base */
	switch (CPC_GetHardware())
	{
		case CPC_HW_CPC:
		{
					/* interrupt vector normally on bus on a standard CPC.
					Although, this is not really guaranteed, depending
					on what peripheral's are plugged in etc. NWC mentioned
					that fact. */
					Z80_SetInterruptVector(0x0ff);

//	#ifdef MULTIFACE
//					Multiface_SetMode(MULTIFACE_CPC_MODE);
//	#endif
			}
			break;

			case CPC_HW_KCCOMPACT:
			{
				Z80_SetInterruptVector(0x0ff);

//	#ifdef MULTIFACE
//				Multiface_SetMode(MULTIFACE_CPC_MODE);
//	#endif
			}
			break;

			case CPC_HW_CPCPLUS:
			{

//	#ifdef MULTIFACE
//					Multiface_SetMode(MULTIFACE_CPCPLUS_MODE);
//	#endif
			}
	}


	if (CPC_GetHardware()==CPC_HW_CPCPLUS)
	{
		/* when we are in CPC+ mode, and ints are
		not enabled, the ASIC appears to set the IM2
		Vector Base to 0. Could be the default reset state.
		Discovered from testing and running Chany's DreamEnd
		megademo. */

		/* set Z80 vector base for mode 2. This will
		be overridden when the ASIC IM2 ints are enabled. */
		Z80_SetInterruptVector(0);
	
		/* set lower rom */
		pOS = ASIC_GetCartPage(0);
	}
	
	/* reset lower rom */
	pLowerRom = pOS;

	
	CPC_ResetTiming();
	CPC_ResetNopCount();

	Audio_Reset();

    /* reset ASIC */
    ASIC_Reset();

	/* PAL16L8 controlling RAM configuration reset */
	PAL_Reset();

    /* reset Gate Array (CPC or ASIC) */
    GateArray_Reset();

    /* reset 8255 PPI */
    PPI_Reset();

    /* reset FDC - Floppy Disc Controller */
    FDC_Reset();

    /* reset CRTC - Cathode Ray Tube Controller */
    CRTC_Reset();

    /* reset PSG - Programmable Sound Generator */
    PSG_Reset();

	/* refresh expansion rom table */
	ExpansionROM_RefreshTable();

    /* select rom 0 */
    ROM_Select(0x000);

    /* reset Z80 */
    Z80_Reset();

	/* call the additional reset functions */
	for (i=0; i<NumResetFunctions; i++)
	{
		resetFunctions[i]();
	}
}

/*=====================================================================================*/
/* RAM */

/* CPC Ram configurations (0x07fcx) */
/* 0..3 refer to 16k blocks in the base ram. */
/* 4..7 refer to 16k blocks in the 2nd block of 64k ram */
static int     RamConfigurationSetup[8*4]=
{
        0,1,2,3,                        /* selection 0 */
        0,1,2,7,                        /* selection 1 */
        4,5,6,7,                        /* selection 2 */
        0,3,2,7,                        /* selection 3 */
        0,4,2,3,                        /* selection 4 */
        0,5,2,3,                        /* selection 5 */
        0,6,2,3,                        /* selection 6 */
        0,7,2,3                         /* selection 7 */
};

/* 64*4 pointers to blocks of memory. 64 ram selections, with 4 pointers per selection */
unsigned char   *RamConfigurationTable[64*4];   

unsigned char   *Z80MemoryBase=NULL;                    /* Location of memory block */

/* pointers to ram pages - additional ram 64k-512k */
static unsigned char	*Amstrad_MemoryPages[32]={NULL};
/* extra ram */
unsigned char	*Amstrad_ExtraRam = NULL;

/* free ram pages allocated */
static void	FreeExtraRam(void)
{
	if (Amstrad_ExtraRam!=NULL)
	{
		free(Amstrad_ExtraRam);
		Amstrad_ExtraRam = NULL;
	}
}

/* free all ram allocated */
void    FreeEmulatorMemory(void)
{
    if (Z80MemoryBase!=NULL)
    {
            free(Z80MemoryBase);
            Z80MemoryBase = NULL;
    }

	/* free additional ram */
	FreeExtraRam();
}

/* allocate emulator base memory */
BOOL    AllocateEmulatorMemory(void)
{
	/* allocate base ram size */
	Z80MemoryBase = malloc(64*1024);

	if (Z80MemoryBase==NULL)
		return FALSE;

	return TRUE;
}


/* Memory paging:

  No memory:

  CPC: selections make no difference to the memory arrangement!

  64k Dk'Tronics ram expansion, or 128k standard memory on CPC:

  To select ram, (1 indicates significant bits, x indicates a bit
  that is ignored):

  %11xxx111	- 
	&c4,&cc,&d4,&dc,&e4,&ec,&f4,&fc - select block 0
	&c5,&cd,&d5,&dd,&e5,&ed,&f5,&fd - select block 1
	&c6,&ce,&d6,&de,&e6,&ee,&f6,&fe - select block 2 
	&c7,&cf,&d7,&df,&e7,&ef,&f7,&ff - select block 3 

  256k Dk'Tronics silicon disk expansion only (tested on Craig Harrison's ram expansion):

  %111xx111 - 
	&e4 - select block 0
	&e5 - select block 1
	&e6 - select block 2
	&e7 - select block 3
	&ec - select block 4
	&ed - select block 5
	&ee - select block 6
	&ef - select block 7
	&f4 - select block 8
	&f5 - select block 9
	&f6 - select block 10
	&f7 - select block 11
	&fc - select block 12
	&fd - select block 13
	&fe - select block 14
	&ff - select block 15
	(no other selections choose blocks in the 256k silicon disk expansion)
*/



/* RamConfig = Config value programmed to hardware, 
   RamConfigAdjusted = Config adjusted to access expansion memory,
   pExtraMemoryBase = Address of Extra ram */
void	SetupMemoryPaging(int RamConfig, int RamConfigAdjusted, unsigned char *pExtraMemoryBase)
{
	int ConfigSelection = RamConfigAdjusted & 0x07;
	int BankIndex = (RamConfigAdjusted>>3) & 0x07;
	int p;
	unsigned char *pBlockAddr;

	/* loop through all 4 16k pages of the Z80 ram */
	for (p=0; p<4; p++)
	{
		int BlockIndex;

		BlockIndex = RamConfigurationSetup[(ConfigSelection<<2) + p];

		if (BlockIndex>3)
		{
			/* extra ram memory block - adjusted for z80 memory address to be added on for access */
			pBlockAddr = pExtraMemoryBase + (unsigned long)((BlockIndex-4)<<14) + (BankIndex<<16) 
						- (unsigned long)(p<<14);
		}
		else
		{
			/* base 64k ram memory block - adjusted for Z80 memory address to be added on for access */
			if (BlockIndex==p)
			{
				pBlockAddr = Z80MemoryBase;	
			}
			else
			{
				// BlockIndex is original position 
				// (e.g. "3" in position 1. Here we need to add &c000-&4000
				pBlockAddr = Z80MemoryBase + ((BlockIndex<<14) - (p<<14));
			}
		}

		/* set ram address in table */
		RamConfigurationTable[((RamConfig)<<2) + p] = pBlockAddr;
	}
}

void	SetupNoExpansionMemory(void)
{
	int i;

	/* 64k only - CPC style */
	for (i=0; i<64; i++)
	{
        /* setup pointers for 64k ram - each is adjusted for z80 memory address to be added on */
        RamConfigurationTable[(i<<2)] = Z80MemoryBase;
        RamConfigurationTable[(i<<2)+1] = Z80MemoryBase;
        RamConfigurationTable[(i<<2)+2] = Z80MemoryBase;
        RamConfigurationTable[(i<<2)+3] = Z80MemoryBase;
	}
}

void	Setup64kRamExpansion(unsigned char *pExpansionMemoryAddress)
{
	int i;

	for (i=0; i<64; i++)
	{
		SetupMemoryPaging(i,(i & 0x07),/*Amstrad_ExtraRam,*/pExpansionMemoryAddress);
	}
}

void	Setup256kSiliconDiskExpansion(unsigned char *pExpansionMemoryAddress)
{
	int i;

	for (i=0; i<64; i++)
	{
		if ((i & 0x020)!=0)
		{
			SetupMemoryPaging(i, (i & 0x01f), pExpansionMemoryAddress);
		}
	}
}

/* handles 64k and 128k */
void InitialiseMemoryPaging(unsigned long RamConfig)	
{
	SetupNoExpansionMemory();

	if (RamConfig & CPC_RAM_CONFIG_64K_RAM)
	{
		Setup64kRamExpansion(Amstrad_MemoryPages[0]);
	}

	if (RamConfig & CPC_RAM_CONFIG_256K_SILICON_DISK)
	{
		Setup256kSiliconDiskExpansion(Amstrad_MemoryPages[16]);
	}
}




/* bit per 16k expansion ram used */
static unsigned long CPC_RamUsedMask;

/* allocate ram for ram pages */
static void	AllocateRamPages(void)
{
	int i;
	unsigned long	RamMask;	
	unsigned long	RamRequired;

	/* free extra ram */
	FreeExtraRam();

	RamRequired = 0;
	RamMask  = CPC_RamUsedMask;
	/* calculate size of total ram required */
	for (i=0; i<32; i++)
	{
		if (RamMask & 1)
		{
			RamRequired+=16384;
		}

		RamMask = RamMask>>1;
	}

	/* allocate extra ram required */
	Amstrad_ExtraRam = malloc(RamRequired);

	if (Amstrad_ExtraRam!=NULL)
	{
		unsigned char *pRamPtr;

		pRamPtr = Amstrad_ExtraRam;
		RamMask = CPC_RamUsedMask;

		for (i=0; i<32; i++)
		{
			unsigned char *pBlockAddr;

			if (RamMask & 1)
			{
				pBlockAddr = pRamPtr;
				pRamPtr+=16384;
			}
			else
			{
				pBlockAddr = NULL;
			}

			Amstrad_MemoryPages[i] = pBlockAddr;

			RamMask = RamMask>>1;
		}
	}
}

static unsigned long CPC_CurrentRamConfig;

void	CPC_SetRamConfig(unsigned long RamConfig)
{
/*	int i;
	unsigned long RamMask;
*/
	CPC_RamUsedMask = 0;

	CPC_CurrentRamConfig = RamConfig;

	if (RamConfig!=0)
	{
		/* add additional ram required */

		if (RamConfig & CPC_RAM_CONFIG_64K_RAM)
		{
			CPC_RamUsedMask |= 
				(CPC_RAM_BLOCK_0_MASK |
				CPC_RAM_BLOCK_1_MASK |
				CPC_RAM_BLOCK_2_MASK |
				CPC_RAM_BLOCK_3_MASK);
		}

		/* dk'tronics 256k expansion */
/*		if (RamConfig & CPC_RAM_CONFIG_256K_RAM)
		{

			CPC_RamUsedMask |= 
				(CPC_RAM_BLOCK_4_MASK |
				CPC_RAM_BLOCK_5_MASK |
				CPC_RAM_BLOCK_6_MASK |
				CPC_RAM_BLOCK_7_MASK |
				CPC_RAM_BLOCK_8_MASK |
				CPC_RAM_BLOCK_9_MASK |
				CPC_RAM_BLOCK_10_MASK |
				CPC_RAM_BLOCK_11_MASK |
				CPC_RAM_BLOCK_12_MASK |
				CPC_RAM_BLOCK_13_MASK |
				CPC_RAM_BLOCK_14_MASK |
				CPC_RAM_BLOCK_15_MASK |
				CPC_RAM_BLOCK_16_MASK |
				CPC_RAM_BLOCK_17_MASK |
				CPC_RAM_BLOCK_18_MASK |
				CPC_RAM_BLOCK_19_MASK);
		}
*/
		
		/* dk'tronics 256k silicon disk */
		if (RamConfig & CPC_RAM_CONFIG_256K_SILICON_DISK)
		{
			CPC_RamUsedMask |= 
				(CPC_RAM_BLOCK_16_MASK |
				CPC_RAM_BLOCK_17_MASK |
				CPC_RAM_BLOCK_18_MASK |
				CPC_RAM_BLOCK_19_MASK |
				CPC_RAM_BLOCK_20_MASK |
				CPC_RAM_BLOCK_21_MASK |
				CPC_RAM_BLOCK_22_MASK |
				CPC_RAM_BLOCK_23_MASK |
				CPC_RAM_BLOCK_24_MASK |
				CPC_RAM_BLOCK_25_MASK |
				CPC_RAM_BLOCK_26_MASK |
				CPC_RAM_BLOCK_27_MASK |
				CPC_RAM_BLOCK_28_MASK |
				CPC_RAM_BLOCK_29_MASK |
				CPC_RAM_BLOCK_30_MASK |
				CPC_RAM_BLOCK_31_MASK);
		}
	}

	/* allocate extra ram */
	AllocateRamPages();

	InitialiseMemoryPaging(RamConfig);	
}

unsigned long CPC_GetRamConfig(void)
{
	return CPC_CurrentRamConfig;
}
          
void    CPC_SetHardware(int Hardware)
{
        CPC_Hardware = Hardware;

        CRTC_SetCPCMode(CPC_Hardware);

        if (Hardware==CPC_HW_CPCPLUS)
        {    
			pOS = ASIC_GetCartPage(0);
			pBasic = ASIC_GetCartPage(1);
			pDOS = ASIC_GetCartPage(3);
  
            CPC_EnableASICRamWrites(FALSE);

            /* if green screen was specified, and we changed to CPC+ mode,
            then change to grey scale display */
            if (CPC_GetMonitorType()== CPC_MONITOR_GREEN_SCREEN)
            {
                CPC_SetMonitorType(CPC_MONITOR_GREY_SCALE);
			}
        }
        else
        {
            CPC_EnableASICRamWrites(FALSE);

        }

        CRTC_SetRenderFunction(RENDER_MODE_STANDARD);
		
		PPI_UpdatePortBInputData();
}


int     CPC_GetHardware(void)
{
        return CPC_Hardware;
}


BOOL    CPC_Initialise(void)
{
		PAL_Initialise();

/*		Memory_Init(); */

		BrightnessControl_Initialise();

		/* setup rendering tables etc */
		Render_Initialise();

		/* allocate base memory */
		AllocateEmulatorMemory();

		/* allocate additional ram */
		CPC_SetRamConfig(CPC_RAM_CONFIG_64K_RAM | CPC_RAM_CONFIG_256K_SILICON_DISK);	

		/*CPC_SetMemorySize(128); */

		FDD_InitialiseAll();
		
		PSG_Init();

        CRTC_Initialise();

        GateArray_Initialise();
        
        /* set colour mode */
        GateArray_SetMonitorColourMode(MONITOR_MODE_COLOUR);

        /* Initialise rom */
        ExpansionRom_Init();

        /* initialise keyboard */
        CPC_ClearKeyboard();

        /* TAPEFILE */
        /* patch rom for tape file loading/saving */
/*      Tape_PatchRom(Roms6128.pOs, Roms6128.pBasic); */
/*      Z80_InstallPatches(TRUE); */


        /* CPC PLUS EMULATION */

        /* Initialise ASIC */
        ASIC_Initialise();


	Cassette_Init();

   DiskImage_Initialise();

   /* initialise drive 0 */
   FDD_Initialise(0);
   /* initialise drive 1 */
   FDD_Initialise(1);


//   WavOutput_Init("wavout.tmp");

   AudioEvent_Initialise();


		/* initialise z80 emulation */
		Z80_Init();

   return TRUE;
}

void    CPC_Finish(void)
{
	
	Render_Finish();
	Cassette_Finish();

        AudioEvent_Finish();

        Cartridge_Remove();
        
        ASIC_Finish();

        /* clear memory used by any expansion roms */
        ExpansionRom_Finish();

        /* free ram allocated for memory */
        FreeEmulatorMemory();

		/* removes all disc images; doesn't save! */
        DiskImage_Finish();

#ifdef AY_OUTPUT
      YMOutput_Finish();
#endif

//       WavOutput_Finish();

		/* remove any tape image inserted */
		Tape_Remove();
}


/*--------------------------------------------------------------*/

/* read a byte from base memory without memory paging */
Z80_BYTE Z80_RD_BASE_BYTE(Z80_WORD Addr)
{
        return Z80MemoryBase[Addr];
}

/* read a word from base memory without memory paging */
Z80_WORD Z80_RD_BASE_WORD(Z80_WORD Addr)
{
        return (unsigned short)((((Z80_WORD)Z80_RD_BASE_BYTE(Addr)) | (((Z80_WORD)Z80_RD_BASE_BYTE((Z80_WORD)(Addr+1)))<<8)));
}


extern unsigned char *pReadRamPtr[8];

/* read a byte from emulator memory with paging */
Z80_BYTE        Z80_RD_MEM(Z80_WORD Addr)
{
        unsigned long    MemBlock;
        unsigned char                   *pAddr;

        /* calculate 16k page */
        MemBlock = (Addr>>13);	

        /* calculate address to read from */
        pAddr = pReadRamPtr[MemBlock] + Addr;	

        /* return byte at memory address */
        return pAddr[0];
}

/*--------------------------------------------------------------*/

/* write a byte to emulator memory with paging */
void CPC_WriteMemory(Z80_WORD Addr,Z80_BYTE Data)
{
        unsigned int MemBlock;
        unsigned char           *pAddr;

        /* calculate 16k page */
        MemBlock = (Addr>>13) & 0x07;

        /* calculate address to write to */
        pAddr = pWriteRamPtr[MemBlock] + Addr;	

        /* write byte to memory address */
        pAddr[0] = Data;
}

/* write a byte to emulator memory with paging */
void CPCPlus_WriteMemoryWithASICRamEnabled(Z80_WORD Addr,Z80_BYTE Data)
{
        unsigned int MemBlock;
        unsigned char           *pAddr;

        /* calculate 16k page */
        MemBlock = (Addr>>13) & 0x07;

        /* calculate address to write to */
        pAddr = pWriteRamPtr[MemBlock] + Addr;	

        /* write byte to memory address */
        pAddr[0] = Data;

		if ((Addr & 0x04000)!=0)	
		{
			/* execute function which determines if writes
			to ASIC ram have been performed */
			ASIC_WriteRam(Addr,Data);
		}
}

/*BOOL	ASIC_WriteRamEnabled = FALSE; */

void    CPC_EnableASICRamWrites(BOOL Status)
{
/*	ASIC_WriteRamEnabled = Status; */

    if (Status)
    {
         pWriteMemory = CPCPlus_WriteMemoryWithASICRamEnabled;
    }
    else
    {
        pWriteMemory = CPC_WriteMemory;
    }
}


/* write a byte to emulator memory with paging */
void Z80_WR_MEM(Z80_WORD Addr,Z80_BYTE Data)
{
	pWriteMemory(Addr,Data);
}

/*--------------------------------------------------------------------------*/
/* Port Write
  
  bit 15 = 0; Gate Array 
  bit 14 = 0; CRTC Write 
  bit 13 = 0; Select Expansion Rom 
  bit 12 = 0; Printer Port 
  bit 11 = 0; PPI Write 
  bit 10 = bit 7 = 0; FDC Write 
*/

static void AmstradDiscInterface_PortWrite(Z80_WORD Port, Z80_BYTE Data)
{
	unsigned int            Index;

	Index = ((Port & 0x0100)>>(8-1)) | (Port & 0x01);

	switch (Index)
	{
		case 0:
		case 1:
		{
			FDI_SetMotorState(Data);
		}
		break;

		case 2:
		case 3:
		{
			FDC_WriteDataRegister(Data);
		}
		break;

		default:
			break;
	}
}

static Z80_BYTE Amstrad_DiscInterface_PortRead(Z80_WORD Port)
{
	unsigned int            Index;

	Index = ((Port & 0x0100)>>(8-1)) | (Port & 0x01);

	switch (Index)
	{
		case 2:
		{
			return FDC_ReadMainStatusRegister();
		}
		break;

		case 3:
		{
			return FDC_ReadDataRegister();
		}
		break;
	
		default:
			break;
	}

	return 0x0ff;
}


void	CPC_OR_CPCPLUS_Out(Z80_WORD Port, Z80_BYTE Data)
{

    if ((Port & 0x0c000)==0x04000)
    {
		/* gate array cannot be selected if CRTC is also
		selected */
        GateArray_Write(Data);
	}


	if (Amstrad_RamExpansion_Enabled)
	{
		if ((Port & 0x08000)==0x00000)
		{
			/* RAM expansion PAL16L8 write in CPC6128*/
			PAL_WriteConfig(Data);
		}
	}

    if ((Port & 0x04000)==0)
    {
		/* crtc selected */

        unsigned int            Index;

        Index = (Port>>8) & 0x03;	

		switch (Index)
		{
			case 0:
			{
				CRTC_RegisterSelect(Data);
			}
			break;

			case 1:
			{
				CRTC_WriteData(Data);
			}
			break;

			default:
				break;
		}
	}

    if ((Port & 0x02000)==0)
    {
            ROM_Select(Data);
    }

	if ((Port & 0x01000)==0)
	{
		/* on CPC, write to printer through LS chip */
		Printer_WriteDataByte(Data);
	}
		
	if ((Port & 0x0800)==0)
    {
        unsigned int            Index;

        Index = (Port & 0x0300)>>8;

		switch (Index)
		{
			case 0:
			{
				PPI_WritePortA(Data);
			}
			break;

			case 1:
			{
				PPI_WritePortB(Data);
			}
			break;

			case 2:
			{
				PPI_WritePortC(Data);
			}
			break;

			case 3:
			{
				PPI_WriteControl(Data);
			}
			break;
		}
    }

	if (Amstrad_DiscInterface_Enabled)
	{
		if ((Port & 0x0480)==0) 
		{
			AmstradDiscInterface_PortWrite(Port, Data);
		}
	}
}

/*

  4 = s = 5v
  2 = d = 5v
  3 = c = 8.
  1 = r = reset/request.
  10 = s = 5v
	12 = d = 8
  11 = c = aic
  13 = r = mf/db4 reset.


  s = pr
  d = d
  c = clk
  r = clr

  pr d
*/






/* 
	Z8536 OPERATION IN KC COMPACT:

	Port C - Timer - Handles interrupts

	PC0 = Counter/Timer Output = AIC
	PC1 = Counter Input = CRTC HSYNC
	PC2 = Trigger Input = VSYV
	PC3 = Gate Input = 5V


  VSYV is TRUE if a HSYNC ended while VSYNC was active.


  */


/*
10 = PR = 5V
12 = D = D2 = VSYNC
11 = C = CLK 2 = /HSYNC
13 = CLR = 5V

10  13  /HSYNC	VSYNC		VSYV
H	H	^		H			H
H	H	^		L			L

/HSYNC +VE TRANSITION.

HSYNC ACTIVE /HSYNC = 0
HSYNC NOT ACTIVE /HSYNC = 1

/HSYNC INPUT TRIGGERS WHEN HSYNC ENDS.
VSYV = TRUE IF HSYNC TRIGGERS DURING VSYNC
*/

#include "z8536.h"
extern Z8536 z8536;

static int kcc_int_ff;


void	KCC_AcknowledgeInterrupt(void)
{
	kcc_int_ff = 0;
}

static int previous_counter_outputs=0;

void	KCC_Update(void)
{
	unsigned char port_c_inputs;

	port_c_inputs = 0;

	if (CRTC_InternalState.CRTC_Flags & CRTC_HS_FLAG)
	{
		port_c_inputs |= 0x02;
	}

	if (CRTC_InternalState.CRTC_Flags & CRTC_VS_FLAG)
	{
		port_c_inputs |= 0x04;
	}

	z8536.inputs[2] = port_c_inputs;

	Z8536_Update();


	if (((previous_counter_outputs^z8536.counter_outputs[2])&0x01)!=0)
	{
		/* changed */

		if (z8536.counter_outputs[2]&0x01)
		{
			/* positive edge */

			kcc_int_ff = 1;
		}
	
		previous_counter_outputs = z8536.counter_outputs[2];
	}


	if (kcc_int_ff)
	{
		Z80_SetInterruptRequest();
	}
}


void	KCCompact_Out(Z80_WORD Port, Z80_BYTE Data)
{
	if ((Port & 0x08000)==0)
    {
		/* gate array can be selected if CRTC is also
		selected */
        GateArray_Write(Data);
    }

    if ((Port & 0x04000)==0)
    {
		/* crtc selected */

        unsigned int            Index;

        Index = (Port>>8) & 0x03;	

		switch (Index)
		{
			case 0:
			{
				CRTC_RegisterSelect(Data);
			}
			break;

			case 1:
			{
				CRTC_WriteData(Data);
			}
			break;

			default:
				break;
		}
	}

    if ((Port & 0x02000)==0)
    {
            ROM_Select(Data);
    }

	if ((Port & 0x01000)==0)
	{
		/* CIO chip */
		unsigned int Index;

		Index = ((Port^0x0100) & 0x0300)>>8;

		Z8536_WriteData(Index, Data);
	}
	
	if ((Port & 0x0800)==0)
    {
        unsigned int            Index;

        Index = (Port & 0x0300)>>8;

		switch (Index)
		{
			case 0:
			{
				PPI_WritePortA(Data);
			}
			break;

			case 1:
			{
				PPI_WritePortB(Data);
			}
			break;

			case 2:
			{
				PPI_WritePortC(Data);
			}
			break;

			case 3:
			{
				PPI_WriteControl(Data);
			}
			break;

			default:
				break;
		}
    }

	if (Amstrad_DiscInterface_Enabled)
	{
		if ((Port & 0x0480)==0) 
		{
			AmstradDiscInterface_PortWrite(Port, Data);
		}
	}
}



/* Write data to a I/O port */
void    Z80_DoOut(Z80_WORD Port,Z80_BYTE Data)
{
	switch (CPC_GetHardware())
	{
		case CPC_HW_CPC:
		case CPC_HW_CPCPLUS:
		{
			CPC_OR_CPCPLUS_Out(Port,Data);
		}
		break;

		case CPC_HW_KCCOMPACT:
		{
			KCCompact_Out(Port,Data);
		}
		break;
	}
		        
	if (NumWritePorts!=0)
	{
		int i;

		for (i=0; i<NumWritePorts; i++)
		{
			if ((Port&writePorts[i].PortAnd)==writePorts[i].PortCmp)
				writePorts[i].pWriteFunction(Port, Data);
		}
	}
}

/*--------------------------------------------------------------------------*/
/* Port Read
  
  bit 14 = 0; CRTC Read 
  bit 11 = 0; PPI Read 
  bit 10 = bit 7 = 0; FDC Read 
*/


/* In of port &efxx on CPC?! */

Z80_BYTE        CPC_In(Z80_WORD Port)
{
        unsigned int Data=0x0ff;

        if ((Port & 0x04000)==0)
        {
            unsigned int            Index;

            Index = (Port & 0x0300)>>8;

			switch (Index)
			{
				case 2:
				{
					Data = CRTC_ReadStatusRegister();
				}
				break;

				case 3:
				{
					Data = CRTC_ReadData();
				}
				break;

				default:
					break;
			}
        }

        if ((Port & 0x0800)==0)
        {
            unsigned int            Index;

            Index = (Port & 0x0300)>>8;

			switch (Index)
			{
				case 0:
				{
					Data = PPI_ReadPortA();
				}
				break;

				case 1:
				{
					Data = PPI_ReadPortB();
				}
				break;

				case 2:
				{
					Data = PPI_ReadPortC();
				}
				break;

				case 3:
				{
					Data = PPI_ReadControl();
				}
				break;

				default:
					break;
			}
        }

		if (Amstrad_DiscInterface_Enabled)
		{
			if ((Port & 0x0480)==0)
			{
				Data = Amstrad_DiscInterface_PortRead(Port);
			}
		}
			
		if (NumReadPorts!=0)
		{
			int i;

			for (i=0; i<NumReadPorts; i++)
			{
				if ((Port&readPorts[i].PortAnd)==readPorts[i].PortCmp)
					Data = readPorts[i].pReadFunction(Port);
			}
		}

        return (Z80_BYTE)Data;
}


/* In of printer on CPC+? */
Z80_BYTE        CPCPlus_In(Z80_WORD Port)
{
		/* CPC6128+ gives 0x079, CPC464+ gives 0x078 */
		unsigned int Data=0x079;

        if ((Port & 0x0c000)==0x04000)
        {
            GateArray_Write(Data);
        }

        if ((Port & 0x04000)==0)
        {
            unsigned int            Index;

            Index = (Port & 0x0300)>>8;

			switch (Index)
			{
				case 0:
				{
					CRTC_RegisterSelect(Data);
				}
				break;

				case 1:
				{
					CRTC_WriteData(Data);
				}
				break;

				case 2:
				{
					Data = CRTC_ReadStatusRegister();
				}
				break;

				case 3:
				{
					Data = CRTC_ReadData();
				}
				break;
			}
        }

        if ((Port & 0x0800)==0)
        {
            unsigned int            Index;

            Index = (Port & 0x0300)>>8;

			switch (Index)
			{
				case 0:
				{
					Data = PPI_ReadPortA();
				}
				break;

				case 1:
				{
					Data = PPI_ReadPortB();
				}
				break;

				case 2:
				{
					Data = PPI_ReadPortC();
				}
				break;

				case 3:
				{
					Data = PPI_ReadControl();
				}
				break;

				default:
					break;
			}
        }


		if (Amstrad_DiscInterface_Enabled)
		{
			if ((Port & 0x0480)==0)
			{
				Data = Amstrad_DiscInterface_PortRead(Port);
			}
		}

		if (NumReadPorts!=0)
		{
			int i;

			for (i=0; i<NumReadPorts; i++)
			{
				if ((Port&readPorts[i].PortAnd)==readPorts[i].PortCmp)
					Data = readPorts[i].pReadFunction(Port);
			}
		}

        return (Z80_BYTE)Data;
}


Z80_BYTE        KCCompact_In(Z80_WORD Port)
{
        unsigned int Data=0x0ff;


		if ((Port & 0x01000)==0)
		{
			int Index;

			Index = ((Port^0x0100) & 0x0300)>>8;

			Data = Z8536_ReadData(Index);
		}

        if ((Port & 0x04000)==0)
        {
            unsigned int            Index;

            Index = (Port & 0x0300)>>8;

			switch (Index)
			{
				case 2:
				{
					Data = CRTC_ReadStatusRegister();
				}
				break;

				case 3:
				{
					Data = CRTC_ReadData();
				}
				break;

				default:
					break;
			}
        }

        if ((Port & 0x0800)==0)
        {
                unsigned int            Index;

                Index = (Port & 0x0300)>>8;

				switch (Index)
				{
					case 0:
					{
						Data = PPI_ReadPortA();
					}
					break;

					case 1:
					{
						Data = PPI_ReadPortB();
					}
					break;

					case 2:
					{
						Data = PPI_ReadPortC();
					}
					break;

					case 3:
					{
						Data = PPI_ReadControl();
					}
					break;

					default:
						break;
				}
        }


		if (Amstrad_DiscInterface_Enabled)
		{
			if ((Port & 0x0480)==0)
			{
				Data = Amstrad_DiscInterface_PortRead(Port);
			}
		}

		if (NumReadPorts!=0)
		{
			int i;

			for (i=0; i<NumReadPorts; i++)
			{
				if ((Port&readPorts[i].PortAnd)==readPorts[i].PortCmp)
					Data = readPorts[i].pReadFunction(Port);
			}
		}

        return (Z80_BYTE)Data;
}

Z80_BYTE        Z80_DoIn(Z80_WORD Port)
{
	switch (CPC_GetHardware())
	{
		case CPC_HW_CPC:
			return CPC_In(Port);
		
		case CPC_HW_CPCPLUS:
			return CPCPlus_In(Port);

		case CPC_HW_KCCOMPACT:
			return KCCompact_In(Port);
	}
	
	return 0x0ff;
}

/*----------------------------------------------------------------------------*/

void	CPC_ResetNopCount(void)
{
	NopCount = 0;
}



void    CPC_UpdateNopCount(unsigned long NopsToAdd)
{
        NopCount+=NopsToAdd;
}

unsigned long CPC_GetNopCount(void)
{
        return NopCount;
}

void    Z80_AcknowledgeInterrupt(void)
{
        /* interrupt acknowledge func */
        
        /* CRTC acknowledge int */
        GateArray_AcknowledgeInterrupt();
        /* ASIC acknowledge int */
        ASIC_AcknowledgeInterrupt();

        Z80_ClearInterruptRequest();

		KCC_AcknowledgeInterrupt();
}


void    CPC_ReleaseKeys(void)
{
	memset(KeyboardData, 0x0ff, 16);
}


void	CPC_UpdateColours(void)
{
	if (CPC_GetHardware()==CPC_HW_CPCPLUS)
	{
		/* CPC+ mode */

		ASIC_UpdateColours();
	}
	else
	{
		/* CPC mode */

		GateArray_UpdateColours();
	}
}

