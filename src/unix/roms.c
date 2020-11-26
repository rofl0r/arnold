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

#include "roms.h"

#include <stdio.h>

/*
 * References to statically linked external binary rom and cartridge data
 */

/* Amsdos */
extern char _binary_roms_amsdose_amsdos_rom_start[];
extern char _binary_roms_amsdose_amsdos_rom_end[];
extern int *_binary_roms_amsdose_amsdos_rom_size;

/* CPC464 */
extern char _binary_roms_cpc464e_os_rom_start[];
extern char _binary_roms_cpc464e_os_rom_end[];
extern int *_binary_roms_cpc464e_os_rom_size;

extern char _binary_roms_cpc464e_basic_rom_start[];
extern char _binary_roms_cpc464e_basic_rom_end[];
extern int *_binary_roms_cpc464e_basic_rom_size;

/* CPC664 */
extern char _binary_roms_cpc664e_os_rom_start[];
extern char _binary_roms_cpc664e_os_rom_end[];
extern int *_binary_roms_cpc664e_os_rom_size;

extern char _binary_roms_cpc664e_basic_rom_start[];
extern char _binary_roms_cpc664e_basic_rom_end[];
extern int *_binary_roms_cpc664e_basic_rom_size;

/* CPC6128 */
extern char _binary_roms_cpc6128e_os_rom_start[];
extern char _binary_roms_cpc6128e_os_rom_end[];
extern int *_binary_roms_cpc6128e_os_rom_size;

extern char _binary_roms_cpc6128e_basic_rom_start[];
extern char _binary_roms_cpc6128e_basic_rom_end[];
extern int *_binary_roms_cpc6128e_basic_rom_size;

/* CPC6128 spanish */
extern char _binary_roms_cpc6128s_os_rom_start[];
extern char _binary_roms_cpc6128s_os_rom_end[];
extern int *_binary_roms_cpc6128s_os_rom_size;

extern char _binary_roms_cpc6128s_basic_rom_start[];
extern char _binary_roms_cpc6128s_basic_rom_end[];
extern int *_binary_roms_cpc6128s_basic_rom_size;

/* CPCplus */
extern char _binary_roms_cpcplus_system_cpr_start[];
extern char _binary_roms_cpcplus_system_cpr_end[];
extern int *_binary_roms_cpcplus_system_cpr_size;

/* KCC */
extern char _binary_roms_kcc_os_rom_start[];
extern char _binary_roms_kcc_os_rom_end[];
extern int *_binary_roms_kcc_os_rom_size;

extern char _binary_roms_kcc_basic_rom_start[];
extern char _binary_roms_kcc_basic_rom_end[];
extern int *_binary_roms_kcc_basic_rom_size;

/*
 * functions
 */

void printroms(char *name, roms_t *roms);
void printrom(char *s1, char *s2, rom_t *rom);

void roms_init() {
#define SETUP_ROM(RNAME, BNAME) do { \
	RNAME.start = BNAME ## _start; \
	RNAME.end = BNAME ## _end; \
	RNAME.size = (int) & BNAME ## _size; \
	} while(0)

#define SETUP_ROMS(RNAME, SUFFIX) do { \
	SETUP_ROM(roms_ ## RNAME .os, _binary_roms_ ## RNAME ## SUFFIX ## _os_rom); \
	SETUP_ROM(roms_ ## RNAME .basic, _binary_roms_ ## RNAME ## SUFFIX ## _basic_rom); \
	} while(0)

	/* Amsdos */
	SETUP_ROM(rom_amsdos, _binary_roms_amsdose_amsdos_rom);

	/* CPC 464 */
	SETUP_ROMS(cpc464, e);

	/* CPC 664 */
	SETUP_ROMS(cpc664, e);

	/* CPC 6128 */
	SETUP_ROMS(cpc6128, e);

	/* CPC 6128 spanish */
	SETUP_ROMS(cpc6128s, );

	/* CPC plus */
	SETUP_ROM(cartridge_cpcplus, _binary_roms_cpcplus_system_cpr);

	/* kcc */
	SETUP_ROMS(kcc, );

	/* debugging output */
	printf("----------------------------------------------------------\n");
	printf("Compiled in ROMS:\n");
	printrom("  Amsdos", "   ", &rom_amsdos);
	printroms("  464  ", &roms_cpc464);
	printroms("  664  ", &roms_cpc664);
	printroms("  6128 ", &roms_cpc6128);
	printroms("  6128s", &roms_cpc6128s);
	printrom("  plus ", "Cart ", &cartridge_cpcplus);
	printroms("  kcc  ", &roms_kcc);
	printf("----------------------------------------------------------\n");

}

void printroms(char *name, roms_t *roms) {
	printrom(name, "OS   ", &roms->os);
	printrom(name, "Basic", &roms->basic);
}

void printrom(char *s1, char *s2, rom_t *rom) {
	printf("%s %s: %08x to %08x, %d (0x%04x) bytes\n",
		s1, s2,
		(unsigned int) rom->start, (unsigned int) rom->end,
		rom->size, rom->size);
}

