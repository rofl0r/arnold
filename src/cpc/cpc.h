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

/* status codes returned from functions */
enum
{
	/* file was opened ok */
	ARNOLD_STATUS_OK,
	/* file was not recognised */
	ARNOLD_STATUS_UNRECOGNISED,
	/* version of this file not supported */
	ARNOLD_VERSION_UNSUPPORTED,
	/* a general error */
	ARNOLD_STATUS_ERROR,
	/* the file or data was invalid */
	ARNOLD_STATUS_INVALID,
	/* the file or data had a bad length */
	ARNOLD_STATUS_INVALID_LENGTH,
	/* memory allocation request failed */
	ARNOLD_STATUS_OUT_OF_MEMORY,
};


#include "z80/z80.h"

#include "cpcglob.h"

#include "debugger/gdebug.h"

#include "printer.h"
#include "asic.h"
#include "crtc.h"
#include "fdc.h"
#include "fdd.h"
#include "fdi.h"
#include "psg.h"
#include "snapshot.h"
#include "render.h"
#include "garray.h"
#include "arnold.h"
#include "westpha.h"
#include "cassette.h"
#include "vortex.h"
#include "ramrom.h"
#include "magnum.h"

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

typedef struct
{
	/* The port I/O address is logically ANDed with this value
	to remove bits we are not interested in */
	Z80_WORD PortAnd;
	/* Then the port I/O address is compared against this value
	which has the state of the bits we are interested in. The
	bits we are not interested in should be set to 0 */
	Z80_WORD PortCmp;
	/* if the comparison matches, then this function is called
	to write the data to the port */
	void (*pWriteFunction)(Z80_WORD, Z80_BYTE);
} CPCPortWrite;

typedef struct
{
	/* The port I/O address is logically ANDed with this value
	to remove bits we are not interested in */
	Z80_WORD PortAnd;
	/* Then the port I/O address is compared against this value
	which has the state of the bits we are interested in. The
	bits we are not interested in should be set to 0 */
	Z80_WORD PortCmp;
	/* if the comparison matches, then this function is called
	to write the data to the port */
	Z80_BYTE (*pReadFunction)(Z80_WORD);
} CPCPortRead;

typedef void (*CPC_RESET_FUNCTION)(void);

/* install a reset function for a expansion peripheral */
void CPC_InstallResetFunction(CPC_RESET_FUNCTION resetFunction);

/* install a port read function */
/* currently used for expansion peripherals */
void CPC_InstallReadPort(CPCPortRead *readPort);
/* install a port write function */
/* currently used for expansion peripherals */
void CPC_InstallWritePort(CPCPortWrite *writePort);

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

/* digital joystick 0 */
#define CPC_JOY0_UP	CPC_KEY_JOY_UP
#define CPC_JOY0_DOWN CPC_KEY_JOY_DOWN
#define CPC_JOY0_LEFT	CPC_KEY_JOY_LEFT
#define CPC_JOY0_RIGHT CPC_KEY_JOY_RIGHT
#define CPC_JOY0_FIRE1	CPC_KEY_JOY_FIRE1
#define CPC_JOY0_FIRE2 CPC_KEY_JOY_FIRE2
#define CPC_JOY0_SPARE CPC_KEY_SPARE

/* digital joystick 1 */
#define CPC_JOY1_UP	CPC_KEY_6
#define CPC_JOY1_DOWN CPC_KEY_5
#define CPC_JOY1_LEFT	CPC_KEY_R
#define CPC_JOY1_RIGHT CPC_KEY_T
#define CPC_JOY1_FIRE1	CPC_KEY_G
#define CPC_JOY1_FIRE2 CPC_KEY_F
#define CPC_JOY1_SPARE CPC_KEY_B

enum
{
	/* CPC/CPC+ digital joystick 0 */
	CPC_DIGITAL_JOYSTICK0,
	/* CPC/CPC+ digital joystick 1 (joystick splitter required for CPC) */
	CPC_DIGITAL_JOYSTICK1,
	/* CPC+ analogue joystick */
	CPC_ANALOGUE_JOYSTICK
};

