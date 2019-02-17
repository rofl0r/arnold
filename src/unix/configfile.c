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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define USER_CONFIGFILE_NAME "~/.arnold"
#define SYSTEM_CONFIGFILE_NAME "/etc/arnold"
#define USER_CONFIGFILE_ERROR "Could not open ~/.arnold"
#define SYSTEM_CONFIGFILE_ERROR "Could not open /etc/arnold"

#define BUFLEN 1024
char buf[BUFLEN];

#define KEY_COMMENT "#"
#define KEY_ROMDIR "romdir="
char *romDirectory = NULL;
#define KEY_DISKDIR "diskdir="
char *diskDirectory = NULL;
#define KEY_TAPEDIR "tapedir="
char *tapeDirectory = NULL;

/* Forward declarations */
int parseConfigFile(FILE *file);
char *getLine(FILE *file);
void parseLine(char *s);
char *trim(char *s);

void loadConfigfile() {
	int len;
	FILE *file;
	char *filename;
	/* First try to load user configfile */
	filename = USER_CONFIGFILE_NAME;
	fprintf(stderr, "Opening %s\n", filename);
	file = fopen(filename, "r");
	if (file == NULL) {
		perror(USER_CONFIGFILE_ERROR);
		filename = SYSTEM_CONFIGFILE_NAME;
		fprintf(stderr, "Opening %s\n", filename);
		file = fopen(filename, "r");
		if (file == NULL) {
			perror(SYSTEM_CONFIGFILE_ERROR);
			return;
		}
	}
	fprintf(stderr, "Parsing %s\n", filename);
	if (parseConfigFile(file) < 0) {
		perror("Could not parse configfile");
		return;
	}
}

int parseConfigFile(FILE *file) {
	char *s;
	while ((s = getLine(file)) != NULL) {
		parseLine(s);
	}
	return 0;
}

char *getLine(FILE *file) {
	char *s;
	s = malloc(1024);
	return (fgets(s, 1024, file));
}

void parseLine(char *s) {
	if (strncmp(KEY_COMMENT, s, 1) == 0) return;	/* Comment */
	/* ROM directory */
	if (strncmp(KEY_ROMDIR, s, strlen(KEY_ROMDIR)) == 0) {
		romDirectory = s + strlen(KEY_ROMDIR);
		romDirectory = trim(romDirectory);
		fprintf(stderr, "RomDirectory: <%s>\n", romDirectory);
	} else if (strncmp(KEY_DISKDIR, s, strlen(KEY_DISKDIR)) == 0) {
		diskDirectory = s + strlen(KEY_DISKDIR);
		diskDirectory = trim(diskDirectory);
		fprintf(stderr, "DiskDirectory: <%s>\n", diskDirectory);
	} else if (strncmp(KEY_TAPEDIR, s, strlen(KEY_TAPEDIR)) == 0) {
		tapeDirectory = s + strlen(KEY_TAPEDIR);
		tapeDirectory = trim(tapeDirectory);
		fprintf(stderr, "TapeDirectory: <%s>\n", tapeDirectory);
	}
}

char *trim(char *s) {
	char *p;
	while (*s != '\0' && (*s == ' ' || *s == '\t')) s++;
	p = s + strlen(s) - 1;
	while (p > s && (*p == '\n' || *p == '\r'
		|| *p == ' ' || *p == '\t')) *p--='\0';
	return s;
}

char *getRomDirectory() {
	return romDirectory;
}

char *getDiskDirectory() {
	return diskDirectory;
}

char *getTapeDirectory() {
	return tapeDirectory;
}
