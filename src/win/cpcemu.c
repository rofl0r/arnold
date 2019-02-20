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
 *  
 *  
 */
#define STRICT

#include "precomp.h"
#define AUDIO

//#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <windows.h>

/* resource data */
#include "resource.h"

#define DIRECTINPUT_VERSION 0x0300

#include "../cpc/messages.h"

//#define WIN32_LEAN_AND_MEAN
//#define WIN32_EXTRA_LEAN
//#include <windows.h>
#include <direct.h>

#include <dinput.h>

#include "directx/graphlib.h"
#include "filedlg.h"
#include "reg.h"
#include "../cpc/cpc.h"
#include "../cpc/diskimage/diskimg.h"
#include "../cpc/multface.h"
#include "../cpc/cheatdb.h"

//#include "cpc/audio.h"
#include "directx/dd.h"
#include "directx\di.h"
#include "directx\ds.h"
#include "mylistvw.h"
#include "cassdlg.h"
#include "romcfg.h"
#include "chtdb.h"
#include "../cpc/debugger/gdebug.h"
#include "joy.h"
#include "win.h"
#include "debugger.h"
/*#include "..\contrib\include\console.h" */

extern BOOL Host_LockSpeed;

BOOL	Settings_OpenSettings(int Mode)
{
	if (MyApp_Registry_CheckKeyPresent())
	{
		/* get settings */


	}
	else
	{
		MyApp_Registry_InitialiseKey();
	}

	return MyApp_Registry_OpenKey();
}


/*----------------------------------------------------------------------------------*/

/* class name for creation of windows etc */
#define APP_CLASS_NAME "ArnoldEmu"

/* name of app as reported in dialogs */
#define APP_TITLE_TEXT	"Arnold Emulator"

/* name of app for DDE */
#define APP_NAME	"Arnold"

/* release/version of Arnold as seen by Joe Public */
const TCHAR *sVersion=_T("04/01/2004");

/*----------------------------------------------------------------------------------*/

/* main header for this file */
#include "cpcemu.h"

/* the ZIP archive handling code */
#include "zipsupport.h"
/* the user interface for browsing an archive */
#include "zipsupui.h"

/* for microsoft wheel mouse/intellimouse */
#include <zmouse.h>

/* for drag and drop */
#include <shellapi.h>

/* for DDE */
#include <ddeml.h>

#ifdef _DEBUG
/* for CRT debugging functions (memory leak checking etc) */
#include <crtdbg.h>
#endif

/* for ANSI/UNICODE text functions */
#include <tchar.h>

/* for Windows' common controls */
#include <commctrl.h>

/* for converting command-line string into argc, argv style */
#include "cmdline.h"

#include <ctype.h>

/* for Auto-Type and Auto-Run */
#include "../cpc/autotype.h"

/* for Auto-Run */
#include "../cpc/amsdos.h"

/* combo box helper macros */
#include "comboh.h"


/*----------------------------------------------------------------------------------------------*/

void	CPCEMU_DetectFileAndLoad(const TCHAR *pFilename);
void	CPCEMU_ProcessCommandLine(LPCTSTR sCommandLine);

/*----------------------------------------------------------------------------------------------*/
void	Interface_LoadMultifaceROM(const MULTIFACE_ROM_TYPE RomType, const TCHAR *sFilename)
{
	unsigned char *pRomData;
	unsigned long RomSize;

	/* try to load it */
	LoadFile(sFilename, &pRomData, &RomSize);

	if (pRomData!=NULL)
	{
		Multiface_SetRomData(RomType, pRomData, RomSize);

		free(pRomData);
	}
}

/*----------------------------------------------------------------------------------------------*/
void	Interface_InsertDiskFromFile(int nDriveID, const TCHAR *sFilename)
{
	unsigned char *pLocation;
	unsigned long Length;

	LoadFile(sFilename, &pLocation, &Length);

	if (pLocation)
	{
		DiskImage_InsertDisk(nDriveID, pLocation, Length);

		free(pLocation);
	}
}
/*----------------------------------------------------------------------------------------------*/
BOOL Interface_InsertRomFromFile(const int RomIndex,const TCHAR *pFilename)
{
	unsigned char *pRomData;
	unsigned long RomDataSize;

	LoadFile(pFilename, &pRomData, &RomDataSize);
	if (pRomData!=NULL)
	{
		ExpansionRom_SetRomData(pRomData, RomDataSize, RomIndex);
	
		free(pRomData);
	
		return TRUE;
	}

	return FALSE;
}

/*----------------------------------------------------------------------------------------------*/
typedef struct
{
	unsigned char *pData;		/* pointer to binary data */
	unsigned long nLength;		/* length of binary data */
} BINARY_DATA_BLOCK;

/* built in data */
static BINARY_DATA_BLOCK	CPCPLUS_SystemCartridge_ENG;
static BINARY_DATA_BLOCK	CPC464_OperatingSystemRom_ENG;
static BINARY_DATA_BLOCK	CPC464_BASICRom_ENG;
static BINARY_DATA_BLOCK	CPC664_OperatingSystemRom_ENG;
static BINARY_DATA_BLOCK	CPC664_BASICRom_ENG;
static BINARY_DATA_BLOCK	CPC6128_OperatingSystemRom_ENG;
static BINARY_DATA_BLOCK	CPC6128_BASICRom_ENG;
static BINARY_DATA_BLOCK	KCC_OperatingSystemRom_ENG;
static BINARY_DATA_BLOCK	KCC_BASICRom_ENG;
static BINARY_DATA_BLOCK	AMSDOSRom_ENG;

APP_DATA AppData;


#ifdef _UNICODE

#include <winnls.h>
/*----------------------------------------------------------------------------------------------*/
LPCSTR ConvertUnicodeStringToMultiByte(const TCHAR *sUnicodeString)
{
	LPSTR MultiByteString = NULL;

	int nBytes;

	/* get number of bytes required for converted string */
	nBytes = WideCharToMultiByte(CP_ACP,0,sUnicodeString,_tcslen(sUnicodeString), NULL,0, NULL, NULL);
	
	if (nBytes!=0)
	{
		/* allocate space for converted string */
		MultiByteString = malloc(nBytes);

		if (MultiByteString)
		{
			/* do the conversion */
			WideCharToMultiByte(CP_ACP, 0,sUnicodeString, _tcslen(sUnicodeString), MultiByteString, nBytes, NULL, NULL);
		}
	}

	/* return converted string or NULL if there was an error */
	return (LPCSTR)MultiByteString;
}

/*----------------------------------------------------------------------------------------------*/
const TCHAR *ConvertMultiByteToUnicode(LPCSTR sMultiByte)
{
	TCHAR *sUnicodeString = NULL;
	int nChars;

	nChars = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,sMultiByte,strlen(sMultiByte),NULL,0);

	if (nChars!=0)
	{
		/* allocate space for converted string */
		sUnicodeString = malloc(nChars*sizeof(TCHAR));

		if (sUnicodeString)
		{
			MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,sMultiByte,strlen(sMultiByte),sUnicodeString,nChars);
		}
	}
	
	return sUnicodeString;
}
#endif
/*----------------------------------------------------------------------------------------------*/
static void GenerateTempFilename(TCHAR *TempFilename)
{
	TCHAR TempPath[MAX_PATH];

	/* get temp path */
	GetTempPath(MAX_PATH,TempPath);
	/* generate full temp filename */
	GetTempFileName(TempPath, _T("ARN"),0,TempFilename);
}

/*----------------------------------------------------------------------------------------------*/

static void GetDataFromCustomResource(HMODULE hModule, unsigned char **ppPtr, unsigned long *pLength, LPTSTR sResource)
{
	/* find our custom resource by the id of the resource and get a handle to it's info block */
	HRSRC hResource = FindResource(hModule,(LPCTSTR)sResource,RT_RCDATA);

	if (hResource!=NULL)
	{
		/* load the resource into global memory; and get the handle to the memory */
		HGLOBAL hResourceMemory = LoadResource(hModule, hResource);

		if (ppPtr!=NULL)
		{
			/* lock the resource to get a pointer to the data in memory */
			LPVOID pResourceData = LockResource(hResourceMemory);

			*ppPtr = (unsigned char *)pResourceData;
		}

		if (pLength!=NULL)
		{
			/* get size of the resource; most functions in the main core require a size to work with */
			DWORD nResourceSize = SizeofResource(hModule, hResource);

			*pLength = nResourceSize;
		}
	}
}

/*----------------------------------------------------------------------------------------------*/
BOOL LoadFile(const TCHAR *pFilename, unsigned char **ppLocation, unsigned long *pLength)
{
	BOOL fSuccess = FALSE;
	HANDLE FileHandle;
	DWORD FileLength;
	DWORD FileLengthRead;

	*ppLocation = NULL;
	*pLength = 0;

	/* attempt to open file for reading
	
	 when reading allow sharing for reading only */
	FileHandle = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (FileHandle!=INVALID_HANDLE_VALUE)
	{
		/* file opened ok */

		BY_HANDLE_FILE_INFORMATION FileInformation;

		if (GetFileInformationByHandle(FileHandle, &FileInformation))
		{
			/* got file information */

			FileLength = FileInformation.nFileSizeLow;

			/* if file is not zero size */
			if ((FileInformation.nFileSizeLow | FileInformation.nFileSizeHigh)!=0)
			{
				void	*pFileMemory;

				/* allocate */
				pFileMemory = (void *)malloc(FileLength);
				
				if (pFileMemory!= NULL)
				{
					/* load */
					if (ReadFile(FileHandle, (LPVOID)pFileMemory, FileLength, &FileLengthRead, NULL))
					{
						*ppLocation = pFileMemory;
						*pLength = FileLength;

						fSuccess = TRUE;
					}

					if (!fSuccess)
						free(pFileMemory);
				}

			}
		}

		CloseHandle(FileHandle);
	}

	return fSuccess;
}

/*----------------------------------------------------------------------------------------------*/
BOOL SaveFile(const TCHAR *pFilename, unsigned char *pLocation, unsigned long Length)
{
	BOOL fSuccess = FALSE;
	HANDLE FileHandle;

	/* attempt to open file for output
	
	 when writing, prevent sharing of file */
	FileHandle = CreateFile(pFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);

	if (FileHandle!=INVALID_HANDLE_VALUE)
	{
		DWORD FileLengthWritten;
	
		if (WriteFile(FileHandle, (LPVOID)pLocation, Length, &FileLengthWritten, NULL))
		{
			fSuccess = TRUE;
		}

		CloseHandle(FileHandle);
	}

	return FALSE;
}

/*----------------------------------------------------------------------------------------------*/

void CPCEMU_HandleDragDrop(HANDLE DropHandle)
{
	if (DropHandle!=0)
	{
		unsigned int i;

		/* get number of files dropped */
		unsigned int NoOfFilesDropped = DragQueryFile(DropHandle, 0xFFFFFFFF, NULL, 0);
		
		if (NoOfFilesDropped!=0)
		{
			int MaxFilenameSize;
			TCHAR *pFilenameBuffer;

			MaxFilenameSize = 0;
			
			/* get size of largest filename */
			for (i=0; i!=NoOfFilesDropped; i++)
			{
				int FilenameSize;

				/* get size of the filename for this file */
				FilenameSize = DragQueryFile(DropHandle, i, NULL, 0);

				if (FilenameSize>MaxFilenameSize)
				{
					/* update max filename size if the length of this
					filename is greater than current */
					MaxFilenameSize = FilenameSize;
				}
			}

			/* allocate memory for filenames to use */
			pFilenameBuffer = malloc((MaxFilenameSize+2)*sizeof(TCHAR));

			if (pFilenameBuffer!=NULL)
			{
				for (i=0; i<NoOfFilesDropped; i++)
				{
					/* get this filename */
					DragQueryFile(DropHandle, i, pFilenameBuffer, MaxFilenameSize+1);

					/* do function to detect what this file is and
					 to load it if necessary */

					CPCEMU_DetectFileAndLoad(pFilenameBuffer);
				}
			
				/* free buffer to hold filenames */
				free(pFilenameBuffer);
			}
		}

		/* finish */
		DragFinish(DropHandle);
	}
}
/*----------------------------------------------------------------------------------------------*/
enum
{
	ZIP_SUPPORT_NOT_ZIP,		/* not a zip file */
	ZIP_SUPPORT_ZIP_OK,			/* a zip file and user selected a file */
	ZIP_SUPPORT_ZIP_CANCEL,		/* a zip file but user selected cancel */
	ZIP_SUPPORT_ZIP_ERROR,		/* a zip file but there was an error */
};

/* returns NULL if the file is not a zip file or there are problems with it,
otherwise returns name of file extracted from zip */
int Interface_HandleZipFile(const HWND hwnd, const TCHAR *pFilename, TCHAR *OutputFilename)
{
	int nStatus = ZIP_SUPPORT_NOT_ZIP;

	if (ZipSupport_IsZipArchive(pFilename)==ZIP_SUPPORT_OK)
	{
		file_list theFileList;
		archive_file_open fileOpen;

		file_list_init(&theFileList);

		/* generate file list */
		ZipSupport_GenerateFileList(pFilename, &theFileList);

		/* show file selector */
		if (ArchiveUI_Show(hwnd,&theFileList,&fileOpen))
		{
			/* generate temp filename for output */
			GenerateTempFilename(OutputFilename);

			/* extract from archive to temp file */
			if (ZipSupport_ExtractFile(pFilename, fileOpen.pFilename, OutputFilename))
			{
				nStatus = ZIP_SUPPORT_ZIP_OK;
			}
			else
			{
				nStatus = ZIP_SUPPORT_ZIP_ERROR;
			}
		}
		else
		{
			nStatus = ZIP_SUPPORT_ZIP_CANCEL;
		}

		/* delete file list */
		file_list_delete(&theFileList);
	}

	return nStatus;
}

/*----------------------------------------------------------------------------------------------*/
static BOOL CALLBACK About_DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			/* setup version/release text */
			TCHAR InfoText[256];

			/* the release date/version; defined at top of this file.
			Not based on file date of this file. */
			_stprintf(InfoText,_T("Release date %s"),sVersion);
				
#ifdef _DEBUG
#ifndef _INFO
#define _INFO
#endif
#endif

#ifdef _UNICODE
#ifndef _INFO
#define _INFO
#endif
#endif

#ifdef _INFO
			_tcscat(InfoText," (");
#endif

#ifdef _DEBUG
			/* only show if debug build. Don't bother reporting if release build */
			_tcscat(InfoText,"Debug Build.");
#endif

#ifdef _UNICODE
			/* only show if compiled for unicode. Don't bother otherwise */
			_tcscat(InfoText,"Unicode enabled.");
#endif

#ifdef _INFO
			_tcscat(InfoText,")");
#endif

#undef _INFO

			SetDlgItemText(hwndDlg, IDC_STATIC_INFO, InfoText);
		}
		break;

		/* white background; the background to the app logo is white */
		case WM_CTLCOLORSTATIC:
			return GetStockObject(WHITE_BRUSH);

		/* white background; the background to the app logo is white */
		case WM_CTLCOLORDLG:
			return GetStockObject(WHITE_BRUSH);

		case WM_COMMAND:
		{
			if (LOWORD(wParam)==IDOK)
				return EndDialog(hwndDlg, 0);
		}
		break;

	
		default:
			break;
	}

	return FALSE;
}
/*----------------------------------------------------------------------------------------------*/
typedef struct
{
	LPCTSTR sMessage;				/* message for message box */
	LPCTSTR sCaption;				/* caption for message box */
	BOOL	fCheck;					/* returns state of "show next time" checkbox */
	UINT	uType;					/* style of message box */
} MESSAGE_BOX_DATA;

#define WARNING_MESSAGE_BOX_SHOW_NEXT_TIME	0x8000
#define WARNING_MESSAGE_BOX_OK				0x0001
#define WARNING_MESSAGE_BOX_CANCEL			0x0002

/* this is static, because I can't work out how to use GetWindowLong(GWL_USERDATA) with a 
modal dialog box :( */
static MESSAGE_BOX_DATA MessageBoxData;

static BOOL CALLBACK WarningMessageBox_DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			int i;
			int nDefButton;

			const int ButtonIds[]={
				IDOK,
				IDCANCEL
			};

			/* get the static which shows the icon */
			HWND hIconStatic = GetDlgItem(hwndDlg, IDC_WARNING_ICON);
			
			/* load the icon */
			HICON hIcon = LoadIcon(NULL, IDI_WARNING);

			/* set the icon */
			SendMessage(hIconStatic, STM_SETICON, (WPARAM)hIcon, (LPARAM)0);

			/* set the title text */
			SetWindowText(hwndDlg, MessageBoxData.sCaption);

			/* set the message text */
			SetDlgItemText(hwndDlg, IDC_WARNING_MESSAGE, MessageBoxData.sMessage);
		
			/* set the text on the buttons; is there a proper way to get the correct
			language?? */
			switch (MessageBoxData.uType & MB_TYPEMASK)
			{
				case MB_OKCANCEL:
				{
					SetDlgItemText(hwndDlg, IDOK, _T("Ok"));
					SetDlgItemText(hwndDlg, IDCANCEL, _T("Cancel"));
				}
				break;

				case MB_YESNO:
				{
					SetDlgItemText(hwndDlg, IDOK, _T("Yes"));
					SetDlgItemText(hwndDlg, IDCANCEL, _T("No"));
				}
				break;
			
				default:
					break;
			}

			/* set the default button */
			nDefButton = MessageBoxData.uType & MB_DEFMASK;

			for (i=0; i<2; i++)
			{
				HWND hwndButton;
				DWORD nStyle;
				DWORD nStyleOrMask = 0;

				if (i==nDefButton)
				{
					nStyleOrMask = BS_DEFPUSHBUTTON;
				}
				/* get button */
				hwndButton = GetDlgItem(hwndDlg, ButtonIds[i]);
				/* adjust style */
				nStyle = GetWindowLong(hwndButton,GWL_STYLE);
				nStyle&=~BS_DEFPUSHBUTTON;
				nStyle|=nStyleOrMask;
				SetWindowLong(hwndButton, GWL_STYLE,nStyle);
			}

			CheckDlgButton(hwndDlg, IDC_CHECK_SHOW_WARNING, BST_CHECKED);
		}
		break;

		case WM_COMMAND:
		{
			if ((LOWORD(wParam)==IDOK) || (LOWORD(wParam)==IDCANCEL))	
			{
				/* get check state */
				MessageBoxData.fCheck = IsDlgButtonChecked(hwndDlg, IDC_CHECK_SHOW_WARNING);

				/* end the dialog; returning ID of the button pressed */
		
				return EndDialog(hwndDlg, LOWORD(wParam));
			}
		}
		break;

		default:
			break;
	}

	return FALSE;
}

/* a MessageBox implementation which has a check box to control if the warning is shown again */
static int CPCEMU_WarningMessageBox(HWND hwndParent, LPCTSTR sMessage, LPCTSTR sCaption, UINT uType)
{
	int nStatus;
	int nID;

	MessageBoxData.sCaption = sCaption;
	MessageBoxData.sMessage = sMessage;
	MessageBoxData.uType = uType;
	
	nID = DialogBox((HINSTANCE)GetWindowLong(hwndParent, GWL_HINSTANCE),MAKEINTRESOURCE(IDD_DIALOG_WARNING),hwndParent, WarningMessageBox_DialogProc);

	nStatus = 0;
	if (nID==IDOK)
		nStatus |= WARNING_MESSAGE_BOX_OK;
	else if (nID==IDCANCEL)
		nStatus |= WARNING_MESSAGE_BOX_CANCEL;
	if (MessageBoxData.fCheck)
		nStatus |= WARNING_MESSAGE_BOX_SHOW_NEXT_TIME;

	return nStatus;
}


/*----------------------------------------------------------------------------------------------*/
BOOL CPCEMU_DoWarningMessageBox(HWND hwndParent, LPCTSTR sMessage, BOOL *fShowFlag)
{
	BOOL fDoAction = TRUE;

	/* show message box? */
	if (*fShowFlag)
	{
		int nStatus = CPCEMU_WarningMessageBox(hwndParent,sMessage,_T("Arnold"),MB_YESNO);	

		/* show message box next time? */
		if ((nStatus&WARNING_MESSAGE_BOX_SHOW_NEXT_TIME)==0)
			*fShowFlag = FALSE;

		/* do reset? */
		fDoAction = FALSE;
		if (nStatus&WARNING_MESSAGE_BOX_OK)
			fDoAction = TRUE;
	}

	return fDoAction;
}

/*----------------------------------------------------------------------------------------------*/
static BOOL fShowReset = TRUE;

void CPCEMU_DoReset(HWND hwndParent)
{
	if (CPCEMU_DoWarningMessageBox(hwndParent,_T("Are you sure you want to reset the Amstrad?\nAll unsaved data and RAM contents will be lost."), &fShowReset))
	{
		/* do reset */
		CPC_Reset();
	}
}

/*----------------------------------------------------------------------------------------------*/
static BOOL fShowQuit = TRUE;

BOOL CPCEMU_DoQuit(HWND hwndParent)
{
	return CPCEMU_DoWarningMessageBox(hwndParent,_T("Are you sure you want to exit Arnold?"),&fShowQuit);

}
/*----------------------------------------------------------------------------------------------*/

typedef struct
{
	const int nIconIndex;	/* icon in resources for this type */
	LPCTSTR sName;			/* name of this type */
	LPCTSTR sDescription;	/* description used in shell */
	LPCTSTR sLinkName;		/* link type used in shell */
} MEDIA_DATA;

/* these are used to group the extensions together */
/* so all extensions which are MEDIA_TYPE_DISK will share the same identification in the UI,
share the same icon and description in the shell and will be launched in the same way with Arnold */
const MEDIA_DATA Media[]=
{
	{IDI_ICON_DISK,_T("Disk"),_T("Amstrad CPC Disk Image File"),_T("Arnold.DskFile")},
	{IDI_ICON_CASSETTE,_T("Cassette"),_T("Amstrad CPC Tape Image File"), _T("Arnold.CdtFile")},
	{IDI_ICON_CARTRIDGE,_T("Cartridge"),_T("Amstrad CPC+ Cartridge File"),_T("Arnold.CprFile")},
	{IDI_ICON_SNAPSHOT,_T("Snapshot"),_T("Amstrad CPC Memory Snapshot File"),_T("Arnold.SnaFile")},

	/* add new media definitions here */
	/* these must be in the same order as defined in the enum below */
};

enum
{
	MEDIA_TYPE_DISK = 0,
	MEDIA_TYPE_CASSETTE,
	MEDIA_TYPE_CARTRIDGE,
	MEDIA_TYPE_SNAPSHOT
	/* add new media types here */
};

typedef struct
{
	LPCTSTR sExtension;		/* extension of this type */
	const int nMedia;				/* the media type to register it under */
} ASSOCIATION_DATA;

/* this defines the mapping of extensions to media types */
const ASSOCIATION_DATA Associations[]=
{
	{_T(".dsk"),MEDIA_TYPE_DISK},
	{_T(".cpr"),MEDIA_TYPE_CARTRIDGE},
	{_T(".cdt"),MEDIA_TYPE_CASSETTE},
	{_T(".tzx"),MEDIA_TYPE_CASSETTE},
	{_T(".csw"),MEDIA_TYPE_CASSETTE},
	{_T(".wav"),MEDIA_TYPE_CASSETTE},
	{_T(".voc"),MEDIA_TYPE_CASSETTE},
	{_T(".sna"),MEDIA_TYPE_SNAPSHOT},
	/* add new extensions here */
};
/*----------------------------------------------------------------------------------------------*/
void Associations_SelectAll(HWND hListView, BOOL fSelect)
{
	int i;

	DWORD nState = INDEXTOSTATEIMAGEMASK(1);
	if (fSelect)
	{
		nState = INDEXTOSTATEIMAGEMASK(2);
	}

	for (i=0; i<ListView_GetItemCount(hListView); i++)
	{
		ListView_SetItemState(hListView, i, nState, LVIS_STATEIMAGEMASK);
	}
}
/*----------------------------------------------------------------------------------------------*/

BOOL CALLBACK Associations_DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			HWND hListView = GetDlgItem(hwndDlg,IDC_LIST_ASSOCIATIONS);
			int i;

			ListView_SetExtendedListViewStyleEx(hListView, LVS_EX_CHECKBOXES ,LVS_EX_CHECKBOXES);

			/* create columns */
			MyListView_AddColumn(hListView, _T("Extension"), 0);
			MyListView_AddColumn(hListView, _T("Media"), 1);
			
			/* add items */
			for (i=0; i<sizeof(Associations)/sizeof(Associations[0]); i++)
			{
				int nMedia = Associations[i].nMedia;
				const MEDIA_DATA *pMedia = &Media[nMedia];

				MyListView_AddItem(hListView,Associations[i].sExtension,0,i, i);
				MyListView_AddItem(hListView,pMedia->sName, 1,i, i);
			}

			Associations_SelectAll(hListView, TRUE);
		}
		break;

		case WM_COMMAND:
		{
			WORD wID;

			wID = LOWORD(wParam);

			switch (wID)
			{
				case ID_SELECT_ALL:
				case ID_CLEAR_ALL:
				{
					HWND hListView = GetDlgItem(hwndDlg,IDC_LIST_ASSOCIATIONS);

					Associations_SelectAll(hListView, (wID==ID_SELECT_ALL));
				}
				break;

				case IDOK:
				{
					HWND hListView = GetDlgItem(hwndDlg,IDC_LIST_ASSOCIATIONS);
					int i;
					EXTENSION_INFO Extension;
					TCHAR Path[MAX_PATH];
					BOOL fOpenWith = FALSE;

					/* get filename and path of this executable */
					GetModuleFileName(NULL, Path, MAX_PATH);

					if (IsDlgButtonChecked(hwndDlg, IDC_CHECK_OPENWITHARNOLD))
						fOpenWith = TRUE;


					/* for those items which are checked, then associate */
					for (i=0; i<ListView_GetItemCount(hListView); i++)
					{
						/* get checked state */
						int nState = ListView_GetItemState(hListView, i,LVIS_STATEIMAGEMASK);
						int nAssociation = -1;
						BOOL fAssociate = FALSE;

						/* get data for item which is the index of the association */
						LVITEM Item;
						Item.mask = LVIF_PARAM;
						Item.iItem = i;
						Item.iSubItem = 0;
						if (ListView_GetItem(hListView,&Item))
						{
							const ASSOCIATION_DATA *pAssociation;
							const MEDIA_DATA *pMedia;
							int nMedia;
							
							nAssociation = (int)Item.lParam;
							pAssociation = &Associations[nAssociation];
							nMedia = pAssociation->nMedia;
							pMedia = &Media[nMedia];

							Extension.nIconIndex = pMedia->nIconIndex;
							Extension.pApplicationPath = Path;
							Extension.pExtensionDescription = pMedia->sDescription;
							Extension.pExtensionKeyName = pAssociation->sExtension;
							Extension.pExtensionLinkKeyName = pMedia->sLinkName;
						}

						if (nAssociation!=-1)
						{
							/* checked? */
							if (nState==INDEXTOSTATEIMAGEMASK(2))
								fAssociate = TRUE;

							if (fAssociate)
							{
								RegisterExtension(&Extension,FALSE);
							}
						
							if (fOpenWith)
							{
								RegisterExtension(&Extension,TRUE);
							}
						}
					}

				}
				return EndDialog(hwndDlg,1);
			
				case IDCANCEL:
					return EndDialog(hwndDlg,0);
		
				default:
					break;
			
			}
		}
		break;
	}

	return FALSE;
}
/*----------------------------------------------------------------------------------------------*/
void Associations_Dialog(HWND hwndParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwndParent, GWL_HINSTANCE);

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_ASSOCIATIONS),hwndParent, Associations_DialogProc);
}
/*----------------------------------------------------------------------------------------------*/
BOOL CALLBACK Snapshot_DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			/* fill version combo */
			HWND hCombo = GetDlgItem(hwndDlg, IDC_COMBO_VERSION);

			ComboBoxHelper_InsertString(hCombo, 0, _T("2"));
			ComboBoxHelper_InsertString(hCombo, 1, _T("3"));
			
			ComboBoxHelper_SetCurSel(hCombo, 1);

			/* set check mark */
			CheckDlgButton(hwndDlg, IDC_CHECK_WRITE_2ND_64K,BST_CHECKED);
		}
		break;

		case WM_COMMAND:
		{
			WORD wID;

			wID = LOWORD(wParam);

			switch (wID)
			{
				case IDOK:
				{
					/* save the snapshot */
					HWND hCombo = GetDlgItem(hwndDlg, IDC_COMBO_VERSION);
					int nSel;
					int nVersion;

					if (IsDlgButtonChecked(hwndDlg,IDC_CHECK_WRITE_2ND_64K))
						AppData.SnapshotSize = 128;
					else
						AppData.SnapshotSize = 64;

					nSel = ComboBoxHelper_GetCurSel(hCombo);

					nVersion = 2;
					switch (nSel)
					{
						case 1:
							nVersion = 3;
							break;
						default:
							break;
					}

					AppData.SnapshotVersion = 3;
				}
				return EndDialog(hwndDlg, TRUE);

				case IDCANCEL:
					return EndDialog(hwndDlg, FALSE);
			
				default:
					break;
			}
		}
		break;

		default:
			break;
	}

	return FALSE;
}
/*----------------------------------------------------------------------------------------------*/

