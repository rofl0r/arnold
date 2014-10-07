#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "../cpc/cheatdb.h"
#include "mylistvw.h"
#include "..\cpc\messages.h"
#include "chtdb.h"

extern CHEAT_DATABASE *pCheatDatabase;


void	InitialiseListView(HWND hListView)
{
	int RowIndex = 0;
	CHEAT_DATABASE_ENTRY *pCurrentEntry;

	if ((pCheatDatabase!=NULL) && (hListView!=NULL))
	{
		// ensure list-view is empty
		ListView_DeleteAllItems(hListView);

		pCurrentEntry = pCheatDatabase->pFirst;

		while (pCurrentEntry!=NULL)
		{
			char *TypeString;

			/* add name to list view */
			MyListView_AddItem(hListView, pCurrentEntry->Name, 0, RowIndex,pCurrentEntry);
			/* add size text to list view */
			MyListView_AddItem(hListView, pCurrentEntry->Description, 1, RowIndex,NULL);
			/* add name to list view */

			switch (toupper(pCurrentEntry->Type))
			{
				case 'T':
				{
					TypeString = Messages[20];
				}
				break;

				case 'D':
				{
					TypeString = Messages[21];
				}
				break;

				default:
				case 'A':
				{
					TypeString = Messages[22];
				}
				break;

				case 'C':
				{
					TypeString = Messages[23];
				}
				break;
			}

			MyListView_AddItem(hListView, TypeString, 2, RowIndex,NULL);

			RowIndex++;
			pCurrentEntry = pCurrentEntry->pNext;
		}
	}
}


BOOL	CALLBACK CheatDatabase_Dialog_Proc(HWND hDialog, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
		case WM_INITDIALOG:
		{
			HWND hListView;

			hListView = GetDlgItem(hDialog,IDC_LIST_CHEATS);
		
			if (hListView!=0)
			{
				MyListView_AddColumn(hListView, Messages[25],0);
				MyListView_AddColumn(hListView, Messages[26],1);
				MyListView_AddColumn(hListView, Messages[27],2);
			}

			InitialiseListView(hListView);
		}
		break;

		case WM_NOTIFY:
		{
			NMHDR FAR *pnmh = (NMHDR FAR *)lParam;


		}
		break;


		case WM_COMMAND:
		{
			// depending on ID selected
			switch (LOWORD(wParam))
			{
				case ID_OPEN_DATABASE:
				{
					Interface_OpenCheatDatabase();
				}
				break;

				case ID_POKE:
				{
					int i;
					int Count;
			HWND hListView;

			hListView = GetDlgItem(hDialog,IDC_LIST_CHEATS);

					Count = ListView_GetItemCount(hListView);

					for (i=0; i<Count; i++)
					{
						if (ListView_GetItemState(hListView, i, LVIS_SELECTED)==LVIS_SELECTED)
						{
							CHEAT_DATABASE_ENTRY *pEntry;
							LV_ITEM Item;

							Item.mask = LVIF_PARAM;
							Item.iItem = i;
							Item.iSubItem = 0;
							ListView_GetItem(hListView, &Item);

							pEntry = (CHEAT_DATABASE_ENTRY *)Item.lParam;

							if (pEntry!=NULL)
							{
								CheatDatabase_Poke(pEntry);
							}
						}

					}
				}
				break;
			}
		}
		break;

		case WM_CLOSE:
			DestroyWindow(hDialog);
			break;


		case WM_DESTROY:
		{
			HWND hListView;

			hListView = GetDlgItem(hDialog,IDC_LIST_CHEATS);

			if (hListView!=0)
			{
				ListView_DeleteAllItems(hListView);
				hListView = 0;
			}
		}
		break;

	}

	return FALSE;
}

void	CheatDatabaseDialog(HWND hParent)
{
	HWND hDialog;

	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE);

	hDialog = CreateDialog (hInstance, MAKEINTRESOURCE(IDD_DIALOG_CHEAT_DATABASE), 0, CheatDatabase_Dialog_Proc);

	ShowWindow(hDialog,TRUE);
}

