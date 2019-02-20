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



/*------------------------------------------*/
/* Z80 Dissassembler						*/
/* (smaller program because of less tables) */
const unsigned char *RegA[]=
{
	"B",
	"C",
	"D",
	"E",
	"H",
	"L",
	"(HL)",
	"A",
	"X",
	"0"
};


const unsigned char *RegB[]=
{
	"BC",
	"DE",
	"HL",
	"SP"
};

const unsigned char *RegC[]=
{
	"BC",
	"DE",
	"HL",
	"AF"
};

const unsigned char *CB_ShiftMneumonics[]=
{
	"RLC",
	"RRC",
	"RL",
	"RR",
	"SLA",
	"SRA",
	"SLL",
	"SRL"
};

const unsigned char *CB_BitOperationMneumonics[]=
{
	"BIT",
	"RES",
	"SET"
};

const unsigned char *ArithmeticMneumonics[]=
{
	"ADD",
	"ADC",
	"SUB",
	"SBC",
	"AND",
	"XOR",
	"OR",
	"CP"
};

const unsigned char *ConditionCodes[]=
{
	"NZ",
	"Z",
	"NC",
	"C",
	"PO",
	"PE",
	"P",
	"M"
};

const unsigned char *ShiftMneumonics[]=
{
	"RLCA",
	"RRCA",
	"RLA",
	"RRA"
};

const unsigned char *MiscMneumonics1[]=
{
	"DAA",
	"CPL",
	"SCF",
	"CCF"
}; 

const unsigned char *MiscMneumonics2[]=
{
	"NOP",
	"EX AF, AF",
	"DJNZ",
	"JR"
};

const unsigned char *MiscMneumonics3[]=
{
	"RET",
	"EXX",
	"JP (HL)",
	"LD SP,HL"
};

const unsigned char *MiscMneumonics4[]=
{
	"LD I,A",
	"LD R,A",
	"LD A,I",
	"LD A,R",
	"RLD",
	"RRD",
	"ED NOP",
	"ED NOP"
};

const unsigned char *MiscMneumonics5[]=
{
	"LDI",
	"LDD",
	"LDIR",
	"LDDR"
};

const unsigned char *MiscMneumonics6[]=
{
	"CPI",
	"CPDR",
	"CPIR",
	"CPD"
};

const unsigned char *MiscMneumonics7[]=
{
	"INI",
	"INIR",
	"IND",
	"INDR"
};

const unsigned char *MiscMneumonics8[]=
{
	"OUTI",
	"OTIR",
	"OUTD",
	"OTDR"
};

const unsigned char *MiscMneumonics9[]=
{
	"EX (SP),HL",
	"EX DE,HL",
	"DI",
	"EI"
};


/* get word at address specified by Addr */
unsigned long Diss_GetWord(Z80_WORD Addr)
{
	return (Z80_RD_MEM(Addr) & 0x0ff) | 
		((Z80_RD_MEM((Addr+1) & 0x0ffff) & 0x0ff)<<8);
}

/* get relative addr for JR,DJNZ */
Z80_WORD	Diss_GetRelativeAddr(Z80_WORD Addr)
{
	Z80_WORD Offset;

	/* get signed byte offset */
	Offset = Z80_RD_MEM(Addr);

	/* sign extend */
	if ((Offset & 0x080)!=0)
	{
		Offset|=0x0ff00;
	}
	else
	{
		Offset&=0x0ff;
	}

	/* return addr */
	return (Addr+1+Offset);
}

unsigned char *pDissassemblyString;
unsigned char *pDissassemblyStringStart;


static void	Diss_strcat(const char *pSourceString)
{
	unsigned char *pDestString = pDissassemblyString;
	unsigned char ch;

	int Index = 0;
	
	do
	{
		
		ch = pSourceString[Index];

		if (ch=='\0')
			break;


		pDestString[Index] = ch;

		Index++;
	}
	while (1==1);

	pDissassemblyString = &pDestString[Index];
}

static void	Diss_strcat_only(const char *pSourceString)
{
	unsigned char *pDestString = pDissassemblyString;
	unsigned char ch;

	int Index = 0;
	
	do
	{
		
		ch = pSourceString[Index];

		pDestString[Index] = ch;

		if (ch=='\0')
			break;

		Index++;
	}
	while (1==1);

	pDissassemblyString = &pDestString[Index];
}


static unsigned char Diss_GetHexDigitAsChar(unsigned char HexDigit)
{
	HexDigit&=0x0f;
	HexDigit+='0';

	/* 10-15? */
	if (HexDigit>=('0'+10))
	{
		/* A-F */
		HexDigit+='A'-'0'-10;
	}
	
	return HexDigit;
}
	

static void	Diss_WriteHexByte(unsigned char Value)
{
	unsigned char HexDigit;
	unsigned char *pString = pDissassemblyString;

	/* write identifier to indicate hex value */
	pString[0] = '#';

	/* write high digit */
	HexDigit = Value>>4;

	pString[1] = Diss_GetHexDigitAsChar(HexDigit);
	
	HexDigit = Value;

	pString[2] = Diss_GetHexDigitAsChar(HexDigit);

	pDissassemblyString = &pString[3];
}

static void Diss_WriteHexWord(unsigned long Value)
{
	unsigned char HexDigit;
	unsigned char *pString = pDissassemblyString;

	pString[0] = '#';

	HexDigit = Value>>12;

	pString[1] = Diss_GetHexDigitAsChar(HexDigit);

	HexDigit = Value>>8;

	pString[2] = Diss_GetHexDigitAsChar(HexDigit);

	HexDigit = Value>>4;

	pString[3] = Diss_GetHexDigitAsChar(HexDigit);

	HexDigit = Value;

	pString[4] = Diss_GetHexDigitAsChar(HexDigit);

	pDissassemblyString = &pString[5];
}


static void	Diss_IndexedOffset(signed char Offset, unsigned char IndexCh)
{
	unsigned char *pString = pDissassemblyString;

	pString[0]='(';
	pString[1]='I';
	pString[2]=IndexCh;

	/* write signed offset */
	if (Offset<0)
	{
		pString[3] = '-';
	}
	else
	{
		pString[3] = '+';
	}
	pString+=4;
	pDissassemblyString = pString;


	Diss_WriteHexByte(Offset & 0x07f);

	/* up to here is quite optimal for C anyway */

	pString = pDissassemblyString;

	pString[0]=')';
	pString++;

	pDissassemblyString = pString;
}

INLINE static void Diss_ContentsOfAddress(Z80_WORD DisAddr)
{
	*pDissassemblyString = '(';
	pDissassemblyString++;

	Diss_WriteHexWord(Diss_GetWord(DisAddr));

	*pDissassemblyString = ')';
	pDissassemblyString++;
}										


