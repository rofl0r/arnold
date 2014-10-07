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
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "reg.h"
#include <stdlib.h>


/*----------------------------------------------------------------------------------------------------------*/
/* length of string in bytes including null termination character; works for ASCII and UNICODE */
DWORD StringLengthBytes(const TCHAR *sString)
{
	return ((_tcslen(sString)+1)*sizeof(TCHAR));
}


//#define ARNOLD_REGISTRY_KEY "Arnold"
#define ARNOLD_REGISTRY_KEY PUB_szProfileArnold /* Troels K. */
const TCHAR* PUB_szProfileArnold = _T("Arnold");


HKEY CurrentKey;

/*

HKEY DefaultKey;

if (RegOpenKeyEx(HKEY_USERS, ".Default", 0, KEY_READ,  &DefaultKey)==ERROR_SUCCESS)
{
	HKEY SoftwareKey;

	if (RegOpenKeyEx(DefaultKey, "Software", 0, KEY_READ, &SoftwareKey)==ERROR_SUCCESS)
	{
		HKEY ArnoldKey;

		if (RegOpenKeyEx(SoftwareKey, PUB_szProfileArnold, 0, KEY_READ, &ArnoldKey)==ERROR_SUCCESS)
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
	if (RegOpenKeyEx(HKEY_CURRENT_USER,_T("Software"), 0, KEY_READ, &SoftwareKey)==ERROR_SUCCESS)
	{
		HKEY ArnoldKey;

		// open Arnold key
		if (RegOpenKeyEx(SoftwareKey, PUB_szProfileArnold, 0, KEY_READ, &ArnoldKey)==ERROR_SUCCESS)
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

BOOL	MyApp_Registry_CheckValuePresent(const TCHAR *ValueName)
{
	DWORD Type;
	DWORD Size;

	if (RegQueryValueEx(CurrentKey, ValueName, 0, &Type, NULL, &Size)==ERROR_SUCCESS)
	{
		return TRUE;
	}

	return FALSE;
}


// add a null-terminated string value to the selected key
BOOL	MyApp_Registry_AddStringValueToKey(HKEY	Key, const TCHAR *ValueName, const TCHAR *ValueString)
{
	if (RegSetValueEx(Key, ValueName, 0, REG_SZ, (const BYTE *)ValueString, StringLengthBytes(ValueString))==ERROR_SUCCESS);
		return TRUE;
	
	return FALSE;
}

// add a dword value to the selected key
BOOL	MyApp_Registry_AddDWORDValueToKey(HKEY Key, const TCHAR *ValueName, DWORD ValueData)
{
	if (RegSetValueEx(Key, ValueName, 0, REG_DWORD, (const BYTE *)&ValueData, sizeof(DWORD))==ERROR_SUCCESS);
		return TRUE;
		
	return FALSE;
}


// get value data from registry 
BOOL MyApp_Registry_GetValue(HKEY Key, const TCHAR *ValueName, char *pBuffer, int BufferSize)
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
int	MyApp_Registry_GetValueSize(HKEY Key, const TCHAR *ValueName)
{
	DWORD Type;
	DWORD Size;

	if (RegQueryValueEx(Key, ValueName, 0, &Type, NULL, &Size)==ERROR_SUCCESS)
	{
		return Size;
	}
	
	return 0;
}

TCHAR *pTempString;

BOOL MyApp_Registry_OpenKey()
{
	BOOL Opened = FALSE;

	// create Arnold key in current User
	HKEY SoftwareKey;

	pTempString = NULL;

	// get software key for current user
	if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software"), 0, KEY_CREATE_SUB_KEY, &SoftwareKey)==ERROR_SUCCESS)
	{
		HKEY ArnoldKey;
		DWORD DispositionValue;

		// create Arnold key
		if (RegCreateKeyEx(SoftwareKey, PUB_szProfileArnold, 0, (LPTSTR)_T(""),REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS ,NULL, &ArnoldKey, &DispositionValue)==ERROR_SUCCESS)
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

void	MyApp_Registry_StoreStringToCurrentKey(const TCHAR *KeyName, const TCHAR *pString)
{
	if (CurrentKey!=0)
	{
		MyApp_Registry_AddStringValueToKey(CurrentKey, KeyName, pString);
	}
}

void	MyApp_Registry_StoreIntToCurrentKey(const TCHAR *KeyName, int Value)
{
	if (CurrentKey!=0)
	{
		MyApp_Registry_AddDWORDValueToKey(CurrentKey, KeyName, Value);
	}
}


TCHAR *MyApp_Registry_GetStringFromCurrentKey(const TCHAR *KeyName)
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
				MyApp_Registry_GetValue(CurrentKey, KeyName, (char *)pTempString, TextLength);
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

int MyApp_Registry_GetIntFromCurrentKey(const TCHAR *KeyName)
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
	if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software"), 0, KEY_CREATE_SUB_KEY, &SoftwareKey)==ERROR_SUCCESS)
	{
		HKEY ArnoldKey;
		DWORD DispositionValue;

		// create Arnold key
		if (RegCreateKeyEx(SoftwareKey, PUB_szProfileArnold, 0, _T(""),REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS ,NULL, &ArnoldKey, &DispositionValue)==ERROR_SUCCESS)
		{
			// created a key or opened an existing one?
			if (DispositionValue != REG_OPENED_EXISTING_KEY)
			{
				// created a new key

				MyApp_Registry_AddDWORDValueToKey(ArnoldKey, _T("iCPCType"), 0);
				MyApp_Registry_AddDWORDValueToKey(ArnoldKey, _T("iCRTCType"), 0);
			}

			// close arnold key
			RegCloseKey(ArnoldKey);
		}

		// close software key
		RegCloseKey(SoftwareKey);
	}
}

/*----------------------------------------------------------------------------------------------------------*/

