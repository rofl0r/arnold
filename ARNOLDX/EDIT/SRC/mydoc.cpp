// mydoc.cpp : implementation file
//

#include "stdafx.h"
#include "edit.h"
#include "mydoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyDocument

IMPLEMENT_DYNCREATE(CMyDocument, CxEditDoc)

CMyDocument::CMyDocument() : CxEditDoc(), m_handle(NULL)
{
}

BOOL CMyDocument::OnNewDocument()
{
	BOOL bOK = CxEditDoc::OnNewDocument();
   if (bOK)
   {
   }
	return bOK;
}

CMyDocument::~CMyDocument()
{
   if (m_handle != NULL) (*m_handle->api->close)(&m_handle);
}

BEGIN_MESSAGE_MAP(CMyDocument, CxEditDoc)
	//{{AFX_MSG_MAP(CMyDocument)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
void CMyDocument::SetModifiedFlag(BOOL bModified)
{
   //CxEditDoc::SetModifiedFlag(bModified);
}
*/

BOOL CMyDocument::CanCloseFrame(CFrameWnd* pFrame)
{
   // return CxEditDoc::CanCloseFrame(pFrame); default behaviour is annoying here
   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

