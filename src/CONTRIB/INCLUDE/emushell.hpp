/* emushell.hpp */
/* Generic CPC emulator app api/interface. Copyright (c) Troels K. 2003 */

#ifndef __EMUSHELL_HPP__
#define __EMUSHELL_HPP__

#ifndef __EMUSHELL_H__
#include "emushell.h"
#endif

class CEmuShell
{
// Attributes
public:
   EMUHANDLE m_handle;

// Construction
public:
   CEmuShell();

// Operations
public:
   BOOL Create    (const EMUAPI*, LPCTSTR szPath);
   HANDLE Run     (HWND hwndParent, int argc, TCHAR* argv[]);
   BOOL PutExePath(LPCTSTR lpszPath);
   BOOL GetDisk   (int no, LPTSTR  lpszBuffer, int cbBuffer) const;
   BOOL PutDisk   (int no, LPCTSTR lpszBuffer);
   BOOL GetRom    (int no, LPTSTR  lpszBuffer = NULL, int cbBuffer = 0) const;
   BOOL PutRom    (int no, LPCTSTR lpszName);
   int  GetName   (LPTSTR lpszBuffer, int cbBuffer) const;
   BOOL GetFileProfile(LPTSTR lpszBuffer, int cbBuffer) const;
   BOOL GetPath   (enum emupath, LPTSTR lpszBuffer, int cbBuffer) const;
   BOOL PutPath   (enum emupath, LPCTSTR lpszBuffer);

   BOOL IsOpen(void) const;

#ifdef __AFX_H__
   CString GetDisk(int no) const;
   CString GetPath(enum emupath) const;
   CString GetRom (int no) const;
   CString GetName(void) const;
   CString GetFileProfile(void) const;
   CString GetFileProfileName(void) const;
#endif

// Implementation
public:
   virtual ~CEmuShell();
   virtual void  Close();
};

#include "emushell.inl"

#endif /* __EMUSHELL_HPP__ */
