// telnet.cpp : implementation of the CTelnetServer class
// Arnold Telnet. Copyright (c) Troels K. 2003

#include "stdafx.h"
#include "telnet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTelnetServer

IMPLEMENT_DYNAMIC(CTelnetServer, CSocket)

/////////////////////////////////////////////////////////////////////////////
// CTelnetServer Construction

CTelnetServer::CTelnetServer(CRuntimeClass* pSessionRuntimeClass) : 
   CSocket(), m_nSocketPort(IPPORT_TELNET), m_pSessionRuntimeClass(pSessionRuntimeClass),
   m_strWelcome(_T("Welcome to my Telnet server\r\n")),
   m_strPrompt (_T(">")),
   m_strGoodbye(_T("\r\nGoodbye...\r\n"))
{
#ifdef _DEBUG
   CObject* pObject = m_pSessionRuntimeClass->CreateObject();
   ASSERT_KINDOF(CTelnetServerSession, pObject);
   delete pObject;
#endif
}

BOOL CTelnetServer::Create()
{
   BOOL bOK = CSocket::Create(m_nSocketPort, SOCK_STREAM);
   if (bOK)
   {
	   bOK = Listen();
   }
   return bOK;
}

void CTelnetServer::Close()
{
   for (int i = 0; i < m_aConnection.GetSize(); i++)
   {
      m_aConnection[i]->Close();
   }
   CSocket::Close();
}

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CTelnetServer, CSocket)
	//{{AFX_MSG_MAP(CTelnetServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CTelnetServer Implementation

CTelnetServer::~CTelnetServer()
{
   ASSERT(0 == m_aConnection.GetSize());
}

/////////////////////////////////////////////////////////////////////////////
// CTelnetServer Overridable callbacks

void CTelnetServer::OnAccept(int nErrorCode)
{
	CSocket::OnAccept(nErrorCode);
   if (nErrorCode == 0)
   {
      ProcessPendingAccept();
   }
   else
   {
      ASSERT(FALSE);
   }
}

BOOL CTelnetServer::ProcessPendingAccept(void)
{
   //create new client socket, and add to list
   CTelnetServerSession* pSocket = (CTelnetServerSession*)m_pSessionRuntimeClass->CreateObject();
   ASSERT_KINDOF(CTelnetServerSession, pSocket);
	BOOL bOK = (NULL != pSocket);
	if(bOK)
   {
      pSocket->m_pParent = this;
      bOK = Accept(*pSocket);
      if (bOK)
	   {
		   bOK = pSocket->Init();
	   }
   }

	if(!bOK)
   {
		delete pSocket;
   }
   return bOK;
}

BOOL CTelnetServer::OnConnect(CTelnetServerSession* pSocket, BOOL bConnect)
{
   ASSERT_VALID(this);
   if (bConnect)
   {
		m_aConnection.Add(pSocket);
   }
   else
   {
		for (int i = 0; i < m_aConnection.GetSize(); i++) 
      {
         if (m_aConnection[i] == pSocket)
         {
            m_aConnection.RemoveAt(i);
            break;
         }
      }
      delete pSocket;
   }
   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTelnetServerSession

IMPLEMENT_DYNCREATE(CTelnetServerSession, CSocket)

/////////////////////////////////////////////////////////////////////////////
// CTelnetServerSession Construction

CTelnetServerSession::CTelnetServerSession() : CSocket(), m_bEcho(TRUE), m_pFile(NULL), m_bContinue(TRUE), m_pParent(NULL)
{
}

CTelnetServerSession::~CTelnetServerSession()
{
	delete m_pFile; m_pFile = NULL;
}

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CTelnetServerSession, CSocket)
	//{{AFX_MSG_MAP(CTelnetServerSession)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CTelnetServerSession Overridable callbacks

CString CTelnetServerSession::GetWelcomeString(void)
{
   return m_pParent->m_strWelcome;
}

CString CTelnetServerSession::GetGoodbyeString(void)
{
   return m_pParent->m_strGoodbye;
}

CString CTelnetServerSession::GetPromptString(void)
{
   return m_pParent->m_strPrompt;
}

int CTelnetServerSession::Send(const void* lpBuf, int nBufLen, int nFlags /*= 0*/)
{
   return CSocket::Send(lpBuf, nBufLen, nFlags);
}

int CTelnetServerSession::Send(LPCSTR sz, int nFlags /*= 0*/)
{
   return CSocket::Send(sz, strlen(sz), nFlags);
}

int CTelnetServerSession::Send(LPCWSTR sz, int nFlags /*= 0*/)
{
   const int len = wcslen(sz) + 1;
   char* buffer = new char[len];
   _wcstombsz(buffer, sz, len);
   const int n = Send(buffer, nFlags);
   delete [] buffer;
   return n;
}

int CTelnetServerSession::Send(char c, int nFlags /*= 0*/)
{
   return CSocket::Send(&c, sizeof(c), nFlags);
}

int CTelnetServerSession::Send(wchar_t c, int nFlags /*= 0*/)
{
   return Send((char)c, nFlags);
}

void CTelnetServerSession::OnReceive(int nErrorCode)
{
   ASSERT_VALID(this);
   CSocket::OnReceive(nErrorCode);

   TRY
   {
      char sz[100];
      while (m_bContinue)
      {
         const int dwRead = m_pFile->Read(sz, _countof(sz));
         for (int i = 0; i < dwRead; i++)
         {
            switch (sz[i])
            {
               case '\x8':
                  if (!m_cmdline.IsEmpty())
                  {
                     if (m_bEcho) Send("\x8 \x8");
                     m_cmdline = m_cmdline.Left(m_cmdline.GetLength() - 1);
                  }
                  break;
               case '\n':
                  break;
               case '\r':
                  if (m_bEcho) Send("\r\n");
                  ParseCmdLine();
                  if (m_bContinue) Send(GetPromptString());
                  m_cmdline.Empty();
                  break;
               default:
                  if (m_bEcho) Send(sz[i]);
                  m_cmdline+=sz[i];
                  break;
            }
         }
         if (_countof(sz) != dwRead)
         {
            break;
         }
      }
   }
	CATCH(CFileException, e)
	{
      ASSERT(FALSE); 
	}
	END_CATCH
   if (!m_bContinue)
   {
      Close();
   }
}

/////////////////////////////////////////////////////////////////////////////
// CTelnetServerSession Operations

BOOL CTelnetServerSession::Init()
{
	ASSERT(m_pFile == NULL);
   m_pFile = new CSocketFile(this);
   BOOL bOK = (m_pFile != NULL);
   if (bOK)
   {
   }
   m_pParent->OnConnect(this, TRUE);
   Send(GetWelcomeString() + GetPromptString());
   return bOK;
}

void CTelnetServerSession::Close()
{
   Send(GetGoodbyeString());
   CSocket::Close();
   m_pParent->OnConnect(this, FALSE);
}

void CTelnetServerSession::OnClose(int nErrorCode) 
{
	CSocket::OnClose(nErrorCode);
   m_pParent->OnConnect(this, FALSE);
}

void CTelnetServerSession::ParseCmdLine(void)
{
}

/////////////////////////////////////////////////////////////////////////////