void SnapshotSave_Dialog(HWND hwndParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwndParent,GWL_HINSTANCE);

	if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_SNAPSHOT), hwndParent,Snapshot_DialogProc))
	{
		Interface_SaveSnapshot(hwndParent);
	}
}

/*----------------------------------------------------------------------------------------------*/
typedef struct
{
	TCHAR *pPtr;			/* string to parse */
	unsigned long nOffset;		/* current character position */
	unsigned long nSize;		/* length in chars */
} DDEParse;

/*------------------------------------------------------------------------*/
void DDEParse_SkipWhitespace(DDEParse *pParse)
{
	TCHAR ch;

	if (pParse->nOffset==pParse->nSize)
		return;

	ch = pParse->pPtr[pParse->nOffset];
	pParse->nOffset++;

	while ((pParse->nOffset!=pParse->nSize) && ((ch==_T(' ')) || (ch==_T('\t'))))
	{
		ch = pParse->pPtr[pParse->nOffset];
		pParse->nOffset++;
	}

	if (pParse->nOffset!=pParse->nSize)
		pParse->nOffset--;
}
/*------------------------------------------------------------------------*/
/* escape a string. The reason to do this is so we can pass
quoted strings to our app and enclose them in quotes, so that they can be treated
as a single parameter */

/* TODO: Support escaped strings using \ ? */
LPCTSTR DDE_EscapeString(LPCTSTR sUnescapedString)
{
	int i;
	int nStringLen;
	int nOutputLen = 0;
	TCHAR *sEscapedString;

	nStringLen = _tcslen(sUnescapedString);

	/* escaped version of string may be longer! Calculate length */

	/* scan through input string calculating length of escaped string */
	for (i=0; i<nStringLen; i++)
	{
		TCHAR ch;

		ch = sUnescapedString[i];

		switch (ch)
		{
			/* escaped chars */
			case _T('"'):
			{
				nOutputLen++;
			}
			break;
			
			/* unescaped chars */
			default:
				break;
		}

		nOutputLen++;
	}

	sEscapedString = malloc((nOutputLen+1)*sizeof(TCHAR));

	if (sEscapedString)
	{
		int nOutputPos;

		nOutputPos = 0;

		for (i=0; i<nStringLen; i++)
		{
			TCHAR ch;

			ch = sUnescapedString[i];

			switch (ch)
			{
				/* escaped chars */
				case _T('"'):
				{
					sEscapedString[nOutputPos] = ch;
					nOutputPos++;
					sEscapedString[nOutputPos] = ch;
					nOutputPos++;
				}
				break;
				
				/* unescaped chars */
				default:
				{
					sEscapedString[nOutputPos] = ch;
					nOutputPos++;
				}
				break;
			}
		}	

		sEscapedString[nOutputLen] = _T('\0');
	}

	return sEscapedString;

}

/*------------------------------------------------------------------------*/
/* TODO: Support escaped strings using \ ? */
LPCTSTR DDE_UnEscapeString(LPCTSTR sEscapedString)
{
	int nStringLen = _tcslen(sEscapedString);
	TCHAR *sUnescapedString;

	/* unescaped string may be less than length of this string, depending
	on if there are escape characters in the input string */
	/* allocate buffer large enough to hold escaped string and if unescaped
	string is smaller then some of the buffer is unused */
	sUnescapedString = malloc((nStringLen+1)*sizeof(TCHAR));

	if (sUnescapedString)
	{
		int nInputPos;
		int nOutputPos;

		nInputPos = 0;
		nOutputPos = 0;

		while (nInputPos!=nStringLen)
		{
			TCHAR ch;

			ch = sEscapedString[nInputPos];
			nInputPos++;

			/* look for escape char */
			if (ch==_T('"'))
			{
				if (nInputPos!=nStringLen)
				{
					ch = sEscapedString[nInputPos];
					nInputPos++;

					sUnescapedString[nOutputPos]=ch;
					nOutputPos++;
				}
			}
			else
			{
				/* not escape char, store to output */
				sUnescapedString[nOutputPos] = ch;
				nOutputPos++;
			}
		}

		/* null terminate string */
		sUnescapedString[nOutputPos] = _T('\0');
	}
	return sUnescapedString;
}

/*------------------------------------------------------------------------*/
/* get parameter */
TCHAR *DDEParse_GetParameter(DDEParse *pParse)
{
	TCHAR ch;
	BOOL fEscapeChar = FALSE;
	TCHAR chEscapeChar;
	BOOL fQuoted = TRUE;
	TCHAR *pStringStart;

	if (pParse->nOffset==pParse->nSize)
		return NULL;

	ch = pParse->pPtr[pParse->nOffset];

	/* parameter is initially quoted? */
	if (ch==_T('"'))
	{
		fQuoted = TRUE;
		pParse->nOffset++;
	}

	pStringStart = &pParse->pPtr[pParse->nOffset];

	while (pParse->nOffset!=pParse->nSize)
	{
		ch = pParse->pPtr[pParse->nOffset];
		
		if (!fEscapeChar)
		{
			if (ch==_T('"'))
			{
				/* seen quote, may be first quote of escaped quote */
				fEscapeChar = TRUE;
				chEscapeChar = _T('"');
			}
		}
		else
		{
			if (chEscapeChar==_T('"'))
			{
				/* seen an escaped quote */
		
				/* if we didn't see a second quote then quit */
				if (ch!='"')
				{
					pParse->pPtr[pParse->nOffset-1] = _T('\0');
					break;
				}
				else
				{
					/* saw second quote; this is an escaped quote! */
					fEscapeChar = FALSE;
				}
			}
		}

		pParse->nOffset++;
	}

	return pStringStart;
}
/*------------------------------------------------------------------------*/

void DDEParse_GetToken(DDEParse *pParse, TCHAR *TokenBuffer, unsigned long TokenBufferSize)
{
	TCHAR ch;
	unsigned long nTokenChars = 0;

	TokenBuffer[0] = _T('\0');

	if (pParse->nOffset==pParse->nSize)
		return;

	/* TODO: Support these chars only? [a-zA-Z0-9!#$%^&()-_{}~] */

	do
	{
		/* get character and advance position */
		ch = pParse->pPtr[pParse->nOffset];
		pParse->nOffset++;
	
		/* is a token character? */
		if (isalpha(ch))
		{
			/* can we fit any more chars into buffer? */
			if (nTokenChars<(TokenBufferSize-1))
			{
				/* add char */
				TokenBuffer[nTokenChars] = ch;
				nTokenChars++;
			}
		}
	}
	while (isalpha(ch) && (pParse->nOffset!=pParse->nSize));

	/* if we didn't hit the end of the parse buffer, go back one char */
	if (pParse->nOffset!=pParse->nSize)
	{
		pParse->nOffset--;
	}

	/* complete the token */
	TokenBuffer[nTokenChars]=_T('\0');
}
/*------------------------------------------------------------------------*/
HDDEDATA CALLBACK DdeClientCallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD dwData1, DWORD dwData2)
{
	return (HDDEDATA)0;
}
/*------------------------------------------------------------------------*/
HDDEDATA CALLBACK DdeCallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD dwData1, DWORD dwData2)
{

	switch (uType)
	{   
		case XTYP_DISCONNECT:
			return 	(HDDEDATA)0;

		case XTYP_EXECUTE:
		{
			DWORD nSize;

			/* get length of buffer required */
			nSize = DdeGetData(hData,NULL,0,0);

			if (nSize!=0)
			{
				TCHAR *pBuffer = (TCHAR *)malloc(nSize+sizeof(TCHAR));

				if (pBuffer!=NULL)
				{
					/* copy the data */
					if (DdeGetData(hData, pBuffer, nSize, 0)==nSize)
					{
						DDEParse parse;
						TCHAR ch;

						pBuffer[nSize-1] = _T('\0');

						parse.nOffset = 0;
						parse.pPtr = pBuffer;
						parse.nSize = nSize;

						while (parse.nOffset!=parse.nSize)
						{
							ch = parse.pPtr[parse.nOffset];
							parse.nOffset++;

							if (ch==_T('['))
							{
								TCHAR TokenBuffer[32];

								DDEParse_GetToken(&parse, TokenBuffer, sizeof(TokenBuffer));

								if (_tcsicmp(TokenBuffer,_T("SetForeground"))==0)
								{



								}
								else if (_tcsicmp(TokenBuffer,_T("Open"))==0)
								{
									TCHAR *pString;

									DDEParse_SkipWhitespace(&parse);

									pString = DDEParse_GetParameter(&parse);

									if (pString!=NULL)
									{
										LPCTSTR pUnescapedString = DDE_UnEscapeString(pString);

										if (pUnescapedString)
										{
											CPCEMU_DetectFileAndLoad(pUnescapedString);
									
											free((void *)pUnescapedString);
										}
									}
								}
								else if (_tcsicmp(TokenBuffer,_T("CmdLine"))==0)
								{
									TCHAR *pString;

									DDEParse_SkipWhitespace(&parse);

									pString = DDEParse_GetParameter(&parse);

									if (pString!=NULL)
									{
										LPCTSTR pUnescapedString = DDE_UnEscapeString(pString);

										if (pUnescapedString)
										{
											CPCEMU_ProcessCommandLine(pUnescapedString);
									
											free((void *)pUnescapedString);
										}
									}
								}
							}
							else if (ch==_T(' '))
							{
								DDEParse_SkipWhitespace(&parse);
							}
							else if (ch==_T(']'))
							{


							}
						}
					}

					/* free the buffer */
					free(pBuffer);
				}
			}
		}
		return (HDDEDATA)DDE_FACK;


		case XTYP_CONNECT:               
		{
			/* hsz1 = Topic Name;
			 hsz2 = Service name */
		   if (
			   (DdeCmpStringHandles(hsz1,AppData.hszSystemTopicName)==0) &&
			   (DdeCmpStringHandles(hsz2,AppData.hszAppName)==0)
				)
			{
			   /* topic/service supported */
				return (HDDEDATA)TRUE;   
			}
		}
		return (HDDEDATA)FALSE;

		case XTYP_CONNECT_CONFIRM:
		{
		}
		break;

		default:
			break;

    }

	return (HDDEDATA)0;
}

/*------------------------------------------------------------------------*/
/* our own implementation of UNIX's getopt; not fully compatible! But UNICODE enabled */

struct option
{
	TCHAR *longname;		/* name of the long option */
	int has_arg;			/* 0 = no option, 1 = requires argument, 2 = optional argument */
	int *flag;				/* if NULL, getopt returns val */
							/* else getopt returns 0 and flag points to a variable
							which is set to val if the option is found, but left unchanged
							if the option is not found */
	int val;				/* val to return, or to load into variable pointed to by flag */
};

/*------------------------------------------------------------------------*/
/* assumes arg is NULL terminated string! */
/* returns pointer to arg string IF it is an arg, NULL otherwise */
static const TCHAR *getopt_long_only_isopt(const TCHAR *arg)
{
	/* get first char */
	if (arg[0]==_T('-'))
	{
		/* first char is '-' */

		switch (arg[1])
		{
			/* has '--' prefix */
			case _T('-'):
			{
				/* now check next char */
				switch (arg[2])
				{
					/* exactly '--' */
					case _T('\0'):
						return NULL;

					default:
						break;
				}
			}
			return &arg[2];
			
			/* exactly '-' */
			case _T('\0'):
				return NULL;
				
			/* has '-' prefix */
			default:
				return &arg[1];
		}
	}

	/* not an arg */
	return NULL;
}

/*------------------------------------------------------------------------*/
const TCHAR *optarg;
int optind;	/* not used */ 
int opterr=1; /* not used */
int optopt;	

static int argpos;

/* UNIX version uses a static variable to keep track of processing options; this is useless
for us, because we may reuse the command-line function more than once */
void getopt_init()
{
	argpos= 1;
}
/* get next option */
int getopt_long_only(int argc, const TCHAR **argv, const TCHAR *optstring, const struct option *options, int *longindex)
{
	const TCHAR *opt;
	
	/* check for end of arg list */
	if (argpos==argc)
		return -1;

	optarg = NULL;

	/* is this arg an option? */
	opt = getopt_long_only_isopt(argv[argpos]);
	argpos++;

	if (opt!=NULL)
	{
		const struct option *curopt= options;
		
		while (curopt->val!=0)
		{
			/* found? */
			if (_tcsicmp(curopt->longname, opt)==0)
			{
				switch (curopt->has_arg)
				{
					/* no argument */
					case 0:
						return curopt->val;

					/* argument required */
					case 1:
					{
						/* any more parameters */
						if (argpos!=argc)
						{
							opt = getopt_long_only_isopt(argv[argpos]);

							if (opt==NULL)
							{							
								/* setup argument */
								optarg = argv[argpos];
								argpos++;
							}
							return curopt->val;
						}
					
						/* error */
						/* - no parameters left in argument list */
						/* - next item in argument list is an option */
						optopt = curopt->val;
					}
					return '?';

					/* optional argument */
					case 2:
					{
						/* if no furthur arguments in list, assume argument not given and
						return value */
						if (argpos!=argc)
						{
							/* next is an option? */
							opt = getopt_long_only_isopt(argv[argpos]);

							if (opt==NULL)
							{
								/* next not an option, so must be an argument */

								/* return it */
								optarg = argv[argpos];
								argpos++;
							}
						}
					}
					return curopt->val;
				}

			}

			curopt++;
		}

		/* end of list */
		return -1;
	}

	optarg = argv[argpos-1];

	return 1;
}

/*------------------------------------------------------------------------*/
/* execute command-line; passed to executable or passed via DDE from another instance
of Arnold app (or even another prog) */

/* the options we support */
const struct option long_options[] = 
{
	/* the last column doesn't need to be converted into unicode!
	This column can almost be any integer value; but some values are reserved by 
	the getopt function */
	{_T("tape"), 1, NULL, 't'},
	{_T("drivea"), 1, NULL, 'a'},
	{_T("driveb"), 1, NULL, 'b'},
	{_T("cart"), 1, NULL, 'c'},
	{_T("snapshot"), 1, NULL, 's'},
	{NULL, 0, NULL, '\0'}
};

void CPCEMU_ProcessCommandLine(LPCTSTR sCommandLine)
{
	CommandLineData commandLineData;

	CommandLineData_Init(&commandLineData);
	if (sCommandLine)
	{
		int c;

		CommandLineData_Create(&commandLineData, sCommandLine);

		getopt_init();

		do 
		{
			int option_index = 0;
			c = getopt_long_only (commandLineData.argc, commandLineData.argv, _T(""), long_options, &option_index);
			switch(c) 
			{
				case 't':
				{
					unsigned char *pLocation;
					unsigned long Length;

					LoadFile(optarg, &pLocation, &Length);

					if (pLocation)
					{
						TapeImage_Insert(pLocation, Length);
						
						free(pLocation);
					}
				}
				break;
				case 'a':
				{
					Interface_InsertDiskFromFile(0, optarg);
				}
				break;
				case 'b':
				{
					Interface_InsertDiskFromFile(1, optarg);
				}
				break;
				case 'c':
				{
					unsigned char *pLocation;
					unsigned long Length;

					LoadFile(optarg, &pLocation, &Length);

					if (pLocation)
					{
						Cartridge_AttemptInsert(pLocation, Length);

						free(pLocation);
					}
				}
				break;
				case 's':
				{
					unsigned char *pLocation;
					unsigned long Length;

					LoadFile(optarg, &pLocation, &Length);

					if (pLocation)
					{
						Snapshot_Insert(pLocation, Length);
					
						free(pLocation);
					}
				}
				break;
				
				/* error */
				case '?':
				{
					c=-1;
				}
				break;

				case '\x1':
				{
					/* no option specified */
					CPCEMU_DetectFileAndLoad(optarg);
					break;
				}
			}
		} while (c != -1);
					
		CommandLineData_Free(&commandLineData);
	}
}
/*----------------------------------------------------------------------------------------------*/

/* This will be checked now because Arnold is currently windowed only */
/* get the bits per pixel in current display mode */
BOOL	CheckBitDepth()
{
	HWND	DesktopWindow;
	int		BitsPerPixel = 0;

	/* get desktop window */
	DesktopWindow = GetDesktopWindow();

	if (DesktopWindow!=NULL)
	{
		HDC hDC;

		/* get DC of desktop window */
		hDC = GetDC(DesktopWindow);

		if (hDC!=NULL)
		{
			/* get device caps */
			BitsPerPixel = GetDeviceCaps(hDC, BITSPIXEL);

			/* release DC */
			ReleaseDC(DesktopWindow,hDC);
		}
	}

	/* check display depth */
	if (BitsPerPixel<8)
	{
		/* quit */
		return FALSE;
	}

	return TRUE;
}

/*----------------------------------------------------------------------------------------------*/
typedef struct
{
	LPTSTR	sFilename;				/* the fully qualified path and filename */
	LPTSTR	sDisplayName;			/* the name displayed in the recent menu */
} RECENT_FILE_ITEM;

#define MAX_RECENT_ITEMS	8

typedef struct
{
	int					nItems;		/* number of items defined */
	RECENT_FILE_ITEM	Items[MAX_RECENT_ITEMS];	 /* the items */
} RECENT_ITEMS;

enum
{
	RECENT_LIST_FILES = 0,

	/* this must be last */
	RECENT_LIST_COUNT
};

static RECENT_ITEMS RecentFiles;

/*----------------------------------------------------------------------------------------------*/
/* recent list id must be 0 based and all lists must be contiguous */
/* the idea here is that there can be multiple recent lists for tapes,discs,cartridges etc */
RECENT_ITEMS *RecentItems_GetItemsList(int nRecentListID)
{
	return &RecentFiles;
}
/*----------------------------------------------------------------------------------------------*/
void RecentItems_Init(int nRecentListID)
{
	int i;
	RECENT_ITEMS *pRecentItems;

	pRecentItems = RecentItems_GetItemsList(nRecentListID);
	
	pRecentItems->nItems = 0;
	for (i=0; i<MAX_RECENT_ITEMS; i++)
	{
		pRecentItems->Items[i].sDisplayName = NULL;
		pRecentItems->Items[i].sFilename = NULL;
	}
}

/*----------------------------------------------------------------------------------------------*/
void RecentItem_Free(RECENT_FILE_ITEM *pRecentFileItem)
{
	if (pRecentFileItem->sDisplayName)
		free(pRecentFileItem->sDisplayName);
	pRecentFileItem->sDisplayName = NULL;

	if (pRecentFileItem->sFilename)
		free(pRecentFileItem->sFilename);
	pRecentFileItem->sFilename = NULL;
}


/*----------------------------------------------------------------------------------------------*/
void RecentItems_Free(int nRecentListID)
{
	RECENT_ITEMS *pRecentItems;
	int i;

	pRecentItems = RecentItems_GetItemsList(nRecentListID);

	pRecentItems->nItems = 0;
	for (i=0; i<MAX_RECENT_ITEMS; i++)
	{
		RecentItem_Free(&pRecentItems->Items[i]);
	}
}


/*----------------------------------------------------------------------------------------------*/
LPTSTR RecentItems_GenerateDisplayName(LPCTSTR sFilename)
{
	LPTSTR sDisplayName;

	/* for now, copy the filename as the display name */
	sDisplayName = (LPTSTR)malloc((_tcslen(sFilename)+1)*sizeof(TCHAR));
	if (sDisplayName)
		_tcscpy(sDisplayName, sFilename);

	return sDisplayName;
}

/*----------------------------------------------------------------------------------------------*/
/* find item by filename */
int RecentItems_Find(RECENT_ITEMS *pRecentItems, LPCTSTR sFilename)
{
	int i;

	for (i=0; i<pRecentItems->nItems; i++)
	{
		/* compare filenames */
		if (_tcsicmp(pRecentItems->Items[i].sFilename, sFilename)==0)
		{
			return i;
		}
	}

	return -1;
}

/*----------------------------------------------------------------------------------------------*/
/* remove item. Do this if we try to load the file and it can't be loaded */
void RecentItems_Remove(LPCTSTR sFilename, int nRecentListID)
{
	int nIndex;
	RECENT_ITEMS *pRecentItems;

	pRecentItems = RecentItems_GetItemsList(nRecentListID);

	/* find the item */
	nIndex = RecentItems_Find(pRecentItems, sFilename);

	if (nIndex==-1)
		return;

	/* free the item */
	RecentItem_Free(&pRecentItems->Items[nIndex]);

	pRecentItems->nItems--;

	/* copy up remaining items */
	for (;nIndex<pRecentItems->nItems; nIndex++)
	{
		pRecentItems->Items[nIndex].sDisplayName = pRecentItems->Items[nIndex+1].sDisplayName;
		pRecentItems->Items[nIndex].sFilename= pRecentItems->Items[nIndex+1].sFilename;
	}
}

/*----------------------------------------------------------------------------------------------*/
/* if the item already exists in the list, promote it to the top */
/* if the item doesn't exist, insert it at the top */
void RecentItems_Add(LPCTSTR sFilename, int nRecentListID)
{
	RECENT_ITEMS *pRecentItems;
	LPTSTR sDisplayName = NULL;		/* the display name in the UI */
	LPTSTR sCopiedFilename = NULL;	/* the duplicated filename */
	int nItemsEnd = 0; /* number of items to move down list */
	int nItemsStart = 0; /* position to start copying to*/
	int nItem;

	if (_tcslen(sFilename)==0)
		return;

	pRecentItems = RecentItems_GetItemsList(nRecentListID);

	nItem = RecentItems_Find(pRecentItems, sFilename);

	if (nItem!=-1)
	{
		/* we don't reallocate these as they have already been done
		when this filename was first added to the list */

		/* get the display name */
		sDisplayName = pRecentItems->Items[nItem].sDisplayName;
		/* get the duplicated filename */
		sCopiedFilename = pRecentItems->Items[nItem].sFilename;
	
		nItemsEnd = 0;
		nItemsStart = nItem;
	}
	else
	{
		if (pRecentItems->nItems!=MAX_RECENT_ITEMS)
		{
			pRecentItems->nItems++;
		}
		else
		{
			RecentItem_Free(&pRecentItems->Items[MAX_RECENT_ITEMS-1]);
		}

		/* not in list, so new item will be first in list */
		nItemsEnd = 0;
		nItemsStart = pRecentItems->nItems-1;
		
		/* for now copy the filename as the display name */
		sDisplayName = RecentItems_GenerateDisplayName(sFilename);

		/* copy the filename */
		sCopiedFilename = (LPTSTR)malloc((_tcslen(sFilename)+1)*sizeof(TCHAR));
		if (sCopiedFilename)
			_tcscpy(sCopiedFilename, sFilename);
	}

	/* copy items down */
	for (; nItemsStart!=nItemsEnd; nItemsStart--)
	{
		pRecentItems->Items[nItemsStart] = pRecentItems->Items[nItemsStart-1];
	}

	/* setup first item */
	pRecentItems->Items[0].sDisplayName = sDisplayName;
	pRecentItems->Items[0].sFilename = sCopiedFilename;
}
/*----------------------------------------------------------------------------------------------*/
/* returns TRUE if this is an item from one of the recent lists */
BOOL RecentItems_Handle(WORD wID)
{
	int nRecentList;
	int nRecentItem;
	RECENT_ITEMS *pItems;

	if (wID>((RECENT_LIST_COUNT*MAX_RECENT_ITEMS)+1))
		return FALSE;

	nRecentList = (wID-1)/MAX_RECENT_ITEMS;
	nRecentItem = (wID-1)%MAX_RECENT_ITEMS;

	pItems = RecentItems_GetItemsList(nRecentList);
	
	if (!pItems)
		return TRUE;

	if (nRecentItem>pItems->nItems)
		return TRUE;

	CPCEMU_DetectFileAndLoad(pItems->Items[nRecentItem].sFilename);

	return TRUE;
}

/*----------------------------------------------------------------------------------------------*/
HMENU RecentItems_GetFileMenu(HWND hwnd)
{
	HMENU hMainMenu;
	
	/* get the menu of this window */
	hMainMenu= GetMenu(hwnd);

	if (hMainMenu)
	{
		return GetSubMenu(hMainMenu, 0);
	}

	return (HMENU)0;
}
#define RECENT_ITEMS_FIRST_MENU_POS 6
/*----------------------------------------------------------------------------------------------*/
void RecentItems_EnableMenu(HWND hwnd, int nPosition, BOOL fEnable)
{
	HMENU hFileMenu = RecentItems_GetFileMenu(hwnd);
	UINT uEnable = MF_BYPOSITION;
	if (fEnable)
		uEnable |= MF_ENABLED;
	else
		uEnable |= MF_GRAYED;

	EnableMenuItem(hFileMenu, nPosition+RECENT_ITEMS_FIRST_MENU_POS, uEnable);
}

