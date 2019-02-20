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
#include <stdint.h>

#ifndef max
#define max(a,b) ((a<b) ? b : a )
#endif

static int	GetSectorSize(int N)
{
    return (1<<(N&0x07))<<7;
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



void    ExtDskInternal_AddSectorToTrack(EXTDSK_INTERNAL *pExtDsk, int TrackIndex, EXTDSKCHRN *pCHRN, int FillerByte, int nAllocationSize)
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
                                pTrack->pSectorData[pTrack->NoOfSectors] = malloc(nAllocationSize);

                                /* copy C, H, R, N. */
                                memcpy(&pTrack->SectorIDs[pTrack->NoOfSectors], pCHRN, sizeof(EXTDSKCHRN));

                                /* store size of actual data! */
                                pTrack->SectorIDs[pTrack->NoOfSectors].SectorSizeLow = (unsigned char)(SectorSize & 0x0ff);
                                pTrack->SectorIDs[pTrack->NoOfSectors].SectorSizeHigh = (unsigned char)((SectorSize>>8) & 0x0ff);

                                /* if we succeeded in allocating memory, increase number of sectors
                                 in track */
                                if (pTrack->pSectorData[pTrack->NoOfSectors]!=NULL)
                                {
                                        memset(pTrack->pSectorData[pTrack->NoOfSectors], FillerByte, nAllocationSize);
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

/* build header */
void ExtDskInternal_BuildHeader(DISKIMAGE_UNIT *pDrive, EXTDSKHEADER *pHeader)
{
	int i;
        EXTDSK_INTERNAL *pExtDsk = (EXTDSK_INTERNAL *)pDrive->pDiskImage;
	BOOL    DoubleSided = FALSE;
	int LastUsedTrack;

    /* clear the header */
    memset(pHeader, 0, sizeof(EXTDSKHEADER));

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

	LastUsedTrack = -1;

	/* find last written track; this is the number of tracks described in whole
	image */
    for (i=84; i>=0; i--)
    {
		/* is side 0 or side 1 written? */
        if ((pExtDsk->pTrackList[(i<<1)]!=NULL) || (pExtDsk->pTrackList[(i<<1)+1]!=NULL))
        {
			/* yes, then this is the last track, quit out here */
			LastUsedTrack = i;
			break;
        }
    }

    /* set number of sides */
    if (DoubleSided)
            pHeader->NumSides = 2;
    else
            pHeader->NumSides = 1;

    /* set main header id text */
	strcpy(pHeader->DskHeader, MainHeader_Text);

    /* write disk image creator */
    strcpy(pHeader->DskCreator, ImageCreator_Text);

	if (LastUsedTrack==-1)
	{
		/* set number of tracks */
		pHeader->NumTracks = (unsigned char)0;
	}
	else
	{
		/* set number of tracks */
		pHeader->NumTracks = (unsigned char)(LastUsedTrack+1);
	}

    /* build track sizes table */
    for (i=0; i<85*2; i++)
    {
        int j;
        int TrackSize = 0;

		if ((i & 1) && (pHeader->NumSides==1))
			continue;

        if (pExtDsk->pTrackList[i]!=NULL)
        {
                EXTDSK_INTERNAL_TRACK *pTrack = pExtDsk->pTrackList[i];

                for (j=0; j<pTrack->NoOfSectors; j++)
                {
                    int SectorSize;

                    SectorSize = (pTrack->SectorIDs[j].SectorSizeLow & 0x0ff) | ((pTrack->SectorIDs[j].SectorSizeHigh & 0x0ff)<<8);

                    TrackSize += SectorSize;
                }

                /* add size of header */
                TrackSize += sizeof(EXTDSKTRACKHEADER);

				{
					int TrackIndex;

					if (pHeader->NumSides == 1)
					{
						TrackIndex = (i>>1);
					}
					else
					{
						TrackIndex = i;
					}
					/* round up track size to 256 byte and write out high byte */
					pHeader->TrackSizeTable[TrackIndex] = (unsigned char)(((TrackSize+255)>>8) & 0x0ff);
				}
		}
	}
}

unsigned long ExtDskInternal_CalculateOutputDataSize(DISKIMAGE_UNIT *pDrive)
{
	unsigned long nSize;
	int i;
	EXTDSKHEADER MainHeader;

	ExtDskInternal_BuildHeader(pDrive, &MainHeader);

	nSize = sizeof(EXTDSKHEADER);

	for (i=0; i<MainHeader.NumSides*MainHeader.NumTracks; i++)
	{
		nSize+=MainHeader.TrackSizeTable[i]<<8;
	}

	return nSize;


}


/* write ext dsk out to disk, creating a new extdsk from the data stored */
void    ExtDskInternal_GenerateOutputData(unsigned char *pBuffer, DISKIMAGE_UNIT *pDrive)
{
        EXTDSK_INTERNAL *pExtDsk = (EXTDSK_INTERNAL *)pDrive->pDiskImage;
        int i;
        EXTDSKHEADER    MainHeader;
        EXTDSKTRACKHEADER TrackHeader;

		ExtDskInternal_BuildHeader(pDrive, &MainHeader);

        /* Main Header is complete */

		memcpy(pBuffer, &MainHeader, sizeof(EXTDSKHEADER));
		pBuffer+=sizeof(EXTDSKHEADER);

        for (i=0; i<85*2; i++)
        {
                /* found a track */
                if (pExtDsk->pTrackList[i]!=NULL)
                {
						int Pad = 0;
						int TrackSize = 0;
                        int NumSectors;
                        int s;
                        EXTDSK_INTERNAL_TRACK *thisTrack = pExtDsk->pTrackList[i];

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

                        TrackHeader.SPT = (unsigned char)NumSectors;

						/* find largest N value, this will be stored in track header */
						/* the N parameter is setup like this for CPDRead created disc
						images, so we want to mimic those */
						{
							int MaxN=-1;

							for (s=0; s<NumSectors; s++)
							{
								if (s==0)
								{
									MaxN = thisTrack->SectorIDs[s].N;
								}
								else
								{
									if (thisTrack->SectorIDs[s].N>MaxN)
									{
										MaxN = thisTrack->SectorIDs[s].N;
									}
								}
							}

							TrackHeader.BPS = MaxN;
						}

                        TrackHeader.FillerByte = 0x0e5;
                        TrackHeader.Gap3 = 0x04a;

                        for (s=0; s<NumSectors; s++)
                        {
							int SectorSize;

							SectorSize = (thisTrack->SectorIDs[s].SectorSizeLow & 0x0ff) | ((thisTrack->SectorIDs[s].SectorSizeHigh & 0x0ff)<<8);

							TrackSize += SectorSize;
                            /* write sector IDs to header */
                            memcpy(&TrackHeader.SectorIDs[s], &thisTrack->SectorIDs[s], sizeof(EXTDSKCHRN));
                        }

                        /* add size of header */
                        TrackSize += sizeof(EXTDSKTRACKHEADER);

                        /* write track header */
						memcpy(pBuffer, &TrackHeader, sizeof(EXTDSKTRACKHEADER));
						pBuffer+=sizeof(EXTDSKTRACKHEADER);

                        for (s=0; s<NumSectors; s++)
                        {
                                /* calc sector size */
                                int SectorSize = (thisTrack->SectorIDs[s].SectorSizeLow & 0x0ff) |
                                                                (thisTrack->SectorIDs[s].SectorSizeHigh & 0x0ff)<<8;

								memcpy(pBuffer, thisTrack->pSectorData[s], SectorSize);
								pBuffer+=SectorSize;
						}

						if ((TrackSize & 0x0ff)!=0)
						{
							/* calculate padding required to make data a multiple of 256 bytes */
							Pad = 256 - (TrackSize & 0x0ff);

							memset(pBuffer, 0, Pad);
							pBuffer+=Pad;
						}
				}
        }
}



#if 0
/* write ext dsk out to disk, creating a new extdsk from the data stored */
void    ExtDskInternal_WriteImage(simple_expanding_buffer *pBuffer, DISKIMAGE_UNIT *pDrive)
{
        EXTDSK_INTERNAL *pExtDsk = (EXTDSK_INTERNAL *)pDrive->pDiskImage;
        int i;
        BOOL    DoubleSided = FALSE;
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
			int LastUsedTrack;

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

			LastUsedTrack = -1;

			/* find last written track; this is the number of tracks described in whole
			image */
            for (i=84; i>=0; i--)
            {
				/* is side 0 or side 1 written? */
                if ((pExtDsk->pTrackList[(i<<1)]!=NULL) || (pExtDsk->pTrackList[(i<<1)+1]!=NULL))
                {
					/* yes, then this is the last track, quit out here */
					LastUsedTrack = i;
					break;
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

			if (LastUsedTrack==-1)
			{
				/* set number of tracks */
				MainHeader.NumTracks = (unsigned char)0;
			}
			else
			{
				/* set number of tracks */
				MainHeader.NumTracks = (unsigned char)(LastUsedTrack+1);
			}

            /* build track sizes table */
            for (i=0; i<85*2; i++)
            {
                    int j;
                    int TrackSize = 0;

					if ((i & 1) && (MainHeader.NumSides==1))
						continue;

                    if (pExtDsk->pTrackList[i]!=NULL)
                    {
                            EXTDSK_INTERNAL_TRACK *pTrack = pExtDsk->pTrackList[i];

                            for (j=0; j<pTrack->NoOfSectors; j++)
                            {
                                int SectorSize;

                                SectorSize = (pTrack->SectorIDs[j].SectorSizeLow & 0x0ff) | ((pTrack->SectorIDs[j].SectorSizeHigh & 0x0ff)<<8);

                                TrackSize += SectorSize;
                            }

                            /* add size of header */
                            TrackSize += sizeof(EXTDSKTRACKHEADER);

							{
								int TrackIndex;

								if (MainHeader.NumSides == 1)
								{
									TrackIndex = (i>>1);
								}
								else
								{
									TrackIndex = i;
								}
								/* round up track size to 256 byte and write out high byte */
								MainHeader.TrackSizeTable[TrackIndex] = (unsigned char)(((TrackSize+255)>>8) & 0x0ff);
							}


					}

			}

            /* Main Header is complete */

            /* write main header to disk */
            Host_WriteData(fh, (unsigned char *)&MainHeader, sizeof(EXTDSKHEADER));

            for (i=0; i<85*2; i++)
            {
                    /* found a track */
                    if (pExtDsk->pTrackList[i]!=NULL)
                    {
							int j;
							int Pad = 0;
							int TrackSize = 0;
                            int NumSectors;
                            int s;
                            EXTDSK_INTERNAL_TRACK *thisTrack = pExtDsk->pTrackList[i];

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

                            TrackHeader.SPT = (unsigned char)NumSectors;

							/* find largest N value, this will be stored in track header */
							/* the N parameter is setup like this for CPDRead created disc
							images, so we want to mimic those */
							{
								int MaxN;

								for (s=0; s<NumSectors; s++)
								{
									if (s==0)
									{
										MaxN = thisTrack->SectorIDs[s].N;
									}
									else
									{
										if (thisTrack->SectorIDs[s].N>MaxN)
										{
											MaxN = thisTrack->SectorIDs[s].N;
										}
									}
								}

								TrackHeader.BPS = MaxN;
							}

                            TrackHeader.FillerByte = 0x0e5;
                            TrackHeader.Gap3 = 0x04a;

                            for (s=0; s<NumSectors; s++)
                            {
								int SectorSize;

								SectorSize = (thisTrack->SectorIDs[s].SectorSizeLow & 0x0ff) | ((thisTrack->SectorIDs[s].SectorSizeHigh & 0x0ff)<<8);

								TrackSize += SectorSize;
                                /* write sector IDs to header */
                                memcpy(&TrackHeader.SectorIDs[s], &thisTrack->SectorIDs[s], sizeof(EXTDSKCHRN));
                            }

                            /* add size of header */
                            TrackSize += sizeof(EXTDSKTRACKHEADER);

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

							if ((TrackSize & 0x0ff)!=0)
							{
								/* calculate padding required to make data a multiple of 256 bytes */
								Pad = 256 - (TrackSize & 0x0ff);

								/* if track size is not divisible by 256, then pad */
								for (j=0; j<Pad; j++)
								{
									unsigned char pad_byte = 0;

									Host_WriteData(fh, (unsigned char *)&pad_byte, sizeof(unsigned char));
								}
							}
                    }
            }


            /* close output file */
            Host_CloseFile(fh);
        }
}
#endif

void ExtDskInternal_GetID(DISKIMAGE_UNIT *pDrive,int PhysicalTrack,int PhysicalSide,int Index,CHRN *pCHRN)
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

void ExtDskInternal_PutSector(DISKIMAGE_UNIT *pDrive,int PhysicalTrack,int PhysicalSide,int Index,char *pData, int Mark)
{
        EXTDSK_INTERNAL *pExtDsk = (EXTDSK_INTERNAL *)pDrive->pDiskImage;
		int TrackIndex = (PhysicalTrack<<1)|PhysicalSide;
        EXTDSK_INTERNAL_TRACK *pTrack;

        int SectorSize = ExtDskInternal_GetSectorSize(pExtDsk, TrackIndex, Index);
        char *pSectorData = ExtDskInternal_GetPointerToSectorData(pExtDsk, TrackIndex, Index);

        if (pSectorData!=NULL)
        {
                memcpy((unsigned char *)pSectorData, pData, SectorSize);
        }

		if (pExtDsk!=NULL)
		{
			if (pExtDsk->pTrackList!=NULL)
			{
				if (pExtDsk->pTrackList[TrackIndex]!=NULL)
				{
					pTrack = pExtDsk->pTrackList[TrackIndex];

					if (Mark)
					{
						pTrack->SectorIDs[Index].ST2|=0x040;
					}
					else
					{
						pTrack->SectorIDs[Index].ST2&=~0x040;
					}
				}
			}
		}
}

void    ExtDskInternal_AddSector(DISKIMAGE_UNIT *pDrive, int PhysicalTrack, int PhysicalSide, CHRN *pCHRN, int FormatN, int FillerByte)
{
        EXTDSK_INTERNAL *pExtDsk = (EXTDSK_INTERNAL *)pDrive->pDiskImage;

        EXTDSKCHRN LocalCHRN;


        LocalCHRN.C = (unsigned char)pCHRN->C;
        LocalCHRN.H = (unsigned char)pCHRN->H;
        LocalCHRN.R = (unsigned char)pCHRN->R;
        LocalCHRN.N = (unsigned char)pCHRN->N;
        LocalCHRN.ST1 = 0;
        LocalCHRN.ST2 = 0;

		if (FormatN!=LocalCHRN.N)
		{
			/* data error in data field! */
			LocalCHRN.ST1|=0x020;
			LocalCHRN.ST2|=0x020;
		}

        ExtDskInternal_AddSectorToTrack(pExtDsk, (PhysicalTrack<<1) | PhysicalSide, &LocalCHRN,FillerByte, GetSectorSize(LocalCHRN.N));
}



void	ExtDskInternal_EmptyTrack(DISKIMAGE_UNIT *pDrive, int PhysicalTrack, int PhysicalSide)
{
    EXTDSK_INTERNAL *pExtDsk = (EXTDSK_INTERNAL *)pDrive->pDiskImage;

	ExtDskInternal_RemoveSectorsInTrack(pExtDsk, (PhysicalTrack<<1) | PhysicalSide);
}


void            ExtDskInternal_Dsk2ExtDskInternal(DISKIMAGE_UNIT *pUnit, const unsigned char *pDiskImage, const unsigned long DiskImageSize)
{
        EXTDSK_INTERNAL *pExtDsk = NULL;

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
                        DSKTRACKHEADER *pTrack = (DSKTRACKHEADER *)((uintptr_t)pHeader + sizeof(DSKHEADER));

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

                                        ExtDskInternal_AddSectorToTrack(pExtDsk, TrackIndex, &LocalCHRN,pTrack->FillerByte,GetSectorSize(LocalCHRN.N));

                                        pSectorData = ExtDskInternal_GetPointerToSectorData(pExtDsk, TrackIndex, j);

                                        Sector = (char *)pTrack + sizeof(DSKTRACKHEADER) + SectorSize*j;
          
										memcpy(pSectorData, Sector, SectorSize);
                                }
                                
                                pTrack = (DSKTRACKHEADER *)((uintptr_t)pTrack + TrackSize);
                        }
                }
        }
}


