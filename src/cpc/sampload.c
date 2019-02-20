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
#include "cpcglob.h"
#include "cpc.h"
#include "voc.h"
#include "wav.h"
#include "csw.h"
#include "sampload.h"
#include "host.h"

#include "headers.h"

#define FPS				50
#ifndef NOPS_PER_FRAME
#define NOPS_PER_FRAME 19968
#endif
#define NOPS_PER_SECOND 999997		/* 998400 */


/* the current audio stream */
static SAMPLE_AUDIO_STREAM	SampleAudioStream;

/* the type of sample we are currently using */
static SAMPLE_TYPE_ID SampleType = SAMPLE_TYPE_NONE;

BOOL	Sample_Load(const char *Filename)
{
	HOST_FILE_HANDLE	FileHandle;
	SAMPLE_AUDIO_STREAM	*pSampleAudioStream = &SampleAudioStream;

	if (pSampleAudioStream->FileHandle!=0)
	{
		Sample_Close();
	}

	/* set initial type */	
	pSampleAudioStream->SampleType = SAMPLE_TYPE_NONE;

	/* is file a VOC? */
	if (VOC_Validate(Filename))
	{
		/* yes */
		pSampleAudioStream->SampleType = SAMPLE_TYPE_VOC;
	}
	else 
	/* is file a WAV? */	
	if (WAV_Validate(Filename))
	{
		/* yes */
		pSampleAudioStream->SampleType = SAMPLE_TYPE_WAV;
	}
	else 
	/* is file a WAV? */	
	if (CSW_Validate(Filename))
	{
		/* yes */
		pSampleAudioStream->SampleType = SAMPLE_TYPE_CSW;
	}

	/* got a valid type */
	if (pSampleAudioStream->SampleType != SAMPLE_TYPE_NONE)
	{
		/* attempt to open file */
		FileHandle = Host_OpenFile(Filename, HOST_FILE_ACCESS_READ);

		if (FileHandle!=0)
		{
			unsigned long FileSize;

			FileSize = Host_GetFileSize(FileHandle);

			/* initialise structure */
			pSampleAudioStream->PreviousFraction = 0;
			pSampleAudioStream->OffsetInFile = 0;
			pSampleAudioStream->FileSize = FileSize;
			pSampleAudioStream->BytesInBuffer = 0;
			pSampleAudioStream->BufferOffset = 0;
			pSampleAudioStream->pSampleSpecificData = NULL;
			pSampleAudioStream->FileHandle = FileHandle;

			switch (pSampleAudioStream->SampleType)
			{
				case SAMPLE_TYPE_VOC:
				{
					VOC_Open(&SampleAudioStream);
				}
				break;

				case SAMPLE_TYPE_WAV:
				{
					WAV_Open(&SampleAudioStream);
				}
				break;
			
				case SAMPLE_TYPE_CSW:
				{
					CSW_Open(&SampleAudioStream);
				}
				break;

				default:
					break;
			}

			/* number_of_samples_per_nop = samples_per_second / nops_per_second */
			pSampleAudioStream->NumberOfSamplesPerNop = (pSampleAudioStream->SampleFrequency<<16)/NOPS_PER_SECOND; /* 16:16 fraction */
			
			/* set tape type to be a sample */
			CPC_SetCassetteType(CASSETTE_TYPE_SAMPLE);

			/* file opened ok */
			return TRUE;
		}
	}

	/* set file handle */
	pSampleAudioStream->FileHandle = 0;
			
	return FALSE;
}


void	Sample_Init(void)
{
	memset(&SampleAudioStream, 0, sizeof(SAMPLE_AUDIO_STREAM));
}


void	Sample_Close(void)
{
	SAMPLE_AUDIO_STREAM *pAudioStream;

	pAudioStream = &SampleAudioStream;

	/* free any specific data */
	switch (pAudioStream->SampleType)
	{
		case SAMPLE_TYPE_VOC:
		{
			/*VOC_Close(pAudioStream); */
		}
		break;

		case SAMPLE_TYPE_WAV:
		{
			/*WAV_Close(pAudioStream); */
		}
		break;

		case SAMPLE_TYPE_CSW:
		{
			/*CSW_Close(pAudioStream); */
		}
		break;

		default:
			break;
	}

	/* close file */
	if (pAudioStream->FileHandle!=0)
	{
		Host_CloseFile(pAudioStream->FileHandle);
		pAudioStream->FileHandle = 0;
	}
}


static unsigned char Sample_GetDataByte(SAMPLE_AUDIO_STREAM *pAudioStream)
{
	switch (pAudioStream->SampleType)
	{
		case SAMPLE_TYPE_VOC:
			return VOC_GetDataByte(pAudioStream);
	
		case SAMPLE_TYPE_WAV:
			return WAV_GetDataByte(pAudioStream);

		case SAMPLE_TYPE_CSW:
			return CSW_GetDataByte(pAudioStream);

		default:
			break;
	
	}

	return 0x080;
}






/*----------------------------------------------------------------------*/

