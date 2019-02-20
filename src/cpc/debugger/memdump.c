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
#include "memdump.h"

#define MEMDUMP_DATA_LEFT_EDGE	7

static void	MemDump_SetupView(MEMDUMP_WINDOW *pMemdumpWindow)
{
	int NumBytes, NumCharsPerElement,Spacing;

	switch (pMemdumpWindow->ViewType)
	{
		default:
		case MEMDUMP_VIEW_BYTES:
		{
			NumBytes = 1;
			NumCharsPerElement = 2;
			Spacing = 1;
		}
		break;

		case MEMDUMP_VIEW_WORDS:
		{
			NumBytes = 2;
			NumCharsPerElement = 4;
			Spacing = 2;
		}
		break;
	}

	pMemdumpWindow->NumberOfBytesPerElement = NumBytes;
	pMemdumpWindow->NumberOfCharsPerElement = NumCharsPerElement;
	pMemdumpWindow->Spacing = Spacing;

	MemDump_RefreshState(pMemdumpWindow);
}


static BOOL IsHexDigit(char ch)
{
	/* is a digit */
	if ((ch>='0') && (ch<='9'))
		return TRUE;

	ch = toupper(ch);

	/* is a letter between A and F */
	if ((ch>='A') && (ch<='F'))
		return TRUE;

	return FALSE;
}

static int HexDigitToInt(char ch)
{
	if (isdigit(ch))
		return ch-'0';

	return toupper(ch)-'A'+10;
}

/* get memory address for start of line */
int Memdump_GetLineAddress(MEMDUMP_WINDOW *pMemdumpWindow, int Line)
{
	return pMemdumpWindow->BaseAddress + (Line*pMemdumpWindow->NumberOfElementsVisible*pMemdumpWindow->NumberOfBytesPerElement);
}

/* fill text buffer with memory dump display */
static char *Memdump_DumpLine_ByteView(MEMDUMP_WINDOW *pMemdumpWindow, int Line)
{
    int i;
	int x;
	char *pString = (char *) pMemdumpWindow->pBuffer;
	int Addr;

	Addr = Memdump_GetLineAddress(pMemdumpWindow, Line);
	
    /* show memory address */
    sprintf(&pString[0],"%04x:  ",Addr & 0x0ffff);
    
	x = MEMDUMP_DATA_LEFT_EDGE;
    /* show bytes */
    for (i=0; i<pMemdumpWindow->NumberOfElementsVisible; i++)
    {
        sprintf(&pString[x],"%02x ",(unsigned char)pMemdumpWindow->pReadOperation(Addr + i,1));

		x+=3;
	}

    /* bytes seperating text dump */
    sprintf(&pString[x],"  ");

	x+=2;

    /* dump text */
    for (i=0; i<pMemdumpWindow->NumberOfElementsVisible; i++)
    {
        unsigned char   ch;

        ch = (unsigned char)pMemdumpWindow->pReadOperation(Addr,1);
		Addr++;

        /* convert un-printable chars into '.' */
        if ((ch<21) || (ch>127))
                ch = '.';

        sprintf(&pString[x],"%c",ch);
		x++;
	}

    /* return mem-dump string */
    return (char *) pMemdumpWindow->pBuffer;
}


/* fill text buffer with memory dump display */
/* will only work with little endian systems */
static char *Memdump_DumpLine_WordView(MEMDUMP_WINDOW *pMemdumpWindow, int Line)
{
    int i;
	int x;
	char *pString = (char *) pMemdumpWindow->pBuffer;
	int Addr;

	Addr = Memdump_GetLineAddress(pMemdumpWindow, Line);
	
    /* show memory address */
    sprintf(&pString[0],"%04x:  ",Addr & 0x0ffff);
    
	x = MEMDUMP_DATA_LEFT_EDGE;
    /* show words */
    for (i=0; i<pMemdumpWindow->NumberOfElementsVisible; i++)
    {
		int WordData;
		
		WordData = pMemdumpWindow->pReadOperation(Addr + (i<<1),2);
		
		sprintf(&pString[x],"%04x  ",WordData);
		x+=6;
	}
#if 0
    /* bytes seperating text dump */
    sprintf(&pString[x],"  ");

	x+=2;

    /* dump text */
    for (i=0; i<pMemdumpWindow->NumberOfBytesPerLine; i++)
    {
        unsigned char   ch;

        ch = (unsigned char)pMemdumpWindow->pReadOperation(Addr + i);

        /* convert un-printable chars into '.' */
        if ((ch<21) || (ch>127))
                ch = '.';

        sprintf(&pString[x],"%c",ch);
		x++;
	}
#endif
    /* return mem-dump string */
    return (char *) pMemdumpWindow->pBuffer;
}

