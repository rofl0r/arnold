// app.h : main header file for the TELNETSERVER application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "session.h"


/////////////////////////////////////////////////////////////////////////////
// CApp:
// See telnetserver.cpp for the implementation of this class
//

class CApp : public CWinApp
{
public:
	CApp();

   static CApp m_app;
   static CApp& GetApp();

   CArnoldTelnetServer m_server;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CApp)
	//}}AFX_MSG
   afx_msg LRESULT OnChar(LPARAM,WPARAM);
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
