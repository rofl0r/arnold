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
/* todo:

  1. debug report
  2. Call stack
  3. highlight PC on dissassembly etc
  4. change byte-search to use function, so can use Z80_RD_MEM etc
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../cpc.h"
#include "../z80/z80.h"
#include "breakpt.h"
#include "gdebug.h"
#include "../host.h"

/*static char     MemDumpString[256];
static char DissassembleString[256];
*/
static int Debugger_State = DEBUG_RUNNING;

static int Debug_RunToAddrSpecified;
static int Debug_RunToAddr;

static void (*pDebug_OpenDebuggerWindow)(void)=NULL;
static void (*pDebug_RefreshCallback)(void) = NULL;

void    Debug_Init(void)
{
        Debug_InitialiseComparisonLists();
}

void    Debug_Finish(void)
{
        Debug_FreeComparisonLists();
}

void    Debug_SetState(int State)
{
	switch (Debugger_State)
	{
		case DEBUG_HIT_BREAKPOINT:
		{
		/*	if ((State == DEBUG_RUNNING) || (State == DEBUG_STEP_INTO))
				Z80_ExecuteInstruction();
		*/
		}
		break;

		case DEBUG_STOPPED:
		{
			if (pDebug_OpenDebuggerWindow!=NULL)
            {
                    pDebug_OpenDebuggerWindow();
            }
		}
		break;

		default:
			break;
	}

    Debugger_State = State;

    if (State == DEBUG_STOPPED)
    {
            if (pDebug_OpenDebuggerWindow!=NULL)
            {
                    pDebug_OpenDebuggerWindow();
            }
    }
}

void    Debug_SetRunTo(int Addr)
{
        Debug_SetState(DEBUG_RUNNING);

		Debug_RunToAddrSpecified = TRUE;
        Debug_RunToAddr = Addr & 0x0ffff;
}


int		Debugger_TestHalt(int PC)
{
	/* breakpoint? */
	if (Breakpoints_IsABreakpoint(PC))
	{
		/* yes, so we want to halt */
		return TRUE;
	}

	if (Debug_RunToAddrSpecified)
	{
		if (PC == Debug_RunToAddr)
		{
			Debug_RunToAddrSpecified = FALSE;

			return TRUE;
		}
	}

	return FALSE;
}



int    Debugger_Execute(void)
{
	int PC;
	int Cycles;

	if ((Debugger_State == DEBUG_STOPPED) ||
		(Debugger_State == DEBUG_HIT_BREAKPOINT))
	{
		Host_ProcessSystemEvents();
		return 0;
	}

	/* get PC */
	PC = Z80_GetReg(Z80_PC);

	/* stop? */
	if (Debugger_TestHalt(PC))
	{
		Debugger_State = DEBUG_HIT_BREAKPOINT;

		Debug_Refresh();
		return 0;
	}

	Cycles = Z80_ExecuteInstruction();

	/* step? */
	if (Debugger_State == DEBUG_STEP_INTO)
	{
		/* executed an instruction, now halt */
		Debugger_State = DEBUG_STOPPED;
	}

	return Cycles;

}



/* is character a hex digit? */
BOOL IsHexDigit(char ch)
{
        /* is it a digit? */
        if (isdigit(ch))
                return TRUE;

        /* is it a letter? */
        if (!(isalpha(ch)))
                return FALSE;

        /* it is a letter */

        /* convert to upper case */
        ch = (char)toupper(ch);

        /* check it is a valid hex digit letter */
        if ((ch<'A') || (ch>'F'))
                return FALSE;

        /* it is a valid hex digit letter */
        return TRUE;
}


