#ifndef __ZIP_SUPPORT_UI_HEADER_INCLUDED__
#define __ZIP_SUPPORT_UI_HEADER_INCLUDED__

#include <windows.h>

#include "zipsupport.h"

typedef struct
{
	const char *pFilename;	 /* filename selected; filename using zip charset */
} archive_file_open;

BOOL ArchiveUI_Show(HWND hParent, file_list *fileList, archive_file_open *open);


#endif