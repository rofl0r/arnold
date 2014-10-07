// MyView.cpp : implementation file
// CPC edit box. Copyright (c) Troels K. 2003

#include "stdafx.h"

#include "myview.h"
#include "mydoc.h"
#include "..\..\..\arnold\src\win\cpcemu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern const CPCAPI PUB_cpc_editview_api;


/////////////////////////////////////////////////////////////////////////////
// CMyView

IMPLEMENT_DYNCREATE(CMyView, CxEditView)

CMyView::CMyView() : CxEditView()
{
}

CMyView::~CMyView()
{
}


BEGIN_MESSAGE_MAP(CMyView, CxEditView)
	//{{AFX_MSG_MAP(CMyView)
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
   ON_MESSAGE(WM_USER_CHAR, OnCharFromCPC)
   ON_MESSAGE_VOID(WM_USER_SETMODE, OnSetMode)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyView drawing

void CMyView::OnDraw(CDC*)
{
}

/////////////////////////////////////////////////////////////////////////////
// CMyView diagnostics

#ifdef _DEBUG
void CMyView::AssertValid() const
{
	CxEditView::AssertValid();
}

void CMyView::Dump(CDumpContext& dc) const
{
	CxEditView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyView message handlers

void CMyView::ClrScr(void)
{
   CPCHANDLE handle = GetDocument()->m_handle;
   if (handle)
   {
      CxEdit& ctrl = GetEditCtrl();
      ctrl.Empty();
      CSize size;
      ::CPCAPI_GetTextModeSize((*handle->api->get_mode)(handle), (int*)&size.cx, (int*)&size.cy);

      for (int y = 0; y < size.cy; y++)
      {
         if (y != 0) ctrl.AddString(_T("\r\n"));
         ctrl.AddString(CString(' ', size.cx));
      }
      ctrl.GotoBegin();
   }
}

int CMyView::OnCreate(LPCREATESTRUCT lpcs)
{
   /*
   if (m_font.m_hObject == NULL)
   {
      m_font.CreatePointFont(120, _T("Amstrad CPC"));
      // strange: function always return ok, also if Amstrad font not installed
   }
   */
   m_font.CreatePointFont(100, _T("Courier New"));

   if (CxEditView::OnCreate(lpcs) == -1)
   {
		return -1;
   }

   CMyDocument* pDoc = GetDocument();
   CPCHANDLE handle = pDoc->m_handle = (*PUB_cpc_editview_api.create)(NULL, this);
   if (handle == NULL)
   {
      return -1;
   }
   (*handle->api->set_mode)(handle, 1);
   ClrScr();
   (*handle->api->set_cursor_pos)(handle, 1, 1);
	return 0;
}

BOOL CMyView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	BOOL bPreCreated = CxEditView::PreCreateWindow(cs);
	//cs.style &= ~(ES_AUTOVSCROLL | WS_VSCROLL | ES_AUTOHSCROLL|WS_HSCROLL);	// Enable word-wrapping
	cs.style &= ~(WS_VSCROLL | WS_HSCROLL);	// Enable word-wrapping
	cs.style |= ES_READONLY | ES_AUTOVSCROLL | ES_AUTOHSCROLL;

	return bPreCreated;
}

void CMyView::SetMode(void)
{
   CPCHANDLE handle = GetDocument()->m_handle;
   if (handle)
   {
      CDC* pDC = GetDC();
      CFont* old = pDC->SelectObject(&m_font);
      TEXTMETRIC tm;
      pDC->GetTextMetrics(&tm);
      pDC->SelectObject(old);
      ReleaseDC(pDC);

      CSize size;
      ::CPCAPI_GetTextModeSize((*handle->api->get_mode)(handle), (int*)&size.cx, (int*)&size.cy);

      CRect rect(0, 0, tm.tmAveCharWidth * size.cx + 12, tm.tmHeight * size.cy + 30);
      if(0)ResizeParentToFit(CSize(tm.tmAveCharWidth * size.cx, tm.tmHeight * size.cy), FALSE);
   
      if(1)CWnd::CalcWindowRect(rect);
      CFrameWnd* pFrameWnd = GetParentFrame();
	   if(1)pFrameWnd->SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(),		SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
   }
}

void CMyView::OnSetMode(void)
{
   SetMode();
   //SendMessage(WM_USER_CHAR, NULL, 12);
   OnCharFromCPC(0, 12);
}

