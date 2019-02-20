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
#ifndef __CPC_PRINTER_HEADER_INCLUDED__
#define __CPC_PRINTER_HEADER_INCLUDED__

typedef struct
{
	int CurrentBit7State;
	int PreviousBit7State;
	int CurrentDataByte;
	int PreviousDataByte;
} CPC_PRINTER_HW;

void	Printer_SetDataBit7State(int);
int		Printer_GetDataBit7State(void);
void	Printer_WriteDataByte(int);

void	Printer_Initialise(void);
void	Printer_Finish(void);

/* This is the value of the last data byte which was written to port &efxx */
/* This function is used by the snapshot load/save functions */

/* bit 0-6 are data, bit 7 is strobe.*/
unsigned char Printer_GetDataByte(void);

/* this is the actual value of the 8-bit data from the printer connector */
/* for CPC, bit 7 will always be 0, unless a 8-bit printer port hardware
modification has been made; for the CPC+, bit 7 will be 0 or 1 depending
on the state set through CRTC register 12 */
/* bit 7-0 are data */
unsigned char Printer_Get8BitData(void);
unsigned char Printer_GetPrevious8BitData(void);

/* get the state of the strobe */
int		Printer_GetStrobeState(void);
int		Printer_GetPreviousStrobeState(void);

/* set the state of the busy input to the printer connector */
void	Printer_SetBusyInput(int);

#endif


