#include <windows.h>
#include "keystack.h"
#include "reg.h"

typedef struct
{
	/* extension excluding seperating period character */
	const char *pExtension;
	/* extension description */
	const char *pDescription;
	/* index of icon */
	int IconIndex;
} Extension;

typedef struct
{
	const char *pApplicationTitle;
	int Type;
	const char *pApplicationPath;
} ApplicationData;

void	FileAssociation_Init(Extension *pExtension, ApplicationData *pAppData)
{
	char ExtensionString[256];
	HKEY ExtensionKey;
	DWORD Result;
	char ValueString[256];

	sprintf(ExtensionString,".%s",pExtension->pExtension);


	if (RegCreateKeyEx(HKEY_CLASSES_ROOT, 
		ExtensionString, 
		0, 
		"REG_SZ",
		REG_OPTION_NON_VOLATILE,
		KEY_QUERY_VALUE|KEY_SET_VALUE,
		NULL, &ExtensionKey,&Result)==ERROR_SUCCESS)
	{
		sprintf(ExtensionString,"%s.%sfile",pAppData->pApplicationTitle,pExtension->pExtension);

		if (RegSetValue(
		  ExtensionKey,         // handle to key to set value for
		  NULL,  // address of subkey name
		  REG_SZ,      // type of value
		  ExtensionString,    // address of value data
		  strlen(ExtensionString) // size of value data
		)==ERROR_SUCCESS)
		{
		printf("hello");




 
		}
		RegCloseKey(ExtensionKey);
	}

	if (RegCreateKeyEx(HKEY_CLASSES_ROOT, 
		ExtensionString, 
		0, 
		"REG_SZ",
		REG_OPTION_NON_VOLATILE,
		KEY_SET_VALUE|KEY_CREATE_SUB_KEY,
		NULL, &ExtensionKey,&Result)==ERROR_SUCCESS)
	{
		HKEY ExtensionSubKeys[3];

		sprintf(ValueString,"%s",pExtension->pDescription);

		if (RegSetValue(
		  ExtensionKey,         // handle to key to set value for
		  NULL,  // address of subkey name
		  REG_SZ,      // type of value
		  ValueString,    // address of value data
		  strlen(ValueString) // size of value data
		)==ERROR_SUCCESS)
		{ 
		}

		if (RegCreateKeyEx(ExtensionKey, "DefaultIcon",0,"REG_SZ", REG_OPTION_NON_VOLATILE,
			KEY_SET_VALUE, NULL, &ExtensionSubKeys[0], &Result)==ERROR_SUCCESS)
		{
			sprintf(ValueString,"\"%s\",-%d",pAppData->pApplicationPath,pExtension->IconIndex);

			if (RegSetValue(
			  ExtensionSubKeys[0],         // handle to key to set value for
			  NULL,  // address of subkey name
			  REG_SZ,      // type of value
			  ValueString,    // address of value data
			  strlen(ValueString) // size of value data
			)==ERROR_SUCCESS)
			{ 
			}

			RegCloseKey(ExtensionSubKeys[0]);

		}

		if (RegCreateKeyEx(ExtensionKey, "shell",0,"REG_SZ", REG_OPTION_NON_VOLATILE,
			KEY_CREATE_SUB_KEY, NULL, &ExtensionSubKeys[0], &Result)==ERROR_SUCCESS)
		{
			if (RegCreateKeyEx(ExtensionSubKeys[0], "open",0,"REG_SZ", REG_OPTION_NON_VOLATILE,
				KEY_CREATE_SUB_KEY, NULL, &ExtensionSubKeys[1], &Result)==ERROR_SUCCESS)
			{
				if (RegCreateKeyEx(ExtensionSubKeys[1], "command",0,"REG_SZ", REG_OPTION_NON_VOLATILE,
					KEY_SET_VALUE, NULL, &ExtensionSubKeys[2], &Result)==ERROR_SUCCESS)
				{
					sprintf(ValueString,"\"%s\" \"%%1\"",pAppData->pApplicationPath);

					if (RegSetValue(
						  ExtensionSubKeys[2],         // handle to key to set value for
						  NULL,  // address of subkey name
						  REG_SZ,      // type of value
						  ValueString,    // address of value data
						  strlen(ValueString) // size of value data
						)==ERROR_SUCCESS)
						{ 
						}

					RegCloseKey(ExtensionSubKeys[2]);
				}


				RegCloseKey(ExtensionSubKeys[1]);
			}



			RegCloseKey(ExtensionSubKeys[0]);

		}

		RegCloseKey(ExtensionKey);
	}
}


void main()
{
	Extension extension;
	ApplicationData appData;

	extension.pExtension="cpr";
	extension.pDescription="Amstrad CPC+/GX4000 cartridge file";
	extension.IconIndex = 143;
	
	appData.pApplicationPath="f:\\arnold\\arnold.exe";
	appData.pApplicationTitle="arnold";

	FileAssociation_Init(&extension, &appData);

	extension.pExtension="dsk";
	extension.pDescription="Amstrad CPC/CPC+ disk file";
	extension.IconIndex = 144;
	
	FileAssociation_Init(&extension, &appData);

	extension.pExtension="cdt";
	extension.pDescription="Amstrad CPC/CPC+ cassette tape file";
	extension.IconIndex = 142;
	
	FileAssociation_Init(&extension, &appData);

	extension.pExtension="tzx";
	extension.pDescription="Amstrad CPC/CPC+ cassette tape file";
	extension.IconIndex = 142;
	
	FileAssociation_Init(&extension, &appData);
}