/*----------------------------------------------------------------------------------------------*/
/* supports multiple recent menus */
HMENU RecentItems_GetMenu(HWND hwnd, int nPosition)
{
	HMENU hFileMenu = RecentItems_GetFileMenu(hwnd);

	if (hFileMenu)
	{
		/* 6 is the position of the first recent sub menu */
		return GetSubMenu(hFileMenu, nPosition+RECENT_ITEMS_FIRST_MENU_POS);
	}

	return (HMENU)0;
}

/*----------------------------------------------------------------------------------------------*/
void RecentItems_RefreshMenu(int nRecentListID, HWND hwnd)
{
	int i;
	RECENT_ITEMS *pRecentItems;
	HMENU hRecentMenu;

	pRecentItems = RecentItems_GetItemsList(nRecentListID);

	RecentItems_EnableMenu(hwnd, nRecentListID,(pRecentItems->nItems));
	if (pRecentItems->nItems!=0)
	{
		/* get handle to recent sub menu */
		hRecentMenu = RecentItems_GetMenu(hwnd,nRecentListID);


		/* initially delete all items */
		for (i=GetMenuItemCount(hRecentMenu)-1; i>=0; i--)
		{
			DeleteMenu(hRecentMenu, i, MF_BYPOSITION);
		}

		/* now fill with new items */
		for (i=0; i<pRecentItems->nItems; i++)
		{
			TCHAR sMenuItem[256];

			/* format string for menu item */
			wsprintf(sMenuItem,_T("&%d %s"),(i+1),pRecentItems->Items[i].sDisplayName);

			InsertMenu(hRecentMenu,i, MF_BYPOSITION|MF_STRING, ((nRecentListID*MAX_RECENT_ITEMS)+i)+1, sMenuItem);
		}
	}
}
/*----------------------------------------------------------------------------------------------*/
void RecentItems_AddItem(RECENT_ITEMS *pItems, LPCTSTR sFilename)
{
	/* used during restore */
	LPTSTR sDisplayName = RecentItems_GenerateDisplayName(sFilename);
	LPTSTR sCopiedFilename;

	sCopiedFilename = (LPTSTR)malloc((_tcslen(sFilename)+1)*sizeof(TCHAR));
	if (sCopiedFilename)
		_tcscpy(sCopiedFilename, sFilename);

	pItems->Items[pItems->nItems].sDisplayName = sDisplayName;
	pItems->Items[pItems->nItems].sFilename = sCopiedFilename;
	pItems->nItems++;
}

/*----------------------------------------------------------------------------------------------*/
void RecentItems_AddAndRefresh(HWND hwnd,LPCTSTR sFilename, int nRecentListID)
{
	RecentItems_Add(sFilename, nRecentListID);
	RecentItems_RefreshMenu(nRecentListID, hwnd);
}
/*----------------------------------------------------------------------------------------------*/
void RecentItems_Restore(int nRecentListID, LPCTSTR sPrefix)
{
	RECENT_ITEMS *pRecentItems;

	pRecentItems = RecentItems_GetItemsList(nRecentListID);

	if (Settings_OpenSettings(0))
	{
		int i;
		for (i=0; i<MAX_RECENT_ITEMS; i++)
		{
			TCHAR sKey[256];

			TCHAR *sFilename;
			
			_stprintf(sKey,_T("%s%d"),sPrefix, i);
			sFilename = MyApp_Registry_GetStringFromCurrentKey(sKey);
	
			if (sFilename!=NULL)
			{
				if (_tcslen(sFilename)!=0)
				{
					RecentItems_AddItem(pRecentItems, sFilename);
				}
			}
		}

		MyApp_Registry_CloseKey();
	}
}
/*----------------------------------------------------------------------------------------------*/
void RecentItems_Store(int nRecentListID, LPCTSTR sPrefix)
{
	RECENT_ITEMS *pRecentItems;

	pRecentItems = RecentItems_GetItemsList(nRecentListID);

	if (Settings_OpenSettings(0))
	{
		int i;

		for (i=0; i<pRecentItems->nItems; i++)
		{
			TCHAR sKey[256];

			_stprintf(sKey,_T("%s%d"),sPrefix,i);
			MyApp_Registry_StoreStringToCurrentKey(sKey, pRecentItems->Items[i].sFilename);
		}

		MyApp_Registry_CloseKey();
	}
}
/*----------------------------------------------------------------------------------------------*/
LPCTSTR GetString(const TCHAR *pString)
{
	if (pString==NULL)
		return _T("");

	return pString;
}

/*----------------------------------------------------------------------------------------------*/
/* set a string */
void	SetString(TCHAR **ppReplaceString, const TCHAR *pSourceString)
{
	TCHAR *pDestString = *ppReplaceString;

	/* string defined? */
	if (pDestString!=NULL)
	{
		/* free it */
		free(pDestString);
	}

	pDestString = NULL;

	/* quit if new string is not defined */
	if (pSourceString!=NULL)
	{
		int nStringLenBytes = (_tcslen(pSourceString)+1)*sizeof(TCHAR);

		/* allocate for new string */
		pDestString = (TCHAR *)malloc(nStringLenBytes);

		if (pDestString)
		{
			memcpy(pDestString, pSourceString, nStringLenBytes);
		}
	}

	*ppReplaceString = pDestString;
}
/*----------------------------------------------------------------------------------------------*/
void SetStringAsPath(TCHAR **ppReplaceString, const TCHAR *sString)
{
	TCHAR sDir[MAX_PATH];
	TCHAR sDrive[MAX_PATH];

	_tsplitpath(sString, sDrive, sDir, NULL, NULL);

	_tcscat(sDrive,sDir);

	SetString(ppReplaceString, sDrive);
}
/*----------------------------------------------------------------------------------------------*/
void InitFilenameForFileOpenDlg(const TCHAR *sFilename, TCHAR *sFileOpenDialogFilename)
{
	TCHAR sName[MAX_PATH];
	TCHAR sExt[MAX_PATH];

	if (sFilename==NULL)
	{
		sFileOpenDialogFilename[0]=_T('\0');
		return;
	}

	_tsplitpath(sFilename, NULL, NULL, sName, sExt);

	_stprintf(sFileOpenDialogFilename,_T("%s.%s"),sFilename,sExt);
}

/*----------------------------------------------------------------------------------------------*/

void	CPCEMU_DetectFileAndLoad(const TCHAR *pFilename)
{
	unsigned char *pLocation;
	unsigned long Length;

	LoadFile(pFilename, &pLocation, &Length);

	if (pLocation==NULL)
		return;

	if (Cartridge_AttemptInsert(pLocation, Length)==ARNOLD_STATUS_OK)
	{
		free(pLocation);
		return;
	}

	if (Snapshot_Insert(pLocation, Length)==ARNOLD_STATUS_OK)
	{
		free(pLocation);
		return;
	}

	if (TapeImage_Insert(pLocation, Length)==ARNOLD_STATUS_OK)
	{
		free(pLocation);
		return;
	}

	if (DiskImage_InsertDisk(0, pLocation, Length)==ARNOLD_STATUS_OK)
	{
		free(pLocation);
		return;
	}

	free(pLocation);
}
/*----------------------------------------------------------------------------------------------*/

const char *sAutoStringTape = "|TAPE\nRUN\"\n\001\n";

static char AutoType_String[256];


BOOL	Interface_OpenCheatDatabase(void);

#if 0
extern BOOL MySavePNG(const TCHAR* pFilename, int cx, int cy, int (*getpixel)(int x, int y, unsigned char* r, unsigned char* g, unsigned char* b, void* pUser), void* pUser);

static int mygetpixel(int x, int y, unsigned char* r, unsigned char* g, unsigned char* b, void* pUser)
{
   pUser;
	Render_GetPixelRGBAtXY(x,y, r,g,b);
   return TRUE;
}

extern int PIXEL_STEP_SHIFT;

BOOL Screen_SaveSnapshot_PNG(const TCHAR*pFilename)
{
	const int cx = (X_CRTC_CHAR_WIDTH<<(1+3))>>PIXEL_STEP_SHIFT;
	const int cy = Y_CRTC_LINE_HEIGHT;	
   return MySavePNG(pFilename, cx, cy, mygetpixel, NULL);
}

void Screen_SaveSnapshot(const TCHAR* p)
{
   TCHAR szExt[_MAX_PATH];
   if (p != NULL) _tsplitpath(p, NULL, NULL, NULL, szExt);
   if ((p == NULL) || (0 == _tcsicmp(szExt, _T(".png"))))
   {
      Screen_SaveSnapshot_PNG(p);
   }
   else
   {
      Screen_SaveSnapshot_BMP(p);
   }
}

/////////////////////////////////////////
// The following glue code doesn't fit well in neither src/cpc or src/win

#include "..\contrib\include\libpngw.h"
#include "..\contrib\include\unicode.h"

extern int Render_RenderingAccuracyForWindowedMode;
TCHAR PUB_szPngAuthor[80] = _T("");

EXTERN_C BOOL MySavePNG(const TCHAR* pFilename, int cx, int cy, int (*getpixel)(int x, int y, unsigned char* r, unsigned char* g, unsigned char* b, void* pUser), void* pUser)
{
   TCHAR szTitle[_MAX_PATH];
   const char* pText[ENUM_pngtext_enumcount];
   BOOL bOK;

   _tsplitpath(pFilename, NULL, NULL, szTitle, NULL);   
   pText[ENUM_pngtext_desc    ] = "Arnold Screen Snapshot";
   pText[ENUM_pngtext_software] = "Arnold CPC Emulator";
#ifdef _UNICODE
    pText[ENUM_pngtext_title   ] = wcstombs_dup(szTitle);
   pText[ENUM_pngtext_author  ] = *PUB_szPngAuthor ? wcstombs_dup(PUB_szPngAuthor) : NULL;
#else
   pText[ENUM_pngtext_title   ] = szTitle;
   pText[ENUM_pngtext_author  ] = PUB_szPngAuthor;
#endif
   
   bOK = pngw_tsave(pFilename, NULL, cx, cy, getpixel, pUser, pText, Render_RenderingAccuracyForWindowedMode != RENDERING_ACCURACY_LOW);
#ifdef _UNICODE
   free((void*)pText[ENUM_pngtext_title ]);
   free((void*)pText[ENUM_pngtext_author]);
#endif
   return bOK;
}
#endif



static HANDLE fhPrinterOutput = INVALID_HANDLE_VALUE;
static int PrinterOutputMethod;

enum
{
	PRINTER_OUTPUT_TO_FILE,
	PRINTER_OUTPUT_TO_PRINTER,
	PRINTER_OUTPUT_DO_NOTHING,
	PRINTER_OUTPUT_TO_DIGIBLASTER
};

void	Printer_CloseOutputFile(void)
{
	if (fhPrinterOutput!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(fhPrinterOutput);
		fhPrinterOutput = INVALID_HANDLE_VALUE;
	}
}

static void	Printer_OpenOutputFile(TCHAR *pFilename)
{
	fhPrinterOutput = CreateFile(pFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}


static void	Printer_WriteToOutputFile(unsigned char DataByte)
{
	if (fhPrinterOutput!=0)
	{
		BYTE Buffer[1];
		DWORD dwBytesWritten;

		Buffer[0] = DataByte;
		WriteFile(fhPrinterOutput, Buffer, sizeof(Buffer), &dwBytesWritten, NULL);
	}
}

void	Host_HandlePrinterOutput(void)
{
	switch (PrinterOutputMethod)
	{
		case PRINTER_OUTPUT_TO_FILE:
		{
			if ((Printer_GetStrobeState()==0) && (Printer_GetStrobeState()==0))
			{
				/* write databyte to printer text output file */
				Printer_WriteToOutputFile(Printer_Get8BitData());
			}
		}
		break;

		case PRINTER_OUTPUT_TO_PRINTER:
		{
		}
		break;

		case PRINTER_OUTPUT_TO_DIGIBLASTER:
		{
			Audio_Digiblaster_Write(Printer_GetDataByte());
		}
		break;

		case PRINTER_OUTPUT_DO_NOTHING:
			break;
	}
}




void    CPC_ReloadSystemCartridge(void)
{
	Cartridge_Insert(CPCPLUS_SystemCartridge_ENG.pData, CPCPLUS_SystemCartridge_ENG.nLength);

	Cartridge_Autostart();
}



const CHEAT_DATABASE *pCheatDatabase = NULL;

void	Interface_FreeCheatDatabase()
{
	if (pCheatDatabase!=NULL)
	{
		CheatDatabase_Delete(pCheatDatabase);
		pCheatDatabase = NULL;
	}
}



void	Interface_SetupCPCTypeMenu(int);

void	DoKeyboard(void);

void	ErrorMessage(TCHAR *ErrorText)
{
	MessageBox(GetActiveWindow(),ErrorText,Messages[70]/*_T(APP_TITLE_TEXT)*/,MB_OK);
}


enum
{
	EXTENSION_TEXT_ALL,
	EXTENSION_TEXT_DISK_IMAGES,
	EXTENSION_TEXT_DSK_IMAGE,
	EXTENSION_TEXT_DSK,
	EXTENSION_TEXT_ZIP,
	EXTENSION_TEXT_ARCHIVES,
	EXTENSION_TEXT_ZIP_ARCHIVE
};

TCHAR *ExtensionText[]=
{
	_T("All"),
	_T("Disk Images"),
	_T("DSK disk Image"),
	_T("dsk"),
	_T("zip"),
	_T("Archives"),
	_T("ZIP Archive")
};

/* an extension and it's description */
typedef struct 
{
	/* ids are used for multi-language reasons */
	const int extensionTextID;
	const int descriptionTextID;
} EXTENSION_DATA;


// define the start of an extension group
#define EXTENSION_GROUP_BEGIN(_description_) \
{ \
	-1, \
	_description_ \
}

// define the end of an extension group
#define EXTENSION_GROUP_END() \
{ \
	-1, \
	-1, \
}

#define EXTENSION(_extension_, _description_) \
{ \
	_extension_, \
	_description_, \
}

#define EXTENSION_IS_GROUP_BEGIN(_item_) \
	((_item_->extensionTextID==-1) && (_item_->descriptionTextID!=-1))

#define EXTENSION_IS_GROUP_END(_item_) \
	((_item_->extensionTextID==-1) && (_item_->descriptionTextID==-1))

#define EXTENSION_IS_EXTENSION(_item_) \
	((_item_->extensionTextID!=-1) && (_item_->descriptionTextID!=-1))


const EXTENSION_DATA disc[]=
{
	EXTENSION_GROUP_BEGIN(EXTENSION_TEXT_ALL),
		EXTENSION_GROUP_BEGIN(EXTENSION_TEXT_DISK_IMAGES),
			EXTENSION(EXTENSION_TEXT_DSK,EXTENSION_TEXT_DSK_IMAGE),
		EXTENSION_GROUP_END(),
		EXTENSION_GROUP_BEGIN(EXTENSION_TEXT_ARCHIVES),
			EXTENSION(EXTENSION_TEXT_ZIP,EXTENSION_TEXT_ZIP_ARCHIVE),
		EXTENSION_GROUP_END(),
	EXTENSION_GROUP_END(),
};



#if 0
#define MENU_ITEM_DESC_FLAG_SEPERATOR (1<<0)
#define MENU_ITEM_DESC_FLAG_GREYED (1<<1)
#define MENU_ITEM_DESC_FLAG_SUBMENU_BEGIN (1<<2)
#define MENU_ITEM_DESC_FLAG_SUBMENU_END (1<<3)

typedef struct
{
	/* flags */
	int Flags;
	/* id of text */
	int TextID;
	/* id of selection */
	int ItemID;
} menu_item_desc;

#define MENU_BEGIN(_title_) \
{ \
	MENU_ITEM_DESC_FLAG_SUBMENU_BEGIN, \
	_title_, \
	0, \
} \

#define MENU_ITEM(_flags_,_textid_,_itemid_) \
{ \
	_flags_, \
	_textid_, \
	_itemid_ \
}


#define MENU_END() \
{ \
	MENU_ITEM_DESC_FLAG_SUBMENU_END, \
	0, \
	0, \
}

menu_item_desc menus[]=
{
	MENU_BEGIN("File"),
		MENU_BEGIN("Drive A"),
			MENU_BEGIN("Blank Disk"),
				MENU_ITEM("Unformatted"),0,ID_FILE_DRIVEA_INSERTNEWDISK_UNFORMATTED),
			MENU_END(),
		MENU_END(),
	MENU_END(),
};
#endif


static TCHAR FilenameBuffer[MAX_PATH];



static TCHAR *DiskImagePath;			/* path for opening disk images */
static TCHAR *CartridgePath;			/* path for opening cartridges */
static TCHAR *SnapshotPath;			/* path for opening snapshots */
static TCHAR *RomPath;					/* path for opening roms */
static TCHAR *ScreenSnapshotPath;		/* path for writing screen snapshots */
static TCHAR *WavOutputPath;			/* path for writing wav recording of sound */
static TCHAR *AYOutputPath;			/* path for writing recording of ay */
static TCHAR *TapePath;				/* path for opening tapes */

static TCHAR *RomFilename;
static TCHAR *RomFilenames[17];		/* filenames of opened roms */
static TCHAR *CartridgeFilename;		/* filename of last opened cartridge */
static TCHAR *SnapshotFilename;		/* filename of last opened/saved snapshot */
static TCHAR *WavOutputFilename;		/* filename of last wav recording */
static TCHAR *AYOutputFilename;		/* filename of last AY recording */
static TCHAR *DriveFilenames[2];		/* filename of last disk image opened for drive A */
static TCHAR *TapeFilename;			/* filename of last tape opened */
static TCHAR *Multiface_CPC_ROM_Filename;	/* filename of CPC version of multiface rom */
static TCHAR *Multiface_CPCPLUS_ROM_Filename;		/* filename of CPC+ version of multiface rom */

/*------------------------------------------------------------------------------------------*/


void	GenericInterface_Initialise(void)
{
	int i;

	DiskImagePath = NULL;			
	CartridgePath = NULL;			
	SnapshotPath = NULL;			
	RomPath = NULL;					
	ScreenSnapshotPath = NULL;		
	WavOutputPath = NULL;			
	AYOutputPath = NULL;			
	TapePath = NULL;				

	CartridgeFilename = NULL;		
	SnapshotFilename = NULL;		
	WavOutputFilename = NULL;		
	AYOutputFilename = NULL;		
	TapeFilename = NULL;			
	Multiface_CPC_ROM_Filename = NULL;	
	Multiface_CPCPLUS_ROM_Filename = NULL;		
	RomFilename = NULL;

	for (i=0; i<2; i++)
	{
		DriveFilenames[i] = NULL;
	}


	for (i=0; i<17; i++)
	{
		RomFilenames[i] = NULL;
	}
}


void	GenericInterface_Finish(void)
{
	int i;

	SetString(&DiskImagePath,NULL);			
	SetString(&CartridgePath,NULL);			
	SetString(&SnapshotPath,NULL);			
	SetString(&RomPath,NULL);					
	SetString(&ScreenSnapshotPath,NULL);		
	SetString(&WavOutputPath,NULL);			
	SetString(&AYOutputPath,NULL);			
	SetString(&TapePath,NULL);				

	SetString(&CartridgeFilename,NULL);		
	SetString(&SnapshotFilename,NULL);		
	SetString(&WavOutputFilename,NULL);		
	SetString(&AYOutputFilename,NULL);		
	SetString(&TapeFilename,NULL);			
	SetString(&Multiface_CPC_ROM_Filename,NULL);	
	SetString(&Multiface_CPCPLUS_ROM_Filename,NULL);		

	for (i=0; i<2; i++)
	{
		SetString(&DriveFilenames[i],NULL);			
	}

	SetString(&RomFilename,NULL);		
	for (i=0; i<17; i++)
	{
		SetString(&RomFilenames[i],NULL);
	}
}


void	GenericInterface_RestoreRomSettings(void)
{
	int i;

	for (i=1; i<17; i++)
	{
		TCHAR SettingsKey[32];
		TCHAR *pCurrentRomFilename;
		
		/* setup settings key */
		_stprintf(SettingsKey, "sRom%2d", i);
		
		pCurrentRomFilename = MyApp_Registry_GetStringFromCurrentKey(SettingsKey);
	
		/* string present for this key, or key present ? */
		if (pCurrentRomFilename != NULL)
		{
			/* string exists */

			if (strlen(pCurrentRomFilename)!=0)
			{
				if (Interface_InsertRomFromFile(i, pCurrentRomFilename))
				{
					int ActiveFlag;

					/* if it's active in settings, make it active */
					_stprintf(SettingsKey,_T("iRom%2dActive"),i);

					/* get active state */
					ActiveFlag = MyApp_Registry_GetIntFromCurrentKey(SettingsKey);

					if (ActiveFlag != 0)
					{
						ExpansionRom_SetActiveState(i, TRUE);
					}
					else
					{
						ExpansionRom_SetActiveState(i, FALSE);
					}
				}
			}
		}
	}
}

static  TCHAR *EmptyString = _T("");

void	GenericInterface_StoreRomSettings(void)
{
	int i;

	for (i=1; i<17; i++)
	{
		TCHAR SettingsKey[32];
		const TCHAR *pCurrentRomFilename;
				
		/* get filename of rom in slot i */
		pCurrentRomFilename = RomFilenames[i];
			
		/* initialise settings key */
		_stprintf(SettingsKey, "sRom%2d", i);
		
		if (pCurrentRomFilename!=NULL)
		{
			/* store filename if specified - i.e. rom was inserted */
			MyApp_Registry_StoreStringToCurrentKey(SettingsKey, pCurrentRomFilename);

			/* store if rom is active or not */
			{
				_stprintf(SettingsKey,_T("iRom%2dActive"), i);

					MyApp_Registry_StoreIntToCurrentKey(SettingsKey, ExpansionRom_IsActive(i));
			}
		}
		else
		{
			/* store empty string, no rom specified */
			MyApp_Registry_StoreStringToCurrentKey(SettingsKey, EmptyString);
		}
	}
}

BOOL	Snapshot_Load(const TCHAR *SnapshotFilename)
{
	unsigned char *pSnapshotData;
	unsigned long SnapshotDataLength;

	LoadFile(SnapshotFilename, &pSnapshotData, &SnapshotDataLength);

	if (pSnapshotData!=NULL)
	{
		BOOL bStatus;

		bStatus = Snapshot_Insert(pSnapshotData, SnapshotDataLength);

		free(pSnapshotData);
	
		return bStatus;
	}

	return FALSE;
}


BOOL	GenericInterface_SnapshotSave(const TCHAR *FilenameBuffer, int SnapshotVersion,int SnapshotSize)
{
	unsigned long nLength;
	unsigned char *pSnapshotData = NULL;

	SetStringAsPath(&SnapshotPath, FilenameBuffer);
	
	nLength = Snapshot_CalculateOutputSize(SnapshotSize, SnapshotVersion);

	pSnapshotData = malloc(nLength);

	if (pSnapshotData!=NULL)
	{
		Snapshot_GenerateOutputData(pSnapshotData, SnapshotSize, SnapshotVersion);

		SaveFile(FilenameBuffer, pSnapshotData, nLength);
	
		free(pSnapshotData);
	}

	return TRUE;
}


BOOL	GenericInterface_SaveScreenSnapshot(const TCHAR *FilenameBuffer)
{
	if (FilenameBuffer!=NULL)
	{
		if (strlen(FilenameBuffer)!=0)
		{
			SetStringAsPath(&ScreenSnapshotPath, FilenameBuffer);
			
			Screen_SaveSnapshot(FilenameBuffer);
		
			return TRUE;
		}
	}

	return FALSE;
}

BOOL	GenericInterface_WavOutputStartRecording(const TCHAR *FilenameBuffer)
{
	if (FilenameBuffer!=NULL)
	{
		if (strlen(FilenameBuffer)!=0)
		{
			SetStringAsPath(&WavOutputPath, FilenameBuffer);

			WavOutput_StartRecording(FilenameBuffer);

			return TRUE;
		}
	}

	return FALSE;
}

void	GenericInterface_AYStartRecording(const TCHAR *FilenameBuffer)
{
	int YMVersion;

	if (FilenameBuffer!=NULL)
	{
		if (strlen(FilenameBuffer)!=0)
		{
			SetStringAsPath(&AYOutputPath, FilenameBuffer);

			{
				const TCHAR *pFilename = GetFilenameFromPathAndFilename(FilenameBuffer);
				const TCHAR *pExt = GetExtensionFromFilename(pFilename);

				if ((toupper(pExt[0])=='Y') && (toupper(pExt[1])=='M') && (pExt[3]=='5'))
				{
					YMVersion = 5;
				}
				else
				{
					YMVersion = 3;
				}

			}
			
			YMOutput_StartRecording(FilenameBuffer,YMVersion);
		}
	}
}

