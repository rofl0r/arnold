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
#include <direct.h>
// for microsoft wheel mouse/intellimouse
#include <zmouse.h>
// for drag and drop
#include <shellapi.h>

//#define CRTDEBUG
#define DRAGANDDROP

#include "cpcemu.h"
#include "../cpc/debugger/gdebug.h"
#include "joy.h"
#include "ziphandle.h"
#include "commctrl.h"
#include "detectos.h"
#include "myapp.h"
#include "../cpc/cpc.h"
//#include <crtdbg.h>
#include "win.h"
#include "debugger.h"

int	WindowWidth;
int WindowHeight;
RECT WindowRectBeforeMinimize;
BOOL Minimized = FALSE;
// for drag and drop

//#include "myz80/z80.h"

#include "directx\di.h"
#include "directx\dd.h"
#include "directx\ds.h"

#define APPLICATION_CLASS_NAME "ArnoldEmu"
#define APP_ERROR_TEXT	"Arnold Emulator Error"

BOOL ApplicationIsActive;
BOOL	ApplicationHasFocus;

HINSTANCE hCommonControls;
HWND ApplicationHwnd;
static HINSTANCE AppInstance;

//int	MousePosX, MousePosY, MouseDeltaX, MouseDeltaY;
int xPos, yPos,Buttons;

#define IDSTATUSWINDOW			1

BOOL	WindowedMode = TRUE;
int ScreenResX;
int ScreenResY;

BOOL	DoNotScanKeyboard = TRUE;

void	MyApp_SetFullScreen(int Width, int Height)
{
	WindowedMode = FALSE;
	ScreenResX = Width;
	ScreenResY = Height;
}

void	MyApp_SetWindowed(int Width, int Height)
{
	WindowedMode = TRUE;
}


void	CPCEMU_DetectFileAndLoad(char *pFilename);
	

extern	char CurrentWorkingDirectory[MAX_PATH];
void	ErrorMessage(char *ErrorText)
{
	MessageBox(GetActiveWindow(),ErrorText,APP_ERROR_TEXT,MB_OK);
}

#include "general/lnklist/lnklist.h"


LIST_HEADER *pDialogList;

void	DialogList_Initialise()
{
	LinkList_InitialiseList(&pDialogList);
}

void	DialogList_Finish()
{
	LinkList_DeleteList(&pDialogList,NULL);
}

void	DialogList_AddDialog(HWND hDialog)
{
	LinkList_AddItemToListEnd(pDialogList, hDialog);
}

int		DialogList_RemoveCallback(HWND hThisDialog, HWND hDialogWanted)
{	
	if (hThisDialog == hDialogWanted)
		return 0;

	return -1;
}


void	DialogList_RemoveDialog(HWND hDialog)
{
	LIST_NODE *pDialogListNode;

	pDialogListNode = LinkList_SearchListForwards(pDialogList, hDialog, DialogList_RemoveCallback);

	if (pDialogListNode!=NULL)
	{
		LinkList_DeleteItem(pDialogList, pDialogListNode,NULL);
	}

}

int	DialogList_IsDialogMessageCallback(MSG *pMsg, HWND hThisDialog)
{
	if (IsDialogMessage(hThisDialog, pMsg))
		return 0;

	return -1;
}


BOOL	DialogList_IsDialogMessage(MSG *msg)
{
	LIST_NODE *pDialogListNode = NULL;

	pDialogListNode = LinkList_SearchListForwards(pDialogList, msg, DialogList_IsDialogMessageCallback);
	
	if (pDialogListNode!=NULL)
		return TRUE;

	return FALSE;
}


/*----------------------------------------------------------------------*/

BOOL	MyApp_RegisterClass(WNDCLASSEX *pClass)
{
	// attempt to register the class
	if (RegisterClassEx(pClass)==0)
	{
		// register failed
		return FALSE;
	}
	
	// register succeeded
	return TRUE;
}

HWND hwndStatus;


/*----------------------------------------------------------------------*/

