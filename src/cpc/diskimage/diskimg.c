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

#define BE_SAFE

/*#define INTERNAL */
#include "diskimg.h"
#include "dsk.h"
#include "extdsk.h"
#include "iextdsk.h"
#include "../host.h"
#include "../cpcdefs.h"

static DISKIMAGE_UNIT   Drives[4];

static void DiskImage_ImageRecognised(int DriveID,int Type,char *Filename, unsigned char *pDiskImage, unsigned long DiskImageSize)
{
        DISKIMAGE_UNIT  *pDrive=&Drives[DriveID];

        pDrive->Flags |= DISKIMAGE_DISK_INSERTED;
        pDrive->Type = Type;

        if (Filename!=NULL)
        {
                pDrive->Filename = (char *)malloc(strlen(Filename)+1);
                memcpy(pDrive->Filename,Filename,strlen(Filename)+1);
        }

        pDrive->pDiskImage = pDiskImage;
        pDrive->DiskImageSize = DiskImageSize;
}

void    DiskImage_Initialise(void)
{
        memset(Drives,0,sizeof(DISKIMAGE_UNIT));
}

void    DiskImage_Finish(void)
{
        DiskImage_RemoveDisk(0);

        DiskImage_RemoveDisk(1);
}

/*********************************************************************************/


/* install a disk image into the drive */
int     DiskImage_InsertDisk(int DriveID, int Type, char *Filename)
{
        unsigned char   *pDiskImage = NULL;
        unsigned long DiskImageLength = 0;

        /* if disk inserted, remove it */
        if (Drives[DriveID].Flags & DISKIMAGE_DISK_INSERTED)
                DiskImage_RemoveDisk(DriveID);

        if (Type == DISK_UNFORMATTED)
        {
                /* valid */
                DiskImage_ImageRecognised(DriveID,INTERNAL_DISK_IMAGE,Filename,pDiskImage, DiskImageLength);
                                
                ExtDskInternal_Initialise(&Drives[DriveID]);
                return TRUE;
        }
        else
        {
                /* load disk image file to memory */
                if (!(Host_LoadFile(Filename, &pDiskImage, &DiskImageLength)))
                {
                        return FALSE;
                }
                else
                {
                        /* image loaded */

                }

                /* detect DSK type using header bytes */
                if (memcmp(pDiskImage,"MV - CPC",8)==0)
                {
                        /* standard disk image .DSK */
                        
                        /* validate disk image */
                        if (Dsk_Validate(Filename))
                        {
        #ifndef INTERNAL
                        
                                /* valid */
                                DiskImage_ImageRecognised(DriveID,STANDARD_DISK_IMAGE,Filename,pDiskImage, DiskImageLength);
                        
                                /* initialise disk image */
                                Dsk_Initialise(&Drives[DriveID]);
        #else
                                
                                /* valid */
                                DiskImage_ImageRecognised(DriveID,STANDARD_DISK_IMAGE,Filename,pDiskImage, DiskImageLength);
                        
                                ExtDskInternal_Dsk2ExtDskInternal(&Drives[DriveID]);

                                Drives[DriveID].Type = INTERNAL_DISK_IMAGE;

                                if (pDiskImage!=NULL)
                                        free(pDiskImage);
        #endif          
                                
                                return TRUE;
                        }
                }
        
                if (memcmp(pDiskImage,"EXTENDED",8)==0)
                {
                        /* extended disk image .DSK */

                        /* validate disk image */
                        if (ExtDsk_Validate(Filename))
                        {
                                /* valid */
        #ifndef INTERNAL
                                DiskImage_ImageRecognised(DriveID,EXTENDED_DISK_IMAGE,Filename,pDiskImage, DiskImageLength);
                                
                                /* initialise extended disk image */
                                ExtDsk_Initialise(&Drives[DriveID]);
        #else
                                DiskImage_ImageRecognised(DriveID,EXTENDED_DISK_IMAGE,Filename,pDiskImage, DiskImageLength);

                                ExtDskInternal_ExtDsk2ExtDskInternal(&Drives[DriveID]);

                                Drives[DriveID].Type = INTERNAL_DISK_IMAGE;

                                if (pDiskImage!=NULL)
                                        free(pDiskImage);
        #endif

                                return TRUE;
                        }
                }

                /* not recognised or not valid */
                free(pDiskImage);
        }

        return FALSE;
}

