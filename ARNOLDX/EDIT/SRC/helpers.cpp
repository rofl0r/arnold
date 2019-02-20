// helpers.cpp : 
//

#include "stdafx.h"

#include "helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char BASED_CODE THIS_FILE[] = __FILE__;
#endif

class CMyNewTypeDlg : public CDialog
{
protected:
   CPtrList*   m_pList;        // actually a list of doc templates
public:
   CDocTemplate*   m_pSelectedTemplate;

public:
   //{{AFX_DATA(CMyNewTypeDlg)
   enum { IDD = AFX_IDD_NEWTYPEDLG };
   //}}AFX_DATA
   CMyNewTypeDlg(CPtrList* pList) : CDialog(CMyNewTypeDlg::IDD)
   {
      m_pList = pList;
      m_pSelectedTemplate = NULL;
   }
   ~CMyNewTypeDlg() { }

protected:
   BOOL OnInitDialog();
   void OnOK();
   //{{AFX_MSG(CMyNewTypeDlg)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CMyNewTypeDlg, CDialog)
   //{{AFX_MSG_MAP(CMyNewTypeDlg)
   ON_LBN_DBLCLK(AFX_IDC_LISTBOX, OnOK)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CMyNewTypeDlg::OnInitDialog()
{
   CListBox* pListBox = (CListBox*)GetDlgItem(AFX_IDC_LISTBOX);
   ASSERT(pListBox != NULL);

   // fill with document templates in list
   pListBox->ResetContent();

   POSITION pos = m_pList->GetHeadPosition();
   // add all the CDocTemplates in the list by name
   while (pos != NULL)
   {
      CDocTemplate* pTemplate = (CDocTemplate*)m_pList->GetNext(pos);
      ASSERT_KINDOF(CDocTemplate, pTemplate);

      CString strTypeName;
      if (pTemplate->GetDocString(strTypeName, CDocTemplate::fileNewName) &&
         !strTypeName.IsEmpty())
      {
         // add it to the listbox
         int nIndex = pListBox->AddString(strTypeName);
         if (nIndex == -1)
         {
            EndDialog(-1);
            return FALSE;
         }
         pListBox->SetItemDataPtr(nIndex, pTemplate);
      }
   }

   int nTemplates = pListBox->GetCount();
   if (nTemplates == 0)
   {
      TRACE0("Error: no document templates to select from!\n");
      EndDialog(-1); // abort
   }
   else if (nTemplates == 1)
   {
      // get the first/only item
      m_pSelectedTemplate = (CDocTemplate*)pListBox->GetItemDataPtr(0);
      ASSERT_VALID(m_pSelectedTemplate);
      ASSERT_KINDOF(CDocTemplate, m_pSelectedTemplate);
      EndDialog(IDOK);    // done
   }
   else
   {
      // set selection to the first one (NOT SORTED)
      pListBox->SetCurSel(0);
   }
   
   SetWindowText(_T("Open in"));

   return CDialog::OnInitDialog();
}

void CMyNewTypeDlg::OnOK()
{
   CListBox* pListBox = (CListBox*)GetDlgItem(AFX_IDC_LISTBOX);
   ASSERT(pListBox != NULL);
   // if listbox has selection, set the selected template
   int nIndex;
   if ((nIndex = pListBox->GetCurSel()) == -1)
   {
      // no selection
      m_pSelectedTemplate = NULL;
   }
   else
   {
      m_pSelectedTemplate = (CDocTemplate*)pListBox->GetItemDataPtr(nIndex);
      ASSERT_VALID(m_pSelectedTemplate);
      ASSERT_KINDOF(CDocTemplate, m_pSelectedTemplate);
   }
   CDialog::OnOK();
}


HGLOBAL WINAPI LMRT_GlobalAlloc_StringA(const char* lpsz)
{
   const int len = strlen(lpsz) + 1;
   HGLOBAL handle = GlobalAlloc(GMEM_MOVEABLE, sizeof(char) * len);
   if (handle != NULL)
   {
      memcpy(GlobalLock(handle), lpsz, sizeof(char) * len);
      GlobalUnlock(handle);
   }
   return handle;
}

BOOL WINAPI SetClipboardData_StringA(HWND hwnd, const char* lpsz)
{
   BOOL bOK = OpenClipboard(hwnd);
   if (bOK)
   {
      HGLOBAL hGlobal;
      char* temp = NULL;
      
      if (NULL == lpsz)
      {
         const int len = GetWindowTextLengthA(hwnd);
         temp = (char*)malloc(sizeof(char) * (len + 1));
         GetWindowTextA(hwnd, temp, len + 1);
         lpsz = temp;
      }
      
      hGlobal = LMRT_GlobalAlloc_StringA(lpsz);
      EmptyClipboard();
      bOK = (hGlobal != NULL);
      if (bOK)
      {
         bOK = (NULL != SetClipboardData(CF_TEXT, hGlobal));
      }
      if (!CloseClipboard())
      {
         bOK = FALSE;
      }
      free(temp);
   }
   return bOK;
}
#ifdef _WIN32
HGLOBAL WINAPI LMRT_GlobalAlloc_StringW(const wchar_t* lpsz)
{
   const int len = wcslen(lpsz) + 1;
   HGLOBAL handle = GlobalAlloc(GMEM_MOVEABLE, sizeof(wchar_t) * len);
   if (handle != NULL)
   {
      MoveMemory(GlobalLock(handle), lpsz, sizeof(wchar_t) * len);
      GlobalUnlock(handle);
   }
   return handle;
}

BOOL WINAPI SetClipboardData_StringW(HWND hwnd, const wchar_t* lpsz)
{
   BOOL bOK = OpenClipboard(hwnd);
   if (bOK)
   {
      HGLOBAL hGlobal;
      wchar_t* temp = NULL;
      
      if (NULL == lpsz)
      {
         const int len = GetWindowTextLengthW(hwnd);
         temp = (wchar_t*)malloc(sizeof(wchar_t) * (len + 1));
         GetWindowTextW(hwnd, temp, len + 1);
         lpsz = temp;
      }
      
      hGlobal = LMRT_GlobalAlloc_StringW(lpsz);
      EmptyClipboard();
      bOK = (hGlobal != NULL);
      if (bOK)
      {
         bOK = (NULL != SetClipboardData(CF_UNICODETEXT, hGlobal));
      }
      if (!CloseClipboard())
      {
         bOK = FALSE;
      }
      free(temp);
   }
   return bOK;
}
#endif

