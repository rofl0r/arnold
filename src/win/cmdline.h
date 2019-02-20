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
#ifndef __COMMAND_LINE_PARSER_INCLUDED__
#define __COMMAND_LINE_PARSER_INCLUDED__

/* windows command-line parser, breaks command line string into argv and argc arrays for
furthur processing */

#include <windows.h>
#include <tchar.h>

enum
{
	COMMAND_LINE_PARSE_LOOKING = 0,
	COMMAND_LINE_PARSE_PROCESS_QUOTED_STRING,
	COMMAND_LINE_PARSE_PROCESS_STRING_SEQUENCE
};

typedef struct
{
	/* a buffer created to store command line */
	TCHAR *pCommandLine;
	/* parameters */
	TCHAR **argv;
	/* parameter count */
	int	 argc;
} CommandLineData;

/* free command-line data */
void CommandLineData_Free(CommandLineData *pCommandLineData);

void CommandLineData_Init(CommandLineData *pCommandLineData);

/* process the command-line and convert to argv and argc */
BOOL CommandLineData_Create(CommandLineData *pCommandLineData,LPCTSTR theCommandLine);


#endif