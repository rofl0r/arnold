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
#include <string.h>
#include "cpcglob.h"
#include "cheatdb.h"
#include <ctype.h>

#include "z80/z80.h"
#if 0
/* find entry by name */
CHEAT_DATABASE_ENTRY	*CheatDatabase__GetEntryByName(CHEAT_DATABASE *pCheatDatabase,char *Name)
{
	CHEAT_DATABASE_ENTRY *pCurrentEntry;

	pCurrentEntry = pCheatDatabase->pFirst;

	while (pCurrentEntry!=NULL)
	{
		if (pCurrentEntry->Name!=NULL)
		{
			if (stricmp(pCurrentEntry->Name, Name)==0)
			{
				return pCurrentEntry;
			}
		}

		pCurrentEntry = pCurrentEntry->pNext;
	}

	return pCurrentEntry;
}
#endif
static CHEAT_DATABASE	*CheatDatabase_New(void)
{
	CHEAT_DATABASE *pDatabase;

	pDatabase = (CHEAT_DATABASE *)malloc(sizeof(CHEAT_DATABASE));

	if (pDatabase!=NULL)
	{
		memset(pDatabase, 0, sizeof(CHEAT_DATABASE));
	}
	
	return pDatabase;
}

void	CheatDatabase_Poke(CHEAT_DATABASE_ENTRY *pEntry)
{
	unsigned long i;

	for (i=0; i<pEntry->NoOfParts; i++)
	{
		int Addr, Byte;
		
		Addr = pEntry->Parts[i].Addr;
		Byte = pEntry->Parts[i].NewValue;

		Z80_WR_MEM((Z80_WORD)Addr, (Z80_BYTE)Byte);
	}
}


/* allocate memory for a new entry */
static CHEAT_DATABASE_ENTRY *CheatDatabaseEntry_New(void)
{
	CHEAT_DATABASE_ENTRY *pEntry;

	pEntry = (CHEAT_DATABASE_ENTRY *)malloc(sizeof(CHEAT_DATABASE_ENTRY));

	if (pEntry!=NULL)
	{
		memset(pEntry, 0, sizeof(CHEAT_DATABASE_ENTRY));

	}

	return pEntry;
}

/* insert a new entry before the specified entry */
static void	CheatDatabase_InsertBefore(CHEAT_DATABASE *pDatabase,CHEAT_DATABASE_ENTRY *pAfter, CHEAT_DATABASE_ENTRY *pEntry)
{
	/* fix new entry */
	pEntry->pPrev = pAfter->pPrev;
	pEntry->pNext = pAfter;

	/* fix item pointing to pAfter */
	if (pAfter->pPrev!=NULL)
	{
		pAfter->pPrev->pNext = pEntry;
	}
	else
	{
		/* inserted before start of list */
		pDatabase->pFirst = pEntry;
	}

	/* fix insert point */
	pAfter->pPrev = pEntry;
}

/* insert a new entry after the specified entry */
static void	CheatDatabase_InsertAfter(/*CHEAT_DATABASE *pDatabase,*/CHEAT_DATABASE_ENTRY *pBefore, CHEAT_DATABASE_ENTRY *pEntry)
{
	/* fix new entry */
	pEntry->pPrev = pBefore;
	pEntry->pNext = pBefore->pNext;

	/* fix item pointing to pAfter */
	if (pBefore->pNext!=NULL)
	{
		pBefore->pNext->pPrev= pEntry;
	}

	/* fix insert point */
	pBefore->pNext = pEntry;
}


