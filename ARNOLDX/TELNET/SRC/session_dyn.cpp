// session_dyn.cpp
// Arnold Telnet. Copyright (c) Troels K. 2003
// Test of using LoadLibrary. No benefit using LoadLibrary :(

#include "stdafx.h"
#include "app.h"
#include "session.h"

extern const CPCAPI PUB_cpc_telnet_api;
#include "..\arnold\src\win\cpcemu.h"

/////////////////////////////////////////////////////////////////////////////
// CArnoldTelnetServer

static VOID CALLBACK TimerProc(HWND /*hwnd*/, UINT /*uMsg*/, UINT /*idEvent*/, DWORD /*dwTime*/)
{
   CApp::GetApp().m_server.OnTimer();
}

CArnoldTelnetServer::CArnoldTelnetServer() : CTelnetServer(RUNTIME_CLASS(CArnoldTelnetSession)),
   m_maxusercount(-1)
{
}

BOOL CArnoldTelnetServer::Create(UINT port, int maxusercount)
{
   m_nSocketPort = port, m_maxusercount = maxusercount;
   BOOL bOK = CTelnetServer::Create();
   if (bOK)
   {
      m_idtimer = ::SetTimer(NULL, 0, LIT_timeout*1000, TimerProc);
      ASSERT(m_idtimer != 0);
   }
   return bOK;
}

void CArnoldTelnetServer::Close()
{
   if (m_idtimer != 0)
   {
      VERIFY(::KillTimer(NULL, m_idtimer));  m_idtimer = NULL;
   }
   CTelnetServer::Close();
}

void CArnoldTelnetServer::OnTimer(void)
{
// const time_t now = time(NULL);
   for (int i = m_aConnection.GetSize() - 1; i >= 0; i--)
   {
      CArnoldTelnetSession* pSession = (CArnoldTelnetSession*)m_aConnection[i];
      ASSERT_KINDOF(CArnoldTelnetSession, pSession);

      if (pSession->m_bChar)
      {
         pSession->m_bChar = FALSE;
      }
      else
      {
         pSession->Close();
      }
   }
}

CString GetComputerName(void)
{
   CString str;
   DWORD dw = _MAX_PATH;
   ::GetComputerName(str.GetBuffer(dw), &dw);
   str.ReleaseBuffer();
   return str;
}

/////////////////////////////////////////////////////////////////////////////
// CArnoldTelnetSession

IMPLEMENT_DYNCREATE(CArnoldTelnetSession, CTelnetServerSession)

/////////////////////////////////////////////////////////////////////////////
// CArnoldTelnetSession Construction

CArnoldTelnetSession::CArnoldTelnetSession() : CTelnetServerSession(), m_handle(NULL), m_bChar(TRUE)
{
   m_bEcho = FALSE;
}

CArnoldTelnetSession::~CArnoldTelnetSession()
{
}

void CArnoldTelnetSession::OnClose(int nErrorCode)
{
   if (m_handle != NULL) (*m_handle->api->close)(&m_handle);
   CTelnetServerSession::OnClose(nErrorCode);
}

void CArnoldTelnetSession::Close()
{
   if (m_handle != NULL) (*m_handle->api->close)(&m_handle);
   CTelnetServerSession::Close();
}

BOOL CArnoldTelnetSession::Init(void)
{
   const int count    = m_pParent->GetConnectionCount();
   const int maxcount = ((CArnoldTelnetServer*)m_pParent)->m_maxusercount;
   
   BOOL bOK = (maxcount == -1) || (count < maxcount);
   if (bOK)
   {
      bOK = CTelnetServerSession::Init();
      if (bOK)
      {
      }   
      if (bOK)
      {
         m_handle = (*PUB_cpc_telnet_api.create)(NULL, this);
      }
   }
   else
   {
      Send(_T("Too many users...sorry\r\n"));
   }
   return bOK;
}

CString CArnoldTelnetSession::GetWelcomeString(void)
{
   CString str;
   str.Format(_T("Welcome to the Arnold %s server on %s\r\n"   )
              _T("Arnold Copyright (c) Kevin Thacker 1995-2001\r\n")
              _T("Arnold %s Copyright (c) Troels K. 2003\r\n"  ), 
      AfxGetAppName(),
      ::GetComputerName().operator LPCTSTR(),
      AfxGetAppName()
      );
   return str;
}

CString CArnoldTelnetSession::GetPromptString(void)
{
   return CString();
}

CString CArnoldTelnetSession::GetGoodbyeString(void)
{
   CString str;
   str.Format(_T("Goodbye..."));
   return str;
}

typedef void (*PFNSETKEY)(int win_virtualkey, BOOL bKeyDown, BOOL bNumPad);

typedef struct _CPCIMPLEMENTATION
{
   int      mode;
   CArnoldTelnetSession* session;
   DWORD    thread_handle;
   BOOL     running;
   LPTSTR   szInitString;
   HINSTANCE hModule;
   PFNSETKEY CPCEMU_SetKey_VK;
} TELNETIMPLEMENTATION;