         class CDialogHack : public CDialog
         {
         public:
            inline CWnd* GetParentMember(void) const { return m_pParentWnd; }
            inline void SetParentMember(CWnd* pWnd) { m_pParentWnd = pWnd; }
         };


BOOL FontDialog(CWnd* pWnd, CFont* pFont, DWORD dwFlags /*= CF_EFFECTS | CF_SCREENFONTS*/, CFontDialog* pFontDialog /*= NULL*/, BOOL bRedraw /*= TRUE*/)
{
   BOOL bOK = (pFont != NULL);
   if (bOK)
   {
      LOGFONT lf;
#ifdef _WIN32
      bOK = pFont->GetLogFont(&lf);
#else
      bOK = pFont->GetObject(sizeof(lf), &lf);
#endif
      if (bOK)
      {
         CFontDialog* temp = NULL;
         if (pFontDialog == NULL)
         {
            pFontDialog = temp = new CFontDialog;
         }
           
         ((CDialogHack*)pFontDialog)->SetParentMember(pWnd);
         pFontDialog->m_cf.lpLogFont = &lf;
         pFontDialog->m_cf.Flags |= dwFlags | CF_INITTOLOGFONTSTRUCT;
         memcpy(&pFontDialog->m_lf, pFontDialog->m_cf.lpLogFont, sizeof(pFontDialog->m_lf));

         bOK = (pFontDialog->DoModal() == IDOK);
         if (bOK)
         {
            pFont->DeleteObject();
            bOK = pFont->CreateFontIndirect(pFontDialog->m_cf.lpLogFont);
         }
         if (bOK)
         {
            pWnd->SetFont(pFont, bRedraw);
         }
         delete temp;
      }
   }
   return bOK;
}

CMyMultiDocTemplate::CMyMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass) :
   CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}

