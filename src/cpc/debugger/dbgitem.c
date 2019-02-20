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
#include <stdlib.h>
/*#include <memory.h> */
#include "../cpcglob.h"
#include "item.h"

static DEBUG_ITEM	*pFirstItem = NULL;
static DEBUG_CURSOR	Cursor;



void	DebugItem__Initialise(void)
{
	pFirstItem = NULL;
}


void	DebugItem__Finish(void)
{
	while (pFirstItem!=NULL)
	{
		DebugItem__RemoveItem(pFirstItem);
	}
}


DEBUG_ITEM *DebugItem__NewItem(void)
{
	DEBUG_ITEM *pNewItem;

	pNewItem = (DEBUG_ITEM *)malloc(sizeof(DEBUG_ITEM));

	if (pNewItem!=NULL)
	{
		memset(pNewItem, 0, sizeof(DEBUG_ITEM));
	}

	return pNewItem;
}

void	DebugItem__AddItemToList(DEBUG_ITEM *pItem)
{
	if (pFirstItem == NULL)
	{
		pFirstItem = pItem;
	}
	else
	{
		DEBUG_ITEM *pLastItem;

		pLastItem = pFirstItem;
	
		while (pLastItem->pNext!=NULL)
		{
			pLastItem = (DEBUG_ITEM *)pLastItem->pNext;
		}

		pLastItem->pNext = (struct DEBUG_ITEM *)pItem;
	}
}

void	DebugItem__RemoveItem(DEBUG_ITEM *pItem)
{
	DEBUG_ITEM *pCurrentItem;
	DEBUG_ITEM *pPreviousItem;
	DEBUG_ITEM *pNextItem;

	pCurrentItem = pFirstItem;
	pPreviousItem = NULL;

	do
	{
		pNextItem = (DEBUG_ITEM *)pCurrentItem->pNext;

		if (pCurrentItem == pItem)
		{
			if (pPreviousItem!=NULL)
			{
				pPreviousItem->pNext = (struct DEBUG_ITEM *)pNextItem;
			}
			else
			{
				pFirstItem = pNextItem;
			}

			free(pCurrentItem);
		}

		pPreviousItem = pCurrentItem;
		pCurrentItem = pNextItem;

	}
	while (pCurrentItem!=NULL);
}



/* add a new item to the list */
void	DebugItem__AddNewItem(int RelativeX, int RelativeY, int Width)
{
	DEBUG_ITEM *pNewItem;

	pNewItem = DebugItem__NewItem();
	
	if (pNewItem!=NULL)
	{
		pNewItem->RelativeX = RelativeX;
		pNewItem->RelativeY = RelativeY;
		pNewItem->Width = Width;
	}

	DebugItem__AddItemToList(pNewItem);
}







/* select a item based on x,y coordinates */
DEBUG_ITEM *DebugItem__SelectItemByXY(int X, int Y)
{
	DEBUG_ITEM *pCurrentItem;

	pCurrentItem = pFirstItem;

	while (pCurrentItem!=NULL)
	{
		if ((X>=pCurrentItem->RelativeX) && (X<=(pCurrentItem->RelativeX+pCurrentItem->Width)))
		{
			if (Y == pCurrentItem->RelativeY)
			{
				return pCurrentItem;
			}
		}

		pCurrentItem = (DEBUG_ITEM *)pCurrentItem->pNext;
	}

	return NULL;
}

void	DebugItem__SetCursorFromXY(int X, int Y)
{
	DEBUG_ITEM *pDebugItem;

	pDebugItem = DebugItem__SelectItemByXY(X, Y);

	if (pDebugItem!=NULL)
	{
		Cursor.RelativeX = pDebugItem->RelativeX;
		Cursor.RelativeY = pDebugItem->RelativeY;
		Cursor.pItem = pDebugItem;
	}
}


void	DebugItem__GetCursorPos(int *X, int *Y)
{
	*X = Cursor.RelativeX;
	*Y = Cursor.RelativeY;
}