long FAR PASCAL WindowProc( HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT PaintStruct;

    switch(iMsg)
    {
	case WM_CREATE:
//			hwndStatus = CreateStatusWindow(WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP, "", hwnd, 101);

		return 0;

	case WM_ACTIVATE:
	{
		if (LOWORD(wParam)!=WA_INACTIVE)
		{
			// active 

			if (HIWORD(wParam)==0)
			{
				// maximized
				DI_AcquireDevices();
				DD_RestoreSurfaces();
				DD_ClearBothSurfaces();
				DoNotScanKeyboard = FALSE;
					
				return TRUE;
			}
		}

		CPC_ReleaseKeys();

		// maximized
		DI_UnAcquireDevices();
		DoNotScanKeyboard = TRUE;
		
		// restore initial state of scroll lock
		ScrollLock_RestoreState();
	
	}
	return 0;


	case WM_ACTIVATEAPP:
	{
		BOOL	ActiveState = (BOOL)wParam;
	
		if (ActiveState == TRUE)
		{
			DI_AcquireDevices();
			DD_RestoreSurfaces();
			DD_ClearBothSurfaces();
			DoNotScanKeyboard = FALSE;
			Minimized = FALSE;
			GetWindowRect(hwnd,&WindowRectBeforeMinimize);
		}
		else
		{

			DI_UnAcquireDevices();
			DS_ClearBuffer();
			CPC_ReleaseKeys();
			DoNotScanKeyboard = TRUE;
			Minimized = TRUE;

			// restore state of scrolllock
			ScrollLock_RestoreState();
		
		}

		
		ApplicationIsActive = ActiveState;

	}
	return 0;
///	case WM_NOTIFY:
//	{
//		int ControlID = (int)wParam;
//		NMHDR *pNotifyHeader = (NMHDR *)lParam;
//
//	}
//	break;

//	case WM_NCLBUTTONDOWN:
//	case WM_NCMBUTTONDOWN:
//	case WM_NCRBUTTONDOWN:
	case WM_ENTERSIZEMOVE:
	case WM_ENTERMENULOOP:
	{
		DI_UnAcquireDevices();
		DS_StopSound();
		CPC_ReleaseKeys();
		DoNotScanKeyboard = TRUE;		
		//DS_ClearBuffer();
	}
	break;

//	case WM_NCLBUTTONUP:
//	case WM_NCMBUTTONUP:
//	case WM_NCRBUTTONUP:
	case WM_EXITSIZEMOVE:
	case WM_EXITMENULOOP:
	{
		DI_AcquireDevices();
		DS_StartSound();
		DoNotScanKeyboard = FALSE;
	}
	break;

	
	case WM_SIZING:
		{
			LPRECT	pSizingRect = (LPRECT)lParam;

			if (WindowedMode)
			{
				pSizingRect->right = pSizingRect->left + WindowWidth;
				pSizingRect->bottom = pSizingRect->top + WindowHeight;
			}
			else
			{
				pSizingRect->left = 0;
				pSizingRect->right = ScreenResX;
				pSizingRect->top = 0;
				pSizingRect->bottom = ScreenResY;
			}
		}
		return 0;

//			switch (wParam)
//			{
//				case WMSZ_BOTTOM:
//					break;
//				case WMSZ_BOTTOMLEFT:
//					break;
//				case WMSZ_BOTTOMRIGHT:
//					break;
//				case WMSZ_LEFT:
//					break;
//				case WMSZ_RIGHT:
//					break;
//				case WMSZ_TOP:
//					break;
//				case WMSZ_TOPLEFT:
//					break;
//				case WMSZ_TOPRIGHT:
//					break;
//			}
//
//		}
/*
	case WM_SIZE:
	{
		int Width = LOWORD(lParam);
		int Height= HIWORD(lParam);
		int	ResizeFlag = wParam;

		switch (ResizeFlag)
		{
			case SIZE_MAXIMIZED:
			{
				SetWindowPos(hwnd, NULL, 0,0, WindowWidth, WindowHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER); 
			}
			return 0;
			
			default:
				break;
		}
	}
	break;
*/

		
	case WM_PARENTNOTIFY:
	{

//		int EventFlags = LOWORD(wParam);  // event flags
//		int idChild = HIWORD(wParam);  // identifier of child window 
//	
//		if (EventFlags == WM_DESTROY)
//		{
//			HWND hChild = lParam;
//
//			DestroyWindow(hChild);
//		}

	}
	break;

	
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO *pMinMax = (LPMINMAXINFO) lParam;
		
		if (WindowedMode)
		{
			RECT		WindowRect;
			
			// get current x,y position.
			GetWindowRect(hwnd, &WindowRect);

			pMinMax->ptMaxSize.x = WindowRect.right-WindowRect.left;
			pMinMax->ptMaxSize.y = WindowRect.bottom-WindowRect.top;
			pMinMax->ptMaxPosition.x = WindowRect.left;
			pMinMax->ptMaxPosition.y = WindowRect.top;
			pMinMax->ptMinTrackSize.x = WindowWidth;
			pMinMax->ptMinTrackSize.y = WindowHeight;
			pMinMax->ptMaxTrackSize.x = pMinMax->ptMaxSize.x;
			pMinMax->ptMaxTrackSize.y = pMinMax->ptMaxSize.y;

			return 0;

#if 0
			if ((WindowWidth!=0) && (WindowHeight!=0))
			{
				pMinMax->ptMaxSize.x = WindowWidth;
				pMinMax->ptMaxSize.y = WindowHeight;
				pMinMax->ptMaxPosition.x = WindowRect.left;
				pMinMax->ptMaxPosition.y = WindowRect.top;
				pMinMax->ptMinTrackSize.x = WindowWidth;
				pMinMax->ptMinTrackSize.y = WindowHeight;
				pMinMax->ptMaxTrackSize.x = WindowWidth;
				pMinMax->ptMaxTrackSize.y = WindowHeight;
				return 0;
			}
#endif
		}
		else
		{
			pMinMax->ptMaxSize.x = ScreenResX;
			pMinMax->ptMaxSize.y = ScreenResY;
			pMinMax->ptMaxPosition.x = 0;
			pMinMax->ptMaxPosition.y = 0;
			pMinMax->ptMinTrackSize.x = ScreenResX;
			pMinMax->ptMinTrackSize.y = ScreenResY;
			pMinMax->ptMaxTrackSize.x = ScreenResX;
			pMinMax->ptMaxTrackSize.y = ScreenResY;
			return 0;
		}
	}
	break;

	case WM_MOVING:
	{
		if (!WindowedMode)
		{
			LPRECT pMovingRect = (LPRECT)lParam;

			pMovingRect->left = 0;
			pMovingRect->right = ScreenResX;
			pMovingRect->top = 0;
			pMovingRect->bottom = ScreenResY;

			return TRUE;
		}
	}
	break;

	case WM_LBUTTONDOWN:
	{
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);

		Buttons |= MK_LBUTTON;
	}
	break;

	case WM_LBUTTONUP:
	{
		Buttons &= ~MK_LBUTTON;
	}
	break;

	case WM_RBUTTONDOWN:
	{
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
		Buttons |= MK_RBUTTON;
	}
	break;

	case WM_RBUTTONUP:
	{
		Buttons &= ~MK_RBUTTON;
	}
	break;

	case WM_MOUSEMOVE:
	{
//		int xPos, yPos;

		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
		Buttons = wParam;
	}
	break;

	//case WM_MOVE:

	case WM_SETFOCUS:
	{
		DI_AcquireDevices();
		DoNotScanKeyboard = FALSE;
	}
	return 0;

	case WM_KILLFOCUS:
	{
		DI_UnAcquireDevices();
		DoNotScanKeyboard = TRUE;

		CPC_ReleaseKeys();

		// restore scroll lock state
		ScrollLock_RestoreState();

/*		if (!WindowedMode)
		{
			// in full-screen and focus changed.
			// something bad might have happened
		
			// go to windowed mode in an attempt
			// to force a reset
			CPCEMU_SetWindowed();
		}
*/	}
	return 0;

