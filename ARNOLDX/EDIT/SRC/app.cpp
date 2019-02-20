// app.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "app.h"

#include "frame.h"

#include "edit.h"
#include "editfrm.h"
#include "helpers.h"
#include "myview.h"
#include "mydoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CApp Instance

/*static*/ CApp  CApp::m_app;
/*static*/ CApp& CApp::GetApp(void)
{ 
   return m_app; 
}

/////////////////////////////////////////////////////////////////////////////
// CApp

BEGIN_MESSAGE_MAP(CApp, CWinApp)
   //{{AFX_MSG_MAP(CApp)
   ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
   //}}AFX_MSG_MAP
   // Standard file based document commands
   ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
   ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
   // Standard print setup command
   ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApp construction

CApp::CApp() : CWinApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// CApp initialization

BOOL CApp::InitInstance()
{
   CFrame* pMainFrame = NULL;
   CMyMultiDocTemplate* pDefaultDocTemplate = NULL;
   BOOL bOK = CWinApp::InitInstance();
   if (bOK)
   {
      // Standard initialization
   #ifdef _WIN32
   #ifdef _AFXDLL
      Enable3dControls();        // Call this when using MFC in a shared DLL
   #else
      Enable3dControlsStatic();  // Call this when linking to MFC statically
   #endif
      SetRegistryKey(_T("troelsk"));
   #endif

      LoadStdProfileSettings();  // Load standard INI file options (including MRU)

      // Register the application's document templates.  Document templates
      //  serve as the connection between documents, frame windows and views.

      //delete m_pDocManager;      m_pDocManager = new CMyDocManager;
      
      CMyMultiDocTemplate* pDocTemplate;
      
      pDefaultDocTemplate = pDocTemplate = new CMyMultiDocTemplate(
         IDR_TYPE_EDIT,
         RUNTIME_CLASS(CMyDocument/*CxEditDoc*/),
         RUNTIME_CLASS(CEditFrame), // custom MDI child frame
         RUNTIME_CLASS(/*CxEditView*/CMyView));
      AddDocTemplate(pDocTemplate);

      // create main MDI Frame window
      pMainFrame = new CFrame;
      bOK = pMainFrame->LoadFrame(CFrame::IDR, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE | FWS_PREFIXTITLE);

      if (bOK)
      {
         m_pMainWnd = pMainFrame;
      }
      else
      {
         delete pMainFrame;
      }
   }
#ifdef _WIN32
   if (bOK)
   {
      // Parse command line for standard shell commands, DDE, file open
      ParseCommandLine();
      if (m_cmdInfo.m_nShellCommand == m_cmdInfo.FileNew)
      {
         m_cmdInfo.m_nShellCommand = m_cmdInfo.FileNothing;
         pDefaultDocTemplate->OpenDocumentFile(NULL);
      }

      // Dispatch commands specified on the command line
      bOK = ProcessShellCommand();
   }
#endif
   if (bOK)
   {
      // The main window has been initialized, so show and update it.
      pMainFrame->ShowWindow(m_nCmdShow);
      pMainFrame->UpdateWindow();
   }
   return bOK;
}

/////////////////////////////////////////////////////////////////////////////
// CApp message handlers

void CApp::OnAppAbout()
{
   CDialog(IDD_ABOUTBOX, m_pMainWnd).DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CApp message handlers

