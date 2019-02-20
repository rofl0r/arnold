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
/* Internal way to store Extended disk images which allows formatting of tracks */

#include "iextdsk.h"
#include "extdsk.h"
#include "dsk.h"
#include "../host.h"


static int	GetSectorSize(int N)
{
	if (N<=5)
		return (1<<N)<<7;	
	else
		return 0x01800;	
}

/* create a new one ready for use */
EXTDSK_INTERNAL *ExtDskInternal_New(void)
{
        EXTDSK_INTERNAL *pExtDsk;

        /* allocate main header */
        pExtDsk = (EXTDSK_INTERNAL *)malloc(sizeof(EXTDSK_INTERNAL));

        if (pExtDsk!=NULL)
        {
                memset(pExtDsk, 0, sizeof(EXTDSK_INTERNAL));

                pExtDsk->pTrackList = (EXTDSK_INTERNAL_TRACK **)malloc(sizeof(EXTDSK_INTERNAL_TRACK *)*85*2);
        
                if (pExtDsk->pTrackList!=0)
                {
                        int i;

                        for (i=0; i<(85*2); i++)
                        {
                                pExtDsk->pTrackList[i] = NULL;
                        }
                
                }

        }

        return pExtDsk;
}


int     ExtDskInternal_Initialise(DISKIMAGE_UNIT *pDskUnit)
{
        
        EXTDSK_INTERNAL *pExtDsk =ExtDskInternal_New();
        
        pDskUnit->pDiskImage = (unsigned char *)pExtDsk;

        return TRUE;
}

void    ExtDskInternal_Free(DISKIMAGE_UNIT *pUnit)
{
        EXTDSK_INTERNAL *pExtDsk = (EXTDSK_INTERNAL *)pUnit->pDiskImage;

        if (pExtDsk!=NULL)
        {
                if (pExtDsk->pTrackList!=NULL)
                {
                        int i;

                        for (i=0; i<85*2; i++)
                        {
                                if (pExtDsk->pTrackList[i]!=NULL)
                                {
                                        int j;
                                        EXTDSK_INTERNAL_TRACK *pTrack;

                                        pTrack = pExtDsk->pTrackList[i];
                                
                                        for (j=0; j<pTrack->NoOfSectors; j++)
                                        {
                                                if (pTrack->pSectorData[j]!=NULL)
                                                {
                                                        free(pTrack->pSectorData[j]);
                                                }
                                        }
                                        
                                        free(pTrack);
                                }
                        }
                
                        free(pExtDsk->pTrackList);
                }

                free(pExtDsk);
        }
}


void    ExtDskInternal_AddTrack(EXTDSK_INTERNAL *pExtDsk, int TrackIndex)
{
        if (pExtDsk!=NULL)
        {
                if (pExtDsk->pTrackList!=NULL)
                {
                        if (pExtDsk->pTrackList[TrackIndex]==NULL)
                        {
                                pExtDsk->pTrackList[TrackIndex] = malloc(sizeof(EXTDSK_INTERNAL_TRACK));

                                if (pExtDsk->pTrackList[TrackIndex]!=NULL)
                                {
                                        memset(pExtDsk->pTrackList[TrackIndex], 0, sizeof(EXTDSK_INTERNAL_TRACK));
                                }
                        }
                }
        }
}

void	ExtDskInternal_RemoveSectorsInTrack(EXTDSK_INTERNAL *pExtDsk, int TrackIndex)
{
	if (pExtDsk!=NULL)
	{
		if (pExtDsk->pTrackList!=NULL)
		{
			if (pExtDsk->pTrackList[TrackIndex] != NULL)
			{
				int i;
				EXTDSK_INTERNAL_TRACK *pTrack;

				pTrack = pExtDsk->pTrackList[TrackIndex];

				for (i=0; i<pTrack->NoOfSectors; i++)
				{
					if (pTrack->pSectorData[i]!=NULL)
					{
						free(pTrack->pSectorData[i]);
					}
				}

				pTrack->NoOfSectors = 0;
			}
		}
	}
}



