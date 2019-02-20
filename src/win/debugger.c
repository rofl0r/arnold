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
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <zmouse.h>
#include "../cpc/messages.h"

//#include <stdio.h>
//#include <stdlib.h>
#include "debugger.h"
#include "resource.h"

#include "../cpc/z80/z80.h"
#include "../cpc/cpc.h"
#include "mylistvw.h"
#include "../cpc/debugger/gdebug.h"
#include "../cpc/debugger/item.h"
#include "../cpc/debugger/parse.h"
#include "../cpc/debugger/memdump.h"
#include "../cpc/debugger/dissasm.h"
#include "cpcemu.h"
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

extern APP_DATA AppData;

void	Debugger_DebugHooks(HWND);
void	Debugger_RegisterSpriteClass(HWND hParent);
void	Debugger_RegisterShowGfxClass(HWND hParent);
void	Debugger_OpenDissassemble(HWND);
void	Debugger_OpenMemdump(HWND);
void	Debugger_OpenDebugDialog(void);
//void	Debugger_OpenCRTCDialog(HWND);
void	Debugger_ShowSprite(HWND);
void	Debugger_ShowGfx(HWND);
void	Debugger_DestroyCRTCDialog();
void	ShowASICDialog();
void	Debugger_RedrawShowGfxWindow();
void Debugger_OpenCPCPlusInfo(HWND hParent);
void	Debugger_RegisterCPCInfoClass(HWND hParent);
void	Debugger_RegisterCPCPLUSInfoClass(HWND hParent);
void	Debugger_RegisterCRTCInfoClass(HWND hParent);
void	Debugger_OpenCRTCInfo(HWND hParent);

void	Debugger_UpdateCPCInfo(void);
void	Debugger_UpdateCPCPLUSInfo(void);
void	Debugger_UpdateCRTCInfo(void);
void Debugger_OpenCPCInfo(HWND hParent);


#include "general\lnklist\lnklist.h"


typedef struct
{

	int SearchStringCount;
	char *pSearchString;
	char *pSearchStringMask;

	// memdump pop-up menu
	HMENU hMemdumpPopupMenu;
	// dissassemble pop-up menu
	HMENU hDissassemblePopupMenu;
	// memdump pop-up menu
	HMENU hShowGfxPopupMenu;

	HWND hSprite;

	/* list of memdump window's */
	LIST_HEADER	*pMemdump_WindowList;
	/* list of dissassemble window's */
	LIST_HEADER *pDissassemble_WindowList;

	int *HexSearchCount;
	char * HexSearchString;
	char * HexSearchMask;

	HWND hMemDump;


	SEARCH_DATA	SearchData;


	HWND	hDebuggerDialog;
	HWND	hParentHwnd;
	HWND	hDissassemble;

} DEBUGGER_DATA;

HWND hCPCInfo;
#define CPCEMU_DEBUG_CPCINFO_CLASS "ARNOLD_DEBUG_CPCINFO_CLASS"

HWND hCPCPLUSInfo;
#define CPCEMU_DEBUG_CPCPLUSINFO_CLASS "ARNOLD_DEBUG_CPCPLUSINFO_CLASS"

HWND hCRTCInfo;
#define CPCEMU_DEBUG_CRTCINFO_CLASS "ARNOLD_DEBUG_CRTCINFO_CLASS"

static DEBUGGER_DATA DebuggerData;






BOOL	HexDialog(HWND hwnd,int *);

//extern HINSTANCE hInstCommonControls;

void Debug_ErrorMessage(TCHAR *ErrorText)
{
	MessageBox(GetActiveWindow(),ErrorText,Messages[41],MB_OK);
}

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////


/*----------------------------------------------------------------------*/

void	ForceRedraw(HWND hwnd)
{
	if (hwnd!=0)
	{
		RECT	WindowRect;

		/* get the client rectangle */
		GetClientRect(hwnd,&WindowRect);

		/* invalidate rect but don't erase background */
		InvalidateRect(hwnd,&WindowRect,FALSE);
	}
}



/*----------------------------------------------------------------------*/
/* When the window is allocated we use 2 words to store the address of our structure */
void *GetWindowDataPtr(HWND hwnd)
{
	if (hwnd==0)
		return NULL;

	{
		int AddrLow = GetWindowWord(hwnd,0);
		int AddrHigh = GetWindowWord(hwnd,2);

		return (void *)((AddrLow & 0x0ffff) | ((AddrHigh & 0x0ffff)<<16));
	}
}

void	SetWindowDataPtr(HWND hwnd, void *pPtr)
{
	if (hwnd!=NULL)
	{
		WORD data;

		data = ((long)pPtr) & 0x0ffff;

		SetWindowWord(hwnd, 0, data);

		data = (((long)pPtr)>>16) & 0x0ffff;

		SetWindowWord(hwnd, 2, data);
	}
}

/*----------------------------------------------------------------------*/


typedef struct
{
	int FontWidth;
	int FontHeight;
	int WindowWidth;
	int WindowHeight;
	int NoOfCharsInWidth;
	int NoOfCharsInHeight;
	int NoOfBytes;
} WINDOW_INFO;

void	GetWindowInfoI(HWND hwnd, WINDOW_INFO *pInfo)
{
HDC hDC;
RECT	WindowRect;
HFONT hFont,hOldFont;
TEXTMETRIC FontMetric;

	hDC = GetDC(hwnd);

	if (hDC!=NULL)
	{
		hFont = GetStockObject(ANSI_FIXED_FONT);

		hOldFont = SelectObject(hDC, hFont);

		GetTextMetrics(hDC,&FontMetric);

		pInfo->FontWidth = FontMetric.tmMaxCharWidth;
		pInfo->FontHeight = FontMetric.tmHeight + FontMetric.tmExternalLeading;

        GetClientRect(hwnd,&WindowRect);

		pInfo->WindowWidth = WindowRect.right-WindowRect.left;
		pInfo->WindowHeight = WindowRect.bottom-WindowRect.top;

		pInfo->NoOfCharsInWidth = pInfo->WindowWidth/pInfo->FontWidth;
		pInfo->NoOfCharsInHeight = pInfo->WindowHeight/pInfo->FontHeight;

		SelectObject(hDC, hOldFont);

		DeleteObject(hFont);

		ReleaseDC(hwnd,hDC);
	}
}

///////////////////////////////////////////////////////////////////////////////////////


BOOL CALLBACK  HexSearchDialogProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

	TCHAR HexValueText[64];

    switch (iMsg)
    {

	case WM_INITDIALOG:
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
//						int Number;

					GetDlgItemText(hwnd,IDC_EDIT_HEX_VALUE,HexValueText,16);

					HexSearch_Evaluate(HexValueText,DebuggerData.HexSearchCount, DebuggerData.HexSearchString,DebuggerData.HexSearchMask);

//					Number = EvaluateExpression(HexValueText);

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

BOOL HexSearchDialog(HWND hwnd,int *pSearchCount, TCHAR **pSearchString, TCHAR **pSearchStringMask)
{
	int Result;

	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

	DebuggerData.HexSearchCount = pSearchCount;
	DebuggerData.HexSearchString = pSearchString;
	DebuggerData.HexSearchMask = pSearchStringMask;

	Result = DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_ENTER_HEX),hwnd,HexSearchDialogProc);

//	*pValue = Result;

	if (Result==-1)
	{
		return FALSE;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////

#include <commctrl.h>
#define TOOLBAR_BUTTON_ENTRY(x,y)	{x,y,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0}

#define CPCEMU_DEBUG_MEMDUMP_CLASS "ARNOLD_DEBUG_MEMDUMP_CLASS"


long FAR PASCAL MemDumpWindowProc( HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT PaintStruct;

    switch(iMsg)
    {


		case WM_CREATE:
		{
			MEMDUMP_WINDOW *pMemdumpWindow;

			pMemdumpWindow = MemDump_Create();
			pMemdumpWindow->pReadOperation = MemoryRead;
			pMemdumpWindow->pWriteOperation = MemoryWrite;
			SetWindowDataPtr(hwnd,(void *)pMemdumpWindow);
		}
		break;


		case WM_RBUTTONDOWN:
		{
			POINT point;

			point.x = LOWORD(lParam);  // horizontal position of cursor
			point.y = HIWORD(lParam);  // vertical position of cursor

			ClientToScreen(hwnd, &point);

			// display pop-up menu
			TrackPopupMenu(GetSubMenu(DebuggerData.hMemdumpPopupMenu,0),
				TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				point.x, point.y, 0, hwnd,
				NULL);
		}
		break;

	case WM_COMMAND:
		{
			MEMDUMP_WINDOW *pMemdumpWindow = (MEMDUMP_WINDOW *)GetWindowDataPtr(hwnd);


			switch (LOWORD(wParam))
			{
				case ID_MEMDUMP_GOTOADDRESS:
				{
					int NewAddr;

					if (HexDialog(hwnd, &NewAddr))
					{
						MemDump_SetAddress(pMemdumpWindow,NewAddr);

						ForceRedraw(hwnd);
					}
				}
				break;

				case ID_MEMDUMP_SEARCHFORDATA:
				{
					HexSearchDialog(hwnd,&DebuggerData.SearchStringCount, &DebuggerData.pSearchString, &DebuggerData.pSearchStringMask);
					DebuggerData.SearchData.pSearchString = DebuggerData.pSearchString;
					DebuggerData.SearchData.pSearchStringMask = DebuggerData.pSearchStringMask;
					DebuggerData.SearchData.NumBytes = DebuggerData.SearchStringCount;

					DebuggerData.SearchData.FoundAddress = -1;

					if (Memdump_FindData(pMemdumpWindow,&DebuggerData.SearchData)==-1)
					{
						MessageBox(hwnd,Messages[42], Messages[43], MB_ICONEXCLAMATION | MB_OK);
					}


					ForceRedraw(hwnd);


				}
				break;
			}
		}
		break;

	case WM_KEYDOWN:
	{
		int VirtualKeyCode = (int)wParam;
		MEMDUMP_WINDOW *pMemdumpWindow = (MEMDUMP_WINDOW *)GetWindowDataPtr(hwnd);

		switch (VirtualKeyCode)
		{
			case VK_F3:
			{
				if (Memdump_FindData(pMemdumpWindow,&DebuggerData.SearchData)==-1)
				{
					MessageBox(hwnd,Messages[42], Messages[43], MB_ICONEXCLAMATION | MB_OK);
				}

			}
			break;

			case VK_UP:
			{
				MemDump_CursorUp(pMemdumpWindow);
			}
			break;

			case VK_DOWN:
			{
				MemDump_CursorDown(pMemdumpWindow);
			}
			break;

			case VK_LEFT:
			{
				MemDump_CursorLeft(pMemdumpWindow);
			}
			break;

			case VK_RIGHT:
			{
				MemDump_CursorRight(pMemdumpWindow);
			}
			break;

			case VK_TAB:
			{
				Memdump_ToggleLocation(pMemdumpWindow);
			}
			break;

			case VK_PRIOR:
			{
				MemDump_PageUp(pMemdumpWindow);
			}
			break;

			case VK_NEXT:
			{
				MemDump_PageDown(pMemdumpWindow);
			}
			break;


			default:
				break;
		}

		ForceRedraw(hwnd);
	}
	break;

	case WM_LBUTTONDOWN:
	{
		int xPos, yPos;
		int CharX, CharY;
		MEMDUMP_WINDOW *pMemdumpWindow = (MEMDUMP_WINDOW *)GetWindowDataPtr(hwnd);

		WINDOW_INFO WindowInfo;

		GetWindowInfoI(hwnd, &WindowInfo);

		xPos = LOWORD(lParam);  // horizontal position of cursor
		yPos = HIWORD(lParam);  // vertical position of cursor

		CharX = xPos/WindowInfo.FontWidth;
		CharY = yPos/WindowInfo.FontHeight;

		MemDump_SelectByCharXY(pMemdumpWindow, CharX,CharY);

		ForceRedraw(hwnd);
	}
	break;

	case WM_MOUSEWHEEL:
	{
		MEMDUMP_WINDOW *pMemdumpWindow = (MEMDUMP_WINDOW *)GetWindowDataPtr(hwnd);
		int WheelRotation = wParam;

		if (WheelRotation<0)
		{
			MemDump_CursorUp(pMemdumpWindow);
		}
		else
		{
			MemDump_CursorDown(pMemdumpWindow);
		}


		ForceRedraw(hwnd);
		// wParam = wheel rotation expressed in multiples of WHEEL_DELTA
   // lParam is the mouse coordinates



	}
	break;

	case WM_CHAR:
	{
		int chCharCode = (TCHAR) wParam;
		MEMDUMP_WINDOW *pMemdumpWindow = (MEMDUMP_WINDOW *)GetWindowDataPtr(hwnd);

		switch (chCharCode)
		{
			case 'V':
			case 'v':
				Memdump_ToggleView(pMemdumpWindow);
				break;
			default:
				Memdump_UpdateEdit(pMemdumpWindow, chCharCode);
				break;
		}

		ForceRedraw(hwnd);
	}
	break;

	case WM_PAINT:
        {
			HDC hDC;
			HFONT hFont,hOldFont;
			int j;
			RECT WindowRect;
			int NoOfCharsInWidth, NoOfCharsInHeight;

			WINDOW_INFO WindowInfo;

			MEMDUMP_WINDOW *pMemdumpWindow = (MEMDUMP_WINDOW *)GetWindowDataPtr(hwnd);

			/* get window info */
			GetWindowInfoI(hwnd,&WindowInfo);

			GetClientRect(hwnd, &WindowRect);

			BeginPaint( hwnd, &PaintStruct);

			hDC = PaintStruct.hdc;

			hFont = GetStockObject(ANSI_FIXED_FONT);

			hOldFont = SelectObject(hDC, hFont);

			SetTextColor(hDC,RGB(0,0,0));

			NoOfCharsInWidth = WindowInfo.NoOfCharsInWidth;
			NoOfCharsInHeight = WindowInfo.NoOfCharsInHeight;

			pMemdumpWindow->WidthInChars = NoOfCharsInWidth;
			pMemdumpWindow->HeightInChars = NoOfCharsInHeight;
			MemDump_RefreshState(pMemdumpWindow);

			/* clear screen */
			FillRect(hDC,&WindowRect,GetStockObject(WHITE_BRUSH));

			/* show cursor */
			{
				RECT CursorRect;

				CursorRect.left = pMemdumpWindow->CursorXAbsolute*WindowInfo.FontWidth;
				CursorRect.top = pMemdumpWindow->CursorYAbsolute*WindowInfo.FontHeight;
				CursorRect.right = CursorRect.left + WindowInfo.FontWidth;
				CursorRect.bottom = CursorRect.top + WindowInfo.FontHeight;

				FillRect(hDC, &CursorRect, GetStockObject(GRAY_BRUSH));
			}

			SetBkMode(hDC, TRANSPARENT);

			for (j=0; j<NoOfCharsInHeight; j++)
			{
				int TextOut_Length;
				TCHAR *pMemDumpString;

				pMemDumpString = Memdump_DumpLine(pMemdumpWindow, j);

				TextOut_Length = _tcslen(pMemDumpString);

				if (TextOut_Length>pMemdumpWindow->WidthInChars)
				{
					TextOut_Length = pMemdumpWindow->WidthInChars;
				}

				TextOut(hDC, 0, j*WindowInfo.FontHeight, pMemDumpString,TextOut_Length);
			}


			SelectObject(hDC,hOldFont);

			DeleteObject(hFont);

			EndPaint( hwnd, &PaintStruct );

			return TRUE;
		}

	case WM_ERASEBKGND:
		return 1;


	case WM_VSCROLL:
		{
			int ScrollCode = LOWORD(wParam);
			int Pos = HIWORD(wParam);
			HWND hwndScrollBar = (HWND)lParam;
			MEMDUMP_WINDOW *pMemdumpWindow = (MEMDUMP_WINDOW *)GetWindowDataPtr(hwnd);

			switch (ScrollCode)
			{

				case SB_TOP:
					break;

				case SB_BOTTOM:
					break;

				case SB_LINEDOWN:
					MemDump_CursorDown(pMemdumpWindow);
					break;

				case SB_LINEUP:
					MemDump_CursorUp(pMemdumpWindow);
					break;
			}

			ForceRedraw(hwnd);

		}
		return TRUE;

		case WM_SIZE:
		{
			UpdateWindow(hwnd);
		}
		return TRUE;

		case WM_DESTROY:
        {
			MEMDUMP_WINDOW *pMemdumpWindow = (MEMDUMP_WINDOW *)GetWindowDataPtr(hwnd);

			MemDump_Finish(pMemdumpWindow);
			DebuggerData.hMemDump = 0;
		}
		break;
   }

    return DefWindowProc(hwnd, iMsg, wParam, lParam);

}


void	Debugger_RegisterMemdumpClass(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE);

	WNDCLASSEX	DebugWindowClass;

	DebugWindowClass.cbSize = sizeof(WNDCLASSEX);
	DebugWindowClass.style = CS_HREDRAW | CS_VREDRAW;	// | CS_OWNDC;
	DebugWindowClass.lpfnWndProc = MemDumpWindowProc;
	DebugWindowClass.cbClsExtra = 0;
	DebugWindowClass.cbWndExtra = 4;
	DebugWindowClass.hInstance = hInstance;
	DebugWindowClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	DebugWindowClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	DebugWindowClass.hbrBackground = GetStockObject(GRAY_BRUSH); //NULL; //GetStockObject(COLOR_APPWORKSPACE);
	DebugWindowClass.lpszMenuName = NULL;
	DebugWindowClass.lpszClassName = _T(CPCEMU_DEBUG_MEMDUMP_CLASS);
	DebugWindowClass.hIconSm = LoadIcon(NULL,IDI_APPLICATION);

	if (RegisterClassEx(&DebugWindowClass)==0)
	{
		Debug_ErrorMessage(Messages[44]);
	}
}