const TCHAR *OpenString = _T("&Open");
const TCHAR *OpenWithString = _T("Open with &Arnold");
const TCHAR *DDE_DefaultKey = _T("[SetForeground][Open \"%1\"]");
const TCHAR *DDE_AppName = _T("Arnold");
const TCHAR *DDE_TopicName = _T("system");

void RegisterExtension_OpenKeySetup(const EXTENSION_INFO *pExtensionInfo,HKEY hOpenKey, BOOL fOpenWith)
{
	DWORD dwDisposition;
	HKEY hCommandKey;
	HKEY hDDEKey;

	if (!fOpenWith)
	{
		RegSetValueEx(hOpenKey, NULL, 0, REG_SZ, (const BYTE *)OpenString,StringLengthBytes(OpenString));
	}

	if (RegCreateKeyEx(hOpenKey, _T("command"), 0, _T(""),REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,&hCommandKey, &dwDisposition)==ERROR_SUCCESS)
	{
		TCHAR Buffer[256];

		_stprintf(Buffer,_T("\"%s\" \"%%1\""),pExtensionInfo->pApplicationPath);

		RegSetValueEx(hCommandKey, NULL, 0, REG_SZ, (const BYTE *)Buffer, StringLengthBytes(Buffer));

		RegCloseKey(hCommandKey);
	}

	if (RegCreateKeyEx(hOpenKey, _T("ddeexec"),0, _T(""),REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hDDEKey, &dwDisposition)==ERROR_SUCCESS)
	{
		HKEY hApplicationKey;

		/* set the default key */
		RegSetValueEx(hDDEKey, NULL, 0, REG_SZ, (const BYTE *)DDE_DefaultKey, StringLengthBytes(DDE_DefaultKey));

		if (RegCreateKeyEx(hDDEKey, _T("application"),0, _T(""),REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hApplicationKey, &dwDisposition)==ERROR_SUCCESS)
		{

			RegSetValueEx(hApplicationKey, NULL, 0, REG_SZ, (const BYTE *)DDE_AppName, StringLengthBytes(DDE_AppName));

			RegCloseKey(hApplicationKey);
		}

		if (RegCreateKeyEx(hDDEKey, _T("topic"),0, (LPTSTR)_T(""),REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hApplicationKey, &dwDisposition)==ERROR_SUCCESS)
		{
			RegSetValueEx(hApplicationKey, NULL, 0, REG_SZ, (const BYTE *)DDE_TopicName, StringLengthBytes(DDE_TopicName));

			RegCloseKey(hApplicationKey);
		}

		RegCloseKey(hDDEKey);
	}
}


