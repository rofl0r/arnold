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
#ifndef __CPC_HEADER_INCLUDED__
#define __CPC_HEADER_INCLUDED__

#include "cpcdefs.h"

#include "z80/z80.h"

#include "cpcglob.h"	

#include "debugger/gdebug.h"

#include "printer.h"
#include "asic.h"
#include "crtc.h"
#include "multface.h"
#include "tzx.h"
#include "fdc.h"
#include "psg.h"
#include "dumpym.h"
#include "scrsnap.h"
#include "snapshot.h"
#include "wav.h"
#include "render.h"
#include "garray.h"
#include "arnold.h"
#include "westpha.h"

#define NUM_CRTC_TYPES 5

#define PSG_CLOCK_FREQUENCY 1000000
#define Z80_CLOCK_FREQUENCY	4000000

/* number of nops in whole display */
/* use this to signal when we render the screen */

#define		NOPS_PER_LINE	64			/* time for a single scan line */
#define		BYTES_PER_NOP	2			/* number of bytes in a time unit */
#define		LINES_PER_SCREEN	39*8	/* number of scan-lines on a monitor screen */
#define		NOPS_PER_FRAME	(NOPS_PER_LINE*LINES_PER_SCREEN)


#define NOPS_PER_MONITOR_SCREEN (NOPS_PER_LINE*LINES_PER_SCREEN)


typedef struct AMSTRAD_ROMS
{
	unsigned char	*pBasic;
	unsigned char	*pOs;
} AMSTRAD_ROMS;



typedef enum
{
	/* line 0, bit 0..bit 7 */
	CPC_KEY_CURSOR_UP = 0,
	CPC_KEY_CURSOR_RIGHT,
	CPC_KEY_CURSOR_DOWN,
	CPC_KEY_F9,
	CPC_KEY_F6,
	CPC_KEY_F3,
	CPC_KEY_SMALL_ENTER,
	CPC_KEY_FDOT,
	/* line 1, bit 0..bit 7 */
	CPC_KEY_CURSOR_LEFT,
	CPC_KEY_COPY,
	CPC_KEY_F7,
	CPC_KEY_F8,
	CPC_KEY_F5,
	CPC_KEY_F1,
	CPC_KEY_F2,
	CPC_KEY_F0,
	/* line 2, bit 0..bit 7 */
	CPC_KEY_CLR,
	CPC_KEY_OPEN_SQUARE_BRACKET,
	CPC_KEY_RETURN,
	CPC_KEY_CLOSE_SQUARE_BRACKET,
	CPC_KEY_F4,
	CPC_KEY_SHIFT,
	CPC_KEY_FORWARD_SLASH,
	CPC_KEY_CONTROL,
	/* line 3, bit 0.. bit 7 */
	CPC_KEY_HAT,
	CPC_KEY_MINUS,
	CPC_KEY_AT,
	CPC_KEY_P,
	CPC_KEY_SEMICOLON,
	CPC_KEY_COLON,
	CPC_KEY_BACKSLASH,
	CPC_KEY_DOT,
	/* line 4, bit 0..bit 7 */
	CPC_KEY_ZERO,
	CPC_KEY_9,
	CPC_KEY_O,
	CPC_KEY_I,
	CPC_KEY_L,
	CPC_KEY_K,
	CPC_KEY_M,
	CPC_KEY_COMMA,
	/* line 5, bit 0..bit 7 */
	CPC_KEY_8,
	CPC_KEY_7,
	CPC_KEY_U,
	CPC_KEY_Y,
	CPC_KEY_H,
	CPC_KEY_J,
	CPC_KEY_N,
	CPC_KEY_SPACE,
	/* line 6, bit 0..bit 7 */
	CPC_KEY_6,
	CPC_KEY_5,
	CPC_KEY_R,
	CPC_KEY_T,
	CPC_KEY_G,
	CPC_KEY_F,
	CPC_KEY_B,
	CPC_KEY_V,
	/* line 7, bit 0.. bit 7 */
	CPC_KEY_4,
	CPC_KEY_3,
	CPC_KEY_E,
	CPC_KEY_W,
	CPC_KEY_S,
	CPC_KEY_D,
	CPC_KEY_C,
	CPC_KEY_X,
	/* line 8, bit 0.. bit 7 */
	CPC_KEY_1,
	CPC_KEY_2,
	CPC_KEY_ESC,
	CPC_KEY_Q,
	CPC_KEY_TAB,
	CPC_KEY_A,
	CPC_KEY_CAPS_LOCK,
	CPC_KEY_Z,
	/* line 9, bit 7..bit 0 */
	CPC_KEY_JOY_UP,
	CPC_KEY_JOY_DOWN,
	CPC_KEY_JOY_LEFT,
	CPC_KEY_JOY_RIGHT,
	CPC_KEY_JOY_FIRE1,
	CPC_KEY_JOY_FIRE2,
	CPC_KEY_SPARE,
	CPC_KEY_DEL, 


	/* no key press */
	CPC_KEY_NULL
} CPC_KEY_ID;
	