void            ExtDskInternal_ExtDsk2ExtDskInternal(DISKIMAGE_UNIT *pUnit, const unsigned char *pDiskImage, const unsigned long DiskImageSize)
{
        EXTDSK_INTERNAL *pExtDsk = NULL;

        if (pDiskImage!=NULL)
        {

                pExtDsk = ExtDskInternal_New();

                pUnit->pDiskImage = (unsigned char *)pExtDsk;

                if (pExtDsk!=NULL)
                {
                        int i;
                        EXTDSKHEADER *pHeader = (EXTDSKHEADER *)pDiskImage;
                        int     TotalTracks;
                        EXTDSKTRACKHEADER *pTrack = (EXTDSKTRACKHEADER *)((uintptr_t)pHeader + sizeof(EXTDSKHEADER));

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
                                                int SectorSizeFromID = GetSectorSize(pTrack->SectorIDs[j].N);
												int SectorSizeFromDSK;
												int nAllocationSize;

                                                LocalCHRN.C = pTrack->SectorIDs[j].C;
                                                LocalCHRN.H = pTrack->SectorIDs[j].H;
                                                LocalCHRN.R = pTrack->SectorIDs[j].R;
                                                LocalCHRN.N = pTrack->SectorIDs[j].N;
                                                LocalCHRN.ST1 = pTrack->SectorIDs[j].ST1;
                                                LocalCHRN.ST2 = pTrack->SectorIDs[j].ST2;

                                                SectorSizeFromDSK = (pTrack->SectorIDs[j].SectorSizeLow & 0x0ff) |
                                                        ((pTrack->SectorIDs[j].SectorSizeHigh & 0x0ff)<<8);

												nAllocationSize = max(SectorSizeFromDSK, SectorSizeFromID);

                                                ExtDskInternal_AddSectorToTrack(pExtDsk, TrackIndex, &LocalCHRN,pTrack->FillerByte, nAllocationSize);

                                                pSectorData = ExtDskInternal_GetPointerToSectorData(pExtDsk, TrackIndex, j);

                                                memcpy(pSectorData, Sector, nAllocationSize);

                                                Sector += nAllocationSize;

                                        }

                                        pTrack = (EXTDSKTRACKHEADER *)((uintptr_t)pTrack + ((pHeader->TrackSizeTable[i] & 0x0ff)<<8));
                                }
                        }
                }
        }
}

