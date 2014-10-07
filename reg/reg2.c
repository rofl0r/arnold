


void	MyApp_Registry_StoreStringToCurrentKey(char *, char *);
char	*MyApp_Registry_GetStringFromCurrentKey(char *);
void	MyApp_Registry_StoreIntToCurrentKey(char *, int);
int		MyApp_Registry_GetIntFromCurrentKey(char *);
BOOL	MyApp_Registry_CheckValueIsPresentInCurrentKey(char *ValueName);


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

void	MyApp_Registry_InitialiseKey();
BOOL	MyApp_Registry_CheckKeyPresent();
BOOL	MyApp_Registry_OpenKey();
void MyApp_Registry_CloseKey();

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
