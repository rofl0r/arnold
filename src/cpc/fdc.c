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
/* I HAVE STARTED TO CONVERT THIS INTO A MORE ACCURATE FDC EMULATION. EACH TRACK
FROM THE DISK IMAGE WOULD BE CONVERTED INTO MFM DATA, THIS MEANS THE WHOLE
EMULATION COULD BE MADE MORE ACCURATE IN TERMS OF TIMING AND WHAT IT DOES.
AS A RESULT DISCOLOGY WOULD THEN WORK PERFECTLY */
 /*
	FDC CODE

	1. FDC BUSY is set after first command byte.
	2. FDC EXECUTION PHASE is only set if a command has a execution phase.
	
  
	3. Side is not set in Recalibrate or Seek.
	4. Drive 0 on CPC+ drive doesn't report two side, however, drive
	B used on CPC+ drive does.

  When Specify done, it appears to report for each drive, the
  ready status if a sense interrupt status is done. On CPC
  it reports ready state change.


  tested in Sep 98:

  Drive is 3.5", Drive B:

  Disc not in drive, but drive connected:

  0x069 for recalibrate.
  0x049 for sense drive status

  Disc in drive, drive connected:

  0x021 for recalibrate,
  0x031 for sense drive status

  - Senwse interrupt status returns invalid after read data.





*/

/*#define OVERRUN */

#include "fdc.h"
#include "cpcglob.h"
#include "diskimage/diskimg.h"
#include "diskimage/maketrk.h"
#include "debugmain.h"
#include "cpcdefs.h"
#include "host.h"
#include "cpc.h"

#ifdef FDC_DEBUG
extern	DEBUG_HANDLE	FDC_Debug;
extern  char			DebugString[256];
#endif

static int FDD_MotorState;
/*static BOOL FDC_InterruptActive = FALSE; */

/* write functions/states */
static void	FDC_WriteFirstCommandByte(int);
static void FDC_WriteCommandBytes(int);
static void FDC_WriteExecutionPhase(int);

/* read functions/states */
/*static int FDC_ReadDataNull(void); */
static int FDC_ReadResultPhase(void);
static int FDC_ReadExecutionPhase(void);

static void FDC_Standby(void);
static void FDC_SetStatus0(void);
static void FDC_ClearStatusRegisters(void);
static void FDC_StartResultPhase(int);
static int	FDC_GetSectorSize(int);

static void	FDC_SetDriveStatus(int);

static void	FDC_Invalid(int);
static void	FDC_ReadATrack(int);
static void	FDC_Specify(int);
static void	FDC_SenseDriveStatus(int);
static void	FDC_SenseInterruptStatus(int);
static void	FDC_WriteData(int);
static void	FDC_ReadData(int);
static void	FDC_Recalibrate(int);
static void	FDC_WriteDeletedData(int);
static void	FDC_ReadID(int);
static void	FDC_ReadDeletedData(int);
static void	FDC_FormatATrack(int);
static void	FDC_Seek(int);
static void	FDC_ScanEqual(int);
static void	FDC_ScanLowOrEqual(int);
static void	FDC_ScanHighOrEqual(int);
void	FDC_UpdateState(void);


#include "fdd.h"

/* the two drives */
static FDD drive[2];

/* floppy disc controller */
static NEC765 fdc;

/* FDC defines */

#define FDC_COMMAND_MULTI_TRACK						0x080
#define FDC_COMMAND_MFM								0x040
#define FDC_COMMAND_SKIP							0x020
#define FDC_COMMAND_WORD							0x01f
#define FDC_COMMAND_HEAD_ADDRESS					0x004

#define FDC_COMMAND_UNIT_STANDARD					0x003
#define FDC_COMMAND_UNIT_CPC						0x001

/* Main Status Register defines */
#define FDC_MSR_DATA_REQUEST							0x080
#define FDC_MSR_DATA_FLOW_DIRECTION					0x040
#define FDC_MSR_EXECUTION_PHASE						0x020
#define FDC_MSR_BUSY								0x010
#define FDC_MSR_FDD_3_BUSY							0x008
#define FDC_MSR_FDD_2_BUSY							0x004
#define FDC_MSR_FDD_1_BUSY							0x002
#define FDC_MSR_FDD_0_BUSY							0x001


/* Status Register 0 (ST0) defines */
#define FDC_ST0_INTERRUPT_CODE1						0x080
#define FDC_ST0_INTERRUPT_CODE0						0x040
#define FDC_ST0_SEEK_END							0x020
#define FDC_ST0_EQUIPMENT_CHECK						0x010
#define FDC_ST0_NOT_READY							0x008
#define FDC_ST0_HEAD_ADDRESS						0x004
#define FDC_ST0_UNIT_SELECT1						0x002
#define FDC_ST0_UNIT_SELECT0						0x001

/* Status Register 1 (ST1) defines */
#define FDC_ST1_END_OF_CYLINDER						0x080
#define FDC_ST1_UNUSED_BIT6							0x040
#define FDC_ST1_DATA_ERROR							0x020
#define FDC_ST1_OVERRUN								0x010
#define FDC_ST1_UNUSED_BIT3							0x008
#define FDC_ST1_NO_DATA								0x004
#define FDC_ST1_NOT_WRITEABLE						0x002
#define FDC_ST1_MISSING_ADDRESS_MARK				0x001

/* Status Register 2 (ST2) defines */
#define FDC_ST2_UNUSED_BIT7							0x080
#define FDC_ST2_CONTROL_MARK						0x040
#define FDC_ST2_DATA_ERROR_IN_DATA_FIELD			0x020
#define FDC_ST2_WRONG_CYLINDER						0x010
#define FDC_ST2_SCAN_EQUAL_HIT						0x008
#define FDC_ST2_SCAN_NOT_SATISFIED					0x004
#define FDC_ST2_BAD_CYLINDER						0x002
#define FDC_ST2_MISSING_ADDRESS_MARK_IN_DATA_FIELD	0x001

/* Status Register 3 (ST3) defines */
#define FDC_ST3_FAULT								0x080
#define FDC_ST3_WRITE_PROTECTED						0x040
#define FDC_ST3_READY								0x020
#define FDC_ST3_TRACK_0								0x010
#define FDC_ST3_TWO_SIDE							0x008
#define FDC_ST3_HEAD_ADDRESS						0x004
#define FDC_ST3_UNIT_SELECT1						0x002
#define FDC_ST3_UNIT_SELECT0						0x001

/* normal termination, command completed successfully */
#define FDC_INTERRUPT_CODE_NT						(0x000)
/* abnormal termination, command was executed, but didn't complete
successfully */
#define FDC_INTERRUPT_CODE_AT						(FDC_ST0_INTERRUPT_CODE0)
/* invalid command issued */
#define FDC_INTERRUPT_CODE_IC						(FDC_ST0_INTERRUPT_CODE1)
/* abnormal termination, ready state of drive changed */
#define FDC_INTERRUPT_CODE_AT_READY_CHANGED			(FDC_ST0_INTERRUPT_CODE0 | FDC_ST0_INTERRUPT_CODE1)


#define FDC_ST0_SET_DRIVE_AND_SIDE		\
	fdc.ST0 &= ~(FDC_ST0_HEAD_ADDRESS | FDC_ST0_UNIT_SELECT0 | FDC_ST0_UNIT_SELECT1);	\
	fdc.ST0 |= (fdc.CurrentSide<<2) | fdc.CurrentDrive

#define FDC_ST3_SET_DRIVE_AND_SIDE		\
	FDC_ST3 &= ~(FDC_ST0_HEAD_ADDRESS | FDC_ST0_UNIT_SELECT0 | FDC_ST0_UNIT_SELECT1);	\
	FDC_ST3 |= (fdc.CurrentSide<<2) | fdc.CurrentDrive

#define FDC_ST0_SET_DRIVE				\
	fdc.ST0 &= ~(FDC_ST0_HEAD_ADDRESS | FDC_ST0_UNIT_SELECT0 | FDC_ST0_UNIT_SELECT1);	\
	fdc.ST0 |= fdc.CurrentDrive

static char	FDC_DataBuffer[16384];

static FDC_COMMAND FDC_CommandTable[]=
{
	{1-1,FDC_Invalid},				/* %00000	** invalid ** */
	{1-1,FDC_Invalid},				/* %00001	** invalid ** */
	{9-1,FDC_ReadATrack},				/* %00010 READ A TRACK */
	{3-1,FDC_Specify},				/* %00011 SPECIFY */
	{2-1,FDC_SenseDriveStatus},		/* %00100 SENSE DRIVE STATUS */
	{9-1,FDC_WriteData},				/* %00101 WRITE DATA */
	{9-1,FDC_ReadData},				/* %00110 READ DATA */
	{2-1,FDC_Recalibrate},			/* %00111 RECALIBRATE */
	{1-1,FDC_SenseInterruptStatus},	/* %01000 SENSE INTERRUPT STATUS */
	{9-1,FDC_WriteData /*FDC_WriteDeletedData*/},		/* %01001 WRITE DELETED DATA */
	{2-1,FDC_ReadID},					/* %01010 READ ID */
	{1-1,FDC_Invalid},				/* %01011 ** invalid ** */
	{9-1,FDC_ReadDeletedData},		/* %01100 READ DELETED DATA */
	{6-1,FDC_FormatATrack},			/* %01101 FORMAT A TRACK */
	{1-1,FDC_Invalid},				/* %01110 ** invalid ** */
	{3-1,FDC_Seek},					/* %01111 SEEK */
	{1-1,FDC_Invalid},				/* %10000 ** invalid ** */
	{9-1,FDC_ScanEqual},				/* %10001 SCAN EQUAL */
	{1-1,FDC_Invalid},				/* %10010 ** invalid ** */
	{1-1,FDC_Invalid},				/* %10011 ** invalid ** */
	{1-1,FDC_Invalid},				/* %10100 ** invalid ** */
	{1-1,FDC_Invalid},				/* %10101 ** invalid ** */
	{1-1,FDC_Invalid},				/* %10110 ** invalid ** */
	{1-1,FDC_Invalid},				/* %10111 ** invalid ** */
	{1-1,FDC_Invalid},				/* %11000 ** invalid ** */
	{9-1,FDC_ScanLowOrEqual},				/* %11001 SCAN LOW OR EQUAL */
	{1-1,FDC_Invalid},				/* %11010 ** invalid ** */
	{1-1,FDC_Invalid},				/* %11011 ** invalid ** */
	{1-1,FDC_Invalid},				/* %11100 ** invalid ** */
	{9-1,FDC_ScanHighOrEqual},		/* %11101 SCAN HIGH OR EQUAL */
	{1-1,FDC_Invalid},				/* %11110 ** invalid ** */
	{1-1,FDC_Invalid},				/* %11111 ** invalid ** */
};

#ifdef FDC_DEBUG
static char * FDC_CommandTableNames[]=
{
	"Invalid",
	"Invalid",
	"Read A Track",
	"Specify",
	"Sense Drive Status",
	"Write Data",
	"Read Data",
	"Recalibrate",
	"Sense Interrupt Status",
	"Write Deleted Data",
	"Read ID",
	"Invalid",
	"Read Deleted Data",
	"Format A Track",
	"Invalid",
	"Seek",
	"Invalid",
	"Scan Equal",
	"Invalid",
	"Invalid",
	"Invalid",
	"Invalid",
	"Invalid",
	"Invalid",
	"Invalid",
	"Scan Low Or Equal",
	"Invalid",
	"Invalid",
	"Invalid",
	"Scan High Or Equal",
	"Invalid",
	"Invalid"
};
#endif

FDD		*FDD_GetDrive(int DriveIndex);


static void FDC_SetupPhysicalDriveAndSide(void)
{

	fdc.PhysicalDrive = fdc.CurrentDrive;

	fdc.drive = FDD_GetDrive(fdc.PhysicalDrive);

	fdc.PhysicalSide = fdc.CurrentSide^fdc.drive->PhysicalSide;

	fdc.PhysicalTrack = fdc.drive->CurrentTrack;
}

static void FDC_GetDriveAndSide(void)
{
	unsigned char DriveAndSide = fdc.CommandBytes[1];

	fdc.CurrentDrive = DriveAndSide & FDC_COMMAND_UNIT_CPC;					
	fdc.CurrentSide = ((DriveAndSide & FDC_COMMAND_HEAD_ADDRESS)>>2);

	FDC_SetupPhysicalDriveAndSide();
}

static void FDC_GetDrive(void)
{
	fdc.CurrentDrive = fdc.CommandBytes[1] & FDC_COMMAND_UNIT_CPC;	
	fdc.CurrentSide = 0;

	FDC_SetupPhysicalDriveAndSide();
}



void	FDC_InitialiseIDAndDataPositions(void);
void	FDD_SetNopCount(void);



char TrackBuffer[7*1024];

void	FDD_InitialiseAll(void)
{
	int i;

	MakeTrack(TrackBuffer);

	FDC_InitialiseIDAndDataPositions();


	for (i=0; i<2; i++)
	{
		FDD *theDrive = &drive[i];

		theDrive->CurrentTrack = 40;
		theDrive->NumberOfTracks = 43;
		theDrive->Flags = 0;
		theDrive->PhysicalSide = 0;
	}
}

