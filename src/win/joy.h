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
#ifndef __JOY_HEADER_INCLUDED__
#define __JOY_HEADER_INCLUDED__

typedef struct
{
	int x,y;
	int buttons;
} JoyInfo;

enum
{
	JOYSTICK_BUTTON0 = 0,
	JOYSTICK_BUTTON1,
	JOYSTICK_BUTTON2,
	JOYSTICK_BUTTON3,
	JOYSTICK_BUTTON4,
	JOYSTICK_BUTTON5,
	JOYSTICK_BUTTON6,
	JOYSTICK_BUTTON7,
	JOYSTICK_BUTTON8,
	JOYSTICK_BUTTON9,
	JOYSTICK_BUTTON10,
	JOYSTICK_BUTTON11,
	JOYSTICK_BUTTON12,
	JOYSTICK_BUTTON13,
	JOYSTICK_BUTTON14,
	JOYSTICK_BUTTON15,
	JOYSTICK_BUTTON16,
	JOYSTICK_BUTTON17,
	JOYSTICK_BUTTON18,
	JOYSTICK_BUTTON19,
	JOYSTICK_BUTTON20,
	JOYSTICK_BUTTON21,
	JOYSTICK_BUTTON22,
	JOYSTICK_BUTTON23,
	JOYSTICK_BUTTON24,
	JOYSTICK_BUTTON25,
	JOYSTICK_BUTTON26,
	JOYSTICK_BUTTON27,
	JOYSTICK_BUTTON28,
	JOYSTICK_BUTTON29,
	JOYSTICK_BUTTON30,
	JOYSTICK_BUTTON31,
	JOYSTICK_BUTTON32,
	JOYSTICK_X_AXIS,
	JOYSTICK_Y_AXIS,
	JOYSTICK_Z_AXIS,
	JOYSTICK_R_AXIS,
	JOYSTICK_U_AXIS,
	JOYSTICK_V_AXIS,
};


typedef struct
{
	int FromID;
	int ToID;
} JoyMap;


void	Joystick_Read(int, JoyInfo *);
void	Joystick_Init(void);
#endif