typedef enum
{
	JOYSTICK_HARDWARE_AMX_MOUSE = 0,
	JOYSTICK_HARDWARE_JOYSTICK,
	JOYSTICK_HARDWARE_WESTPHASER,
	JOYSTICK_HARDWARE_SPANISH_LIGHTGUN
} JOYSTICK_HARDWARE_ID;

typedef struct PATCH_ENTRY
{
	unsigned short	Addr;
	unsigned short	Byte;
} PATCH_ENTRY;

typedef void (*WRITE_FUNCTION)(int);
typedef unsigned int	(*READ_FUNCTION)(void);
typedef void (*FUNCTION)(void);

BOOL	CPC_Initialise();
void	CPC_Finish();
void	CPC_Reset();

#define CPC_HW_CPCPLUS		0x0001
#define CPC_HW_CPC			0x0002
#define CPC_HW_KCCOMPACT	0x0003

void	CPC_SetHardware(int);
int		CPC_GetHardware(void);

/* PPI */
void	PPI_WritePortA(int);
void	PPI_WritePortB(int);
void	PPI_WritePortC(int);
void	PPI_WriteControl(int);

unsigned int		PPI_ReadPortA(void);
unsigned int		PPI_ReadPortB(void);
unsigned int		PPI_ReadPortC(void);

/* PPI functions for snapshot */
int PPI_GetControlForSnapshot(void);
void	PPI_SetPortADataFromSnapshot(int Data);
int PPI_GetPortADataForSnapshot(void);
void    PPI_SetPortBDataFromSnapshot(int Data);
int PPI_GetPortBDataForSnapshot(void);
void	PPI_SetPortCDataFromSnapshot(int Data);
int PPI_GetPortCDataForSnapshot(void);

/* writing functions */
void	ROM_Select(int);

void	CPC_DumpRamToBuffer(char *Buffer);
void	CPC_DumpBaseRamToBuffer(char *pBuffer);

BOOL	AllocateEmulatorMemory(void);
void	InitialiseMemoryPaging(unsigned long);	
void	FreeEmulatorMemory(void);

/* select crtc emulation */
void	CPC_SetCRTCType(unsigned int);
int CPC_GetCRTCType(void);

/* define CPC types */
typedef enum
{
	CPC_TYPE_CPC464 = 0,
	CPC_TYPE_CPC664,
	CPC_TYPE_CPC6128,
	CPC_TYPE_464PLUS,
	CPC_TYPE_6128PLUS,
	CPC_TYPE_KCCOMPACT
} CPC_TYPE_ID;

typedef enum
{
	CPC_MONITOR_COLOUR = 0,
	CPC_MONITOR_GREEN_SCREEN,
	CPC_MONITOR_GREY_SCALE
} CPC_MONITOR_TYPE_ID;