//	case WM_SETCURSOR:

	//case WM_ENTERSIZEMOVE:
	//{
	//}
	//break;

#ifdef DRAGANDDROP
	case WM_DROPFILES:
	{
		/* files dropped over it */

		/* drop handle */
		HANDLE DropHandle = (HANDLE)wParam;

		if (DropHandle!=0)
		{
			int i;

			/* get number of files dropped */
			int NoOfFilesDropped = DragQueryFile(DropHandle, -1, NULL, 0);
			
			if (NoOfFilesDropped!=0)
			{
				int MaxFilenameSize;
				char *pFilenameBuffer;

				MaxFilenameSize = 0;
				
				/* get size of largest filename */
				for (i=0; i<NoOfFilesDropped; i++)
				{
					int FilenameSize;

					/* get size of the filename for this file */
					FilenameSize = DragQueryFile(DropHandle, i, NULL, 0);

					if (FilenameSize>MaxFilenameSize)
					{
						/* update max filename size if the length of this
						filename is greater than current */
						MaxFilenameSize = FilenameSize;
					}
				}

				/* allocate memory for filenames to use */
				pFilenameBuffer = malloc(MaxFilenameSize+2);

				if (pFilenameBuffer!=NULL)
				{
					for (i=0; i<NoOfFilesDropped; i++)
					{
						/* get this filename */
						DragQueryFile(DropHandle, i, pFilenameBuffer, MaxFilenameSize+1);

						// do function to detect what this file is and
						// to load it if necessary

						CPCEMU_DetectFileAndLoad(pFilenameBuffer);
					}
				
					/* free buffer to hold filenames */
					free(pFilenameBuffer);
				}
			}

			// finish
			DragFinish(DropHandle);
		}

	}
	return 0;
