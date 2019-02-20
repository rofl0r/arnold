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
#include <stdlib.h>
#include <string.h>
#include "dumpym.h"
#include "cpcglob.h"
#include "cpcdefs.h"
#include "endian.h"
#include "host.h"

/* current state of registers and info about if
they have changed */
static AY_INFO	AYRegs[16];
static AY_INFO	PreviousAYRegs[16];
static BOOL		AYRegsChanged[16];

/* file handle for temporary file */
static HOST_FILE_HANDLE	fhYMOutput=0;
/* status of recording */
static BOOL	YMOutputEnabled=FALSE;
/* filename for output YM */
static char	*pYMFilename;
/* filename for temp data */
static char	*pYMTempFilename;

/* TRUE if actively recording AY output */
static BOOL YMOutput_Recording = FALSE;
/* TRUE if we should wait until the AY output is not silent before recording */
static BOOL	YMOutput_StartRecordingWhenSilenceEnds = FALSE;

static int YM_Version = 5;

static unsigned char *YM5_SongName = NULL;
static unsigned char *YM5_AuthorName = NULL;
static unsigned char *YM5_Comments = NULL;
static unsigned char YM5_EndFileText[4]="End!";


static unsigned char YM5_Ident_Text[4]="YM5!";
static unsigned char YM5_IdentString_Text[8]="LeOnArD!";

#define YM5_SONG_ATTRIBUTE_DATA_INTERLEAVED	0x0001

#ifdef _WINDOWS
#pragma pack(1)
#endif

typedef struct YM5_HEADER
{
	unsigned long ID;
	unsigned char IDString[8];
	unsigned long NumVBL;
	unsigned long SongAttributes;
	unsigned short NoOfDigiDrumSamples;
	unsigned long YMFrequency;
	unsigned short PlayerFrequency;
	unsigned long VBLLoopIndex;
	unsigned short SizeOfExtraData;
} YM5_HEADER;

#ifdef _WINDOWS
#pragma pack()
#endif

void	YMOutput_SetStartRecordingState(BOOL State)
{
	YMOutput_StartRecordingWhenSilenceEnds = State;
}

unsigned char *YMOutput_GetName(void)
{
	return YM5_SongName;
}

unsigned char *YMOutput_GetAuthor(void)
{
	return YM5_AuthorName;
}

unsigned char *YMOutput_GetComment(void)
{
	return YM5_Comments;
}


void	YMOutput_SetName(unsigned char *pNewName)
{
	int StringLength;

	if (pNewName == NULL)
		return;
	
	if (YM5_SongName!=NULL)
	{
		free(YM5_SongName);
		YM5_SongName = NULL;
	}

	StringLength = strlen((const char *)pNewName);

	if (StringLength==0)
		return;

	YM5_SongName = malloc(StringLength+1);

	if (YM5_SongName!=NULL)
	{
		memcpy(YM5_SongName, pNewName, StringLength+1);
	}
}

void	YMOutput_SetAuthor(unsigned char *pNewAuthor)
{
	int StringLength;

	if (pNewAuthor == NULL)
		return;
	
	if (YM5_AuthorName!=NULL)
	{
		free(YM5_AuthorName);
		YM5_AuthorName = NULL;
	}

	StringLength = strlen((const char *)pNewAuthor);

	if (StringLength==0)
		return;

	YM5_AuthorName = malloc(StringLength+1);

	if (YM5_AuthorName!=NULL)
	{
		memcpy(YM5_AuthorName, pNewAuthor, StringLength+1);
	}
}

void	YMOutput_SetComment(unsigned char *pNewComment)
{
	int StringLength;

	if (pNewComment == NULL)
		return;
	
	if (YM5_Comments!=NULL)
	{
		free(YM5_Comments);
		YM5_Comments = NULL;
	}

	StringLength = strlen((const char *)pNewComment);

	if (StringLength==0)
		return;

	YM5_Comments = malloc(StringLength+1);

	if (YM5_Comments!=NULL)
	{
		memcpy(YM5_Comments, pNewComment, StringLength+1);
	}
}


static void	YMOutput_InitialiseRegisters(void)
{
	int i;

	/* initialise register data */
	for (i=0; i<16; i++)
	{
		/* current */
		AYRegs[i].RegisterData = 0;
		AYRegs[i].Flags = 0;
	
		/* previous */
		PreviousAYRegs[i].RegisterData = 0;
		PreviousAYRegs[i].Flags = 0;

		/* changed flag */
		AYRegsChanged[i] = FALSE;
	}
}


