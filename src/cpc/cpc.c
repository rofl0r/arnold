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
#include "cpcdefs.h"
#include "debugmain.h"
#include "host.h"
#include "dirstuff.h"
#include "yiq.h"
#include "z8536.h"
#ifdef SPEECH
#include "spo256.h"
#endif

void	CheatSystem_Initialise(void);


#ifdef KEY_DEBUG
#include "debugger/debug.h"
extern char DebugString[256];
extern DEBUG_HANDLE Debug_Keyboard;
#endif

/*#define AMX
#define WESTPHASER */

#include "fdc.h"
#include "crtc.h"
#include "garray.h"
#include "asic.h"
#include "z80/z80.h"
#include "cpcglob.h"    
#include "diskimage/diskimg.h"
#include "printer.h"
#include "sampload.h"
#include "cheatdb.h"
#ifdef MULTIFACE
#include "multface.h"
#endif

/* Hardware we are emulating */
/* CPC, PLUS or KC Compact */
static int CPC_Hardware;


static unsigned long   NopCount = 0;

/* pointer to CPC Amsdos rom, used for 464, 664 and 6128 CPC */
/* NOT used for KC Compact or CPC+ */
static unsigned char *pCPCAmsdos;       

/* these hold pointers to the Basic, OS, Amsdos for each CPC type */
static AMSTRAD_ROMS     Roms464;
static AMSTRAD_ROMS     Roms664;
static AMSTRAD_ROMS     Roms6128;
static AMSTRAD_ROMS		RomsKCC;

/* if CPC464/664/6128 or KC Compact selected these pointers are defined */
/* DOS rom is NULL if no DOS is on the motherboard */
/* basic rom */
static unsigned char *pBasic;

/* os rom */
unsigned char *pOS;

/* dos rom */
static unsigned char *pDOS;


static unsigned long CPC_CassetteType = CASSETTE_TYPE_NONE;

void	CPC_SetCassetteType(int CassetteType)
{
	CPC_CassetteType = CassetteType;
}

unsigned long CPC_GetCassetteType(void)
{
	return CPC_CassetteType;
}


/******************/
/* Expansion ROMs */

/* in the CPC, the BASIC rom is visible in all rom-slots unless there is a */
/* rom in a rom-board at the same slot. In this case the rom on the romboard */
/* overrides the BASIC rom */

/* in the CPC+, the rom data from the cartridge can be overriden by a rom */
/* in a rom-board */

/* this table holds the pointers for each expansion ROM selection that can */
/* be made by writing a rom-slot index to the ROM select I/O port */

/* this table is refreshed when a cartridge is loaded, a expansion rom is enabled */
/* or the computer is changed through the UI */
static unsigned char *ExpansionROMTable[256];

/* this is the index of the currently selected rom as written by the ROM select I/O */
static int     RomSelection;   

/* selected expansion rom */
unsigned char   *pUpperRom;

/* 16k position in read memory map of rom */
unsigned long	LowerRomIndex;
/* lower rom */
unsigned char	*pLowerRom;

/********************/

/* keyboard data */
static int              SelectedKeyboardLine;

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

void	AmxMouse_UpdateMovement(unsigned long);

extern unsigned char    *pReadRamPtr[8];
extern unsigned char    *pWriteRamPtr[8];

extern  unsigned char *Z80MemoryBase;

extern CRTC_INTERNAL_STATE CRTC_InternalState;


unsigned char Joystick_ReadJoystickPort(void)
{
	return KeyboardData[9];
}

unsigned char WestPhaser_ReadJoystickPort(void);
unsigned char AmxMouse_ReadJoystickPort(void);
unsigned char SpanishLightGun_ReadJoystickPort(void);

unsigned char Keyboard_Read(void)
{
    if (SelectedKeyboardLine!=9)
    {
            return KeyboardData[SelectedKeyboardLine];
    }
    else
    {
		unsigned char Data = 0x07f;

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
				Data = Joystick_ReadJoystickPort();
				break;
		}		

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
unsigned short ExpansionRom_GetWord(unsigned char *pAddr, int Offset)
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

BOOL    ExpansionRom_Validate(unsigned char *pData, unsigned long DataSize)
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
                unsigned char   *pNames = pData + (NameTableAddr & 0x03fff);
                int             NameCount = 0;

                do
                {
                        /* the last character in each name has bit 7 set */
                        if (pNames[0] & 0x080)
                        {
                                NameCount++;
                        }

                        /* a 0 indicates the end of the name table */
                        if (pNames[0]==0x00)
                                break;
                
                        pNames++;

                        /* if we were reading through rom data and ran out of
                        space the name table is damaged */
                        if (pNames>=(pData+0x04000))
                                return FALSE;
                }
                while (1==1);

                /* must have at least one name = name for startup of rom */
                if (NameCount==0)
                        return FALSE;
        }

        return TRUE;
}

static char     RomName[64];

/* return expansion rom name */
char    *ExpansionRom_GetRomName(int RomIndex)
{
        unsigned char *pRomData = ExpansionRomData[RomIndex];
        
        if (pRomData!=NULL)
        {
                unsigned short NameTableAddr;
                unsigned char *pName;
                unsigned int RomMark = pRomData[1] & 0x0ff;
                unsigned int RomVersion = pRomData[2] & 0x0ff;
                unsigned int RomModification = pRomData[3] & 0x0ff;

                int i;

                /* get rom name */

                /* get address of name table */
                NameTableAddr = ExpansionRom_GetWord(pRomData, 4);

                /* get pointer to name */
                pName = pRomData + (NameTableAddr & 0x03fff);

                i = 0;
                do
                {
                        /* get name chars stripping of high bit */
                        RomName[i] = (unsigned char)(pName[i] & 0x07f);
                
                        /* if high bit set, then got to end of name */
                        if ((pName[i] & 0x080)==0x080)
                                break;
                
                        i++;

                }
                while (1==1);

                i++;

                RomName[i] = ' ';

                i++;

                /* put version number on name string */
                sprintf(&RomName[i],"[v%1x.%01x%01x]",RomMark, RomVersion, RomModification);

                return RomName;
        }

        return "EMPTY SLOT";
}
 

