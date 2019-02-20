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
/* code to handle tree-views */
#include "treev.h"
/*
void	TreeViewControl_Empty(HWND hTreeView)
{
	TreeView_DeleteAllItems(hTreeView);
}

void	TreeViewControl_AddChildToParent(HWND hTreeView)
{
	TV_ITEM TV_Item;

	TV_Item.mask = TV_ITEXT;
	TV_Item.hItem
	TV_Item.state
	TV_Item.stateMask
	TV_Item.pszText = 
	TV_Item.cchTextMax =
	TV_Item.iImage =
	TV_Item.iSelectedImage =
	TV_Item.cChildren =
	TV_Item.lParam =

	TreeView_SetItem(hTreeView, &TVItem);
}

HWND TreeViewControl_AddChildToParent(HWND hTreeView, HTREEITEM hParent)
{
	TV_INSERTSTRUCT TV_Insert;
	TV_GETITEM	TV_Get;

	if (hParent!=0)
	{
		TV_Insert.hParent = hParent;
	}
	else
	{
		TV_Insert.hParent = TVI_ROOT;
	}

	TV_Insert.hInsertAfter = TVI_LAST;

	TV_Insert.item.mask = TVIF_TEXT;

	TreeView_InsertItem(hTreeView, &TV_Insert);

	TreeView_GetItem(hTreeView, &TV_Get);
}

*/