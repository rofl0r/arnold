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

/* Compile options needed: */ 
#include "precomp.h"

//#include <windows.h> 
#include "win.h"

static BOOL	ScrollLock_InitialState = FALSE;
static BOOL ScrollLock_CurrentState;

void ScrollLock_Set( BOOL bState ) 
{ 
   BYTE keyState[256];

	ScrollLock_CurrentState = bState;

   GetKeyboardState((LPBYTE)&keyState);
   if( (bState && !(keyState[/*VK_NUMLOCK*/ VK_SCROLL] & 1)) ||
       (!bState && (keyState[/*VK_NUMLOCK*/ VK_SCROLL] & 1)) )
   {
   // Simulate a key press
      keybd_event( /*VK_NUMLOCK*/VK_SCROLL,
                   0x046,	//0x45,
                   KEYEVENTF_EXTENDEDKEY | 0,
                   0 );

   // Simulate a key release
      keybd_event( /*VK_NUMLOCK*/VK_SCROLL,
                   0x046,	//0x45,
                   KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
                   0);
   }
} 
/*
void	ScrollLock_RestoreCurrentState()
{
	// get scroll lock state
	char KeyState[256];

	GetKeyboardState((LPBYTE)&KeyState);

	if (
		// currently pressed, but initial state is off 
		(((KeyState[VK_SCROLL] & 1)!=0) && (ScrollLockState==FALSE)) ||
		// not currently pressed, but initiale state is on 
		(((KeyState[VK_SCROLL] & 1)==0) && (ScrollLockState==TRUE))
		)
	{
	  // Simulate a key press
		keybd_event( VK_SCROLL,
             0x046,	//0x45,
             KEYEVENTF_EXTENDEDKEY | 0,
             0 );

	   // Simulate a key release
		keybd_event( VK_SCROLL,
             0x046,	//0x45,
             KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
             0 );
	}
}
*/


void	ScrollLock_RestoreState()
{
	// get scroll lock state
	char KeyState[256];

	GetKeyboardState((LPBYTE)&KeyState);

	if (
		/* currently pressed, but initial state is off */
		(((KeyState[VK_SCROLL] & 1)!=0) && (ScrollLock_InitialState==FALSE)) ||
		/* not currently pressed, but initiale state is on */
		(((KeyState[VK_SCROLL] & 1)==0) && (ScrollLock_InitialState==TRUE))
		)
	{
	  // Simulate a key press
		keybd_event( /*VK_NUMLOCK*/VK_SCROLL,
             0x046,	//0x45,
             KEYEVENTF_EXTENDEDKEY | 0,
             0 );

	   // Simulate a key release
		keybd_event( /*VK_NUMLOCK*/VK_SCROLL,
             0x046,	//0x45,
             KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
             0 );
	}
}


/* store initial state of scroll lock */
void	ScrollLock_StoreState()
{
		char KeyState[256];

		GetKeyboardState((LPBYTE)&KeyState);
   
		if ((KeyState[VK_SCROLL] & 1)!=0)
		{
			ScrollLock_InitialState = TRUE;
		}
		else
		{
			ScrollLock_InitialState = FALSE;
		}
	}

