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
#ifndef __DETECT_WINDOWS_OS__
#define __DETECT_WINDOWS_OS

#define VER_DETECT_WIN31			0	/* Windows 3.1 with Win32s */
#define VER_DETECT_WIN9x			1	/* Windows 95 or 98 */
#define VER_DETECT_WINNT			2	/* Windows NT */

typedef struct	
{
	int WindowsOSType;
	int	MajorVersion;
	int MinorVersion;
	int BuildNumber;
} WINDOWS_OS_INFO;

WINDOWS_OS_INFO *DetectOS(void);

#endif