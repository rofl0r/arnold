// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxsock.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <process.h>			// 
#include <afxtempl.h>			// 

#define _countof(a) (sizeof(a)/sizeof((a)[0]))

#define WM_USER_CHAR (WM_USER+123)

extern BOOL MySetWindowText(HWND hwnd, LPCTSTR lpszFormat, ...);
