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
#include <windows.h>
#include "reg.h"
#include <stdio.h>
#include <stdlib.h>

BOOL	MyApp_Registry_CheckValuePresent(HKEY Key, char *ValueName)
{
	DWORD Type;
	DWORD Size;

	if (RegQueryValueEx(Key, ValueName, 0, &Type, NULL, &Size)==ERROR_SUCCESS)
	{
		return TRUE;
	}

	return FALSE;
}

// add a null-terminated string value to the specified key
BOOL	MyApp_Registry_AddStringValueToKey(HKEY	Key, char *ValueName, char *ValueString)
{
	if (RegSetValueEx(Key, ValueName, 0, REG_SZ, ValueString, strlen(ValueString)+1)==ERROR_SUCCESS);
		return TRUE;
	
	return FALSE;
}

// add a dword value to the specified key
BOOL	MyApp_Registry_AddDWORDValueToKey(HKEY Key, char *ValueName, DWORD ValueData)
{
	if (RegSetValueEx(Key, ValueName, 0, REG_DWORD, (unsigned char *)&ValueData, sizeof(DWORD))==ERROR_SUCCESS);
		return TRUE;
		
	return FALSE;
}


// get value from specified key
BOOL MyApp_Registry_GetValue(HKEY Key, char *ValueName, char *pBuffer, int BufferSize)
{
	DWORD Type;
	DWORD BuffSize = BufferSize;

	if (RegQueryValueEx(Key, ValueName, 0, &Type, pBuffer, &BuffSize)==ERROR_SUCCESS)
	{
		return TRUE;
	}
	
	return FALSE;
}


// get size of specified key
int	MyApp_Registry_GetValueSize(HKEY Key, char *ValueName)
{
	DWORD Type;
	DWORD Size;

	if (RegQueryValueEx(Key, ValueName, 0, &Type, NULL, &Size)==ERROR_SUCCESS)
	{
		return Size;
	}
	
	return 0;
}
