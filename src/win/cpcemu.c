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
/*#define LOCK_SPEED */

#define AUDIO
/*#define IMM_AUDIO_UPDATE */

/*#define INTERNAL */

//#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <windows.h>
//#include <dinput.h>
#include "directx/graphlib.h"
#include "resource.h"
#include "filedlg.h"
#include "reg.h"
#include "../cpc/cpc.h"
#include "../cpc/diskimage/diskimg.h"
#include "../cpc/cpcdefs.h"
#include "../cpc/multface.h"
#include "../cpc/cheatdb.h"
#include "iface.h"
#include "cpcemu.h"
#include "debugger.h"
#include "ziphandle.h"
//#include "cpc/audio.h"
#include "myapp.h"
#include "directx/dd.h"
//#include "settings.h"
#include "../cpc/debugmain.h"
#include "directx/dd.h"
#include "../ifacegen/ifacegen.h"
#include "../ifacegen/fnp.h"
#include "../cpc/cpcdefs.h"
#include "../ifacegen/filetool.h"
#include "../cpc/cheatsys.h"
#include "win.h"
#include <ctype.h>

//#define NEW_RENDER
//#define WESTPHASER
//#define MULTIFACE

//void	DoEmulatedJoystick(void);
//void	ScanJoyPad(void);
//void	CPCEMU_SetFullScreen();
//void	CPCEMU_SetWindowed();

#define CPCEMU_WARNING(x)	\
	MessageBox(GetActiveWindow(), x, "Arnold: WARNING!", MB_OK | MB_ICONWARNING)

//void	Interface_OpcodeFunc();
//void	Interface_OpcodeFuncAudio();

void	Debugger_UpdateDisplay(void);
void	Interface_FreeCheatDatabase(void);

static void (*pScanJoystick)(void)=DoEmulatedJoystick;
void	CheatDatabaseDialog(HWND hParent);

char	ProgramDirectory[MAX_PATH];
char	ZipDirectory[MAX_PATH];
char	CurrentWorkingDirectory[MAX_PATH];

BOOL Windowed=TRUE;
//int CurrentFrameIndex = 0;
//BOOL	DontRender = TRUE;

void	SetTaskPriority(int PriorityIndex);
void	Interface_SetupCPCTypeMenu(int);

HMENU ModeMenu;

#define MENU_MODE         300
//#define MENU_DEVICE       200

void	YM5_PropertiesDialog(HWND hParent);


#define MAKE_MENU_MODE_ITEM_DATA(w,h,d) \
	((w & 0x01fff) | ((h & 0x01fff)<<13) | ((d & 0x0ff)<<(13+13)))


//static int SnapshotVersion = 2;


//int FrameSkip = 0;
int FullScreenWidth = 640;
int FullScreenHeight = 480;
int FullScreenDepth = 16;

extern FILENAME_AND_PATH	DiskImage;
extern FILENAME_AND_PATH	Cartridge;
extern FILENAME_AND_PATH	Snapshot;
extern FILENAME_AND_PATH	Rom;
extern FILENAME_AND_PATH	ScreenSnapshot;
extern FILENAME_AND_PATH	WavOutput;
extern FILENAME_AND_PATH	AYOutput;
extern FILENAME_AND_PATH	DriveADiskImage;
extern FILENAME_AND_PATH	DriveBDiskImage;
extern FILENAME_AND_PATH	TapeImage;
extern FILENAME_AND_PATH	Multiface_CPCPLUS_ROM;
extern FILENAME_AND_PATH	Multiface_CPC_ROM;

#ifdef FDC_DEBUG
extern DEBUG_HANDLE FDC_Debug;
#endif

#ifdef CRTC_DEBUG
extern DEBUG_HANDLE CRTC_Debug;
#endif

#ifdef AUDIOEVENT_DEBUG
extern DEBUG_HANDLE AudioEvent_Debug;
#endif

#ifdef ASIC_DEBUGGING
extern DEBUG_HANDLE ASIC_Debug;
#endif

	HMENU	hAppMenu;

//void	Interface_DoReset(void);

// change these vectors for PLUS emulation
//int		Interface_PlusMode;
void	DoKeyboard(void);

extern	HWND	ApplicationHwnd;

//#ifdef DEBUGGING

//#endif

char	ErrorText[256];

#define ErrorMessage(ErrorText) \
	MessageBox(GetActiveWindow(),ErrorText,"Emulator Error",MB_OK)


//BOOL	DiskPresent[2] = {FALSE,FALSE};

#include <direct.h>


void	Interface_HandleZipFile(char *FilenameAndPath)
{
	_chdir(ZipDirectory);

	/* delete all files in temp dir */
	DeleteAllFilesInCurrentDir();
	
	_chdir(ZipDirectory);

	/* un-zip file */
	ZipHandle_UnZipFile(FilenameAndPath, ZipDirectory);

}


static char FilenameBuffer[MAX_PATH+1];


void	Interface_RemoveDisk(int Drive)
{
	/* has image been modified? */
	if (DiskImage_IsImageDirty(Drive))
	{
		char ImageModifiedText[128];

		sprintf(ImageModifiedText,"Disk Image in drive %d has been modified. Do you want to save changes?",Drive);

		/* ask if we want changes to be saved.. */
		if (MessageBox(GetActiveWindow(), ImageModifiedText,"Arnold", MB_YESNO | MB_ICONQUESTION)==IDYES)
		{
			/* write the image if yes */
			DiskImage_WriteImage(Drive);
		}
	}
	
	GenericInterface_RemoveDiskImage(Drive);
}

void	InitialiseFilenameForOpenFilename(FILENAME_AND_PATH *pFilenameAndPath)
{
	char			*Filename = FilenameAndPath_GetFilename(pFilenameAndPath);

	FilenameBuffer[0]='\0';

	if (Filename!=NULL)
	{
		/* setup initial filename */
		strcpy(FilenameBuffer, Filename);
	}
}

// Insert a unformatted disk image into an Amstrad drive
BOOL	Interface_InsertUnformattedDiskWithRequester(int Drive, char *ImagePath)
{
	OPENFILENAME	DiskImageOpenFilename;
	HWND			hwnd = GetActiveWindow();
//	char			DiskImageFilenameBuffer[MAX_PATH+1];

	InitialiseFilenameForOpenFilename(&DiskImage);

	InitFileDlg(hwnd,&DiskImageOpenFilename,"*.dsk","Disk Image (*.dsk)\0*.dsk\0All files (*.*)\0*.*\0\0",0);
	if (GetFileNameFromDlg(hwnd,&DiskImageOpenFilename,NULL,"Open Unformatted Disk Image",FilenameBuffer, ImagePath))
	{
		if (strlen(FilenameBuffer)!=0)
		{
			GenericInterface_InsertUnformattedDisk(Drive, FilenameBuffer);
		}

	}

	return TRUE;
}


// Insert a disk image into an Amstrad drive
BOOL	Interface_InsertDisk(int Drive, char *ImagePath, BOOL Zip)
{
	OPENFILENAME	DiskImageOpenFilename;
	HWND			hwnd = GetActiveWindow();
//	char			DiskImageFilenameBuffer[MAX_PATH+1];

	InitialiseFilenameForOpenFilename(&DiskImage);

	InitFileDlg(hwnd,&DiskImageOpenFilename,"*.dsk;*.zip","Disk Image (*.dsk)\0*.dsk;*.zip\0Zip archives (*.zip)\0*.zip\0All files (*.*)\0*.*\0\0",0);
	if (GetFileNameFromDlg(hwnd,&DiskImageOpenFilename,NULL,"Open Disk Image",FilenameBuffer, ImagePath))
	{

		if (strlen(FilenameBuffer)!=0)
		{
			
			/* detect if Zip File??? */
			char *pExtension;

			pExtension = GetExtensionFromFilename(FilenameBuffer);

			if (pExtension!=NULL)
			{
				if (stricmp("ZIP",pExtension)==0)
				{
					Interface_HandleZipFile(FilenameBuffer);
					return Interface_InsertDisk(Drive, ZipDirectory,TRUE);
				
				}
				
			}

/*
			if (Zip!=TRUE)
			{
				char *pPath, *pFile;

				pPath = GetPathFromPathAndFilename(FilenameBuffer);
				
				FilenameAndPath_SetPath(&DiskImage, pPath);
			
			}

			{ 
				char *pFile;

				pFile = GetFilenameFromPathAndFilename(FilenameBuffer);
				FilenameAndPath_SetFilename(&DiskImage, pFile);
			}



			if (Zip!=TRUE)
			{
				SetCurrentDirectory(FilenameAndPath_GetPath(&DiskImage));
			}
			else
			{
				SetCurrentDirectory(ZipDirectory);
			}
			*/
			
			if (FDD_IsDiskPresent(Drive))
			{
				Interface_RemoveDisk(Drive);
			}

			if (!GenericInterface_InsertDiskImage(Drive, FilenameBuffer))
			{
				ErrorMessage("Disk Image not inserted.\r\n\r\nThe disc image has not been recognised,\r\nor it contains errors.");
			}
		}

		return TRUE;
	}

	return FALSE;
}

void	Interface_OutputYM()
{
	OPENFILENAME	AYOpenFilename;
	HWND			hwnd = GetActiveWindow();

	FilenameBuffer[0] = '\0';

	InitFileDlg(hwnd,&AYOpenFilename,"*.ym5","YM5 uncompressed (*.ym5)\0*.ym5\0YM3 uncompressed (*.ym3)\0*.ym3\0All files (*.*)\0*.*\0\0",0);
	if (GetFileNameFromDlg(hwnd,&AYOpenFilename,NULL,"Start Recording YM",FilenameBuffer, FilenameAndPath_GetPath(&AYOutput)))
	{
		GenericInterface_AYStartRecording(FilenameBuffer);
	}
}


void	Interface_OutputWav()
{
	OPENFILENAME	WavOpenFilename;
	HWND			hwnd = GetActiveWindow();

	FilenameBuffer[0]='\0';

	InitFileDlg(hwnd,&WavOpenFilename,"*.wav","Wave audio file (*.wav)\0*.wav\0All files (*.*)\0*.*\0\0",0);
	if (GetFileNameFromDlg(hwnd,&WavOpenFilename,NULL,"Start Recording Wave audio",FilenameBuffer, FilenameAndPath_GetPath(&WavOutput)))
	{
		if (strlen(FilenameBuffer)!=0)
		{
			GenericInterface_WavOutputStartRecording(FilenameBuffer);
		}
	}
}




