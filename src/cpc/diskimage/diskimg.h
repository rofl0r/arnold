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
#ifndef __DSKIMG_HEADER_INCLUDED__
#define __DSKIMG_HEADER_INCLUDED__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../cpcglob.h"	

#include "../device.h"

#define STANDARD_DISK_IMAGE	0x0001
#define EXTENDED_DISK_IMAGE	0x0002

#define INTERNAL_DISK_IMAGE 0x0003

#define DISKIMAGE_DISK_INSERTED	0x0001
#define DISKIMAGE_DISK_DIRTY	0x0002

#define DISK_UNFORMATTED	0x0001
#define DISK_FORMATTED		0x0002

typedef struct DISKIMAGE_UNIT
{
	int	Flags;
	int Type;
	char	*Filename;
	unsigned char 	*pDiskImage;
	unsigned long	DiskImageSize;
	int		NumTracks;
	int		NumSides;
	int		TrackSize;
	char	Header[256];
	int		*pTrackSize;
	int		TrackOffset;
	int		SectorOffset;
	int		CurrentTrack;
	int		CurrentSide;
} DISKIMAGE_UNIT;

void	DiskImage_Initialise(void);
void	DiskImage_Finish(void);

BOOL	DiskImage_IsImageDirty(int DriveID);
void	DiskImage_WriteImage(int);

int	DiskImage_InsertDisk(int,int,char *);
void DiskImage_RemoveDisk(int);

int		DiskImage_GetSectorsPerTrack(int,int,int);
void	DiskImage_GetID(int,int, int, int, FDC_CHRN *);
void	DiskImage_GetSector(int, int, int, int, char *);
void	DiskImage_PutSector(int, int, int, int, char *);
void	DiskImage_AddSector(int, int, int, FDC_CHRN *,int);
void	DiskImage_EmptyTrack(int DriveID, int PhysicalTrack, int PhysicalSide);

#endif
