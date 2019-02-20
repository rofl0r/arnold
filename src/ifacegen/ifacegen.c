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
#include "fnp.h"
#include "romfn.h"
#include "global.h"
#include "../cpc/cpc.h"
#include "settings.h"
#include "../cpc/diskimage/diskimg.h"
#include <string.h>
#include <ctype.h>

static int SnapshotVersion = 3;
static int SnapshotSize = 64;

FILENAME_AND_PATH	DiskImage;
FILENAME_AND_PATH	Cartridge;
FILENAME_AND_PATH	Snapshot;
FILENAME_AND_PATH	Rom;
FILENAME_AND_PATH	ScreenSnapshot;
FILENAME_AND_PATH	WavOutput;
FILENAME_AND_PATH	AYOutput;
FILENAME_AND_PATH	DriveADiskImage;
FILENAME_AND_PATH	DriveBDiskImage;
FILENAME_AND_PATH	TapeImage;
FILENAME_AND_PATH	Multiface_CPC_ROM;
FILENAME_AND_PATH	Multiface_CPCPLUS_ROM;

void	GenericInterface_Initialise(void)
{
	FilenameAndPath_Initialise(&DiskImage);
	FilenameAndPath_Initialise(&Cartridge);
	FilenameAndPath_Initialise(&Snapshot);
	FilenameAndPath_Initialise(&Rom);
	FilenameAndPath_Initialise(&ScreenSnapshot);
	FilenameAndPath_Initialise(&WavOutput);
	FilenameAndPath_Initialise(&AYOutput);
	FilenameAndPath_Initialise(&DriveADiskImage);
	FilenameAndPath_Initialise(&DriveBDiskImage);
	FilenameAndPath_Initialise(&TapeImage);
	FilenameAndPath_Initialise(&Multiface_CPC_ROM);
	FilenameAndPath_Initialise(&Multiface_CPCPLUS_ROM);

	RomFilenames_Initialise();
}


void	GenericInterface_Finish(void)
{
	RomFilenames_Finish();

	FilenameAndPath_Finish(&TapeImage);
	FilenameAndPath_Finish(&DriveADiskImage);
	FilenameAndPath_Finish(&DriveBDiskImage);
	FilenameAndPath_Finish(&DiskImage);
	FilenameAndPath_Finish(&Cartridge);
	FilenameAndPath_Finish(&Snapshot);
	FilenameAndPath_Finish(&Rom);
	FilenameAndPath_Finish(&ScreenSnapshot);
	FilenameAndPath_Finish(&WavOutput);
	FilenameAndPath_Finish(&AYOutput);
	FilenameAndPath_Finish(&Multiface_CPC_ROM);
	FilenameAndPath_Finish(&Multiface_CPCPLUS_ROM);

	GenericInterface_RemoveDiskImage(0);
	GenericInterface_RemoveDiskImage(1);
}


/* remove disk image in drive - do not save out if dirty */
void GenericInterface_RemoveDiskImage(int Drive)
{
	if (Drive==0)
	{
/*		FilenameAndPath_SetPath(&DriveADiskImage,NULL); */
		FilenameAndPath_SetFilename(&DriveADiskImage, NULL);
	}
	else
	{
/*		FilenameAndPath_SetPath(&DriveBDiskImage,NULL); */
		FilenameAndPath_SetFilename(&DriveBDiskImage, NULL);
	}

	/* remove disk image from drive */
	DiskImage_RemoveDisk(Drive);
	FDD_InsertDisk(Drive,FALSE);
}


void	StorePathAndFilenameAsSingleStringInSettings(char *SettingsKey, FILENAME_AND_PATH *pFilenameAndPath)
{
	/* path and filename must both be specified */
	if ((pFilenameAndPath->pPath!=NULL) && (pFilenameAndPath->pFilename!=NULL))
	{
		char *PathAndFilenameString;

		/* join path and filename into a single string to give absolute path */
		PathAndFilenameString = BuildPathAndFilenameFromPathAndFilename(FilenameAndPath_GetPath(pFilenameAndPath), FilenameAndPath_GetFilename(pFilenameAndPath));
	
		if (PathAndFilenameString!=NULL)
		{
			/* store string in settings */
			Settings_StoreString(SettingsKey, PathAndFilenameString);
	
			/* free string holding absolute path */
			free(PathAndFilenameString);
		}
	}
	else
	{
		/* something went wrong, store a null string */
		Settings_StoreString(SettingsKey, "");
	}
}


