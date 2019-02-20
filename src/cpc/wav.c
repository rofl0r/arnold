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
#include "wav.h"
#include "headers.h"
#include "endian.h"
#include "host.h"

#ifdef _WINDOWS
#pragma pack(1)
#endif

typedef struct
{
	unsigned long								ChunkName;
	unsigned long								ChunkLength;
} WAV_RIFF_CHUNK;

typedef struct
{  
    unsigned short  FormatTag; 
    unsigned short NoOfChannels;
    unsigned long SamplesPerSecond;
	unsigned long AverageBytesPerSecond;
	unsigned short BlockAlign;
	unsigned short BitsPerSample;
	unsigned short SizeOfExtraFormatInfo;
} WAV_WAVEFORMAT; 

#ifdef _WINDOWS
#pragma pack()
#endif

#define WAV_WAVE_FORMAT_PCM	1

static HOST_FILE_HANDLE fhTempWav = 0;
static char *pWavFilename = NULL;
static char *TempFilename = NULL;
static int Wav_NoOfChannels = 0;
static int Wav_Frequency = 0;
static int Wav_BitsPerSample = 0;
static const char *WaveRiffType="WAVE";

#ifdef CPC_LSB_FIRST
static unsigned long Riff_GenerateChunkName(unsigned char A, unsigned char B, unsigned char C, unsigned char D)
{
	return (unsigned long)((A | (B<<8) | (C<<16) | (D<<24)));
}
#else
static unsigned long Riff_GenerateChunkName(unsigned char A, unsigned char B, unsigned char C, unsigned char D)
{
	return (unsigned long)((A<<24) | (B<<16) | (C<<8) | D);
}
#endif

void	WavOutput_Init(char *NewTempFilename)
{
	fhTempWav = 0;
	pWavFilename = NULL;

	TempFilename = (char *)malloc(strlen(NewTempFilename)+1);

	if (TempFilename!=NULL)
	{
		memcpy(TempFilename, NewTempFilename, strlen(NewTempFilename)+1);
	}

}

void	WavOutput_InitialiseFormat(int NoOfChannels, int SampleRate, int BitsPerSample)
{
	Wav_NoOfChannels = NoOfChannels;
	Wav_Frequency = SampleRate;
	Wav_BitsPerSample = BitsPerSample;
}

void	WavOutput_Finish(void)
{
	/* close file if open */
	WavOutput_StopRecording();

	if (TempFilename!=NULL)
	{
		free(TempFilename);
	}

	if (pWavFilename!=NULL)
	{
		free(pWavFilename);
	}
}


void	WavOutput_StartRecording(char *NewWavFilename)
{
	/* free filename if one already present */
	if (pWavFilename!=NULL)
	{
		free(pWavFilename);
		pWavFilename = NULL;
	}

	/* copy filename for final wav */
	pWavFilename = (char *)malloc(strlen(NewWavFilename)+1);

	if (pWavFilename!=NULL)
	{
		memcpy(pWavFilename, NewWavFilename, strlen(NewWavFilename)+1);
	}

	/* open output file to store raw data */
	fhTempWav = Host_OpenFile(TempFilename,HOST_FILE_ACCESS_WRITE);
}

void	WavOutput_WriteBlock(char *pBlock, unsigned long BlockSize)
{
	if (fhTempWav!=0)
	{
		Host_WriteData(fhTempWav,(unsigned char *)pBlock, BlockSize);
	}
}

#define BUFFER_SIZE	(32*1024)

