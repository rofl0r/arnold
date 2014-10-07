/* libpng/win.c */

/*

  PNG<->DIBSECTION by Troels K. 2003

- LoadPngZ is derived from code found at http://hp.vector.co.jp/authors/VA016379/cpplib/libpng.htm
- LoadPngZ/SavePngZ is implemented using Gilles Vollant's ioapi (unzip Copyright (C) 1998-2003 Gilles Vollant)

*/

#define STRICT
#include <windows.h>
//#define DEBUG #include <dprintf.h>
#include "..\..\..\..\png\png.h"
#include "..\..\..\..\ioapi\ioapiw.h" /* zlib_filefunc_def_s */
#include "..\include\libpngw.h"

#ifndef _countof
   #define _countof(array) (sizeof(array)/sizeof((array)[0]))
#endif

const char* const PUB_text_key[] =
{
   "Title",
   "Author",
   "Description",
   "Software"
};
C_ASSERT(_countof(PUB_text_key) == ENUM_pngtext_enumcount);

//---------------------------------------------------------------------------
// LoadPng用ファイル読み込みプロシージャ

typedef struct _ZFILE
{
   zlib_filefunc_def* api;
   voidpf*            stream;
} ZFILE;
/*
static void PngReadProc_Win32(png_structp png_ptr, png_bytep data, png_size_t length) 
{
   ReadFile(png_get_io_ptr(png_ptr), data, length, (DWORD*)&length, NULL);
}

static void PngReadProc_C(png_structp png_ptr, png_bytep data, png_size_t length) 
{
   if (length != fread(data, 1, length, (FILE*)png_get_io_ptr(png_ptr)))
   {
      png_error(png_ptr, "Failed to read data");
   }
}
*/
static void PNGAPI PngReadProc_Z(png_structp png_ptr, png_bytep data, png_size_t length) 
{
   ZFILE* file = (ZFILE*)png_get_io_ptr(png_ptr);
   if (length != ZREAD(*file->api,file->stream, data, length))
   {
      png_error(png_ptr, "Failed to read data");
   }
}

static void PNGAPI PngWriteProc_Z(png_structp png_ptr, png_bytep data, png_size_t length) 
{
   ZFILE* file = (ZFILE*)png_get_io_ptr(png_ptr);
   if (length != ZWRITE(*file->api,file->stream, data, length))
   {
      png_error(png_ptr, "Failed to write data");
   }
}

//---------------------------------------------------------------------------
// LoadPng用4色形式→16色形式変換関数
void to4bpp(png_structp png_ptr, png_row_infop row_info, png_bytep data) 
{
   static const png_byte pix[] = 
   {
      0x00, 0x01, 0x02, 0x03, 0x10, 0x11, 0x12, 0x13,
      0x20, 0x21, 0x22, 0x23, 0x30, 0x31, 0x32, 0x33,
   };
   png_uint_32 rowb;
   png_byte *p, *q, c;
   
   png_ptr; /* unused */
   rowb = (row_info->width + 1) / 2;
   q = data + rowb;
   p = data + rowb / 2;

   if (rowb % 2 == 1) 
   {
      c = *p;
      *(--q) = pix[c >> 4];
   }
   while (p > data) 
   {
      c = *(--p);
      *(--q) = pix[c & 0x0f];
      *(--q) = pix[c >> 4];
   }
   row_info->bit_depth   = 4;
   row_info->pixel_depth = 4;
   row_info->rowbytes    = rowb;
}

//---------------------------------------------------------------------------
// PNGファイルを読み込む

