/* unicode.c - Troels K. 2003 */

#include <stdlib.h>
#include <string.h>
#include "..\include\unicode.h"

#define ALLOC_STRINGA(len) (char   *)(malloc((len) * sizeof(char)))
#define ALLOC_STRINGW(len) (wchar_t*)(malloc((len) * sizeof(wchar_t)))

char* wcstombs_dup(const wchar_t* wcstr)
{
   int len = wcslen(wcstr) + 1;
   char* szBuffer = ALLOC_STRINGA(len);
   wcstombs(szBuffer, wcstr, len);
   return szBuffer;
}

wchar_t* mbstowcs_dup(const char* mbstr)
{
   int len = strlen(mbstr) + 1;
   wchar_t* szBuffer = ALLOC_STRINGW(len);
   mbstowcs(szBuffer, mbstr, len);
   return szBuffer;
}
