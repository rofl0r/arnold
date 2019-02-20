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
/* generic interface code */
#include "ifacegen.h"
#include "filetool.h"
#include "global.h"
#include "../cpc/cpc.h"
#include "../cpc/diskimage/diskimg.h"
#include <string.h>
#include <ctype.h>
#include "../cpc/host.h"
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "../cpc/snapshot.h"
#include "../cpc/tzx.h"

#ifndef WIN32
#define _stprintf sprintf
#endif

TCHAR *DiskImagePath;			/* path for opening disk images */
TCHAR *CartridgePath;			/* path for opening cartridges */
TCHAR *SnapshotPath;			/* path for opening snapshots */
TCHAR *RomPath;					/* path for opening roms */
TCHAR *ScreenSnapshotPath;		/* path for writing screen snapshots */
TCHAR *WavOutputPath;			/* path for writing wav recording of sound */
TCHAR *AYOutputPath;			/* path for writing recording of ay */
TCHAR *TapePath;				/* path for opening tapes */

TCHAR *RomFilenames[17];		/* filenames of opened roms */
TCHAR *CartridgeFilename;		/* filename of last opened cartridge */
TCHAR *SnapshotFilename;		/* filename of last opened/saved snapshot */
TCHAR *WavOutputFilename;		/* filename of last wav recording */
TCHAR *AYOutputFilename;		/* filename of last AY recording */
TCHAR *DriveAFilename;			/* filename of last disk image opened for drive A */
TCHAR *DriveBFilename;			/* filename of last disk image opened for drive B */
TCHAR *TapeFilename;			/* filename of last tape opened */
TCHAR *Multiface_CPC_ROM_Filename;	/* filename of CPC version of multiface rom */
TCHAR *Multiface_CPCPLUS_ROM;		/* filename of CPC+ version of multiface rom */

/*------------------------------------------------------------------------------------------*/
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
/*------------------------------------------------------------------------------------------*/

int Multiface_LoadRomFromFile(const MULTIFACE_ROM_TYPE RomType, const TCHAR *pFilename)
{
    unsigned long   RomSize;
    unsigned char   *pRomData;
	int Status;

    /* attempt to load rom data */
    LoadFile((TCHAR *)pFilename,&pRomData, &RomSize);

	if (pRomData!=NULL)
	{
		Status = Multiface_SetRomData(RomType, pRomData, RomSize);

		free(pRomData);
	}

	return Status;
}
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
	DriveAFilename = NULL;			
	DriveBFilename = NULL;			
	TapeFilename = NULL;			
	Multiface_CPC_ROM_Filename = NULL;	
	Multiface_CPCPLUS_ROM = NULL;		

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
	SetString(&DriveAFilename,NULL);			
	SetString(&DriveBFilename,NULL);			
	SetString(&TapeFilename,NULL);			
	SetString(&Multiface_CPC_ROM_Filename,NULL);	
	SetString(&Multiface_CPCPLUS_ROM,NULL);		

	for (i=0; i<17; i++)
	{
		SetString(&RomFilenames[i],NULL);
	}
}

BOOL DiskImage_InsertFormattedDisk(int DriveID, const TCHAR *Filename)
{
	unsigned char *pDiskImage;
	unsigned long DiskImageLength;

    /* load disk image file to memory */
    LoadFile(Filename, &pDiskImage, &DiskImageLength);

	if (pDiskImage!=NULL)
	{
		BOOL bStatus;

		bStatus = DiskImage_InsertDisk(DriveID, pDiskImage, DiskImageLength);

		free(pDiskImage);
	
		return bStatus;
	}

	return FALSE;
}


