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

/* keyword contains a character, number or _. It must start
with a letter or _ */

/* number contains digits only */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parse.h"
#include "../cpcglob.h"

static char ParseBuffer[256];





typedef enum
{
	EQUATION_ELEMENT_NUMBER = 0,	/* number */
	EQUATION_ELEMENT_OPERATOR,	/* operator */
	EQUATION_ELEMENT_KEYWORD,	/* key-word */
	EQUATION_ELEMENT_IGNORE,
	EQUATION_ELEMENT_EOL,

	EQUATION_ELEMENT_HEXSEARCH_UNKNOWN_NUMBER,
	EQUATION_ELEMENT_HEXSEARCH_COMMA

} EQUATION_ELEMENT_ID;

typedef struct
{
	EQUATION_ELEMENT_ID Type;
	int Value;
} EQUATION_ELEMENT;



typedef struct
{
	unsigned char *pString;
} PARSE;

typedef enum
{
	EQUATION_OPERATOR_ADD = 0,
	EQUATION_OPERATOR_SUBTRACT,
	EQUATION_OPERATOR_MULTIPLY,
	EQUATION_OPERATOR_DIVIDE,
	EQUATION_OPERATOR_AND,
	EQUATION_OPERATOR_OR,
	EQUATION_OPERATOR_EQUAL,
	EQUATION_OPERATOR_OPEN_BRACKET,
	EQUATION_OPERATOR_CLOSE_BRACKET
} EQUATION_OPERATOR_ID;


typedef struct
{
	int ID;
	char *pString;
} PARSE_LOOKUP;

PARSE_LOOKUP	Operators[]=
{
	{EQUATION_OPERATOR_ADD,"+"},
	{EQUATION_OPERATOR_SUBTRACT,"-"},
	{EQUATION_OPERATOR_MULTIPLY,"*"},
	{EQUATION_OPERATOR_DIVIDE,"/"},
	{EQUATION_OPERATOR_AND,"&"},
	{EQUATION_OPERATOR_OR,"|"},
	{EQUATION_OPERATOR_OR,"OR"},
	{EQUATION_OPERATOR_EQUAL,"="},
	{EQUATION_OPERATOR_OPEN_BRACKET,"("},
	{EQUATION_OPERATOR_CLOSE_BRACKET,")"}
};

typedef struct
{
	int	OperatorID;
	int OperatorPriority;
	int OperatorPushPriority;
} REVERSE_POLISH_OPERAND_PRIORITY;

REVERSE_POLISH_OPERAND_PRIORITY	OperatorPriorities[]=
{

	{EQUATION_OPERATOR_EQUAL, 1,1},
	{EQUATION_OPERATOR_ADD,3,3},
	{EQUATION_OPERATOR_SUBTRACT,3,3},
	{EQUATION_OPERATOR_MULTIPLY,4,4},
	{EQUATION_OPERATOR_DIVIDE,4,4},
	{EQUATION_OPERATOR_OPEN_BRACKET,5,2},
	{EQUATION_OPERATOR_CLOSE_BRACKET,0,0},


};


#include "../z80/z80.h"

typedef enum
{
	EQUATION_KEYWORD_PC,
	EQUATION_KEYWORD_SP,
	EQUATION_KEYWORD_IX,
	EQUATION_KEYWORD_IY,
	EQUATION_KEYWORD_AF,
	EQUATION_KEYWORD_BC,
	EQUATION_KEYWORD_DE,
	EQUATION_KEYWORD_HL,
	EQUATION_KEYWORD_I,
	EQUATION_KEYWORD_R,
	EQUATION_KEYWORD_ALTAF,
	EQUATION_KEYWORD_ALTBC,
	EQUATION_KEYWORD_ALTDE,
	EQUATION_KEYWORD_ALTHL,
	EQUATION_KEYWORD_HIX,
	EQUATION_KEYWORD_LIX,
	EQUATION_KEYWORD_HIY,
	EQUATION_KEYWORD_LIY,
	EQUATION_KEYWORD_A,
	EQUATION_KEYWORD_F,
	EQUATION_KEYWORD_B,
	EQUATION_KEYWORD_C,
	EQUATION_KEYWORD_D,
	EQUATION_KEYWORD_E,
	EQUATION_KEYWORD_H,
	EQUATION_KEYWORD_L,
	EQUATION_KEYWORD_DMA0_PC,
	EQUATION_KEYWORD_DMA1_PC,
	EQUATION_KEYWORD_DMA2_PC

} EQUATION_KEYWORD_ID;

