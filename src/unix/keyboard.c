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
#include <X11/keysym.h>

extern Atom Protocols[1];

// table to map KeySym values to CPC Key values
int	KeySymToCPCKey[256];
int	FFKeySymToCPCKey[256];

int DisplayMode = 0;

// State is True for Key Pressed, False for Key Release.
// theEvent holds the keyboard event.
void	HandleKey(int State, XEvent *theEvent)
{
	int key_upper_code;
	int keycode;
	CPC_KEY_ID	theKeyPressed;
		
	// get KeySym
	keycode = XLookupKeysym((XKeyEvent *)theEvent,0);

	if (keycode == XK_F1)
	{
		CPC_Reset();
	}
	else if (keycode == XK_F2)
	{	
		DisplayMode ^=0x0ff;
		

	}
	else
	{
//	printf("Keycode: %04x\r\n", keycode);

	key_upper_code = ((keycode & 0x0ff00)>>8);

	switch (key_upper_code)
	{
		case 0:
		{
			// get cpc key code
			theKeyPressed = KeySymToCPCKey[keycode];
		}
		break;
	
		case 0x0ff:
		{
			// get cpc key code
			theKeyPressed = FFKeySymToCPCKey[keycode & 0x0ff];
		}
		break;

		// any unhandled keys
		default:
			theKeyPressed = CPC_KEY_NULL;
			break;
	}

	// set or release key depending on state	
	if (State==True)
	{	
		// set key
		CPC_SetKey(theKeyPressed);
	}
	else
	{
		// release key
		CPC_ClearKey(theKeyPressed);
	}
	}
}