#endif

	case WM_ERASEBKGND:
		return 0;

	case WM_DISPLAYCHANGE:
	{
		if (WindowedMode)
		{
//			DD_ReleaseSurfaces();

			//DD_ShutdownWindowed();

			CPCEMU_SetWindowed();
		}
	}
	break;

    case WM_PAINT:
        BeginPaint( hwnd, &PaintStruct);
    //    
	//	DD_FlipWindowed();
//
		EndPaint( hwnd, &PaintStruct );
      return 0;

    case WM_DESTROY:

		Debugger_Close();

        PostQuitMessage( 0 );
        return 0;

	default:
		break;
	}

    return DefWindowProc(hwnd, iMsg, wParam, lParam);
} 

/* This will be checked now because Arnold is currently windowed only */
/* get the bits per pixel in current display mode */
BOOL	CheckBitDepth()
{
	HWND	DesktopWindow;
	int		BitsPerPixel = 0;

	/* get desktop window */
	DesktopWindow = GetDesktopWindow();

	if (DesktopWindow!=NULL)
	{
		HDC hDC;

		/* get DC of desktop window */
		hDC = GetDC(DesktopWindow);

		if (hDC!=NULL)
		{
			/* get device caps */
			BitsPerPixel = GetDeviceCaps(hDC, BITSPIXEL);

			/* release DC */
			ReleaseDC(DesktopWindow,hDC);
		}
	}

	/* check display depth */
	if (BitsPerPixel<8)
	{
		/* show error message */
		ErrorMessage("This application will only work with\r\n8, 16,24 or 32 bit colour display modes.");

		/* quit */
		return FALSE;
	}

	return TRUE;
}


