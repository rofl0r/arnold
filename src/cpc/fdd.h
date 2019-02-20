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
#ifndef __FDD_HEADER_INCLUDED__
#define __FDD_HEADER_INCLUDED__

#include "cpcglob.h"

/* FDD functions */
void	FDD_InitialiseAll();
void	FDD_Initialise(int);
void	FDD_TurnDisk(int);
void	FDD_InsertDisk(int,int);
BOOL	FDD_IsDiskPresent(int);

int		FDD_LED_GetState(unsigned long Drive);
void	FDD_LED_SetState(unsigned long Drive, int LedState);

/* head is positioned at track 0 */
#define FDD_FLAGS_HEAD_AT_TRACK_0 0x001
/* disk inserted into drive */
#define FDD_FLAGS_DISK_PRESENT	0x002
/* drive is enabled */
#define FDD_FLAGS_DRIVE_ENABLED 0x004
/* drive is double sided */
#define FDD_FLAGS_DOUBLE_SIDED 0x008
/* drive is ready */
#define FDD_FLAGS_DRIVE_READY 0x010
/* write protected */
#define FDD_FLAGS_WRITE_PROTECTED 0x040
/* motor state */
#define FDD_FLAGS_MOTOR_STATE 0x020
/* index flag */
#define FDD_FLAGS_DRIVE_INDEX 0x080
/* led is on */
#define FDD_FLAGS_LED_ON 0x0100

typedef struct
{
	/* flags */
	unsigned long Flags;
	/* current track drive is on */
	unsigned long CurrentTrack;
	/* total number of tracks the head can move. */
	unsigned long NumberOfTracks;
	
	/* temp here until more accurate emulation is done */
	unsigned long CurrentIDIndex;			/* current id index */
} FDD;

FDD		*FDD_GetDrive(int DriveIndex);
/* get flags */
unsigned long FDD_GetFlags(int DriveIndex);
/* set motor state */
void	FDD_SetMotorState(int DriveIndex, int State);

void	FDD_PerformStep(unsigned long DriveIndex, signed int StepDirection);

#endif