BOOL    Debug_ValidateNumberIsHex(char *HexString, int *HexNumber)
{
        /* get string length */
        int HexStringLength = strlen(HexString);
        int i;
        int Number=-1;

        /* check number entered is a hex value */
        for (i=0; i<HexStringLength; i++)
        {
                char    ch;

                ch = HexString[i];

                if (!IsHexDigit(ch))
                        break;
        }

        if (i==HexStringLength)
        {
                /* is a valid hex number */

                /* convert from string to number and return */

                Number = 0;

                for (i=0; i<HexStringLength; i++)
                {
                        char    ch;

                        ch = HexString[i];

                        /* number = number * 16 */
                        Number=Number<<4;

                        if (isalpha(ch))
                        {
                                /* hex digit is letter */

                                /* convert to upper case */
                                ch = (char)toupper(ch);

                                Number = Number + ch - 'A' + 10;
                        }

                        if (isdigit(ch))
                        {
                                /* hex digit is number */
                                Number = Number + ch - '0';
                        }
                }

                *HexNumber = Number;

                return TRUE;
        }

        return FALSE;
}

static char     BinaryString[9];

/* dump byte as binary string */
char    *Debug_BinaryString(unsigned char Byte)
{
        int             i;

        for (i=0; i<8; i++)
        {
                if (Byte & 0x080)
                {
                        BinaryString[i] = '1';
                }
                else
                {
                        BinaryString[i] = '0';
                }

                Byte=(char)(Byte<<1);
        }

        BinaryString[9] = '\0';

        return BinaryString;
}

static  char  FlagsText[9];

char    *Debug_FlagsAsString(void)
{
		int Flags = Z80_GetReg(Z80_F);

        sprintf(FlagsText,"--------");

        if (Flags & Z80_SIGN_FLAG)
        {
                FlagsText[0] = 'N';
        }

        if (Flags & Z80_ZERO_FLAG)
        {
                FlagsText[1] = 'Z';
        }

        if (Flags  & Z80_UNUSED_FLAG2)
        {
                FlagsText[2] = '1';
        }

        if (Flags  & Z80_HALFCARRY_FLAG)
        {
                FlagsText[3] = 'H';
        }

        if (Flags  & Z80_UNUSED_FLAG1)
        {
                FlagsText[4] = '2';
        }

        if (Flags & Z80_PARITY_FLAG)
        {
                FlagsText[5] = 'P';
        }

        if (Flags & Z80_SUBTRACT_FLAG)
        {
                FlagsText[6] = 'S';
        }

        if (Flags  & Z80_CARRY_FLAG)
        {
                FlagsText[7] = 'C';
        }

        FlagsText[8] = '\0';

        return FlagsText;
}

/****************************************************************************/
/* "Debug Comparison" code                                                  */
/* The comparisons are held in a linked list. These are scanned, and if any */
/* conditions match, then the debugger is invoked, and execution halts.     */

/* create a new node */
DEBUG_CMP_LIST_NODE     *Debug_CreateNewComparisonListNode(DEBUG_CMP_STRUCT *pComparison)
{
        DEBUG_CMP_LIST_NODE *pNode;

        /* must have a valid comparison pointer */
        if (pComparison==NULL)
                return NULL;

        /* allocate memory for node */
        pNode = (DEBUG_CMP_LIST_NODE *)malloc(sizeof(DEBUG_CMP_LIST_NODE));

        if (pNode!=NULL)
        {
                /* initialise node */
                pNode->pPrev = NULL;
                pNode->pNext = NULL;

                /* copy comparison details */
                memcpy(&pNode->Comparison, pComparison, sizeof(DEBUG_CMP_STRUCT));
        }

        return pNode;
}

/* initialise the list */
void    Debug_InitialiseComparisonList(DEBUG_CMP_LIST_HEADER *pList)
{
        /* initialise a comparison list */
        if (pList!=NULL)
        {
                pList->pLast = &pList->Dummy;
                pList->Dummy.pNext = &pList->Dummy;
                pList->Dummy.pPrev = &pList->Dummy;
                pList->Enabled = FALSE;
        }
}

