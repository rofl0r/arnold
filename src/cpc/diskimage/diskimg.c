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
/*
        DISK IMAGE HANDLING CODE

        If disk image is single sided, and access made to 2nd side, currently
        forces to side 0.

*/
#include "../fdd.h"
#include "diskimg.h"
#include "dsk.h"
#include "extdsk.h"
#include "iextdsk.h"
#include "../host.h"
#include "../device.h"
#include "../cpc.h"

static DISKIMAGE_UNIT   Drives[4];

static void DiskImage_ImageRecognised(int DriveID,int Type)
{
        DISKIMAGE_UNIT  *pDrive=&Drives[DriveID];

        pDrive->Flags |= DISKIMAGE_DISK_INSERTED;
        pDrive->nImageType = Type;
}

void    DiskImage_Initialise(void)
{
        memset(Drives,0,sizeof(DISKIMAGE_UNIT));
}

void    DiskImage_Finish(void)
{
        DiskImage_RemoveDisk(0);
		FDD_InsertDisk(0,FALSE);

        DiskImage_RemoveDisk(1);
		FDD_InsertDisk(1,FALSE);
}

/*********************************************************************************/

int		DiskImage_InsertUnformattedDisk(int DriveID)
{
	/* remove existing disk */
	DiskImage_RemoveDisk(DriveID);

    DiskImage_ImageRecognised(DriveID,DISK_IMAGE_TYPE_UNDEFINED);
                            
    ExtDskInternal_Initialise(&Drives[DriveID]);
 
	Drives[DriveID].Flags |= DISKIMAGE_DISK_DIRTY;

	FDD_InsertDisk(DriveID, TRUE);
	return TRUE;
}

/* install a disk image into the drive */

/* pDiskImage is pointer to disk image data loaded from host */
/* DiskImageLength is length of data loaded from host */
/* assumes therefore that disc image data is contained in a single file */
int     DiskImage_InsertDisk(int DriveID, const unsigned char *pDiskImage, const unsigned long DiskImageLength)
{
 	/* validate disk image */
	if (Dsk_Validate(pDiskImage, DiskImageLength))
	{
		/* standard dsk */
		DiskImage_RemoveDisk(DriveID);

		/* valid */
		DiskImage_ImageRecognised(DriveID,DISK_IMAGE_TYPE_STANDARD);

		ExtDskInternal_Dsk2ExtDskInternal(&Drives[DriveID],pDiskImage, DiskImageLength);

		FDD_InsertDisk(DriveID, TRUE);
		return ARNOLD_STATUS_OK;
	}
	else
	if (ExtDsk_Validate(pDiskImage,DiskImageLength))
	{
		/* extdsk */
		DiskImage_RemoveDisk(DriveID);

		DiskImage_ImageRecognised(DriveID,DISK_IMAGE_TYPE_EXTENDED);

		ExtDskInternal_ExtDsk2ExtDskInternal(&Drives[DriveID],pDiskImage, DiskImageLength);

		FDD_InsertDisk(DriveID, TRUE);
		return ARNOLD_STATUS_OK;
	}
	else
	if (Dif_Validate(pDiskImage,DiskImageLength))
	{
		/* dif */
		DiskImage_RemoveDisk(DriveID);

		DiskImage_ImageRecognised(DriveID,DISK_IMAGE_TYPE_DIF);

		ExtDskInternal_Dif2ExtDskInternal(&Drives[DriveID],pDiskImage,DiskImageLength);

		FDD_InsertDisk(DriveID, TRUE);
		return ARNOLD_STATUS_OK;
	}
    return ARNOLD_STATUS_UNRECOGNISED;
}

/* remove a disk image from the drive */
void    DiskImage_RemoveDisk(int DriveID)
{
        DISKIMAGE_UNIT  *pDrive=&Drives[DriveID];

         ExtDskInternal_Free(pDrive);

        /* initialise drive */
        memset(pDrive,0,sizeof(DISKIMAGE_UNIT));

		FDD_InsertDisk(DriveID,FALSE);
}

/**********************************************************************/
/* DISK IMAGE ACCESS FUNCTIONS - INTERFACE BETWEEN DISK IMAGE AND FDC */

int             DiskImage_GetSectorsPerTrack(int DriveID, int PhysicalTrack,int PhysicalSide)
{
        DISKIMAGE_UNIT  *pDrive=&Drives[DriveID];

		return ExtDskInternal_GetSectorsPerTrack(pDrive, PhysicalTrack, PhysicalSide);
}

/* get a ID from physical track, physical side and fill in CHRN structure with details */
void    DiskImage_GetID(int DriveID, int PhysicalTrack,int PhysicalSide, int Index, CHRN *pCHRN)
{
        DISKIMAGE_UNIT  *pDrive=&Drives[DriveID];
       
		ExtDskInternal_GetID(pDrive, PhysicalTrack, PhysicalSide, Index, pCHRN);
}

/* get a sector of data from the disk image */
void    DiskImage_GetSector(int DriveID, int PhysicalTrack, int PhysicalSide, int Index, char *pData)
{
        DISKIMAGE_UNIT  *pDrive = &Drives[DriveID];

        ExtDskInternal_GetSector(pDrive, PhysicalTrack, PhysicalSide, Index, pData);
}

/* write a sector of data to the disk image */
void    DiskImage_PutSector(int DriveID, int PhysicalTrack, int PhysicalSide, int Index, char *pData, int Mark)
{
        DISKIMAGE_UNIT *pDrive = &Drives[DriveID];

        pDrive->Flags |= DISKIMAGE_DISK_DIRTY;

	    ExtDskInternal_PutSector(pDrive, PhysicalTrack, PhysicalSide, Index, pData,Mark);
}


/* write a sector of data to the disk image */
void    DiskImage_AddSector(int DriveID, int PhysicalTrack, int PhysicalSide, CHRN *pCHRN, int FormatN,int FillerByte)
{
        DISKIMAGE_UNIT *pDrive = &Drives[DriveID];

        pDrive->Flags |= DISKIMAGE_DISK_DIRTY;

        ExtDskInternal_AddSector(pDrive, PhysicalTrack, PhysicalSide, pCHRN, FormatN,FillerByte);
}

void	DiskImage_EmptyTrack(int DriveID, int PhysicalTrack, int PhysicalSide)
{
    DISKIMAGE_UNIT *pDrive = &Drives[DriveID];

    pDrive->Flags |= DISKIMAGE_DISK_DIRTY;

    ExtDskInternal_EmptyTrack(pDrive, PhysicalTrack, PhysicalSide);
}
	

BOOL    DiskImage_IsImageDirty(int DriveID)
{
    DISKIMAGE_UNIT  *pDrive = &Drives[DriveID];

    if (pDrive->Flags & DISKIMAGE_DISK_DIRTY)
    {
            return TRUE;
    }

    return FALSE;
}

unsigned long DiskImage_CalculateOutputSize(int DriveID)
{
	DISKIMAGE_UNIT  *pDrive = &Drives[DriveID];

	return ExtDskInternal_CalculateOutputDataSize(pDrive);
}


void    DiskImage_GenerateOutputData(unsigned char *pBuffer, int DriveID)
{
       DISKIMAGE_UNIT  *pDrive = &Drives[DriveID];
		
	   ExtDskInternal_GenerateOutputData(pBuffer, pDrive);
}