BOOL FDD_GetMotorState(void)
{
	if ((FDD_MotorState & 0x01)==0x01)
	{
		return TRUE;
	}
	
	return FALSE;
}

unsigned char FDC_GetMainStatusRegister(void)
{
	return fdc.MainStatusRegister;
}

void	FDC_SetMainStatusRegister(unsigned char Status)
{
	fdc.MainStatusRegister = Status;
}

unsigned char FDC_GetDataRegister(void)
{
	return 0x0ff;
}

void	FDC_SetDataRegister(unsigned char Data)
{
}



static int LEDState = 0;

void	FDD_UpdateLEDState(void)
{
    /* disc drive light indicator*/
    if (FDD_GetLEDState()!=0)
    {
            Host_DoDriveLEDIndicator(0, TRUE);
    }
    else
    {
            Host_DoDriveLEDIndicator(0, FALSE);
    }
}

/* the disc light comes on for a read/write operation only */
void	FDD_LedOn(int LedState)
{
	LEDState = LedState;	

/*	FDD_UpdateLEDState(); */

}

int		FDD_GetLEDState(void)
{
	return LEDState;
}



void	FDD_MotorControl(int MotorStatus)
{
#ifdef FDC_DEBUG
	if (Debug_Active(FDC_Debug))
	{
		sprintf(DebugString,"Disc motor state: %d\r\n",MotorStatus & 0x01);
		Debug_WriteString(FDC_Debug,DebugString);
	}
#endif
	
	FDD_MotorState = MotorStatus & 0x01;

	FDD_SetNopCount();

}

FDD		*FDD_GetDrive(int DriveIndex)
{
	/* on Amstrad only DS0 is connected! */
	
	return &drive[DriveIndex & 0x01];
}

/* perform the actual step */
void	FDD_PerformStep(int DriveIndex, int StepDirection)
{
	FDD *theDrive;
	int CurrentTrack;
	int Flags;

	theDrive = FDD_GetDrive(DriveIndex);
	
	/* perform step */
	CurrentTrack = theDrive->CurrentTrack;
	CurrentTrack += StepDirection;

	/* range check head position */
	if (CurrentTrack<0)
	{
		CurrentTrack = 0;
	}
	else
	if (CurrentTrack>=theDrive->NumberOfTracks)
	{
		CurrentTrack = theDrive->NumberOfTracks-1;
	}

	Flags = theDrive->Flags;
	Flags &= ~FDD_FLAGS_HEAD_AT_TRACK_0;

	/* head at track 0? */
	if (CurrentTrack == 0)
	{
		/* yes */
		Flags |= FDD_FLAGS_HEAD_AT_TRACK_0;
	}
	
	theDrive->Flags = Flags;

	theDrive->CurrentTrack = CurrentTrack;

	theDrive->CurrentIDIndex = 0;
}


/* data rate in microseconds - this is the nominal rate -
max time for a bit of data and clock is 4 us. (4*8)=32! */
#define DATA_RATE_US	32		





#define TRACK_SIZE	6656

#define MAX_MARKS			20

#define FDC_ID_MARK				0x0fe
#define FDC_DATA_MARK			0x0fb
#define FDC_DELETED_DATA_MARK	0x0f8

typedef struct 
{
	unsigned long Positions[MAX_MARKS];
	unsigned long Count;
} FDC_MFM_MARK;

FDC_MFM_MARK	MFM_ID;
FDC_MFM_MARK	Data;


void	FDC_InitialiseIDAndDataPositions(void)
{
	int i;
	unsigned char *pDataPtr;
	unsigned char *pDataStart;

	MFM_ID.Count = 0;
	Data.Count = 0;

	pDataPtr = (pDataStart = (unsigned char *)TrackBuffer);

	for (i=0; i<TRACK_SIZE; i++)
	{
		switch (pDataPtr[0])
		{
			case FDC_ID_MARK:
			{
				MFM_ID.Positions[MFM_ID.Count] = (unsigned long)pDataPtr - (unsigned long)pDataStart;
				MFM_ID.Count++;
			}
			break;


			case FDC_DATA_MARK:
			case FDC_DELETED_DATA_MARK:
			{
				Data.Positions[Data.Count] = (unsigned long)pDataPtr - (unsigned long)pDataStart;
				Data.Count++;
			}
			break;
		}
	
		pDataPtr++;
	}
}




/* get closest mark to the current position, marks points to either ID or data marks */
unsigned long	FDC_GetClosestMarkToCurrentPosition(unsigned long PositionInTrack, FDC_MFM_MARK *marks)
{
	int i;

	/* simple search through list for now */
	for (i=0; i<marks->Count; i++)
	{
		if (marks->Positions[i]>=PositionInTrack)
		{
			return marks->Positions[i];
		}
	}

	/* if no mark is close enough, then the next closest mark is at the beginning of the track */
	return marks->Positions[0];
}


/* gets the number of bytes from the current position to the next mark position */
unsigned long	FDC_GetBytesToNextMark(unsigned long PositionInTrack, FDC_MFM_MARK *marks)
{
	if (marks->Count==0)
	{
		/* time to get to index from current position + time to see next index after that. */
		return (TRACK_SIZE - PositionInTrack) + TRACK_SIZE;
	}

	{
		unsigned long ClosestMarkPosition = FDC_GetClosestMarkToCurrentPosition(PositionInTrack, marks);

		if (ClosestMarkPosition<PositionInTrack)
		{
			return (TRACK_SIZE - PositionInTrack)+ClosestMarkPosition;
		}

		return ClosestMarkPosition - PositionInTrack;
	}
}

static unsigned long LastPositionInTrack = 0;
static unsigned long NopFraction = 0;
static unsigned long PreviousNopCount = 0;

void	FDD_SetNopCount(void)
{
	PreviousNopCount = CPC_GetNopCount();
}


unsigned long	FDD_CalculatePositionInTrack(void)
{
	unsigned long NopCount;
	unsigned long BytesPassed;
	unsigned long CurrentNopCount;
	unsigned long NopDifference;
	unsigned long Offset;

	CurrentNopCount = CPC_GetNopCount();
	NopDifference = CurrentNopCount - PreviousNopCount;
	NopDifference = NopDifference+NopFraction;

	/* calculate new nop fraction */
	NopFraction = NopDifference & 31;
	/* calculate bytes passed since last time */
	BytesPassed = NopDifference>>5;

	PreviousNopCount = CurrentNopCount;

	/* calculate new position from old position and number of bytes
	passed, and make sure it is within the track size */
	Offset = (LastPositionInTrack + BytesPassed) % TRACK_SIZE;

	/* store position for next time */
	LastPositionInTrack = Offset;

	return Offset;
}

/* get NOPS to ID position */
unsigned long	FDC_GetNopsToNextID(void)
{
	unsigned long Position;
	unsigned long BytesToNextMark;

	/* calculate current position in track */
	Position = FDD_CalculatePositionInTrack();

	/* get number of bytes to next mark */
	BytesToNextMark = FDC_GetBytesToNextMark(Position, &MFM_ID);

	return BytesToNextMark<<5;
}


unsigned long FDC_GetNopsToNextIndex(void)
{
	unsigned long PositionInTrack;

	PositionInTrack = FDD_CalculatePositionInTrack();

	return ((TRACK_SIZE - PositionInTrack)<<5);
}


unsigned long FDD_GetFlags(int DriveIndex)
{
	return drive[DriveIndex].Flags;
}


INLINE void	FDC_SeekComplete(int DriveIndex)
{
	{
		int ST0;

		ST0 = fdc.ST0;
		ST0 &= ~(FDC_ST0_HEAD_ADDRESS | FDC_ST0_UNIT_SELECT0 | FDC_ST0_UNIT_SELECT1);	
		ST0 |= DriveIndex;
		fdc.ST0 = ST0;
	}

	{
		int Flags;

		Flags = fdc.Flags;
		Flags |= NEC765_FLAGS_INTERRUPT;
		Flags &= ~NEC765_FLAGS_SEEK_OPERATION;
		fdc.Flags = Flags;
	}

/*	FDC_InterruptActive = TRUE; */

}

void	FDC_NormalSeekComplete(int DriveIndex)
{
	/* normal termination */
	fdc.ST0 |= FDC_ST0_SEEK_END | FDC_INTERRUPT_CODE_NT;

	/* end seek */
	FDC_SeekComplete(DriveIndex);
}



/* update seek */
void	FDC_CheckSeek(int DriveIndex)
{
	/* recalibrate? */
	if (fdc.Flags & NEC765_FLAGS_SEEK_OPERATION_IS_RECALIBRATE)
	{
		/* at track 0? */
		if (FDD_GetFlags(DriveIndex) & FDD_FLAGS_HEAD_AT_TRACK_0)
		{
			/* seek complete */
			FDC_NormalSeekComplete(DriveIndex);
			return;
		}
	}
	else
	{
		/* seek? */
		int PCN;

		/* get PCN */
		PCN = fdc.PCN[DriveIndex];

		/* is PCN == NCN? */
		if (PCN == fdc.NCN)
		{
			/* seek complete */
			FDC_NormalSeekComplete(DriveIndex);
			return;
		}
	}

	/* not complete after 77 step pulses have been issued? */
	if (fdc.StepCount[DriveIndex]>=77)
	{
		int ST0;

		ST0 = fdc.ST0;
		ST0 |= FDC_ST0_SEEK_END | FDC_ST0_EQUIPMENT_CHECK | FDC_ST0_INTERRUPT_CODE0;
		ST0 &= ~FDC_ST0_INTERRUPT_CODE1;
		fdc.ST0 = ST0;

		FDC_SeekComplete(DriveIndex);
	}

}

void	FDC_BeginSeek(void)
{
	int DriveIndex;

	/* initialise ST0 */
	fdc.ST0 = 0;
	fdc.ST1 = 0;
	fdc.ST2 = 0;
	/* get drive index */
	DriveIndex = fdc.CommandBytes[1] & 0x03;

	/* set seek active bit in main status register */
	fdc.MainStatusRegister|=(1<<DriveIndex);
	
	/* initialise step count */
	fdc.StepCount[DriveIndex] = 0;

	/* doing a seek operation */
	/* say it's a recalibrate. Will get cleared if it is a seek operation */
	fdc.Flags |= NEC765_FLAGS_SEEK_OPERATION | 
				NEC765_FLAGS_SEEK_OPERATION_IS_RECALIBRATE;
	
	fdc.Flags &= ~NEC765_FLAGS_INTERRUPT;

	/* seek? */
	if ((fdc.CommandBytes[0] & FDC_COMMAND_WORD)==0x0f)
	{
		int PCN;

		/* get PCN */
		PCN = fdc.PCN[DriveIndex];

		/* yes */
		fdc.Flags &= ~NEC765_FLAGS_SEEK_OPERATION_IS_RECALIBRATE;
	
		/* new cylinder number */
		fdc.NCN = fdc.CommandBytes[2] & 0x0ff;
	
		/* set seek direction */
		if (PCN<fdc.NCN)
		{
			fdc.StepDirection = 1;
		}
		else
		{
			fdc.StepDirection = -1;
		}
	}
	else
	{
		fdc.PCN[DriveIndex] = 0;

		/* set step direction */
		fdc.StepDirection = -1;
	}

	fdc.StepTimeForDrive[DriveIndex] = CPC_GetNopCount();

	/* check if seek has completed */
	FDC_CheckSeek(DriveIndex);

	/* can accept attempts to write commands */
	FDC_Standby();

}


void	FDC_DoSeekOperation(int DriveIndex)
{
	/* step drive head */
	FDD_PerformStep(DriveIndex, fdc.StepDirection);

	/* update step count */
	fdc.StepCount[DriveIndex]++;

	if ((fdc.Flags & NEC765_FLAGS_SEEK_OPERATION_IS_RECALIBRATE)==0)
	{
		/* update PCN */
		fdc.PCN[DriveIndex]+=fdc.StepDirection;
	}

	/* check seek completed */
	FDC_CheckSeek(DriveIndex);
}

void	FDC_DoSeekOperationOnDrive(int DriveIndex, unsigned long NopCount)
{
	unsigned long NopDifference;

	/* get number of nops that have passed since the last seek update */
	NopDifference = NopCount - fdc.StepTimeForDrive[DriveIndex];

	/* while we can step some more and a seek operation is active for this drive */
	while ((NopDifference>=fdc.StepTime) && ((fdc.Flags & NEC765_FLAGS_SEEK_OPERATION)!=0))
	{
		/* update nop difference to account for a single step */
		NopDifference -= fdc.StepTime;

		/* do step */
		FDC_DoSeekOperation(DriveIndex);
	}

	/* store new time taking into account the number of nops remaining */
	fdc.StepTimeForDrive[DriveIndex] = NopCount - NopDifference;
}

void	FDC_UpdateDrives(void)
{
	unsigned long NopCount;

	NopCount = CPC_GetNopCount();

	if (fdc.Flags & NEC765_FLAGS_SEEK_OPERATION)
	{
		int i;
		int DriveMask;
		int MSR = fdc.MainStatusRegister;

		DriveMask = 1<<3;

		for (i=3; i>=0; i--)
		{
			/* seek? */
			if (MSR & DriveMask)
			{
				FDC_DoSeekOperationOnDrive(i, NopCount);
			}

			DriveMask = DriveMask>>1;
		}
	}
}