void Debugger_OpenMemdump(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE);

	if (DebuggerData.hMemDump==NULL)
	{
		DebuggerData.hMemDump = CreateWindowEx(
			WS_EX_OVERLAPPEDWINDOW,
			CPCEMU_DEBUG_MEMDUMP_CLASS,
			Messages[45],
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_VSCROLL,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			NULL,
			NULL,
			hInstance,
			NULL );

		ShowWindow( DebuggerData.hMemDump, TRUE);
		UpdateWindow( DebuggerData.hMemDump );
	}
	else
	{
		/* window already shown - make it visible */
		SetWindowPos(DebuggerData.hMemDump, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	}

}

//--------------------------------------------------------------------//

#define CPCEMU_DEBUG_DISSASSEMBLE_CLASS "ARNOLD_DEBUG_DISSASSEMBLE_CLASS"

void	Debugger_SetDissassembleAddress(int Addr)
{
	if (DebuggerData.hDissassemble!=NULL)
	{
		DISSASSEMBLE_WINDOW *pWindow;

		pWindow = (DISSASSEMBLE_WINDOW *)GetWindowDataPtr(DebuggerData.hDissassemble);

		Dissassemble_SetAddress(pWindow, Addr);

		ForceRedraw(DebuggerData.hDissassemble);
	}
}

void	DissassembleWindow_Render(DISSASSEMBLE_WINDOW *pDissassembleWindow, HDC hDC, int X, int Y,int FontHeight)
{
	int YCoord;
	int i;

	YCoord = Y;

	Dissassemble_BeginDissassemble(pDissassembleWindow);

	for (i=0; i<pDissassembleWindow->WindowHeight; i++)
	{
		TCHAR *pDebugString;
		int		TextOut_Length;

		pDebugString = Dissassemble_DissassembleNextLine(pDissassembleWindow);

		TextOut_Length = _tcslen(pDebugString);

		if (TextOut_Length>pDissassembleWindow->WidthInChars)
		{
			TextOut_Length = pDissassembleWindow->WidthInChars;
		}

		TextOut(hDC, X, YCoord, pDebugString,TextOut_Length);

		YCoord += FontHeight;
	}
}


long FAR PASCAL DissassembleWindowProc( HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT PaintStruct;

    switch(iMsg)
    {


    case WM_CREATE:
		{
			DISSASSEMBLE_WINDOW *pDissassembleWindow;

			pDissassembleWindow = Dissassemble_Create();


//			pDissassembleWindow->GetOpcodeCountFunction = Debug_GetOpcodeCount;
//			pDissassembleWindow->DissassembleInstruction = Debug_DissassembleInstruction;

			SetWindowDataPtr(hwnd, (void *)pDissassembleWindow);

			{
				TCHAR	Title[256];

				_stprintf(Title, Messages[46], Dissassemble_GetViewName(pDissassembleWindow));

				SetWindowText(hwnd,Title);
			}

		}



		break;

		case WM_RBUTTONDOWN:
		{
			POINT point;

			point.x = LOWORD(lParam);  // horizontal position of cursor
			point.y = HIWORD(lParam);  // vertical position of cursor

			ClientToScreen(hwnd, &point);

			// display pop-up menu
			TrackPopupMenu(GetSubMenu(DebuggerData.hDissassemblePopupMenu,0),
				TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				point.x, point.y, 0, hwnd,
				NULL);
		}
		break;

	case WM_COMMAND:
		{
			DISSASSEMBLE_WINDOW *pDissassembleWindow = (DISSASSEMBLE_WINDOW *)GetWindowDataPtr(hwnd);

			switch (LOWORD(wParam))
			{
				case ID_DISSASSEMBLE_GOTOADDRESS:
				{
					int NewAddr;

					if (HexDialog(hwnd, &NewAddr))
					{
						Dissassemble_SetAddress(pDissassembleWindow,NewAddr);

						ForceRedraw(hwnd);
					}
				}
				break;
			}
		}
		break;

    case WM_PAINT:
        {
			HDC	hDC;
			RECT	WindowRect;
			WINDOW_INFO WindowInfo;
			HFONT hFont,hOldFont;
			HPEN hPen, hOldPen;
//			int PC;

//			int WindowHeight;
			int NoOfCharsInWidth, NoOfCharsInHeight;

			//int Addr = GetWindowWord(hwnd,0);
			DISSASSEMBLE_WINDOW *pDissassembleWindow;

			pDissassembleWindow = (DISSASSEMBLE_WINDOW *)GetWindowDataPtr(hwnd);

			/* get window info */
			GetWindowInfoI(hwnd,&WindowInfo);

			/* get client rect of window */
			GetClientRect(hwnd,&WindowRect);

			BeginPaint( hwnd, &PaintStruct);

			hDC = PaintStruct.hdc;

			hFont = GetStockObject(ANSI_FIXED_FONT);
			hPen = GetStockObject(BLACK_BRUSH);

			hOldFont = SelectObject(hDC, hFont);
			hOldPen = SelectObject(hDC, hPen);

			FillRect(hDC,&WindowRect,GetStockObject(WHITE_BRUSH));

			NoOfCharsInWidth = WindowInfo.NoOfCharsInWidth;
			NoOfCharsInHeight = WindowInfo.NoOfCharsInHeight;

			pDissassembleWindow->WidthInChars = NoOfCharsInWidth;
			pDissassembleWindow->WindowHeight = NoOfCharsInHeight;
			Dissassemble_RefreshState(pDissassembleWindow);

			SetBkMode(hDC, TRANSPARENT);


			// debug display
			{
				RECT CursorRect;

				CursorRect.left = 0;
				CursorRect.top = pDissassembleWindow->CursorYAbsolute*WindowInfo.FontHeight;
				CursorRect.right = WindowInfo.NoOfCharsInWidth*WindowInfo.FontWidth;
				CursorRect.bottom = CursorRect.top + WindowInfo.FontHeight;


				FillRect(hDC, &CursorRect, GetStockObject(GRAY_BRUSH));
			}



			DissassembleWindow_Render(pDissassembleWindow, hDC, 0,0, WindowInfo.FontHeight);

			SelectObject(hDC, hOldPen);
			SelectObject(hDC, hOldFont);

			DeleteObject(hPen);
			DeleteObject(hFont);

			EndPaint( hwnd, &PaintStruct );

			return TRUE;
		}



	case WM_KEYDOWN:
	{
		int VirtualKeyCode = (int)wParam;
		DISSASSEMBLE_WINDOW *pMemdumpWindow = (DISSASSEMBLE_WINDOW *)GetWindowDataPtr(hwnd);

		switch (VirtualKeyCode)
		{
			case VK_UP:
			{
				Dissassemble_CursorUp(pMemdumpWindow);
			}
			break;

			case VK_DOWN:
			{
				Dissassemble_CursorDown(pMemdumpWindow);
			}
			break;

			case VK_LEFT:
			{
			}
			break;

			case VK_RIGHT:
			{
			}
			break;

			case VK_TAB:
			{
			}
			break;

			case VK_PRIOR:
			{
				Dissassemble_PageUp(pMemdumpWindow);
			}
			break;

			case VK_NEXT:
			{
				Dissassemble_PageDown(pMemdumpWindow);
			}
			break;

			case VK_F5:
			{
				Debug_SetState(DEBUG_RUNNING);
			}
			break;

			case VK_F10:
			{
				Debug_SetState(DEBUG_STEP_INTO);
			}
			break;


			default:
				break;
		}

		ForceRedraw(hwnd);

	}
	break;

	case WM_LBUTTONDOWN:
	{
		int xPos, yPos;
		int CharX, CharY;
		WINDOW_INFO WindowInfo;
		DISSASSEMBLE_WINDOW *pDissassembleWindow = (DISSASSEMBLE_WINDOW *)GetWindowDataPtr(hwnd);

		// get window info
		GetWindowInfoI(hwnd, &WindowInfo);

		xPos = LOWORD(lParam);  // horizontal position of cursor
		yPos = HIWORD(lParam);  // vertical position of cursor

		CharX = xPos/WindowInfo.FontWidth;
		CharY = yPos/WindowInfo.FontHeight;

		Dissassemble_SelectByCharXY(pDissassembleWindow, CharX,CharY);

		ForceRedraw(hwnd);
	}
	break;

	case WM_MOUSEWHEEL:
	{
		DISSASSEMBLE_WINDOW *pDissassembleWindow = (DISSASSEMBLE_WINDOW *)GetWindowDataPtr(hwnd);
		int WheelRotation = wParam;

		if (WheelRotation<0)
		{
			Dissassemble_CursorUp(pDissassembleWindow);
		}
		else
		{
			Dissassemble_CursorDown(pDissassembleWindow);
		}


		ForceRedraw(hwnd);
		// wParam = wheel rotation expressed in multiples of WHEEL_DELTA
   // lParam is the mouse coordinates



	}
	break;


	case WM_ERASEBKGND:
		return 1;

	case WM_CHAR:
	{
		int chCharCode = (TCHAR) wParam;
		DISSASSEMBLE_WINDOW *pDissassembleWindow = (DISSASSEMBLE_WINDOW *)GetWindowDataPtr(hwnd);

		switch (chCharCode)
		{
			case 'O':
			case 'o':
				Dissassemble_ToggleOpcodes(pDissassembleWindow);
				break;
			case 'A':
			case 'a':
				Dissassemble_ToggleAscii(pDissassembleWindow);
				break;
			case 'B':
			case 'b':
				Dissassemble_ToggleBreakpoint(pDissassembleWindow);
				break;
			case 'V':
			case 'v':
			{
				TCHAR	Title[256];

				Dissassemble_ToggleView(pDissassembleWindow);


				_stprintf(Title, Messages[46], Dissassemble_GetViewName(pDissassembleWindow));

				SetWindowText(hwnd,Title);

			}
			break;

			default:
				break;
		}

		ForceRedraw(hwnd);
	}
	break;

	case WM_VSCROLL:
		{
			int ScrollCode = LOWORD(wParam);
			int Pos = HIWORD(wParam);
			HWND hwndScrollBar = (HWND)lParam;
			DISSASSEMBLE_WINDOW *pDissassembleWindow = (DISSASSEMBLE_WINDOW *)GetWindowDataPtr(hwnd);

			switch (ScrollCode)
			{

				case SB_TOP:
					break;

				case SB_BOTTOM:
					break;

				case SB_LINEDOWN:
					Dissassemble_CursorDown(pDissassembleWindow);
					break;

				case SB_LINEUP:
					Dissassemble_CursorUp(pDissassembleWindow);
					break;
			}

			ForceRedraw(hwnd);

		}
		return TRUE;

	case WM_SIZE:
	//	Dissassem
		UpdateWindow(hwnd);
		return TRUE;

	    case WM_DESTROY:
        {
			DISSASSEMBLE_WINDOW *pDissassembleWindow = (DISSASSEMBLE_WINDOW *)GetWindowDataPtr(hwnd);

			Dissassemble_Finish(pDissassembleWindow);
			DebuggerData.hDissassemble = 0;
		}
		break;

   }

    return DefWindowProc(hwnd, iMsg, wParam, lParam);

}

