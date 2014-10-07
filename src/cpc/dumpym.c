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
#include "dumpym.h"
#include "cpcglob.h"
#include "endian.h"
#include "host.h"
#include "headers.h"

/* current state of registers and info about if
they have changed */
static AY_INFO	AYInfo;

static unsigned char *YM5_SongName = NULL;
static unsigned char *YM5_AuthorName = NULL;
static unsigned char *YM5_Comments = NULL;

const unsigned char YM5_EndFileText[4]="End!";
const unsigned char YM3_Ident_Text[4] = "YM3!";
const unsigned char YM5_Ident_Text[4] = "YM5!";
const unsigned char YM5_IdentString_Text[8]="LeOnArD!";

#define YM5_SONG_ATTRIBUTE_DATA_INTERLEAVED	0x0001

#ifdef _WINDOWS
#pragma pack(1)
#endif

typedef struct
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


void	YMOutput_StartRecording(void)
{
	int i;

	/* initialise register data */
	for (i=0; i<16; i++)
	{
		AYInfo.RegisterData[i] = AYInfo.PreviousRegisterData[i] = 0;
		AYInfo.Flags[i] = 0;
	}
}

void	YMOutput_Finish(void)
{
	if (YM5_SongName!=NULL)
	{
		free(YM5_SongName);
		YM5_SongName = NULL;
	}

	if (YM5_AuthorName!=NULL)
	{
		free(YM5_AuthorName);
		YM5_AuthorName = NULL;
	}

	if (YM5_Comments!=NULL)
	{
		free(YM5_Comments);
		YM5_Comments = NULL;
	}
}

/* TRUE if output is silent, FALSE if not */
BOOL YMOutput_IsSilent(void)
{
	/* what defines silence? */

	/* - if R8, R9 and R10 are all zero, then no noise or tone will be output (noise and
	    tone can be active or inactive)
	   - if tone or noise are not active, we will not get any sound provided that R8,R9,R10
	do not change. 
	   - if tone is active, but tone is in a specific in-audible range, it will be silent.
	   R8, R9, R10 can be any value.
	*/

	if ((AYInfo.RegisterData[8]==0) || (AYInfo.RegisterData[9]==0) || (AYInfo.RegisterData[10]==0))
	{
		/* volume regs are all zero = silence regardless of mixer,
		tone and noise settings. */
		return TRUE;
	}

	/* a volume register is not zero, something *could* be audible */
	if (
		(!(
		(AYInfo.Flags[8]&AY_INFO_REG_DATA_CHANGED) | 
		(AYInfo.Flags[9]&AY_INFO_REG_DATA_CHANGED) | 
		(AYInfo.Flags[10]&AY_INFO_REG_DATA_CHANGED)
		)) &&
		((AYInfo.RegisterData[7] & 0x03f)==0x03f))
	{
		/* AY Regs haven't changed, and tone and noise are disabled */
		return TRUE;
	}

	return FALSE;
}

static void	YMOutput_ClearRegUpdatedStatus(void)
{
	int i;

	for (i=0; i<16; i++)
	{
		/* clear updated flag */
		AYInfo.Flags[i] &=~AY_INFO_REG_UPDATED;
	}
}


/* generate a temporary record which can be written to a file */
void	YMOutput_GenerateTempRecord(unsigned char *Regs)
{
	int i;

	for (i=0; i<13; i++)
	{
		Regs[i] = AYInfo.RegisterData[i];
	}

	if ((AYInfo.Flags[13] & AY_INFO_REG_UPDATED)==0)
	{
		Regs[13] = 0x0ff;
	}
	else
	{
		Regs[13] = AYInfo.RegisterData[13];
	}

	for (i=14; i<16; i++)
	{
		Regs[i] = AYInfo.RegisterData[i];	
	}

	/* clear changed status */
	YMOutput_ClearRegUpdatedStatus();
}


/* record AY/YM output */
void	YMOutput_StoreRegData(int PSG_SelectedRegister, int Data)
{
	/* setup previous register data */
	AYInfo.PreviousRegisterData[PSG_SelectedRegister] = 
		AYInfo.RegisterData[PSG_SelectedRegister];

	/* data changed? */
	if (Data!=AYInfo.RegisterData[PSG_SelectedRegister])
	{
		AYInfo.Flags[PSG_SelectedRegister]|=AY_INFO_REG_DATA_CHANGED;
	}
	else
	{
		AYInfo.Flags[PSG_SelectedRegister]&=~AY_INFO_REG_DATA_CHANGED;
	}

	/* this register has been updated this frame */
	AYInfo.Flags[PSG_SelectedRegister]|=AY_INFO_REG_UPDATED;

	/* store register data */
	AYInfo.RegisterData[PSG_SelectedRegister] = (unsigned char)Data;
}

