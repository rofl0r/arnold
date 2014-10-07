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
#ifndef __AMSDOS_HEADER_INCLUDED__
#define __AMSDOS_HEADER_INCLUDED__

#include "cpcglob.h"

/* BASIC parameters to describe an AMSDOS format. A CP/M based format */
typedef struct
{
	int nFirstSectorId;				/* id of first sector. it is assumed all sectors 
									are numbered sequentially */
	int nReservedTracks;			/* number of complete reserved tracks */
	int nSectorsPerTrack;			/* number of sectors per track. It is assumed that
									all tracks have the same number of sectors */
} AMSDOS_FORMAT;

typedef struct
{
	int nTrack;						/* physical track number */
	int nSector;					/* physical sector id */
	int nSide;						/* physical side */
} AMSDOS_TRACK_SECTOR_SIDE;

typedef struct 
{
	unsigned char UserNumber;		/* user number */
	unsigned char Filename[8];		/* name part of filename */
	unsigned char Extension[3];		/* extension part of filename */
	unsigned char Extent;			/* 16K extent of file */
	unsigned char unused[2];		/* not used */
	unsigned char LengthInRecords;	/* length of this extent in records */
	unsigned char Blocks[16];		/* blocks used by this directory entry; 8-bit or 16-bit values */
} amsdos_directory_entry;


typedef struct
{
	unsigned char Unused1;
	unsigned char Filename[8];
	unsigned char Extension[3];
	unsigned char Unused2[6];
	unsigned char FileType;
	unsigned char LengthLow;
	unsigned char LengthHigh;
	unsigned char LocationLow;
	unsigned char LocationHigh;
	unsigned char FirstBlockFlag;
	unsigned char LogicalLengthLow;
	unsigned char LogicalLengthHigh;
	unsigned char ExecutionAddressLow;
	unsigned char ExecutionAddressHigh;
	unsigned char DataLengthLow;
	unsigned char DataLengthMid;
	unsigned char DataLengthHigh;
	unsigned char ChecksumLow;
	unsigned char ChecksumHigh;
} AMSDOS_HEADER;

unsigned int AMSDOS_CalculateChecksum(const unsigned char *pHeader);
BOOL     AMSDOS_HasAmsdosHeader(const unsigned char *pHeader);

/* returns TRUE if the ch is a valid filename character, FALSE otherwise.
A valid filename character is a character which can be typed by the user and one
which, when used in a typed filename and is present on the disc, can then be loaded by
AMSDOS */
BOOL	AMSDOS_IsValidFilenameCharacter(char ch);


BOOL	AMSDOS_GetFilenameThatQualifiesForAutorun(amsdos_directory_entry *entry);

enum
{
	AUTORUN_OK,						/* found a valid autorun method */
	AUTORUN_NOT_POSSIBLE,			/* auto-run is not possible; no suitable files found and |CPM will not work */
	AUTORUN_TOO_MANY_POSSIBILITIES,	/* too many files qualify for auto-run and it is not possible to
									identify the correct one */
	AUTORUN_NO_FILES_QUALIFY,		/* no files qualify for auto-run */
	AUTORUN_FAILURE_READING_DIRECTORY,	/* failed to read directory */
};

int AMSDOS_GenerateAutorunCommand(unsigned char *pBuffer, char *AutorunCommand);


#endif