void	Debugger_RegisterDissassemblerClass(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE);

	WNDCLASSEX	DissassembleWindowClass;

	DissassembleWindowClass.cbSize = sizeof(WNDCLASSEX);
	DissassembleWindowClass.style = CS_HREDRAW | CS_VREDRAW;	// | CS_OWNDC;
	DissassembleWindowClass.lpfnWndProc = DissassembleWindowProc;
	DissassembleWindowClass.cbClsExtra = 0;
	DissassembleWindowClass.cbWndExtra = 4;
	DissassembleWindowClass.hInstance = hInstance;
	DissassembleWindowClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	DissassembleWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW );
	DissassembleWindowClass.hbrBackground = GetStockObject(GRAY_BRUSH); //NULL; //GetStockObject(COLOR_APPWORKSPACE);
	DissassembleWindowClass.lpszMenuName = NULL;
	DissassembleWindowClass.lpszClassName = _T(CPCEMU_DEBUG_DISSASSEMBLE_CLASS);
	DissassembleWindowClass.hIconSm = LoadIcon(NULL,IDI_APPLICATION);

	if (RegisterClassEx(&DissassembleWindowClass)==0)
	{
		Debug_ErrorMessage(Messages[47]);
	}
}


void Debugger_OpenDissassemble(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE);

	if (DebuggerData.hDissassemble==NULL)
	{
		DebuggerData.hDissassemble = CreateWindowEx(
			WS_EX_OVERLAPPEDWINDOW,
			_T(CPCEMU_DEBUG_DISSASSEMBLE_CLASS),
			_T(""),
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_VSCROLL,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			NULL,
			NULL,
			hInstance,
			NULL );

		ShowWindow( DebuggerData.hDissassemble, TRUE);
		UpdateWindow( DebuggerData.hDissassemble );
	}
	else
	{
		/* window already shown - make it visible */
		SetWindowPos(DebuggerData.hDissassemble, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	}

}

void	Debugger_Initialise(HWND hwnd)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);

	/* initialise the dissasemble window list */
	LinkList_InitialiseList(&DebuggerData.pDissassemble_WindowList);
	/* initialise the memdump window list */
	LinkList_InitialiseList(&DebuggerData.pMemdump_WindowList);

	/* load mem-dump pop-up window */
	DebuggerData.hMemdumpPopupMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU_MEMDUMP));
	/* load dissassemble pop-up window */
	DebuggerData.hDissassemblePopupMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU_DISSASSEMBLE));
	/* load mem-dump pop-up window */
	DebuggerData.hShowGfxPopupMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU_SHOWASGFX));

	DebuggerData.hParentHwnd = hwnd;

	Debugger_RegisterDissassemblerClass(hwnd);
	Debugger_RegisterMemdumpClass(hwnd);

	Debugger_RegisterSpriteClass(hwnd);
	Debugger_RegisterShowGfxClass(hwnd);
	Debugger_RegisterCPCInfoClass(hwnd);
	Debugger_RegisterCPCPLUSInfoClass(hwnd);
	Debugger_RegisterCRTCInfoClass(hwnd);

	Debug_SetDebuggerWindowOpenCallback(Debugger_OpenDebugDialog);
}

void	Debugger_Close()
{
	/* delete dissasemble window list */
//	LinkList_DeleteList(pDissassemble_WindowList);
	/* delete memdump window list */
//	LinkList_DeleteList(pMemdump_WindowList);

	if (DebuggerData.hMemdumpPopupMenu!=NULL)
	{
		DestroyMenu(DebuggerData.hMemdumpPopupMenu);
	}


	if (DebuggerData.hMemDump!=NULL)
	{
		PostMessage(DebuggerData.hMemDump, WM_CLOSE, 0,0);
	}

	if (DebuggerData.hDissassemble!=NULL)
	{
		PostMessage(DebuggerData.hDissassemble, WM_CLOSE, 0,0);
	}

	if (DebuggerData.hSprite!=NULL)
	{
		PostMessage(DebuggerData.hSprite, WM_CLOSE, 0,0);
	}
}


void	Debugger_UpdateDebugDialog();

void	UpdateEdit(HWND hwnd,int ID, int NotifyCode)
{
	int Z80_REG=-1;

	switch (ID)
	{
		case IDC_EDIT_REG_AF:
			Z80_REG = Z80_AF;
			break;
		case IDC_EDIT_REG_BC:
			Z80_REG = Z80_BC;
			break;
		case IDC_EDIT_REG_DE:
			Z80_REG = Z80_DE;
			break;
		case IDC_EDIT_REG_HL:
			Z80_REG = Z80_HL;
			break;
		case IDC_EDIT_REG_IX:
			Z80_REG = Z80_IX;
			break;
		case IDC_EDIT_REG_IY:
			Z80_REG = Z80_IY;
			break;
		case IDC_EDIT_REG_PC:
			Z80_REG = Z80_PC;
			break;
		case IDC_EDIT_REG_SP:
			Z80_REG = Z80_SP;
			break;
		case IDC_EDIT_REG_ALTAF:
			Z80_REG = Z80_AF2;
			break;
		case IDC_EDIT_REG_ALTBC:
			Z80_REG = Z80_BC2;
			break;
		case IDC_EDIT_REG_ALTDE:
			Z80_REG = Z80_DE2;
			break;
		case IDC_EDIT_REG_ALTHL:
			Z80_REG = Z80_HL2;
			break;
		case IDC_EDIT_REG_I:
			Z80_REG = Z80_I;
			break;
		case IDC_EDIT_REG_R:
			Z80_REG = Z80_R;
			break;
		case IDC_EDIT_REG_IM:
			Z80_REG = Z80_IM;
			break;
		case IDC_EDIT_REG_IFF1:
			Z80_REG = Z80_IFF1;
			break;
		case IDC_EDIT_REG_IFF2:
			Z80_REG = Z80_IFF2;
			break;
		default:
			return;
	}

	// notification code
	switch (NotifyCode)
	{
		case EN_KILLFOCUS:
		{
			TCHAR HexValueText[16];
			int Number;

			GetDlgItemText(hwnd,ID,HexValueText,16);

			if (Debug_ValidateNumberIsHex(HexValueText, &Number))
			{
				Z80_SetReg(Z80_REG, Number);

				Debugger_UpdateDebugDialog();
			}
		}
		break;

		default:
			break;


	}
}

BOOL CALLBACK  DebuggerDialogProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

    switch (iMsg)
    {

        case WM_CREATE:
            return TRUE;

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
			case IDC_BUTTON_STEP_INTO:
				{
					Debug_SetState(DEBUG_STEP_INTO);
				}
				return TRUE;
            case IDC_BUTTON_RUN_TO:
				{
					int NewAddr;

					if (HexDialog(hwnd, &NewAddr))
					{
						Debug_SetRunTo(NewAddr);
					}
				}
				return TRUE;
			case IDC_BUTTON_DEBUG_GO:
				{
					Debug_SetState(DEBUG_RUNNING);
				}
				return TRUE;
			case IDC_BUTTON_DEBUG_BREAK:
				{
					Debug_SetState(DEBUG_STOPPED);
				}
				return TRUE;

			case ID_VIEW_OPENMEMORYDUMPWINDOW:
			{
				Debugger_OpenMemdump(hwnd);
			}
			return TRUE;

			case ID_VIEW_OPENDISSASSEMBLYWINDOW:
			{
				Debugger_OpenDissassemble(hwnd);

				Debugger_SetDissassembleAddress(Z80_GetReg(Z80_PC));
			}
			return TRUE;



//			case IDC_BUTTON_DEBUG_HOOKS:
//			{
//				Debugger_DebugHooks(hwnd);
//			}
//			return TRUE;

//			case IDC_WRITE_MEM:
//			{
//				Debug_WriteMemoryToDisk("memdump2.bin");	//BaseMemoryToDisk("memdump2.bin");
//			}
//			return TRUE;

			case ID_VIEW_OPENCPCHARDWAREWINDOW:
				{
					Debugger_OpenCPCInfo(hwnd);

				}
				return TRUE;


			case ID_VIEW_OPENCRTCVIEW:
				{
					Debugger_OpenCRTCInfo(hwnd);
				}
				return TRUE;

			case ID_VIEW_OPENCPCPLUSHARDWAREVIEW:
				{
					Debugger_ShowSprite(hwnd);
					Debugger_OpenCPCPlusInfo(hwnd);
					//ShowASICDialog(hwnd);
				}
				return TRUE;

			case IDC_DEBUGGER_SHOWGFX:
				{
					Debugger_ShowGfx(hwnd);
				}
				return TRUE;
//			case IDC_REFRESH_DISPLAY:
//				{
//
//					// dump whole display to screen
//				Render_DumpDisplay();
//
//				}
//				return TRUE;

			case IDC_EDIT_REG_AF:
			case IDC_EDIT_REG_BC:
			case IDC_EDIT_REG_DE:
			case IDC_EDIT_REG_HL:
			case IDC_EDIT_REG_PC:
			case IDC_EDIT_REG_SP:
			case IDC_EDIT_REG_IX:
			case IDC_EDIT_REG_IY:
			case IDC_EDIT_REG_ALTAF:
			case IDC_EDIT_REG_ALTDE:
			case IDC_EDIT_REG_ALTHL:
			case IDC_EDIT_REG_ALTBC:
			case IDC_EDIT_REG_I:
			case IDC_EDIT_REG_R:
			case IDC_EDIT_REG_IM:
			case IDC_EDIT_REG_IFF1:
			case IDC_EDIT_REG_IFF2:
			{
				UpdateEdit(hwnd,LOWORD(wParam), HIWORD(wParam));
			}
			break;













			default:
				break;

			}




        }
		break;

		case WM_CLOSE:
			{

				Debugger_Close();
				//Debugger_DestroyCRTCDialog();
				DestroyWindow(hwnd);

				DebuggerData.hDebuggerDialog = NULL;
			}
			break;

		case WM_DESTROY:
			{
				Debugger_Close();

			}
		break;
	}
    return FALSE;
}



BOOL CALLBACK  EnterHexDialogProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

	TCHAR	HexValueText[64];

    switch (iMsg)
    {

	case WM_INITDIALOG:
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
						int Number;

					GetDlgItemText(hwnd,IDC_EDIT_HEX_VALUE,HexValueText,16);

					Number = EvaluateExpression(HexValueText);

					EndDialog(hwnd, Number & 0x0ffff);
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

BOOL HexDialog(HWND hwnd, int *pValue)
{
	int Result;

	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

	Result = DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_ENTER_HEX),hwnd,EnterHexDialogProc);

	*pValue = Result;

	if (Result==-1)
	{
		return FALSE;
	}

	return TRUE;
}




void	SetDlgHexWord(HWND hDialog, int Item, int Data)
{
	TCHAR	HexWord[5];
	_stprintf(HexWord,_T("%04x"),Data & 0x0ffff);

	SetDlgItemText(hDialog,Item,HexWord);
}

