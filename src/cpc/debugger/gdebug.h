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
#ifndef __GENERIC_DEBUGGER_HEADER_INCLUDED__
#define __GENERIC_DEBUGGER_HEADER_INCLUDED__

#include "../cpcglob.h" 


int		ASIC_DMA_GetOpcodeCount(int);
void	ASIC_DMA_DissassembleInstruction(int, char *);


enum
{
	DEBUG_STOPPED,
	DEBUG_RUNNING,
	DEBUG_HIT_BREAKPOINT,
	DEBUG_STEP_INTO
} DEBUG_STATE;


/*****************************************************************************/
/* DEBUG COMPARISON CODE */
/* NOT FUNCTIONAL YET */
/* CompareWhat types */

#define 	DEBUG_CMP_ADDR	0x0001		/* compare addr */
#define		DEBUG_CMP_DATA	0x0002		/* compare data */
#define		DEBUG_CMP_ADDR_AND_DATA (DEBUG_CMP_ADDR | DEBUG_CMP_DATA)	/* compare addr and data */

/* Comparison types */
typedef enum
{
	DEBUG_CMP_ANY = 0,			/* any value */
	DEBUG_CMP_RANGE,			/* range of values */
	DEBUG_CMP_AND_MASK,			/* AND with value 1, if result is value2, halt */
	DEBUG_CMP_EQUAL,			/* equals value */
	DEBUG_CMP_NOT_EQUAL			/* not equal to value */
} DEBUG_CMP_TYPE;

typedef struct
{
	DEBUG_CMP_TYPE Comparison;					/* comparison type */
	int Value1, Value2;				/* values to compare against */
} DEBUG_CMP_SUB_STRUCT;

typedef struct
{
	int Enabled;							/* TRUE if enabled, FALSE if disabled */
	int CompareWhat;						/* describes which details to look at */
	DEBUG_CMP_SUB_STRUCT	AddressCompare;	/* compare details for address */
	DEBUG_CMP_SUB_STRUCT	DataCompare;	/* compare details for data */
} DEBUG_CMP_STRUCT;

typedef struct _DEBUG_CMP_LIST_NODE
{
	/* pointers to next and previous nodes */
	struct _DEBUG_CMP_LIST_NODE *pPrev;
	struct _DEBUG_CMP_LIST_NODE *pNext;

	/* comparison info */
	DEBUG_CMP_STRUCT  Comparison;
} DEBUG_CMP_LIST_NODE;

typedef struct
{
	int Enabled;						/* enable comparison checking on this list */

	/* pointer to last node in list - used for adding nodes */
	DEBUG_CMP_LIST_NODE	*pLast;
	/* dummy node - used for traversal */
	DEBUG_CMP_LIST_NODE	Dummy;
} DEBUG_CMP_LIST_HEADER;

/* initialise each list (WriteIO, ReadIO, WriteMemory, ReadMemory */
void	Debug_InitialiseComparisonLists(void);
/* free each list */
void	Debug_FreeComparisonLists(void);
/* remove a comparison node from a comparison list */
void	Debug_DeleteNodeFromComparisonList(DEBUG_CMP_LIST_HEADER *pList, DEBUG_CMP_LIST_NODE *pNode);

/* debug hooks */

/* write data to IO */
void	Debug_WriteIO(int,int);
/* write data to memory */
void	Debug_WriteMemory(int,int);
/* read data from IO */
void	Debug_ReadIO(int,int);
/* read data from memory */
void	Debug_ReadMemory(int,int);

/* set write IO Comparison */
void	Debug_SetWriteIOComparison(DEBUG_CMP_STRUCT *pComparison);
/* set read IO Comparison */
void	Debug_SetReadIOComparison(DEBUG_CMP_STRUCT *pComparison);
/* set Write Memory Comparison */
void	Debug_SetWriteMemoryComparison(DEBUG_CMP_STRUCT *pComparison);
/* set Read Memory Comparison */
void	Debug_SetReadMemoryComparison(DEBUG_CMP_STRUCT *pComparison);

void	Debug_WriteIO_Comparison_Enable(BOOL State);
void	Debug_ReadIO_Comparison_Enable(BOOL State);
void	Debug_WriteMemory_Comparison_Enable(BOOL State);
void	Debug_ReadMemory_Comparison_Enable(BOOL State);

void	Debug_SetDebuggerWindowOpenCallback(void (*pOpenDebugWindow)(void));


/*****************************************************************************/

char	*Debug_DumpFromAddress(int Addr, int NoOfBytes, int (*pReadFunction)(int));
int		Debug_ByteSearch(int AddrStart,int AddrEnd,char *pSearchString,int NumChars, unsigned char (*pReadFunction)(int));



void	Debug_SetState(int);
void	Debug_SetRunTo(int);
void	Debug_WriteMemoryToDisk(char *);
BOOL	Debug_ValidateNumberIsHex(char *HexString, int *HexNumber);
void		Debug_DissassembleInstruction(int Addr, char *OutputString);
int		Debug_GetOpcodeCount(int Addr);
int		Debug_CalcNumberOfBytesVisibleInMemDump(int WindowWidth);
char	*Debug_DissassembleLine(int Addr, int *OpcodeSize);
char	*Debug_BinaryString(unsigned char Data);
char	*Debug_FlagsAsString(void);


void	Debug_Refresh(void);

/* to be implemented */
void	Debug_Init(void);
void	Debug_Finish(void);

void	Debug_OpenWindow(void);
void	Debug_CloseWindow(void);

void	Debug_OpenReport(void);
void	Debug_CloseReport(void);

void	Debug_ShowDebug(void);
void	Debug_HideDebug(void);
void	Debug_WriteBaseMemoryToDisk(char *Filename);
int	Debugger_Execute(void);
void	Debug_SetDebuggerRefreshCallback(void (*pRefresh)(void));

#include "memdump.h"

typedef struct
{
	int				FoundAddress;
	unsigned char *pSearchString;
	unsigned char *pSearchStringMask;
	unsigned long  NumBytes;
} SEARCH_DATA;

unsigned char MemoryRead(int, int);
void MemoryWrite(int, int,int);
int	Memdump_FindData(MEMDUMP_WINDOW *pMemdumpWindow,SEARCH_DATA	*pSearchData);

#endif
