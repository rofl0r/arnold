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
#ifndef __INTERNAL_EXTENDED_DISK_IMAGE_HEADER_INCLUDED__
#define __INTERNAL_EXTENDED_DISK_IMAGE_HEADER_INCLUDED__

/* include standard ext disk structures */
#include "extdsk.h"

#define TrackHeader_Text "Track-Info\r\n\0"

#define MainHeader_Text	"EXTENDED CPC DSK File\r\nDisk-Info\r\n\0"

#define ImageCreator_Text "Arnold\0"

typedef struct EXTDSK_INTERNAL_TRACK
{
	/* true if track has been modified (formatted, or written to) */
	BOOL	ModifiedFlag;			

	int NoOfSectors;
	EXTDSKCHRN	SectorIDs[64];

	/* pointer to data for each sector */
	char	*pSectorData[64];
} EXTDSK_INTERNAL_TRACK;

typedef struct EXTDSK_INTERNAL
{
	/* header for each track 85 tracks with 2 sides */
	EXTDSK_INTERNAL_TRACK	**pTrackList;
} EXTDSK_INTERNAL;

void ExtDskInternal_PutSector(DISKIMAGE_UNIT *pDrive,int PhysicalTrack,int PhysicalSide,int Index,char *pData);
void ExtDskInternal_GetSector(DISKIMAGE_UNIT *pDrive,int PhysicalTrack,int PhysicalSide,int Index,char *pData);
int	ExtDskInternal_GetSectorsPerTrack(DISKIMAGE_UNIT *pDrive,int PhysicalTrack, int PhysicalSide);
void ExtDskInternal_GetID(DISKIMAGE_UNIT *pDrive,int PhysicalTrack,int PhysicalSide,int Index,FDC_CHRN *pCHRN);
void	ExtDskInternal_WriteImage(DISKIMAGE_UNIT *pDrive);
void	ExtDskInternal_Free(DISKIMAGE_UNIT *pUnit);
int     ExtDskInternal_Initialise(DISKIMAGE_UNIT *pDskUnit);
void	ExtDskInternal_EmptyTrack(DISKIMAGE_UNIT *pDrive, int PhysicalTrack, int PhysicalSide);
void            ExtDskInternal_ExtDsk2ExtDskInternal(DISKIMAGE_UNIT *pUnit);
void    ExtDskInternal_AddSector(DISKIMAGE_UNIT *pDrive, int PhysicalTrack, int PhysicalSide, FDC_CHRN *pCHRN, int FillerByte);
void            ExtDskInternal_Dsk2ExtDskInternal(DISKIMAGE_UNIT *pUnit);
EXTDSK_INTERNAL *ExtDskInternal_New(void);
void    ExtDskInternal_AddTrack(EXTDSK_INTERNAL *pExtDsk, int TrackIndex);
void	ExtDskInternal_RemoveSectorsInTrack(EXTDSK_INTERNAL *pExtDsk, int TrackIndex);
void    ExtDskInternal_AddSectorToTrack(EXTDSK_INTERNAL *pExtDsk, int TrackIndex, EXTDSKCHRN *pCHRN, int FillerByte);
char *ExtDskInternal_GetPointerToSectorData(EXTDSK_INTERNAL *pExtDsk, int TrackIndex, int SectorIndex);
EXTDSKCHRN *ExtDskInternal_GetSectorCHRN(EXTDSK_INTERNAL *pExtDsk, int TrackIndex, int SectorIndex);
int ExtDskInternal_GetSPT(EXTDSK_INTERNAL *pExtDsk, int TrackIndex);
int ExtDskInternal_GetSectorSize(EXTDSK_INTERNAL *pExtDsk, int TrackIndex, int SectorIndex);

#endif