/* open a requester to select a rom */
BOOL	Interface_InsertRom(int RomIndex)
{
	OPENFILENAME	RomOpenFilename;
	HWND			hwnd = GetActiveWindow();

	InitialiseFilenameForOpenFilename(&Rom);

	InitFileDlg(hwnd,&RomOpenFilename,"*.rom","Rom Data (*.rom)\0*.rom\0Binary data (*.bin)\0*.bin\0All files (*.*)\0*.*\0\0",0);
	if (GetFileNameFromDlg(hwnd,&RomOpenFilename,NULL,"Open Rom Data",FilenameBuffer, FilenameAndPath_GetPath(&Rom)))
	{
		if (strlen(FilenameBuffer)!=0)
		{
			GenericInterface_LoadRom(RomIndex, FilenameBuffer);
		}

		return TRUE;
	}

	return FALSE;
}


/* open a requester for Multiface CPC+ rom */
BOOL	Interface_InsertMultifaceCPCPLUSROM(void)
{
	OPENFILENAME	RomOpenFilename;
	HWND			hwnd = GetActiveWindow();

	InitialiseFilenameForOpenFilename(&Multiface_CPCPLUS_ROM);

	InitFileDlg(hwnd,&RomOpenFilename,"*.rom","Rom Data (*.rom)\0*.rom\0Binary data (*.bin)\0*.bin\0All files (*.*)\0*.*\0\0",0);
	if (GetFileNameFromDlg(hwnd,&RomOpenFilename,NULL,"Open Rom Data for CPC+ version of Multiface",FilenameBuffer, FilenameAndPath_GetPath(&Multiface_CPCPLUS_ROM)))
	{
		if (strlen(FilenameBuffer)!=0)
		{
			Multiface_LoadRom(MULTIFACE_ROM_CPCPLUS_VERSION,FilenameBuffer);
		}

		return TRUE;
	}

	return FALSE;
}

/* open a requester for Multiface CPC+ rom */
BOOL	Interface_InsertMultifaceCPCROM(void)
{
	OPENFILENAME	RomOpenFilename;
	HWND			hwnd = GetActiveWindow();

	InitialiseFilenameForOpenFilename(&Multiface_CPC_ROM);

	InitFileDlg(hwnd,&RomOpenFilename,"*.rom","Rom Data (*.rom)\0*.rom\0Binary data (*.bin)\0*.bin\0All files (*.*)\0*.*\0\0",0);
	if (GetFileNameFromDlg(hwnd,&RomOpenFilename,NULL,"Open Rom Data for CPC version of Multiface",FilenameBuffer, FilenameAndPath_GetPath(&Multiface_CPC_ROM)))
	{
		if (strlen(FilenameBuffer)!=0)
		{
			Multiface_LoadRom(MULTIFACE_ROM_CPC_VERSION,FilenameBuffer);
		}

		return TRUE;
	}

	return FALSE;
}


/* open a requester to select a tape image */
BOOL	Interface_InsertTapeImage()
{
	OPENFILENAME	TapeImageOpenFilename;
	HWND			hwnd = GetActiveWindow();

	InitialiseFilenameForOpenFilename(&TapeImage);

	InitFileDlg(hwnd,&TapeImageOpenFilename,"*.cdt;*.tzx","Tape Image (*.cdt;*.tzx)\0*.cdt;*.tzx\0All files (*.*)\0*.*\0\0",0);
	if (GetFileNameFromDlg(hwnd,&TapeImageOpenFilename,NULL,"Open Tape Image",FilenameBuffer,FilenameAndPath_GetPath(&TapeImage)))
	{
		if (strlen(FilenameBuffer)!=0)
		{
			GenericInterface_InsertTapeImage(FilenameBuffer);
		}

		return TRUE;
	}

	return FALSE;
}


/* open a requester to select a tape image */
BOOL	Interface_InsertSample()
{
	OPENFILENAME	TapeImageOpenFilename;
	HWND			hwnd = GetActiveWindow();

	InitialiseFilenameForOpenFilename(&TapeImage);

	InitFileDlg(hwnd,&TapeImageOpenFilename,"*.voc;*.wav","VOC/WAV (*.voc;*.wav)\0*.voc;*.wav\0All files (*.*)\0*.*\0\0",0);
	if (GetFileNameFromDlg(hwnd,&TapeImageOpenFilename,NULL,"Open Sample",FilenameBuffer,FilenameAndPath_GetPath(&TapeImage)))
	{
		if (strlen(FilenameBuffer)!=0)
		{
			Sample_Load(FilenameBuffer);
		}

		return TRUE;
	}

	return FALSE;
}


void	Interface_RemoveTapeImage(void)
{
	TapeImage_Remove();
}


/* open a requester to load a snapshot */
BOOL	Interface_LoadSnapshot()
{
	OPENFILENAME	SnapshotOpenFilename;
	HWND			hwnd = GetActiveWindow();
	
	InitialiseFilenameForOpenFilename(&Snapshot);

	InitFileDlg(hwnd,&SnapshotOpenFilename,"*.sna","Snapshot (*.sna)\0*.sna\0All files (*.*)\0*.*\0\0",OFN_EXPLORER);
	if (GetFileNameFromDlg(hwnd,&SnapshotOpenFilename,NULL,"Open Snapshot",FilenameBuffer,FilenameAndPath_GetPath(&Snapshot)))
	{

		if (strlen(FilenameBuffer)!=0)
		{
			if (!GenericInterface_LoadSnapshot(FilenameBuffer))
			{
				/* an error occured */
				ErrorMessage("Failed to load snapshot, or error in snapshot file.");
			}
			/* update menu so it reflects CPC type selected */
			Interface_SetupCPCTypeMenu(CPC_GetCPCType());
		}

		return TRUE;
	}

	return FALSE;
}



void	Interface_OpenCartridge()
{
	OPENFILENAME	CartridgeOpenFilename;
	HWND			hwnd = GetActiveWindow();
	//char			CartridgeFilenameBuffer[MAX_PATH+1];

	InitialiseFilenameForOpenFilename(&Cartridge);

	InitFileDlg(hwnd,&CartridgeOpenFilename,"*.cpr","Cartridge (*.cpr)\0*.cpr\0All files (*.*)\0*.*\0\0",OFN_EXPLORER);
	
	if (GetFileNameFromDlg(hwnd,&CartridgeOpenFilename,NULL,"Open Cartridge",FilenameBuffer,FilenameAndPath_GetPath(&Cartridge)))
	{

		if (strlen(FilenameBuffer)!=0)
		{
			if (!(GenericInterface_InsertCartridge(FilenameBuffer)))
			{
				/* an error occured */
				ErrorMessage("Failed to load cartridge, or error in cartridge data file.");
			}
			
			Interface_SetupCPCTypeMenu(CPC_GetCPCType());
			
		}
	}
}

	



// write a snapshot
BOOL	Interface_SaveSnapshot()
{
	OPENFILENAME	SnapshotSaveFilename;
	HWND			hwnd = GetActiveWindow();

	FilenameBuffer[0]='\0';

	InitFileDlg(hwnd,&SnapshotSaveFilename,"*.sna","Snapshot (*.sna)\0*.sna\0All files (*.*)\0*.*\0\0",OFN_EXPLORER | OFN_OVERWRITEPROMPT);
	if (GetSaveNameFromDlg(hwnd,&SnapshotSaveFilename,NULL,"Save Snapshot As",FilenameBuffer,FilenameAndPath_GetPath(&Snapshot)))
	{
		if (strlen(FilenameBuffer)!=0)
		{
			GenericInterface_SnapshotSave(FilenameBuffer);
		}

		return TRUE;
	}

	return FALSE;
}

