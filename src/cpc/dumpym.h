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
#ifndef __DUMP_YM_HEADER_INCLUDED__
#define __DUMP_YM_HEADER_INCLUDED__

#include "cpcglob.h"

/* register was updated this frame */
#define AY_INFO_REG_UPDATED 0x0001
/* register data is different from previous frame */
#define AY_INFO_REG_DATA_CHANGED 0x0002

typedef struct
{
	unsigned char RegisterData[16];
	unsigned char PreviousRegisterData[16];
	unsigned char Flags[16];
} AY_INFO;

void	YMOutput_Finish(void);
void	YMOutput_StoreRegData(int PSG_SelectedRegister, int Data);
void	YMOutput_SetName(unsigned char *);
void	YMOutput_SetAuthor(unsigned char *);
void	YMOutput_SetComment(unsigned char *);
unsigned char *YMOutput_GetComment(void);
unsigned char *YMOutput_GetName(void);
unsigned char *YMOutput_GetAuthor(void);

/* true if output is silent, false otherwise */
BOOL YMOutput_IsSilent(void);

/* calculate size for YM header */
unsigned long YMOutput_GenerateHeaderOutputSize(int nVersion);

/* calculate size for YM trailer (if present in chosen format */
unsigned long YMOutput_GenerateTrailerOutputSize(int nVersion);

/* validate the output version supported */
int		YMOutput_ValidateVersion(int Version);

/* generate a record which can be written to a temporary file */
/* record is 16 bytes long */
void	YMOutput_GenerateTempRecord(unsigned char *Regs);

/* setup header data */
void YMOutput_GenerateHeaderData(unsigned char *pData, int nVersion, int nVBL);
/* setup trailer data */
void YMOutput_GenerateTrailerData(unsigned char *pData, int nVersion);

/* convert the data */
void YMOutput_ConvertTempData(const unsigned char *pSrcData, unsigned char *pDestData, int nVersion, int nVBL);

#endif
