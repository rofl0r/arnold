// edit.inl

/////////////////////////////////////////////////////////////////////////////
// CxEdit - thin wrapper class

inline BOOL CxEdit::IsEmpty() const
{
   return (GetWindowTextLength() == 0);
}

inline void CxEdit::SetWindowText(TCHAR ch)
{ 
   CEdit::SetWindowText(CString(ch)); 
}

inline void CxEdit::SetWindowText(LPCTSTR lpsz)
{ 
   CEdit::SetWindowText(lpsz);
}
   
inline void CxEdit::Empty    (void)
{ 
   SetWindowText(&afxChNil); 
}

inline void CxEdit::SelectAll(void)
{ 
   SetSel(0, -1); 
}
   
inline void CxEdit::SelectNone(void)
{ 
   SetSel(-1, -1); 
}
   
inline int CxEdit::GetCount (void) const
{
   return GetLineCount(); 
}

inline int CxEdit::GetRow(void) const 
{ 
   return LineFromChar(); 
}

#ifdef _WIN32
   inline void CxEdit::ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo /*= FALSE*/) 
   { 
      CEdit::ReplaceSel(lpszNewText      , bCanUndo); 
   }
   inline void CxEdit::ReplaceSel(TCHAR chNew        , BOOL bCanUndo /*= FALSE*/) 
   { 
      CEdit::ReplaceSel(CString(chNew), bCanUndo); 
   }
#else
   inline void CxEdit::ReplaceSel(LPCTSTR lpszNewText)
   { 
      CEdit::ReplaceSel(lpszNewText      ); 
   }
   inline void CxEdit::ReplaceSel(TCHAR chNew        )
   { 
      CEdit::ReplaceSel(CString(chNew)); 
   }
#endif

inline BOOL CxEdit::IsStyleReadOnly(void) const 
{ 
   return ((GetStyle() & ES_READONLY) == ES_READONLY); 
}

inline BOOL CxEdit::IsStyleLeft    (void) const 
{ 
   return (ES_LEFT   == (GetStyle() & (ES_LEFT | ES_RIGHT | ES_CENTER)));
}

inline BOOL CxEdit::IsStyleRight   (void) const 
{ 
   return (ES_RIGHT  == (GetStyle() & (ES_LEFT | ES_RIGHT | ES_CENTER)));
}
inline BOOL CxEdit::IsStyleCenter  (void) const
{ 
   return (ES_CENTER == (GetStyle() & (ES_LEFT | ES_RIGHT | ES_CENTER)));
}

/////////////////////////////////////////////////////////////////////////////
// CxEditView view

#ifdef __AFXEXT_H__
inline BOOL CxEditView::IsWordWrap() const
{
   return (GetStyle() & ES_AUTOHSCROLL) == 0;
}

inline CxEdit& CxEditView::GetEditCtrl() const
{ 
   return (CxEdit&)CEditView::GetEditCtrl(); 
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CxFontDialog

#ifdef __AFXDLGS_H__
inline CxFontDialog::CxFontDialog(CWnd* pParentWnd = NULL): CFontDialog(NULL,0, NULL, pParentWnd)
{
}
inline CxFontDialog::CxFontDialog( LPLOGFONT lplfInitial /*= NULL*/, DWORD dwFlags /*= CF_EFFECTS | CF_SCREENFONTS*/, CDC* pdcPrinter /*= NULL*/, CWnd* pParentWnd /*= NULL*/ )
      : CFontDialog( lplfInitial, dwFlags, pdcPrinter, pParentWnd)
   {
   }
#ifndef _AFX_NO_RICHEDIT_SUPPORT
inline CxFontDialog::CxFontDialog(const CHARFORMAT& charformat, DWORD dwFlags /*= CF_SCREENFONTS*/,
      CDC* pdcPrinter /*= NULL*/, CWnd* pParentWnd /*= NULL*/) 
      : CFontDialog(charformat, dwFlags, pdcPrinter, pParentWnd)
   {
   }
#endif
#endif

/////////////////////////////////////////////////////////////////////////////

