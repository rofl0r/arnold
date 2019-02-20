// edit.cpp
//

#include "stdafx.h"
#include "app.h"

#include "edit.h"

#include "helpers.h"

IMPLEMENT_DYNCREATE(CxEditView, CEditView)
IMPLEMENT_DYNCREATE(CxEditDoc, CDocument)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CxEdit

void CxEdit::OnEditClearAll() 
{
   Empty();
}

void CxEdit::OnUpdateEditClearAll(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(!IsEmpty());
   OnUpdateCanModify(pCmdUI);
}

void CxEdit::OnUpdateCanModify(CCmdUI* pCmdUI)
{
   if (IsStyleReadOnly())
   {
      pCmdUI->Enable(FALSE);
   }
}

void CxEdit::OnUpdatePosition(CCmdUI* pCmdUI)
{
   CString str, strFormat;
   strFormat.LoadString(pCmdUI->m_nID);
   str.Format(strFormat, 
      GetRow() + 1, 
      GetCol() + 1
      );
   pCmdUI->SetText(str);
}

CString CxEdit::GetSelText(void) const
{
   CString str;
   int nStartChar, nEndChar;
   GetSel(nStartChar, nEndChar);
   if (nStartChar != nEndChar)
   {
      GetWindowText(str);
      str = str.Mid(nStartChar, nEndChar - nStartChar);
   }
   return str;
}

void CxEdit::OnUpdateReadOnly(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(IsStyleReadOnly() ? 1 : 0);
}

int CxEdit::GetCol(void) const 
{ 
   int begin = 0, end = 0;
   GetSel(begin, end);
   int nRow = LineFromChar();
   int nCol = begin - LineIndex(nRow);
   return nCol;
}

BOOL CxEdit::IsSelection(void) const
{
   int begin, end;
   GetSel(begin, end);
   return (begin != end);
}

int CxEdit::DeleteLine(int nLine, BOOL bScroll, int nCount /*= 1*/)
{
   if (nCount >= 1)
   {
      const int linecount = GetLineCount();
      const BOOL bFirstLine = (nLine == 0);
      const BOOL bLastLine  = (nLine == (linecount - 1));
      
//TRACE("%i,%i,%i\n", LineIndex(nLine - 1), LineLength(nLine - 1), LineIndex(nLine));

      const int nStartChar = (bLastLine && (!bFirstLine)) 
         ? (LineIndex(nLine - 1) + LineLength(nLine - 1) + 0) 
         : LineIndex(nLine);
      const int nEndChar   = ((nLine + nCount) >= linecount) 
         ? (LineIndex(linecount - 1) + LineLength(linecount - 1) + 0)
         : LineIndex(nLine + nCount);
      const BOOL bReadOnly = IsStyleReadOnly();

      if (bReadOnly)
      {
         SetReadOnly(FALSE);
      }
      SetSel(nStartChar, nEndChar, !bScroll); // end of edit text
      Clear();        // ..then delete
      GotoEnd(bScroll);
      if (bReadOnly)
      {
         SetReadOnly(TRUE);
      }
   }
   return GetLineCount();
}

void CxEdit::GotoLine(int nLine, BOOL bScroll /*= TRUE*/)
{
   const int lastline = LineIndex(nLine);
   SetSel(lastline, lastline, !bScroll);
}

int CxEdit::GotoBegin(BOOL bScroll /*= TRUE*/)
{
   SetSel(0, 0, !bScroll); // end of edit text
   return 0;
}
int CxEdit::GotoEnd(BOOL bScroll /*= TRUE*/)
{
   const int length = GetWindowTextLength();
   SetSel(length, length, !bScroll); // end of edit text
   return LineFromChar(length);
}

int CxEdit::GetLine(int nLine, CString* str) const
{
   LPTSTR lpsz = str->GetBuffer(512);
   const int len = CEdit::GetLine(nLine, lpsz, 512 - 1);
   lpsz[len] = '\0';
   str->ReleaseBuffer();
   return len;
}

int CxEdit::AddString(LPCTSTR lpsz, BOOL bScroll /*= TRUE*/, BOOL bCanUndo /*= FALSE*/)
{
   const int line = GotoEnd(bScroll);
   ReplaceSel(lpsz, bCanUndo);
   return line;
}

