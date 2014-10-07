// emushell.inl

inline CEmuShell::CEmuShell(void) : m_handle(NULL)
{ 
}

inline CEmuShell::~CEmuShell()
{
   if (IsOpen()) Close();
}

inline BOOL CEmuShell::IsOpen(void) const
{ 
   return (m_handle != NULL); 
}

inline BOOL CEmuShell::Create(const EMUAPI* api, LPCTSTR szPath)
{
   _ASSERTE(m_handle == NULL);
   BOOL bOK = (NULL != api->create);
   if (bOK)
   {
      m_handle = (*api->create)(szPath);
      bOK = (m_handle != NULL);
   }
   return bOK;
}

inline BOOL CEmuShell::PutExePath(LPCTSTR lpszPath)
{
   _ASSERTE(m_handle != NULL);
   BOOL bOK = (m_handle->api->put_exe_path != NULL);
   if (bOK)
   {
      (*m_handle->api->put_exe_path)(m_handle, lpszPath);
   }
   return bOK;
}

inline HANDLE CEmuShell::Run(HWND hwndParent, int argc, TCHAR* argv[])
{
   _ASSERTE(m_handle != NULL);
   return (*m_handle->api->run)(m_handle, hwndParent, argc, argv);
}

inline BOOL CEmuShell::GetDisk(int no, LPTSTR lpszBuffer, int cbBuffer) const
{
   _ASSERTE(m_handle != NULL);
   return (*m_handle->api->get_disk)(m_handle, no, lpszBuffer, cbBuffer);
}

inline BOOL CEmuShell::PutDisk(int no, LPCTSTR lpszBuffer)
{
   _ASSERTE(m_handle != NULL);
   return (*m_handle->api->put_disk)(m_handle, no, lpszBuffer);
}

inline BOOL CEmuShell::GetPath(enum emupath index, LPTSTR lpszBuffer, int cbBuffer) const
{
   _ASSERTE(m_handle != NULL);
   return (*m_handle->api->get_path)(m_handle, index, lpszBuffer, cbBuffer);
}

inline BOOL CEmuShell::PutPath(enum emupath index, LPCTSTR lpszBuffer)
{
   _ASSERTE(m_handle != NULL);
   return (*m_handle->api->put_path)(m_handle, index, lpszBuffer);
}

inline BOOL CEmuShell::GetRom(int no, LPTSTR lpszBuffer, int cbBuffer) const
{
   _ASSERTE(m_handle != NULL);
   return (*m_handle->api->get_rom)(m_handle, no, lpszBuffer, cbBuffer);
}

inline BOOL CEmuShell::PutRom(int no, LPCTSTR lpszName)
{
   _ASSERTE(m_handle != NULL);
   return (*m_handle->api->put_rom)(m_handle, no, lpszName);
}

inline int  CEmuShell::GetName(LPTSTR lpszBuffer, int cbBuffer) const
{
   _ASSERTE(m_handle != NULL);
   return (*m_handle->api->get_name)(m_handle, lpszBuffer, cbBuffer);
}

inline BOOL CEmuShell::GetFileProfile(LPTSTR lpszBuffer, int cbBuffer) const
{
   _ASSERTE(m_handle != NULL);
   return (*m_handle->api->get_fileprofile)(m_handle, lpszBuffer, cbBuffer);
}

inline void CEmuShell::Close()
{
   _ASSERTE(m_handle != NULL);
   (*m_handle->api->close)(&m_handle);
}

#ifdef __AFX_H__
inline CString CEmuShell::GetDisk(int no) const
{
   CString str;
   GetDisk(no, str.GetBuffer(_MAX_PATH), _MAX_PATH);
   str.ReleaseBuffer();
   return str;
}

inline CString CEmuShell::GetPath(enum emupath index) const
{
   CString str;
   GetPath(index, str.GetBuffer(_MAX_PATH), _MAX_PATH);
   str.ReleaseBuffer();
   return str;
}

inline CString CEmuShell::GetRom(int no) const
{
   CString str;
   GetRom(no, str.GetBuffer(_MAX_PATH), _MAX_PATH);
   str.ReleaseBuffer();
   return str;
}

inline CString CEmuShell::GetName(void) const
{
   CString str;
   GetName(str.GetBuffer(_MAX_PATH), _MAX_PATH);
   str.ReleaseBuffer();
   return str;
}

inline CString CEmuShell::GetFileProfile(void) const
{
   CString str;
   GetFileProfile(str.GetBuffer(_MAX_PATH), _MAX_PATH);
   str.ReleaseBuffer();
   return str;
}

/*
inline CString CEmuShell::GetFileProfileName(void) const
{
   CString strPath = GetFileProfile();
   TCHAR sz;
   _tsplitpath();
   str.ReleaseBuffer();
   return str;
}
*/
#endif
