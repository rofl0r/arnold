#ifndef __LANGUAGE_HELPER_HEADER_INCLUDED__
#define __LANGUAGE_HELPER_HEADER_INCLUDED__

#include "cpcglob.h"

const char *Language_GetString(unsigned long ID);

typedef struct
{
	/* language name; in this language */
	/* e.g. Deutsch for German */
	const char *Name;

	const char **Strings;
} language_data;

/* number of languages defined */
unsigned long	 Language_GetNumLanguages(void);
/* load language data; add to internal list */
BOOL	Language_Load(char *pDataFile);

#endif
