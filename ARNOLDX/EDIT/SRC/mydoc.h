// mydoc.h : header file
//

#include "edit.h"

#include "..\..\..\arnold\src\contrib\include\cpcapi.h"

/////////////////////////////////////////////////////////////////////////////
// CMyDocument document

class CMyDocument : public CxEditDoc
{
protected:
	CMyDocument();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMyDocument)

// Attributes
public:
   CPCHANDLE m_handle;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyDocument)
	public:
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyDocument();
	//virtual void SetModifiedFlag(BOOL bModified = TRUE);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyDocument)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
