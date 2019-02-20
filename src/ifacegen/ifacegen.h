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
#ifndef __GENERIC_INTERFACE_HEADER_INCLUDED__
#define __GENERIC_INTERFACE_HEADER_INCLUDED__

#include "../cpc/cpcglob.h"

void	GenericInterface_Initialise(void);
void	GenericInterface_Finish(void);

/* insert a snapshot */
BOOL	GenericInterface_LoadSnapshot(char *FilenameBuffer);

/* insert a tape image - TZX type */
BOOL	GenericInterface_InsertTapeImage(char *FilenameBuffer);

/* insert a cartridge */BOOL	GenericInterface_InsertCartridge(char *FilenameBuffer);

/* restore settings */
void	GenericInterface_RestoreSettings(void);
/* store settings */
void	GenericInterface_StoreSettings(void);

void GenericInterface_RemoveDiskImage(int Drive);

void	GenericInterface_DoReset(void);

BOOL	GenericInterface_SaveScreenSnapshot(char *FilenameBuffer);
BOOL	GenericInterface_WavOutputStartRecording(char *FilenameBuffer);

BOOL	GenericInterface_LoadRom(int RomIndex, char *FilenameBuffer);

void	GenericInterface_SetSnapshotSize(int);
void	GenericInterface_SetSnapshotVersion(int);

BOOL	GenericInterface_SnapshotSave(char *FilenameBuffer);

void GenericInterface_InsertUnformattedDisk(int Drive, char *pFilename);

BOOL	GenericInterface_InsertDiskImage(int DriveIndex, char *pFilename);

/* start recording a sound file for StSound */
void	GenericInterface_AYStartRecording(char *FilenameBuffer);



#endif