HBITMAP LoadPngZ(voidpf stream, struct zlib_filefunc_def_s* file_api_ptr, char* pText[/*ENUM_pngtext_enumcount*/])
{
   png_struct* png_ptr;
   png_info  * info_ptr;
   png_info  * end_info;
   HBITMAP hBitmap = NULL;
   BOOL bOK;
   
   ZFILE file;
   file.api    = file_api_ptr;
   file.stream = stream;

   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   bOK = (NULL != png_ptr);
   if (bOK)
   {
      info_ptr = png_create_info_struct(png_ptr);
      bOK = (NULL != info_ptr);
   }
   if (bOK)
   {
      end_info = png_create_info_struct(png_ptr);
      bOK = (NULL != end_info);
   }
   if (bOK)
   {
      png_uint_32 nWidth, nHeight;
      int nDepth, nPal;
      int nPngDepth, nColorType, nInterlaceType, nCompType, nFilterType;
      png_color vPalette[256];
      int nRowBytes, nImgBytes, y, i, j;
      BYTE* pImgPtr;
      BYTE** vRowPtr;
      BITMAPINFO* bi;
      HWND hwnd;
      HDC hdc;
      void* pBits;

      png_set_read_fn(png_ptr, &file, PngReadProc_Z);

	   if (setjmp(png_ptr->jmpbuf))
      {
		   /* If we get here, we had a problem reading the file */
		   free(info_ptr->palette);
         png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		   //png_destroy_write_struct(&png_ptr,  (png_infopp)&info_ptr);
		   return NULL;
	   }
   
      png_read_info(png_ptr, info_ptr);
   
      if (pText != NULL) for (i = 0; i < info_ptr->num_text; i++)
      {
         for (j = 0; j < ENUM_pngtext_enumcount; j++)
         {
            if ( (pText[j] != NULL) && (0 == stricmp(info_ptr->text[i].key, PUB_text_key[j])))
            {
               strcpy(pText[j], info_ptr->text[i].text);
               break;
            }
         }
         //dprintf("%s: %s", info_ptr->text[i].key, info_ptr->text[i].text);
      }
   
      png_get_IHDR(png_ptr, info_ptr, &nWidth, &nHeight, &nPngDepth, &nColorType, &nInterlaceType, &nCompType, &nFilterType);

      switch (nColorType)
      {
         case PNG_COLOR_TYPE_RGB:
         case PNG_COLOR_TYPE_RGB_ALPHA:
            nPngDepth = 24;
            nDepth    = 24;
            nPal      = 0;
            break;
         default:
            switch (nPngDepth) 
            {
               case 2:  nDepth = 4; break;
               case 16: nDepth = 8; break;
               default: nDepth = nPngDepth; break;
            }
            nPal = 1 << nDepth;
            break;
      }

      nRowBytes = (nWidth * nDepth + 31) / 32 * 4;
      nImgBytes = nRowBytes * nHeight;
      pImgPtr = (BYTE*)GlobalAlloc(GMEM_FIXED, nImgBytes);
   
      vRowPtr = (BYTE**)malloc(sizeof(BYTE*) * nHeight);
   
      for (y = 0; y < nHeight; y++)
      {
         vRowPtr[nHeight - (y + 1)] = pImgPtr + y * nRowBytes;
      }

      if (nColorType & PNG_COLOR_MASK_ALPHA)
      {
         png_set_strip_alpha(png_ptr);
      }
      switch (nPngDepth)
      {
         case 2:
            png_set_read_user_transform_fn(png_ptr, to4bpp);
            break;
         case 16:
            png_set_strip_16(png_ptr);
            break;
      }
      switch (nColorType)
      {
         case PNG_COLOR_TYPE_RGB:
         case PNG_COLOR_TYPE_RGB_ALPHA:
            png_set_bgr(png_ptr);
            break;
      }
      png_read_update_info(png_ptr, info_ptr);

      if (nPal > 0) 
      {
         if (nColorType == PNG_COLOR_TYPE_PALETTE) 
         {
            png_color *palette;
            int num_palette;
            png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
            if (num_palette > nPal)
               num_palette = nPal;
            memset(vPalette,       0, nPal * sizeof(png_color));
            memcpy(vPalette, palette, num_palette * sizeof(png_color));
         } 
         else 
         {
            int depth = nPngDepth == 16 ? 8 : nPngDepth;
            png_build_grayscale_palette(depth, vPalette);
         }
      }

      png_read_image(png_ptr, vRowPtr);
      png_read_end(png_ptr, end_info);

      bi = (BITMAPINFO*)GlobalAlloc(GMEM_FIXED, sizeof(BITMAPINFOHEADER) + nPal * sizeof(RGBQUAD));

      memset(bi, 0, sizeof(BITMAPINFOHEADER));
      bi->bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
      bi->bmiHeader.biWidth       = nWidth;
      bi->bmiHeader.biHeight      = nHeight;
      bi->bmiHeader.biPlanes      = 1;
      bi->bmiHeader.biBitCount    = (WORD)nDepth;
      bi->bmiHeader.biCompression = BI_RGB;
      bi->bmiHeader.biSizeImage   = nImgBytes;
      bi->bmiHeader.biClrUsed     = nPal;

      for (i = 0; i < nPal; ++i) 
      {
         bi->bmiColors[i].rgbRed   = vPalette[i].red;
         bi->bmiColors[i].rgbGreen = vPalette[i].green;
         bi->bmiColors[i].rgbBlue  = vPalette[i].blue;
      }

      hwnd = GetDesktopWindow();
      hdc = GetDC(hwnd);
      hBitmap = CreateDIBSection(hdc, bi, DIB_RGB_COLORS, &pBits, NULL, 0);
      if (pBits != NULL)
      {
         memcpy(pBits, pImgPtr, nImgBytes);
      }
      ReleaseDC(hwnd, hdc);

      GlobalFree(pImgPtr);
      GlobalFree(bi);
      free(vRowPtr);
   }
   png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
   return hBitmap;
}