PARSE_LOOKUP	Keywords[]=
{
	{EQUATION_KEYWORD_AF,"AF"},
	{EQUATION_KEYWORD_BC,"BC"},
	{EQUATION_KEYWORD_DE,"DE"},
	{EQUATION_KEYWORD_HL,"HL"},
	{EQUATION_KEYWORD_ALTAF,"ALT_AF"},
	{EQUATION_KEYWORD_ALTBC,"ALT_BC"},
	{EQUATION_KEYWORD_ALTDE,"ALT_DE"},
	{EQUATION_KEYWORD_ALTHL,"ALT_HL"},

	{EQUATION_KEYWORD_PC,"PC"},
	{EQUATION_KEYWORD_SP,"SP"},

	{EQUATION_KEYWORD_IX,"IX"},
	{EQUATION_KEYWORD_IY,"IY"},
	
	{EQUATION_KEYWORD_I,"I"},
	{EQUATION_KEYWORD_R,"R"},

	
	{EQUATION_KEYWORD_HIX,"HIX"},
	{EQUATION_KEYWORD_LIX,"LIX"},
	{EQUATION_KEYWORD_HIY,"HIY"},
	{EQUATION_KEYWORD_LIY,"LIY"},

	{EQUATION_KEYWORD_A,"A"},
	{EQUATION_KEYWORD_F,"F"},
	{EQUATION_KEYWORD_B,"B"},
	{EQUATION_KEYWORD_C,"C"},
	{EQUATION_KEYWORD_D,"D"},
	{EQUATION_KEYWORD_E,"E"},
	{EQUATION_KEYWORD_H,"H"},
	{EQUATION_KEYWORD_L,"L"},

	{EQUATION_KEYWORD_DMA0_PC, "DMA0_PC"},
	{EQUATION_KEYWORD_DMA1_PC, "DMA1_PC"},
	{EQUATION_KEYWORD_DMA2_PC, "DMA2_PC"},


};

int	Parse__LookupID(unsigned char *pString, PARSE_LOOKUP *pTable, int NumEntries)
{
	int i;

	for (i=0; i<NumEntries; i++)
	{
		if (strcmp(pTable[i].pString, pString)==0)
		{
			return pTable[i].ID;
		}
	}

	return -1;
}

int	Parse__LookupOperator(unsigned char *pOperatorString)
{
	return Parse__LookupID(pOperatorString, Operators, sizeof(Operators)/sizeof(PARSE_LOOKUP));
}

int	Parse__LookupKeyword(unsigned char *pKeyword)
{
	return Parse__LookupID(pKeyword, Keywords, sizeof(Keywords)/sizeof(PARSE_LOOKUP));
}

/*-------------------------------------*/

BOOL	EquationParse__IsAOperatorChar(char ch)
{
	if 
		((ch=='+') || (ch=='-') || (ch=='*') || (ch=='/') ||
		(ch=='&') || (ch=='|') || (ch=='(') || (ch==')'))
	{
		return TRUE;
	}

	return FALSE;
}


BOOL	EquationParse__IsAKeywordChar(char ch)
{
	if (isalpha(ch) || (ch=='_'))
	{
		return TRUE;

	}

	return FALSE;
}

/* is a digit */
BOOL EquationParse__IsDigit(char ch)
{
	if ((ch>='0') && (ch<='9'))
		return TRUE;

	return FALSE;
}

BOOL EquationParse__IsABinaryDigit(char ch)
{
	if ((ch=='0') || (ch=='1'))
		return TRUE;

	return FALSE;
}


BOOL EquationParse__ConvertDigitToNumber(char ch)
{
	return ch-'0';
}

/* is a hex digit */
BOOL EquationParse__IsHexDigit(char ch)
{
	unsigned char upper_ch;

	if (EquationParse__IsDigit(ch))
		return TRUE;

	upper_ch = toupper(ch);

	if ((upper_ch>='A') && (upper_ch<='F'))
		return TRUE;

	return FALSE;
}

