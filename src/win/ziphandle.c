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
/**************************************************************/
/* InfoZip support as DLL */

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include "ziphandle.h"
#include <windows.h>
#include "infozip\structs.h"
//#include "infozip\unzip.h"
//#include "infozip\decs.h"
#include <io.h>
#include <direct.h>
#include <string.h>
//#define WIN32
#include <stdlib.h>
#include <stdio.h>

#include "infozip\unzver.h"

#ifdef WIN32
#include <winver.h>
#else
#include <ver.h>
#endif


//#include <windows.h>
//#include <assert.h>    /* required for all Windows applications */
//#include <stdlib.h>
//#include <stdio.h>
//#include <commdlg.h>
//#include <dlgs.h>
//#include <windowsx.h>

//#include "structs.h"

/* Defines */
#ifndef MSWIN
#define MSWIN
#endif

typedef int (WINAPI * _DLL_UNZIP)(int, char **, int, char **,
                                  LPDCL, LPUSERFUNCTIONS);
typedef int (WINAPI * _USER_FUNCTIONS)(LPUSERFUNCTIONS);


HINSTANCE hUnZip = NULL;

_DLL_UNZIP	pwindll_unzip = NULL;

char	ZipDirectory[MAX_PATH+1];

void	ZipHandle_Init()
{
	char	CurrentDirectory[MAX_PATH+1];

	hUnZip = LoadLibrary("unzip32.dll");

	if (hUnZip!=NULL)
	{
		(_DLL_UNZIP)pwindll_unzip = (_DLL_UNZIP)GetProcAddress(hUnZip, "windll_unzip");
	}

	GetCurrentDirectory(MAX_PATH, CurrentDirectory);

	GetTempPath(MAX_PATH, ZipDirectory);

	_chdir(ZipDirectory);

	_mkdir("Arnold");

	strcat(ZipDirectory,"Arnold");

	SetCurrentDirectory(CurrentDirectory);
}

void	ZipHandle_Finish()
{
	if (hUnZip!=NULL)
	{
		FreeLibrary(hUnZip);
	}

	_chdir(ZipDirectory);

	/* delete all files in temp dir */
	DeleteAllFilesInCurrentDir();
}

/********************************************************************************/

/****************************************************************************************/

/* FOLLOWING RIPPED OUT OF EXAMPLE.C COMES WITH INFOZIP */

/* This is a very stripped down version of what is done in WiZ. Essentially
   what this function is for is to do a listing of an archive contents. It
   is actually never called in this example, but a dummy procedure had to
   be put in, so this was used.
 */
void WINAPI ReceiveDllMessage(unsigned long ucsize,unsigned long csiz,
        ush cfactor, ush mo, ush dy, ush yr, ush hh, ush mm,
    char c, char *filename, char *methbuf, unsigned long crc, char fCrypt)
{
char psLBEntry[PATH_MAX];
char LongHdrStats[] =
          "%7lu  %7lu %4s  %02u-%02u-%02u  %02u:%02u  %c%s";
char CompFactorStr[] = "%c%d%%";
char CompFactor100[] = "100%%";
char szCompFactor[10];
char sgn;

if (csiz > ucsize)
   sgn = '-';
else
   sgn = ' ';
if (cfactor == 100)
   lstrcpy(szCompFactor, CompFactor100);
else
   sprintf(szCompFactor, CompFactorStr, sgn, cfactor);
wsprintf(psLBEntry, LongHdrStats,
      ucsize, csiz, szCompFactor, mo, dy, yr, hh, mm, c, filename);

printf("%s\n", psLBEntry);
}

int WINAPI password(char *p, int n, const char *m, const char * name)
{
return 1;
}

/* Dummy "print" routine that simply outputs what is sent from the dll */
int WINAPI DisplayBuf(char far *buf, unsigned long size)
{
printf("%s", buf);
return (unsigned int) size;
}


int WINAPI GetReplaceDlgRetVal(char *filename)
{
/* This is where you will decide if you want to replace, rename etc existing
   files.
 */
return 1;
}

void	ZipHandle_UnZipFile(char *Filename, char *ExtractDir)
{
	char	*pFiles[1];
	char	DskExtension[]="*.dsk";

	DCL	MyDCL;
	USERFUNCTIONS	MyUserFunctions;

	memset(&MyDCL, 0, sizeof(DCL));

	MyDCL.ncflag = 0; /* Write to stdout if true */
	MyDCL.fQuiet = 2; /* We want all messages.
						  1 = fewer messages,
						  2 = no messages */
	MyDCL.ntflag = 0; /* test zip file if true */
	MyDCL.nvflag = 0; /* give a verbose listing if true */
	MyDCL.nUflag = 0; /* Do not extract only newer */
	MyDCL.nzflag = 0; /* display a zip file comment if true */
	MyDCL.ndflag = 1; /* Recreate directories if true */
	MyDCL.noflag = 1; /* Over-write all files if true */
	MyDCL.naflag = 0; /* Do not convert CR to CRLF */
	MyDCL.lpszZipFN = Filename; /* The archive name */
	MyDCL.lpszExtractDir = ExtractDir; /* The directory to extract to. This is set
									 to NULL if you are extracting to the
									 current directory.
								   */



	//memset(&MyUserFunctions, 0, sizeof(USERFUNCTIONS));
	MyUserFunctions.password = password;
	MyUserFunctions.print = DisplayBuf;
	MyUserFunctions.sound = NULL;
	MyUserFunctions.replace = GetReplaceDlgRetVal;
	MyUserFunctions.SendApplicationMessage = ReceiveDllMessage;


	if (pwindll_unzip!=NULL)
	{
		/* extract all files */
		if (pwindll_unzip(0,NULL, 0, NULL,  &MyDCL , &MyUserFunctions)!=0)
		{
			printf("Error unzipping");
		}
	}
}

