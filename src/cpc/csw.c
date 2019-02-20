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
#include "csw.h"
#include "headers.h"

typedef struct
{
	/* number of samples to output */
	unsigned long Samples;
	/* volume for sample to output */
	unsigned short Level;
} CSW_DECODER;

CSW_DECODER Decoder;

/* TRUE if code was successfully read, FALSE if end of file or code was not successfully read */
static BOOL CSWDecoder_FetchCode(SAMPLE_AUDIO_STREAM *pAudioStream,CSW_DECODER *pDecoder)
{
	unsigned char Code;
	unsigned long LongCode;

	/* attempt to read byte-code */
	Sample_ReadData(pAudioStream, &Code, sizeof(unsigned char));

	/* if code is not zero, then byte defines length of pulse */
	if (Code!=0)
	{
		pDecoder->Samples = Code & 0x0ff;
		return TRUE;
	}

	/* code is zero, next 4 bytes define the length of the pulse */

	/* read 4 byte length */
	Sample_ReadData(pAudioStream, (unsigned char *)&LongCode, sizeof(unsigned long));

	/* read 4 byte length ok! */

#ifdef CPC_MSB_FIRST
	LongCode= SwapEndianLong(LongCode);
#endif
	pDecoder->Samples = LongCode;
	return TRUE;
}

/* TRUE if decoder needs more data, FALSE otherwise */
static BOOL CSWDecoder_NeedMoreData(CSW_DECODER *pDecoder)
{
	return (pDecoder->Samples==0);
}

/* update decoding */
static unsigned char CSWDecoder_Update(CSW_DECODER *pDecoder)
{
	unsigned short Level;

	Level = pDecoder->Level;

	/* decrement count */
	pDecoder->Samples--;

	/* toggle level */
	if (pDecoder->Samples==0)
	{
		/* change level */
		if (pDecoder->Level == 0x07f)
			pDecoder->Level = 0x080;
		else
			pDecoder->Level = 0x07f;
	}

	return Level;
}

BOOL CSW_ValidateHeader(HOST_FILE_HANDLE FileHandle)
{
	CSW_FILE_COMMON_HEADER Header;

	/* get header */
	Host_ReadData(FileHandle, (unsigned char *)&Header, sizeof(CSW_FILE_COMMON_HEADER));

	if (memcmp(Header.Ident, CSW_IDENT, 20)!=0)
		return FALSE;

	switch (Header.MajorFormatRevision)
	{
		case 1:
		{
			CSW_FILE_HEADER_V1	V1Header;
			
			if ((Header.MinorFormatRevision!=0) && (Header.MinorFormatRevision!=1))
				return FALSE;
	
			Host_ReadData(FileHandle, (unsigned char *)&V1Header, sizeof(CSW_FILE_HEADER_V1));

			if (V1Header.CompressionType!=1)
				return FALSE;

			return TRUE;
		}
		break;

		case 2:
		{
			CSW_FILE_HEADER_V2	V2Header;
			
			if (Header.MinorFormatRevision!=0)
				return FALSE;
		
			Host_ReadData(FileHandle, (unsigned char *)&V2Header, sizeof(CSW_FILE_HEADER_V2));

			if (V2Header.CompressionType!=1)
				return FALSE;

			return TRUE;
		}
		break;

		default:
			break;
	}

	return FALSE;
}

int	CSW_Validate(char *Filename)
{
	BOOL Valid = FALSE;
	HOST_FILE_HANDLE FileHandle;

	FileHandle = Host_OpenFile(Filename, HOST_FILE_ACCESS_READ);

	if (FileHandle!=0)
	{
		Valid = CSW_ValidateHeader(FileHandle);
	
		/* close the file */
		Host_CloseFile(FileHandle);
	}

	return Valid;
}

/* assumes validated file */
void	CSW_Open(SAMPLE_AUDIO_STREAM *pAudioStream)
{	
	CSW_FILE_COMMON_HEADER Header;
	short InitialLevel = (short)0x080;

	Sample_ReadData(pAudioStream,(unsigned char *)&Header, sizeof(CSW_FILE_COMMON_HEADER));


	switch (Header.MajorFormatRevision)
	{
		case 1:
		{
			CSW_FILE_HEADER_V1 V1Header;

			Sample_ReadData(pAudioStream, (unsigned char *)&V1Header, sizeof(CSW_FILE_HEADER_V1));
	
			if (Header.MinorFormatRevision!=0)
			{
				/* set initial level */
				if (V1Header.Flags & CSW_POLARITY_MASK)
				{
					InitialLevel = 0x07f;
				}
				else
				{
					InitialLevel = 0x080;
				}
			}


			/* setup sample description */
			pAudioStream->SampleFrequency = 
				(
				(V1Header.SampleRateHzL & 0x0ff) | 
				((V1Header.SampleRateHzH & 0x0ff)<<8)
				);

		}
		break;

		case 2:
		{
			CSW_FILE_HEADER_V2 V2Header;
			unsigned long Skip;

			Sample_ReadData(pAudioStream, (unsigned char *)&V2Header, sizeof(CSW_FILE_HEADER_V2));

			if (V2Header.Flags & CSW_POLARITY_MASK)
			{
				InitialLevel = 0x07f;
			}
			else
			{
				InitialLevel = 0x080;
			}		

			pAudioStream->SampleFrequency = 
				(
				(V2Header.SampleRateHzL & 0x0ff) | 
				((V2Header.SampleRateHzML & 0x0ff)<<8) | 
				((V2Header.SampleRateHzMH & 0x0ff)<<16) | 
				((V2Header.SampleRateHzH & 0x0ff)<<24)
				);

			Skip = V2Header.HeaderExtensionLength&0x0ff;

			if (Skip!=0)
			{
				Sample_SkipData(pAudioStream, Skip);
			}
		}
		break;
	}

	Decoder.Level = InitialLevel;
	pAudioStream->SampleBits = 1;
	pAudioStream->SampleChannels = 1;
	CSWDecoder_FetchCode(pAudioStream, &Decoder);
}



unsigned char CSW_GetDataByte(SAMPLE_AUDIO_STREAM *pAudioStream)
{
	/* more data required? */
	if (CSWDecoder_NeedMoreData(&Decoder))
	{
		/* fetch */
		if (!CSWDecoder_FetchCode(pAudioStream, &Decoder))
		{
			/* error fetching or end of file */
			/* return number read so far */
		}
	}

		/* update decoder and generate a sample */
	return CSWDecoder_Update(&Decoder);
}