void	SetDlgHexByte(HWND hDialog, int Item, int Data)
{
	TCHAR	HexByte[3];
	_stprintf(HexByte,_T("%02x"),Data & 0x0ff);

	SetDlgItemText(hDialog,Item, HexByte);
}

void	SetDlgHexDigit(HWND hDialog, int Item, int Data)
{
	TCHAR	HexByte[2];
	_stprintf(HexByte,_T("%01x"),Data & 0x0f);

	SetDlgItemText(hDialog,Item, HexByte);
}

void	SetDlgBinByte(HWND hDialog, int Item, int Data)
{
	SetDlgItemText(hDialog,Item, Debug_BinaryString((unsigned char)Data));
}


void	Debugger_UpdateDebugDialog()
{
	if (DebuggerData.hDebuggerDialog!=NULL)
	{
	TCHAR	OutputString[64];

//	Z80_REGISTERS	*R;

//	R = Z80_GetReg();

	SetDlgHexWord(DebuggerData.hDebuggerDialog,IDC_EDIT_REG_AF, Z80_GetReg(Z80_AF));
	SetDlgHexWord(DebuggerData.hDebuggerDialog,IDC_EDIT_REG_BC, Z80_GetReg(Z80_BC));
	SetDlgHexWord(DebuggerData.hDebuggerDialog,IDC_EDIT_REG_DE, Z80_GetReg(Z80_DE));
	SetDlgHexWord(DebuggerData.hDebuggerDialog,IDC_EDIT_REG_HL, Z80_GetReg(Z80_HL));

	SetDlgHexWord(DebuggerData.hDebuggerDialog,IDC_EDIT_REG_PC, Z80_GetReg(Z80_PC));
	SetDlgHexWord(DebuggerData.hDebuggerDialog,IDC_EDIT_REG_SP, Z80_GetReg(Z80_SP));

	SetDlgHexWord(DebuggerData.hDebuggerDialog,IDC_EDIT_REG_ALTAF, Z80_GetReg(Z80_AF2));
	SetDlgHexWord(DebuggerData.hDebuggerDialog,IDC_EDIT_REG_ALTHL, Z80_GetReg(Z80_HL2));
	SetDlgHexWord(DebuggerData.hDebuggerDialog,IDC_EDIT_REG_ALTDE, Z80_GetReg(Z80_DE2));
	SetDlgHexWord(DebuggerData.hDebuggerDialog,IDC_EDIT_REG_ALTBC, Z80_GetReg(Z80_BC2));

	SetDlgHexWord(DebuggerData.hDebuggerDialog,IDC_EDIT_REG_IX, Z80_GetReg(Z80_IX));
	SetDlgHexWord(DebuggerData.hDebuggerDialog,IDC_EDIT_REG_IY, Z80_GetReg(Z80_IY));


	SetDlgHexByte(DebuggerData.hDebuggerDialog,IDC_EDIT_REG_I, Z80_GetReg(Z80_I));

	SetDlgHexByte(DebuggerData.hDebuggerDialog,IDC_EDIT_REG_R, Z80_GetReg(Z80_R));

	SetDlgHexDigit(DebuggerData.hDebuggerDialog,IDC_EDIT_REG_IM, Z80_GetReg(Z80_IM));

	SetDlgHexDigit(DebuggerData.hDebuggerDialog,IDC_EDIT_REG_IFF1, Z80_GetReg(Z80_IFF1));
	SetDlgHexDigit(DebuggerData.hDebuggerDialog,IDC_EDIT_REG_IFF2, Z80_GetReg(Z80_IFF2));

	SetDlgItemText(DebuggerData.hDebuggerDialog,IDC_STATIC_FLAGS, Debug_FlagsAsString());


	//memset(OutputString,0,sizeof(OutputString));
	//Debug_DissassembleInstruction(Z80_GetReg(Z80_PC), OutputString);
//	SetDlgItemText(DebuggerData.hDebuggerDialog,IDC_STATIC_DISSASSEMBLY, OutputString);

	//Debugger_SetDissassembleAddress(Z80_GetReg(Z80_PC));

	}
}


void	Debugger_OpenDebugDialog(void)
{
	if (!AppData.Windowed)
	{
		CPCEMU_SetWindowed();
	}
	{
		HWND hwnd = DebuggerData.hParentHwnd;
		HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

		if (DebuggerData.hDebuggerDialog==NULL)
		{
			DebuggerData.hDebuggerDialog = CreateDialog (hInstance, MAKEINTRESOURCE(IDD_DIALOG_DEBUGGER), hwnd, DebuggerDialogProc);

			if (DebuggerData.hDebuggerDialog!=NULL)
			{
				HMENU hMenu;

				ShowWindow(DebuggerData.hDebuggerDialog,TRUE);

				hMenu = LoadMenu(hInstance,MAKEINTRESOURCE(IDR_MENU2));

				SetMenu(DebuggerData.hDebuggerDialog,hMenu);

				/* set default text sizes for controls */


				{
					int i;

					const int Item_Size[] =
					{
						IDC_EDIT_REG_AF, 4,
						IDC_EDIT_REG_BC, 4,
						IDC_EDIT_REG_DE, 4,
						IDC_EDIT_REG_HL, 4,
						IDC_EDIT_REG_PC, 4,
						IDC_EDIT_REG_SP, 4,
						IDC_EDIT_REG_ALTAF, 4,
						IDC_EDIT_REG_ALTBC, 4,
						IDC_EDIT_REG_ALTDE, 4,
						IDC_EDIT_REG_ALTHL, 4,
						IDC_EDIT_REG_IX, 4,
						IDC_EDIT_REG_IY, 4,
						IDC_EDIT_REG_I, 2,
						IDC_EDIT_REG_R, 2,
						IDC_EDIT_REG_IM, 1,
						IDC_EDIT_REG_IFF1, 1,
						IDC_EDIT_REG_IFF2, 1
					};

					for (i=0; i<(sizeof(Item_Size)>>1); i++)
					{
						SendMessage(
							GetDlgItem(DebuggerData.hDebuggerDialog, Item_Size[(i<<1)]),
							EM_SETLIMITTEXT, Item_Size[(i<<1)+1], 0);
					}
				}
			}
		}

		if (DebuggerData.hDebuggerDialog!=NULL)
		{
			Debugger_UpdateDebugDialog();
			Debugger_UpdateCRTCInfo();
		}
	}
}

void	Debugger_CloseDebugDialog()
{
	if (DebuggerData.hDebuggerDialog)
	{
		DestroyWindow(DebuggerData.hDebuggerDialog);
		DebuggerData.hDebuggerDialog = NULL;
	}
}

/*
HWND hCRTCDialog = NULL;

void	Debugger_DestroyCRTCDialog()
{
	if (hCRTCDialog!=NULL)
	{
		DestroyWindow(hCRTCDialog);
		hCRTCDialog = NULL;
	}
}


BOOL CALLBACK  CRTCDialogProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

    switch (iMsg)
    {
		case WM_INITDIALOG:
			return TRUE;

		case WM_CREATE:
           return TRUE;


		case WM_CLOSE:
			Debugger_DestroyCRTCDialog();
			break;

		case WM_DESTROY:
			break;

		default:
			break;

	}
    return FALSE;
}
*/
/*
void	Debugger_UpdateCRTCDialog()
{
	if (hCRTCDialog!=NULL)
	{
	CRTC_STATE	CRTC_State;

	CRTC_GetState(&CRTC_State);

	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_REG0,CRTC_GetRegisterData(0));
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_REG1,CRTC_GetRegisterData(1));
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_REG2,CRTC_GetRegisterData(2));
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_REG3,CRTC_GetRegisterData(3));
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_REG4,CRTC_GetRegisterData(4));
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_REG5,CRTC_GetRegisterData(5));
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_REG6,CRTC_GetRegisterData(6));
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_REG7,CRTC_GetRegisterData(7));
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_REG8,CRTC_GetRegisterData(8));
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_REG9,CRTC_GetRegisterData(9));
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_REG10,CRTC_GetRegisterData(10));
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_REG11,CRTC_GetRegisterData(11));
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_REG12,CRTC_GetRegisterData(12));
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_REG13,CRTC_GetRegisterData(13));
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_REG14,CRTC_GetRegisterData(14));
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_REG15,CRTC_GetRegisterData(15));

	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_HCCOUNT, CRTC_State.HorizontalCharCount);
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_VCCOUNT, CRTC_State.LineCount);

	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_HSYNCWIDTH, CRTC_State.HSyncWidth);
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_VSYNCWIDTH, CRTC_State.VSyncWidth);

	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_HSYNCSTATE, CRTC_State.Flags & CRTC_HS_FLAG);
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_VSYNCSTATE, CRTC_State.Flags & CRTC_VS_FLAG);

	SetDlgHexWord(hCRTCDialog,IDC_EDIT_CRTC_MA, CRTC_State.MA_AtLineStart);
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_RA, CRTC_State.RasterCount);
	SetDlgHexWord(hCRTCDialog,IDC_EDIT_CRTC_MEMORYADDR, CRTC_State.CurrentVideoMemoryAddress);

	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_HSYNCCOUNT, CRTC_State.HorizontalSyncWidthCount);
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_VSYNCCOUNT, CRTC_State.VerticalSyncWidthCount);

	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_ILC, CRTC_State.InterruptLineCount);
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_LAV, CRTC_State.LinesAfterVsyncTaken);
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_VADJUSTCOUNT, CRTC_State.VerticalTotalAdjustCount);
//	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_LAV, CRTC_State.LinesAfterVsyncTaken);

	SetDlgHexByte(hCRTCDialog,IDC_EDIT_WPSTATE, WestPhaser_GetState());
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_WPSTATECNT, WestPhaser_GetStateCount());


	}


}
*/
//////////////////////////////////////////////////////////////////////////////////////////

#define CPCEMU_DEBUG_SPRITE_CLASS "ARNOLD_DEBUG_SPRITE_CLASS"

void	Debugger_RenderSprite(HWND hwnd,HDC hdc, int SpriteIndex);


long FAR PASCAL SpriteWindowProc( HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT PaintStruct;
    switch(iMsg)
    {


    case WM_CREATE:
		break;

    case WM_PAINT:
        {
			BeginPaint( hwnd, &PaintStruct);

			Debugger_RenderSprite(hwnd,PaintStruct.hdc, 0);

			EndPaint( hwnd, &PaintStruct );
			return TRUE;
		}
		break;

	case WM_SIZE:
		UpdateWindow(hwnd);
		return TRUE;

    case WM_ERASEBKGND:
		return 1;

	case WM_DESTROY:
	{
	    DebuggerData.hSprite = NULL;
	}
	break;

   }

    return DefWindowProc(hwnd, iMsg, wParam, lParam);

}

void	Debugger_ShowSprite(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE);

	if (DebuggerData.hSprite==NULL)
	{
		/* create window and show it */
		DebuggerData.hSprite = CreateWindowEx(
			WS_EX_OVERLAPPEDWINDOW,
			CPCEMU_DEBUG_SPRITE_CLASS,
			Messages[49],
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_VSCROLL,
			0,
			0,
			(16*8)+32,
			32+32,
			NULL,
			NULL,
			hInstance,
			NULL );

		ShowWindow( DebuggerData.hSprite, TRUE);
		UpdateWindow( DebuggerData.hSprite );
	}
	else
	{
		/* window already shown - make it visible */
		SetWindowPos(DebuggerData.hSprite, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	}
}


void	Debugger_RegisterSpriteClass(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE);

	WNDCLASSEX	SpriteWindowClass;

	SpriteWindowClass.cbSize = sizeof(WNDCLASSEX);
	SpriteWindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	SpriteWindowClass.lpfnWndProc = SpriteWindowProc;
	SpriteWindowClass.cbClsExtra = 0;
	SpriteWindowClass.cbWndExtra = 0;
	SpriteWindowClass.hInstance = hInstance;
	SpriteWindowClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	SpriteWindowClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	SpriteWindowClass.hbrBackground = GetStockObject(GRAY_BRUSH); //NULL; //GetStockObject(COLOR_APPWORKSPACE);
	SpriteWindowClass.lpszMenuName = NULL;
	SpriteWindowClass.lpszClassName = CPCEMU_DEBUG_SPRITE_CLASS;
	SpriteWindowClass.hIconSm = LoadIcon(NULL,IDI_APPLICATION);

	if (RegisterClassEx(&SpriteWindowClass)==0)
	{
		Debug_ErrorMessage(Messages[48]);
	}
}



