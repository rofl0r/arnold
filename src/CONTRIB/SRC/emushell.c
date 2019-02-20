/* emushell.c */
/* Arnold shell implementation. Copyright (c) Troels K. 2003 */

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#include "../../win/precomp.h"

#include <direct.h>

#ifndef _countof
   #define _countof(array) (sizeof(array)/sizeof((array)[0]))
#endif

#include "..\include\emushell.h"

static const struct _ARNOLDPROFILE
{
   LPCTSTR szSection;
   LPCTSTR szPath_Snapshot;
   LPCTSTR szPath_Snapshot_Screen;
   LPCTSTR szPath_Rom;
   LPCTSTR aDisk[2];
   LPCTSTR aRom[16];
   LPCTSTR aRomActive[16];
} MOD_arnold_profile = 
{
   _T("Software\\Arnold"),
   _T("sSnapshotPath"),
   _T("sScreenSnapshotPath"),
   _T("sRomPath"),
   {
      _T("sDriveADiskImage"),
      _T("sDriveBDiskImage")
   },
   {
      _T("sRom 1"),
      _T("sRom 2"),
      _T("sRom 3"),
      _T("sRom 4"),
      _T("sRom 5"),
      _T("sRom 6"),
      _T("sRom 7"),
      _T("sRom 8"),
      _T("sRom 9"),
      _T("sRom10"),
      _T("sRom11"),
      _T("sRom12"),
      _T("sRom13"),
      _T("sRom14"),
      _T("sRom15"),
      _T("sRom16")
   },
   {
      _T("iRom 1Active"),
      _T("iRom 2Active"),
      _T("iRom 3Active"),
      _T("iRom 4Active"),
      _T("iRom 5Active"),
      _T("iRom 6Active"),
      _T("iRom 7Active"),
      _T("iRom 8Active"),
      _T("iRom 9Active"),
      _T("iRom10Active"),
      _T("iRom11Active"),
      _T("iRom12Active"),
      _T("iRom13Active"),
      _T("iRom14Active"),
      _T("iRom15Active"),
      _T("iRom16Active"),
   }
};

typedef struct _EMUIMPLEMENTATION
{
   HINSTANCE hInstance;
   TCHAR     szProfile[_MAX_PATH];
   TCHAR     szPath[_MAX_PATH];
} CPCEMUIMPL;

extern HINSTANCE ResourceInstance;

static HANDLE run_internal(EMUHANDLE handle, HWND hwndParent, int argc, TCHAR* argv[])
{
   TCHAR szDir[_MAX_PATH];
   _tgetcwd(szDir, _countof(szDir)); /* cwd is overwritten by Arnold */
   ArnoldMain(handle->impl->hInstance, SW_SHOW, argc, argv);
   _tchdir(szDir); /* restore cwd */
   return NULL;
}

const TCHAR* PUB_szProfileArnold;

static BOOL arnold_get_disk(EMUHANDLE handle, int no, LPTSTR lpszBuffer, int cbBuffer)
{
   BOOL bOK = (no >= 0) && (no < _countof(MOD_arnold_profile.aDisk));
   if (bOK && lpszBuffer)
   {
      HKEY key;
	   bOK = (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, handle->impl->szProfile, 0, KEY_READ, &key));
      if (bOK)
	   {
		   DWORD dwType, dwCount = cbBuffer * sizeof(TCHAR);
		   bOK = (ERROR_SUCCESS == RegQueryValueEx(key, MOD_arnold_profile.aDisk[no], NULL, 
                                    &dwType, (LPBYTE)lpszBuffer, &dwCount));
         if (bOK)
         {
		      bOK = (dwType == REG_SZ);
         }
		   RegCloseKey(key);
	   }
   }
   return bOK;
}

static BOOL arnold_put_disk(EMUHANDLE handle, int no, LPCTSTR lpszBuffer)
{
   BOOL bOK = (no >= 0) && (no < _countof(MOD_arnold_profile.aDisk)) && lpszBuffer;
   if (bOK)
   {
      HKEY key;
	   bOK = (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, handle->impl->szProfile, 0, KEY_ALL_ACCESS, &key));
      if (bOK)
	   {
	      bOK = (ERROR_SUCCESS == RegSetValueEx(key, MOD_arnold_profile.aDisk[no], 0, 
                     REG_SZ, (LPBYTE)lpszBuffer, (lstrlen(lpszBuffer)+1)*sizeof(TCHAR)));
		   RegCloseKey(key);
	   }
   }
   return bOK;
}

