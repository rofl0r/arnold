#include <windows.h>
#include "resource.h"

BOOL CALLBACK  DisplayConfiguration_DialogProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
        
		case WM_INITDIALOG:
		{
#if 0
			HWND hList = GetDlgItem(hwnd, IDC_LIST_JOY);

			if (hList)
			{
				ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
				JoystickConfiguration_SetupListView(hList);

				{
					UINT State;
					State = ListView_GetItemState(hList, joystickConfigs[0].realJoystickData.id, LVIS_SELECTED);
					State|=LVIS_SELECTED;
					ListView_SetItemState(hList, joystickConfigs[0].realJoystickData.id, LVIS_SELECTED,State);
				}
			}	
#endif
		}
		return TRUE;


        case WM_CREATE:
            return TRUE;

			//EndDialog(hwnd,0);

        case WM_COMMAND:
        {
			switch LOWORD(wParam)
	        {
				case IDOK:
				{
#if 0
					HWND hList = GetDlgItem(hwnd, IDC_LIST_JOY);


					int index = ListView_GetNextItem(hList, -1, LVIS_SELECTED);
					joystickConfigs[0].Type = JOYSTICK_REAL;
					joystickConfigs[0].realJoystickData.id = index;
#endif
					EndDialog(hwnd, 0);
					return TRUE;

				//	if (Debug_ValidateNumberIsHex(HexValueText, &Number))
				//	{
				//		EndDialog(hwnd,Number & 0x0ffff);
				//		return TRUE;
				//	}
				}
				break;

				case IDCANCEL:
					{
						EndDialog(hwnd, -1);
					}
					return TRUE;


				default:
					break;

			}
		}
		break;

		case WM_CLOSE:

			break;
		case WM_DESTROY:
			break;

/*    case WM_DESTROY:
		PostQuitMessage (0);
		return TRUE;
*/
	}
    return FALSE;
}

void DisplayConfigurationDialog(HWND hwnd)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_DISPLAY),hwnd,DisplayConfiguration_DialogProc);
}


