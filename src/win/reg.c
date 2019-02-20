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

#define ARNOLD_REGISTRY_KEY "Arnold"

/*

HKEY DefaultKey;

if (RegOpenKeyEx(HKEY_USERS, ".Default", 0, KEY_READ,  &DefaultKey)==ERROR_SUCCESS)
{
	HKEY SoftwareKey;

	if (RegOpenKeyEx(DefaultKey, "Software", 0, KEY_READ, &SoftwareKey)==ERROR_SUCCESS)
	{
		HKEY ArnoldKey;

		if (RegOpenKeyEx(SoftwareKey, ARNOLD_REGISTRY_KEY, 0, KEY_READ, &ArnoldKey)==ERROR_SUCCESS)
		{
			// got arnold key

			// close arnold key
			RegCloseKey(&ArnoldKey);
		}
	
		RegCloseKey(SoftwareKey);
	}

	RegCloseKey(DefaultKey);
}
*/


BOOL	MyApp_Registry_CheckKeyPresent()
{
	BOOL Present = FALSE;

	// open current user, and get settings for Arnold
	HKEY SoftwareKey;

	// open Software tab for current user
	if (RegOpenKeyEx(HKEY_CURRENT_USER,"Software", 0, KEY_READ, &SoftwareKey)==ERROR_SUCCESS)
	{
		HKEY ArnoldKey;

		// open Arnold key
		if (RegOpenKeyEx(SoftwareKey, ARNOLD_REGISTRY_KEY, 0, KEY_READ, &ArnoldKey)==ERROR_SUCCESS)
		{
			// found Arnold key. Retrieve settings

			// close arnold key
			RegCloseKey(ArnoldKey);

			Present = TRUE;
		}
		else
		{
			// get default settings

		}

		// close software key
		RegCloseKey(SoftwareKey);
	}

	return Present;
}

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


// add a null-terminated string value to the selected key
BOOL	MyApp_Registry_AddStringValueToKey(HKEY	Key, char *ValueName, char *ValueString)
{
	if (RegSetValueEx(Key, ValueName, 0, REG_SZ, ValueString, strlen(ValueString)+1)==ERROR_SUCCESS);
		return TRUE;
	
	return FALSE;
}

// add a dword value to the selected key
BOOL	MyApp_Registry_AddDWORDValueToKey(HKEY Key, char *ValueName, DWORD ValueData)
{
	if (RegSetValueEx(Key, ValueName, 0, REG_DWORD, (unsigned char *)&ValueData, sizeof(DWORD))==ERROR_SUCCESS);
		return TRUE;
		
	return FALSE;
}


// get value data from registry 
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


// get size of registry value
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

HKEY CurrentKey;
char *pTempString;

BOOL MyApp_Registry_OpenKey()
{
	BOOL Opened = FALSE;

	// create Arnold key in current User
	HKEY SoftwareKey;

	pTempString = NULL;

	// get software key for current user
	if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software", 0, KEY_CREATE_SUB_KEY, &SoftwareKey)==ERROR_SUCCESS)
	{
		HKEY ArnoldKey;
		DWORD DispositionValue;

		// create Arnold key
		if (RegCreateKeyEx(SoftwareKey, ARNOLD_REGISTRY_KEY, 0, "",REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS ,NULL, &ArnoldKey, &DispositionValue)==ERROR_SUCCESS)
		{
			CurrentKey = ArnoldKey;

			
			Opened = TRUE;
		}

		RegCloseKey(SoftwareKey);
	}

	return Opened;
}

void MyApp_Registry_CloseKey()
{
	if (CurrentKey!=0)
	{
		RegCloseKey(CurrentKey);
		CurrentKey = 0;
	}

	if (pTempString!=NULL)
	{
		free(pTempString);
	}
}

void	MyApp_Registry_StoreStringToCurrentKey(char *KeyName, char *pString)
{
	if (CurrentKey!=0)
	{
		MyApp_Registry_AddStringValueToKey(CurrentKey, KeyName, pString);
	}
}

void	MyApp_Registry_StoreIntToCurrentKey(char *KeyName, int Value)
{
	if (CurrentKey!=0)
	{
		MyApp_Registry_AddDWORDValueToKey(CurrentKey, KeyName, Value);
	}
}

BOOL	MyApp_Registry_CheckValueIsPresentInCurrentKey(char *ValueName)
{
	if (CurrentKey!=0)
	{
		return MyApp_Registry_CheckValuePresent(CurrentKey, ValueName);
	}

	return FALSE;
}


char *MyApp_Registry_GetStringFromCurrentKey(char *KeyName)
{
	if (CurrentKey!=0)
	{
		int TextLength = MyApp_Registry_GetValueSize(CurrentKey, KeyName);

		if (TextLength!=0)
		{
			if (pTempString!=NULL)
			{
				free(pTempString);
			}

			pTempString = malloc(TextLength);

			if (pTempString!=NULL)
			{
				MyApp_Registry_GetValue(CurrentKey, KeyName, pTempString, TextLength);
			}
		}
		else
		{
			return NULL;
		}
	}

	return pTempString;
}

int TempValue;

int MyApp_Registry_GetIntFromCurrentKey(char *KeyName)
{
	if (CurrentKey!=0)
	{
		MyApp_Registry_GetValue(CurrentKey, KeyName, (char *)&TempValue, sizeof(int));
	}

	return TempValue;

}

void	MyApp_Registry_InitialiseKey()
{
	// create Arnold key in current User
	HKEY SoftwareKey;

	// get software key for current user
	if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software", 0, KEY_CREATE_SUB_KEY, &SoftwareKey)==ERROR_SUCCESS)
	{
		HKEY ArnoldKey;
		DWORD DispositionValue;

		// create Arnold key
		if (RegCreateKeyEx(SoftwareKey, ARNOLD_REGISTRY_KEY, 0, "",REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS ,NULL, &ArnoldKey, &DispositionValue)==ERROR_SUCCESS)
		{
			// created a key or opened an existing one?
			if (DispositionValue != REG_OPENED_EXISTING_KEY)
			{
				// created a new key

				MyApp_Registry_AddDWORDValueToKey(ArnoldKey, "iCPCType", 0);
				MyApp_Registry_AddDWORDValueToKey(ArnoldKey, "iCRTCType", 0);
				MyApp_Registry_AddStringValueToKey(ArnoldKey, "sCartridgePath", "c:\\windows\\desktop");
			}

			// close arnold key
			RegCloseKey(ArnoldKey);
		}

		// close software key
		RegCloseKey(SoftwareKey);
	}
}