void	WavOutput_StopRecording(void)
{
	if (fhTempWav!=0)
	{
		char *pWaveData;
		int WaveDataSize;
		int WaveDataSizeAlign;
		HOST_FILE_HANDLE fh;
		HOST_FILE_HANDLE fhIn;

		/* close temp file */
		Host_CloseFile(fhTempWav);
		fhTempWav = 0;

		pWaveData = malloc(BUFFER_SIZE);

		fhIn = Host_OpenFile(TempFilename, HOST_FILE_ACCESS_READ);

		fh = Host_OpenFile(pWavFilename,HOST_FILE_ACCESS_WRITE);

		if ((fh!=0) && (fhIn!=0) && (pWaveData!=NULL))
		{
			/* build output file */
			WAV_RIFF_CHUNK	TempChunk;
			WAV_WAVEFORMAT	WaveFormat;

			WaveDataSize = Host_GetFileSize(fhIn);

			WaveDataSizeAlign = WaveDataSize + ((4-(WaveDataSize & 0x03)) & 0x03);

			/* setup main riff header */
			TempChunk.ChunkName = Riff_GenerateChunkName('R','I','F','F');
			TempChunk.ChunkLength = 
				sizeof(WAV_RIFF_CHUNK) + sizeof(WAV_WAVEFORMAT) + /* size of format information */
				4 +						/* size of "WAVE" text in riff chunk */
				sizeof(WAV_RIFF_CHUNK) + WaveDataSizeAlign;

#ifndef CPC_LSB_FIRST
			TempChunk.ChunkLength = SwapEndianLong(TempChunk.ChunkLength);
#endif

			/* write main header */
			Host_WriteData(fh, (unsigned char *)&TempChunk, sizeof(WAV_RIFF_CHUNK));

			Host_WriteData(fh, (unsigned char *)WaveRiffType, 4);

			/* data format chunk header */
			TempChunk.ChunkName = Riff_GenerateChunkName('f','m','t',' ');
			TempChunk.ChunkLength = sizeof(WAV_WAVEFORMAT);

#ifndef CPC_LSB_FIRST
			TempChunk.ChunkLength = SwapEndianLong(TempChunk.ChunkLength);
#endif

			/* write header */
			Host_WriteData(fh, (unsigned char *)&TempChunk, sizeof(WAV_RIFF_CHUNK));

			/* setup format info */
			WaveFormat.FormatTag = WAV_WAVE_FORMAT_PCM;
			WaveFormat.NoOfChannels = (unsigned short)Wav_NoOfChannels;
			WaveFormat.SamplesPerSecond = Wav_Frequency;
			WaveFormat.BlockAlign = (unsigned short)(Wav_NoOfChannels * (Wav_BitsPerSample>>3));
			WaveFormat.BitsPerSample = (unsigned short)Wav_BitsPerSample;
			WaveFormat.SizeOfExtraFormatInfo = 0;

			WaveFormat.AverageBytesPerSecond = WaveFormat.SamplesPerSecond * WaveFormat.BlockAlign;

#ifndef CPC_LSB_FIRST
			WaveFormat.FormatTag = SwapEndianWord(WaveFormat.FormatTag);
			WaveFormat.NoOfChannels = SwapEndianWord(WaveFormat.NoOfChannels);
			WaveFormat.SamplesPerSecond = SwapEndianLong(WaveFormat.SamplesPerSecond);
			WaveFormat.AverageBytesPerSecond = SwapEndianLong(WaveFormat.AverageBytesPerSecond);
			WaveFormat.BlockAlign = SwapEndianWord(WaveFormat.BlockAlign);
			WaveFormat.BitsPerSample = SwapEndianWord(WaveFormat.BitsPerSample);
			WaveFormat.SizeOfExtraFormatInfo = SwapEndianWord(WaveFormat.SizeOfExtraFormatInfo);
#endif
			/* write format information */
			Host_WriteData(fh, (unsigned char *)&WaveFormat, sizeof(WAV_WAVEFORMAT));
					
			/* data format chunk header */
			TempChunk.ChunkName = Riff_GenerateChunkName('d','a','t','a');
			TempChunk.ChunkLength = WaveDataSizeAlign;
		
#ifndef CPC_LSB_FIRST
			TempChunk.ChunkLength = SwapEndianLong(TempChunk. ChunkLength);
#endif

			Host_WriteData(fh, (unsigned char *)&TempChunk, sizeof(WAV_RIFF_CHUNK));

			/* speed up writing of output data by reading and writing
			 it in BUFFER_SIZE blocks. */
			{
				int CurrentOffset = 0;

				do
				{
					/* is there a block worth of more of data remaining? */
					if ((WaveDataSize-CurrentOffset)>=BUFFER_SIZE)
					{
						/* read and write a full block of data */
						Host_ReadData(fhIn, (unsigned char *)pWaveData, BUFFER_SIZE);
						Host_WriteData(fh, (unsigned char *)pWaveData, BUFFER_SIZE);

						CurrentOffset+=BUFFER_SIZE;
					}
					else
					{
						int SizeToRead = WaveDataSize-CurrentOffset;
						
						/* read and write part of a block of data */
						Host_ReadData(fhIn, (unsigned char *)pWaveData, SizeToRead);
						Host_WriteData(fh, (unsigned char *)pWaveData, SizeToRead);

						CurrentOffset+=SizeToRead;
					}

				}
				while (CurrentOffset<WaveDataSize);

				/* pad rest of file so that it is aligned to correct boundary */
				if ((WaveDataSizeAlign-WaveDataSize)!=0)
				{
					memset(pWaveData, 0x080, WaveDataSizeAlign-WaveDataSize);

					Host_WriteData(fh, (unsigned char *)pWaveData, WaveDataSizeAlign - WaveDataSize);
				}
			}
		}		

		/* free buffer */
		if (pWaveData!=NULL)
		{
			free(pWaveData);
		}


		/* close input raw data file */
		if (fhIn!=0)
		{
			Host_CloseFile(fhIn);
		}

		/* close output wav file */
		if (fh!=0)
		{
			Host_CloseFile(fh);
		}
	}
}