// write a screen snapshot
BOOL	Interface_SaveScreenSnapshot()
{
	OPENFILENAME	SnapshotSaveFilename;
	HWND			hwnd = GetActiveWindow();

	FilenameBuffer[0]='\0';

	InitFileDlg(hwnd,&SnapshotSaveFilename,"*.bmp","Screen Snapshot (*.bmp)\0*.bmp\0All files (*.*)\0*.*\0\0",OFN_EXPLORER | OFN_OVERWRITEPROMPT);
	if (GetSaveNameFromDlg(hwnd,&SnapshotSaveFilename,NULL,"Save Screen Snapshot As",FilenameBuffer,FilenameAndPath_GetPath(&ScreenSnapshot)))
	{

		if (strlen(FilenameBuffer)!=0)
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


void	Interface_AddMenuItem(HMENU hMenu, int Width, int Height, int Depth)
{
	MENUITEMINFO	MenuItemInfo;
	int				NumItemsOnMenu;
	int				ItemPosition;
	char	ResolutionText[16];

	sprintf(ResolutionText, "%dx%dx%d",Width, Height, Depth);

	NumItemsOnMenu = GetMenuItemCount(hMenu);

	ItemPosition = NumItemsOnMenu;

	MenuItemInfo.cbSize = sizeof(MENUITEMINFO);
	MenuItemInfo.fMask = MIIM_DATA | MIIM_ID | MIIM_TYPE;
	MenuItemInfo.fType = MFT_STRING;
	MenuItemInfo.fState = MFS_DEFAULT;
	MenuItemInfo.wID = MENU_MODE + NumItemsOnMenu;
	MenuItemInfo.hSubMenu = NULL;
	MenuItemInfo.hbmpChecked = NULL;
	MenuItemInfo.hbmpUnchecked = NULL;
	MenuItemInfo.dwItemData = MAKE_MENU_MODE_ITEM_DATA(Width,Height,Depth);
	MenuItemInfo.dwTypeData = ResolutionText;
	MenuItemInfo.cch = strlen(ResolutionText);

	InsertMenuItem(hMenu, ItemPosition, TRUE, &MenuItemInfo);
}


void	Interface_AddResolutionToMenu(int Width, int Height, int BPP)
{

	Interface_AddMenuItem(ModeMenu, Width, Height, BPP);
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

/////////////////////////////////////////////////////////////////////////

const int		Rom_UpdateArray[]=
{
	IDC_STATIC_ROM1,
	IDC_CHECK_ROM1,
	IDC_STATIC_ROM2,
	IDC_CHECK_ROM2,
	IDC_STATIC_ROM3,
	IDC_CHECK_ROM3,
	IDC_STATIC_ROM4,
	IDC_CHECK_ROM4,
	IDC_STATIC_ROM5,
	IDC_CHECK_ROM5,
	IDC_STATIC_ROM6,
	IDC_CHECK_ROM6,
	IDC_STATIC_ROM7,
	IDC_CHECK_ROM7,
	IDC_STATIC_ROM8,
	IDC_CHECK_ROM8,
	IDC_STATIC_ROM9,
	IDC_CHECK_ROM9,
	IDC_STATIC_ROM10,
	IDC_CHECK_ROM10,
	IDC_STATIC_ROM11,
	IDC_CHECK_ROM11,
	IDC_STATIC_ROM12,
	IDC_CHECK_ROM12,
	IDC_STATIC_ROM13,
	IDC_CHECK_ROM13,
	IDC_STATIC_ROM14,
	IDC_CHECK_ROM14,
	IDC_STATIC_ROM15,
	IDC_CHECK_ROM15,
	IDC_STATIC_ROM16,
	IDC_CHECK_ROM16,
};

const int NUM_ROMS = (sizeof(Rom_UpdateArray)/(sizeof(int)*2));

static void	RomDialog_Update(HWND hRomDialog)
{
	const int *pArray = Rom_UpdateArray;
	int i;

	for (i=0; i<NUM_ROMS; i++)
	{
		SetDlgItemText(hRomDialog,pArray[0],ExpansionRom_GetRomName(i+1));
		pArray++;
		SetCheckButtonState(hRomDialog, pArray[0], ExpansionRom_IsActive(i+1));
		pArray++;
	}
}

static void	DoRomCheck(HWND hwnd, WPARAM wParam)
{
	int RomIndex;
	int CheckState;

	switch (LOWORD(wParam))
	{
	default:
		case IDC_CHECK_ROM1:
		{
			RomIndex = 1;
		}
		break;

		case IDC_CHECK_ROM2:
		{
			RomIndex = 2;
		}
		break;

		case IDC_CHECK_ROM3:
		{
			RomIndex = 3;
		}
		break;

		case IDC_CHECK_ROM4:
		{
			RomIndex = 4;
		}
		break;

		case IDC_CHECK_ROM5:
		{
			RomIndex = 5;
		}
		break;

		case IDC_CHECK_ROM6:
		{
			RomIndex = 6;
		}
		break;

		case IDC_CHECK_ROM7:
		{
			RomIndex = 7;
		}
		break;

		case IDC_CHECK_ROM8:
		{
			RomIndex = 8;
		}
		break;

		case IDC_CHECK_ROM9:
		{
			RomIndex = 9;
		}
		break;

		case IDC_CHECK_ROM10:
		{
			RomIndex = 10;
		}
		break;

		case IDC_CHECK_ROM11:
		{
			RomIndex = 11;
		}
		break;

		case IDC_CHECK_ROM12:
		{
			RomIndex = 12;
		}
		break;

		case IDC_CHECK_ROM13:
		{
			RomIndex = 13;
		}
		break;

		case IDC_CHECK_ROM14:
		{
			RomIndex = 14;
		}
		break;

		case IDC_CHECK_ROM15:
		{
			RomIndex = 15;
		}
		break;

		case IDC_CHECK_ROM16:
		{
			RomIndex = 16;
		}
		break;

	}

	/* get check state */
	CheckState = SendDlgItemMessage(hwnd,LOWORD(wParam), BM_GETCHECK,0,0);

	/* checked */
	ExpansionRom_SetActiveState(RomIndex,(CheckState==BST_CHECKED));
}

BOOL CALLBACK  RomDialogProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

    switch (iMsg)
    {
        
        case WM_CREATE:
            return TRUE;

        case WM_COMMAND:
        {
			if (HIWORD(wParam)==BN_CLICKED)
			{
		
				switch (LOWORD(wParam))
				{
					/* check box clicked */
					case IDC_CHECK_ROM1:
					case IDC_CHECK_ROM2:
					case IDC_CHECK_ROM3:
					case IDC_CHECK_ROM4:
					case IDC_CHECK_ROM5:
					case IDC_CHECK_ROM6:
					case IDC_CHECK_ROM7:
					case IDC_CHECK_ROM8:
					case IDC_CHECK_ROM9:
					case IDC_CHECK_ROM10:
					case IDC_CHECK_ROM11:
					case IDC_CHECK_ROM12:
					case IDC_CHECK_ROM13:
					case IDC_CHECK_ROM14:
					case IDC_CHECK_ROM15:
					case IDC_CHECK_ROM16:
					{
						DoRomCheck(hwnd, wParam);
					}
					break;

					/* insert rom button clicked */
					case IDC_BUTTON_ROM1:
					case IDC_BUTTON_ROM2:
					case IDC_BUTTON_ROM3:
					case IDC_BUTTON_ROM4:
					case IDC_BUTTON_ROM5:
					case IDC_BUTTON_ROM6:
					case IDC_BUTTON_ROM7:
					case IDC_BUTTON_ROM8:
					case IDC_BUTTON_ROM9:
					case IDC_BUTTON_ROM10:
					case IDC_BUTTON_ROM11:
					case IDC_BUTTON_ROM12:
					case IDC_BUTTON_ROM13:
					case IDC_BUTTON_ROM14:
					case IDC_BUTTON_ROM15:
					case IDC_BUTTON_ROM16:
					{
						int RomIndex;

						switch (LOWORD(wParam))
						{
							default:
							case IDC_BUTTON_ROM1:
							{
								RomIndex = 1;
							}
							break;

							case IDC_BUTTON_ROM2:
							{
								RomIndex = 2;
							}
							break;

							case IDC_BUTTON_ROM3:
							{
								RomIndex = 3;
							}
							break;

							case IDC_BUTTON_ROM4:
							{
								RomIndex = 4;
							}
							break;

							case IDC_BUTTON_ROM5:
							{
								RomIndex = 5;
							}
							break;

							case IDC_BUTTON_ROM6:
							{
								RomIndex = 6;
							}
							break;

							case IDC_BUTTON_ROM7:
							{
								RomIndex = 7;
							}
							break;

							case IDC_BUTTON_ROM8:
							{
								RomIndex = 8;
							}
							break;

							case IDC_BUTTON_ROM9:
							{
								RomIndex = 9;
							}
							break;

							case IDC_BUTTON_ROM10:
							{
								RomIndex = 10;
							}
							break;

							case IDC_BUTTON_ROM11:
							{
								RomIndex = 11;
							}
							break;

							case IDC_BUTTON_ROM12:
							{
								RomIndex = 12;
							}
							break;

							case IDC_BUTTON_ROM13:
							{
								RomIndex = 13;
							}
							break;

							case IDC_BUTTON_ROM14:
							{
								RomIndex = 14;
							}
							break;

							case IDC_BUTTON_ROM15:
							{
								RomIndex = 15;
							}
							break;

							case IDC_BUTTON_ROM16:
							{
								RomIndex = 16;
							}
							break;
						}

						Interface_InsertRom(RomIndex);
						RomDialog_Update(hwnd);
					}
					break;
				}

				return TRUE;
			}
		}
		break;
			case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			break;
	}
    return FALSE;
}

void	Interface_RomDialog(HWND hwnd)
{
	HWND hRomDialog;
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

	hRomDialog = CreateDialog (hInstance, MAKEINTRESOURCE(IDD_DIALOG_EXPANSIONROMS), 0, RomDialogProc);

	if (hRomDialog!=0)
	{
		ShowWindow(hRomDialog,SW_SHOW);
	
		RomDialog_Update(hRomDialog);
	}
}

/*-----------------------------------------*/

const int		ROMRAM_UpdateArray[]=
{
	IDC_CHECK_BANK0,
	IDC_CHECK_BANK1,
	IDC_CHECK_BANK2,
	IDC_CHECK_BANK3,
	IDC_CHECK_BANK4,
	IDC_CHECK_BANK5,
	IDC_CHECK_BANK6,
	IDC_CHECK_BANK7,
	IDC_CHECK_BANK8,
	IDC_CHECK_BANK9,
	IDC_CHECK_BANK10,
	IDC_CHECK_BANK11,
	IDC_CHECK_BANK12,
	IDC_CHECK_BANK13,
	IDC_CHECK_BANK14,
	IDC_CHECK_BANK15,
};

const int RAMROM_ArraySize= (sizeof(ROMRAM_UpdateArray)/sizeof(int));

void	RAMROM_Dialog_Update(HWND hRAMROMDialog)
{
	int i;
	const int *pArray = ROMRAM_UpdateArray;

	for (i=0; i<RAMROM_ArraySize; i++)
	{
		SetCheckButtonState(hRAMROMDialog,pArray[0],RAM_ROM_GetBankEnableState(i));

		pArray++;
	}

	SetCheckButtonState(hRAMROMDialog, IDC_CHECK_RAM_ON, RAM_ROM_IsRamOn());
	SetCheckButtonState(hRAMROMDialog, IDC_CHECK_RAM_WRITE_ENABLE, RAM_ROM_IsRamWriteEnabled());
	SetCheckButtonState(hRAMROMDialog, IDC_CHECK_EPROM_ON, RAM_ROM_IsEPROMOn());


}

void	RAMROM_DoCheck(HWND hwnd, WPARAM wParam)
{
	/* get check state */
	int CheckState;
	int BankIndex;

	switch (LOWORD(wParam))
	{
		default:
		case IDC_CHECK_BANK0:
		{
			BankIndex = 0;
		}
		break;

		case IDC_CHECK_BANK1:
		{
			BankIndex = 1;
		}
		break;

		case IDC_CHECK_BANK2:
		{
			BankIndex = 2;
		}
		break;
		
		case IDC_CHECK_BANK3:
		{
			BankIndex = 3;
		}
		break;

		case IDC_CHECK_BANK4:
		{
			BankIndex = 4;
		}
		break;

		case IDC_CHECK_BANK5:
		{
			BankIndex = 5;
		}
		break;

		case IDC_CHECK_BANK6:
		{
			BankIndex = 6;
		}
		break;

		case IDC_CHECK_BANK7:
		{
			BankIndex = 7;
		}
		break;

		case IDC_CHECK_BANK8:
		{
			BankIndex = 8;
		}
		break;

		case IDC_CHECK_BANK9:
		{
			BankIndex = 9;
		}
		break;

		case IDC_CHECK_BANK10:
		{
			BankIndex = 10;
		}
		break;

		case IDC_CHECK_BANK11:
		{
			BankIndex = 11;
		}
		break;

		case IDC_CHECK_BANK12:
		{
			BankIndex = 12;
		}
		break;
		
		case IDC_CHECK_BANK13:
		{
			BankIndex = 13;
		}
		break;

		case IDC_CHECK_BANK14:
		{
			BankIndex = 14;
		}
		break;

		case IDC_CHECK_BANK15:
		{
			BankIndex = 15;
		}
		break;
	}

	CheckState = SendDlgItemMessage(hwnd,LOWORD(wParam), BM_GETCHECK,0,0);

	RAM_ROM_SetBankEnable(BankIndex, (CheckState == BST_CHECKED));
}


BOOL CALLBACK  RAMROM_DialogProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

    switch (iMsg)
    {
        
        case WM_CREATE:
            return TRUE;

        case WM_COMMAND:
        {
			if (HIWORD(wParam)==BN_CLICKED)
			{
		
				switch (LOWORD(wParam))
				{
					/* check box clicked */
					case IDC_CHECK_BANK0:
					case IDC_CHECK_BANK1:
					case IDC_CHECK_BANK2:
					case IDC_CHECK_BANK3:
					case IDC_CHECK_BANK4:
					case IDC_CHECK_BANK5:
					case IDC_CHECK_BANK6:
					case IDC_CHECK_BANK7:
					case IDC_CHECK_BANK8:
					case IDC_CHECK_BANK9:
					case IDC_CHECK_BANK10:
					case IDC_CHECK_BANK11:
					case IDC_CHECK_BANK12:
					case IDC_CHECK_BANK13:
					case IDC_CHECK_BANK14:
					case IDC_CHECK_BANK15:
					{
						RAMROM_DoCheck(hwnd, wParam);
					}
					break;

					case IDC_CHECK_RAM_ON:
					case IDC_CHECK_RAM_WRITE_ENABLE:
					case IDC_CHECK_EPROM_ON:
					{
						BOOL	CheckState;

						/* get check state */
						CheckState = (SendDlgItemMessage(hwnd,LOWORD(wParam), BM_GETCHECK,0,0)==BST_CHECKED);

						switch (LOWORD(wParam))
						{
							default:
								break;

							case IDC_CHECK_RAM_ON:
							{
								RAM_ROM_SetRamOnState(CheckState);
								break;
							}

							case IDC_CHECK_RAM_WRITE_ENABLE:
							{
								RAM_ROM_SetRamWriteEnableState(CheckState);
								break;
							}

							case IDC_CHECK_EPROM_ON:
							{
								RAM_ROM_SetEPROMOnState(CheckState);
								break;
							}
						}
					}
					break;

				}

				return TRUE;
			}
		}
		break;
			case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			break;
	}
    return FALSE;
}

void	Interface_RAMROM_Dialog(HWND hwnd)
{
	HWND hRAMROMDialog;
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

	hRAMROMDialog = CreateDialog (hInstance, MAKEINTRESOURCE(IDD_DIALOG_RAM_ROM), 0, RAMROM_DialogProc);

	if (hRAMROMDialog!=0)
	{
		ShowWindow(hRAMROMDialog,SW_SHOW);
	
		RAMROM_Dialog_Update(hRAMROMDialog);
	}
}




/////////////////////////////////////////////////////////////////////////

const char *ROM_NOT_LOADED="ROM NOT LOADED";
const char *ROM_LOADED="ROM LOADED";

void	MultifaceDialog_Update(HWND hDialog)
{
	const char *text;

	text = ROM_NOT_LOADED;

	if (Multiface_IsRomLoaded(MULTIFACE_ROM_CPC_VERSION))
	{
		text = ROM_LOADED;
	}

	SetDlgItemText(hDialog,IDC_STATIC_CPCROMSTATUS,text);
	
	text = ROM_NOT_LOADED;

	if (Multiface_IsRomLoaded(MULTIFACE_ROM_CPCPLUS_VERSION))
	{
		text = ROM_LOADED;
	}

	SetDlgItemText(hDialog,IDC_STATIC_CPCPLUSROMSTATUS,text);
		
	// is emulation active
	SetCheckButtonState(hDialog, IDC_CHECK2, Multiface_IsEmulationEnabled());
}

BOOL CALLBACK  MultifaceSettingsDialogProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

    switch (iMsg)
    {
        
        case WM_CREATE:
            return TRUE;

        case WM_COMMAND:
        {
			if (HIWORD(wParam)==BN_CLICKED)
			{
				/* check box clicked */

				switch (LOWORD(wParam))
				{
					case IDC_BUTTON_ROM1:
					case IDC_BUTTON_ROM2:
					{
						switch (LOWORD(wParam))
						{
							default:
							case IDC_BUTTON_ROM1:
							{
								Interface_InsertMultifaceCPCROM();
							}
							break;

							case IDC_BUTTON_ROM2:
							{
								Interface_InsertMultifaceCPCPLUSROM();
							}
							break;

						}

						MultifaceDialog_Update(hwnd);
					}
					return TRUE;

					case IDC_CHECK_MULTIFACE_HARDWARE_IS_ACTIVE:
					{
						/* get check state */
						int CheckState;

						CheckState = SendDlgItemMessage(hwnd,LOWORD(wParam), BM_GETCHECK,0,0);

						/* checked */
						Multiface_EnableEmulation((CheckState == BST_CHECKED));
					}
					break;

				}
			}
		}
		break;
			case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			break;
	}
    return FALSE;
}

