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
#include <stdint.h>

static int	DskGetSectorSize(int);

/* load and validate a disk image */
/* checks are done so that the chances of a crash resulting from a bad disk */
/* image are reduced or eliminated */
int		Dsk_Validate(const unsigned char *pDiskImage, const unsigned long DiskImageSize)
{
	BOOL			ValidImage = FALSE;

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
						DSKTRACKHEADER *pTrack = (DSKTRACKHEADER *)((uintptr_t)pHeader + sizeof(DSKHEADER));

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

							pTrack = (DSKTRACKHEADER *)((uintptr_t)pTrack + TrackSize);
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

static int	DskGetSectorSize(int N)
{
	return (1<<(N&0x07))<<7;
}