static unsigned long FDC_NopOfLastDataTransfer;
#if 0
void	FDC_SignalDataRequest()
{
	fdc.Flags |= NEC765_FLAGS_DATA_TRANSFER;
	/* byte is waiting */
	fdc.MainStatusRegister |= FDC_MSR_DATA_REQUEST;
	/* setup when this byte was made available */
	fdc.NopOfLastDataTransfer = CPC_GetNopCount();
}
#endif
/* clear the low level state */
void	FDC_ClearLowLevelState(void)
{
	fdc.LowLevelState = -1;

	FDC_UpdateState();
}

/* setup a new low level state */
void	FDC_SetLowLevelState(int NewState)
{
	fdc.LowLevelState = NewState;

	FDC_UpdateState();

}

void	FDC_PopHighLevelState(void)
{
	fdc.HighLevelState = fdc.PushedHighLevelState;

	FDC_UpdateState();
}

void	FDC_PushHighLevelState(void)
{
	fdc.PushedHighLevelState = fdc.HighLevelState;
}

void	FDC_SetHighLevelState(int NewState)
{
	fdc.HighLevelState = NewState;

	FDC_UpdateState();

}

#if 0
void	FDC_ClearDataRequest(void)
{
	fdc.Flags &= ~NEC765_FLAGS_DATA_TRANSFER;
	fdc.MainStatusRegister &= ~FDC_MSR_DATA_REQUEST;
}
#endif

/* setup a delay before data request is set */
void	FDC_SetupDataRequestDelay(unsigned long CyclesToDataRequest)
{
	fdc.NopCountOfDataRequestStart = CPC_GetNopCount();
	fdc.CyclesToDataRequest = CyclesToDataRequest;

	FDC_SetLowLevelState(NEC765_LOW_LEVEL_STATE_DELAY);

}

void	FDC_SetupReadExecutionPhase(int DataSize, char *pBuffer)
{
	fdc.ExecutionBufferByteIndex = 0;
	fdc.ExecutionNoOfBytes = DataSize;
	fdc.ExecutionBuffer = pBuffer;

	/* push the high-level state */
	FDC_PushHighLevelState();

	/* clear data request */
	fdc.MainStatusRegister |= FDC_MSR_DATA_FLOW_DIRECTION;
	fdc.MainStatusRegister &= ~FDC_MSR_DATA_REQUEST;

	/* setup delay before first data request is issued (first byte is ready) */
	FDC_SetupDataRequestDelay(DATA_RATE_US);

	/* set high level state for data transfer */
/*	FDC_SetHighLevelState(NEC765_HIGH_LEVEL_STATE_EXECUTION_PHASE_READ_DATA); */

	fdc.HighLevelState = NEC765_HIGH_LEVEL_STATE_EXECUTION_PHASE_READ_DATA;

}

void	FDC_SetupResultPhase(int NoOfStatusBytes)
{
#ifdef FDC_DEBUG
	if (Debug_Active(FDC_Debug))
	{
		Debug_WriteString(FDC_Debug,"--- FDC Starting Result Phase ---\r\n");
	}
#endif

	/* fdc not in execution phase */
	fdc.MainStatusRegister &= ~(FDC_MSR_EXECUTION_PHASE | FDC_MSR_DATA_REQUEST);
	/* fdc data ready, and direction to cpu */
	fdc.MainStatusRegister |= FDC_MSR_DATA_FLOW_DIRECTION;
	
	fdc.CommandBytesRemaining = NoOfStatusBytes;
	fdc.CommandByteIndex = 0;

	FDC_SetHighLevelState(NEC765_HIGH_LEVEL_STATE_RESULT_PHASE);
}



void	FDC_SetupWriteExecutionPhase(int DataSize, char *pBuffer)
{
	fdc.ExecutionBufferByteIndex = 0;
	fdc.ExecutionNoOfBytes = DataSize;
	fdc.ExecutionBuffer = pBuffer;

	/* push the high-level state */
	FDC_PushHighLevelState();

	/* set data direction */
	fdc.MainStatusRegister &= ~(FDC_MSR_DATA_FLOW_DIRECTION);
	/* request data */
	fdc.MainStatusRegister |= FDC_MSR_DATA_REQUEST;
	/* setup delay before data request is re-issued */
	FDC_SetupDataRequestDelay(DATA_RATE_US);

	/* set high level state for data transfer */
/*	FDC_SetHighLevelState(NEC765_HIGH_LEVEL_STATE_EXECUTION_PHASE_WRITE_DATA); */
	fdc.HighLevelState = NEC765_HIGH_LEVEL_STATE_EXECUTION_PHASE_WRITE_DATA;
}


void	FDC_SetupForExecutionPhase()
{
	/* setup initial command state */
	fdc.CommandState = 0;

	/* no data ready */
	fdc.MainStatusRegister &= ~FDC_MSR_DATA_REQUEST;
	/* entering execution phase */
	fdc.MainStatusRegister |= FDC_MSR_EXECUTION_PHASE;
	
#ifdef FDC_DEBUG
	if (Debug_Active(FDC_Debug))
	{
		Debug_WriteString(FDC_Debug,"--- FDC starting execution phase ---\r\n");
	}
#endif


	/* enable ID on a read/write operation */
	switch (fdc.CommandBytes[0] & FDC_COMMAND_WORD)
	{
		/* read a track */
		/* write data */
		/* read data */
		/* write deleted data */
		/* read id */	
		/* read deleted data */
		/* format a track */
		/* scan equal */
		case 2:
		case 5:
		case 6:
		case 9:
		case 10:
		case 12:
		case 13:
		case 17:
		case 25:
		case 29:
		{
			FDD_LedOn(TRUE);
		}
		break;
	
		default:
			break;
	}
	
	FDC_SetHighLevelState(NEC765_HIGH_LEVEL_STATE_EXECUTION_PHASE);
	
}

void	FDC_UpdateState(void)
{
	FDC_UpdateDrives();

	if (fdc.LowLevelState!=-1)
	{
		switch (fdc.LowLevelState)
		{	
			/* delay for a specified period, then set data request.
			If data request is already set, then Overrun condition is set */
			case NEC765_LOW_LEVEL_STATE_DELAY:
			{
				unsigned long NopCount;
				unsigned long Difference;

				/* get current nop count */
				NopCount = CPC_GetNopCount();

				/* get difference = number of cycles passed so far */
				Difference = NopCount - fdc.NopCountOfDataRequestStart;
				
				/* exceeded time to data request? */
				if (Difference>=fdc.CyclesToDataRequest)
				{
					/* clear low level state */
					FDC_ClearLowLevelState();

				}


			}
			break;

			default:
				break;
		}

		return;
	}



	switch (fdc.HighLevelState)
	{
		case NEC765_HIGH_LEVEL_STATE_COMMAND_PHASE_FIRST_BYTE:
		{
			/* data has been written */
			if ((fdc.MainStatusRegister & FDC_MSR_DATA_REQUEST)==0)
			{
				unsigned long Data;
				int	CommandIndex;

				Data = fdc.DataRegister;

				/* seek operation active?  */
				if (fdc.Flags & NEC765_FLAGS_SEEK_OPERATION)
				{
					/* invalid */
					Data = 0;
				}

				/* busy set after writing first byte of command  */
				fdc.MainStatusRegister |= FDC_MSR_BUSY;

				/* store byte */
				fdc.CommandBytes[0] = Data;
				/* store number of bytes written so far */
				fdc.CommandByteIndex = 1;
			
				/* get command word */
				CommandIndex = Data & FDC_COMMAND_WORD;
				
#if 0
#ifdef FDC_DEBUG
	if (Debug_Active(FDC_Debug))
	{
		sprintf(DebugString,"Command: %02x (%s)\r\n",Data, FDC_CommandTableNames[CommandIndex]);
		Debug_WriteString(FDC_Debug,DebugString);
	}
#endif
#endif
				/* set current command */
				fdc.CommandHandler = FDC_CommandTable[CommandIndex].CommandHandler; 

				/* set number of bytes left to transfer */
				fdc.CommandBytesRemaining = FDC_CommandTable[CommandIndex].NoOfCommandBytes;

				/* completed command? */
				if (fdc.CommandBytesRemaining==0)
				{
					FDC_SetupForExecutionPhase();
				}
				else
				{
					/* issue a data request */
					fdc.MainStatusRegister |= FDC_MSR_DATA_REQUEST;
					FDC_SetHighLevelState(NEC765_HIGH_LEVEL_STATE_COMMAND_PHASE_REMAINING_BYTES);
				}
			}
		}
		break;

		case NEC765_HIGH_LEVEL_STATE_COMMAND_PHASE_REMAINING_BYTES:
		{
			/* data has been written */
			if ((fdc.MainStatusRegister & FDC_MSR_DATA_REQUEST)==0)
			{
				/* store byte */
				fdc.CommandBytes[fdc.CommandByteIndex] = fdc.DataRegister;
				/* store number of bytes written so far */
				fdc.CommandByteIndex++;
				/* decrement number of bytes to be transfered. */
				fdc.CommandBytesRemaining--;
#if 0
#ifdef FDC_DEBUG
	if (Debug_Active(FDC_Debug))
	{
		sprintf(DebugString,"Command Data: %02x \r\n",fdc.DataRegister);
		Debug_WriteString(FDC_Debug,DebugString);
	}
#endif
#endif

				/* completed command? */
				if (fdc.CommandBytesRemaining==0)
				{
					/* go to next state */
					FDC_SetupForExecutionPhase();
				}
				else
				{
					/* issue a data request */
					fdc.MainStatusRegister |= FDC_MSR_DATA_REQUEST;
				
				}
			}
		}
		break;

		case NEC765_HIGH_LEVEL_STATE_EXECUTION_PHASE_READ_DATA:
		{
			/* a read of the data register will clear the data request */
		
			/* overrun condition doesn't apply for last byte */
 			if (fdc.ExecutionNoOfBytes==0)
			{
				fdc.MainStatusRegister &=~FDC_MSR_DATA_REQUEST;
				FDC_PopHighLevelState();
				return;
			}
		

			/* data request set? */
			if (fdc.MainStatusRegister & FDC_MSR_DATA_REQUEST)
			{
				/* set overrun condition */
				fdc.ST1 |= FDC_ST1_OVERRUN;
				/* clear request */
				fdc.MainStatusRegister &=~FDC_MSR_DATA_REQUEST;
				/* go to high level state */
				FDC_PopHighLevelState();
				return;
			}
		
			
			{
				/* overrun not set */

				/* get byte of data - store in data register */
				fdc.DataRegister = fdc.ExecutionBuffer[fdc.ExecutionBufferByteIndex];
				fdc.ExecutionBufferByteIndex++;
				fdc.ExecutionNoOfBytes--;

				fdc.MainStatusRegister |= FDC_MSR_DATA_REQUEST;
				/* setup a delay before data request is set */
				FDC_SetupDataRequestDelay(DATA_RATE_US);
			}
		}
		break;
	
		case NEC765_HIGH_LEVEL_STATE_EXECUTION_PHASE_WRITE_DATA:
		{
			/* overrun not set */

			/* get data register and store data */
			fdc.ExecutionBuffer[fdc.ExecutionBufferByteIndex] = fdc.DataRegister;
			fdc.ExecutionBufferByteIndex++;
			fdc.ExecutionNoOfBytes--;

			/* any bytes remaining? */
			if (fdc.ExecutionNoOfBytes==0)
			{
				/* no */
				fdc.MainStatusRegister &=~FDC_MSR_DATA_REQUEST;
				FDC_PopHighLevelState();
				return;
			}
			else
			{
				/* yes */

				/* data request set? */
				if (fdc.MainStatusRegister & FDC_MSR_DATA_REQUEST)
				{
					/* set overrun condition */
					fdc.ST1 |= FDC_ST1_OVERRUN;
					/* clear request */
					fdc.MainStatusRegister &=~FDC_MSR_DATA_REQUEST;
					/* go to high level state */
					FDC_PopHighLevelState();
					return;
				}
			
				
				fdc.MainStatusRegister |= FDC_MSR_DATA_REQUEST;
				/* setup a delay before data request is set */
				FDC_SetupDataRequestDelay(DATA_RATE_US);
			}
		}
		break;

		case NEC765_HIGH_LEVEL_STATE_EXECUTION_PHASE:
		{
			/* execute command handler */
			fdc.CommandHandler(fdc.CommandState);
		}
		break;

		case NEC765_HIGH_LEVEL_STATE_RESULT_PHASE:
		{
			/* data has been read */
			if ((fdc.MainStatusRegister & FDC_MSR_DATA_REQUEST)==0)
			{
				if (fdc.CommandBytesRemaining==0)
				{
					/* go to next state */
					FDC_Standby();
				}
				else
				{

					/* store byte */
					fdc.DataRegister = fdc.CommandBytes[fdc.CommandByteIndex];
					/* store number of bytes written so far */
					fdc.CommandByteIndex++;
					/* decrement number of bytes to be transfered. */
					fdc.CommandBytesRemaining--;
#if 0
#ifdef FDC_DEBUG
	if (Debug_Active(FDC_Debug))
	{
		sprintf(DebugString,"Result Data: %02x \r\n",fdc.DataRegister);
		Debug_WriteString(FDC_Debug,DebugString);
	}
#endif
#endif

					/* issue a data request */
					fdc.MainStatusRegister |= FDC_MSR_DATA_REQUEST;
				}
			}



		}
		break;



	}

}



