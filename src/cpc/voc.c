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
#include "cpcdefs.h"
#include "cpc.h"
#include "sampload.h"
#include "host.h"
#include "endian.h"
#include "voc.h"
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/* Voc file format:

Header					0x01a bytes	"Creative Voice File\0x01a"
Offset to first block	2 bytes		

  Block Header:

  1 byte		Block ID
  3 byte		Block Size (not including size of this block header)


  Block headers:

  1: sound data

  1 byte		sample rate
  1 byte		compression type

  2: sound continue

  (no header)

  3: silence

  2 bytes		length of silence - 1
  1 byte		sample rate

  4: marker

  2 bytes		marker id

  5: ascii string

  null terminated string

  6: repeat

  2 bytes count

  7: end repeat

  (no data)

  8: extended info

  2 bytes time constant
  1 byte pack
  1 byte mode

  9:

  2 bytes		sample frequency
  2 bytes		?
  1 byte		bits per sample
  1 byte		no of channels
  6 bytes		?
  x bytes		data
*/

#define VOC_MAIN_HEADER_IDENT "Creative Voice File\x1a"

/* encountered a repeat start with infinite marker */
#define VOC_AUDIO_STREAM_FLAGS_REPEAT_INFINITE 0x0001
/* encountered a silence block */
#define VOC_AUDIO_STREAM_FLAGS_SILENCE_ACTIVE  0x0002


#ifdef _WINDOWS
#pragma pack(1)
#endif

typedef struct VOC_MAIN_HEADER
{
	/* identification text */
	unsigned char	IdentText[0x014];
	/* offset to first block */
	unsigned char	FirstBlockOffsetLow;
	unsigned char	FirstBlockOffsetHigh;
	/* unencoded version */
	unsigned char	UnencodedVersionMinor;
	unsigned char	UnencodedVersionMajor;
	/* encodeded version */
	unsigned char	EncodedVersionMinor;
	unsigned char	EncodedVersionMajor;
} VOC_MAIN_HEADER;

typedef struct VOC_BLOCK_HEADER
{
	/* id of block */
	unsigned char	ID;
	/* length of block */
	unsigned char	LengthLow;
	unsigned char	LengthMid;
	unsigned char	LengthHigh;
} VOC_BLOCK_HEADER;

typedef struct VOC_BLOCK9_HEADER
{
	/* sample frequency in Hz */
	unsigned short	SampleFrequency;
	/* ?? */
	unsigned short	pad0;
	/* bits per sample */
	unsigned char	BitsPerSample;
	/* no of channels */
	unsigned char	NoOfChannels;
	/* ?? */
	unsigned char	pad1[6];
} VOC_BLOCK9_HEADER;

typedef struct VOC_BLOCK1_HEADER
{
	unsigned char	SampleRate;
	unsigned char	CompressionType;
} VOC_BLOCK1_HEADER;

typedef struct VOC_BLOCK3_HEADER
{
	unsigned short	LengthOfSilence;
	unsigned char	SampleRate;
} VOC_BLOCK3_HEADER;

typedef struct VOC_BLOCK4_HEADER
{
	unsigned short	MarkerID;
} VOC_BLOCK4_HEADER;

typedef struct VOC_BLOCK6_HEADER
{
	unsigned short	RepeatCount;
} VOC_BLOCK6_HEADER;

#ifdef _WINDOWS
#pragma pack()
#endif


typedef struct VOC_AUDIO_STREAM
{
	/* offset into block */
	unsigned long	CurrentBlockOffset;
	/* size of data in block */
	unsigned long	CurrentBlockSize;

	unsigned long	Flags;

	/* number of times to repeat */
	unsigned long	RepeatCount;
	/* start of repeat */
	unsigned long	RepeatPosition;


} VOC_AUDIO_STREAM;

static VOC_AUDIO_STREAM VocAudioStream;


static int	VOC_IdentifyHeader(VOC_MAIN_HEADER *VocHeader)
{
	/* header text matches?? */
	if (memcmp(VocHeader, VOC_MAIN_HEADER_IDENT,19)==0)
	{
		/* yes */
		
		unsigned long Version;
		unsigned long EncodedVersion;

		/* get unencoded version */

		/* get version major */
		Version = VocHeader->UnencodedVersionMajor<<8;
		/* get version minor */
		Version |= VocHeader->UnencodedVersionMinor;

		/* get encoded version */
		EncodedVersion = VocHeader->EncodedVersionMajor<<8;
		EncodedVersion |= VocHeader->EncodedVersionMinor;

		EncodedVersion -= 0x01234;
		EncodedVersion ^= 0x0ffff;

		if ((EncodedVersion & 0x0ffff) == (Version & 0x0ffff))
		{
			/* header is ok */
			return 1;
		}
	}

	/* header is not ok */
	return 0;
}

static int VOC_GetBlockSize(VOC_BLOCK_HEADER *pBlockHeader)
{
	return pBlockHeader->LengthLow | (pBlockHeader->LengthMid<<8) |
			(pBlockHeader->LengthHigh<<16);
}