/* dump registers to file */
void	YMOutput_Init(char *pTempFilename)
{
	fhYMOutput = 0;
	YMOutputEnabled = FALSE;

	/* set temp filename */
	pYMTempFilename = (char *)malloc(strlen(pTempFilename)+1);

	if (pYMTempFilename!=NULL)
	{
		strcpy(pYMTempFilename, pTempFilename);
	}

	YMOutput_InitialiseRegisters();

	YMOutput_SetName((unsigned char *)"Untitled");
	YMOutput_SetAuthor((unsigned char *)"Unknown");

}

void	YMOutput_Finish(void)
{
	/* stop any recording */
	YMOutput_StopRecording();

	/* free temp filename */
	if (pYMTempFilename!=NULL)
	{
		free(pYMTempFilename);
	}

	if (YM5_SongName!=NULL)
	{
		free(YM5_SongName);
	}

	if (YM5_AuthorName!=NULL)
	{
		free(YM5_AuthorName);
	}

	if (YM5_Comments!=NULL)
	{
		free(YM5_Comments);
	}
}

/* TRUE if output is silent, FALSE if not */
static BOOL YMOutput_IsSilent(void)
{
	/* what defines silence? */

	/* - if R8, R9 and R10 are all zero, then no noise or tone will be output (noise and
	    tone can be active or inactive)
	   - if tone or noise are not active, we will not get any sound provided that R8,R9,R10
	do not change. 
	   - if tone is active, but tone is in a specific in-audible range, it will be silent.
	   R8, R9, R10 can be any value.
	*/

	if ((AYRegs[8].RegisterData==0) || (AYRegs[9].RegisterData==0) || (AYRegs[10].RegisterData==0))
	{
		/* volume regs are all zero = silence regardless of mixer,
		tone and noise settings. */
		return TRUE;
	}

	/* a volume register is not zero, something *could* be audible */
	if ((!(AYRegsChanged[8] | AYRegsChanged[9] | AYRegsChanged[10])) &&
		((AYRegs[7].RegisterData & 0x03f)==0x03f))
	{
		/* AY Regs haven't changed, and tone and noise are disabled */
		return TRUE;
	}

	return FALSE;
}


/* write regs to temp file */
void	YMOutput_WriteRegs(void)
{
	int i;

	if (YMOutputEnabled==TRUE)
	{
		if (fhYMOutput!=0)
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

			/* if we are recording, then write register data to file */
			if (YMOutput_Recording)
			{
				for (i=0; i<14; i++)
				{
					if (i==13)
					{
						if (!(AYRegs[i].Flags & AY_INFO_REG_CHANGED))
						{
							unsigned char Unchanged = 0x0ff;

							Host_WriteData(fhYMOutput, &Unchanged,sizeof(unsigned char));
						}
						else
						{
							/* write reg data */
							Host_WriteData(fhYMOutput, &AYRegs[13].RegisterData, sizeof(unsigned char));
						}
					}

					if (i!=13)
					{
						/* write reg data */
						Host_WriteData(fhYMOutput,&AYRegs[i].RegisterData, sizeof(unsigned char));
					}
				}
			}
		}
	}

	/* clear changed status */
	YMOutput_ClearRegStatus();
}

void	YMOutput_ClearRegStatus(void)
{
	int i;

	for (i=0; i<16; i++)
	{
		AYRegs[i].Flags = 0;
	}
}

/* record AY/YM output */
void	YMOutput_StoreRegData(int PSG_SelectedRegister, int Data)
{
	if (Data!=AYRegs[PSG_SelectedRegister].RegisterData)
	{
		/* data has changed from previous frame */
		AYRegsChanged[PSG_SelectedRegister] = TRUE;
	}

	/* store current values as previous */
	PreviousAYRegs[PSG_SelectedRegister].RegisterData = 
		AYRegs[PSG_SelectedRegister].RegisterData;

	/* store register data */
	AYRegs[PSG_SelectedRegister].RegisterData = (unsigned char)Data;
	/* mark data as changed */
	AYRegs[PSG_SelectedRegister].Flags |= AY_INFO_REG_CHANGED;
}

void	YMOutput_StartRecording(char *pFilename, int Version)
{
	YMOutputEnabled = FALSE;

	if ((Version!=3) && (Version!=5))
	{
		Version = 5;
	}

	YM_Version = Version;

	
	pYMFilename = (char *)malloc(strlen(pFilename)+1);

	if (pYMFilename!=NULL)
	{
		strcpy(pYMFilename, pFilename);
	
		/* open temp file */
		fhYMOutput = Host_OpenFile(pYMTempFilename,HOST_FILE_ACCESS_WRITE);
		
		if (fhYMOutput!=0)
		{
			/* open succeeded, enable recording */
			YMOutputEnabled = TRUE;
			return;
		}
	
		free(pYMFilename);
	}

	YMOutput_InitialiseRegisters();
}

