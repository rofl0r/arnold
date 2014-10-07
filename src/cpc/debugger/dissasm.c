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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "../cpcglob.h"
#include "../z80/z80.h"
#include "dissasm.h"
#include "gdebug.h"
#include "breakpt.h"
#include "../messages.h"

DISSASSEMBLE_WINDOW *Dissassemble_Create(void)
{
	DISSASSEMBLE_WINDOW *pDissassembleWindow;

	pDissassembleWindow = (DISSASSEMBLE_WINDOW *)malloc(sizeof(DISSASSEMBLE_WINDOW));

	if (pDissassembleWindow!=NULL)
	{
		memset(pDissassembleWindow, 0, sizeof(DISSASSEMBLE_WINDOW));
	}

	return pDissassembleWindow;

}

/* what the following piece of code does is the following:

  When we go up a line it attempts to find the instruction,
  which when the opcode count is added onto the address for
  that instruction will give the address of the instruction
  on the screen */
#define MAX_INSTRUCTION_SIZE 4

int		Dissassemble_GetPreviousPageBase(DISSASSEMBLE_WINDOW *pWindow, int Address)
{
	int i;
	int OpcodeCount;
	int CurrentBaseAddress;

	CurrentBaseAddress = Address;

	/* maximum number of bytes in the dissassembly view =
	 max number of bytes per instruction * window height

	 The aim of the following code is to work out a suitable base address
	 for the previous page of dissassembly, so that if we dissassemble from this address
	 we end up with pWindow->WindowHeight lines or greater, before we reach "Address".
	
	 If the number of lines is greater than "pWindow->WindowHeight", we adjust the address
	 by the size of the first opcode. Hopefully this should give a close approximation to
	 paging up.
	*/
	for (i=0; i<(MAX_INSTRUCTION_SIZE*(pWindow->WindowHeight+1)); i++)
	{
		int TempAddress;
		int LineCount;
		
		LineCount = 0;
		TempAddress = CurrentBaseAddress;
		do
		{
			/* get opcode count for this address */
			OpcodeCount = pWindow->GetOpcodeCountFunction(TempAddress);

			TempAddress += OpcodeCount;

			if (TempAddress<=Address)
			{
				LineCount++;
			}
		}
		while ((TempAddress<Address) && (LineCount<pWindow->WindowHeight));

		if (LineCount >= pWindow->WindowHeight)
		{
			if (LineCount>pWindow->WindowHeight)
			{
				CurrentBaseAddress += pWindow->GetOpcodeCountFunction(CurrentBaseAddress);
			}
			return CurrentBaseAddress;
		}

		CurrentBaseAddress--;
	}

	/* panic!!! */
	return Address;
}


/* cursor up and page up are more difficult, because the opcodes
can be any size */
void	Dissassemble_CursorUp(DISSASSEMBLE_WINDOW *pWindow)
{
	pWindow->CursorYRelative--;

	if (pWindow->CursorYRelative<0)
	{
		int i;
		int Addr;
		int OpcodeCount;

		int PreviousPageBase;
		
		pWindow->CursorYRelative = 0;

		PreviousPageBase = Dissassemble_GetPreviousPageBase(pWindow,pWindow->BaseAddress);

		
		Addr = PreviousPageBase;
		
		for (i=0; i<pWindow->WindowHeight-1; i++)
		{
			/* update opcode count */
			OpcodeCount = pWindow->GetOpcodeCountFunction(Addr & 0x0ffff);
			
			Addr+=OpcodeCount;
		}
		pWindow->BaseAddress = Addr;
	}

	pWindow->CursorYAbsolute = pWindow->CursorYRelative;
}


void	Dissassemble_CursorDown(DISSASSEMBLE_WINDOW *pWindow)
{
	pWindow->CursorYRelative++;

	if (pWindow->CursorYRelative>=(pWindow->WindowHeight-1))
	{
		pWindow->CursorYRelative = (pWindow->WindowHeight-1);
		pWindow->BaseAddress += pWindow->GetOpcodeCountFunction(pWindow->BaseAddress & 0x0ffff);
	}

	pWindow->CursorYAbsolute = pWindow->CursorYRelative;
}