void CMyView::OnInitialUpdate() 
{
   //ApplyMode();
  
	CxEditView::OnInitialUpdate();

   //CxEdit& ctrl = GetEditCtrl();   ctrl.GotoBegin();
}

void CMyView::ResizeParentToFit(const SIZE& desired_size, BOOL bShrinkOnly)
{
	// determine current size of the client area as if no scrollbars present
	CRect rectClient;
	GetWindowRect(rectClient);
	CRect rect = rectClient;
	CalcWindowRect(rect);
	rectClient.left += rectClient.left - rect.left;
	rectClient.top += rectClient.top - rect.top;
	rectClient.right -= rect.right - rectClient.right;
	rectClient.bottom -= rect.bottom - rectClient.bottom;
	rectClient.OffsetRect(-rectClient.left, -rectClient.top);
	ASSERT(rectClient.left == 0 && rectClient.top == 0);

	// determine desired size of the view
	CRect rectView(0, 0, desired_size.cx, desired_size.cy);
	if (bShrinkOnly)
	{
		if (rectClient.right <= desired_size.cx)
			rectView.right = rectClient.right;
		if (rectClient.bottom <= desired_size.cy)
			rectView.bottom = rectClient.bottom;
	}
	CalcWindowRect(rectView, CWnd::adjustOutside);
	rectView.OffsetRect(-rectView.left, -rectView.top);
	ASSERT(rectView.left == 0 && rectView.top == 0);
	if (bShrinkOnly)
	{
		if (rectClient.right <= desired_size.cx)
			rectView.right = rectClient.right;
		if (rectClient.bottom <= desired_size.cy)
			rectView.bottom = rectClient.bottom;
	}

	// dermine and set size of frame based on desired size of view
	CRect rectFrame;
	CFrameWnd* pFrame = GetParentFrame();
	ASSERT_VALID(pFrame);
	pFrame->GetWindowRect(rectFrame);
	CSize size = rectFrame.Size();
	size.cx += rectView.right - rectClient.right;
	size.cy += rectView.bottom - rectClient.bottom;
	pFrame->SetWindowPos(NULL, 0, 0, size.cx, size.cy,
		SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
}

extern const CPCAPI PUB_cpc_editview_api;

typedef struct _CPCIMPLEMENTATION
{
   int      mode;
   CMyView* session;
   DWORD    thread_handle;
   BOOL     running;
   LPTSTR   szInitString;
} TELNETIMPLEMENTATION;


static void cpc_thread(LPVOID ptr)
{
   CPCHANDLE handle = (CPCHANDLE)ptr;
   CPCEMU_MainLoop(FALSE);
   handle->impl->thread_handle  = 0;
}

static CPCHANDLE create_impl(LPCTSTR szInitString, void* parent)
{
   CPCHANDLE handle = NULL;

   handle = (CPCHANDLE)malloc(sizeof(CPCDATA));   
   handle->impl = (TELNETIMPLEMENTATION*)malloc(sizeof(TELNETIMPLEMENTATION));

   handle->api = &PUB_cpc_editview_api;
   handle->impl->session = (CMyView*)parent;

   CPCAPI_SetConsoleBreak(FALSE);
   CPCAPI_Z80_SetInterceptHandler(handle);
   handle->impl->running = TRUE;
   handle->impl->thread_handle = _beginthread(cpc_thread, 1024, handle);
   handle->impl->szInitString = (szInitString != NULL) ? _tcsdup(szInitString) : NULL;
   return handle;
}

static void close_impl(CPCHANDLE* handle)
{
   ::CPCAPI_SetConsoleBreak(TRUE);
   
   (*handle)->impl->running = FALSE;
   while ((*handle)->impl->thread_handle != 0) /* coordinated shutdown */
   {
      Sleep(0);
   }
   free((*handle)->impl->szInitString);
   free((*handle)->impl);
   free(*handle);
   *handle = NULL;
}

static void clrscr_impl(CPCHANDLE handle)
{
   (*handle->api->writechar)(handle, 12);
}

static void set_mode_impl(CPCHANDLE handle, int mode)
{
   if ((mode >= 0) && (mode < CPC_TEXTMODECOUNT))
   {
      handle->impl->mode = mode;
      //(*handle->api->clrscr)(handle);
      //CString str;      str.Format(_T("\04%c"), mode);      SendMessage(handle->impl->session->m_hWnd, WM_USER_CHAR, (LPARAM)str.operator LPCTSTR(), 0);
      SendMessage(handle->impl->session->m_hWnd, WM_USER_SETMODE, 0, 0);
   }
}

static int get_mode_impl(CPCHANDLE handle)
{
   return handle->impl->mode;
}

LRESULT CMyView::OnCharFromCPC(LPARAM p,WPARAM ch) // WM_USER_CHAR
{
   CxEdit& ctrl = GetEditCtrl();

   CString str;
   if (ch) str = (TCHAR)ch; else str = (LPCTSTR)p;

   CPoint position;
   GetPosition(&position.x, &position.y);

	if(0)TRACE(_T("%2d,%2d: %s\n"), position.x, position.y, str);

   const CSize size = GetSize();
   const int pos = ctrl.LineIndex(position.y);

   switch (str[0])
   {
      case 8:
         //SetPosition(position.x-1, position.y);
         break;
      case 12:
         ClrScr();
         break;
      case 10:
         if (position.y == size.cy-1)
         {
            ctrl.DeleteLine(0);
            ctrl.AddString(CString(' ', size.cx) + _T("\r\n"));
         }
         else
         {
            SetPosition(position.x, position.y + 1);
         }
         break;
      case 13:
         SetPosition(0, position.y);
         break;
      default:
         ctrl.SetSel(pos + position.x, pos + position.x + 1, TRUE);
         ctrl.ReplaceSel(str);
         position.x++;
         if (position.x == size.cx)
         {
            position.y = (position.y + 1) % ctrl.GetLineCount(), position.x = 0;
            SetPosition(position.x, position.y);
         }
         break;
   }
   return 0;
}

void CMyView::SetPosition(int x, int y)
{
   if(0)TRACE(_T("SetPosition(%2d,%2d)\n"), x, y);
   CxEdit& ctrl = GetEditCtrl();
   const CSize size = GetSize();
   ASSERT( (x >= 0) && (x < size.cx));
	ASSERT( (y >= 0) && (y < size.cy));
   int pos = ctrl.LineIndex(y);
   ctrl.SetSel(pos + x, pos + x);
}

void CMyView::GetPosition(int* x, int* y) const
{
   CxEdit& ctrl = GetEditCtrl();
   int start, end;
   ctrl.GetSel(start, end);
   int y1 = ctrl.LineFromChar(start);
   int y2 = ctrl.LineFromChar(end);
   int lineoffset = ctrl.LineIndex(y1);
   if (y) *y = y1;
   if (x) *x = start - lineoffset;
	if(0)TRACE(_T("GetPosition(%2d,%2d)\n"), start - y1, y1);
}

void CMyView::GetPosition(long* x, long* y) const
{
   GetPosition((int*)x, (int*)y);
}

CSize CMyView::GetSize(void) const
{
   CxEdit& ctrl = GetEditCtrl();
	return CSize(ctrl.LineLength(0), ctrl.GetLineCount());
}

LRESULT CMyView::OnSetFont(LPARAM,WPARAM)
{
   LRESULT res =  Default();
   SetMode();
	return res;
}

static void writechar_impl(CPCHANDLE handle, TCHAR ch)
{
   SendMessage(handle->impl->session->m_hWnd, WM_USER_CHAR, NULL, ch);
   //AfxGetApp()->PostThreadMessage(WM_USER_CHAR, (LPARAM)handle->impl->session, ch);
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
   handle->impl->session->SetPosition(x-0,y-0);
   return 0;
}


static BOOL get_cursor_pos_impl(CPCHANDLE handle, int* x, int* y)
{
   handle->impl->session->GetPosition(x,y);
   return TRUE;
}

static void bell_impl(CPCHANDLE handle)
{
}

const CPCAPI PUB_cpc_editview_api = 
{
   create_impl, close_impl, clrscr_impl, set_mode_impl, 
   get_mode_impl, writechar_impl, printf_impl, set_cursor_pos_impl, get_cursor_pos_impl, bell_impl
};

void CMyView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   ::CPCEMU_SetKey_VK(nChar, TRUE, FALSE);
}

void CMyView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
}

void CMyView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   ::CPCEMU_SetKey_VK(nChar, FALSE, FALSE);
}
