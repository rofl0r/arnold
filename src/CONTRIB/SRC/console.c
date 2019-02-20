/* console.c */
/* Win32 console mode support. Copyright (c) Troels K. 2003 */
/* This module knows how to use the console. It knows nothing about the cpc. */

#include "../../win/precomp.h"
#include "..\..\win\cpcemu.h"
#include "..\include\console.h"
#include "..\include\autoexec.h"
#include <conio.h>

#include <io.h> /* _open_osfhandle */
#include <fcntl.h> /* _O_TEXT */
#include <process.h> /* _beginthread */
#ifndef _countof
   #define _countof(array) (sizeof(array)/sizeof((array)[0]))
#endif

#define VERIFY(e) e

typedef struct _CPCIMPLEMENTATION
{
   HANDLE hConOut;
   FILE* output_stream;
   int mode;
   DWORD thread_handle;
   BOOL running;
   AUTOEXECHANDLE autoexec;
} CONSOLEIMPLEMENTATION;

#define BACKGROUND_WHITE  (WORD) 0x00f0
#define BACKGROUND_CYAN   (WORD) 0x0030
#define FOREGROUND_YELLOW (WORD) 0x0006
#define FOREGROUND_CYAN   (WORD) 0x0003
#define FOREGROUND_WHITE  (WORD) 0x0007

static void SetConsoleSize(HANDLE hConsole, SHORT xSize, SHORT ySize);
static void cpc_keyboard_thread(void* ptr);
static BOOL WINAPI consolectrl_function(DWORD dwCtrlType);

static CPCHANDLE create_impl(LPCTSTR szInitString, void* parent)
{
   COORD dwWriteCoord = {0, 0}; /* where to write the screen attributes */
   DWORD cCharsWritten;
   CONSOLE_CURSOR_INFO cursor;
   COORD textmode;
   
   CPCHANDLE handle = (CPCHANDLE)malloc(sizeof(CPCDATA));
   handle->impl = (CONSOLEIMPLEMENTATION*)malloc(sizeof(CONSOLEIMPLEMENTATION));
   CPCAPI_SetConsoleBreak(FALSE);

   AllocConsole();
   *stdout = *_fdopen(_open_osfhandle((long) GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT), "w" );
   setvbuf( stdout, NULL, _IONBF, 0);
   *stdin  = *_fdopen(_open_osfhandle((long) GetStdHandle(STD_INPUT_HANDLE), _O_TEXT), "r" );
   setvbuf( stdin, NULL, _IONBF, 0);

   handle->api = &PUB_cpc_console_api;
   handle->impl->hConOut = CreateFile(_T("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
   handle->impl->output_stream = stdout;

   (*handle->api->set_mode)(handle, 1);
   textmode = PUB_CPC_TextMode[(*handle->api->get_mode)(handle)];
   
   SetConsoleTextAttribute(handle->impl->hConOut, FOREGROUND_YELLOW | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
   FillConsoleOutputAttribute(handle->impl->hConOut, BACKGROUND_BLUE, textmode.X * textmode.Y, dwWriteCoord, &cCharsWritten);
   
   /*(*handle->api->printf)(handle, _T("Arnold: K. Thacker  -  Console:Troels K."));*/
   (*handle->api->set_cursor_pos)(handle, 1, 1);

   cursor.bVisible = TRUE;
   cursor.dwSize = 100;
   SetConsoleCursorInfo(handle->impl->hConOut, &cursor);
   
   CPCAPI_Z80_SetInterceptHandler(handle);

   handle->impl->running = TRUE;
   handle->impl->thread_handle = _beginthread(cpc_keyboard_thread, 1024, handle);
   
   handle->impl->autoexec = autoexec_open(szInitString);

   SetConsoleCtrlHandler(consolectrl_function, TRUE);
   
   return handle;
}

static void close_impl(CPCHANDLE* handle)
{
   (*handle)->impl->running = FALSE;
   while ((*handle)->impl->thread_handle != 0) /* coordinated shutdown */
   {
      Sleep(0);
   }
   if ((*handle)->impl->autoexec) autoexec_close(&(*handle)->impl->autoexec);
   CloseHandle((*handle)->impl->hConOut);
   free((*handle)->impl);
   free(*handle);
   *handle = NULL;
   FreeConsole();
}

static void clrscr_impl(CPCHANDLE handle)
{
   CONSOLE_SCREEN_BUFFER_INFO csbi;
   BOOL bOK = GetConsoleScreenBufferInfo(handle->impl->hConOut, &csbi);
   if (bOK)
   {
      const COORD coordScreen = {0, 0}; /* location to start the attribute fill */
      DWORD dw = 0;
      bOK = SetConsoleCursorPosition(handle->impl->hConOut, coordScreen);
      FillConsoleOutputCharacter(handle->impl->hConOut, ' ', csbi.dwSize.X * csbi.dwSize.Y, coordScreen, &dw);
      SetConsoleCursorPosition(handle->impl->hConOut, coordScreen);
   }
}

static void set_mode_impl(CPCHANDLE handle, int mode)
{
   if ((mode >= 0) && (mode < CPC_TEXTMODECOUNT))
   {
      handle->impl->mode = mode;
      SetConsoleSize(handle->impl->hConOut, PUB_CPC_TextMode[mode].X, PUB_CPC_TextMode[mode].Y);
      (*handle->api->clrscr)(handle);
   }
}

static int get_mode_impl(CPCHANDLE handle)
{
   return handle->impl->mode;
}

static void printf_impl(CPCHANDLE handle, const TCHAR* fmt, ...)
{
   va_list args;
	va_start(args, fmt);
   _vftprintf(handle->impl->output_stream, fmt, args);
	va_end(args);
}

static void writechar_impl(CPCHANDLE handle, TCHAR ch)
{
   _ftprintf(handle->impl->output_stream, _T("%c"), ch);
}

static int set_cursor_pos_impl(CPCHANDLE handle, int x , int  y)
{
   COORD pos;
   pos.X = (short)x;
   pos.Y = (short)y;
   return SetConsoleCursorPosition(handle->impl->hConOut, pos);
}

static BOOL get_cursor_pos_impl(CPCHANDLE handle, int* x, int* y)
{
   CONSOLE_SCREEN_BUFFER_INFO info;
   BOOL bOK = GetConsoleScreenBufferInfo(handle->impl->hConOut, &info);
   if (bOK)
   {
      if(x) *x = info.dwCursorPosition.X;
      if(y) *y = info.dwCursorPosition.Y;
   }
   return bOK;
}

static void bell_impl(CPCHANDLE handle)
{
   handle;
   MessageBeep(0);
}

const CPCAPI PUB_cpc_console_api = 
{
   create_impl, close_impl, clrscr_impl, set_mode_impl, 
   get_mode_impl, writechar_impl, printf_impl, set_cursor_pos_impl, get_cursor_pos_impl, bell_impl
};

/*********************************************************************
* FUNCTION: SetConsoleSize(HANDLE hConsole, SHORT xSize,      *
*                                 SHORT ySize)                       *
*                                                                    *
* PURPOSE: resize both the console output buffer and the console     *
*          window to the given x and y size parameters               *
*                                                                    *
* INPUT: the console output handle to resize, and the required x and *
*        y size to resize the buffer and window to.                  *
*                                                                    *
* COMMENTS: Note that care must be taken to resize the correct item  *
*           first; you cannot have a console buffer that is smaller  *
*           than the console window.                                 *
*********************************************************************/

void SetConsoleSize(HANDLE hConsole, SHORT xSize, SHORT ySize)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi; /* hold current console buffer info */
  BOOL bSuccess;
  SMALL_RECT srWindowRect; /* hold the new console size */
  COORD coordScreen;

  bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
  /* get the largest size we can size the console window to */
  coordScreen = GetLargestConsoleWindowSize(hConsole);
  /* define the new console window size and scroll position */
  srWindowRect.Right = (SHORT) (min(xSize, coordScreen.X) - 1);
  srWindowRect.Bottom = (SHORT) (min(ySize, coordScreen.Y) - 1);
  srWindowRect.Left = srWindowRect.Top = (SHORT) 0;
  /* define the new console buffer size */
  coordScreen.X = xSize;
  coordScreen.Y = ySize;
  /* if the current buffer is larger than what we want, resize the */
  /* console window first, then the buffer */
  if ((DWORD) csbi.dwSize.X * csbi.dwSize.Y > (DWORD) xSize * ySize)
    {
    bSuccess = SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect);
    bSuccess = SetConsoleScreenBufferSize(hConsole, coordScreen);
    }
  /* if the current buffer is smaller than what we want, resize the */
  /* buffer first, then the console window */
  if ((DWORD) csbi.dwSize.X * csbi.dwSize.Y < (DWORD) xSize * ySize)
    {
    bSuccess = SetConsoleScreenBufferSize(hConsole, coordScreen);
    bSuccess = SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect);
    }
  /* if the current buffer *is* the size we want, don't do anything! */
  return;
}

