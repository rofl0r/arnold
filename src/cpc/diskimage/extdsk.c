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

int		ExtDsk_Validate(const unsigned char *pDiskImage, const unsigned long DiskImageSize)
{
	BOOL			ValidImage = FALSE;

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
	}

	return ValidImage;
}

/* Return sector size from N value in sector ID */

static int      GetSectorSize(int N)
{
        if (N<8)
			return (1<<N)<<7;	
        else
                return 0x0200;
}