void	Dissassemble_PageUp(DISSASSEMBLE_WINDOW *pWindow)
{
	pWindow->BaseAddress = Dissassemble_GetPreviousPageBase(pWindow,pWindow->BaseAddress);
}

/* page down is easy because we can calculate the last address
we are displaying and show that for the next page */
void	Dissassemble_PageDown(DISSASSEMBLE_WINDOW *pWindow)
{
	int i;
	int Addr;
	int OpcodeCount;

	Addr = pWindow->BaseAddress;

	for (i=0; i<pWindow->WindowHeight; i++)
	{
		/* update opcode count */
        OpcodeCount = pWindow->GetOpcodeCountFunction(Addr & 0x0ffff);
		
		Addr+=OpcodeCount;
	}

	pWindow->BaseAddress = Addr;

}


/* call after a resize operation */
void	Dissassemble_RefreshState(DISSASSEMBLE_WINDOW *pWindow)
{
	/* ensure Y position is valid */
	if (pWindow->CursorYRelative>=pWindow->WindowHeight)
	{
		pWindow->CursorYRelative = pWindow->WindowHeight-1;
	}

	pWindow->CursorYAbsolute = pWindow->CursorYRelative;
}

void		Dissassemble_SelectByCharXY(DISSASSEMBLE_WINDOW *pDissassembleWindow, int CharX,int CharY)
{
	pDissassembleWindow->CursorYRelative = CharY;
	pDissassembleWindow->CursorYAbsolute = CharY;
}


int		Dissassemble_GetCursorAddress(DISSASSEMBLE_WINDOW *pWindow)
{
	int i;
	int Addr;
	int OpcodeCount;

	Addr = pWindow->BaseAddress;

	for (i=0; i<pWindow->CursorYRelative; i++)
	{
		/* update opcode count */
        OpcodeCount = pWindow->GetOpcodeCountFunction(Addr & 0x0ffff);
		
		Addr+=OpcodeCount;
	}

	return Addr;
}


void	Dissassemble_SetAddress(DISSASSEMBLE_WINDOW *pWindow, int Address)
{
	int i;
	int Addr;
	int OpcodeCount;

	Addr = pWindow->BaseAddress;

	for (i=0; i<pWindow->WindowHeight; i++)
	{
		/* don't set address it is already visible */
		if (Addr == Address)
		{
			return;
		}

		/* update opcode count */
        OpcodeCount = pWindow->GetOpcodeCountFunction(Addr & 0x0ffff);
		
		Addr+=OpcodeCount;
	}

	/* address is not visible. Set new address */

	pWindow->BaseAddress = Address;

}


void	Dissassemble_ToggleOpcodes(DISSASSEMBLE_WINDOW *pDissassembleWindow)
{
	pDissassembleWindow->PersistentFlags ^= DISSASSEMBLE_FLAGS_SHOW_OPCODES;
}

void	Dissassemble_ToggleAscii(DISSASSEMBLE_WINDOW *pDissassembleWindow)
{
	pDissassembleWindow->PersistentFlags ^= DISSASSEMBLE_FLAGS_SHOW_ASCII;
}

void	Dissassemble_ToggleBreakpoint(DISSASSEMBLE_WINDOW *pDissassembleWindow)
{
	int Addr;

	if (pDissassembleWindow->ViewType != DISSASSEMBLE_VIEW_Z80_INSTRUCTIONS)
		return;

	Addr = Dissassemble_GetCursorAddress(pDissassembleWindow);

	/* breakpoint exists? */
	if (Breakpoints_IsABreakpoint(Addr))
	{
		/* yes, remove it */
		Breakpoints_RemoveBreakpoint(Addr);
	}
	else
	{
		/* no, add one */
		Breakpoints_AddBreakpoint(Addr);
	}
}