typedef enum
{
	JOYSTICK_HARDWARE_AMX_MOUSE = 0,
	JOYSTICK_HARDWARE_JOYSTICK,
	JOYSTICK_HARDWARE_WESTPHASER,
	JOYSTICK_HARDWARE_SPANISH_LIGHTGUN
} JOYSTICK_HARDWARE_ID;


BOOL	CPC_Initialise();
void	CPC_Finish();
void	CPC_Reset();

enum
{
	/* CPC+ hardware design */
	/* ASIC combining 8255, CRTC and Gate Array */
	CPC_HW_CPCPLUS,
	/* CPC hardware design */
	/* seperate 8255, CRTC and Gate Array */
	CPC_HW_CPC,
	/* standard CPC464 hardware */
	CPC_HW_CPC464,
	/* standard CPC664 hardware */
	CPC_HW_CPC664,
	/* standard CPC6128 hardware */
	CPC_HW_CPC6128,
	/* cost down CPC464 */
	/* seperate 8255, combined CRTC and Gate Array */
	CPC_HW_CPC464_COST_DOWN,
	/* cost down CPC6128 */
	/* seperate 8255, combined CRTC, PAL and Gate Array */
	CPC_HW_CPC6128_COST_DOWN,
	/* KC Compact hardware design */
	/* Z8536 CIO */
	CPC_HW_KCCOMPACT
};

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

void CPC_SetOSRom(const unsigned char *pOSRom);

void CPC_SetBASICRom(const unsigned char *pBASICROM);

void CPC_SetDOSRom(const unsigned char *pDOSRom);


/* rom */
BOOL ExpansionRom_GetRomName(const int RomIndex, char **);
int    ExpansionRom_SetRomData(const unsigned char *RomData, const unsigned long RomDataSize, const int RomIndex);
unsigned char *ExpansionRom_Get(int RomIndex);
BOOL	ExpansionRom_IsActive(int RomIndex);
void	ExpansionRom_SetActiveState(int RomIndex, BOOL State);
void	ExpansionROM_RefreshTable(void);
void	ExpansionROM_SetTableEntry(int, unsigned char *);

Z80_WORD Z80_RD_BASE_WORD(Z80_WORD Addr);


void	CPC_ResetTiming(void);
void	CPC_ResetNopCount(void);
unsigned long	CPC_GetNopCount(void);
void	CPC_UpdateNopCount(unsigned long);



/*void	CPC_AcknowledgeInterrupt(void); */
void	CPC_EnableASICRamWrites(BOOL);

void	CPC_ReleaseKeys();

#if 0
void	DebugHooks_WriteMemory_Active(BOOL State);
void	DebugHooks_ReadMemory_Active(BOOL State);
void	DebugHooks_WriteIO_Active(BOOL State);
void	DebugHooks_ReadIO_Active(BOOL State);

BOOL	DebugHooks_ReadMemory_GetActiveState(void);
BOOL	DebugHooks_WriteMemory_GetActiveState(void);
BOOL	DebugHooks_WriteIO_GetActiveState(void);
BOOL	DebugHooks_ReadIO_GetActiveState(void);
#endif

/*char *CPC_Debug_GetTextForIOID(CPC_IO_ID IOID);
void	CPC_Debug_SetupIOCompare(DEBUG_CMP_STRUCT *pComparison, CPC_IO_ID IOPort); */


void	AudioDAC_SetVolume(unsigned char, unsigned char);


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

int             ROM_GetSelectedROM(void);



Z80_BYTE Z80_RD_BASE_BYTE(Z80_WORD Addr);

void	KCC_Update(void);


/* internal disc interface of CPC664, CPC6128 and CPC6128+ */
/* or external DDI-1 disc interface */
void Amstrad_DiscInterface_Install(void);
void Amstrad_DiscInterface_DeInstall(void);

/* internal ram of CPC6128, CPC6128+ or Dk'Tronics or Dobbertin
compatible ram expansion */
void	Amstrad_RamExpansion_Install(void);
void	Amstrad_RamExpansion_DeInstall(void);

BOOL Keyboard_HasBeenScanned();
void Keyboard_ResetHasBeenScanned();

#endif
