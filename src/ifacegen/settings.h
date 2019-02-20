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
#ifndef __SETTINGS_HEADER_INCLUDED__
#define __SETTINGS_HEADER_INCLUDED__

#include "../cpc/cpcglob.h"

#define SETTINGS_WRITE_MODE	0x0001
#define SETTINGS_READ_MODE 0x0002

BOOL	Settings_OpenSettings(int Mode);
void	Settings_CloseSettings(void);

/* store a string to settings */
void Settings_StoreString(char *KeyName, char *pString);
/* store a int to settings */
void Settings_StoreInt(char *KeyName, int Value);
/* get int from settings */
int Settings_GetInt(char *KeyName);
/* get string from settings */
char *Settings_GetString(char *KeyName);
/* is a key present? */
BOOL  Settings_IsKeyPresent(char *KeyName);

#endif