void    Debug_DeleteComparisonList(DEBUG_CMP_LIST_HEADER *pList)
{
        if (pList!=NULL)
        {
                DEBUG_CMP_LIST_NODE *pNode;

                pNode = pList->Dummy.pNext;

                /* traverse as long as node isn't the dummy node */
                while (pNode!=&pList->Dummy)
                {
                        DEBUG_CMP_LIST_NODE *pNext;

                        /* get pointer to next node before memory is released */
                        pNext = pNode->pNext;

                        /* detach it and free memory allocated for it */
                        Debug_DeleteNodeFromComparisonList(pList, pNode);

                        /* next node */
                        pNode = pNext;
                }
        }
}


/* add a node to the list */
void    Debug_AddNodeToComparisonList(DEBUG_CMP_LIST_HEADER *pList, DEBUG_CMP_LIST_NODE *pNode)
{
        if ((pList!=NULL) && (pNode!=NULL))
        {
                /* node will become new end of list */

                if (pList->pLast == &pList->Dummy)
                {
                        pList->Enabled = TRUE;
                }

                /* make previous pointer of new node point to old end of list */
                pNode->pPrev = pList->pLast;

                pNode->pNext = pList->pLast->pNext;

                /* make end of list next pointer point to new node */
                pList->pLast->pNext = pNode;

                /* setup last pointer to point to the new end of list */
                pList->pLast = pNode;

        }
}

/* remove a node from list */
void    Debug_RemoveNodeFromComparisonList(DEBUG_CMP_LIST_HEADER *pList, DEBUG_CMP_LIST_NODE *pNode)
{
        if ((pList!=NULL) && (pNode!=NULL))
        {
                /* node that is before this node, now points to node after this node */
                pNode->pPrev->pNext = pNode->pNext;
                /* node that is after this node, now points to node before this node */
                pNode->pNext->pPrev = pNode->pPrev;

                /* is node last in the list? */
                if (pList->pLast == pNode)
                {
                        /* set new list end */
                        pList->pLast = pNode->pPrev;
                }

                if (pList->pLast == &pList->Dummy)
                {
                        /* disable list */
                        pList->Enabled = FALSE;
                }

        }
}

static DEBUG_CMP_LIST_HEADER WriteIOComparisonList;
static DEBUG_CMP_LIST_HEADER ReadIOComparisonList;
static DEBUG_CMP_LIST_HEADER WriteMemoryComparisonList;
static DEBUG_CMP_LIST_HEADER ReadMemoryComparisonList;

void    Debug_InitialiseComparisonLists(void)
{
        Debug_InitialiseComparisonList(&WriteIOComparisonList);
        Debug_InitialiseComparisonList(&ReadIOComparisonList);
        Debug_InitialiseComparisonList(&WriteMemoryComparisonList);
        Debug_InitialiseComparisonList(&ReadMemoryComparisonList);
}

void    Debug_FreeComparisonLists(void)
{
        Debug_DeleteComparisonList(&WriteIOComparisonList);
        Debug_DeleteComparisonList(&ReadIOComparisonList);
        Debug_DeleteComparisonList(&WriteMemoryComparisonList);
        Debug_DeleteComparisonList(&ReadMemoryComparisonList);
}

/* do a sub-comparison.

  Taken what the comparison type is, we use either Value1 or Value1 and Value2 and Compare
  with CompareTo. Return TRUE if comparison is ok, otherwise FALSE */

BOOL    Debug_DoSubComparison(DEBUG_CMP_SUB_STRUCT *pSubCompare, int CompareTo)
{
        BOOL    CompareResult = FALSE;

        /* do comparison */
        switch (pSubCompare->Comparison)
        {
                case DEBUG_CMP_EQUAL:
                {
                        /* equals. Only uses value 1 */
                        if (pSubCompare->Value1 == CompareTo)
                        {
                                CompareResult = TRUE;
                        }
                }
                break;

                case DEBUG_CMP_ANY:
                {
                        CompareResult = TRUE;
                }
                break;

                case DEBUG_CMP_NOT_EQUAL:
                {
                        /* equals. Only uses value 1 */
                        if (pSubCompare->Value1 != CompareTo)
                        {
                                CompareResult = TRUE;
                        }
                }
                break;

                case DEBUG_CMP_RANGE:
                {
                        if ((pSubCompare->Value1<=CompareTo) && (pSubCompare->Value2>=CompareTo))
                        {
                                CompareResult = TRUE;
                        }
                }
                break;

                case DEBUG_CMP_AND_MASK:
                {
                        if ((CompareTo & pSubCompare->Value1)==pSubCompare->Value2)
                        {
                                CompareResult = TRUE;
                        }
                }
                break;


                default:
                        break;
        }

        return CompareResult;
}