int	EquationParse__ConvertHexDigitToNumber(char ch)
{
	int Number;


	Number = toupper(ch)-'0';

	if (Number>=10)
	{
		Number -= ('A'-'0'-10);
	}

	return Number;
}

int	EquationParse__ConvertBinaryDigitToNumber(char ch)
{
	return ch-'0';
}

unsigned char EquationParse__GetChar(PARSE *pParse)
{
	char ch;

	ch = pParse->pString[0];
	pParse->pString++;

	return ch;
}

void EquationParse__BackChar(PARSE *pParse)
{
	pParse->pString--;
}


unsigned long	Parse__ReadHexNumber(PARSE *pParse)
{
	char ch;
	unsigned long Number;
	BOOL IsAHexDigit;

	EquationParse__GetChar(pParse);
	
	Number = 0;

	do
	{
	
		/* get char */
		ch = EquationParse__GetChar(pParse);
		
		/* is a hex digit? */
		IsAHexDigit = EquationParse__IsHexDigit(ch);

		/* yes */
		if (IsAHexDigit)
		{
			Number = Number<<4;
			/* convert to hex and add to current number */
			Number |= EquationParse__ConvertHexDigitToNumber(ch);
		}
	}
	while ((IsAHexDigit) && (ch!='\0'));

	EquationParse__BackChar(pParse);
	
	return Number;
}

unsigned long Parse__ReadBinaryNumber(PARSE *pParse)
{
	char ch;
	unsigned long Number;
	BOOL IsABinaryDigit;

	EquationParse__GetChar(pParse);
	
	Number = 0;

	do
	{

		/* get char */
		ch = EquationParse__GetChar(pParse);
	
		/* is a binary digit? */
		IsABinaryDigit = EquationParse__IsABinaryDigit(ch);

		/* yes */
		if (IsABinaryDigit)
		{
			Number = Number<<1;
			/* convert to a binary digit and add to current number */
			Number |= EquationParse__ConvertBinaryDigitToNumber(ch);
		}
	}
	while ((IsABinaryDigit) && (ch!='\0'));

	EquationParse__BackChar(pParse);
	
	return Number;
}


unsigned long	Parse__ReadDecimalNumber(PARSE *pParse)
{
	char ch;
	unsigned long Number;
	BOOL IsADigit;

	Number = 0;

	do
	{
	
		/* get char */
		ch = EquationParse__GetChar(pParse);
	
		/* is a hex digit? */
		IsADigit = EquationParse__IsDigit(ch);

		/* yes */
		if (IsADigit)
		{
			Number = Number * 10;
			/* convert to hex and add to current number */
			Number += EquationParse__ConvertDigitToNumber(ch);
		}
	}
	while ((IsADigit) && (ch!='\0'));

	EquationParse__BackChar(pParse);
	

	return Number;
}

unsigned char *Parse__ReadOperator(PARSE *pParse)
{
	char ch;
	BOOL IsAOperatorChar;
	int Index;
	
	Index = 0;

	do
	{
	
		/* get char */
		ch = EquationParse__GetChar(pParse);
	
		/* is a hex digit? */
		IsAOperatorChar = EquationParse__IsAOperatorChar(ch);

		/* yes */
		if (IsAOperatorChar)
		{
			ParseBuffer[Index] = ch;
			Index++;

		}
	}
	while ((IsAOperatorChar) && (ch!='(') && (ch!=')'));

	ParseBuffer[Index] = '\0';

	if ((ch!='(') && (ch!=')'))
		EquationParse__BackChar(pParse);
	

	return ParseBuffer;
}


unsigned char *Parse__ReadKeyword(PARSE *pParse)
{
	char ch;
	BOOL IsAKeywordChar;
	int Index;
	
	Index = 0;

	do
	{
		/* get char */
		ch = EquationParse__GetChar(pParse);
	
		/* is a keyword char? */
		IsAKeywordChar = (EquationParse__IsAKeywordChar(ch) || isdigit(ch));

		/* yes */
		if (IsAKeywordChar)
		{
			ParseBuffer[Index] = toupper(ch);
			Index++;

		}
	}
	while (IsAKeywordChar);

	ParseBuffer[Index] = '\0';

	EquationParse__BackChar(pParse);
	

	return ParseBuffer;
}