/* insert disk image into drive */
BOOL	GenericInterface_InsertDiskImage(int DriveIndex, const TCHAR *pFilename)
{
	if (pFilename!=NULL)
	{
		if (strlen(pFilename)!=0)
		{
			if (DiskImage_InsertFormattedDisk(DriveIndex,pFilename))
			{
				TCHAR *pPath, *pFile;

				/* get path */
				pPath =GetPathFromPathAndFilename(pFilename);
				
				SetString(&DiskImagePath, pPath);

				/* get filename */
				pFile = GetFilenameFromPathAndFilename(pFilename);
	
				if (DriveIndex==0)
				{
					SetString(&DriveAFilename,pFile);
				}
				else
				{
					SetString(&DriveBFilename,pFile);
				}
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL	ExpansionRom_Insert(const TCHAR *FilenameBuffer, const int RomIndex)
{
	unsigned char *pRomData;
	unsigned long RomDataSize;

	LoadFile(FilenameBuffer, &pRomData, &RomDataSize);
	if (pRomData!=NULL)
	{
		
		ExpansionRom_SetRomData(pRomData, RomDataSize, RomIndex);
	
		free(pRomData);

		return TRUE;
	}

	return FALSE;
}

BOOL	GenericInterface_LoadRom(int RomIndex, const TCHAR *FilenameBuffer)
{
	if (FilenameBuffer!=NULL)
	{
		if (strlen(FilenameBuffer)!=0)
		{
			if (ExpansionRom_Insert(FilenameBuffer, RomIndex))
			{
				TCHAR *pPath;
				TCHAR *pFile;

				pPath = GetPathFromPathAndFilename(FilenameBuffer);
				SetString(&RomPath, pPath);
				
				pFile = GetFilenameFromPathAndFilename(FilenameBuffer);
				SetString(&RomFilenames[RomIndex],FilenameBuffer);

				return TRUE;
			}
		}
	}

	return FALSE;
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
				if (GenericInterface_LoadRom(i, pCurrentRomFilename))
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


BOOL	GenericInterface_LoadSnapshot(const TCHAR *FilenameBuffer)
{
	if (FilenameBuffer!=NULL)
	{
		if (strlen(FilenameBuffer)!=0)
		{
			TCHAR *pPath, *pFile;

			pPath = GetPathFromPathAndFilename(FilenameBuffer);

			SetString(&SnapshotPath, pPath);
			
			if (Snapshot_Load(FilenameBuffer))
			{
				pFile = GetFilenameFromPathAndFilename(FilenameBuffer);
			
				SetString(&SnapshotFilename, pFile);

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL	GenericInterface_SnapshotSave(const TCHAR *FilenameBuffer, int SnapshotVersion,int SnapshotSize)
{
	TCHAR *pPath;
	unsigned long nLength;
	unsigned char *pSnapshotData = NULL;

	pPath = GetPathFromPathAndFilename(FilenameBuffer);

	SetString(&SnapshotPath, pPath);
	
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
			TCHAR *pPath;

			pPath = GetPathFromPathAndFilename(FilenameBuffer);
			SetString(&ScreenSnapshotPath, pPath);
			
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
			TCHAR *pPath;

			pPath = GetPathFromPathAndFilename(FilenameBuffer);

			SetString(&WavOutputPath, pPath);

			WavOutput_StartRecording(FilenameBuffer);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL	TapeImage_InsertFromFile(const TCHAR *Filename)
{
	unsigned char *pTapeImage;
	unsigned long TapeImageLength;

	LoadFile(Filename, &pTapeImage, &TapeImageLength);

	if (pTapeImage!=NULL)
	{
		BOOL bStatus;

		bStatus = TapeImage_Insert(pTapeImage, TapeImageLength);

		free(pTapeImage);
	
		return bStatus;
	}

	return FALSE;
}


BOOL	GenericInterface_InsertTape(const TCHAR *FilenameBuffer)
{
	if (FilenameBuffer!=NULL)
	{
		if (strlen(FilenameBuffer)!=0)
		{
			TCHAR *pPath,  *pFile;

			pPath = GetPathFromPathAndFilename(FilenameBuffer);

			SetString(&TapePath, pPath);

			// attempt tape first
			if (TapeImage_InsertFromFile(FilenameBuffer))
			{
				pFile = GetFilenameFromPathAndFilename(FilenameBuffer);
				SetString(&TapeFilename, pFile);
				
				return TRUE;
			}

			// now attempt samples..
			if (Sample_Load(FilenameBuffer))
			{
				pFile = GetFilenameFromPathAndFilename(FilenameBuffer);
				SetString(&TapeFilename, pFile);
			
				return TRUE;
			}

		}
	}

	return FALSE;
}

BOOL	Cartridge_Load(const TCHAR *pFilename)
{
	unsigned char *pCartridgeData;
	unsigned long CartridgeLength;

	LoadFile(pFilename, &pCartridgeData, &CartridgeLength);

	if (pCartridgeData!=NULL)
	{
		int Status;

		Status = Cartridge_Insert(pCartridgeData, CartridgeLength);

		Cartridge_Autostart();

		free(pCartridgeData);
		return (Status==ARNOLD_STATUS_ROM_OK);
	}

	return FALSE;
}

BOOL	GenericInterface_InsertCartridge(const TCHAR *FilenameBuffer)
{
	if (FilenameBuffer!=NULL)
	{
		if (strlen(FilenameBuffer)!=0)
		{
			TCHAR *pPath, *pFile;

			pPath = GetPathFromPathAndFilename(FilenameBuffer);
			SetString(&CartridgePath, pPath);
	
			/* load cartridge */
			if (Cartridge_Load(FilenameBuffer))
			{
				pFile = GetFilenameFromPathAndFilename(FilenameBuffer);
				SetString(&CartridgeFilename, pFile);
				return TRUE;
			}
		}
	}

	/* cartridge failed to load */
	return FALSE;
}

void	GenericInterface_AYStartRecording(const TCHAR *FilenameBuffer)
{
	int YMVersion;

	if (FilenameBuffer!=NULL)
	{
		if (strlen(FilenameBuffer)!=0)
		{
			TCHAR *pPath;

			pPath = GetPathFromPathAndFilename(FilenameBuffer);

			SetString(&AYOutputPath, pPath);

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


void	GenericInterface_RemoveCartridge(void)
{
	/* remove old cartridge */
	Cartridge_Remove();
}

void	GenericInterface_RestoreSettings(void)
{
	/* if no settings file exist, then the default settings will be used.
	otherwise one or more of the default settings may be overridden */

	/* open settings */
	if (Settings_OpenSettings(SETTINGS_READ_MODE))
	{
		/* restore stored settings */

		/* restore CPC type */
//		if (Settings_IsKeyPresent(_T("iCPCType")))
//		{
//			CPC_SetCPCType(MyApp_Registry_GetIntFromCurrentKey(_T("iCPCType")));
//		
//		}

		/* restore CRTC type */
		if (Settings_IsKeyPresent(_T("iCRTCType")))
		{
			CPC_SetCRTCType(MyApp_Registry_GetIntFromCurrentKey(_T("iCRTCType")));
		}

		/* restore monitor type */
		if (Settings_IsKeyPresent(_T("iMonitorType")))
		{
			CPC_SetMonitorType(MyApp_Registry_GetIntFromCurrentKey(_T("iMonitorType")));
		}
		
		/* restore monitor brightness */
		if (Settings_IsKeyPresent(_T("iMonitorBrightness")))
		{
			CPC_SetMonitorBrightness(MyApp_Registry_GetIntFromCurrentKey(_T("iMonitorBrightness")));
		}

		/* restore printer output method */
//		if (Settings_IsKeyPresent(_T("iPrinterOutputMethod")))
//		{
//			Printer_SetOutputMethod(MyApp_Registry_GetIntFromCurrentKey(_T("iPrinterOutputMethod")));
//		}

		/* restore frame skip settings */
		if (Settings_IsKeyPresent(_T("iFrameSkip")))
		{
			CPC_SetFrameSkip(MyApp_Registry_GetIntFromCurrentKey(_T("iFrameSkip")));
		}

		/* restore cartridge path */
		if (Settings_IsKeyPresent(_T("sCartridgePath")))
		{
			SetString(&CartridgePath, MyApp_Registry_GetStringFromCurrentKey(_T("sCartridgePath")));
		}

		/* restore disk image path */
		if (Settings_IsKeyPresent(_T("sDiskImagePath")))
		{
			SetString(&DiskImagePath, MyApp_Registry_GetStringFromCurrentKey(_T("sDiskImagePath")));
		}

		/* restore screen snapshot path */
		if (Settings_IsKeyPresent(_T("sScreenSnapshotPath")))
		{
			SetString(&ScreenSnapshotPath, MyApp_Registry_GetStringFromCurrentKey(_T("sScreenSnapshotPath")));
		}

		/* restore wave output settings */
		if (Settings_IsKeyPresent(_T("sWavOutputPath")))
		{
			SetString(&WavOutputPath, MyApp_Registry_GetStringFromCurrentKey(_T("sWavOutputPath")));
		}

		/* restore snapshot path */
		if (Settings_IsKeyPresent(_T("sSnapshotPath")))
		{
			SetString(&SnapshotPath, MyApp_Registry_GetStringFromCurrentKey(_T("sSnapshotPath")));
		}

		/* restore AY/PSG output path */
		if (Settings_IsKeyPresent(_T("sAYOutputPath")))
		{
			SetString(&AYOutputPath, MyApp_Registry_GetStringFromCurrentKey(_T("sAYOutputPath")));
		}

		/* restore rom path */
		if (Settings_IsKeyPresent(_T("sRomPath")))
		{
			SetString(&RomPath, MyApp_Registry_GetStringFromCurrentKey(_T("sRomPath")));
		}

		/* restore tape image path */
		if (Settings_IsKeyPresent(_T("sTapePath")))
		{
			SetString(&TapePath, MyApp_Registry_GetStringFromCurrentKey(_T("sTapePath")));
		}

		/* restore roms */
		GenericInterface_RestoreRomSettings();

		/* restore disk images */
		GenericInterface_InsertDiskImage(0, MyApp_Registry_GetStringFromCurrentKey(_T("sDriveADiskImage")));
		GenericInterface_InsertDiskImage(1, MyApp_Registry_GetStringFromCurrentKey(_T("sDriveBDiskImage")));
		
			MyApp_Registry_CloseKey();
	}
}


void	GenericInterface_StoreSettings(void)
{
	/* store current settings */
	if (Settings_OpenSettings(SETTINGS_WRITE_MODE))
	{
		/* store CPC type */
//			MyApp_Registry_StoreIntToCurrentKey(_T("iCPCType"), CPC_GetCPCType());
		/* store CRTC type */
			MyApp_Registry_StoreIntToCurrentKey(_T("iCRTCType"), CPC_GetCRTCType());
		/* store monitor type */
			MyApp_Registry_StoreIntToCurrentKey(_T("iMonitorType"), CPC_GetMonitorType());
		/* store monitor brightness */
			MyApp_Registry_StoreIntToCurrentKey(_T("iMonitorBrightness"), CPC_GetMonitorBrightness());
		/* store printer output method */
//			MyApp_Registry_StoreIntToCurrentKey(_T("iPrinterOutputMethod"), Printer_GetOutputMethod());
		/* store frame skip */
			MyApp_Registry_StoreIntToCurrentKey(_T("iFrameSkip"), CPC_GetFrameSkip());
		/* store cartridge path */
		MyApp_Registry_StoreStringToCurrentKey(_T("sCartridgePath"), CartridgePath);
		/* store disk image path */
		MyApp_Registry_StoreStringToCurrentKey(_T("sDiskImagePath"), DiskImagePath);
		/* store screen snapshot path */
		MyApp_Registry_StoreStringToCurrentKey(_T("sScreenSnapshotPath"), ScreenSnapshotPath);
		/* store snapshot path */
		MyApp_Registry_StoreStringToCurrentKey(_T("sSnapshotPath"), SnapshotPath);
		/* store wave output path */
		MyApp_Registry_StoreStringToCurrentKey(_T("sWavOutputPath"), WavOutputPath);
		/* store AY output path */
		MyApp_Registry_StoreStringToCurrentKey(_T("sAYOutputPath"), AYOutputPath);
		/* store tape image path */
		MyApp_Registry_StoreStringToCurrentKey(_T("sTapePath"), TapePath);
		/* store full path and filename for disk image that has been inserted */
		MyApp_Registry_StoreStringToCurrentKey(_T("sDriveADiskImage"), DriveAFilename);
		/* store full path and filename for disk image that has been inserted */
		MyApp_Registry_StoreStringToCurrentKey(_T("sDriveBDiskImage"), DriveBFilename);
		/* store full path and filename for cartridge that has been inserted */
		MyApp_Registry_StoreStringToCurrentKey(_T("sCartridge"), CartridgeFilename);
		/* store rom path */
		MyApp_Registry_StoreStringToCurrentKey(_T("sRomPath"), RomPath);
		/* store roms that were loaded */
		GenericInterface_StoreRomSettings();
		/* close settings */
			MyApp_Registry_CloseKey();
	}
}