void	Interface_MultifaceDialog(HWND hwnd)
{
	HWND hDialog;

	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

	hDialog = CreateDialog (hInstance, MAKEINTRESOURCE(IDD_DIALOG_MULTIFACE_SETTINGS), 0, MultifaceSettingsDialogProc);

	if (hDialog!=NULL)
	{
		ShowWindow(hDialog,SW_SHOW);

		MultifaceDialog_Update(hDialog);
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


	Interface_SetItemCheckState(hAppMenu, ID_CPCTYPE_CPC464,CPC464_ENABLED);
	Interface_SetItemCheckState(hAppMenu, ID_CPCTYPE_CPC664,CPC664_ENABLED);
	Interface_SetItemCheckState(hAppMenu, ID_CPCTYPE_CPC6128,CPC6128_ENABLED);
	Interface_SetItemCheckState(hAppMenu, ID_CPCTYPE_464PLUS,CPC464PLUS_ENABLED);
	Interface_SetItemCheckState(hAppMenu, ID_CPCTYPE_6128PLUS,CPC6128PLUS_ENABLED);
	Interface_SetItemCheckState(hAppMenu, ID_CPCTYPE_KCCOMPACT,KCCOMPACT_ENABLED);
}

const int CRTC_TypeArray[]=
{
	ID_CRTCTYPE_TYPE0,
	ID_CRTCTYPE_TYPE1,
	ID_CRTCTYPE_TYPE2,
	ID_CRTCTYPE_TYPEASIC,
	ID_CRTCTYPE_TYPEPREASIC,
	ID_CRTCTYPE_TYPE5
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
		Interface_SetItemCheckState(hAppMenu, CRTC_TypeArray[i],CRTC_Type_Enabled[i]);
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

	Interface_SetItemCheckState(hAppMenu, ID_MISC_RAMCONFIG_128K,RAM_64K_ENABLED);
	Interface_SetItemCheckState(hAppMenu, ID_MISC_RAMCONFIG_256KRAMEXPANSION,RAM_256K_ENABLED);
	Interface_SetItemCheckState(hAppMenu, ID_MISC_RAMCONFIG_256KSILICONDISK,RAM_256K_SILICON_DISK_ENABLED);
}


//////////////////////////////////////////////////////////////////////////////////////
WNDPROC PreviousMessageHandler;
extern BOOL Host_LockSpeed;

LRESULT CALLBACK	Interface_MessageHandler(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
		case WM_COMMAND:
		{
			int ID;

			ID = LOWORD(wParam);

			if ((ID>=MENU_MODE) && (ID<MENU_MODE+GetMenuItemCount(ModeMenu)))
			{
				int Width, Height, Depth;

				/* get mode details */

				MENUITEMINFO MenuItemInfo;

				/* get data */
				memset(&MenuItemInfo, 0, sizeof(MENUITEMINFO));
				MenuItemInfo.cbSize = sizeof(MENUITEMINFO);
				MenuItemInfo.fMask = MIIM_DATA;

				GetMenuItemInfo(ModeMenu, ID, FALSE, &MenuItemInfo);

				Width = MenuItemInfo.dwItemData & 0x01fff;
				Height = (MenuItemInfo.dwItemData>>13) & 0x01fff;
				Depth = (MenuItemInfo.dwItemData>>(13+13)) & 0x0ff;

				FullScreenWidth = Width;
				FullScreenHeight = Height;
				FullScreenDepth = Depth;
				return TRUE;
			}
			else
			{
			switch (LOWORD(wParam))
			{
				
				// drive A
				case ID_FILE_DRIVEA_INSERTDISK:
					Interface_InsertDisk(0,FilenameAndPath_GetPath(&DiskImage),FALSE);
					return 0;
				case ID_FILE_DRIVEA_REMOVEDISK:
					Interface_RemoveDisk(0);
					return 0;
				case ID_FILE_DRIVEA_TURNOVERDISK:
					FDD_TurnDisk(0);
					return 0;
				case ID_FILE_DRIVEA_INSERTNEWDISK_UNFORMATTED:
					Interface_InsertUnformattedDiskWithRequester(0,FilenameAndPath_GetPath(&DiskImage));
					return 0;

				// drive B
				case ID_FILE_DRIVEB_INSERTDISK:
					Interface_InsertDisk(1,FilenameAndPath_GetPath(&DiskImage),FALSE);
					return 0;
				case ID_FILE_DRIVEB_REMOVEDISK:
					Interface_RemoveDisk(1);
					return 0;
				case ID_FILE_DRIVEB_TURNOVERDISK:
					FDD_TurnDisk(1);
					return 0;
				case ID_FILE_DRIVEB_INSERTNEWDISK_UNFORMATTED:
					Interface_InsertUnformattedDiskWithRequester(1,FilenameAndPath_GetPath(&DiskImage));
					return 0;

				// cartridge
				case ID_FILE_CARTRIDGE_INSERTCARTRIDGE:
					Interface_OpenCartridge();
					return 0;
				case ID_FILE_CARTRIDGE_INSERTSYSTEMCARTRIDGE:
					{
						//FilenameAndPath_SetPath(&Cartridge, NULL);
						FilenameAndPath_SetFilename(&Cartridge, NULL);
						CPC_ReloadSystemCartridge();
					}
					return 0;
					
					//case ID_FILE_CARTRIDGE_REMOVECARTRIDGE:
				//	Interface_RemoveCartridge();
				//	return 0;

				
				case ID_FILE_OPENSNAPSHOT:
					Interface_LoadSnapshot();
					return 0;
				case ID_FILE_SAVESNAPSHOT:
					GenericInterface_SetSnapshotSize(64);
					Interface_SaveSnapshot();
					return 0;
				case ID_FILE_SAVESNAPSHOT_128K:
					GenericInterface_SetSnapshotSize(128);
					Interface_SaveSnapshot();
					return 0;
				case ID_FILE_SNAPSHOT_SETVERSIONTOWRITE_VERSION2:
					GenericInterface_SetSnapshotVersion(2);
					return 0;
				case ID_FILE_SNAPSHOT_SETVERSIONTOWRITE_VERSION3:
					GenericInterface_SetSnapshotVersion(3);
					return 0;


				case ID_FILE_WRITESCREENSNAPSHOT:
					Interface_SaveScreenSnapshot();
					return 0;

				case ID_FILE_INSERTROM:
					{
						Interface_RomDialog(hwnd);
					return 0;
					}

				case ID_FILE__INSERTSAMPLE:
				{
					Interface_InsertSample();
				}
				return 0;

				case ID_FILE_TAPEIMAGE_INSERT:
					{
						Interface_InsertTapeImage();
					}
					return 0;
				case ID_FILE_TAPEIMAGE_REMOVE:
					{
						Interface_RemoveTapeImage();
					}
					return 0;
				case ID_FILE_TAPEIMAGE_REWINDTOSTART:
					{
						TapeImage_RewindToStart();
					}
					return 0;


				case ID_MISC_RESET:
					GenericInterface_DoReset();
					return 0;

				case ID_MISC_PRINTEROUTPUT_DIGIBLASTER:
				{
					Printer_SetOutputMethod(PRINTER_OUTPUT_TO_DIGIBLASTER);
				}
				return 0;

				case ID_MISC_PRINTEROUTPUT_FILE:
				{
					Printer_SetOutputMethod(PRINTER_OUTPUT_TO_FILE);
				}
				return 0;

				case ID_MISC_TASKPRIORITY_NORMAL:
				{
					HANDLE hProcess;

					hProcess = GetCurrentProcess();

					SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS);
				}
				break;

				case ID_MISC_TASKPRIORITY_PRIORITY:
				{
					HANDLE hProcess;

					hProcess = GetCurrentProcess();

					SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS);
				}
				break;


				case ID_MISC_CHEATS_CHEATDATABASE:
				{
					CheatDatabaseDialog(hwnd);
				}
				break;

				case ID_MISC_CHEATS_CHEATSYSTEM:
				{
					CheatSystem_Update();
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



#ifdef AY_OUTPUT
				case ID_MISC_STSOUNDOUTPUT_STARTRECORDING:
					{
						Interface_OutputYM();	
					}
					break;

				case ID_MISC_STSOUNDOUTPUT_STOPRECORDING:
					{
						YMOutput_StopRecording();
					}
					break;

				case ID_MISC_STSOUNDOUTPUT_PROPERTIES:
					{
						YM5_PropertiesDialog(ApplicationHwnd);
					}
					break;
#endif

				case ID_MISC_RECORDSOUNDASWAV_STARTRECORDING:
					{
						Interface_OutputWav();
					}
					break;

				case ID_MISC_RECORDSOUNDASWAV_STOPRECORDING:
					{
						WavOutput_StopRecording();
					}
					break;

				case ID_MISC_FRAMESKIP_NONE:
					{
						CPC_SetFrameSkip(0);
					}
					break;
				case ID_MISC_FRAMESKIP_1:
					{
						CPC_SetFrameSkip(1);
					}
					break;
				case ID_MISC_FRAMESKIP_2:
					{
						CPC_SetFrameSkip(2);
					}
					break;
				case ID_MISC_FRAMESKIP_3:
					{
						CPC_SetFrameSkip(3);
					}
					break;
				case ID_MISC_FRAMESKIP_4:
					{
						CPC_SetFrameSkip(4);

					}
					break;


				case ID_MISC_FULLSCREEN:
				{
					CPCEMU_SetFullScreen();
				}
				break;

				case ID_MISC_LOCK_SPEED:
				{
					Host_LockSpeed = Interface_ToggleItemState(hAppMenu, ID_MISC_LOCK_SPEED);
				}
				break;
/*
				case ID_MISC_SOUND:
					{
						BOOL	State;

						State = Interface_ToggleItemState(hAppMenu,ID_MISC_SOUND);				

						if (State)
						{
							Z80_SetUserOpcodeFunction(Interface_OpcodeFuncAudio);
							Audio_Enable(TRUE);
						}
						else
						{
							Z80_SetUserOpcodeFunction(Interface_OpcodeFunc);
							Audio_Enable(FALSE);

						}
					}
*/					break;
	
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

					State = Interface_ToggleItemState(hAppMenu, ID_MISC_RAMCONFIG_128K);

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

					State = Interface_ToggleItemState(hAppMenu, ID_MISC_RAMCONFIG_256KRAMEXPANSION);

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

					State = Interface_ToggleItemState(hAppMenu, ID_MISC_RAMCONFIG_256KSILICONDISK);

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
	
				case ID_MISC_USEJOYSTICK:
				{
					BOOL	State;

					State = Interface_ToggleItemState(hAppMenu,ID_MISC_USEJOYSTICK);				
					
					if (State)
					{
						pScanJoystick = ScanJoyPad;
					}
					else
					{
						pScanJoystick = DoEmulatedJoystick;
					}
				}
				return 0;

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
							CPCType = CPC_TYPE_CPC464;
						}
						break;

						case ID_CPCTYPE_CPC664:
						{
							CPCType = CPC_TYPE_CPC664;
						}
						break;

						default:
						case ID_CPCTYPE_CPC6128:
						{
							CPCType = CPC_TYPE_CPC6128;
						}
						break;

						case ID_CPCTYPE_464PLUS:
						{
							CPCType = CPC_TYPE_464PLUS;
						}
						break;

						case ID_CPCTYPE_6128PLUS:
						{
							CPCType = CPC_TYPE_6128PLUS;
						}
						break;

						case ID_CPCTYPE_KCCOMPACT:
						{
							CPCType = CPC_TYPE_KCCOMPACT;
						}
						break;
					}

					CPC_SetCPCType(CPCType);
					Interface_SetupCPCTypeMenu(CPCType);
				}
				return 0;


				case ID_CRTCTYPE_TYPE0:
				case ID_CRTCTYPE_TYPE1:
				case ID_CRTCTYPE_TYPE2:
				case ID_CRTCTYPE_TYPEASIC:
				case ID_CRTCTYPE_TYPEPREASIC:
				case ID_CRTCTYPE_TYPE5:
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

						case ID_CRTCTYPE_TYPE5:
						{
							CRTCType = 5;
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


	//#ifdef DEBUGGING
#ifdef FDC_DEBUG
				case ID_DEBUG_FDCDEBUG:
				{
					BOOL	State;

					State = Interface_ToggleItemState(hAppMenu,ID_DEBUG_FDCDEBUG);				
					
					Debug_Enable(FDC_Debug,State);
					return 0;
				}
				break;
#endif

#ifdef ASIC_DEBUGGING
				case ID_DEBUG_ASICDEBUG:
				{
					BOOL	State;

					State = Interface_ToggleItemState(hAppMenu,ID_DEBUG_ASICDEBUG);
					
					Debug_Enable(ASIC_Debug,State);
				
					ASIC_SetDebug(State);

					return 0;
				}
				break;
#endif

#ifdef CRTC_DEBUG
				case ID_DEBUG_CRTCDEBUG:
				{
					BOOL	State;

					State = Interface_ToggleItemState(hAppMenu,ID_DEBUG_CRTCDEBUG);

					Debug_Enable(CRTC_Debug,State);
					
					CRTC_SetDebug(State);
					return 0;
				}
				break;
#endif

#ifdef AUDIOEVENT_DEBUG
				case ID_DEBUG_AUDIOEVENTDEBUGLOG:
				{
					BOOL	State;

					State = Interface_ToggleItemState(hAppMenu, ID_DEBUG_AUDIOEVENTDEBUGLOG);

					Debug_Enable(AudioEvent_Debug, State);
					return 0;
				}
				break;
#endif

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
	}
	return CallWindowProc(PreviousMessageHandler,hwnd,iMsg, wParam,lParam);
}


