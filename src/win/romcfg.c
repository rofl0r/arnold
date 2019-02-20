#include <windows.h>
#include "resource.h"
#include "romcfg.h"
#include <tchar.h>
#include "../cpc/cpc.h"
#include "cpcemu.h"

/////////////////////////////////////////////////////////////////////////

const int		Rom_UpdateArray[]=
{
	IDC_STATIC_ROM1,
	IDC_CHECK_ROM1,
	IDC_STATIC_ROM2,
	IDC_CHECK_ROM2,
	IDC_STATIC_ROM3,
	IDC_CHECK_ROM3,
	IDC_STATIC_ROM4,
	IDC_CHECK_ROM4,
	IDC_STATIC_ROM5,
	IDC_CHECK_ROM5,
	IDC_STATIC_ROM6,
	IDC_CHECK_ROM6,
	IDC_STATIC_ROM7,
	IDC_CHECK_ROM7,
	IDC_STATIC_ROM8,
	IDC_CHECK_ROM8,
	IDC_STATIC_ROM9,
	IDC_CHECK_ROM9,
	IDC_STATIC_ROM10,
	IDC_CHECK_ROM10,
	IDC_STATIC_ROM11,
	IDC_CHECK_ROM11,
	IDC_STATIC_ROM12,
	IDC_CHECK_ROM12,
	IDC_STATIC_ROM13,
	IDC_CHECK_ROM13,
	IDC_STATIC_ROM14,
	IDC_CHECK_ROM14,
	IDC_STATIC_ROM15,
	IDC_CHECK_ROM15,
	IDC_STATIC_ROM16,
	IDC_CHECK_ROM16,
};

const int NUM_ROMS = (sizeof(Rom_UpdateArray)/(sizeof(int)*2));

static void	RomDialog_Update(HWND hRomDialog)
{
	const int *pArray = Rom_UpdateArray;
	int i;

	for (i=0; i<NUM_ROMS; i++)
	{
		char *pName;

		if (ExpansionRom_GetRomName(i+1, &pName))
		{
#ifdef _UNICODE
			TCHAR *sUnicodeName = ConvertMultiByteToUnicode(pName);

			if (sUnicodeName)
			{
				SetDlgItemText(hRomDialog,pArray[0],sUnicodeName);
			
				free(sUnicodeName);
			}
#else
			SetDlgItemText(hRomDialog,pArray[0],pName);
#endif
			
			free(pName);
		}
		else
		{
			SetDlgItemText(hRomDialog,pArray[0],_T("EMPTY SLOT"));
		}
		pArray++;
		SetCheckButtonState(hRomDialog, pArray[0], ExpansionRom_IsActive(i+1));
		pArray++;
	}
}

static void	DoRomCheck(HWND hwnd, WPARAM wParam)
{
	int RomIndex;
	int CheckState;

	switch (LOWORD(wParam))
	{
	default:
		case IDC_CHECK_ROM1:
		{
			RomIndex = 1;
		}
		break;

		case IDC_CHECK_ROM2:
		{
			RomIndex = 2;
		}
		break;

		case IDC_CHECK_ROM3:
		{
			RomIndex = 3;
		}
		break;

		case IDC_CHECK_ROM4:
		{
			RomIndex = 4;
		}
		break;

		case IDC_CHECK_ROM5:
		{
			RomIndex = 5;
		}
		break;

		case IDC_CHECK_ROM6:
		{
			RomIndex = 6;
		}
		break;

		case IDC_CHECK_ROM7:
		{
			RomIndex = 7;
		}
		break;

		case IDC_CHECK_ROM8:
		{
			RomIndex = 8;
		}
		break;

		case IDC_CHECK_ROM9:
		{
			RomIndex = 9;
		}
		break;

		case IDC_CHECK_ROM10:
		{
			RomIndex = 10;
		}
		break;

		case IDC_CHECK_ROM11:
		{
			RomIndex = 11;
		}
		break;

		case IDC_CHECK_ROM12:
		{
			RomIndex = 12;
		}
		break;

		case IDC_CHECK_ROM13:
		{
			RomIndex = 13;
		}
		break;

		case IDC_CHECK_ROM14:
		{
			RomIndex = 14;
		}
		break;

		case IDC_CHECK_ROM15:
		{
			RomIndex = 15;
		}
		break;

		case IDC_CHECK_ROM16:
		{
			RomIndex = 16;
		}
		break;

	}

	/* get check state */
	CheckState = SendDlgItemMessage(hwnd,LOWORD(wParam), BM_GETCHECK,0,0);

	/* checked */
	ExpansionRom_SetActiveState(RomIndex,(CheckState==BST_CHECKED));
}