static unsigned int AMSDOS_CalculateChecksum(unsigned char *pHeader)
{
        unsigned int Checksum;
        int i;

        Checksum = 0;

        for (i=0; i<67; i++)
        {
                unsigned int CheckSumByte;

                CheckSumByte = pHeader[i] & 0x0ff;

                Checksum+=CheckSumByte;
        }

        return Checksum;
}

static BOOL     AMSDOS_HasAmsdosHeader(unsigned char *pHeader)
{
        unsigned int CalculatedChecksum;
        unsigned int ChecksumFromHeader;

        CalculatedChecksum = AMSDOS_CalculateChecksum(pHeader);

        ChecksumFromHeader = (pHeader[67] & 0x0ff) |
                                                (pHeader[68] & 0x0ff)<<8;

        if (ChecksumFromHeader == CalculatedChecksum)
                return TRUE;

        return FALSE;
}


/* insert a expansion rom */
BOOL    ExpansionRom_Insert(char *RomFilename, int RomIndex)
{
        unsigned char *RomData = NULL;
        unsigned long RomDataSize = 0;

        /* remove an existing rom */
        if (ExpansionRomData[RomIndex]!=NULL)
        {
                ExpansionRom_Remove(RomIndex);
        }

        /* load new rom data */
        Host_LoadFile(RomFilename, &RomData, &RomDataSize);
        
        if (RomData!=NULL)
        {
                /* as long as rom data is large enough to hold a amsdos header, we can check it
                for one */
                if (RomDataSize>128)
                {
                        /* does rom data have a amsdos header? */
                        if (AMSDOS_HasAmsdosHeader(RomData))
                        {
                                /* does have a amsdos header */
                                unsigned char *NewRomData;

                                /* allocate size of rom minus header */
                                NewRomData = malloc(RomDataSize-128);

                                if (NewRomData!=NULL)
                                {
                                        /* copy data to new rom block (data after header) */
                                        memcpy(NewRomData, RomData+128, RomDataSize-128);
                                
                                        /* free old data */
                                        free(RomData);
                                }
                        
                                /* set pointer to point to new block */
                                RomData = NewRomData;
                                RomDataSize = RomDataSize-128;
                        }
                }
        
                if (RomDataSize>0)
                {
                        /* loaded data */
                        if (ExpansionRom_Validate(RomData,RomDataSize))
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
                                        memcpy(ExpansionRomData[RomIndex],RomData,RomDataSize);
                        
                                        /* mark it as active */
                                        ExpansionRom_SetActiveState(RomIndex, TRUE);
                                        
                                        free(RomData);
                                        return TRUE;
                                }
                        }
                }

                /* free rom data */
                free(RomData);
        }

        return FALSE;
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
	RAM_ROM_SetupTable();

	GateArray_RethinkMemory();
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

typedef struct PPI_8255
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


#define CPCPLUS_8255_STATE_ACTIVE 0x0001

static unsigned char PPI_PSG_LatchedOperation = 0;
static int PPI_State = 0;