#if 0
typedef enum
{
	IO_GATE_ARRAY_WRITE = 0,
	IO_EXPANSION_ROM_SELECT_WRITE,
	IO_CRTC_WRITE_DATA,
	IO_CRTC_WRITE_REG
} CPC_IO_ID;
#endif

typedef enum
{
	CASSETTE_TYPE_SAMPLE = 0,
	CASSETTE_TYPE_TAPE_IMAGE,
	CASSETTE_TYPE_NONE
} CPC_CASSETTE_TYPE_ID;

/* select CPC emulation */
void	CPC_SetCPCType(CPC_TYPE_ID);
int		CPC_GetCPCType(void);

/* colour, green screen or grey scale monitor type */
void	CPC_SetMonitorType(CPC_MONITOR_TYPE_ID);
int		CPC_GetMonitorType(void);

#define MONITOR_BRIGHTNESS_MAX	256
#define MONITOR_BRIGHTNESS_MIN	10

/* monitor brightness 0-256. 256 is max brightness */
void	CPC_SetMonitorBrightness(int);
int		CPC_GetMonitorBrightness(void);

char	*ExpansionRom_GetRomName(int RomIndex);
BOOL	ExpansionRom_Insert(char *RomFilename, int RomIndex);
Z80_WORD Z80_RD_BASE_WORD(Z80_WORD Addr);
unsigned char *ExpansionRom_Get(int RomIndex);
BOOL	ExpansionRom_IsActive(int RomIndex);
void	ExpansionRom_SetActiveState(int RomIndex, BOOL State);

void	ExpansionROM_RefreshTable(void);

void	CPC_ResetTiming(void);
void	CPC_ResetNopCount(void);
unsigned long	CPC_GetNopCount(void);
void	CPC_UpdateNopCount(unsigned long);

/*void	CPC_AcknowledgeInterrupt(void); */
void	CPC_EnableASICRamWrites(BOOL);

void	CPC_ReleaseKeys();


void	DebugHooks_WriteMemory_Active(BOOL State);
void	DebugHooks_ReadMemory_Active(BOOL State);
void	DebugHooks_WriteIO_Active(BOOL State);
void	DebugHooks_ReadIO_Active(BOOL State);

BOOL	DebugHooks_ReadMemory_GetActiveState(void);
BOOL	DebugHooks_WriteMemory_GetActiveState(void);
BOOL	DebugHooks_WriteIO_GetActiveState(void);
BOOL	DebugHooks_ReadIO_GetActiveState(void);


/*char *CPC_Debug_GetTextForIOID(CPC_IO_ID IOID);
void	CPC_Debug_SetupIOCompare(DEBUG_CMP_STRUCT *pComparison, CPC_IO_ID IOPort); */


void	AudioDAC_SetVolume(unsigned char, unsigned char);

void	CPC_ReloadSystemCartridge();

void	CPC_SetComputerNameIndex(int);
int		CPC_GetComputerNameIndex(void);
unsigned char Keyboard_Read(void);
void	CPC_ClearKeyboard(void);
void CPC_SetKey(int KeyID);
void CPC_ClearKey(int KeyID);
void	CPC_SetHardwareConnectedToJoystickPort(JOYSTICK_HARDWARE_ID HardwareID);


/*
typedef struct
{
	union 
	{
		signed long	L;

		struct
		{
			unsigned short Fraction;
			signed short Int;
		} W;
	};
} FIXED_POINT16;
*/
BOOL	CPC_SetMemorySize(int MemorySizeInK);
void	CPC_UpdateColours(void);


/* dk tronics 256k ram expansion */
#define CPC_RAM_CONFIG_256K_RAM				0x0001
/* dk tronics 256k silicon disk expansion */
#define CPC_RAM_CONFIG_256K_SILICON_DISK	0x0002
/* dk'tronics 64k expansion or 2nd bank of 64k in CPC6128 */
#define CPC_RAM_CONFIG_64K_RAM				0x0004

