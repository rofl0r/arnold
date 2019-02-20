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

#ifndef __ROMS_HEADER_INCLUDED__
#define __ROMS_HEADER_INCLUDED__

#define BUILTIN "^"

/*
 * typedefs for roms and cartridges
 */

typedef struct {
	char *start;
	char *end;
	int size;
} rom_t;

typedef struct {
	rom_t os;
	rom_t basic;
} roms_t;

typedef rom_t cartridge_t;

/*
 * Rom structures
 */
rom_t		rom_amsdos;
roms_t		roms_cpc464;
roms_t		roms_cpc664;
roms_t		roms_cpc6128;
roms_t		roms_kcc;
cartridge_t	cartridge_cpcplus;

/*
 * functions
 */

void roms_init();

#endif

