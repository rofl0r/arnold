#include <windows.h>
#include "resource.h"

/*-----------------------------------------*/

const int		ROMRAM_UpdateArray[]=
{
	IDC_CHECK_BANK0,
	IDC_CHECK_BANK1,
	IDC_CHECK_BANK2,
	IDC_CHECK_BANK3,
	IDC_CHECK_BANK4,
	IDC_CHECK_BANK5,
	IDC_CHECK_BANK6,
	IDC_CHECK_BANK7,
	IDC_CHECK_BANK8,
	IDC_CHECK_BANK9,
	IDC_CHECK_BANK10,
	IDC_CHECK_BANK11,
	IDC_CHECK_BANK12,
	IDC_CHECK_BANK13,
	IDC_CHECK_BANK14,
	IDC_CHECK_BANK15,
};

const int RAMROM_ArraySize= (sizeof(ROMRAM_UpdateArray)/sizeof(int));

void	RAMROM_Dialog_Update(HWND hRAMROMDialog)
{
	int i;
	const int *pArray = ROMRAM_UpdateArray;

	for (i=0; i<RAMROM_ArraySize; i++)
	{
		SetCheckButtonState(hRAMROMDialog,pArray[0],RAM_ROM_GetBankEnableState(i));

		pArray++;
	}

	SetCheckButtonState(hRAMROMDialog, IDC_CHECK_RAM_ON, RAM_ROM_IsRamOn());
	SetCheckButtonState(hRAMROMDialog, IDC_CHECK_RAM_WRITE_ENABLE, RAM_ROM_IsRamWriteEnabled());
	SetCheckButtonState(hRAMROMDialog, IDC_CHECK_EPROM_ON, RAM_ROM_IsEPROMOn());


}

void	RAMROM_DoCheck(HWND hwnd, WPARAM wParam)
{
	/* get check state */
	int CheckState;
	int BankIndex;

	switch (LOWORD(wParam))
	{
		default:
		case IDC_CHECK_BANK0:
		{
			BankIndex = 0;
		}
		break;

		case IDC_CHECK_BANK1:
		{
			BankIndex = 1;
		}
		break;

		case IDC_CHECK_BANK2:
		{
			BankIndex = 2;
		}
		break;
		
		case IDC_CHECK_BANK3:
		{
			BankIndex = 3;
		}
		break;

		case IDC_CHECK_BANK4:
		{
			BankIndex = 4;
		}
		break;

		case IDC_CHECK_BANK5:
		{
			BankIndex = 5;
		}
		break;

		case IDC_CHECK_BANK6:
		{
			BankIndex = 6;
		}
		break;

		case IDC_CHECK_BANK7:
		{
			BankIndex = 7;
		}
		break;

		case IDC_CHECK_BANK8:
		{
			BankIndex = 8;
		}
		break;

		case IDC_CHECK_BANK9:
		{
			BankIndex = 9;
		}
		break;

		case IDC_CHECK_BANK10:
		{
			BankIndex = 10;
		}
		break;

		case IDC_CHECK_BANK11:
		{
			BankIndex = 11;
		}
		break;

		case IDC_CHECK_BANK12:
		{
			BankIndex = 12;
		}
		break;
		
		case IDC_CHECK_BANK13:
		{
			BankIndex = 13;
		}
		break;

		case IDC_CHECK_BANK14:
		{
			BankIndex = 14;
		}
		break;

		case IDC_CHECK_BANK15:
		{
			BankIndex = 15;
		}
		break;
	}

	CheckState = SendDlgItemMessage(hwnd,LOWORD(wParam), BM_GETCHECK,0,0);

	RAM_ROM_SetBankEnable(BankIndex, (CheckState == BST_CHECKED));
}


BOOL CALLBACK  RAMROM_DialogProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
					case IDC_CHECK_BANK0:
					case IDC_CHECK_BANK1:
					case IDC_CHECK_BANK2:
					case IDC_CHECK_BANK3:
					case IDC_CHECK_BANK4:
					case IDC_CHECK_BANK5:
					case IDC_CHECK_BANK6:
					case IDC_CHECK_BANK7:
					case IDC_CHECK_BANK8:
					case IDC_CHECK_BANK9:
					case IDC_CHECK_BANK10:
					case IDC_CHECK_BANK11:
					case IDC_CHECK_BANK12:
					case IDC_CHECK_BANK13:
					case IDC_CHECK_BANK14:
					case IDC_CHECK_BANK15:
					{
						RAMROM_DoCheck(hwnd, wParam);
					}
					break;

					case IDC_CHECK_RAM_ON:
					case IDC_CHECK_RAM_WRITE_ENABLE:
					case IDC_CHECK_EPROM_ON:
					{
						BOOL	CheckState;

						/* get check state */
						CheckState = (SendDlgItemMessage(hwnd,LOWORD(wParam), BM_GETCHECK,0,0)==BST_CHECKED);

						switch (LOWORD(wParam))
						{
							default:
								break;

							case IDC_CHECK_RAM_ON:
							{
								RAM_ROM_SetRamOnState(CheckState);
								break;
							}

							case IDC_CHECK_RAM_WRITE_ENABLE:
							{
								RAM_ROM_SetRamWriteEnableState(CheckState);
								break;
							}

							case IDC_CHECK_EPROM_ON:
							{
								RAM_ROM_SetEPROMOnState(CheckState);
								break;
							}
						}
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

void	Interface_RAMROM_Dialog(HWND hwnd)
{
	HWND hRAMROMDialog;
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

	hRAMROMDialog = CreateDialog (hInstance, MAKEINTRESOURCE(IDD_DIALOG_RAM_ROM), 0, RAMROM_DialogProc);

	if (hRAMROMDialog!=0)
	{
		ShowWindow(hRAMROMDialog,SW_SHOW);
	
		RAMROM_Dialog_Update(hRAMROMDialog);
	}
}