void	GenericInterface_RestoreSettings(void)
{
	/* if no settings file exist, then the default settings will be used.
	otherwise one or more of the default settings may be overridden */

	/* open settings */
	if (Settings_OpenSettings(0))
	{
		/* restore stored settings */

		/* restore CPC type */
//		if (MyApp_Registry_CheckValuePresent(_T("iCPCType")))
//		{
//			CPC_SetCPCType(MyApp_Registry_GetIntFromCurrentKey(_T("iCPCType")));
//		
//		}

		/* restore CRTC type */
		if (MyApp_Registry_CheckValuePresent(_T("iCRTCType")))
		{
			CPC_SetCRTCType(MyApp_Registry_GetIntFromCurrentKey(_T("iCRTCType")));
		}

		Host_LockSpeed = TRUE;
		if (MyApp_Registry_CheckValuePresent(_T("iRealtime")))
		{
			Host_LockSpeed = (MyApp_Registry_GetIntFromCurrentKey(_T("iRealtime"))!=0);
		}

		fShowReset = TRUE;
		if (MyApp_Registry_CheckValuePresent(_T("iWarnReset")))
		{
			fShowReset = (MyApp_Registry_GetIntFromCurrentKey(_T("iWarnReset"))!=0);
		}

		fShowQuit = TRUE;
		if (MyApp_Registry_CheckValuePresent(_T("iWarnExit")))
		{
			fShowQuit = (MyApp_Registry_GetIntFromCurrentKey(_T("iWarnExit"))!=0);
		}

		/* restore monitor type */
		if (MyApp_Registry_CheckValuePresent(_T("iMonitorType")))
		{
			CPC_SetMonitorType(MyApp_Registry_GetIntFromCurrentKey(_T("iMonitorType")));
		}
		
		/* restore monitor brightness */
		if (MyApp_Registry_CheckValuePresent(_T("iMonitorBrightness")))
		{
			CPC_SetMonitorBrightness(MyApp_Registry_GetIntFromCurrentKey(_T("iMonitorBrightness")));
		}

		/* restore printer output method */
//		if (MyApp_Registry_CheckValuePresent(_T("iPrinterOutputMethod")))
//		{
//			Printer_SetOutputMethod(MyApp_Registry_GetIntFromCurrentKey(_T("iPrinterOutputMethod")));
//		}

		/* restore frame skip settings */
		if (MyApp_Registry_CheckValuePresent(_T("iFrameSkip")))
		{
			CPC_SetFrameSkip(MyApp_Registry_GetIntFromCurrentKey(_T("iFrameSkip")));
		}

		/* restore cartridge path */
		if (MyApp_Registry_CheckValuePresent(_T("sCartridgePath")))
		{
			SetString(&CartridgePath, MyApp_Registry_GetStringFromCurrentKey(_T("sCartridgePath")));
		}

		/* restore disk image path */
		if (MyApp_Registry_CheckValuePresent(_T("sDiskImagePath")))
		{
			SetString(&DiskImagePath, MyApp_Registry_GetStringFromCurrentKey(_T("sDiskImagePath")));
		}

		/* restore disk image path */
		if (MyApp_Registry_CheckValuePresent(_T("sMultifaceCPC")))
		{
			SetString(&Multiface_CPC_ROM_Filename, MyApp_Registry_GetStringFromCurrentKey(_T("sMultifaceCPC")));
		
			Interface_LoadMultifaceROM(MULTIFACE_ROM_CPC_VERSION, Multiface_CPC_ROM_Filename);
		}

		/* restore disk image path */
		if (MyApp_Registry_CheckValuePresent(_T("sMultifaceCPC+")))
		{
			SetString(&Multiface_CPCPLUS_ROM_Filename, MyApp_Registry_GetStringFromCurrentKey(_T("sMultifaceCPC+")));
		
			Interface_LoadMultifaceROM(MULTIFACE_ROM_CPCPLUS_VERSION, Multiface_CPCPLUS_ROM_Filename);
		}

		/* restore screen snapshot path */
		if (MyApp_Registry_CheckValuePresent(_T("sScreenSnapshotPath")))
		{
			SetString(&ScreenSnapshotPath, MyApp_Registry_GetStringFromCurrentKey(_T("sScreenSnapshotPath")));
		}

		/* restore wave output settings */
		if (MyApp_Registry_CheckValuePresent(_T("sWavOutputPath")))
		{
			SetString(&WavOutputPath, MyApp_Registry_GetStringFromCurrentKey(_T("sWavOutputPath")));
		}

		/* restore snapshot path */
		if (MyApp_Registry_CheckValuePresent(_T("sSnapshotPath")))
		{
			SetString(&SnapshotPath, MyApp_Registry_GetStringFromCurrentKey(_T("sSnapshotPath")));
		}

		/* restore AY/PSG output path */
		if (MyApp_Registry_CheckValuePresent(_T("sAYOutputPath")))
		{
			SetString(&AYOutputPath, MyApp_Registry_GetStringFromCurrentKey(_T("sAYOutputPath")));
		}

		/* restore rom path */
		if (MyApp_Registry_CheckValuePresent(_T("sRomPath")))
		{
			SetString(&RomPath, MyApp_Registry_GetStringFromCurrentKey(_T("sRomPath")));
		}

		/* restore tape image path */
		if (MyApp_Registry_CheckValuePresent(_T("sTapePath")))
		{
			SetString(&TapePath, MyApp_Registry_GetStringFromCurrentKey(_T("sTapePath")));
		}

		/* restore roms */
		GenericInterface_RestoreRomSettings();

		/* restore disk images */
		Interface_InsertDiskFromFile(0, MyApp_Registry_GetStringFromCurrentKey(_T("sDriveADiskImage")));
		Interface_InsertDiskFromFile(1, MyApp_Registry_GetStringFromCurrentKey(_T("sDriveBDiskImage")));
		
			MyApp_Registry_CloseKey();
	}
}


void	GenericInterface_StoreSettings(void)
{
	/* store current settings */
	if (Settings_OpenSettings(0))
	{
		/* store CPC type */
//			MyApp_Registry_StoreIntToCurrentKey(_T("iCPCType"), CPC_GetCPCType());
		/* store CRTC type */
			MyApp_Registry_StoreIntToCurrentKey(_T("iCRTCType"), CPC_GetCRTCType());
			MyApp_Registry_StoreIntToCurrentKey(_T("iRealtime"), Host_LockSpeed);
			MyApp_Registry_StoreIntToCurrentKey(_T("iWarnReset"), fShowReset);
			MyApp_Registry_StoreIntToCurrentKey(_T("iWarnExit"), fShowQuit);
		/* store monitor type */
			MyApp_Registry_StoreIntToCurrentKey(_T("iMonitorType"), CPC_GetMonitorType());
		/* store monitor brightness */
			MyApp_Registry_StoreIntToCurrentKey(_T("iMonitorBrightness"), CPC_GetMonitorBrightness());
		/* store printer output method */
//			MyApp_Registry_StoreIntToCurrentKey(_T("iPrinterOutputMethod"), Printer_GetOutputMethod());
		/* store frame skip */
			MyApp_Registry_StoreIntToCurrentKey(_T("iFrameSkip"), CPC_GetFrameSkip());
		/* store cartridge path */
		MyApp_Registry_StoreStringToCurrentKey(_T("sCartridgePath"), GetString(CartridgePath));
		/* store disk image path */
		MyApp_Registry_StoreStringToCurrentKey(_T("sDiskImagePath"), GetString(DiskImagePath));
		/* store screen snapshot path */
		MyApp_Registry_StoreStringToCurrentKey(_T("sScreenSnapshotPath"), GetString(ScreenSnapshotPath));
		/* store snapshot path */
		MyApp_Registry_StoreStringToCurrentKey(_T("sSnapshotPath"), GetString(SnapshotPath));
		/* store wave output path */
		MyApp_Registry_StoreStringToCurrentKey(_T("sWavOutputPath"), GetString(WavOutputPath));
		/* store AY output path */
		MyApp_Registry_StoreStringToCurrentKey(_T("sAYOutputPath"), GetString(AYOutputPath));
		/* store tape image path */
		MyApp_Registry_StoreStringToCurrentKey(_T("sTapePath"), GetString(TapePath));
		/* store full path and filename for disk image that has been inserted */
		MyApp_Registry_StoreStringToCurrentKey(_T("sDriveADiskImage"), GetString(DriveFilenames[0]));
		/* store full path and filename for disk image that has been inserted */
		MyApp_Registry_StoreStringToCurrentKey(_T("sDriveBDiskImage"), GetString(DriveFilenames[1]));
		/* store full path and filename for cartridge that has been inserted */
		MyApp_Registry_StoreStringToCurrentKey(_T("sCartridge"), GetString(CartridgeFilename));
		/* store rom path */
		MyApp_Registry_StoreStringToCurrentKey(_T("sRomPath"), GetString(RomPath));

		
		MyApp_Registry_StoreStringToCurrentKey(_T("sMultifaceCPC"), GetString(Multiface_CPC_ROM_Filename));
		MyApp_Registry_StoreStringToCurrentKey(_T("sMultifaceCPC+"), GetString(Multiface_CPCPLUS_ROM_Filename));

		/* store roms that were loaded */
		GenericInterface_StoreRomSettings();
		/* close settings */
			MyApp_Registry_CloseKey();
	}
}




/* append a null terminated string */
static TCHAR *AppendString(TCHAR *pPtr, const TCHAR *pDescription)
{
	/* get string length */
	unsigned long length = _tcslen(pDescription);

	/* copy the characters */
	memcpy(pPtr, pDescription, (length*sizeof(TCHAR)));
	pPtr+=length;

	return pPtr;
}

/* append char */
static TCHAR *AppendChar(TCHAR *pPtr, const TCHAR ch)
{
	pPtr[0]=ch;
	pPtr++;

	return pPtr;
}

static TCHAR *AddExtensions(TCHAR *pPtr, const TCHAR **pExtensionList)
{
	char **pCurrentExtension;
	BOOL bFirst;
	
	/* begin extension list part of description */
	pPtr = AppendString(pPtr,_T(" ("));
	bFirst = TRUE;

	/* complete description */
	pCurrentExtension = pExtensionList;
	while (*pCurrentExtension!=NULL)
	{
		char *pExtension = *pCurrentExtension;

		/* if there is a extension preceeding, then add a ; symbol */
		if (!bFirst)
		{
			pPtr = AppendChar(pPtr, _T(';'));
		}
		bFirst = FALSE;

		/* add the extension */
		pPtr = AppendString(pPtr, _T("*."));
		pPtr = AppendString(pPtr, pExtension);
				
		pCurrentExtension++;
	}

	/* end description part */
	pPtr = AppendChar(pPtr,_T(')'));
	pPtr = AppendChar(pPtr,_T('\0'));

	bFirst = TRUE;
	/* now do file type list part */
	pCurrentExtension = pExtensionList;
	while (*pCurrentExtension!=NULL)
	{
		char *pExtension = *pCurrentExtension;

		/* if there is a extension preceeding, then add a ; symbol */
		if (!bFirst)
		{
			pPtr = AppendChar(pPtr,_T(';'));
		}
		bFirst = FALSE;

		/* add the extension */
		pPtr = AppendString(pPtr, _T("*."));
		pPtr = AppendString(pPtr, pExtension);
		
		pCurrentExtension++;
	}

	return pPtr;
}


const TCHAR *pDiskImageWriteExtensions[]=
{
	_T("dsk"),
	NULL
};

const TCHAR *pDiskImageReadExtensions[]=
{
	_T("dsk"),
	_T("dif"),
	NULL
};

const TCHAR *ZipExtensions[]=
{
	_T("zip"),
	NULL
};

const TCHAR *AllFileExtensions[]=
{
	_T("*"),
	NULL,
};

const TCHAR *TextFileExtensions[]=
{
	_T("txt"),
	NULL,
};






void	Interface_RemoveDisk(HWND hwndParent,int Drive)
{
	/* has image been modified? */
	if (DiskImage_IsImageDirty(Drive))
	{
		TCHAR ImageModifiedText[128];

		_stprintf(ImageModifiedText,Messages[0],Drive);

		/* ask if we want changes to be saved.. */
		if (MessageBox(hwndParent, ImageModifiedText,Messages[1], MB_YESNO | MB_ICONQUESTION)==IDYES)
		{
			const TCHAR *sFilename;
			unsigned char *pDiskImage;
			unsigned long nDiskImage;

			sFilename = DriveFilenames[Drive];
			if (sFilename==NULL)
			{
				/* ask for filename */
				OPENFILENAME	DiskImageOpenFilename;
				TCHAR			FilesOfType[256];
				FilenameBuffer[0]=_T('\0');

				{
					TCHAR *pPtr;


					pPtr = FilesOfType;

					pPtr = AppendString(pPtr, Messages[23]);
					pPtr = AddExtensions(pPtr, pDiskImageWriteExtensions);
					pPtr = AppendChar(pPtr,_T('\0'));
					pPtr = AppendString(pPtr, Messages[28]);
					pPtr = AddExtensions(pPtr, AllFileExtensions);
					pPtr = AppendChar(pPtr,_T('\0'));
					pPtr = AppendChar(pPtr,_T('\0'));
				}

				InitFileDlg(hwndParent,&DiskImageOpenFilename,_T("dsk"),FilesOfType,OFN_EXPLORER|OFN_OVERWRITEPROMPT|OFN_CREATEPROMPT);
				if (GetSaveNameFromDlg(hwndParent,&DiskImageOpenFilename,NULL,Messages[2],FilenameBuffer, DiskImagePath))
				{
					if (_tcslen(FilenameBuffer)!=0)
					{
						sFilename = FilenameBuffer;
					}
				}
			}

			if (sFilename)
			{
				nDiskImage = DiskImage_CalculateOutputSize(Drive);
				
				pDiskImage = malloc(nDiskImage);

				if (pDiskImage)
				{
					DiskImage_GenerateOutputData(pDiskImage,Drive);

					SaveFile(sFilename, pDiskImage, nDiskImage);

					free(pDiskImage);
				}
			}
		}
	}

	DiskImage_RemoveDisk(Drive);

	SetString(&DriveFilenames[0], NULL);
}

/*------------------------------------------------------------------------------------------*/
void	Interface_InsertUnformattedDisk(HWND hwndParent,int Drive)
{
	Interface_RemoveDisk(hwndParent,Drive);
	
	/* insert the unformatted disk */
	DiskImage_InsertUnformattedDisk(Drive);
}