void	Debugger_RenderSprite(HWND hwnd,HDC hDC, int SpriteIndex)
{
char	BitmapData[16*8*32];
BITMAPINFO	*pBitmapInfo;
int		i;
//HDC	hDC;
RECT	ClientRect;
//char	*pSpriteData;
int	x,y;
int	j;
	// initialise space for header
	pBitmapInfo = (BITMAPINFO *)malloc(sizeof(BITMAPINFO) + sizeof(RGBQUAD)*256);

	if (pBitmapInfo!=NULL)
	{
		// initialise bitmap info
		pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pBitmapInfo->bmiHeader.biWidth = 16*8;
		pBitmapInfo->bmiHeader.biHeight = 16*2;
		pBitmapInfo->bmiHeader.biPlanes = 1;
		pBitmapInfo->bmiHeader.biBitCount = 8;
		pBitmapInfo->bmiHeader.biCompression = 0; //DI_RGB;
		pBitmapInfo->bmiHeader.biXPelsPerMeter = 640;
		pBitmapInfo->bmiHeader.biYPelsPerMeter = 480;
		pBitmapInfo->bmiHeader.biClrUsed = 32;
		pBitmapInfo->bmiHeader.biClrImportant = 32;
		pBitmapInfo->bmiHeader.biSizeImage = 16*16;

		pBitmapInfo->bmiColors[0].rgbReserved = 0;
		pBitmapInfo->bmiColors[0].rgbRed = 0;
		pBitmapInfo->bmiColors[0].rgbGreen = 0;
		pBitmapInfo->bmiColors[0].rgbBlue = 0;

		// setup colour palette
		for (i=1; i<16; i++)
		{
			pBitmapInfo->bmiColors[i].rgbReserved = 0;
			pBitmapInfo->bmiColors[i].rgbRed = (unsigned char)(ASIC_GetRed(i+16)<<4);
			pBitmapInfo->bmiColors[i].rgbGreen = (unsigned char)(ASIC_GetGreen(i+16)<<4);
			pBitmapInfo->bmiColors[i].rgbBlue = (unsigned char)(ASIC_GetBlue(i+16)<<4);
		}


		for (j=0; j<2; j++)
		{
			for (i=0; i<8; i++)
			{
	//			pSpriteData = ASIC_GetSpriteDataAddr((j<<3)+i);

				for (y=0; y<16; y++)
				{
					for (x=0; x<16; x++)
					{

						BitmapData[((1-j)*(pBitmapInfo->bmiHeader.biWidth*16)) + (i*16) + (((15-y)*pBitmapInfo->bmiHeader.biWidth)+x)] = ASIC_GetSpritePixel((j<<3)+i, x, y);
					}
				}
			}
		}


		GetClientRect(hwnd, &ClientRect);

	//	hDC = GetDC(hwnd);

		StretchDIBits(hDC,ClientRect.left,ClientRect.top,ClientRect.right-ClientRect.left,ClientRect.bottom-ClientRect.top,0,0,pBitmapInfo->bmiHeader.biWidth,pBitmapInfo->bmiHeader.biHeight,BitmapData,pBitmapInfo,DIB_RGB_COLORS, SRCCOPY);

	//	ReleaseDC(hwnd,hDC);

		free(pBitmapInfo);
	}
}

/****************************************************************************/

#define CPCEMU_DEBUG_SHOWGFX_CLASS "ARNOLD_DEBUG_SHOWGFX_CLASS"

typedef struct
{
	int Mode;
	int BaseAddr;
	int WidthInBytes;
	int WindowHeight;
} SHOW_GFX;

void	Debugger_RenderShowGfx(HWND hwnd,HDC,SHOW_GFX *);

HWND hShowGfx = NULL;
HWND hShowGfxToolbar = NULL;

TBBUTTON ShowGfxToolbarButtons[] =
{
    TOOLBAR_BUTTON_ENTRY(0,ID_SHOWASGFX_SET_ADDR),
    TOOLBAR_BUTTON_ENTRY(1,ID_SHOWASGFX_DECREASEWIDTH),
    TOOLBAR_BUTTON_ENTRY(2,ID_SHOWASGFX_MODE_MODE0),
    TOOLBAR_BUTTON_ENTRY(3,ID_SHOWASGFX_MODE_MODE1),
    TOOLBAR_BUTTON_ENTRY(4,ID_SHOWASGFX_MODE_MODE2),
	TOOLBAR_BUTTON_ENTRY(5,ID_SHOWASGFX_MODE_MODE3),
	TOOLBAR_BUTTON_ENTRY(6,ID_SHOWASGFX_INCREASEWIDTH),
};

#define SHOWGFX_NUM_TOOLBAR_BUTTONS (sizeof(ShowGfxToolbarButtons)/sizeof(TBBUTTON))

#define SHOWGFX_TOOLBAR_BUTTON_WIDTH 16
#define SHOWGFX_TOOLBAR_BUTTON_HEIGHT 15


#define SHOWGFX_TOOLBAR_CONTROL_IDENTIFIER	3


void	Debugger_DestroyShowGfxWindow()
{
	if (hShowGfx!=NULL)
	{
		/* destroy it */
		DestroyWindow(hShowGfx);
//		DestroyWindow(hShowGfxToolbar);
//		hShowGfxToolbar = NULL;
		hShowGfx = NULL;
	}

}


void	ShowGfx_LineUp(SHOW_GFX *pShowGfx)
{
	pShowGfx->BaseAddr-=pShowGfx->WidthInBytes;
}

void	ShowGfx_LineDown(SHOW_GFX *pShowGfx)
{
	pShowGfx->BaseAddr+=pShowGfx->WidthInBytes;
}

void	ShowGfx_PageUp(SHOW_GFX *pShowGfx)
{
	pShowGfx->BaseAddr-=(pShowGfx->WidthInBytes*pShowGfx->WindowHeight);
}

void	ShowGfx_PageDown(SHOW_GFX *pShowGfx)
{
	pShowGfx->BaseAddr+=(pShowGfx->WidthInBytes*pShowGfx->WindowHeight);
}

void	ShowGfx_CursorLeft(SHOW_GFX *pShowGfx)
{
	pShowGfx->BaseAddr--;
}

void	ShowGfx_CursorRight(SHOW_GFX *pShowGfx)
{
	pShowGfx->BaseAddr++;
}


long FAR PASCAL ShowGfxWindowProc( HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT PaintStruct;
    switch(iMsg)
    {


    case WM_CREATE:

//	    hShowGfxToolbar = CreateToolbarEx(
//		hwnd,
  //      WS_CHILD | WS_BORDER | WS_VISIBLE,	// | CCS_TOP,
    //    SHOWGFX_TOOLBAR_CONTROL_IDENTIFIER,
      //  SHOWGFX_NUM_TOOLBAR_BUTTONS,
//		(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),
  //      IDR_TOOLBAR3,
   ///     &ShowGfxToolbarButtons[0],
    //    SHOWGFX_NUM_TOOLBAR_BUTTONS,
    //    SHOWGFX_TOOLBAR_BUTTON_WIDTH,
    //    SHOWGFX_TOOLBAR_BUTTON_HEIGHT,
    //    SHOWGFX_TOOLBAR_BUTTON_WIDTH,
    //    SHOWGFX_TOOLBAR_BUTTON_HEIGHT,
    //    sizeof(TBBUTTON));

		break;


	case WM_KEYDOWN:
	{
		SHOW_GFX *pShowGfx = (SHOW_GFX *)GetWindowDataPtr(hwnd);

		int VirtualKeyCode = (int)wParam;

		switch (VirtualKeyCode)
		{
			case '0':
			{
				pShowGfx->Mode = 0;
			}
			break;

			case '1':
			{
				pShowGfx->Mode = 1;
			}
			break;

			case '2':
			{
				pShowGfx->Mode = 2;
			}
			break;

			case '3':
			{
				pShowGfx->Mode = 3;
			}
			break;


			case VK_UP:
			{
				ShowGfx_LineUp(pShowGfx);
			}
			break;

			case VK_DOWN:
			{
				ShowGfx_LineDown(pShowGfx);
			}
			break;

			case VK_LEFT:
			{
				ShowGfx_CursorLeft(pShowGfx);
			}
			break;

			case VK_RIGHT:
			{
				ShowGfx_CursorRight(pShowGfx);
			}
			break;

			case VK_PRIOR:
			{
				ShowGfx_PageUp(pShowGfx);
			}
			break;

			case VK_NEXT:
			{
				ShowGfx_PageDown(pShowGfx);
			}
			break;

			case VK_SUBTRACT:
			{
				if (pShowGfx->WidthInBytes!=1)
				{
					pShowGfx->WidthInBytes--;
				}
			}
			break;

			case VK_ADD:
			{
				pShowGfx->WidthInBytes++;
			}
			break;





			default:
				break;
		}

		ForceRedraw(hwnd);
	}
	break;

	case WM_RBUTTONDOWN:
	{
		POINT point;

		point.x = LOWORD(lParam);  // horizontal position of cursor
		point.y = HIWORD(lParam);  // vertical position of cursor

		ClientToScreen(hwnd, &point);

		// display pop-up menu
		TrackPopupMenu(GetSubMenu(DebuggerData.hShowGfxPopupMenu,0),
			TPM_LEFTALIGN | TPM_RIGHTBUTTON,
			point.x, point.y, 0, hwnd,
			NULL);
	}
	break;



	case WM_COMMAND:
	{
		SHOW_GFX *pShowGfx = (SHOW_GFX *)GetWindowDataPtr(hwnd);

		switch (LOWORD(wParam))
		{
			case ID_SHOWASGFX_SET_ADDR:
			{
				int NewAddr;

				if (HexDialog(hwnd,&NewAddr))
				{
					pShowGfx->BaseAddr = NewAddr;
				}

				ForceRedraw(hwnd);
			}
			break;

			case ID_SHOWASGFX_DECREASEWIDTH:
				{
				pShowGfx->WidthInBytes--;

				ForceRedraw(hwnd);
				}
				break;

			case ID_SHOWASGFX_INCREASEWIDTH:
			{
				pShowGfx->WidthInBytes++;

				ForceRedraw(hwnd);
			}
			break;

			case ID_SHOWASGFX_MODE_MODE0:
			{
				pShowGfx->Mode = 0;

				ForceRedraw(hwnd);
			}
			break;

			case ID_SHOWASGFX_MODE_MODE1:
			{
				pShowGfx->Mode = 1;

				ForceRedraw(hwnd);
			}
			break;

			case ID_SHOWASGFX_MODE_MODE2:
				{
				pShowGfx->Mode = 2;

				ForceRedraw(hwnd);
				}
				break;

			case ID_SHOWASGFX_MODE_MODE3:
				{
				pShowGfx->Mode = 3;

				ForceRedraw(hwnd);
				}
				break;
		}
	}
	break;

	case WM_VSCROLL:
	{
		SHOW_GFX *pShowGfx = (SHOW_GFX *)GetWindowDataPtr(hwnd);

		int ScrollCode = (int)LOWORD(wParam); // scroll bar value

		switch (ScrollCode)
		{

			case SB_TOP:
				break;

			case SB_BOTTOM:
				break;

			case SB_LINEDOWN:
				ShowGfx_LineUp(pShowGfx);
				break;

			case SB_LINEUP:
				ShowGfx_LineDown(pShowGfx);
				break;

			case SB_PAGEDOWN:
				ShowGfx_PageUp(pShowGfx);
				break;

			case SB_PAGEUP:
				ShowGfx_PageDown(pShowGfx);
				break;

			case SB_THUMBPOSITION:
				break;

			case SB_THUMBTRACK:
				break;


		}

		ForceRedraw(hwnd);
	}
	return TRUE;


    case WM_PAINT:
        {
			SHOW_GFX *pShowGfx = (SHOW_GFX *)GetWindowDataPtr(hwnd);

			if (BeginPaint( hwnd, &PaintStruct))
			{
				HFONT hFont, hOldFont;
				HDC hDC = PaintStruct.hdc;
				WINDOW_INFO WindowInfo;

				GetWindowInfoI(hwnd,&WindowInfo);

				hFont = GetStockObject(ANSI_FIXED_FONT);

				hOldFont = SelectObject(hDC, hFont);

				SetTextColor(hDC,RGB(255,255,255));

				SetBkMode(hDC, TRANSPARENT);

				Debugger_RenderShowGfx(hwnd,hDC,pShowGfx);

				{
					TCHAR Text[256];

					_stprintf(Text,_T("Addr: %04x Width: %02d"), pShowGfx->BaseAddr & 0x0ffff, pShowGfx->WidthInBytes);

					TextOut(hDC, 0, 8, Text, _tcslen(Text));
				}

				SelectObject(hDC,hOldFont);

				DeleteObject(hFont);

				EndPaint( hwnd, &PaintStruct );
			}
			return TRUE;
		}
		return TRUE;

	case WM_SIZE:
		UpdateWindow(hwnd);

		SendMessage(hShowGfxToolbar, WM_SIZE, 0,0);

		return TRUE;

	case WM_ERASEBKGND:
		return 1;

	case WM_CLOSE:
	{
		Debugger_DestroyShowGfxWindow();
	}
	break;

    case WM_DESTROY:
	{
		SHOW_GFX *pShowGfx = (SHOW_GFX *)GetWindowDataPtr(hwnd);

		free(pShowGfx);

		hShowGfx = NULL;
		//Debugger_DestroyShowGfxWindow();
		//DestroyWindow(hShowGfxToolbar);

	}
	break;

   }

    return DefWindowProc(hwnd, iMsg, wParam, lParam);

}

void	Debugger_ShowGfx(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE);

	if (hShowGfx==NULL)
	{
		SHOW_GFX *pShowGfx;

		/* create window and show it */
		hShowGfx = CreateWindowEx(
			WS_EX_OVERLAPPEDWINDOW,
			CPCEMU_DEBUG_SHOWGFX_CLASS,
			Messages[50],
			WS_OVERLAPPEDWINDOW | WS_VSCROLL,
			0,
			0,
			320,
			200,
			NULL,
			NULL,
			hInstance,
			NULL );


		pShowGfx = (SHOW_GFX *)malloc(sizeof(SHOW_GFX));
		pShowGfx->Mode = 0;
		pShowGfx->WidthInBytes = 80;
		pShowGfx->BaseAddr = 0;
		pShowGfx->WindowHeight= 200;

		SetWindowDataPtr(hShowGfx, pShowGfx);

		ShowWindow( hShowGfx, TRUE);
		UpdateWindow( hShowGfx );
	}
	else
	{
		/* window already shown - make it visible */
		SetWindowPos(hShowGfx, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	}
}