#include "riff.h"
#include "sampload.h"

/* validates the wave file before we use it */
BOOL	WAV_Validate(const char *Filename)
{
	BOOL Valid = FALSE;
	HOST_FILE_HANDLE FileHandle;

	FileHandle = Host_OpenFile(Filename, HOST_FILE_ACCESS_READ);

	if (FileHandle!=0)
	{
		unsigned long FileSize;

		FileSize = Host_GetFileSize(FileHandle);

		/* we require at least a block plus 4 bytes of data */
		if (FileSize>=sizeof(RIFF_CHUNK)+4)
		{
			RIFF_CHUNK Chunk;
			unsigned char RiffType[4];

			/* read chunk */
			Host_ReadData(FileHandle, (unsigned char *)&Chunk, sizeof(RIFF_CHUNK));

			/* read data */
			Host_ReadData(FileHandle, (unsigned char *)&RiffType[0], 4);

			/* riff as main header? */
			if (Riff_GetChunkName(&Chunk) == RIFF_FOURCC_CODE('R','I','F','F'))
			{
				if ((Riff_GetChunkLength(&Chunk)+sizeof(RIFF_CHUNK))<=FileSize)
				{
					if (
						(RiffType[0] == 'W') &&
						(RiffType[1] == 'A') &&
						(RiffType[2] == 'V') &&
						(RiffType[3] == 'E')
						)
					{
						Valid = TRUE;
					}
				}
			}
		}

		Host_CloseFile(FileHandle);
	}

	return Valid;
}


#if 0
	RIFF_CHUNK Chunk;

	/* read header */
	Sample_ReadData(pAudioStream, (unsigned char *)&Chunk, sizeof(RIFF_CHUNK));

	/* check header is RIFF */
	if (Riff_GetChunkName(&Chunk) == RIFF_FOURCC_CODE('R','I','F','F'))
	{
		unsigned long ChunkSize;
		
		/* get chunk size */
		ChunkSize = Riff_GetChunkLength(&Chunk);

		/* check chunk size is valid */
		if (ChunkSize<=pAudioStream->FileSize)
		{
			unsigned char ChunkData[4];

			Sample_ReadData(pAudioStream, &ChunkData[0], sizeof(unsigned char)*4);

			/* "WAVE" type? */
			if ((ChunkData[0] == 'W') &&
				(ChunkData[1] == 'A') &&
				(ChunkData[2] == 'V') &&
				(ChunkData[3] == 'E'))
			{
				/* yes */

				/* read chunk */
				Sample_ReadData(pAudioStream, (unsigned char *)&Chunk, sizeof(RIFF_CHUNK));

				if (
					/* "fmt " tag and has size of WAV_WAVEFORMAT */
					(Riff_GetChunkName(&Chunk) == RIFF_FOURCC_CODE('f','m','t',' ')) &&
					Riff_GetChunkLength(&Chunk) == sizeof(WAV_WAVEFORMAT)
					)

				{
					unsigned short Format;
					WAV_WAVEFORMAT WaveFormat;

					/* read structure */
					Sample_ReadData(pAudioStream, (unsigned char *)&WaveFormat, sizeof(WAV_WAVEFORMAT));

					/* get format */
#ifdef CPC_LSB_FIRST
					Format = WaveFormat.FormatTag;
#else
					Format = SwapEndianWord(WaveFormat.FormatTag);
#endif

					/* pcm? */
					if (Format==WAV_WAVE_FORMAT_PCM)
					{

						/* get frequencey */
#ifdef CPC_LSB_FIRST
						pAudioStream->SampleFrequency = WaveFormat.SamplesPerSecond;
#else
						pAudioStream->SampleFrequency = SwapEndianLong(WaveFormat.SamplesPerSecond);
#endif

	
					/* get number of channels */
#ifdef CPC_LSB_FIRST
						pAudioStream->SampleChannels = WaveFormat.NoOfChannels;
#else
						pAudioStream->SampleChannels = SwapEndianWord(WaveFormat.NoOfChannels);
#endif


				/* get bits per sample */
#ifdef CPC_LSB_FIRST
						pAudioStream->SampleBits = WaveFormat.BitsPerSample;
#else
						pAudioStream->SampleBits = SwapEndianWord(WaveFormat.BitsPerSample);
#endif
					}
				}

				Sample_ReadData(pAudioStream, (unsigned char *)&Chunk, sizeof(RIFF_CHUNK));

				if (Riff_GetChunkName(&Chunk) == RIFF_FOURCC_CODE('d','a','t','a'))
				{
					return TRUE;
				}

			}
		}
	}

	return FALSE;

#endif


