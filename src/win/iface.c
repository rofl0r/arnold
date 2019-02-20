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
#include <io.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void	DeleteFunction(struct _finddata_t *fileinfo);

void	DoOperationOnFiles(char *FileSpec, void (*pCallback)(struct _finddata_t *fileinfo))
{
	long findhandle;

	struct _finddata_t	fileinfo;

	/* find first file/directory matching filespec */
	findhandle = _findfirst(FileSpec, &fileinfo);

	if (findhandle!=-1)
	{
		/* found a file/directory */

		if (pCallback!=NULL)
			pCallback(&fileinfo);

		do
		{
			int success;

			/* find next file/directory matching filespec */
			success = _findnext(findhandle, &fileinfo);

			if (success==-1)
				break;

			if (pCallback!=NULL)
				pCallback(&fileinfo);

		}
		while (1);

		/* finish finding files/directories */
		_findclose(findhandle);
	}
}


void	DeleteAllFilesInCurrentDir()
{
	DoOperationOnFiles("*.*",DeleteFunction);
}

void	DeleteFunction(struct _finddata_t *fileinfo)
{
	if (fileinfo->attrib & _A_RDONLY)
	{
		/* read only */

		/* make it r/w */
		_chmod(fileinfo->name, _S_IWRITE | _S_IREAD);
	}

	if (fileinfo->attrib & _A_SUBDIR)
	{
		/* sub-directory */

		if ((stricmp(fileinfo->name,".")!=0) && (stricmp(fileinfo->name,"..")!=0))
		{
			/* enter sub-dir */
			_chdir(fileinfo->name);

			/* delete files in that dir */
			DeleteAllFilesInCurrentDir();

			/* change back to parent dir (this dir) */
			_chdir("..");

			/* remove directory */
			_rmdir(fileinfo->name);
		}
	}

	if (!(fileinfo->attrib & _A_SUBDIR))
	{
		/* file. Remove it */
		remove(fileinfo->name);
	}
}