static void	Diss_Index_Reg8Bit(unsigned char RegIndex, Z80_WORD DisAddr, unsigned char IndexCh)
{
	if ((RegIndex<4) || (RegIndex>6))
		return;

	if (RegIndex==6)
	{
		/* (HL) -> (IX+offs) */
		unsigned char Offset;

		/* signed offset from index register */
		Offset = Z80_RD_MEM(DisAddr);

		Diss_IndexedOffset(Offset,IndexCh);

	}
	else if ((RegIndex==4) || (RegIndex==5))
	{
		/* H -> HIX */
		/* L -> HIX */

		unsigned char *pString = pDissassemblyString;

		if (RegIndex==4)
		{
			pString[0] = 'H';
		}
		else
		{
			pString[0] = 'L';
		}

		pString[1] = 'I';
		pString[2] = IndexCh;

		pDissassemblyString = &pString[3];
	}
}

static BOOL	Diss_Index_IsReg8Bit(unsigned char RegIndex)
{
	if ((RegIndex<4) || (RegIndex>6))
		return FALSE;

	return TRUE;
}

static BOOL Diss_Index_IsRegIndex(unsigned char RegIndex)
{
	if (RegIndex==2)
		return TRUE;

	return FALSE;
}


static void	Diss_Port(void)
{
	unsigned char *pString = pDissassemblyString;

	pString[0]='(';
	pString[1]='C';
	pString[2]=')';

	pDissassemblyString=&pString[3];
}

INLINE static void Diss_IndexReg(unsigned char IndexCh)
{
	unsigned char *pString = pDissassemblyString;

	pString[0] = 'I';
	pString[1] = IndexCh;

	pDissassemblyString = &pString[2];
}



INLINE static void	Diss_space(void)
{
	*pDissassemblyString = ' ';
	pDissassemblyString++;
}

INLINE static void	Diss_colon(void)
{
	*pDissassemblyString = ':';
	pDissassemblyString++;
}


INLINE static void	Diss_comma(void)
{
	*pDissassemblyString = ',';
	pDissassemblyString++;
}

INLINE static void	Diss_endstring(void)
{
	*pDissassemblyString = '\0';
	pDissassemblyString++;
}

static void	Diss_Index(int DisAddr, char *OutputString,unsigned char IndexCh)
{
	unsigned char Opcode;

	/* DD prefix - IX */
	Opcode = Z80_RD_MEM(DisAddr);
	DisAddr++;

	switch (Opcode)
	{
		case 0x0dd:
		case 0x0fd:
		case 0x0ed:
		{
/*			Diss_strcat("DEFB");
			Diss_WriteHexByte(Opcode);
			Diss_endstring();
*/		}
		break;

		case 0x0cb:
		{
			signed char Offset;

			/* CB prefix */

			/* signed offset from IX */
			Offset = Z80_RD_MEM(DisAddr);
			DisAddr++;

			/* opcode */
			Opcode = Z80_RD_MEM(DisAddr);

			if ((Opcode & 0x0c0)==0x00)
			{
				/* 00000rrr - RLC */
				/* 00001rrr - RRC */
				/* 00010rrr - RL */
				/* 00011rrr - RR */
				/* 00100rrr - SLA */
				/* 00101rrr - SRA */
				/* 00110rrr - SLL */
				/* 00111rrr - SRL */

				if ((Opcode & 7)!=6)
				{
					/* LD r, */
					Diss_strcat("LD");
					Diss_space();
					Diss_strcat(RegA[(Opcode & 7)]);
					Diss_comma();
				}

				/* write shift mneumonic */
				Diss_strcat(CB_ShiftMneumonics[((Opcode>>3) & 0x07)]);
				Diss_space();

				Diss_IndexedOffset(Offset, IndexCh);

				Diss_endstring();

			}
			else
			{
				/* 01bbbrrr - BIT */
				/* 10bbbrrr - RES */
				/* 11bbbrrr - SET */
				int BitIndex = ((Opcode>>3) & 0x07);
				int RegIndex = (Opcode & 0x07);

				if (((Opcode & 0x0c0)!=0x040) && (RegIndex!=6))
				{
					Diss_strcat("LD");
					Diss_space();
					Diss_strcat(RegA[RegIndex]);
					Diss_comma();
				}
								
				Diss_strcat(CB_BitOperationMneumonics[(((Opcode>>6)&3)-1)]);
				Diss_space();
				*pDissassemblyString = BitIndex+'0';
				pDissassemblyString++;
				Diss_comma();

				Diss_IndexedOffset(Offset, IndexCh);
				Diss_endstring();

			}

		}
		return;

		default:
		{
			switch (Opcode & 0x0c0)
			{
				case 0x000:
				{
					switch (Opcode & 0x07)
					{
						case 1:
						{
							if ((Opcode & 0x08)!=0)
							{
								/* 00ss1001 - ADD HL,ss */
								unsigned char RegIndex = ((Opcode>>4) & 0x03);

								
								Diss_strcat("ADD");
								Diss_space();
								Diss_IndexReg(IndexCh);
								Diss_comma();

								if (Diss_Index_IsRegIndex(RegIndex))
								{
									Diss_IndexReg(IndexCh);
								}
								else
								{
									Diss_strcat(RegB[RegIndex]);
								}
								Diss_endstring();
								return;
							}
							else
							{
								/* 00dd0001 - LD dd,nn */
							
								if (Diss_Index_IsRegIndex(((Opcode>>4) & 0x03)))
								{
									Diss_strcat("LD");
									Diss_space();
									Diss_IndexReg(IndexCh);
									Diss_comma();
									Diss_WriteHexWord(Diss_GetWord(DisAddr));
									Diss_endstring();
									return;
								}
							}
					
						}
						break;

						case 2:
						{
							switch ((Opcode>>4) & 0x03)
							{
								case 2:
								{
									/* 00100010 - LD (nnnn),HL */
									/* 00101010 - LD HL,(nn) */

									Diss_strcat("LD");
									Diss_space();


									if ((Opcode & (1<<3))==0)
									{
										Diss_ContentsOfAddress(DisAddr);
										Diss_comma();
										Diss_IndexReg(IndexCh);
										
	
									}
									else
									{
										Diss_IndexReg(IndexCh);
										Diss_comma();
										Diss_ContentsOfAddress(DisAddr);
									}
									Diss_endstring();

								}
								return;

								default:
									break;
							}

						}
						break;

						case 3:
						{
							unsigned char *Instruction;

							if (Diss_Index_IsRegIndex(((Opcode>>4) & 0x03)))
							{
								if ((Opcode & 0x08)==0)
								{
									/* 00ss0011 - INC ss */
									Instruction = "INC";
								}
								else
								{
									/* 00ss1011 - DEC ss */
									Instruction= "DEC";
								}

								Diss_strcat(Instruction);
								Diss_space();
								Diss_IndexReg(IndexCh);
								Diss_endstring();
								return;
							}

						}
						break;

						case 4:
						{
							/* 00rrr100 - INC r */
							Diss_strcat("INC");
							Diss_space();
							Diss_Index_Reg8Bit(((Opcode>>3) & 0x07),DisAddr, IndexCh);
							Diss_endstring();

						}
						return;				
					
						case 5:
						{
							/* 00rrr101 - DEC r */
							Diss_strcat("DEC");
							Diss_space();
							Diss_Index_Reg8Bit(((Opcode>>3) & 0x07),DisAddr, IndexCh);
							Diss_endstring();
						}
						return;				

						case 6:
						{
							/* LD r,n - 00rrr110 */
							unsigned char Data = Z80_RD_MEM(DisAddr+1);

							Diss_strcat("LD");
							Diss_space();
							Diss_Index_Reg8Bit(((Opcode>>3) & 0x07),DisAddr, IndexCh);
							Diss_comma();
							Diss_WriteHexByte(Data);
							Diss_endstring();

						}
						break;

						default:
							break;
					}
				}
				break;

				case 0x040:
				{
					/* 01xxxxxx */
					/* HALT, LD r,R */
					unsigned char Reg1,Reg2;
					/* will not get here if defb &dd:HALT is encountered! */
					
					/* 01rrrRRR - LD r,R */
					Diss_strcat("LD");
					Diss_space();
					
					Reg1 = (Opcode>>3) & 0x07;
					Reg2 = Opcode & 0x07; 

					if ((Reg1==6) || (Reg2==6))
					{
						if (Reg1==6)
						{
							Diss_Index_Reg8Bit(Reg1, DisAddr, IndexCh);
						}
						else
						{
							Diss_strcat(RegA[Reg1]);
						}

						Diss_comma();
						
						if (Reg2==6)
						{
							Diss_Index_Reg8Bit(Reg2, DisAddr, IndexCh);
						}
						else
						{
							Diss_strcat(RegA[Reg2]);
						}

					}
					else
					{ 
						Diss_Index_Reg8Bit(Reg1,DisAddr, IndexCh);
						Diss_comma();
						Diss_Index_Reg8Bit(Reg2,DisAddr,IndexCh);
					}

					Diss_endstring();
				}
				return;

				case 0x080:
				{
					/* 10xxxxxx */
					/* 10000rrr - ADD */
					/* 10001rrr - ADC */
					/* 10010rrr - SUB */
					/* 10011rrr - SBC */
					/* 10100rrr - AND */
					/* 10101rrr - XOR */
					/* 10110rrr - OR */
					/* 10111rrr - CP */
					Diss_strcat(ArithmeticMneumonics[((Opcode>>3) & 0x07)]);
					Diss_space();
					Diss_Index_Reg8Bit((Opcode & 0x07),DisAddr, IndexCh);
					Diss_endstring();
				}
				return;

				case 0x0c0:
				{
					/* 11xxxxxx */
			
			
					switch (Opcode & 0x07)
					{
						case 1:
						{
							if ((Opcode & (1<<3))==0)
							{
								/* 11qq0001 - POP qq */
								Diss_strcat("POP");
								Diss_space();
								Diss_IndexReg(IndexCh);
								Diss_endstring();
							}
							else
							{	
								/* 11001001 - RET */
								/* 11011001 - EXX */
								/* 11101001 - JP (HL) */
								/* 11111001 - LD SP,HL */

								if (Opcode==0x0e9)
								{
									Diss_strcat("JP");
									Diss_space();
									pDissassemblyString[0]='(';
									pDissassemblyString++;
									Diss_IndexReg(IndexCh);
									pDissassemblyString[0]=')';
									pDissassemblyString++;
									Diss_endstring();
								} 
								else if (Opcode==0x0f9)
								{
									Diss_strcat("LD");
									Diss_space();
									Diss_strcat("SP");
									Diss_comma();
									Diss_IndexReg(IndexCh);
									Diss_endstring();
								}
							}
						}
						return;
				
						case 3:
						{
							/* 11001011 - CB prefix */
						
							/* 11011011 - IN A,(n) */
							/* 11010011 - OUT (n),A */
							/* 11000011 - JP nn */
							/* 11100011 - EX (SP).HL */
							/* 11101011 - EX DE,HL */
							/* 11110011 - DI */
							/* 11111011 - EI */

							if (Opcode==0x0e3)
							{
								/* EX (SP), IX */
								Diss_strcat("EX");
								Diss_space();
								pDissassemblyString[0]='(';
								pDissassemblyString++;
								Diss_strcat("SP");
								pDissassemblyString[0]=')';
								pDissassemblyString++;
								Diss_IndexReg(IndexCh);
								Diss_endstring();	
							}

						}
						return;

						case 5:
						{
							/* 11qq0101 - PUSH qq */
							if ((Opcode & (1<<3))==0)
							{
								Diss_strcat("PUSH");
								Diss_space();
								Diss_IndexReg(IndexCh);
								Diss_endstring();
							}
						}
						return;


						default:
							break;
					}

				}
				break;
			}
		}
		break;
	}

	Diss_strcat("DEFB");
	Diss_WriteHexByte(Opcode);
	Diss_endstring();

}