/* insert a new entry after the specified entry */
static void	CheatDatabase_InsertAtEnd(CHEAT_DATABASE *pDatabase,CHEAT_DATABASE_ENTRY *pEntry)
{
	CHEAT_DATABASE_ENTRY *pCurrentEntry;

	if (pDatabase->pFirst==NULL)
	{
		pDatabase->pFirst = pEntry;
	}
	else
	{
		pCurrentEntry = pDatabase->pFirst;

		while (pCurrentEntry->pNext!=NULL)
			pCurrentEntry = pCurrentEntry->pNext;

		CheatDatabase_InsertAfter(/*pDatabase,*/pCurrentEntry, pEntry);
	}
}
#if 0
/* insert entry in the correct position for alphabetical order */
void	CheatDatabase_InsertInAlphabeticalOrder(CHEAT_DATABASE *pDatabase, CHEAT_DATABASE_ENTRY *pEntry)
{
	if (pDatabase->pFirst==NULL)
	{
		pDatabase->pFirst = pEntry;
	}
	else
	{
		if (pEntry->Name==NULL)
		{
			CheatDatabase_InsertBefore(pDatabase, pDatabase->pFirst, pEntry);
		}
		else
		{
			CHEAT_DATABASE_ENTRY *pCurrentEntry;

			while (pCurrentEntry!=NULL)
			{
				if (stricmp(pCurrentEntry->Name, pEntry->Name)<0)
				{
					CheatDatabase_InsertBefore(pDatabase,pCurrentEntry, pEntry);
					return;
				}
		
				pCurrentEntry = pCurrentEntry->pNext;
			}

			CheatDatabase_InsertAtEnd(pDatabase, pEntry);
		
		}

	}
}
#endif

/* delete a cheat database entry */
static void	CheatDatabaseEntry_Delete(CHEAT_DATABASE_ENTRY *pEntry)
{
	if (pEntry->Name!=NULL)
	{
		free(pEntry->Name);
		pEntry->Name = NULL;
	}

	if (pEntry->Description!=NULL)
	{
		free(pEntry->Description);
		pEntry->Description = NULL;
	}

	if (pEntry!=NULL)
		free(pEntry);
}

/* delete whole cheat database */
void	CheatDatabase_Delete(CHEAT_DATABASE *pDatabase)
{
	CHEAT_DATABASE_ENTRY *pCurrentEntry;

	pCurrentEntry = pDatabase->pFirst;

	while (pCurrentEntry!=NULL)
	{
		CHEAT_DATABASE_ENTRY *pNextEntry;
		
		pNextEntry = pCurrentEntry->pNext;

		CheatDatabaseEntry_Delete(pCurrentEntry);

		pCurrentEntry = pNextEntry;
	}
}

static CHEAT_DATABASE_POKE_ENTRY	*CheatDatabaseEntry_AddPart(CHEAT_DATABASE_ENTRY *pEntry)
{
	CHEAT_DATABASE_POKE_ENTRY *pPart;

	if (pEntry->NoOfParts>=CHEAT_DATABASE_ENTRY_MAX_PARTS)
		return NULL;


	pPart = &pEntry->Parts[pEntry->NoOfParts];

	pEntry->NoOfParts++;

	return pPart;
}

static void	CheatDatabaseEntry_SetName(CHEAT_DATABASE_ENTRY *pEntry, char *Name)
{
	if (pEntry->Name!=NULL)
	{
		free(pEntry->Name);
	}

	pEntry->Name = (unsigned char *)malloc(strlen(Name)+1);

	if (pEntry->Name!=NULL)
	{
		strcpy((char *)pEntry->Name, Name);
	}
}


static void	CheatDatabaseEntry_SetDescription(CHEAT_DATABASE_ENTRY *pEntry, char *Description)
{
	if (pEntry->Description!=NULL)
	{
		free(pEntry->Description);
	}

	pEntry->Description = (unsigned char *)malloc(strlen(Description)+1);

	if (pEntry->Description!=NULL)
	{
		strcpy((char *)pEntry->Description, Description);
	}
}

static void	CheatDatabase_Warning(char *pWarningText)
{
	printf(pWarningText);
}

/* define this for checking on database */
#define CPCEMU_FRIENDLY

/* max buffer space for reading in quoted text */
#define CHEAT_DATABASE_MAX_QUOTED_STRING_LENGTH 128
/* length of quoted text */
static unsigned long CheatDatabase_QuotedText_Length;
/* space to store quoted text */
static unsigned char CheatDatabase_QuotedText[CHEAT_DATABASE_MAX_QUOTED_STRING_LENGTH];

static unsigned char *pDataPtr, *pDataPtrEnd;