char *Memdump_DumpLine(MEMDUMP_WINDOW *pMemdumpWindow, int Line)
{
	switch (pMemdumpWindow->ViewType)
	{
		case MEMDUMP_VIEW_BYTES:
			return Memdump_DumpLine_ByteView(pMemdumpWindow, Line);
		case MEMDUMP_VIEW_WORDS:
			return Memdump_DumpLine_WordView(pMemdumpWindow, Line);
		default:
			break;
	}

	return NULL;
}


int	Memdump_CalcNumberOfVisibleElements(MEMDUMP_WINDOW *pMemdumpWindow,int WindowWidth)
{
	switch (pMemdumpWindow->ViewType)
	{
		case MEMDUMP_VIEW_BYTES:
			return (WindowWidth - (MEMDUMP_DATA_LEFT_EDGE+2))/(pMemdumpWindow->NumberOfCharsPerElement + pMemdumpWindow->Spacing + 1);
		case MEMDUMP_VIEW_WORDS:
			return (WindowWidth - MEMDUMP_DATA_LEFT_EDGE)/(pMemdumpWindow->NumberOfCharsPerElement + pMemdumpWindow->Spacing);
		default:
			break;
	}
	
	return 0;
}


/* recalc absolute position to display cursor */
void	Memdump_UpdateCursorAbsolute(MEMDUMP_WINDOW *pMemdumpWindow)
{
	switch (pMemdumpWindow->EditLocation)
	{
		case MEMDUMP_EDIT_LOCATION_ADDRESS:
		{
			pMemdumpWindow->CursorXAbsolute = pMemdumpWindow->CursorXRelativeFraction;
		}
		break;

		case MEMDUMP_EDIT_LOCATION_DATA:
		{
			pMemdumpWindow->CursorXAbsolute = MEMDUMP_DATA_LEFT_EDGE + (pMemdumpWindow->CursorXRelative*(pMemdumpWindow->NumberOfCharsPerElement+pMemdumpWindow->Spacing)) + pMemdumpWindow->CursorXRelativeFraction;
		}
		break;

		case MEMDUMP_EDIT_LOCATION_TEXT:
		{
			pMemdumpWindow->CursorXAbsolute = MEMDUMP_DATA_LEFT_EDGE + 2 + (pMemdumpWindow->NumberOfElementsVisible*(pMemdumpWindow->NumberOfCharsPerElement+pMemdumpWindow->Spacing)) + pMemdumpWindow->CursorXRelative;
		}
		break;

		default:
			break;
	}

	pMemdumpWindow->CursorYAbsolute = pMemdumpWindow->CursorYRelative;
}


/* make edit changes */
void	Memdump_DoEdit(MEMDUMP_WINDOW *pMemdumpWindow)
{

	/* if no edit in process quit */
	if (pMemdumpWindow->EditOperation==MEMDUMP_EDIT_NONE)
		return;

	switch (pMemdumpWindow->EditLocation)
	{
		/* change address of memory dump */
		case MEMDUMP_EDIT_LOCATION_ADDRESS:
		{
			Memdump_SetCursorAddress(pMemdumpWindow, pMemdumpWindow->EditData);
		}
		break;

		/* write bytes to memory */
		case MEMDUMP_EDIT_LOCATION_DATA:
		{
				int Address;

			/* get address */
			Address = Memdump_GetLineAddress(pMemdumpWindow, pMemdumpWindow->CursorYRelative) + 
				(pMemdumpWindow->CursorXRelative*pMemdumpWindow->NumberOfBytesPerElement);

			/* write data to memory */
			if (pMemdumpWindow->pWriteOperation!=NULL)
				pMemdumpWindow->pWriteOperation(Address, pMemdumpWindow->EditData,pMemdumpWindow->NumberOfBytesPerElement);
		}
		break;
		
		case MEMDUMP_EDIT_LOCATION_TEXT:
		{
			int Address;

			/* get address */
			Address = Memdump_GetLineAddress(pMemdumpWindow, pMemdumpWindow->CursorYRelative) + 
				pMemdumpWindow->CursorXRelative;

			/* write data to memory */
			if (pMemdumpWindow->pWriteOperation!=NULL)
				pMemdumpWindow->pWriteOperation(Address, pMemdumpWindow->EditData,1);
		}
		break;
	}

	pMemdumpWindow->EditOperation = MEMDUMP_EDIT_NONE;
	pMemdumpWindow->CursorXRelativeFraction = 0;
	pMemdumpWindow->EditData = 0;

	/* move cursor to right */
	MemDump_CursorRight(pMemdumpWindow);
}

