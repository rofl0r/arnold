// MyView.h : header file
// CPC edit box. Copyright (c) Troels K. 2003

#include "edit.h"

/////////////////////////////////////////////////////////////////////////////
// CMyView view

class CMyDocument;
class CMyView : public CxEditView
{
protected:
	CMyView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMyView)

// Attributes
public:
   CMyDocument* GetDocument(void) { return (CMyDocument*)m_pDocument;}

// Operations
public:
   void SetPosition(int x, int y);
   void GetPosition(int* x, int* y) const;
   void GetPosition(long* x, long* y) const;
   void ClrScr(void);
   CSize GetSize(void) const;
   void SetMode(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
   
   void ResizeParentToFit(const SIZE& desired_size, BOOL bShrinkOnly);

// Implementation
protected:
	virtual ~CMyView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
   afx_msg LRESULT OnCharFromCPC(LPARAM,WPARAM);
   afx_msg void OnSetMode(void);
   afx_msg LRESULT OnSetFont(LPARAM,WPARAM);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

