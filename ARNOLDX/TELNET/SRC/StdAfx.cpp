// stdafx.cpp : source file that includes just the standard includes
//	telnetserver.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

BOOL WINAPI SetWindowTextV(HWND hwnd, LPCTSTR lpszFormat, va_list args)
{
   TCHAR sz[120], szFormat[120];
   BOOL bOK = (hwnd != NULL);
   if (bOK)
   {
      if (NULL == lpszFormat)
      {
         GetWindowText(hwnd, szFormat, _countof(szFormat) - 1);
         lpszFormat = szFormat;
      }

   #ifdef _WIN32_WCE
      wvsprintf(sz, lpszFormat, args);
   #else
      _vsntprintf(sz, _countof(sz) - 1, lpszFormat, args);
   #endif

#ifdef _WIN32
      bOK = SetWindowText(hwnd, sz);
#else
      SetWindowText(hwnd, sz);
#endif
   }
   return bOK;
}

BOOL MySetWindowText(HWND hwnd, LPCTSTR lpszFormat, ...)
{
   BOOL bOK;
   va_list args;
   va_start(args, lpszFormat);
   bOK = SetWindowTextV(hwnd, lpszFormat, args);
   va_end(args);
   return bOK;
}