/* performs a buffered read of a file */
unsigned char Sample_GetByte(SAMPLE_AUDIO_STREAM *pAudioStream)
{
	/* update offset in buffer */
	pAudioStream->BufferOffset++;

	/* gone past end of buffer? */
	if (pAudioStream->BufferOffset>=pAudioStream->BytesInBuffer)
	{
		unsigned long ReadSize;
		unsigned long SizeRemaining;

		/* if we've fetched bytes beyond buffer ... */

		/* reset offset in buffer */
		pAudioStream->BufferOffset = 0;

		/* max size of buffered data */
		ReadSize = FILE_BUFFER_SIZE;

		/* calc size remaining in file */
		SizeRemaining = pAudioStream->FileSize - pAudioStream->OffsetInFile;

		/* if size remaining < max size of buffered data, only read the
		remaining data */
		if (SizeRemaining<ReadSize)
		{
			ReadSize = SizeRemaining;
		}

		/* set number of bytes that will be in the buffer */
		pAudioStream->BytesInBuffer = ReadSize;
		
		/* update offset in file */
		pAudioStream->OffsetInFile += ReadSize;

		if (ReadSize!=0)
		{
			/* read bytes into buffer */
			Host_ReadData(pAudioStream->FileHandle, (unsigned char*)pAudioStream->Buffer, ReadSize);
		}
	}

	/* return byte from buffer */
	return pAudioStream->Buffer[pAudioStream->BufferOffset];
}

/*----------------------------------------------------------------------*/

/* read a specified size of data */
void	Sample_ReadData(SAMPLE_AUDIO_STREAM *pAudioStream, unsigned char *pStart, unsigned long Size)
{
	unsigned long i;

	for (i=0; i<Size; i++)
	{
		pStart[i] = Sample_GetByte(pAudioStream);
	}
}

/*----------------------------------------------------------------------*/

/* skip bytes from file */
void	Sample_SkipData(SAMPLE_AUDIO_STREAM *pAudioStream, unsigned long Size)
{
	unsigned long i;
	
	for (i=0; i<Size; i++)
	{
		Sample_GetByte(pAudioStream);
	}
}


/*----------------------------------------------------------------------*/

unsigned char Sample_GetDataByteTimed(unsigned long NopsPassed)
{
	SAMPLE_AUDIO_STREAM *pAudioStream = &SampleAudioStream;
	unsigned long NopDifference;
	unsigned char SampleByte;
	unsigned long NumberOfSamplesPassed, NumberOfSamplesToSkip;
	unsigned long i;

	/* number of nops passed */
	NopDifference = NopsPassed;	//NextNopCount - PreviousNopCount;
	
	/* number_of_samples_passed = number of complete samples passed */
	/* so we add on the fraction left over to get number of samples to skip */

	/* number_of_samples_passed = number_of_samples_per_nop * number_of_nops */
	NumberOfSamplesPassed = (pAudioStream->NumberOfSamplesPerNop * NopDifference)+pAudioStream->PreviousFraction; /* 16:16 fraction */

	/* number_of_samples_to_skip = int(number_of_samples_passed) */
	NumberOfSamplesToSkip = (NumberOfSamplesPassed>>16);

	/* store fraction */
	pAudioStream->PreviousFraction = NumberOfSamplesPassed & 0x0ffff;

	if (NumberOfSamplesToSkip!=0)
	{
		/* actual_number_to_skip = number_of_samples_to_skip-1, why?
		because we skip then fetch the next byte and store it */

		NumberOfSamplesToSkip--;

		/* skip bytes */
		for (i=0; i<NumberOfSamplesToSkip; i++)
		{
			Sample_GetDataByte(pAudioStream);
		}

		/* and get the next data byte */
		pAudioStream->CurrentSample = Sample_GetDataByte(pAudioStream);
	
	}

	return (pAudioStream->CurrentSample>>7) & 0x01;
//	SampleByte = (unsigned char)(pAudioStream->CurrentSample^0x080);
//
//	if (SampleByte<0x080)
//		return 0;

//	return 1;
#if 0

	SAMPLE_AUDIO_STREAM *pAudioStream = &SampleAudioStream;
	unsigned long NopDifference;
	unsigned char SampleByte;
	unsigned long NumberOfSamplesPassed, NumberOfSamplesToSkip;
	unsigned long i;

	/* number of nops passed */
	NopDifference = NopsPassed;
	
	/* number_of_samples_passed = number of complete samples passed */
	/* so we add on the fraction left over to get number of samples to skip */

	/* number_of_samples_passed = number_of_samples_per_nop * number_of_nops */
	NumberOfSamplesPassed = (pAudioStream->NumberOfSamplesPerNop * NopDifference);
	
	if (pAudioStream->PreviousFraction!=0)
	{
		unsigned long FractionRemaining;

		/* get amount of time remaining to complete this sample */
		FractionRemaining = 0x010000 - pAudioStream->PreviousFraction;

		/* greater than time that has just passed? */
		if (NumberOfSamplesPassed<FractionRemaining)
		{		
			/* update position within sample and return current sample value */
			pAudioStream->PreviousFraction+=NumberOfSamplesPassed;
			pAudioStream->PreviousFraction&=0x0ffff;
			return (pAudioStream->CurrentSample>>7) & 0x01;
		}
		else
		{
			/* has overflowed over end of sample, adjust for fractional amount completed */
			NumberOfSamplesPassed -= FractionRemaining;
		}
	}

	/* number_of_samples_to_skip = int(number_of_samples_passed) */
	NumberOfSamplesToSkip = ((NumberOfSamplesPassed+0x0ffff)>>16);

	/* store fraction */
	pAudioStream->PreviousFraction = NumberOfSamplesPassed & 0x0ffff;

	if (NumberOfSamplesToSkip!=0)
	{
		/* skip bytes */
		for (i=0; i<NumberOfSamplesToSkip; i++)
		{
			Sample_GetDataByte(pAudioStream);
		}

		/* and get the next data byte */
		pAudioStream->CurrentSample = Sample_GetDataByte(pAudioStream);
	
	}

	/* unsigned 8-bit data */
	/* if top bit set then say that bit is 1 else it is 0 */
	return (pAudioStream->CurrentSample>>7) & 0x01;
#endif
}