static void	CheatDatabase_GetDataInQuotes(void)
{
	unsigned char ch;

	CheatDatabase_QuotedText_Length = 0;
	
	do
	{
		ch = pDataPtr[0];
		pDataPtr++;

		if (ch!='"')
		{
			/* store char */
			CheatDatabase_QuotedText[CheatDatabase_QuotedText_Length] = ch;
			CheatDatabase_QuotedText_Length++;
		}

		if (pDataPtr==pDataPtrEnd)
			break;

	}
	while ((ch!='"') && (CheatDatabase_QuotedText_Length<CHEAT_DATABASE_MAX_QUOTED_STRING_LENGTH));

	CheatDatabase_QuotedText[CheatDatabase_QuotedText_Length]='\0';


	if (CheatDatabase_QuotedText_Length==CHEAT_DATABASE_MAX_QUOTED_STRING_LENGTH)
	{
		/* exceeded buffer space */
		CheatDatabase_Warning("Quoted String has exceeded buffer space. String has been truncated.\r\n");

		/* go to quote */
		do
		{
			ch = pDataPtr[0];
			pDataPtr++;

			if (pDataPtr==pDataPtrEnd)
				break;
		}
		while (ch!='"');
	}


}

/* is a hex digit if it is a digit (0..9), or a letter in range A..F */
#define is_hex_digit(ch) \
	(isdigit(ch) | (isalpha(ch) && ((toupper(ch)>='A') && (toupper(ch)<='F'))))

/* given a char will return the value of the char */
static unsigned long CheatDatabase_GetHexDigit(char ch)
{
	if (isdigit(ch))
	{
		return ch-'0';
	}

	if (isalpha(ch))
	{
		return toupper(ch)-'A' + 10;
	}

	return 0;
}



static int CheatDatabase_Parse_PartNumber(void)
{
	unsigned char PartNumberChar;
	unsigned long PartNumber;

#ifdef CPCEMU_FRIENDLY
	if (CheatDatabase_QuotedText_Length!=1)
	{
		/* error with part number - quoted text is too long */
		CheatDatabase_Warning("Part Number string is too long. Should be 1 char Maximum!\r\n");
	}
#endif

	PartNumberChar = CheatDatabase_QuotedText[0];

#ifdef CPCEMU_FRIENDLY
	if (!(is_hex_digit(PartNumberChar)))
	{
		/* part number char is invalid */
		CheatDatabase_Warning("Part Number is not a valid hex digit!\r\n");
	}
#endif

	/* convert from text to value */

	PartNumber = CheatDatabase_GetHexDigit(PartNumberChar);

	return PartNumber;

}

static void	CheatDatabase_Parse_Name(void)
{
#ifdef CPCEMU_FRIENDLY
	if (CheatDatabase_QuotedText_Length>20)
	{
		/* name is too long */
		CheatDatabase_Warning("Name is too long. Should be 20 chars Maximum!\r\n");
	}
#endif

}

static void	CheatDatabase_Parse_Description(void)
{
#ifdef CPCEMU_FRIENDLY
	if (CheatDatabase_QuotedText_Length>20)
	{
		/* description is too long */
	
		CheatDatabase_Warning("Description is too long. Should be 20 chars Maximum!\r\n");
	}
#endif
}

static int CheatDatabase_Parse_Type(void)
{
	unsigned char Type;

#ifdef CPCEMU_FRIENDLY
	if (CheatDatabase_QuotedText_Length!=1)
	{
		/* type is not 1 char */
		CheatDatabase_Warning("Type string is too long. Should be 1 char maximum!\r\n");
	}
#endif

	Type = (unsigned char)toupper(CheatDatabase_QuotedText[0]);

#ifdef CPCEMU_FRIENDLY
	
	if ((Type!='T') && (Type!='D') &&
		(Type!='A') && (Type!='C'))
	{
		CheatDatabase_Warning("Type is not valid. It should be 'a', 'c', 't' or 'd'!\r\n");
	}
#endif
	if ((Type!='T') && (Type!='D') && (Type!='A') && (Type!='C'))
	{
		/* if it's not a valid char, say it is valid for all */
		Type = 'A';
	}

	return Type;
}

