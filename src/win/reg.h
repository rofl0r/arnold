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
#include <tchar.h>
void	MyApp_Registry_InitialiseKey();
BOOL	MyApp_Registry_CheckKeyPresent();
BOOL	MyApp_Registry_OpenKey();
void MyApp_Registry_CloseKey();

void	MyApp_Registry_StoreStringToCurrentKey(const TCHAR *, const TCHAR *);
TCHAR	*MyApp_Registry_GetStringFromCurrentKey(const TCHAR *);
void	MyApp_Registry_StoreIntToCurrentKey(const TCHAR *, int);
int		MyApp_Registry_GetIntFromCurrentKey(const TCHAR *);
BOOL	MyApp_Registry_CheckValueIsPresent(const TCHAR *ValueName);	//InCurrentKey(const TCHAR *ValueName);

typedef struct
{
	LPCTSTR pExtensionKeyName;			/* e.g. ".dsk" */
	LPCTSTR pExtensionLinkKeyName;		/* e.g. "Arnold.DskFile" */
	LPCTSTR pExtensionDescription;		/* e.g. "Disk Image File" */
	int	nIconIndex;						/* e.g. index of icon in main file */
	LPCTSTR pApplicationPath;				/* e.g. c:\program files\arnold.exe" */
} EXTENSION_INFO;

void	RegisterExtension(const EXTENSION_INFO *pExtensionInfo, BOOL fOpenWith);

#endif
