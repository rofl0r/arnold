#include <windows.h>
#include "resource.h"

/*********************************************************************/

BOOL	CALLBACK YM5_Dialog_Proc(HWND hDialog, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
		case WM_INITDIALOG:
		{
			char *Text;

			Text = YMOutput_GetName();

			if (Text!=NULL)
			{
				SetDlgItemText(hDialog, IDC_EDIT_NAME, Text);
			}
			
			Text = YMOutput_GetAuthor();

			if (Text!=NULL)
			{
				SetDlgItemText(hDialog, IDC_EDIT_AUTHOR, Text);
			}
			
			Text = YMOutput_GetComment();

			if (Text!=NULL)
			{
				SetDlgItemText(hDialog, IDC_EDIT_COMMENT, Text);
			}
		}
		break;

		case WM_COMMAND:
		{
			// depending on ID selected
			switch (LOWORD(wParam))
			{
				case IDOK:
				{
					char	DialogItemTextBuffer[256];

					/* ok selected */

					/* set name */
					GetDlgItemText(hDialog, IDC_EDIT_NAME, DialogItemTextBuffer, 256);
					YMOutput_SetName(DialogItemTextBuffer);

					/* set author */
					GetDlgItemText(hDialog, IDC_EDIT_AUTHOR, DialogItemTextBuffer, 256);
					YMOutput_SetAuthor(DialogItemTextBuffer);

					/* set comment */
					GetDlgItemText(hDialog, IDC_EDIT_COMMENT, DialogItemTextBuffer, 256);
					YMOutput_SetComment(DialogItemTextBuffer);

					EndDialog(hDialog, 0);
				}
				break;

				case IDCANCEL:
				{
					/* cancel selected */
					EndDialog(hDialog, -1);
				}
				break;
			}
		}
		break;

		case WM_CLOSE:
			DestroyWindow(hDialog);
			break;

	}

	return FALSE;
}

void	YM5_PropertiesDialog(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE);

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_YM5), hParent, YM5_Dialog_Proc);
}
