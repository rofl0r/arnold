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

#include "cpcglob.h"
#include "host.h"

/* floppy disc controller */
#include "fdc.h"

/* floppy disc drive */
#include "fdd.h"
/* floppy disc interface */
#include "fdi.h"

#include "cpc.h"


#define DATA_RATE_US 32
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
static void	FDC_UpdateState(void);
static void	FDC_SetDriveStatus(int);


/* commands */
static void	FDC_Invalid(int);
static void	FDC_ReadATrack(int);
static void	FDC_Specify(int);
static void	FDC_SenseDriveStatus(int);
static void	FDC_SenseInterruptStatus(int);
static void	FDC_WriteData(int);
static void	FDC_ReadData(int);
static void	FDC_Recalibrate(int);
static void	FDC_ReadID(int);
static void	FDC_FormatATrack(int);
static void	FDC_Seek(int);
static void FDC_Scan(int);



/* floppy disc controller */
static NEC765 fdc;

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

static char	FDC_DataBuffer[32768];

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
	{9-1,FDC_ReadData},		/* %01100 READ DELETED DATA */
	{6-1,FDC_FormatATrack},			/* %01101 FORMAT A TRACK */
	{1-1,FDC_Invalid},				/* %01110 ** invalid ** */
	{3-1,FDC_Seek},					/* %01111 SEEK */
	{1-1,FDC_Invalid},				/* %10000 ** invalid ** */
	{9-1,FDC_Scan},				/* %10001 SCAN EQUAL */
	{1-1,FDC_Invalid},				/* %10010 ** invalid ** */
	{1-1,FDC_Invalid},				/* %10011 ** invalid ** */
	{1-1,FDC_Invalid},				/* %10100 ** invalid ** */
	{1-1,FDC_Invalid},				/* %10101 ** invalid ** */
	{1-1,FDC_Invalid},				/* %10110 ** invalid ** */
	{1-1,FDC_Invalid},				/* %10111 ** invalid ** */
	{1-1,FDC_Invalid},				/* %11000 ** invalid ** */
	{9-1,FDC_Scan},				/* %11001 SCAN LOW OR EQUAL */
	{1-1,FDC_Invalid},				/* %11010 ** invalid ** */
	{1-1,FDC_Invalid},				/* %11011 ** invalid ** */
	{1-1,FDC_Invalid},				/* %11100 ** invalid ** */
	{9-1,FDC_Scan},		/* %11101 SCAN HIGH OR EQUAL */
	{1-1,FDC_Invalid},				/* %11110 ** invalid ** */
	{1-1,FDC_Invalid},				/* %11111 ** invalid ** */
};

/*-----------------------------------------------------------------------*/
/* set the side output from the fdc. */
static void FDC_SetSideOutput(unsigned long Value)
{
	fdc.CurrentSide = Value;
	/* adjust physical side selection */
	FDI_SetPhysicalSide(Value);
}

/*-----------------------------------------------------------------------*/
/* set the drive output from the fdc */
static void FDC_SetDriveOutput(unsigned long Value)
{
	fdc.CurrentDrive = Value;

	FDI_SetPhysicalDrive(Value);
}

/*-----------------------------------------------------------------------*/
/* get drive and side from command */
static void FDC_GetDriveAndSide(void)
{
	unsigned char DriveAndSide = fdc.CommandBytes[1];

	/* set drive output */
	FDC_SetDriveOutput(DriveAndSide & 0x03);

	/* set side output */
	FDC_SetSideOutput(((DriveAndSide & FDC_COMMAND_HEAD_ADDRESS)>>2));
}

/*-----------------------------------------------------------------------*/
/* get drive from command (side is forced to 0) */
static void FDC_GetDrive(void)
{
	unsigned char DriveAndSide = fdc.CommandBytes[1];

	FDC_SetDriveOutput(DriveAndSide & 0x03);

	FDC_SetSideOutput(0);
}

/*-----------------------------------------------------------------------*/


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

/*-----------------------------------------------------------------------*/
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

/*-----------------------------------------------------------------------*/
void	FDC_NormalSeekComplete(int DriveIndex)
{
	/* normal termination */
	fdc.ST0 |= FDC_ST0_SEEK_END | FDC_INTERRUPT_CODE_NT;

	/* end seek */
	FDC_SeekComplete(DriveIndex);
}

/*-----------------------------------------------------------------------*/
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
		unsigned long PCN;

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

/*-----------------------------------------------------------------------*/