void    ExtDskInternal_AddSectorToTrack(EXTDSK_INTERNAL *pExtDsk, int TrackIndex, EXTDSKCHRN *pCHRN, int FillerByte)
{
        EXTDSK_INTERNAL_TRACK *pTrack;

        /* add track if it doesn't exist already. */
        ExtDskInternal_AddTrack(pExtDsk, TrackIndex);

        if (pExtDsk!=NULL)
        {
                if (pExtDsk->pTrackList!=NULL)
                {
                        if (pExtDsk->pTrackList[TrackIndex]!=NULL)
                        {
							/* get pointer to track */
                            pTrack = pExtDsk->pTrackList[TrackIndex];


							if (pTrack->NoOfSectors<=0x03e)
							{
                                int SectorSize;

                                /* get size of data for this "N" */
                                SectorSize = GetSectorSize(pCHRN->N);
                            
                                /* allocate space for sector data based on N */
                                pTrack->pSectorData[pTrack->NoOfSectors] = malloc(SectorSize);

                                /* copy C, H, R, N. */
                                memcpy(&pTrack->SectorIDs[pTrack->NoOfSectors], pCHRN, sizeof(EXTDSKCHRN));

                                /* store size of actual data! */
                                pTrack->SectorIDs[pTrack->NoOfSectors].SectorSizeLow = (unsigned char)(SectorSize & 0x0ff);
                                pTrack->SectorIDs[pTrack->NoOfSectors].SectorSizeHigh = (unsigned char)((SectorSize>>8) & 0x0ff);

                                /* if we succeeded in allocating memory, increase number of sectors
                                 in track */
                                if (pTrack->pSectorData[pTrack->NoOfSectors]!=NULL)
                                {
                                        memset(pTrack->pSectorData[pTrack->NoOfSectors], FillerByte, SectorSize);
                                        pTrack->NoOfSectors++;
                                }
							}
                        }
                }
        }
}

char *ExtDskInternal_GetPointerToSectorData(EXTDSK_INTERNAL *pExtDsk, int TrackIndex, int SectorIndex)
{
        EXTDSK_INTERNAL_TRACK *pTrack;
        if (pExtDsk!=NULL)
        {
                if (pExtDsk->pTrackList!=NULL)
                {
                        if (pExtDsk->pTrackList[TrackIndex]!=NULL)
                        {
                                /* get pointer to track */
                                pTrack = pExtDsk->pTrackList[TrackIndex];

                                if (pTrack->NoOfSectors<SectorIndex)
                                        return NULL;

                                return pTrack->pSectorData[SectorIndex];
                        }
                }
        }
        
        return NULL;
}

EXTDSKCHRN *ExtDskInternal_GetSectorCHRN(EXTDSK_INTERNAL *pExtDsk, int TrackIndex, int SectorIndex)
{
        EXTDSK_INTERNAL_TRACK *pTrack;
        if (pExtDsk!=NULL)
        {
                if (pExtDsk->pTrackList!=NULL)
                {
                        if (pExtDsk->pTrackList[TrackIndex]!=NULL)
                        {
                                /* get pointer to track */
                                pTrack = pExtDsk->pTrackList[TrackIndex];

                                return &pTrack->SectorIDs[SectorIndex];
                        }
                }
        }
        
        return NULL;
}


int ExtDskInternal_GetSPT(EXTDSK_INTERNAL *pExtDsk, int TrackIndex)
{
        if (pExtDsk!=NULL)
        {
                if (pExtDsk->pTrackList!=NULL)
                {
                        if (pExtDsk->pTrackList[TrackIndex]!=NULL)
                        {
                                return pExtDsk->pTrackList[TrackIndex]->NoOfSectors;
                        }
                }
        }
        
        return 0;
}

int ExtDskInternal_GetSectorSize(EXTDSK_INTERNAL *pExtDsk, int TrackIndex, int SectorIndex)
{
        EXTDSK_INTERNAL_TRACK *pTrack;
        if (pExtDsk!=NULL)
        {
                if (pExtDsk->pTrackList!=NULL)
                {
                        if (pExtDsk->pTrackList[TrackIndex]!=NULL)
                        {
                                int SectorSize;

                                /* get pointer to track */
                                pTrack = pExtDsk->pTrackList[TrackIndex];

                                SectorSize = (pTrack->SectorIDs[SectorIndex].SectorSizeLow & 0x0ff) |
                                                ((pTrack->SectorIDs[SectorIndex].SectorSizeHigh & 0x0ff)<<8);

                                return SectorSize;
                        }
                }
        }
        
        return 0;
}



