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
#include "fnp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* initialise filename and path */
void	FilenameAndPath_Initialise(FILENAME_AND_PATH *pFilenameAndPath)
{
	pFilenameAndPath->pFilename = NULL;
	pFilenameAndPath->pPath = NULL;
}

/* set a new filename */
void	FilenameAndPath_SetFilename(FILENAME_AND_PATH *pFilenameAndPath, char *pNewFilename)
{
	/* if a filename is already set free it */
	if (pFilenameAndPath->pFilename!=NULL)
	{
		free(pFilenameAndPath->pFilename);
		pFilenameAndPath->pFilename = NULL;
	}

	if (pNewFilename == NULL)
		return;

	/* set new filename */
	pFilenameAndPath->pFilename = malloc(strlen(pNewFilename)+1);

	if (pFilenameAndPath->pFilename!=NULL)
	{
		memcpy(pFilenameAndPath->pFilename, pNewFilename, strlen(pNewFilename)+1);
	}
}

/* free filename and path */
void	FilenameAndPath_Finish(FILENAME_AND_PATH *pFilenameAndPath)
{
	/* free filename */
	if (pFilenameAndPath->pFilename!=NULL)
	{
		free(pFilenameAndPath->pFilename);
		pFilenameAndPath->pFilename = NULL;
	}

	/* free path */
	if (pFilenameAndPath->pPath!=NULL)
	{
		free(pFilenameAndPath->pPath);
		pFilenameAndPath->pPath = NULL;
	}
}

/* set path */
void	FilenameAndPath_SetPath(FILENAME_AND_PATH *pFilenameAndPath, char *pNewPath)
{
	/* if a path is already set, free it */
	if (pFilenameAndPath->pPath!=NULL)
	{
		free(pFilenameAndPath->pPath);
		pFilenameAndPath->pPath = NULL;
	}

	if (pNewPath == NULL)
		return;

	/* set new path */
	pFilenameAndPath->pPath = malloc(strlen(pNewPath)+1);

	if (pFilenameAndPath->pPath!=NULL)
	{
		memcpy(pFilenameAndPath->pPath, pNewPath, strlen(pNewPath)+1);
	}
}

char	*FilenameAndPath_GetFilename(FILENAME_AND_PATH *pFilenameAndPath)
{
	if (pFilenameAndPath->pFilename!=NULL)
	{
		return pFilenameAndPath->pFilename;
	}
	else
	{
		return "";
	}
}

char	*FilenameAndPath_GetPath(FILENAME_AND_PATH *pFilenameAndPath)
{
	if (pFilenameAndPath->pPath!=NULL)
	{
		return pFilenameAndPath->pPath;
	}
	else
	{
		return "";
	}
}
