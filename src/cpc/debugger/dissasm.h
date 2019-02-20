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
#ifndef __DISSASSEMBLY_WINDOW_HEADER_INCLUDED__
#define __DISSASSEMBLY_WINDOW_HEADER_INCLUDED__

#define DISSASSEMBLE_FLAGS_SHOW_OPCODES			0x0001
#define DISSASSEMBLE_FLAGS_SHOW_ASCII			0x0002
#define DISSASSEMBLE_FLAGS_MARK_AS_PC			0x0004
#define DISSASSEMBLE_FLAGS_MARK_AS_BREAKPOINT	0x0008
#define DISSASSEMBLE_FLAGS_SHOW_INFORMATION		0x0010


typedef enum
{
	DISSASSEMBLE_VIEW_Z80_INSTRUCTIONS = 0,
	DISSASSEMBLE_VIEW_DMA_CHANNEL_0_INSTRUCTIONS,
	DISSASSEMBLE_VIEW_DMA_CHANNEL_1_INSTRUCTIONS,
	DISSASSEMBLE_VIEW_DMA_CHANNEL_2_INSTRUCTIONS
} DISSASSEMBLE_VIEW_TYPE;

#define DISSASSEMBLE_VIEW_FIRST		DISSASSEMBLE_VIEW_Z80_INSTRUCTIONS
#define DISSASSEMBLE_VIEW_LAST		DISSASSEMBLE_VIEW_DMA_CHANNEL_2_INSTRUCTIONS


typedef struct
{
	/* these flags are toggled on/off */
	unsigned long PersistentFlags;

	int BaseAddress;

	int WidthInChars;
	int WindowHeight;
	int CursorXRelative;
	int CursorYRelative;
	int CursorYAbsolute;


	int *AddressessVisible;

	int ViewType;

	/* current address that is being dissassembled */
	int	CurrentAddr;

	/* program counter - used for displaying dissassembled listing */
	int PC;

	int	(*GetOpcodeCountFunction)(int);
	void (*DissassembleInstruction)(int, char *);
} DISSASSEMBLE_WINDOW;

DISSASSEMBLE_WINDOW *Dissassemble_Create(void);
void	Dissassemble_CursorUp(DISSASSEMBLE_WINDOW *pWindow);
void	Dissassemble_CursorDown(DISSASSEMBLE_WINDOW *pWindow);
void		Dissassemble_SelectByCharXY(DISSASSEMBLE_WINDOW *pDissassembleWindow, int CharX,int CharY);
int		Dissassemble_GetCursorAddress(DISSASSEMBLE_WINDOW *pWindow);
void	Dissassemble_SetAddress(DISSASSEMBLE_WINDOW *pWindow, int Address);
void	Dissassemble_ToggleOpcodes(DISSASSEMBLE_WINDOW *pDissassembleWindow);
void	Dissassemble_ToggleAscii(DISSASSEMBLE_WINDOW *pDissassembleWindow);
void	Dissassemble_ToggleBreakpoint(DISSASSEMBLE_WINDOW *pDissassembleWindow);
void	Dissassemble_Finish(DISSASSEMBLE_WINDOW *pDissassembleWindow);
int	Dissassemble_SetDissassemblyFlagsBasedOnAddress(DISSASSEMBLE_WINDOW *,int Address);
char    *Dissassemble_DissassembleLine(DISSASSEMBLE_WINDOW *pWindow, int Addr, int *OpcodeSize, unsigned long Flags);
void	Dissassemble_PageDown(DISSASSEMBLE_WINDOW *pWindow);
void	Dissassemble_RefreshState(DISSASSEMBLE_WINDOW *pWindow);
void	Dissassemble_PageUp(DISSASSEMBLE_WINDOW *pWindow);
void	Dissassemble_BeginDissassemble(DISSASSEMBLE_WINDOW *pDissassembleWindow);
char *Dissassemble_DissassembleNextLine(DISSASSEMBLE_WINDOW *pDissassembleWindow);
char	*Dissassemble_GetViewName(DISSASSEMBLE_WINDOW *pDissassembleWindow);
void	Dissassemble_ToggleView(DISSASSEMBLE_WINDOW *pDissassembleWindow);



#endif

