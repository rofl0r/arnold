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
#include <ctype.h>
#include <stdlib.h>
#include "configfile.h"
#include "../cpc/cpc.h"
#include "ifacegen.h"

/* Forward declarations */
int parseConfigFile(FILE *file);
static const char *getLine(FILE *file);
static void parseLine(const char *s);
static char *trim(char *s);

/* todo: put in proper header for this */
extern void LoadRom(int, const char *);

/* list of possible config file paths */
/* searched in the order listed */
const char *sConfigPaths[]=
{
	"~/.arnold",
	"/etc/arnold"
};

/* buffer for fgets */
#define BUFLEN 1024
static char buf[BUFLEN];

static char *romDirectory = NULL;
static char *tapeDirectory = NULL;
static char *diskDirectory = NULL;
static char *cartDirectory = NULL;
static char *snapDirectory = NULL;
static char *diskPathDriveA = NULL;
static char *diskPathDriveB = NULL;
static char *insertedCartPath = NULL;
static char *insertedTapePath = NULL;
static char *multifaceCPCPath = NULL;
static char *multifacePLUSPath = NULL;

static char *insertedRomPath[16];

void configInit()
{
	int i;

	romDirectory = NULL;
	tapeDirectory = NULL;
	diskDirectory = NULL;
	cartDirectory = NULL;
	snapDirectory = NULL;
	diskPathDriveA = NULL;
	diskPathDriveB = NULL;
	insertedCartPath = NULL;
	insertedTapePath = NULL;
	multifaceCPCPath = NULL;
	multifacePLUSPath = NULL;

	for (i=0; i<16; i++)
	{
		insertedRomPath[i] = NULL;
	}

}

void configFree()
{
	int i;

	if (romDirectory)
		free(romDirectory);
	if (tapeDirectory)
		free(tapeDirectory);
	if (diskDirectory)
		free(diskDirectory);
	if (cartDirectory)
		free(cartDirectory);
	if (snapDirectory)
		free(snapDirectory);
	if (diskPathDriveA)
		free(diskPathDriveA);
	if (diskPathDriveB)
		free(diskPathDriveB);
	if (insertedCartPath)
		free(insertedCartPath);
	if (insertedTapePath)
		free(insertedTapePath);

	for (i=0; i<16; i++)
	{	
		if (insertedRomPath[i])
			free(insertedRomPath[i]);
	}
}

/* config file of form 
<variable>=<value>
*/

/* list of supported variables */
#define KEY_ROMDIR "romdir"	/* directory to open rom files from */
#define KEY_DISKDIR "diskdir"	/* directory to open disks from */
#define KEY_TAPEDIR "tapedir"	/* directory to open tapes from */
#define KEY_CARTDIR "cartdir"	/* directory to open cartridges from */
#define KEY_SNAPDIR "snapdir"	/* directory to open snapshots from */
#define KEY_REALTIME "realtime"	/* realtime speed */
#define KEY_DRIVEA	"drivea"	/* disk in drive A */
#define KEY_DRIVEB	"driveb"	/* disk in drive B */
#define KEY_CART	"cart"		/* cartridge inserted */
#define KEY_TAPE	"tape"		/* cassette inserted */
#define KEY_MONITORTYPE "monitortype"	/* monitor type */
#define KEY_MULTIFACECPC "mfcpcrom"	/* path to rom for CPC multiface */
#define KEY_MULTIFACEPLUS "mfplusrom" /* path to rom for CPC+ multiface */
#define KEY_ROM		"rom"		/* path for rom */

struct parse
{
	const char *sLine;
	int nPos;
};

static void setPath(char **pPath, const char *sNewPath)
{
	char *sPath;
	int nNewPathLength;

	sPath = *pPath;
	if (sPath)
		free(sPath);

	sPath = NULL;

	if (sNewPath!=NULL)
	{
		printf("%s\n",sNewPath);

		nNewPathLength = strlen(sNewPath);

		sPath = malloc(nNewPathLength+1);
		if (sPath)
		{
			strcpy(sPath, sNewPath);
		}
		printf("%s\n",sPath);		
	}
	
	*pPath = sPath;	
}

static char myPath[256];
const char *absolutePath(const char *path) {
	char *home;
	int i;
	if (path[0] == '~') {
		home = getenv("HOME");
		i = snprintf(myPath, 255, "%s", home);
		if (i<255) {
			i += snprintf(myPath+i, 255-i, "%s", sConfigPaths[0]+1);
		}
		myPath[i] = '\0';
		return myPath;
	} else {
		return sConfigPaths[0];
	}
}