void	Debug_DissassembleInstruction(int Addr, char *OutputString)
{
	unsigned char Opcode;
	Z80_WORD		DisAddr = Addr;
	int			OpcodeCount;

	pDissassemblyString = OutputString;

	Opcode = Z80_RD_MEM(DisAddr);
	DisAddr++;

	OpcodeCount = Debug_GetOpcodeCount(Addr);

	if (OpcodeCount==1)
	{
		switch (Opcode)
		{
			case 0x0dd:
			case 0x0fd:
			{
				Diss_strcat("DEFB");
				Diss_space();
				Diss_WriteHexByte(Opcode);
				Diss_endstring();
				return;
			}

			default:
				break;
		}
	}

	switch (Opcode)
	{
		case 0x0cb:
		{
			/* CB prefix */
			Opcode = Z80_RD_MEM(DisAddr);

			if ((Opcode & 0x0c0)==0x00)
			{
				/* 00000rrr - RLC */
				/* 00001rrr - RRC */
				/* 00010rrr - RL */
				/* 00011rrr - RR */
				/* 00100rrr - SLA */
				/* 00101rrr - SRA */
				/* 00110rrr - SLL */
				/* 00111rrr - SRL */
				
				Diss_strcat(CB_ShiftMneumonics[((Opcode>>3) & 0x07)]);
				Diss_space();
				Diss_strcat(RegA[(Opcode & 0x07)]);
				Diss_endstring();
			}
			else
			{
				/* 01bbbrrr - BIT */
				/* 10bbbrrr - RES */
				/* 11bbbrrr - SET */
				int BitIndex = ((Opcode>>3) & 0x07);
			
				Diss_strcat(CB_BitOperationMneumonics[(((Opcode>>6)&3)-1)]);
				Diss_space();
				*pDissassemblyString = BitIndex+'0';
				*pDissassemblyString++;
				Diss_comma();
				Diss_strcat(RegA[Opcode & 0x07]);
				Diss_endstring();
			}
		}
		break;

		case 0x0dd:
		{
			Diss_Index(DisAddr, OutputString,'X');
		}
		break;

		case 0x0ed:
		{
			/* ED prefix */
			Opcode = Z80_RD_MEM(DisAddr);
			DisAddr++;
		
			if ((Opcode & 0x0c0)==0x040)
			{
				switch (Opcode & 0x07)
				{
					case 0:
					{

						/* IN r,(C) - 01rrr000 */
						int RegIndex = ((Opcode>>3) & 0x07);

						if ((Opcode & (0x07<<3))==(6<<3))
						{
							/* IN X,(C) */
							RegIndex = 8;							
						}

						Diss_strcat("IN");
						Diss_space();
						Diss_strcat(RegA[RegIndex]);
						Diss_comma();
						Diss_Port();
						Diss_endstring();
					}
					break;

					case 1:
					{
						/* OUT (C),r - 01rrr001 */
						int RegIndex = ((Opcode>>3) & 0x07);

						if ((Opcode & (0x07<<3))==(6<<3))
						{
							/* OUT (C),0 */
							RegIndex = 9;
						}

						Diss_strcat("OUT");
						Diss_space();
						Diss_Port();
						Diss_comma();
						Diss_strcat(RegA[RegIndex]);
						Diss_endstring();
					}
					break;

					case 2:
					{
						unsigned char *Instruction;

						if ((Opcode & 0x08)!=0)
						{
							/* ADC HL,ss - 01ss1010 */
							Instruction = "ADC";
						}
						else
						{
							/* SBC HL,ss - 01ss0010 */
							Instruction = "SBC";
						}

						Diss_strcat(Instruction);
						Diss_space();
						Diss_strcat(RegB[2]);
						Diss_comma();
						Diss_strcat(RegB[((Opcode>>4) & 0x03)]);
						Diss_endstring();
					}
					break;

					case 3:
					{
						/* LD dd,(nn) - 01dd1011 */
						/* LD (nn),dd - 01dd0011 */

						Diss_strcat("LD");
						Diss_space();

						if ((Opcode & 0x08)!=0)
						{
							Diss_strcat(RegB[((Opcode>>4) & 0x03)]);
							Diss_comma();
							Diss_ContentsOfAddress(DisAddr);
							Diss_endstring();
						}
						else
						{
							Diss_ContentsOfAddress(DisAddr);
							Diss_comma();
							Diss_strcat(RegB[((Opcode>>4) & 0x03)]);
						}
						Diss_endstring();

					}
					break;

					case 4:
					{
						/* NEG - 01xxx100 */
						Diss_strcat("NEG");
						Diss_endstring();
					}
					break;

					case 5:
					{
						/* RETI - 01xx1010 */
						/* RETN - 01xx0010 */
						if ((Opcode & 0x08)!=0)
						{
							Diss_strcat("RETI");
							Diss_endstring();
						}
						else
						{
							Diss_strcat("RETN");
							Diss_endstring();
						}
					}
					break;

					case 6:
					{
						unsigned char IM_Type = 0;

						/* IM 0 - 01x00110 */
						/* IM ? - 01x01110 */
						/* IM 1 - 01x10110 */
						/* IM 2 - 01x11110 */

						switch ((Opcode>>3) & 0x03)
						{
							default:
							case 0:
							{
								IM_Type = '0';
							}
							break;

							case 1:
							{
								IM_Type = '?';
							}
							break;

							case 2:
							{
								IM_Type = '1';
							}
							break;

							case 3:
							{
								IM_Type = '2';
							}
							break;
						}

						Diss_strcat("IM");
						Diss_space();
						pDissassemblyString[0] = IM_Type;
						pDissassemblyString++;
						Diss_endstring();
					}
					break;

					case 7:
					{
						/* 01000111 - LD I,A */
						/* 01001111 - LD R,A */
						/* 01010111 - LD A,I */
						/* 01011111 - LD A,R */

						/* 01101111 - RLD */
						/* 01100111 - RRD */
						/* 01110111 - ED NOP */
						/* 01111111 - ED NOP */
						sprintf(OutputString,"%s",
							MiscMneumonics4[((Opcode>>3) & 0x07)]);


					}
					break;


				}

			}
			else if ((Opcode & 0x0e4)==0x0a0)
			{

				switch (Opcode & 0x03)
				{
					case 0:
					{
						/* 10100000 - LDI */
						/* 10101000 - LDD */
						/* 10110000 - LDIR */
						/* 10111000 - LDDR */

						sprintf(OutputString,"%s",
							MiscMneumonics5[((Opcode>>3) & 0x03)]);

					}
					break;
					
					case 1:
					{
						/* 10100001 - CPI */
						/* 10111001 - CPDR */
						/* 10110001 - CPIR */
						/* 10101001 - CPD */

						sprintf(OutputString,"%s",
							MiscMneumonics6[((Opcode>>3) & 0x03)]);

					}
					break;

					case 2:
					{
						/* 10100010 - INI */
						/* 10110010 - INIR */
						/* 10101010 - IND */
						/* 10111010 - INDR */
						sprintf(OutputString,"%s",
							MiscMneumonics7[((Opcode>>3) & 0x03)]);

					}
					break;

					case 3:
					{
						/* 10100011 - OUTI */
						/* 10110011 - OTIR */
						/* 10101011 - outd */
						/* 10111011 - otdr */

						sprintf(OutputString,"%s",
							MiscMneumonics8[((Opcode>>3) & 0x03)]);

					}
					break;

				}
			}
			else
			{
				Diss_strcat("DEFB");
				Diss_space();
				Diss_WriteHexByte(0x0ed);
				Diss_comma();
				Diss_WriteHexByte(Opcode);
				Diss_endstring();
			}
		}
		break;

		case 0x0fd:
		{

			Diss_Index(DisAddr,OutputString,'Y');
		}
		break;

		default:
		{
			switch (Opcode & 0x0c0)
			{
				case 0x000:
				{
					switch (Opcode & 0x07)
					{
						case 0:
						{
							if ((Opcode & 0x020)!=0)
							{
								/* 001cc000 - JR cc */
				
								Diss_strcat("JR");
								Diss_space();
								Diss_strcat(ConditionCodes[((Opcode>>3) & 0x03)]);
								Diss_comma();
								Diss_WriteHexWord(Diss_GetRelativeAddr(DisAddr));
								Diss_endstring();
							}
							else
							{
						
								if ((Opcode & 0x010)!=0)
								{
									/* 00010000 - DJNZ */
									/* 00011000 - JR */

									unsigned char *Instruction;

									if (Opcode==0x010)
									{
										Instruction = "DJNZ";
									}
									else
									{
										Instruction = "JR";
									}

									Diss_strcat(Instruction);
									Diss_space();
									Diss_WriteHexWord(Diss_GetRelativeAddr(DisAddr));
									Diss_endstring();
								}
								else
								{
									/* 00000000 - NOP */
									/* 00001000 - EX AF,AF */
									sprintf(OutputString,"%s",
										MiscMneumonics2[((Opcode>>3) & 0x07)]);
								}

							}
						}
						break;

						case 1:
						{
							if ((Opcode & 0x08)!=0)
							{
								/* 00ss1001 - ADD HL,ss */
								sprintf(OutputString,"ADD HL,%s",
									RegB[((Opcode>>4) & 0x03)]);
							}
							else
							{
								/* 00dd0001 - LD dd,nn */
								Diss_strcat("LD");
								Diss_space();
								Diss_strcat(RegB[((Opcode>>4) & 0x03)]);
								Diss_comma();
								Diss_WriteHexWord(Diss_GetWord(DisAddr));
								Diss_endstring();
							}
						}
						break;

						case 2:
						{
							switch ((Opcode>>4) & 0x03)
							{
								case 0:
								{
									/* 00000010 - LD (BC),A */
									/* 00001010 - LD A,(BC) */

									if ((Opcode & (1<<3))!=0)
									{
										sprintf(OutputString,"LD A,(BC)");
									}
									else
									{
										sprintf(OutputString,"LD (BC),A");
									}
							
								}
								break;

								case 1:
								{
									/* 00010010 - LD (DE),A */
									/* 00011010 - LD A,(DE) */
				
									if ((Opcode & (1<<3))!=0)
									{
										sprintf(OutputString,"LD A,(DE)");
									}
									else
									{
										sprintf(OutputString,"LD (DE),A");
									}
								}
								break;

								case 2:
								{
									/* 00100010 - LD (nnnn),HL */
									/* 00101010 - LD HL,(nn) */
				
									Diss_strcat("LD");
									Diss_space();

									if ((Opcode & (1<<3))==0)
									{
										Diss_ContentsOfAddress(DisAddr);
										Diss_comma();
										Diss_strcat("HL");
									}
									else
									{
										Diss_strcat("HL");
										Diss_comma();
										Diss_ContentsOfAddress(DisAddr);
									}
									Diss_endstring();
								}
								break;

								case 3:
								{
									/* 00110010 - LD (nnnn),A */
									/* 00111010 - LD A,(nnnn) */

									Diss_strcat("LD");
									Diss_space();
									
									if ((Opcode & (1<<3))==0)
									{
										Diss_ContentsOfAddress(DisAddr);
										Diss_comma();
										Diss_strcat("A");
									}
									else
									{
										Diss_strcat("A");
										Diss_comma();
										Diss_ContentsOfAddress(DisAddr);
									}
									Diss_endstring();
								}
								break;
							}

						}
						break;

						case 3:
						{
							unsigned char *Instruction;

							if ((Opcode & 0x08)==0)
							{
								/* 00ss0011 - INC ss */
								Instruction = "INC";
							}
							else
							{
								/* 00ss1011 - DEC ss */
								Instruction = "DEC";
							}

							Diss_strcat(Instruction);
							Diss_space();
							Diss_strcat_only(RegB[((Opcode>>4) & 0x03)]);
						}
						break;

						case 4:
						{
							/* 00rrr100 - INC r */
							sprintf(OutputString,"INC %s",
								RegA[((Opcode>>3) & 0x07)]);
							
						}
						break;

						case 5:
						{
							/* 00rrr101 - DEC r */
							sprintf(OutputString,"DEC %s",
								RegA[((Opcode>>3) & 0x07)]);
							
						}
						break;

						case 6:
						{
							unsigned char Data = Z80_RD_MEM(DisAddr);
							/* LD r,n - 00rrr110 */
							Diss_strcat("LD");
							Diss_space();
							Diss_strcat(RegA[(Opcode>>3) & 0x07]);
							Diss_comma();
							Diss_WriteHexByte(Data);
							Diss_endstring();

						}
						break;

						case 7:
						{

							const unsigned char **MneumonicsList;

							if ((Opcode & (1<<5))==0)
							{
								/* 00000111 - RLCA */
								/* 00001111 - RRCA */
								/* 00010111 - RLA */
								/* 00011111 - RRA */
								MneumonicsList = ShiftMneumonics;
							}
							else
							{
								/* 00100111 - DAA */
								/* 00101111 - CPL */
								/* 00110111 - SCF */
								/* 00111111 - CCF */
								MneumonicsList = MiscMneumonics1;
							}

							Diss_strcat_only(MneumonicsList[((Opcode>>3) & 0x03)]);
						}
						break;
					}
				}
				break;

				case 0x040:
				{
					/* 01xxxxxx */
					/* HALT, LD r,R */

					if (Opcode==0x076)
					{
						Diss_strcat_only("HALT");
					}
					else
					{
						/* 01rrrRRR - LD r,R */
						Diss_strcat("LD");
						Diss_space();
						Diss_strcat(RegA[((Opcode>>3) & 0x07)]);
						Diss_comma();
						Diss_strcat(RegA[(Opcode & 0x07)]);
						Diss_endstring();
					}
				}
				break;

				case 0x080:
				{
					/* 10xxxxxx */
					/* 10000rrr - ADD */
					/* 10001rrr - ADC */
					/* 10010rrr - SUB */
					/* 10011rrr - SBC */
					/* 10100rrr - AND */
					/* 10101rrr - XOR */
					/* 10110rrr - OR */
					/* 10111rrr - CP */
					Diss_strcat(ArithmeticMneumonics[((Opcode>>3) & 0x07)]);
					Diss_space();
					Diss_strcat(RegA[7]);
					Diss_comma();
					Diss_strcat(RegA[Opcode & 0x07]);
					Diss_endstring();
				}
				break;

				case 0x0c0:
				{
					/* 11xxxxxx */
			
			
					switch (Opcode & 0x07)
					{

						case 0:
						{
							/* 11 ccc 000 - RET cc */
							sprintf(OutputString,"RET %s",
								ConditionCodes[((Opcode>>3) & 0x07)]);

						}
						break;

						case 1:
						{
							if ((Opcode & (1<<3))==0)
							{
								/* 11qq0001 - POP qq */
								sprintf(OutputString,"POP %s",
									RegC[((Opcode>>4) & 0x03)]);
							}
							else
							{	
								/* 11001001 - RET */
								/* 11011001 - EXX */
								/* 11101001 - JP (HL) */
								/* 11111001 - LD SP,HL */

								sprintf(OutputString,
									MiscMneumonics3[((Opcode>>4) & 0x03)]);
							}
						}
						break;
				
						case 2:
						{
							/* 11 ccc 010 - JP cc,nnnn */
						
							Diss_strcat("JP");
							Diss_space();
							Diss_strcat(ConditionCodes[((Opcode>>3) & 0x07)]);
							Diss_comma();
							Diss_WriteHexWord(Diss_GetWord(DisAddr));
							Diss_endstring();
						}
						break;

						case 3:
						{
							/* 11001011 - CB prefix */
							if ((Opcode & (3<<4))==(1<<4))
							{
								unsigned char PortByte;

								PortByte = Z80_RD_MEM(DisAddr);

								/* 11011011 - IN A,(n) */
								/* 11010011 - OUT (n),A */

								if ((Opcode & (1<<3))!=0)
								{
									/* 11011011 - IN A,(n) */
								
									Diss_strcat("IN");
									Diss_space();
									Diss_strcat("A");
									Diss_comma();
									Diss_WriteHexByte(PortByte);
									Diss_endstring();
								}
								else
								{
									/* 11010011 - OUT (n),A */
									Diss_strcat("OUT");
									Diss_space();
									Diss_WriteHexByte(PortByte);
									Diss_comma();
									Diss_strcat("A");
									Diss_endstring();
								}
		
							}	
							else if (Opcode == 0x0c3) 
							{
								/* 11000011 - JP nn */
								Diss_strcat("JP");
								Diss_space();
								Diss_WriteHexWord(Diss_GetWord(DisAddr));
								Diss_endstring();
								
							}
							else
							{
								/* 11100011 - EX (SP).HL */
								/* 11101011 - EX DE,HL */
							
								/* 11110011 - DI */
								/* 11111011 - EI */

								sprintf(OutputString,"%s",
									MiscMneumonics9[(((Opcode>>3) & 0x07)-4)]);
							}

						}
						break;

						case 4:
						{
							/* 11 ccc 100 - CALL cc,nnnn */
							Diss_strcat("CALL");
							Diss_space();
							Diss_strcat(ConditionCodes[((Opcode>>3) & 0x07)]);
							Diss_comma();
							Diss_WriteHexWord(Diss_GetWord(DisAddr));
							Diss_endstring();
			
						}
						break;


						case 5:
						{
							/* 11qq0101 - PUSH qq */
							if ((Opcode & (1<<3))==0)
							{
								/* 11qq0101 - PUSH qq */
								sprintf(OutputString,"PUSH %s",
									RegC[((Opcode>>4) & 0x03)]);

							}
							else
							{
								/* 11001101 - CALL nn */
								/* 11011101 - DD */
								/* 11101101 - ED */
								/* 11111101 - FD */

								Diss_strcat("CALL");
								Diss_space();
								Diss_WriteHexWord(Diss_GetWord(DisAddr));
								Diss_endstring();
							}
						}
						break;

						case 6:
						{
							/* 11000110 - ADD n */
							/* 11001110 - ADC n */
							/* 11010110 - SUB n */
							/* 11011110 - SBC n */
							/* 10100110 - AND */
							/* 10101110 - XOR */
							/* 10110110 - OR */
							/* 10111110 - CP */
							unsigned char Data = Z80_RD_MEM(DisAddr);

							Diss_strcat(ArithmeticMneumonics[((Opcode>>3) & 0x07)]);
							Diss_space();
							Diss_strcat(RegA[7]);
							Diss_comma();
							Diss_WriteHexByte(Data);
							Diss_endstring();
						}
						break;

						case 7:
						{
							int Addr;

							Addr = Opcode & 0x038;

							Diss_strcat("RST");
							Diss_space();
							Diss_WriteHexWord(Addr);
							Diss_endstring();
						}
						break;
					}

				}
				break;
			}
		}
		break;
	}
}



