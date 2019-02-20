#ifndef __ZIP_SUPPORT_HEADER_INCLUDED__
#define __ZIP_SUPPORT_HEADER_INCLUDED__

#include "../cpc/cpcglob.h"
#include "file_list.h"
#include <tchar.h>

enum
{
	ZIP_SUPPORT_OK = 0,
	ZIP_SUPPORT_NOT_ARCHIVE,
	ZIP_SUPPORT_ERROR_IN_ARCHIVE,
};


int ZipSupport_IsZipArchive(const TCHAR *pZipFilename);
void ZipSupport_GenerateFileList(const TCHAR *pZipFilename, file_list *);
BOOL	ZipSupport_ExtractFile(const TCHAR *pZipFilename, const char *pFilename, const TCHAR *pOutputFilename);

#endif