void	EquationParse__GetItem(PARSE *pParse, EQUATION_ELEMENT *pElement)
{
	char ch;


	ch = EquationParse__GetChar(pParse);
	EquationParse__BackChar(pParse);
	
	if (ch=='\0')
	{
		pElement->Type = EQUATION_ELEMENT_EOL;
		return;
	}
	else
	if (ch==' ')
	{
		/* space */
		do
		{
			ch = EquationParse__GetChar(pParse);
		}
		while (ch==' ');

		EquationParse__BackChar(pParse);
	

	
		pElement->Type = EQUATION_ELEMENT_IGNORE;
		pElement->Value = 0;
	}
	else
	if (ch=='#')
	{
		/* hex number */
		unsigned long Number;

		Number = Parse__ReadHexNumber(pParse);
	
		pElement->Type = EQUATION_ELEMENT_NUMBER;
		pElement->Value = Number;
	}
	else
	if (ch=='%')
	{
		/* decimal number */
		unsigned long Number;

		Number = Parse__ReadBinaryNumber(pParse);
		
		pElement->Type = EQUATION_ELEMENT_NUMBER;
		pElement->Value = Number;
	}
	else
	if (EquationParse__IsDigit(ch))
	{
		unsigned long Number;

		Number = Parse__ReadDecimalNumber(pParse);

		pElement->Type = EQUATION_ELEMENT_NUMBER;
		pElement->Value = Number;
	}
	else
	if (EquationParse__IsAOperatorChar(ch))
	{
		unsigned long OperatorID;
		unsigned char *Operator;

		Operator = Parse__ReadOperator(pParse);
		
		OperatorID = Parse__LookupOperator(Operator);
		
		pElement->Type = EQUATION_ELEMENT_OPERATOR;
		pElement->Value = OperatorID;
	}
	else
	{
		unsigned long OperatorID;
		unsigned char *Operator;

		Operator = Parse__ReadKeyword(pParse);


		OperatorID = Parse__LookupKeyword(Operator);
		
		pElement->Type = EQUATION_ELEMENT_KEYWORD;
		pElement->Value = OperatorID;
	}
}

/*---------------------------------------------------------------------*/

int	operator_get_priority(int operator_id)
{
	int i;

	for (i=0; i<sizeof(OperatorPriorities)/sizeof(REVERSE_POLISH_OPERAND_PRIORITY); i++)
	{
		if (OperatorPriorities[i].OperatorID==operator_id)
		{
			return OperatorPriorities[i].OperatorPriority;
		}
	}

	return -1;
}

	
int	operator_get_push_priority(int operator_id)
{
	int i;

	for (i=0; i<sizeof(OperatorPriorities)/sizeof(REVERSE_POLISH_OPERAND_PRIORITY); i++)
	{
		if (OperatorPriorities[i].OperatorID==operator_id)
		{
			return OperatorPriorities[i].OperatorPushPriority;
		}
	}

	return -1;
}

int ReversePolishEquationCount;
EQUATION_ELEMENT	ReversePolishEquation[256];

/* OPERATOR STACK */

int		OperatorStack_PriorityOfTopItem = 0;
int		OperatorStack_Count = 0;
EQUATION_OPERATOR_ID	OperatorStack_Stack[256];

/* push operator onto stack */
void	operatorstack_push_operator_onto_stack(int operator_id)
{
	/* get priority of this operator */
	OperatorStack_PriorityOfTopItem= operator_get_push_priority(operator_id);

	/* store this operator on the stack */
	OperatorStack_Stack[OperatorStack_Count] = operator_id;
	OperatorStack_Count++;
}

/* get priority of operator at top of stack */
int operatorstack_get_top_operator_priority(void)
{
	return OperatorStack_PriorityOfTopItem;
}

