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
#ifndef __FILENAME_AND_PATH_HEADER_INCLUDED__
#define __FILENAME_AND_PATH_HEADER_INCLUDED__

typedef struct FILENAME_AND_PATH
{
	char *pFilename;
	char *pPath;
} FILENAME_AND_PATH;

void	FilenameAndPath_Initialise(FILENAME_AND_PATH *pFilenameAndPath);
void	FilenameAndPath_Finish(FILENAME_AND_PATH *pFilenameAndPath);

void	FilenameAndPath_SetFilename(FILENAME_AND_PATH *pFilenameAndPath, char *pNewFilename);
void	FilenameAndPath_SetPath(FILENAME_AND_PATH *pFilenameAndPath, char *pNewPath);

char	*FilenameAndPath_GetFilename(FILENAME_AND_PATH *pFilenameAndPath);
char	*FilenameAndPath_GetPath(FILENAME_AND_PATH *pFilenameAndPath);

#endif

