// status.h : main header file for the STATUS application
//

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CApp:
// See status.cpp for the implementation of this class
//

class CApp : public CWinApp
{
// Attributes
protected:
   static CApp m_app;

#ifdef _WIN32
   CCommandLineInfo m_cmdInfo;
#endif
public:
   CApp();

// Operations
public:
   static CApp& GetApp(void);
#ifdef _WIN32
   inline void ParseCommandLine   (void) {        CWinApp::ParseCommandLine   (m_cmdInfo); }
   inline BOOL ProcessShellCommand(void) { return CWinApp::ProcessShellCommand(m_cmdInfo); }
#endif

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CApp)
   public:
   virtual BOOL InitInstance();
   //}}AFX_VIRTUAL

// Implementation

public:
   //{{AFX_MSG(CApp)
   afx_msg void OnAppAbout();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
