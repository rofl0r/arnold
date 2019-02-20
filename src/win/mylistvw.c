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
#include <windows.h>
#include <commctrl.h>
#include "mylistvw.h"


void	MyListView_AddItem(HWND hListView, char *ItemText, int ColumnIndex, int RowIndex, void *pData) 
{
	LV_ITEM	ListViewItem;

	ListViewItem.mask = LVIF_TEXT;
	ListViewItem.iItem = RowIndex;
	ListViewItem.iSubItem = ColumnIndex;
	ListViewItem.state = 0;
	ListViewItem.stateMask = 0; 


	ListViewItem.pszText = ItemText;
	
	ListViewItem.cchTextMax = 0;
	ListViewItem.iImage = 0;
	ListViewItem.lParam = 0;
	pData;

	if (ColumnIndex == 0)
	{
		ListViewItem.mask |= LVIF_PARAM;
		ListViewItem.lParam = (LPARAM)pData;

		ListView_InsertItem(hListView, &ListViewItem);

	}

	ListView_SetItem(hListView, &ListViewItem);
}


/* add a column to a list view control */
void	MyListView_AddColumn(HWND hListView, char *ColumnTitle, int ColumnIndex)
{
	LV_COLUMN	ListViewColumn;

	int StringWidth = ListView_GetStringWidth(hListView, ColumnTitle);

	ListViewColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	ListViewColumn.cx = StringWidth + (GetSystemMetrics(SM_CXEDGE)<<1);
	ListViewColumn.pszText = ColumnTitle;
	ListViewColumn.cchTextMax = 0;
	ListViewColumn.iSubItem = 0;

	ListView_InsertColumn(hListView, ColumnIndex, &ListViewColumn);
}


#if 0
int	ListView_HitTest(HWND hListView,POINT *pPoint)
{
	int				ItemIndex;
	LV_HITTESTINFO	ListViewHitTestInfo;

	ListViewHitTestInfo.pt.x = pPoint->x;		/* the point to hit test against */
	ListViewHitTestInfo.pt.y = pPoint->y;
	ListViewHitTestInfo.flags = 0;				/* receives results of hit-test */
	ListViewHitTestInfo.nItem = 0;				/* receives index of item that was hit */

	/* get item at point specified */
	ItemIndex = ListView_HitTest(hListView,&ListViewHitTestInfo);
	
	if (ItemIndex!=-1)
	{
		/* got an item */

	}
}
#endif