/* insert disk image into drive */
BOOL	GenericInterface_InsertDiskImage(int DriveIndex, char *pFilename)
{
	GenericInterface_RemoveDiskImage(DriveIndex);

	if (pFilename!=NULL)
	{
		if (strlen(pFilename)!=0)
		{
			if (DiskImage_InsertDisk(DriveIndex,DISK_FORMATTED, pFilename))
			{
				char *pPath, *pFile;

				/* get path */
				pPath =GetPathFromPathAndFilename(pFilename);
				
				if (DriveIndex==0)
				{
					FilenameAndPath_SetPath(&DriveADiskImage, pPath);
				}
				else
				{
					FilenameAndPath_SetPath(&DriveBDiskImage, pPath);
				}

				/* set disc image path */
				FilenameAndPath_SetPath(&DiskImage, pPath);

				/* get filename */
				pFile = GetFilenameFromPathAndFilename(pFilename);
	
				if (DriveIndex==0)
				{
					FilenameAndPath_SetFilename(&DriveADiskImage, pFile);
				}
				else
				{
					FilenameAndPath_SetFilename(&DriveBDiskImage,pFile);
				}

				FDD_InsertDisk(DriveIndex,TRUE);

				return TRUE;
			}
			else
			{
				FDD_InsertDisk(DriveIndex,FALSE);
				return FALSE;
			}
		}
	}

	return FALSE;
}



void GenericInterface_InsertUnformattedDisk(int Drive, char *pFilename)
{
	/* remove existing disk */
	GenericInterface_RemoveDiskImage(Drive);

	/* insert unformatted disk */
	DiskImage_InsertDisk(Drive,DISK_UNFORMATTED, pFilename);
	
	/* inserted */
	FDD_InsertDisk(Drive,TRUE);
}