/* return number of extra bytes required including prefix */
int	Diss_Index_Get8BitRegCount(unsigned char OpcodeIndex)
{
	/* High or Low byte of Index register e.g. HIX */
	if ((OpcodeIndex == 4) || (OpcodeIndex==5))
		return 1;

	/* Index register with offset e.g. (IX+dd) */
	if (OpcodeIndex == 6)
		return 2;

	/* not index register */
	return 0;
}


int	Diss_Index_GetOpcodeCountForInstruction(int DisAddr)
{
	unsigned char Opcode;

	/* DD prefix - IX */
	Opcode = Z80_RD_MEM(DisAddr);
	DisAddr++;

	switch (Opcode)
	{
		case 0x0dd:
		case 0x0fd:
		case 0x0ed:
			return 1;
		
		case 0x0cb:
			return 4;
		
		default:
		{
			switch (Opcode & 0x0c0)
			{
				case 0x000:
				{
					switch (Opcode & 0x07)
					{
						case 1:
						{
							if ((Opcode & 0x08)!=0)
							{
								/* 00ss1001 - ADD HL,ss */
								return 2;
							}
							else
							{
								/* 00dd0001 - LD dd,nn */
								if (Diss_Index_IsRegIndex((Opcode>>4) & 0x03))
								{
									return 4;
								}
							}
						}
						break;

						case 2:
						{
							/* 00100010 - LD (nnnn),HL */
							/* 00101010 - LD HL,(nn) */
				
							switch ((Opcode>>4) & 0x03)
							{
								case 2:
									return 4;

								default:
									break;
							}

						}
						break;

						case 3:
						{
							if (Diss_Index_IsRegIndex((Opcode>>4) & 0x03))
								return 2;
						}
						break;

						case 4:
						case 5:
						{
							/* 00rrr100 - INC r */
							/* 00rrr101 - DEC r */
							unsigned char OpcodeIndex;

							OpcodeIndex = ((Opcode>>3) & 0x07);

							if (Diss_Index_IsReg8Bit(OpcodeIndex))
								return Diss_Index_Get8BitRegCount(OpcodeIndex) + 1;
						}
						break;

						case 6:
						{
							/* LD r,n - 00rrr110 */
							unsigned char OpcodeIndex;
							
							OpcodeIndex = (Opcode>>3) & 0x07;

							if (Diss_Index_IsReg8Bit(OpcodeIndex))
								return Diss_Index_Get8BitRegCount(OpcodeIndex) + 2;
						}
						break;

						default:
							break;
					}
				}
				break;

				case 0x040:
				{
					/* 01xxxxxx */
					/* HALT, LD r,R */

					if (Opcode!=0x076)
					{
						/* 01rrrRRR - LD r,R */

						unsigned char Reg1, Reg2;
						
						Reg1 = (Opcode>>3) & 0x07;
						Reg2 = Opcode & 0x07;

						if (Diss_Index_IsReg8Bit(Reg1) ||
							Diss_Index_IsReg8Bit(Reg2))
						{
							/* Reg1 or Reg2 is Indexed */
							unsigned char Length1, Length2;

							/* get longest length and return that */
							Length1 = Diss_Index_Get8BitRegCount(Reg1);
							Length2 = Diss_Index_Get8BitRegCount(Reg2);

							if (Length1<Length2)
							{
								return Length2 + 1;
							}
							
							return Length1 + 1;
						}
					}
				}
				break;

				case 0x080:
				{
					/* 10xxxxxx */
					/* 10000rrr - ADD */
					/* 10001rrr - ADC */
					/* 10010rrr - SUB */
					/* 10011rrr - SBC */
					/* 10100rrr - AND */
					/* 10101rrr - XOR */
					/* 10110rrr - OR */
					/* 10111rrr - CP */
					
					unsigned char OpcodeIndex;

					OpcodeIndex = (Opcode & 0x07);

					if (Diss_Index_IsReg8Bit(OpcodeIndex))
						return Diss_Index_Get8BitRegCount(OpcodeIndex) + 1;
				}
				break;

				case 0x0c0:
				{
					/* 11xxxxxx */
			
			
					switch (Opcode & 0x07)
					{
						case 1:
						{
							if ((Opcode & (1<<3))==0)
							{
								/* 11qq0001 - POP qq */
								if (Diss_Index_IsRegIndex((Opcode>>4) & 0x03))
									return 2;
							}
							else
							{	
								/* 11001001 - RET */
								/* 11011001 - EXX */
								/* 11101001 - JP (HL) */
								/* 11111001 - LD SP,HL */

								if ((Opcode==0x0f9) || (Opcode==0x0e9))
									return 2;

							}
						}
						break;
				
						case 3:
						{
							/* 11001011 - CB prefix */
							if ((Opcode & (3<<4))==(1<<4))
							{
		
							}	
							else if (Opcode == 0x0c3) 
							{
							}
							else
							{
								/* 11100011 - EX (SP).HL */
								/* 11101011 - EX DE,HL */
							
								/* 11110011 - DI */
								/* 11111011 - EI */

								if (Opcode==0x0e3)
									return 2;

							}

						}
						break;


						case 5:
						{
							/* 11qq0101 - PUSH qq */
							if ((Opcode & (1<<3))==0)
							{
								/* 11qq0101 - PUSH qq */
								if (Diss_Index_IsRegIndex((Opcode>>4) & 0x03))
									return 2;
							}
						}
						break;

						default:
							break;
					}
				}
				break;
			}
		}
		break;
	}

	/* default is 1, which is the Index prefix */
	return 1;
}