void	Memdump_UpdateHexEdit(MEMDUMP_WINDOW *pMemdumpWindow, char ch, int NumberOfHexDigits)
{
	if (IsHexDigit(ch))
	{
		/* update edit data */
		pMemdumpWindow->EditData<<=4;
		pMemdumpWindow->EditData|=HexDigitToInt(ch);

		/* update cursor fraction position */
		pMemdumpWindow->CursorXRelativeFraction++;

		pMemdumpWindow->EditOperation=MEMDUMP_EDIT_DATA;

		/* if fraction exceeds number of hex digits for number.. */
		if (pMemdumpWindow->CursorXRelativeFraction>=NumberOfHexDigits)
		{
			Memdump_DoEdit(pMemdumpWindow);
		}
	}
}

void	Memdump_UpdateEdit(MEMDUMP_WINDOW *pMemdumpWindow, char ch)
{
	switch (pMemdumpWindow->EditLocation)
	{
		case MEMDUMP_EDIT_LOCATION_ADDRESS:
		{
			Memdump_UpdateHexEdit(pMemdumpWindow,ch,4);
		}
		break;

		case MEMDUMP_EDIT_LOCATION_DATA:
		{
			Memdump_UpdateHexEdit(pMemdumpWindow,ch,pMemdumpWindow->NumberOfCharsPerElement);
		}
		break;

		case MEMDUMP_EDIT_LOCATION_TEXT:
		{
			/* greater than space, but less than 128 */
			if ((ch & 0x0a0)==0x020)
			{
				pMemdumpWindow->EditOperation=MEMDUMP_EDIT_DATA;
				pMemdumpWindow->EditData = ch;

				Memdump_DoEdit(pMemdumpWindow);
			}
		}
		break;
	}

	Memdump_UpdateCursorAbsolute(pMemdumpWindow);
}

/* call after a resize operation */
void	MemDump_RefreshState(MEMDUMP_WINDOW *pMemdumpWindow)
{
	/* update number of bytes visible on line */
	pMemdumpWindow->NumberOfElementsVisible = Memdump_CalcNumberOfVisibleElements(pMemdumpWindow,pMemdumpWindow->WidthInChars);	

	/* ensure X position is valid */
	if (pMemdumpWindow->CursorXRelative>=pMemdumpWindow->NumberOfElementsVisible)
	{
		pMemdumpWindow->CursorXRelative = pMemdumpWindow->NumberOfElementsVisible-1;
	}

	/* ensure Y position is valid */
	if (pMemdumpWindow->CursorYRelative>=pMemdumpWindow->HeightInChars)
	{
		pMemdumpWindow->CursorYRelative = pMemdumpWindow->HeightInChars-1;
	}

	/* malloc space for text buffer */
	if (pMemdumpWindow->pBuffer!=NULL)
	{
		free(pMemdumpWindow->pBuffer);
		pMemdumpWindow->pBuffer = NULL;
	}

	/* allocate space for text */
	pMemdumpWindow->pBuffer = malloc(pMemdumpWindow->WidthInChars+1);

	Memdump_UpdateCursorAbsolute(pMemdumpWindow);
}


/* move cursor down */
void	MemDump_CursorUp(MEMDUMP_WINDOW *pMemdumpWindow)
{
	Memdump_DoEdit(pMemdumpWindow);

	pMemdumpWindow->CursorYRelative--;

	/* moved off top of Memdump window? */
	if (pMemdumpWindow->CursorYRelative<0)
	{
		/* move up so more bytes are visible */
		pMemdumpWindow->CursorYRelative = 0;
		pMemdumpWindow->BaseAddress -= (pMemdumpWindow->NumberOfElementsVisible*pMemdumpWindow->NumberOfBytesPerElement);
	}

	Memdump_UpdateCursorAbsolute(pMemdumpWindow);
}

/* move cursor down */
void	MemDump_CursorDown(MEMDUMP_WINDOW *pMemdumpWindow)
{
	Memdump_DoEdit(pMemdumpWindow);

	pMemdumpWindow->CursorYRelative++;

	/* moved off bottom of Memdump window? */
	if (pMemdumpWindow->CursorYRelative>=pMemdumpWindow->HeightInChars)
	{
		pMemdumpWindow->CursorYRelative = pMemdumpWindow->HeightInChars-1;
		pMemdumpWindow->BaseAddress += (pMemdumpWindow->NumberOfElementsVisible*pMemdumpWindow->NumberOfBytesPerElement);
	}
	
	Memdump_UpdateCursorAbsolute(pMemdumpWindow);
}

