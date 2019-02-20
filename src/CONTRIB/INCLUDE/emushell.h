/* emushell.h */
/* Generic CPC emulator app api/interface. Copyright (c) Troels K. 2003 */

#ifndef __EMUSHELL_H__
#define __EMUSHELL_H__

#ifdef _INC_WINDOWS
   #define _BOOL_DEFINED
#endif
#ifndef _BOOL_DEFINED
   #define _BOOL_DEFINED
   typedef signed  int BOOL;
#endif
#ifndef FALSE
   #define FALSE 0
#endif
#ifndef TRUE
   #define TRUE  1
#endif

enum emushell_version
{
   LIT_current_version = 0x0100
};

#define EMUSHELL_ENTRYPOINT      emushell_open
#define EMUSHELL_ENTRYPOINT_TEXT "emushell_open"

struct _EMUSHELL;
typedef struct _EMUSHELL *EMUHANDLE;

enum emupath
{
   ENUM_emupath_exe,
   ENUM_emupath_rom,
   ENUM_emupath_snapshot,
   ENUM_emupath_snapshot_screen,
   ENUM_emupath_enumcount
};

typedef struct _EMUAPI
{
   EMUHANDLE (*create   )(LPCTSTR lpszPath/* = NULL */);
   void   (*put_exe_path)(EMUHANDLE, LPCTSTR lpszPath);
   HANDLE (*run         )(EMUHANDLE, HWND hwndParent, int argc, TCHAR* argv[]);
   BOOL   (*get_disk    )(EMUHANDLE, int no, LPTSTR  lpszBuffer, int cbBuffer); /* lpszBuffer = NULL is ok */
   BOOL   (*put_disk    )(EMUHANDLE, int no, LPCTSTR lpszBuffer);
   BOOL   (*get_rom     )(EMUHANDLE, int no, LPTSTR  lpszBuffer, int cbBuffer); /* lpszBuffer = NULL is ok */
   BOOL   (*put_rom     )(EMUHANDLE, int no, LPCTSTR lpszName);
   int    (*get_name    )(EMUHANDLE, LPTSTR lpszBuffer, int cbBuffer);
   BOOL   (*get_fileprofile)(EMUHANDLE, LPTSTR lpszBuffer, int cbBuffer);
   BOOL   (*get_path    )(EMUHANDLE, enum emupath, LPTSTR lpszBuffer, int cbBuffer);
   BOOL   (*put_path    )(EMUHANDLE, enum emupath, LPCTSTR lpszBuffer);
   void   (*close       )(EMUHANDLE*);
} EMUAPI;

struct _EMUIMPLEMENTATION;

typedef struct _EMUSHELL
{
   const EMUAPI* api;
   struct _EMUIMPLEMENTATION* impl; /* instance data */
} EMUSHELL;

#ifdef __cplusplus
#include "emushell.hpp"
#endif

#endif /* __EMUSHELL_H__ */