void	FDC_BeginSeek(void)
{
	int DriveIndex;

	FDC_GetDrive();

	FDC_ClearStatusRegisters();

	/* get drive index */
	DriveIndex = fdc.CommandBytes[1] & 0x03;

	if ((FDI_GetSelectedDriveFlags() & FDD_FLAGS_DRIVE_READY)==0)
	{
		/* not ready */
		if ((fdc.CommandBytes[0] & FDC_COMMAND_WORD)==0x0f)
		{


		}
		else
		{
			fdc.PCN[DriveIndex] = 0;
		}

		fdc.ST0 =  FDC_ST0_NOT_READY|FDC_ST0_SEEK_END |FDC_ST0_INTERRUPT_CODE0;
		FDC_SeekComplete(DriveIndex);
		FDC_Standby();
		return;
	}



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
		unsigned long PCN;

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

/*-----------------------------------------------------------------------*/

void	FDC_DoSeekOperation(int DriveIndex)
{
	/* step drive head */
	FDI_PerformStep(DriveIndex, fdc.StepDirection);

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
/*-----------------------------------------------------------------------*/

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
/*-----------------------------------------------------------------------*/

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

/*-----------------------------------------------------------------------*/

static unsigned long FDC_NopOfLastDataTransfer;

/* clear the low level state */
void	FDC_ClearLowLevelState(void)
{
	fdc.LowLevelState = -1;

//	FDC_UpdateState();
}

/* setup a new low level state */
void	FDC_SetLowLevelState(int NewState)
{
	fdc.LowLevelState = NewState;

//	FDC_UpdateState();

}

void	FDC_PopHighLevelState(void)
{
	fdc.HighLevelState = fdc.PushedHighLevelState;

//	FDC_UpdateState();
}

void	FDC_PushHighLevelState(void)
{
	fdc.PushedHighLevelState = fdc.HighLevelState;
}

void	FDC_SetHighLevelState(int NewState)
{
	fdc.HighLevelState = NewState;

//	FDC_UpdateState();

}

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

	fdc.MainStatusRegister |= FDC_MSR_DATA_REQUEST;
	fdc.DataRegister = pBuffer[0];
	fdc.ExecutionNoOfBytes--;
	fdc.ExecutionBufferByteIndex=1;

	/* setup delay before first data request is issued (first byte is ready) */
	FDC_SetupDataRequestDelay(DATA_RATE_US*8);

	/* set high level state for data transfer */
/*	FDC_SetHighLevelState(NEC765_HIGH_LEVEL_STATE_EXECUTION_PHASE_READ_DATA); */

	fdc.HighLevelState = NEC765_HIGH_LEVEL_STATE_EXECUTION_PHASE_READ_DATA;

}

void	FDC_SetupResultPhase(int NoOfStatusBytes)
{

	/* fdc not in execution phase */
	fdc.MainStatusRegister &= ~(FDC_MSR_EXECUTION_PHASE | FDC_MSR_DATA_REQUEST);
	/* fdc data ready, and direction to cpu */
	fdc.MainStatusRegister |= FDC_MSR_DATA_FLOW_DIRECTION;
	
	fdc.MainStatusRegister |= FDC_MSR_DATA_REQUEST;
	fdc.DataRegister = fdc.CommandBytes[0];

	fdc.CommandBytesRemaining = NoOfStatusBytes-1;
	fdc.CommandByteIndex = 1;

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
			/* set the LED for this disc drive */
			FDD_LED_SetState(fdc.CurrentDrive, TRUE);
		}
		break;
	
		default:
			break;
	}
	
	FDC_SetHighLevelState(NEC765_HIGH_LEVEL_STATE_EXECUTION_PHASE);
	
	fdc.CommandHandler(fdc.CommandState);
}

void	FDC_UpdateStateStatus(void)
{
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
#if 0
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
#endif
		}
		break;

		case NEC765_HIGH_LEVEL_STATE_COMMAND_PHASE_REMAINING_BYTES:
		{
#if 0
			/* data has been written */
			if ((fdc.MainStatusRegister & FDC_MSR_DATA_REQUEST)==0)
			{
				/* store byte */
				fdc.CommandBytes[fdc.CommandByteIndex] = fdc.DataRegister;
				/* store number of bytes written so far */
				fdc.CommandByteIndex++;
				/* decrement number of bytes to be transfered. */
				fdc.CommandBytesRemaining--;

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
#endif
		}
		break;

		case NEC765_HIGH_LEVEL_STATE_EXECUTION_PHASE_READ_DATA:
		{
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
		}
		break;
	
		case NEC765_HIGH_LEVEL_STATE_EXECUTION_PHASE_WRITE_DATA:
		{
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

					/* issue a data request */
					fdc.MainStatusRegister |= FDC_MSR_DATA_REQUEST;
				}
			}

		}
		break;

	}

}