/*=====================================================================================*/

/********************************************************************************
 KEYBOARD 
 ********************************************************************************/

#include <dinput.h>

CPC_KEY_ID KeyboardRemapData[]=
{
	CPC_KEY_NULL,			//0x00
	CPC_KEY_ESC,  // DIK_ESCAPE          0x01
	CPC_KEY_1, // DIK_1               0x02
	CPC_KEY_2, // DIK_2               0x03
	CPC_KEY_3, // DIK_3               0x04
	CPC_KEY_4,	// DIK_4               0x05
	CPC_KEY_5,	// DIK_5               0x06
	CPC_KEY_6,	// DIK_6               0x07
	CPC_KEY_7,	// DIK_7               0x08
	CPC_KEY_8,	// DIK_8               0x09
	CPC_KEY_9,	// DIK_9               0x0A
	CPC_KEY_ZERO,	// DIK_0               0x0B
	CPC_KEY_MINUS,	// DIK_MINUS           0x0C    // - on main keyboard //
	CPC_KEY_HAT,	// DIK_EQUALS          0x0D
	CPC_KEY_DEL,	// DIK_BACK            0x0E    // backspace //
	CPC_KEY_TAB,	// DIK_TAB             0x0F
	CPC_KEY_Q,	// DIK_Q               0x10
	CPC_KEY_W,	// DIK_W               0x11
	CPC_KEY_E,	// DIK_E               0x12
	CPC_KEY_R,	// DIK_R               0x13
	CPC_KEY_T,	// DIK_T               0x14
	CPC_KEY_Y,	// DIK_Y               0x15
	CPC_KEY_U,	// DIK_U               0x16
	CPC_KEY_I,	// DIK_I               0x17
	CPC_KEY_O,	// DIK_O               0x18
	CPC_KEY_P,	// DIK_P               0x19
	CPC_KEY_AT,	// DIK_LBRACKET        0x1A
	CPC_KEY_OPEN_SQUARE_BRACKET,	// DIK_RBRACKET        0x1B
	CPC_KEY_RETURN,	// DIK_RETURN          0x1C    // Enter on main keyboard //
	CPC_KEY_CONTROL,	// DIK_LCONTROL        0x1D
	CPC_KEY_A,	// DIK_A               0x1E
	CPC_KEY_S,	// DIK_S               0x1F
	CPC_KEY_D,	// DIK_D               0x20
	CPC_KEY_F,	// DIK_F               0x21
	CPC_KEY_G,	// DIK_G               0x22
	CPC_KEY_H,	// DIK_H               0x23
	CPC_KEY_J,	// DIK_J               0x24
	CPC_KEY_K,	// DIK_K               0x25
	CPC_KEY_L,	// DIK_L               0x26
	CPC_KEY_COLON,	// DIK_SEMICOLON       0x27
	CPC_KEY_SEMICOLON,	// DIK_APOSTROPHE      0x28
	CPC_KEY_CLOSE_SQUARE_BRACKET,	// DIK_GRAVE           0x29    // accent grave //
	CPC_KEY_SHIFT,	// DIK_LSHIFT          0x2A
	CPC_KEY_BACKSLASH,	// DIK_BACKSLASH       0x2B
	CPC_KEY_Z,	// DIK_Z               0x2C
	CPC_KEY_X,	// DIK_X               0x2D
	CPC_KEY_C,	// DIK_C               0x2E
	CPC_KEY_V,	// DIK_V               0x2F
	CPC_KEY_B,	// DIK_B               0x30
	CPC_KEY_N,	// DIK_N               0x31
	CPC_KEY_M,	// DIK_M               0x32
	CPC_KEY_COMMA,	// DIK_COMMA           0x33
	CPC_KEY_DOT,	// DIK_PERIOD          0x34    // . on main keyboard //
	CPC_KEY_FORWARD_SLASH,	// DIK_SLASH           0x35    // / on main keyboard //
	CPC_KEY_SHIFT,	// DIK_RSHIFT          0x36
	CPC_KEY_NULL,		// DIK_MULTIPLY        0x37    // * on numeric keypad //
	CPC_KEY_NULL,		// DIK_LMENU           0x38    // left Alt //
	CPC_KEY_SPACE,	// DIK_SPACE           0x39
	CPC_KEY_CAPS_LOCK,	// DIK_CAPITAL         0x3A
	CPC_KEY_NULL,		// DIK_F1              0x3B
	CPC_KEY_NULL,		// DIK_F2              0x3C
	CPC_KEY_NULL,		// DIK_F3              0x3D
	CPC_KEY_NULL,		// DIK_F4              0x3E
	CPC_KEY_NULL,		// DIK_F5              0x3F
	CPC_KEY_NULL,		// DIK_F6              0x40
	CPC_KEY_NULL,		// DIK_F7              0x41
	CPC_KEY_NULL,		// DIK_F8              0x42
	CPC_KEY_NULL,		// DIK_F9              0x43
	CPC_KEY_NULL,		// DIK_F10             0x44
	CPC_KEY_NULL,		// DIK_NUMLOCK         0x45
	CPC_KEY_NULL,		// DIK_SCROLL          0x46    // Scroll Lock //
	CPC_KEY_F7,	// DIK_NUMPAD7         0x47
	CPC_KEY_F8,	// DIK_NUMPAD8         0x48
	CPC_KEY_F9,	// DIK_NUMPAD9         0x49
	CPC_KEY_NULL,		// DIK_SUBTRACT        0x4A    // - on numeric keypad //
	CPC_KEY_F4,	// DIK_NUMPAD4         0x4B
	CPC_KEY_F5,	// DIK_NUMPAD5         0x4C
	CPC_KEY_NULL,		// DIK_NUMPAD6         0x4D
	CPC_KEY_NULL,		// DIK_ADD             0x4E    // + on numeric keypad //
	CPC_KEY_F1,	// DIK_NUMPAD1         0x4F
	CPC_KEY_F2,	// DIK_NUMPAD2         0x50
	CPC_KEY_F3,	// DIK_NUMPAD3         0x51
	CPC_KEY_F0,	// DIK_NUMPAD0         0x52
	CPC_KEY_FDOT,	// DIK_DECIMAL         0x53    // . on numeric keypad //
	CPC_KEY_NULL,		// DIK_F11             0x57
	CPC_KEY_NULL		// DIK_F12             0x58
};

