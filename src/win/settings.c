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
#include "../ifacegen/settings.h"
#include "reg.h"

BOOL	Settings_OpenSettings(int Mode)
{
	if (MyApp_Registry_CheckKeyPresent())
	{
		/* get settings */


	}
	else
	{
		MyApp_Registry_InitialiseKey();
	}

	return MyApp_Registry_OpenKey();
}

BOOL  Settings_IsKeyPresent(char *KeyName)
{
	return MyApp_Registry_CheckValueIsPresentInCurrentKey(KeyName);
}

void	Settings_CloseSettings(void)
{
	MyApp_Registry_CloseKey();
}

void Settings_StoreString(char *KeyName, char *String)
{
	MyApp_Registry_StoreStringToCurrentKey(KeyName, String);
}

void	Settings_StoreInt(char *KeyName, int Value)
{
	MyApp_Registry_StoreIntToCurrentKey(KeyName, Value);
}

char *Settings_GetString(char *KeyName)
{
	return MyApp_Registry_GetStringFromCurrentKey(KeyName);
}

int	Settings_GetInt(char *KeyName)
{
	return MyApp_Registry_GetIntFromCurrentKey(KeyName);
}