INLINE static unsigned long VOC_SampleRateToFrequency(int Rate)
{
	return 1000000L/(256 - Rate);
}

static void	VOC_GetNextDataBlock(SAMPLE_AUDIO_STREAM *pAudioStream)
{
	VOC_BLOCK_HEADER VocBlockHeader;
	VOC_AUDIO_STREAM *pVocAudioStream = &VocAudioStream;

	/* read header */
	Sample_ReadData(pAudioStream,(unsigned char *)&VocBlockHeader, sizeof(VOC_BLOCK_HEADER));

	pVocAudioStream->CurrentBlockOffset = 0;

	if (VocBlockHeader.ID!=0)
	{
		unsigned long BlockSize;

		BlockSize = VOC_GetBlockSize(&VocBlockHeader);
	
		switch (VocBlockHeader.ID)
		{
			/* sample data */
			case 9:
			{
				VOC_BLOCK9_HEADER Block9Header;
				
				Sample_ReadData(pAudioStream, (unsigned char *)&Block9Header, sizeof(VOC_BLOCK9_HEADER));

				pVocAudioStream->CurrentBlockSize = BlockSize - sizeof(VOC_BLOCK9_HEADER);

#ifdef CPC_LSB_FIRST
				pAudioStream->SampleFrequency = Block9Header.SampleFrequency;
#else
				pAudioStream->SampleFrequency = SwapEndianWord(Block9Header.SampleFrequency);
#endif

				pAudioStream->SampleBits = Block9Header.BitsPerSample;
				pAudioStream->SampleChannels = Block9Header.NoOfChannels;
			}
			break;

			case 1:
			{
				VOC_BLOCK1_HEADER Block1Header;
				
				Sample_ReadData(pAudioStream, (unsigned char *)&Block1Header, sizeof(VOC_BLOCK1_HEADER));

				pVocAudioStream->CurrentBlockSize = BlockSize - sizeof(VOC_BLOCK1_HEADER);
				
				if (Block1Header.CompressionType==0)
				{
					pAudioStream->SampleBits = 8;
					pAudioStream->SampleChannels = 1;
				}
				
				pAudioStream->SampleFrequency = VOC_SampleRateToFrequency(Block1Header.SampleRate);
			}
			break;

			/* data block */
			case 2:
			{
				pVocAudioStream->CurrentBlockSize = BlockSize;
			}
			break;


			/* silence block */
			case 3:
			{
			
				pVocAudioStream->Flags |= VOC_AUDIO_STREAM_FLAGS_SILENCE_ACTIVE;
			}
			break;

			/* start repeat */
			case 6:
			{
				int RepeatCount;
				VOC_BLOCK6_HEADER Block6Header;

				Sample_ReadData(pAudioStream, (unsigned char *)&Block6Header, sizeof(VOC_BLOCK6_HEADER));

#ifdef CPC_LSB_FIRST
				RepeatCount = Block6Header.RepeatCount;
#else
				RepeatCount = SwapEndianWord(Block6Header.RepeatCount);
#endif

				if (RepeatCount==0x0ffff)
				{
					pVocAudioStream->Flags |= VOC_AUDIO_STREAM_FLAGS_REPEAT_INFINITE;
				}
				else
				{
					pVocAudioStream->RepeatCount = RepeatCount-1;
				}

				/*(pVocAudioStream->RepeatPosition = Sample_GetPosition(); */
			}
			break;

			/* end repeat */
			case 7:
			{
				BOOL bRepeat = FALSE;
				if (pVocAudioStream->Flags & VOC_AUDIO_STREAM_FLAGS_REPEAT_INFINITE)
				{
					bRepeat = TRUE;
				}
				else
				{
					/* update repeat count */
					pVocAudioStream->RepeatCount--;

					if (pVocAudioStream->RepeatCount!=0)
					{
						bRepeat = TRUE;
					}
				}

				if (bRepeat)
				{
/*					Sample_SetPosition(pVocAudioStream->RepeatPosition); */
				}
			}
			break;


			default:
			{
			}
			break;	
		}
	}
}

static void	VOC_Validate_SkipData(HOST_FILE_HANDLE FileHandle, unsigned long Size)
{
	unsigned long i;
	unsigned long DataLong;
	unsigned char DataByte;

	for (i=0; i<(Size>>2); i++)
	{
		Host_ReadData(FileHandle, (unsigned char *)&DataLong, sizeof(unsigned long));
	}

	for (i=0; i<(Size&3); i++)
	{
		Host_ReadData(FileHandle, (unsigned char *)&DataByte, sizeof(unsigned char));
	}
}