void	operatorstack_empty_stack(void)
{

	if (OperatorStack_Count!=0)
	{
		int operator_id;

		do
		{

			operator_id = OperatorStack_Stack[OperatorStack_Count-1];

			if (operator_id!=EQUATION_OPERATOR_OPEN_BRACKET)
			{
				ReversePolishEquation[ReversePolishEquationCount].Type = EQUATION_ELEMENT_OPERATOR;
				ReversePolishEquation[ReversePolishEquationCount].Value = operator_id;
				ReversePolishEquationCount++;

				OperatorStack_Count--;
			}
		}
		while ((OperatorStack_Count!=0) && (operator_id!=EQUATION_OPERATOR_OPEN_BRACKET));

	}
}


void	ConvertToReversePolish(char *ParseString)
{
	EQUATION_ELEMENT	EquationElement;
	PARSE Parsef;
	int Type;

	Parsef.pString = ParseString;
	ReversePolishEquationCount = 0;

	do
	{
		int Value;

		EquationParse__GetItem(&Parsef, &EquationElement);

		Type = EquationElement.Type;
		Value = EquationElement.Value;

		if (Type!= EQUATION_ELEMENT_EOL)
		{
			if (Type == EQUATION_ELEMENT_NUMBER)
			{
				ReversePolishEquation[ReversePolishEquationCount].Type = EQUATION_ELEMENT_NUMBER;
				ReversePolishEquation[ReversePolishEquationCount].Value = Value;
				ReversePolishEquationCount++;
			}
			else
			if (Type == EQUATION_ELEMENT_KEYWORD)
			{
				int KeywordValue = 0;

				switch (EquationElement.Value)
				{
					case EQUATION_KEYWORD_PC:
					{
						KeywordValue = Z80_GetReg(Z80_PC);
					}
					break;
					
					case EQUATION_KEYWORD_SP:
					{
						KeywordValue = Z80_GetReg(Z80_SP);
					}
					break;
					
					case EQUATION_KEYWORD_IX:
					{
						KeywordValue = Z80_GetReg(Z80_IX);
					}
					break;

					case EQUATION_KEYWORD_IY:
										{
						KeywordValue = Z80_GetReg(Z80_IY);
					}
					break;

					case EQUATION_KEYWORD_AF:
										{
						KeywordValue = Z80_GetReg(Z80_AF);
					}
					break;

					case EQUATION_KEYWORD_BC:
										{
						KeywordValue = Z80_GetReg(Z80_BC);
					}
					break;

					case EQUATION_KEYWORD_DE:
										{
						KeywordValue = Z80_GetReg(Z80_DE);
					}
					break;

					case EQUATION_KEYWORD_HL:
										{
						KeywordValue = Z80_GetReg(Z80_HL);
					}
					break;

					case EQUATION_KEYWORD_I:
										{
						KeywordValue = Z80_GetReg(Z80_I);
					}
					break;

					case EQUATION_KEYWORD_R:
										{
						KeywordValue = Z80_GetReg(Z80_R);
					}
					break;

					case EQUATION_KEYWORD_ALTAF:
										{
							KeywordValue = Z80_GetReg(Z80_AF2);
					}
					break;

					case EQUATION_KEYWORD_ALTBC:
										{
						KeywordValue = Z80_GetReg(Z80_BC2);
					}
					break;

					case EQUATION_KEYWORD_ALTDE:
										{
						KeywordValue = Z80_GetReg(Z80_DE2);
					}
					break;

					case EQUATION_KEYWORD_ALTHL:
										{
						KeywordValue = Z80_GetReg(Z80_HL2);
					}
					break;
					case EQUATION_KEYWORD_HIX:
					{
						KeywordValue = (Z80_GetReg(Z80_IX)>>8) & 0x0ff;
					}
					break;
					case EQUATION_KEYWORD_LIX:
					{
						KeywordValue = Z80_GetReg(Z80_IX) & 0x0ff;
					}
					break;

					case EQUATION_KEYWORD_HIY:
										{
						KeywordValue = (Z80_GetReg(Z80_IY)>>8) & 0x0ff;
					}
					break;
					case EQUATION_KEYWORD_LIY:
					{
						KeywordValue = Z80_GetReg(Z80_IY) & 0x0ff;
					}
					break;
					
					case EQUATION_KEYWORD_A:
					{
						KeywordValue = (Z80_GetReg(Z80_AF)>>8) & 0x0ff;
					}
					break;
					case EQUATION_KEYWORD_F:
					{
						KeywordValue = Z80_GetReg(Z80_AF) & 0x0ff;
					}
					break;
					case EQUATION_KEYWORD_B:
					{
						KeywordValue = (Z80_GetReg(Z80_BC)>>8) & 0x0ff;;
					}
					break;
					case EQUATION_KEYWORD_C:
					{
						KeywordValue = Z80_GetReg(Z80_BC) & 0x0ff;
					}
					break;
					case EQUATION_KEYWORD_D:
					{
						KeywordValue = (Z80_GetReg(Z80_DE)>>8) & 0x0ff;;
					}
					break;
					case EQUATION_KEYWORD_E:
					{
						KeywordValue = Z80_GetReg(Z80_DE) & 0x0ff;
					}
					break;
					case EQUATION_KEYWORD_H:
					{
						KeywordValue = (Z80_GetReg(Z80_HL)>>8) & 0x0ff;;
					}
					break;
					case EQUATION_KEYWORD_L:
					{
						KeywordValue = Z80_GetReg(Z80_HL) & 0x0ff;
					}
					break;
					case EQUATION_KEYWORD_DMA0_PC:
					{
						KeywordValue = ASIC_DMA_GetChannelAddr(0);
					}
					break;
					case EQUATION_KEYWORD_DMA1_PC:
					{
						KeywordValue = ASIC_DMA_GetChannelAddr(1);
					}
					break;
					case EQUATION_KEYWORD_DMA2_PC:
					{
						KeywordValue = ASIC_DMA_GetChannelAddr(2);
					}
					break;
		
				}

				ReversePolishEquation[ReversePolishEquationCount].Type = EQUATION_ELEMENT_NUMBER;
				ReversePolishEquation[ReversePolishEquationCount].Value = KeywordValue;
				ReversePolishEquationCount++;
			}
			else
			if (Type == EQUATION_ELEMENT_OPERATOR)
			{
				/* operator */

				int operator_id;
				int stack_priority;
				int operator_priority;

				/* get operator id */
				operator_id = Value;
				
				/* get priority of item at top of stack */
				stack_priority = operatorstack_get_top_operator_priority();
				
				/* get comparison priority */
				operator_priority = operator_get_priority(operator_id);

				/* if priority is higher push onto stack */
				if (operator_priority>=stack_priority)
				{
					/* push operator */
					operatorstack_push_operator_onto_stack(operator_id);
				}
				else
				{
					/* pop operators */
					operatorstack_empty_stack();	

					operatorstack_push_operator_onto_stack(operator_id);
				}
			}
		}
	}
	while (Type!= EQUATION_ELEMENT_EOL);

	operatorstack_empty_stack();	

}