void	Debugger_RegisterShowGfxClass(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE);

	WNDCLASSEX	ShowGfxWindowClass;

	ShowGfxWindowClass.cbSize = sizeof(WNDCLASSEX);
	ShowGfxWindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	ShowGfxWindowClass.lpfnWndProc = ShowGfxWindowProc;
	ShowGfxWindowClass.cbClsExtra = 0;
	ShowGfxWindowClass.cbWndExtra = 6;
	ShowGfxWindowClass.hInstance = hInstance;
	ShowGfxWindowClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	ShowGfxWindowClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	ShowGfxWindowClass.hbrBackground = GetStockObject(GRAY_BRUSH); //NULL; //GetStockObject(COLOR_APPWORKSPACE);
	ShowGfxWindowClass.lpszMenuName = NULL;
	ShowGfxWindowClass.lpszClassName = CPCEMU_DEBUG_SHOWGFX_CLASS;
	ShowGfxWindowClass.hIconSm = LoadIcon(NULL,IDI_APPLICATION);

	if (RegisterClassEx(&ShowGfxWindowClass)==0)
	{
		Debug_ErrorMessage(Messages[51]);
	}
}


void	Debugger_RenderShowGfx(HWND hwnd, HDC hDC,SHOW_GFX *pShowGfx)
{
char	BitmapData[320*200];
BITMAPINFO	*pBitmapInfo;
int		i;
//HDC	hDC;
RECT	ClientRect;
//char	*pSpriteData;
int	x; //,y;
int	j;

	int BaseAddr;
	int				Width;
	int				ModeIndex;
	PIXEL_DATA *pPixelData;
	int				PlotWidth = 320;

	// get address we are rendering from
	BaseAddr = pShowGfx->BaseAddr;
	Width = pShowGfx->WidthInBytes;
	ModeIndex = pShowGfx->Mode;

	if ((Width<<3)>PlotWidth)
	{
		PlotWidth = 320>>3;
	}
	else
	{
		PlotWidth = Width;
	}

	pPixelData = CPC_GetModePixelData(ModeIndex);

	// initialise space for header
	pBitmapInfo = (BITMAPINFO *)malloc(sizeof(BITMAPINFO) + sizeof(RGBQUAD)*256);

	if (pBitmapInfo!=NULL)
	{
		// initialise bitmap info
		pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pBitmapInfo->bmiHeader.biWidth = 320;
		pBitmapInfo->bmiHeader.biHeight = 200;
		pBitmapInfo->bmiHeader.biPlanes = 1;
		pBitmapInfo->bmiHeader.biBitCount = 8;
		pBitmapInfo->bmiHeader.biCompression = 0; //DI_RGB;
		pBitmapInfo->bmiHeader.biXPelsPerMeter = 640;
		pBitmapInfo->bmiHeader.biYPelsPerMeter = 480;
		pBitmapInfo->bmiHeader.biClrUsed = 16;
		pBitmapInfo->bmiHeader.biClrImportant = 16;
		pBitmapInfo->bmiHeader.biSizeImage = 320*200;

		// setup colour palette to render memory graphics with
		//
		for (i=0; i<16; i++)
		{
			pBitmapInfo->bmiColors[i].rgbReserved = 0;
			pBitmapInfo->bmiColors[i].rgbRed = (unsigned char)(GateArray_GetRed(i)<<4);
			pBitmapInfo->bmiColors[i].rgbGreen = (unsigned char)(GateArray_GetGreen(i)<<4);
			pBitmapInfo->bmiColors[i].rgbBlue = (unsigned char)(GateArray_GetBlue(i)<<4);
		}

		memset(BitmapData, 0, 320*200);


		// if "sprite" method
		for (j=0; j<200; j++)
		{
			for (x=0; x<PlotWidth; x++)
			{
				int p;

				/* calc address to get graphics from */
				Z80_WORD ByteAddr = (unsigned short)(BaseAddr + x + (j*Width));
				/* get gfx byte */
				Z80_BYTE GfxByte = Z80_RD_MEM(ByteAddr);



				/* get pixel translation for this byte for selected mode */
				PIXEL_DATA 	*pBytePixelData = &pPixelData[GfxByte & 0x0ff];
				/* pointer to bitmap data to write to */
				unsigned char *pByteBitmap = (unsigned char *)(&BitmapData[(x*8)+((199-j)*320)]);

				/* write pixel index's to bitmap */
				for (p=0; p<4; p++)
				{
					pByteBitmap[p] = (unsigned char)pBytePixelData->Pixel[p];
				}
				for (p=0; p<4; p++)
				{
					pByteBitmap[p+4] = (unsigned char)pBytePixelData->Pixel[p+4];
				}

			}
		}

		GetClientRect(hwnd, &ClientRect);

		//hDC = GetDC(hwnd);

		StretchDIBits(hDC,ClientRect.left,ClientRect.top,ClientRect.right-ClientRect.left,ClientRect.bottom-ClientRect.top,0,0,pBitmapInfo->bmiHeader.biWidth,pBitmapInfo->bmiHeader.biHeight,BitmapData,pBitmapInfo,DIB_RGB_COLORS, SRCCOPY);

		//ReleaseDC(hwnd,hDC);

		free(pBitmapInfo);
	}
}

//void	Debugger_RedrawShowGfxWindow()
//{
//	if (hShowGfx!=NULL)
//	{
//		ForceRedraw(hShowGfx);
//	}
//}



/****************************************************************************/

/*
void	Debugger_OpenCRTCDialog(HWND hwnd)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
	//HWND	hwnd;

	if (hCRTCDialog==NULL)
	{
		hCRTCDialog = CreateDialog (hInstance, MAKEINTRESOURCE(IDD_DIALOG_CRTC), hwnd, CRTCDialogProc);

		ShowWindow(hCRTCDialog,TRUE);
		Debugger_UpdateCRTCDialog();
	}
	else
	{
		// window already shown - make it visible
		SetWindowPos(hCRTCDialog, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		ShowWindow(hCRTCDialog,TRUE);
	}
	//RegisterDialog(hDebuggerDialog);

}
*/

void	Debugger_Enable(HWND hParent, BOOL Status)
{
	if (Status)
	{
		Debugger_OpenDebugDialog();
	}
	else
	{
		Debugger_CloseDebugDialog();
	}

}

void	Debugger_UpdateDisplay()
{
//	Z80_REGISTERS *R;

//	// get registers
//	R = Z80_GetReg();

	// update dissassembly
	Debugger_SetDissassembleAddress(Z80_GetReg(Z80_PC));

	Debugger_UpdateDebugDialog();
	//Debugger_UpdateCRTCDialog();

	Debugger_UpdateCRTCInfo();
	Debugger_UpdateCPCInfo();
	Debugger_UpdateCPCPLUSInfo();

	//Debugger_RedrawSpriteWindow();
}


// sub-classed edit control

WNDPROC FloatEditOldProc = NULL;

//int (__stdcall *FloatEditOldProc)(void)=NULL;

long FAR PASCAL HexEditProc(HWND hWnd, WORD wMessage,WORD wParam,LONG lParam)
{

    switch (wMessage)
     {

          case WM_GETDLGCODE:
            return (DLGC_WANTALLKEYS |
                    CallWindowProc(FloatEditOldProc, hWnd, wMessage,
                                   wParam, lParam));

          case WM_CHAR:
          //Process this message to avoid message beeps.
         if ((wParam == VK_RETURN) || (wParam == VK_TAB))
           return 0;
            else
           return (CallWindowProc(FloatEditOldProc, hWnd,
                                     wMessage, wParam, lParam));

       case WM_KEYDOWN:
            if ((wParam == VK_RETURN) || (wParam == VK_TAB)) {
              PostMessage (GetParent(hWnd), WM_NEXTDLGCTL, 0, 0L);
              return FALSE;
            }

         return (CallWindowProc(FloatEditOldProc, hWnd, wMessage,
                                   wParam, lParam));
         break ;

       default:
            break;

     } /* end switch */

    return CallWindowProc(FloatEditOldProc,hWnd,wMessage,wParam,lParam);
}

/*
void	Debugger_Setup()
{

	Debug_SetState(DEBUG_STEP_INTO);

	{
		Z80_REGISTERS	*R;

		R = Z80_GetReg();

		// have a "open new mem dump window"
		// have a "open new dissassemble window"
		Debugger_OpenDebugDialog();
		Debugger_OpenMemdump(ApplicationHwnd);
		Debugger_SetMemDumpAddr(R->PC.W);
		Debugger_OpenDissassemble(ApplicationHwnd);
		Debugger_SetDissassembleAddr(R->PC.W);
	}
}
*/

#define ErrorMessage(ErrorText) \
	MessageBox(GetActiveWindow(),ErrorText,Messages[52],MB_OK)

void	DEBUG_DisplayError(TCHAR *pMessage)
{
		ErrorMessage(pMessage);
}

//#include <commdlg.h>


HWND hASICDialog = NULL;

void	Debugger_DestroyASICDialog()
{
	if (hASICDialog!=NULL)
	{
		DestroyWindow(hASICDialog);
		hASICDialog = NULL;
	}
}

#if 0
BOOL CALLBACK  ASICDialogProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

    switch (iMsg)
    {

        case WM_CREATE:
			{
				HWND hListView;

				hListView = GetDlgItem(hwnd, IDC_LIST1);

				MyListView_AddColumn(hListView, Messages[53], 0);
				MyListView_AddColumn(hListView,_T("X"), 1);
				MyListView_AddColumn(hListView,_T("Y"), 2);
				MyListView_AddColumn(hListView,_T("X Mag"), 3);
				MyListView_AddColumn(hListView,_T("Y Mag"), 4);



			}

			return TRUE;


		case WM_CLOSE:
			Debugger_DestroyASICDialog();
			break;	//return TRUE;

		case WM_DESTROY:
//			hASICDialog = NULL;
			//DestroyWindow(hwnd);
			break; //return TRUE;

		default:
			break;

	}
    return FALSE;
}


void	ShowASICDialog(HWND hwnd)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
	//HWND	hwnd;

	if (hASICDialog==NULL)
	{
		hASICDialog = CreateDialog (hInstance, MAKEINTRESOURCE(IDD_DIALOG_PLUS_HARDWARE), hwnd, ASICDialogProc);

		ShowWindow(hASICDialog,TRUE);
	}
}
#endif
#if 0
HWND hDebugHooksDialog = NULL;

//#include "gdebug.c"

void	Debugger_UpdateDebugHooksDialog(void)
{
	if (hDebugHooksDialog)
	{
#ifdef DEBUG_HOOKS
		SetCheckButtonState(hDebugHooksDialog, IDC_CHECK_MEMORY_READ, DebugHooks_ReadMemory_GetActiveState());
		SetCheckButtonState(hDebugHooksDialog, IDC_CHECK_MEMORY_WRITE, DebugHooks_WriteMemory_GetActiveState());
		SetCheckButtonState(hDebugHooksDialog, IDC_CHECK_IO_READ, DebugHooks_ReadIO_GetActiveState());
		SetCheckButtonState(hDebugHooksDialog, IDC_CHECK_IO_WRITE, DebugHooks_WriteIO_GetActiveState());
#endif
	}
}

BOOL CALLBACK  ConditionSetDialogProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
		case WM_CREATE:
			break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDOK:
				{
					EndDialog(hwnd, 1);
				}
				return TRUE;

				case IDCANCEL:
				{
					EndDialog(hwnd, 0);
				}
				return TRUE;
			}
		}

		case WM_CLOSE:
			break;

		case WM_DESTROY:
			break;
	}

	return FALSE;
}




void	OpenConditionSetDialog(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE);

	DialogBox(hInstance,
			MAKEINTRESOURCE(IDD_DIALOG_DEBUG_CONDITION),
			hParent,
			ConditionSetDialogProc);
}

