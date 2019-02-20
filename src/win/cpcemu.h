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
#ifndef __CPCEMU_HEADER_INCLUDED__
#define __CPCEMU_HEADER_INCLUDED__


#ifdef __cplusplus /* Troels */
   extern "C" {
#endif

#include <ddeml.h>

typedef struct 
{
	/* DDE variables */
	DWORD ddeId;				
	HSZ hszAppName;
	HSZ hszSystemTopicName;

	/*---*/
	int	WindowWidth;
	int WindowHeight;
	RECT WindowRectBeforeMinimize;
	BOOL Minimized;
	HWND hwndStatus;
	HWND hwndToolbar;
	BOOL	WindowedMode;
	int ScreenResX;
	int ScreenResY;
	HINSTANCE AppInstance;
	HINSTANCE ResourceInstance;
	int xPos, yPos,Buttons;
	BOOL	DoNotScanKeyboard;
	BOOL Windowed;
	int FullScreenWidth;
	int FullScreenHeight;
	int FullScreenDepth;
	HWND	ApplicationHwnd;
	BOOL  ApplicationIsActive;
	BOOL	ApplicationHasFocus;
	HMENU	hAppMenu;
	int MousePosX, MousePosY, MouseDeltaX, MouseDeltaY;
	BOOL	LeftPressed, RightPressed;

	int SnapshotVersion;
	int SnapshotSize;	


	int		nFrameSkip;
} APP_DATA;


void	DoKeyboard(void);
int		CPCEMU_Initialise(void);
void	CPCEMU_Exit(void);
void	CPCEMU_Run(void);
void	CPCEMU_Finish(void);
void	CPCEMU_SetWindowed(void);
void	CPCEMU_SetFullScreen(void);
void	CPCEMU_MainLoop(BOOL bWin);

typedef struct
{
	/* key to use as up */
	int Up;
	/* key to use as down */
	int Down;
	/* key to use as left */
	int Left;
	/* key to use as right */
	int Right;
	/* key to use as fire 1 */
	int Fire1;
	/* key to use as fire 2 */
	int Fire2;
} SimulatedJoystickData;

typedef struct
{
	/* true if auto-fire is active, false otherwise */
	BOOL Active;
	/* auto-fire timer; updated */
	int AutoFireTimer;
	/* auto-fire timer rate */
	int AutoFireTimerRate;
} AutoFireSimulation;

typedef struct
{
	/* the id of the physical joystick */
	int id;
} RealJoystickData;

#define JOYSTICK_DATA_AXIS_MIN -32767
#define JOYSTICK_DATA_AXIS_MAX 32767

/* this is the final processed data */
typedef struct
{
	int x;
	int y;
	int buttons;
} JoystickData;

typedef struct
{
	int MapFromID;
	int MapToID;
} JoystickRemap;

enum
{
	/* real joystick or joypad connected to computer */
	JOYSTICK_REAL,
	/* joystick simulated by keyboard */
	JOYSTICK_SIMULATED_BY_KEYBOARD,
};


typedef struct
{
	/* joystick is active? */
	BOOL Active;
	int Type;
	/* id of CPC joystick */
	int CPCJoystickID;
	AutoFireSimulation autoFire;
	RealJoystickData realJoystickData;
	SimulatedJoystickData simulatedJoystickData;
} JoystickConfigData;

void CPCEMU_SetKey_VK(int win_virtualkey, BOOL bKeyDown, BOOL bNumPad);

#include <windows.h>

BOOL	WinApp_ProcessSystemEvents();
void	MyApp_SetWindowed(int Width, int Height);
void	MyApp_SetFullScreen(int Width, int Height);
void	MyApp_CentraliseWindow(HWND hWindow, HWND hParent);
int ArnoldMain(HINSTANCE hInstance, int iCmdShow, int argc, TCHAR* argv[]);


#ifdef _UNICODE
LPCSTR ConvertUnicodeStringToMultiByte(const TCHAR *sUnicodeString);
const TCHAR *ConvertMultiByteToUnicode(LPCSTR sMultiByte);
#endif

#ifdef __cplusplus
   }
#endif

#endif