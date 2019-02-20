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
#ifndef __WINDOW__
#define __WINDOW__

/* a item that can be selected */
typedef struct 
{
	unsigned long RelativeX;
	unsigned long RelativeY;

	unsigned long AbsoluteX;
	unsigned long AbsoluteY;

	/* width of item that can be selected */
	unsigned long Width;

	struct DEBUG_ITEM	*pNext;

} DEBUG_ITEM;

typedef struct 
{
	unsigned long RelativeX;
	unsigned long RelativeY;

	DEBUG_ITEM *pItem;
} DEBUG_CURSOR;

void	DebugItem__RemoveItem(DEBUG_ITEM *);
void	DebugItem__SetCursorFromXY(int X, int Y);
void	DebugItem__GetCursorPos(int *X, int *Y);
void	DebugItem__AddNewItem(int X, int Y, int Count);
void	DebugItem__Initialise(void);
void	DebugItem__Finish(void);

#endif

