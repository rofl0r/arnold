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
#include <windows.h>
#include "detectos.h"

static WINDOWS_OS_INFO	WindowsOSInfo;

WINDOWS_OS_INFO *DetectOS()
{
	OSVERSIONINFO	OsVersionInfo;

    OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	/* get version info */
	if (GetVersionEx(&OsVersionInfo))
	{
		if (OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32s)
		{
			/* Windows 3.1 */
			WindowsOSInfo.WindowsOSType = VER_DETECT_WIN31;
		}

		if (OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			/* Windows NT */
			WindowsOSInfo.WindowsOSType = VER_DETECT_WINNT;
			WindowsOSInfo.MinorVersion = OsVersionInfo.dwMinorVersion;
			WindowsOSInfo.MajorVersion = OsVersionInfo.dwMajorVersion;
			WindowsOSInfo.BuildNumber = OsVersionInfo.dwBuildNumber;
		}

		if (OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		{
			/* Windows 9x */
			WindowsOSInfo.WindowsOSType = VER_DETECT_WIN9x;
			WindowsOSInfo.MajorVersion = OsVersionInfo.dwMajorVersion;
			WindowsOSInfo.MinorVersion = OsVersionInfo.dwBuildNumber>>16;
			WindowsOSInfo.BuildNumber = OsVersionInfo.dwBuildNumber & 0x0ffff;
		}

		return &WindowsOSInfo;
	}

	return NULL;
}
