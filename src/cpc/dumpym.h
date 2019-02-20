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

#define AY_INFO_REG_CHANGED	0x0001

typedef struct	AY_INFO
{
	unsigned char RegisterData;
	unsigned char Flags;
} AY_INFO;

void	YMOutput_Init(char *pTempFilename);
void	YMOutput_Finish(void);
void	YMOutput_WriteRegs(void);
void	YMOutput_StartRecording(char *,int);
void	YMOutput_StopRecording(void);
void	YMOutput_ClearRegStatus(void);
void	YMOutput_StoreRegData(int PSG_SelectedRegister, int Data);
void	YMOutput_SetStartRecordingState(BOOL State);
void	YMOutput_SetName(unsigned char *);
void	YMOutput_SetAuthor(unsigned char *);
void	YMOutput_SetComment(unsigned char *);
unsigned char *YMOutput_GetComment(void);
unsigned char *YMOutput_GetName(void);
unsigned char *YMOutput_GetAuthor(void);


#endif