/* compare Addr with valu, data with value, or addr and data with own values */
/* returns TRUE if comparison is made, else FALSE */
BOOL    Debug_DoComparison(DEBUG_CMP_STRUCT *pCompareStruct, int Addr, int Data)
{
        BOOL    CompareResult = FALSE;

        switch (pCompareStruct->CompareWhat)
        {
                case DEBUG_CMP_ADDR:
                {
                        /* comparing against addr only */

                        CompareResult = Debug_DoSubComparison(&pCompareStruct->AddressCompare, Addr);
                }
                break;

                case DEBUG_CMP_DATA:
                {
                        /* comparing against data only */

                        CompareResult = Debug_DoSubComparison(&pCompareStruct->DataCompare, Data);
                }
                break;

                case DEBUG_CMP_ADDR_AND_DATA:
                {
                        /* comparing against address and data */

                        CompareResult = (Debug_DoSubComparison(&pCompareStruct->AddressCompare, Addr) &&
                                                        Debug_DoSubComparison(&pCompareStruct->DataCompare, Data));

                }
                break;
        }

        return CompareResult;
}


/* check comparisons in list if enabled */
void    Debug_CheckComparisonsInList(DEBUG_CMP_LIST_HEADER *pList, int Addr, int Data)
{
  if (pList->Enabled)
  {
    DEBUG_CMP_LIST_NODE *pNode;

        /* get first node */
        pNode = pList->Dummy.pNext;

        /* finish traversing list when node points to dummy */
        while (pNode!=&pList->Dummy)
        {
                /* is comparison enabled? */
                if (pNode->Comparison.Enabled)
                {
                        /* do the comparison */
                        if (Debug_DoComparison(&pNode->Comparison, Addr, Data))
                        {
/*                                Debug_SetState(DEBUG_HALT); */
                        }
                }

                /* next node */
                pNode = pNode->pNext;
        }
  }
}

/* add a new comparison to the list specified */
void    Debug_AddComparisonToList(DEBUG_CMP_LIST_HEADER *pList, DEBUG_CMP_STRUCT *pComparison)
{
        DEBUG_CMP_LIST_NODE *pNode;

        /* create new node */
        pNode = Debug_CreateNewComparisonListNode(pComparison);

        if (pNode!=NULL)
        {
                /* add to list */
                Debug_AddNodeToComparisonList(pList, pNode);
        }
}

/* delete comparison from the list specified */
void    Debug_DeleteNodeFromComparisonList(DEBUG_CMP_LIST_HEADER *pList, DEBUG_CMP_LIST_NODE *pNode)
{
        if (pNode!=NULL)
        {
                /* detach it from list */
                Debug_RemoveNodeFromComparisonList(pList, pNode);

                /* free node memory */
                free(pNode);
        }
}

/* set a Debug Comparison for Write IO */
void    Debug_SetWriteIOComparison(DEBUG_CMP_STRUCT *pComparison)
{
        pComparison->Enabled = TRUE;

        Debug_AddComparisonToList(&WriteIOComparisonList, pComparison);
}

/* set a Debug Comparison for Read IO */
void    Debug_SetReadIOComparison(DEBUG_CMP_STRUCT *pComparison)
{
        pComparison->Enabled = TRUE;

        Debug_AddComparisonToList(&ReadIOComparisonList, pComparison);
}

