/* 
 * (c) Kevin Thacker 2002
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "cmdline.h"



/* at Win2000 command-line:

  filename on own = no commandline
  filename and parameter = command line

  if filename and path has spaces in it, then filename and path will be in quotes.

*/


/* free command-line data */
void CommandLineData_Free(CommandLineData *pCommandLineData)
{
	if (pCommandLineData->pCommandLine!=NULL)
	{
		free(pCommandLineData->pCommandLine);
		pCommandLineData->pCommandLine = NULL;
	}

	if (pCommandLineData->argv!=NULL)
	{
		free(pCommandLineData->argv);
		pCommandLineData->pCommandLine = NULL;
	}

	pCommandLineData->argc = 0;
}

void CommandLineData_Init(CommandLineData *pCommandLineData)
{
	pCommandLineData->pCommandLine = NULL;
	pCommandLineData->argc = 0;
	pCommandLineData->argv = NULL;
}

/* process the command-line and convert to argv and argc */
BOOL CommandLineData_Create(CommandLineData *pCommandLineData,LPCTSTR theCommandLine)
{
	unsigned long CommandLineLength;

	// get length of command-line in bytes
	CommandLineLength=_tcslen(theCommandLine);

	CommandLineData_Init(pCommandLineData);

	// command-line specified?
	if (CommandLineLength!=0)
	{
		int nLength;

		nLength = (CommandLineLength+1)*sizeof(TCHAR);

		// allocate space for local copy of command-line
		pCommandLineData->pCommandLine = (TCHAR *)malloc(nLength);

		if (pCommandLineData->pCommandLine!=NULL)
		{
			BOOL bParseComplete = FALSE;
			BOOL bError = FALSE;
			int Code;
			TCHAR *pPtr;
			TCHAR *pStart;
			TCHAR ch;

			/* copy command-line into local copy */
			memcpy(pCommandLineData->pCommandLine, theCommandLine, nLength);

			/* process command-line and convert into argv, argc type */
			pPtr = pCommandLineData->pCommandLine;

			/* initial state */
			Code = COMMAND_LINE_PARSE_LOOKING;
			
			/* count number of parameters */
			do
			{
				/* get char */
				ch = pPtr[0];

				switch (Code)
				{
					case COMMAND_LINE_PARSE_LOOKING:
					{
						/* end of string? */
						if (ch==_T('\0'))
						{
							/* end of string found and no error */
							bParseComplete = TRUE;
							break;
						}

						/* quoted string? */
						if (ch==_T('"'))
						{
							/* found quoted string */
							Code = COMMAND_LINE_PARSE_PROCESS_QUOTED_STRING;

							/* start is the next character */
							pStart = pPtr+1;

							pCommandLineData->argc++;
							break;
						}

						/* a non-whitespace character? */
						if ((ch!=_T(' ')) && (ch!=_T('\t')))
						{
							Code = COMMAND_LINE_PARSE_PROCESS_STRING_SEQUENCE;
							pStart = pPtr;
							
							pCommandLineData->argc++;
							break;
						}

						/* other characters are ignored */
					}
					break;

					case COMMAND_LINE_PARSE_PROCESS_QUOTED_STRING:
					{
						/* end of string? */
						if (ch==_T('\0'))
						{
							/* end of string found, and closing quote not found - error */
							bError = TRUE;
							bParseComplete = TRUE;
							break;
						}

						/* closing quote? */
						if (ch==_T('"'))
						{
							/* closing quote found */
							
							/* back to looking */
							Code = COMMAND_LINE_PARSE_LOOKING;
						}

						/* all other chars are part of quoted string */
					}
					break;

					case COMMAND_LINE_PARSE_PROCESS_STRING_SEQUENCE:
					{
						/* white-space char or end of string? */
						if ((ch==_T(' ')) || (ch==_T('\t')) || (ch==_T('\0')))
						{
							/* if end of string, quit now */
							if (ch==0)
							{
								bParseComplete = TRUE;
								break;
							}

							/* back to looking */
							Code = COMMAND_LINE_PARSE_LOOKING;
						}
						
						/* all other chars are part of string sequence */
					}
					break;

					/* should not get here */
					default:
					{
						bError = TRUE;
						break;
					}
				}

				pPtr++;
			}
			while (!bParseComplete);

			if (!bError)
			{
				bParseComplete = FALSE;

				/* allocate argv array */
				pCommandLineData->argv = (TCHAR **)malloc(sizeof(TCHAR *)*pCommandLineData->argc);
				/* reset count */
				pCommandLineData->argc = 0;

				/* process command-line and convert into argv, argc type */
				pPtr = pCommandLineData->pCommandLine;

				/* initial state */
				Code = COMMAND_LINE_PARSE_LOOKING;


				do
				{
					/* get char */
					ch = pPtr[0];

					switch (Code)
					{
						case COMMAND_LINE_PARSE_LOOKING:
						{
							/* end of string? */
							if (ch==_T('\0'))
							{
								/* end of string found and no error */
								bParseComplete = TRUE;
								break;
							}

							/* quoted string? */
							if (ch==_T('"'))
							{
								/* found quoted string */
								Code = COMMAND_LINE_PARSE_PROCESS_QUOTED_STRING;
								/* start is the next character */
								pStart = pPtr+1;
								break;
							}

							/* a non-whitespace character? */
							if ((ch!=_T(' ')) && (ch!=_T('\t')))
							{
								Code = COMMAND_LINE_PARSE_PROCESS_STRING_SEQUENCE;
								pStart = pPtr;
								break;
							}

							/* other characters are ignored */
						}
						break;

						case COMMAND_LINE_PARSE_PROCESS_QUOTED_STRING:
						{
							/* end of string? */
							if (ch==_T('\0'))
							{
								/* end of string found, and closing quote not found - error */
								bError = TRUE;
								bParseComplete = TRUE;
								break;
							}

							/* closing quote? */
							if (ch==_T('"'))
							{
								/* closing quote found */

								/* finish this string */
								pPtr[0] = _T('\0');

								/* update argv list */
								pCommandLineData->argv[pCommandLineData->argc] = pStart;
								pCommandLineData->argc++;

								/* back to looking */
								Code = COMMAND_LINE_PARSE_LOOKING;
							}

							/* all other chars are part of quoted string */
						}
						break;

						case COMMAND_LINE_PARSE_PROCESS_STRING_SEQUENCE:
						{
							/* white-space char or end of string? */
							if ((ch==_T(' ')) || (ch==_T('\t')) || (ch==_T('\0')))
							{
								/* finish this string */
								pPtr[0] = _T('\0');

								/* update argv list */
								pCommandLineData->argv[pCommandLineData->argc] = pStart;
								pCommandLineData->argc++;

								/* if end of string, quit now */
								if (ch==0)
								{
									bParseComplete = TRUE;
									break;
								}

								/* back to looking */
								Code = COMMAND_LINE_PARSE_LOOKING;
							}
							
							/* all other chars are part of string sequence */
						}
						break;

						/* should not get here */
						default:
						{
							bError = TRUE;
							break;
						}
					}

					pPtr++;
				}
				while (!bParseComplete);
			}

			if (bError)
			{
				// free command-line copy
				CommandLineData_Free(pCommandLineData);
				return FALSE;
			}

		}
	}

	return TRUE;
}