void	XWindows_InitialiseKeyboardMapping()
{
	int	 i;

	for (i=0; i<256; i++)
	{
		KeySymToCPCKey[i] = CPC_KEY_NULL;
	}
	
	for (i=0; i<256; i++)
	{
		FFKeySymToCPCKey[i] = CPC_KEY_NULL;
	}	

	KeySymToCPCKey[XK_0] = CPC_KEY_ZERO;
	KeySymToCPCKey[XK_1] = CPC_KEY_1;
	KeySymToCPCKey[XK_2] = CPC_KEY_2;
	KeySymToCPCKey[XK_3] = CPC_KEY_3;
	KeySymToCPCKey[XK_4] = CPC_KEY_4;
	KeySymToCPCKey[XK_5] = CPC_KEY_5;
	KeySymToCPCKey[XK_6] = CPC_KEY_6;
	KeySymToCPCKey[XK_7] = CPC_KEY_7;
	KeySymToCPCKey[XK_8] = CPC_KEY_8;
	KeySymToCPCKey[XK_9] = CPC_KEY_9;
	KeySymToCPCKey[XK_a] = CPC_KEY_A;
	KeySymToCPCKey[XK_b] = CPC_KEY_B;
	KeySymToCPCKey[XK_c] = CPC_KEY_C;
	KeySymToCPCKey[XK_d] = CPC_KEY_D;
	KeySymToCPCKey[XK_e] = CPC_KEY_E;
	KeySymToCPCKey[XK_f] = CPC_KEY_F;
	KeySymToCPCKey[XK_g] = CPC_KEY_G;
	KeySymToCPCKey[XK_h] = CPC_KEY_H;
	KeySymToCPCKey[XK_i] = CPC_KEY_I;
	KeySymToCPCKey[XK_j] = CPC_KEY_J;
	KeySymToCPCKey[XK_k] = CPC_KEY_K;
	KeySymToCPCKey[XK_l] = CPC_KEY_L;
	KeySymToCPCKey[XK_m] = CPC_KEY_M;
	KeySymToCPCKey[XK_n] = CPC_KEY_N;
	KeySymToCPCKey[XK_o] = CPC_KEY_O;
	KeySymToCPCKey[XK_p] = CPC_KEY_P;
	KeySymToCPCKey[XK_q] = CPC_KEY_Q;
	KeySymToCPCKey[XK_r] = CPC_KEY_R;
	KeySymToCPCKey[XK_s] = CPC_KEY_S;
	KeySymToCPCKey[XK_t] = CPC_KEY_T;
	KeySymToCPCKey[XK_u] = CPC_KEY_U;
	KeySymToCPCKey[XK_v] = CPC_KEY_V;
	KeySymToCPCKey[XK_w] = CPC_KEY_W;
	KeySymToCPCKey[XK_x] = CPC_KEY_X;
	KeySymToCPCKey[XK_y] = CPC_KEY_Y;
	KeySymToCPCKey[XK_z] = CPC_KEY_Z;
	KeySymToCPCKey[XK_space] = CPC_KEY_SPACE;
	KeySymToCPCKey[XK_comma] = CPC_KEY_COMMA;
	KeySymToCPCKey[XK_period] = CPC_KEY_DOT;
	KeySymToCPCKey[XK_semicolon] = CPC_KEY_COLON;
	KeySymToCPCKey[XK_minus] = CPC_KEY_MINUS;
	KeySymToCPCKey[XK_equal] = CPC_KEY_HAT;
	KeySymToCPCKey[XK_bracketleft] = CPC_KEY_AT;
	KeySymToCPCKey[XK_bracketright] =CPC_KEY_OPEN_SQUARE_BRACKET;

	FFKeySymToCPCKey[(XK_Tab & 0x0ff)] = CPC_KEY_TAB;
	FFKeySymToCPCKey[(XK_Return & 0x0ff)] = CPC_KEY_RETURN;
	FFKeySymToCPCKey[(XK_BackSpace & 0x0ff)] = CPC_KEY_DEL;
	FFKeySymToCPCKey[(XK_Escape & 0x0ff)] = CPC_KEY_ESC;

	//FFKeySymToCPCKey[(XK_Equals & 0x0ff)] = CPC_KEY_CLR;

	FFKeySymToCPCKey[(XK_Up & 0x0ff)] = CPC_KEY_CURSOR_UP;
	FFKeySymToCPCKey[(XK_Down & 0x0ff)] = CPC_KEY_CURSOR_DOWN;
	FFKeySymToCPCKey[(XK_Left & 0x0ff)] = CPC_KEY_CURSOR_LEFT;
	FFKeySymToCPCKey[(XK_Right & 0x0ff)] = CPC_KEY_CURSOR_RIGHT;
	
	FFKeySymToCPCKey[(XK_KP_Insert & 0x0ff)] = CPC_KEY_F0;
	FFKeySymToCPCKey[(XK_KP_End & 0x0ff)] = CPC_KEY_F1;
	FFKeySymToCPCKey[(XK_KP_Down & 0x0ff)] = CPC_KEY_F2;
	FFKeySymToCPCKey[(XK_KP_Page_Down & 0x0ff)] = CPC_KEY_F3;
	FFKeySymToCPCKey[(XK_KP_Left & 0x0ff)] = CPC_KEY_F4;
	FFKeySymToCPCKey[(XK_KP_Begin & 0x0ff)] = CPC_KEY_F5;
	FFKeySymToCPCKey[(XK_KP_Right & 0x0ff)] = CPC_KEY_F6;
	FFKeySymToCPCKey[(XK_KP_Home & 0x0ff)] = CPC_KEY_F7;
	FFKeySymToCPCKey[(XK_KP_Up & 0x0ff)] = CPC_KEY_F8;
	FFKeySymToCPCKey[(XK_KP_Page_Up & 0x0ff)] = CPC_KEY_F9;

	FFKeySymToCPCKey[(XK_KP_Decimal & 0x0ff)] = CPC_KEY_FDOT;

	FFKeySymToCPCKey[(XK_Shift_L & 0x0ff)] = CPC_KEY_SHIFT;
	FFKeySymToCPCKey[(XK_Shift_R & 0x0ff)] = CPC_KEY_SHIFT;
	FFKeySymToCPCKey[(XK_Control_L & 0x0ff)] = CPC_KEY_CONTROL;
	FFKeySymToCPCKey[(XK_Control_R & 0x0ff)] = CPC_KEY_CONTROL;
	FFKeySymToCPCKey[(XK_Caps_Lock & 0x0ff)] = CPC_KEY_CAPS_LOCK;
	
	FFKeySymToCPCKey[(XK_KP_Enter & 0x0ff)] = CPC_KEY_SMALL_ENTER;

	FFKeySymToCPCKey[(XK_Delete & 0x0ff)] = CPC_KEY_JOY_LEFT;
	FFKeySymToCPCKey[(XK_End & 0x0ff)] = CPC_KEY_JOY_DOWN;
	FFKeySymToCPCKey[(XK_Page_Down & 0x0ff)] = CPC_KEY_JOY_RIGHT;
	FFKeySymToCPCKey[(XK_Insert & 0x0ff)] = CPC_KEY_JOY_FIRE1;
	FFKeySymToCPCKey[(XK_Home & 0x0ff)] = CPC_KEY_JOY_UP;
	FFKeySymToCPCKey[(XK_Page_Up & 0x0ff)] = CPC_KEY_JOY_FIRE2;	
}				

BOOL	XWindows_ProcessSystemEvents()
{
	XEvent	theEvent;

	while (XPending(display))
	{
		XNextEvent(display, &theEvent);

		switch (theEvent.type)
		{
			case DestroyNotify:
				return TRUE;

			case KeyPress:
			{
				HandleKey(True, &theEvent);
			}
			break;		

			case KeyRelease:
			{
				HandleKey(False, &theEvent);
			}
			break;

			default:
				break;
		}
	
	}

	return FALSE;
}