#define NUM_KEYS	(sizeof(KeyboardRemapData)/sizeof(int))

int EmulatedJoystick = 0;

void	DoEmulatedJoystick()
{
	// fire
	if (IsKeyPressed(DIK_NUMPAD5))
	{
		CPC_SetKey(CPC_KEY_JOY_FIRE1);

	}

	WestPhaser_SetCoordinates(0,0);
	WestPhaser_Trigger();

	// joystick emulated using Keypad

	// left
	if (IsKeyPressed(DIK_NUMPAD4))
	{
		CPC_SetKey(CPC_KEY_JOY_LEFT);
	}

	
	// right
	if (IsKeyPressed(DIK_NUMPAD6))
	{
		CPC_SetKey(CPC_KEY_JOY_RIGHT);
	}

	// up
	if (IsKeyPressed(DIK_NUMPAD8))
	{
		CPC_SetKey(CPC_KEY_JOY_UP);
	}
	
	
	// down
	if (IsKeyPressed(DIK_NUMPAD2))
	{
		CPC_SetKey(CPC_KEY_JOY_DOWN);
	}
	
	// fire
	if (IsKeyPressed(DIK_NUMPAD5))
	{
		CPC_SetKey(CPC_KEY_JOY_FIRE1);
	}
	
	// fire 2
	if (IsKeyPressed(DIK_NUMPAD0))
	{
		CPC_SetKey(CPC_KEY_JOY_FIRE2);
	}
}

#include "joy.h"


void	ScanJoyPad(void)
{

	JOY_STATE_DIGITAL	JoystickState;

	Joystick_Read();
	Joystick_StateDigital(&JoystickState);

	if (JoystickState.Left)
	{
		CPC_SetKey(CPC_KEY_JOY_LEFT);
	}
	
	if (JoystickState.Right)
	{
		CPC_SetKey(CPC_KEY_JOY_RIGHT);
	}
	
	if (JoystickState.Up)
	{
		CPC_SetKey(CPC_KEY_JOY_UP);
	}

	if (JoystickState.Down)
	{
		CPC_SetKey(CPC_KEY_JOY_DOWN);
	}

	if (JoystickState.Buttons & 0x0001)
	{
		CPC_SetKey(CPC_KEY_JOY_FIRE1);
	}

	if (JoystickState.Buttons & 0x0002)
	{
		CPC_SetKey(CPC_KEY_JOY_FIRE2);
	}

}

extern int xPos, yPos, Buttons;
int MousePosX, MousePosY, MouseDeltaX, MouseDeltaY;

void	Magnum_Update(int,int,int);
BOOL	LeftPressed, RightPressed;
BOOL	ToggleKeyPressed = FALSE;
void	KempstonMouse_Update(int DeltaX, int DeltaY, int LeftButton, int RightButton);
void	AmxMouse_Update(int, int, int, int);
void	SpanishLightGun_Update(int, int,int);
// map PC keyboard to CPC keyboard
void	DoKeyboard()
{
	int	i;

	CPC_ClearKeyboard();
	
	for (i=0; i<NUM_KEYS; i++)
	{
		if (IsKeyPressed(i))
		{
			CPC_SetKey(KeyboardRemapData[i]);
		}
		
	}

	if (IsKeyPressed(DIK_RCONTROL))
	{
		CPC_SetKey(CPC_KEY_CONTROL);
	}

	// prior and next are page up, page down                                                                                                                                                                           
	if (IsKeyPressed(DIK_NEXT))
	{
		CPC_SetKey(CPC_KEY_COPY);
	}

	if (IsKeyPressed(DIK_NUMPADENTER))
	{
		CPC_SetKey(CPC_KEY_SMALL_ENTER);
	}

	if (IsKeyPressed(DIK_UP))
	{
		CPC_SetKey(CPC_KEY_CURSOR_UP);
	}

	if (IsKeyPressed(DIK_DOWN))
	{
		CPC_SetKey(CPC_KEY_CURSOR_DOWN);
	}

	if (IsKeyPressed(DIK_LEFT))
	{
		CPC_SetKey(CPC_KEY_CURSOR_LEFT);
	}

	
	if (IsKeyPressed(DIK_RIGHT))
	{
		CPC_SetKey(CPC_KEY_CURSOR_RIGHT);
	}

	if (IsKeyPressed(DIK_F5))
	{
		CPC_Reset();
	}
	
	if (IsKeyPressed(DIK_F4))
	{
		if (!ToggleKeyPressed)
		{
			if (Windowed)
			{
				CPCEMU_SetFullScreen();
			}
			else
			{
				CPCEMU_SetWindowed();
			}
		}

		ToggleKeyPressed = TRUE;
	}
	else
	{
		ToggleKeyPressed = FALSE;
	}

	MouseDeltaX = xPos - MousePosX;
	MouseDeltaY = yPos - MousePosY;
	MousePosX = xPos;
	MousePosY = yPos;

	LeftPressed = ((Buttons & MK_LBUTTON)!=0);
	RightPressed = ((Buttons & MK_RBUTTON)!=0);
	
	KempstonMouse_Update(MouseDeltaX, MouseDeltaY, LeftPressed, RightPressed);


	{
		AmxMouse_Update(MouseDeltaX, MouseDeltaY, LeftPressed, RightPressed);
	}

//	if (LeftPressed)
//	{
//		CRTC_LightPen_Trigger(MousePosX, MousePosY);
//	}

	SpanishLightGun_Update(MousePosX, MousePosY, LeftPressed);

	Magnum_Update(MousePosX, MousePosY, LeftPressed);

	pScanJoystick();
  }

