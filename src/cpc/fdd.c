#include "fdd.h"

/* the two drives */
static FDD drive[2];

FDD		*FDD_GetDrive(int DriveIndex)
{
	return &drive[DriveIndex];
}

unsigned long FDD_GetFlags(int DriveIndex)
{
	return drive[DriveIndex].Flags;
}


void	FDD_InitialiseAll(void)
{
	int i;

//	MakeTrack(TrackBuffer);

//	FDC_InitialiseIDAndDataPositions();


	for (i=0; i<2; i++)
	{
		FDD *theDrive = &drive[i];

		theDrive->CurrentTrack = 40;
		theDrive->Flags = 0;

		if (i==0)
		{
			theDrive->NumberOfTracks = 43;
		}
		else
		{
			theDrive->NumberOfTracks = 80;
			theDrive->Flags |= FDD_FLAGS_DOUBLE_SIDED;
		}
//		theDrive->PhysicalSide = 0;
	}
}


/* perform the actual step */
void	FDD_PerformStep(unsigned long DriveIndex, signed int StepDirection)
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

//	FDC_SetDriveStatus(Drive);

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

//	drive->PhysicalSide^=0x01;
}

void	FDD_Initialise(int Drive)
{
	FDD *drive = FDD_GetDrive(Drive);

	/* set default side */
//	drive->PhysicalSide = 0;

	drive->Flags |= FDD_FLAGS_DRIVE_ENABLED;
	drive->Flags &= ~(FDD_FLAGS_DISK_PRESENT | FDD_FLAGS_WRITE_PROTECTED);

}

/* the disc light comes on for a read/write operation only */
void	FDD_LED_SetState(unsigned long Drive, int LedState)
{
	FDD *drive = FDD_GetDrive(Drive);

	if (LedState)
	{
		drive->Flags &=~FDD_FLAGS_LED_ON;
	}
	else
	{
		drive->Flags |= FDD_FLAGS_LED_ON;
	}
}

int		FDD_LED_GetState(unsigned long Drive)
{
	FDD *drive = FDD_GetDrive(Drive);

	return ((drive->Flags & FDD_FLAGS_LED_ON)!=0);
}


void	FDD_SetMotorState(int Drive, int MotorState)
{
	BOOL bReady = FALSE;
	FDD *drive = FDD_GetDrive(Drive);

	/* drive enabled? */
	if (drive->Flags & FDD_FLAGS_DRIVE_ENABLED)
	{
		/* disk present? */
		if (drive->Flags & FDD_FLAGS_DISK_PRESENT)
		{
			/* motor on? */
			if (MotorState!=0)
				bReady = TRUE;
		}
	}

	if (bReady)
		drive->Flags |=FDD_FLAGS_DRIVE_READY;
	else
		drive->Flags &=~FDD_FLAGS_DRIVE_READY;
}

