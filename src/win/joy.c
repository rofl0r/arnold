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
/* Joystick functions */


//#define WIN32_LEAN_AND_MEAN
//#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include "joy.h"
#include "resource.h"
#include "cpcemu.h"
#include "keystack.h"
#include <tchar.h>

typedef struct
{
	/* current (after processing)*/
	int cur;
	/* minimum value */
	int min;
	/* maximum value */
	int max;
	/* dead range */
	int dead[2];
} JoystickAnalogueElement;

typedef struct
{
	int id;
	JoystickAnalogueElement x;
	JoystickAnalogueElement y;
	int buttons;
} JoystickInternalData;

#define MAX_JOYSTICKS 32
static JoystickInternalData joysticks[MAX_JOYSTICKS];

//static BOOL			JoystickEnabled;

/* number of joysticks supported by device */
//static int			NoOfJoysticks;

/* joystick capabilities */
//static JOYCAPS		JoystickCaps;

/* id of joystick to read */
//static int joystickID = JOYSTICKID1;

//static int	XPos, YPos, Buttons;
//static int XDead,YDead,XMid,YMid;

DWORD HandleDeadZone(JoystickAnalogueElement *element, DWORD current)
{
	DWORD newVal;

	if (current<element->dead[0])
	{
		DWORD range;
		/* outside deadzone */
		range = (element->dead[0] - element->min);

		newVal = current-element->dead[0];
	}
	else if (current>element->dead[1])
	{
		/* outside deadzone */
		DWORD range;

		range = (element->max - element->dead[1]);

		newVal = current-element->dead[1];
	}
	else
	{
		newVal = 0;
	}

	element->cur = newVal;
}

/* keypath to get to OEM destination (WinNT/Win98) */
const TCHAR *KeyPath[]=
{
	_T("System"),
	_T("CurrentControlSet"),
	_T("Control"),
	_T("MediaProperties"),
	_T("PrivateProperties"),
	_T("Joystick"),
	_T("OEM"),
	NULL
};

/* keypath to get to joystick key (Win98) */
const TCHAR *KeyPathWin9x[]=
{
	_T("System"),
	_T("CurrentControlSet"),
	_T("Control"),
	_T("MediaResources"),
	_T("joystick"),
	NULL
};

BOOL	KeyStack_ExecutePath(KeyStack *pStack, HKEY RootKey, const TCHAR **pPath)
{
	HKEY CurrentKey;
	BOOL bSuccess = TRUE;
	HKEY ParentKey = RootKey;
	char **pPathPtr = pPath;

	/* attempt to open keys */
	while ((*pPathPtr)!=NULL)
	{
		/* attempt to open this key for reading */
		if (RegOpenKeyEx(ParentKey,(*pPathPtr), 0, KEY_READ, &CurrentKey)!=ERROR_SUCCESS)
		{
			/* failed */
			bSuccess = FALSE;
			break;
		}

		KeyStack_Push(pStack,CurrentKey);

		ParentKey = CurrentKey;
		pPathPtr++;
	}

	return bSuccess;
}

/******************************************************************************************/

BOOL	GetStringValue(HKEY Key, TCHAR *pValueName, TCHAR **ppValueData)
{
	DWORD BufferSize;
	DWORD Type;
	
	/* get size of data */
	if (RegQueryValueEx(Key, pValueName, 0, &Type, NULL, &BufferSize)==ERROR_SUCCESS)
	{
		/* string? */
		if ((Type==REG_SZ) && (BufferSize!=0))
		{
			char *pBuffer;

			/* allocate a buffer */
			pBuffer = (char *)malloc(BufferSize*sizeof(TCHAR));

			/* get value */
			if (RegQueryValueEx(Key, pValueName, 0, &Type, pBuffer, &BufferSize)==ERROR_SUCCESS)
			{
				*ppValueData= pBuffer;
				return TRUE;
			}
		}
	}
	
	return FALSE;
}
/******************************************************************************************/