BOOL	GenericInterface_LoadRom(int RomIndex, char *FilenameBuffer)
{
	if (FilenameBuffer!=NULL)
	{
		if (strlen(FilenameBuffer)!=0)
		{
			if (ExpansionRom_Insert(FilenameBuffer, RomIndex))
			{
				char *pPath;
				char *pFile;

				pPath = GetPathFromPathAndFilename(FilenameBuffer);
				FilenameAndPath_SetPath(&Rom, pPath);
				
				
				pFile = GetFilenameFromPathAndFilename(FilenameBuffer);
				FilenameAndPath_SetFilename(&Rom, pFile);

				RomFilenames_SetFilename(RomIndex, FilenameBuffer);
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
		char SettingsKey[32];
		char *pCurrentRomFilename;
		
		/* setup settings key */
		sprintf(SettingsKey, "sRom%2d", i);
		
		pCurrentRomFilename = Settings_GetString(SettingsKey);
	
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
					sprintf(SettingsKey,"iRom%2dActive",i);

					/* get active state */
					ActiveFlag = Settings_GetInt(SettingsKey);

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

static  char *EmptyString = "";

void	GenericInterface_StoreRomSettings(void)
{
	int i;

	for (i=1; i<17; i++)
	{
		char SettingsKey[32];
		char *pCurrentRomFilename;
				
		/* get filename of rom in slot i */
		pCurrentRomFilename = RomFilenames_GetFilename(i);
			
		/* initialise settings key */
		sprintf(SettingsKey, "sRom%2d", i);
		
		if (pCurrentRomFilename!=NULL)
		{
			/* store filename if specified - i.e. rom was inserted */
			Settings_StoreString(SettingsKey, pCurrentRomFilename);

			/* store if rom is active or not */
			{
				sprintf(SettingsKey,"iRom%2dActive", i);

				Settings_StoreInt(SettingsKey, ExpansionRom_IsActive(i));
			}
		}
		else
		{
			/* store empty string, no rom specified */
			Settings_StoreString(SettingsKey, EmptyString);
		}
	}
}

BOOL	GenericInterface_LoadSnapshot(char *FilenameBuffer)
{
	if (FilenameBuffer!=NULL)
	{
		if (strlen(FilenameBuffer)!=0)
		{
			char *pPath, *pFile;

			pPath = GetPathFromPathAndFilename(FilenameBuffer);

			FilenameAndPath_SetPath(&Snapshot, pPath);

			if (Snapshot_Load(FilenameBuffer))
			{
				pFile = GetFilenameFromPathAndFilename(FilenameBuffer);
				FilenameAndPath_SetFilename(&Snapshot, pFile);
			
				return TRUE;
			}
		}
	}

	return FALSE;
}

void	GenericInterface_SetSnapshotVersion(int Version)
{
	SnapshotVersion = Version;
}

void	GenericInterface_SetSnapshotSize(int Size)
{
	SnapshotSize = Size;
}


BOOL	GenericInterface_SnapshotSave(char *FilenameBuffer)
{
	char *pPath;

	pPath = GetPathFromPathAndFilename(FilenameBuffer);

	FilenameAndPath_SetPath(&Snapshot, pPath);
	
	Snapshot_Save(FilenameBuffer,SnapshotSize,SnapshotVersion);

	return TRUE;
}


BOOL	GenericInterface_SaveScreenSnapshot(char *FilenameBuffer)
{
	if (FilenameBuffer!=NULL)
	{
		if (strlen(FilenameBuffer)!=0)
		{
			char *pPath;

			pPath = GetPathFromPathAndFilename(FilenameBuffer);
			FilenameAndPath_SetPath(&ScreenSnapshot, pPath);
		
			Screen_SaveSnapshot(FilenameBuffer);
		
			return TRUE;
		}
	}

	return FALSE;
}

BOOL	GenericInterface_WavOutputStartRecording(char *FilenameBuffer)
{
	if (FilenameBuffer!=NULL)
	{
		if (strlen(FilenameBuffer)!=0)
		{
			char *pPath;

			pPath = GetPathFromPathAndFilename(FilenameBuffer);

			FilenameAndPath_SetPath(&WavOutput, pPath);

			WavOutput_StartRecording(FilenameBuffer);

			return TRUE;
		}
	}

	return FALSE;
}


BOOL	GenericInterface_InsertTapeImage(char *FilenameBuffer)
{
	if (FilenameBuffer!=NULL)
	{
		if (strlen(FilenameBuffer)!=0)
		{
			char *pPath,  *pFile;

			pPath = GetPathFromPathAndFilename(FilenameBuffer);

			FilenameAndPath_SetPath(&TapeImage, pPath);

			if (TapeImage_Insert(FilenameBuffer))
			{
				pFile = GetFilenameFromPathAndFilename(FilenameBuffer);
				FilenameAndPath_SetFilename(&TapeImage, pFile);
			
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL	GenericInterface_InsertCartridge(char *FilenameBuffer)
{
	if (FilenameBuffer!=NULL)
	{
		if (strlen(FilenameBuffer)!=0)
		{
			char *pPath, *pFile;

			pPath = GetPathFromPathAndFilename(FilenameBuffer);
			FilenameAndPath_SetPath(&Cartridge, pPath);
				
			/* load cartridge */
			if (Cartridge_Load(FilenameBuffer))
			{
				pFile = GetFilenameFromPathAndFilename(FilenameBuffer);
				FilenameAndPath_SetFilename(&Cartridge, pFile);
				return TRUE;
			}
		}
	}

	/* cartridge failed to load */
	return FALSE;
}

void	GenericInterface_AYStartRecording(char *FilenameBuffer)
{
	int YMVersion;

	if (FilenameBuffer!=NULL)
	{
		if (strlen(FilenameBuffer)!=0)
		{
			char *pPath;

			pPath = GetPathFromPathAndFilename(FilenameBuffer);

			FilenameAndPath_SetPath(&AYOutput, pPath);

			{
				char *pFilename = GetFilenameFromPathAndFilename(FilenameBuffer);
				char *pExt = GetExtensionFromFilename(pFilename);

				if ((toupper(pExt[0])=='Y') &&
(toupper(pExt[1])=='M') && (pExt[3]='5'))
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
		if (Settings_IsKeyPresent("iCPCType"))
		{
			CPC_SetCPCType(Settings_GetInt("iCPCType"));
		}

		/* restore CRTC type */
		if (Settings_IsKeyPresent("iCRTCType"))
		{
			CPC_SetCRTCType(Settings_GetInt("iCRTCType"));
		}

		/* restore monitor type */
		if (Settings_IsKeyPresent("iMonitorType"))
		{
			CPC_SetMonitorType(Settings_GetInt("iMonitorType"));
		}
		
		/* restore monitor brightness */
		if (Settings_IsKeyPresent("iMonitorBrightness"))
		{
			CPC_SetMonitorBrightness(Settings_GetInt("iMonitorBrightness"));
		}

		/* restore printer output method */
		if (Settings_IsKeyPresent("iPrinterOutputMethod"))
		{
			Printer_SetOutputMethod(Settings_GetInt("iPrinterOutputMethod"));
		}

		/* restore frame skip settings */
		if (Settings_IsKeyPresent("iFrameSkip"))
		{
			CPC_SetFrameSkip(Settings_GetInt("iFrameSkip"));
		}

		/* restore cartridge path */
		if (Settings_IsKeyPresent("sCartridgePath"))
		{
			FilenameAndPath_SetPath(&Cartridge, Settings_GetString("sCartridgePath"));
		}

		/* restore disk image path */
		if (Settings_IsKeyPresent("sDiskImagePath"))
		{
			FilenameAndPath_SetPath(&DiskImage, Settings_GetString("sDiskImagePath"));
		}

		/* restore screen snapshot path */
		if (Settings_IsKeyPresent("sScreenSnapshotPath"))
		{
			FilenameAndPath_SetPath(&ScreenSnapshot, Settings_GetString("sScreenSnapshotPath"));
		}

		/* restore wave output settings */
		if (Settings_IsKeyPresent("sWavOutputPath"))
		{
			FilenameAndPath_SetPath(&WavOutput, Settings_GetString("sWavOutputPath"));
		}

		/* restore snapshot path */
		if (Settings_IsKeyPresent("sSnapshotPath"))
		{
			FilenameAndPath_SetPath(&Snapshot, Settings_GetString("sSnapshotPath"));
		}

		/* restore AY/PSG output path */
		if (Settings_IsKeyPresent("sAYOutputPath"))
		{
			FilenameAndPath_SetPath(&AYOutput, Settings_GetString("sAYOutputPath"));
		}

		/* restore rom path */
		if (Settings_IsKeyPresent("sRomPath"))
		{
			FilenameAndPath_SetPath(&Rom, Settings_GetString("sRomPath"));
		}

		/* restore tape image path */
		if (Settings_IsKeyPresent("sTapeImagePath"))
		{
			FilenameAndPath_SetPath(&TapeImage, Settings_GetString("sTapeImagePath"));
		}

		/* restore roms */
		GenericInterface_RestoreRomSettings();

		/* restore disk images */
		GenericInterface_InsertDiskImage(0, Settings_GetString("sDriveADiskImage"));
		GenericInterface_InsertDiskImage(1, Settings_GetString("sDriveBDiskImage"));
		
		Settings_CloseSettings();
	}
}


void	GenericInterface_StoreSettings(void)
{
	/* store current settings */
	if (Settings_OpenSettings(SETTINGS_WRITE_MODE))
	{
		/* store CPC type */
		Settings_StoreInt("iCPCType", CPC_GetCPCType());
		/* store CRTC type */
		Settings_StoreInt("iCRTCType", CPC_GetCRTCType());
		/* store monitor type */
		Settings_StoreInt("iMonitorType", CPC_GetMonitorType());
		/* store monitor brightness */
		Settings_StoreInt("iMonitorBrightness", CPC_GetMonitorBrightness());
		/* store printer output method */
		Settings_StoreInt("iPrinterOutputMethod", Printer_GetOutputMethod());
		/* store frame skip */
		Settings_StoreInt("iFrameSkip", CPC_GetFrameSkip());
		/* store cartridge path */
		Settings_StoreString("sCartridgePath", FilenameAndPath_GetPath(&Cartridge));
		/* store disk image path */
		Settings_StoreString("sDiskImagePath", FilenameAndPath_GetPath(&DiskImage));
		/* store screen snapshot path */
		Settings_StoreString("sScreenSnapshotPath", FilenameAndPath_GetPath(&ScreenSnapshot));
		/* store snapshot path */
		Settings_StoreString("sSnapshotPath", FilenameAndPath_GetPath(&Snapshot));
		/* store wave output path */
		Settings_StoreString("sWavOutputPath", FilenameAndPath_GetPath(&WavOutput));
		/* store AY output path */
		Settings_StoreString("sAYOutputPath", FilenameAndPath_GetPath(&AYOutput));
		/* store tape image path */
		Settings_StoreString("sTapeImagePath", FilenameAndPath_GetPath(&TapeImage));
		/* store full path and filename for disk image that has been inserted */
		StorePathAndFilenameAsSingleStringInSettings("sDriveADiskImage", &DriveADiskImage);
		/* store full path and filename for disk image that has been inserted */
		StorePathAndFilenameAsSingleStringInSettings("sDriveBDiskImage", &DriveBDiskImage);
		/* store full path and filename for cartridge that has been inserted */
		StorePathAndFilenameAsSingleStringInSettings("sCartridge", &Cartridge);
		/* store rom path */
		Settings_StoreString("sRomPath", FilenameAndPath_GetPath(&Rom));
		/* store roms that were loaded */
		GenericInterface_StoreRomSettings();
		/* close settings */
		Settings_CloseSettings();
	}
}

void	GenericInterface_DoReset(void)
{
	CPC_Reset();
}