#define CPC_RAM_BLOCK_0_MASK				(1<<0)
#define CPC_RAM_BLOCK_1_MASK				(1<<1)
#define CPC_RAM_BLOCK_2_MASK				(1<<2)
#define CPC_RAM_BLOCK_3_MASK				(1<<3)
#define CPC_RAM_BLOCK_4_MASK				(1<<4)
#define CPC_RAM_BLOCK_5_MASK				(1<<5)
#define CPC_RAM_BLOCK_6_MASK				(1<<6)
#define CPC_RAM_BLOCK_7_MASK				(1<<7)
#define CPC_RAM_BLOCK_8_MASK				(1<<8)
#define CPC_RAM_BLOCK_9_MASK				(1<<9)
#define CPC_RAM_BLOCK_10_MASK				(1<<10)
#define CPC_RAM_BLOCK_11_MASK				(1<<11)
#define CPC_RAM_BLOCK_12_MASK				(1<<12)
#define CPC_RAM_BLOCK_13_MASK				(1<<13)
#define CPC_RAM_BLOCK_14_MASK				(1<<14)
#define CPC_RAM_BLOCK_15_MASK				(1<<15)
#define CPC_RAM_BLOCK_16_MASK				(1<<16)
#define CPC_RAM_BLOCK_17_MASK				(1<<17)
#define CPC_RAM_BLOCK_18_MASK				(1<<18)
#define CPC_RAM_BLOCK_19_MASK				(1<<19)
#define CPC_RAM_BLOCK_20_MASK				(1<<20)
#define CPC_RAM_BLOCK_21_MASK				(1<<21)
#define CPC_RAM_BLOCK_22_MASK				(1<<22)
#define CPC_RAM_BLOCK_23_MASK				(1<<23)
#define CPC_RAM_BLOCK_24_MASK				(1<<24)
#define CPC_RAM_BLOCK_25_MASK				(1<<25)
#define CPC_RAM_BLOCK_26_MASK				(1<<26)
#define CPC_RAM_BLOCK_27_MASK				(1<<27)
#define CPC_RAM_BLOCK_28_MASK				(1<<28)
#define CPC_RAM_BLOCK_29_MASK				(1<<29)
#define CPC_RAM_BLOCK_30_MASK				(1<<30)
#define CPC_RAM_BLOCK_31_MASK				(1<<31)

void	CPC_SetRamConfig(unsigned long);
unsigned long CPC_GetRamConfig(void);

void	CPC_SetCassetteType(int);
int             ROM_GetSelectedROM(void);


void	Vortex_Initialise(void);
void	Vortex_Finish(void);
void	Vortex_RethinkMemory(void);

/* Inicron RAM-ROM */

#define RAM_ROM_FLAGS_RAM_ON	0x0001
#define RAM_ROM_FLAGS_RAM_WRITE_ENABLE	0x0002
#define RAM_ROM_FLAGS_EPROM_ON	0x0004

void	RAM_ROM_Initialise(int NumBlocks);
void	RAM_ROM_Finish(void);
void	RAM_ROM_SetBankEnable(int Bank, BOOL State);
BOOL	RAM_ROM_GetBankEnableState(int Bank);
void	RAM_ROM_RethinkMemory(unsigned char **, unsigned char **);
BOOL	RAM_ROM_IsRamOn(void);
BOOL	RAM_ROM_IsRamWriteEnabled(void); 
void	RAM_ROM_SetRamOnState(BOOL);
void	RAM_ROM_SetRamWriteEnableState(BOOL);
void	RAM_ROM_SetEPROMOnState(BOOL);
BOOL	RAM_ROM_IsEPROMOn(void);
void	RAM_ROM_SetupTable(void);

void	Magnum_DoOut(Z80_WORD Port, Z80_BYTE Data);

Z80_BYTE Z80_RD_BASE_BYTE(Z80_WORD Addr);

void	KCC_Update(void);

unsigned long CPC_GetCassetteType(void);
#endif