/* remove a disk image from the drive */
void    DiskImage_RemoveDisk(int DriveID)
{
        DISKIMAGE_UNIT  *pDrive=&Drives[DriveID];

        if (pDrive->Type!=0)
         {
                switch (pDrive->Type)
                {
                        case STANDARD_DISK_IMAGE:
                        {
                                
                
                                Dsk_Free(pDrive);
                
								/* close the file */
                                if (pDrive->pDiskImage!=NULL)
                                        free(pDrive->pDiskImage);

                                break;
                        }
                
                case EXTENDED_DISK_IMAGE:
                {

                        ExtDsk_Free(pDrive);

                        
                        /* close the file */
                        if (pDrive->pDiskImage!=NULL)
                                free(pDrive->pDiskImage);

                        break;
                }

                case INTERNAL_DISK_IMAGE:
                        ExtDskInternal_Free(pDrive);
                        break;

                }
        }

        /* free filename */
        if (pDrive->Filename!=NULL)
                free(pDrive->Filename);
        
        /* initialise drive */
        memset(pDrive,0,sizeof(DISKIMAGE_UNIT));
}

/**********************************************************************/
/* DISK IMAGE ACCESS FUNCTIONS - INTERFACE BETWEEN DISK IMAGE AND FDC */

int             DiskImage_GetSectorsPerTrack(int DriveID, int PhysicalTrack,int PhysicalSide)
{
        DISKIMAGE_UNIT  *pDrive=&Drives[DriveID];

#ifdef BE_SAFE
        if (pDrive->NumSides==1)
        {
                PhysicalSide = 0;
        }
#endif

        switch (pDrive->Type)
        {
                case STANDARD_DISK_IMAGE:
                        return Dsk_GetSectorsPerTrack(pDrive,PhysicalTrack,PhysicalSide);
                case EXTENDED_DISK_IMAGE:
                        return ExtDsk_GetSectorsPerTrack(pDrive,PhysicalTrack,PhysicalSide);
                case INTERNAL_DISK_IMAGE:
                        return ExtDskInternal_GetSectorsPerTrack(pDrive, PhysicalTrack, PhysicalSide);
        }

        return 0;
}

/* get a ID from physical track, physical side and fill in FDC_CHRN structure with details */
void    DiskImage_GetID(int DriveID, int PhysicalTrack,int PhysicalSide, int Index, FDC_CHRN *pCHRN)
{
        DISKIMAGE_UNIT  *pDrive=&Drives[DriveID];

#ifdef BE_SAFE
        if (pDrive->NumSides==1)
        {
                PhysicalSide = 0;
        }
#endif
        switch (pDrive->Type)
        {
        case STANDARD_DISK_IMAGE:
                Dsk_GetID(pDrive, PhysicalTrack, PhysicalSide,Index, pCHRN);
                return;

        case EXTENDED_DISK_IMAGE:
                ExtDsk_GetID(pDrive, PhysicalTrack,PhysicalSide,Index, pCHRN);
                return;
        case INTERNAL_DISK_IMAGE:
                ExtDskInternal_GetID(pDrive, PhysicalTrack, PhysicalSide, Index, pCHRN);
                return;
        }
}