#if 0
			case NEC765_LOW_LEVEL_STATE_TRANSFER_DATA_BYTE:
			{
				unsigned long NopCount;	
				unsigned long Difference;

				/* get current nop count */
				NopCount = CPC_GetNopCount();

				/* calculate difference */
				Difference = NopCount - FDC_NopOfLastDataTransfer;

				/* does difference exceed data transfer speed? */
				if ((Difference>DATA_RATE_US) && (FDC_ExecutionBufferByteIndex!=0))
				{
					/* set over-run condition */
					fdc.ST1 |= FDC_ST1_OVERRUN;

					FDC_ClearDataRequest();

					/* no data ready */
					FDC_MainStatusRegister &= ~FDC_MSR_DATA_REQUEST;
				
					FDC_ExecutionPhase();			
				}
			}
			break;
#endif





/* read from main status register */
unsigned int	FDC_ReadMainStatusRegister(void)
{
#if 0
#ifdef FDC_DEBUG
	if (Debug_Active(FDC_Debug))
	{
		sprintf(DebugString,"Read main status\r\n");
		Debug_WriteString(FDC_Debug,DebugString);
	}
#endif
#endif

	/* update current state */
	FDC_UpdateState();

	/* return current status */
	return fdc.MainStatusRegister;
}

/* read from data register */
unsigned int	FDC_ReadDataRegister(void)
{
	unsigned char Data;
	/* to CPU? */
	if ((fdc.MainStatusRegister & FDC_MSR_DATA_FLOW_DIRECTION)!=0)
	{
		/* clear data request */
		fdc.MainStatusRegister &=~FDC_MSR_DATA_REQUEST;
	}

	/* get data from data register */
	Data = fdc.DataRegister;

	FDC_UpdateState();
#if 0
#ifdef FDC_DEBUG
	if (Debug_Active(FDC_Debug))
	{
		sprintf(DebugString,"Read data register: %02x\r\n",Data);
		Debug_WriteString(FDC_Debug,DebugString);
	}
#endif
#endif

	/* return data */
	return Data;

}

/* - when writing command bytes, data written to port can be read back again */
void	FDC_WriteDataRegister(int Data)
{
#if 0
#ifdef FDC_DEBUG
	if (Debug_Active(FDC_Debug))
	{
		sprintf(DebugString,"Write data register %02x\r\n",Data);
		Debug_WriteString(FDC_Debug,DebugString);
	}
#endif
#endif

	/* from CPU? */
	if ((fdc.MainStatusRegister & FDC_MSR_DATA_FLOW_DIRECTION)==0)
	{
		/* clear data request */
		fdc.MainStatusRegister &= ~FDC_MSR_DATA_REQUEST;
	}

	/* set data */
	fdc.DataRegister = Data;

	FDC_UpdateState();

}
#if 0
int		FDC_ReadDataNull(void)
{
	return 0x0ff;
}
#endif
#if 0
int		FDC_ReadResultPhase(void)
{
	int	Data;

	/* get result phase data */
	Data = 	fdc.CommandBytes[FDC_CommandByteIndex];
	FDC_CommandByteIndex++;
	FDC_NoOfCommandBytes--;

#ifdef FDC_DEBUG
	if (Debug_Active(FDC_Debug))
	{
		sprintf(DebugString,"Result: %02x\r\n",Data);
		Debug_WriteString(FDC_Debug,DebugString);
	}
#endif

	if (FDC_NoOfCommandBytes==0)
		FDC_Standby();


	return Data;
}
#endif
#if 0
int FDC_PreviousNopCount = 0;

int		FDC_ReadExecutionPhase(void)
{
	int	Data;

	/* get byte from buffer */
	Data = FDC_ExecutionBuffer[FDC_ExecutionBufferByteIndex];
	FDC_ExecutionBufferByteIndex++;
	FDC_ExecutionNoOfBytes--;

	if (FDC_ExecutionNoOfBytes==0)
	{
		FDC_ClearDataRequest();

		/* no data ready */
		FDC_MainStatusRegister &= ~FDC_MSR_DATA_REQUEST;
	
		/* continue with execution phase */
		fdc.CommandState++;
		fdc.CommandHandler(fdc.CommandState);

	}
	else
	{
		FDC_SignalDataRequest();
	}

	return Data;
}

void	FDC_WriteExecutionPhase(int Data)
{
	/* write byte to buffer */
	FDC_ExecutionBuffer[FDC_ExecutionBufferByteIndex] = (char)Data;
	FDC_ExecutionBufferByteIndex++;
	FDC_ExecutionNoOfBytes--;

	if (FDC_ExecutionNoOfBytes==0)
	{
		FDC_ClearDataRequest();
		/* no data ready */
		FDC_MainStatusRegister &= ~FDC_MSR_DATA_REQUEST;

		/* continue with execution phase */
		fdc.CommandState++;
		fdc.CommandHandler(fdc.CommandState);
	}
	else
	{
		FDC_SignalDataRequest();
	}



}



void	FDC_WriteFirstCommandByte(int Data)
{

}

void	FDC_WriteCommandBytes(int Data)
{
	/* write command bytes */

#if 0
#ifdef FDC_DEBUG
	if (Debug_Active(FDC_Debug))
	{
		sprintf(DebugString,"Command Byte: %02x\r\n",Data);
		Debug_WriteString(FDC_Debug,DebugString);
	}
#endif
#endif

	fdc.CommandBytes[FDC_CommandByteIndex] = Data;
	FDC_CommandByteIndex++;

	FDC_NoOfCommandBytes--;

	if (FDC_NoOfCommandBytes==0)
	{
		FDC_ExecuteCommandFunction();

	}
}
#endif

void	FDC_Standby(void)
{
#ifdef FDC_DEBUG
	if (Debug_Active(FDC_Debug))
	{
		Debug_WriteString(FDC_Debug,"-- FDC Standby ---\r\n");
	}
#endif
	/* set data flow to accept commands */
	/* set fdc not busy */
	/* set fdc not in execution phase */
	/* set fdc ready to accept data */
	fdc.MainStatusRegister &= (~(FDC_MSR_DATA_FLOW_DIRECTION|FDC_MSR_BUSY|FDC_MSR_EXECUTION_PHASE));
	fdc.MainStatusRegister |= FDC_MSR_DATA_REQUEST;
	fdc.LowLevelState = -1;
	FDC_SetHighLevelState(NEC765_HIGH_LEVEL_STATE_COMMAND_PHASE_FIRST_BYTE);

	/* was LED switched on? */
	if (FDD_GetLEDState()!=0)
	{
		/* yes - so disable it */
		FDD_LedOn(0);
	}

}

