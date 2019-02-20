// app.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "app.h"
#include "telnetdlg.h"
#include "session.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CApp

BEGIN_MESSAGE_MAP(CApp, CWinApp)
	//{{AFX_MSG_MAP(CApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
   ON_THREAD_MESSAGE(WM_USER_CHAR, OnChar)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApp construction

CApp::CApp() : CWinApp(), m_server()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CApp object

/*static*/ CApp CApp::m_app;
/*static*/ CApp& CApp::GetApp()
{
   return m_app;
}

/////////////////////////////////////////////////////////////////////////////
// CApp initialization

#include "..\..\..\arnold\src\win\myapp.h"

BOOL CApp::InitInstance()
{
   AfxSocketInit();

   // Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
   SetRegistryKey(_T("Arnold"));
   const int port    = GetProfileInt  (_T("Settings"), _T("Port"), IPPORT_TELNET);
                       WriteProfileInt(_T("Settings"), _T("Port"), port);
   const int maxuser = GetProfileInt  (_T("Settings"), _T("MaxUserCount"), 1);
                       WriteProfileInt(_T("Settings"), _T("MaxUserCount"), maxuser);

   BOOL bOK = m_server.Create((UINT)port, maxuser);
   if (bOK)
   {
      CTelnetDialog dlg;
	   m_pMainWnd = &dlg;
	   dlg.DoModal();
	   m_pMainWnd = NULL;
   }
   else
   {
      AfxMessageBox(_T("Failed to create server"), MB_OK | MB_ICONERROR);
   }
	
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

LRESULT CApp::OnChar(LPARAM p,WPARAM ch)
{
   ((CArnoldTelnetSession*)p)->Send((char)ch);
   return 0;
}