BOOL CALLBACK  DebugHooksConditionsDialogProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

    switch (iMsg)
    {

        case WM_CREATE:
            return TRUE;

        case WM_COMMAND:
        {
			case IDC_BUTTON_ADD:
			{
				OpenConditionSetDialog(hwnd);
			}
			break;
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





HWND	OpenConditionListDialog(HWND hParent, TCHAR *DialogTitleText)
{
	HWND hDialog;

	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE);

	hDialog = CreateDialog (hInstance,
		MAKEINTRESOURCE(IDD_DIALOG_CONDITIONS), 0,
		DebugHooksConditionsDialogProc);

	if (hDialog!=NULL)
	{
		ShowWindow(hDialog, TRUE);
		SetWindowText(hDialog, DialogTitleText);
	}

	return hDialog;
}


BOOL CALLBACK  DebugHooksDialogProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

    switch (iMsg)
    {

        case WM_CREATE:
            return TRUE;

        case WM_COMMAND:
        {
			if (HIWORD(wParam)==BN_CLICKED)
			{
				/* check box clicked */

				switch (LOWORD(wParam))
				{
#ifdef DEBUG_HOOKS
				case IDC_CHECK_MEMORY_READ:
					{
						/* get check state */
						int CheckState;

						CheckState = SendDlgItemMessage(hwnd,LOWORD(wParam), BM_GETCHECK,0,0);

						if (CheckState == BST_CHECKED)
						{
							/* checked */
							DebugHooks_ReadMemory_Active(TRUE);
							Debug_ReadMemory_Comparison_Enable(TRUE);
						}
						else
						{
							/* not checked */
							DebugHooks_ReadMemory_Active(FALSE);
							Debug_ReadMemory_Comparison_Enable(FALSE);
						}
					}
					break;


					case IDC_CHECK_MEMORY_WRITE:
					{
						/* get check state */
						int CheckState;

						CheckState = SendDlgItemMessage(hwnd,LOWORD(wParam), BM_GETCHECK,0,0);

						if (CheckState == BST_CHECKED)
						{
							/* checked */
							DebugHooks_WriteMemory_Active(TRUE);
							Debug_WriteMemory_Comparison_Enable(TRUE);
						}
						else
						{
							/* not checked */
							DebugHooks_WriteMemory_Active(FALSE);
							Debug_WriteMemory_Comparison_Enable(FALSE);
						}
					}
					break;

					case IDC_CHECK_IO_READ:
					{
						/* get check state */
						int CheckState;

						CheckState = SendDlgItemMessage(hwnd,LOWORD(wParam), BM_GETCHECK,0,0);

						if (CheckState == BST_CHECKED)
						{
							/* checked */
							DebugHooks_ReadIO_Active(TRUE);
							Debug_ReadIO_Comparison_Enable(TRUE);
						}
						else
						{
							/* not checked */
							DebugHooks_ReadIO_Active(FALSE);
							Debug_ReadIO_Comparison_Enable(FALSE);
						}
					}
					break;

					case IDC_CHECK_IO_WRITE:
					{
						/* get check state */
						int CheckState;

						CheckState = SendDlgItemMessage(hwnd,LOWORD(wParam), BM_GETCHECK,0,0);

						if (CheckState == BST_CHECKED)
						{
							/* checked */
							DebugHooks_WriteIO_Active(TRUE);
							Debug_WriteIO_Comparison_Enable(TRUE);
						}
						else
						{
							/* not checked */
							DebugHooks_WriteIO_Active(FALSE);
							Debug_WriteIO_Comparison_Enable(FALSE);
						}

					}
					break;
#endif

					case IDC_BUTTON_MEMORY_READ:
					{
						OpenConditionListDialog(hwnd,
								Messages[54]);
					}
					return TRUE;

					case IDC_BUTTON_MEMORY_WRITE:
					{
						OpenConditionListDialog(hwnd,
								Messages[55]);
					}
					return TRUE;

					case IDC_BUTTON_IO_READ:
					{
						OpenConditionListDialog(hwnd,
								Messages[56]);
					}
					return TRUE;

					case IDC_BUTTON_IO_WRITE:
					{
						OpenConditionListDialog(hwnd,
								Messages[57]);
					}
					return TRUE;
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

void	Debugger_DebugHooks(HWND hwnd)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

	hDebugHooksDialog = CreateDialog (hInstance, MAKEINTRESOURCE(IDD_DIALOG_DEBUG_HOOKS), 0, DebugHooksDialogProc);

	if (hDebugHooksDialog!=NULL)
	{
		ShowWindow(hDebugHooksDialog,TRUE);

		Debugger_UpdateDebugHooksDialog();
	}
}
#endif
/******************** RENDERING STUFF TO WINDOW **************************/


static int RenderFontHeight;
static int RenderFontWidth;
static HDC RenderDC;
static 	HFONT hFont,hOldFont;
static  HPEN hPen, hOldPen;

POINT	TextCoordsToPixelCoords(int X, int Y)
{
	POINT	PixelCoords;

	PixelCoords.x = X*RenderFontWidth;
	PixelCoords.y = Y*RenderFontHeight;

	return PixelCoords;
}

void	PrintString(int X, int Y, TCHAR *pString, int nMax)
{
	int Count;

	POINT PlotPoint;

	Count = _tcslen(pString);

	if (Count>nMax)
	{
		Count = nMax;
	}

	PlotPoint = TextCoordsToPixelCoords(X,Y);

	TextOut(RenderDC, PlotPoint.x,
		PlotPoint.y, pString,Count);
}

static TCHAR DisplayString[256];

void	TextWindow_Ready(HWND hwnd, PAINTSTRUCT *pPaintStruct)
{
	RECT WindowRect;
	WINDOW_INFO WindowInfo;

	RenderDC = BeginPaint( hwnd, pPaintStruct);

	GetWindowInfoI(hwnd, &WindowInfo);


	hFont = GetStockObject(ANSI_FIXED_FONT);
	hPen = GetStockObject(BLACK_BRUSH);

	hOldFont = SelectObject(RenderDC, hFont);
	hOldPen = SelectObject(RenderDC, hPen);

	//GetTextMetrics(RenderDC,&FontMetric);

	RenderFontWidth = WindowInfo.FontWidth;	//.tmMaxCharWidth;
	RenderFontHeight = WindowInfo.FontHeight;	//FontMetric.tmHeight;

	GetClientRect(hwnd,&WindowRect);

	FillRect(RenderDC,&WindowRect,GetStockObject(WHITE_BRUSH));

	SetBkMode(RenderDC, TRANSPARENT);

	// debug display
	{
		int X, Y;

		RECT CursorRect;

		DebugItem__GetCursorPos(&X, &Y);

		CursorRect.left = X*WindowInfo.FontWidth;
		CursorRect.top = Y*WindowInfo.FontHeight;
		CursorRect.right = CursorRect.left+WindowInfo.FontWidth;
		CursorRect.bottom = CursorRect.top + WindowInfo.FontHeight;

		FillRect(RenderDC, &CursorRect, GetStockObject(GRAY_BRUSH));
	}

}

void	TextWindow_Finish(HWND hwnd, PAINTSTRUCT *pPaintStruct)
{
	SelectObject(RenderDC,hOldFont);
	SelectObject(RenderDC,hOldPen);

	DeleteObject(hFont);

	DeleteObject(hPen);

	EndPaint(hwnd, pPaintStruct);
}


/******************** CPC INFO WINDOW ************************************/

void	CPC_Info_Display()
{
	int X,Y;
	int i;





	DebugItem__Finish();

	DebugItem__Initialise();

	/* Dump PSG Registers */

	{
		X=0;
		Y=0;

		_stprintf(DisplayString,_T("PSG"));

		PrintString(X,Y, DisplayString,10);

		for (i=0; i<16; i++)
		{
			_stprintf(DisplayString,_T("%1x: %02x"),i, PSG_GetRegisterData(i));

			PrintString(X, Y+2+i, DisplayString,10);

			DebugItem__AddNewItem(X+3,Y+2+i,2);


		}
	}

	/* GATE ARRAY PENS */
	{
		X = 10;
		Y = 0;

		_stprintf(DisplayString,Messages[58]);

		PrintString(X, Y, DisplayString,10);

		for (i=0; i<17; i++)
		{
			if (i!=16)
			{
				_stprintf(DisplayString,_T("%1x: %02x"),i, GateArray_GetPaletteColour(i));
			}
			else
			{
				_stprintf(DisplayString,_T("B: %02x"),GateArray_GetPaletteColour(i));
			}

			PrintString(X, Y+2+i, DisplayString,10);

			DebugItem__AddNewItem(X+3,Y+2+i,2);

		}
	}

	/* GATE ARRAY SETTINGS */
	{
		X = 20;
		Y = 0;

		_stprintf(DisplayString,_T("GA"));
		PrintString(X,Y, DisplayString,10);


		_stprintf(DisplayString,_T("0x000: %02x PPR"), GateArray_GetSelectedPen());
		PrintString(X,Y+2, DisplayString,10);

		DebugItem__AddNewItem(X+7,Y+2,2);


		//_stprintf(DisplayString,_T("0x040: %02x PMEM"), GateArray_GetSelectedPen());
		//PrintString(X,Y+3, DisplayString);

		_stprintf(DisplayString,_T("0x080: %02x MRER"), GateArray_GetMultiConfiguration());
		PrintString(X,Y+4, DisplayString,10);

		DebugItem__AddNewItem(X+7,Y+4,2);


		_stprintf(DisplayString,_T("RAM: %02x RAM"), PAL_GetRamConfiguration());
		PrintString(X,Y+5, DisplayString,10);

		DebugItem__AddNewItem(X+7,Y+5,2);

		_stprintf(DisplayString,_T("ILC  : %02x"), GateArray_GetInterruptLineCount());
		PrintString(X,Y+6, DisplayString, 9);

		DebugItem__AddNewItem(X+7, Y+6,2);

		_stprintf(DisplayString,_T("VSS  : %02x"), GateArray_GetVsyncSynchronisationCount());
		PrintString(X,Y+7, DisplayString, 9);

		DebugItem__AddNewItem(X+7, Y+7,2);


	}
}

/******************** CPC PLUS WINDOW ************************************/

#define DMA_OPCODES_SHOWN	4

void	CPCPLUS_Info_Display()
{
	int X,Y;
	int i;

	DebugItem__Finish();

	DebugItem__Initialise();




	/* Dump DMA Registers */
	{


		{

			int dma_channel;

			X = 0;
			Y = 0;

			for (dma_channel=0; dma_channel<3; dma_channel++)
			{

				_stprintf(DisplayString,_T("DMA%1x"), dma_channel);
				PrintString(X, Y, DisplayString, 20);
				Y+=2;

				_stprintf(DisplayString,Messages[59], ASIC_DMA_GetChannelAddr(dma_channel));
				PrintString(X, Y, DisplayString, 20);
				DebugItem__AddNewItem(X+6,Y,4);
				Y++;
				_stprintf(DisplayString,Messages[60], ASIC_DMA_GetChannelPrescale(dma_channel));
				PrintString(X, Y, DisplayString, 20);
				DebugItem__AddNewItem(X+10,Y,2);
				Y++;
				Y++;
			}
		}
	}

	/* colours */
	{
		X = 20;
		Y = 0;

		_stprintf(DisplayString,Messages[61]);
		PrintString(X, Y, DisplayString,20);

		for (i=0; i<16; i++)
		{
			_stprintf(DisplayString,_T("%02d: %01x%01x%01x  %02d: %01x%01x%01x"),
				i, ASIC_GetRed(i), ASIC_GetGreen(i), ASIC_GetBlue(i),
				(i+16), ASIC_GetRed(i+16),ASIC_GetGreen(i+16), ASIC_GetBlue(i+16));

			PrintString(X, Y+2+i, DisplayString,40-20);

			DebugItem__AddNewItem(X+4,Y+2+i,3);
			DebugItem__AddNewItem(X+13,Y+2+i,3);

		}
	}

	/* regs */
	{

		X = 40;
		Y = 0;

		_stprintf(DisplayString,_T("PRI:    %02x"),ASIC_GetPRI());
		PrintString(X,Y,DisplayString,10);

		DebugItem__AddNewItem(X+8,Y,2);

		_stprintf(DisplayString,_T("SPLT:   %02x"),ASIC_GetSPLT());
		PrintString(X,Y+1,DisplayString,10);

		DebugItem__AddNewItem(X+8,Y+1,2);


		_stprintf(DisplayString,_T("SSA:  %04x"),ASIC_GetSSA());
		PrintString(X,Y+2,DisplayString,10);

		DebugItem__AddNewItem(X+6,Y+2,4);


		_stprintf(DisplayString,_T("SSCR:   %02x"),ASIC_GetSSCR());
		PrintString(X,Y+3,DisplayString,10);

		DebugItem__AddNewItem(X+8,Y+3,2);


		_stprintf(DisplayString,_T("IVR:    %02x"),ASIC_GetIVR());
		PrintString(X,Y+4, DisplayString,10);

		DebugItem__AddNewItem(X+8,Y+4,2);

	}

	{
		X = 60;
		Y = 0;
		if (ASIC_GetUnLockState())
		{
			PrintString(X, Y, Messages[62],8);
		}
		else
		{
			PrintString(X, Y, Messages[63],8);
		}

		_stprintf(DisplayString,_T("RMR2:   %02x"),ASIC_GetSecondaryRomMapping());
		PrintString(X,Y+1,DisplayString,10);
		DebugItem__AddNewItem(X+8,Y+1,2);
	}

	{
		X = 40;
		Y = 15;

		for (i=0; i<16; i++)
		{
			_stprintf(DisplayString,_T("X  : %04x"),ASIC_GetSpriteX(i));
			PrintString(X,Y,DisplayString,10);
			DebugItem__AddNewItem(X+5,Y,4);

			_stprintf(DisplayString,_T("Y  : %04x"),ASIC_GetSpriteY(i));
			PrintString(X+10,Y,DisplayString,10);
			DebugItem__AddNewItem(X+5+10,Y,4);

			_stprintf(DisplayString,_T("Mag: %02x"),ASIC_GetSpriteMagnification(i));
			PrintString(X+20,Y,DisplayString,10);
			DebugItem__AddNewItem(X+5+20,Y,2);
			Y++;
		}
	}
}


/************************** CRTC INFO ***********************************/


void	CRTC_Info_Display()
{
	int X,Y;
	int i;

	DebugItem__Finish();

	DebugItem__Initialise();


	/* register data */
	{
		X = 0;
		Y = 0;

		for (i=0; i<18; i++)
		{
			_stprintf(DisplayString,_T("R%02d: %02x"), i, CRTC_GetRegisterData(i));
			PrintString(X,Y+i, DisplayString,10);

			DebugItem__AddNewItem(X+5,Y+i,3);

		}
	}

	{
		CRTC_INTERNAL_STATE *pCRTC_State;

		X = 10;
		Y = 0;

		pCRTC_State = CRTC_GetInternalState();

		_stprintf(DisplayString,_T("HC: %02x"), pCRTC_State->HCount);
		PrintString(X,Y, DisplayString,10);
		DebugItem__AddNewItem(X+4,Y,2);

		Y++;


		_stprintf(DisplayString,_T("LC: %02x"), pCRTC_State->LineCounter);
		PrintString(X,Y, DisplayString,10);
		DebugItem__AddNewItem(X+4,Y,2);
		Y++;

		_stprintf(DisplayString,_T("RC: %02x"), pCRTC_State->RasterCounter);
		PrintString(X,Y, DisplayString,10);
		DebugItem__AddNewItem(X+4,Y,2);
		Y++;

		_stprintf(DisplayString,_T("HS-WIDTH: %02x"), pCRTC_State->HorizontalSyncWidth);
		PrintString(X,Y, DisplayString,10);
		DebugItem__AddNewItem(X+10,Y,2);
		Y++;

		_stprintf(DisplayString,_T("VS-WIDTH: %02x"), pCRTC_State->VerticalSyncWidth);
		PrintString(X,Y, DisplayString,10);
		DebugItem__AddNewItem(X+10,Y,2);
		Y++;

		_stprintf(DisplayString,_T("HS-C: %02x"), pCRTC_State->HorizontalSyncCount);
		PrintString(X,Y, DisplayString,10);
		DebugItem__AddNewItem(X+6,Y,2);
		Y++;

		_stprintf(DisplayString,_T("VS-C: %02x"), pCRTC_State->VerticalSyncCount);
		PrintString(X,Y, DisplayString,10);
		DebugItem__AddNewItem(X+6,Y,2);
		Y++;
//		_stprintf(DisplayString,_T("ILC: %02x"),pCRTC_State->GA_State.InterruptLineCount);
//		PrintString(X,Y, DisplayString);
//		Y++;
		//_stprintf(DisplayString,_T("LAV: %0d"),CRTC_State.MonitorState.LinesAfterVsync);
		//PrintString(X,Y, DisplayString);
/*
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_HSYNCSTATE, CRTC_State.Flags & CRTC_HS_FLAG);
	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_VSYNCSTATE, CRTC_State.Flags & CRTC_VS_FLAG);

	SetDlgHexWord(hCRTCDialog,IDC_EDIT_CRTC_MA, CRTC_State.MA_AtLineStart);
	SetDlgHexWord(hCRTCDialog,IDC_EDIT_CRTC_MEMORYADDR, CRTC_State.CurrentVideoMemoryAddress);


	SetDlgHexByte(hCRTCDialog,IDC_EDIT_CRTC_VADJUSTCOUNT, CRTC_State.VerticalTotalAdjustCount);
*/
	}

}


long FAR PASCAL CPC_Info_WindowProc( HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT PaintStruct;

    switch(iMsg)
    {
	    case WM_CREATE:
			break;

		case WM_PAINT:
        {
			TextWindow_Ready(hwnd,&PaintStruct);

			CPC_Info_Display();

			TextWindow_Finish(hwnd, &PaintStruct);

			return TRUE;
		}

	case WM_ERASEBKGND:
		return 1;


	case WM_LBUTTONDOWN:
	{
		int xPos, yPos;
		int CharX, CharY;
		//MEMDUMP_WINDOW *pMemdumpWindow = (MEMDUMP_WINDOW *)GetWindowDataPtr(hwnd);

		WINDOW_INFO WindowInfo;

		GetWindowInfoI(hwnd, &WindowInfo);

		xPos = LOWORD(lParam);  // horizontal position of cursor
		yPos = HIWORD(lParam);  // vertical position of cursor

		CharX = xPos/WindowInfo.FontWidth;
		CharY = yPos/WindowInfo.FontHeight;

		//MemDump_SelectByCharXY(pMemdumpWindow, CharX,CharY);

		DebugItem__SetCursorFromXY(CharX, CharY);


		ForceRedraw(hwnd);
	}
	break;

//		DebugItem__SetCursorFromXY(int X, int Y)


	case WM_SIZE:
		return TRUE;

	case WM_CLOSE:
		break;

    case WM_DESTROY:
        hCPCInfo = NULL;
		break;
   }

    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

long FAR PASCAL CPCPLUS_Info_WindowProc( HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT PaintStruct;

    switch(iMsg)
    {
	    case WM_CREATE:
			break;

		case WM_PAINT:
        {
			TextWindow_Ready(hwnd,&PaintStruct);

			CPCPLUS_Info_Display();

			TextWindow_Finish(hwnd, &PaintStruct);

			return TRUE;
		}

	case WM_ERASEBKGND:
		return 1;

	case WM_LBUTTONDOWN:
	{
		int xPos, yPos;
		int CharX, CharY;
		//MEMDUMP_WINDOW *pMemdumpWindow = (MEMDUMP_WINDOW *)GetWindowDataPtr(hwnd);

		WINDOW_INFO WindowInfo;

		GetWindowInfoI(hwnd, &WindowInfo);

		xPos = LOWORD(lParam);  // horizontal position of cursor
		yPos = HIWORD(lParam);  // vertical position of cursor

		CharX = xPos/WindowInfo.FontWidth;
		CharY = yPos/WindowInfo.FontHeight;

		//MemDump_SelectByCharXY(pMemdumpWindow, CharX,CharY);

		DebugItem__SetCursorFromXY(CharX, CharY);


		ForceRedraw(hwnd);
	}
	break;

	case WM_SIZE:
		return TRUE;

	case WM_CLOSE:
		break;

    case WM_DESTROY:
		hCPCPLUSInfo = NULL;
		break;
   }

    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}


long FAR PASCAL CRTC_Info_WindowProc( HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT PaintStruct;

    switch(iMsg)
    {
	    case WM_CREATE:
			break;

		case WM_PAINT:
        {
			TextWindow_Ready(hwnd,&PaintStruct);

			CRTC_Info_Display();

			TextWindow_Finish(hwnd, &PaintStruct);

			return TRUE;
		}

	case WM_LBUTTONDOWN:
	{
		int xPos, yPos;
		int CharX, CharY;
		//MEMDUMP_WINDOW *pMemdumpWindow = (MEMDUMP_WINDOW *)GetWindowDataPtr(hwnd);

		WINDOW_INFO WindowInfo;

		GetWindowInfoI(hwnd, &WindowInfo);

		xPos = LOWORD(lParam);  // horizontal position of cursor
		yPos = HIWORD(lParam);  // vertical position of cursor

		CharX = xPos/WindowInfo.FontWidth;
		CharY = yPos/WindowInfo.FontHeight;

		//MemDump_SelectByCharXY(pMemdumpWindow, CharX,CharY);

		DebugItem__SetCursorFromXY(CharX, CharY);


		ForceRedraw(hwnd);
	}
	break;

	case WM_ERASEBKGND:
		return 1;


	case WM_SIZE:
		return TRUE;

	case WM_CLOSE:
		break;

    case WM_DESTROY:
        hCRTCInfo = NULL;
		break;
   }

    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}




void	Debugger_UpdateCPCInfo(void)
{
	if (hCPCInfo!=NULL)
	{
		ForceRedraw(hCPCInfo);
	}
}



void	Debugger_RegisterCPCInfoClass(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE);

	WNDCLASSEX	DebugWindowClass;

	DebugWindowClass.cbSize = sizeof(WNDCLASSEX);
	DebugWindowClass.style = CS_HREDRAW | CS_VREDRAW;	// | CS_OWNDC;
	DebugWindowClass.lpfnWndProc = CPC_Info_WindowProc;
	DebugWindowClass.cbClsExtra = 0;
	DebugWindowClass.cbWndExtra = 0;
	DebugWindowClass.hInstance = hInstance;
	DebugWindowClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	DebugWindowClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	DebugWindowClass.hbrBackground = GetStockObject(GRAY_BRUSH); //NULL; //GetStockObject(COLOR_APPWORKSPACE);
	DebugWindowClass.lpszMenuName = NULL;
	DebugWindowClass.lpszClassName = _T(CPCEMU_DEBUG_CPCINFO_CLASS);
	DebugWindowClass.hIconSm = LoadIcon(NULL,IDI_APPLICATION);

	if (RegisterClassEx(&DebugWindowClass)==0)
	{
		Debug_ErrorMessage(Messages[64]);
	}
}


void Debugger_OpenCPCInfo(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE);

	if (hCPCInfo==NULL)
	{
		hCPCInfo = CreateWindowEx(
			WS_EX_OVERLAPPEDWINDOW,
			CPCEMU_DEBUG_CPCINFO_CLASS,
			_T("CPC Info"),
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			NULL,
			NULL,
			hInstance,
			NULL );

		ShowWindow( hCPCInfo, TRUE);
		UpdateWindow( hCPCInfo);
	}
	else
	{
		/* window already shown - make it visible */
		SetWindowPos(hCPCInfo, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	}
}



void	Debugger_UpdateCPCPLUSInfo(void)
{
	if (hCPCPLUSInfo!=NULL)
	{
		ForceRedraw(hCPCPLUSInfo);
	}
}


void	Debugger_RegisterCPCPLUSInfoClass(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE);

	WNDCLASSEX	DebugWindowClass;

	DebugWindowClass.cbSize = sizeof(WNDCLASSEX);
	DebugWindowClass.style = CS_HREDRAW | CS_VREDRAW;	// | CS_OWNDC;
	DebugWindowClass.lpfnWndProc = CPCPLUS_Info_WindowProc;
	DebugWindowClass.cbClsExtra = 0;
	DebugWindowClass.cbWndExtra = 0;
	DebugWindowClass.hInstance = hInstance;
	DebugWindowClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	DebugWindowClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	DebugWindowClass.hbrBackground = GetStockObject(GRAY_BRUSH); //NULL; //GetStockObject(COLOR_APPWORKSPACE);
	DebugWindowClass.lpszMenuName = NULL;
	DebugWindowClass.lpszClassName = _T(CPCEMU_DEBUG_CPCPLUSINFO_CLASS);
	DebugWindowClass.hIconSm = LoadIcon(NULL,IDI_APPLICATION);

	if (RegisterClassEx(&DebugWindowClass)==0)
	{
		Debug_ErrorMessage(Messages[64]);
	}
}

void Debugger_OpenCPCPlusInfo(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE);

	if (hCPCPLUSInfo==NULL)
	{
		hCPCPLUSInfo = CreateWindowEx(
			WS_EX_OVERLAPPEDWINDOW,
			CPCEMU_DEBUG_CPCPLUSINFO_CLASS,
			_T("CPC PLUS Info"),
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			NULL,
			NULL,
			hInstance,
			NULL );

		ShowWindow( hCPCPLUSInfo, TRUE);
		UpdateWindow( hCPCPLUSInfo);
	}
	else
	{
		/* window already shown - make it visible */
		SetWindowPos(hCPCInfo, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	}
}





void	Debugger_UpdateCRTCInfo(void)
{
	if (hCRTCInfo!=NULL)
	{
		ForceRedraw(hCRTCInfo);
	}
}

void	Debugger_RegisterCRTCInfoClass(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE);

	WNDCLASSEX	DebugWindowClass;

	DebugWindowClass.cbSize = sizeof(WNDCLASSEX);
	DebugWindowClass.style = CS_HREDRAW | CS_VREDRAW;	// | CS_OWNDC;
	DebugWindowClass.lpfnWndProc = CRTC_Info_WindowProc;
	DebugWindowClass.cbClsExtra = 0;
	DebugWindowClass.cbWndExtra = 0;
	DebugWindowClass.hInstance = hInstance;
	DebugWindowClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	DebugWindowClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	DebugWindowClass.hbrBackground = GetStockObject(GRAY_BRUSH); //NULL; //GetStockObject(COLOR_APPWORKSPACE);
	DebugWindowClass.lpszMenuName = NULL;
	DebugWindowClass.lpszClassName = _T(CPCEMU_DEBUG_CRTCINFO_CLASS);
	DebugWindowClass.hIconSm = LoadIcon(NULL,IDI_APPLICATION);

	if (RegisterClassEx(&DebugWindowClass)==0)
	{
		Debug_ErrorMessage(Messages[64]);
	}
}


void Debugger_OpenCRTCInfo(HWND hParent)
{
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE);

	if (hCRTCInfo==NULL)
	{
		hCRTCInfo = CreateWindowEx(
			WS_EX_OVERLAPPEDWINDOW,
			CPCEMU_DEBUG_CRTCINFO_CLASS,
			_T("CRTC Info"),
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			NULL,
			NULL,
			hInstance,
			NULL );

		ShowWindow( hCRTCInfo, TRUE);
		UpdateWindow( hCRTCInfo);
	}
	else
	{
		/* window already shown - make it visible */
		SetWindowPos(hCRTCInfo, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	}
}
