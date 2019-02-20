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
/* Code specific to disk images */

/* Standard disk image code */


#include "diskimg.h"
#include "dsk.h"
#include "../host.h"

static int	DskGetSectorSize(int);


/* load and validate a disk image */
/* checks are done so that the chances of a crash resulting from a bad disk */
/* image are reduced or eliminated */
int		Dsk_Validate(char *pFilename)
{
	unsigned long	DiskImageSize;
	unsigned char			*pDiskImage;
	BOOL			ValidImage = FALSE;

	Host_LoadFile(pFilename, &pDiskImage, &DiskImageSize);

	if (pDiskImage!=NULL)
	{
		DSKHEADER *pHeader = (DSKHEADER *)pDiskImage;
		
		if (memcmp(pHeader,"MV - CPC",8)==0)
		{
			/* has main header */

			int TrackSize;
			int	TotalTracks;

			if ((pHeader->NumSides==1) || (pHeader->NumSides==2))
			{
				/* 1 or 2 sides */

				if ((pHeader->NumTracks>1) && (pHeader->NumTracks<85))
				{
					TrackSize = (pHeader->TrackSizeHigh<<8) | (pHeader->TrackSizeLow);

					TotalTracks = (pHeader->NumTracks * pHeader->NumSides);
					
					/* if size is same as or greater to the size calculated, we should be ok */
					if (DiskImageSize>=((TrackSize*TotalTracks)+sizeof(DSKHEADER)))
					{
						/* correct size image */
						int	i;
						DSKTRACKHEADER *pTrack = (DSKTRACKHEADER *)((int)pHeader + sizeof(DSKHEADER));

						for (i=0; i<TotalTracks; i++)
						{
							int thisTrackSize;

							/* check track header text is present */
							if (memcmp(pTrack->TrackHeader,"Track-Info",10)!=0)
								break;

							/* size of this track, is sector size * sectors per track */
							thisTrackSize = DskGetSectorSize(pTrack->BPS)*pTrack->SPT + sizeof(DSKTRACKHEADER);

							/* is calculated track size greater than global track size */
							/* in a DSK, each track must be the same size or smaller than */
							/* the global track size */
							if (thisTrackSize>TrackSize)
								break;

							pTrack = (DSKTRACKHEADER *)((int)pTrack + TrackSize);
						}

						if (i==TotalTracks)
							ValidImage = TRUE;
					}
				}
			}
		}

		free(pDiskImage);
	}

	return ValidImage;
}

int	Dsk_Initialise(DISKIMAGE_UNIT *pDskUnit)
{	
DSKHEADER	*pDskHeader;

	/* read header into memory */
	memcpy(pDskUnit->Header, pDskUnit->pDiskImage, sizeof(DSKHEADER));
	/* identify if disk image is standard or extended */
		
	pDskHeader = (DSKHEADER *)pDskUnit->Header;
		
	/* Setup data for disk image format found */
	
	pDskUnit->TrackSize = (pDskHeader->TrackSizeHigh<<8)+pDskHeader->TrackSizeLow;
	pDskUnit->NumSides = pDskHeader->NumSides;
	pDskUnit->NumTracks = pDskHeader->NumTracks;
	pDskUnit->pTrackSize = NULL;
	pDskUnit->CurrentTrack = -1;
	pDskUnit->CurrentSide = -1;

	return TRUE;	
}	

void	Dsk_Free(DISKIMAGE_UNIT *pDskUnit)
{
}

static void	GetTrackHeader(DISKIMAGE_UNIT *pDskUnit,int Track,int Side)
{
	if ((pDskUnit->CurrentTrack==Track) && (pDskUnit->CurrentSide==Side))
		return;

	/* calculate offset to track */
	pDskUnit->TrackOffset=((Track*pDskUnit->NumSides)*pDskUnit->TrackSize) + sizeof(DSKHEADER);

	/* add on side 0 or 1 */
	pDskUnit->TrackOffset+=pDskUnit->TrackSize*Side;
	
	memcpy(&pDskUnit->Header[0], ((unsigned char *)pDskUnit->pDiskImage + pDskUnit->TrackOffset), sizeof(DSKTRACKHEADER));

	pDskUnit->CurrentTrack=Track;
	pDskUnit->CurrentSide = Side;
}

