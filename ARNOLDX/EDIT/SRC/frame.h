// frame.h : interface of the CFrame class
//
/////////////////////////////////////////////////////////////////////////////

class CFrame : public CMDIFrameWnd
{
public:
   CFrame();
protected: 
   DECLARE_DYNAMIC(CFrame)

// Attributes
public:
   enum
   {
      IDR = IDR_MAINFRAME
   };

// Operations
public:
#ifdef _MSDOS 
   inline CString GetTitle(void) const        { return m_strTitle;      }
   inline void    SetTitle(LPCTSTR lpszTitle) { m_strTitle = lpszTitle; }
#endif

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CFrame)
   public:
   virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CFrame();
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
   CStatusBar  m_wndStatusBar;
   class CMyToolBar : public CToolBar
   {
   public:
#ifdef _WIN32
      virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
#endif
   } m_wndToolBar;

// Generated message map functions
protected:
   //{{AFX_MSG(CFrame)
   afx_msg int OnCreate(LPCREATESTRUCT);
   afx_msg void OnUpdatePosition(CCmdUI* pCmdUI);
   //}}AFX_MSG
   afx_msg BOOL OnEscape(UINT);
   afx_msg BOOL OnMDIWindowCmd(UINT nID);
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
