// editframe.cpp : implementation file
//

#include "stdafx.h"
#include "editfrm.h"
#include "helpers.h"

IMPLEMENT_DYNCREATE(CEditFrame, CMDIChildWnd)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditFrame

CEditFrame::CEditFrame() : CMDIChildWnd()
{
}

CEditFrame::~CEditFrame()
{
}

BEGIN_MESSAGE_MAP(CEditFrame, CMDIChildWnd)
   //{{AFX_MSG_MAP(CEditFrame)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditFrame message handlers

BOOL CEditFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
   BOOL bOK = CMDIChildWnd::PreCreateWindow(cs);
   if (bOK)
   {
	   cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | FWS_ADDTOTITLE | WS_MINIMIZEBOX;
      CSize ext;
      if (::MFC_FrameWnd_GetMDIClientSize(&ext))
      {
         cs.x = cs.y = 0, cs.cx = ext.cx, cs.cy = ext.cy;
      }
   }  
   return bOK;
}