/* move cursor left */
void	MemDump_CursorLeft(MEMDUMP_WINDOW *pMemdumpWindow)
{
	Memdump_DoEdit(pMemdumpWindow);

	pMemdumpWindow->CursorXRelative--;

	switch (pMemdumpWindow->EditLocation)
	{
		case MEMDUMP_EDIT_LOCATION_TEXT:
		case MEMDUMP_EDIT_LOCATION_DATA:
		{
			if (pMemdumpWindow->CursorXRelative<0)
			{
				pMemdumpWindow->CursorXRelative = pMemdumpWindow->NumberOfElementsVisible-1;
				MemDump_CursorUp(pMemdumpWindow);
			}

		}
		break;

		case MEMDUMP_EDIT_LOCATION_ADDRESS:
		{
			pMemdumpWindow->CursorXRelative = 0;
			MemDump_CursorUp(pMemdumpWindow);
		}
		break;

		default:
			break;
	}

	Memdump_UpdateCursorAbsolute(pMemdumpWindow);

}

void	MemDump_CursorRight(MEMDUMP_WINDOW *pMemdumpWindow)
{
	Memdump_DoEdit(pMemdumpWindow);

	pMemdumpWindow->CursorXRelative++;

	switch (pMemdumpWindow->EditLocation)
	{
		case MEMDUMP_EDIT_LOCATION_TEXT:
		case MEMDUMP_EDIT_LOCATION_DATA:
		{
			if (pMemdumpWindow->CursorXRelative>=pMemdumpWindow->NumberOfElementsVisible)
			{
				pMemdumpWindow->CursorXRelative = 0;
				MemDump_CursorDown(pMemdumpWindow);
			}
		}
		break;

		case MEMDUMP_EDIT_LOCATION_ADDRESS:
		{
			pMemdumpWindow->CursorXRelative = 0;
			MemDump_CursorDown(pMemdumpWindow);
		}
		break;
	}

	Memdump_UpdateCursorAbsolute(pMemdumpWindow);
}

void	MemDump_PageUp(MEMDUMP_WINDOW *pMemdumpWindow)
{
	Memdump_DoEdit(pMemdumpWindow);

	pMemdumpWindow->BaseAddress -= (pMemdumpWindow->NumberOfElementsVisible*pMemdumpWindow->NumberOfBytesPerElement*pMemdumpWindow->HeightInChars);
}

void	MemDump_PageDown(MEMDUMP_WINDOW *pMemdumpWindow)
{
	Memdump_DoEdit(pMemdumpWindow);

	pMemdumpWindow->BaseAddress += (pMemdumpWindow->NumberOfElementsVisible*pMemdumpWindow->NumberOfBytesPerElement*pMemdumpWindow->HeightInChars);
}

void	MemDump_SetAddress(MEMDUMP_WINDOW *pMemdumpWindow, int Address)
{
	pMemdumpWindow->BaseAddress = Address;
}

MEMDUMP_WINDOW *MemDump_Create(void)
{
	MEMDUMP_WINDOW *pMemdumpWindow = (MEMDUMP_WINDOW *)malloc(sizeof(MEMDUMP_WINDOW));
	
	if (pMemdumpWindow!=NULL)
	{
		memset(pMemdumpWindow, 0, sizeof(MEMDUMP_WINDOW));
		MemDump_SetupView(pMemdumpWindow);
	}


	return pMemdumpWindow;
}

void	MemDump_Finish(MEMDUMP_WINDOW *pMemdumpWindow)
{
	if (pMemdumpWindow->pBuffer!=NULL)
	{
		free(pMemdumpWindow->pBuffer);
	}

	free(pMemdumpWindow);
}

void	Memdump_ToggleLocation(MEMDUMP_WINDOW *pMemdumpWindow)
{
	pMemdumpWindow->EditLocation++;

	if (pMemdumpWindow->EditLocation>MEMDUMP_EDIT_LOCATION_LAST)
	{
		pMemdumpWindow->EditLocation = MEMDUMP_EDIT_LOCATION_FIRST;
	}

	Memdump_UpdateCursorAbsolute(pMemdumpWindow);
}



void	Memdump_ToggleView(MEMDUMP_WINDOW *pMemdumpWindow)
{
	pMemdumpWindow->ViewType++;

	if (pMemdumpWindow->ViewType>MEMDUMP_VIEW_LAST)
	{
		pMemdumpWindow->ViewType = MEMDUMP_VIEW_FIRST;
	}

	MemDump_SetupView(pMemdumpWindow);
}