void	FDC_Reset(void)
{
	
	fdc.Flags = 0;
	fdc.PCN[0] = fdc.PCN[1] = fdc.PCN[2] = fdc.PCN[3] = 0;

	FDC_Standby();
}
#if 0
void	FDC_StartResultPhase(int FDC_NoOfStatusBytes)
{
#ifdef FDC_DEBUG
	if (Debug_Active(FDC_Debug))
	{
		Debug_WriteString(FDC_Debug,"--- FDC Starting Result Phase ---\r\n");
	}
#endif

	FDC_ClearDataRequest();

	/* fdc not in execution phase */
	fdc.MainStatusRegister &= ~FDC_MSR_EXECUTION_PHASE;
	/* fdc data ready, and direction to cpu */
	FDC_MainStatusRegister |= FDC_MSR_DATA_REQUEST | FDC_MSR_DATA_FLOW_DIRECTION;
	
	FDC_NoOfCommandBytes = FDC_NoOfStatusBytes;
	FDC_CommandByteIndex = 0;

	FDC_State = FDC_READ_RESULT_PHASE;	
}
#endif
#if 0
void	FDC_StartWriteExecutionPhase(int NoOfBytes, char *Buffer)
{
	/* data flow from CPU to FDC */
	fdc.MainStatusRegister &= ~FDC_MSR_DATA_FLOW_DIRECTION;
	/* data ready */
	fdc.MainStatusRegister |= FDC_MSR_DATA_REQUEST;

	fdc.ExecutionBufferByteIndex = 0;
	fdc.ExecutionNoOfBytes = NoOfBytes;
	fdc.ExecutionBuffer = Buffer;

	FDC_SetHighLevelState(
}

void	FDC_StartReadExecutionPhase(int NoOfBytes, char *Buffer)
{
	FDC_SignalDataRequest();

	/* data flow from FDC to CPU, and data ready */
	fdc.MainStatusRegister |= FDC_MSR_DATA_FLOW_DIRECTION;

	fdc.ExecutionBufferByteIndex = 0;
	fdc.ExecutionNoOfBytes = NoOfBytes;
	fdc.ExecutionBuffer = Buffer;

}
#endif

/*-------------------------------------------------------------*/
/* COMMANDS */

void	FDC_ScanHighOrEqual(int Phase)
{
}

void	FDC_ScanLowOrEqual(int Phase)
{
}

void	FDC_ScanEqual(int Phase)
{
}

/*
 Bit of a weird command this one.
 
 1. EOT holds the number of sectors to read. If this is more than the number
    of sectors in a track it will repeat the data from the sectors.

 2. During reading, if a sector contains an error. It continues.

 3. During reading, it compares the C,H,R,N values given against the sector
    values in the track. If there is no comparison it will set the ND bit.
    (no data)

 4. Multitrack or skip are not allowed. So it will read both data and
    deleted data without errors.

 5. It always reads from the first sector in the track (well that is what
    I think)

 6. If it cannot find any sectors on the track after encountering the index
    for the second time it will set the missing address mark bit.
*/

/* READ A TRACK */
#if 0
int ReadATrack_SectorOffset;

void	FDC_ReadATrackEnd(void)
{
	/* setup result data */
	FDC_SetStatus0();

	fdc.CommandBytes[6] = fdc.CommandBytes[5];
	fdc.CommandBytes[5] = fdc.CommandBytes[4];
	fdc.CommandBytes[4] = fdc.CommandBytes[3];
	fdc.CommandBytes[3] = fdc.CommandBytes[2];
	fdc.CommandBytes[0] = fdc.ST0;
	fdc.CommandBytes[1] = fdc.ST1;
	fdc.CommandBytes[2] = fdc.ST2;
	
	FDC_SetupResultPhase(7);

	fdc.Flags &= ~NEC765_FLAGS_INTERRUPT;
}

void	FDC_ReadATrackMiddle(void);

void	FDC_ReadATrackMiddle2(void)
{
	if (fdc.ST1 & FDC_ST1_OVERRUN)
	{
		FDC_ReadATrackEnd();
		return;
	}

	/* is sector offset == EOT?? */
	if (ReadATrack_SectorOffset == fdc.CommandBytes[6])
	{
		/* current offset = EOT. All sectors transfered.
		Finish command */
		FDC_ReadATrackEnd();
		return;
	}

	/* next sector */
	ReadATrack_SectorOffset++;

	/* go to attempt to read it */
	FDC_ReadATrackMiddle();

}

void	FDC_ReadATrackMiddle(void)
{
	FDC_CHRN	ReadATrack_CHRN;
	int			SectorIndexToRead;
	int			SectorsPerTrack;
	FDD			*drive = FDD_GetDrive(fdc.CurrentDrive);
	
	int			CurrentTrack = drive->CurrentTrack;
	
	int			CurrentSide;
	
	CurrentSide = fdc.CurrentSide^drive->PhysicalSide;

	/* get sectors per track */
	SectorsPerTrack = DiskImage_GetSectorsPerTrack(fdc.CurrentDrive,CurrentTrack,CurrentSide);

	if (SectorsPerTrack==0)
	{
		fdc.ST1 |= FDC_ST1_MISSING_ADDRESS_MARK;
	
		/* current offset = EOT. All sectors transfered.
		Finish command */
		FDC_ReadATrackEnd();
		return;
	}


	/* DIV here! */
	/* make sure sector index to read is within sectors per track */
  	SectorIndexToRead = ReadATrack_SectorOffset % SectorsPerTrack;

	/* is sector offset == EOT?? */
	if (ReadATrack_SectorOffset == fdc.CommandBytes[6])
	{
		/* current offset = EOT. All sectors transfered.
		Finish command */
		FDC_ReadATrackEnd();
		return;
	}

	/* get ID info for this sector */
	DiskImage_GetID(fdc.CurrentDrive,CurrentTrack,CurrentSide,SectorIndexToRead,&ReadATrack_CHRN);

	/* does programmed ID match this ID? */
	if ((ReadATrack_CHRN.C == fdc.CommandBytes[2]) &&
		(ReadATrack_CHRN.H == fdc.CommandBytes[3]) &&
		(ReadATrack_CHRN.R == fdc.CommandBytes[4]) &&
		(ReadATrack_CHRN.N == fdc.CommandBytes[5]))
	{
		/* C,H,R,N match those programmed */

		/* clear the no data flag */
		fdc.ST1 &= ~FDC_ST1_NO_DATA;
	}

	/* get sector data */
	DiskImage_GetSector(fdc.CurrentDrive,CurrentTrack,CurrentSide,ReadATrack_SectorOffset,FDC_DataBuffer);

	/* setup for execution phase */
	/*FDC_ExecutionPhase = FDC_ReadATrackMiddle2; */


	/* execution phase 2 */
	/*FDC_StartReadExecutionPhase(FDC_GetSectorSize(ReadATrack_CHRN.N),FDC_DataBuffer); */
	FDC_SetupReadExecutionPhase(FDC_GetSectorSize(ReadATrack_CHRN.N),FDC_DataBuffer);
}
#endif


void	FDC_ReadATrack(int State)
{
	switch (State)
	{
		case 0:
		{
			/* reset status registers */
			FDC_ClearStatusRegisters();

			/* get current drive and side */
			FDC_GetDriveAndSide();

			/* drive ready? */
			if (!(fdc.drive->Flags & FDD_FLAGS_DISK_PRESENT))
			{
				/* not ready */
				fdc.ST0 = FDC_ST0_NOT_READY;

				/* end command */
				fdc.CommandState = 4;	
				return;
			}


			/* for now */
			/* setup the initial sector offset */
			fdc.SectorCounter = 0;
			
			/* If a sector is found which has C,H,R,N matching that
			programmed, this flag will be reset. Otherwise it will
			remain set. Therefore indicating if a sector was found
			or not with ID that matched the programmed ID */
			fdc.ST1 |= FDC_ST1_NO_DATA;

			/* start command execution */
			fdc.CommandState++;
		}
		break;

		case 1:
		{
			FDC_CHRN	ReadATrack_CHRN;
			int			SectorIndexToRead;
			int			SectorsPerTrack;
			
			/* get sectors per track */
			SectorsPerTrack = DiskImage_GetSectorsPerTrack(
				fdc.PhysicalSide,
				fdc.PhysicalTrack,
				fdc.PhysicalSide);
			
			if (SectorsPerTrack==0)
			{
				fdc.ST1 |= FDC_ST1_MISSING_ADDRESS_MARK;
			
				/* current offset = EOT. All sectors transfered.
				Finish command */
				fdc.CommandState = 4;	
				return;
			}


			/* DIV here! */
			/* make sure sector index to read is within sectors per track */
  			SectorIndexToRead = fdc.SectorCounter % SectorsPerTrack;

			/* is sector offset == EOT?? */
			if (fdc.SectorCounter == fdc.CommandBytes[6])
			{
				/* current offset = EOT. All sectors transfered.
				Finish command */
				fdc.CommandState = 4;	
				return;
			}

			/* get ID info for this sector */
			DiskImage_GetID(
				fdc.PhysicalDrive,
				fdc.PhysicalTrack,
				fdc.PhysicalSide,
				SectorIndexToRead,&ReadATrack_CHRN);

			/* does programmed ID match this ID? */
			if ((ReadATrack_CHRN.C == fdc.CommandBytes[2]) &&
				(ReadATrack_CHRN.H == fdc.CommandBytes[3]) &&
				(ReadATrack_CHRN.R == fdc.CommandBytes[4]) &&
				(ReadATrack_CHRN.N == fdc.CommandBytes[5]))
			{
				/* C,H,R,N match those programmed */

				/* clear the no data flag */
				fdc.ST1 &= ~FDC_ST1_NO_DATA;
			}

			/* get sector data */
			DiskImage_GetSector(
				fdc.PhysicalDrive, 
				fdc.PhysicalTrack,
				fdc.PhysicalSide,
				fdc.SectorCounter,FDC_DataBuffer);

			/* setup for execution phase */
			fdc.CommandState++;
			/* execution phase 2 */
			FDC_SetupReadExecutionPhase(FDC_GetSectorSize(ReadATrack_CHRN.N),FDC_DataBuffer);
		}
		break;

		case 2:
		{
			if (fdc.ST1 & FDC_ST1_OVERRUN)
			{
				fdc.CommandState = 4;
				return;
			}

			/* is sector offset == EOT?? */
			if (fdc.SectorCounter == fdc.CommandBytes[6])
			{
				/* current offset = EOT. All sectors transfered.
				Finish command */
				fdc.CommandState = 4;
				return;
			}

			/* next sector */
			fdc.SectorCounter++;
			
			/* go to attempt to read it */
			fdc.CommandState--;	
		}
		break;

		case 4:
		{
			/* setup result data */
			FDC_SetStatus0();

			fdc.CommandBytes[6] = fdc.CommandBytes[5];
			fdc.CommandBytes[5] = fdc.CommandBytes[4];
			fdc.CommandBytes[4] = fdc.CommandBytes[3];
			fdc.CommandBytes[3] = fdc.CommandBytes[2];
			fdc.CommandBytes[0] = fdc.ST0;
			fdc.CommandBytes[1] = fdc.ST1;
			fdc.CommandBytes[2] = fdc.ST2;
			
			FDC_SetupResultPhase(7);

			fdc.Flags &= ~NEC765_FLAGS_INTERRUPT;
		}
		break;
	}
}



/* RECALIBRATE */
void	FDC_Recalibrate(int State)
{
	switch (State)
	{
		case 0:
		{
			FDC_BeginSeek();
	
			fdc.CommandState++;
		}
		break;

		case 1:
		{
		}
		break;
	}

}


void	FDC_Seek(int State)
{
	switch (State)
	{
		case 0:
		{
			FDC_BeginSeek();

			fdc.CommandState++;
		}
		break;

		case 1:
		{
			FDC_BeginSeek();
		}
		break;
	}
}

/* FORMAT A TRACK */
#if 0
static int FormatATrack_SectorCount;
#endif
static FDC_CHRN	FormatATrack_CHRN;
#if 0
void	FDC_FormatATrackMiddle(void);

void	FDC_FormatATrackEnd(void)
{
	/* setup result data */
	FDC_SetStatus0();

	fdc.CommandBytes[6] = FormatATrack_CHRN.N;
	fdc.CommandBytes[5] = (FormatATrack_CHRN.R+1) & 0x0ff;
	fdc.CommandBytes[4] = FormatATrack_CHRN.H;
	fdc.CommandBytes[3] = FormatATrack_CHRN.C;
	fdc.CommandBytes[0] = fdc.ST0;
	fdc.CommandBytes[1] = fdc.ST1;
	fdc.CommandBytes[2] = fdc.ST2;
	
	FDC_SetupResultPhase(7);

	fdc.Flags &= ~NEC765_FLAGS_INTERRUPT;
}

void	FDC_FormatATrackMiddle2(void)
{
	FDD *drive = FDD_GetDrive(fdc.CurrentDrive);

	/* got C,H,R,N, store for result phase*/
	FormatATrack_CHRN.C = FDC_DataBuffer[0];
	FormatATrack_CHRN.H = FDC_DataBuffer[1];
	FormatATrack_CHRN.R = FDC_DataBuffer[2];
	FormatATrack_CHRN.N = FDC_DataBuffer[3];

	/* write sector */
	/* fdc.CommandBytes[2] = N */ 
	/* fdc.CommandBytes[3] = SC */
	/* fdc.CommandBytes[4] = GPL */
	/* fdc.CommandBytes[5] = D */

	/* add sector to track */
	DiskImage_AddSector(fdc.CurrentDrive, drive->CurrentTrack, fdc.CurrentSide^drive->PhysicalSide, &FormatATrack_CHRN, fdc.CommandBytes[5]);

	/* next sector */
	FormatATrack_SectorCount++;

	FDC_FormatATrackMiddle();

}

void	FDC_FormatATrackMiddle(void)
{
	/* have we done all sectors */
	if (FormatATrack_SectorCount == fdc.CommandBytes[3])
	{
		/* finished transfering ID's */
		FDC_FormatATrackEnd();
		return;
	}
	else
	{

		/* setup for execution phase */
		/*FDC_ExecutionPhase = FDC_FormatATrackMiddle2; */
		
		/* get ID for this sector C,H,R,N */
		/*FDC_StartWriteExecutionPhase(4,FDC_DataBuffer); */
		FDC_SetupWriteExecutionPhase(4,FDC_DataBuffer);
	}
}


#endif

void	FDD_WriteBytesToTrack(char Data, unsigned long Count)
{
	unsigned long BytesToTrackEnd;

	/* calculate number of bytes to track end */
	BytesToTrackEnd = fdc.pTrackEnd - fdc.pTrackPtr;

	/* goes over end of track */
	if (BytesToTrackEnd>Count)
	{
		int i;

		/* doesn't go over end of track */
		unsigned char *pTrack;

		pTrack = fdc.pTrackPtr;

		for (i=0; i<Count; i++)
		{
			pTrack[0] = Data;
			pTrack++;
		}
	
		fdc.pTrackPtr = pTrack;
	}
	else
	{
		/* goes over end of track */
		int i;

		unsigned char *pTrack;

		/* write bytes to end of track */
		pTrack = fdc.pTrackPtr;

		for (i=0; i<BytesToTrackEnd; i++)
		{
			pTrack[0] = Data;
			pTrack++;
		}

		pTrack = fdc.pTrackStart;

		for (i=0; i<(Count - BytesToTrackEnd); i++)
		{
			pTrack[0] = Data;
			pTrack++;
		}

		fdc.pTrackPtr = pTrack;
	}
}

void	FDD_WriteByteToTrack(char Data)
{
	unsigned char *pTrack;

	pTrack = fdc.pTrackPtr;

	pTrack[0] = Data;
	pTrack++;

	if (pTrack>=fdc.pTrackEnd)
	{
		pTrack = fdc.pTrackStart;
	}

	fdc.pTrackPtr = pTrack;
}

/* FORMAT A TRACK */
/* currently only fetches ID information, and then gives result data */
void	FDC_FormatATrack(int State)
{
	switch (State)
	{
		case 0:
		{
			unsigned long NopsToNextIndex;

			/* reset status registers */
			FDC_ClearStatusRegisters();

			/* get current drive and side */
			FDC_GetDriveAndSide();

			/* initialise sector count */
			fdc.SectorCounter  = 0;

			DiskImage_EmptyTrack(
				fdc.PhysicalDrive,
				fdc.PhysicalTrack,
				fdc.PhysicalSide
				);
		
			fdc.CommandState++;
		
			/* this is the delay from the point that the ID field is written to
			the end of data for a sector */
			fdc.StoredDelay = 
			(
			/* CRC */
			2 + 
			/* GAP 2 */
			22 +
			/* Sync (0) */
			12 + 
			/* A1 */
			3 +
			/* Data/Deleted Data Address Mark */
			1 + 
			/* N */
			((1<<fdc.CommandBytes[2])<<7) + 
			/* CRC */
			2 +
			/* GAP 3 */
			fdc.CommandBytes[4]
			) * DATA_RATE_US;

			/* initialise write ptr */
			fdc.pTrackPtr = (unsigned char *)TrackBuffer;
			fdc.pTrackStart = (unsigned char *)TrackBuffer;
			fdc.pTrackEnd = fdc.pTrackStart + TRACK_SIZE;

			/* time to first data request is:
			time_to_next_index + 80 + 12 + 3 + 1 + 50 + 12 + 3
			*/

			NopsToNextIndex = FDC_GetNopsToNextIndex();

			FDC_SetupDataRequestDelay(((80+12+3+1+50+12+3)*DATA_RATE_US)+NopsToNextIndex);
		}
		break;

		case 1:
		{

			unsigned long NopsToNextIndex;


			/* have we done all sectors */
			if (fdc.SectorCounter == fdc.CommandBytes[3])
			{
				{
					/* C,H,R,N of previous track */
					FDD_WriteByteToTrack(FormatATrack_CHRN.C);
					FDD_WriteByteToTrack(FormatATrack_CHRN.H);
					FDD_WriteByteToTrack(FormatATrack_CHRN.R);
					FDD_WriteByteToTrack(FormatATrack_CHRN.N);

					/* write CRC for ID */
					FDD_WriteByteToTrack(fdc.CRC>>8);

					FDD_WriteByteToTrack(fdc.CRC);

					/* GAP 2 */
					FDD_WriteBytesToTrack(0x04e, 22);
					
					/* SYNC */
					FDD_WriteBytesToTrack(0x00, 12);

					FDD_WriteBytesToTrack(0x0a1, 3);

					/* data mark */
					FDD_WriteByteToTrack(FDC_DATA_MARK);

					/* sector data */
					{
						unsigned char FillerByte = fdc.CommandBytes[5];
						int SectorSize = ((1<<fdc.CommandBytes[2])<<7);

						FDD_WriteBytesToTrack(FillerByte, SectorSize);

					}

					FDD_WriteByteToTrack(fdc.CRC>>8);
					FDD_WriteByteToTrack(fdc.CRC);

					FDD_WriteBytesToTrack(0x04e, fdc.CommandBytes[4]);
				
				
					FDD_WriteBytesToTrack(0x04e, fdc.pTrackEnd - fdc.pTrackPtr);
				}

				


				NopsToNextIndex = FDC_GetNopsToNextIndex();

				/* finished transfering ID's */
				fdc.CommandState = 4;	
	
				/* last sector */
				FDC_SetupDataRequestDelay(fdc.StoredDelay + NopsToNextIndex);
				return;
			}
			else
			{
				if (fdc.SectorCounter==0)
				{
					FDD_WriteBytesToTrack(0x04e, 80);
					FDD_WriteBytesToTrack(0x00, 12);
					FDD_WriteBytesToTrack(0x0c2,3);
					FDD_WriteByteToTrack(0x0fc);
					FDD_WriteBytesToTrack(0x04e, 50);
					FDD_WriteBytesToTrack(0x00, 12);
					FDD_WriteBytesToTrack(0x0a1, 3);
				}


				if (fdc.SectorCounter!=0)
				{
					int i;

					/* C,H,R,N of previous track */
					FDD_WriteByteToTrack(FormatATrack_CHRN.C);
					FDD_WriteByteToTrack(FormatATrack_CHRN.H);
					FDD_WriteByteToTrack(FormatATrack_CHRN.R);
					FDD_WriteByteToTrack(FormatATrack_CHRN.N);

					/* write CRC for ID */
					FDD_WriteByteToTrack(fdc.CRC>>8);

					FDD_WriteByteToTrack(fdc.CRC);

					/* GAP 2 */
					FDD_WriteBytesToTrack(0x04e, 22);
					
					/* SYNC */
					FDD_WriteBytesToTrack(0x00, 12);

					FDD_WriteBytesToTrack(0x0a1, 3);

					/* data mark */
					FDD_WriteByteToTrack(FDC_DATA_MARK);

					/* sector data */
					{
						unsigned char FillerByte = fdc.CommandBytes[5];
						int SectorSize = ((1<<fdc.CommandBytes[2])<<7);

						FDD_WriteBytesToTrack(FillerByte, SectorSize);

					}

					FDD_WriteByteToTrack(fdc.CRC>>8);
					FDD_WriteByteToTrack(fdc.CRC);

					FDD_WriteBytesToTrack(0x04e, fdc.CommandBytes[4]);
				}



				/* setup for execution phase */
				fdc.CommandState++;

				/* get ID for this sector C,H,R,N */
				FDC_SetupWriteExecutionPhase(4,FDC_DataBuffer);
			}
		}
		break;

		case 2:
		{
			FDD *drive = FDD_GetDrive(fdc.CurrentDrive);

			/* got C,H,R,N, store for result phase*/
			FormatATrack_CHRN.C = FDC_DataBuffer[0];
			FormatATrack_CHRN.H = FDC_DataBuffer[1];
			FormatATrack_CHRN.R = FDC_DataBuffer[2];
			FormatATrack_CHRN.N = FDC_DataBuffer[3];

			/* write sector */
			/* fdc.CommandBytes[2] = N */ 
			/* fdc.CommandBytes[3] = SC */
			/* fdc.CommandBytes[4] = GPL */
			/* fdc.CommandBytes[5] = D */

			/* add sector to track */
			DiskImage_AddSector(
				fdc.PhysicalDrive, 
				fdc.PhysicalTrack,
				fdc.PhysicalSide, &FormatATrack_CHRN, fdc.CommandBytes[5]);

			/* next sector */
			fdc.SectorCounter++;

			fdc.CommandState--;	
		
			FDC_SetupDataRequestDelay(fdc.StoredDelay + 12 + 3 + 1);
		}
		break;

		case 4:
		{
			/* setup result data */
			FDC_SetStatus0();

			fdc.CommandBytes[6] = FormatATrack_CHRN.N;
			fdc.CommandBytes[5] = (FormatATrack_CHRN.R+1) & 0x0ff;
			fdc.CommandBytes[4] = FormatATrack_CHRN.H;
			fdc.CommandBytes[3] = FormatATrack_CHRN.C;
			fdc.CommandBytes[0] = fdc.ST0;
			fdc.CommandBytes[1] = fdc.ST1;
			fdc.CommandBytes[2] = fdc.ST2;
			
			FDC_SetupResultPhase(7);

			fdc.Flags &= ~NEC765_FLAGS_INTERRUPT;


		}
		break;
	}
}

/******************************************************************************************/

int	FDC_GetSector(void)
{
	int SPT;
	int IndexCount;
	int Side,Track;

	SPT = DiskImage_GetSectorsPerTrack(
		fdc.PhysicalDrive,
		fdc.PhysicalTrack,
		fdc.PhysicalSide);

	if (SPT==0)
	{
		fdc.ST1 |= FDC_ST1_NO_DATA;

		return -1;
	}


	IndexCount = 0;


	if (fdc.drive->CurrentIDIndex>=SPT)
	{
		fdc.drive->CurrentIDIndex-=SPT;
		IndexCount++;
	}

	
	do
	{
		FDC_CHRN	CHRN;
		int CurrentID;

		CurrentID = fdc.drive->CurrentIDIndex;

		DiskImage_GetID(
			fdc.PhysicalDrive,
			fdc.PhysicalTrack,
			fdc.PhysicalSide,
			CurrentID,&CHRN);

		fdc.drive->CurrentIDIndex++;
		
		if (fdc.drive->CurrentIDIndex>=SPT)
		{
			fdc.drive->CurrentIDIndex-=SPT;
			IndexCount++;

			if (IndexCount==2)
			{
				break;
			}
		}

		if (CHRN.R == fdc.CommandBytes[4])
		{
			/* found sector with id we want */

			if (CHRN.C == fdc.CommandBytes[2])
			{
				/* C value the same */

				if ((CHRN.H == fdc.CommandBytes[3]) && (CHRN.N == fdc.CommandBytes[5]))
				{
/*					fdc.ST0 = 0;
					fdc.ST1 |= FDC_ST1_END_OF_CYLINDER;
					fdc.ST2 = 0;
*/			
					fdc.ST1 = CHRN.ST1;
					fdc.ST2 = CHRN.ST2;
					return CurrentID;
				}
			}
			else
			{
				fdc.ST2 |= FDC_ST2_WRONG_CYLINDER;
				return -1;
			}
		}
	}
	while (1==1);

/*	fdc.ST0 = 0;  */
	fdc.ST1 |= FDC_ST1_NO_DATA;
/*	fdc.ST2 = 0; */
	return -1;
}
#if 0
int	FDC_GetSector(int Drive)
{
	int SPT;
	int	i;

	SPT = DiskImage_GetSectorsPerTrack(Drive,DriveStatus[Drive].CurrentTrack,fdc.CurrentSide^DriveStatus[fdc.CurrentDrive].PhysicalSide);

	if (SPT==0)
	{
		/* no sectors. */

/*		fdc.ST0 = 0; */
		fdc.ST1 |= FDC_ST1_NO_DATA;
/*		fdc.ST2 = 0; */

		return -1;
	}
	for (i=0; i<SPT; i++)
	{
		FDC_CHRN	CHRN;

		DiskImage_GetID(Drive,DriveStatus[Drive].CurrentTrack,fdc.CurrentSide^DriveStatus[fdc.CurrentDrive].PhysicalSide,i,&CHRN);

		if (CHRN.R == fdc.CommandBytes[4])
		{
			/* found sector with id we want */

			if (CHRN.C == fdc.CommandBytes[2])
			{
				/* C value the same */

				if ((CHRN.H == fdc.CommandBytes[3]) && (CHRN.N == fdc.CommandBytes[5]))
				{
				
			/*		fdc.ST0 = 0; 
					fdc.ST1 |= FDC_ST1_END_OF_CYLINDER;
					fdc.ST2 = 0;
			*/
					fdc.ST1 = CHRN.ST1;
					fdc.ST2 = CHRN.ST2;
					return i;
				}
			}
			else
			{
				fdc.ST2 |= FDC_ST2_WRONG_CYLINDER;
				return -1;
			}
		}
	}

/*	fdc.ST0 = 0; */
	fdc.ST1 |= FDC_ST1_NO_DATA;
/*	fdc.ST2 = 0; */
	return -1;
}
#endif
/*************************************************************************/
#if 0
void	FDC_ReadDeletedDataMiddle(void);

void	FDC_ReadDeletedDataEnd(void)
{
	/* finish whole command */
	FDC_SetStatus0();

	fdc.CommandBytes[6] = fdc.CommandBytes[5];
	fdc.CommandBytes[5] = fdc.CommandBytes[4];
	fdc.CommandBytes[4] = fdc.CommandBytes[3];
	fdc.CommandBytes[3] = fdc.CommandBytes[2];
	fdc.CommandBytes[0] = fdc.ST0;
	fdc.CommandBytes[1] = fdc.ST1;
	fdc.CommandBytes[2] = fdc.ST2;
	
	FDC_SetupResultPhase(7);
	
	fdc.Flags &= ~NEC765_FLAGS_INTERRUPT;
}

void	FDC_ReadDeletedDataMiddle2(void)
{
	if (fdc.ST1 & FDC_ST1_OVERRUN)
	{
		FDC_ReadDeletedDataEnd();
		return;
	}

	if (
	/* SKIP ? */
	((fdc.CommandBytes[0] & FDC_COMMAND_SKIP)==0) &&
	/* DATA ? */
	((fdc.ST2 & FDC_ST2_CONTROL_MARK)==0))
	{
		/* just read a data sector with Skip = 0 */
		fdc.ST2 |= FDC_ST2_CONTROL_MARK;

		/* we want to quit */
		FDC_ReadDeletedDataEnd();
		return;
	}

	fdc.ST2 &= ~FDC_ST2_CONTROL_MARK;

	if ((fdc.ST1 & FDC_ST1_MISSING_ADDRESS_MARK) ||
		(fdc.ST2 & FDC_ST2_DATA_ERROR_IN_DATA_FIELD) ||
		(fdc.ST2 & FDC_ST2_MISSING_ADDRESS_MARK_IN_DATA_FIELD))
	{
		FDC_ReadDeletedDataEnd();
		return;
	}

	/* if we haven't read the last sector */
	if (fdc.CommandBytes[4] != fdc.CommandBytes[6])
	{
		/* next sector please */
		fdc.CommandBytes[4]++;
	
		/* go to attempt to read it */
		FDC_ReadDeletedDataMiddle();
		return;
	}

	/* have read last sector */
	fdc.ST1 |= FDC_ST1_END_OF_CYLINDER;

	FDC_ReadDeletedDataEnd();
}

void	FDC_ReadDeletedDataMiddle(void)
{
	FDD *drive = FDD_GetDrive(fdc.CurrentDrive);
	int	SectorIndex;

	/* transfer data */
	do
	{
		/* get index of sector data */
		SectorIndex = FDC_GetSector(fdc.CurrentDrive);

		if (SectorIndex!=-1)
		{
			int CurrentSide,CurrentTrack;
			FDC_CHRN	CHRN;

			CurrentTrack = drive->CurrentTrack;
			CurrentSide = drive->PhysicalSide^fdc.CurrentSide;

			/* got sector data */
			DiskImage_GetSector(fdc.CurrentDrive,CurrentTrack,CurrentSide,SectorIndex,FDC_DataBuffer);
			
			DiskImage_GetID(fdc.CurrentDrive,CurrentTrack,CurrentSide,SectorIndex,&CHRN);

/*			fdc.ST1 = CHRN.ST1;
			fdc.ST2 = CHRN.ST2;
*/

			/* If skip=0, and sector contains data, then transfer data to CPU
			 If skip=1, and sector contains data, then dont transfer data to CPU
			 If skip=0, and sector contains deleted data, then transfer data to CPU
			 If skip=1, and sector contains deleted data, then transfer to CPU
			*/

			/* if SKIP=1 and data then do not transfer */
			if  /* SKIP? */
				(!(
				((fdc.CommandBytes[0] & FDC_COMMAND_SKIP)!=0) &&
				/* DATA? */
				((fdc.ST2 & FDC_ST2_CONTROL_MARK)==0)
				))
			{
				
				FDC_SetupReadExecutionPhase(FDC_GetSectorSize(CHRN.N),FDC_DataBuffer);

				return;
			}
			else
			{
				if (fdc.CommandBytes[4]!=fdc.CommandBytes[6])
				{
					fdc.CommandBytes[4]++;
				}
				else
				{
					/* have read last sector */
					fdc.ST1 |= FDC_ST1_END_OF_CYLINDER;

					FDC_ReadDeletedDataEnd();

					return;
				}
			}
		}
		else
		{
			/* error finding sector */
			FDC_ReadDeletedDataEnd();
			return;
		}
	}
	while (1==1);
}
#endif


void	FDC_ReadDeletedData(int State)
{
	switch (State)
	{
		case 0:
		{
			FDC_GetDriveAndSide();
		
			FDC_ClearStatusRegisters();

			/* drive ready ? */
			if (!(fdc.drive->Flags & FDD_FLAGS_DISK_PRESENT))
			{
				/* not ready */

				fdc.ST0 |= FDC_ST0_NOT_READY;
			
				fdc.CommandState = 4;
				return;
			}

			fdc.CommandState++;
		}
		break;

		case 1:
		{
			int	SectorIndex;

			/* transfer data */
			do
			{
				/* get index of sector data */
				SectorIndex = FDC_GetSector();

				if (SectorIndex!=-1)
				{
					FDC_CHRN	CHRN;


					/* got sector data */
					DiskImage_GetSector(
						fdc.PhysicalDrive,
						fdc.PhysicalTrack,
						fdc.PhysicalSide,
						SectorIndex,FDC_DataBuffer);
					
					DiskImage_GetID(
						fdc.PhysicalDrive, 
						fdc.PhysicalTrack,
						fdc.PhysicalSide,
						SectorIndex,&CHRN);

		/*			fdc.ST1 = CHRN.ST1;
					fdc.ST2 = CHRN.ST2;
*/

					/* If skip=0, and sector contains data, then transfer data to CPU
					 If skip=1, and sector contains data, then dont transfer data to CPU
					 If skip=0, and sector contains deleted data, then transfer data to CPU
					 If skip=1, and sector contains deleted data, then transfer to CPU
					*/

					/* if SKIP=1 and data then do not transfer */
					if  /* SKIP? */
						(!(
						((fdc.CommandBytes[0] & FDC_COMMAND_SKIP)!=0) &&
						/* DATA? */
						((fdc.ST2 & FDC_ST2_CONTROL_MARK)==0)
						))
					{
						fdc.CommandState++;

						FDC_SetupReadExecutionPhase(FDC_GetSectorSize(CHRN.N),FDC_DataBuffer);

						return;
					}
					else
					{
						if (fdc.CommandBytes[4]!=fdc.CommandBytes[6])
						{
							fdc.CommandBytes[4]++;
						}
						else
						{
							/* have read last sector */
 							fdc.ST1 |= FDC_ST1_END_OF_CYLINDER;

							fdc.CommandState = 4;	

							return;
						}
					}
				}
				else
				{
					/* error finding sector */
					fdc.CommandState = 4;
					return;
				}
			}
			while (1==1);
		}
		break;
	
		case 2:
		{
			if (fdc.ST1 & FDC_ST1_OVERRUN)
			{
				fdc.CommandState = 4;	
				return;
			}

			if (
			/* SKIP ? */
			((fdc.CommandBytes[0] & FDC_COMMAND_SKIP)==0) &&
			/* DATA ? */
			((fdc.ST2 & FDC_ST2_CONTROL_MARK)==0))
			{
				/* just read a data sector with Skip = 0 */
				fdc.ST2 |= FDC_ST2_CONTROL_MARK;

				/* we want to quit */
				fdc.CommandState = 4;
				return;
			}

			fdc.ST2 &= ~FDC_ST2_CONTROL_MARK;

			if ((fdc.ST1 & FDC_ST1_MISSING_ADDRESS_MARK) ||
				(fdc.ST2 & FDC_ST2_DATA_ERROR_IN_DATA_FIELD) ||
				(fdc.ST2 & FDC_ST2_MISSING_ADDRESS_MARK_IN_DATA_FIELD))
			{
				fdc.CommandState = 4;	
				return;
			}

			/* if we haven't read the last sector */
			if (fdc.CommandBytes[4] != fdc.CommandBytes[6])
			{
				/* next sector please */
				fdc.CommandBytes[4]++;
			
				/* go to attempt to read it */
				fdc.CommandState = 1;	
				return;
			}

			/* have read last sector */
			fdc.ST1 |= FDC_ST1_END_OF_CYLINDER;

			fdc.CommandState = 4;
		}
		break;
		case 4:
		{
			/* finish whole command */
			FDC_SetStatus0();

			fdc.CommandBytes[6] = fdc.CommandBytes[5];
			fdc.CommandBytes[5] = fdc.CommandBytes[4];
			fdc.CommandBytes[4] = fdc.CommandBytes[3];
			fdc.CommandBytes[3] = fdc.CommandBytes[2];
			fdc.CommandBytes[0] = fdc.ST0;
			fdc.CommandBytes[1] = fdc.ST1;
			fdc.CommandBytes[2] = fdc.ST2;
			
			FDC_SetupResultPhase(7);
			
			fdc.Flags &= ~NEC765_FLAGS_INTERRUPT;
		}
		break;
	}
}

/********************************************************************************************/


void	FDC_ReadID(int State)
{
	switch (State)
	{
		case 0:
		{
			unsigned long NopTime = 0;
		
			FDC_CHRN	CHRN;

			CHRN.C = 0;
			CHRN.H = 0;
			CHRN.R = 0;
			CHRN.N = 0;

			/* clear status reg */
			FDC_ClearStatusRegisters();

			/* get drive and side */
			FDC_GetDriveAndSide();

			/* drive ready ? */
			if (fdc.drive->Flags & FDD_FLAGS_DISK_PRESENT)
			{
				int SPT;

			
				NopTime = FDC_GetNopsToNextID();
			
				/* get sectors per track */
				SPT = DiskImage_GetSectorsPerTrack(
					fdc.PhysicalDrive,
					fdc.PhysicalTrack,
					fdc.PhysicalSide);
				
				if (SPT!=0)
				{
					/* id index to get */
					fdc.drive->CurrentIDIndex = fdc.drive->CurrentIDIndex % SPT;
				
					/* get the ID */
					DiskImage_GetID(fdc.PhysicalDrive,
						fdc.PhysicalTrack,
						fdc.PhysicalSide,
						fdc.drive->CurrentIDIndex,&CHRN);

					/* increment for next ID index */
					fdc.drive->CurrentIDIndex++;
			
					fdc.ST1 = 0;
					fdc.ST2 = 0;

				}
				else
				{

					CHRN.C = fdc.PCN[fdc.PhysicalDrive];
					CHRN.H = 0;
					CHRN.R = 1;
					CHRN.N = 2;
					
					/* no sectors */
		/*			fdc.ST0 = 0;  */
					fdc.ST1 |= FDC_ST1_NO_DATA;
		/*			fdc.ST2 = 0; */
				}
			
			}
			else
			{
				fdc.ST0 |= FDC_ST0_NOT_READY;
			}
		fdc.CommandState++;

		
		FDC_SetStatus0();
		fdc.CommandBytes[0] = fdc.ST0;
		fdc.CommandBytes[1] = fdc.ST1;
		fdc.CommandBytes[2] = fdc.ST2;
		fdc.CommandBytes[3] = CHRN.C & 0x0ff;
		fdc.CommandBytes[4] = CHRN.H & 0x0ff;
		fdc.CommandBytes[5] = CHRN.R & 0x0ff;
		fdc.CommandBytes[6] = CHRN.N & 0x0ff;

		FDC_SetupDataRequestDelay(NopTime);


		}
		break;

		case 1:
		{
			FDC_SetupResultPhase(7);

			fdc.Flags &= ~NEC765_FLAGS_INTERRUPT;


		}
		break;


		default:
			break;
		}

}


/********************************************************************************************/

#if 0
void	FDC_WriteDeletedData(int State)
{
}
#endif

/* drive bits in Main Status Register
are cleared *only* when a sense interrupt
status command is executed, and a interrupt
has been caused by the seek command 


- If there is no interrupt, then invalid is returned. In this case
a single byte is returned in the result phase. (0x080).
- If there is a interrupt, 2 bytes are returned. First is ST0, second
is PCN.


*/

void	FDC_SenseInterruptStatus(int State)
{
	if (fdc.Flags & NEC765_FLAGS_INTERRUPT)
	{
		/* clear drive busy bits */
		fdc.MainStatusRegister &= ~(1|2|4|8);

		/* clear interrupt. */
		fdc.Flags &=~NEC765_FLAGS_INTERRUPT;

		/* store st0 */
		fdc.CommandBytes[0] = fdc.ST0;
		fdc.CommandBytes[1] = fdc.PCN[fdc.ST0 & 0x03];
		FDC_SetupResultPhase(2);
	}
	else
	{
		fdc.CommandBytes[0] = 0x080;
		FDC_SetupResultPhase(1);
	}

}




#if 0
void	FDC_ReadDataMiddle2(void)
{
	if (fdc.ST1 & FDC_ST1_OVERRUN)
	{
		FDC_ReadDataEnd();
		return;
	}

	/* If skip=0, and sector contained normal data, then continue
	 If skip=1, and sector contained normal data, then continue
	 If skip=0, and sector contained deleted data, then quit
	 If skip=1, and sector contained deleted data, then continue.
	*/
	if (
		/* SKIP? */
		((fdc.CommandBytes[0] & FDC_COMMAND_SKIP)==0) && 
		/* DELETED DATA? */
		((fdc.ST2 & FDC_ST2_CONTROL_MARK)!=0)
	)
	{
		/* read a deleted data sector with SKIP = 0 */
		fdc.ST2 |= FDC_ST2_CONTROL_MARK;

		FDC_ReadDataEnd();
		return;
	}

	/* error ? */
	if (
	   (fdc.ST1 & FDC_ST1_MISSING_ADDRESS_MARK) || 
	   (fdc.ST2 & FDC_ST2_DATA_ERROR_IN_DATA_FIELD) || 
	   (fdc.ST2 & FDC_ST2_MISSING_ADDRESS_MARK_IN_DATA_FIELD)
	   )
	{
		/* we want to quit */
		FDC_ReadDataEnd();
		return;
	}

	/* if we haven't read the last sector */
	if (fdc.CommandBytes[4] != fdc.CommandBytes[6])
	{
		/* next sector please */
		fdc.CommandBytes[4]++;
	
		/* go to attempt to read it */
		FDC_ReadDataMiddle();
		return;
	}

	/* have read last sector */
	fdc.ST1 |= FDC_ST1_END_OF_CYLINDER;

	FDC_ReadDataEnd();
}
#endif
#if 0
void	FDC_ReadDataMiddle(void)
{
	int	SectorIndex;
	FDD *drive = FDD_GetDrive(fdc.CurrentDrive);
	
	/* transfer data */
	do
	{
		/* get index of sector data */
		SectorIndex = FDC_GetSector(fdc.CurrentDrive);

		if (SectorIndex!=-1)
		{
			int CurrentTrack, CurrentSide;
			FDC_CHRN	CHRN;
			
			CurrentTrack = drive->CurrentTrack;
			CurrentSide = drive->PhysicalSide^fdc.CurrentSide;

			DiskImage_GetSector(fdc.CurrentDrive,CurrentTrack,CurrentSide,SectorIndex,FDC_DataBuffer);

			DiskImage_GetID(fdc.CurrentDrive,CurrentTrack,CurrentSide,SectorIndex,&CHRN);
			


			/* If skip=0, and sector contains data, then transfer data to CPU
			 If skip=1, and sector contains data, then transfer data to CPU
			 If skip=0, and sector contains deleted data, then transfer data to CPU
			 If skip=1, and sector contains deleted data, then do not transfer.
			*/

			/* if SKIP=1 and Deleted data then do not transfer */
			if  (!(
				((fdc.CommandBytes[0] & FDC_COMMAND_SKIP)!=0) && 
				((fdc.ST2 & FDC_ST2_CONTROL_MARK)!=0)
				))
			{

				FDC_SetupReadExecutionPhase(FDC_GetSectorSize(CHRN.N),FDC_DataBuffer);

				return;
			}
			else
			{
				if (fdc.CommandBytes[4]!=fdc.CommandBytes[6])
				{
					fdc.CommandBytes[4]++;
				}
				else
				{
					/* have read last sector */
					fdc.ST1 |= FDC_ST1_END_OF_CYLINDER;

					FDC_ReadDataEnd();
					return;
				}
			}
		}
		else
		{
			/* error finding sector */
			FDC_ReadDataEnd();
			return;
		}
	}
	while (1==1);
}
#endif


void	FDC_ReadData(int State)
{
	switch (State)
	{
		case 0:
		{
			FDC_GetDriveAndSide();

			FDC_ClearStatusRegisters();
			
			if (!(fdc.drive->Flags & FDD_FLAGS_DISK_PRESENT))
			{
				/* not ready */

				fdc.ST0 = FDC_ST0_NOT_READY;
			
				fdc.CommandState = 4;
				return;
			}

			fdc.CommandState++;
		}
		break;

		case 1:
		{
			int	SectorIndex;
		
			/* transfer data */
			do
			{
				/* get index of sector data */
				SectorIndex = FDC_GetSector();

				if (SectorIndex!=-1)
				{
					FDC_CHRN	CHRN;
					

					DiskImage_GetSector(
						fdc.PhysicalDrive, 
						fdc.PhysicalTrack,
						fdc.PhysicalSide,
						SectorIndex,FDC_DataBuffer);

					DiskImage_GetID(
						fdc.PhysicalDrive,
						fdc.PhysicalTrack,
						fdc.PhysicalSide,
						SectorIndex,&CHRN);
					

					/* If skip=0, and sector contains data, then transfer data to CPU
					 If skip=1, and sector contains data, then transfer data to CPU
					 If skip=0, and sector contains deleted data, then transfer data to CPU
					 If skip=1, and sector contains deleted data, then do not transfer.
					*/

					/* if SKIP=1 and Deleted data then do not transfer */
					if  (!(
						((fdc.CommandBytes[0] & FDC_COMMAND_SKIP)!=0) && 
						((fdc.ST2 & FDC_ST2_CONTROL_MARK)!=0)
						))
					{
						
						fdc.CommandState++;

						FDC_SetupReadExecutionPhase(FDC_GetSectorSize(CHRN.N),FDC_DataBuffer);



						return;
					}
					else
					{
						if (fdc.CommandBytes[4]!=fdc.CommandBytes[6])
						{
							fdc.CommandBytes[4]++;
						}
						else
						{
							/* have read last sector */
							fdc.ST1 |= FDC_ST1_END_OF_CYLINDER;

							fdc.CommandState = 4;	
							return;
						}
					}
				}
				else
				{
					/* error finding sector */
					fdc.CommandState = 4;	
					return;
				}
			}
			while (1==1);
	
		}
		break;

		case 2:
		{
			if (fdc.ST1 & FDC_ST1_OVERRUN)
			{
				fdc.CommandState = 4;	
				return;
			}

			/* If skip=0, and sector contained normal data, then continue
			 If skip=1, and sector contained normal data, then continue
			 If skip=0, and sector contained deleted data, then quit
			 If skip=1, and sector contained deleted data, then continue.
			*/
			if (
				/* SKIP? */
				((fdc.CommandBytes[0] & FDC_COMMAND_SKIP)==0) && 
				/* DELETED DATA? */
				((fdc.ST2 & FDC_ST2_CONTROL_MARK)!=0)
			)
			{
				/* read a deleted data sector with SKIP = 0 */
				fdc.ST2 |= FDC_ST2_CONTROL_MARK;

				fdc.CommandState = 4;	
				return;
			}

			/* error ? */
			if (
			   (fdc.ST1 & FDC_ST1_MISSING_ADDRESS_MARK) || 
			   (fdc.ST2 & FDC_ST2_DATA_ERROR_IN_DATA_FIELD) || 
			   (fdc.ST2 & FDC_ST2_MISSING_ADDRESS_MARK_IN_DATA_FIELD)
			   )
			{
				/* we want to quit */
				fdc.CommandState = 4;	
				return;
			}

			/* if we haven't read the last sector */
			if (fdc.CommandBytes[4] != fdc.CommandBytes[6])
			{
				/* next sector please */
				fdc.CommandBytes[4]++;
			
				/* go to attempt to read it */
				fdc.CommandState = 1;	
				return;
			}

			/* have read last sector */
			fdc.ST1 |= FDC_ST1_END_OF_CYLINDER;

			fdc.CommandState = 4;	

		}
		break;



		case 4:
		{
			/* finish whole command */
			FDC_SetStatus0();

			fdc.CommandBytes[6] = fdc.CommandBytes[5];
			fdc.CommandBytes[5] = fdc.CommandBytes[4];
			fdc.CommandBytes[4] = fdc.CommandBytes[3];
			fdc.CommandBytes[3] = fdc.CommandBytes[2];
			
			if (fdc.ST1 & FDC_ST1_NO_DATA)
			{
				fdc.CommandBytes[4] = 0;
				fdc.CommandBytes[5] = 1;
				fdc.CommandBytes[6] = 2;
			}

			fdc.CommandBytes[0] = fdc.ST0;
			fdc.CommandBytes[1] = fdc.ST1;
			fdc.CommandBytes[2] = fdc.ST2;

			FDC_SetupResultPhase(7);

			/* read-data clears interrupt so that sense interrupt status will
			return 0x080 after read */
			fdc.Flags &= ~NEC765_FLAGS_INTERRUPT;
		}
		break;
	}

}



/********************************************************************************************/
#if 0
void	FDC_WriteDataMiddle(void);

void	FDC_WriteDataEnd(void)
{
	/* finish whole command */
	FDC_SetStatus0();

	fdc.CommandBytes[6] = fdc.CommandBytes[5];
	fdc.CommandBytes[5] = fdc.CommandBytes[4];
	fdc.CommandBytes[4] = fdc.CommandBytes[3];
	fdc.CommandBytes[3] = fdc.CommandBytes[2];
	fdc.CommandBytes[0] = fdc.ST0;
	fdc.CommandBytes[1] = fdc.ST1;
	fdc.CommandBytes[2] = fdc.ST2;
	
	FDC_SetupResultPhase(7);

	fdc.Flags &= ~NEC765_FLAGS_INTERRUPT;
}

void	FDC_WriteDataMiddle2(void)
{
	int SectorIndex;
	FDD *drive;

	drive = FDD_GetDrive(fdc.CurrentDrive);

	/* get index of sector data */
	SectorIndex = FDC_GetSector(fdc.CurrentDrive);

	if (SectorIndex!=-1)
	{
		int CurrentTrack, CurrentSide;

		CurrentTrack = drive->CurrentTrack;
		CurrentSide = drive->PhysicalSide^fdc.CurrentSide;

		DiskImage_PutSector(fdc.CurrentDrive,CurrentTrack,CurrentSide,SectorIndex,FDC_DataBuffer);
	}

	/* next sector please */
	fdc.CommandBytes[4]++;
	
	/* go to attempt to write it */
	FDC_WriteDataMiddle();
}

void	FDC_WriteDataMiddle(void)
{
	FDD *drive;
	int	SectorIndex;

	drive = FDD_GetDrive(fdc.CurrentDrive);

	/* transfer data */
	while (fdc.CommandBytes[4] <= fdc.CommandBytes[6])
	{
		/* get index of sector data */
		SectorIndex = FDC_GetSector(fdc.CurrentDrive);

		if (SectorIndex!=-1)
		{
			int CurrentTrack, CurrentSide;

			

			FDC_CHRN	CHRN;

			CurrentTrack = drive->CurrentTrack;
			CurrentSide = drive->PhysicalSide^fdc.CurrentSide;

			DiskImage_GetID(fdc.CurrentDrive,CurrentTrack,CurrentSide,SectorIndex,&CHRN);

			FDC_SetupWriteExecutionPhase(FDC_GetSectorSize(CHRN.N),FDC_DataBuffer);
		
			return;
		}
		else
		{
			/* error finding sector */
			FDC_WriteDataEnd();
			return;
		}
	}

	FDC_WriteDataEnd();
}
#endif

void	FDC_WriteData(int State)
{
	switch (State)
	{
		case 0:
		{
			FDC_GetDriveAndSide();

			FDC_ClearStatusRegisters();

			if (!(fdc.drive->Flags & FDD_FLAGS_DISK_PRESENT))
			{
				/* not ready */

				fdc.ST0 = FDC_ST0_NOT_READY;
				fdc.ST1 = 0;
				fdc.ST2 = 0;
			
				fdc.CommandState = 4;
				return;
			}

			fdc.CommandState++;	
		}
		break;

		case 1:
		{
			int	SectorIndex;

			/* transfer data */
			while (fdc.CommandBytes[4] <= fdc.CommandBytes[6])
			{
				/* get index of sector data */
				SectorIndex = FDC_GetSector();

				if (SectorIndex!=-1)
				{
					FDC_CHRN	CHRN;


					DiskImage_GetID(
						fdc.PhysicalDrive,
						fdc.PhysicalTrack,
						fdc.PhysicalSide,
						SectorIndex,&CHRN);
			
					fdc.CommandState++;
					FDC_SetupWriteExecutionPhase(FDC_GetSectorSize(CHRN.N),FDC_DataBuffer);
				
					return;
				}
				else
				{
					/* error finding sector */
					fdc.CommandState = 4;	
					return;
				}
			}

			fdc.CommandState = 4;	
		}
		break;

		case 2:
		{
			int SectorIndex;
		
			/* get index of sector data */
			SectorIndex = FDC_GetSector();

			if (SectorIndex!=-1)
			{
				DiskImage_PutSector(
					fdc.PhysicalDrive,
					fdc.PhysicalTrack,
					fdc.PhysicalSide,
					SectorIndex,FDC_DataBuffer);
			}

			/* next sector please */
			fdc.CommandBytes[4]++;
			
			/* go to attempt to write it */
			fdc.CommandState = 1;	
		}
		break;

		case 4:
		{
			/* finish whole command */
			FDC_SetStatus0();

			fdc.CommandBytes[6] = fdc.CommandBytes[5];
			fdc.CommandBytes[5] = fdc.CommandBytes[4];
			fdc.CommandBytes[4] = fdc.CommandBytes[3];
			fdc.CommandBytes[3] = fdc.CommandBytes[2];
			fdc.CommandBytes[0] = fdc.ST0;
			fdc.CommandBytes[1] = fdc.ST1;
			fdc.CommandBytes[2] = fdc.ST2;
			
			FDC_SetupResultPhase(7);

			fdc.Flags &= ~NEC765_FLAGS_INTERRUPT;

		}
		break;

	}
}


void	FDC_SenseDriveStatus(int State)
{
	int FDC_ST3;

	FDC_GetDriveAndSide();

	FDC_ST3 = 0;

	if (fdc.drive->Flags & FDD_FLAGS_DRIVE_ACTIVE)
	{
		/* drive is active */

		/* get status of drive - interested only in Write Protect,
		Ready and Track 0 */

		if (fdc.drive->Flags & FDD_FLAGS_DISK_PRESENT)
		{
			/* disk is present */

			FDC_ST3 |= FDC_ST3_READY;

			/* write protected? */
			if (fdc.drive->Flags & FDD_FLAGS_WRITE_PROTECTED)
			{
				FDC_ST3 |= FDC_ST3_WRITE_PROTECTED;
			}

			/* track 0? */
			if (fdc.drive->Flags & FDD_FLAGS_HEAD_AT_TRACK_0)
			{
				FDC_ST3 |= FDC_ST3_TRACK_0;
			}
			
		}
		else
		{
			/* disk is not present */

			FDC_ST3 |= FDC_ST3_WRITE_PROTECTED;
		}

		/* if not drive 0 - internal 3" drive */
		if (fdc.PhysicalDrive!=0)
		{	
			/* say it's two side */
			FDC_ST3 |= FDC_ST3_TWO_SIDE;
		}
	}

	/* if drive is inactive, only the drive and side is set */

	/* Sense Drive Status sets drive and side */
	FDC_ST3_SET_DRIVE_AND_SIDE;

	fdc.CommandBytes[0] = FDC_ST3;
	
	FDC_SetupResultPhase(1);

	fdc.Flags &= ~NEC765_FLAGS_INTERRUPT;
}

#define MILLISECONDS_TO_MICROSECONDS(x)	(x*1000)

void	FDC_Specify(int State)
{
	int SRT;

	SRT = 15-((fdc.CommandBytes[1]>>4) & 0x0f);

	fdc.StepTime = MILLISECONDS_TO_MICROSECONDS(SRT);

	FDC_Standby();
}


void	FDC_Invalid(int State)
{
	fdc.CommandBytes[0] = 0x080;

	FDC_SetupResultPhase(1);
}

void	FDC_ClearStatusRegisters(void)
{
	fdc.ST0 = fdc.ST1 = fdc.ST2 = 0;
}

/* set FDC Status Register Interrupt Code, based on the values 
currently stored in ST0, ST1 and ST2 */
void	FDC_SetStatus0(void)
{	
	
	if ((
		/* ST0 bits that specify an error */
		(fdc.ST0 & (FDC_ST0_EQUIPMENT_CHECK | 
					FDC_ST0_NOT_READY)) 	|
		/* ST1 bits that specify an error */
		(fdc.ST1 & (FDC_ST1_MISSING_ADDRESS_MARK |
					FDC_ST1_NOT_WRITEABLE |
					FDC_ST1_NO_DATA |
					FDC_ST1_OVERRUN |
					FDC_ST1_DATA_ERROR |
					FDC_ST1_END_OF_CYLINDER))	|
		/* ST2 bits that specify an error */		
		(fdc.ST2 & (FDC_ST2_CONTROL_MARK | 
					FDC_ST2_DATA_ERROR_IN_DATA_FIELD |
					FDC_ST2_WRONG_CYLINDER |
					FDC_ST2_SCAN_EQUAL_HIT |
					FDC_ST2_SCAN_NOT_SATISFIED |
					FDC_ST2_BAD_CYLINDER |
					FDC_ST2_MISSING_ADDRESS_MARK_IN_DATA_FIELD))
		)!=0)
	{
		if (fdc.ST0 & FDC_ST0_NOT_READY)
		{
			fdc.ST0 |= 0x0c0;
		}
		else
		{
			fdc.ST0 |= 0x040;
		}
	}
	else
	{
		fdc.ST0 &= 0x03f;
	}

	/* include drive and side details */
	fdc.ST0 |= (fdc.CurrentDrive | (fdc.CurrentSide<<2));
}

void	FDC_SetDriveStatus(int Drive)
{
}

/* insert or remove a disk from a drive */
void	FDD_InsertDisk(int Drive,BOOL Status)
{
	FDD *drive = FDD_GetDrive(Drive);

	/* say disk is or isn't present */
	drive->Flags &=~FDD_FLAGS_DISK_PRESENT;
	
	if (Status)
	{
		drive->Flags |= FDD_FLAGS_DISK_PRESENT;
	}
	/* set appropiate drive status */

	FDC_SetDriveStatus(Drive);

	/* setup initial parameters for when a disk is present */
	drive->CurrentIDIndex = 0;

}

BOOL	FDD_IsDiskPresent(int Drive)
{
	return FDD_GetDrive(Drive)->Flags & FDD_FLAGS_DISK_PRESENT;
}


/* turn disk in the drive */
void	FDD_TurnDisk(int Drive)
{
	FDD *drive = FDD_GetDrive(Drive);

	drive->PhysicalSide^=0x01;
}

void	FDD_Initialise(int Drive)
{
	FDD *drive = FDD_GetDrive(Drive);

	/* set default side */
	drive->PhysicalSide = 0;

	drive->Flags |= FDD_FLAGS_DRIVE_ACTIVE;
	drive->Flags &= ~(FDD_FLAGS_DISK_PRESENT | FDD_FLAGS_WRITE_PROTECTED);

}

static int	FDC_GetSectorSize(int N)
{
	if (N<7)
		return (1<<N)<<7;	
	else
		return 0x0200;
}