HBITMAP pngw_load(const char *path, struct zlib_filefunc_def_s* file_api_ptr, char* pText[/*ENUM_pngtext_enumcount*/])
{
   HBITMAP bitmap = NULL;
   voidpf stream;
   
   zlib_filefunc_def temp;
   if (file_api_ptr == NULL)
   {
      fill_fopen_filefunc(&temp);
      file_api_ptr = &temp;
   }
   stream = (*file_api_ptr->zopen_file)(file_api_ptr->opaque, path, ZLIB_FILEFUNC_MODE_READ);
   if (stream != NULL)
   {
      bitmap = LoadPngZ(stream, file_api_ptr, pText);
      ZCLOSE(*file_api_ptr,stream);
   }
   return bitmap;
}

HBITMAP pngw_loadw(const wchar_t *path, struct zlib_filefunc_defW_s* file_api_ptr, char* pText[/*ENUM_pngtext_enumcount*/])
{
   HBITMAP bitmap = NULL;
   voidpf stream;
   
   zlib_filefunc_defW temp;
   if (file_api_ptr == NULL)
   {
      fill_fopen_filefuncW(&temp);
      file_api_ptr = &temp;
   }
   stream = (*file_api_ptr->zopen_file)(file_api_ptr->opaque, path, ZLIB_FILEFUNC_MODE_READ);
   if (stream != NULL)
   {
      bitmap = LoadPngZ(stream, (struct zlib_filefunc_def_s*)file_api_ptr, pText);
      ZCLOSE(*file_api_ptr,stream);
   }
   return bitmap;
}

