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
#ifndef __SAMPLE_LOAD_HEADER_INCLUDED__
#define __SAMPLE_LOAD_HEADER_INCLUDED__

#include "cpcdefs.h"
#include "host.h"

/* size of buffer to read spooled data into */
#define FILE_BUFFER_SIZE	32768

/* define for sample type */
typedef enum
{
	SAMPLE_TYPE_NONE = 0,
	SAMPLE_TYPE_VOC,
	SAMPLE_TYPE_WAV
} SAMPLE_TYPE_ID;


typedef struct SAMPLE_AUDIO_STREAM
{
	/* The following is for playback of the data */

	/* number of samples per nop */
	unsigned long	NumberOfSamplesPerNop;
	/* previous fraction */
	unsigned long	PreviousFraction;
	/* frequency of sample loaded */
	unsigned long	SampleFrequency;
	/* number of bits per sample */
	unsigned long	SampleBits;
	/* number of channels */
	unsigned long	SampleChannels;
	/* current sample */
	unsigned char CurrentSample;
	/* type of sample */
	SAMPLE_TYPE_ID	SampleType;

	/* The following is for loading for a sample file */
	/* The data is buffered */

	/* file handle for loading data */
	HOST_FILE_HANDLE FileHandle;
	/* pointer to buffer to hold data */
	unsigned char Buffer[FILE_BUFFER_SIZE];
	/* offset in the above buffer */
	unsigned long BufferOffset;
	/* bytes in buffer */
	unsigned long BytesInBuffer;

	/* offset in file */
	unsigned long OffsetInFile;

	/* total size of file */
	unsigned long FileSize;

	/* data specific to actual file loaded */
	void	*pSampleSpecificData;
} SAMPLE_AUDIO_STREAM;


unsigned char Sample_GetDataByteTimed(unsigned long PreviousNopCount, unsigned long NextNopCount);
unsigned char Sample_GetByte(SAMPLE_AUDIO_STREAM *pAudioStream);
void	Sample_ReadData(SAMPLE_AUDIO_STREAM *pAudioStream, unsigned char *pStart, unsigned long Size);
void	Sample_SkipData(SAMPLE_AUDIO_STREAM *pAudioStream, unsigned long Size);
void	Sample_Init(void);
void	Sample_Close(void);
BOOL Sample_Load(char *);


#endif
