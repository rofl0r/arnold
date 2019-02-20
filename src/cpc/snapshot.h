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
#ifndef __SNAPSHOT_HEADER_INCLUDED__
#define __SNAPSHOT_HEADER_INCLUDED__


#include "cpcglob.h"

#define SNAPSHOT_HEADER_TEXT "MV - SNA"

typedef struct SNAPSHOT_HEADER
{
	char	SnapshotID[8];
	char	pad0[8];
	char	Version;
	char	Z80_F;
	char	Z80_A;
	char	Z80_C;
	char	Z80_B;
	char	Z80_E;
	char	Z80_D;
	char	Z80_L;
	char	Z80_H;
	char	Z80_R;
	char	Z80_I;
	char	Z80_IFF0;
	char	Z80_IFF1;
	char	Z80_LIX;
	char	Z80_HIX;
	char	Z80_LIY;
	char	Z80_HIY;
	char	Z80_LSP;
	char	Z80_HSP;
	char	Z80_LPC;
	char	Z80_HPC;
	char	Z80_IM;
	char	Z80_F_alt;
	char	Z80_A_alt;
	char	Z80_C_alt;
	char	Z80_B_alt;
	char	Z80_E_alt;
	char	Z80_D_alt;
	char	Z80_L_alt;
	char	Z80_H_alt;

	char	GateArray_Pen;
	char	GateArray_Inks[17];

	char	GateArray_RomConfig;
	char	GateArray_RamConfig;

	char	CRTC_Register;
	char	CRTC_Registers[18];

	char	ROM_UpperRom;

	char	PIO_PortA,PIO_PortB,PIO_PortC, PIO_Control;

	char	PSG_Register;
	char	PSG_Registers[16];

	char	MemSizeLow;
	char	MemSizeHigh;

	char	CPCType;

	char	InterruptBlock;

	char	MultiMode[6];

	char	pad1[0x0100-0x075];
} SNAPSHOT_HEADER;


BOOL	Snapshot_Load(char *);
BOOL	Snapshot_Save(char *,int,int);

#endif