BOOL SavePngZ(voidpf stream, struct zlib_filefunc_def_s* file_api_ptr, int cx, int cy, BYTE** image, int colors, const png_color* palette, const char* pText[/*ENUM_pngtext_enumcount*/])
{
	png_struct *png_ptr;
	png_info *info_ptr;
   int wBPP, row_stride;
   ZFILE file;
   
   file.api    = file_api_ptr;
   file.stream = stream;

   /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also check that
    * the library version is compatible with the one used at compile time,
    * in case we are using dynamically linked libraries.  REQUIRED.
    */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,(void *)NULL,NULL,NULL);
	if (png_ptr == NULL) return FALSE;

	/* Allocate/initialize the image information data.  REQUIRED */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
   {
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return FALSE;
	}

   /* Set error handling.  REQUIRED if you aren't supplying your own
    * error hadnling functions in the png_create_write_struct() call.
    */
	if (setjmp(png_ptr->jmpbuf))
   {
		/* If we get here, we had a problem reading the file */
		free(info_ptr->palette);
		png_destroy_write_struct(&png_ptr,  (png_infopp)&info_ptr);
		return FALSE;
	}
   

   wBPP = ((colors > 256) || (palette == NULL)) ? 24 : ((colors > 16) ? 8 : 4);
   row_stride = ((((wBPP * cx) + 31) / 32) * 4);
	/* set up the output control */
	//png_init_io(png_ptr, &file);
   png_set_write_fn(png_ptr, &file, PngWriteProc_Z, NULL);

	/* set the file information here */
	info_ptr->width         = cx;
	info_ptr->height        = cy;
	info_ptr->pixel_depth   = (BYTE)wBPP;
	info_ptr->channels      = (BYTE)((wBPP>8) ? 3 : 1);
	info_ptr->bit_depth     = (BYTE)(wBPP/info_ptr->channels);
	info_ptr->color_type    = PNG_COLOR_TYPE_RGB;
	info_ptr->compression_type = info_ptr->filter_type = info_ptr->interlace_type=0;
	info_ptr->valid         = 0;
	info_ptr->interlace_type=PNG_INTERLACE_NONE;
   info_ptr->rowbytes = /*(wBPP == 4) ? ((cx / 2) + ((cx % 2) ? 1 : 0)) :*/ row_stride;

   if (pText != NULL)
	{
		time_t		gmt;		// G.M.T.
      int i = 0;
		png_time	mod_time;
		png_text	text_ptr[ENUM_pngtext_enumcount+1];

		time(&gmt);
		png_convert_from_time_t(&mod_time, gmt);
		png_set_tIME(png_ptr, info_ptr, &mod_time);
		
      if ((pText[ENUM_pngtext_title] != NULL) && (*pText[ENUM_pngtext_title]))
      {
		   text_ptr[i].key  = (char*)PUB_text_key[ENUM_pngtext_title];
		   text_ptr[i].text = (char*)pText[ENUM_pngtext_title];
		   text_ptr[i].compression = PNG_TEXT_COMPRESSION_NONE;
         i++;
      }
      if ((pText[ENUM_pngtext_author] != NULL) && (*pText[ENUM_pngtext_author]))
      {
		   text_ptr[i].key  = (char*)PUB_text_key[ENUM_pngtext_author];
		   text_ptr[i].text = (char*)pText[ENUM_pngtext_author];
		   text_ptr[i].compression = PNG_TEXT_COMPRESSION_NONE;
         i++;
      }
      if ((pText[ENUM_pngtext_desc] != NULL) && (*pText[ENUM_pngtext_desc]))
      {
		   text_ptr[i].key  = (char*)PUB_text_key[ENUM_pngtext_desc];
		   text_ptr[i].text = (char*)pText[ENUM_pngtext_desc];
		   text_ptr[i].compression = PNG_TEXT_COMPRESSION_NONE;
         i++;
      }
		text_ptr[i].key  = "Creation Time";
		text_ptr[i].text = png_convert_to_rfc1123(png_ptr, &mod_time);
		text_ptr[i].compression = PNG_TEXT_COMPRESSION_NONE;
      i++;
      if ((pText[ENUM_pngtext_software] != NULL) && (*pText[ENUM_pngtext_software]))
      {
		   text_ptr[i].key  = (char*)PUB_text_key[ENUM_pngtext_software];
		   text_ptr[i].text = (char*)pText[ENUM_pngtext_software];
		   text_ptr[i].compression = PNG_TEXT_COMPRESSION_NONE;
         i++;
      }
		png_set_text(png_ptr, info_ptr, text_ptr, i);
	}
	
   // set the palette if there is one
	switch (wBPP)
   {
      case 4:
      case 8:
		   png_set_IHDR(png_ptr, info_ptr, info_ptr->width, info_ptr->height, info_ptr->bit_depth, 
					   PNG_COLOR_TYPE_PALETTE, info_ptr->interlace_type, 
					   PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		   info_ptr->valid |= PNG_INFO_PLTE;
	      info_ptr->color_type = PNG_COLOR_TYPE_PALETTE;
		   info_ptr->palette     = (png_color*)malloc(sizeof(png_color) * colors);
		   info_ptr->num_palette = (WORD)colors;
         memcpy(info_ptr->palette, palette, sizeof(png_color) * colors);
         break;
      case 24:
         break;
	}
   
   /* write the file information */
	png_write_info(png_ptr, info_ptr);
   png_set_packing(png_ptr); /* min. 1 byte per pixel */
	/* If you are only writing one row at a time, this works */

   png_write_image(png_ptr, image);

   /* It is REQUIRED to call this to finish writing the rest of the file */
	png_write_end(png_ptr, info_ptr);

	/* if you malloced the palette, free it here */
	free(info_ptr->palette);

	/* clean up after the write, and free any memory allocated */
	png_destroy_write_struct(&png_ptr, (png_infopp)&info_ptr);
   return TRUE;
}