void	YMOutput_StopRecording(void)
{
	if (YMOutputEnabled)
	{
		unsigned char *pTempData;
		unsigned long TempDataLength;

		/* close temp output file */
		Host_CloseFile(fhYMOutput);
		fhYMOutput = 0;
		
		/* build YM data */

		/* load temp file */
		Host_LoadFile(pYMTempFilename, &pTempData, &TempDataLength);

		if ((pTempData!=NULL) && (TempDataLength!=0))
		{
			HOST_FILE_HANDLE FileHandle;
			
			FileHandle = Host_OpenFile(pYMFilename, HOST_FILE_ACCESS_WRITE);
			
			if (FileHandle!=0)
			{
				int i,j;
				int NoOfVBL = TempDataLength/14;	

				if (YM_Version == 3)
				{
					static unsigned char YMIdent[4] = "YM3!";

					/* write output ident */
					Host_WriteData(FileHandle, YMIdent, 4);
				}
				else
				{
					YM5_HEADER YM5_Header;

					memcpy(&YM5_Header.ID, YM5_Ident_Text, 4);
					memcpy(&YM5_Header.IDString, YM5_IdentString_Text, 8);
					YM5_Header.NumVBL = NoOfVBL;
					YM5_Header.SongAttributes = YM5_SONG_ATTRIBUTE_DATA_INTERLEAVED;
					YM5_Header.NoOfDigiDrumSamples = 0;
					YM5_Header.YMFrequency = 1000000;
					YM5_Header.PlayerFrequency = 50;
					YM5_Header.VBLLoopIndex = 0;
					YM5_Header.SizeOfExtraData = 0;
					
#ifdef CPC_LSB_FIRST
				YM5_Header.NumVBL = SwapEndianLong(YM5_Header.NumVBL);
				YM5_Header.SongAttributes = SwapEndianLong(YM5_Header.SongAttributes);
				YM5_Header.YMFrequency = SwapEndianLong(YM5_Header.YMFrequency);
				YM5_Header.VBLLoopIndex = SwapEndianLong(YM5_Header.VBLLoopIndex);
				YM5_Header.NoOfDigiDrumSamples = SwapEndianWord(YM5_Header.NoOfDigiDrumSamples);
				YM5_Header.PlayerFrequency = SwapEndianWord(YM5_Header.PlayerFrequency);
				YM5_Header.SizeOfExtraData = SwapEndianWord(YM5_Header.SizeOfExtraData);
#endif
					/* write header */
					Host_WriteData(FileHandle, (unsigned char *)&YM5_Header, sizeof(YM5_HEADER));
		
						
					/* write name of song */
					if (YM5_SongName!=NULL)
					{
						Host_WriteData(FileHandle, YM5_SongName, strlen((const char *)YM5_SongName)+1);
					}
					else
					{
						unsigned char Filler = 0;

						Host_WriteData(FileHandle, &Filler, sizeof(unsigned char));
					}

					/* write author name */
					if (YM5_AuthorName!=NULL)
					{
						Host_WriteData(FileHandle, YM5_AuthorName, strlen((const char *)YM5_AuthorName)+1);
					}
					else
					{
						unsigned char Filler = 0;

						Host_WriteData(FileHandle, &Filler, sizeof(unsigned char));
					}

					/* write comments */
					if (YM5_Comments!=NULL)
					{
						Host_WriteData(FileHandle, YM5_Comments, strlen((const char *)YM5_Comments)+1);
					}
					else
					{
						unsigned char Filler = 0;
						Host_WriteData(FileHandle, &Filler, sizeof(unsigned char));
					}
				}

				for (j=0; j<14; j++)
				{
					for (i=0; i<NoOfVBL; i++)
					{
						unsigned char Data = pTempData[(i*14) + j];

						Host_WriteData(FileHandle, &Data, sizeof(unsigned char));
					}
				}

				if (YM_Version == 5)
				{
					/* write register 14, 15 details - in this case just zero's. */
					for (i=0; i<NoOfVBL*2; i++)
					{
						unsigned char Filler = 0;
						Host_WriteData(FileHandle, &Filler, sizeof(unsigned char));
					}
		
				}


				if (YM_Version==5)
				{
					/* write end of file text */
					Host_WriteData(FileHandle, YM5_EndFileText,4);
				}

				Host_CloseFile(FileHandle);
				
			}

			free(pTempData);
		}
	}
}