int OperandStackSize = 0;
int OperandStack[256];

void	operand_push(int value)
{
	OperandStack[OperandStackSize] = value;
	OperandStackSize++;
}

int	operand_pop(void)
{
	OperandStackSize--;
	return OperandStack[OperandStackSize];
}


int	InterpretReversePolish(void)
{
	int i;

	for (i=0; i<ReversePolishEquationCount; i++)
	{
		if (ReversePolishEquation[i].Type == EQUATION_ELEMENT_NUMBER)
		{
			operand_push(ReversePolishEquation[i].Value);
		}
		else
		{
			int operator_id;

			operator_id = ReversePolishEquation[i].Value;

			switch (operator_id)
			{
				case EQUATION_OPERATOR_MULTIPLY:
				{
					int a,b;

					a = operand_pop();
					b = operand_pop();

					operand_push(a*b);
				}
				break;
				case EQUATION_OPERATOR_ADD:
				{
					int a,b;

					a = operand_pop();
					b = operand_pop();

					operand_push(a+b);
				}
				break;
				case EQUATION_OPERATOR_SUBTRACT:
				{
					int a,b;

					a = operand_pop();
					b = operand_pop();
					operand_push(b-a);
				}
				break;

				case EQUATION_OPERATOR_DIVIDE:
				{
					int a,b;

					a = operand_pop();
					b = operand_pop();
					if (b!=0)
					{
						operand_push(a/b);
					}
					else
					{
						operand_push(0);
					}
				}
				break; 

				case EQUATION_OPERATOR_AND:
				{
					int a,b;

					a = operand_pop();
					b = operand_pop();
				
					operand_push(a & b);
				
				}
				break; 

				case EQUATION_OPERATOR_OR:
				{
					int a,b;

					a = operand_pop();
					b = operand_pop();
				
					operand_push(a | b);
				}
				break;

				default:
					break;
			}
		}

	}

	return operand_pop();
}

