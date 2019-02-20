// helpers.h : 
//

extern BOOL FontDialog(CWnd* pWnd, CFont* pFont, DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS, CFontDialog* pFontDialog = NULL, BOOL bRedraw = TRUE);

extern BOOL WINAPI SetClipboardData_StringA(HWND hwnd, const char* lpsz);
extern BOOL WINAPI SetClipboardData_StringW(HWND hwnd, const wchar_t* lpsz);

#ifdef _UNICODE
   #define SetClipboardData_String SetClipboardData_StringW
#else
   #define SetClipboardData_String SetClipboardData_StringA
#endif

class CMyMultiDocTemplate : public CMultiDocTemplate
{
public:
   CMyMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
   virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE);
};

extern CString MFC_GetClipboardData_String(CWnd* pWnd);

extern BOOL MFC_FrameWnd_GetMDIClientSize(SIZE* size, CMDIFrameWnd* pFrame = NULL);
#ifdef _WIN32
extern int MFC_ControlBar_OnToolHitTest(const CControlBar& wnd, POINT pt, TOOLINFO* pTI, BOOL bOnlyWhenDocked = TRUE); // GetWindowText
#endif