void	FDC_UpdateStateData(void)
{

	switch (fdc.HighLevelState)
	{
		case NEC765_HIGH_LEVEL_STATE_COMMAND_PHASE_FIRST_BYTE:
		{
			/* data has been written */
			if ((fdc.MainStatusRegister & FDC_MSR_DATA_REQUEST)==0)
			{
				unsigned char Data;
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

//		case NEC765_HIGH_LEVEL_STATE_EXECUTION_PHASE:
//		{
//			/* execute command handler */
//			fdc.CommandHandler(fdc.CommandState);
//		}
//		break;

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

					/* issue a data request */
					fdc.MainStatusRegister |= FDC_MSR_DATA_REQUEST;
				}
			}



		}
		break;

	}
}

/* read from main status register */
unsigned int	FDC_ReadMainStatusRegister(void)
{
	unsigned char Data;

	Data = fdc.MainStatusRegister;

	FDC_UpdateDrives();

	/* update current state */
	FDC_UpdateStateStatus();

	/* return current status */
	return Data;
}

/* read from data register */
unsigned int	FDC_ReadDataRegister(void)
{
	unsigned char Data;

	/* clear data request */
	fdc.MainStatusRegister &=~FDC_MSR_DATA_REQUEST;

	/* get data from data register */
	Data = fdc.DataRegister;

	FDC_UpdateDrives();

	FDC_UpdateStateData();

	/* return data */
	return Data;

}

/* - when writing command bytes, data written to port can be read back again */
void	FDC_WriteDataRegister(int Data)
{

	/* clear data request */
	fdc.MainStatusRegister &= ~FDC_MSR_DATA_REQUEST;

	/* set data */
	fdc.DataRegister = Data;

	FDC_UpdateStateData();

}

void	FDC_Standby(void)
{
	/* set data flow to accept commands */
	/* set fdc not busy */
	/* set fdc not in execution phase */
	/* set fdc ready to accept data */
	fdc.MainStatusRegister &= (~(FDC_MSR_DATA_FLOW_DIRECTION|FDC_MSR_BUSY|FDC_MSR_EXECUTION_PHASE));
	fdc.MainStatusRegister |= FDC_MSR_DATA_REQUEST;
	fdc.LowLevelState = -1;
	FDC_SetHighLevelState(NEC765_HIGH_LEVEL_STATE_COMMAND_PHASE_FIRST_BYTE);

	/* disable led */
	FDD_LED_SetState(fdc.CurrentDrive,FALSE);
}

void	FDC_Reset(void)
{
	
	fdc.Flags = 0;
	fdc.PCN[0] = fdc.PCN[1] = fdc.PCN[2] = fdc.PCN[3] = 0;

	FDC_Standby();
}

/*-------------------------------------------------------------*/