/*-------------------------*/
/* does a simple validation by checking size of each data block is correct */
BOOL	VOC_Validate(char *Filename)
{	
	BOOL Valid = FALSE;
	HOST_FILE_HANDLE FileHandle;

	FileHandle = Host_OpenFile(Filename, HOST_FILE_ACCESS_READ);

	if (FileHandle!=0)
	{
		unsigned long FileSize;

		FileSize = Host_GetFileSize(FileHandle);

		/* size must be at least size of voc header.
		The minimal VOC file in this case is a header, plus a single 0 byte indicating
		end of file.
		*/
		if (FileSize>=(sizeof(VOC_MAIN_HEADER)+1))
		{
			VOC_BLOCK_HEADER VocBlockHeader;
			VOC_MAIN_HEADER VocHeader[32];
			unsigned long FileOffset;
			
			/* get header */
			Host_ReadData(FileHandle, (unsigned char *)&VocHeader[0], sizeof(VOC_MAIN_HEADER));

			/* setup file offset */
			FileOffset = sizeof(VOC_MAIN_HEADER);

			/* check header for identification string */
			if (VOC_IdentifyHeader(&VocHeader[0]))
			{
				unsigned long OffsetOfFirstDataBlock;
				unsigned long DataToSkip;

				/* identification string matches */

				Valid = TRUE;

				OffsetOfFirstDataBlock = VocHeader->FirstBlockOffsetLow | 
						(VocHeader->FirstBlockOffsetHigh<<8);
				
				DataToSkip = OffsetOfFirstDataBlock - sizeof(VOC_MAIN_HEADER);

				/* as long as data we need to skip is still within file */
				if (DataToSkip<(FileSize - FileOffset))
				{
					FileOffset += DataToSkip;

					/* offset is within file, and there is at least one byte in the file */
					if (DataToSkip!=0)
					{
						/* skip some data */
						VOC_Validate_SkipData(FileHandle,DataToSkip);
					}

					/* loop through all blocks */
					do
					{
						BOOL	ReadFullHeader;
						unsigned long DataRemaining;
						unsigned long BlockSize;
						unsigned long SizeToRead;
						
						/* get size of data remaining */
						DataRemaining = FileSize - FileOffset;

						/* size of data remaining is less than a block header? */
						if (DataRemaining<sizeof(VOC_BLOCK_HEADER))
						{
							/* size of data remaining is less than VOC_BLOCK_HEADER */
							SizeToRead = DataRemaining;
							ReadFullHeader = FALSE;							
						}
						else
						{
							/* size of data is greater or equal to VOC_BLOCK_HEADER */
							SizeToRead = sizeof(VOC_BLOCK_HEADER);
							ReadFullHeader = TRUE;
						}

						/* update offset */
						FileOffset+= SizeToRead;

						/* read data */
						Host_ReadData(FileHandle, (unsigned char *)&VocBlockHeader, SizeToRead);

						/* get size of data remaining */
						DataRemaining = FileSize - FileOffset;

						if (ReadFullHeader)
						{
							/* read a full header */

							if (VocBlockHeader.ID!=0)
							{
								/* get block size */
								BlockSize = VOC_GetBlockSize(&VocBlockHeader);

								/* is this block larger or equal to size of data remaining? 
								Indicates this block is too big, or this block goes to end of 
								file and there is no terminator. */
								if (BlockSize>=DataRemaining) 
								{
									Valid = FALSE;
								}
								else
								{
									/* update offset in file */
									FileOffset += BlockSize;
		
									/* skip data */
									VOC_Validate_SkipData(FileHandle, BlockSize);
								}
							}
						}
						else
						{
							/* didn't read a full header - in this case the only valid 
							block is the terminator */

							if (VocBlockHeader.ID!=0)
							{
								Valid = FALSE;
							}
						}

					} 
					while ((Valid) && (VocBlockHeader.ID!=0));
				}
			}
		}
	
		/* close the file */
		Host_CloseFile(FileHandle);
	}

	return Valid;
}

void	VOC_Open(SAMPLE_AUDIO_STREAM *pAudioStream)
{	
	VOC_MAIN_HEADER VocHeader[32];

	/* clear flags */
	VocAudioStream.Flags = 0;

	/* get header */
	Sample_ReadData(pAudioStream, (unsigned char *)&VocHeader[0], sizeof(VOC_MAIN_HEADER));

/*	if (VOC_IdentifyHeader(&VocHeader[0])) */
	{
		unsigned long OffsetOfFirstDataBlock;
		unsigned long DataToSkip;

		OffsetOfFirstDataBlock = VocHeader->FirstBlockOffsetLow | 
				(VocHeader->FirstBlockOffsetHigh<<8);
		
		DataToSkip = OffsetOfFirstDataBlock - sizeof(VOC_MAIN_HEADER);

		if (DataToSkip!=0)
		{
			Sample_SkipData(pAudioStream, DataToSkip);
		}

		VOC_GetNextDataBlock(pAudioStream);

	}
}



unsigned char VOC_GetDataByte(SAMPLE_AUDIO_STREAM *pAudioStream)
{
	VOC_AUDIO_STREAM *pVocAudioStream = &VocAudioStream;
	
	if (pVocAudioStream->CurrentBlockOffset==pVocAudioStream->CurrentBlockSize)
	{
		VOC_GetNextDataBlock(pAudioStream);
	}
	else
	{
		/* increment position in stream */
		pVocAudioStream->CurrentBlockOffset+=((pAudioStream->SampleBits>>3)*(pAudioStream->SampleChannels));
	}

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

			return (unsigned char)(((SampleData1 + SampleData2)>>1)>>8);
		}
	}
}



