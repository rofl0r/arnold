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
/* Debug */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "debug.h"

char	DebugString[256];

void			Debug_Enable(DEBUG_HANDLE	DebugHandle, int State)
{
	DEBUG_STRUCT	*pDebugStruct = (DEBUG_STRUCT *)DebugHandle;

	if (DebugHandle==0)
		return;

	pDebugStruct->Enabled = State;
}

BOOL			Debug_Active(DEBUG_HANDLE DebugHandle)
{
	DEBUG_STRUCT	*pDebugStruct = (DEBUG_STRUCT *)DebugHandle;

	if (DebugHandle==0)
		return FALSE;

	return pDebugStruct->Enabled;
}

DEBUG_HANDLE	Debug_Start(char *DebugName)
{
	DEBUG_STRUCT	*pDebugStruct;

	pDebugStruct = (DEBUG_STRUCT *)malloc(sizeof(DEBUG_STRUCT));
	
	if (pDebugStruct!=NULL)
	{
		memset(pDebugStruct,0,sizeof(DEBUG_STRUCT));

		pDebugStruct->Enabled = FALSE;

		pDebugStruct->fh = fopen(DebugName,"wb");

	
		if (pDebugStruct->fh!=NULL)
		{
			fprintf(pDebugStruct->fh,"-- DEBUG BEGIN --\r\n");
		}
	}

	return (DEBUG_HANDLE)pDebugStruct;
}

void	Debug_WriteString(DEBUG_HANDLE DebugHandle,char *Message)
{
	DEBUG_STRUCT	*pDebugStruct = (DEBUG_STRUCT *)DebugHandle;

	if (DebugHandle==0)
		return;

	if (!pDebugStruct->Enabled)
		return;

	if (pDebugStruct->fh!=NULL)
		fprintf(pDebugStruct->fh,Message);
}

void	Debug_End(DEBUG_HANDLE	DebugHandle)
{
	DEBUG_STRUCT	*pDebugStruct = (DEBUG_STRUCT *)DebugHandle;

	if (DebugHandle==0)
		return;

	if (pDebugStruct->fh!=NULL)
	{
		fprintf(pDebugStruct->fh,"-- DEBUG END --\r\n");
		fclose(pDebugStruct->fh);
	}

	free(pDebugStruct);	
}