void	Dissassemble_Finish(DISSASSEMBLE_WINDOW *pDissassembleWindow)
{
	if (pDissassembleWindow!=NULL)
	{
		free(pDissassembleWindow);
	}
}


void Dissassemble_GetPC(DISSASSEMBLE_WINDOW *pDissassembleWindow)
{
	switch (pDissassembleWindow->ViewType)
	{
		case DISSASSEMBLE_VIEW_Z80_INSTRUCTIONS:
		{
			pDissassembleWindow->PC = Z80_GetReg(Z80_PC);
		}
		break;
		
		case DISSASSEMBLE_VIEW_DMA_CHANNEL_0_INSTRUCTIONS:
		case DISSASSEMBLE_VIEW_DMA_CHANNEL_1_INSTRUCTIONS:
		case DISSASSEMBLE_VIEW_DMA_CHANNEL_2_INSTRUCTIONS:
		{
			pDissassembleWindow->PC = ASIC_DMA_GetChannelAddr(pDissassembleWindow->ViewType - DISSASSEMBLE_VIEW_DMA_CHANNEL_0_INSTRUCTIONS);
		}
		break;
	
		default:
			break;
	}
}


int	Dissassemble_SetDissassemblyFlagsBasedOnAddress(DISSASSEMBLE_WINDOW *pDissassembleWindow,int Address)
{
	int DissassemblyFlags;

	DissassemblyFlags = 0;
	
	/* mark line with PC marker */
	if (Address == pDissassembleWindow->PC)
	{
		DissassemblyFlags |= DISSASSEMBLE_FLAGS_MARK_AS_PC;
	}

	if (pDissassembleWindow->ViewType == DISSASSEMBLE_VIEW_Z80_INSTRUCTIONS)
	{
		if (Breakpoints_IsABreakpoint(Address))
		{
			DissassemblyFlags |= DISSASSEMBLE_FLAGS_MARK_AS_BREAKPOINT;
		}
	}

	return DissassemblyFlags;

}

static char DissassembleString[256];

char    *Dissassemble_DissassembleLine(DISSASSEMBLE_WINDOW *pWindow, int Addr, int *OpcodeSize, unsigned long Flags)
{
        int OpcodeCount;
		int x;

        /* write address */
        sprintf(DissassembleString,"%04x:  ", Addr & 0x0ffff);

        OpcodeCount = pWindow->GetOpcodeCountFunction(Addr);

		x = 7;

        /* display opcodes ?*/
		if (Flags & DISSASSEMBLE_FLAGS_SHOW_OPCODES)
		{
			int i;
			int OpcodeX;

			OpcodeX = 0;

			for (i=0; i<OpcodeCount; i++)
			{
				sprintf(&DissassembleString[x + OpcodeX],"%02x ",Z80_RD_MEM(Addr + i));

				OpcodeX+=3;
			}
		
			memset(&DissassembleString[x + OpcodeX], ' ', 4*3 - (OpcodeX));

			/* max number of bytes per opcode is 4 */
			x += 4*3;
	
			memset(&DissassembleString[x], ' ', 3);
			x+=3;

		}

		
	
		if (Flags & DISSASSEMBLE_FLAGS_SHOW_ASCII)
		{
			int i;
			int byte;

		
			for (i=0; i<OpcodeCount; i++)
			{
				byte = Z80_RD_MEM(Addr + i);

			     /* convert un-printable chars into '.' */
				if ((byte<21) || (byte>127))
					byte = '.';

				DissassembleString[x+i] = byte;
			}

			memset(&DissassembleString[x + OpcodeCount], ' ', 4*1 - (OpcodeCount));

			x += 4*1;

			memset(&DissassembleString[x], ' ', 3);
			x+=3;
		}




		/* highlight program counter? */
		if (Flags & DISSASSEMBLE_FLAGS_MARK_AS_BREAKPOINT)
		{
			DissassembleString[x] = '*';
		}
		else
		{
			DissassembleString[x] = ' ';
		}

		x++;

		/* highlight program counter? */
		if (Flags & DISSASSEMBLE_FLAGS_MARK_AS_PC)
		{
			DissassembleString[x] = '>';
		}
		else
		{
			DissassembleString[x] = ' ';
		}
		x++;

        /* write mneumonic */
        pWindow->DissassembleInstruction(Addr & 0x0ffff, &DissassembleString[x]);

        *OpcodeSize = OpcodeCount;
        
		return DissassembleString;
}

