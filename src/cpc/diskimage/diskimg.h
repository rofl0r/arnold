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

#include "../headers.h"
#include "../cpcglob.h"	

#include "../device.h"

enum
{
	DISK_IMAGE_TYPE_UNDEFINED,	/* not created from any image type */
	DISK_IMAGE_TYPE_STANDARD,	/* image opened from DSK */
	DISK_IMAGE_TYPE_EXTENDED,	/* image opened from EDSK */
	DISK_IMAGE_TYPE_DIF,		/* image opened from DIF */
};

#define DISKIMAGE_DISK_INSERTED	0x0001
#define DISKIMAGE_DISK_DIRTY	0x0002

typedef struct
{
	int	Flags;
	int nImageType;				/* type of image this was inserted from */
	unsigned char 	*pDiskImage;
} DISKIMAGE_UNIT;

void	DiskImage_Initialise(void);
void	DiskImage_Finish(void);
int		DiskImage_InsertUnformattedDisk(int DriveID);
BOOL	DiskImage_IsImageDirty(int DriveID);

int DiskImage_InsertUnformatted(int);
int	DiskImage_InsertDisk(int,const unsigned char *, const unsigned long);
void DiskImage_RemoveDisk(int);

int		DiskImage_GetSectorsPerTrack(int,int,int);
void	DiskImage_GetID(int,int, int, int, CHRN *);
void	DiskImage_GetSector(int, int, int, int, char *);
void	DiskImage_PutSector(int, int, int, int, char *,int);
void	DiskImage_AddSector(int, int, int, CHRN *,int,int);
void	DiskImage_EmptyTrack(int DriveID, int PhysicalTrack, int PhysicalSide);
int Dif_Validate(const unsigned char *pDiskImage, const unsigned long DiskImageLength);

unsigned long DiskImage_CalculateOutputSize(int DriveID);
void    DiskImage_GenerateOutputData(unsigned char *pBuffer, int nDrive);

#endif