static unsigned long CheatDatabase_Parse_GetHexNumber(int MaxDigits)
{
	int DigitsGot;
	unsigned long i;
	unsigned long HexNumber = 0;

	/* 	construct address, and skip any non-hex digit chars */

	DigitsGot = 0;

	for (i=0; i<CheatDatabase_QuotedText_Length; i++)
	{
		if (is_hex_digit(CheatDatabase_QuotedText[i]))
		{
			HexNumber = HexNumber<<4;

			HexNumber = HexNumber | CheatDatabase_GetHexDigit(CheatDatabase_QuotedText[i]);

			DigitsGot++;
		}

		if (DigitsGot>=MaxDigits)
			break;
	}

	return HexNumber;
}


static int	CheatDatabase_Parse_Addr(void)
{
	unsigned long Addr;
	unsigned long i;

#ifdef CPCEMU_FRIENDLY
	if (CheatDatabase_QuotedText_Length!=4)
	{
		CheatDatabase_Warning("Addr string is not 4 chars!\r\n");
	}
#endif

#ifdef CPCEMU_FRIENDLY
	for (i=0; i<CheatDatabase_QuotedText_Length; i++)
	{
		if (!is_hex_digit(CheatDatabase_QuotedText[i]))
		{
			CheatDatabase_Warning("Addr string contains non hex-digit chars!\r\n");
		}
	}
#endif


	/* addr contains value */

	Addr = CheatDatabase_Parse_GetHexNumber(4);

	return Addr;
}

static int	CheatDatabase_Parse_ByteToPoke(void)
{
	unsigned long ByteToPoke;
	unsigned long i;
	
#ifdef CPCEMU_FRIENDLY
	if (CheatDatabase_QuotedText_Length!=2)
	{
		CheatDatabase_Warning("Byte to poke string is not 2 chars!\r\n");
	}
#endif

#ifdef CPCEMU_FRIENDLY
	for (i=0; i<CheatDatabase_QuotedText_Length; i++)
	{
		if (!is_hex_digit(CheatDatabase_QuotedText[i]))
		{
			CheatDatabase_Warning("Byte to poke string contains non hex-digit chars!\r\n");
		}
	}
#endif

	/* addr contains value */

	ByteToPoke = CheatDatabase_Parse_GetHexNumber(2);

	return ByteToPoke;
}

static int	CheatDatabase_Parse_OldByte(void)
{
	unsigned long OldByte;
	unsigned long i;
	
#ifdef CPCEMU_FRIENDLY
	if (CheatDatabase_QuotedText_Length!=2)
	{
		CheatDatabase_Warning("Old byte string is not 2 chars!\r\n");
	}
#endif

#ifdef CPCEMU_FRIENDLY
	for (i=0; i<CheatDatabase_QuotedText_Length; i++)
	{
		if (!is_hex_digit(CheatDatabase_QuotedText[i]))
		{
			CheatDatabase_Warning("Old byte string contains non hex-digit chars!\r\n");
		}
	}
#endif

	/* addr contains value */

	OldByte = CheatDatabase_Parse_GetHexNumber(2);

	return OldByte;
}

	

/* I chose to do it this way so it is portable */