/*+-----------------------------------------------------------------------*/


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR pCmdLine, int iCmdShow)
{
//		MSG			Message;

	if (CheckBitDepth()==FALSE)
	{
		return 0;
	}
	else
	{
		WNDCLASSEX	WindowClass;
		BOOL		Quit=FALSE;


		
		InitCommonControls();

// not found with mingw
//		RegisterWindowMessage(MSH_MOUSEWHEEL);

		AppInstance = hInstance;

		/* initialise class */
		WindowClass.cbSize = sizeof(WNDCLASSEX);
		WindowClass.style = /*CS_HREDRAW | CS_VREDRAW |*/ CS_OWNDC;	// | CS_SAVEBITS;
		WindowClass.lpfnWndProc = WindowProc;
		WindowClass.cbClsExtra = 0;
		WindowClass.cbWndExtra = 0;
		WindowClass.hInstance = hInstance;
		WindowClass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
		WindowClass.hCursor = LoadCursor( NULL, IDC_ARROW );
		WindowClass.hbrBackground = GetStockObject(GRAY_BRUSH);
		WindowClass.lpszMenuName = NULL;
		WindowClass.lpszClassName = APPLICATION_CLASS_NAME;
		WindowClass.hIconSm = LoadIcon(NULL,IDI_APPLICATION);
		
		/* register class */
		if (MyApp_RegisterClass(&WindowClass))
		{
			/* create app window */
			ApplicationHwnd = CreateWindowEx(
				WS_EX_APPWINDOW,
				APPLICATION_CLASS_NAME,
				NULL,
				WS_OVERLAPPEDWINDOW,
				0,
				0,
				GetSystemMetrics( SM_CXSCREEN ),
				GetSystemMetrics( SM_CYSCREEN ), // + 30,
				NULL,
				NULL,
				hInstance,
				NULL );

							
			/* show the window */
			ShowWindow( ApplicationHwnd, iCmdShow );
			/* update window */
			UpdateWindow( ApplicationHwnd );
#ifdef DRAGANDDROP
			{
				LONG WindowLong;

				WindowLong = GetWindowLong(ApplicationHwnd, GWL_EXSTYLE);
				WindowLong |= WS_EX_ACCEPTFILES;
				SetWindowLong(ApplicationHwnd, GWL_EXSTYLE, WindowLong);
				
				/* accept dropped files */
				DragAcceptFiles(ApplicationHwnd, TRUE);
			}
#endif
			// prevent window from being resized
			{
				LONG WindowLong;

				WindowLong = GetWindowLong(ApplicationHwnd, GWL_STYLE);
				WindowLong &=~WS_THICKFRAME;
				SetWindowLong(ApplicationHwnd, GWL_STYLE, WindowLong);
			}

			ApplicationIsActive = TRUE;
			ApplicationHasFocus = TRUE;

			//DialogList_Initialise();

			/* check all required directx components are available */
			if (!DirectX_CheckComponentsArePresent())
			{
				ErrorMessage("The required DirectX components have not been found\r\nPlease re-install DirectX.");
			}
			else
			{

#if 0
				{
					char		TitleText[80];
					char		*Build;

				#ifdef _DEBUG
					Build = "Debug";
				#else
					Build = "Release";
				#endif

					wsprintf(TitleText, "Arnold - Built with %s on %s at %s",Build, __DATE__, __TIME__);
					SetWindowText(ApplicationHwnd,TitleText);
				}
#endif
				SetWindowText(ApplicationHwnd,"Arnold");

#ifdef CRTDEBUG
				{
		
		// Get current flag
		int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

		// Turn on leak-checking bit
		tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

		// Turn off CRT block checking bit
		//tmpFlag &= ~_CRTDBG_CHECK_CRT_DF;

		// Set flag to the new value
		_CrtSetDbgFlag( tmpFlag );
			}
#endif

				DD_Init();
				DD_SetupDD(ApplicationHwnd,NULL);

				DI_Init(hInstance);
		
				DS_Init(ApplicationHwnd);



				Joystick_Init();
			

				_getcwd(CurrentWorkingDirectory,MAX_PATH);

				//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

				CPCEMU_MainLoop();
				/*
				while (Quit==FALSE)
				{
					// Is a message available?
					if (PeekMessage(&Message,NULL,0,0,PM_NOREMOVE))
					{
						//if (!(DialogList_IsDialogMessage(&Message)))
						//{
							// yes, get the message
							if (GetMessage(&Message,NULL,0,0))
							{
								// if the message is not WM_QUIT
								// Translate it and dispatch it
								TranslateMessage(&Message);
								DispatchMessage(&Message);
							}
							else
							{
								// Message was WM_QUIT. So break out of message loop
								// and quit
								Quit=TRUE;
								break;
							}
						//}
					}
					else
					{
						// No message, so idle, execute user function
						if (ApplicationHasFocus)
							DI_ScanKeyboard();

//						if (ApplicationIsActive)
							CPCEMU_MainLoop();
					}
				}
				*/

//				DestroyWindow(ApplicationHwnd);


//				CPCEMU_Exit();


				DS_Close();

				DD_CloseDD();

				DI_Close();
				DD_Close();
			}

			// Destroy Main Dialog Window

//			DestroyWindow(hwndStatus);

			return 0; //Message.wParam;
		}
	}
#ifdef  CRT_DEBUG
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}