static BOOL arnold_get_rom(EMUHANDLE handle, int no, LPTSTR lpszBuffer, int cbBuffer)
{
   BOOL bOK = (no >= 0) && (no < _countof(MOD_arnold_profile.aRom));
   if (bOK && lpszBuffer)
   {
      HKEY key;

	   bOK = (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, handle->impl->szProfile, 0, KEY_READ, &key));
      if (bOK)
	   {
         DWORD dwType, dwCount;
         if (bOK)
         {
		      BOOL bActive;
            dwCount = sizeof(bActive);
            bOK = (ERROR_SUCCESS == RegQueryValueEx(key, MOD_arnold_profile.aRomActive[no], NULL, &dwType, (LPBYTE)&bActive, &dwCount));
            if (bOK)
            {
		         bOK = (dwType == REG_DWORD);
            }
            bOK = bOK && bActive;
         }
         if (bOK)
         {
            dwCount = cbBuffer * sizeof(TCHAR);
		      bOK = (ERROR_SUCCESS == RegQueryValueEx(key, MOD_arnold_profile.aRom[no], NULL, 
                                       &dwType, (LPBYTE)lpszBuffer, &dwCount));
            if (bOK)
            {
		         bOK = (dwType == REG_SZ);
            }
         }
		   RegCloseKey(key);
	   }
   }
   return bOK;
}

static BOOL arnold_put_rom(EMUHANDLE handle, int no, LPCTSTR lpszBuffer)
{
   BOOL bOK = (no >= 0) && (no < _countof(MOD_arnold_profile.aRom)) && lpszBuffer;
   if (bOK)
   {
      HKEY key;

	   bOK = (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, handle->impl->szProfile, 0, KEY_ALL_ACCESS, &key));
      if (bOK)
	   {
	      bOK = (ERROR_SUCCESS == RegSetValueEx(key, MOD_arnold_profile.aRom[no], 0, 
            REG_SZ, (LPBYTE)lpszBuffer, (lstrlen(lpszBuffer)+1)*sizeof(TCHAR)));
         if (bOK)
         {
            DWORD bEnable = TRUE;
            bOK = (ERROR_SUCCESS == RegSetValueEx(key, MOD_arnold_profile.aRomActive[no], 0, REG_DWORD, (LPBYTE)(&bEnable), sizeof(bEnable)));
         }
		   RegCloseKey(key);
	   }
   }
   return bOK;
}

static BOOL set_path(EMUHANDLE handle, enum emupath index, LPCTSTR lpszBuffer)
{
   BOOL bOK = (NULL != lpszBuffer);
   if (bOK)
   {
      switch (index)
      {
         case ENUM_emupath_exe:
            _tmakepath(handle->impl->szPath, NULL, lpszBuffer, NULL, NULL);
            break;
         case ENUM_emupath_rom:
         {
            HKEY key;
	         bOK = (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, handle->impl->szProfile, 0, KEY_ALL_ACCESS, &key));
            if (bOK)
	         {
	            bOK = (ERROR_SUCCESS == RegSetValueEx(key, MOD_arnold_profile.szPath_Rom, 0, 
                           REG_SZ, (LPBYTE)lpszBuffer, (lstrlen(lpszBuffer)+1)*sizeof(TCHAR)));
		         RegCloseKey(key);
	         }
            break;
         }
         case ENUM_emupath_snapshot:
         {
            HKEY key;
	         bOK = (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, handle->impl->szProfile, 0, KEY_ALL_ACCESS, &key));
            if (bOK)
	         {
	            bOK = (ERROR_SUCCESS == RegSetValueEx(key, MOD_arnold_profile.szPath_Snapshot, 0, 
                           REG_SZ, (LPBYTE)lpszBuffer, (lstrlen(lpszBuffer)+1)*sizeof(TCHAR)));
		         RegCloseKey(key);
	         }
            break;
         }
         case ENUM_emupath_snapshot_screen:
         {
            HKEY key;
	         bOK = (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, handle->impl->szProfile, 0, KEY_ALL_ACCESS, &key));
            if (bOK)
	         {
	            bOK = (ERROR_SUCCESS == RegSetValueEx(key, MOD_arnold_profile.szPath_Snapshot_Screen, 0, 
                           REG_SZ, (LPBYTE)lpszBuffer, (lstrlen(lpszBuffer)+1)*sizeof(TCHAR)));
		         RegCloseKey(key);
	         }
            break;
         }
         default:
            bOK = FALSE;
            break;
      }
   }
   return bOK;
}

