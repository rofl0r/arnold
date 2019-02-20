// telnet.h : interface of the CTelnetServer class
// Arnold Telnet. Copyright (c) Troels K. 2003

#ifndef __TELNET_H__
#define __TELNET_H__

/////////////////////////////////////////////////////////////////////////////
// CTelnetServer

class CTelnetServerSession;
class CTelnetServer : public CSocket
{
	DECLARE_DYNAMIC(CTelnetServer);
protected:

// Construction
public:
	CTelnetServer(CRuntimeClass* pSessionRuntimeClass);
   
   BOOL Create();

// Attributes
protected:
   CRuntimeClass* m_pSessionRuntimeClass;
   UINT           m_nSocketPort;
   CString        m_strWelcome;
   CString        m_strGoodbye;
   CString        m_strPrompt;
	
   CTypedPtrArray<CObArray, CTelnetServerSession*> m_aConnection;

// Operations
public:
   BOOL ProcessPendingAccept(void);
   BOOL OnConnect(CTelnetServerSession*, BOOL bConnect);
   int GetConnectionCount(void) const { return m_aConnection.GetSize(); }

// Overridable callbacks
protected:
	virtual void OnAccept ( int nErrorCode );

// Implementation
public:
	virtual void Close();
	virtual ~CTelnetServer();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTelnetServer)
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CTelnetServer)
	//}}AFX_MSG

   friend class CTelnetServerSession;
};

/////////////////////////////////////////////////////////////////////////////
// CTelnetServerSession

class CTelnetServerSession : public CSocket
{
	DECLARE_DYNCREATE(CTelnetServerSession);

// Construction
protected:
	CTelnetServerSession(void);

// Attributes
protected:
   CSocketFile  * m_pFile;
   CTelnetServer* m_pParent;

   CString      m_cmdline;
   BOOL         m_bEcho;
   BOOL         m_bContinue;

// Operations
public:
   int Send(LPCSTR , int nFlags = 0);
	int Send(LPCWSTR, int nFlags = 0);
	int Send(char   , int nFlags = 0);
	int Send(wchar_t, int nFlags = 0);

// Implementation
public:
	virtual void Close();
	virtual ~CTelnetServerSession();
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);

// New virtual functions
public:
   virtual void ParseCmdLine(void);
   virtual CString GetWelcomeString(void);
   virtual CString GetGoodbyeString(void);
   virtual CString GetPromptString(void);

// Overrides
public:
   virtual BOOL Init(void);

   // ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTelnetServerSession)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CTelnetServerSession)
	//}}AFX_MSG

   friend class CTelnetServer;
};


#endif // __TELNET_H__
