// edit.h

#ifndef __EDIT_H__
#define __EDIT_H__

/////////////////////////////////////////////////////////////////////////////
// CxEdit - thin wrapper class

class CxEdit : public CEdit
{
// Operations
public:
   void SetWindowText(TCHAR);
   void SetWindowText(LPCTSTR);
   
   BOOL IsEmpty    (void) const;
   BOOL IsSelection(void) const;
   
   void Empty     (void);
   void SelectAll (void);
   void SelectNone(void);
   
   int GetCount (void) const;
   int GetRow(void) const;
   int GetCol(void) const;
   
   int DeleteLine(int nLine, BOOL bScroll = TRUE, int nCount = 1);
   void GotoLine(int nLine, BOOL bScroll = TRUE);
   int GotoBegin(BOOL bScroll = TRUE);
   int GotoEnd(BOOL bScroll = TRUE);
   int GetLine(int nLine, CString* str) const;
   int LineLength(int nLine = -1) const;
   CString GetSelText(void) const;

#ifdef _WIN32
   void ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo = FALSE);
   void ReplaceSel(TCHAR chNew        , BOOL bCanUndo = FALSE);
#else
   void ReplaceSel(LPCTSTR lpszNewText);
   void ReplaceSel(TCHAR chNew        );
   void ReplaceSel(LPCTSTR lpsz, BOOL /*bCanUndo*/) { CEdit::ReplaceSel(lpsz); }
#endif

   BOOL CopyAll(void) const;

   int  AddString   (LPCTSTR lpsz, BOOL bScroll = TRUE, BOOL bCanUndo = FALSE);
   BOOL InsertString(int nLine, LPCTSTR lpsz, BOOL bScroll = TRUE, BOOL bCanUndo = FALSE);
   BOOL LineSwap(int line0, int line1, BOOL bScroll = TRUE, BOOL bCanUndo = FALSE);

   BOOL IsStyleReadOnly(void) const;
   BOOL IsStyleLeft    (void) const;
   BOOL IsStyleRight   (void) const;
   BOOL IsStyleCenter  (void) const;

   void OnUpdatePosition(CCmdUI*);
   void OnUpdateReadOnly(CCmdUI*);
   void OnEditClearAll(void);
   void OnUpdateEditClearAll(CCmdUI*);
   void OnUpdateCanModify(CCmdUI*);


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CxEdit)
   //}}AFX_VIRTUAL
};

/////////////////////////////////////////////////////////////////////////////
// CxEditDoc

class CxEditDoc : public CDocument
{
protected:
   DECLARE_DYNCREATE(CxEditDoc)
   CxEditDoc();

// Attributes
protected:
   BOOL m_bIgnoreChanges;
   BOOL m_bFullPathTitle;
   BOOL m_bEnableSave;
   BOOL m_bEnableSaveModifiedOnly;

public:
// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CxEditDoc)
   public:
   virtual void SetTitle(LPCTSTR lpszTitle);
   virtual void Serialize(CArchive& ar);   // overridden for document i/o
   protected:
   virtual BOOL SaveModified();
   //}}AFX_VIRTUAL

// Generated message map functions
protected:
   //{{AFX_MSG(CxEditDoc)
   afx_msg void OnUpdateFileSave(CCmdUI*);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CxEditView view

#ifdef __AFXEXT_H__
class CxEditView : public CEditView
{
protected:
   DECLARE_DYNCREATE(CxEditView)
   CxEditView();           // protected constructor used by dynamic creation

// Attributes
public:
   class CMyFont : public CFont
   {
   public:
   #ifndef _WIN32
      BOOL CreatePointFont(int nPointSize, LPCTSTR lpszFaceName, CDC* pDC = NULL);
      BOOL GetLogFont(LOGFONT* lf) { return GetObject(sizeof(*lf), lf); }
   #endif
   } m_font;

// Operations
public:
   CxEdit& GetEditCtrl(void) const;
   BOOL IsWordWrap(void) const;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CxEditView)
   protected:
   virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
   virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);
   //}}AFX_VIRTUAL

// Implementation
protected:
   virtual ~CxEditView();

   // Generated message map functions
protected:
   //{{AFX_MSG(CxEditView)
   afx_msg int OnCreate(LPCREATESTRUCT lpcs);
   afx_msg void OnFormatFont();
   afx_msg void OnEditClearAll();
   afx_msg void OnUpdateEditClearAll(CCmdUI* pCmdUI);
   //}}AFX_MSG
   afx_msg BOOL OnEscape(UINT);
   afx_msg void OnUpdatePosition(CCmdUI*);
   afx_msg void OnUpdateReadOnly(CCmdUI*);
   afx_msg void OnWordWrap();
   afx_msg void OnUpdateWordWrap(CCmdUI* pCmdUI);
   DECLARE_MESSAGE_MAP()
};
#endif

/////////////////////////////////////////////////////////////////////////////
// CxFontDialog

#ifdef __AFXDLGS_H__
class CxFontDialog : public CFontDialog 
{
// Attributes
public:
   CString m_sample;

   CxFontDialog(CWnd* pParentWnd);
   CxFontDialog( LPLOGFONT lplfInitial = NULL, DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS, CDC* pdcPrinter = NULL, CWnd* pParentWnd = NULL );
#ifndef _AFX_NO_RICHEDIT_SUPPORT
   CxFontDialog(const CHARFORMAT& charformat, DWORD dwFlags = CF_SCREENFONTS, CDC* pdcPrinter = NULL, CWnd* pParentWnd = NULL);
#endif

public:

// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CxFontDialog)
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CxFontDialog)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
#endif

#include "edit.inl"

#endif // __EDIT_H__
