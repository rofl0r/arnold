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
#ifndef __MEMDUMP_HEADER_INCLUDED__
#define __MEMDUMP_HEADER_INCLUDED__

typedef enum
{
	/* not editing anything yet */
	MEMDUMP_EDIT_NONE = 0,
	/* editing data */
	MEMDUMP_EDIT_DATA
} MEMDUMP_EDIT_OPERATION;

typedef enum
{
	/* cursor in address area */
	MEMDUMP_EDIT_LOCATION_ADDRESS = 0,
	/* cursor in hex digits area */
	MEMDUMP_EDIT_LOCATION_DATA,
	/* cursor in text area */
	MEMDUMP_EDIT_LOCATION_TEXT
} MEMDUMP_CURSOR_LOCATION;

#define MEMDUMP_EDIT_LOCATION_FIRST  MEMDUMP_EDIT_LOCATION_ADDRESS
#define MEMDUMP_EDIT_LOCATION_LAST	 MEMDUMP_EDIT_LOCATION_TEXT

typedef enum
{
	MEMDUMP_VIEW_BYTES = 0,
	MEMDUMP_VIEW_WORDS
} MEMDUMP_VIEW_TYPE;

#define MEMDUMP_VIEW_FIRST		MEMDUMP_VIEW_BYTES
#define MEMDUMP_VIEW_LAST		MEMDUMP_VIEW_WORDS

typedef struct MEMDUMP_WINDOW
{
	/* dimensions of window in chars */
	int WidthInChars;
	int HeightInChars;
	/* edit operation type */
	MEMDUMP_EDIT_OPERATION	EditOperation;
	/* edit location */
	MEMDUMP_CURSOR_LOCATION	EditLocation;
	/* view type */
	MEMDUMP_VIEW_TYPE		ViewType;
	/* first address visible */
	int	BaseAddress;
	/* number of bytes per line - dictates x and y position */
/*	int NumberOfBytesPerLine; */
	/* cursor X and Y */
	int CursorXRelative;
	int CursorXRelativeFraction;
	int CursorYRelative;
	/* X and Y absolute position within window */
	int CursorXAbsolute;
	int CursorYAbsolute;

	int NumberOfElementsVisible;
	int NumberOfBytesPerElement;
	int NumberOfCharsPerElement;
	int Spacing;
	unsigned char *pBuffer;

	int EditData;

	unsigned char (*pReadOperation)(int,int);
	void (*pWriteOperation)(int,int,int);
} MEMDUMP_WINDOW;

char *Memdump_DumpLine(MEMDUMP_WINDOW *pMemdumpWindow, int Line);
int Memdump_GetLineAddress(MEMDUMP_WINDOW *pMemdumpWindow, int Line);
void	MemDump_CursorRight(MEMDUMP_WINDOW *pMemdumpWindow);
void	MemDump_CursorLeft(MEMDUMP_WINDOW *pMemdumpWindow);
void	MemDump_CursorDown(MEMDUMP_WINDOW *pMemdumpWindow);
void	MemDump_CursorUp(MEMDUMP_WINDOW *pMemdumpWindow);
void	MemDump_RefreshState(MEMDUMP_WINDOW *pMemdumpWindow);
void	MemDump_SetAddress(MEMDUMP_WINDOW *pMemdumpWindow, int);
MEMDUMP_WINDOW *MemDump_Create(void);
void	MemDump_Finish(MEMDUMP_WINDOW *);
void	Memdump_UpdateEdit(MEMDUMP_WINDOW *pMemdumpWindow, char ch);
void	MemDump_PageUp(MEMDUMP_WINDOW *);
void	MemDump_PageDown(MEMDUMP_WINDOW *);
void	MemDump_SelectByCharXY(MEMDUMP_WINDOW *pMemdump, int X, int Y);
void	Memdump_ToggleLocation(MEMDUMP_WINDOW *pMemdumpWindow);
void	Memdump_SetCursorAddress(MEMDUMP_WINDOW *pMemdumpWindow, int NewAddress);
void	Memdump_GotoAddress(MEMDUMP_WINDOW *pMemdumpWindow, int Address);
void	Memdump_ToggleView(MEMDUMP_WINDOW *pMemdumpWindow);


#endif
