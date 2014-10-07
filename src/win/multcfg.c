#include <windows.h>
#include "resource.h"
#include "../cpc/cpc.h"
#include "../cpc/messages.h"
#include "../cpc/multface.h"
/////////////////////////////////////////////////////////////////////////

//const char *ROM_NOT_LOADED=Messages[16];
//const char *ROM_LOADED=Messages[17];

void	MultifaceDialog_Update(HWND hDialog)
{
	const char *text;

	text = Messages[16];
	
	if (Multiface_IsRomLoaded(MULTIFACE_ROM_CPC_VERSION))
	{
		text = Messages[17];
	}

	SetDlgItemText(hDialog,IDC_STATIC_CPCROMSTATUS,text);
	
	text = Messages[16];

	if (Multiface_IsRomLoaded(MULTIFACE_ROM_CPCPLUS_VERSION))
	{
		text = Messages[17];
	}

	SetDlgItemText(hDialog,IDC_STATIC_CPCPLUSROMSTATUS,text);
		
	// is emulation active
	SetCheckButtonState(hDialog, IDC_CHECK_MULTIFACE_HARDWARE_IS_ACTIVE, Multiface_IsEmulationEnabled());
}

BOOL CALLBACK  MultifaceSettingsDialogProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

    switch (iMsg)
    {
        
        case WM_INITDIALOG:
		{
			MultifaceDialog_Update(hwnd);
		}
		break;

        case WM_COMMAND:
        {
			if (HIWORD(wParam)==BN_CLICKED)
			{
				/* check box clicked */

				switch (LOWORD(wParam))
				{
					case IDC_BUTTON_ROM1:
					case IDC_BUTTON_ROM2:
					{
						switch (LOWORD(wParam))
						{
							default:
							case IDC_BUTTON_ROM1:
							{
								Interface_InsertMultifaceROM(hwnd,MULTIFACE_ROM_CPC_VERSION);
							}
							break;

							case IDC_BUTTON_ROM2:
							{
								Interface_InsertMultifaceROM(hwnd,MULTIFACE_ROM_CPCPLUS_VERSION);
							}
							break;

						}

						MultifaceDialog_Update(hwnd);
					}
					return TRUE;

					case IDC_CHECK_MULTIFACE_HARDWARE_IS_ACTIVE:
					{
						/* get check state */
						int CheckState;

						CheckState = SendDlgItemMessage(hwnd,LOWORD(wParam), BM_GETCHECK,0,0);

						/* checked */
						Multiface_EnableEmulation((CheckState == BST_CHECKED));
					}
					break;

				}
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


void	Interface_MultifaceDialog(HWND hwnd)
{
	HWND hDialog;

	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

	hDialog = CreateDialog (hInstance, MAKEINTRESOURCE(IDD_DIALOG_MULTIFACE_SETTINGS), 0, MultifaceSettingsDialogProc);

	if (hDialog!=NULL)
	{
		ShowWindow(hDialog,SW_SHOW);

		MultifaceDialog_Update(hDialog);
	}
}

