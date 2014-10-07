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
#ifdef _WIN32
#include <tchar.h>
#else
#define TCHAR char
#endif


#include "../cpc/cpcglob.h"

void	GenericInterface_Initialise(void);
void	GenericInterface_Finish(void);

void SetString(TCHAR **, const TCHAR *);

extern TCHAR *DiskImagePath;			/* path for opening disk images */
extern TCHAR *CartridgePath;			/* path for opening cartridges */
extern TCHAR *SnapshotPath;			/* path for opening snapshots */
extern TCHAR *RomPath;					/* path for opening roms */
extern TCHAR *ScreenSnapshotPath;		/* path for writing screen snapshots */
extern TCHAR *WavOutputPath;			/* path for writing wav recording of sound */
extern TCHAR *AYOutputPath;			/* path for writing recording of ay */
extern TCHAR *TapePath;				/* path for opening tapes */

extern TCHAR *RomFilenames[17];		/* filenames of opened roms */
extern TCHAR *CartridgeFilename;		/* filename of last opened cartridge */
extern TCHAR *SnapshotFilename;		/* filename of last opened/saved snapshot */
extern TCHAR *WavOutputFilename;		/* filename of last wav recording */
extern TCHAR *AYOutputFilename;		/* filename of last AY recording */
extern TCHAR *DriveAFilename;			/* filename of last disk image opened for drive A */
extern TCHAR *DriveBFilename;			/* filename of last disk image opened for drive B */
extern TCHAR *TapeFilename;			/* filename of last tape opened */
extern TCHAR *Multiface_CPC_ROM_Filename;	/* filename of CPC version of multiface rom */
extern TCHAR *Multiface_CPCPLUS_ROM;		/* filename of CPC+ version of multiface rom */

/* insert a snapshot */
BOOL	GenericInterface_LoadSnapshot(const TCHAR *FilenameBuffer);
/* insert a tape-image or sample file */
BOOL	GenericInterface_InsertTape(const TCHAR *FilenameBuffer);

/* insert a cartridge */BOOL	GenericInterface_InsertCartridge(const TCHAR *FilenameBuffer);

/* restore settings */
void	GenericInterface_RestoreSettings(void);
/* store settings */
void	GenericInterface_StoreSettings(void);

BOOL	GenericInterface_SaveScreenSnapshot(const TCHAR *FilenameBuffer);
BOOL	GenericInterface_WavOutputStartRecording(const TCHAR *FilenameBuffer);

BOOL	GenericInterface_LoadRom(int RomIndex, const TCHAR *FilenameBuffer);

BOOL	GenericInterface_SnapshotSave(const TCHAR *FilenameBuffer, int nSnapshotVersion, int nSnapshotSize);

BOOL	GenericInterface_InsertDiskImage(int DriveIndex, const TCHAR *pFilename);

/* start recording a sound file for StSound */
void	GenericInterface_AYStartRecording(const TCHAR *FilenameBuffer);

#include "../cpc/multface.h"

int Multiface_LoadRomFromFile(const MULTIFACE_ROM_TYPE RomType, const TCHAR *pFilename);



#endif

