// session.h
// Arnold Telnet. Copyright (c) Troels K. 2003

#ifndef __SESSION_H__
#define __SESSION_H__

#include "telnet.h"
#include "..\..\..\arnold\src\contrib\include\cpcapi.h"

class CArnoldTelnetServer : public CTelnetServer
{
public:
   CArnoldTelnetServer();

   int m_idtimer;
   int m_maxusercount;

   enum
   {
#ifdef _DEBUG
      LIT_timeout = 20 // [sec]
#else
      LIT_timeout = 1*60 // [sec]
#endif
   };
   
   BOOL Create(UINT port, int maxusercount);
   void OnTimer();

// Implementation
public:
	virtual void Close();
};

class CArnoldTelnetSession : public CTelnetServerSession
{
	DECLARE_DYNCREATE(CArnoldTelnetSession);

// Construction
protected:
	CArnoldTelnetSession(void);
public:
   CPCHANDLE m_handle;
   BOOL      m_bChar;

// Commands
public:

// Attributes
protected:

// Implementation
public:
   virtual CString GetWelcomeString(void);
   virtual CString GetGoodbyeString(void);
   virtual CString GetPromptString(void);
   virtual ~CArnoldTelnetSession();
   virtual BOOL Init(void);
	virtual void OnClose(int nErrorCode);
	virtual void Close();
   virtual void OnReceive(int nErrorCode);
};

#endif // __SESSION_H__
