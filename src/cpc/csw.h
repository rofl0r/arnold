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
#ifndef __CSW_INPUT_DRIVER_HEADER_INCLUDED__
#define __CSW_INPUT_DRIVER_HEADER_INCLUDED__

/* identification */
#define CSW_IDENT "Compressed Square Wave\x01a"

#define CSW_COMPRESSION_RLE 0x01

#define CSW_POLARITY_MASK (1<<0)




typedef struct 
{
	/* identification string */
 	unsigned char Ident[0x017];
	/* version */
	unsigned char MajorFormatRevision;
	unsigned char MinorFormatRevision;
} CSW_FILE_COMMON_HEADER;

/* common header first then this header */

typedef struct 
{

	/* sample rate */
	unsigned char SampleRateHzL;
	unsigned char SampleRateHzH;
	/* compression type */
	unsigned char CompressionType;
	/* flags */
	unsigned char Flags;
	/* reserved */
	unsigned char Reserved[3];
} CSW_FILE_HEADER_V1;

/* common header first then this header */

typedef struct 
{
	/* sample rate */
	unsigned char SampleRateHzL;
	unsigned char SampleRateHzML;
	unsigned char SampleRateHzMH;
	unsigned char SampleRateHzH;
	/* total number of pulses after decompression */
	unsigned char PulseTotalL;
	unsigned char PulseTotalML;
	unsigned char PulseTotalMH;
	unsigned char PulseTotalH;
	/* compression type */
	unsigned char CompressionType;
	/* flags */
	unsigned char Flags;
	/* reserved */
	unsigned char HeaderExtensionLength;

	unsigned char EncodingApplication[16];
} CSW_FILE_HEADER_V2;

/* compression information:

  byte-code

  if byte-code!=0, then byte-code defines number of samples that wave is at current level,
  if byte-code==0, then read 4-bytes (little endian format) which defines the number of samples that wave is at
  current level.

*/

#include "sampload.h"

BOOL	CSW_Validate(const char *);
void	CSW_Open(SAMPLE_AUDIO_STREAM *);
unsigned char CSW_GetDataByte(SAMPLE_AUDIO_STREAM *pAudioStream);

#endif