CDocument* CMyMultiDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible /*= TRUE*/)
{
   if (lpszPathName != NULL)
   {
   #ifdef _WIN32
      class CDocManagerHack : public CDocManager
      {
      public:
         CPtrList* GetTemplateList(void) { return &m_templateList; }
      };
      CPtrList* templateList = ((CDocManagerHack*)AfxGetApp()->m_pDocManager)->GetTemplateList();
   #else
      CPtrList* templateList = &AfxGetApp()->m_templateList;
   #endif   

      CMultiDocTemplate* pTemplate = (CMultiDocTemplate*)templateList->GetHead();
      ASSERT_KINDOF(CMultiDocTemplate, pTemplate);
      if (templateList->GetCount() > 1)
      {
         // more than one document template to choose from
         // bring up dialog prompting user
         CMyNewTypeDlg dlg(templateList);
         int nID = dlg.DoModal();
         if (nID == IDOK)
            pTemplate = (CMultiDocTemplate*)dlg.m_pSelectedTemplate;
         else
            return NULL;     // none - cancel operation
      }

      ASSERT(pTemplate != NULL);
      ASSERT_KINDOF(CDocTemplate, pTemplate);

      return pTemplate->CMultiDocTemplate::OpenDocumentFile(lpszPathName, bMakeVisible);
   }
   else
   {
      return CMultiDocTemplate::OpenDocumentFile(lpszPathName, bMakeVisible);
   }
}

int WINAPI GetClipboardData_StringA(HWND hwnd, char* lpsz, int cc)
{
   int len = 0;
   BOOL bOK = OpenClipboard(hwnd);
   if (bOK)
   {
      HANDLE handle = GetClipboardData(CF_TEXT);
      strncpy(lpsz, (const char*)GlobalLock(handle), cc);
      len = strlen(lpsz);
      GlobalUnlock(handle);
      if (!CloseClipboard())
      {
         bOK = FALSE;
      }
   }
   return len;
}

#ifdef _WIN32
int WINAPI GetClipboardData_StringW(HWND hwnd, wchar_t* lpsz, int cc)
{
   int len = 0;
   BOOL bOK = OpenClipboard(hwnd);
   if (bOK)
   {
      HANDLE handle = GetClipboardData(CF_UNICODETEXT);
      lstrcpynW(lpsz, (const wchar_t*)GlobalLock(handle), cc);
      len = lstrlenW(lpsz);
      GlobalUnlock(handle);
      if (!CloseClipboard())
      {
         bOK = FALSE;
      }
   }
   return len;
}
#endif

#ifdef _UNICODE
   #define GetClipboardData_String GetClipboardData_StringW
#else
   #define GetClipboardData_String GetClipboardData_StringA
#endif

CString MFC_GetClipboardData_String(CWnd* pWnd)
{
   CString str;
   GetClipboardData_String(pWnd->GetSafeHwnd(), str.GetBuffer(512), 512-1);
   str.ReleaseBuffer();
   return str;
}

BOOL MFC_FrameWnd_GetMDIClientSize(SIZE* size, CMDIFrameWnd* pFrame /*= NULL*/)
{
   if (pFrame == NULL)
   {
      pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
      ASSERT(pFrame->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));
   }
   BOOL bOK = (NULL != pFrame);
   if (bOK)
   {
      bOK = !pFrame->IsIconic();
      if (bOK)
      {
         CRect rc;
         ::GetClientRect(pFrame->m_hWndMDIClient, &rc);
         size->cx = rc.Width(), size->cy = rc.Height();
      }
   }  
   return bOK;
}
#ifdef _WIN32
int MFC_ControlBar_OnToolHitTest(const CControlBar& wnd, POINT pt, TOOLINFO* pTI, BOOL bOnlyWhenDocked /*= TRUE*/) // GetWindowText
{
   int nHit = -1;
   if (   (pTI != NULL) 
       && ((!bOnlyWhenDocked) || (bOnlyWhenDocked && (!wnd.IsFloating())) 
       && (0 != wnd.GetWindowTextLength())))
   {
      CString str;
      wnd.GetWindowText(str);
      nHit = MAKELONG(pt.x, pt.y);
      pTI->hwnd = wnd.m_hWnd;
      pTI->uId = (UINT)nHit;
      pTI->rect = CRect(CPoint(pt.x-1,pt.y-1),CSize(2,2));
      pTI->uFlags = 0;
      pTI->lpszText = _tcsdup(str);
   }
   return nHit;
}
#endif