int		YMOutput_ValidateVersion(int Version)
{
	if ((Version!=3) && (Version!=5))
	{
		Version = 5;
	}

	return Version;
}

/* calculate size of header for YM file */
unsigned long YMOutput_GenerateHeaderOutputSize(int nVersion)
{
	unsigned long nSize;

	nSize = 0;

	nVersion = YMOutput_ValidateVersion(nVersion);

	if (nVersion==3)
	{
		nSize = strlen(YM3_Ident_Text);
	}
	else if (nVersion == 5)
	{
		/* version 5 */
		nSize = sizeof(YM5_HEADER);

		/* write name of song */
		if (YM5_SongName!=NULL)
		{
			nSize += strlen(YM5_SongName);
		}
		nSize++;

		/* write author name */
		if (YM5_AuthorName!=NULL)
		{
			nSize += strlen(YM5_AuthorName);
		}
		nSize++;

		if (YM5_Comments!=NULL)
		{
			nSize += strlen(YM5_Comments);
		}
		nSize++;
	}
	
	return nSize;
}

unsigned long YMOutput_GenerateTrailerOutputSize(int nVersion)
{
	unsigned long nSize;

	nSize = 0;

	nVersion = YMOutput_ValidateVersion(nVersion);

	if (nVersion == 5)
	{
		/* version 5 */
		nSize = strlen(YM5_EndFileText);
	}
	
	return nSize;
}

/* setup header */
void YMOutput_GenerateHeaderData(unsigned char *pData, int nVersion, int nVBL)
{	
	nVersion = YMOutput_ValidateVersion(nVersion);

	if (nVersion == 3)
	{
		memcpy(pData, YM3_Ident_Text, strlen(YM3_Ident_Text));
	}
	else
	{
		YM5_HEADER YM5_Header;

		memcpy(&YM5_Header.ID, YM5_Ident_Text, 4);
		memcpy(&YM5_Header.IDString, YM5_IdentString_Text, 8);
		YM5_Header.NumVBL = nVBL;
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
		memcpy(pData, &YM5_Header,sizeof(YM5_HEADER));
		pData+=sizeof(YM5_HEADER);
		
		/* write name of song */
		if (YM5_SongName!=NULL)
		{
			int nStringLen = strlen(YM5_SongName);
			memcpy(pData, YM5_SongName, nStringLen);
			pData+=nStringLen;
		}

		pData[0] = '\0';
		pData++;

		/* write author name */
		if (YM5_AuthorName!=NULL)
		{
			int nStringLen = strlen(YM5_AuthorName);
			memcpy(pData, YM5_AuthorName, nStringLen);
			pData+=nStringLen;
		}

		pData[0] = '\0';
		pData++;

		if (YM5_Comments!=NULL)
		{
			int nStringLen = strlen(YM5_Comments);
			memcpy(pData, YM5_Comments, nStringLen);
			pData+=nStringLen;
		}

		pData[0] = '\0';
		pData++;
	}
}

void YMOutput_GenerateTrailerData(unsigned char *pData, int nVersion)
{
	unsigned long nSize;

	nSize = 0;

	nVersion = YMOutput_ValidateVersion(nVersion);

	if (nVersion == 5)
	{
		/* version 5 */
		memcpy(pData, YM5_EndFileText, strlen(YM5_EndFileText));
	}
}

void YMOutput_ConvertTempData(const unsigned char *pSrcData, unsigned char *pOutputData, int nVersion, int nVBL)
{
	int j,i;
	unsigned long nPos = 0;
	
	nVersion = YMOutput_ValidateVersion(nVersion);

	for (j=0; j<14; j++)
	{
		for (i=0; i<nVBL; i++)
		{
			unsigned char Data = pSrcData[(i*16) + j];

			pOutputData[nPos] = Data;
			nPos++;
		}
	}

	if (nVersion == 5)
	{
		/* write register 14, 15 details - in this case just zero's. */
		memset(pOutputData, 0, (nVBL*2));
	}
}



