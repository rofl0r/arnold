/* libpng/win.h */

/*

  PNG<->DIBSECTION by Troels K. 2003

- LoadPngZ is derived from code found at http://hp.vector.co.jp/authors/VA016379/cpplib/libpng.htm
- LoadPngZ/SavePngZ is implemented using Gilles Vollant's ioapi (unzip Copyright (C) 1998-2003 Gilles Vollant)

*/

#ifndef __LIBPNGW_H__
#define __LIBPNGW_H__

#ifdef __cplusplus
   extern "C" {
#endif

struct png_color_struct;
struct zlib_filefunc_def_s;
enum
{
   ENUM_pngtext_title,
   ENUM_pngtext_author,
   ENUM_pngtext_desc,
   ENUM_pngtext_software,
   ENUM_pngtext_enumcount
};

#ifdef _ZLIBIOAPI_H
extern HBITMAP LoadPngZ (voidpf stream, struct zlib_filefunc_def_s*, char* pText[/*ENUM_pngtext_enumcount*/]);
extern BOOL SavePngZ(voidpf stream, struct zlib_filefunc_def_s*, int cx, int cy, BYTE** image, int colors, const struct png_color_struct* palette, 
                         const char* pText[/*ENUM_pngtext_enumcount*/]);
#endif

extern HBITMAP pngw_load(const char *path, struct zlib_filefunc_def_s*, char* pText[/*ENUM_pngtext_enumcount*/]);
extern BOOL    pngw_save(const char *path, struct zlib_filefunc_def_s*, int cx, int cy, 
                     int (*getpixel)(int x, int y, unsigned char* r, unsigned char* g, unsigned char* b, void* pUser), 
                     void* pUser, 
                     const char* pText[/*ENUM_pngtext_enumcount*/],
                     BOOL bHalfWidth);

extern const char* const PUB_png_fileext;

/* Helpers */
extern BOOL pngw_setclipboard(HWND, HBITMAP);

#ifdef __cplusplus
   }
#endif

#endif /* __LIBPNGW_H__ */

