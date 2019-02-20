// editframe.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditFrame frame

class CEditFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CEditFrame)
protected:
	CEditFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditFrame)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CEditFrame();

	// Generated message map functions
	//{{AFX_MSG(CEditFrame)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
