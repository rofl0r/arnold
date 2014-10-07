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
/* load files from specified dirs */

#include "dirstuff.h"
#include <stdio.h>
#include <stdlib.h>
#include "../cpc/host.h"
#include <string.h>

/* list of directorys in host form */
static unsigned char *Directorys[LAST_DIR];

/* setup list of directorys to point to NULL */
void	DirStuff_Initialise(void)
{
	int i;

	for (i=0; i<LAST_DIR; i++)
	{
		Directorys[i] = NULL;
	}
}

/* free up any directory names setup */
void	DirStuff_Finish(void)
{
	int i;

	for (i=0; i<LAST_DIR; i++)
	{
		if (Directorys[i]!=NULL)
		{
			free(Directorys[i]);
		}
	}
}

/* set directory path for location id */
void	SetDirectoryForLocation(LOCATION_ID ID, unsigned char *pDirectoryPath)
{
	/* is there an existing path setup? */
	if (Directorys[ID]!=NULL)
	{
		/* free it */
		free(Directorys[ID]);
	}

	/* allocate space for new path */
	Directorys[ID] = (unsigned char *)malloc(strlen((const char *)pDirectoryPath)+1);
	
	if (Directorys[ID]!=NULL)
	{
		/* copy in string */
		strcpy((char *)Directorys[ID], (char *)pDirectoryPath);
	}
}



void	ChangeToLocationDirectory(LOCATION_ID LocationID)
{
	if (Directorys[LocationID]!=NULL)
	{
		Host_SetDirectory((char *)Directorys[LocationID]);
	}
}
