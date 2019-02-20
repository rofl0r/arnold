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
#ifndef __FDC_HEADER_INCLUDED__
#define __FDC_HEADER_INCLUDED__

#include <stdio.h>
#include "cpcglob.h"
#include "device.h"

/* FDD functions */
void	FDD_InitialiseAll();
void	FDD_Initialise(int);
void	FDD_MotorControl(int);
void	FDD_TurnDisk(int);
void	FDD_InsertDisk(int,int);
BOOL	FDD_IsDiskPresent(int);

int		FDD_GetLEDState(void);
void	FDD_UpdateLEDState(void);

BOOL FDD_GetMotorState(void);


/* FDC functions */
void	FDC_Reset(void);
void	FDC_WriteDataRegister(int);
unsigned int		FDC_ReadDataRegister(void);
unsigned int		FDC_ReadMainStatusRegister(void);


#define NEC765_FLAGS_INTERRUPT 0x01
#define NEC765_FLAGS_SEEK_OPERATION 0x02
#define NEC765_FLAGS_SEEK_OPERATION_IS_RECALIBRATE 0x04
#define NEC765_FLAGS_DATA_TRANSFER 0x08
#define NEC765_FLAGS_COMMAND_ACTIVE 0x010

enum
{
	NEC765_LOW_LEVEL_STATE_DELAY = 0,
	
	NEC765_HIGH_LEVEL_STATE_EXECUTION_PHASE_READ_DATA,
	NEC765_HIGH_LEVEL_STATE_EXECUTION_PHASE_WRITE_DATA,
	NEC765_HIGH_LEVEL_STATE_EXECUTION_PHASE,
	NEC765_HIGH_LEVEL_STATE_COMMAND_PHASE_FIRST_BYTE,
	NEC765_HIGH_LEVEL_STATE_COMMAND_PHASE_REMAINING_BYTES,
	NEC765_HIGH_LEVEL_STATE_RESULT_PHASE
};



typedef struct NEC765
{
	unsigned long Flags;

	unsigned long ST0;
	unsigned long ST1;
	unsigned long ST2;
	unsigned long ST3;

	unsigned long CurrentDrive;
	unsigned long CurrentSide;

	/* drive and side we are actually accessing */
	struct FDD *drive;
	unsigned long PhysicalDrive;
	unsigned long PhysicalSide;
	unsigned long PhysicalTrack;

/*	unsigned long MainStatusRegister; */

	/* step direction */
	unsigned long StepDirection;

	unsigned long StepCount[4];

	unsigned long StepTime;
	unsigned long StepTimeForDrive[4];

	unsigned long NCN;

	/* present cylinder numbers of all 4 drives */
	unsigned long PCN[4];

	/* fdc state */
	unsigned long LowLevelState;

	unsigned long HighLevelState;
	unsigned long PushedHighLevelState;

	void	(*CommandHandler)(int);

	/* command state */
	unsigned long CommandState;

	/* state to go to when complete */
	unsigned long NextCommandState;

	/* number of cycles before a data request is issued */
	unsigned long CyclesToDataRequest;
	/* nop count when we setup the data request delay */
	unsigned long NopCountOfDataRequestStart;

	unsigned long ExecutionBufferByteIndex;
	unsigned long ExecutionNoOfBytes;
	char *ExecutionBuffer;

	unsigned long MainStatusRegister;
	unsigned long DataRegister;

	unsigned long CommandBytes[8];
	unsigned long CommandByteIndex;
	unsigned long CommandBytesRemaining;

	unsigned long StoredDelay;

	unsigned long SectorCounter;

	unsigned char *pTrackPtr;
	unsigned char *pTrackStart;
	unsigned char *pTrackEnd;

	unsigned short CRC;
} NEC765;


typedef void (*FDC_COMMAND_FUNCTION)(int);

typedef struct DRIVE_STATUS
{
	int		CurrentTrack;			/* Current Track head is over */
	int		CurrentIDIndex;			/* current id index */
	int		CurrentSide;
	BOOL	WriteProtected;			/* true if write protected */
	BOOL	DiskPresent;			/* true if disc inserted */
	int		PhysicalSide;
	BOOL	DriveActive;
} DRIVE_STATUS;

typedef struct	FDC_COMMAND
{
	int			NoOfCommandBytes;
	FDC_COMMAND_FUNCTION	CommandHandler;
} FDC_COMMAND;

typedef struct CHRN
{
	int		C;
	int		H;
	int		R;
	int		N;
	int		ST1;
	int		ST2;
} CHRN;

unsigned char FDC_GetMainStatusRegister(void);
unsigned char FDC_GetDataRegister(void);

void	FDC_SetMainStatusRegister(unsigned char);
void	FDC_SetDataRegister(unsigned char);

typedef enum
{
	FDC_WRITE_FIRST_COMMAND_BYTE = 0,
	FDC_WRITE_COMMAND_BYTES,
	FDC_WRITE_EXECUTION_PHASE,
	FDC_READ_EXECUTION_PHASE,
	FDC_READ_RESULT_PHASE
} FDC_STATE;

#endif