static void cpc_keyboard_thread(void* pUser)
{
   CPCHANDLE handle = (CPCHANDLE)pUser;
   HANDLE hConIn = CreateFile(_T("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

   while (handle->impl->running)
   {
      INPUT_RECORD input;
      DWORD dw;
      BOOL bOK;

      switch (WaitForSingleObject(hConIn, INFINITE))
      {
         case WAIT_OBJECT_0:
            bOK = ReadConsoleInput(hConIn, &input, 1, &dw) && (input.EventType == KEY_EVENT);
            if (bOK)
            {
               const KEY_EVENT_RECORD* key = &input.Event.KeyEvent;
               BOOL bEnhanced = (key->uChar.AsciiChar == 0) || (key->dwControlKeyState & ENHANCED_KEY);
               BOOL bASCII    = !bEnhanced;
            #ifdef _DEBUG
               if (key->bKeyDown)
               _asm NOP
               if (key->wVirtualKeyCode == VK_NEXT)
               _asm NOP
               if (key->wVirtualKeyCode == VK_RETURN)
               _asm NOP
            #endif               
               //CPCEMU_SetKey_VK(isalpha(key->wVirtualKeyCode) ? key->uChar.AsciiChar : key->wVirtualKeyCode, key->bKeyDown);
               CPCEMU_SetKey_VK(bASCII ? key->uChar.AsciiChar : key->wVirtualKeyCode, key->bKeyDown, bEnhanced);
               
               if (   (!key->bKeyDown) 
                   && (VK_BACK == key->wVirtualKeyCode))
               {
                  (*handle->api->writechar)(handle, VK_BACK); /* there must be a better way! */
               }
            }
            break;
         default:
            _asm NOP
            break;
      }
   }
   handle->impl->thread_handle = 0;
   CloseHandle(hConIn);
}

static BOOL WINAPI consolectrl_function(DWORD dwCtrlType)
{
   CPCAPI_SetConsoleBreak(TRUE);
   return TRUE;
}

BOOL ConsoleBreak = FALSE;
void CPCAPI_SetConsoleBreak(BOOL bSet)
{
   ConsoleBreak = bSet;
}
