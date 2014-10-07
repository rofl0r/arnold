/* cpcapi.c */
/* Generic CPC api/interface. Copyright (c) Troels K. 2003 */
/* This module maps from the CPC world to the CPCAPI world */

#include <tchar.h>
#include "..\include\cpcapi.h"
#include "..\..\cpc\cpc.h"
#include "..\include\cpcfirmware.h"
#ifndef _WINNT_
   #define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]
#endif
#ifndef _countof
   #define _countof(array) (sizeof(array)/sizeof((array)[0]))
#endif
#ifndef _INC_WINDOWS
   typedef unsigned short WORD ;
   typedef unsigned char  BYTE ;
#endif
#define  LIT_soh                       '\x1'
#define  LIT_stx                       '\x2'
#define  LIT_etx                       '\x3'
#define  LIT_eot                       '\x4'
#define  LIT_bel                       '\x7'
#define  LIT_bs                        '\x8'
#define  LIT_tab                       '\t'
#define  LIT_cr                        '\r'
#define  LIT_lf                        '\n'
#define  LIT_nl                        LIT_lf
#define  LIT_ff                        '\f'

/* This functions doesn't know anything about how CPCAPI are implemented. 
   It doesn't know whether the api calls are going to a text window, a graphics window, a Telnet session... */

static void z80_intercept_function(const Z80_REGISTERS* p, void* lpUser)
{
   CPCHANDLE handle = (CPCHANDLE)lpUser;
   switch (p->PC.L)
   {
      case TXT_SET_ROW:
         /*TRACE("y -> %d", p->AF.B.h);*/
         _asm NOP
         break;
      case TXT_SET_COLUMN:
         /*TRACE("x -> %d", p->AF.B.h);*/
         _asm NOP
         break;
      case 0xBBC0:
      case 0xBBC9:
         _asm NOP
         break;
      case TXT_SET_CURSOR:
         (*handle->api->set_cursor_pos)(handle, p->HL.B.h, p->HL.B.l);
         break;
      case TXT_OUT_ACTION:
      {
         enum
         {
            ENUM_char_setmode = 0x04,
            ENUM_char_clrscr  = 0x0C,
            ENUM_char_invers  = 0x18,
            ENUM_char_topleft = 0x1E,
            ENUM_char_setpos  = 0x1F,
            ENUM_char_escape  = 0x1B
         };
         static enum
         {
            ENUM_mode_default,
            ENUM_mode_setpos_x,
            ENUM_mode_setpos_y,
            ENUM_mode_setmode,
         } mode = ENUM_mode_default;
         static union action
         {
            COORD setpos;
         } action;
         BYTE ch = p->AF.B.h;
         switch (mode)
         {
            case ENUM_mode_default:
               switch (ch)
               {
                  case ENUM_char_setmode:
                     mode = ENUM_mode_setmode;
                     break;
                  case ENUM_char_clrscr:
                     (handle->api->clrscr)(handle);
                     break;
                  case ENUM_char_topleft:
                     (*handle->api->set_cursor_pos)(handle, 0, 0);
                     break;
                  case ENUM_char_setpos:
                     mode = ENUM_mode_setpos_x;
                     break;
                  case ENUM_char_invers:
                  case ENUM_char_escape:
                     break;
                  case LIT_bs:
                  case LIT_tab:
                  case LIT_cr:
                  case LIT_lf:
                     (*handle->api->writechar)(handle, ch);
                     break;
                  case LIT_bel:
                     (*handle->api->bell)(handle);
                     break;
                  default:
                     /*printf((ch >= ' ') ? "%c" : "(%d)", ch);*/
                     break;
               }
               break;
            case ENUM_mode_setmode:
               (handle->api->set_mode)(handle, ch);
               mode = ENUM_mode_default;
               break;
            case ENUM_mode_setpos_x:
               action.setpos.X = (short)(ch - 1);
               mode = ENUM_mode_setpos_y;
               break;
            case ENUM_mode_setpos_y:
               action.setpos.Y = (short)(ch - 1);
               (*handle->api->set_cursor_pos)(handle, action.setpos.X, action.setpos.Y);
               mode = ENUM_mode_default;
               break;
         }
         break;
      }
      case SCR_SET_MODE:
         (*handle->api->set_mode)(handle, p->AF.B.h);
         break;
      case TXT_CLR_WINDOW:
         (*handle->api->clrscr)(handle);
         break;
      case TXT_WRITE_CHAR:
      {
         BYTE ch = p->AF.B.h;
         (*handle->api->writechar)(handle, ch);
         break;
      }
      case GRA_WR_CHAR:
      case TXT_WR_CHAR:
      case TXT_OUTPUT:
      {
         /*
         BYTE ch = p->AF.B.h;
         if(0)TRACE((ch >= ' ') ? "%c" : "(%d)", ch);
         */
         break;
      }
   }
}

const COORD PUB_CPC_TextMode[/* CPC_TEXTMODECOUNT */] =
{
   {
      20, 25
   },
   {
      40, 25
   },
   {
      80, 25
   }
};
C_ASSERT(CPC_TEXTMODECOUNT == _countof(PUB_CPC_TextMode));

BOOL CPCAPI_GetTextModeSize(int mode, int* x, int* y)
{
   BOOL bOK = (mode >= 0) && (mode < CPC_TEXTMODECOUNT);
   if (bOK)
   {
      if (x) *x = PUB_CPC_TextMode[mode].X;
      if (y) *y = PUB_CPC_TextMode[mode].Y;
   }
   return bOK;
}

void CPCAPI_Z80_SetInterceptHandler(CPCHANDLE handle)
{
   Z80_AddInterceptHandler(z80_intercept_function, handle);
}

typedef struct _Z80INTERCEPT
{
   Z80INTERCEPTFN function;
   void* pUser;
   struct _Z80INTERCEPT* next;
} Z80INTERCEPT;

static Z80INTERCEPT* MOD_intercept = NULL;

Z80INTERCEPTHANDLE Z80_AddInterceptHandler(Z80INTERCEPTFN function, void* pUser)
{
   Z80INTERCEPTHANDLE handle;
   Z80INTERCEPT** element;
   for (element = &MOD_intercept; *element; element = &(*element)->next)
   {
   }
   handle = *element = (Z80INTERCEPT*)malloc(sizeof(Z80INTERCEPT));
   handle->function = function;
   handle->pUser    = pUser;
   handle->next     = NULL;
   return handle;
}

void Z80_RemoveInterceptHandler(Z80INTERCEPTHANDLE* handle)
{
   Z80INTERCEPT** element;
   for (element = &MOD_intercept; *element; element = &(*element)->next)
   {
      if (*element == *handle)
      {
         *element = (*handle)->next;
         break;
      }
   }
   free(*handle);
   *handle = NULL;
}

void Z80_InterceptChain(const Z80_REGISTERS* p)
{
   Z80INTERCEPT* element;
   Z80INTERCEPT* next;
   for (element = MOD_intercept; element; )
   {
      next = element->next; /* preserve next */
      (*element->function)(p, element->pUser); /* element might not longer be valid after this call */
      element = next;
   }
}