void	Dissassemble_BeginDissassemble(DISSASSEMBLE_WINDOW *pDissassembleWindow)
{
	pDissassembleWindow->CurrentAddr = pDissassembleWindow->BaseAddress;

	Dissassemble_GetPC(pDissassembleWindow);


	switch (pDissassembleWindow->ViewType)
	{
		case DISSASSEMBLE_VIEW_Z80_INSTRUCTIONS:
		{
			pDissassembleWindow->GetOpcodeCountFunction = Debug_GetOpcodeCount;
			pDissassembleWindow->DissassembleInstruction = Debug_DissassembleInstruction;
			pDissassembleWindow->CurrentAddr &= 0x0ffff;
		}
		break;

		case DISSASSEMBLE_VIEW_DMA_CHANNEL_0_INSTRUCTIONS:
		case DISSASSEMBLE_VIEW_DMA_CHANNEL_1_INSTRUCTIONS:
		case DISSASSEMBLE_VIEW_DMA_CHANNEL_2_INSTRUCTIONS:
		{

			pDissassembleWindow->GetOpcodeCountFunction = ASIC_DMA_GetOpcodeCount;
			pDissassembleWindow->DissassembleInstruction = ASIC_DMA_DissassembleInstruction;
			pDissassembleWindow->CurrentAddr &= (0x0ffff^1);
		}
		break;

		default:
			break;
	}

}


char *Dissassemble_DissassembleNextLine(DISSASSEMBLE_WINDOW *pDissassembleWindow)
{
	int	OpcodeCount;
	char *pDebugString;
	unsigned long DissassemblyFlags;

	DissassemblyFlags = Dissassemble_SetDissassemblyFlagsBasedOnAddress(pDissassembleWindow,pDissassembleWindow->CurrentAddr);
	DissassemblyFlags |= pDissassembleWindow->PersistentFlags;

	pDebugString = Dissassemble_DissassembleLine(pDissassembleWindow,pDissassembleWindow->CurrentAddr, &OpcodeCount,DissassemblyFlags);

	pDissassembleWindow->CurrentAddr+= OpcodeCount;

	return pDebugString;
}

void	Dissassemble_ToggleView(DISSASSEMBLE_WINDOW *pDissassembleWindow)
{
	pDissassembleWindow->ViewType++;

	if (pDissassembleWindow->ViewType>DISSASSEMBLE_VIEW_LAST)
	{
		pDissassembleWindow->ViewType = DISSASSEMBLE_VIEW_FIRST;
	}

	Dissassemble_RefreshState(pDissassembleWindow);
}

char	*Dissassemble_GetViewName(DISSASSEMBLE_WINDOW *pDissassembleWindow)
{
	switch (pDissassembleWindow->ViewType)
	{
		case DISSASSEMBLE_VIEW_Z80_INSTRUCTIONS:
			return Messages[115];
		
		case DISSASSEMBLE_VIEW_DMA_CHANNEL_0_INSTRUCTIONS:
			return Messages[116];

		case DISSASSEMBLE_VIEW_DMA_CHANNEL_1_INSTRUCTIONS:
			return Messages[117];

		case DISSASSEMBLE_VIEW_DMA_CHANNEL_2_INSTRUCTIONS:
			return Messages[118];
		
			default:
			break;
	}

	return "";
}