static BOOL get_path(EMUHANDLE handle, enum emupath index, LPTSTR lpszBuffer, int cbBuffer)
{
   BOOL bOK = lpszBuffer && (cbBuffer >= _MAX_PATH);
   if (bOK && lpszBuffer)
   {
      switch (index)
      {
         case ENUM_emupath_exe:
         {
            TCHAR szDrive[_MAX_PATH], szDir[_MAX_PATH];
      
            _tsplitpath(handle->impl->szPath, szDrive, szDir, NULL      , NULL);
            _tmakepath(lpszBuffer, szDrive, szDir, NULL, NULL);
            break;
         }
         case ENUM_emupath_rom:
         {
            HKEY key;

	         bOK = (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, handle->impl->szProfile, 0, KEY_READ, &key));
            if (bOK)
	         {
		         DWORD dwType, dwCount = cbBuffer * sizeof(TCHAR);
		         bOK = (ERROR_SUCCESS == RegQueryValueEx(key, MOD_arnold_profile.szPath_Rom, NULL, 
                                          &dwType, (LPBYTE)lpszBuffer, &dwCount));
               if (bOK)
               {
		            bOK = (dwType == REG_SZ);
               }
		         RegCloseKey(key);
	         }
            if (bOK && (!*lpszBuffer))
            {
               TCHAR szDrive[_MAX_PATH], szDir[_MAX_PATH];
      
               _tsplitpath(handle->impl->szPath, szDrive, szDir, NULL, NULL);
               _tmakepath(lpszBuffer, szDrive, szDir, _T("ROMS"), NULL);
               _tmkdir(lpszBuffer);
            }
            break;
         }
         case ENUM_emupath_snapshot:
         {
            HKEY key;

	         bOK = (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, handle->impl->szProfile, 0, KEY_READ, &key));
            if (bOK)
	         {
		         DWORD dwType, dwCount = cbBuffer * sizeof(TCHAR);
		         bOK = (ERROR_SUCCESS == RegQueryValueEx(key, MOD_arnold_profile.szPath_Snapshot, NULL, 
                                          &dwType, (LPBYTE)lpszBuffer, &dwCount));
               if (bOK)
               {
		            bOK = (dwType == REG_SZ);
               }
		         RegCloseKey(key);
	         }
            break;
         }
         case ENUM_emupath_snapshot_screen:
         {
            HKEY key;

	         bOK = (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, handle->impl->szProfile, 0, KEY_READ, &key));
            if (bOK)
	         {
		         DWORD dwType, dwCount = cbBuffer * sizeof(TCHAR);
		         bOK = (ERROR_SUCCESS == RegQueryValueEx(key, MOD_arnold_profile.szPath_Snapshot_Screen, NULL, 
                                          &dwType, (LPBYTE)lpszBuffer, &dwCount));
               if (bOK)
               {
		            bOK = (dwType == REG_SZ);
               }
		         RegCloseKey(key);
	         }
            break;
         }
         default:
            bOK = FALSE;
            break;
      }
   }
   return bOK;
}

static int cpcemu_get_name(EMUHANDLE handle, LPTSTR lpszBuffer, int cbBuffer)
{
   return _tcslen(_tcsncpy(lpszBuffer, _T("Arnold"), cbBuffer));
}

static BOOL cpcemu_get_fileprofile(EMUHANDLE handle, LPTSTR lpszBuffer, int cbBuffer)
{
   return FALSE;
}

static void cpcemu_put_exe_path(EMUHANDLE handle, LPCTSTR lpszBuffer)
{
   _tcsncpy(handle->impl->szPath, lpszBuffer, _countof(handle->impl->szPath));
}

static void cpcemu_close(EMUHANDLE* handle)
{
   free((*handle)->impl);
   free(*handle);
   *handle = NULL;
}

