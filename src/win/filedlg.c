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
// File Dialog Stuff
//
// Written by Mark Incley, October '96

#include <windows.h>
#include <commdlg.h>

#include "filedlg.h"

void InitFileDlg(HWND hwnd, OPENFILENAME *ofn, char *DefExt, char *Filter, DWORD Flags)
	{
	ofn->lStructSize		= sizeof(OPENFILENAME);
	ofn->hwndOwner			= hwnd;
	ofn->hInstance			= NULL;
	ofn->lpstrFilter		= Filter;
	ofn->lpstrCustomFilter	= 0;
	ofn->nMaxCustFilter		= 0;
	ofn->nFilterIndex		= 0;
	ofn->lpstrFile			= NULL;
	ofn->nMaxFile			= MAX_PATH;
	ofn->lpstrFileTitle		= NULL;
	ofn->nMaxFileTitle		= MAX_PATH;	/*MAX_FNAME + MAX_EXT;*/
	ofn->lpstrInitialDir	= NULL;
	ofn->lpstrTitle			= NULL;
	ofn->Flags				= Flags;
	ofn->nFileOffset		= 0;
	ofn->nFileExtension		= 0;
	ofn->lpstrDefExt		= DefExt;
	ofn->lCustData			= 0L;
	ofn->lpfnHook			= NULL;
	ofn->lpTemplateName		= NULL;

	return;
	}

BOOL GetFileNameFromDlg(HWND hwnd, OPENFILENAME *ofn, char *SuggestName, char *Title, char *Buffer, char *Directory)
	{
	ofn->hwndOwner		= hwnd;
	ofn->lpstrFile		= Buffer;
	ofn->Flags			&= (~OFN_HIDEREADONLY);
	ofn->lpstrTitle		= Title;
	ofn->lpstrInitialDir = Directory;

	return GetOpenFileName(ofn);
	}


BOOL GetSaveNameFromDlg(HWND hwnd, OPENFILENAME *ofn, char *SuggestName, char *Title, char *Buffer, char *Directory)
	{
	ofn->hwndOwner		= hwnd;
	ofn->lpstrFile		= Buffer;
	ofn->Flags			|= OFN_HIDEREADONLY;
	ofn->lpstrTitle		= Title;
	ofn->lpstrInitialDir = Directory;
	return GetSaveFileName(ofn);
	}