/* get a sector of data from the disk image */
void    DiskImage_GetSector(int DriveID, int PhysicalTrack, int PhysicalSide, int Index, char *pData)
{
        DISKIMAGE_UNIT  *pDrive = &Drives[DriveID];

#ifdef BE_SAFE
        if (pDrive->NumSides==1)
        {
                PhysicalSide = 0;
        }
#endif

        switch (pDrive->Type)
        {
                case STANDARD_DISK_IMAGE:
                        Dsk_GetSector(pDrive,PhysicalTrack,PhysicalSide,Index,pData);
                        return;
                case EXTENDED_DISK_IMAGE:
                        ExtDsk_GetSector(pDrive,PhysicalTrack,PhysicalSide, Index,pData);
                        return;
                case INTERNAL_DISK_IMAGE:
                        ExtDskInternal_GetSector(pDrive, PhysicalTrack, PhysicalSide, Index, pData);
                        return;
        }
}

/* write a sector of data to the disk image */
void    DiskImage_PutSector(int DriveID, int PhysicalTrack, int PhysicalSide, int Index, char *pData)
{
        DISKIMAGE_UNIT *pDrive = &Drives[DriveID];

        pDrive->Flags |= DISKIMAGE_DISK_DIRTY;

#ifdef BE_SAFE
        if (pDrive->NumSides==1)
        {
                PhysicalSide = 0;
        }
#endif

        switch (pDrive->Type)
        {
                case STANDARD_DISK_IMAGE:
                        Dsk_PutSector(pDrive, PhysicalTrack, PhysicalSide, Index, pData);
                        return;
                case EXTENDED_DISK_IMAGE:
                        ExtDsk_PutSector(pDrive, PhysicalTrack, PhysicalSide, Index, pData);
                        return;
                case INTERNAL_DISK_IMAGE:
                        ExtDskInternal_PutSector(pDrive, PhysicalTrack, PhysicalSide, Index, pData);
                        return;
        }
}


/* write a sector of data to the disk image */
void    DiskImage_AddSector(int DriveID, int PhysicalTrack, int PhysicalSide, FDC_CHRN *pCHRN, int FillerByte)
{
        DISKIMAGE_UNIT *pDrive = &Drives[DriveID];

        pDrive->Flags |= DISKIMAGE_DISK_DIRTY;

#ifdef BE_SAFE
        if (pDrive->NumSides==1)
        {
                PhysicalSide = 0;
        }
#endif

        switch (pDrive->Type)
        {
                case STANDARD_DISK_IMAGE:
                        return;
                case EXTENDED_DISK_IMAGE:
                        return;
                case INTERNAL_DISK_IMAGE:
                        ExtDskInternal_AddSector(pDrive, PhysicalTrack, PhysicalSide, pCHRN, FillerByte);
                        return;

        }
}

void	DiskImage_EmptyTrack(int DriveID, int PhysicalTrack, int PhysicalSide)
{
        DISKIMAGE_UNIT *pDrive = &Drives[DriveID];

        pDrive->Flags |= DISKIMAGE_DISK_DIRTY;

#ifdef BE_SAFE
        if (pDrive->NumSides==1)
        {
                PhysicalSide = 0;
        }
#endif

        switch (pDrive->Type)
        {
                case STANDARD_DISK_IMAGE:
                        return;
                case EXTENDED_DISK_IMAGE:
                        return;
                case INTERNAL_DISK_IMAGE:
                        ExtDskInternal_EmptyTrack(pDrive, PhysicalTrack, PhysicalSide);
                        return;

        }
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

void    DiskImage_WriteImage(int DriveID)
{
        DISKIMAGE_UNIT  *pDrive = &Drives[DriveID];

        switch (pDrive->Type)
        {
        case STANDARD_DISK_IMAGE:
                Dsk_WriteImage(pDrive);
                return;
        case EXTENDED_DISK_IMAGE:
                ExtDsk_WriteImage(pDrive);
                return;
        case INTERNAL_DISK_IMAGE:
                ExtDskInternal_WriteImage(pDrive);
                return;
        }
}