#ifndef min
#define min(a,b) ((a<b) ? a : b )
#endif

void  ExtDskInternal_Dif2ExtDskInternal(DISKIMAGE_UNIT *pUnit,const unsigned char *pDiskImage, const unsigned long DiskImageSize)
{
	EXTDSK_INTERNAL *pExtDsk = NULL;

	if (pDiskImage!=NULL)
	{
		int Tracks, Sectors, Position, SectorPos, SecSize;
		unsigned char *pSectorData;
		int i, j;

		pExtDsk = ExtDskInternal_New();

		pUnit->pDiskImage = (unsigned char *)pExtDsk;

		if (pExtDsk!=NULL)
		{
			Tracks = pDiskImage[1];

			Position = 2;
			SectorPos = 8192;

			for (i = 0; i < Tracks; i++)
			{
				ExtDskInternal_AddTrack(pExtDsk, i);
				Sectors =  pDiskImage[Position++];

				for (j = 0; j < Sectors; j++)
				{

					EXTDSKCHRN LocalCHRN;


					LocalCHRN.C =   pDiskImage[Position+0];
					LocalCHRN.H =   pDiskImage[Position+1];
					LocalCHRN.R =   pDiskImage[Position+2];
					LocalCHRN.N =   pDiskImage[Position+3];
					LocalCHRN.ST1 = pDiskImage[Position+5];
					LocalCHRN.ST2 = pDiskImage[Position+6];

					SecSize = 128 << (min(LocalCHRN.N, 6));


					ExtDskInternal_AddSectorToTrack(pExtDsk, i, &LocalCHRN,
						pDiskImage[Position+7],LocalCHRN.N);

					pSectorData = ExtDskInternal_GetPointerToSectorData(pExtDsk, i, j);
					if  (LocalCHRN.ST2 & 0x80)
					{

						LocalCHRN.ST2 &= 0x7F;

						memset(pSectorData, pDiskImage[Position+7], GetSectorSize(LocalCHRN.N));
					}
					else
					{
 						memcpy(pSectorData, &pDiskImage[SectorPos], GetSectorSize(LocalCHRN.N));//SecSize);

						SectorPos += SecSize;
					}

					Position += 8;
				}
			}
		}
	}
}

