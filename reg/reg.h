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
#ifndef __MYAPP_REGISTRY_HEADER_INCLUDED__
#define __MYAPP_REGISTRY_HEADER_INCLUDED__

#include <windows.h>

// test if the value exists in the specified key
BOOL	MyApp_Registry_CheckValuePresent(HKEY Key, char *ValueName);

// add a null-terminated string value to the specified key
BOOL	MyApp_Registry_AddStringValueToKey(HKEY	Key, char *ValueName, char *ValueString);

// add a dword value to the specified key
BOOL	MyApp_Registry_AddDWORDValueToKey(HKEY Key, char *ValueName, DWORD ValueData);

// get value data from specified key
BOOL MyApp_Registry_GetValue(HKEY Key, char *ValueName, char *pBuffer, int BufferSize);

// get size of value of specified key
int	MyApp_Registry_GetValueSize(HKEY Key, char *ValueName);

#endif