/* Return sector size from N value in sector ID */

static int	DskGetSectorSize(int N)
{
	if (N<8)
		return (1<<N)<<7;	
	else
		return 0x0200;
}


int	Dsk_GetSectorsPerTrack(DISKIMAGE_UNIT *pDrive,int PhysicalTrack, int PhysicalSide)
{

	DSKTRACKHEADER	*pTrackHeader = (DSKTRACKHEADER *)pDrive->Header;

	if (PhysicalTrack<pDrive->NumTracks)
	{
		GetTrackHeader(pDrive,PhysicalTrack,PhysicalSide);

		return pTrackHeader->SPT & 0x0ff;
	}
	else
	{
		return 0;
	}

}

void Dsk_GetID(DISKIMAGE_UNIT *pDrive,int PhysicalTrack,int PhysicalSide,int Index,FDC_CHRN *pCHRN)
{

	DSKTRACKHEADER	*pTrackHeader = (DSKTRACKHEADER *)pDrive->Header;
	DSKCHRN			*pDskCHRN;

	if (PhysicalTrack<pDrive->NumTracks)
	{
		GetTrackHeader(pDrive,PhysicalTrack,PhysicalSide);

		Index = Index % pTrackHeader->SPT;

		pDskCHRN = (DSKCHRN *)&pTrackHeader->SectorIDs[Index];
		
		pCHRN->C = pDskCHRN->C & 0x0ff;
		pCHRN->H = pDskCHRN->H & 0x0ff;
		pCHRN->R = pDskCHRN->R & 0x0ff;
		pCHRN->N = pDskCHRN->N & 0x0ff;
		pCHRN->ST1 = pDskCHRN->ST1 & 0x0ff;
		pCHRN->ST2 = pDskCHRN->ST2 & 0x0ff;
	}
}

void Dsk_GetSector(DISKIMAGE_UNIT *pDrive,int PhysicalTrack,int PhysicalSide,int Index,char *pData)
{
	int	SectorPos, SectorSize;
	DSKTRACKHEADER *pDskTrackHeader;

	if (PhysicalTrack<pDrive->NumTracks)
	{
		GetTrackHeader(pDrive,PhysicalTrack,PhysicalSide);

		pDskTrackHeader = (DSKTRACKHEADER *)&pDrive->Header[0];

		SectorPos = Index;

		SectorSize = DskGetSectorSize(pDskTrackHeader->BPS);

		pDrive->SectorOffset = pDrive->TrackOffset + sizeof(DSKTRACKHEADER) + (SectorSize*SectorPos);

		memcpy(pData, ((unsigned char *)pDrive->pDiskImage + pDrive->SectorOffset), SectorSize);
	}
}

void Dsk_PutSector(DISKIMAGE_UNIT *pDrive,int PhysicalTrack,int PhysicalSide,int Index,char *pData)
{
	int	SectorPos, SectorSize;
	DSKTRACKHEADER *pDskTrackHeader;

	if (PhysicalTrack<pDrive->NumTracks)
	{

		GetTrackHeader(pDrive,PhysicalTrack,PhysicalSide);

		pDskTrackHeader = (DSKTRACKHEADER *)&pDrive->Header[0];

		SectorPos = Index;

		SectorSize = DskGetSectorSize(pDskTrackHeader->BPS);

		pDrive->SectorOffset = pDrive->TrackOffset + sizeof(DSKTRACKHEADER) + (SectorSize*SectorPos);

		memcpy(((unsigned char *)pDrive->pDiskImage + pDrive->SectorOffset), pData, SectorSize);
	}
}


void	Dsk_WriteImage(DISKIMAGE_UNIT *pDrive)
{
	HOST_FILE_HANDLE fh;	
	
	fh = Host_OpenFile(pDrive->Filename, HOST_FILE_ACCESS_WRITE);

	if (fh!=0)
	{
		Host_WriteData(fh, pDrive->pDiskImage, pDrive->DiskImageSize);
		
		Host_CloseFile(fh);

		pDrive->Flags &= ~DISKIMAGE_DISK_DIRTY;
	}
}
