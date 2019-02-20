/* 
 *  Arnold emulator (c) Copyright, Kevin Thacker 1995-2001
 *  
 *  This file is part of the Arnold emulator source code distribution.
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
#include <ctype.h>
#include "../cpcglob.h"

#include "breakpt.h"

typedef struct BREAKPOINT
{
	/* breakpoint address */
	int Address;
	
	/* next breakpoint */
	struct BREAKPOINT *pNext;
} BREAKPOINT;

static BREAKPOINT *pFirstBreakpoint = NULL;


/* add a breakpoint */
void Breakpoints_AddBreakpoint(int Address)
{
	BREAKPOINT *pBreakpoint;

	/* already set as a breakpoint? */
	if (Breakpoints_IsABreakpoint(Address))
		return;

	/* no, add it. */
	pBreakpoint = (BREAKPOINT *)malloc(sizeof(BREAKPOINT));

	if (pBreakpoint!=NULL)
	{
		pBreakpoint->Address = Address & 0x0ffff;
		pBreakpoint->pNext = pFirstBreakpoint;
		pFirstBreakpoint = pBreakpoint;
	}
}

/* remove a breakpoint */
void	Breakpoints_RemoveBreakpoint(int Address)
{
	/* breakpoint exists? */
	if (Breakpoints_IsABreakpoint(Address))
	{
		/* yes. Remove it */
		BREAKPOINT *pBreakpoint,*pPrev;

		/* previous breakpoint entry */
		pPrev = NULL;
		/* current breakpoint entry */
		pBreakpoint = pFirstBreakpoint;

		while (pBreakpoint!=NULL)
		{
			/* address matches */
			if (pBreakpoint->Address == (Address & 0x0ffff))
			{
				/* found the breakpoint */

				/* first in list? */
				if (pPrev!=NULL)
				{
					/* no */
					pPrev->pNext = pBreakpoint->pNext;
				}
				else
				{
					/* yes */
					pFirstBreakpoint = pBreakpoint->pNext;
				}

				/* free it */
				free(pBreakpoint);
				break;
			}

			pPrev = pBreakpoint;
			pBreakpoint = pBreakpoint->pNext;

		}
	}
}

/* free all breakpoints */
void	Breakpoints_Free(void)
{
	BREAKPOINT *pBreakpoint;

	pBreakpoint = pFirstBreakpoint;

	while (pBreakpoint!=NULL)
	{
		BREAKPOINT *pNext;

		pNext = pBreakpoint->pNext;

		free(pBreakpoint);

		pBreakpoint = pNext;
	}

	pFirstBreakpoint = NULL;
}


/* true if the address matches any of the breakpoints */
BOOL Breakpoints_IsABreakpoint(int Address)
{
	BREAKPOINT *pBreakpoint;

	pBreakpoint = pFirstBreakpoint;

	while (pBreakpoint!=NULL)
	{
		if (pBreakpoint->Address == (Address & 0x0ffff))
			return TRUE;

		pBreakpoint = pBreakpoint->pNext;
	}

	return FALSE;
}