HANDLE EMUSHELL_Run(HWND hwnd, LPCTSTR lpszFile, LPCTSTR lpszParm, LPCTSTR lpszDir)
{
   HANDLE handle = NULL;
   SHELLEXECUTEINFO parm;
   parm.cbSize      = sizeof(parm);
   parm.fMask       = SEE_MASK_NOCLOSEPROCESS; 
   parm.hwnd        = hwnd; 
   parm.lpVerb      = _T("open"); 
   parm.lpFile      = lpszFile;
   parm.lpParameters= lpszParm; 
   parm.lpDirectory = lpszDir; 
   parm.nShow       = SW_SHOW; 
   parm.hInstApp    = NULL; 
   
   // Optional members 
   parm.lpIDList    = NULL; 
   parm.lpClass     = NULL; 
   parm.hkeyClass   = NULL; 
   parm.dwHotKey    = 0; 
   parm.hIcon       = NULL; 
   parm.hProcess    = NULL;
   if (ShellExecuteEx(&parm))
   {
      handle = parm.hProcess;
   }
   return handle;
}

static HANDLE run_external(EMUHANDLE handle, HWND hwndParent, int argc, TCHAR* argv[])
{
   /* arnold is sensitive to work dir */
   TCHAR szDrive[_MAX_PATH], szDir[_MAX_PATH], szTitle[_MAX_PATH], szExt[_MAX_PATH];
   _tsplitpath(handle->impl->szPath, szDrive, szDir, szTitle, szExt);
   _tcscat(szDrive, szDir);
   _tcscat(szTitle, szExt);
   //return ((int)ShellExecute(hwndParent, NULL, szTitle, NULL, szDrive, SW_SHOW) > 32);
   return EMUSHELL_Run(hwndParent, szTitle, NULL, szDrive);
}

static EMUAPI MOD_InternalAPI;
static EMUHANDLE create_internal(LPCTSTR lpszPath)
{
   HKEY key;
	DWORD DispositionValue;
   EMUHANDLE handle = (EMUSHELL*)malloc(sizeof(EMUSHELL));

   handle->api  = &MOD_InternalAPI;
   handle->impl = (CPCEMUIMPL*)malloc(sizeof(CPCEMUIMPL));
   handle->impl->hInstance = lpszPath ? (HINSTANCE)lpszPath : ResourceInstance;
   GetModuleFileName(handle->impl->hInstance, handle->impl->szPath, _countof(handle->impl->szPath));
   _sntprintf(handle->impl->szProfile, _countof(handle->impl->szProfile), _T("%s\\%s"), _T("Software"), PUB_szProfileArnold);

	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, handle->impl->szProfile, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS ,NULL, &key, &DispositionValue))
	{
		RegCloseKey(key);
	}

   return handle;
}

static EMUAPI MOD_ExternalAPI;
static EMUHANDLE create_external(LPCTSTR lpszPath)
{
   HKEY key;
	DWORD DispositionValue;
   EMUHANDLE handle = (EMUSHELL*)malloc(sizeof(EMUSHELL));

   handle->api  = &MOD_ExternalAPI;
   handle->impl = (CPCEMUIMPL*)malloc(sizeof(CPCEMUIMPL));
   handle->impl->hInstance = ResourceInstance;
   _tcscpy(handle->impl->szPath, lpszPath ? lpszPath : _T(""));
   _sntprintf(handle->impl->szProfile, _countof(handle->impl->szProfile), _T("%s\\%s"), _T("Software"), _T("Arnold"));

	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, handle->impl->szProfile, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS ,NULL, &key, &DispositionValue))
	{
		RegCloseKey(key);
	}

   return handle;
}

static EMUAPI MOD_InternalAPI =
{
   create_internal, NULL, run_internal, arnold_get_disk, arnold_put_disk, arnold_get_rom, 
   arnold_put_rom, cpcemu_get_name, cpcemu_get_fileprofile, get_path, set_path, cpcemu_close
};

static EMUAPI MOD_ExternalAPI =
{
   create_external, cpcemu_put_exe_path, run_external, arnold_get_disk, arnold_put_disk, arnold_get_rom, 
   arnold_put_rom, cpcemu_get_name, cpcemu_get_fileprofile, get_path, set_path, cpcemu_close
};

const EMUAPI* EMUSHELL_ArnoldInternal(void)
{
   return &MOD_InternalAPI;
}

const EMUAPI* EMUSHELL_ArnoldExternal(void)
{
   return &MOD_ExternalAPI;
}
