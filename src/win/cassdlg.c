#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "cassdlg.h"
#include "../cpc/cpc.h"
#include "../cpc/messages.h"

HWND hCassetteControls = 0;

/*-----------------------------------------------------------------------------------------------------------------*/

void	CassetteControls_Refresh(HWND hControls)
{
	int i;

	HWND hListView = GetDlgItem(hControls, IDC_LIST_BLOCKS);

	SendMessage(hListView, WM_SETREDRAW,(WPARAM)FALSE, (LPARAM)0);

	/* ensure list-view is empty */
	ListView_DeleteAllItems(hListView);

	for (i=0; i<TapeImage_GetNumBlocks(); i++)
	{
		int BlockID;

		BlockID = TapeImage_GetBlockID(i);

		/* add name to list view */
		MyListView_AddItem(hListView, TapeImage_GetBlockDescription(BlockID), 0, i,0);
	}

	SendMessage(hListView, WM_SETREDRAW,(WPARAM)TRUE, (LPARAM)0);
	InvalidateRect(hListView, NULL, TRUE);
}

/*-----------------------------------------------------------------------------------------------------------------*/

static BOOL CALLBACK  CassetteControlsDialogProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
		case WM_INITDIALOG:
		{
			HWND hListView = GetDlgItem(hwnd, IDC_LIST_BLOCKS);

			/* add the column */
			MyListView_AddColumn(hListView, Messages[25],0);

			/* fill in the list */
			CassetteControls_Refresh(hwnd);
		}
		return TRUE;

		default:
			break;
	}
    return FALSE;
}

/*-----------------------------------------------------------------------------------------------------------------*/

void CassetteControls(HWND hwnd)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

	hCassetteControls = CreateDialog (hInstance, MAKEINTRESOURCE(IDD_DIALOG_CASSETTE), hwnd, CassetteControlsDialogProc);

	if (hCassetteControls)
		ShowWindow(hCassetteControls, SW_SHOW);
}