BOOL CxEdit::InsertString(int nLine, LPCTSTR lpsz, BOOL bScroll /*= TRUE*/, BOOL bCanUndo /*= FALSE*/)
{
   const int linecount = GetLineCount();
   BOOL bOK = (nLine <= linecount);
   if (bOK)
   {
      if (nLine == linecount)
      {
         AddString(_T("\r\n"), FALSE, bCanUndo);
      }
      const int index = LineIndex(nLine);
      SetSel(index, index, !bScroll);
      ReplaceSel(lpsz, bCanUndo);
   }
   return bOK;
}

BOOL CxEdit::LineSwap(int line0, int line1, BOOL bScroll /*= TRUE*/, BOOL bCanUndo /*= FALSE*/)
{
   BOOL bOK = TRUE;
   if (line0 != line1)
   {
      const int linecount = GetLineCount();
      CString str[2];
      int line[2];
      line[0] = min(line0, line1);
      line[1] = max(line0, line1);

      bOK = (linecount > line[1]);
      if (bOK)
      {
         GetLine(line[0], &str[0]);
         GetLine(line[1], &str[1]);
         DeleteLine(line[1], FALSE);
         DeleteLine(line[0], FALSE);
//return bOK;
         CString temp = str[1] + _T("\r\n");
         InsertString(line[0], temp, FALSE, bCanUndo);

         temp = str[0] + ((line[1] == (linecount - 1)) ? &afxChNil : _T("\r\n"));
         InsertString(line[1], temp, bScroll, bCanUndo);
      }
   }
   return bOK;
}

// CEdit::LineLength is wrong!
int CxEdit::LineLength(int nLine /*= -1*/) const
{
   int len;
   if (nLine == -1)
   {
      len = CEdit::LineLength(nLine);
   }
   else
   {
      CString str;
      GetLine(nLine, &str);
      len = str.GetLength();
   }
   return len;
}



