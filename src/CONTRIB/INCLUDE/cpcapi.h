/* cpcapi.h */
/* Generic CPC api/interface. Copyright (c) Troels K. 2003 */

#ifndef __CPCAPI_H__
#define __CPCAPI_H__

#ifdef _INC_WINDOWS
   #define _BOOL_DEFINED
#endif
#ifndef _BOOL_DEFINED
   #define _BOOL_DEFINED
   typedef signed  int BOOL;
#endif

/* Forward declarations */
struct _CPCDATA;
struct _CPCIMPLEMENTATION;
struct _Z80_REGISTERS;
struct _Z80INTERCEPT;

typedef struct _CPCDATA     * CPCHANDLE;
typedef struct _Z80INTERCEPT* Z80INTERCEPTHANDLE;

typedef struct _CPCAPI
{
   CPCHANDLE (*create)(const TCHAR* szInitString, void* parent);
   void (*close    )(CPCHANDLE*);
   void (*clrscr   )(CPCHANDLE);
   void (*set_mode )(CPCHANDLE, int mode);
   int  (*get_mode )(CPCHANDLE);
   void (*writechar)(CPCHANDLE, TCHAR ch);
   void (*printf   )(CPCHANDLE, const TCHAR* fmt, ...);
   int  (*set_cursor_pos)(CPCHANDLE, int x , int  y);
   BOOL (*get_cursor_pos)(CPCHANDLE, int* x, int* y);
   void (*bell)(CPCHANDLE);
} CPCAPI;

typedef struct _CPCDATA
{
   const CPCAPI* api;
   struct _CPCIMPLEMENTATION* impl;
} CPCDATA;

#ifdef __cplusplus
   extern "C" {
#endif

extern void z80_intercept_function(const struct _Z80_REGISTERS* p, void* lpUser);

typedef void (*Z80INTERCEPTFN)(const struct _Z80_REGISTERS*, void* lpUser);

extern Z80INTERCEPTHANDLE Z80_AddInterceptHandler   (Z80INTERCEPTFN newhandler, void* pUser);
extern void               Z80_RemoveInterceptHandler(Z80INTERCEPTHANDLE*);

extern void CPCAPI_Z80_SetInterceptHandler(CPCHANDLE);
extern void CPCAPI_SetConsoleBreak(BOOL);
extern BOOL CPCAPI_GetTextModeSize(int mode, int* x, int* y);

#ifndef _WINCON_
typedef struct _COORD {
    short X;
    short Y;
} COORD, *PCOORD;
#endif

#define CPC_TEXTMODECOUNT 3
extern const COORD PUB_CPC_TextMode[/* CPC_TEXTMODECOUNT */];

#ifdef __cplusplus
   }
#endif

#endif /* __CPCAPI_H__ */