BOOL	GetJoystickName(int id,JOYCAPS *pJoystickCaps, char **ppName)
{
	BOOL bSuccess;
	KeyStack keyStack;

	KeyStack_Init(&keyStack);

	/* windows 98: manufacturer id and product id are returned for joystick driver and not joystick! */
	if ((pJoystickCaps->wMid == MM_MICROSOFT) && (pJoystickCaps->wPid == MM_PC_JOYSTICK))
	{
		if (KeyStack_ExecutePath(&keyStack, HKEY_LOCAL_MACHINE, KeyPathWin9x))
		{				
			TCHAR *SubKeyPath[3];

			SubKeyPath[0] = pJoystickCaps->szRegKey;
			SubKeyPath[1] = _T("CurrentJoystickSettings");
			SubKeyPath[2] = NULL;

			if (KeyStack_ExecutePath(&keyStack, KeyStack_GetTopItem(&keyStack), SubKeyPath))
			{
				TCHAR *pOEMName;
				TCHAR ValueName[32];
			
				_stprintf(ValueName,_T("Joystick%dOEMName"),id+1);

				if (GetStringValue(KeyStack_GetTopItem(&keyStack), ValueName, &pOEMName))
				{
					KeyStack_Close(&keyStack);

					/* got oem name */
					if (KeyStack_ExecutePath(&keyStack, HKEY_LOCAL_MACHINE, KeyPath))
					{
						if (KeyStack_OpenKey(&keyStack, KEY_READ,pOEMName))
						{
							/* get value */
							if (GetStringValue(KeyStack_GetTopItem(&keyStack), _T("OEMName"), ppName))
							{
								bSuccess = TRUE;
							}
						}
					}

					free(pOEMName);
				}
			}
		}
	}
	else
	{
		TCHAR VID_PID_RegKeyName[18];

		/* generate the vendor id/product id key */
		_stprintf(VID_PID_RegKeyName,"VID_%04x&PID_%04x",pJoystickCaps->wMid, pJoystickCaps->wPid);

		if (KeyStack_ExecutePath(&keyStack, HKEY_LOCAL_MACHINE, KeyPath))
		{
			if (KeyStack_OpenKey(&keyStack, KEY_READ,VID_PID_RegKeyName))
			{			
				/* get value */
				if (GetStringValue(KeyStack_GetTopItem(&keyStack), _T("OEMName"), ppName))
				{
					bSuccess = TRUE;
				}
			}
		}
	}

	KeyStack_Close(&keyStack);

	return bSuccess;
}

#include <commctrl.h>
#include <windows.h>
#include "mylistvw.h"

void	JoystickConfiguration_SetupListView(HWND hListView)
{
	int NumJoysticks;
	int i;

	SendMessage(hListView, WM_SETREDRAW, (WPARAM)FALSE, (LPARAM)0);

	/* delete all existing items */
	ListView_DeleteAllItems(hListView);

	/* add columns */
	MyListView_AddColumn(hListView, _T("ID"),0);
	MyListView_AddColumn(hListView, _T("Name"),1);

	/* get number of devices that are supported */
	NumJoysticks = joyGetNumDevs();

	for (i=0; i<NumJoysticks; i++)
	{
		int Count;
		MMRESULT	hResult;
		JOYCAPS JoystickCaps;		
		char IDText[32];

		Count = ListView_GetItemCount(hListView);

		sprintf(IDText,"%d",i+1);

		/* add column 0 data */
		MyListView_AddItem(hListView,IDText,0,Count, (void *)i);

		/* add column 1 data */
		hResult = joyGetDevCaps(i, &JoystickCaps, sizeof(JoystickCaps));

		if (hResult==JOYERR_NOERROR)
		{
			char *name;

			if (GetJoystickName(i,&JoystickCaps,&name))
			{
				MyListView_AddItem(hListView, name,1,Count,NULL);
				
				free(name);
			}
			else
			{
				char JoystickName[32];
				
				sprintf(JoystickName,"Joystick #%d",i);

				MyListView_AddItem(hListView, name,1,Count,NULL);
			}
		}
		else
		{
			/* (none) */
			MyListView_AddItem(hListView, _T("(none)"),1,Count, NULL);
		}
	}
	SendMessage(hListView, WM_SETREDRAW, (WPARAM)TRUE, (LPARAM)0);
	InvalidateRect(hListView,NULL, FALSE);

}

