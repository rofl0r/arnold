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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../cpc/host.h"

#include "global.h"
#include "roms.h"

char currentDir[MAXCURDIR];

/* only for debugging */
#if 0
#define	DBG_BUILTIN(a) fprintf(stderr,"BUILTIN: %s\n",a)
#else
#define	DBG_BUILTIN(a)
#endif

/*
void	dbgl(unsigned char *location, unsigned long length) {
	fprintf(stderr, "Location: %08x, Length: %i\n", location, length);
}
*/

BOOL	loadBuiltin(unsigned char **pLocation, unsigned long *pLength,
	rom_t *rom)
{
	unsigned char	*pData;

	pData = (unsigned char *)malloc(rom->size);

	if (pData!=NULL)
	{
		memcpy(pData, rom->start, rom->size);
					
		*pLocation = pData;
		*pLength = rom->size;

		/*dbgl(*pLocation,*pLength);*/

		return TRUE;
	}

	return FALSE;
}

BOOL	LoadFile(const char *Filename, char **pLocation, 
unsigned long *pLength)
{
	FILE	*fh;
	unsigned char	*pData;
	roms_t	*pRoms = NULL;


	*pLocation = NULL;
	*pLength = 0;

	if (Filename!=NULL)
	{
		if (strlen(Filename)!=0)
		{
			/* fprintf(stderr,"Host_LoadFile(%s)\n",Filename); */
			fh = fopen(Filename,"rb");

			if (fh!=NULL)
			{
				int FileSize;

				/* seek to end */
				fseek(fh, 0, SEEK_END);				

				/* report position */
				FileSize = ftell(fh);

				fseek(fh, 0, SEEK_SET);
			
				if (FileSize!=0)
				{
					pData = (unsigned char *)malloc(FileSize);

					if (pData!=NULL)
					{
						fread(pData,1,FileSize,fh);
					
						*pLocation = pData;
						*pLength = FileSize;
					
						fclose(fh);
						return TRUE;
					}
				}

				fclose(fh);
			}
		}
	}

	return FALSE;
}

HOST_FILE_HANDLE	Host_OpenFile(const char *Filename, int Access)
{
	HOST_FILE_HANDLE fh;

	/* fprintf(stderr,"Host_OpenFile(%s)\n",Filename); */
	if (Access == HOST_FILE_ACCESS_READ)
	{
		fh = (HOST_FILE_HANDLE)fopen(Filename,"rb");
	}
	else
	{
		fh = (HOST_FILE_HANDLE)fopen(Filename,"wb");
	}

	return (HOST_FILE_HANDLE)fh;
}

void	Host_CloseFile(HOST_FILE_HANDLE Handle)
{
	if (Handle!=0)
	{
		fclose((FILE *)Handle);
	}
}

int	Host_GetFileSize(HOST_FILE_HANDLE Handle)
{
	if (Handle!=0)
	{
		/* get current offset in file */
		int CurrentPos = ftell((FILE *)Handle);
		int FileSize;
		
		/* seek to end of file */
		fseek((FILE *)Handle, 0, SEEK_END);
		
		/* report position */
		FileSize = ftell((FILE *)Handle);
		
		/* go back to original position relative to start */
		fseek((FILE *)Handle, CurrentPos, SEEK_SET);
		
		return FileSize;
	}

	return 0;
}



void	Host_ReadData(HOST_FILE_HANDLE Handle, unsigned char *pData, unsigned long Size)
{
	if (Handle!=0)
	{
		fread(pData, Size, 1, (FILE *)Handle);
	}
}

void	Host_WriteData(HOST_FILE_HANDLE Handle, unsigned char *pData, unsigned long Size)
{
	if (Handle!=0)
	{
		fwrite(pData, Size, 1, (FILE *)Handle);
	}
}


BOOL	SaveFile(const char *Filename,char *pData, unsigned 
long
Length)
{
	FILE	*fh;

	if ((pData!=NULL) && (Length!=0))
	{
		if (Filename!=NULL)
		{
			if (strlen(Filename)!=0)
			{
				fh = fopen(Filename,"wb");

				if (fh==NULL)
					return FALSE;

				fwrite(pData,1,Length,fh);

				fclose(fh);
			
				return TRUE;

			}
		}
	}

	return FALSE;
}