/*=====================================================================================*/


/*=====================================================================================*/
/* PATCHING */



//void	Interface_DoReset()
//{
//	CPC_Reset();
//}



/*------------------------------------------- */

//unsigned long NopCountToDate = 0;


DWORD	startTime, endTime, timeElapsed;
float	ScrTime;
float	PercentSpeed;

/*
void Interface_OpcodeFunc(int NopCount)
{
	int	i;

	if ((NopCount<0) || (NopCount>7))
	{
		NopCount = 3;
//		printf("debug this");
	}

	// update CPC nop count 
	CPC_UpdateNopCount(NopCount);

//	 execute CRTC for NopCount cycles 
//	for (i=0; i<NopCount; i++)
//	{
//		 execute CRTC for NopCount cycles 
//		CRTC_DoCycle();
//	}

	CRTC_DoCycles(NopCount);

	// determine if we should dump whole display 
	NopCountToDate += NopCount;

	// nop counter is used to define when we render the whole display
	if (NopCountToDate>=(NOPS_PER_MONITOR_SCREEN*(FRAME_SKIP+1)))
	{
		NopCountToDate = NopCountToDate - (NOPS_PER_MONITOR_SCREEN*(FRAME_SKIP+1));

		// dump whole display to screen
		Render_DumpDisplay();

#ifdef AY_OUTPUT
		YMOutput_WriteRegs();
#endif

		endTime = timeGetTime();

		DoKeyboard();

		timeElapsed = endTime-startTime;
	
		ScrTime = ((1.0f/50.0f)*1000.0f)*(FRAME_SKIP+1);

		PercentSpeed = (ScrTime/timeElapsed)*100;

		{
			char SpeedText[256];
	
			sprintf(SpeedText,"Speed: %2.2f%%",PercentSpeed);
			SetMainTitleText(SpeedText);
		}	

		// time emulation
		startTime = timeGetTime();

	}


}
*/

#if 0
DWORD timerID;

void CALLBACK MyTimerProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	CPC_UpdateAudio();
}
#endif

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
	Style = GetWindowLong(ApplicationHwnd, GWL_STYLE);

	Style |= WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
//	Style &= WS_MAXIMIZEBOX;
	SetWindowLong(ApplicationHwnd, GWL_STYLE, Style);

	// remove ex-styles
	ExStyle = GetWindowLong(ApplicationHwnd, GWL_EXSTYLE);

	ExStyle &= ~WS_EX_TOPMOST;

	SetWindowLong(ApplicationHwnd, GWL_EXSTYLE, ExStyle);

 // winnt crashes here
	if (hAppMenu)
	{
		SetMenu(ApplicationHwnd, NULL);
		SetMenu(ApplicationHwnd,hAppMenu);
	}


	Windowed = TRUE;
	
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

	Interface_OpenDebugDisplay(ApplicationHwnd,FALSE);

	{
		// remove some styles
		int Style = GetWindowLong(ApplicationHwnd, GWL_STYLE);

		Style &= ~(WS_BORDER | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU);

		SetWindowLong(ApplicationHwnd, GWL_STYLE, Style);

		// remove current menu
		SetMenu(ApplicationHwnd, NULL);

//		if (Windowed==TRUE)
//		{
//			DD_FromWindowed();
//		}

		Render_SetDisplayFullScreen(FullScreenWidth, FullScreenHeight, FullScreenDepth);

		ShowCursor(FALSE);

		Windowed = FALSE;
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
		

#include "../cpc/dirstuff.h"

void	CPCEMU_MainLoop()
{
	GetCurrentDirectory(MAX_PATH, ProgramDirectory);

	_chdir(ProgramDirectory);

	DirStuff_Initialise();

	{
		char LocalDirectory[256];

		strcpy(LocalDirectory,ProgramDirectory);
		strcat(LocalDirectory,"\\roms");

		SetDirectoryForLocation(EMULATOR_ROM_DIR, LocalDirectory);


		strcpy(LocalDirectory,ProgramDirectory);
		strcat(LocalDirectory,"\\roms\\amsdose");

		SetDirectoryForLocation(EMULATOR_ROM_CPCAMSDOS_DIR, LocalDirectory);

		strcpy(LocalDirectory,ProgramDirectory);
		strcat(LocalDirectory,"\\roms\\cpc464e");

		SetDirectoryForLocation(EMULATOR_ROM_CPC464_DIR, LocalDirectory);

		strcpy(LocalDirectory,ProgramDirectory);
		strcat(LocalDirectory,"\\roms\\cpc664e");

		SetDirectoryForLocation(EMULATOR_ROM_CPC664_DIR, LocalDirectory);

		strcpy(LocalDirectory,ProgramDirectory);
		strcat(LocalDirectory,"\\roms\\cpc6128e");

		SetDirectoryForLocation(EMULATOR_ROM_CPC6128_DIR, LocalDirectory);

		strcpy(LocalDirectory,ProgramDirectory);
		strcat(LocalDirectory,"\\roms\\cpcplus");

		SetDirectoryForLocation(EMULATOR_ROM_CPCPLUS_DIR, LocalDirectory);

		strcpy(LocalDirectory,ProgramDirectory);
		strcat(LocalDirectory,"\\roms\\kcc");

		SetDirectoryForLocation(EMULATOR_ROM_KCCOMPACT_DIR, LocalDirectory);

	}


	{
		HINSTANCE	hInstance;

		PreviousMessageHandler = (WNDPROC)GetWindowLong(ApplicationHwnd, GWL_WNDPROC);

		SetWindowLong(ApplicationHwnd, GWL_WNDPROC,(LONG)Interface_MessageHandler);

		hInstance = (HINSTANCE)GetWindowLong(ApplicationHwnd,GWL_HINSTANCE);
		
		hAppMenu = LoadMenu(hInstance,MAKEINTRESOURCE(IDR_MENU1));
	}
	{

		ModeMenu = CreatePopupMenu();
		AppendMenu(hAppMenu,MF_POPUP,(UINT)ModeMenu,"&Modes");
	

		DD_BuildModeMenu(Interface_AddResolutionToMenu);


	
	}

	Debug_SetDebuggerRefreshCallback(Debugger_UpdateDisplay);
	

	Debugger_Initialise(ApplicationHwnd);

	GenericInterface_Initialise();

#ifdef CRTC_DEBUG
	/* set state of debug menu */
	Interface_SetItemCheckState(hAppMenu, ID_DEBUG_CRTCDEBUG, Debug_Active(CRTC_Debug));
#endif
	
#ifdef AUDIOEVENT_DEBUG
	Interface_SetItemCheckState(hAppMenu, ID_DEBUG_AUDIOEVENTDEBUGLOG, Debug_Active(AudioEvent_Debug));
#endif
	
#ifdef FDC_DEBUG
	Interface_SetItemCheckState(hAppMenu, ID_DEBUG_FDCDEBUG, Debug_Active(FDC_Debug));
#endif
	
#ifdef ASIC_DEBUG
	Interface_SetItemCheckState(hAppMenu, ID_DEBUG_ASICDEBUG, Debug_Active(ASIC_Debug));
#endif

	/* initialise handling of zip archives */
	ZipHandle_Init();

	/* store state of Scroll Lock LED */
	ScrollLock_StoreState();


	
	CPCEMU_SetupPalette();


	if (CPCEmulation_Initialise())
	{
		CPCEMU_SetWindowed();

		GenericInterface_RestoreSettings();

		Interface_SetupCRTCTypeMenu(CPC_GetCRTCType());
		Interface_SetupCPCTypeMenu(CPC_GetCPCType());

//#ifndef _DEBUG
		//AudioEnable(TRUE);
		CPC_SetAudioActive(TRUE);
//#endif

	//	Breakpoints_AddBreakpoint(0);

//		Multiface_EnableEmulation(TRUE);
//		Multiface_LoadRom(MULTIFACE_ROM_CPC_VERSION, "f:\\arnkcc2\\roms\\multface.rom");
//		Multiface_LoadRom(MULTIFACE_ROM_CPCPLUS_VERSION, "f:\\arnkcc2\\roms\\mface+.rom");

		/* run emulation */
		CPCEmulation_Run();

		/* store settings */
		GenericInterface_StoreSettings();

		Interface_RemoveDisk(0);
		Interface_RemoveDisk(1);

		//AudioEnable(FALSE);
		//CPC_SetAudioActive(FALSE);

		/* close CPC emulation */
		CPCEmulation_Finish();

		/* close generic interface */
		GenericInterface_Finish();
	}

	/* restore state of scroll lock LED */
	ScrollLock_RestoreState();

	/* finish handling zip archives */
	ZipHandle_Finish();

	DirStuff_Finish();

	Interface_FreeCheatDatabase();
}


/*----------------------------------------------------------------------*/


BOOL	OS_LoadFile(char *, unsigned char **, unsigned long *);
BOOL	OS_SaveFile(char *, unsigned char *,unsigned long);

// open file in Windows way
BOOL OS_LoadFile(char *pFilename, unsigned char **ppLocation, unsigned long *pLength)
{
	HANDLE FileHandle;
	DWORD FileLength;
	DWORD FileLengthRead;

	*ppLocation = NULL;
	*pLength = 0;

	// attempt to open file
	FileHandle = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (FileHandle!=INVALID_HANDLE_VALUE)
	{
		// file opened ok

		BY_HANDLE_FILE_INFORMATION FileInformation;

		if (GetFileInformationByHandle(FileHandle, &FileInformation))
		{
			// got file information

			FileLength = FileInformation.nFileSizeLow;

			// if file is not zero size
			if ((FileInformation.nFileSizeLow | FileInformation.nFileSizeHigh)!=0)
			{
				void	*pFileMemory;

				// allocate some memory (fixed, non-moveable)
				pFileMemory = (void *)GlobalAlloc(0,FileLength);
	
				if (pFileMemory!= NULL)
				{
					if (ReadFile(FileHandle, (LPVOID)pFileMemory, FileLength, &FileLengthRead, NULL))
					{
						*ppLocation = pFileMemory;
						*pLength = FileLength;

						CloseHandle(FileHandle);
						
						return TRUE;
					}

					GlobalFree((HGLOBAL)pFileMemory);
				}

			}
		}

		CloseHandle(FileHandle);
	}

	return FALSE;
}


// open file in Windows way
BOOL OS_SaveFile(char *pFilename, unsigned char *pLocation, unsigned long Length)
{
	HANDLE FileHandle;
	DWORD FileLengthWritten;

	// attempt to open file
	FileHandle = CreateFile(pFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);

	if (FileHandle!=INVALID_HANDLE_VALUE)
	{
	
		if (WriteFile(FileHandle, (LPVOID)pLocation, Length, &FileLengthWritten, NULL))
		{
			CloseHandle(FileHandle);

			return TRUE;
		}

		CloseHandle(FileHandle);
	}

	return FALSE;
}



/*
static int RelativePriority[]=
{
	THREAD_PRIORITY_LOWEST,				// -2
	THREAD_PRIORITY_BELOW_NORMAL,		// -1
	THREAD_PRIORITY_NORMAL,				// 0
	THREAD_PRIORITY_ABOVE_NORMAL,		// +1 
	THREAD_PRIORITY_HIGHEST,			// +2
};

void	SetTaskPriority(int PriorityIndex)
{
	int PriorityClass;
	int PriorityBase;
	int PriorityLevel;
	HPROCESS hProcess;
	HTHREAD hThread;

	if (PriorityIndex<0)
	{
		PriorityIndex = 1;
	}

	if (PriorityIndex>32)
	{
		PriorityIndex = 32;
	}


	if ((PriorityIndex>=2) && (PriorityIndex<=6))
	{
		PriorityClass = IDLE_PRIORITY_CLASS;
		PriorityBase = 4;
	}

	if ((PriorityIndex>=7) && (PriorityIndex<=11))
	{
		PriorityClass = NORMAL_PRIORITY_CLASS;
		PriorityBase = 9;
	}

	if ((PriorityIndex>=12) && (PriorityIndex<=
	{
		PriorityClass = HIGH_PRIORITY_CLASS;
		PriorityBase = 13;
	}

	if (PriorityIndex>=24)
	{
		PriorityClass = REALTIME_PRIORITY_CLASS;
		PriorityBase = 24;
	}

	hProcess = GetCurrentProcess()
	hThread = GetCurrentThread();

	if (PriorityIndex!=1)
	{
		SetPriorityClass(hProcess, PriorityClass);

		PriorityLevel = PriorityIndex - PriorityBase;

		SetThreadPriority(hThread, PriorityLevel);
	}
	else
	{
		SetPriorityClass(hProcess, IDLE_PRIORITY_CLASS);
		SetThreadPriority(hThread, THREAD_PRIORITY_IDLE);
	}
}
*/

void	CPCEMU_DetectFileAndLoad(char *pFilename)
{
	/* if snapshot load it and execute it */
	if (GenericInterface_LoadSnapshot(pFilename))
	{
		/* update menu so it reflects CPC type selected */
		Interface_SetupCPCTypeMenu(CPC_GetCPCType());
		return;
	}

	/* if cartridge load it and execute it */
	if (GenericInterface_InsertCartridge(pFilename))
	{
		/* update menu so it reflects CPC type selected */
		Interface_SetupCPCTypeMenu(CPC_GetCPCType());
		return;
	}
}

/*-------------------------------------------------------------------*/
#include "mylistvw.h"
#include "..\cpc\cheatdb.h"
#include <commctrl.h>

HWND hListView = NULL;

CHEAT_DATABASE *pCheatDatabase = NULL;

void	Interface_FreeCheatDatabase()
{
	if (pCheatDatabase!=NULL)
	{
		CheatDatabase_Delete(pCheatDatabase);
		pCheatDatabase = NULL;
	}
}


void	InitialiseListView()
{
	int RowIndex = 0;
	CHEAT_DATABASE_ENTRY *pCurrentEntry;

	if ((pCheatDatabase!=NULL) && (hListView!=NULL))
	{
		// ensure list-view is empty
		ListView_DeleteAllItems(hListView);

		pCurrentEntry = pCheatDatabase->pFirst;

		while (pCurrentEntry!=NULL)
		{
			char *TypeString;

			/* add name to list view */
			MyListView_AddItem(hListView, pCurrentEntry->Name, 0, RowIndex,pCurrentEntry);
			/* add size text to list view */
			MyListView_AddItem(hListView, pCurrentEntry->Description, 1, RowIndex,NULL);
			/* add name to list view */

			switch (toupper(pCurrentEntry->Type))
			{
				case 'T':
				{
					TypeString = "Tape";
				}
				break;

				case 'D':
				{
					TypeString = "Disk";
				}
				break;

				default:
				case 'A':
				{
					TypeString = "All";
				}
				break;

				case 'C':
				{
					TypeString = "Disk Image";
				}
				break;
			}

			MyListView_AddItem(hListView, TypeString, 2, RowIndex,NULL);

			RowIndex++;
			pCurrentEntry = pCurrentEntry->pNext;
		}
	}
}
	

// Insert a unformatted disk image into an Amstrad drive
BOOL	Interface_OpenCheatDatabase(void)
{
	OPENFILENAME	CheatDatabaseOpenFilename;
	HWND			hwnd = GetActiveWindow();

	InitFileDlg(hwnd,&CheatDatabaseOpenFilename,"*.dbf","Cheat Database (*.dbf)\0*.dbf\0All files (*.*)\0*.*\0\0",0);
	if (GetFileNameFromDlg(hwnd,&CheatDatabaseOpenFilename,NULL,"Open Cheat Database",FilenameBuffer, ""))
	{
		if (strlen(FilenameBuffer)!=0)
		{
			// free previous cheat database
			Interface_FreeCheatDatabase();

			// open new cheat-database
			pCheatDatabase = CheatDatabase_Read(FilenameBuffer);

			InitialiseListView();

		}

	}

	return TRUE;
}

BOOL	CALLBACK CheatDatabase_Dialog_Proc(HWND hDialog, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
		case WM_INITDIALOG:
		{
			hListView = GetDlgItem(hDialog,IDC_LIST_CHEATS);
		
			if (hListView!=0)
			{
				MyListView_AddColumn(hListView, "Name",0);
				MyListView_AddColumn(hListView, "Description",1);
				MyListView_AddColumn(hListView, "Type",2);
			}

			InitialiseListView();
		}
		break;

		case WM_NOTIFY:
		{
			NMHDR FAR *pnmh = (NMHDR FAR *)lParam;


		}
		break;


		case WM_COMMAND:
		{
			// depending on ID selected
			switch (LOWORD(wParam))
			{
				case ID_OPEN_DATABASE:
				{
					Interface_OpenCheatDatabase();
				}
				break;

				case ID_POKE:
				{
					int i;
					int Count;

					Count = ListView_GetItemCount(hListView);

					for (i=0; i<Count; i++)
					{
						if (ListView_GetItemState(hListView, i, LVIS_SELECTED)==LVIS_SELECTED)
						{
							CHEAT_DATABASE_ENTRY *pEntry;
							LV_ITEM Item;

							Item.mask = LVIF_PARAM;
							Item.iItem = i;
							Item.iSubItem = 0;
							ListView_GetItem(hListView, &Item);

							pEntry = (CHEAT_DATABASE_ENTRY *)Item.lParam;

							if (pEntry!=NULL)
							{
								CheatDatabase_Poke(pEntry);
							}
						}

					}
				}
				break;
			}
		}
		break;

		case WM_CLOSE:
			DestroyWindow(hDialog);
			break;


		case WM_DESTROY:
		{
			if (hListView!=0)
			{
				ListView_DeleteAllItems(hListView);
				hListView = 0;
			}
		}
		break;

	}

	return FALSE;
}

