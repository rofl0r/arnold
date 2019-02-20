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
#define INITGUID
#define DIRECTINPUT_VERSION 0x0300
#include <dinput.h>
#include "di.h"
static char		KeyboardData[256];
static char     PreviousKeyboardData[256];

static LPDIRECTINPUT		pDirectInput=NULL;
static LPDIRECTINPUTDEVICE	pKeyboardDevice=NULL;
static LPDIRECTINPUTDEVICE	pMouseDevice=NULL;

static BOOL	DI_InitKeyboard(void);
static void DI_CloseKeyboard(void);

static void	DI_InitMouse();
static void DI_CloseMouse(void);


#define DI_VERSION	0x0300

/*------------------------------------------------------------------------------*/

BOOL	DI_Init(HINSTANCE hInstance)
{
	
	if (DirectInputCreate(hInstance,/*DIRECTINPUT_VERSION*/DI_VERSION,&pDirectInput,NULL)!=DI_OK)
		return FALSE;

	DI_InitKeyboard();

	DI_InitMouse();

	return TRUE;

}

/*------------------------------------------------------------------------------*/

void	DI_Close()
{
	DI_CloseKeyboard();

	DI_CloseMouse();

	if (pDirectInput!=NULL)
		IDirectInput_Release(pDirectInput);
}

/*------------------------------------------------------------------------------*/

BOOL	DI_InitKeyboard()
{
	// create keyboard device
	if (IDirectInput_CreateDevice(pDirectInput,&GUID_SysKeyboard,&pKeyboardDevice,NULL)!=DI_OK)
		return FALSE;

	// set data format
	if (IDirectInputDevice_SetDataFormat(pKeyboardDevice,&c_dfDIKeyboard)!=DI_OK)
		return FALSE;
	
	// acquire it
	if (IDirectInputDevice_Acquire(pKeyboardDevice)!=DI_OK)
		return FALSE;

	return TRUE;
}

void	DI_CloseKeyboard()
{
	if (pKeyboardDevice!=NULL)
	{
		// unacquire device
		IDirectInputDevice_Unacquire(pKeyboardDevice);

		// release it
		IDirectInputDevice_Release(pKeyboardDevice);
	}
}

void	DI_ScanKeyboard()
{
	HRESULT hResult;

    //memcpy(PreviousKeyboardData,KeyboardData,256);

	while (1==1)
	{
		hResult = IDirectInputDevice_GetDeviceState(pKeyboardDevice,256,KeyboardData);

		if (hResult==DI_OK)
			break;

		if ((hResult==DIERR_NOTACQUIRED) || (hResult == DIERR_INPUTLOST))
			IDirectInputDevice_Acquire(pKeyboardDevice);
	}

}

BOOL	IsKeyPressed(int KeyCode)
{
	return ((KeyboardData[KeyCode] & 0x080)!=0);
}

BOOL    DebounceIsKeyPressed(int KeyCode)
{
    if ( ((PreviousKeyboardData[KeyCode] & 0x080)==0) && ((KeyboardData[KeyCode] & 0x080)!=0) )
        return TRUE;
    else
        return FALSE;
}


/*------------------------------------------------------------------------------*/

void	DI_InitMouse()
{
	IDirectInput_CreateDevice(pDirectInput,&GUID_SysMouse,&pMouseDevice,NULL);
}

void	DI_CloseMouse()
{
	if (pMouseDevice!=NULL)
	{
		IDirectInputDevice_Unacquire(pMouseDevice);

		IDirectInputDevice_Release(pMouseDevice);
	}
}

void	DI_UnAcquireDevices()
{
	if (pMouseDevice!=NULL)
	{
		IDirectInputDevice_Unacquire(pMouseDevice);
	}

	if (pKeyboardDevice!=NULL)
	{
		IDirectInputDevice_Unacquire(pKeyboardDevice);
	}
}

void	DI_AcquireDevices()
{
	if (pMouseDevice!=NULL)
	{
		IDirectInputDevice_Acquire(pMouseDevice);
	}

	if (pKeyboardDevice!=NULL)
	{
		IDirectInputDevice_Acquire(pKeyboardDevice);
	}
}
