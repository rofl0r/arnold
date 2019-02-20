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
#include "joy.h"

static BOOL		JoystickEnabled;

/* number of joysticks supported by device */
static int			NoOfJoysticks;

/* x,y,z and buttons */
static JOYINFO		JoystickInfo;

/* joystick capabilities */
static JOYCAPS		JoystickCaps;

static int XMid, YMid;
static int XDead, YDead;

/* id of joystick to read */
static int joystickID = JOYSTICKID1;

static int	XPos, YPos, Buttons;

/* read joystick */
void	Joystick_Read()
{
	MMRESULT	hResult;

	if (JoystickEnabled)
	{
		hResult = joyGetPos(joystickID, &JoystickInfo);

		if (hResult==JOYERR_NOERROR)
		{
			XPos = JoystickInfo.wXpos;
			YPos = JoystickInfo.wYpos;
			Buttons = JoystickInfo.wButtons;
		}
	}
}

void	Joystick_StateDigital(JOY_STATE_DIGITAL *pState)
{
	pState->Left = FALSE;
	pState->Right = FALSE;
	pState->Up = FALSE;
	pState->Down = FALSE;
	pState->Buttons = 0;

	if (JoystickEnabled)
	{

		
		if (XPos<(XMid-XDead))
			pState->Left = TRUE;
		
		if (XPos>(XMid+XDead))
			pState->Right = TRUE;

		if (YPos<(YMid-YDead))
			pState->Up = TRUE;
 
		if (YPos>(YMid+YDead))
			pState->Down = TRUE;

		pState->Buttons = Buttons;
	}
		
}

void	Joystick_Init()
{
	MMRESULT hResult;

	JoystickEnabled = FALSE;

	/* get number of devices supported by this driver */
	NoOfJoysticks = joyGetNumDevs();

	if (NoOfJoysticks!=0)
	{
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
	}
}