/* write ext dsk out to disk, creating a new extdsk from the data stored */
void    ExtDskInternal_WriteImage(DISKIMAGE_UNIT *pDrive)
{
        EXTDSK_INTERNAL *pExtDsk = (EXTDSK_INTERNAL *)pDrive->pDiskImage;
        int i;
        BOOL    DoubleSided = FALSE;
        int             NoOfTracks;
        EXTDSKHEADER    MainHeader;
        EXTDSKTRACKHEADER TrackHeader;
        HOST_FILE_HANDLE fh;

        if (pDrive->Filename!=NULL)
        {
                fh = Host_OpenFile(pDrive->Filename, HOST_FILE_ACCESS_WRITE);
        }
        else
        {
                fh = Host_OpenFile("newdisk.dsk",HOST_FILE_ACCESS_WRITE);
        }

        if (fh!=0)
        {
                /* clear the header */
                memset(&MainHeader, 0, sizeof(EXTDSKHEADER));
                
                /* check all pointers for second side to determine if it is double sided */
                for (i=1; i<85*2; i+=2)
                {
                        /* found a track that exists on the second side */
                        if (pExtDsk->pTrackList[i]!=NULL)
                        {
                                DoubleSided = TRUE;
                                break;
                        }
                }

                NoOfTracks = 0;

                /* count number of tracks in image */
                /* if either side 0 or side 1 has track data, then track is present */
                for (i=0; i<85; i++)
                {
                        if ((pExtDsk->pTrackList[(i<<1)]!=NULL) || (pExtDsk->pTrackList[(i<<1)+1]!=NULL))
                        {
                                NoOfTracks++;
                        }
                }

                /* set number of sides */
                if (DoubleSided)
                        MainHeader.NumSides = 2;
                else
                        MainHeader.NumSides = 1;

                /* set main header id text */
		strcpy(MainHeader.DskHeader, MainHeader_Text);

                /* write disk image creator */
                strcpy(MainHeader.DskCreator, ImageCreator_Text);

                /* set number of tracks */
                MainHeader.NumTracks = (unsigned char)NoOfTracks;

                /* build track sizes table */
                for (i=0; i<(MainHeader.NumTracks*MainHeader.NumSides); i++)
                {
                        int j;
                        int TrackSize = 0;
                        int TrackIndex;

                        if (MainHeader.NumSides == 1)
                        {
                                TrackIndex = i<<1;
                        }
                        else
                        {
                                TrackIndex = i;
                        }
                        
                        if (pExtDsk->pTrackList[TrackIndex]!=NULL)
                        {
                                EXTDSK_INTERNAL_TRACK *pTrack = pExtDsk->pTrackList[TrackIndex];

                                for (j=0; j<pTrack->NoOfSectors; j++)
                                {
                                        int SectorSize;

                                        SectorSize = (pTrack->SectorIDs[j].SectorSizeLow & 0x0ff) | ((pTrack->SectorIDs[j].SectorSizeHigh & 0x0ff)<<8);

                                        TrackSize += SectorSize;
                                }
                        
                                /* add size of header */
                                TrackSize += sizeof(EXTDSKTRACKHEADER);
                        }
                
                        /* store high byte of track size */
                        MainHeader.TrackSizeTable[i] = (unsigned char)((TrackSize>>8) & 0x0ff);
                }

                /* Main Header is complete */

                /* write main header to disk */
                Host_WriteData(fh, (unsigned char *)&MainHeader, sizeof(EXTDSKHEADER));

                for (i=0; i<(MainHeader.NumTracks*MainHeader.NumSides); i++)
                {
                        int TrackIndex;

                        if (MainHeader.NumSides == 1)
                        {
                                TrackIndex = i<<1;
                        }
                        else
                        {
                                TrackIndex = i;
                        }
                        
                        /* found a track */
                        if (pExtDsk->pTrackList[TrackIndex]!=NULL)
                        {
                                int NumSectors;
                                int s;
                                EXTDSK_INTERNAL_TRACK *thisTrack = pExtDsk->pTrackList[TrackIndex];

                                /* setup track header */
                                memset(&TrackHeader, 0, sizeof(EXTDSKTRACKHEADER));
                                strcpy(TrackHeader.TrackHeader, TrackHeader_Text);
                        
                                if (thisTrack->NoOfSectors>=29)
                                {
                                        NumSectors = 29;
                                }
                                else
                                {
                                        NumSectors = thisTrack->NoOfSectors;
                                }

                                if (MainHeader.NumSides == 1)
                                {
                                        TrackHeader.track = (unsigned char)i;
                                        TrackHeader.side = 0;
                                }
                                else
                                {
                                        TrackHeader.track = (unsigned char)(i>>1);
                                        TrackHeader.side = (unsigned char)(i & 1);
                                }
                                
                                TrackHeader.BPS = 2;
                                TrackHeader.SPT = (unsigned char)NumSectors;
                                TrackHeader.FillerByte = 0x0e5;
                                TrackHeader.Gap3 = 0x04a;

                                for (s=0; s<NumSectors; s++)
                                {
                                        /* write sector IDs to header */
                                        memcpy(&TrackHeader.SectorIDs[s], &thisTrack->SectorIDs[s], sizeof(EXTDSKCHRN));
                                }

                                /* write track header */
                                Host_WriteData(fh,(unsigned char *)&TrackHeader, sizeof(EXTDSKTRACKHEADER));
                        
                                for (s=0; s<NumSectors; s++)    
                                {
                                        /* calc sector size */
                                        int SectorSize = (thisTrack->SectorIDs[s].SectorSizeLow & 0x0ff) |
                                                                        (thisTrack->SectorIDs[s].SectorSizeHigh & 0x0ff)<<8;

                                        /* write sector data */
                                        Host_WriteData(fh, (unsigned char *)thisTrack->pSectorData[s], SectorSize);
                                }
                        }
                }

                /* close output file */
                Host_CloseFile(fh);
        }
}