int	EvaluateExpression(char *Expression)
{
	ConvertToReversePolish(Expression);
	return InterpretReversePolish();
}

/*-------------------------------------*/

void	HexSearch__GetItem(PARSE *pParse, EQUATION_ELEMENT *pElement)
{
	char ch;


	ch = EquationParse__GetChar(pParse);
	EquationParse__BackChar(pParse);
	
	if (ch=='\0')
	{
		pElement->Type = EQUATION_ELEMENT_EOL;
		return;
	}
	else
	if (ch==' ')
	{
		/* space */
		do
		{
			ch = EquationParse__GetChar(pParse);
		}
		while (ch==' ');

		EquationParse__BackChar(pParse);
	

	
		pElement->Type = EQUATION_ELEMENT_IGNORE;
		pElement->Value = 0;
	}
	else
	if (ch=='#')
	{
		/* hex number */
		unsigned long Number;

		Number = Parse__ReadHexNumber(pParse);
	
		pElement->Type = EQUATION_ELEMENT_NUMBER;
		pElement->Value = Number;
	}
	else
	if (ch=='%')
	{
		/* decimal number */
		unsigned long Number;

		Number = Parse__ReadBinaryNumber(pParse);
		
		pElement->Type = EQUATION_ELEMENT_NUMBER;
		pElement->Value = Number;
	}
	else
	if (EquationParse__IsDigit(ch))
	{
		unsigned long Number;

		Number = Parse__ReadDecimalNumber(pParse);

		pElement->Type = EQUATION_ELEMENT_NUMBER;
		pElement->Value = Number;
	}
	else
	if (ch=='?')
	{
		do
		{
			ch = EquationParse__GetChar(pParse);

		}
		while (ch=='?');

		EquationParse__BackChar(pParse);
		
		pElement->Type = EQUATION_ELEMENT_HEXSEARCH_UNKNOWN_NUMBER;
		pElement->Value = 0;
	}
	else
	if (ch==',')
	{
		EquationParse__GetChar(pParse);
		
		pElement->Type = EQUATION_ELEMENT_HEXSEARCH_COMMA;
		pElement->Value = 0;
	}
}

static char SearchString[256];
static char SearchStringMask[256];

void	HexSearch_Evaluate(char *pString,int *pSearchStringCount, char **pSearchString, char **pSearchMask)
{
	EQUATION_ELEMENT	EquationElement;
	PARSE Parsef;
	int Type;
	int SearchStringCount;

	Parsef.pString = pString;

	SearchStringCount = 0;

	do
	{
		int Value;

		HexSearch__GetItem(&Parsef, &EquationElement);

		Type = EquationElement.Type;
		Value = EquationElement.Value;

		if ((Type!=EQUATION_ELEMENT_EOL) && (Type!=EQUATION_ELEMENT_IGNORE) && (Type!=EQUATION_ELEMENT_HEXSEARCH_COMMA))
		{
			if (Type == EQUATION_ELEMENT_NUMBER)
			{
				SearchString[SearchStringCount] = Value;
				SearchStringMask[SearchStringCount] = 0x0ff;
				SearchStringCount++;
			}
			else
			if (Type == EQUATION_ELEMENT_HEXSEARCH_UNKNOWN_NUMBER)
			{
				SearchString[SearchStringCount] = 0;
				SearchStringMask[SearchStringCount] = 0x000;
				SearchStringCount++;
			}
		}

	}
	while ((Type!=EQUATION_ELEMENT_EOL) && (SearchStringCount<16));

	*pSearchStringCount = SearchStringCount;
	*pSearchString = SearchString;
	*pSearchMask = SearchStringMask;
}