typedef struct
{
	/* Version number */
	unsigned char Version;
	/* number of tracks in image */
	unsigned char NumberOfTracks;
} dif_header;

typedef struct
{
	unsigned char C;	/* from id field */
	unsigned char H;	/* from id field */
	unsigned char R;	/* from id field */
	unsigned char N;	/* from id field */
	unsigned char Flag1;
	unsigned char Flag2;
	unsigned char Flag3;
	unsigned char FillerByte; /* if sector data contains a single byte repeated, then this is the data-byte */
} dif_id;

int Dif_Validate(const unsigned char *pDiskImage, const unsigned long DiskImageSize)
{
	/* first byte is version */
	/* second byte is number of tracks */
	/* then number of sectors */
	return FALSE;



}

typedef struct
{
	unsigned char trackID;
	/* bit 7..1 is track number, bit 0 is side number */
	unsigned char NumberOfSectors;
} DSC_TRACK_HEADER;

typedef struct
{
	unsigned char C;
	unsigned char H;
	unsigned char R;
	unsigned char N;
	unsigned char data1;
	unsigned char data2;
} DSC_ID;
#if 0
void  ExtDskInternal_Dsc2ExtDskInternal(DISKIMAGE_UNIT *pUnit)
{
	unsigned long   DataSize;
	unsigned char	*pData;
	unsigned long	HeaderSize;
	unsigned char	*pHeader;

	EXTDSK_INTERNAL *pExtDsk = NULL;

	if ((pData!=NULL) && (pHeader!=NULL))
	{
		int s;
		DSC_TRACK_HEADER *pTrackHeader;
		unsigned char *pCurHeaderPointer = pHeader;
		DSC_ID *pID;

		for (s=0; s<pTrackHeader->NumberOfSectors; s++)
		{
			EXTDSKCHRN LocalCHRN;
			int SecSize;



			LocalCHRN.C =   pID->C;
			LocalCHRN.H =   pID->H;
			LocalCHRN.R =   pID->R;
			LocalCHRN.N =   pID->N;
			LocalCHRN.ST1 = 0;
			LocalCHRN.ST2 = 0;

			SecSize = 128 << (min(LocalCHRN.N, 6));

			// add sector to track
			ExtDskInternal_AddSectorToTrack(pExtDsk, s, &LocalCHRN,	pID->N);

			pID++;
		}
	}
}

#endif
