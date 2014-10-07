// telnetdlg.cpp : implementation file
//

#include "stdafx.h"
#include "app.h"
#include "telnetdlg.h"
#include "telnet.h"
#include "session.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTelnetDialog dialog

CTelnetDialog::CTelnetDialog(CWnd* pParent /*=NULL*/)	: CDialog(CTelnetDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTelnetDialog)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CTelnetDialog::~CTelnetDialog()
{
}

void CTelnetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTelnetDialog)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTelnetDialog, CDialog)
	//{{AFX_MSG_MAP(CTelnetDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTelnetDialog message handlers

BOOL CTelnetDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
   CString str;
   UINT port;
   CApp::GetApp().m_server.GetSockName(str, port);
   
   MySetWindowText(m_hWnd, NULL, AfxGetAppName(), port);
	
   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTelnetDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
      class CAboutDialog : public CDialog
      {
      public:
         CAboutDialog(UINT nIDD, CWnd* pParentWnd) : CDialog(nIDD, pParentWnd) {}
         virtual BOOL OnInitDialog() 
         { 
            MySetWindowText(::GetDlgItem(m_hWnd, IDC_STATIC_0), NULL, AfxGetAppName()); 
            return CDialog::OnInitDialog(); 
         }
      };	
      CAboutDialog(IDD_ABOUTBOX, this).DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTelnetDialog::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CTelnetDialog::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTelnetDialog::OnDestroy() 
{
   CApp& app = CApp::GetApp();
   app.m_server.Close();
	CDialog::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////

