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
#ifndef __FDD_HEADER_INCLUDED__
#define __FDD_HEADER_INCLUDED__


/* fdd has a disc present */
/*#define FDD_FLAGS_DISK_PRESENT	0x080 */


/* write protected */
#define FDD_FLAGS_WRITE_PROTECTED 0x040
/* head is positioned at track 0 */
#define FDD_FLAGS_HEAD_AT_TRACK_0 0x001
/* disk present */
#define FDD_FLAGS_DISK_PRESENT	0x002
/* drive active */
#define FDD_FLAGS_DRIVE_ACTIVE 0x004

typedef struct FDD
{
	/* flags */
	unsigned long Flags;

/*	BOOL	WriteProtected; */			/* true if write protected */
/*	BOOL	DiskPresent; */			/* true if disc inserted */
/*	BOOL	DriveActive; */

	unsigned long PhysicalSide;

	/* current track drive is on */
	unsigned long CurrentTrack;
	/* total number of tracks the head can move.
	On a 3" drive the maximum number is 43 */
	unsigned long NumberOfTracks;

	int		CurrentIDIndex;			/* current id index */

} FDD;




#endif