extern JoystickConfigData joystickConfigs[3];

BOOL CALLBACK  JoystickConfiguration_DialogProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
        
		case WM_INITDIALOG:
		{
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
					HWND hList = GetDlgItem(hwnd, IDC_LIST_JOY);


					int index = ListView_GetNextItem(hList, -1, LVIS_SELECTED);
					joystickConfigs[0].Type = JOYSTICK_REAL;
					joystickConfigs[0].realJoystickData.id = index;

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

void JoystickConfigurationDialog(HWND hwnd)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_JOYSTICK),hwnd,JoystickConfiguration_DialogProc);
}


/* read joystick */
void	Joystick_Read(int id, JoyInfo *joy)
{
	MMRESULT	hResult;



//	if (JoystickEnabled)
	{
		JOYCAPS JoystickCaps;		

		hResult = joyGetDevCaps(id, &JoystickCaps, sizeof(JoystickCaps));

		if (hResult==JOYERR_NOERROR)
		{
			JOYINFOEX JoystickInfo;
			DWORD Xmid;
			DWORD Ymid;

			JoystickInfo.dwSize = sizeof(JOYINFOEX);
			JoystickInfo.dwFlags = JOY_RETURNALL;

			Xmid = ((JoystickCaps.wXmax-JoystickCaps.wXmin)>>1) + JoystickCaps.wXmin;
			Ymid = ((JoystickCaps.wYmax-JoystickCaps.wYmin)>>1) + JoystickCaps.wYmin;

			joysticks[id].x.min = JoystickCaps.wXmin;
			joysticks[id].x.max = JoystickCaps.wXmax;
			joysticks[id].y.min = JoystickCaps.wYmin;
			joysticks[id].y.max = JoystickCaps.wYmax;
			joysticks[id].x.dead[0] = Xmid-2000;
			joysticks[id].x.dead[1] = Xmid+2000;
			joysticks[id].y.dead[0] = Ymid-2000;
			joysticks[id].y.dead[1] = Ymid+2000;
				
			hResult = joyGetPosEx(id, &JoystickInfo);

			if (hResult==JOYERR_NOERROR)
			{
				HandleDeadZone(&joysticks[id].x, JoystickInfo.dwXpos);
				HandleDeadZone(&joysticks[id].y, JoystickInfo.dwYpos);
				joysticks[id].buttons = JoystickInfo.dwButtons;

				joy->x = joysticks[id].x.cur;
				joy->y = joysticks[id].y.cur;
				joy->buttons = joysticks[id].buttons;

			}
		}
	}
}

void	Joystick_Init()
{
	int NoOfJoysticks;

//	JoystickEnabled = FALSE;

	/* get number of devices supported by this driver */
	NoOfJoysticks = joyGetNumDevs();

	if (NoOfJoysticks!=0)
	{
#if 0
		/* select joystick 1 */
		joystickID = JOYSTICKID1;

		/* is joystick attached */
		hResult = joyGetPos(joystickID, &JoystickInfo);

		if (hResult != MMSYSERR_NODRIVER)
		{
			/* driver available */

			hResult = joyGetDevCaps(joystickID, &JoystickCaps, sizeof(JoystickCaps));

			if (hResult == JOYERR_NOERROR)
			{
				XMid = (JoystickCaps.wXmax-JoystickCaps.wXmin)>>1;
				YMid = (JoystickCaps.wYmax-JoystickCaps.wYmin)>>1;
				XDead = 2000; 
				YDead = 2000;				

				JoystickEnabled = TRUE;
			}
		}
#endif
	}
//	JoystickEnabled = TRUE;
}