void	SetStatusText(char *text)
{
	SendMessage(hwndStatus, SB_SETTEXT,0, (LPARAM)text);
}

void	SetMainTitleText(char *text)
{
	SetWindowText(ApplicationHwnd, text);
}


void	MyApp_SetWindowDimensions(int Width, int Height)
{
	WindowWidth = Width;
	WindowHeight = Height;
}

typedef struct
{
	int WindowX, WindowY;
} WINDOW_SETTINGS;


void	MyApp_GetWindowSettings(WINDOW_SETTINGS *pWindowSettings)
{
	GetWindowRect(ApplicationHwnd, 0);
}


void	MyApp_CentraliseWindow(HWND hWindow, HWND hParent)
{
	RECT WindowRect;
	RECT ParentWindowRect;
	int WindowWidth, WindowHeight;
	int ParentWindowWidth, ParentWindowHeight;
	POINT WindowPos;
	HWND hParentWindow;

	if (hParent == 0)
	{
		// no parent specified, therefore centralise in desktop window
		hParentWindow = GetDesktopWindow();
	}
	else
	{
		// use specified parent
		hParentWindow = hParent;
	}


	// get window width and height.
	GetWindowRect(hWindow, &WindowRect);

	// calc width and height of this window
	WindowWidth = WindowRect.right-WindowRect.left;
	WindowHeight = WindowRect.bottom-WindowRect.top;

	// get parent window width and height.
	GetWindowRect(hParentWindow, &ParentWindowRect);

	// calc parent width and height
	ParentWindowWidth = ParentWindowRect.right - ParentWindowRect.left;
	ParentWindowHeight = ParentWindowRect.bottom - ParentWindowRect.top;

	// calc new position
	WindowPos.x = (ParentWindowWidth>>1)-(WindowWidth>>1) + ParentWindowRect.left;
	WindowPos.y = (ParentWindowHeight>>1)-(WindowHeight>>1) + ParentWindowRect.top;

	// set new pos
	SetWindowPos(hWindow, HWND_TOPMOST, WindowPos.x, WindowPos.y, 
		WindowWidth, WindowHeight, SWP_NOSIZE | SWP_NOZORDER);
}



BOOL	WinApp_ProcessSystemEvents()
{
	MSG	Message;

	// Is a message available?
	while (PeekMessage(&Message,NULL,0,0,PM_NOREMOVE))
	{
		// yes, get the message
		if (GetMessage(&Message,NULL,0,0))
		{
			// if the message is not WM_QUIT
			// Translate it and dispatch it
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		else
		{
			// Message was WM_QUIT. So break out of message loop
			// and quit
			return TRUE;
		}
	}

#ifdef SHOW_TIME
	{
		char NewTitle[256];
		unsigned long Speed = CPCEmulation_GetPercentRelativeSpeed();

		sprintf(NewTitle,"Arnold %d%%",Speed);

		SetMainTitleText(NewTitle);
	}
#endif

	// No message, so idle, execute user function
	if (!(DoNotScanKeyboard))
	{
		DI_ScanKeyboard();
	}

	return FALSE;
}


void	MyApp_GetWindowRect(RECT *pRect)
{
	int Width, Height;


	if (!Minimized)
	{
		RECT WindowRect;

		/* get client rect of window */
		GetWindowRect(ApplicationHwnd,&WindowRect);
		
		/* width of rectangle */
		Width = WindowRect.right-WindowRect.left;
		Height = WindowRect.bottom-WindowRect.top;

		MyApp_SetWindowDimensions(Width,Height);
	
		*pRect = WindowRect;
	
	}
	else
	{
		Width = WindowRectBeforeMinimize.right - WindowRectBeforeMinimize.left;
		Height = WindowRectBeforeMinimize.bottom - WindowRectBeforeMinimize.top;

		MyApp_SetWindowDimensions(Width, Height);
	
		*pRect = WindowRectBeforeMinimize;
	}
}

//	MyApp_CentraliseWindow(hwnd, NULL);