void loadConfigFile() 
{
	int i;

	for (i=0; i<sizeof(sConfigPaths)/sizeof(sConfigPaths[0]); i++)
	{
		FILE *file;
		const char *sFilename = absolutePath(sConfigPaths[i]);
	
		fprintf(stdout, "Opening %s\n", sFilename);
		
		file = fopen(sFilename, "r");
		if (file == NULL) 
		{
			fprintf(stderr,"Failed to open %s\n",sFilename);
		}
		else
		{
			fprintf(stderr, "Parsing %s\n", sFilename);
			if (parseConfigFile(file) < 0) 
			{
				perror("Could not parse configfile");
			}
		
			fclose(file);

			return;
		}
	}
	fprintf(stderr,"No config files found!\n");
}

void saveConfigFile()
{
	FILE *fh;

	/* write a config file in user's space */
	fh = fopen(absolutePath(sConfigPaths[0]), "w");
	
	if (fh)
	{
		//printf("a");
		int i;

		if (diskDirectory)
			fprintf(fh,"diskdir=%s\n",diskDirectory);
		if (tapeDirectory)
			fprintf(fh,"tapedir=%s\n",tapeDirectory);
		if (cartDirectory)
			fprintf(fh,"cartdir=%s\n",cartDirectory);
		

		/*if (diskPathDriveA)	// FIXME: Disabled because of segv
			fprintf(fh,"drivea=%s\n",diskPathDriveA);
		if (diskPathDriveB)
			fprintf(fh,"driveb=%s\n",diskPathDriveB);
		if (insertedCartPath)
			fprintf(fh,"cart=%s\n",insertedCartPath);
		if (insertedTapePath)
			fprintf(fh,"tape=%s\n",insertedTapePath);
		if (multifaceCPCPath)
			fprintf(fh,"mfcpcrom=%s\n",multifaceCPCPath);
		if (multifacePLUSPath)
			fprintf(fh,"mfplusrom=%s\n",multifacePLUSPath);*/
		if (snapDirectory)
			fprintf(fh,"snapdir=%s\n",snapDirectory);

		for (i=0; i<16; i++)
		{
			if (insertedRomPath[i])
			{
				fprintf(fh,"rom%d=%s\n",i,insertedRomPath[i]);
			}
		}

		fclose(fh);
	}
}


int parseConfigFile(FILE *file) 
{
	const char *s;
	while ((s = getLine(file)) != NULL) 
	{
		parseLine(s);
	}
	return 0;
}

// read line from input file into 
const char *getLine(FILE *file) 
{
	return (fgets(buf, 1024, file));
}

// returns true if character is whitespace, false otherwise
int	iswhitespace(char ch)
{
	return ((ch==' ') || (ch=='\t'));
}

/* returns a buffer containing the variable name; buffer must be freed */
const char *getVariable(struct parse *parser)
{
	char *sVariable;

	char ch;
	int nVariableStart;
	int nVariableEnd;
	int nVariableLength;

	/* skip whitespace */
	do
	{	
		ch = parser->sLine[parser->nPos];
		parser->nPos++;
	} while (iswhitespace(ch));
	parser->nPos--;


	/* end of line already? */
	if (ch=='\0')
		return NULL;

	/* comment? */	
	if (ch=='#')
		return NULL;

	/* found first character of variable */
	nVariableStart = parser->nPos;
	
	/* variable is a digit, a letter or a digit */
	do
	{
		ch = parser->sLine[parser->nPos];
		parser->nPos++;
	} while (isalpha(ch) || (ch=='_') || (isdigit(ch)));
	parser->nPos--;

	nVariableEnd = parser->nPos;
		
	nVariableLength = nVariableEnd - nVariableStart;

	/* skip whitespace */
	do
	{	
		ch = parser->sLine[parser->nPos];
		parser->nPos++;
	} while (iswhitespace(ch));
	parser->nPos--;

	/* look for equals which seperates variable name from value */
	ch = parser->sLine[parser->nPos];
	if (ch!='=')
		return NULL;
	
	parser->nPos++;

	sVariable = malloc(nVariableLength+1);

	if (sVariable)
	{
		strncpy(sVariable, &parser->sLine[nVariableStart], nVariableLength);
		sVariable[nVariableLength] = '\0';
	}



	return sVariable;	
}

/* returns a buffer containing the parameter; buffer must be freed */
const char *getParameter(struct parse *parser)
{
	char ch;
	int nValueStart;
	int nValueEnd;
	int nValueLength;
	char *sValue;

	/* skip whitespace */
	do
	{	
		ch = parser->sLine[parser->nPos];
		parser->nPos++;
	} while (iswhitespace(ch));
	parser->nPos--;

	nValueStart = parser->nPos;

	parser->nPos = strlen(parser->sLine)-1;
	do
	{	
		ch = parser->sLine[parser->nPos];
		parser->nPos--;
	} while (iswhitespace(ch));
	parser->nPos++;

	nValueEnd = parser->nPos;

	if (nValueEnd>nValueStart)
	{
		nValueLength = nValueEnd-nValueStart;

		sValue = malloc(nValueLength+1);

		if (sValue)
		{
			strncpy(sValue, &parser->sLine[nValueStart], nValueLength);
			sValue[nValueLength] = '\0';
			return sValue;
		}
	}

	return NULL;
}