void ExtDskInternal_GetID(DISKIMAGE_UNIT *pDrive,int PhysicalTrack,int PhysicalSide,int Index,FDC_CHRN *pCHRN)
{
        EXTDSK_INTERNAL *pExtDsk = (EXTDSK_INTERNAL *)pDrive->pDiskImage;
        EXTDSKCHRN *pExtDskCHRN;
                
        Index = Index % ExtDskInternal_GetSPT(pExtDsk, (PhysicalTrack<<1) | PhysicalSide);

        pExtDskCHRN = ExtDskInternal_GetSectorCHRN(pExtDsk, (PhysicalTrack<<1) | PhysicalSide, Index);
        
        pCHRN->C = pExtDskCHRN->C & 0x0ff;
        pCHRN->H = pExtDskCHRN->H & 0x0ff;
        pCHRN->R = pExtDskCHRN->R & 0x0ff;
        pCHRN->N = pExtDskCHRN->N & 0x0ff;
        pCHRN->ST1 = pExtDskCHRN->ST1 & 0x0ff;
        pCHRN->ST2 = pExtDskCHRN->ST2 & 0x0ff;
}

int     ExtDskInternal_GetSectorsPerTrack(DISKIMAGE_UNIT *pDrive,int PhysicalTrack, int PhysicalSide)
{
        EXTDSK_INTERNAL *pExtDsk = (EXTDSK_INTERNAL *)pDrive->pDiskImage;

        return ExtDskInternal_GetSPT(pExtDsk, (PhysicalTrack<<1) | PhysicalSide);
}


void ExtDskInternal_GetSector(DISKIMAGE_UNIT *pDrive,int PhysicalTrack,int PhysicalSide,int Index,char *pData)
{
        EXTDSK_INTERNAL *pExtDsk = (EXTDSK_INTERNAL *)pDrive->pDiskImage;

        int SectorSize = ExtDskInternal_GetSectorSize(pExtDsk, (PhysicalTrack<<1) | PhysicalSide, Index);
        char *pSectorData = ExtDskInternal_GetPointerToSectorData(pExtDsk, (PhysicalTrack<<1) | PhysicalSide, Index);

        if (pSectorData!=NULL)
        {
                memcpy(pData, (unsigned char *)pSectorData, SectorSize);
        }
}