BOOL CALLBACK  RomDialogProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

    switch (iMsg)
    {
        
        case WM_CREATE:
            return TRUE;

        case WM_COMMAND:
        {
			if (HIWORD(wParam)==BN_CLICKED)
			{
		
				switch (LOWORD(wParam))
				{
					/* check box clicked */
					case IDC_CHECK_ROM1:
					case IDC_CHECK_ROM2:
					case IDC_CHECK_ROM3:
					case IDC_CHECK_ROM4:
					case IDC_CHECK_ROM5:
					case IDC_CHECK_ROM6:
					case IDC_CHECK_ROM7:
					case IDC_CHECK_ROM8:
					case IDC_CHECK_ROM9:
					case IDC_CHECK_ROM10:
					case IDC_CHECK_ROM11:
					case IDC_CHECK_ROM12:
					case IDC_CHECK_ROM13:
					case IDC_CHECK_ROM14:
					case IDC_CHECK_ROM15:
					case IDC_CHECK_ROM16:
					{
						DoRomCheck(hwnd, wParam);
					}
					break;

					/* insert rom button clicked */
					case IDC_BUTTON_ROM1:
					case IDC_BUTTON_ROM2:
					case IDC_BUTTON_ROM3:
					case IDC_BUTTON_ROM4:
					case IDC_BUTTON_ROM5:
					case IDC_BUTTON_ROM6:
					case IDC_BUTTON_ROM7:
					case IDC_BUTTON_ROM8:
					case IDC_BUTTON_ROM9:
					case IDC_BUTTON_ROM10:
					case IDC_BUTTON_ROM11:
					case IDC_BUTTON_ROM12:
					case IDC_BUTTON_ROM13:
					case IDC_BUTTON_ROM14:
					case IDC_BUTTON_ROM15:
					case IDC_BUTTON_ROM16:
					{
						int RomIndex;

						switch (LOWORD(wParam))
						{
							default:
							case IDC_BUTTON_ROM1:
							{
								RomIndex = 1;
							}
							break;

							case IDC_BUTTON_ROM2:
							{
								RomIndex = 2;
							}
							break;

							case IDC_BUTTON_ROM3:
							{
								RomIndex = 3;
							}
							break;

							case IDC_BUTTON_ROM4:
							{
								RomIndex = 4;
							}
							break;

							case IDC_BUTTON_ROM5:
							{
								RomIndex = 5;
							}
							break;

							case IDC_BUTTON_ROM6:
							{
								RomIndex = 6;
							}
							break;

							case IDC_BUTTON_ROM7:
							{
								RomIndex = 7;
							}
							break;

							case IDC_BUTTON_ROM8:
							{
								RomIndex = 8;
							}
							break;

							case IDC_BUTTON_ROM9:
							{
								RomIndex = 9;
							}
							break;

							case IDC_BUTTON_ROM10:
							{
								RomIndex = 10;
							}
							break;

							case IDC_BUTTON_ROM11:
							{
								RomIndex = 11;
							}
							break;

							case IDC_BUTTON_ROM12:
							{
								RomIndex = 12;
							}
							break;

							case IDC_BUTTON_ROM13:
							{
								RomIndex = 13;
							}
							break;

							case IDC_BUTTON_ROM14:
							{
								RomIndex = 14;
							}
							break;

							case IDC_BUTTON_ROM15:
							{
								RomIndex = 15;
							}
							break;

							case IDC_BUTTON_ROM16:
							{
								RomIndex = 16;
							}
							break;
						}

						Interface_InsertRom(RomIndex);
						RomDialog_Update(hwnd);
					}
					break;
				}

				return TRUE;
			}
		}
		break;
			case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			break;
	}
    return FALSE;
}

void	Interface_RomDialog(HWND hwnd)
{
	HWND hRomDialog;
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

	hRomDialog = CreateDialog (hInstance, MAKEINTRESOURCE(IDD_DIALOG_EXPANSIONROMS), 0, RomDialogProc);

	if (hRomDialog!=0)
	{
		ShowWindow(hRomDialog,SW_SHOW);
	
		RomDialog_Update(hRomDialog);
	}
}
