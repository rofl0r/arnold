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
/* CPCEMU cheat database support code */
#ifndef __CHEAT_DATABASE_HEADER_INCLUDED__
#define __CHEAT_DATABASE_HEADER_INCLUDED__

#define CHEAT_DATABASE_ENTRY_MAX_PARTS 16

#define CHEAT_DATABASE_POKE_ENTRY_FLAGS_ENTER_VALUE 0x001

typedef struct CHEAT_DATABASE_POKE_ENTRY
{
	unsigned char	Flags;
	unsigned long	Addr;
	unsigned char	NewValue;
	unsigned char	OldValue;
} CHEAT_DATABASE_POKE_ENTRY;

typedef struct CHEAT_DATABASE_ENTRY
{
	struct	CHEAT_DATABASE_ENTRY *pNext;
	struct	CHEAT_DATABASE_ENTRY *pPrev;

	unsigned char	*Name;
	unsigned char	*Description;

	unsigned long	NoOfParts;

	unsigned char	Type;

	CHEAT_DATABASE_POKE_ENTRY	Parts[CHEAT_DATABASE_ENTRY_MAX_PARTS];
} CHEAT_DATABASE_ENTRY;

typedef struct CHEAT_DATABASE
{
	struct CHEAT_DATABASE_ENTRY	*pFirst;
} CHEAT_DATABASE;

CHEAT_DATABASE *CheatDatabase_Read(char *);
void	CheatDatabase_Delete(CHEAT_DATABASE *pDatabase);
void	CheatDatabase_Poke(CHEAT_DATABASE_ENTRY *pEntry);

#endif