void ExtDskInternal_PutSector(DISKIMAGE_UNIT *pDrive,int PhysicalTrack,int PhysicalSide,int Index,char *pData)
{
        EXTDSK_INTERNAL *pExtDsk = (EXTDSK_INTERNAL *)pDrive->pDiskImage;

        int SectorSize = ExtDskInternal_GetSectorSize(pExtDsk, (PhysicalTrack<<1) | PhysicalSide, Index);
        char *pSectorData = ExtDskInternal_GetPointerToSectorData(pExtDsk, (PhysicalTrack<<1) | PhysicalSide, Index);

        if (pSectorData!=NULL)
        {
                memcpy((unsigned char *)pSectorData, pData, SectorSize);
        }
}

void    ExtDskInternal_AddSector(DISKIMAGE_UNIT *pDrive, int PhysicalTrack, int PhysicalSide, FDC_CHRN *pCHRN, int FillerByte)
{
        EXTDSK_INTERNAL *pExtDsk = (EXTDSK_INTERNAL *)pDrive->pDiskImage;

        EXTDSKCHRN LocalCHRN;


        LocalCHRN.C = (unsigned char)pCHRN->C;
        LocalCHRN.H = (unsigned char)pCHRN->H;
        LocalCHRN.R = (unsigned char)pCHRN->R;
        LocalCHRN.N = (unsigned char)pCHRN->N;
        LocalCHRN.ST1 = 0;
        LocalCHRN.ST2 = 0;


        ExtDskInternal_AddSectorToTrack(pExtDsk, (PhysicalTrack<<1) | PhysicalSide, &LocalCHRN,FillerByte);
}

void	ExtDskInternal_EmptyTrack(DISKIMAGE_UNIT *pDrive, int PhysicalTrack, int PhysicalSide)
{
    EXTDSK_INTERNAL *pExtDsk = (EXTDSK_INTERNAL *)pDrive->pDiskImage;

	ExtDskInternal_RemoveSectorsInTrack(pExtDsk, (PhysicalTrack<<1) | PhysicalSide);
}


void            ExtDskInternal_Dsk2ExtDskInternal(DISKIMAGE_UNIT *pUnit)
{
        unsigned long   DiskImageSize;
        unsigned char                   *pDiskImage;
        EXTDSK_INTERNAL *pExtDsk = NULL;

        Host_LoadFile(pUnit->Filename, &pDiskImage, &DiskImageSize);

        if (pDiskImage!=NULL)
        {

                pExtDsk = ExtDskInternal_New();

                pUnit->pDiskImage = (unsigned char *)pExtDsk;

                if (pExtDsk!=NULL)
                {
                        int i;
                        DSKHEADER *pHeader = (DSKHEADER *)pDiskImage;
                        int TrackSize;
                        int     TotalTracks;
                        DSKTRACKHEADER *pTrack = (DSKTRACKHEADER *)((int)pHeader + sizeof(DSKHEADER));

                        TrackSize = (pHeader->TrackSizeHigh<<8) | (pHeader->TrackSizeLow);
                        TotalTracks = (pHeader->NumTracks * pHeader->NumSides);
                                        

                        for (i=0; i<TotalTracks; i++)
                        {
                                int j;
                                int TrackIndex;

                                if (pHeader->NumSides == 1)
                                {
                                        TrackIndex = (i<<1);
                                }
                                else
                                {
                                        TrackIndex = i;
                                }


                                ExtDskInternal_AddTrack(pExtDsk, TrackIndex);

                                for (j=0; j<pTrack->SPT; j++)
                                {
                                        char *Sector;
                                        EXTDSKCHRN LocalCHRN;
                                        char *pSectorData;
                                        int SectorSize = GetSectorSize(pTrack->SectorIDs[j].N);

                                        LocalCHRN.C = pTrack->SectorIDs[j].C;
                                        LocalCHRN.H = pTrack->SectorIDs[j].H;
                                        LocalCHRN.R = pTrack->SectorIDs[j].R;
                                        LocalCHRN.N = pTrack->SectorIDs[j].N;
                                        LocalCHRN.ST1 = pTrack->SectorIDs[j].ST1;
                                        LocalCHRN.ST2 = pTrack->SectorIDs[j].ST2;

                                        ExtDskInternal_AddSectorToTrack(pExtDsk, TrackIndex, &LocalCHRN,pTrack->FillerByte);

                                        pSectorData = ExtDskInternal_GetPointerToSectorData(pExtDsk, TrackIndex, j);

                                        Sector = (char *)pTrack + sizeof(DSKTRACKHEADER) + SectorSize*j;
          
										memcpy(pSectorData, Sector, SectorSize);
                                }
                                
                                pTrack = (DSKTRACKHEADER *)((int)pTrack + TrackSize);
                        }
                }

                free(pDiskImage);
        }
}