int	Debug_GetOpcodeCount(int Addr)
{
	unsigned char Opcode;
	Z80_WORD		DisAddr = Addr;

	Opcode = Z80_RD_MEM(DisAddr);
	DisAddr++;

	switch (Opcode)
	{
		case 0x0cb:
			/* 00000rrr - RLC */
			/* 00001rrr - RRC */
			/* 00010rrr - RL */
			/* 00011rrr - RR */
			/* 00100rrr - SLA */
			/* 00101rrr - SRA */
			/* 00110rrr - SLL */
			/* 00111rrr - SRL */
			/* 01bbbrrr - BIT */
			/* 10bbbrrr - RES */
			/* 11bbbrrr - SET */
			return 2;

		case 0x0ed:
		{
			/* ED prefix */
			Opcode = Z80_RD_MEM(DisAddr);
			DisAddr++;
		
			if ((Opcode & 0x0c0)==0x040)
			{
				switch (Opcode & 0x07)
				{
					case 0:
					case 1:
					case 2:
						/* IN r,(C) - 01rrr000 */
						/* OUT (C),r - 01rrr001 */
						/* ADC HL,ss - 01ss1010 */
						/* SBC HL,ss - 01ss0010 */
						return 2;
				
					case 3:
						/* LD dd,(nn) - 01dd1011 */
						/* LD (nn),dd - 01dd0011 */
						return 4;

					case 4:
					case 5:
					case 6:
					case 7:
						/* NEG - 01xxx100 */
						/* RETI - 01xx1010 */
						/* RETN - 01xx0010 */
						/* IM 0 - 01x00110 */
						/* IM ? - 01x01110 */
						/* IM 1 - 01x10110 */
						/* IM 2 - 01x11110 */
						return 2;
				}

			}
			else if ((Opcode & 0x0e4)==0x0a0)
			{

				switch (Opcode & 0x03)
				{
					case 0:
					case 1:
					case 2:
					case 3:
						/* 10100000 - LDI */
						/* 10101000 - LDD */
						/* 10110000 - LDIR */
						/* 10111000 - LDDR */
						/* 10100001 - CPI */
						/* 10111001 - CPDR */
						/* 10110001 - CPIR */
						/* 10101001 - CPD */
						/* 10100010 - INI */
						/* 10110010 - INIR */
						/* 10101010 - IND */
						/* 10111010 - INDR */
						/* 10100011 - OUTI */
						/* 10110011 - OTIR */
						/* 10101011 - outd */
						/* 10111011 - otdr */
						return 2;
				}
			}
			else
			{
				return 2;
			}
		}
		break;

		case 0x0dd:
		case 0x0fd:
			return Diss_Index_GetOpcodeCountForInstruction(DisAddr);

		default:
		{
			switch (Opcode & 0x0c0)
			{
				case 0x000:
				{
					switch (Opcode & 0x07)
					{
						case 0:
						{
							if ((Opcode & 0x020)!=0)
							{
								/* 001cc000 - JR cc */
								return 2;
							}
							else
							{
						
								if ((Opcode & 0x010)!=0)
								{
									/* 00010000 - DJNZ */
									/* 00011000 - JR */
									return 2;
								}
								else
								{
									return 1;
								}
							}
							
						}
						break;

						case 1:
						{
							if ((Opcode & 0x08)!=0)
							{
								/* 00ss1001 - ADD HL,ss */
								return 1;
							}

							/* 00dd0001 - LD dd,nn */
							return 3;
						}

						case 2:
						{
							switch ((Opcode>>4) & 0x03)
							{
								case 0:
								case 1:
									/* 00000010 - LD (BC),A */
									/* 00001010 - LD A,(BC) */
									/* 00010010 - LD (DE),A */
									/* 00011010 - LD A,(DE) */
									return 1;

								case 2:
								case 3:
									/* 00100010 - LD (nnnn),HL */
									/* 00101010 - LD HL,(nn) */
									/* 00110010 - LD (nnnn),A */
									/* 00111010 - LD A,(nnnn) */
									return 3;
							}

						}
						break;

						case 3:
						case 4:
						case 5:
							/* 00ss0011 - INC ss */
							/* 00ss1011 - DEC ss */
							/* 00rrr100 - INC r */
							/* 00rrr101 - DEC r */
							return 1;

						case 6:
							/* LD r,n - 00rrr110 */
							return 2;

						case 7:
							/* 00000111 - RLCA */
							/* 00001111 - RRCA */
							/* 00010111 - RLA */
							/* 00011111 - RRA */
							/* 00100111 - DAA */
							/* 00101111 - CPL */
							/* 00110111 - SCF */
							/* 00111111 - CCF */
							return 1;
					}
				}
				break;

				case 0x040:
					/* 01xxxxxx */
					/* 01110110 - HALT*/
					/* 01rrrRRR - LD r,R */
					return 1;

				case 0x080:
					/* 10xxxxxx */
					/* 10000rrr - ADD */
					/* 10001rrr - ADC */
					/* 10010rrr - SUB */
					/* 10011rrr - SBC */
					/* 10100rrr - AND */
					/* 10101rrr - XOR */
					/* 10110rrr - OR */
					/* 10111rrr - CP */
					return 1;

				case 0x0c0:
				{
					/* 11xxxxxx */

					/* 11110011 - DI */
					/* 11111011 - EI */
					/* 11101011 - EX DE,HL */
					/* 11011011 - IN A,(n) */
					/* 11010011 - OUT (n),A */
					/* 11000011 - JP */
					/* 11100011 - EX (SP).HL */

			
					switch (Opcode & 0x07)
					{

						case 0:
							/* 11 ccc 000 - RET cc */
							return 1;

						case 1:
						{
							if ((Opcode & (1<<3))==0)
							{
								/* 11qq0001 - POP qq */
								return 1;
							}
							else
							{	
								/* 11001001 - RET */
								/* 11011001 - EXX */
								/* 11101001 - JP (HL) */
								/* 11111001 - LD SP,HL */

								return 1;
							}
						}
						break;
				
						case 2:
						case 4:
							/* 11 ccc 010 - JP cc,nnnn */
							/* 11 ccc 100 - CALL cc,nnnn */
							return 3;

						case 3:
						{
							if ((Opcode & (3<<4))==(1<<4))
							{
								/* 11011011 - IN A,(n) */
								/* 11010011 - OUT (n),A */
								return 2;
							}
							else if (Opcode == 0x0c3)
							{	
								return 3;
							}
							else
							{
								return 1;
							}
						}
						break;					

						case 5:
						{
							if ((Opcode & (1<<3))==0)
							{
								/* 11qq0101 - PUSH qq */
								return 1;
							}
							else
							{
								/* 11001101 - CALL nn */
								/* 11011101 - DD */
								/* 11101101 - ED */
								/* 11111101 - FD */
								return 3;
							}
						}
						break;

						case 6:
							/* 11000110 - ADD n */
							/* 11001110 - ADC n */
							/* 11010110 - SUB n */
							/* 11011110 - SBC n */
							/* 10100110 - AND */
							/* 10101110 - XOR */
							/* 10110110 - OR */
							/* 10111110 - CP */
							return 2;

						case 7:
							/* 11ttt111 - RST */
							return 1;
					}

				}
				break;
			}
		}
		break;
	}

	return 0;
}