/* set a Debug Comparison for Write Memory */
void    Debug_SetWriteMemoryComparison(DEBUG_CMP_STRUCT *pComparison)
{
        pComparison->Enabled = TRUE;

        Debug_AddComparisonToList(&WriteMemoryComparisonList, pComparison);
}

/* set a Debug Comparison for Read Memory */
void    Debug_SetReadMemoryComparison(DEBUG_CMP_STRUCT *pComparison)
{
        pComparison->Enabled = TRUE;

        Debug_AddComparisonToList(&ReadMemoryComparisonList, pComparison);
}

void    Debug_ReadMemory_Comparison_Enable(BOOL State)
{
        ReadMemoryComparisonList.Enabled = State;
}

void    Debug_WriteMemory_Comparison_Enable(BOOL State)
{
        WriteMemoryComparisonList.Enabled = State;
}

void    Debug_ReadIO_Comparison_Enable(BOOL State)
{
        ReadIOComparisonList.Enabled = State;
}

void    Debug_WriteIO_Comparison_Enable(BOOL State)
{
        WriteIOComparisonList.Enabled = State;
}




/* I/O ACCESS */
/* Debugger version of I/O write */
/* Addr = port address, Data = Data value to write*/
void    Debug_WriteIO(int Addr, int Data)
{
        Debug_CheckComparisonsInList(&WriteIOComparisonList, Addr, Data);
}

/* Debugger version of I/O read */
/* Addr = port address, Data = data value read */
void    Debug_ReadIO(int Addr, int Data)
{
        Debug_CheckComparisonsInList(&ReadIOComparisonList, Addr, Data);
}

/* MEMORY ACCESS */

/* Debugger Version of write memory */
/* Addr = memory address, Data = Data value to write*/
void    Debug_WriteMemory(int Addr, int Data)
{
        Debug_CheckComparisonsInList(&WriteMemoryComparisonList, Addr, Data);
}

/* Debugger Version of read memory */
/* Addr = memory address, Data = Data value read */
void    Debug_ReadMemory(int Addr, int Data)
{
        Debug_CheckComparisonsInList(&ReadMemoryComparisonList, Addr, Data);
}


/*
char    *Debug_GetCompareString(int CompareId)
{
        switch (CompareID)
        {
                case DEBUG_CMP_ANY:
                {
                        return "any value";
                }
                break;
                case DEBUG_CMP_RANGE:
                {
                        return "in the range value1 to value2";
                }
                break;
                case DEBUG_CMP_EQUAL:
                {
                        return "equal to value1";
                }
                break;
                case DEBUG_CMP_NOT_EQUAL:
                {
                        return "not equal to value1";
                }
                break;
        }
}
*/



static char     ByteString[16];

BOOL    Debug_StringContainsValidHexBytes(char *pString)
{
        int DigitCount = 0;
        int i;

        for (i=0; i<(int)strlen(pString); i++)
        {
                char ch;

                /* get char from string */
                ch = pString[i];

                if (IsHexDigit(ch))
                {
                        /* is a hex digit */

                        DigitCount++;

                        if (DigitCount>2)
                                return FALSE;

                }
                else
                {
                        /* not hex digit */
                        if (ch==',')
                        {
                                /* seperation char */

                                if (DigitCount==0)
                                        return FALSE;

                                /* reset digit count */
                                DigitCount=0;
                        }
                        else
                        {
                                /* not hex digit or seperation char */
                                return FALSE;
                        }
                }
        }

        return TRUE;
}


