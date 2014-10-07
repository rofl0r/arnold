// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define VC_EXTRALEAN    // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdlgs.h>         // MFC extensions
#ifdef _WIN32
   #include <afxrich.h>         // 
#else
   #define _AFX_NO_RICHEDIT_SUPPORT
   extern TCHAR afxChNil;
   #define ASSERT_KINDOF(class_name, object) ASSERT((object)->IsKindOf(RUNTIME_CLASS(class_name)))
   #define FWS_PREFIXTITLE 0
   #define GetWindowTextLengthA GetWindowTextLength
   #define GetWindowTextA       GetWindowText
#endif
#include <process.h>         // MFC extensions

#ifndef _countof
   #define _countof(array) (sizeof(array)/sizeof((array)[0]))
#endif


#define WM_USER_CHAR    (WM_USER+123)
#define WM_USER_SETMODE (WM_USER+124)

#ifndef ON_MESSAGE_VOID
#define ON_MESSAGE_VOID(message, memberFxn) \
	{ message, 0, 0, 0, AfxSig_vv, \
		(AFX_PMSG)(AFX_PMSGW)(void (AFX_MSG_CALL CWnd::*)(void))&memberFxn },
#endif