void	MemDump_SelectByCharXY(MEMDUMP_WINDOW *pMemdumpWindow, int x, int y)
{
	int TextXCoord;

	Memdump_DoEdit(pMemdumpWindow);
	
	/* set y pos */
	pMemdumpWindow->CursorYRelative = y;

	/* address column? */
	if (x<MEMDUMP_DATA_LEFT_EDGE)
	{
		pMemdumpWindow->CursorXRelative = 0;
		pMemdumpWindow->EditLocation = MEMDUMP_EDIT_LOCATION_ADDRESS;
	}

	/* text column? */
	TextXCoord = (MEMDUMP_DATA_LEFT_EDGE +(pMemdumpWindow->NumberOfElementsVisible*(pMemdumpWindow->NumberOfCharsPerElement+pMemdumpWindow->Spacing))+2);

	if (x>=TextXCoord)
	{
		pMemdumpWindow->EditLocation = MEMDUMP_EDIT_LOCATION_TEXT;

		pMemdumpWindow->CursorXRelative = x - TextXCoord;
		
		if (pMemdumpWindow->CursorXRelative>=pMemdumpWindow->NumberOfElementsVisible)
		{
			pMemdumpWindow->CursorXRelative = pMemdumpWindow->NumberOfElementsVisible-1;
		}

	}

	/* bytes column */
	if ((x>=MEMDUMP_DATA_LEFT_EDGE) && (x<TextXCoord))
	{
		pMemdumpWindow->EditLocation = MEMDUMP_EDIT_LOCATION_DATA;

		pMemdumpWindow->CursorXRelative = (x - MEMDUMP_DATA_LEFT_EDGE)/(pMemdumpWindow->NumberOfCharsPerElement+pMemdumpWindow->Spacing);

		if (pMemdumpWindow->CursorXRelative>=pMemdumpWindow->NumberOfElementsVisible)
		{
			pMemdumpWindow->CursorXRelative = pMemdumpWindow->NumberOfElementsVisible-1;
		}
	}



	Memdump_UpdateCursorAbsolute(pMemdumpWindow);
}

void	Memdump_GotoAddress(MEMDUMP_WINDOW *pMemdumpWindow, int Address)
{
	int StartAddress = pMemdumpWindow->BaseAddress;
	int BytesPerLine = pMemdumpWindow->NumberOfElementsVisible*pMemdumpWindow->NumberOfBytesPerElement;
	int EndAddress = pMemdumpWindow->BaseAddress + 
		(BytesPerLine*pMemdumpWindow->HeightInChars);

	if ((Address<StartAddress) || (Address>EndAddress))
	{
		/* address not visible */
		int NewBaseAddress;
		int ByteOffs;

		ByteOffs = Address - ((Address/BytesPerLine)*BytesPerLine);
		ByteOffs /= pMemdumpWindow->NumberOfBytesPerElement;
		
		StartAddress = NewBaseAddress = Address - ByteOffs;

		pMemdumpWindow->BaseAddress = NewBaseAddress;
		pMemdumpWindow->CursorYRelative = 0;
		pMemdumpWindow->CursorXRelativeFraction = 0;
	}
	
	{
		/* address is visible */

		int XCoord, YCoord;
		int AddressOffset;

		AddressOffset = Address - StartAddress;

		YCoord = AddressOffset/BytesPerLine;
		XCoord = (AddressOffset - (YCoord*BytesPerLine));
		XCoord /= pMemdumpWindow->NumberOfBytesPerElement;
		
		pMemdumpWindow->CursorXRelative = XCoord;
		pMemdumpWindow->CursorYRelative = YCoord;
		pMemdumpWindow->CursorXRelativeFraction = 0;

	}

	Memdump_UpdateCursorAbsolute(pMemdumpWindow);

}


void	Memdump_SetCursorAddress(MEMDUMP_WINDOW *pMemdumpWindow, int NewAddress)
{
	int AddressOffset;

	/* get current address for this line */
	AddressOffset = Memdump_GetLineAddress(pMemdumpWindow, pMemdumpWindow->CursorYRelative);
	/* subtract base address to get offset for this line */
	AddressOffset -= pMemdumpWindow->BaseAddress;

	/* adjust so that address we typed in is now on this line */
	pMemdumpWindow->BaseAddress = NewAddress - AddressOffset;

	MemDump_RefreshState(pMemdumpWindow);
}