static void     UpdatePSG(void)
{
	/* psg databus at high impedance */
	ppi8255.inputs[0] = 0x0ff;

    switch (PPI_PSG_LatchedOperation)
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
 

/* refresh hardware connected to port C depending on outputs from the port */
void    Amstrad_WritePPIPortC(void)
{
	unsigned char Data;

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
		unsigned char DataBit;

		if ((Data & (1<<5))!=0)
		{
			DataBit = 0x0ff;
		}
		else
		{
			DataBit = 0x00;
		}

		/* write data to audio event */
		AudioEvent_AddEventToBuffer(AUDIO_EVENT_TAPE, DataBit, DataBit);
	
		if (PPI_CurrentPortC & 0x010)
		{
			unsigned long CurrentNopCount;
/*			unsigned char CassetteReadBit; */
			unsigned long NopsPassed;

			/* get current nop count */
			CurrentNopCount = CPC_GetNopCount();

			NopsPassed = CurrentNopCount - PortCWrite_PreviousNopCount;

			TZX_Write(NopsPassed, (Data>>5) & 0x01);
													
			/* store previous port B read nop count */
			PortCWrite_PreviousNopCount = CurrentNopCount;
		}

	}

	SelectedKeyboardLine = Data & 0x0f; 

	{
		unsigned char OperationCode = (unsigned char)((Data>>6) & 0x03);

		/* cpc+? */
		if (CPC_GetHardware()==CPC_HW_CPCPLUS)
		{
			/* cpc+ */
			if (PPI_State & CPCPLUS_8255_STATE_ACTIVE)
			{
				/* active */
				
				/* if code is 0, go back to inactive otherwise stay in current state */
				if (OperationCode==0)
				{
					PPI_PSG_LatchedOperation = 0;
					PPI_State &= ~CPCPLUS_8255_STATE_ACTIVE;
				}
			}
			else
			{
				/* inactive */

				/* if 0, stay inactive, otherwise do operation specified */
				PPI_PSG_LatchedOperation = OperationCode;

				if (OperationCode!=0)
				{
					PPI_State |= CPCPLUS_8255_STATE_ACTIVE;
				}
			}
		}
		else
		{
			/* cpc */
			PPI_PSG_LatchedOperation = OperationCode;
		}
	}

    UpdatePSG();

	/* tape motor */
	if (((PPI_PreviousPortC^PPI_CurrentPortC)&0x010)!=0)
	{
		/* changed state! */

		if (PPI_CurrentPortC & 0x010)
		{
			/* tape motor has been switched on */
			PortBRead_PreviousNopCount = CPC_GetNopCount();
			PortCWrite_PreviousNopCount = CPC_GetNopCount();
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

void PPI_DoPortBInput(void)
{
		unsigned char Data = PPI_PortBInputData;
		
        /* set state of vsync bit */
        if (CRTC_InternalState.CRTC_Flags & CRTC_VS_FLAG)                    
        {
                Data |= VSYNC_ACTIVE;
        }

		/**** TAPE ****/
		/* tape motor is on? */
		if (PPI_CurrentPortC & 0x010)
		{
			unsigned long CurrentNopCount;
			unsigned char CassetteReadBit;

			/* get current nop count */
			CurrentNopCount = CPC_GetNopCount();

			switch (CPC_CassetteType)
			{
				case CASSETTE_TYPE_SAMPLE:
				{
					CassetteReadBit = Sample_GetDataByteTimed(PortBRead_PreviousNopCount, CurrentNopCount);
				}
				break;

				case CASSETTE_TYPE_TAPE_IMAGE:
				{
					unsigned long TStatesPassed;

					/* 4 t-states per NOP */
					TStatesPassed = (CurrentNopCount - PortBRead_PreviousNopCount)<<2;
                
					CassetteReadBit = TapeImage_GetBit(TStatesPassed);
				}
				break;
			
				default:
					CassetteReadBit = 0;
					break;
			}
			
#if 0
			{
				int TapeAudio;

				if (CassetteReadBit)
				{
					TapeAudio = 128;
				}
				else
				{
					TapeAudio = 0;
				}


				AudioEvent_AddEventToBuffer(AUDIO_EVENT_DIGIBLASTER, TapeAudio, TapeAudio);
			}
#endif

	        Data |= (CassetteReadBit<<7);
		
			/* store previous port B read nop count */
			PortBRead_PreviousNopCount = CurrentNopCount;

		}
        

        ppi8255.inputs[1] = Data;

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

    UpdatePSG();
}

void    PPI_WritePortB(int Data)
{
	ppi_write_port(1, Data);
}

void	PPI_WritePortC(int Data)
{
	ppi_write_port(2, Data);

	Amstrad_WritePPIPortC();
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
		}
		else
		{
			/* CPC type is CPC+ */

            /* with CPC+ port B is always in input mode. Therefore it always
            returns the inputs to the port */
			PPI_PortControl |= PPI_CONTROL_PORT_B_STATUS;

            /* on CPC+ port C is always in output mode */
            PPI_PortControl &= ~(PPI_CONTROL_PORT_C_LOWER_STATUS | PPI_CONTROL_PORT_C_UPPER_STATUS);
		}

		ppi8255.control = PPI_PortControl;

		if (PPI_PortControl & PPI_CONTROL_PORT_A_STATUS)
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
        if (PPI_PortControl & PPI_CONTROL_PORT_B_STATUS)
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

        if ((PPI_PortControl & PPI_CONTROL_PORT_C_LOWER_STATUS)==0)
        {
            /* lower part of port is in output mode */
			
			ppi8255.io_mask[2] &= 0x0f0;
        }

        if ((PPI_PortControl & PPI_CONTROL_PORT_C_UPPER_STATUS)==0)
        {
            /* upper part of port is in output mode */
            ppi8255.io_mask[2] &= 0x00f;
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
    }


    PPI_WritePortA(ppi8255.latched_outputs[0]);
	PPI_WritePortB(ppi8255.latched_outputs[1]);
	PPI_WritePortC(ppi8255.latched_outputs[2]);
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

	Amstrad_WritePPIPortC();
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
                        pOS = Roms464.pOs;
                        pBasic = Roms464.pBasic;
                        pDOS = pCPCAmsdos;

						/* if CPC, do not allow type 3 to be selected */
						if (CPC_GetCRTCType()==3)
						{
							CPC_SetCRTCType(0);
						}
				}
                break;

                case CPC_TYPE_CPC664:
                {
                        pOS = Roms664.pOs;
                        pBasic = Roms664.pBasic;
                        pDOS = pCPCAmsdos;
         
						/* if CPC, do not allow type 3 to be selected */
						if (CPC_GetCRTCType()==3)
						{
							CPC_SetCRTCType(0);
						}

				}
                break;

                case CPC_TYPE_CPC6128:
                {
                        pOS = Roms6128.pOs;
                        pBasic = Roms6128.pBasic;
                        pDOS = pCPCAmsdos;
        
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
					pOS = RomsKCC.pOs;
					pBasic = RomsKCC.pBasic;
					pDOS = pCPCAmsdos;
        
					/* force type 0, my KC Compact has a HD6845S */
					/* this is likely to be the only type of CRTC */
					/* used by this system */
					CPC_SetCRTCType(0);
				}
				break;

                case CPC_TYPE_464PLUS:
                case CPC_TYPE_6128PLUS:
                {
                        pOS = ASIC_GetCartPage(0);
                        pBasic = ASIC_GetCartPage(1);
                        pDOS = ASIC_GetCartPage(3);
         
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
	/* reset lower rom index */
	LowerRomIndex = 0;


	/* refresh vector base */
	switch (CPC_GetCPCType())
	{
			case CPC_TYPE_CPC464:
			case CPC_TYPE_CPC664:
			case CPC_TYPE_CPC6128:
			{
					/* interrupt vector normally on bus on a standard CPC.
					Although, this is not really guaranteed, depending
					on what peripheral's are plugged in etc. NWC mentioned
					that fact. */
					Z80_SetInterruptVector(0x0ff);

					CPC_SetHardware(CPC_HW_CPC);
	#ifdef MULTIFACE
					Multiface_SetMode(MULTIFACE_CPC_MODE);
	#endif
			}
			break;

			case CPC_TYPE_KCCOMPACT:
			{
				Z80_SetInterruptVector(0x0ff);

				CPC_SetHardware(CPC_HW_KCCOMPACT);
	#ifdef MULTIFACE
				Multiface_SetMode(MULTIFACE_CPC_MODE);
	#endif
			}
			break;

			case CPC_TYPE_464PLUS:
			case CPC_TYPE_6128PLUS:
			{
					/* set mode */
					CPC_SetHardware(CPC_HW_CPCPLUS);

	#ifdef MULTIFACE
					Multiface_SetMode(MULTIFACE_CPCPLUS_MODE);
	#endif
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

#ifdef MULTIFACE
    Multiface_Reset();
#endif
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
			pBlockAddr = Z80MemoryBase;	
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


/**************************************************************************
  
 INITIALISATION

 **************************************************************************/
/*
static PATCH_ENTRY      Amsdos_Patch[]=
{
        {0x07e0,0x0c9},
        {0x07d0,0x000},
        {0x07d1,0x000},
        {0x0931,0x000},
        {0x0932,0x000},
        {0x0933,0x000},
        {0x0934,0x000},
        {0x0935,0x000},
        {0x096e,0x000},
        {0x096f,0x000},
        {0x0970,0x000},
        {0x0972,0x000},
        {0x0973,0x000},
        {0x05d4,0x001},
        {0x05d5,0x000},
        {0x05d6,0x001},
        {0x05d7,0x000}
};

#define NUM_AMSDOS_PATCH_ENTRIES (sizeof(Amsdos_Patch)/sizeof(PATCH_ENTRY))

static PATCH_ENTRY      PlusAmsdos_Patch[]=
{
        {0x080c,0x0c9},
        {0x07fc,0x000},
        {0x07fd,0x000},
        {0x095f,0x000},
        {0x0960,0x000},
        {0x0961,0x000},
        {0x0962,0x000},
        {0x099d,0x000},
        {0x099e,0x000},
        {0x09b7,0x000},
        {0x09b8,0x000},
        {0x09b9,0x000},
        {0x09ba,0x000},
        {0x05d4,0x001},
        {0x05d5,0x000},
        {0x05d6,0x001},
        {0x05d7,0x000}
};

#define NUM_PLUS_AMSDOS_PATCH_ENTRIES (sizeof(PlusAmsdos_Patch)/sizeof(PATCH_ENTRY))
        if (pCPCAmsdos!=NULL)
        {
                for (i=0; i<NUM_AMSDOS_PATCH_ENTRIES; i++)
                {
                        int             Data;

                        Data = Amsdos_Patch[i].Byte & 0x0ff;

                        pCPCAmsdos[(long)Amsdos_Patch[i].Addr] = Data;
                }
        }


         Patch ASIC Amsdos 
        
        RomBase = ASIC_GetCartPage(3);

        if (RomBase!=NULL)
        {
                for (i=0; i<NUM_PLUS_AMSDOS_PATCH_ENTRIES; i++)
                {
                        int             Data;

                        Data = PlusAmsdos_Patch[i].Byte & 0x0ff;

                        RomBase[(long)PlusAmsdos_Patch[i].Addr] = Data;
                }
        }       
*/

/* can be called at any time to set memory size */
/*
BOOL	CPC_SetMemorySize(int MemorySizeInK)
{
    if (AllocateEmulatorMemory(MemorySizeInK))
	{
		InitialiseMemoryPaging(MemorySizeInK);

		return TRUE;
	}

	return FALSE;
}
*/

BOOL    CPC_Initialise(void)
{
        unsigned long RomLength;

		RAM_ROM_Initialise(16);

/*		Vortex_Initialise(); */

		BrightnessControl_Initialise();

		/* setup rendering tables etc */
		Render_Initialise();

		/* allocate base memory */
		AllocateEmulatorMemory();

		/* allocate additional ram */
		CPC_SetRamConfig(CPC_RAM_CONFIG_64K_RAM | CPC_RAM_CONFIG_256K_SILICON_DISK);	

		/*CPC_SetMemorySize(128); */

		FDD_InitialiseAll();

		CheatSystem_Initialise();
        
		
		PSG_Init();

        CRTC_Initialise();

        GateArray_Initialise();
        
        /* set colour mode */
        GateArray_SetMonitorColourMode(MONITOR_MODE_COLOUR);

        /* Initialise rom */
        ExpansionRom_Init();

        /* initialise keyboard */
        CPC_ClearKeyboard();

        /* CPC EMULATION */

        /* same amsdos rom for 464,664 and 6128 */

		ChangeToLocationDirectory(EMULATOR_ROM_CPCAMSDOS_DIR);

        Host_LoadFile("amsdos.rom", &pCPCAmsdos,&RomLength);

		ChangeToLocationDirectory(EMULATOR_ROM_CPC464_DIR);

        /* load roms for CPC464 */
        Host_LoadFile("basic.rom", &Roms464.pBasic, &RomLength);
        Host_LoadFile("os.rom", &Roms464.pOs,&RomLength);

		ChangeToLocationDirectory(EMULATOR_ROM_CPC664_DIR);

        /* load roms for CPC664 */
        Host_LoadFile("basic.rom", &Roms664.pBasic, &RomLength);
        Host_LoadFile("os.rom", &Roms664.pOs, &RomLength);

		ChangeToLocationDirectory(EMULATOR_ROM_CPC6128_DIR);

        /* load roms for CPC6128 */
        Host_LoadFile("basic.rom", &Roms6128.pBasic, &RomLength);
        Host_LoadFile("os.rom", &Roms6128.pOs, &RomLength);

		ChangeToLocationDirectory(EMULATOR_ROM_KCCOMPACT_DIR);

		/* load roms for KC Compact */
        Host_LoadFile("kccbas.rom", &RomsKCC.pBasic, &RomLength);
        Host_LoadFile("kccos.rom", &RomsKCC.pOs, &RomLength);

        /* TAPEFILE */
        /* patch rom for tape file loading/saving */
/*      Tape_PatchRom(Roms6128.pOs, Roms6128.pBasic); */
/*      Z80_InstallPatches(TRUE); */


        /* CPC PLUS EMULATION */

        /* Initialise ASIC */
        ASIC_Initialise();

		ChangeToLocationDirectory(EMULATOR_ROM_CPCPLUS_DIR);
		
        Cartridge_Load("system.cpr");

#ifdef MULTIFACE
        Multiface_Initialise();
#endif

   DiskImage_Initialise();

   /* initialise drive 0 */
   FDD_Initialise(0);
   /* initialise drive 1 */
   FDD_Initialise(1);

   TZX_Write_Initialise("out.tmp");

	TapeImage_Init();

#ifdef AY_OUTPUT
   YMOutput_Init("ymout.tmp");
#endif

   WavOutput_Init("wavout.tmp");

   AudioEvent_Initialise();


   if (
	(Z80MemoryBase==NULL) ||
          (Roms464.pBasic==NULL) || 
          (Roms464.pOs==NULL) ||
          (pCPCAmsdos==NULL) ||
          (Roms664.pBasic==NULL) ||
          (Roms664.pOs==NULL) ||
          (Roms6128.pBasic==NULL) ||
          (Roms6128.pOs==NULL)
          )
          {
           return FALSE;
   }


   return TRUE;
}

void    CPC_Finish(void)
{
	TZX_Write_End();

	/* free ram-rom ram */
	RAM_ROM_Finish();
	/* free vortex ram */
/*	Vortex_Finish(); */

	if (RomsKCC.pBasic!=NULL)
	{
		free(RomsKCC.pBasic);
	}

	if (RomsKCC.pOs!=NULL)
	{
		free(RomsKCC.pOs);
	}

        if (pCPCAmsdos!=NULL)
                free(pCPCAmsdos);

        if (Roms464.pBasic!=NULL)
                free(Roms464.pBasic);

        if (Roms464.pOs!=NULL)
                free(Roms464.pOs);

        if (Roms664.pBasic!=NULL)
                free(Roms664.pBasic);

        if (Roms664.pOs!=NULL)
                free(Roms664.pOs);

        if (Roms6128.pBasic!=NULL)
                free(Roms6128.pBasic);

        if (Roms6128.pOs!=NULL)
                free(Roms6128.pOs);

        AudioEvent_Finish();

        Cartridge_Remove();
        
        ASIC_Finish();

        /* clear memory used by any expansion roms */
        ExpansionRom_Finish();

        /* free ram allocated for memory */
        FreeEmulatorMemory();

#ifdef MULTIFACE
        Multiface_Finish();
#endif

        DiskImage_Finish();

#ifdef AY_OUTPUT
        YMOutput_Finish();
#endif

        WavOutput_Finish();

		/* remove any tape image inserted */
		TapeImage_Remove();
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

void	Vortex_Write(Z80_WORD Port, Z80_BYTE Data);

static void	ExpansionPeripherals_Write(Z80_WORD Port, Z80_BYTE Data)
{
#ifdef MULTIFACE
	Multiface_WriteIO(Port,Data);
#endif

#ifdef SPEECH
			/* SSA-1 */
			if (Port == 0x0fbee)
			{
					CPC_SPO256_WriteData(Data);
			}

			/* Dk'Tronics speech */
	/*      if (Port == 0x0fbfe)
	      {
	              CPC_SPO256_WriteData(NewData);
	      }
	*/
#endif

	Magnum_DoOut(Port, Data);

	Vortex_Write(Port, Data);

}

static Z80_BYTE ExpansionPeripherals_Read(Z80_WORD Port)
{
	int Data = 0x0ff;

#ifdef SPEECH
        
        /* SSA-1 */
        if (Port==0x0fbee)
        {
                Data = CPC_SPO256_ReadData();
        }

        /* DK'Tronics speech */
        /*if (Port==0x0fbfe)
        {
              Data = CPC_SPO256_ReadData();
        }*/
#endif

		return (unsigned char)Data;
}


void	CPC_OR_CPCPLUS_Out(Z80_WORD Port, Z80_BYTE Data)
{

#ifdef KEY_DEBUG
	{
		char DebugString[256];

		sprintf(DebugString,"P: %04x D: %02x\r\n",
			Port, Data);

		Debug_WriteString(Debug_Keyboard, DebugString);
	}
#endif

    if ((Port & 0x0c000)==0x04000)
    {
		/* gate array cannot be selected if CRTC is also
		selected */
        GateArray_Write(Data);

		if ((((~Port)>>8) & (0x0ff>>2))==0)
			return;
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

    if ((Port & 0x0480)==0) 
    {
        unsigned int            Index;

        Index = ((Port & 0x0100)>>(8-1)) | (Port & 0x01);

		switch (Index)
		{
			case 0:
			{
				FDD_MotorControl(Data);
			}
			break;

			case 3:
			{
				FDC_WriteDataRegister(Data);
			}
			break;

			default:
				break;
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

    if ((Port & 0x0480)==0) 
    {
       unsigned int            Index;

        Index = ((Port & 0x0100)>>(8-1)) | (Port & 0x01);

		switch (Index)
		{
			case 0:
			{
				FDD_MotorControl(Data);
			}
			break;

			case 3:
			{
				FDC_WriteDataRegister(Data);
			}
			break;

			default:
				break;
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
		        
	/* expansion peripherals */
	ExpansionPeripherals_Write(Port, Data);
}

static unsigned long ActiveDelay;
static int AmxMouse_DelayActive = 0;
static unsigned char AmxMouse_Current;
static unsigned char AmxMouse_Next;
static unsigned char AmxMouse_MovementData;

static int AmxMouse_PreviousNopCount = 0;

unsigned char AmxMouse_ReadJoystickPort(void)
{
	int CurrentNopCount = CPC_GetNopCount();
	int NopsPassed;

	NopsPassed = CurrentNopCount - AmxMouse_PreviousNopCount;

	AmxMouse_UpdateMovement(NopsPassed);

	AmxMouse_PreviousNopCount = CurrentNopCount;

	return AmxMouse_Current;
}

void	AmxMouse_UpdateMovement(unsigned long NopsPassed)
{
	if (AmxMouse_DelayActive)
	{
		if (NopsPassed>=ActiveDelay)
		{
			/* turn off directions */

			AmxMouse_Current |= AmxMouse_MovementData;

			AmxMouse_DelayActive = 0;
		}
		
		ActiveDelay = ActiveDelay - NopsPassed;
	}


}



void	AmxMouse_Update(int DeltaX, int DeltaY, int LeftButton, int RightButton)
{
	AmxMouse_MovementData = 0x00;

	/* left */
	if (DeltaX<0)
	{
		ActiveDelay = (-DeltaX)*3328;

		AmxMouse_MovementData |= 1<<(CPC_KEY_JOY_LEFT & 0x07);
	}
	else if (DeltaX>0)
	{
		ActiveDelay = (DeltaX)*3328;

		AmxMouse_MovementData |= 1<<(CPC_KEY_JOY_RIGHT & 0x07);
	}

	/* up */
	if (DeltaY<0)
	{
		ActiveDelay = (-DeltaY)*3328;

		AmxMouse_MovementData |= 1<<(CPC_KEY_JOY_UP & 0x07);
	}
	else if (DeltaY>0)
	{
		ActiveDelay = (DeltaY)*3328;

		AmxMouse_MovementData |= 1<<(CPC_KEY_JOY_DOWN & 0x07);
	}

	if ((AmxMouse_MovementData & 0x0f)!=0)
	{
		AmxMouse_DelayActive = 1;
	}


	AmxMouse_Current = 0x0ff^AmxMouse_MovementData;

	
	/* fire */
	if (LeftButton)
	{
		AmxMouse_MovementData &= ~(1<<4);
		AmxMouse_Current &= ~(1<<4);
	}
	
	/* fire 2 */
	if (RightButton)
	{
		AmxMouse_MovementData &= ~(1<<5);
		AmxMouse_Current &= ~(1<<5);
	}
}

void	AmxMouse_Reset(void)
{
	AmxMouse_PreviousNopCount = 0;
}

static unsigned char KempstonMouse_Y = 0;
static unsigned char KempstonMouse_X = 0;
static unsigned char KempstonMouse_Buttons = 0x0ff;

void	KempstonMouse_Update(int DeltaX, int DeltaY, int LeftButton, int RightButton)
{
	KempstonMouse_X += DeltaX;
	KempstonMouse_Y += -DeltaY;
	KempstonMouse_Buttons = 0x0ff;

	if (LeftButton)
	{
		KempstonMouse_Buttons &= ~(1<<0);
	}

	if (RightButton)
	{
		KempstonMouse_Buttons &= ~(1<<1);
	}

}

static unsigned char KempstonMouse_Read(Z80_WORD Port, Z80_BYTE Data)
{
	if (Port==0x0fbef)
	{
		return KempstonMouse_Y;
	}

	if (Port==0x0fbee)
	{
		return KempstonMouse_X;
	}

	if (Port==0x0faef)
	{
		return KempstonMouse_Buttons;
	}

	return Data;
}


static unsigned char ExpansionPeripheralsRead(Z80_WORD Port)
{

/*		if ((Port & 0x0400)==0)
		{
			Data = ExpansionPeripherals_Read(Port);
		}

		return Data;
*/
	return 0x0ff;
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

        if ((Port & 0x0480)==0)
        {
            unsigned int            Index;

            Index = ((Port & 0x0100)>>(8-1)) | (Port & 0x01);

			switch (Index)
			{
				case 2:
				{
					Data = FDC_ReadMainStatusRegister();
				}
				break;

				case 3:
				{
					Data = FDC_ReadDataRegister();
				}
				break;
			
				default:
					break;
			}
		}

		
		Data = KempstonMouse_Read(Port, Data);


	/*	ExpansionPeripheralsRead(); */

        return (Z80_BYTE)Data;
}

/* In of printer on CPC+? */
Z80_BYTE        CPCPlus_In(Z80_WORD Port)
{
        unsigned int Data=0x0ff;

        if ((Port & 0x0c000)==0x04000)
        {
			Data = 0x079;
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
					Data = 0x079;
					CRTC_RegisterSelect(Data);
				}
				break;

				case 1:
				{
					Data = 0x079;
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

        if ((Port & 0x0480)==0)
        {
            unsigned int            Index;

            Index = ((Port & 0x0100)>>(8-1)) | (Port & 0x01);

			switch (Index)
			{
				case 2:
				{
					Data = FDC_ReadMainStatusRegister();
				}
				break;

				case 3:
				{
					Data = FDC_ReadDataRegister();
				}
				break;
			
				default:
					break;
			}
		}

		
		Data = KempstonMouse_Read(Port, Data);
		/*ExpansionPeripheralsRead(); */

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

        if ((Port & 0x0480)==0)
        {
            unsigned int            Index;

            Index = ((Port & 0x0100)>>(8-1)) | (Port & 0x01);

			switch (Index)
			{
				case 2:
				{
					Data = FDC_ReadMainStatusRegister();
				}
				break;

				case 3:
				{
					Data = FDC_ReadDataRegister();
				}
				break;
			
				default:
					break;
			}
		}

		/*ExpansionPeripheralsRead(); */

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

void    CPC_ReloadSystemCartridge(void)
{
	/* change to directory for CPC+ roms */
	ChangeToLocationDirectory(EMULATOR_ROM_CPCPLUS_DIR);

	/* load system cartridge */
	Cartridge_Load("system.cpr");

	/* reset CPC */
    CPC_Reset();
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

/***********/
/* VORTEX */

/* 2 blocks 64 32	without vortex
 9 block 288 256 */

/* ram select */
static unsigned char Vortex_RamExpansion_Block = 0x0;
/* allocated ram */
static unsigned char *Vortex_Ram = NULL;
/* pointer to current block */
static unsigned char *Vortex_RamPtr = NULL;

void	Vortex_Initialise(void)
{
	Vortex_Ram = malloc(128*1024);
}

void	Vortex_Finish(void)
{
	if (Vortex_Ram!=NULL)
	{
		free(Vortex_Ram);
		Vortex_Ram = NULL;
	}
}

extern GATE_ARRAY_STATE GateArray_State;

#define VORTEX_RAM_ENABLE	((1<<5) | (1<<2))

void	Vortex_RethinkMemory(void)
{
	if (
		((GateArray_State.RomConfiguration & VORTEX_RAM_ENABLE)==VORTEX_RAM_ENABLE) && 
		((Vortex_RamExpansion_Block & 0x020)!=0)
		)
	{
		pReadRamPtr[0] = pReadRamPtr[1] = Vortex_RamPtr;
		pWriteRamPtr[0] = pWriteRamPtr[1] = Vortex_RamPtr;
	}
}

void	Vortex_Write(Z80_WORD Port, Z80_BYTE Data)
{
	if (Port==0x0fbbd)
	{
		/* bit 6 = 0, bit 5 = 1, bit 2 = 1 */
	/*	if (Data & 0x020) */
		{
			/* ram select */
			Vortex_RamExpansion_Block = Data;

			/* pointer to ram data */
			Vortex_RamPtr = Vortex_Ram + ((Data & 0x07)<<14);

			GateArray_RethinkMemory();
		}

		/* rethink memory */
/*		Vortex_RethinkMemory(); */
	}
}
#if 0
/*************************/
/*** AMRAM/AMRAM 2     ***/

static unsigned char *Amram_RamState = 0;

/* amram - fbf1, fbf0 */

void	Amram_Write(Z80_WORD Port, Z80_BYTE Data)
{
	if (Port==0x0fbf1)
	{
		Amram_RamState = Data;
	}
}
#endif


#if 0
/*************************/
/**** RAM7 MEMCARD    ****/

static unsigned char *MemCard_Ram;

void	MemCard_Write(Z80_WORD Port, Z80_BYTE Data)
{
	/* detect memcard write */
	unsigned char Upper;

	Upper = Port>>8;

	if ((Upper & 0x0fc)==0x07c)
	{
		int BankIndex;

		BankIndex = Upper & 0x03;

	}
}
#endif


/*************************/
/**** INICRON RAM-ROM ****/

/* buffer to hold ram contents; split into banks of 16k */
static unsigned char	*RAM_ROM_RAM = NULL;
/* an array of bits, a bit will be 1 if the corresponding bank is enabled,
and 0 if the corresponding bank is disabled */
static unsigned char	*RAM_ROM_BankEnables = NULL;
/* rom select index for eprom onboard ram-rom (not emulated) */
static unsigned char	RAM_ROM_EPROM_Bank = 0;
/* flags about the ram-rom status */
static unsigned char	RAM_ROM_Flags = 0;
/* number of banks the ram-rom holds */
static unsigned long	RAM_ROM_NumBlocks;
/* the mask will be 0 if ram is write disabled and 0x0ffffffff if the ram is write enabled */
static unsigned long	RAM_ROM_WriteMask;

BOOL	RAM_ROM_IsRamOn(void)
{
	return ((RAM_ROM_Flags & RAM_ROM_FLAGS_RAM_ON)!=0);
}

/* set if ram-rom ram is enabled and therefore if roms can be seen */
void	RAM_ROM_SetRamOnState(BOOL State)
{
	if (State)
	{
		RAM_ROM_Flags |= RAM_ROM_FLAGS_RAM_ON;
	}
	else
	{
		RAM_ROM_Flags &= ~RAM_ROM_FLAGS_RAM_ON;
	}
	
	ExpansionROM_RefreshTable();
}

/* set read/write state for whole ram */
void	RAM_ROM_SetRamWriteEnableState(BOOL State)
{
	if (State)
	{
		RAM_ROM_Flags |= RAM_ROM_FLAGS_RAM_WRITE_ENABLE;
	}
	else
	{
		RAM_ROM_Flags &= ~RAM_ROM_FLAGS_RAM_WRITE_ENABLE;
	}
	ExpansionROM_RefreshTable();
}

/* set if eprom on ram-rom is visible */
void	RAM_ROM_SetEPROMOnState(BOOL State)
{
	if (State)
	{
		RAM_ROM_Flags |= RAM_ROM_FLAGS_EPROM_ON;
	}
	else
	{
		RAM_ROM_Flags &= ~RAM_ROM_FLAGS_EPROM_ON;
	}

	ExpansionROM_RefreshTable();
}

/* true if ram is write enabled, false if ram is write disabled */
BOOL	RAM_ROM_IsRamWriteEnabled(void)
{
	return ((RAM_ROM_Flags & RAM_ROM_FLAGS_RAM_WRITE_ENABLE)!=0);
}

/* true if rom is on and visible, false if off */
BOOL	RAM_ROM_IsEPROMOn(void)
{
	return ((RAM_ROM_Flags & RAM_ROM_FLAGS_EPROM_ON)!=0);
}

/* get selection value for rom */
int		RAM_ROM_GetEPROMBank(void)
{
	return RAM_ROM_EPROM_Bank;
}

/* initialise, allocate memory and setup */
void	RAM_ROM_Initialise(int NumBlocks)
{
	int Size;

	RAM_ROM_NumBlocks = NumBlocks;

	Size = NumBlocks*16*1024;

	RAM_ROM_RAM = malloc(Size);

	if (RAM_ROM_RAM!=NULL)
	{
		memset(RAM_ROM_RAM, 0, Size);
	}
	
	Size = NumBlocks>>3;

	
	RAM_ROM_BankEnables = malloc(Size);

	if (RAM_ROM_BankEnables!=NULL)
	{
		memset(RAM_ROM_BankEnables, 0x0ff, Size);
	}
		
	RAM_ROM_WriteMask = 0;
	RAM_ROM_Flags = 0;

}

void	RAM_ROM_Finish(void)
{
	if (RAM_ROM_RAM!=NULL)
	{
		free(RAM_ROM_RAM);
		RAM_ROM_RAM = NULL;
	}

	if (RAM_ROM_BankEnables!=NULL)
	{
		free(RAM_ROM_BankEnables);
		RAM_ROM_BankEnables = NULL;
	}
}

/* called when ram-rom has changed state and some tables need to be re-setup */
void	RAM_ROM_SetupTable(void)
{
	int i;

	RAM_ROM_WriteMask = 0;

	if (RAM_ROM_RAM==NULL)
		return;

	if ((RAM_ROM_Flags & RAM_ROM_FLAGS_RAM_ON)==0)
		return;

	if (RAM_ROM_Flags & RAM_ROM_FLAGS_RAM_WRITE_ENABLE)
	{
		RAM_ROM_WriteMask = ~0;
	}

	for (i=0; i<16; i++)
	{
		unsigned long BankByte;
		unsigned char BankBit;
		unsigned long BankSelected;

		if (i<RAM_ROM_NumBlocks)
		{
			BankSelected = i;

			BankByte = BankSelected>>3;
			BankBit = BankSelected & 0x07;

			/* if enabled... */
			if (RAM_ROM_BankEnables[BankByte] & (1<<BankBit))
			{
				unsigned char	*RAM_ROM_BankPtr;

				/* get pointer to data */
				RAM_ROM_BankPtr = (unsigned char *)((unsigned long)(RAM_ROM_RAM + (BankSelected<<14)) - (unsigned long)0x0c000);

				/* setup entry in table */
				ExpansionROMTable[i] = RAM_ROM_BankPtr;
			}
		}
	}
}

/* set bank enabled  state - if bank is enabled it is visible */
void	RAM_ROM_SetBankEnable(int Bank, BOOL State)
{
	unsigned long BankByte;
	unsigned char BankBit;

	if (RAM_ROM_BankEnables==NULL)
		return;

	BankByte = Bank>>3;
	BankBit = Bank & 0x07;

	if (State)
	{
		RAM_ROM_BankEnables[BankByte] |= (1<<BankBit);
	}
	else
	{
		RAM_ROM_BankEnables[BankByte] &= ~(1<<BankBit);
	}

	ExpansionROM_RefreshTable();
}

/* true if bank is enabled, false otherwise */
BOOL	RAM_ROM_GetBankEnableState(int Bank)
{
	unsigned long BankByte;
	unsigned char BankBit;

	if (RAM_ROM_BankEnables==NULL)
		return FALSE;

	BankByte = Bank>>3;
	BankBit = Bank & 0x07;

	return ((RAM_ROM_BankEnables[BankByte] & (1<<BankBit))!=0);
}

void	RAM_ROM_RethinkMemory(unsigned char **pReadPtr, unsigned char **pWritePtr)
{	
	unsigned char *pPtr;
	unsigned long Mask = RAM_ROM_WriteMask;

	/* this will be pointer to rom or ram in ram/rom */
	
	/* if mask is all 1's, then pPtr = pReadPtr[6] 
	if mask is all 0's then pPtr = pWritePtr[6]; */
	pPtr = (unsigned char *)(((unsigned long)pReadPtr[6]&Mask)|((unsigned long)pWritePtr[6]&(~Mask)));

	pWritePtr[6] = pWritePtr[7] = pPtr;
}


/**** MAGNUM LIGHT PHASER ****/

static int Magnum_FirePressed = FALSE;
static unsigned char Magnum_PreviousByteWritten = 0x07f;

void	Magnum_Update(int X, int Y, int FirePressed)
{
	Magnum_FirePressed = FirePressed;

	if (Magnum_FirePressed)
	{
	unsigned long Nops;
	unsigned long MonitorLine;
	unsigned long LinesToTrigger;
	unsigned long MonitorX;
	unsigned long CharsToTrigger;

	/* get current monitor line we are on. */
	MonitorLine = CRTC_InternalState.Monitor_State.MonitorScanLineCount;

	/* work out number of complete lines until the trigger should occur */
	if (Y>MonitorLine)
	{
		LinesToTrigger = Y - MonitorLine;
	}
	else
	{
		LinesToTrigger = (312 - MonitorLine) + Y;
	}
	
	Nops = (LinesToTrigger<<6);

	MonitorX = CRTC_InternalState.Monitor_State.MonitorHorizontalCount;

	X = (X>>(4-1)) + X_CRTC_CHAR_OFFSET;

	if (X>MonitorX)
	{
		CharsToTrigger = X - MonitorX;
	}
	else
	{
		CharsToTrigger = (64 - MonitorX) + X;
	}

	Nops += (CharsToTrigger>>1);
	
		
/*		unsigned long Nops;

		Nops = (YPos<<6) + (XPos>>5) + ((8*10)*64);
*/
		CRTC_LightPen_Trigger(Nops);
	}
}

void	Magnum_DoOut(Z80_WORD Port, Z80_BYTE Data)
{
	/* when a write is done to this port, it signifies that the
	magnum should trigger the LPEN input to signify the fire button is pressed */
	if (Port==0x0fbfe)
	{
		if (((Data^Magnum_PreviousByteWritten) & 0x080)!=0)
		{
			/* bit changed state */
			if ((Data & 0x080)!=0)
			{
				/* if the data is the same when read, LPEN has not been triggered,
				 and therefore the fire button is pressed.
				*/
				if (!(Magnum_FirePressed))
				{
					/* not pressed, trigger LPEN. */

					/* number of cycles before lpen is triggered... */
					CRTC_LightPen_Trigger(1);
				}
			}

		}
			
		Magnum_PreviousByteWritten = Data;
	}
}



static unsigned long SpanishLightGun_CyclesToTrigger = 0;
static BOOL		SpanishLightGun_DoTrigger = FALSE;
static unsigned long SpanishLightGun_PreviousNopCount = 0;
extern unsigned char CRTCRegisters[32];
extern CRTC_INTERNAL_STATE CRTC_InternalState;

/**** SPANISH LIGHT GUN ****/
void	SpanishLightGun_Update(int X, int Y, int Button)
{
	unsigned long Nops;
	unsigned long MonitorLine;
	unsigned long LinesToTrigger;
	unsigned long MonitorX;
	unsigned long CharsToTrigger;

	/* get current monitor line we are on. */
	MonitorLine = CRTC_InternalState.Monitor_State.MonitorScanLineCount;

	/* work out number of complete lines until the trigger should occur */
	if (Y>MonitorLine)
	{
		LinesToTrigger = Y - MonitorLine;
	}
	else
	{
		LinesToTrigger = (312 - MonitorLine) + Y;
	}
	
	Nops = (LinesToTrigger<<6);

	MonitorX = CRTC_InternalState.Monitor_State.MonitorHorizontalCount;

	X = (X>>3) + X_CRTC_CHAR_OFFSET;

	if (X>MonitorX)
	{
		CharsToTrigger = X - MonitorX;
	}
	else
	{
		CharsToTrigger = (64 - MonitorX) + X;
	}

	Nops += (CharsToTrigger>>1);
	
	
	if (Button)
	{
/*		unsigned long XPos;
		unsigned long Nops;

		 1 NOP = 2 bytes
		 2 bytes = 16 mode 2 pixels.

		 get Nops accross screen + adjust for rendering accuracy
		XPos = X>>(4+1);
		XPos = XPos + 

		Nops = (Y<<6) + (X>>5) + ((8*10)*64);

		 hsync position + chars so that 
		(CRTCRegisters[2] + 6)
*/
		SpanishLightGun_CyclesToTrigger = Nops;
		SpanishLightGun_PreviousNopCount = CPC_GetNopCount();
		SpanishLightGun_DoTrigger = TRUE;
	}		

}

unsigned char SpanishLightGun_ReadJoystickPort(void)
{
	unsigned char Data = 0x0ff;

	if (SpanishLightGun_DoTrigger)
	{
	    int CurrentNopCount = CPC_GetNopCount();
	    int NopsPassed;

	    NopsPassed = CurrentNopCount - SpanishLightGun_PreviousNopCount;
		
		if (NopsPassed>=SpanishLightGun_CyclesToTrigger)
		{
			SpanishLightGun_DoTrigger = 0;

			Data &= ~2;
		}

		SpanishLightGun_CyclesToTrigger -= NopsPassed;


		SpanishLightGun_PreviousNopCount = CurrentNopCount;
	
		Data &= ~(16|32);
	}

	return Data;
}


void	SpanishLightGun_Reset(void)
{
	SpanishLightGun_PreviousNopCount = 0;
}