CHEAT_DATABASE *CheatDatabase_Read(char *pFilename)
{
	CHEAT_DATABASE *pCheatDatabase;
	CHEAT_DATABASE_ENTRY *pCurrentEntry;

	unsigned char *pDatabase;
	unsigned long Database_FileLength;

	pCurrentEntry = NULL;
	if (Host_LoadFile(pFilename, &pDatabase, &Database_FileLength))
	{
		/* loaded database, parse it */
		unsigned char ch;

		pDataPtr = pDatabase;
		pDataPtrEnd = pDatabase + Database_FileLength;
		

		pCheatDatabase = CheatDatabase_New();

		ch = pDataPtr[0];
		
		do
		{
			switch (ch)
			{
				case ';':
				{
					/* get chars until end of line */
					do
					{
						ch = pDataPtr[0];
						pDataPtr++;
					
						if (pDataPtr==pDataPtrEnd)
							break;
					}
					while ((ch!=10) && (ch!=13));

					if (pDataPtr==pDataPtrEnd)
						break;
					
					/* skip CR, LF chars */
					do
					{
						ch = pDataPtr[0];
						pDataPtr++;	
					
						if (pDataPtr==pDataPtrEnd)
							break;
					} 
					while ((ch==10) || (ch==13));
				
					if (pDataPtr==pDataPtrEnd)
						break;
				}
				break;

				case 10:
				case 13:
				{
					/* skip CR, LF chars */
					do
					{
						ch = pDataPtr[0];
						pDataPtr++;	
					
						if (pDataPtr==pDataPtrEnd)
							break;
					} 
					while ((ch==10) || (ch==13));
				
					if (pDataPtr==pDataPtrEnd)
						break;
				}
				break;

				case '"':
				{
					int PartNumber;
					int Type;
					int Addr;
					int ByteToPoke;
					int OldByte;
					CHEAT_DATABASE_POKE_ENTRY *pCurrentPart = NULL;
					/* get part number */
					CheatDatabase_GetDataInQuotes();

					PartNumber = CheatDatabase_Parse_PartNumber();
					
					if (PartNumber==0)
					{
						pCurrentEntry = CheatDatabaseEntry_New();
					
						CheatDatabase_InsertAtEnd(pCheatDatabase, pCurrentEntry);
					}

					pCurrentPart = CheatDatabaseEntry_AddPart(pCurrentEntry);


					/* skip comma */
					ch = pDataPtr[0];
					pDataPtr++;

					/* skip quote */
					ch = pDataPtr[0];
					pDataPtr++;

					/* name */
					CheatDatabase_GetDataInQuotes();

					CheatDatabase_Parse_Name();

					if (PartNumber == 0)
					{
						CheatDatabaseEntry_SetName(pCurrentEntry,(char *)&CheatDatabase_QuotedText[0]);
					}


					/* skip comma */
					ch = pDataPtr[0];
					pDataPtr++;
					
					/* skip quote */
					ch = pDataPtr[0];
					pDataPtr++;

					/* description */
					CheatDatabase_GetDataInQuotes();

					CheatDatabase_Parse_Description();

					if (PartNumber == 0)
					{
						CheatDatabaseEntry_SetDescription(pCurrentEntry,(char *)&CheatDatabase_QuotedText[0]);
					}


					/* skip comma */
					ch = pDataPtr[0];
					pDataPtr++;

					/* skip quote */
					ch = pDataPtr[0];
					pDataPtr++;

					/* type */
					CheatDatabase_GetDataInQuotes();

					Type = CheatDatabase_Parse_Type();

					if (PartNumber == 0)
					{
						pCurrentEntry->Type = (unsigned char)Type;
					}

					/* skip comma */
					ch = pDataPtr[0];
					pDataPtr++;
					
					/* skip quote */
					ch = pDataPtr[0];
					pDataPtr++;

					/* address */
					CheatDatabase_GetDataInQuotes();

					Addr = CheatDatabase_Parse_Addr();

					pCurrentPart->Addr = Addr;

					/* skip comma */
					ch = pDataPtr[0];
					pDataPtr++;

					/* skip quote */
					ch = pDataPtr[0];
					pDataPtr++;


					/* byte to poke */
					CheatDatabase_GetDataInQuotes();

					ByteToPoke = CheatDatabase_Parse_ByteToPoke();
					pCurrentPart->NewValue = (unsigned char)ByteToPoke;

					/* skip comma */
					ch = pDataPtr[0];
					pDataPtr++;

					
					/* skip quote */
					ch = pDataPtr[0];
					pDataPtr++;

					/* old byte */
					CheatDatabase_GetDataInQuotes();

					OldByte = CheatDatabase_Parse_OldByte();

					pCurrentPart->OldValue = (unsigned char)OldByte;

					/* get chars until end of line */
					do
					{
						ch = pDataPtr[0];
						pDataPtr++;
					
						if (pDataPtr==pDataPtrEnd)
							break;
					}
					while ((ch!=10) && (ch!=13));

					if (pDataPtr==pDataPtrEnd)
						break;
					
					/* skip CR, LF chars */
					do
					{
						ch = pDataPtr[0];
						pDataPtr++;	
					
						if (pDataPtr==pDataPtrEnd)
							break;
					} 
					while ((ch==10) || (ch==13));
				
					if (pDataPtr==pDataPtrEnd)
						break;



				}
				break;
			}
		}
		while (pDataPtr<pDataPtrEnd);
	
		free(pDatabase);

		return pCheatDatabase;
	}

	return NULL;
}