BOOL	FDC_LastSector(void)
{
	/* next sector */
	fdc.SectorCounter = (fdc.SectorCounter+1)&0x0ff;

	/* is sector offset == EOT?? */
	if ((fdc.SectorCounter & 0x0ff)== fdc.CommandBytes[6])
	{
		/* multi-track? */
		if (fdc.CommandBytes[0] & 0x080)
		{
			if ((fdc.CommandBytes[3]&1)!=0)
				return TRUE;

			/* set H */
			fdc.CommandBytes[3] |= 1;

			/* swap side */
			FDC_SetSideOutput(fdc.CurrentSide^1);

			/* reset sector counter */
			fdc.SectorCounter = 0;

			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}

	return FALSE;
}

/*-------------------------------------------------------------*/

void	FDC_ReadATrack(int State)
{
	BOOL bUpdateState;

	do
	{
		bUpdateState = FALSE;

		switch (fdc.CommandState)
		{
			case 0:
			{
				CHRN	ReadATrack_CHRN;

				/* reset status registers */
				FDC_ClearStatusRegisters();

				/* get current drive and side */
				FDC_GetDriveAndSide();

				FDI_SetDensity(fdc.CommandBytes[0]&0x040);

				/* drive ready? */
				if ((FDI_GetSelectedDriveFlags() & FDD_FLAGS_DRIVE_READY)==0)
				{
					fdc.SectorCounter = fdc.CommandBytes[4];

					/* not ready */
					fdc.ST0 = FDC_ST0_NOT_READY;

					/* end command */
					fdc.CommandState = 4;
					bUpdateState = TRUE;
					break;
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
				bUpdateState = TRUE;

				/* wait for index pulse */
				while (FDI_GetNextID(&ReadATrack_CHRN)==0);
			}
			break;

			case 1:
			{
				BOOL bSkip = FALSE;
				BOOL bControlMark = FALSE;
				CHRN	ReadATrack_CHRN;

				do
				{
					unsigned long status;

					status = FDI_GetNextID(&ReadATrack_CHRN);

					/* if seen index pulse and sector counter is 0 */
					if ((status!=0) && (fdc.SectorCounter==0))
					{
						fdc.SectorCounter = fdc.CommandBytes[4];

						/* missing address mark */
						fdc.ST1 = FDC_ST1_MISSING_ADDRESS_MARK;
						/* quit */
						fdc.CommandState=4;
						bUpdateState = TRUE;
						break;
					}

					if (status!=0)
					{
						FDI_GetNextID(&ReadATrack_CHRN);
					}

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

					/* READ DATA */

					/* if deleted data mark is detected, set control mark flag */
					if  (
						((ReadATrack_CHRN.ST2 & FDC_ST2_CONTROL_MARK)!=0)
						)
					{
						/* skip and deleted data mark found */
						bControlMark = TRUE;
					}

					if (bControlMark)
					{
						/* read a "deleted data" using "read data" */
						/* read a "data" using "read deleted data" */
						fdc.ST2 |= FDC_ST2_CONTROL_MARK;
					}

					bSkip = FALSE;

					if (
						/* skip? */
						((fdc.CommandBytes[0] & FDC_COMMAND_SKIP)!=0) && 
						/* deleted data mark? */
						((ReadATrack_CHRN.ST2 & FDC_ST2_CONTROL_MARK)!=0)
						)
					{
						bSkip = TRUE;
					}

					if (bSkip)
					{
						if (FDC_LastSector())
						{
							fdc.ST1 |= FDC_ST1_END_OF_CYLINDER;

							/* current offset = EOT. All sectors transfered.
							Finish command */
							fdc.CommandState = 4;
							bUpdateState = TRUE;
							break;
						}
					}
					else
					{
						/* if N of sector is different to N in command
						then a data error will be reported */
						if (ReadATrack_CHRN.N!=fdc.CommandBytes[5])
						{
							fdc.ST2|=FDC_ST2_DATA_ERROR_IN_DATA_FIELD;
							fdc.ST1|=FDC_ST1_DATA_ERROR;
						}
						else
						{
							/* N specified in read track is same as N specified for sector
							*/

							/* does sector have a data error? */
							if (ReadATrack_CHRN.ST2 & FDC_ST2_DATA_ERROR_IN_DATA_FIELD)
							{
								fdc.ST2|=FDC_ST2_DATA_ERROR_IN_DATA_FIELD;
								fdc.ST1|=FDC_ST1_DATA_ERROR;
							}
						}

						FDI_ReadSector(FDC_DataBuffer);

						/* setup for execution phase */
						fdc.CommandState++;
						/* execution phase 2 */
						FDC_SetupReadExecutionPhase(FDC_GetSectorSize(fdc.CommandBytes[5]),FDC_DataBuffer);
						break;
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
					bUpdateState = TRUE;
					break;
				}

				if (FDC_LastSector())
				{
					fdc.ST1 |= FDC_ST1_END_OF_CYLINDER;

					/* current offset = EOT. All sectors transfered.
					Finish command */
					fdc.CommandState = 4;
					bUpdateState = TRUE;
					break;
				}

				
				/* go to attempt to read it */
				fdc.CommandState--;	
				bUpdateState = TRUE;
			}
			break;

			case 4:
			{
				/* setup result data */
				FDC_SetStatus0();

				fdc.CommandBytes[6] = fdc.CommandBytes[5];
				fdc.CommandBytes[5] = fdc.SectorCounter;
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
	while (bUpdateState);
}

/*-------------------------------------------------------------*/


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
/*-------------------------------------------------------------*/


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
		//	FDC_BeginSeek();
		}
		break;
	}
}

/*-------------------------------------------------------------*/
/* FORMAT A TRACK */
void	FDC_FormatATrack(int State)
{
	BOOL bUpdateState;

	do
	{
		bUpdateState = FALSE;

		switch (fdc.CommandState)
		{
			case 0:
			{
	//			unsigned long NopsToNextIndex;

				/* reset status registers */
				FDC_ClearStatusRegisters();
				
				FDI_SetDensity(fdc.CommandBytes[0]&0x040);

				/* get current drive and side */
				FDC_GetDriveAndSide();

				if ((FDI_GetSelectedDriveFlags() & FDD_FLAGS_DRIVE_READY)==0)
				{
					/* not ready */

					fdc.ST0 = FDC_ST0_NOT_READY;

					fdc.CommandState = 4;
					bUpdateState = TRUE;
					break;
				}


				/* initialise sector count */
				fdc.SectorCounter  = 0;

				FDI_EmptyTrack();
			
				fdc.CommandState++;
				bUpdateState = TRUE;
	#if 0
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
	#endif

			}
			break;

			case 1:
			{

	//			unsigned long NopsToNextIndex;


				/* have we done all sectors */
				if (fdc.SectorCounter == fdc.CommandBytes[3])
				{
	#if 0
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
	#endif

					/* finished transfering ID's */
					fdc.CommandState = 4;	
					bUpdateState = TRUE;
		
					/* last sector */
	//				FDC_SetupDataRequestDelay(fdc.StoredDelay + NopsToNextIndex);
					return;
				}
				else
				{
	#if 0
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
	#endif


					/* setup for execution phase */
					fdc.CommandState++;

					/* get ID for this sector C,H,R,N */
					FDC_SetupWriteExecutionPhase(4,FDC_DataBuffer);
				}
			}
			break;

			case 2:
			{
				CHRN format_chrn;

				format_chrn.C = FDC_DataBuffer[0];
				format_chrn.H = FDC_DataBuffer[1];
				format_chrn.R = FDC_DataBuffer[2];
				format_chrn.N = FDC_DataBuffer[3];

				/* write sector */
				/* fdc.CommandBytes[2] = N */ 
				/* fdc.CommandBytes[3] = SC */
				/* fdc.CommandBytes[4] = GPL */
				/* fdc.CommandBytes[5] = D */

				FDI_AddSector(&format_chrn, fdc.CommandBytes[2],fdc.CommandBytes[5]);

				/* next sector */
				fdc.SectorCounter++;
				bUpdateState = TRUE;

				fdc.CommandState--;	
			
	//			FDC_SetupDataRequestDelay(fdc.StoredDelay + 12 + 3 + 1);
			}
			break;

			case 4:
			{
				/* setup result data */
				FDC_SetStatus0();

	//			fdc.CommandBytes[6] = FormatATrack_CHRN.N;
	//			fdc.CommandBytes[5] = (FormatATrack_CHRN.R+1) & 0x0ff;
	//			fdc.CommandBytes[4] = FormatATrack_CHRN.H;
	//			fdc.CommandBytes[3] = FormatATrack_CHRN.C;
				fdc.CommandBytes[0] = fdc.ST0;
				fdc.CommandBytes[1] = fdc.ST1;
				fdc.CommandBytes[2] = fdc.ST2;
				
				FDC_SetupResultPhase(7);

				fdc.Flags &= ~NEC765_FLAGS_INTERRUPT;


			}
			break;
		}
	}
	while (bUpdateState);
}

/*-------------------------------------------------------------*/

BOOL FDC_LocateSector(void)
{
	int IndexCount;

	/* set no data flag */
	fdc.ST1 |= FDC_ST1_MISSING_ADDRESS_MARK;
	fdc.ST1 &=~FDC_ST1_NO_DATA;

	IndexCount = 0;

	do
	{
		unsigned long status;

		status = FDI_GetNextID(&fdc.chrn);
		
		if (status==0)
		{
			/* got id */
			
			/* got a id, therefore missing address mark is false */
			fdc.ST1 &=~FDC_ST1_MISSING_ADDRESS_MARK;

			if (fdc.chrn.R == fdc.CommandBytes[4])
			{
				/* found sector with id we want */

				if (fdc.chrn.C == fdc.CommandBytes[2])
				{
					/* C value the same */

					if ((fdc.chrn.H == fdc.CommandBytes[3]) && (fdc.chrn.N == fdc.CommandBytes[5]))
					{
						/* found, clear the no data flag */
						return TRUE;
					}
				}
				else
				{
					/* C doesn't match */

					/* signal wrong cylinder */
					fdc.ST2 |= FDC_ST2_WRONG_CYLINDER;

					/* C doesn't match and is 0x0ff */
					if (fdc.chrn.C == 0x0ff)
					{
						/* signal bad cylinder */
						fdc.ST2|=FDC_ST2_BAD_CYLINDER;
					}

					fdc.ST1 |= FDC_ST1_NO_DATA;

					return FALSE;
				}
			}
		}

		/* seen index pulse? */
		if (status & 1)
		{
			/* update number of times that we have seen the index */
			IndexCount++;
		}

	}
	while (IndexCount!=2);

	/* if id field was found, but encountered index pulse for second time
	then the sector was not matched */
	if ((fdc.ST1 & FDC_ST1_MISSING_ADDRESS_MARK)==0)
	{
		fdc.ST1 |= FDC_ST1_NO_DATA;
	}

	/* error, seen index pulse twice without finding sector */
	return FALSE;
}

/*-------------------------------------------------------------*/

void	FDC_ReadID(int State)
{
	BOOL bUpdateState;

	do
	{
		bUpdateState = FALSE;

		switch (fdc.CommandState)
		{
			case 0:
			{
				/* clear status reg */
				FDC_ClearStatusRegisters();

				/* get drive and side */
				FDC_GetDriveAndSide();
				
				FDI_SetDensity(fdc.CommandBytes[0]&0x040);

				/* drive ready ? */
				if ((FDI_GetSelectedDriveFlags() & FDD_FLAGS_DRIVE_READY)!=0)
				{
					int IndexCount;

					IndexCount = 0;

					/* set no data flag */
					fdc.ST1|=FDC_ST1_MISSING_ADDRESS_MARK;

					do
					{
						unsigned long status;

						
						/* get id and store into internal registers */
						status = FDI_GetNextID(&fdc.chrn);
						
						if (status==0)
						{			
							/* clear missing address mark error */
							fdc.ST1 &=~FDC_ST1_MISSING_ADDRESS_MARK;
							break;
						}

						if (status & 1)
						{
							IndexCount++;

							if (IndexCount==2)
							{
								break;
							}
						}
					}
					while (1==1);
				}
				else
				{
					fdc.ST0 |= FDC_ST0_NOT_READY;
				}
				fdc.CommandState++;
				bUpdateState = TRUE;

				FDC_SetStatus0();
				fdc.CommandBytes[0] = fdc.ST0;
				fdc.CommandBytes[1] = fdc.ST1;
				fdc.CommandBytes[2] = fdc.ST2;
				fdc.CommandBytes[3] = fdc.chrn.C;
				fdc.CommandBytes[4] = fdc.chrn.H;
				fdc.CommandBytes[5] = fdc.chrn.R;
				fdc.CommandBytes[6] = fdc.chrn.N;
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
	while (bUpdateState);
}


/*-----------------------------------------------------------------------*/

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



BOOL FDC_UpdateReadID(void)
{
	if (fdc.CommandBytes[4]==fdc.CommandBytes[6])
	{
		/* multi-track */
		if (fdc.CommandBytes[0] & 0x080)
		{
			/* side 1? */
			if ((fdc.CommandBytes[3]&1)!=0)
				return TRUE;

			/* update ID */

			/* set side */
			fdc.CommandBytes[3]|=1;
			/* set id */
			fdc.CommandBytes[4]=1;

			/* swap side */
			FDC_SetSideOutput(fdc.CurrentSide^1);

			/* continue */
			return FALSE;
		}
		else
		{
			/* not multi-track */

			/* done */
			return TRUE;
		}
	}

	/* not done */

	/* increment id */
	fdc.CommandBytes[4]++;
	return FALSE;
}

/*-------------------------------------------------------------*/

void	FDC_ReadData(int State)
{
	BOOL bUpdateState;

	do
	{
		bUpdateState = FALSE;

		switch (fdc.CommandState)
		{
			case 0:
			{
				FDC_GetDriveAndSide();

				FDC_ClearStatusRegisters();
								
				FDI_SetDensity(fdc.CommandBytes[0]&0x040);

				if ((FDI_GetSelectedDriveFlags() & FDD_FLAGS_DRIVE_READY)==0)
				{
					/* not ready */

					fdc.ST0 = FDC_ST0_NOT_READY;
				
					fdc.CommandState = 4;
					State = 4;
					bUpdateState = TRUE;
					break;
				}

				// allow backtro to work.
				// doesn't expect data request to be set so early!
				FDC_SetupDataRequestDelay(DATA_RATE_US);

				fdc.CommandState++;
				//bUpdateState = TRUE;
			}
			break;

			case 1:
			{
				/* transfer data */
				do
				{

					/* get index of sector data */
					if (FDC_LocateSector())
					{
						BOOL bControlMark = FALSE;
						BOOL bSkip = FALSE;

						/* TODO: return sector status here */
						FDI_ReadSector(FDC_DataBuffer);

						if ((fdc.CommandBytes[0] & 0x01f)==0x06)
						{
							/* READ DATA */

							/* if deleted data mark is detected, set control mark flag */
							if  (
								((fdc.chrn.ST2 & FDC_ST2_CONTROL_MARK)!=0)
								)
							{
								/* skip and deleted data mark found */
								bControlMark = TRUE;
							}
						}
						else 
						{
							/* READ DELETED DATA */

							/* if data mark is detected, set control mark flag */

							if  (
								((fdc.chrn.ST2 & FDC_ST2_CONTROL_MARK)==0)
								)
							{
								/* skip and data mark found */
								bControlMark = TRUE;
							}

						}

						if (bControlMark)
						{
							/* read a "deleted data" using "read data" */
							/* read a "data" using "read deleted data" */
							fdc.ST2 |= FDC_ST2_CONTROL_MARK;
						}

						if  (
							/* skip flag set */
							((fdc.CommandBytes[0] & FDC_COMMAND_SKIP)!=0) && 
							/* this sector will set control mark */
							(bControlMark)
							)
						{
							/* skip and deleted data mark found */
							bSkip = TRUE;
						}


						if (!bSkip)
						{	
							/* no skip */

							fdc.CommandState++;

							/* transfer sector data to cpu */
							FDC_SetupReadExecutionPhase(FDC_GetSectorSize(fdc.chrn.N),FDC_DataBuffer);

							break;
						}
						else
						{

							/* skip! */

							/* any more to read? */
							if (FDC_UpdateReadID())
							{
								/* no */
					
								/* have read last sector */
								fdc.ST1 |= FDC_ST1_END_OF_CYLINDER;

								fdc.CommandState = 4;	
								bUpdateState = TRUE;
								break;
							}
						}
					}
					else
					{
						/* error finding sector */
						fdc.CommandState = 4;
						bUpdateState = TRUE;
						break;
					}
				}
				while (1==1);
		
			}
			break;

			case 2:
			{
				BOOL bHalt = FALSE;

				if (fdc.ST1 & FDC_ST1_OVERRUN)
				{
					fdc.CommandState = 4;	
					bUpdateState = TRUE;
					break;
				}

				if (
					/* skip not set? */
					((fdc.CommandBytes[0] & FDC_COMMAND_SKIP)==0) && 
					/* control mark set? */
					((fdc.ST2 & FDC_ST2_CONTROL_MARK)!=0)
					)
				{
					bHalt = TRUE;
				}


				if (bHalt)
				{
					fdc.CommandState = 4;	
					bUpdateState = TRUE;
					break;
				}

				/* error ? */
				if (
				   (fdc.chrn.ST1 & FDC_ST1_MISSING_ADDRESS_MARK) || 
				   (fdc.chrn.ST2 & FDC_ST2_DATA_ERROR_IN_DATA_FIELD) || 
				   (fdc.chrn.ST2 & FDC_ST2_MISSING_ADDRESS_MARK_IN_DATA_FIELD)
				   )
				{
					/* end of cylinder not set! */

					fdc.ST1 |= FDC_ST1_DATA_ERROR;

					/* data error in data field? */
					if (fdc.chrn.ST2 & FDC_ST2_DATA_ERROR_IN_DATA_FIELD)
					{
						/* set data error */
						fdc.ST2 |= FDC_ST2_DATA_ERROR_IN_DATA_FIELD;
					}

					/* we want to quit */
					fdc.CommandState = 4;	
					bUpdateState = TRUE;
					break;
				}

				if (!FDC_UpdateReadID())
				{
					fdc.CommandState = 1;
					bUpdateState = TRUE;
					break;
				}

				/* have read last sector */
				fdc.ST1 |= FDC_ST1_END_OF_CYLINDER;

				fdc.CommandState = 4;
				bUpdateState = TRUE;
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

				fdc.chrn.C = fdc.CommandBytes[3];
				fdc.chrn.H = fdc.CommandBytes[4];
				fdc.chrn.R = fdc.CommandBytes[5];
				fdc.chrn.N = fdc.CommandBytes[6];

				
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
	while (bUpdateState);

}

/*-------------------------------------------------------------*/

void	FDC_WriteData(int State)
{
	BOOL bUpdateState;

	do
	{
		bUpdateState = FALSE;

		switch (fdc.CommandState)
		{
			case 0:
			{
				FDC_GetDriveAndSide();

				FDC_ClearStatusRegisters();
				
				FDI_SetDensity(fdc.CommandBytes[0]&0x040);

				if ((FDI_GetSelectedDriveFlags() & FDD_FLAGS_DRIVE_READY)==0)
				{
					/* not ready */

					fdc.ST0 = FDC_ST0_NOT_READY;
				
					fdc.CommandState = 4;
					bUpdateState = TRUE;
					break;
				}

				fdc.CommandState++;	
				bUpdateState = TRUE;
			}
			break;

			case 1:
			{
				/* transfer data */
				if (FDC_LocateSector())
				{
					fdc.CommandState++;

					FDC_SetupWriteExecutionPhase(FDC_GetSectorSize(fdc.chrn.N),FDC_DataBuffer);
					break;
				}
				else
				{
					/* error finding sector */
					fdc.CommandState = 4;	
					bUpdateState = TRUE;
					break;
				}


				fdc.CommandState = 4;
				bUpdateState = TRUE;
			}
			break;

			case 2:
			{
				/* to get here, the sector must have been located */

				int Mark;

				/* data or deleted data address mark? */
				if ((fdc.CommandBytes[0]&0x01f)==9)
				{
					/* deleted data */
					Mark = 1;
				}
				else
				{
					/* data */
					Mark = 0;
				}

				FDI_WriteSector(FDC_DataBuffer, Mark);

				/* go to next sector */
				if (!FDC_UpdateReadID())
				{
					fdc.CommandState = 1;
					bUpdateState = TRUE;
					break;
				}

				/* set end of cylinder */
				fdc.ST1 |= FDC_ST1_END_OF_CYLINDER;
				fdc.CommandState = 4;
				bUpdateState = TRUE;
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
	while (bUpdateState);

}

/*-------------------------------------------------------------*/

void	FDC_Scan(int State)
{
	BOOL bUpdateState;

	do
	{
		bUpdateState = FALSE;

		switch (fdc.CommandState)
		{
			case 0:
			{
				FDC_GetDriveAndSide();

				FDC_ClearStatusRegisters();

				FDI_SetDensity(fdc.CommandBytes[0]&0x040);

				if ((FDI_GetSelectedDriveFlags() & FDD_FLAGS_DRIVE_READY)==0)
				{
					/* not ready */

					fdc.ST0 = FDC_ST0_NOT_READY;
					bUpdateState = TRUE;				
					fdc.CommandState = 4;
					break;
				}

				bUpdateState = TRUE;
				fdc.CommandState++;	
			}
			break;

			case 1:
			{
				/* transfer data */
				while (fdc.CommandBytes[4] != fdc.CommandBytes[6])
				{
					if (FDC_LocateSector())
					{
						fdc.CommandState++;

						FDC_SetupWriteExecutionPhase(FDC_GetSectorSize(fdc.chrn.N),FDC_DataBuffer);					
						break;
					}
					else
					{
						/* error finding sector */
						fdc.CommandState = 4;
						bUpdateState = TRUE;
						break;
					}
				}

				/* set end of cylinder */
				fdc.ST1 |= FDC_ST1_END_OF_CYLINDER;
				fdc.CommandState = 4;	
				bUpdateState = TRUE;
			}
			break;

			case 2:
			{
				/* TODO: compare sector data */


				/* next sector please */
				fdc.CommandBytes[4]++;
				
				/* go to attempt to write it */
				fdc.CommandState = 1;
				bUpdateState = TRUE;
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
	while (bUpdateState);
}

void	FDC_SenseDriveStatus(int State)
{
	int FDC_ST3;
	unsigned long Flags;
	FDC_GetDriveAndSide();

	FDC_ST3 = 0;
	Flags = FDI_GetSelectedDriveFlags();

	if (Flags & FDD_FLAGS_DRIVE_ENABLED)
	{
		/* drive is active */

		/* get status of drive - interested only in Write Protect,
		Ready and Track 0 */

		if (Flags & FDD_FLAGS_DISK_PRESENT)
		{
			/* disk is present */

			if (Flags & FDD_FLAGS_DRIVE_READY)
			{
				FDC_ST3 |= FDC_ST3_READY;
			}

			/* write protected? */
			if (Flags & FDD_FLAGS_WRITE_PROTECTED)
			{
				FDC_ST3 |= FDC_ST3_WRITE_PROTECTED;
			}			
		}
		else
		{
			/* disk is not present */
			FDC_ST3 |= FDC_ST3_WRITE_PROTECTED;
		
		}

		/* track 0? */
		if (Flags & FDD_FLAGS_HEAD_AT_TRACK_0)
		{
			FDC_ST3 |= FDC_ST3_TRACK_0;
		}

//		/* if not drive 0 - internal 3" drive */
//		if (fdi.PhysicalDrive!=0)
//		{	
//			/* say it's two side */
//			FDC_ST3 |= FDC_ST3_TWO_SIDE;
//		}
	}
	else
	{
		/* disk is not present */
		FDC_ST3 |= FDC_ST3_WRITE_PROTECTED;
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
		(fdc.ST2 & (/*FDC_ST2_CONTROL_MARK | */
					FDC_ST2_DATA_ERROR_IN_DATA_FIELD |
					FDC_ST2_WRONG_CYLINDER |
					FDC_ST2_SCAN_EQUAL_HIT |
					FDC_ST2_SCAN_NOT_SATISFIED |
					FDC_ST2_BAD_CYLINDER |
					FDC_ST2_MISSING_ADDRESS_MARK_IN_DATA_FIELD))
		)!=0)
	{
		fdc.ST0 |= 0x040;

//		if (fdc.ST0 & FDC_ST0_NOT_READY)
//		{
//			fdc.ST0 |= 0x040;
//		}
//		else
//		{
//			fdc.ST0 |= 0x040;
//		}
	}
	else
	{
		fdc.ST0 &= 0x03f;
	}

	/* include drive and side details */
	fdc.ST0 |= (fdc.CurrentDrive | (fdc.CurrentSide<<2));
}

static int	FDC_GetSectorSize(int N)
{
	/* tested on CPC+ fdc */
	/* 0-> 0x0050 */
	/* 1-> 0x0100 */
	/* 2-> 0x0200 */
	/* 3-> 0x0400 */
	/* 4-> 0x1000 */
	/* 5-> 0x2000 */
	/* 6-> 0x4000 */
	/* 7-> 0x8000 */
	/* 8-> 0x8000 */
	if (N==0)
		return 0x0050;
	if (N>=8)
		return 0x8000;
	
	return (1<<N)<<7;
}


