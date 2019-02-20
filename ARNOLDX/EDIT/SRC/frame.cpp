// frame.cpp : implementation of the CFrame class
//

#include "stdafx.h"
#include "resource.h"
#include "helpers.h"
#include "frame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrame

IMPLEMENT_DYNAMIC(CFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CFrame, CMDIFrameWnd)
   //{{AFX_MSG_MAP(CFrame)
   ON_WM_CREATE()
   ON_UPDATE_COMMAND_UI(ID_INDICATOR_POSITION, OnUpdatePosition)
   //}}AFX_MSG_MAP
   ON_COMMAND_EX(ID_ESCAPE, OnEscape)
   ON_COMMAND_EX(ID_WINDOW_CLOSE, OnMDIWindowCmd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrame construction/destruction

CFrame::CFrame() : CMDIFrameWnd()
{
}

CFrame::~CFrame()
{
}

#ifdef _WIN32
int CFrame::CMyToolBar::OnToolHitTest(CPoint pt, TOOLINFO* pTI) const
{
   int nHit = CToolBar::OnToolHitTest(pt, pTI);
   if (-1 == nHit) nHit = MFC_ControlBar_OnToolHitTest(*this, pt, pTI, FALSE);
   return nHit;
}
#endif

int CFrame::OnCreate(LPCREATESTRUCT lpcs)
{
   static const UINT BASED_CODE indicators[] =
   {
      ID_SEPARATOR,           // status line indicator
      ID_INDICATOR_POSITION,
      ID_INDICATOR_CAPS,
      ID_INDICATOR_NUM,
      ID_INDICATOR_OVR,
   };

   static const UINT BASED_CODE buttons[] =
   {
      ID_FILE_NEW,
      ID_FILE_OPEN,
      ID_FILE_SAVE,
      ID_SEPARATOR,
      ID_FILE_PRINT,
      ID_FILE_PRINT_PREVIEW,
      ID_SEPARATOR,
      ID_EDIT_CUT,
      ID_EDIT_COPY,
      ID_EDIT_PASTE,
      ID_SEPARATOR,
      ID_APP_ABOUT,
   };

   if (CMDIFrameWnd::OnCreate(lpcs) == -1)
   {
      return -1;
   }
   DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP;
#ifdef _WIN32
   dwStyle|=CBRS_TOOLTIPS ;
#endif
   if (   !m_wndToolBar.Create(this, dwStyle)
#ifdef _WIN32
       || !m_wndToolBar.LoadToolBar(IDR))
#else
       || !m_wndToolBar.LoadBitmap(IDR)
       || !m_wndToolBar.SetButtons(buttons, _countof(buttons)))
#endif
   {
      TRACE0("Failed to create toolbar\n");
      return -1;      // fail to create
   }
   m_wndToolBar.SetWindowText(_T("Standard"));
   
   dwStyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM;
#ifdef _WIN32
   dwStyle|=CBRS_TOOLTIPS ;
#endif
   if (!m_wndStatusBar.Create(this, dwStyle) || !m_wndStatusBar.SetIndicators(indicators, _countof(indicators)))
   {
      TRACE0("Failed to create status bar\n");
      return -1;      // fail to create
   }

#ifdef _WIN32
   m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
   EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_wndToolBar);
#endif
   return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CFrame diagnostics

#ifdef _DEBUG
void CFrame::AssertValid() const
{
   CMDIFrameWnd::AssertValid();
}

void CFrame::Dump(CDumpContext& dc) const
{
   CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

BOOL CFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
   BOOL bOK = CMDIFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext);
   if (bOK)
   {
      CString str = GetTitle();
   #ifdef _DEBUG
      str+=_T(" (Debug)");
   #endif   
      SetTitle(str);
      SetWindowText(str);
   }
   return bOK;
}

void CFrame::OnUpdatePosition(CCmdUI* pCmdUI) 
{
   pCmdUI->SetText(&afxChNil);
}

BOOL CFrame::OnEscape(UINT) 
{
   BOOL bHandled = FALSE;
   CWnd* pWnd = MDIGetActive();
   if (NULL != pWnd)
   {
      pWnd->PostMessage(WM_SYSCOMMAND, SC_CLOSE);
      bHandled = TRUE;
   }
   return bHandled;
}

BOOL CFrame::OnMDIWindowCmd(UINT nID)
{
   BOOL bHandled = FALSE;
   ASSERT(m_hWndMDIClient != NULL);

   switch (nID)
   {
      case ID_WINDOW_CLOSE:
         bHandled = OnEscape(nID);
         break;
      default:
         break;
   }
   if (!bHandled) bHandled = CMDIFrameWnd::OnMDIWindowCmd(nID);
   return bHandled;
}

