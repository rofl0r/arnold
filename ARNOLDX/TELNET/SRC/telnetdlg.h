// telnetdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTelnetDialog dialog

class CTelnetDialog : public CDialog
{
// Construction
public:
	CTelnetDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTelnetDialog)
	enum { IDD = IDD_TELNETSERVER_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTelnetDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
public:
   
// Implementation
public:
   virtual ~CTelnetDialog();
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTelnetDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