BOOL CxEdit::CopyAll(void) const
{
   return SetClipboardData_String(m_hWnd, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CxEditView

CxEditView::CxEditView() : CEditView()
{
}

CxEditView::~CxEditView()
{
}

BEGIN_MESSAGE_MAP(CxEditView, CEditView)
   //{{AFX_MSG_MAP(CxEditView)
   ON_WM_CREATE()
   ON_COMMAND(ID_FORMAT_FONT, OnFormatFont)
   ON_COMMAND(ID_EDIT_CLEAR_ALL, OnEditClearAll)
   ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR_ALL, OnUpdateEditClearAll)
   ON_COMMAND(ID_FORMAT_WORDWRAP, OnWordWrap)
   ON_UPDATE_COMMAND_UI(ID_FORMAT_WORDWRAP, OnUpdateWordWrap)
   //}}AFX_MSG_MAP
   ON_COMMAND_EX(ID_ESCAPE, OnEscape)
   ON_UPDATE_COMMAND_UI(ID_INDICATOR_POSITION, OnUpdatePosition)
   // Standard printing commands
   ON_COMMAND(ID_FILE_PRINT, CEditView::OnFilePrint)
#ifdef _WIN32
   ON_COMMAND(ID_FILE_PRINT_DIRECT, CEditView::OnFilePrint)
#endif
   ON_COMMAND(ID_FILE_PRINT_PREVIEW, CEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

void CxEditView::OnEditClearAll() 
{
   GetEditCtrl().OnEditClearAll();
   GetDocument()->SetModifiedFlag();
}

void CxEditView::OnUpdateEditClearAll(CCmdUI* pCmdUI) 
{
   GetEditCtrl().OnUpdateEditClearAll(pCmdUI);
}

int CxEditView::OnCreate(LPCREATESTRUCT lpcs)
{
   if (CEditView::OnCreate(lpcs) == -1)
   {
      return -1;
   }  
   if (m_font.m_hObject == NULL)
   {
      VERIFY(m_font.CreatePointFont(120, _T("Courier New")));
   }
   if (m_font.m_hObject != NULL)
   {
      SetFont(&m_font, FALSE);
   }
   return 0;
}

#ifndef _WIN32      
// nPointSize is actually scaled 10x
BOOL CxEditView::CMyFont::CreatePointFont(int nPointSize, LPCTSTR lpszFaceName, CDC* pDC)
{
   ASSERT(AfxIsValidString(lpszFaceName));

   LOGFONT logFont;
   memset(&logFont, 0, sizeof(LOGFONT));
   logFont.lfCharSet = DEFAULT_CHARSET;
   logFont.lfHeight = nPointSize;
   lstrcpyn(logFont.lfFaceName, lpszFaceName, _countof(logFont.lfFaceName));

   HDC hDC;
   if (pDC != NULL)
   {
      ASSERT_VALID(pDC);
      ASSERT(pDC->m_hAttribDC != NULL);
      hDC = pDC->m_hAttribDC;
   }
   else
      hDC = ::GetDC(NULL);

   // convert nPointSize to logical units based on pDC
   POINT pt;
   pt.y = ::GetDeviceCaps(hDC, LOGPIXELSY) * logFont.lfHeight;
   pt.y /= 720;    // 72 points/inch, 10 decipoints/point
   ::DPtoLP(hDC, &pt, 1);
   POINT ptOrg = { 0, 0 };
   ::DPtoLP(hDC, &ptOrg, 1);
   logFont.lfHeight = -abs(pt.y - ptOrg.y);

   if (pDC == NULL)
      ::ReleaseDC(NULL, hDC);

   return CreateFontIndirect(&logFont);

}
#endif
      
void CxEditView::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType) 
{
   CEditView::CalcWindowRect(lpClientRect, nAdjustType);
   // MFC bug: 3D border is lost without this
   lpClientRect->left+=1, lpClientRect->right -=1;
   lpClientRect->top +=1, lpClientRect->bottom-=0;
}

void CxEditView::OnFormatFont() 
{
   CxFontDialog dlg(this);
   dlg.m_sample = GetEditCtrl().GetSelText();

   ::FontDialog(this, &m_font, CF_EFFECTS | CF_SCREENFONTS, &dlg);
}

void CxEditView::OnUpdatePosition(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
   GetEditCtrl().OnUpdatePosition(pCmdUI);
}

void CxEditView::OnUpdateReadOnly(CCmdUI* pCmdUI) 
{
   GetEditCtrl().OnUpdateReadOnly(pCmdUI);
}

BOOL CxEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
   // default preparation
   return DoPreparePrinting(pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CxEditDoc

CxEditDoc::CxEditDoc() : CDocument(), 
   m_bIgnoreChanges(FALSE), m_bFullPathTitle(TRUE), m_bEnableSave(TRUE), m_bEnableSaveModifiedOnly(TRUE)
{
}

BEGIN_MESSAGE_MAP(CxEditDoc, CDocument)
   //{{AFX_MSG_MAP(CxEditDoc)
   //}}AFX_MSG_MAP
   ON_COMMAND(ID_FILE_SEND_MAIL, CDocument::OnFileSendMail)
   ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, CDocument::OnUpdateFileSendMail)
   ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
END_MESSAGE_MAP()

BOOL CxEditDoc::SaveModified() 
{
   if (m_bIgnoreChanges)
   {
      SetModifiedFlag(FALSE);
   }  
   return CDocument::SaveModified();
}

void CxEditDoc::Serialize(CArchive& ar)
{
   POSITION pos = GetFirstViewPosition();
   CxEditView* pView = (CxEditView*)GetNextView(pos);
   ASSERT_KINDOF(CxEditView, pView);
   pView->SerializeRaw(ar);
}

void CxEditDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
   if (m_bEnableSave)
   {
      if (m_bEnableSaveModifiedOnly)
      {
         pCmdUI->Enable(IsModified());
      }
   }
   else
   {
      pCmdUI->Enable(FALSE);
   }
}

void CxEditDoc::SetTitle(LPCTSTR lpszTitle) 
{
   CString str;
   if (m_strPathName.IsEmpty() || (!m_bFullPathTitle))
   {
      str = lpszTitle;
   }
   else
   {
      str = m_strPathName;
   }
   CDocument::SetTitle(str);
}

/////////////////////////////////////////////////////////////////////////////
// CxFontDialog

#include <dlgs.h>

BOOL CxFontDialog::OnInitDialog() 
{
   BOOL r = CFontDialog::OnInitDialog();
   CString str;
   AfxExtractSubString(str, m_sample, 0, '\r');
   if (!str.IsEmpty())
   {
      SetDlgItemText(stc5, str);
   }
   return r;
}

BEGIN_MESSAGE_MAP(CxFontDialog, CFontDialog)
   //{{AFX_MSG_MAP(CxFontDialog)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CxEditView::OnWordWrap() 
{
   // doesn't work - see solution in SuperPad sample
   //ModifyStyle(IsWordWrap() ? ES_AUTOHSCROLL : 0, IsWordWrap() ? 0 : ES_AUTOHSCROLL);
}

void CxEditView::OnUpdateWordWrap(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(IsWordWrap());
}

BOOL CxEditView::OnEscape(UINT)
{
   BOOL bHandled = FALSE;
   if (GetEditCtrl().IsSelection())
   {
      GetEditCtrl().SelectNone();
      bHandled = TRUE;
   }
   return bHandled;
}