static int find_in_palette(png_color color, const png_color* palette, int colorcount)
{
   int j;
   for (j = 0; j < colorcount; j++)
   {
      if (0 == memcmp(&palette[j], &color, sizeof(png_color)))
      {
         return j;
      }
   }
   return -1;
}

BOOL pngw_savestream(voidpf stream, struct zlib_filefunc_def_s* file_api_ptr, int cx, int cy, int (*getpixel)(int x, int y, unsigned char* r, unsigned char* g, unsigned char* b, void* pUser), void* pUser, const char* pText[/*ENUM_pngtext_enumcount*/], BOOL bHalfWidth)
{
   BOOL bOK;
   int wBPP = 24;
   const int row_stride = ((((wBPP * cx) + 31) / 32) * 4);
   int y, colors = 0;
   BYTE** image = (BYTE**)malloc(sizeof(BYTE*) * cy);
   png_color palette[256];

   for (y = 0; y < cy; y++)
   {
		size_t i = 0;
      int x;
      BYTE *row_pointers = (BYTE*)malloc(10+row_stride);

      image[y] = row_pointers;
      for (x = 0; x < cx; x++, i+=sizeof(png_color))
      {
         int iPalette;

         png_color* color = (png_color*)(row_pointers + i);
         (*getpixel)(x, y, &color->red, &color->green, &color->blue, pUser);
         iPalette = find_in_palette(*color, palette, colors);
         if (iPalette == -1)
         {
            if (colors < 256)
            {
               palette[colors] = *color;
            }
            colors++;
         }
         if (bHalfWidth) x++;
      }
	}
   /*
   if (colors <= 16)
   {
      for (y = 0; y < cy; y++)
      {
         BYTE *row_pointers = image[y];
		   size_t i = 0, j = 0;
         for (int x = 0; x < cx; x+=2, j++)
         {
            png_color* color;
            
            color = (png_color*)(row_pointers + i);
            int iPalette0 = find_in_palette(*color, palette, colors);
            i+=sizeof(png_color);
         
            color = (png_color*)(row_pointers + i);
            int iPalette1 = find_in_palette(*color, palette, colors);
            i+=sizeof(png_color);
         
            *(row_pointers + j) = (BYTE)((iPalette1 << 4) || iPalette0); not quite correct
         }
	   }
   }
   else
   */
   if (colors <= 256)
   {
      for (y = 0; y < cy; y++)
      {
         BYTE *row_pointers = image[y];
		   size_t i = 0, j = 0;
         int x;

         for (x = 0; x < cx; x++, i+=sizeof(png_color), j++)
         {
            png_color* color = (png_color*)(row_pointers + i);
            *(row_pointers + j) = (BYTE)find_in_palette(*color, palette, colors);
            if (bHalfWidth) x++;
         }
	   }
   }
//dprintf("%d colors\n", colors);
   bOK = SavePngZ(stream, file_api_ptr, cx / (bHalfWidth ? 2 : 1), cy, image, colors, palette, pText);
   for (y = 0; y < cy; y++)
   {
      free(image[y]);
	}
   free(image);
   return bOK;
}