int	Z80_GetRIncrementForInstruction(Z80_WORD DisAddr)
{
	unsigned char Opcode;

	Opcode = Z80_RD_MEM(DisAddr);

	switch (Opcode)
	{
		case 0x0ed:
			return 2;

		case 0x0cb:
			return 2;

		case 0x0fd:
		case 0x0dd:
		{
			DisAddr++;
			Opcode = Z80_RD_MEM(DisAddr);

			switch (Opcode)
			{
				case 0x0dd:
				case 0x0fd:
				case 0x0ed:
					return 1;
			
				break;
			}
		}
		return 2;

		default:
			break;
	}

	return 1;
}





#if 0
char    *Debug_DumpFromAddress(int Addr, int NoOfBytes, int (*pReadFunction)(int))
{
        int i;

        /* show memory address */
        sprintf(MemDumpString,"%04x: ",Addr & 0x0ffff);
                                
        /* show bytes */
        for (i=0; i<NoOfBytes; i++)
        {
                sprintf(MemDumpString+6+(i*3),"%02x ",(unsigned char)pReadFunction(Addr + i));
        }

        /* bytes seperating text dump */
        sprintf(MemDumpString+6+(NoOfBytes*3),"  ");

        /* dump text */
        for (i=0; i<NoOfBytes; i++)
        {
                unsigned char   ch;

                ch = (unsigned char)pReadFunction(Addr + i);

                /* convert un-printable chars into '.' */
                if ((ch<21) || (ch>127))
                        ch = '.';

                sprintf(MemDumpString+6+(NoOfBytes*3)+2 + i,"%c",ch);
        }

        /* return mem-dump string */
        return MemDumpString;
}
#endif

