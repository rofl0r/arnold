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
#include "z80/z80.h"
#include "debugmain.h"
#include "cpc.h"

void    CPC_DumpRamToBuffer(char *pBuffer)
{
        int i;

        for (i=0; i<0x010000; i++)
        {
                Z80_BYTE        Data;

                Data = Z80_RD_MEM((Z80_WORD)i);

                pBuffer[i] = (char)Data;
        }
}

void    CPC_DumpBaseRamToBuffer(char *pBuffer)
{
        int i;

        for (i=0; i<0x010000; i++)
        {
                Z80_BYTE        Data;

                Data = Z80_RD_BASE_BYTE((Z80_WORD)i);

                pBuffer[i] = (char)Data;
        }
}


#if 0
static BOOL DebugHooks_WriteMemory_Activated = FALSE;
static BOOL DebugHooks_ReadMemory_Activated = FALSE;
static BOOL DebugHooks_WriteIO_Activated = FALSE;
static BOOL DebugHooks_ReadIO_Activated = FALSE;

void    DebugHooks_WriteIO_Active(BOOL State)
{
    DebugHooks_WriteIO_Activated = State;
}

void    DebugHooks_ReadIO_Active(BOOL State)
{
    DebugHooks_ReadIO_Activated = State;
}

BOOL    DebugHooks_ReadMemory_GetActiveState(void)
{
    return DebugHooks_ReadMemory_Activated;
}

BOOL    DebugHooks_WriteMemory_GetActiveState(void)
{
    return DebugHooks_WriteMemory_Activated;
}

BOOL    DebugHooks_ReadIO_GetActiveState(void)
{
    return DebugHooks_ReadIO_Activated;
}

BOOL    DebugHooks_WriteIO_GetActiveState(void)
{
    return DebugHooks_WriteIO_Activated;
}

/* set DebugHooks state */
void    DebugHooks_WriteMemory_Active(BOOL State)
{
    DebugHooks_WriteMemory_Activated = State;
}

void    DebugHooks_ReadMemory_Active(BOOL State)
{
   DebugHooks_ReadMemory_Activated = State;
}



/* detect if a write I/O operation will occur
 this instruction */
#ifdef DEBUG_HOOKS
		/* debugger trap read i/o */
        if (DebugHooks_ReadIO_Activated)
                Debug_ReadIO(Port, Data);

		/* debugger trap write I/o */
        if (DebugHooks_WriteIO_Activated)
                Debug_WriteIO(Port,Data);

        /* debugger trap writes */
        Debug_WriteMemory(Addr,Data);
#endif

#endif


#if 0
#ifdef DEBUG_HOOKS

static char *IO_ID_Text[]=
{
        "Gate Array Write",
        "Expansion Rom Select",
        "CRTC Write Data",
        "CRTC Write Register"
};

char *CPC_Debug_GetTextForIOID(CPC_IO_ID IOID)
{
        return IO_ID_Text[IOID];
}

void    CPC_Debug_SetupIOCompare(DEBUG_CMP_STRUCT *pComparison, CPC_IO_ID IOPort)
{
        /* want to compare addr */
        pComparison->CompareWhat |= DEBUG_CMP_ADDR;

        switch (IOPort)
        {
                case IO_CRTC_WRITE_DATA:
                {
                        
                        /* logical AND address with value1. Result must match value 2 */
                        pComparison->AddressCompare.Comparison = DEBUG_CMP_AND_MASK;

                        pComparison->AddressCompare.Value1 = 0x04300;
                        pComparison->AddressCompare.Value2 = 0x01<<8;
                }
                break;

                case IO_CRTC_WRITE_REG:
                {
                        pComparison->AddressCompare.Comparison = DEBUG_CMP_AND_MASK;
                        pComparison->AddressCompare.Value1 = 0x04300;
                        pComparison->AddressCompare.Value2 = 0x0;
                }
                break;

                case IO_GATE_ARRAY_WRITE:
                {
                        pComparison->AddressCompare.Comparison = DEBUG_CMP_AND_MASK;
                        pComparison->AddressCompare.Value1 = 0x08000;
                        pComparison->AddressCompare.Value2 = 0x0;
                }
                break;
        }
}

#endif


#endif