BOOL pngw_save(const char *path, struct zlib_filefunc_def_s* file_api_ptr, int cx, int cy, int (*getpixel)(int x, int y, unsigned char* r, unsigned char* g, unsigned char* b, void* pUser), void* pUser, const char* pText[/*ENUM_pngtext_enumcount*/], BOOL bHalfWidth)
{
   BOOL bOK;
   voidpf stream;
   
   zlib_filefunc_def temp;
   if (file_api_ptr == NULL)
   {
      fill_fopen_filefunc(&temp);
      file_api_ptr = &temp;
   }
   stream = (*file_api_ptr->zopen_file)(file_api_ptr->opaque, path, ZLIB_FILEFUNC_MODE_WRITE | ZLIB_FILEFUNC_MODE_CREATE);
   bOK = (stream != NULL);
   if (bOK)
   {
      bOK = pngw_savestream(stream, file_api_ptr, cx, cy, getpixel, pUser, pText, bHalfWidth);
      ZCLOSE(*file_api_ptr,stream);
   }
   return bOK;
}

BOOL pngw_savew(const wchar_t *path, struct zlib_filefunc_defW_s* file_api_ptr, int cx, int cy, int (*getpixel)(int x, int y, unsigned char* r, unsigned char* g, unsigned char* b, void* pUser), void* pUser, const char* pText[/*ENUM_pngtext_enumcount*/], BOOL bHalfWidth)
{
   BOOL bOK;
   voidpf stream;
   
   zlib_filefunc_defW temp;
   if (file_api_ptr == NULL)
   {
      fill_fopen_filefuncW(&temp);
      file_api_ptr = &temp;
   }
   stream = (*file_api_ptr->zopen_file)(file_api_ptr->opaque, path, ZLIB_FILEFUNC_MODE_WRITE | ZLIB_FILEFUNC_MODE_CREATE);
   bOK = (stream != NULL);
   if (bOK)
   {
      bOK = pngw_savestream(stream, (struct zlib_filefunc_def_s*)file_api_ptr, cx, cy, getpixel, pUser, pText, bHalfWidth);
      ZCLOSE(*file_api_ptr,stream);
   }
   return bOK;
}

BOOL pngw_setclipboard(HWND hwnd, HBITMAP hBitmap)
{
   BOOL bOK = OpenClipboard(hwnd);
   if (bOK)
   {
      DIBSECTION ds;
      bOK = (sizeof(ds) == GetObject(hBitmap, sizeof(ds), &ds));
      if (bOK)
      {
         const UINT colors = (ds.dsBmih.biBitCount > 8) ? 0 : ds.dsBmih.biClrUsed;
         const size_t bits = GetBitmapBits(hBitmap, 0, NULL);
         HDC memdc = CreateCompatibleDC(NULL);
         HGDIOBJ old;      
         HANDLE handle = GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER) + bits + colors * sizeof(RGBQUAD));
         LPBYTE mem = (LPBYTE)GlobalLock(handle);
         BITMAPINFO* bmi = (BITMAPINFO*)mem;

         bmi->bmiHeader = ds.dsBmih;
         mem+=sizeof(ds.dsBmih);
         old = SelectObject(memdc, hBitmap);
         if (colors != 0)
         {         
            GetDIBColorTable(memdc, 0,colors, bmi->bmiColors);
         }
         mem+=colors * sizeof(RGBQUAD);
         bOK = (ds.dsBmih.biHeight == GetDIBits(memdc, hBitmap, 0,ds.dsBmih.biHeight,mem,bmi, DIB_RGB_COLORS));
         SelectObject(memdc, old);
         GlobalUnlock(handle);
         bOK = (NULL != SetClipboardData(CF_DIB, handle));
      }
      CloseClipboard();
   }
   return bOK;
}