void            ExtDskInternal_ExtDsk2ExtDskInternal(DISKIMAGE_UNIT *pUnit)
{
        unsigned long   DiskImageSize;
        unsigned char                   *pDiskImage;
        EXTDSK_INTERNAL *pExtDsk = NULL;

        Host_LoadFile(pUnit->Filename, &pDiskImage, &DiskImageSize);

        if (pDiskImage!=NULL)
        {

                pExtDsk = ExtDskInternal_New();

                pUnit->pDiskImage = (unsigned char *)pExtDsk;

                if (pExtDsk!=NULL)
                {
                        int i;
                        EXTDSKHEADER *pHeader = (EXTDSKHEADER *)pDiskImage;
                        int     TotalTracks;
                        EXTDSKTRACKHEADER *pTrack = (EXTDSKTRACKHEADER *)((int)pHeader + sizeof(EXTDSKHEADER));

                        TotalTracks = (pHeader->NumTracks * pHeader->NumSides);
                                        
                        for (i=0; i<TotalTracks; i++)
                        {
                                int j;
                                int TrackIndex;

                                if (pHeader->NumSides == 1)
                                {
                                        TrackIndex = (i<<1);
                                }
                                else
                                {
                                        TrackIndex = i;
                                }

                                if (pHeader->TrackSizeTable[i]!=0)
                                {
                                        char *Sector = (char *)((char *)pTrack + sizeof(EXTDSKTRACKHEADER));

                                        /* track has data available */

                                        ExtDskInternal_AddTrack(pExtDsk, TrackIndex);

                                        for (j=0; j<pTrack->SPT; j++)
                                        {
                                                EXTDSKCHRN LocalCHRN;
                                                char *pSectorData;
                                                int SectorSize = GetSectorSize(pTrack->SectorIDs[j].N);

                                                LocalCHRN.C = pTrack->SectorIDs[j].C;
                                                LocalCHRN.H = pTrack->SectorIDs[j].H;
                                                LocalCHRN.R = pTrack->SectorIDs[j].R;
                                                LocalCHRN.N = pTrack->SectorIDs[j].N;
                                                LocalCHRN.ST1 = pTrack->SectorIDs[j].ST1;
                                                LocalCHRN.ST2 = pTrack->SectorIDs[j].ST2;

                                                ExtDskInternal_AddSectorToTrack(pExtDsk, TrackIndex, &LocalCHRN,pTrack->FillerByte);

                                                pSectorData = ExtDskInternal_GetPointerToSectorData(pExtDsk, TrackIndex, j);

                                                SectorSize = (pTrack->SectorIDs[j].SectorSizeLow & 0x0ff) | 
                                                        ((pTrack->SectorIDs[j].SectorSizeHigh & 0x0ff)<<8);

                                                memcpy(pSectorData, Sector, SectorSize);
        
                                                Sector += SectorSize;
        
                                        }
                                
                                        pTrack = (EXTDSKTRACKHEADER *)((int)pTrack + ((pHeader->TrackSizeTable[i] & 0x0ff)<<8));
                                }
                        }
                }

                free(pDiskImage);
        }
}
