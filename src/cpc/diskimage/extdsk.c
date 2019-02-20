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


/* Extended disk image code */

#include "extdsk.h"
#include "../host.h"

static int      GetSectorSize(int);

/* load and validate a extended disk image */
/* checks are done so that the chances of a crash resulting from a bad disk */
/* image are reduced or eliminated */

int		ExtDsk_Validate(char *pFilename)
{
	unsigned long	DiskImageSize;
	unsigned char			*pDiskImage;
	BOOL			ValidImage = FALSE;

	Host_LoadFile(pFilename, &pDiskImage, &DiskImageSize);

	if (pDiskImage!=NULL)
	{
		EXTDSKHEADER *pHeader = (EXTDSKHEADER *)pDiskImage;
		
		if (memcmp(pHeader,"EXTENDED",8)==0)
		{
			/* has main header */

			int TotalTracks;
			int CurrentSize;

			if ((pHeader->NumSides==1) || (pHeader->NumSides==2))
			{
				/* single or double sided */

				if ((pHeader->NumTracks>0) && (pHeader->NumTracks<85))
				{
					int i;

					/* between 1 and 84 tracks in image */

					/* check if disk image has enough data in it */
					TotalTracks = (pHeader->NumTracks * pHeader->NumSides);

					CurrentSize = 0;
					
					for (i=0; i<TotalTracks; i++)
					{
						/* does track exist in image? */
						if (pHeader->TrackSizeTable[i] != 0)
						{
      						CurrentSize += (int)((pHeader->TrackSizeTable[i] & 0x0ff)<<8);
						}
					}       
		
					if (DiskImageSize>=(CurrentSize+sizeof(EXTDSKHEADER)))
					{
						/* correct size image */
				
						CurrentSize = 0;

						for (i=0; i<TotalTracks; i++)
						{
							/* is track header correct? */

							/* does track exist in image? */
							if (pHeader->TrackSizeTable[i] != 0)
							{
								EXTDSKTRACKHEADER *pTrack;
								int calculatedTrackSize, thisTrackSize,j;

								thisTrackSize = 0;

								pTrack = (EXTDSKTRACKHEADER *)((int)pHeader + CurrentSize + sizeof(EXTDSKTRACKHEADER));

								/* check track header text is present */
								if (memcmp(pTrack->TrackHeader,"Track-Info",10)!=0)
									break;

								/* calculate track size */
								calculatedTrackSize = sizeof(EXTDSKTRACKHEADER);

								for (j=0; j<pTrack->SPT; j++)
								{
									thisTrackSize += GetSectorSize(pTrack->SectorIDs[j].N);
								}

								thisTrackSize = (int)((pHeader->TrackSizeTable[i] & 0x0ff)<<8);
							
								if (calculatedTrackSize>thisTrackSize)
									break;

								CurrentSize += thisTrackSize;

							}
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


int     ExtDsk_Initialise(DISKIMAGE_UNIT *pDskUnit)
{       
int     i;
int     TrackOffset;
EXTDSKHEADER    *pDskHeader;
        
        /* read header into memory */
		memcpy(pDskUnit->Header, pDskUnit->pDiskImage, sizeof(EXTDSKHEADER));

        /* Setup data for disk image format found */

        TrackOffset = sizeof(EXTDSKHEADER);
        
        pDskHeader = (EXTDSKHEADER *)pDskUnit->Header;

        pDskUnit->pTrackSize = (int *)malloc(sizeof(int)*(pDskHeader->NumTracks*pDskHeader->NumSides));

        for (i=0; i<(pDskHeader->NumTracks*pDskHeader->NumSides); i++)
        {
                if (pDskHeader->TrackSizeTable[i] != 0)
                {
                
                        pDskUnit->pTrackSize[i] = TrackOffset;
                        TrackOffset += (int)((pDskHeader->TrackSizeTable[i] & 0x0ff)<<8);
                }
                else
                {
                        pDskUnit->pTrackSize[i] = 0;
                }
        }       

        pDskUnit->NumSides = pDskHeader->NumSides & 0x07f;
		pDskUnit->NumTracks = pDskHeader->NumTracks;
		pDskUnit->CurrentTrack = -1;
		pDskUnit->CurrentSide = -1;

        return TRUE;    
}       

void	ExtDsk_Free(DISKIMAGE_UNIT *pDskUnit)
{
	if (pDskUnit->pTrackSize!=NULL)
	{
		free(pDskUnit->pTrackSize);
		pDskUnit->pTrackSize = NULL;
	
	}
}

static void     GetTrackHeader(DISKIMAGE_UNIT *pDskUnit,int Track,int Side)
{
	if ((pDskUnit->CurrentTrack==Track) && (pDskUnit->CurrentSide==Side))
		return;
 
	pDskUnit->TrackOffset=pDskUnit->pTrackSize[(Track*pDskUnit->NumSides)+Side];
        
	memcpy(pDskUnit->Header, ((unsigned char *)pDskUnit->pDiskImage + pDskUnit->TrackOffset), sizeof(EXTDSKTRACKHEADER));

	pDskUnit->CurrentTrack=Track;
	pDskUnit->CurrentSide = Side;
}

/* Return sector size from N value in sector ID */

static int      GetSectorSize(int N)
{
        if (N<8)
			return (1<<N)<<7;	
        else
                return 0x0200;
}


int	ExtDsk_GetSectorsPerTrack(DISKIMAGE_UNIT *pDrive,int PhysicalTrack, int PhysicalSide)
{
	EXTDSKTRACKHEADER	*pTrackHeader = (EXTDSKTRACKHEADER *)pDrive->Header;

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

void ExtDsk_GetID(DISKIMAGE_UNIT *pDrive,int PhysicalTrack,int PhysicalSide,int Index,FDC_CHRN *pCHRN)
{
	EXTDSKTRACKHEADER	*pTrackHeader = (EXTDSKTRACKHEADER *)pDrive->Header;
	EXTDSKCHRN			*pExtDskCHRN;

	if (PhysicalTrack<pDrive->NumTracks)
	{
		GetTrackHeader(pDrive,PhysicalTrack,PhysicalSide);

		Index = Index % pTrackHeader->SPT;

		pExtDskCHRN = (EXTDSKCHRN *)&pTrackHeader->SectorIDs[Index];

			
		pCHRN->C = pExtDskCHRN->C & 0x0ff;
		pCHRN->H = pExtDskCHRN->H & 0x0ff;
		pCHRN->R = pExtDskCHRN->R & 0x0ff;
		pCHRN->N = pExtDskCHRN->N & 0x0ff;
		pCHRN->ST1 = pExtDskCHRN->ST1 & 0x0ff;
		pCHRN->ST2 = pExtDskCHRN->ST2 & 0x0ff;
	}
}

void ExtDsk_GetSector(DISKIMAGE_UNIT *pDrive,int PhysicalTrack,int PhysicalSide,int Index,char *pData)
{
int     SectorPos;
EXTDSKTRACKHEADER *pDskTrackHeader;
int     i;
int     SectorOffset;
int     SectorSize;

        /* Check sector exists */
		if (PhysicalTrack<pDrive->NumTracks)
		{    
			GetTrackHeader(pDrive,PhysicalTrack,PhysicalSide); 

			pDskTrackHeader = (EXTDSKTRACKHEADER *)&pDrive->Header[0];

			SectorPos = Index;
			SectorSize = ((pDskTrackHeader->SectorIDs[SectorPos].SectorSizeHigh & 0x0ff)<<8)|(pDskTrackHeader->SectorIDs[SectorPos].SectorSizeLow & 0x0ff);

			SectorOffset = 0x100;
        
			for (i=0; i<SectorPos; i++)
			{
					SectorOffset +=((pDskTrackHeader->SectorIDs[i].SectorSizeHigh & 0x0ff)<<8)|(pDskTrackHeader->SectorIDs[i].SectorSizeLow & 0x0ff);
			}
        
			SectorOffset += pDrive->TrackOffset;

		memcpy(pData, (unsigned char *)pDrive->pDiskImage + SectorOffset, SectorSize);

	}
}

void ExtDsk_PutSector(DISKIMAGE_UNIT *pDrive,int PhysicalTrack,int PhysicalSide,int Index,char *pData)
{
int     SectorPos;
EXTDSKTRACKHEADER *pDskTrackHeader;
int     i;
int     SectorOffset;
int     SectorSize;

		if (PhysicalTrack<pDrive->NumTracks)
		{
			/* Check sector exists */
        
			GetTrackHeader(pDrive,PhysicalTrack,PhysicalSide); 

			pDskTrackHeader = (EXTDSKTRACKHEADER *)&pDrive->Header[0];

			SectorPos = Index;
			SectorSize = ((pDskTrackHeader->SectorIDs[SectorPos].SectorSizeHigh & 0x0ff)<<8)|(pDskTrackHeader->SectorIDs[SectorPos].SectorSizeLow & 0x0ff);

			SectorOffset = 0x100;
        
			for (i=0; i<SectorPos; i++)
			{
					SectorOffset +=((pDskTrackHeader->SectorIDs[i].SectorSizeHigh & 0x0ff)<<8)|(pDskTrackHeader->SectorIDs[i].SectorSizeLow & 0x0ff);
			}
        
			SectorOffset += pDrive->TrackOffset;

			memcpy(((unsigned char *)pDrive->pDiskImage + SectorOffset), pData, SectorSize);

	}
}



void	ExtDsk_WriteImage(DISKIMAGE_UNIT *pDrive)
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