void    Debug_WriteMemoryToDisk(char *Filename)
{
        char    *pBuffer;
        FILE    *fh;

        pBuffer = (char *)malloc(65536);

        if (pBuffer!=NULL)
        {
                CPC_DumpRamToBuffer(pBuffer);

                fh = fopen(Filename,"wb");
                
                if (fh!=NULL)
                {
                        fwrite(pBuffer,65536,1,fh);
                        fclose(fh);
                }

                free(pBuffer);
        }
}

void    Debug_WriteBaseMemoryToDisk(char *Filename)
{
        char    *pBuffer;
        FILE    *fh;

        pBuffer = (char *)malloc(65536);

        if (pBuffer!=NULL)
        {
                CPC_DumpBaseRamToBuffer(pBuffer);

                fh = fopen(Filename,"wb");
                
                if (fh!=NULL)
                {
                        fwrite(pBuffer,65536,1,fh);
                        fclose(fh);
                }

                free(pBuffer);
        }
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


/**************************************/        
/* ASIC CPC+ DMA					  */
/* dissassemble instruction to buffer */

int		ASIC_DMA_GetOpcodeCount(int Address)
{
	return 2;
}


void	ASIC_DMA_DissassembleInstruction(int Address, char *DebugString)
{
		Z80_WORD        Command;
        int     CommandOpcode;

		pDissassemblyString = DebugString;

		/* make it even */
		Address = Address & (0x0ffff^1);

        /* fetch command */
        Command = Z80_RD_BASE_WORD(Address);
		
        /* get opcode */
        CommandOpcode = (Command>>12) & 0x07;
		
		/* dissassemble */
		if (CommandOpcode == 0x0)
		{
			/* LOAD R,D */
            unsigned long Register,Data;

            Register = (Command>>8) & 0x0f;
			
            Data = Command & 0x0ff;

			Diss_strcat("LOAD");
			Diss_space();
			Diss_WriteHexByte(Register);
			Diss_comma();
			Diss_WriteHexByte(Data);

		}
		else
		{
			/* PAUSE? */
			 if ((CommandOpcode & (1<<0))!=0)
		     {
                /* PAUSE n */

				unsigned long PauseCount = Command & 0x0fff;

				Diss_strcat("PAUSE");
				Diss_space();
				Diss_WriteHexWord(PauseCount);

				/* clear bit */
				CommandOpcode &=~(1<<0);

				/* any other bits set? */
				if (CommandOpcode!=0)
				{
					Diss_colon();
				}
			 }



			/* REPEAT? */
             if ((CommandOpcode & (1<<1))!=0)
             {
                 /* REPEAT n */

				 unsigned long RepeatCount = Command & 0x0fff;

				 Diss_strcat("REPEAT");
				 Diss_space();
				 Diss_WriteHexWord(RepeatCount);
				 CommandOpcode &=~(1<<1);

	 			 /* any other bits set? */
				 if (CommandOpcode!=0)
				 {
					 Diss_colon();
				 }
			 }


			/* NOP, LOOP, INT or STOP? */
            if ((CommandOpcode & (1<<2))!=0)
            {
				Command &=0x01 | 0x0010 | 0x0020;
             
				if (Command == 0)
				{
					Diss_strcat("NOP");
				}
				else
				{

					/* NOP, LOOP, INT, STOP */
					if (Command & 0x0001)
					{
						/* LOOP */

						Diss_strcat("LOOP");

						Command &=~0x0001;

       					if (Command!=0)
						{
							Diss_colon();
						}
					}


					if (Command & 0x0010)
					{
						/* INT */

						Diss_strcat("INT");

						Command &=~0x0010;

						if (Command !=0)
						{
							Diss_colon();
						}
					}



					if (Command & 0x0020)
					{
						/* STOP */
						Diss_strcat("STOP");
					}
				}
            }
   		}

	Diss_endstring();
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