void CArnoldTelnetSession::OnReceive(int nErrorCode)
{
   ASSERT_VALID(this);
   CSocket::OnReceive(nErrorCode); // skip base class
   m_bChar = TRUE;

   TRY
   {
      char sz[100];
      while (m_bContinue)
      {
         const int dwRead = m_pFile->Read(sz, _countof(sz));
         for (int i = 0; i < dwRead; i++)
         {
            (*m_handle->impl->CPCEMU_SetKey_VK)(sz[i], TRUE, FALSE);
            Sleep(100);
            (*m_handle->impl->CPCEMU_SetKey_VK)(sz[i], FALSE, FALSE);
            Sleep(100);
            if (VK_BACK == sz[i])
            {
               (*m_handle->api->writechar)(m_handle, VK_BACK); /* there must be a better way! */
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

extern const CPCAPI PUB_cpc_telnet_api;

static void cpc_thread(LPVOID ptr)
{
   CPCHANDLE handle = (CPCHANDLE)ptr;
   typedef void (*PFN)(BOOL);
   PFN pfnMainLoop = (PFN)GetProcAddress(handle->impl->hModule, "CPCEMU_MainLoop");
   (*pfnMainLoop)(FALSE);
   handle->impl->thread_handle  = 0;
}

#ifdef _UNICODE
   #ifdef _DEBUG
      #define DLLNAME "arnoldw_d.dll"
   #else
      #define DLLNAME "arnoldw.dll"
   #endif
#else
   #ifdef _DEBUG
      #define DLLNAME "arnold_d.dll"
   #else
      #define DLLNAME "arnold.dll"
   #endif
#endif

static CPCHANDLE create_impl(LPCTSTR szInitString, void* parent)
{
   CPCHANDLE handle = NULL;
   HINSTANCE hModule = LoadLibrary(_T(DLLNAME));
   if (NULL != hModule)
   {
      typedef void (*PFN)(CPCHANDLE);
      PFN pfnIntercept;

      handle = (CPCHANDLE)malloc(sizeof(CPCDATA));   
      handle->impl = (TELNETIMPLEMENTATION*)malloc(sizeof(TELNETIMPLEMENTATION));

      handle->api = &PUB_cpc_telnet_api;
      handle->impl->session = (CArnoldTelnetSession*)parent;
      handle->impl->hModule = hModule;
      handle->impl->CPCEMU_SetKey_VK = (PFNSETKEY)GetProcAddress(hModule, "CPCEMU_SetKey_VK");

      (*handle->api->set_mode)(handle, 1);
      (*handle->api->set_cursor_pos)(handle, 1, 1);

      pfnIntercept = (PFN)GetProcAddress(hModule, "CPCAPI_Z80_SetInterceptHandler");
      (*pfnIntercept)(handle);
      handle->impl->running = TRUE;
      handle->impl->thread_handle = _beginthread(cpc_thread, 1024, handle);
      handle->impl->szInitString = (szInitString != NULL) ? _tcsdup(szInitString) : NULL;
   }
   return handle;
}

static void close_impl(CPCHANDLE* handle)
{
   typedef void (*PFN)(BOOL);
   PFN pfn = (PFN)GetProcAddress((*handle)->impl->hModule, "CPCAPI_SetConsoleBreak");
   (*pfn)(TRUE);
   
   (*handle)->impl->running = FALSE;
   while ((*handle)->impl->thread_handle != 0) /* coordinated shutdown */
   {
      Sleep(0);
   }
   FreeLibrary((*handle)->impl->hModule);
   free((*handle)->impl->szInitString);
   free((*handle)->impl);
   free(*handle);
   *handle = NULL;
}

static void clrscr_impl(CPCHANDLE handle)
{
   (*handle->api->writechar)(handle, '\r');
   (*handle->api->writechar)(handle, '\n');
}

static void set_mode_impl(CPCHANDLE handle, int mode)
{
   if ((mode >= 0) && (mode < CPC_TEXTMODECOUNT))
   {
      handle->impl->mode = mode;
      (*handle->api->clrscr)(handle);
   }
}

static int get_mode_impl(CPCHANDLE handle)
{
   return handle->impl->mode;
}

static void writechar_impl(CPCHANDLE handle, TCHAR ch)
{
   AfxGetApp()->PostThreadMessage(WM_USER_CHAR, (LPARAM)handle->impl->session, ch);
}

static void printf_impl(CPCHANDLE handle, const TCHAR* fmt, ...)
{
   CString str;
   va_list args;
	va_start(args, fmt);
   str.FormatV(fmt, args);
	va_end(args);

   for (int i = 0; i < str.GetLength(); i++)
   {
      (*handle->api->writechar)(handle, str[i]);
   }
}

static int set_cursor_pos_impl(CPCHANDLE handle, int x , int  y)
{
   (*handle->api->writechar)(handle, '\r');
   (*handle->api->writechar)(handle, '\n');
   return 0;
}

static BOOL get_cursor_pos_impl(CPCHANDLE handle, int* x, int* y)
{
   return TRUE;
}

static void bell_impl(CPCHANDLE handle)
{
}

const CPCAPI PUB_cpc_telnet_api = 
{
   create_impl, close_impl, clrscr_impl, set_mode_impl, 
   get_mode_impl, writechar_impl, printf_impl, set_cursor_pos_impl, get_cursor_pos_impl, bell_impl
};

