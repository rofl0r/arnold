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
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include "romfn.h"

#ifndef NULL
#define NULL	0
#endif

static char *RomFilenames[17];

/* initialise list of rom filenames */
void	RomFilenames_Initialise(void)
{
	int i;

	for (i=0; i<16; i++)
	{
		RomFilenames[i] = NULL;
	}
}

/* free all rom filenames */
void	RomFilenames_Finish(void)
{
	int i;

	for (i=0; i<16; i++)
	{
		if (RomFilenames[i]!=NULL)
		{
			free(RomFilenames[i]);
			RomFilenames[i] = NULL;
		}
	}
}

/* get filename of rom specified by rom index */
char	*RomFilenames_GetFilename(int RomIndex)
{
	return RomFilenames[RomIndex];
}

/* set filename of rom specified */
void	RomFilenames_SetFilename(int RomIndex, char *newFilename)
{
	/* if filename exists already, free it */
	if (RomFilenames[RomIndex]!=NULL)
	{
		free(RomFilenames[RomIndex]);
		RomFilenames[RomIndex] = NULL;
	}

	/* store new filename */
	if (newFilename!=NULL)
	{
		int NewFilenameLength = strlen(newFilename);

		if (NewFilenameLength!=0)
		{
			/* allocate space for new filename */
			RomFilenames[RomIndex] = malloc(NewFilenameLength+1);

			if (RomFilenames[RomIndex]!=NULL)
			{
				/* copy new filename */
				memcpy(RomFilenames[RomIndex], newFilename, NewFilenameLength+1);
			}
		}
	}
}