/*------------------------------------------------------------------------------------------*/
/* Insert a disk image into an Amstrad drive */
void Interface_InsertDisk(HWND hwndParent, int DriveID, BOOL bAutostart)
{
	OPENFILENAME	DiskImageOpenFilename;
	TCHAR			FilesOfType[256];

	InitFilenameForFileOpenDlg(DriveFilenames[DriveID], FilenameBuffer);

	{
		TCHAR *pPtr;

		pPtr = FilesOfType;

		pPtr = AppendString(pPtr, Messages[23]);
		pPtr = AddExtensions(pPtr, pDiskImageReadExtensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendString(pPtr,Messages[29]);
		pPtr = AddExtensions(pPtr, ZipExtensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendString(pPtr, Messages[28]);
		pPtr = AddExtensions(pPtr, AllFileExtensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendChar(pPtr,_T('\0'));
	}

	InitFileDlg(hwndParent,&DiskImageOpenFilename,_T("dsk"),FilesOfType,OFN_EXPLORER);
	if (GetFileNameFromDlg(hwndParent,&DiskImageOpenFilename,NULL,Messages[3],FilenameBuffer, DiskImagePath))
	{
		if (_tcslen(FilenameBuffer)!=0)
		{
			TCHAR FileFromZip[MAX_PATH];
			TCHAR *pOpenFilename;
			unsigned char *pDiskImage;
			unsigned long DiskImageLength;
			int nStatus;

			/* set the directory */
			SetStringAsPath(&DiskImagePath, FilenameBuffer);

			nStatus = Interface_HandleZipFile(hwndParent, FilenameBuffer, FileFromZip);
			
			/* if cancelled or error then quit */
			if ((nStatus==ZIP_SUPPORT_ZIP_CANCEL) || (nStatus==ZIP_SUPPORT_ZIP_ERROR))
				return;

			/* setup filename based on if user chose file from zip or original file was
			not a zip archive */
			if (nStatus==ZIP_SUPPORT_NOT_ZIP)
			{
				pOpenFilename = FilenameBuffer;
			}
			else
			{
				pOpenFilename = FileFromZip;
			}

			/* if an existing disk is present then ask to save it out */
			if (FDD_IsDiskPresent(DriveID))
			{
				Interface_RemoveDisk(hwndParent,DriveID);
			}

			/* load disk image file to memory */
			LoadFile(pOpenFilename, &pDiskImage, &DiskImageLength);

			if (pDiskImage!=NULL)
			{
				int nStatus;

				/* try to insert it */
				nStatus = DiskImage_InsertDisk(DriveID, pDiskImage, DiskImageLength);

				if (nStatus==ARNOLD_STATUS_OK)
				{
					/* add to recent */
					RecentItems_AddAndRefresh(hwndParent, pOpenFilename, RECENT_LIST_FILES);

					/* autostart? */
					if (bAutostart)
					{
						char *pBuffer = malloc(512*5);
						if (pBuffer)
						{
							/* try auto-start */
 							if (AMSDOS_GenerateAutorunCommand(pBuffer,AutoType_String))
 							{
 								AutoType_SetString(AutoType_String, TRUE);
							}
							else
							{
								MessageBox(GetDesktopWindow(), _T("Unable to auto-start this disc"),_T("Arnold"), MB_OK);

							}
						
							free(pBuffer);
						}
					}
				}

				free(pDiskImage);
			}
		}
	}
}

const TCHAR *YM5_Extensions[]=
{
	_T("ym5"),
	NULL
};


const TCHAR *YM3_Extensions[]=
{
	_T("ym3"),
	NULL
};


void	Interface_OutputYM(HWND hwndParent)
{
	OPENFILENAME	AYOpenFilename;
	TCHAR			FilesOfType[256];

	FilenameBuffer[0] = _T('\0');

	{
		TCHAR *pPtr;


		pPtr = FilesOfType;

		pPtr = AppendString(pPtr, Messages[30]);
		pPtr = AddExtensions(pPtr, YM5_Extensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendString(pPtr, Messages[31]);
		pPtr = AddExtensions(pPtr, YM3_Extensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendString(pPtr, Messages[28]);
		pPtr = AddExtensions(pPtr, AllFileExtensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendChar(pPtr,_T('\0'));
	}


	InitFileDlg(hwndParent,&AYOpenFilename,_T("ym5"),FilesOfType,0);
	if (GetFileNameFromDlg(hwndParent,&AYOpenFilename,NULL,Messages[5],FilenameBuffer, AYOutputPath))
	{

#if 0
	if (YMOutputEnabled==TRUE)
	{
//		if (fhYMOutput!=0)
		{
			if (YMOutput_StartRecordingWhenSilenceEnds)
			{
				/* we want to start recording when silence ends */

				/* are we recording ? */
				if (!YMOutput_Recording)
				{
					/* no */

					/* is output silent? */
					if (!YMOutput_IsSilent())
					{
						/* not silent, so we can start to record */
						YMOutput_Recording = TRUE;
					}
				}
			}
			else
			{
				/* do not record when silence ends, so we must be able
				to record all the time */
				YMOutput_Recording = TRUE;
			}
#endif

		GenericInterface_AYStartRecording(FilenameBuffer);
	}
}

const TCHAR *Wave_Extensions[]=
{
	_T("wav"),
	NULL
};


void	Interface_OutputWav(HWND hwndParent)
{
	OPENFILENAME	WavOpenFilename;
	TCHAR			FilesOfType[256];

	FilenameBuffer[0]=_T('\0');

	{
		TCHAR *pPtr;


		pPtr = FilesOfType;

		pPtr = AppendString(pPtr, Messages[32]);
		pPtr = AddExtensions(pPtr, Wave_Extensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendString(pPtr, Messages[28]);
		pPtr = AddExtensions(pPtr, AllFileExtensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendChar(pPtr,_T('\0'));
	}

	InitFileDlg(hwndParent,&WavOpenFilename,_T("wav"),FilesOfType,0);
	if (GetFileNameFromDlg(hwndParent,&WavOpenFilename,NULL,Messages[6],FilenameBuffer, WavOutputPath))
	{
		if (_tcslen(FilenameBuffer)!=0)
		{
			GenericInterface_WavOutputStartRecording(FilenameBuffer);
		}
	}
}

const TCHAR *ROM_Extensions[]=
{
	_T("rom"),
	NULL
};

const TCHAR *BIN_Extensions[]=
{
	_T("bin"),
	NULL
};



/* open a requester to select a rom */
void Interface_InsertRom(HWND hwndParent, int RomIndex)
{
	OPENFILENAME	RomOpenFilename;
	TCHAR			FilesOfType[256];

	InitFilenameForFileOpenDlg(RomFilename, FilenameBuffer);

	{
		TCHAR *pPtr;


		pPtr = FilesOfType;

		pPtr = AppendString(pPtr, Messages[33]);
		pPtr = AddExtensions(pPtr, ROM_Extensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendString(pPtr, Messages[34]);
		pPtr = AddExtensions(pPtr, BIN_Extensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendString(pPtr, Messages[28]);
		pPtr = AddExtensions(pPtr, AllFileExtensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendChar(pPtr,_T('\0'));
	}

	InitFileDlg(hwndParent,&RomOpenFilename,_T("rom"),FilesOfType,0);
	if (GetFileNameFromDlg(hwndParent,&RomOpenFilename,NULL,Messages[7],FilenameBuffer, RomPath))
	{
		if (_tcslen(FilenameBuffer)!=0)
		{
			TCHAR FileFromZip[MAX_PATH];
			TCHAR *pOpenFilename;
			unsigned char *pRomData;
			unsigned long RomDataSize;
			int nStatus;

			/* set the directory */
			SetStringAsPath(&RomPath,FilenameBuffer);

			nStatus = Interface_HandleZipFile(hwndParent, FilenameBuffer, FileFromZip);
			
			/* if cancelled or error then quit */
			if ((nStatus==ZIP_SUPPORT_ZIP_CANCEL) || (nStatus==ZIP_SUPPORT_ZIP_ERROR))
				return;

			/* setup filename based on if user chose file from zip or original file was
			not a zip archive */
			if (nStatus==ZIP_SUPPORT_NOT_ZIP)
			{
				pOpenFilename = FilenameBuffer;
			}
			else
			{
				pOpenFilename = FileFromZip;
			}

			/* try to load it */
			LoadFile(pOpenFilename, &pRomData, &RomDataSize);

			if (pRomData!=NULL)
			{
				ExpansionRom_SetRomData(pRomData, RomDataSize, RomIndex);
	
				SetString(&RomFilename, pOpenFilename);
				SetString(&RomFilenames[RomIndex],pOpenFilename);
				
				free(pRomData);
			}
		}
	}
}


/* open a requester for Multiface CPC+ rom */
void Interface_InsertMultifaceROM(HWND hwndParent,MULTIFACE_ROM_TYPE RomType)
{
	OPENFILENAME	RomOpenFilename;
	TCHAR			FilesOfType[256];

	InitFilenameForFileOpenDlg(RomFilename, FilenameBuffer);
	
	{
		TCHAR *pPtr;


		pPtr = FilesOfType;

		pPtr = AppendString(pPtr, Messages[33]);
		pPtr = AddExtensions(pPtr, ROM_Extensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendString(pPtr, Messages[34]);
		pPtr = AddExtensions(pPtr, BIN_Extensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendString(pPtr, Messages[28]);
		pPtr = AddExtensions(pPtr, AllFileExtensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendChar(pPtr,_T('\0'));
	}

	InitFileDlg(hwndParent,&RomOpenFilename,_T("rom"),FilesOfType,0);
	if (GetFileNameFromDlg(hwndParent,&RomOpenFilename,NULL,Messages[8],FilenameBuffer, RomPath))
	{
		if (_tcslen(FilenameBuffer)!=0)
		{
			TCHAR FileFromZip[MAX_PATH];
			TCHAR *pOpenFilename;
			unsigned char *pRomData;
			unsigned long RomSize;
			int nStatus;

			/* set the directory */
			SetStringAsPath(&RomPath,FilenameBuffer);

			nStatus = Interface_HandleZipFile(hwndParent, FilenameBuffer, FileFromZip);
			
			/* if cancelled or error then quit */
			if ((nStatus==ZIP_SUPPORT_ZIP_CANCEL) || (nStatus==ZIP_SUPPORT_ZIP_ERROR))
				return;

			/* setup filename based on if user chose file from zip or original file was
			not a zip archive */
			if (nStatus==ZIP_SUPPORT_NOT_ZIP)
			{
				pOpenFilename = FilenameBuffer;
			}
			else
			{
				pOpenFilename = FileFromZip;
			}

			/* try to load it */
			LoadFile(pOpenFilename, &pRomData, &RomSize);

			if (pRomData!=NULL)
			{
				if (Multiface_SetRomData(RomType, pRomData, RomSize)==ARNOLD_STATUS_OK)
				{
					SetString(&RomFilename, pOpenFilename);

					if (RomType==MULTIFACE_ROM_CPC_VERSION)
					{
						SetString(&Multiface_CPC_ROM_Filename, pOpenFilename);
					}
					else
					{
						SetString(&Multiface_CPCPLUS_ROM_Filename, pOpenFilename);
					}
				}
			
				free(pRomData);
			}
		}
	}

}

extern TCHAR *Z80MemoryBase;

const TCHAR *CDT_Extensions[]=
{
	_T("cdt"),
	_T("tzx"),
	_T("wav"),
	_T("voc"),
	_T("csw"),
	NULL
};

const TCHAR *CDTW_Extensions[]=
{
	_T("wav"),
	_T("voc"),
	_T("csw"),
	NULL
};

const TCHAR *CDTT_Extensions[]=
{
	_T("cdt"),
	_T("tzx"),
	NULL
};

/* open a requester to select a tape image */
void Interface_InsertTape(HWND hwndParent,BOOL bAutostart)
{
	OPENFILENAME	TapeOpenFilename;
	TCHAR			FilesOfType[256];

	InitFilenameForFileOpenDlg(TapeFilename, FilenameBuffer);

	{
		TCHAR *pPtr;


		pPtr = FilesOfType;

		pPtr = AppendString(pPtr, Messages[35]);
		pPtr = AddExtensions(pPtr, CDT_Extensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendString(pPtr, Messages[36]);
		pPtr = AddExtensions(pPtr, CDTW_Extensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendString(pPtr, Messages[37]);
		pPtr = AddExtensions(pPtr, CDTT_Extensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendString(pPtr, Messages[28]);
		pPtr = AddExtensions(pPtr, AllFileExtensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendChar(pPtr,_T('\0'));
	}

	InitFileDlg(hwndParent,&TapeOpenFilename,_T("cdt"),FilesOfType,0);
	if (GetFileNameFromDlg(hwndParent,&TapeOpenFilename,NULL,Messages[10],FilenameBuffer,TapePath))
	{
		if (_tcslen(FilenameBuffer)!=0)
		{
			TCHAR FileFromZip[MAX_PATH];
			TCHAR *pOpenFilename;
			unsigned char *pTapeImageData;
			unsigned long TapeImageDataSize;
			int nStatus;

			/* set the directory */
			SetStringAsPath(&TapePath,FilenameBuffer);

			nStatus = Interface_HandleZipFile(hwndParent, FilenameBuffer, FileFromZip);
			
			/* if cancelled or error then quit */
			if ((nStatus==ZIP_SUPPORT_ZIP_CANCEL) || (nStatus==ZIP_SUPPORT_ZIP_ERROR))
				return;

			/* setup filename based on if user chose file from zip or original file was
			not a zip archive */
			if (nStatus==ZIP_SUPPORT_NOT_ZIP)
			{
				pOpenFilename = FilenameBuffer;
			}
			else
			{
				pOpenFilename = FileFromZip;
			}

			/* try to load it */
			LoadFile(pOpenFilename, &pTapeImageData, &TapeImageDataSize);

			if (pTapeImageData!=NULL)
			{
				if (TapeImage_Insert(pTapeImageData, TapeImageDataSize)==ARNOLD_STATUS_OK)
				{
					if (bAutostart)
					{
						AutoType_SetString(sAutoStringTape, TRUE);
					}

					SetString(&TapeFilename, pOpenFilename);
					RecentItems_AddAndRefresh(hwndParent, pOpenFilename, RECENT_LIST_FILES);
				
					free(pTapeImageData);
				}
				else
				{
					free(pTapeImageData);
				
					if (Sample_Load(pOpenFilename))
					{
						if (bAutostart)
						{
							AutoType_SetString(sAutoStringTape, TRUE);
						}

						SetString(&TapeFilename, pOpenFilename);
						RecentItems_AddAndRefresh(hwndParent, pOpenFilename, RECENT_LIST_FILES);

					}
				}
			}
		}
	}
}

/*------------------------------------------------------------------------------------------*/

const TCHAR *SNA_Extensions[]=
{
	_T("sna"),
	NULL
};

/* open a requester to load a snapshot */
void Interface_LoadSnapshot(HWND hwndParent)
{
	OPENFILENAME	SnapshotOpenFilename;
	TCHAR			FilesOfType[256];
	
	InitFilenameForFileOpenDlg(SnapshotFilename, FilenameBuffer);
	{
		TCHAR *pPtr;


		pPtr = FilesOfType;

		pPtr = AppendString(pPtr, Messages[38]);
		pPtr = AddExtensions(pPtr, SNA_Extensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendString(pPtr, Messages[28]);
		pPtr = AddExtensions(pPtr, AllFileExtensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendChar(pPtr,_T('\0'));
	}

	InitFileDlg(hwndParent,&SnapshotOpenFilename,_T("sna"),FilesOfType,OFN_EXPLORER);
	if (GetFileNameFromDlg(hwndParent,&SnapshotOpenFilename,NULL,Messages[12],FilenameBuffer,SnapshotPath))
	{
		if (_tcslen(FilenameBuffer)!=0)
		{
			TCHAR FileFromZip[MAX_PATH];
			TCHAR *pOpenFilename;
			unsigned char *pSnapshotData;
			unsigned long SnapshotDataLength;
			int nStatus;

			/* set the directory */
			SetStringAsPath(&SnapshotPath,FilenameBuffer);

			nStatus = Interface_HandleZipFile(hwndParent, FilenameBuffer, FileFromZip);
			
			/* if cancelled or error then quit */
			if ((nStatus==ZIP_SUPPORT_ZIP_CANCEL) || (nStatus==ZIP_SUPPORT_ZIP_ERROR))
				return;

			/* setup filename based on if user chose file from zip or original file was
			not a zip archive */
			if (nStatus==ZIP_SUPPORT_NOT_ZIP)
			{
				pOpenFilename = FilenameBuffer;
			}
			else
			{
				pOpenFilename = FileFromZip;
			}

			/* try to load it */
			LoadFile(pOpenFilename, &pSnapshotData, &SnapshotDataLength);

			if (pSnapshotData!=NULL)
			{
				if (Snapshot_Insert(pSnapshotData, SnapshotDataLength)==ARNOLD_STATUS_OK)
				{
					SetString(&SnapshotFilename, pOpenFilename);
					RecentItems_AddAndRefresh(hwndParent, pOpenFilename, RECENT_LIST_FILES);
				}

				free(pSnapshotData);
			}
		}
	}
}

/*------------------------------------------------------------------------------------------*/
const TCHAR *CPR_Extensions[]=
{
	_T("cpr"),
	NULL
};

void	Interface_OpenCartridge(HWND hwndParent)
{
	OPENFILENAME	CartridgeOpenFilename;
	TCHAR			FilesOfType[256];

	InitFilenameForFileOpenDlg(CartridgeFilename, FilenameBuffer);
	
	{
		TCHAR *pPtr;


		pPtr = FilesOfType;

		pPtr = AppendString(pPtr, Messages[39]);
		pPtr = AddExtensions(pPtr, CPR_Extensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendString(pPtr, Messages[28]);
		pPtr = AddExtensions(pPtr, AllFileExtensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendChar(pPtr,_T('\0'));
	}

	InitFileDlg(hwndParent,&CartridgeOpenFilename,_T("cpr"),FilesOfType,OFN_EXPLORER);
	
	if (GetFileNameFromDlg(hwndParent,&CartridgeOpenFilename,NULL,Messages[13],FilenameBuffer,CartridgePath))
	{
		if (_tcslen(FilenameBuffer)!=0)
		{
			TCHAR FileFromZip[MAX_PATH];
			TCHAR *pOpenFilename;
			unsigned char *pCartridgeData;
			unsigned long CartridgeLength;
			int nStatus;

			/* set the directory */
			SetStringAsPath(&CartridgePath, FilenameBuffer);

			nStatus = Interface_HandleZipFile(hwndParent, FilenameBuffer, FileFromZip);
			
			/* if cancelled or error then quit */
			if ((nStatus==ZIP_SUPPORT_ZIP_CANCEL) || (nStatus==ZIP_SUPPORT_ZIP_ERROR))
				return;

			/* setup filename based on if user chose file from zip or original file was
			not a zip archive */
			if (nStatus==ZIP_SUPPORT_NOT_ZIP)
			{
				pOpenFilename = FilenameBuffer;
			}
			else
			{
				pOpenFilename = FileFromZip;
			}

			/* try to load it */
			LoadFile(pOpenFilename, &pCartridgeData, &CartridgeLength);

			if (pCartridgeData!=NULL)
			{
				if (Cartridge_AttemptInsert(pCartridgeData, CartridgeLength)==ARNOLD_STATUS_OK)
				{
					SetString(&CartridgeFilename, pOpenFilename);
					RecentItems_AddAndRefresh(hwndParent, pOpenFilename, RECENT_LIST_FILES);
				}

				free(pCartridgeData);
			}
		}
	}
}
/*------------------------------------------------------------------------------------------*/






// write a snapshot
BOOL	Interface_SaveSnapshot(HWND hwndParent)
{
	OPENFILENAME	SnapshotSaveFilename;
	TCHAR			FilesOfType[256];

	FilenameBuffer[0]=_T('\0');
	{
		TCHAR *pPtr;


		pPtr = FilesOfType;

		pPtr = AppendString(pPtr, Messages[38]);
		pPtr = AddExtensions(pPtr, SNA_Extensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendString(pPtr, Messages[28]);
		pPtr = AddExtensions(pPtr, AllFileExtensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendChar(pPtr,_T('\0'));
	}

	InitFileDlg(hwndParent,&SnapshotSaveFilename,_T("sna"),FilesOfType,OFN_EXPLORER | OFN_OVERWRITEPROMPT);
	if (GetSaveNameFromDlg(hwndParent,&SnapshotSaveFilename,NULL,Messages[14],FilenameBuffer,SnapshotPath))
	{
		if (_tcslen(FilenameBuffer)!=0)
		{
			GenericInterface_SnapshotSave(FilenameBuffer, AppData.SnapshotVersion, AppData.SnapshotSize);
		}

		return TRUE;
	}

	return FALSE;
}

const TCHAR *BMP_Extensions[]=
{
	_T("bmp"),
	NULL
};

// write a screen snapshot
BOOL	Interface_SaveScreenSnapshot()
{
	OPENFILENAME	SnapshotSaveFilename;
	HWND			hwnd = GetActiveWindow();
	TCHAR			FilesOfType[256];

	FilenameBuffer[0]=_T('\0');
	{
		TCHAR *pPtr;


		pPtr = FilesOfType;

		pPtr = AppendString(pPtr, Messages[40]);
		pPtr = AddExtensions(pPtr, BMP_Extensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendString(pPtr, Messages[28]);
		pPtr = AddExtensions(pPtr, AllFileExtensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendChar(pPtr,_T('\0'));
	}
#if 0
	/* Troels K. begin */
   InitFileDlg(hwnd,&SnapshotSaveFilename,_T("*.png"),
      "PNG Files (*.png)\0*.png\0"
      "Bitmap Files (*.bmp)\0*.bmp\0"
      "All files (*.*)\0*.*\0"
      "\0"),      
      OFN_EXPLORER | OFN_OVERWRITEPROMPT);
	/* Troels K. end */
#endif


	InitFileDlg(hwnd,&SnapshotSaveFilename,_T("bmp"),FilesOfType,OFN_EXPLORER | OFN_OVERWRITEPROMPT);
	if (GetSaveNameFromDlg(hwnd,&SnapshotSaveFilename,NULL,Messages[15],FilenameBuffer,ScreenSnapshotPath))
	{

		if (_tcslen(FilenameBuffer)!=0)
		{
			GenericInterface_SaveScreenSnapshot(FilenameBuffer);
		}

		return TRUE;
	}

	return FALSE;
}


BOOL	Interface_ToggleItemState(HMENU hMenu,int Ident)
{
	MENUITEMINFO	MenuItemInfo;

	MenuItemInfo.cbSize = sizeof(MENUITEMINFO);
	MenuItemInfo.fMask = MIIM_STATE;
		
	if (GetMenuItemInfo(hMenu,Ident,FALSE,&MenuItemInfo)!=TRUE)
		return FALSE;


	if ((MenuItemInfo.fState & MFS_CHECKED)==MFS_CHECKED)
	{
		MenuItemInfo.fState &= ~(MFS_CHECKED | MFS_UNCHECKED);
		MenuItemInfo.fState |= MFS_UNCHECKED;
	}
	else
	{
		MenuItemInfo.fState &= ~(MFS_CHECKED | MFS_UNCHECKED);
		MenuItemInfo.fState |= MFS_CHECKED;
	}

	SetMenuItemInfo(hMenu,Ident,FALSE,&MenuItemInfo);

	return ((MenuItemInfo.fState & MFS_CHECKED)==MFS_CHECKED);
}


void	Interface_SetItemCheckState(HMENU hMenu, int Ident, BOOL Status)
{
	MENUITEMINFO	MenuItemInfo;

	/* get current state */
	MenuItemInfo.cbSize = sizeof(MENUITEMINFO);
	MenuItemInfo.fMask = MIIM_STATE;
	if (GetMenuItemInfo(hMenu, Ident, FALSE, &MenuItemInfo))
	{
		
		MenuItemInfo.fState &= ~(MFS_CHECKED | MFS_UNCHECKED);

		if (Status)
		{
			MenuItemInfo.fState |= MFS_CHECKED;
		}
		else
		{
			MenuItemInfo.fState |= MFS_UNCHECKED;
		}

		SetMenuItemInfo(hMenu,Ident,FALSE,&MenuItemInfo);
	}
}

void	Interface_SetItemGreyedState(HMENU hMenu, int Ident, BOOL Status)
{
	MENUITEMINFO	MenuItemInfo;

	/* get current state */
	MenuItemInfo.cbSize = sizeof(MENUITEMINFO);
	MenuItemInfo.fMask = MIIM_STATE;
	if (GetMenuItemInfo(hMenu, Ident, FALSE, &MenuItemInfo))
	{

		if (Status)
		{
			MenuItemInfo.fState |= MFS_GRAYED;
		}
		else
		{
			MenuItemInfo.fState &= ~MFS_GRAYED;
		}

		SetMenuItemInfo(hMenu,Ident,FALSE,&MenuItemInfo);
	}
}


void	SetCheckButtonState(HWND hDialog, int Ident, BOOL State)
{
	if (State == TRUE)
	{
		CheckDlgButton(hDialog, Ident,BST_CHECKED);
	}
	else
	{
		CheckDlgButton(hDialog, Ident, BST_UNCHECKED);
	}
}




/*----------------------------------------- */


void	Interface_SetupCPCTypeMenu(int CPCType)
{
	BOOL CPC464_ENABLED = FALSE;
	BOOL CPC664_ENABLED = FALSE;
	BOOL CPC6128_ENABLED = FALSE;
	BOOL CPC464PLUS_ENABLED = FALSE;
	BOOL CPC6128PLUS_ENABLED = FALSE;
	BOOL KCCOMPACT_ENABLED = FALSE;

	switch (CPCType)
	{

		case CPC_TYPE_CPC464:
		{
			CPC464_ENABLED = TRUE;
		}
		break;

		case CPC_TYPE_CPC664:
		{
			CPC664_ENABLED = TRUE;
		}
		break;

		case CPC_TYPE_CPC6128:
		{
			CPC6128_ENABLED = TRUE;
		}
		break;

		case CPC_TYPE_464PLUS:
		{
			CPC464PLUS_ENABLED = TRUE;
		}
		break;

		case CPC_TYPE_6128PLUS:
		{
			CPC6128PLUS_ENABLED = TRUE;
		}
		break;

		case CPC_TYPE_KCCOMPACT:
		{
			KCCOMPACT_ENABLED = TRUE;
		}
		break;

	
	}


	Interface_SetItemCheckState(AppData.hAppMenu, ID_CPCTYPE_CPC464,CPC464_ENABLED);
	Interface_SetItemCheckState(AppData.hAppMenu, ID_CPCTYPE_CPC664,CPC664_ENABLED);
	Interface_SetItemCheckState(AppData.hAppMenu, ID_CPCTYPE_CPC6128,CPC6128_ENABLED);
	Interface_SetItemCheckState(AppData.hAppMenu, ID_CPCTYPE_464PLUS,CPC464PLUS_ENABLED);
	Interface_SetItemCheckState(AppData.hAppMenu, ID_CPCTYPE_6128PLUS,CPC6128PLUS_ENABLED);
	Interface_SetItemCheckState(AppData.hAppMenu, ID_CPCTYPE_KCCOMPACT,KCCOMPACT_ENABLED);
}

const int CRTC_TypeArray[]=
{
	ID_CRTCTYPE_TYPE0,
	ID_CRTCTYPE_TYPE1,
	ID_CRTCTYPE_TYPE2,
	ID_CRTCTYPE_TYPEASIC,
	ID_CRTCTYPE_TYPEPREASIC,
};

const int NUM_CRTCS = (sizeof(CRTC_TypeArray)/sizeof(int));

// 491 520

void	Interface_SetupCRTCTypeMenu(int TypeIndex)
{
	int i;

	BOOL CRTC_Type_Enabled[NUM_CRTC_TYPES];

	for (i=0; i<NUM_CRTC_TYPES; i++)
	{
		CRTC_Type_Enabled[i] = FALSE;
	}

	CRTC_Type_Enabled[TypeIndex] = TRUE;

	for (i=0; i<NUM_CRTC_TYPES; i++)
	{
		Interface_SetItemCheckState(AppData.hAppMenu, CRTC_TypeArray[i],CRTC_Type_Enabled[i]);
	}
}


//======================================================================//


void Interface_OpenDebugDisplay(HWND hParent, BOOL State)
{
	if (State)
	{
		Debugger_OpenDebugDialog();
	}
	else
	{
		Debugger_CloseDebugDialog();
	}

	//Debugger_Enable(hParent,State);
}

static void	Interface_UpdateRamConfig()
{
	unsigned long RamConfig;
	BOOL RAM_64K_ENABLED = FALSE;
	BOOL RAM_256K_ENABLED = FALSE;
	BOOL RAM_256K_SILICON_DISK_ENABLED = FALSE;
	
	RamConfig = CPC_GetRamConfig();

	if (RamConfig & CPC_RAM_CONFIG_64K_RAM)
	{
		RAM_64K_ENABLED = TRUE;
	}

	if (RamConfig & CPC_RAM_CONFIG_256K_RAM)
	{
		RAM_256K_ENABLED = TRUE;
	}

	if (RamConfig & CPC_RAM_CONFIG_256K_SILICON_DISK)
	{
		RAM_256K_SILICON_DISK_ENABLED = TRUE;
	}

	Interface_SetItemCheckState(AppData.hAppMenu, ID_MISC_RAMCONFIG_128K,RAM_64K_ENABLED);
	Interface_SetItemCheckState(AppData.hAppMenu, ID_MISC_RAMCONFIG_256KRAMEXPANSION,RAM_256K_ENABLED);
	Interface_SetItemCheckState(AppData.hAppMenu, ID_MISC_RAMCONFIG_256KSILICONDISK,RAM_256K_SILICON_DISK_ENABLED);
}


/*----------------------------------------------------------------------------------------------*/
BOOL CALLBACK PokeMemory_DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_COMMAND:
		{
			WORD wID;

			wID = LOWORD(wParam);

			switch (wID)
			{
				case IDOK:
				{
					TCHAR sText[256];
					Z80_WORD nAddress;
					Z80_BYTE nValue;

					GetDlgItemText(hwndDlg, IDC_EDIT_ADDRESS, sText, sizeof(sText)/sizeof(TCHAR));

					nAddress= (Z80_WORD)EvaluateExpression(sText);

					GetDlgItemText(hwndDlg, IDC_EDIT_VALUE, sText, sizeof(sText)/sizeof(TCHAR));

					nValue=(Z80_BYTE)EvaluateExpression(sText);

					/* do the poke */
					Z80_WR_MEM(nAddress,nValue);
				}
				return EndDialog(hwndDlg, TRUE);
			
				case IDCANCEL:
					return EndDialog(hwndDlg, FALSE);
			}
		}
		break;
	
		default:
			break;
	}

	return FALSE;
}

/*----------------------------------------------------------------------------------------------*/
BOOL CALLBACK AutoType_DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			HWND hEdit = GetDlgItem(hwndDlg, IDC_EDIT_AUTOTYPE_TEXT);

			/* limit the amount of text that can be entered */
			SendMessage(hEdit, EM_LIMITTEXT, (WPARAM)256, (LPARAM)0);

		}
		break;

		case WM_COMMAND:
		{
			WORD wID;

			wID = LOWORD(wParam);

			switch (wID)
			{
				case IDOK:
				{
#ifdef _UNICODE
					TCHAR AutoTypeText_UNICODE[256];
					const char *AutoTypeText;

					GetDlgItemText(hwndDlg, IDC_EDIT_AUTOTYPE_TEXT,AutoTypeText, sizeof(AutoTypeText)/sizeof(AutoTypeText[0]));

					AutoTypeText = ConvertUnicodeStringToMultiByte(AutoTypeText_UNICODE);

					if (AutoTypeText)
					{
						strcpy(AutoTypeText, AutoType_String);
					
						free(AutoTypeText);
					}
#else
					GetDlgItemText(hwndDlg, IDC_EDIT_AUTOTYPE_TEXT,AutoType_String, sizeof(AutoType_String)/sizeof(AutoType_String[0]));
#endif
				}
				return EndDialog(hwndDlg, TRUE);
			
				case IDCANCEL:
				{

				}
				return EndDialog(hwndDlg, FALSE);
			}
		}
		break;
	}

	return FALSE;
}
/*----------------------------------------------------------------------------------------------*/
BOOL CALLBACK SpeedPropertySheet_DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
		}
		break;

		case WM_COMMAND:
		{

		}
		break;

	
		default:
			break;
	}

	return FALSE;
}

/*----------------------------------------------------------------------------------------------*/
BOOL CALLBACK DisplayPropertySheet_DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{

		}
		break;

		default:
			break;
	}

	return FALSE;
}
/*----------------------------------------------------------------------------------------------*/

void Options_PropertySheet(HWND hwndParent)
{
	PROPSHEETHEADER PropSheetHeader;
	PROPSHEETPAGE PropSheetPage;

	int nPages = 0;
	/* handles of the property pages */
	HPROPSHEETPAGE PropertyPages[2];
	
	memset(&PropSheetPage, 0,sizeof(PROPSHEETPAGE));
	PropSheetPage.dwSize = sizeof(PROPSHEETPAGE);
	PropSheetPage.pfnDlgProc = SpeedPropertySheet_DialogProc;
	PropSheetPage.pszTemplate = MAKEINTRESOURCE(IDD_DIALOG_SPEED);
	
	PropertyPages[nPages] = CreatePropertySheetPage(&PropSheetPage);
	nPages++;

	memset(&PropSheetPage, 0,sizeof(PROPSHEETPAGE));
	PropSheetPage.dwSize = sizeof(PROPSHEETPAGE);
	PropSheetPage.pfnDlgProc = DisplayPropertySheet_DialogProc;
	PropSheetPage.pszTemplate = MAKEINTRESOURCE(IDD_DIALOG_DISPLAY);
	
	PropertyPages[nPages] = CreatePropertySheetPage(&PropSheetPage);
	nPages++;

	memset(&PropSheetHeader,0,sizeof(PROPSHEETHEADER));
	PropSheetHeader.dwSize = sizeof(PROPSHEETHEADER);
	PropSheetHeader.dwFlags = PSH_NOAPPLYNOW;
	PropSheetHeader.hwndParent = hwndParent;
	PropSheetHeader.pszCaption = _T("Options");
	PropSheetHeader.nPages = nPages;
	PropSheetHeader.phpage = PropertyPages;
	PropertySheet(&PropSheetHeader);


}




/*----------------------------------------------------------------------------------------------*/


//////////////////////////////////////////////////////////////////////////////////////
WNDPROC PreviousMessageHandler;

LRESULT CALLBACK	Interface_MessageHandler(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

	switch (iMsg)
	{
		case WM_COMMAND:
		{

			WORD wID = LOWORD(wParam);

			if (RecentItems_Handle(wID))
				return 0;

			switch (wID)
			{				
				case ID_FILE_CASSETTE_CONTROLS:
				{
					CassetteControls(hwnd);
				}
				break;

				// drive A
				case ID_FILE_DRIVEA_INSERTDISK:
					Interface_InsertDisk(hwnd,0,FALSE);
					return 0;

				case ID_FILE_DISKDRIVEA_AUTOSTARTDISK:
				{
					Interface_InsertDisk(hwnd,0,TRUE);
				}
				return 0;


				case ID_FILE_DRIVEA_REMOVEDISK:
					Interface_RemoveDisk(hwnd,0);
					return 0;
				case ID_FILE_DRIVEA_TURNOVERDISK:
					FDD_TurnDisk(0);
					return 0;
				case ID_FILE_DRIVEA_INSERTNEWDISK_UNFORMATTED:
					Interface_InsertUnformattedDisk(hwnd,0);
					return 0;

				// drive B
				case ID_FILE_DRIVEB_INSERTDISK:
					Interface_InsertDisk(hwnd,1,FALSE);
					return 0;
				case ID_FILE_DRIVEB_REMOVEDISK:
					Interface_RemoveDisk(hwnd,1);
					return 0;
				case ID_FILE_DRIVEB_TURNOVERDISK:
					FDD_TurnDisk(1);
					return 0;
				case ID_FILE_DRIVEB_INSERTNEWDISK_UNFORMATTED:
					Interface_InsertUnformattedDisk(hwnd,0);
					return 0;

				// cartridge
				case ID_FILE_CARTRIDGE_INSERTCARTRIDGE:
					Interface_OpenCartridge(hwnd);
					return 0;

				case ID_CONTROL_REALTIME:
				{
					Host_LockSpeed = Interface_ToggleItemState(AppData.hAppMenu, ID_CONTROL_REALTIME);
				}
				break;

				case ID_FILE_CARTRIDGE_INSERTSYSTEMCARTRIDGE:
					{
						SetString(&CartridgeFilename, NULL);
						CPC_ReloadSystemCartridge();
					}
					return 0;

				case ID_HELP_ABOUT:
				{
					DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hwnd, About_DialogProc);
				}
				break;

				case ID_HELP_ARNOLDWEBSITE:
				{
					ShellExecute(hwnd, _T("open"), _T("http://arnold.emuunlim.com"),NULL,NULL,0);
				}
				break;

				case ID_TOOLS_AUTOTYPE:
				{
					AutoType_String[0]=_T('\0');

					/* show the modal dialog box for the auto-type feature */
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_AUTOTYPE), hwnd, AutoType_DialogProc)==TRUE)
					{
						if (strlen(AutoType_String))
						{
							AutoType_SetString(AutoType_String,FALSE);
						}
					}
				}
				break;

				case ID_TOOLS_POKEMEMORY:
				{
					/* show the modal dialog box for the poke memory feature */
					DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_POKEMEMORY), hwnd, PokeMemory_DialogProc);
				}
				break;
				
				case ID_FILE_SNAPSHOT_LOAD:
					Interface_LoadSnapshot(hwnd);
					return 0;
				case ID_FILE_SNAPSHOT_SAVE:
					SnapshotSave_Dialog(hwnd);
					return 0;


				case ID_FILE_WRITESCREENSNAPSHOT:
					Interface_SaveScreenSnapshot(hwnd);
					return 0;

				case ID_FILE_INSERTROM:
					{
						Interface_RomDialog(hwnd);
					return 0;
					}


				case ID_FILE_TAPEIMAGE_INSERT:
					{
						Interface_InsertTape(hwnd,FALSE);
					}
					return 0;
				case ID_FILE_TAPE_AUTOSTARTTAPE:
					{
						Interface_InsertTape(hwnd,TRUE);
					}
					return 0;
				case ID_FILE_TAPEIMAGE_REMOVE:
					{
						Tape_Remove();
					}
					return 0;
//				case ID_FILE_TAPEIMAGE_REWINDTOSTART:
//					{
//						TapeImage_RewindToStart();
//					}
//					return 0;

				case ID_FILE_EXIT:
				{
					SendMessage(hwnd, WM_CLOSE,0,0);

				}
				break;

				case ID_MISC_RESET:
					CPCEMU_DoReset(hwnd);
					return 0;

				case ID_MISC_PRINTEROUTPUT_DIGIBLASTER:
				{
					Printer_CloseOutputFile();

					PrinterOutputMethod = PRINTER_OUTPUT_TO_DIGIBLASTER;
				}
				return 0;

				case ID_MISC_PRINTEROUTPUT_FILE:
				{
					OPENFILENAME	DiskImageOpenFilename;
					TCHAR			FilesOfType[256];
					FilenameBuffer[0]=_T('\0');

					{
						TCHAR *pPtr;


						pPtr = FilesOfType;

						pPtr = AppendString(pPtr, "Text files");
						pPtr = AddExtensions(pPtr, TextFileExtensions);
						pPtr = AppendChar(pPtr,_T('\0'));
						pPtr = AppendString(pPtr, Messages[28]);
						pPtr = AddExtensions(pPtr, AllFileExtensions);
						pPtr = AppendChar(pPtr,_T('\0'));
						pPtr = AppendChar(pPtr,_T('\0'));
					}

					InitFileDlg(hwnd,&DiskImageOpenFilename,_T("dsk"),FilesOfType,OFN_EXPLORER|OFN_OVERWRITEPROMPT|OFN_CREATEPROMPT);
					if (GetSaveNameFromDlg(hwnd,&DiskImageOpenFilename,NULL,Messages[2],FilenameBuffer, ""))
					{
						if (_tcslen(FilenameBuffer)!=0)
						{
							Printer_CloseOutputFile();

							PrinterOutputMethod = PRINTER_OUTPUT_TO_FILE;
						
							Printer_OpenOutputFile(FilenameBuffer);
						}
					}
				}
				return 0;


				case ID_MISC_CHEATS_CHEATDATABASE:
				{
					CheatDatabaseDialog(hwnd);
				}
				break;

				case ID_MISC_RENDERINGACCURACY_LOW:
				{
					Render_SetRenderingAccuracyForWindowedMode(RENDERING_ACCURACY_LOW);

					CPCEMU_SetWindowed();
				}
				break;

				case ID_MISC_RENDERINGACCURACY_HIGH:
				{
					Render_SetRenderingAccuracyForWindowedMode(RENDERING_ACCURACY_HIGH);

					CPCEMU_SetWindowed();
				}
				break;

				case ID_MISC_RENDERINGACCURACY_HIGHER:
				{
					Render_SetRenderingAccuracyForWindowedMode(RENDERING_ACCURACY_HIGHER);

					CPCEMU_SetWindowed();
				}
				break;

				case ID_MISC_INICRONRAMROMSETTINGS:
				{
					Interface_RAMROM_Dialog(hwnd);
				}
				break;

				case ID_SETTINGS_OPTIONS:
				{
					Options_PropertySheet(hwnd);
				}
				break;


#ifdef AY_OUTPUT
				case ID_MISC_STSOUNDOUTPUT_STARTRECORDING:
					{
						Interface_OutputYM();	
					}
					break;

				case ID_MISC_STSOUNDOUTPUT_STOPRECORDING:
					{
//.						YMOutput_StopRecording();
					}
					break;

				case ID_MISC_STSOUNDOUTPUT_PROPERTIES:
					{
						YM5_PropertiesDialog(AppData.ApplicationHwnd);
					}
					break;
#endif

				case ID_MISC_RECORDSOUNDASWAV_STARTRECORDING:
					{
						Interface_OutputWav(hwnd);
					}
					break;


				case ID_SETTINGS_REGISTERFILETYPES:
				{
					Associations_Dialog(hwnd);
				}
				break;


				case ID_MISC_RECORDSOUNDASWAV_STOPRECORDING:
					{
						WavOutput_StopRecording();
					}
					break;

				case ID_VIEW_FULLSCREEN:
				{
					CPCEMU_SetFullScreen();
				}
				break;

//				case ID_MISC_LOCK_SPEED:
//				{
//					Host_LockSpeed = Interface_ToggleItemState(AppData.hAppMenu, ID_MISC_LOCK_SPEED);
//				}
//				break;
	
				case ID_MISC_MONITORTYPE_COLOUR:
					{
						CPC_SetMonitorType(CPC_MONITOR_COLOUR);
					}
					break;

				case ID_MISC_MONITORTYPE_GREENSCREEN:
					{
						CPC_SetMonitorType(CPC_MONITOR_GREEN_SCREEN);
					}
					break;

				case ID_MISC_MONITORTYPE_GREYSCALE:
					{
						CPC_SetMonitorType(CPC_MONITOR_GREY_SCALE);
					}
					break;

//				case ID_MISC_MONITORDISPLAYBRIGHTNESS_MAX:
//				{
//					CPC_SetMonitorBrightness(MONITOR_BRIGHTNESS_MAX);
//				}
//				break;
//
//				case ID_MISC_MONITORDISPLAYBRIGHTNESS_MIN:
//				{
//					CPC_SetMonitorBrightness(64);
//				}
//				break;
//
//				case ID_MISC_MONITORDISPLAYBRIGHTNESS_MIDDLE:
//				{
//					CPC_SetMonitorBrightness(128);
//				}
//				break;

				case ID_MISC_RAMCONFIG_128K:
				{
					BOOL State;
					unsigned long RamConfig;

					State = Interface_ToggleItemState(AppData.hAppMenu, ID_MISC_RAMCONFIG_128K);

					RamConfig = CPC_GetRamConfig();

					if (State)
					{
						RamConfig |= CPC_RAM_CONFIG_64K_RAM;
					}
					else
					{
						RamConfig &= ~CPC_RAM_CONFIG_64K_RAM;
					}

					CPC_SetRamConfig(RamConfig);

					Interface_UpdateRamConfig();
				}
				break;

				case ID_MISC_RAMCONFIG_256KRAMEXPANSION:
				{
					BOOL State;
					unsigned long RamConfig;

					State = Interface_ToggleItemState(AppData.hAppMenu, ID_MISC_RAMCONFIG_256KRAMEXPANSION);

					RamConfig = CPC_GetRamConfig();

					if (State)
					{
						RamConfig |= CPC_RAM_CONFIG_256K_RAM;
					}
					else
					{
						RamConfig &= ~CPC_RAM_CONFIG_256K_RAM;
					}

					CPC_SetRamConfig(RamConfig);

					Interface_UpdateRamConfig();
				}
				break;

				case ID_MISC_RAMCONFIG_256KSILICONDISK:
				{
					BOOL State;	
					unsigned long RamConfig;

					State = Interface_ToggleItemState(AppData.hAppMenu, ID_MISC_RAMCONFIG_256KSILICONDISK);

					RamConfig = CPC_GetRamConfig();

					if (State)
					{
						RamConfig |= CPC_RAM_CONFIG_256K_SILICON_DISK;
					}
					else
					{
						RamConfig &= ~CPC_RAM_CONFIG_256K_SILICON_DISK;
					}

					CPC_SetRamConfig(RamConfig);

					Interface_UpdateRamConfig();
				}
				break;
#ifdef MULTIFACE

				case ID_MISC_MULTIFACESTOP:
					{
					Multiface_Stop();
					}
					return 0;

				case ID_MISC_MULTIFACE_SETTINGS:
				{
					Interface_MultifaceDialog(hwnd);
				}
				break;
#endif

				case ID_SETTINGS_EMULATIONSETTINGS_JOYSTICKCONFIGURATION:
				{
					JoystickConfigurationDialog(hwnd);


				}
				break;


				case ID_CPCTYPE_CPC464:
				case ID_CPCTYPE_CPC664:
				case ID_CPCTYPE_CPC6128:
				case ID_CPCTYPE_464PLUS:
				case ID_CPCTYPE_6128PLUS:
				case ID_CPCTYPE_KCCOMPACT:
				{
					int CPCType;

					switch (LOWORD(wParam))
					{
						case ID_CPCTYPE_CPC464:
						{
							CPC_SetOSRom(CPC464_OperatingSystemRom_ENG.pData);
							CPC_SetBASICRom(CPC464_BASICRom_ENG.pData);
							Amstrad_DiscInterface_DeInstall();
							Amstrad_RamExpansion_DeInstall();
							CPC_SetHardware(CPC_HW_CPC);

//							CPCType = CPC_TYPE_CPC464;
						}
						break;

						case ID_CPCTYPE_CPC664:
						{
							CPC_SetOSRom(CPC664_OperatingSystemRom_ENG.pData);
							CPC_SetBASICRom(CPC664_BASICRom_ENG.pData);
							CPC_SetDOSRom(AMSDOSRom_ENG.pData);
							Amstrad_DiscInterface_Install();
							Amstrad_RamExpansion_DeInstall();

							CPC_SetHardware(CPC_HW_CPC);

							//							CPCType = CPC_TYPE_CPC664;
						}
						break;

						default:
						case ID_CPCTYPE_CPC6128:
						{
							CPC_SetOSRom(CPC6128_OperatingSystemRom_ENG.pData);
							CPC_SetBASICRom(CPC6128_BASICRom_ENG.pData);
							CPC_SetDOSRom(AMSDOSRom_ENG.pData);
							Amstrad_DiscInterface_Install();
							Amstrad_RamExpansion_Install();

							CPC_SetHardware(CPC_HW_CPC);

							//							CPCType = CPC_TYPE_CPC6128;
						}
						break;

						case ID_CPCTYPE_464PLUS:
						{
							CPC_SetHardware(CPC_HW_CPCPLUS);
							Amstrad_DiscInterface_DeInstall();
							Amstrad_RamExpansion_DeInstall();

							//							CPCType = CPC_TYPE_464PLUS;
						}
						break;

						case ID_CPCTYPE_6128PLUS:
						{
							CPC_SetHardware(CPC_HW_CPCPLUS);
							Amstrad_DiscInterface_Install();
							Amstrad_RamExpansion_Install();

							//							CPCType = CPC_TYPE_6128PLUS;
						}
						break;

						case ID_CPCTYPE_KCCOMPACT:
						{
							CPC_SetOSRom(KCC_OperatingSystemRom_ENG.pData);
							CPC_SetBASICRom(KCC_BASICRom_ENG.pData);
							Amstrad_DiscInterface_DeInstall();
							Amstrad_RamExpansion_DeInstall();

							CPC_SetHardware(CPC_HW_KCCOMPACT);

//							CPCType = CPC_TYPE_KCCOMPACT;
						}
						break;
					}

			//		CPC_SetCPCType(CPCType);
//					Interface_SetupCPCTypeMenu(CPCType);
					CPC_Reset();
				}
				return 0;


				case ID_CRTCTYPE_TYPE0:
				case ID_CRTCTYPE_TYPE1:
				case ID_CRTCTYPE_TYPE2:
				case ID_CRTCTYPE_TYPEASIC:
				case ID_CRTCTYPE_TYPEPREASIC:
				{
					int CRTCType;

					switch (LOWORD(wParam))
					{
						default:
						case ID_CRTCTYPE_TYPE0:
						{
							CRTCType = 0;
						}
						break;

						case ID_CRTCTYPE_TYPE1:
						{
							CRTCType = 1;
						}
						break;

						case ID_CRTCTYPE_TYPE2:
						{
							CRTCType = 2;
						}
						break;

						case ID_CRTCTYPE_TYPEASIC:
						{
							CRTCType = 3;
						}
						break;

						case ID_CRTCTYPE_TYPEPREASIC:
						{
							CRTCType = 4;
						}
						break;
					}
							
					CPC_SetCRTCType(CRTCType);
					Interface_SetupCRTCTypeMenu(CRTCType);
				}
				return 0;

				case ID_MISC_HARDWAREONJOYSTICKPORT_JOYSTICK:
				{
					CPC_SetHardwareConnectedToJoystickPort(JOYSTICK_HARDWARE_JOYSTICK);
				}
				break;

				case ID_MISC_HARDWAREONJOYSTICKPORT_AMXMOUSE:
				{
					CPC_SetHardwareConnectedToJoystickPort(JOYSTICK_HARDWARE_AMX_MOUSE);
				}
				break;

				case ID_MISC_HARDWAREONJOYSTICKPORT_WESTPHASER:
				{
					CPC_SetHardwareConnectedToJoystickPort(JOYSTICK_HARDWARE_WESTPHASER);
				}
				break;

				case 	ID_MISC_HARDWAREONJOYSTICKPORT_SPANISHLIGHTGUN:
				{
					CPC_SetHardwareConnectedToJoystickPort(JOYSTICK_HARDWARE_SPANISH_LIGHTGUN);
				}
				break;



				case ID_DEBUG_DEBUGDISPLAY:
					{
						CPCEmulation_EnableDebugger(TRUE);
						Interface_OpenDebugDisplay(hwnd,TRUE);
					}
					return 0;


				default:
					break;
			//	}
			}
		}
		break;
		
		default:
			break;
	}

	return CallWindowProc(PreviousMessageHandler,hwnd,iMsg, wParam,lParam);
}


/*=====================================================================================*/

/********************************************************************************
 KEYBOARD 
 ********************************************************************************/
void ToggleFullScreen(void)
{
	if (AppData.Windowed)
	{
		CPCEMU_SetFullScreen();
	}
	else
	{
		CPCEMU_SetWindowed();
	}
}

 
typedef struct
{
	int PCKeyID;
	int ActionData;
	int Action;
} KeyboardRemap;



/*=====================================================================================*/

#include "joy.h"

/* configurations for the joysticks */
JoystickConfigData joystickConfigs[3];



void	Joystick_InitDefaultSettings(void)
{
	joystickConfigs[0].Active = FALSE;
	joystickConfigs[0].CPCJoystickID = CPC_DIGITAL_JOYSTICK0;
	joystickConfigs[0].Type = 0;	//SIMULATED_BY_KEYBOARD;
	joystickConfigs[0].realJoystickData.id = 0;
	joystickConfigs[0].simulatedJoystickData.Left = DIK_NUMPAD4;
	joystickConfigs[0].simulatedJoystickData.Right = DIK_NUMPAD6;
	joystickConfigs[0].simulatedJoystickData.Up = DIK_NUMPAD8;
	joystickConfigs[0].simulatedJoystickData.Down = DIK_NUMPAD2;
	joystickConfigs[0].simulatedJoystickData.Fire1 = DIK_NUMPAD5;
	joystickConfigs[0].simulatedJoystickData.Fire2 = DIK_NUMPAD0;

	joystickConfigs[1].Active = FALSE;
	joystickConfigs[1].CPCJoystickID = CPC_DIGITAL_JOYSTICK1;
	joystickConfigs[1].Type = JOYSTICK_REAL;
	joystickConfigs[1].realJoystickData.id = 4;
	joystickConfigs[1].simulatedJoystickData.Left = DIK_LEFT;
	joystickConfigs[1].simulatedJoystickData.Right = DIK_RIGHT;
	joystickConfigs[1].simulatedJoystickData.Up = DIK_UP;
	joystickConfigs[1].simulatedJoystickData.Down = DIK_DOWN;
	joystickConfigs[1].simulatedJoystickData.Fire1 = DIK_RCONTROL;
	joystickConfigs[1].simulatedJoystickData.Fire2 = -1;
	joystickConfigs[2].Active = FALSE;
}

typedef struct
{
	/* controller e.g. joystick/joypad */
	int ControllerID;
	int ControllerType;
	int ControllerElement;
	int KeyboardCode;
} Controller2KeyboardRemap;

/* joystick mapped to key presses */
/* keys used to simulate a joystick */


/* cpc joystick 0 */
/* physical controller; map controller buttons to CPC joystick  */
/* simulated by keyboard; define keys + key to enable/disable  */

/* keyboard */

int settingsdone = 0;

void	HandleJoysticks(void)
{
	int i;

	for (i=0; i<2; i++)
	{
		JoystickData joyData;

		joyData.x = 0;
		joyData.y = 0;
		joyData.buttons = 0;

		/* active? */
		if (joystickConfigs[i].Active)
		{
			switch (joystickConfigs[i].Type)
			{
				case JOYSTICK_REAL:
				{
					JoyInfo joyInfo;

					/* read the real joystick */
					Joystick_Read(joystickConfigs[i].realJoystickData.id, &joyInfo);

					joyData.x = joyInfo.x;
					joyData.y = joyInfo.y;
					joyData.buttons = joyInfo.buttons;
				}
				break;

				/* keyboard simulating a joystick */
				case JOYSTICK_SIMULATED_BY_KEYBOARD:
				{
					/* key pressed? */
				
					if (joystickConfigs[i].simulatedJoystickData.Left!=-1)
					{
						if (IsKeyPressed(joystickConfigs[i].simulatedJoystickData.Left))
						{
							joyData.x = JOYSTICK_DATA_AXIS_MIN;
						}
					}

					/* key pressed? */
					if (joystickConfigs[i].simulatedJoystickData.Right!=-1)
					{
						if (IsKeyPressed(joystickConfigs[i].simulatedJoystickData.Right))
						{
							joyData.x = JOYSTICK_DATA_AXIS_MAX;
						}
					}

					/* key pressed? */
					if (joystickConfigs[i].simulatedJoystickData.Up!=-1)
					{
						if (IsKeyPressed(joystickConfigs[i].simulatedJoystickData.Up))
						{
							joyData.y = JOYSTICK_DATA_AXIS_MIN;
						}
					}
					
					/* key pressed? */
					if (joystickConfigs[i].simulatedJoystickData.Down!=-1)
					{
						if (IsKeyPressed(joystickConfigs[i].simulatedJoystickData.Down))
						{
							joyData.y = JOYSTICK_DATA_AXIS_MAX;
						}
					}

					/* key pressed? */
					if (joystickConfigs[i].simulatedJoystickData.Fire1!=-1)
					{
						if (IsKeyPressed(joystickConfigs[i].simulatedJoystickData.Fire1))
						{
							joyData.buttons |= (1<<0);
						}
					}

					/* key pressed? */
					if (joystickConfigs[i].simulatedJoystickData.Fire2!=-1)
					{
						if (IsKeyPressed(joystickConfigs[i].simulatedJoystickData.Fire2))
						{
							joyData.buttons |= (1<<1);
						}
					}
				}
				break;
			}
		}
		else
		{


		}

		/* joyData is setup with information. It contains analogue information
		which has been processed and remapped from original control system (i.e. real or simulated) */
		switch (joystickConfigs[i].CPCJoystickID)
		{
			case CPC_DIGITAL_JOYSTICK0:
			{
				/* CPC digital joystick 0; convert data to digital */

				if (joyData.x<0)
				{
					CPC_SetKey(CPC_JOY0_LEFT);
				}
				else if (joyData.x>0)
				{
					CPC_SetKey(CPC_JOY0_RIGHT);
				}

				if (joyData.y<0)
				{
					CPC_SetKey(CPC_JOY0_UP);
				}
				else if (joyData.y>0)
				{
					CPC_SetKey(CPC_JOY0_DOWN);
				}

				if (joyData.buttons & (1<<0))
				{
					CPC_SetKey(CPC_JOY0_FIRE1);
				}

				if (joyData.buttons & (1<<1))
				{
					CPC_SetKey(CPC_JOY0_FIRE2);
				}
			}
			break;

			case CPC_DIGITAL_JOYSTICK1:
			{
				/* CPC digital joystick 1; convert to digital */
				if (joyData.x<0)
				{
					CPC_SetKey(CPC_JOY1_LEFT);
				}
				else if (joyData.x>0)
				{
					CPC_SetKey(CPC_JOY1_RIGHT);
				}

				if (joyData.y<0)
				{
					CPC_SetKey(CPC_JOY1_UP);
				}
				else if (joyData.y>0)
				{
					CPC_SetKey(CPC_JOY1_DOWN);
				}

				if (joyData.buttons & (1<<0))
				{
					CPC_SetKey(CPC_JOY1_FIRE1);
				}

				if (joyData.buttons & (1<<1))
				{
					CPC_SetKey(CPC_JOY1_FIRE2);
				}
			}
			break;

			case CPC_ANALOGUE_JOYSTICK:
			{
				/* todo! */


			}
			break;
		}
	}
}

/*=====================================================================================*/

const TCHAR *ActionNames[]=
{
	_T("None"),
	_T("Set CPC Key"),
	_T("Reset CPC"),
	_T("Toggle Fullscreen/Windowed"),
	_T("Insert Disk Image into drive A"),
	_T("Insert Disk Image into drive B"),
	_T("Insert Cartridge"),
	_T("Load Snapshot"),
	_T("Save Snapshot"),
	_T("Insert Tape"),
};

enum
{
	ACTION_CPC_NONE,
	ACTION_SET_CPC_KEY,
	ACTION_CPC_RESET,
	ACTION_TOGGLE_FULLSCREEN,
	ACTION_INSERT_DISK_DRIVE_A,
	ACTION_INSERT_DISK_DRIVE_B,
	ACTION_INSERT_CARTRIDGE,
	ACTION_LOAD_SNAPSHOT,
	ACTION_SAVE_SNAPSHOT,
	ACTION_INSERT_TAPE,
	ACTION_SAVE_SCREENSHOT,
	ACTION_QUIT,
};


KeyboardRemap keyMap[]=
{
	{
		DIK_RCONTROL,
		CPC_KEY_CONTROL,
		ACTION_SET_CPC_KEY
	},
	{
		DIK_NEXT,
		CPC_KEY_COPY,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_NUMPADENTER,
		CPC_KEY_SMALL_ENTER,
		ACTION_SET_CPC_KEY
	},
	{
		DIK_UP,
		CPC_KEY_CURSOR_UP,
		ACTION_SET_CPC_KEY
	},
	{
		DIK_DOWN,
		CPC_KEY_CURSOR_DOWN,
		ACTION_SET_CPC_KEY
	},
	{
		DIK_LEFT,
		CPC_KEY_CURSOR_LEFT,
		ACTION_SET_CPC_KEY
	},
	{
		DIK_RIGHT,
		CPC_KEY_CURSOR_RIGHT,
		ACTION_SET_CPC_KEY
	},
	{
		DIK_ESCAPE,
		CPC_KEY_ESC,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_1,
		CPC_KEY_1,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_2,
		CPC_KEY_2,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_3,
		CPC_KEY_3,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_4,
		CPC_KEY_4,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_5,
		CPC_KEY_5,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_6,
		CPC_KEY_6,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_7,
		CPC_KEY_7,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_8,
		CPC_KEY_8,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_9,
		CPC_KEY_9,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_0,
		CPC_KEY_ZERO,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_MINUS,
		CPC_KEY_MINUS,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_EQUALS,
		CPC_KEY_HAT,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_BACK,
		CPC_KEY_DEL,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_Q,
		CPC_KEY_Q,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_W,
		CPC_KEY_W,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_E,
		CPC_KEY_E,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_R,
		CPC_KEY_R,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_T,
		CPC_KEY_T,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_Y,
		CPC_KEY_Y,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_U,
		CPC_KEY_U,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_I,
		CPC_KEY_I,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_O,
		CPC_KEY_O,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_P,
		CPC_KEY_P,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_A,
		CPC_KEY_A,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_S,
		CPC_KEY_S,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_D,
		CPC_KEY_D,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_F,
		CPC_KEY_F,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_G,
		CPC_KEY_G,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_H,
		CPC_KEY_H,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_J,
		CPC_KEY_J,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_K,
		CPC_KEY_K,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_L,
		CPC_KEY_L,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_Z,
		CPC_KEY_Z,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_X,
		CPC_KEY_X,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_C,
		CPC_KEY_C,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_V,
		CPC_KEY_V,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_B,
		CPC_KEY_B,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_N,
		CPC_KEY_N,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_M,
		CPC_KEY_M,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_NUMPAD9,
		CPC_KEY_F9,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_NUMPAD8,
		CPC_KEY_F8,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_NUMPAD7,
		CPC_KEY_F7,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_NUMPAD6,
		CPC_KEY_F6,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_NUMPAD5,
		CPC_KEY_F5,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_NUMPAD4,
		CPC_KEY_F4,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_NUMPAD3,
		CPC_KEY_F3,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_NUMPAD2,
		CPC_KEY_F2,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_NUMPAD1,
		CPC_KEY_F1,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_NUMPAD0,
		CPC_KEY_F0,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_DECIMAL,
		CPC_KEY_FDOT,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_SPACE,
		CPC_KEY_SPACE,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_LSHIFT,
		CPC_KEY_SHIFT,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_RSHIFT,
		CPC_KEY_SHIFT,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_PERIOD,
		CPC_KEY_DOT,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_COMMA,
		CPC_KEY_COMMA,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_RETURN,
		CPC_KEY_RETURN,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_LCONTROL,
		CPC_KEY_CONTROL,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_SEMICOLON,
		CPC_KEY_COLON,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_BACKSLASH,
		CPC_KEY_BACKSLASH,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_SLASH,
		CPC_KEY_FORWARD_SLASH,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_CAPITAL,
		CPC_KEY_CAPS_LOCK,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_APOSTROPHE,
		CPC_KEY_SEMICOLON,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_LBRACKET,
		CPC_KEY_AT,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_RBRACKET,
		CPC_KEY_OPEN_SQUARE_BRACKET,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_GRAVE,
		CPC_KEY_CLOSE_SQUARE_BRACKET,
		ACTION_SET_CPC_KEY,
	},
	{
		DIK_LBRACKET,
		CPC_KEY_AT,
		ACTION_SET_CPC_KEY,
	},
};

int keyMapSize = sizeof(keyMap)/sizeof(keyMap[0]);



/* modifiers */
#define KEYBOARD_SHORTCUT_MODIFIER_CTRL 0x01
#define KEYBOARD_SHORTCUT_MODIFIER_ALT 0x02
#define KEYBOARD_SHORTCUT_MODIFIER_SHIFT 0x04

typedef struct 
{
	unsigned long nModifiers;
	unsigned long nKey;
	unsigned long nAction;
} KEYBOARD_SHORTCUT;

/* not const because they can be modified by user! */
KEYBOARD_SHORTCUT Shortcuts[]=
{
	{ 
		0, DIK_F2, ACTION_LOAD_SNAPSHOT
	},
	{ 
		0, DIK_F3, ACTION_INSERT_TAPE
	},
	{
		0, DIK_F4, ACTION_SAVE_SNAPSHOT
	},
	{
		0, DIK_F5, ACTION_CPC_RESET
	},
	{
		0, DIK_F6, ACTION_INSERT_DISK_DRIVE_A,
	},
	{
		0, DIK_F7, ACTION_INSERT_DISK_DRIVE_B,
	},
	{
		0, DIK_F10, ACTION_QUIT
	},
	/* TROELS K shortcut */
	{
		0, DIK_SYSRQ, ACTION_SAVE_SCREENSHOT
	},
};

void	HandleKeyboard(void)
{
	int i;
	unsigned long nModifiers = 0;
	KEYBOARD_SHORTCUT *pShortCut;

	/* get modifiers */
	if (IsKeyPressed(DIK_LCONTROL) || IsKeyPressed(DIK_RCONTROL))
		nModifiers |= KEYBOARD_SHORTCUT_MODIFIER_CTRL;

	if (IsKeyPressed(DIK_LALT) || IsKeyPressed(DIK_RALT))
		nModifiers |= KEYBOARD_SHORTCUT_MODIFIER_ALT;
	
	if (IsKeyPressed(DIK_LSHIFT) || IsKeyPressed(DIK_RSHIFT))
		nModifiers |= KEYBOARD_SHORTCUT_MODIFIER_SHIFT;

	pShortCut = Shortcuts;

	for (i=0; i<sizeof(Shortcuts)/sizeof(Shortcuts[0]); i++)
	{
		if (
			/* check modifiers */
			((pShortCut->nModifiers & nModifiers)==nModifiers) &&
			IsKeyPressed(pShortCut->nKey)
			)
		{
			switch (pShortCut->nAction)
			{
				/* perform action */
				case ACTION_CPC_RESET:
				{
					CPCEMU_DoReset(AppData.ApplicationHwnd);
				}
				break;

				case ACTION_LOAD_SNAPSHOT:
				{
					Interface_LoadSnapshot(AppData.ApplicationHwnd);
				}
				break;

				case ACTION_SAVE_SNAPSHOT:
				{
					SnapshotSave_Dialog(AppData.ApplicationHwnd);
				}
				break;

				case ACTION_INSERT_CARTRIDGE:
				{
					Interface_OpenCartridge(AppData.ApplicationHwnd);
				}
				break;

				case ACTION_INSERT_TAPE:
				{
					Interface_InsertTape(AppData.ApplicationHwnd,FALSE);
				}
				break;

				case ACTION_TOGGLE_FULLSCREEN:
				{
					ToggleFullScreen();
				}
				break;

				case ACTION_INSERT_DISK_DRIVE_A:
				{
					Interface_InsertDisk(AppData.ApplicationHwnd,0,FALSE);
				}
				break;

				case ACTION_INSERT_DISK_DRIVE_B:
				{
					Interface_InsertDisk(AppData.ApplicationHwnd,1,FALSE);
				}
				break;

				case ACTION_SAVE_SCREENSHOT:
				{
					Interface_SaveScreenSnapshot();
				}
				break;

				case ACTION_QUIT:
				{
					SendMessage(AppData.ApplicationHwnd, WM_CLOSE,0,0);
				}
				break;
			}
		}

		pShortCut++;
	}
	
	for (i=0; i<keyMapSize; i++)
	{
		if (IsKeyPressed(keyMap[i].PCKeyID))
		{
			switch (keyMap[i].Action)
			{
				case ACTION_SET_CPC_KEY:
				{
					CPC_SetKey(keyMap[i].ActionData);
				}
				break;
			}
		}
	}
}

/*=====================================================================================*/


/*=====================================================================================*/


////////////////////////////////

#include "../cpc/magnum.h"
#include "../cpc/gunstick.h"
#include "../cpc/amxms.h"
#include "../cpc/kempston.h"



void HandleMouse(void)
{
	AppData.MouseDeltaX = AppData.xPos - AppData.MousePosX;
	AppData.MouseDeltaY = AppData.yPos - AppData.MousePosY;
	AppData.MousePosX = AppData.xPos;
	AppData.MousePosY = AppData.yPos;

	AppData.LeftPressed = ((AppData.Buttons & MK_LBUTTON)!=0);
	AppData.RightPressed = ((AppData.Buttons & MK_RBUTTON)!=0);
	
	KempstonMouse_Update(AppData.MouseDeltaX, AppData.MouseDeltaY, AppData.LeftPressed, AppData.RightPressed);


	{
		AmxMouse_Update(AppData.MouseDeltaX, AppData.MouseDeltaY, AppData.LeftPressed, AppData.RightPressed);
	}

//	if (LeftPressed)
//	{
//		CRTC_LightPen_Trigger(MousePosX, MousePosY);
//	}

	SpanishLightGun_Update(AppData.MousePosX, AppData.MousePosY, AppData.LeftPressed);

	Magnum_Update(AppData.MousePosX, AppData.MousePosY, AppData.LeftPressed);
}





void	DoKeyboard()
{
	CPC_ClearKeyboard();

	// TEMP

	if (!settingsdone)
	{
		settingsdone = 1;
		Joystick_InitDefaultSettings();
	}


	HandleKeyboard();

	HandleMouse();

	HandleJoysticks();

}

/*------------------------------------------- */

//DWORD	startTime, endTime, timeElapsed;
//float	ScrTime;
//float	PercentSpeed;


void	CPCEMU_SetWindowed()
{
	int ExStyle, Style;

	DD_ReleaseSurfacesAndRestoreVideoMode();

//	if (Windowed == FALSE)
//	{
//		DD_FullScreenToWindow();
//	}
//	else
//	{
//		DD_ShutdownWindowed();
//	}

	// enable some styles
	Style = GetWindowLong(AppData.ApplicationHwnd, GWL_STYLE);

	Style |= WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
//	Style &= WS_MAXIMIZEBOX;
	SetWindowLong(AppData.ApplicationHwnd, GWL_STYLE, Style);

	// remove ex-styles
	ExStyle = GetWindowLong(AppData.ApplicationHwnd, GWL_EXSTYLE);

	ExStyle &= ~WS_EX_TOPMOST;

	
	SetWindowLong(AppData.ApplicationHwnd, GWL_EXSTYLE, ExStyle);

 // winnt crashes here
	if (AppData.hAppMenu)
	{
		SetMenu(AppData.ApplicationHwnd, NULL);
		SetMenu(AppData.ApplicationHwnd,AppData.hAppMenu);
	
		RecentItems_RefreshMenu(RECENT_LIST_FILES, AppData.ApplicationHwnd);
	}


	AppData.Windowed = TRUE;
	
	Render_SetDisplayWindowed();

	ShowCursor(TRUE);
}


void	CPCEMU_SetFullScreen()
{
	DD_ReleaseSurfacesAndRestoreVideoMode();
//	if (Windowed == FALSE)
//	{
//		DD_FullScreenToWindow();
//	}
//	else
//	{
//		DD_ShutdownWindowed();
//	}

	Interface_OpenDebugDisplay(AppData.ApplicationHwnd,FALSE);

	{
		// remove some styles
		int Style = GetWindowLong(AppData.ApplicationHwnd, GWL_STYLE);

		Style &= ~(WS_BORDER | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU);

		SetWindowLong(AppData.ApplicationHwnd, GWL_STYLE, Style);

		// remove current menu
		SetMenu(AppData.ApplicationHwnd, NULL);

//		if (Windowed==TRUE)
//		{
//			DD_FromWindowed();
//		}

		Render_SetDisplayFullScreen(AppData.FullScreenWidth, AppData.FullScreenHeight, AppData.FullScreenDepth);

		ShowCursor(FALSE);

		AppData.Windowed = FALSE;
	}
}



#if 0
//static int timerID = 0;
static BOOL AudioEnabled = FALSE;

void	AudioEnable(BOOL State)
{
	if (State)
	{
		// is direct sound active ? 
		if (Host_AudioPlaybackPossible())
		{
			timerID = timeSetEvent(1000/50, 2, MyTimerProc,0, TIME_PERIODIC);

			if (timerID!=0)
			{
				AudioEnabled = TRUE;
	
		
			}
		}
	}
	else
	{
		 //is timer active? 
		if (timerID!=0)
		{
			//kill event 
			timeKillEvent(timerID);

			timerID = 0;
		
			AudioEnabled = FALSE;
		}
	}
}
#endif

void	CPCEMU_SetupPalette()
{
    /* initialise palette for Windows. Mark some entries as un-matchable and used */
    int i;

    for (i=0; i<10; i++)
    {
        Render_MarkPaletteEntryForHostUse(i);
		
		Render_MarkPaletteEntryForHostUse(i+246);
	}
}
	

	
int	CPCEMU_Initialise(void)
{

	HINSTANCE	hInstance;

	// for windows ce this must not be NULL!
	HMODULE hModule = NULL;

	AutoType_Init();

	PreviousMessageHandler = (WNDPROC)GetWindowLong(AppData.ApplicationHwnd, GWL_WNDPROC);

	SetWindowLong(AppData.ApplicationHwnd, GWL_WNDPROC,(LONG)Interface_MessageHandler);

	hInstance = (HINSTANCE)GetWindowLong(AppData.ApplicationHwnd,GWL_HINSTANCE);
		
	AppData.hAppMenu = LoadMenu(hInstance,MAKEINTRESOURCE(IDR_MENU1));


	/* get system cartridge from resources */
	GetDataFromCustomResource(hModule, &CPCPLUS_SystemCartridge_ENG.pData, &CPCPLUS_SystemCartridge_ENG.nLength, MAKEINTRESOURCE(CPCPLUS_SYSTEM));

	/* get cpc464 roms from resources */
	GetDataFromCustomResource(hModule, &CPC464_OperatingSystemRom_ENG.pData, &CPC464_OperatingSystemRom_ENG.nLength, MAKEINTRESOURCE(CPC464E_OS));
	GetDataFromCustomResource(hModule, &CPC464_BASICRom_ENG.pData, &CPC464_BASICRom_ENG.nLength, MAKEINTRESOURCE(CPC464E_BASIC));

	/* get cpc664 roms from resources */
	GetDataFromCustomResource(hModule, &CPC664_OperatingSystemRom_ENG.pData, &CPC664_OperatingSystemRom_ENG.nLength, MAKEINTRESOURCE(CPC664E_OS));
	GetDataFromCustomResource(hModule, &CPC664_BASICRom_ENG.pData, &CPC664_BASICRom_ENG.nLength, MAKEINTRESOURCE(CPC664E_BASIC));
	
	/* get cpc6128 roms from resources */
	GetDataFromCustomResource(hModule, &CPC6128_OperatingSystemRom_ENG.pData, &CPC6128_OperatingSystemRom_ENG.nLength, MAKEINTRESOURCE(CPC6128E_OS));
	GetDataFromCustomResource(hModule, &CPC6128_BASICRom_ENG.pData, &CPC6128_BASICRom_ENG.nLength, MAKEINTRESOURCE(CPC6128E_BASIC));

	/* get kcc roms from resources */
	GetDataFromCustomResource(hModule, &KCC_OperatingSystemRom_ENG.pData, &KCC_OperatingSystemRom_ENG.nLength, MAKEINTRESOURCE(KCC_KCCOS));
	GetDataFromCustomResource(hModule, &KCC_BASICRom_ENG.pData, &KCC_BASICRom_ENG.nLength, MAKEINTRESOURCE(KCC_KCCBAS));

	/* get amsdos rom from resources */
	GetDataFromCustomResource(hModule, &AMSDOSRom_ENG.pData, &AMSDOSRom_ENG.nLength, MAKEINTRESOURCE(AMSDOSE_AMSDOS));


	/* initialise cpc hardware */
	CPC_Initialise();

	Multiface_Install();

	/* insert the cartridge */
	Cartridge_Insert(CPCPLUS_SystemCartridge_ENG.pData, CPCPLUS_SystemCartridge_ENG.nLength);


	Debug_SetDebuggerRefreshCallback(Debugger_UpdateDisplay);
	

	Debugger_Initialise(AppData.ApplicationHwnd);

	GenericInterface_Initialise();

	/* store state of Scroll Lock LED */
	ScrollLock_StoreState();
	
	CPCEMU_SetupPalette();

	Debug_Init();

	/* disable debugger */
	CPCEmulation_EnableDebugger(FALSE);
	
	/* load cpc 464 config */
/*	Config_LoadConfiguration("cpc464"); */
	/* load cpc 664 config */
/*	Config_LoadConfiguration("cpc664"); */
	/* load cpc 6128 config */
/*	Config_LoadConfiguration("cpc6128"); */
	/* load cpc 464+ config */
/*	Config_LoadConfiguration("464+"); */
	/* load cpc 6128+ config */
/*	Config_LoadConfiguration("6128+"); */
	/* load kc compact config */
/*	Config_LoadConfiguration("kc-compact"); */


	/* set acknowledge interrupt callback function */
/*		Z80_SetUserAckInterruptFunction(CPC_AcknowledgeInterrupt); */

	/* CPC initialised, so set default setup */
	CPCEmulation_InitialiseDefaultSetup();


//	if (CPCEmulation_Initialise())
	{
		CPCEMU_SetWindowed();

		GenericInterface_RestoreSettings();

		CPC_SetOSRom(CPC6128_OperatingSystemRom_ENG.pData);
		CPC_SetBASICRom(CPC6128_BASICRom_ENG.pData);
		CPC_SetHardware(CPC_HW_CPC);
		Amstrad_DiscInterface_Install();
		Amstrad_RamExpansion_Install();
		CPC_Reset();

		{
			Interface_SetItemCheckState(AppData.hAppMenu, ID_CONTROL_REALTIME, Host_LockSpeed);
		}

		Interface_SetupCRTCTypeMenu(CPC_GetCRTCType());
//		Interface_SetupCPCTypeMenu(CPC_GetCPCType());

		CPC_SetAudioActive(TRUE);
	}

	return 1;
}

extern BOOL bWin;

void	CPCEMU_Run(void)
{
		/* run emulation */
		CPCEmulation_Run(bWin);
}

void	CPCEMU_Finish(void)
{

	/* store settings */
	GenericInterface_StoreSettings();

		Multiface_DeInstall();

		//AudioEnable(FALSE);
		//CPC_SetAudioActive(FALSE);

		/* close CPC emulation */
	CPC_Finish();

	Debug_Finish();

	/* not necessary to free resources */

	/* close generic interface */
	GenericInterface_Finish();
	
	/* restore state of scroll lock LED */
	ScrollLock_RestoreState();

	Interface_FreeCheatDatabase();

	return 1;
}




	
const TCHAR *DBF_Extensions[]=
{
	_T("dbf"),
	NULL
};

// Insert a unformatted disk image into an Amstrad drive
BOOL	Interface_OpenCheatDatabase(void)
{
	OPENFILENAME	CheatDatabaseOpenFilename;
	HWND			hwnd = GetActiveWindow();
	char			FilesOfType[256];

	{
		char *pPtr;


		pPtr = FilesOfType;

		pPtr = AppendString(pPtr, Messages[71]);
		pPtr = AddExtensions(pPtr, DBF_Extensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendString(pPtr, Messages[28]);
		pPtr = AddExtensions(pPtr, AllFileExtensions);
		pPtr = AppendChar(pPtr,_T('\0'));
		pPtr = AppendChar(pPtr,_T('\0'));
	}


	InitFileDlg(hwnd,&CheatDatabaseOpenFilename,_T("dbf"),FilesOfType,0);
	if (GetFileNameFromDlg(hwnd,&CheatDatabaseOpenFilename,NULL,Messages[24],FilenameBuffer, ""))
	{
		if (_tcslen(FilenameBuffer)!=0)
		{
			TCHAR *pDatabase;
			unsigned long DatabaseLength;

			// free previous cheat database
			Interface_FreeCheatDatabase();

			LoadFile(FilenameBuffer, &pDatabase, &DatabaseLength);

			if (pDatabase!=NULL)
			{
				// open new cheat-database
				pCheatDatabase = CheatDatabase_Parse(pDatabase, DatabaseLength);

				free(pDatabase);
			}
//			InitialiseListView();

		}

	}

	return TRUE;
}


#ifndef _countof
   #define _countof(array) (sizeof(array)/sizeof((array)[0]))
#endif

void CPCEMU_SetKey_VK(int vk, BOOL bKeyDown, BOOL bNumPad)
{
   static const struct _MAP
   {
      int        vk;
      CPC_KEY_ID cpc;
      CPC_KEY_ID cpc_add;
   } aMapMain[]=
   {
	   { VK_ESCAPE , CPC_KEY_ESC  , CPC_KEY_NULL } , /* 0 */
	   
      { '1'       , CPC_KEY_1    , CPC_KEY_NULL } ,
	   { '!'       , CPC_KEY_1    , CPC_KEY_SHIFT} ,
	   
      { '2'       , CPC_KEY_2    , CPC_KEY_NULL } ,
      { '"'       , CPC_KEY_2    , CPC_KEY_SHIFT} ,
	   
      { '3'       , CPC_KEY_3    , CPC_KEY_NULL } ,
	   { '#'       , CPC_KEY_3    , CPC_KEY_SHIFT} ,
	   
      { '4'       , CPC_KEY_4    , CPC_KEY_NULL } ,
	   { '$'       , CPC_KEY_4    , CPC_KEY_SHIFT} ,
	   
      { '5'       , CPC_KEY_5    , CPC_KEY_NULL } ,
      { '%'       , CPC_KEY_5    , CPC_KEY_SHIFT} , /* 10 */
	   
      { '6'       , CPC_KEY_6    , CPC_KEY_NULL } ,
      { '&'       , CPC_KEY_6    , CPC_KEY_SHIFT} ,
	   
      { '7'       , CPC_KEY_7    , CPC_KEY_NULL } ,
      { '\''      , CPC_KEY_7    , CPC_KEY_SHIFT} ,
	   
      { '8'       , CPC_KEY_8    , CPC_KEY_NULL } ,
      { '('       , CPC_KEY_8    , CPC_KEY_SHIFT} ,
	   
      { '9'       , CPC_KEY_9    , CPC_KEY_NULL } ,
	   { ')'       , CPC_KEY_9    , CPC_KEY_SHIFT} ,
	   
      { '0'       , CPC_KEY_ZERO , CPC_KEY_NULL } ,
	   { '_'       , CPC_KEY_ZERO , CPC_KEY_SHIFT} , /* 20 */
	   
      { '-'       , CPC_KEY_MINUS , CPC_KEY_NULL } ,
      { '=_T('       , CPC_KEY_MINUS , CPC_KEY_SHIFT} ,
	   
      { '^'       , CPC_KEY_HAT  , CPC_KEY_NULL } ,
      { '£'       , CPC_KEY_HAT  , CPC_KEY_SHIFT} ,

	   { VK_BACK   , CPC_KEY_DEL  , CPC_KEY_NULL } ,
	   { VK_TAB    , CPC_KEY_TAB  , CPC_KEY_NULL } ,

      { 'q'       , CPC_KEY_Q    , CPC_KEY_NULL } ,
      { 'Q'       , CPC_KEY_Q    , CPC_KEY_SHIFT} ,

	   { 'w'       , CPC_KEY_W    , CPC_KEY_NULL } ,
	   { 'W'       , CPC_KEY_W    , CPC_KEY_SHIFT} , /* 30 */

      { 'e'       , CPC_KEY_E    , CPC_KEY_NULL } ,
      { 'E'       , CPC_KEY_E    , CPC_KEY_SHIFT} ,

      { 'r'       , CPC_KEY_R    , CPC_KEY_NULL } ,
      { 'R'       , CPC_KEY_R    , CPC_KEY_SHIFT} ,

      { 't'       , CPC_KEY_T    , CPC_KEY_NULL } ,
      { 'T'       , CPC_KEY_T    , CPC_KEY_SHIFT} ,

      { 'y'       , CPC_KEY_Y    , CPC_KEY_NULL } ,
      { 'Y'       , CPC_KEY_Y    , CPC_KEY_SHIFT} ,

      { 'u'       , CPC_KEY_U    , CPC_KEY_NULL } ,
      { 'U'       , CPC_KEY_U    , CPC_KEY_SHIFT} , /* 40 */

      { 'i'       , CPC_KEY_I    , CPC_KEY_NULL } ,
      { 'I'       , CPC_KEY_I    , CPC_KEY_SHIFT} ,

      { 'o'       , CPC_KEY_O    , CPC_KEY_NULL } ,
      { 'O'       , CPC_KEY_O    , CPC_KEY_SHIFT} ,

      { 'p'       , CPC_KEY_P    , CPC_KEY_NULL } ,
      { 'P'       , CPC_KEY_P    , CPC_KEY_SHIFT} ,
	   
      { '@'       , CPC_KEY_AT   , CPC_KEY_NULL } ,
	   { '|'       , CPC_KEY_AT   , CPC_KEY_SHIFT } ,
	   
      { '['       , CPC_KEY_OPEN_SQUARE_BRACKET , CPC_KEY_NULL } ,
      { '{'       , CPC_KEY_OPEN_SQUARE_BRACKET , CPC_KEY_SHIFT} , /* 50 */
	   
      { VK_RETURN , CPC_KEY_RETURN , CPC_KEY_NULL } ,
      { '\n', CPC_KEY_RETURN , CPC_KEY_NULL } ,
	   
      { 'a'       , CPC_KEY_A    , CPC_KEY_NULL } ,
	   { 'A'       , CPC_KEY_A    , CPC_KEY_SHIFT} ,
	   
      { 's'       , CPC_KEY_S    , CPC_KEY_NULL } ,
	   { 'S'       , CPC_KEY_S    , CPC_KEY_SHIFT} ,
	   
      { 'd'       , CPC_KEY_D    , CPC_KEY_NULL } ,
      { 'D'       , CPC_KEY_D    , CPC_KEY_SHIFT} ,
	   
      { 'f'       , CPC_KEY_F    , CPC_KEY_NULL } ,
      { 'F'       , CPC_KEY_F    , CPC_KEY_SHIFT} ,

	   { 'g'       , CPC_KEY_G    , CPC_KEY_NULL } ,
	   { 'G'       , CPC_KEY_G    , CPC_KEY_SHIFT} ,

	   { 'h'       , CPC_KEY_H    , CPC_KEY_NULL } ,
	   { 'H'       , CPC_KEY_H    , CPC_KEY_SHIFT} ,

      { 'j'       , CPC_KEY_J    , CPC_KEY_NULL } ,
      { 'J'       , CPC_KEY_J    , CPC_KEY_SHIFT} ,

	   { 'k'       , CPC_KEY_K    , CPC_KEY_NULL } ,
	   { 'K'       , CPC_KEY_K    , CPC_KEY_SHIFT} ,

      { 'l'       , CPC_KEY_L    , CPC_KEY_NULL } ,
      { 'L'       , CPC_KEY_L    , CPC_KEY_SHIFT} ,
	   
      { ':'       , CPC_KEY_COLON , CPC_KEY_NULL } ,
      { '*'       , CPC_KEY_COLON , CPC_KEY_SHIFT} ,

	   { ';'       , CPC_KEY_SEMICOLON , CPC_KEY_NULL } ,
	   { '+'       , CPC_KEY_SEMICOLON , CPC_KEY_SHIFT} ,

	   { ']'       , CPC_KEY_CLOSE_SQUARE_BRACKET , CPC_KEY_NULL } ,
	   { '}'       , CPC_KEY_CLOSE_SQUARE_BRACKET , CPC_KEY_SHIFT} ,

	   { VK_SHIFT  , CPC_KEY_SHIFT , CPC_KEY_NULL } ,
	   
      { '/'      , CPC_KEY_BACKSLASH , CPC_KEY_NULL } ,
      { '`'      , CPC_KEY_BACKSLASH , CPC_KEY_SHIFT} ,

      { 'z'       , CPC_KEY_Z    , CPC_KEY_NULL } ,
      { 'Z'       , CPC_KEY_Z    , CPC_KEY_SHIFT} ,

      { 'x'       , CPC_KEY_X    , CPC_KEY_NULL } ,
      { 'X'       , CPC_KEY_X    , CPC_KEY_SHIFT} ,

      { 'c'       , CPC_KEY_C    , CPC_KEY_NULL } ,
      { 'C'       , CPC_KEY_C    , CPC_KEY_SHIFT} ,

      { 'v'       , CPC_KEY_V    , CPC_KEY_NULL } ,
      { 'V'       , CPC_KEY_V    , CPC_KEY_SHIFT} ,

      { 'b'       , CPC_KEY_B    , CPC_KEY_NULL } ,
      { 'B'       , CPC_KEY_B    , CPC_KEY_SHIFT} ,

      { 'n'       , CPC_KEY_N    , CPC_KEY_NULL } ,
      { 'N'       , CPC_KEY_N    , CPC_KEY_SHIFT} ,

      { 'm'       , CPC_KEY_M    , CPC_KEY_NULL } ,
      { 'M'       , CPC_KEY_M    , CPC_KEY_SHIFT} ,

      { ','       , CPC_KEY_COMMA, CPC_KEY_NULL } ,
	   { '<'       , CPC_KEY_COMMA, CPC_KEY_SHIFT} ,
      
      { '.'       , CPC_KEY_DOT  , CPC_KEY_NULL } ,
      { '>'       , CPC_KEY_DOT  , CPC_KEY_SHIFT} ,
	   
      { '\\'      , CPC_KEY_FORWARD_SLASH , CPC_KEY_NULL } ,
      { '?'       , CPC_KEY_BACKSLASH, CPC_KEY_SHIFT} ,

	   { VK_SPACE  , CPC_KEY_SPACE, CPC_KEY_NULL } ,
	   { VK_CAPITAL, CPC_KEY_CAPS_LOCK , CPC_KEY_NULL } ,
	   
/*
      { 0xBB      , CPC_KEY_MINUS , CPC_KEY_NULL } ,
      { 0xDB      , CPC_KEY_HAT  , CPC_KEY_NULL } ,
	   { 0xDD      , CPC_KEY_AT   , CPC_KEY_NULL } ,
      { 0xBA      , CPC_KEY_OPEN_SQUARE_BRACKET , CPC_KEY_NULL } ,
      { 0xC0      , CPC_KEY_COLON , CPC_KEY_NULL } ,
	   { 0xDE      , CPC_KEY_SEMICOLON , CPC_KEY_NULL } ,
	   { 0xBF      , CPC_KEY_CLOSE_SQUARE_BRACKET , CPC_KEY_NULL } ,
	   { 0xBD      , CPC_KEY_BACKSLASH , CPC_KEY_NULL } ,
	   { 0xBC      , CPC_KEY_COMMA, CPC_KEY_NULL } ,
      { 0xBE      , CPC_KEY_DOT  , CPC_KEY_NULL } ,
      { 0xE2      , CPC_KEY_FORWARD_SLASH , CPC_KEY_NULL } ,
*/
   };
   static const struct _MAP aMapNum[]=
   {
	   { VK_PRIOR  , CPC_KEY_F9   , CPC_KEY_NULL } ,
	   { VK_NEXT   , CPC_KEY_F3   , CPC_KEY_NULL } ,
      { VK_HOME   , CPC_KEY_F7   , CPC_KEY_NULL } ,
	   { VK_UP     , CPC_KEY_F8   , CPC_KEY_NULL } ,
	   { VK_LEFT   , CPC_KEY_F4   , CPC_KEY_NULL } ,
	   { VK_CLEAR  , CPC_KEY_F5   , CPC_KEY_NULL } ,
	   { VK_END    , CPC_KEY_F1   , CPC_KEY_NULL } ,
	   { VK_DOWN   , CPC_KEY_F2   , CPC_KEY_NULL } ,
	   { VK_INSERT    , CPC_KEY_F0   , CPC_KEY_NULL } ,
	   { '.'       , CPC_KEY_FDOT , CPC_KEY_NULL } ,
      { VK_RETURN , CPC_KEY_SMALL_ENTER, CPC_KEY_NULL},
	   { VK_CONTROL, CPC_KEY_CONTROL , CPC_KEY_NULL } ,
      /*
      { VK_UP     , CPC_KEY_CURSOR_UP, CPC_KEY_NULL},
      { VK_DOWN   , CPC_KEY_CURSOR_DOWN, CPC_KEY_NULL},
      { VK_LEFT   , CPC_KEY_CURSOR_LEFT, CPC_KEY_NULL},
      { VK_RIGHT  , CPC_KEY_CURSOR_RIGHT, CPC_KEY_NULL}
      */
   };

   int i;
   if (bNumPad) for (i = 0; i < _countof(aMapNum); i++)
   {
      if (aMapNum[i].vk == vk)
      {
         if (bKeyDown)
         {
            if (aMapNum[i].cpc_add != CPC_KEY_NULL)
            {
               CPC_SetKey(aMapNum[i].cpc_add);
            }
            CPC_SetKey(aMapNum[i].cpc);
         }
         else 
         {
            CPC_ClearKey(aMapNum[i].cpc);
            if (aMapNum[i].cpc_add != CPC_KEY_NULL)
            {
               CPC_ClearKey(aMapNum[i].cpc_add);
            }
         }
         break;
      }
   }
   else for (i = 0; i < _countof(aMapMain); i++)
   {
      if (aMapMain[i].vk == vk)
      {
         if (bKeyDown)
         {
            if (aMapMain[i].cpc_add != CPC_KEY_NULL)
            {
               CPC_SetKey(aMapMain[i].cpc_add);
            }
            CPC_SetKey(aMapMain[i].cpc);
         }
         else 
         {
            CPC_ClearKey(aMapMain[i].cpc);
            if (aMapMain[i].cpc_add != CPC_KEY_NULL)
            {
               CPC_ClearKey(aMapMain[i].cpc_add);
            }
         }
         break;
      }
   }
}

#define IDSTATUSWINDOW			1


void	MyApp_SetFullScreen(int Width, int Height)
{
	AppData.WindowedMode = FALSE;
	AppData.ScreenResX = Width;
	AppData.ScreenResY = Height;
}

void	MyApp_SetWindowed(int Width, int Height)
{
	AppData.WindowedMode = TRUE;
}


#include "general/lnklist/lnklist.h"


LIST_HEADER *pDialogList;

void	DialogList_Initialise()
{
	LinkList_InitialiseList(&pDialogList);
}

void	DialogList_Finish()
{
	LinkList_DeleteList(&pDialogList,NULL);
}

void	DialogList_AddDialog(HWND hDialog)
{
	LinkList_AddItemToListEnd(pDialogList, hDialog);
}

int		DialogList_RemoveCallback(HWND hThisDialog, HWND hDialogWanted)
{	
	if (hThisDialog == hDialogWanted)
		return 0;

	return -1;
}


void	DialogList_RemoveDialog(HWND hDialog)
{
	LIST_NODE *pDialogListNode;

	pDialogListNode = LinkList_SearchListForwards(pDialogList, hDialog, DialogList_RemoveCallback);

	if (pDialogListNode!=NULL)
	{
		LinkList_DeleteItem(pDialogList, pDialogListNode,NULL);
	}

}

int	DialogList_IsDialogMessageCallback(MSG *pMsg, HWND hThisDialog)
{
	if (IsDialogMessage(hThisDialog, pMsg))
		return 0;

	return -1;
}


BOOL	DialogList_IsDialogMessage(MSG *msg)
{
	LIST_NODE *pDialogListNode = NULL;

	pDialogListNode = LinkList_SearchListForwards(pDialogList, msg, DialogList_IsDialogMessageCallback);
	
	if (pDialogListNode!=NULL)
		return TRUE;

	return FALSE;
}


/*----------------------------------------------------------------------*/

BOOL	MyApp_RegisterClass(WNDCLASSEX *pClass)
{
	// attempt to register the class
	if (RegisterClassEx(pClass)==0)
	{
		// register failed
		return FALSE;
	}
	
	// register succeeded
	return TRUE;
}



/*----------------------------------------------------------------------*/

#if 0
#define TOOLBAR_BUTTON_ENTRY(x,y)	{x,y,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0}

TBBUTTON MainWndToolbarButtons[] =
{
    TOOLBAR_BUTTON_ENTRY(0,ID_BUTTON_SET_ADDR),
    TOOLBAR_BUTTON_ENTRY(1,ID_BUTTON_DECREASE_WIDTH),
    TOOLBAR_BUTTON_ENTRY(2,ID_BUTTON_MODE0),
};

#define MAINWND_NUM_TOOLBAR_BUTTONS (sizeof(MainWndToolbarButtons)/sizeof(TBBUTTON))

#define MAINWND_TOOLBAR_BUTTON_WIDTH 28
#define MAINWND_TOOLBAR_BUTTON_HEIGHT 28

#define MAINWND_TOOLBAR_CONTROL_IDENTIFIER 3

#endif

long FAR PASCAL WindowProc( HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT PaintStruct;

    switch(iMsg)
    {
	case WM_CREATE:

#if 0
		AppData.hwndToolbar= CreateToolbarEx(
		hwnd,
        WS_CHILD /*| WS_BORDER*/ | WS_VISIBLE,	// | CCS_TOP,
        MAINWND_TOOLBAR_CONTROL_IDENTIFIER,
        MAINWND_NUM_TOOLBAR_BUTTONS,
		AppData.AppInstance,
        IDR_TOOLBAR4,
        &MainWndToolbarButtons[0],
        MAINWND_NUM_TOOLBAR_BUTTONS,
        MAINWND_TOOLBAR_BUTTON_WIDTH,
        MAINWND_TOOLBAR_BUTTON_HEIGHT,
        MAINWND_TOOLBAR_BUTTON_WIDTH,
        MAINWND_TOOLBAR_BUTTON_HEIGHT,
        sizeof(TBBUTTON));
		
		
		AppData.hwndStatus = CreateStatusWindow(WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP, _T(""), hwnd, 101);
#endif
		/* accept dropped files */
		DragAcceptFiles(hwnd, TRUE);


		return 0;

	case WM_ENDSESSION:
	{
		/* logout, reboot etc */



	}
	break;
#if 0
	// windows ce
	case WM_HIBERNATE:
	{


	}
	break;
#endif


	case WM_ACTIVATE:
	{
		if (LOWORD(wParam)!=WA_INACTIVE)
		{
			// active 

			if (HIWORD(wParam)==0)
			{
				// maximized
				DI_AcquireDevices();
				DD_RestoreSurfaces();
				DD_ClearBothSurfaces();
				AppData.DoNotScanKeyboard = FALSE;
					
				return TRUE;
			}
		}

		CPC_ReleaseKeys();

		// maximized
		DI_UnAcquireDevices();
		AppData.DoNotScanKeyboard = TRUE;
		
		// restore initial state of scroll lock
		ScrollLock_RestoreState();
	
	}
	return 0;


	case WM_ACTIVATEAPP:
	{
		BOOL	ActiveState = (BOOL)wParam;
	
		if (ActiveState == TRUE)
		{
			DI_AcquireDevices();
			DD_RestoreSurfaces();
			DD_ClearBothSurfaces();
			AppData.DoNotScanKeyboard = FALSE;
			AppData.Minimized = FALSE;
			GetWindowRect(hwnd,&AppData.WindowRectBeforeMinimize);
		}
		else
		{

			DI_UnAcquireDevices();
			DS_ClearBuffer();
			CPC_ReleaseKeys();
			AppData.DoNotScanKeyboard = TRUE;
			AppData.Minimized = TRUE;

			// restore state of scrolllock
			ScrollLock_RestoreState();
		
		}

		
		AppData.ApplicationIsActive = ActiveState;

	}
	return 0;
///	case WM_NOTIFY:
//	{
//		int ControlID = (int)wParam;
//		NMHDR *pNotifyHeader = (NMHDR *)lParam;
//
//	}
//	break;

//	case WM_NCLBUTTONDOWN:
//	case WM_NCMBUTTONDOWN:
//	case WM_NCRBUTTONDOWN:
	case WM_ENTERSIZEMOVE:
	case WM_ENTERMENULOOP:
	{
		DI_UnAcquireDevices();
		DS_StopSound();
		CPC_ReleaseKeys();
		AppData.DoNotScanKeyboard = TRUE;		
		//DS_ClearBuffer();
	}
	break;

//	case WM_NCLBUTTONUP:
//	case WM_NCMBUTTONUP:
//	case WM_NCRBUTTONUP:
	case WM_EXITSIZEMOVE:
	case WM_EXITMENULOOP:
	{
		DI_AcquireDevices();
		DS_StartSound();
		AppData.DoNotScanKeyboard = FALSE;
	}
	break;

	
	case WM_SIZING:
		{
			LPRECT	pSizingRect = (LPRECT)lParam;

			if (AppData.WindowedMode)
			{
				pSizingRect->right = pSizingRect->left + AppData.WindowWidth;
				pSizingRect->bottom = pSizingRect->top + AppData.WindowHeight;
			}
			else
			{
				pSizingRect->left = 0;
				pSizingRect->right = AppData.ScreenResX;
				pSizingRect->top = 0;
				pSizingRect->bottom = AppData.ScreenResY;
			}
		}
		return 0;

//			switch (wParam)
//			{
//				case WMSZ_BOTTOM:
//					break;
//				case WMSZ_BOTTOMLEFT:
//					break;
//				case WMSZ_BOTTOMRIGHT:
//					break;
//				case WMSZ_LEFT:
//					break;
//				case WMSZ_RIGHT:
//					break;
//				case WMSZ_TOP:
//					break;
//				case WMSZ_TOPLEFT:
//					break;
//				case WMSZ_TOPRIGHT:
//					break;
//			}
//
//		}

	case WM_SIZE:
	{
#if 0
		SendMessage(AppData.hwndToolbar,WM_SIZE,wParam,lParam);
#endif	
		
		
	}
	break;
#if 0
		int Width = LOWORD(lParam);
		int Height= HIWORD(lParam);
		int	ResizeFlag = wParam;

		switch (ResizeFlag)
		{
			case SIZE_MAXIMIZED:
			{
				SetWindowPos(hwnd, NULL, 0,0, WindowWidth, WindowHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER); 
			}
			return 0;
			
			default:
				break;
		}
	}
	break;
#endif

		
	case WM_PARENTNOTIFY:
	{

//		int EventFlags = LOWORD(wParam);  // event flags
//		int idChild = HIWORD(wParam);  // identifier of child window 
//	
//		if (EventFlags == WM_DESTROY)
//		{
//			HWND hChild = lParam;
//
//			DestroyWindow(hChild);
//		}

	}
	break;

	
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO *pMinMax = (LPMINMAXINFO) lParam;
		
		if (AppData.WindowedMode)
		{
			RECT		WindowRect;
			
			// get current x,y position.
			GetWindowRect(hwnd, &WindowRect);

			pMinMax->ptMaxSize.x = WindowRect.right-WindowRect.left;
			pMinMax->ptMaxSize.y = WindowRect.bottom-WindowRect.top;
			pMinMax->ptMaxPosition.x = WindowRect.left;
			pMinMax->ptMaxPosition.y = WindowRect.top;
			pMinMax->ptMinTrackSize.x = AppData.WindowWidth;
			pMinMax->ptMinTrackSize.y = AppData.WindowHeight;
			pMinMax->ptMaxTrackSize.x = pMinMax->ptMaxSize.x;
			pMinMax->ptMaxTrackSize.y = pMinMax->ptMaxSize.y;

			return 0;

#if 0
			if ((WindowWidth!=0) && (WindowHeight!=0))
			{
				pMinMax->ptMaxSize.x = WindowWidth;
				pMinMax->ptMaxSize.y = WindowHeight;
				pMinMax->ptMaxPosition.x = WindowRect.left;
				pMinMax->ptMaxPosition.y = WindowRect.top;
				pMinMax->ptMinTrackSize.x = WindowWidth;
				pMinMax->ptMinTrackSize.y = WindowHeight;
				pMinMax->ptMaxTrackSize.x = WindowWidth;
				pMinMax->ptMaxTrackSize.y = WindowHeight;
				return 0;
			}
#endif
		}
		else
		{
			pMinMax->ptMaxSize.x = AppData.ScreenResX;
			pMinMax->ptMaxSize.y = AppData.ScreenResY;
			pMinMax->ptMaxPosition.x = 0;
			pMinMax->ptMaxPosition.y = 0;
			pMinMax->ptMinTrackSize.x = AppData.ScreenResX;
			pMinMax->ptMinTrackSize.y = AppData.ScreenResY;
			pMinMax->ptMaxTrackSize.x = AppData.ScreenResX;
			pMinMax->ptMaxTrackSize.y = AppData.ScreenResY;
			return 0;
		}
	}
	break;

	case WM_MOVING:
	{
		if (!AppData.WindowedMode)
		{
			LPRECT pMovingRect = (LPRECT)lParam;

			pMovingRect->left = 0;
			pMovingRect->right = AppData.ScreenResX;
			pMovingRect->top = 0;
			pMovingRect->bottom = AppData.ScreenResY;

			return TRUE;
		}
	}
	break;

	case WM_LBUTTONDOWN:
	{
		AppData.xPos = LOWORD(lParam);
		AppData.yPos = HIWORD(lParam);

		AppData.Buttons |= MK_LBUTTON;
	}
	break;

	case WM_LBUTTONUP:
	{
		AppData.Buttons &= ~MK_LBUTTON;
	}
	break;

	case WM_RBUTTONDOWN:
	{
		AppData.xPos = LOWORD(lParam);
		AppData.yPos = HIWORD(lParam);
		AppData.Buttons |= MK_RBUTTON;
	}
	break;

	case WM_RBUTTONUP:
	{
		AppData.Buttons &= ~MK_RBUTTON;
	}
	break;

	case WM_MOUSEMOVE:
	{
//		int xPos, yPos;

		AppData.xPos = LOWORD(lParam);
		AppData.yPos = HIWORD(lParam);
		AppData.Buttons = wParam;
	}
	break;

	//case WM_MOVE:

	case WM_SETFOCUS:
	{
		DI_AcquireDevices();
		AppData.DoNotScanKeyboard = FALSE;
		AppData.ApplicationHasFocus = TRUE;

	}
	return 0;

	case WM_KILLFOCUS:
	{
		AppData.ApplicationHasFocus = FALSE;
		DI_UnAcquireDevices();
		AppData.DoNotScanKeyboard = TRUE;

		CPC_ReleaseKeys();

		// restore scroll lock state
		ScrollLock_RestoreState();

/*		if (!WindowedMode)
		{
			// in full-screen and focus changed.
			// something bad might have happened
		
			// go to windowed mode in an attempt
			// to force a reset
			CPCEMU_SetWindowed();
		}
*/	}
	return 0;

//	case WM_SETCURSOR:

	//case WM_ENTERSIZEMOVE:
	//{
	//}
	//break;

	case WM_DROPFILES:
	{
		/* files dropped over it */

		/* drop handle */
		HANDLE DropHandle = (HANDLE)wParam;

		CPCEMU_HandleDragDrop(DropHandle);
	}
	return 0;

	case WM_ERASEBKGND:
		return 0;

	case WM_DISPLAYCHANGE:
	{
		if (AppData.WindowedMode)
		{
//			DD_ReleaseSurfaces();

			//DD_ShutdownWindowed();

			CPCEMU_SetWindowed();
		}
	}
	break;

    case WM_PAINT:
        BeginPaint( hwnd, &PaintStruct);
    //    
	//	DD_FlipWindowed();
//
		EndPaint( hwnd, &PaintStruct );
      break;

	case WM_CLOSE:
	{
		/* quit */
		if (!CPCEMU_DoQuit(hwnd))
			return 0;
	
		/* allow quit to continue */
	}
	break;

    case WM_DESTROY:
	{
		/* do not accept dropped files */
		DragAcceptFiles(hwnd, FALSE);

		Interface_RemoveDisk(hwnd,0);
		Interface_RemoveDisk(hwnd,1);


		Debugger_Close();

        PostQuitMessage( 0 );
	}
	return 0;

	default:
		break;
	}

    return DefWindowProc(hwnd, iMsg, wParam, lParam);
} 


#if 0
	/* TROELS addition if console */
  CPCHANDLE handle = (*PUB_cpc_console_api.create)(InitString, NULL);
     if (handle != NULL)
     {
        CPCEMU_MainLoop(FALSE);
        (*handle->api->close)(&handle);
     }	
#endif

/*------------------------------------------------------------------------*/
/* main entry function */
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int iCmdShow)
{
	/* check if another instance of this program is running already */

	/* create a named mutex */
	HANDLE hMutex = CreateMutex(NULL, TRUE, _T("ArnoldMutex"));

	/* does it already exist?? */
	if (GetLastError()==ERROR_ALREADY_EXISTS)
	{
		/* another instance of this application is open which has already created
		the named mutex */
		HWND hWnd;

		/* display dialog with choice of actions */
		if (MessageBox(GetDesktopWindow(),_T("Another instance of this application is open. Open this instance anyway?"),_T(APP_TITLE_TEXT),MB_ICONINFORMATION|MB_YESNO)==IDNO)
		{
			/* find the window */
			hWnd = FindWindow(_T(APP_CLASS_NAME),NULL);

			if (hWnd!=NULL)
			{
				/* window has been found */
				LPTSTR sCommandLine = GetCommandLine();

				/* if there is a command-line, communicate it to the other instance
				of this application */
				if (_tcslen(sCommandLine)!=0)
				{
					const TCHAR *sEscapedCommandLine = DDE_EscapeString(sCommandLine);

					if (sEscapedCommandLine)
					{
						DWORD ddeClientId;
						HSZ hszService;
						HSZ hszTopic;

						ddeClientId = 0;
						if (DdeInitialize(&ddeClientId,DdeClientCallback,APPCMD_CLIENTONLY,0)==DMLERR_NO_ERROR)
						{
							int iCodePage;

							/* initialise code page */
#ifndef _UNICODE
							iCodePage = CP_WINANSI;
#else
							iCodePage = CP_WINUNICODE;
#endif

							/* get handle to string */
							hszService = DdeCreateStringHandle(ddeClientId,_T(APP_NAME),iCodePage);

							if (hszService!=NULL)
							{
								/* get handle to string */
								hszTopic = DdeCreateStringHandle(ddeClientId,SZDDESYS_TOPIC,iCodePage);

								if (hszTopic!=NULL)
								{
									HCONV hConv;

									hConv = DdeConnect(ddeClientId, hszService, hszTopic,(PCONVCONTEXT)NULL);

									if (hConv!=0)
									{
										HDDEDATA hData;
										int nDataLength;

										nDataLength = (_tcslen(sEscapedCommandLine) + 2 + 10 + 1)*sizeof(TCHAR);

										hData = DdeCreateDataHandle(ddeClientId,NULL,nDataLength,0,0,CF_TEXT,0);
		
										if (hData)
										{
											const TCHAR *DDeKey=_T("[CmdLine \"");
											const TCHAR *DDeKeyEnd=_T("\"]");

											int nOffset = 0;
											hData = DdeAddData(hData, (LPBYTE)DDeKey, _tcslen(DDeKey),nOffset);
											nOffset+=_tcslen(DDeKey);
											hData = DdeAddData(hData, (LPBYTE)sEscapedCommandLine, _tcslen(sEscapedCommandLine), nOffset);
											nOffset+=_tcslen(sEscapedCommandLine);
											/* ensure closing null termination character */
											hData = DdeAddData(hData, (LPBYTE)DDeKeyEnd, (_tcslen(DDeKeyEnd)+sizeof(TCHAR)), nOffset);

											DdeClientTransaction((unsigned char *)hData,0x0ffffffff,hConv,0L,0,XTYP_EXECUTE,1000,NULL);
											
											DdeFreeDataHandle(hData);
										}

										/* disconnect the conversation */
										DdeDisconnect(hConv);
									}

									/* free handle to string */
									DdeFreeStringHandle(ddeClientId, hszTopic);
								}

								/* free handle to string */
								DdeFreeStringHandle(ddeClientId,hszService);
							}
						
							DdeUninitialize(ddeClientId);
						}

						/* flash the task bar */
						FlashWindow(hWnd, TRUE);
					
						/* free command-line string */
						free((void *)sEscapedCommandLine);
					}

				}

				/* release the mutex */
				ReleaseMutex(hMutex);	
				
				/* quit */
				return FALSE;	
			}
		}

	}		

#ifdef _DEBUG
	{
		/* enable memory leak checking */

		/* Get current flag */
		int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

		/* Turn on leak-checking bit */
		tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

		/* Set flag to the new value */
		_CrtSetDbgFlag(tmpFlag);
	}
#endif

	/* creating the mutex fails */
	/* so there are no other instances running */


	/* check to ensure this app can be started */
	if (CheckBitDepth()==FALSE)
	{
		/* show error message */
		ErrorMessage(Messages[65]);
	
		return 0;
	}

	/* app should be able to succeed now */


	/* setup DDE server */
	AppData.ddeId = 0;
	if (DdeInitialize(&AppData.ddeId, DdeCallback,APPCLASS_STANDARD|CBF_SKIP_REGISTRATIONS|CBF_SKIP_CONNECT_CONFIRMS|APPCMD_FILTERINITS|CBF_FAIL_REQUESTS|CBF_SKIP_UNREGISTRATIONS,0)==DMLERR_NO_ERROR)
	{
		int iCodePage;

		/* initialise code page */
#ifndef _UNICODE
		iCodePage = CP_WINANSI;
#else
		iCodePage = CP_WINUNICODE;
#endif

		/* create a string handle identifying the server name */
		AppData.hszAppName = DdeCreateStringHandle(AppData.ddeId, _T(APP_NAME), iCodePage); 

		/* create a string handle identifying the system topic */
		AppData.hszSystemTopicName = DdeCreateStringHandle(AppData.ddeId, SZDDESYS_TOPIC, iCodePage);

		if ((AppData.hszAppName!=0) && (AppData.hszSystemTopicName!=0))
		{
			/* register the server */
			DdeNameService(AppData.ddeId, AppData.hszAppName, NULL, DNS_REGISTER);
		}
	}
	else
	{
		AppData.ddeId = 0;
	}


	AppData.FullScreenDepth = 16;
	AppData.FullScreenWidth = 800;
	AppData.FullScreenHeight = 600;
	RecentItems_Init(RECENT_LIST_FILES);
	RecentItems_Restore(RECENT_LIST_FILES,_T("File"));

	{
		WNDCLASSEX	WindowClass;
		BOOL bDirectX;

		InitCommonControls();

		/* register for mouse wheel messages, this is required for Win95

		not found with mingw */
		RegisterWindowMessage(MSH_MOUSEWHEEL);

		AppData.AppInstance = hInstance;

		/* initialise class */
		WindowClass.cbSize = sizeof(WNDCLASSEX);
		WindowClass.style = CS_OWNDC;	
		WindowClass.lpfnWndProc = WindowProc;
		WindowClass.cbClsExtra = 0;
		WindowClass.cbWndExtra = 0;
		WindowClass.hInstance = hInstance ? hInstance : GetModuleHandle(NULL);
		WindowClass.hIcon = LoadIcon( hInstance,MAKEINTRESOURCE(IDI_ICON_ARNOLD)); 
		WindowClass.hIconSm = WindowClass.hIcon;
		WindowClass.hCursor = LoadCursor( NULL, IDC_ARROW );
		WindowClass.hbrBackground = GetStockObject(GRAY_BRUSH);
		WindowClass.lpszMenuName = NULL;
		WindowClass.lpszClassName = _T(APP_CLASS_NAME);
			
		/* register class */
		if (MyApp_RegisterClass(&WindowClass))
		{
			/* create app window */
			AppData.ApplicationHwnd = CreateWindowEx(
				WS_EX_APPWINDOW,
				_T(APP_CLASS_NAME),
				/* the title is now static; any information should be displayed
				in the status bar */
				_T(APP_NAME),
				WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
				0,
				0,
				GetSystemMetrics( SM_CXSCREEN ),
				GetSystemMetrics( SM_CYSCREEN ), 
				NULL,
				NULL,
				hInstance,
				NULL );

			{
				LONG WindowLong;

				WindowLong = GetWindowLong(AppData.ApplicationHwnd, GWL_EXSTYLE);
				WindowLong |= WS_EX_ACCEPTFILES;
				SetWindowLong(AppData.ApplicationHwnd, GWL_EXSTYLE, WindowLong);
				

				WindowLong = GetWindowLong(AppData.ApplicationHwnd, GWL_STYLE);
				WindowLong &=~WS_THICKFRAME;
				SetWindowLong(AppData.ApplicationHwnd, GWL_STYLE, WindowLong);
			}

			/* show the window */
			ShowWindow( AppData.ApplicationHwnd, iCmdShow );
			/* update window */
			UpdateWindow( AppData.ApplicationHwnd );
	
			bDirectX = TRUE; /*DirectX_CheckComponentsArePresent(); Checks below are safer. Troels. */

				/* check all required directx components are available */
//				if (!DirectX_CheckComponentsArePresent())
//				{
//					ErrorMessage(Messages[66]);
//				}
//				else


			bDirectX = bDirectX && DD_Init();
			if (bDirectX) 
				DD_SetupDD(AppData.ApplicationHwnd,NULL);
				
			bDirectX = bDirectX && DI_Init(hInstance);
			bDirectX = bDirectX && DS_Init(AppData.ApplicationHwnd);
        
			if (bDirectX)
			{
				LPTSTR theCommandLine;

				Joystick_Init();
				
				CPCEMU_Initialise();


				/* get actual command-line */
				theCommandLine = GetCommandLine();

				if (theCommandLine)
				{
					/* process it */
					CPCEMU_ProcessCommandLine(theCommandLine);
				}

				CPCEMU_Run();

				CPCEMU_Finish();


				DS_Close();

				DD_CloseDD();

				DI_Close();
				DD_Close();				
			}
		}
	}

	/* free DDE stuff */
	if (AppData.ddeId!=0)
	{
		if (AppData.hszSystemTopicName!=0)
		{
			/* free the string handle */
			DdeFreeStringHandle(AppData.ddeId, AppData.hszSystemTopicName);
			AppData.hszSystemTopicName = 0;
		}

		if (AppData.hszAppName!=0)
		{
			/* unregister this server */
			DdeNameService(AppData.ddeId, AppData.hszAppName, NULL, DNS_UNREGISTER);

			/* free the string handle */
			DdeFreeStringHandle(AppData.ddeId, AppData.hszAppName);

			AppData.hszAppName = 0;
		}

		DdeUninitialize(AppData.ddeId);
	}

	RecentItems_Store(RECENT_LIST_FILES,_T("File"));

	/* free recent items */
	RecentItems_Free(RECENT_LIST_FILES);

	/* release the mutex */
	ReleaseMutex(hMutex);	

#ifdef _DEBUG
	// if debug, then dump memory leaks
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}

void	SetStatusText(TCHAR *sText)
{
	SendMessage(AppData.hwndStatus, SB_SETTEXT,0, (LPARAM)sText);
}

void	MyApp_SetWindowDimensions(int Width, int Height)
{
	AppData.WindowWidth = Width;
	AppData.WindowHeight = Height;
}

typedef struct
{
	int WindowX, WindowY;
} WINDOW_SETTINGS;


void	MyApp_GetWindowSettings(WINDOW_SETTINGS *pWindowSettings)
{
	GetWindowRect(AppData.ApplicationHwnd, 0);
}


void	MyApp_CentraliseWindow(HWND hWindow, HWND hParent)
{
	RECT WindowRect;
	RECT ParentWindowRect;
	int WindowWidth, WindowHeight;
	int ParentWindowWidth, ParentWindowHeight;
	POINT WindowPos;
	HWND hParentWindow;

	if (hParent == 0)
	{
		// no parent specified, therefore centralise in desktop window
		hParentWindow = GetDesktopWindow();
	}
	else
	{
		// use specified parent
		hParentWindow = hParent;
	}


	// get window width and height.
	GetWindowRect(hWindow, &WindowRect);

	// calc width and height of this window
	WindowWidth = WindowRect.right-WindowRect.left;
	WindowHeight = WindowRect.bottom-WindowRect.top;

	// get parent window width and height.
	GetWindowRect(hParentWindow, &ParentWindowRect);

	// calc parent width and height
	ParentWindowWidth = ParentWindowRect.right - ParentWindowRect.left;
	ParentWindowHeight = ParentWindowRect.bottom - ParentWindowRect.top;

	// calc new position
	WindowPos.x = (ParentWindowWidth>>1)-(WindowWidth>>1) + ParentWindowRect.left;
	WindowPos.y = (ParentWindowHeight>>1)-(WindowHeight>>1) + ParentWindowRect.top;

	// set new pos
	SetWindowPos(hWindow, HWND_TOPMOST, WindowPos.x, WindowPos.y, 
		WindowWidth, WindowHeight, SWP_NOSIZE | SWP_NOZORDER);
}



BOOL	WinApp_ProcessSystemEvents()
{
	MSG	Message;

	// Is a message available?
	while (PeekMessage(&Message,NULL,0,0,PM_NOREMOVE))
	{
		// yes, get the message
		if (GetMessage(&Message,NULL,0,0))
		{
			// if the message is not WM_QUIT
			// Translate it and dispatch it
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		else
		{
			// Message was WM_QUIT. So break out of message loop
			// and quit
			return TRUE;
		}
	}

#ifdef SHOW_TIME
	{
		char Speed[256];
		unsigned long Speed = CPCEmulation_GetPercentRelativeSpeed();

		sprintf(Speed,_T("%d%%"),Speed);

		SetStatusText(Speed);
	}
#endif

	// No message, so idle, execute user function
	if (!(AppData.DoNotScanKeyboard))
	{
		DI_ScanKeyboard();
	}

	return FALSE;
}


void	MyApp_GetWindowRect(RECT *pRect)
{
	int Width, Height;


	if (!AppData.Minimized)
	{
		RECT WindowRect;

		/* get client rect of window */
		GetWindowRect(AppData.ApplicationHwnd,&WindowRect);
		
		/* width of rectangle */
		Width = WindowRect.right-WindowRect.left;
		Height = WindowRect.bottom-WindowRect.top;

		MyApp_SetWindowDimensions(Width,Height);
	
		*pRect = WindowRect;
	
	}
	else
	{
		Width = AppData.WindowRectBeforeMinimize.right - AppData.WindowRectBeforeMinimize.left;
		Height = AppData.WindowRectBeforeMinimize.bottom - AppData.WindowRectBeforeMinimize.top;

		MyApp_SetWindowDimensions(Width, Height);
	
		*pRect = AppData.WindowRectBeforeMinimize;
	}
}

