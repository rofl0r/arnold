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

#define PRINTER_OUTPUT_DO_NOTHING		0x0000
#define PRINTER_OUTPUT_TO_FILE			0x0001
#define PRINTER_OUTPUT_TO_PRINTER		0x0002
#define PRINTER_OUTPUT_TO_DIGIBLASTER	0x0003

void	Printer_SetDataBit7State(int);
int		Printer_GetDataBit7State(void);
void	Printer_SetOutputMethod(int);
int		Printer_GetOutputMethod(void);
void	Printer_WriteDataByte(int);

void	Printer_Initialise(void);
void	Printer_Finish(void);
unsigned char Printer_GetDataByte(void);

#endif