void parseLine(const char *s) 
{
	struct parse parser;
	parser.sLine = s;
	parser.nPos = 0;

	const char *sVariable = getVariable(&parser);
	
	if (sVariable)
	{			
		const char *sValue = getParameter(&parser);

		if (sValue==NULL)
		{
			printf("variable %s has no value.\n",sVariable);
		}
		else
		{
			printf("%s %s\n",sVariable, sValue);

			if (strcmp(sVariable, KEY_ROMDIR)==0)
			{

			}
			else 
			if (strcmp(sVariable, KEY_DISKDIR)==0)
			{
				setDiskDirectory(sValue);
			}
			else
			if (strcmp(sVariable, KEY_TAPEDIR)==0)
			{
				setTapeDirectory(sValue);
			}
			else
			if (strcmp(sVariable, KEY_SNAPDIR)==0)
			{
				setSnapDirectory(sValue);

			}
			else
			if (strcmp(sVariable, KEY_CARTDIR)==0)
			{
				setCartDirectory(sValue);
			}
			else
			if (strcmp(sVariable, KEY_MULTIFACECPC)==0)
			{
				Multiface_LoadRomFromFile(MULTIFACE_ROM_CPC_VERSION, sValue);
			}
			else
			if (strcmp(sVariable, KEY_MULTIFACEPLUS)==0)
			{
				Multiface_LoadRomFromFile(MULTIFACE_ROM_CPCPLUS_VERSION, sValue);
			}
			else
			if (strcmp(sVariable, KEY_DRIVEA)==0)
			{	
				GenericInterface_InsertDiskImage(0, sValue);
			}
			else
			if (strcmp(sVariable, KEY_DRIVEB)==0)
			{
				GenericInterface_InsertDiskImage(1, sValue);
			}
			else
			if (strcmp(sVariable, KEY_MONITORTYPE)==0)
			{

			}
			else
			if (strcmp(sVariable, KEY_TAPE)==0)
			{
				GenericInterface_InsertTape(sValue);
			}
			else
			if (strcmp(sVariable, KEY_CART)==0)
			{
				GenericInterface_InsertCartridge(sValue);
			}
			else
			/* this looks for "rom" as the first 3 letters of the
			variable. These should be checked *before* this variable,
			so they are not confused with this variable */
			/* the number part of the variable is used to determine
			the rom slot to insert the rom image into */
			if (strncmp(sVariable, KEY_ROM, strlen(KEY_ROM))==0)
			{
				/* rom0..rom15 */
				int nSlot = atoi(sVariable+strlen(KEY_ROM));

				GenericInterface_LoadRom(nSlot, sValue);
				
				ExpansionRom_SetActiveState(nSlot, 1);
			}	
			else
			{
				printf("%s is not recognised\n",sVariable);
			}

			free((void *)sValue);
		}

		free((void *)sVariable);
	}
}


const char *getRomDirectory() {
	return romDirectory;
}

const char *getDiskDirectory()
{
	return diskDirectory;
}

const char *getTapeDirectory() {
	return tapeDirectory;
}

const char *getCartDirectory() {
	return cartDirectory;
}

const char *getSnapDirectory() {
	return snapDirectory;
}

void setRomDirectory(const char *sPath) 
{
	setPath(&romDirectory, sPath);
}

void setDiskDirectory(const char *sPath)
{
	setPath(&diskDirectory, sPath);
}

void setTapeDirectory(const char *sPath)
{
	setPath(&tapeDirectory, sPath);
}

void setCartDirectory(const char *sPath)
{
	setPath(&cartDirectory, sPath);
}

void setDiskPathDriveA(const char *sPath)
{
	setPath(&diskPathDriveA, sPath);
}

void setDiskPathDriveB(const char *sPath)
{
	setPath(&diskPathDriveB, sPath);
}

void setInsertedCartPath(const char *sPath)
{
	setPath(&insertedCartPath, sPath);
}

void setInsertedTapePath(const char *sPath)
{
	setPath(&insertedTapePath, sPath);
}

void setInsertedRomPath(int nRomSlot, const char *sPath)
{
	printf("%s\n",sPath);
	setPath(&insertedRomPath[nRomSlot], sPath);
}

void setMultifaceCPCPath(const char *sPath) {
	setPath(&multifaceCPCPath, sPath);
}

void setMultifacePLUSPath(const char *sPath) {
	setPath(&multifacePLUSPath, sPath);
}

void setSnapDirectory(const char *sPath) {
	setPath(&snapDirectory, sPath);
}