void	WAV_Open(SAMPLE_AUDIO_STREAM *pAudioStream)
{
	RIFF_CHUNK Chunk;

	/* read header */
	Sample_ReadData(pAudioStream, (unsigned char *)&Chunk, sizeof(RIFF_CHUNK));

	/* check header is RIFF */
	if (Riff_GetChunkName(&Chunk) == RIFF_FOURCC_CODE('R','I','F','F'))
	{
		unsigned long ChunkSize;
		
		/* get chunk size */
		ChunkSize = Riff_GetChunkLength(&Chunk);

		/* check chunk size is valid */
		if (ChunkSize<=pAudioStream->FileSize)
		{
			unsigned char ChunkData[4];

			Sample_ReadData(pAudioStream, &ChunkData[0], sizeof(unsigned char)*4);

			/* "WAVE" type? */
			if ((ChunkData[0] == 'W') &&
				(ChunkData[1] == 'A') &&
				(ChunkData[2] == 'V') &&
				(ChunkData[3] == 'E'))
			{
				/* yes */

				/* read chunk */
				Sample_ReadData(pAudioStream, (unsigned char *)&Chunk, sizeof(RIFF_CHUNK));

				if (
					/* "fmt " tag and has size of WAV_WAVEFORMAT */
					(Riff_GetChunkName(&Chunk) == RIFF_FOURCC_CODE('f','m','t',' ')) &&
					(Riff_GetChunkLength(&Chunk) >= sizeof(WAV_WAVEFORMAT)-2)
					)

				{
					unsigned short Format;
					WAV_WAVEFORMAT WaveFormat;
					unsigned long SkipSize;

					/* read structure */
					Sample_ReadData(pAudioStream, (unsigned char *)&WaveFormat, sizeof(WAV_WAVEFORMAT)-2);

					SkipSize = Riff_GetChunkLength(&Chunk) - (sizeof(WAV_WAVEFORMAT)-2);

					if (SkipSize!=0)
					{
						Sample_SkipData(pAudioStream, SkipSize);
					}

					/* get format */
#ifdef CPC_LSB_FIRST
					Format = WaveFormat.FormatTag;
#else
					Format = SwapEndianWord(WaveFormat.FormatTag);
#endif

					/* pcm? */
					if (Format==WAV_WAVE_FORMAT_PCM)
					{

						/* get frequencey */
#ifdef CPC_LSB_FIRST
						pAudioStream->SampleFrequency = WaveFormat.SamplesPerSecond;
#else
						pAudioStream->SampleFrequency = SwapEndianLong(WaveFormat.SamplesPerSecond);
#endif

	
					/* get number of channels */
#ifdef CPC_LSB_FIRST
						pAudioStream->SampleChannels = WaveFormat.NoOfChannels;
#else
						pAudioStream->SampleChannels = SwapEndianWord(WaveFormat.NoOfChannels);
#endif


				/* get bits per sample */
#ifdef CPC_LSB_FIRST
						pAudioStream->SampleBits = WaveFormat.BitsPerSample;
#else
						pAudioStream->SampleBits = SwapEndianWord(WaveFormat.BitsPerSample);
#endif
					}
				}

				Sample_ReadData(pAudioStream, (unsigned char *)&Chunk, sizeof(RIFF_CHUNK));

				if (Riff_GetChunkName(&Chunk) == RIFF_FOURCC_CODE('d','a','t','a'))
				{
					return;
				}

			}
		}
	}

	return;
}

unsigned char WAV_GetDataByte(SAMPLE_AUDIO_STREAM *pAudioStream)
{

	/* 8-bit data is always mono */
	/* 16-bit data is always signed */
	if (pAudioStream->SampleChannels==1)
	{
		if (pAudioStream->SampleBits==8)
		{
			/* return byte */
			return Sample_GetByte(pAudioStream);
		}
		else
		{
			unsigned short SampleData;

			SampleData = Sample_GetByte(pAudioStream);
			SampleData |= Sample_GetByte(pAudioStream)<<8;

			SampleData=SampleData^0x08000;

			return (unsigned char)(SampleData>>8);
		}
	}
	else
	{
		/* stereo */

		if (pAudioStream->SampleBits==8)
		{
			unsigned char SampleByte1, SampleByte2;
			SampleByte1 = Sample_GetByte(pAudioStream);
			SampleByte2 = Sample_GetByte(pAudioStream);

			return (unsigned char)((SampleByte1 + SampleByte2)>>1);
		}
		else
		{
			unsigned short SampleData1, SampleData2;

			SampleData1 = Sample_GetByte(pAudioStream);
			SampleData1 |= Sample_GetByte(pAudioStream)<<8;

			SampleData2 = Sample_GetByte(pAudioStream);
			SampleData2 |= Sample_GetByte(pAudioStream)<<8;

			SampleData1=SampleData1^0x08000;
			SampleData2=SampleData2^0x08000;

			return (unsigned char)(((SampleData1 + SampleData2)>>1)>>8);
		}
	}
}