void	CheatDatabaseDialog(HWND hParent)
{
	HWND hDialog;

	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE);

	hDialog = CreateDialog (hInstance, MAKEINTRESOURCE(IDD_DIALOG_CHEAT_DATABASE), 0, CheatDatabase_Dialog_Proc);

	ShowWindow(hDialog,TRUE);
}


/*
static HWND ZipDialog_TreeViewControl;

BOOL	ZipDialog_DialogProc(HWND hDialog, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
		case WM_INITDIALOG:
		{
			// get tree view 
			ZipDialog_TreeViewControl = GetDialogItem(IDC_TREE_ZIP);

		}
		break;
	}
}
*/

/* open zip dialog */
/*void	ZipDialog_Open(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE);
		
	if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_ZIP),
			hParent, 	ZipDialog_DialogProc))
	{


	}
}
*/

/*********************************************************************/

BOOL	CALLBACK YM5_Dialog_Proc(HWND hDialog, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
		case WM_INITDIALOG:
		{
			char *Text;

			Text = YMOutput_GetName();

			if (Text!=NULL)
			{
				SetDlgItemText(hDialog, IDC_EDIT_NAME, Text);
			}
			
			Text = YMOutput_GetAuthor();

			if (Text!=NULL)
			{
				SetDlgItemText(hDialog, IDC_EDIT_AUTHOR, Text);
			}
			
			Text = YMOutput_GetComment();

			if (Text!=NULL)
			{
				SetDlgItemText(hDialog, IDC_EDIT_COMMENT, Text);
			}
		}
		break;

		case WM_COMMAND:
		{
			// depending on ID selected
			switch (LOWORD(wParam))
			{
				case IDOK:
				{
					char	DialogItemTextBuffer[256];

					/* ok selected */

					/* set name */
					GetDlgItemText(hDialog, IDC_EDIT_NAME, DialogItemTextBuffer, 256);
					YMOutput_SetName(DialogItemTextBuffer);

					/* set author */
					GetDlgItemText(hDialog, IDC_EDIT_AUTHOR, DialogItemTextBuffer, 256);
					YMOutput_SetAuthor(DialogItemTextBuffer);

					/* set comment */
					GetDlgItemText(hDialog, IDC_EDIT_COMMENT, DialogItemTextBuffer, 256);
					YMOutput_SetComment(DialogItemTextBuffer);

					EndDialog(hDialog, 0);
				}
				break;

				case IDCANCEL:
				{
					/* cancel selected */
					EndDialog(hDialog, -1);
				}
				break;
			}
		}
		break;

		case WM_CLOSE:
			DestroyWindow(hDialog);
			break;

	}

	return FALSE;
}

void	YM5_PropertiesDialog(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE);

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_YM5), hParent, YM5_Dialog_Proc);
}