/* convert a string in form xx,xx,xx,xx into a list of bytes */
void    Debug_ConvertStringIntoByteString(char *pString)
{
        int i;

        int CurrentHexByte;
        char *pByteList;
        int ByteCount;

        ByteCount = 0;
        pByteList = &ByteString[0];
        CurrentHexByte = 0;

        /* go through whole string including null terminator */
        for (i=0; i<(int)(strlen(pString)+1); i++)
        {
                char ch;

                /* get hex digit or seperation char */
                ch = pString[i];

                if ((ch==',') || (ch=='\0'))
                {
                        /* seperation char */

                        /* store current byte */
                        pByteList[ByteCount] = (char)CurrentHexByte;
                        ByteCount++;

                        /* clear for next byte */
                        CurrentHexByte = 0;
                }
                else
                {
                        /* hex digit */
                        if (IsHexDigit(ch))
                        {
                                CurrentHexByte = CurrentHexByte<<4;

                                if (isalpha(ch))
                                {
                                        CurrentHexByte = CurrentHexByte + ch - 'A' + 10;
                                }
                                else
                                {
                                        CurrentHexByte = CurrentHexByte + ch - '0';
                                }
                        }
                }
        }
}



void    Debug_SetDebuggerWindowOpenCallback(void (*pOpenDebugWindow)(void))
{
        pDebug_OpenDebuggerWindow = pOpenDebugWindow;
}

void    Debug_SetDebuggerRefreshCallback(void (*pRefresh)(void))
{
        pDebug_RefreshCallback = pRefresh;
}



void    Debug_Refresh(void)
{
        if (pDebug_RefreshCallback!=NULL)
        {
                pDebug_RefreshCallback();
        }
}



unsigned char		MemoryRead(int Address, int NumberOfBytes)
{
	unsigned char ReadData;
	int i;
	int Shift;

	ReadData = 0;
	Shift = 0;
	for (i=0; i<NumberOfBytes; i++)
	{
		ReadData|=(Z80_RD_MEM(Address+i)<<Shift);
		Shift+=8;
	}

	return ReadData;
}

void	MemoryWrite(int Address, int Data, int NumberOfBytes)
{
	int i;
	int WriteData;

	WriteData = Data;

	for (i=0; i<NumberOfBytes; i++)
	{
		Z80_WR_MEM(Address+i, WriteData);
		WriteData>>=8;
	}
}

void             ByteSearch(SEARCH_DATA *pSearchData, int Addr,int BlockSize,unsigned char (*pReadFunction)(int,int))
{
        int     Index;
      	int BytesSearchedCount;

		BytesSearchedCount= 0;

        /* initialise count */
        Index = 0;

        do
        {
                unsigned char DataByte;
				unsigned char ComparisonByte;

                /* get data byte from memory */
                DataByte = pReadFunction(Addr,1);

				/* calc data byte */
				DataByte &= pSearchData->pSearchStringMask[Index];
				/* calc comparison byte */
				ComparisonByte = pSearchData->pSearchString[Index] & pSearchData->pSearchStringMask[Index];

                Addr++;

                /* Is this byte the same as the current byte in the search string? */
                if (DataByte==ComparisonByte)
                {
                        /* Yes, increment count */
                        Index++;

                        /* if all bytes match, we have found the string */
                        if (Index==pSearchData->NumBytes)
						{
							pSearchData->FoundAddress = Addr-Index;
							return;
						}
                }
                else
                {
                        /* reset count */
                        Index=0;
                }

				BytesSearchedCount++;

        } while (BytesSearchedCount!=BlockSize);

	pSearchData->FoundAddress = -1;
}



int	Memdump_FindData(MEMDUMP_WINDOW *pMemdumpWindow,SEARCH_DATA	*pSearchData)
{
	int SearchAddress;

	/* do not do a search if no data specified */
	if (pSearchData->NumBytes==0)
		return -1;

	/* if not yet found, set initial search address */
	if (pSearchData->FoundAddress==-1)
	{
		/* not found or new search */
		SearchAddress = pMemdumpWindow->BaseAddress;
	}
	else
	{
		/* continue search */
		SearchAddress = pSearchData->FoundAddress + pSearchData->NumBytes;
	}

	/* search for data */
	ByteSearch(pSearchData, SearchAddress, 64*1024, MemoryRead);

	if (pSearchData->FoundAddress!=-1)
	{
		/* found, so go to address */
		Memdump_GotoAddress(pMemdumpWindow, pSearchData->FoundAddress);
	}

	return pSearchData->FoundAddress;
}