void	RegisterExtension(const EXTENSION_INFO *pExtensionInfo, BOOL fOpenWith)
{
	HKEY hExtensionKey;
	DWORD dwDisposition;
	/* the actual link used */
	TCHAR *pExtensionLinkName = NULL;

	if (RegCreateKeyEx(HKEY_CLASSES_ROOT, pExtensionInfo->pExtensionKeyName,0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hExtensionKey, &dwDisposition)==ERROR_SUCCESS)
	{
		/* create if it doesn't exist, otherwise just opens it */
		BOOL fExists = FALSE;

		if (dwDisposition==REG_OPENED_EXISTING_KEY)
		{
			/* if friendly then use existing key; don't replace with our own */
			if (fOpenWith)
			{
				DWORD dwLength;
				DWORD dwType;
				dwLength = 0;

				/* query existing link */
				if (RegQueryValueEx(hExtensionKey, NULL, NULL, &dwType, NULL, &dwLength)==ERROR_SUCCESS)
				{
					pExtensionLinkName = malloc(dwLength);

					if (pExtensionLinkName)
					{
						DWORD dwType;

						RegQueryValueEx(hExtensionKey, NULL, NULL, &dwType, (BYTE *)pExtensionLinkName, &dwLength);
		
						fExists = TRUE;
					}
				}
			}
		}
		
		if (!fExists)
		{
			DWORD nLen = StringLengthBytes(pExtensionInfo->pExtensionLinkKeyName);
			pExtensionLinkName = malloc(nLen);
			
			if (pExtensionLinkName)
			{
				memcpy(pExtensionLinkName, pExtensionInfo->pExtensionLinkKeyName,nLen);
			}
			
			/* replace value of default key */
			RegSetValueEx(hExtensionKey, NULL, 0, REG_SZ, (const BYTE *)pExtensionLinkName, nLen);
		}

		RegCloseKey(hExtensionKey);
	}

	if (pExtensionLinkName!=NULL)
	{
		DWORD dwDisposition;

		/* create it if it doesn't exist, otherwise open it */
		if (RegCreateKeyEx(HKEY_CLASSES_ROOT, pExtensionLinkName, 0, _T(""),REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,NULL, &hExtensionKey,&dwDisposition)==ERROR_SUCCESS)
		{
			if (!fOpenWith)
			{
				/* if not friendly, replace description and icon for our own */
				HKEY hDefaultIcon;
				TCHAR Buffer[256];

				/* set the description of the extension */
				RegSetValueEx(hExtensionKey, NULL, 0, REG_SZ, (const BYTE *)pExtensionInfo->pExtensionDescription, StringLengthBytes(pExtensionInfo->pExtensionDescription));
			
				_stprintf(Buffer,_T("\"%s\",-%d"),pExtensionInfo->pApplicationPath,pExtensionInfo->nIconIndex);

				if (RegCreateKeyEx(hExtensionKey,_T("DefaultIcon"), 0, _T(""),REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hDefaultIcon, &dwDisposition)==ERROR_SUCCESS)
				{
					RegSetValueEx(hDefaultIcon, NULL, 0, REG_SZ, (const BYTE *)Buffer, StringLengthBytes(Buffer));
				
					RegCloseKey(hDefaultIcon);
				}
			}		

			{
				HKEY hShellKey;

				if (RegCreateKeyEx(hExtensionKey,_T("shell"), 0, _T(""), REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS, NULL, &hShellKey, &dwDisposition)==ERROR_SUCCESS)
				{
					HKEY hOpenKey;
					const TCHAR *sKey;

					/* if not friendly replace the default "open" with our own */
					/* if friendly, create a open with */
					if (!fOpenWith)
					{
						sKey = _T("open");
					}
					else
					{
						sKey = OpenWithString;
					}

					if (RegCreateKeyEx(hShellKey,sKey, 0, _T(""),REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,&hOpenKey, &dwDisposition)==ERROR_SUCCESS)
					{
						RegisterExtension_OpenKeySetup(pExtensionInfo,hOpenKey, fOpenWith);

						RegCloseKey(hOpenKey);

					}

					RegCloseKey(hShellKey);
				}
			}

			RegCloseKey(hExtensionKey);
		}

		free(pExtensionLinkName);

	}

}