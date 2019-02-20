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
/* THIS IS NOT COMPLETE AND DOESN'T WORK */
#include <stdio.h>
#include <stdlib.h>

typedef unsigned short Z80_WORD;
typedef unsigned char  Z80_BYTE;
typedef int BOOL;

#ifndef TRUE
#define TRUE (1==1)
#endif

#ifndef FALSE
#define FALSE (1==0)
#endif

unsigned char Memory[256];

Z80_BYTE	Z80_RD_MEM(Z80_WORD Addr)
{
	return Memory[Addr];
}


/* returns the R register increment for instruction at DisAddr */
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


int	Z80_Index_GetOpcodeCountForInstruction(int DisAddr)
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

//								sprintf(OutputString,
//									MiscMneumonics3[((Opcode>>4) & 0x03)]);
							}
						}
						break;
				
						case 3:
						{
							/* 11001011 - CB prefix */
							if ((Opcode & (3<<4))==(1<<4))
							{
//								unsigned char PortByte;
//
//								PortByte = Z80_RD_MEM(DisAddr);
//
//								/* 11011011 - IN A,(n) */
//								/* 11010011 - OUT (n),A */
//
//								if ((Opcode & (1<<4))!=0)
//								{
//									/* 11011011 - IN A,(n) */
//								
//									sprintf(OutputString,"IN A,(#%02x)",
//										PortByte);
//								}
//								else
//								{
//									/* 11010011 - OUT (n),A */
//									sprintf(OutputString,"OUT (#%02x),A",
//										PortByte);
//								}
		
							}	
							else if (Opcode == 0x0c3) 
							{
//								/* 11000011 - JP nn */
//								sprintf(OutputString,"JP #%04x",
//									Diss_GetWord(DisAddr));
							}
							else
							{
								/* 11100011 - EX (SP).HL */
								/* 11101011 - EX DE,HL */
							
								/* 11110011 - DI */
								/* 11111011 - EI */

								if (Opcode==0x0e3)
									return 2;

//								sprintf(OutputString,"%s",
//									MiscMneumonics9[(((Opcode>>3) & 0x07)-4)]);
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


int	Z80_GetOpcodeCountForInstruction(int Addr)
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
			return Z80_Index_GetOpcodeCountForInstruction(DisAddr);

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







/* true if Opcode is a instruction prefix */
BOOL	Z80_IsPrefix(Z80_BYTE	Opcode)
{
	if  (
		(Opcode==0x0dd) || 
		(Opcode==0x0fd) || 
		(Opcode==0x0ed) ||
		(Opcode==0x0cb)
		)
	{
		return TRUE;
	}

	return FALSE;
}

static int Z80_GetNopCountForInstruction(Z80_WORD);
static void	Z80_GenerateCode(int Addr, FILE *fh);

void	FetchOpcode(FILE *fh,int Offset)
{
//	if (Offset==0)
//	{
//		fprintf(fh,"Opcode = Z80_RD_BYTE(R.PC.W);\r\n");
		fprintf(fh,"Opcode = Z80_RD_OPCODE_BYTE(%d);\r\n",Offset);
//	}
//	else
//	{
//		fprintf(fh,"Opcode = Z80_RD_BYTE((Z80_WORD)(R.PC.W+%d));\r\n",Offset);
//	}
}

void	DoOpcode(FILE *fh,BOOL IsPrefix)
{
	BOOL doNopCycle;
	BOOL bIncrementPC,bIncrementR;
	BOOL bCheckInterrupt;

	int OpcodeCount;

	OpcodeCount = Z80_GetOpcodeCountForInstruction(0);

	if ((IsPrefix) && (OpcodeCount==1))
	{
		fprintf(fh,"PrefixIgnore();\r\n");
	}
	else
	{

		bIncrementPC = TRUE;
		bIncrementR = TRUE;
		doNopCycle = TRUE;

		if (
			(Memory[0]==0x0cb) || 
			(Memory[0]==0x0ed) ||
			(
				((Memory[0]==0x0dd) || (Memory[0]==0x0fd)) && (Memory[1]==0x0cb)
			)
			)
		{
			bIncrementR = FALSE;
		}


		Z80_GenerateCode(0, fh);


		// RST?
		if ((Memory[0] & 0x0c7)==0x0c7)
		{
			bIncrementPC = FALSE;
		}

		// JP (HL), or JP (IX)
		if (
			(Memory[0]==0x0e9) || 
			( ((Memory[0]==0x0fd) || (Memory[0]==0x0dd)) && (Memory[1]==0x0e9))
			)

		{
			bIncrementPC = FALSE;
		}

		// RETI, RETN
		if (
			(Memory[0]==0x0ed) && 
			(
			((Memory[1]&0x0cf)==0x04d) ||
			((Memory[1]&0x0cf)==0x045)
			)
			)
		{
			bIncrementPC = FALSE;
		}

		if (Memory[0]==0x018)
		{
			bIncrementPC = FALSE;
		}

		/* 00010000 - DJNZ */
		/* 001cc000 - JR cc */

		if (
			(Memory[0]==0x010) ||
			((Memory[0] & 0x0e7)==0x020))
		{
			bIncrementPC = FALSE;
			doNopCycle = FALSE;
		}

		/* 11 ccc 010 - JP cc,nnnn */
		if ((Memory[0] & 0x0c7)==0x0c2)
		{
			bIncrementPC = FALSE;
			doNopCycle = FALSE;
		}

		/* 11 ccc 100 - CALL cc,nnnn */
		/* 11 ccc 000 - RET cc */
		if ((Memory[0] & 0x0c7)==0x0c4)
		{
			bIncrementPC = FALSE;
			doNopCycle = FALSE;
		}

		if ((Memory[0] & 0x0c7)==0x0c0)
		{
			bIncrementPC = FALSE;
			doNopCycle = FALSE;
		}

		if ((Memory[0]==0x0c9) || (Memory[0]==0x0c3) || (Memory[0]==0x0cd))
		{
			bIncrementPC = FALSE;
		}


		if (Memory[0]==0x0ed)
		{
			if (
				(Memory[1]==0x0a2) || (Memory[1]==0x0aa) ||
			 (Memory[1]==0x0a3) || (Memory[1]==0x0ab) ||
			 (Memory[1]==0x0b0) || (Memory[1]==0x0b8) ||
			 (Memory[1]==0x0b1) || (Memory[1]==0x0b2) ||
			 (Memory[1]==0x0b3) || (Memory[1]==0x0b9) ||
			 (Memory[1]==0x0ba) || (Memory[1]==0x0bb))
			{
				doNopCycle = FALSE;
			}

			if ((Memory[1]==0x0b0) || (Memory[1]==0x0b8) ||
				(Memory[1]==0x0b1) || (Memory[1]==0x0b2) ||
				(Memory[1]==0x0b3) || (Memory[1]==0x0b9) ||
				(Memory[1]==0x0ba) || (Memory[1]==0x0bb))
			{
				bIncrementPC = FALSE;


			}
		}

		if (Memory[0]==0x076)
		{
			bIncrementPC = FALSE;
		}

		if ((Memory[0]==0x0f3) || (Memory[0]==0x0fb) || (Memory[0]==0x0cb))
		{
			bCheckInterrupt = FALSE;
		}

		if (Memory[0]==0x0cb)
		{
			bIncrementPC = FALSE;
		}
		
		if (((Memory[0]==0x0dd) || (Memory[0]==0x0fd)) && (Memory[1]==0x0cb))
		{
			bIncrementPC = FALSE;
			bCheckInterrupt = FALSE;
		}

		/******************************/
		/* write R register increment */
		if (bIncrementR)
		{
			int R_Increment;

			/* get R increment for instruction */
			R_Increment = Z80_GetRIncrementForInstruction(0);

			/* write macro name with correct increment */
			fprintf(fh,"INC_REFRESH(%d);\r\n",R_Increment);
		}


		/******************************/
		/* write PC increment */
		if (bIncrementPC)
		{
			int OpcodeCount;

			OpcodeCount = Z80_GetOpcodeCountForInstruction(0);

			fprintf(fh,"ADD_PC(%d);\r\n",OpcodeCount);
		}

		
			if (Memory[0]==0x0ed)
			{
				if ((Memory[1] & 0x0c6)==0x040)
				{
					/* IN r,(C) - 01rrr000 */
					/* OUT (C),r - 01rrr001 */
					doNopCycle = FALSE;
				}
			}

			if ((Memory[0]==0x0d3) || (Memory[0]==0x0db))
			{
				/* 11011011 - IN A,(n) */
				/* 11010011 - OUT (n),A */
				doNopCycle = FALSE;
			}


			if (doNopCycle)
			{
				/******************************/
				/* write NOP cycle timing */
				{
					int NOP_Count;

					NOP_Count = Z80_GetNopCountForInstruction(0);

					fprintf(fh,"Z80_UpdateCycles(%d);\r\n", NOP_Count);
				}
			}

		if (bCheckInterrupt)
		{
			fprintf(fh,"R.Flags |= Z80_CHECK_INTERRUPT_FLAG;\r\n");
		}
	}
}


void	StartFunction(unsigned char *FunctionName, FILE *fh,BOOL Local)
{
	fprintf(fh,"/***************************************************************************/\r\n");
	
	if (Local)
	{
		fprintf(fh,"__inline static ");
	}

	fprintf(fh,"void %s(void)\r\n",FunctionName);
	fprintf(fh,"{\r\n");
	/* write variables that will be used */
	fprintf(fh,"unsigned long Opcode;\r\n");	//Z80_BYTE	Opcode;\r\n");
}

void	EndFunction(FILE *fh)
{

	/* write code to check interrupts */
	fprintf(fh,"}\r\n");
}

void	StartSwitch(FILE *fh,int FetchOffset)
{
	FetchOpcode(fh,FetchOffset);

	fprintf(fh,"Opcode = Opcode & 0x0ff;\r\n");
	/* write switch for first opcode */
	fprintf(fh,"switch (Opcode)\r\n");
	fprintf(fh,"{\r\n");
}

void	EndSwitch(FILE *fh)
{
	fprintf(fh,"default:\r\n");
	fprintf(fh,"/* the following tells MSDEV 6 to not generate */\r\n");
	fprintf(fh,"/* code which checks if a input value to the  */\r\n");
	fprintf(fh,"/* switch is not valid.*/\r\n");
	fprintf(fh,"#ifdef _MSC_VER\r\n");
	fprintf(fh,"#if (_MSC_VER>=1200)\r\n");
	fprintf(fh,"__assume(0);\r\n");
	fprintf(fh,"#endif\r\n");
	fprintf(fh,"#endif\r\n");
	fprintf(fh,"break;\r\n");
	fprintf(fh,"}\r\n");
}

void	BeginCase(FILE *fh,unsigned char Opcode)
{
	/* start case */
	fprintf(fh,"case 0x0%02x:\r\n",Opcode);
	fprintf(fh,"{\r\n");
}

void	BeginCaseNoBracket(FILE *fh, unsigned char Opcode)
{
	fprintf(fh,"case 0x0%02x:\r\n",Opcode);
}

void	EndCase(FILE *fh)
{
	fprintf(fh,"}\r\n");
	fprintf(fh,"break;\r\n");
}

BOOL	OpcodeProcessed[256];
int		NumProcessed = 0;

void	IndexOnlyPrefix(FILE *fh)
{
	int i, Opcode;

		StartSwitch(fh,1);
	

		NumProcessed = 0;
		for (i=0; i<256; i++)
		{
			OpcodeProcessed[i] = FALSE;
		}

		for (i=0; i<256; i++)
		{
			int OpcodeCount;

			Memory[1] = i;

			OpcodeCount = Z80_GetOpcodeCountForInstruction(0);

			if ((OpcodeCount==1))
			{
				OpcodeProcessed[i] = TRUE;
			
				BeginCaseNoBracket(fh,i);
				NumProcessed++;
			}
		}

		if (NumProcessed!=0)
		{
			fprintf(fh,"{\r\n");
			fprintf(fh,"PrefixIgnore();\r\n");
			EndCase(fh);
		}

		for (Opcode = 0; Opcode<256; Opcode++)
		{
			Memory[1] = Opcode;

			if (OpcodeProcessed[Opcode]==FALSE)
			{
				BeginCase(fh,Opcode);
	
				if (Opcode==0x0cb)
				{
					if (Memory[0]==0x0dd)
					{
						fprintf(fh,"Z80_DD_CB_ExecuteInstruction();\r\n");
					}
					else
					{
						fprintf(fh,"Z80_FD_CB_ExecuteInstruction();\r\n");
					}

				}
				else
				{
					DoOpcode(fh,TRUE);
				}	
				EndCase(fh);
			}

		}

		EndSwitch(fh);
		EndFunction(fh);
}

void	main()
{
	int i;
	FILE *fh;

	fh = fopen("z80.c","wb");

	if (fh!=NULL)
	{
		int Opcode;

		fprintf(fh,"#include \"z80.h\"\r\n");
		fprintf(fh,"#include \"z80tables.h\"\r\n");
		fprintf(fh,"#include \"z80macros.h\"\r\n");

		fprintf(fh,"Z80_REGISTERS R;\r\n");

		fprintf(fh,"#include \"z80funcs.h\"\r\n");
		fprintf(fh,"#include \"z80funcs.c\"\r\n");


		/*********************/
		/* FD CB prefix opcodes */
		StartFunction("Z80_FD_CB_ExecuteInstruction",fh,TRUE);
		Memory[0] = 0x0fd;
		Memory[1] = 0x0cb;

		StartSwitch(fh,3);
					
		for (Opcode = 0; Opcode<256; Opcode++)
		{
			Memory[3] = Opcode;

			BeginCase(fh,Opcode);

			DoOpcode(fh,TRUE);

			EndCase(fh);
		}
		EndSwitch(fh);

		/* write macro name with correct increment */
		fprintf(fh,"INC_REFRESH(2);\r\n");
		fprintf(fh,"ADD_PC(4);\r\n");
		fprintf(fh,"R.Flags |= Z80_CHECK_INTERRUPT_FLAG;\r\n");

		EndFunction(fh);


		/*********************/
		/* FD prefix opcodes */
		StartFunction("Z80_FD_ExecuteInstruction",fh,TRUE);
		Memory[0] = 0x0fd;
		
		IndexOnlyPrefix(fh);

		/*********************/
		/* DD CB prefix opcodes */
		StartFunction("Z80_DD_CB_ExecuteInstruction",fh,TRUE);
		Memory[0] = 0x0dd;
		Memory[1] = 0x0cb;
	
		StartSwitch(fh,3);
					
		for (Opcode = 0; Opcode<256; Opcode++)
		{
			Memory[3] = Opcode;

			BeginCase(fh,Opcode);

			DoOpcode(fh,TRUE);

			EndCase(fh);
		}
		EndSwitch(fh);
		/* write macro name with correct increment */
		fprintf(fh,"INC_REFRESH(2);\r\n");
		fprintf(fh,"ADD_PC(4);\r\n");
		fprintf(fh,"R.Flags |= Z80_CHECK_INTERRUPT_FLAG;\r\n");
		EndFunction(fh);

		/*********************/
		/* DD prefix opcodes */
		StartFunction("Z80_DD_ExecuteInstruction",fh,TRUE);
		Memory[0] = 0x0dd;
	
		IndexOnlyPrefix(fh);

		/*********************/
		/* CB prefix opcodes */
		StartFunction("Z80_ED_ExecuteInstruction",fh,TRUE);
		Memory[0] = 0x0ed;

		/* write macro name with correct increment */
		fprintf(fh,"INC_REFRESH(2);\r\n");
		
		StartSwitch(fh,1);
		

		NumProcessed = 0;
		for (i=0; i<256; i++)
		{
			OpcodeProcessed[i] = FALSE;
		}

		for (i=0; i<256; i++)
		{
			Memory[1] = i;

			if ((Memory[1] & 0x0c0)==0x040)
			{
			}
			else if ((Memory[1] & 0x0e4)==0x0a0)
			{
			}
			else
			{
				OpcodeProcessed[i] = TRUE;
			
				BeginCaseNoBracket(fh,i);
				NumProcessed++;
			}
		}

		if (NumProcessed!=0)
		{
			Memory[1] = 0x00;
			fprintf(fh,"{\r\n");
			DoOpcode(fh, TRUE);
			EndCase(fh);
		}

		for (Opcode = 0; Opcode<256; Opcode++)
		{
			Memory[1] = Opcode;

			if (OpcodeProcessed[Opcode]==FALSE)
			{
				BeginCase(fh,Opcode);

				DoOpcode(fh,TRUE);

				EndCase(fh);
			}
		}

		EndSwitch(fh);
		EndFunction(fh);


		/*********************/
		/* CB prefix opcodes */
		StartFunction("Z80_CB_ExecuteInstruction",fh,TRUE);
		Memory[0] = 0x0cb;

		StartSwitch(fh,1);
					
		for (Opcode = 0; Opcode<256; Opcode++)
		{
			Memory[1] = Opcode;

			BeginCase(fh,Opcode);

			DoOpcode(fh,TRUE);

			EndCase(fh);
		}

		EndSwitch(fh);

		/* write macro name with correct increment */
		fprintf(fh,"INC_REFRESH(2);\r\n");
		fprintf(fh,"ADD_PC(2);\r\n");
		fprintf(fh,"R.Flags |= Z80_CHECK_INTERRUPT_FLAG;\r\n");

		EndFunction(fh);

		/*********************/
		/* no prefix opcodes */
		StartFunction("Z80_ExecuteInstruction",fh,FALSE);
		StartSwitch(fh,0);

		/* go through all opcodes */
		for (Opcode=0; Opcode<256; Opcode++)
		{
			Memory[0] = Opcode;
			
			BeginCase(fh,Opcode);
				
			if (Z80_IsPrefix(Opcode))
			{
				if (Opcode==0x0cb)
				{
					fprintf(fh,"Z80_CB_ExecuteInstruction();\r\n");
				}
				else if (Opcode==0x0ed)
				{
					fprintf(fh,"Z80_ED_ExecuteInstruction();\r\n");
				}
				else if (Opcode==0x0dd)
				{
					fprintf(fh,"Z80_DD_ExecuteInstruction();\r\n");
				}
				else if (Opcode==0x0fd)
				{
					fprintf(fh,"Z80_FD_ExecuteInstruction();\r\n");
				}
			}
			else
			{
				DoOpcode(fh,FALSE);
			}

			EndCase(fh);
		}
		EndSwitch(fh);
	fprintf(fh,"/* check interrupts? */\r\n");
	fprintf(fh,"if ((R.Flags & (Z80_CHECK_INTERRUPT_FLAG | Z80_EXECUTE_INTERRUPT_HANDLER_FLAG))==(Z80_CHECK_INTERRUPT_FLAG | Z80_EXECUTE_INTERRUPT_HANDLER_FLAG))\r\n");
	fprintf(fh,"{\r\n");
	fprintf(fh,"Z80_ExecuteInterrupt();\r\n");
	fprintf(fh,"}\r\n");


	

		EndFunction(fh);
	}

	fclose(fh);
}





static int	Z80_Index_GetNopCountForInstruction(Z80_WORD DisAddr,Z80_BYTE Flags)
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
			return 1;
		}
		
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

				return 7;
			}
			else
			{
				/* 01bbbrrr - BIT */
				/* 10bbbrrr - RES */
				/* 11bbbrrr - SET */
				
				if ((Opcode & 0x0c0)==0x040)
				{
					return 6;
				}

				return 7;
			}
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
						case 1:
						{
							if ((Opcode & 0x08)!=0)
							{
								/* 00ss1001 - ADD HL,ss */
								return 4;
							}
							else
							{
								/* 00dd0001 - LD dd,nn */
								return 4;
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
									return 6;
								}
								break;

								default:
									break;
							}


						}
						break;

						case 3:
							/* 00ss0011 - INC ss */
							/* 00ss1011 - DEC ss */
							return 3;
						
						case 4:
						case 5:
						{
							/* 00rrr100 - INC r */
							/* 00rrr101 - DEC r */
							if (((Opcode>>3) & 0x07)==6)
								return 6;
							
							return 2;
		
						}

						case 6:
						{
							/* LD r,n - 00rrr110 */
							if (((Opcode>>3) & 0x07)==6)
								return 6;

							return 3;
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
									
					Reg1 = (Opcode>>3) & 0x07;
					Reg2 = Opcode & 0x07; 

					if ((Reg1==6) || (Reg2==6))
					{
						return 5;
					}
					
					return 2;
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
				
					if ((Opcode & 0x07)==6)
						return 5;

					return 2;
				}

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
								return 4;

							}
							else
							{	
								/* 11001001 - RET */
								/* 11011001 - EXX */
								/* 11101001 - JP (HL) */
								/* 11111001 - LD SP,HL */

								if (Opcode==0x0e9)
									return 2;

								if (Opcode==0x0f9)
									return 3;
							}
						}
						break;
				
						case 3:
						{
							/* 11001011 - CB prefix */
							if ((Opcode & (3<<4))==(1<<4))
							{
								/* 11011011 - IN A,(n) */
								/* 11010011 - OUT (n),A */
		
							}	
							else if (Opcode == 0x0c3) 
							{
								/* 11000011 - JP nn */
							}
							else
							{
								/* 11100011 - EX (SP).HL */
								/* 11101011 - EX DE,HL */
							
								/* 11110011 - DI */
								/* 11111011 - EI */

								if (Opcode==0x0e3)
									return 7;
							}

						}
						break;

						case 5:
						{
							/* 11qq0101 - PUSH qq */
							if ((Opcode & (1<<3))==0)
							{
								return 5;
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

	return -1;
}

/* returns NOP count for whole instruction - values may be different
depending on flags! */
int		Z80_GetNopCountForInstruction(Z80_WORD Addr, Z80_BYTE Flags)
{
	unsigned char Opcode;
	Z80_WORD		DisAddr = Addr;
	
	Opcode = Z80_RD_MEM(DisAddr);
	DisAddr++;

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
				
				/* (HL)? */
				if ((Opcode & 0x07)==6)
					return 4;

				return 2;
			}
			else
			{
				/* 01bbbrrr - BIT */
				/* 10bbbrrr - RES */
				/* 11bbbrrr - SET */
				
				
				/* (HL)? */
				if ((Opcode & 0x07)==6)
				{
					/* BIT? */
					if ((Opcode & 0x0c0)==0x040)
						return 3;

					/* RES/SET */
					return 4;
				}

				return 2;
			}
		}
		break;

		case 0x0fd:
		case 0x0dd:
			return Z80_Index_GetNopCountForInstruction(DisAddr,Flags);
		
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
						/* IN r,(C) - 01rrr000 */
						/* OUT (C),r - 01rrr001 */
						return 4;

					case 2:
						/* ADC HL,ss - 01ss1010 */
						/* SBC HL,ss - 01ss0010 */
						return 4;

					case 3:
						/* LD dd,(nn) - 01dd1011 */
						/* LD (nn),dd - 01dd0011 */
						return 6;

					case 4:
						/* NEG - 01xxx100 */
						return 2;

					case 5:
						/* RETI - 01xx1010 */
						/* RETN - 01xx0010 */
						return 4;
						
					case 6:
						/* IM 0 - 01x00110 */
						/* IM ? - 01x01110 */
						/* IM 1 - 01x10110 */
						/* IM 2 - 01x11110 */
						return 2;

					case 7:
					{
						if ((Opcode & 0x020)==0)
						{
							/* 01000111 - LD I,A */
							/* 01001111 - LD R,A */
							/* 01010111 - LD A,I */
							/* 01011111 - LD A,R */
							return 3;
						}
						
						if ((Opcode & 0x010)==0)
						{
							/* 01101111 - RLD */
							/* 01100111 - RRD */
							return 5;
						}

						/* 01110111 - ED NOP */
						/* 01111111 - ED NOP */

						return 2;
					}


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

						// correct?
						if ((Opcode==0x0a0) || (Opcode==0x0a8))
							return 5;

					}
					break;
					
					case 1:
					{
						/* 10100001 - CPI */
						/* 10111001 - CPDR */
						/* 10110001 - CPIR */
						/* 10101001 - CPD */

						// correct?
						if ((Opcode==0x0a1) || (Opcode==0x0a9))
							return 5;


					}
					break;

					case 2:
					{
						/* 10100010 - INI */
						/* 10110010 - INIR */
						/* 10101010 - IND */
						/* 10111010 - INDR */

						// correct?
						if ((Opcode==0x0a2) || (Opcode==0x0aa))
							return 5;
					}
					break;

					case 3:
					{
						/* 10100011 - OUTI */
						/* 10110011 - OTIR */
						/* 10101011 - outd */
						/* 10111011 - otdr */

						// correct?
						if ((Opcode==0x0a3) || (Opcode==0x0ab))
							return 5;
					}
					break;

				}
			}
			else
				/* ED-NOP */
				return 2;
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
										return 3;
									}

								}
								else
								{
									/* 00000000 - NOP */
									/* 00001000 - EX AF,AF */
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
								return 3;
							}

							/* 00dd0001 - LD dd,nn */
							return 3;
						}
						break;

						case 2:
						{
							switch ((Opcode>>4) & 0x03)
							{
								case 0:
									/* 00000010 - LD (BC),A */
									/* 00001010 - LD A,(BC) */
									return 2;

								case 1:
									/* 00010010 - LD (DE),A */
									/* 00011010 - LD A,(DE) */
									return 2;

								case 2:
									/* 00100010 - LD (nnnn),HL */
									/* 00101010 - LD HL,(nn) */
									return 5;

								case 3:
									return 4;
							}

						}
						break;

						case 3:
							/* 00ss0011 - INC ss */
							/* 00ss1011 - DEC ss */
							return 2;


						case 4:
						case 5:
							/* 00rrr100 - INC r */
							/* 00rrr101 - DEC r */
							if (((Opcode>>3) & 0x07)==6)
								return 2;

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
				{
					/* 01xxxxxx */
					/* HALT, LD r,R */

					/* 01rrrRRR - LD r,R */


					if (Opcode == 0x076)
					{
						return 1;
					}
					else
					{
						unsigned char Reg1,Reg2;

						Reg1 = ((Opcode>>3) & 0x07);
						Reg2 = (Opcode & 0x07);

						if ((Reg1==6) || (Reg2==6))
						{
							return 2;
						}
						
						return 1;
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

					if ((Opcode & 0x07)==6)
						return 2;

					return 1;
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
#if 0
							sprintf(OutputString,"RET %s",
								ConditionCodes[((Opcode>>3) & 0x07)]);
#endif

						}
						break;

						case 1:
						{
							if ((Opcode & (1<<3))==0)
							{
								/* 11qq0001 - POP qq */
								return 3;
							}
							else
							{	
								/* 11001001 - RET */
								/* 11011001 - EXX */
								/* 11101001 - JP (HL) */
								/* 11111001 - LD SP,HL */

								switch (Opcode)
								{
									case 0x0c9:
										return 3;

									case 0x0d9:
										return 1;
									
									case 0x0e9:
										return 1;

									case 0x0f9:
										return 2;

									default:
										break;
								}
							}
						}
						break;
				
						case 2:
							/* 11 ccc 010 - JP cc,nnnn */
							return 3;

						case 3:
						{
							/* 11001011 - CB prefix */
							if ((Opcode & (3<<4))==(1<<4))
							{
								/* 11011011 - IN A,(n) */
								/* 11010011 - OUT (n),A */
								return 3;
							}	
							else if (Opcode == 0x0c3) 
							{
								/* 11000011 - JP nn */
								return 3;
							}
							else
							{
								/* 11100011 - EX (SP).HL */
								/* 11101011 - EX DE,HL */
							
								/* 11110011 - DI */
								/* 11111011 - EI */

								switch (Opcode)
								{
									case 0x0e3:
										return 6;
									case 0x0eb:
										return 1;
									case 0x0f3:
										return 1;
									case 0x0fb:
										return 1;
								
									default:
										break;
								}

							}

						}
						break;

						case 4:
						{
							/* 11 ccc 100 - CALL cc,nnnn */
#if 0
							sprintf(OutputString,"CALL %s,#%04x",
								ConditionCodes[((Opcode>>3) & 0x07)],
								Diss_GetWord(DisAddr));
#endif
						}
						break;


						case 5:
						{
							/* 11qq0101 - PUSH qq */
							if ((Opcode & (1<<3))==0)
							{
								/* 11qq0101 - PUSH qq */
								return 4;

							}
							else
							{
								/* 11001101 - CALL nn */
								/* 11011101 - DD */
								/* 11101101 - ED */
								/* 11111101 - FD */

								switch (Opcode)
								{
									case 0x0cd:
										return 5;

									default:
										break;
								}
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
							return 4;						
					}

				}
				break;
			}
		}
		break;
	}

	return -1;
}


static char *RegA[]=
{
	"R.BC.B.h",
	"R.BC.B.l",
	"R.DE.B.h",
	"R.DE.B.l",
	"R.HL.B.h",
	"R.HL.B.l",
	"",
	"R.AF.B.h"
};


const unsigned char *RegB[]=
{
	"R.BC.W",
	"R.DE.W",
	"R.HL.W",
	"R.SP.W"
};

const unsigned char *RegC[]=
{
	"R.BC.W",
	"R.DE.W",
	"R.HL.W",
	"R.AF.W"
};

static void	Z80_Index_GenerateCode(int DisAddr, unsigned char IndexCh,FILE *fh)
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

			fprintf(fh,"PrefixIgnore();\r\n");
		}
		break;

		case 0x0cb:
		{
			signed char Offset;

			/* CB prefix */

			/* signed offset from IX */
//			Offset = Z80_RD_MEM(DisAddr);
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

				unsigned char RegIndex = (Opcode & 0x07);


				switch ((Opcode>>3) & 0x07)
				{
					case 0:
					{
						if (RegIndex==6)
						{
							fprintf(fh,"RLC_INDEX(R.I%c.W);\r\n",IndexCh);

						}
						else
						{
							fprintf(fh,"INDEX_CB_RLC_REG(R.I%c.W,%s);\r\n",IndexCh,RegA[RegIndex]);
						}
					}
					break;
					case 1:
					{
						if (RegIndex==6)
						{
							fprintf(fh,"RRC_INDEX(R.I%c.W);\r\n",IndexCh);

						}
						else
						{
							fprintf(fh,"INDEX_CB_RRC_REG(R.I%c.W,%s);\r\n",IndexCh,RegA[RegIndex]);
						}
					}
					break;
					case 2:
					{
						if (RegIndex==6)
						{
							fprintf(fh,"RL_INDEX(R.I%c.W);\r\n",IndexCh);

						}
						else
						{
							fprintf(fh,"INDEX_CB_RL_REG(R.I%c.W,%s);\r\n",IndexCh,RegA[RegIndex]);
						}
					}
					break;
					case 3:
					{
						if (RegIndex==6)
						{
							fprintf(fh,"RR_INDEX(R.I%c.W);\r\n",IndexCh);

						}
						else
						{
							fprintf(fh,"INDEX_CB_RR_REG(R.I%c.W,%s);\r\n",IndexCh,RegA[RegIndex]);
						}
					}
					break;
					case 4:
					{
						if (RegIndex==6)
						{
							fprintf(fh,"SLA_INDEX(R.I%c.W);\r\n",IndexCh);

						}
						else
						{
							fprintf(fh,"INDEX_CB_SLA_REG(R.I%c.W,%s);\r\n",IndexCh,RegA[RegIndex]);
						}
					}
					break;
					case 5:
					{
						if (RegIndex==6)
						{
							fprintf(fh,"SRA_INDEX(R.I%c.W);\r\n",IndexCh);

						}
						else
						{
							fprintf(fh,"INDEX_CB_SRA_REG(R.I%c.W,%s);\r\n",IndexCh,RegA[RegIndex]);
						}
					}
					break;
					case 6:
					{
						if (RegIndex==6)
						{
							fprintf(fh,"SLL_INDEX(R.I%c.W);\r\n",IndexCh);

						}
						else
						{
							fprintf(fh,"INDEX_CB_SLL_REG(R.I%c.W,%s);\r\n",IndexCh,RegA[RegIndex]);
						}
					}
					break;
					case 7:
					{
						if (RegIndex==6)
						{
							fprintf(fh,"SRL_INDEX(R.I%c.W);\r\n",IndexCh);

						}
						else
						{
							fprintf(fh,"INDEX_CB_SRL_REG(R.I%c.W,%s);\r\n",IndexCh,RegA[RegIndex]);
						}
					}
					break;
				}
			}
			else
			{
				/* 01bbbrrr - BIT */
				/* 10bbbrrr - RES */
				/* 11bbbrrr - SET */
				int BitIndex = ((Opcode>>3) & 0x07);

				switch (Opcode & 0x0c0)
				{
					case 0x040:
					{
						fprintf(fh,"BIT_INDEX(%d,R.I%c.W);\r\n",BitIndex, IndexCh);
					}
					break;

					case 0x080:
					{
						unsigned char Reg;

						Reg = (Opcode & 0x07);

						if (Reg==6)
						{
							fprintf(fh,"RES_INDEX(0x%02x,R.I%c.W);\r\n",(1<<BitIndex), IndexCh);
						}
						else
						{
							fprintf(fh,"INDEX_CB_RES_REG(0x0%02x, R.I%c.W, %s);\r\n",(1<<BitIndex), IndexCh, RegA[Reg]);
						}
					}
					break;

					case 0x0c0:
					{
						unsigned char Reg;

						Reg = (Opcode & 0x07);

						if (Reg==6)
						{
							fprintf(fh,"SET_INDEX(0x0%02x,R.I%c.W);\r\n",(1<<BitIndex), IndexCh);
						}
						else
						{
							fprintf(fh,"INDEX_CB_SET_REG(0x0%02x, R.I%c.W, %s);\r\n",(1<<BitIndex), IndexCh, RegA[Reg]);
						}

					}
					break;
				}
			}
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
						case 1:
						{
							if ((Opcode & 0x08)!=0)
							{
								/* 00ss1001 - ADD HL,ss */
								unsigned char RegIndex = ((Opcode>>4) & 0x03);

								if (RegIndex!=2)
								{
									fprintf(fh,"ADD_RR_rr(R.I%c.W,%s);\r\n",IndexCh,RegB[RegIndex]);
								}
								else
								{
									fprintf(fh,"ADD_RR_rr(R.I%c.W,R.I%c.W);\r\n",IndexCh,IndexCh);
								}

					//			Diss_strcat("ADD");
					//			Diss_space();
					//			Diss_IndexReg(IndexCh);
					//			Diss_comma();

					//			if (Diss_Index_IsRegIndex(RegIndex))
					//			{
					//				Diss_IndexReg(IndexCh);
					//			}
					//			else
					//			{
					//				Diss_strcat(RegB[RegIndex]);
					//			}
					//			Diss_endstring();
							}
							else
							{
								/* 00dd0001 - LD dd,nn */
								fprintf(fh,"LD_INDEXRR_nn(R.I%c.W);\r\n",IndexCh);
					//			sprintf(OutputString,"LD %s,#%04x",
					//				RegB[((Opcode>>4) & 0x03)],
					//				Diss_GetWord(DisAddr));
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
				
									if ((Opcode & (1<<3))==0)
									{
										fprintf(fh,"LD_nnnn_INDEXRR(R.I%c.W);\r\n",IndexCh);
					//					sprintf(OutputString,"LD (#%04x),HL",
					//						Diss_GetWord(DisAddr));
	
									}
									else
									{
										fprintf(fh,"LD_INDEXRR_nnnn(R.I%c.W);\r\n",IndexCh);
					//					sprintf(OutputString,"LD HL,(#%04x)",
					//						Diss_GetWord(DisAddr));
									}
								}
								break;

								default:
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
								fprintf(fh,"INC_rp(R.I%c.W);\r\n",IndexCh);
							}
							else
							{
								/* 00ss1011 - DEC ss */
								fprintf(fh,"DEC_rp(R.I%c.W);\r\n",IndexCh);
							}

							
					//		Diss_strcat(Instruction);
					//		Diss_space();
					//		Diss_IndexReg(IndexCh);
					//		Diss_endstring();
						}
						break;

						case 4:
						{
							/* 00rrr100 - INC r */

							unsigned char RegIndex;

							RegIndex = ((Opcode>>3) & 0x07);

							if (RegIndex==4)
							{
								fprintf(fh,"INC_R(R.I%c.B.h);\r\n",IndexCh);
							}
							else if (RegIndex==5)
							{
								fprintf(fh,"INC_R(R.I%c.B.l);\r\n",IndexCh);
							}
							else if (RegIndex==6)
							{
								fprintf(fh,"_INC_INDEX_(R.I%c.W);\r\n",IndexCh);
							}

					//		Diss_strcat("INC");
					//		Diss_space();
					//		Diss_Index_Reg8Bit(((Opcode>>3) & 0x07),DisAddr, IndexCh);
					//		Diss_endstring();

									
						}
						break;

						case 5:
						{
							/* 00rrr101 - DEC r */
							unsigned char RegIndex;

							RegIndex = ((Opcode>>3) & 0x07);

							if (RegIndex==4)
							{
								fprintf(fh,"DEC_R(R.I%c.B.h);\r\n",IndexCh);
							}
							else if (RegIndex==5)
							{
								fprintf(fh,"DEC_R(R.I%c.B.l);\r\n",IndexCh);
							}
							else if (RegIndex==6)
							{
								fprintf(fh,"_DEC_INDEX_(R.I%c.W);\r\n",IndexCh);
							}

					//		Diss_strcat("DEC");
					//		Diss_space();
					//		Diss_Index_Reg8Bit(((Opcode>>3) & 0x07),DisAddr, IndexCh);
					//		Diss_endstring();
						}
						break;

						case 6:
						{
							/* LD r,n - 00rrr110 */
							//unsigned char Data = Z80_RD_MEM(DisAddr+1);
							unsigned char RegIndex;

							RegIndex = ((Opcode>>3) & 0x07);

							if (RegIndex==4)
							{
								fprintf(fh,"LD_RI_n(R.I%c.B.h);\r\n",IndexCh);
							}
							else if (RegIndex==5)
							{
								fprintf(fh,"LD_RI_n(R.I%c.B.l);\r\n",IndexCh);
							}
							else if (RegIndex==6)
							{
								fprintf(fh,"LD_INDEX_n(R.I%c.W);\r\n",IndexCh);
							}
					//		Diss_strcat("LD");
					//		Diss_space();
					//		Diss_Index_Reg8Bit(((Opcode>>3) & 0x07),DisAddr, IndexCh);
					//		Diss_comma();
					//		Diss_WriteHexByte(Data);
					//		Diss_endstring();
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
			//		Diss_strcat("LD");
			//		Diss_space();
					
					Reg1 = (Opcode>>3) & 0x07;
					Reg2 = Opcode & 0x07; 

					if ((Reg1==6) || (Reg2==6))
					{
						if (Reg1==6)
						{
							fprintf(fh,"LD_INDEX_R(R.I%c.W,%s);\r\n",IndexCh,RegA[Reg2]);
						}
						
						if (Reg2==6)
						{
							fprintf(fh,"LD_R_INDEX(R.I%c.W,%s);\r\n",IndexCh,RegA[Reg1]);
						}
					}
					else
					{ 
						unsigned char Reg1Char, Reg2Char;

						if (Reg1 == 4)
							Reg1Char = 'h';
						if (Reg1 == 5)
							Reg1Char = 'l';

						if (Reg2 == 4)
							Reg2Char = 'h';
						if (Reg2 == 5)
							Reg2Char = 'l';


						if ((Reg1==4) || (Reg1==5))
						{
							if ((Reg2==4) || (Reg2==5))
							{
								if (Reg1!=Reg2)
								{
									/* reg 1,2 IXh type */
									fprintf(fh,"LD_R_R(R.I%c.B.%c,R.I%c.B.%c);\r\n",IndexCh,Reg1Char,IndexCh,Reg2Char);
								}
							}
							else
							{
								/* reg 1 is IXH, Reg 2 isn't */
								fprintf(fh,"LD_R_R(R.I%c.B.%c,%s);\r\n",IndexCh,Reg1Char,RegA[Reg2]);
							}
						}
						else 
						{
							if ((Reg2==4) || (Reg2==5))
							{
								/* reg 1 not IXH type */
								fprintf(fh,"LD_R_R(%s,R.I%c.B.%c);\r\n",RegA[Reg1],IndexCh,Reg2Char);
							}
						}

			//			Diss_Index_Reg8Bit(Reg1,DisAddr, IndexCh);
			//			Diss_comma();
			//			Diss_Index_Reg8Bit(Reg2,DisAddr,IndexCh);
					}

			//		Diss_endstring();
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

					unsigned char Reg;

					Reg = (Opcode & 0x07);

					switch ((Opcode>>3) & 0x07)
					{
						case 0:
						{
							if (Reg==6)
							{
								fprintf(fh,"ADD_A_INDEX(R.I%c.W);\r\n",IndexCh);
							}
							else
							{
								unsigned char RegPartCh;

								if (Reg==4)
									RegPartCh = 'h';

								if (Reg==5)
									RegPartCh = 'l';

								fprintf(fh,"ADD_A_R(R.I%c.B.%c);\r\n",IndexCh,RegPartCh);
							}
						}
						break;

						case 1:
						{
							if (Reg==6)
							{
								fprintf(fh,"ADC_A_INDEX(R.I%c.W);\r\n",IndexCh);
							}
							else
							{
								unsigned char RegPartCh;

								if (Reg==4)
									RegPartCh = 'h';

								if (Reg==5)
									RegPartCh = 'l';

								fprintf(fh,"ADC_A_R(R.I%c.B.%c);\r\n",IndexCh,RegPartCh);
							}
						}
						break;

						case 2:
						{
							if (Reg==6)
							{
								fprintf(fh,"SUB_A_INDEX(R.I%c.W);\r\n",IndexCh);
							}
							else
							{
								unsigned char RegPartCh;

								if (Reg==4)
									RegPartCh = 'h';

								if (Reg==5)
									RegPartCh = 'l';

								fprintf(fh,"SUB_A_R(R.I%c.B.%c);\r\n",IndexCh,RegPartCh);
							}
						}
						break;

						case 3:
						{
							if (Reg==6)
							{
								fprintf(fh,"SBC_A_INDEX(R.I%c.W);\r\n",IndexCh);
							}
							else
							{
								unsigned char RegPartCh;

								if (Reg==4)
									RegPartCh = 'h';

								if (Reg==5)
									RegPartCh = 'l';

								fprintf(fh,"SBC_A_R(R.I%c.B.%c);\r\n",IndexCh,RegPartCh);
							}
						}
						break;

						case 4:
						{
							if (Reg==6)
							{
								fprintf(fh,"AND_A_INDEX(R.I%c.W);\r\n",IndexCh);
							}
							else
							{
								unsigned char RegPartCh;

								if (Reg==4)
									RegPartCh = 'h';

								if (Reg==5)
									RegPartCh = 'l';

								fprintf(fh,"AND_A_R(R.I%c.B.%c);\r\n",IndexCh,RegPartCh);
							}
						}
						break;

						case 5:
						{
							if (Reg==6)
							{
								fprintf(fh,"XOR_A_INDEX(R.I%c.W);\r\n",IndexCh);
							}
							else
							{
								unsigned char RegPartCh;

								if (Reg==4)
									RegPartCh = 'h';

								if (Reg==5)
									RegPartCh = 'l';

								fprintf(fh,"XOR_A_R(R.I%c.B.%c);\r\n",IndexCh,RegPartCh);
							}
						}
						break;

						case 6:
						{
							if (Reg==6)
							{
								fprintf(fh,"OR_A_INDEX(R.I%c.W);\r\n",IndexCh);
							}
							else
							{
								unsigned char RegPartCh;

								if (Reg==4)
									RegPartCh = 'h';

								if (Reg==5)
									RegPartCh = 'l';

								fprintf(fh,"OR_A_R(R.I%c.B.%c);\r\n",IndexCh,RegPartCh);
							}
						}
						break;

						case 7:
						{
							if (Reg==6)
							{
								fprintf(fh,"CP_A_INDEX(R.I%c.W);\r\n",IndexCh);
							}
							else
							{
								unsigned char RegPartCh;

								if (Reg==4)
									RegPartCh = 'h';

								if (Reg==5)
									RegPartCh = 'l';

								fprintf(fh,"CP_A_R(R.I%c.B.%c);\r\n",IndexCh,RegPartCh);
							}
						}
						break;
					}




			//		Diss_strcat(ArithmeticMneumonics[((Opcode>>3) & 0x07)]);
			//		Diss_space();
			//		Diss_Index_Reg8Bit((Opcode & 0x07),DisAddr, IndexCh);
			//		Diss_endstring();
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
								fprintf(fh,"R.I%c.W = POP();\r\n",IndexCh);

					//			Diss_strcat("POP");
					//			Diss_space();
					//			Diss_IndexReg(IndexCh);
					//			Diss_endstring();
							}
							else
							{	
								/* 11001001 - RET */
								/* 11011001 - EXX */
								/* 11101001 - JP (HL) */
								/* 11111001 - LD SP,HL */

								if (Opcode==0x0e9)
								{
									fprintf(fh,"JP_rp(R.I%c.W);\r\n",IndexCh);
								}

								if (Opcode==0x0f9)
								{
									fprintf(fh,"LD_SP_rp(R.I%c.W);\r\n",IndexCh);
								}



					//			sprintf(OutputString,
					//				MiscMneumonics3[((Opcode>>4) & 0x03)]);
							}
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

								if ((Opcode & (1<<4))!=0)
								{
									/* 11011011 - IN A,(n) */
								
					//				sprintf(OutputString,"IN A,(#%02x)",
					//					PortByte);
								}
								else
								{
									/* 11010011 - OUT (n),A */
					//				sprintf(OutputString,"OUT (#%02x),A",
					//					PortByte);
								}
		
							}	
							else if (Opcode == 0x0c3) 
							{
								/* 11000011 - JP nn */
					//			sprintf(OutputString,"JP #%04x",
					//				Diss_GetWord(DisAddr));
							}
							else
							{
								/* 11100011 - EX (SP).HL */
								/* 11101011 - EX DE,HL */
							
								/* 11110011 - DI */
								/* 11111011 - EI */

								if (Opcode==0x0e3)
								{
									fprintf(fh,"EX_SP_rr(R.I%c.W);\r\n",IndexCh);
								}

					//			sprintf(OutputString,"%s",
					//				MiscMneumonics9[(((Opcode>>3) & 0x07)-4)]);
							}

						}
						break;

						case 5:
						{
							/* 11qq0101 - PUSH qq */
							if ((Opcode & (1<<3))==0)
							{
								fprintf(fh,"PUSH(R.I%c.W);\r\n",IndexCh);

					//			Diss_strcat("PUSH");
					//			Diss_space();
					//			Diss_IndexReg(IndexCh);
					//			Diss_endstring();
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
}


void	Z80_GenerateCode(int Addr, FILE *fh)
{
	unsigned char Opcode;
	Z80_WORD		DisAddr = Addr;
	
//	pDissassemblyString = OutputString;

	Opcode = Z80_RD_MEM(DisAddr);
	DisAddr++;

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
				
				unsigned char Reg = (Opcode & 0x07);
 
				switch ((Opcode>>3) & 0x07)
				{
					case 0:
					{
						if (Reg!=6)
						{
							fprintf(fh,"RLC_REG(%s);\r\n",RegA[Reg]);
						}
						else
						{
							fprintf(fh,"RLC_HL();\r\n");
						}
					}
					break;

					case 1:
					{
						if (Reg!=6)
						{
							fprintf(fh,"RRC_REG(%s);\r\n",RegA[Reg]);
						}
						else
						{
							fprintf(fh,"RRC_HL();\r\n");
						}
					}
					break;


					case 2:
					{
						if (Reg!=6)
						{
							fprintf(fh,"RL_REG(%s);\r\n",RegA[Reg]);
						}
						else
						{
							fprintf(fh,"RL_HL();\r\n");
						}
					}
					break;

					case 3:
					{
						if (Reg!=6)
						{
							fprintf(fh,"RR_REG(%s);\r\n",RegA[Reg]);
						}
						else
						{
							fprintf(fh,"RR_HL();\r\n");
						}



					}
					break;

					case 4:
					{
						if (Reg!=6)
						{
							fprintf(fh,"SLA_REG(%s);\r\n",RegA[Reg]);
						}
						else
						{
							fprintf(fh,"SLA_HL();\r\n");
						}



					}
					break;

					case 5:
					{
						if (Reg!=6)
						{
							fprintf(fh,"SRA_REG(%s);\r\n",RegA[Reg]);
						}
						else
						{
							fprintf(fh,"SRA_HL();\r\n");
						}

					}
					break;

					case 6:
						{
						if (Reg!=6)
						{
							fprintf(fh,"SLL_REG(%s);\r\n",RegA[Reg]);
						}
						else
						{
							fprintf(fh,"SLL_HL();\r\n");
						
						}
						}
						break;

					case 7:
						{						
							if (Reg!=6)
							{
								fprintf(fh,"SRL_REG(%s);\r\n",RegA[Reg]);
							}
							else
							{
								fprintf(fh,"SRL_HL();\r\n");
							}


						}
						break;

				}

//				Diss_strcat(CB_ShiftMneumonics[((Opcode>>3) & 0x07)]);
//				Diss_space();
//				Diss_strcat(RegA[(Opcode & 0x07)]);
//				Diss_endstring();
			}
			else
			{
				/* 01bbbrrr - BIT */
				/* 10bbbrrr - RES */
				/* 11bbbrrr - SET */
				int BitIndex = ((Opcode>>3) & 0x07);
				
				unsigned char Reg = (Opcode & 0x07);

				switch (Opcode & 0x0c0)
				{
					case 0x040:
					{
						if (Reg!=6)
						{
							fprintf(fh,"BIT_REG(%d,%s);\r\n",BitIndex, RegA[Reg]);
						}
						else
						{
							fprintf(fh,"BIT_HL(%d);\r\n",BitIndex);
						}
						
					}
					break;

					case 0x080:
					{
						if (Reg!=6)
						{
							fprintf(fh,"RES_REG(0x%02x,%s);\r\n",(1<<BitIndex), RegA[Reg]);
						}
						else
						{
							fprintf(fh,"RES_HL(0x%02x);\r\n",(1<<BitIndex));
						}
					}
					break;


					case 0x0c0:
					{
						if (Reg!=6)
						{
							fprintf(fh,"SET_REG(0x%02x,%s);\r\n",(1<<BitIndex), RegA[Reg]);
						}
						else
						{
							fprintf(fh,"SET_HL(0x%02x);\r\n",(1<<BitIndex));
						}
					}
					break;
				}

//				Diss_strcat(CB_BitOperationMneumonics[(((Opcode>>6)&3)-1)]);
//				Diss_space();
//				*pDissassemblyString = BitIndex+'0';
//				*pDissassemblyString++;
//				Diss_comma();
//				Diss_strcat(RegA[Opcode & 0x07]);
//				Diss_endstring();
			}
		}
		break;

		case 0x0dd:
		{
			Z80_Index_GenerateCode(DisAddr, 'X',fh);
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
							fprintf(fh,"_IN(R.TempByte);\r\n");

						}
						else
						{
							fprintf(fh,"_IN(%s);\r\n",RegA[RegIndex]);
						}
					}
					break;

					case 1:
					{
						/* OUT (C),r - 01rrr001 */
						int RegIndex = ((Opcode>>3) & 0x07);

						if ((Opcode & (0x07<<3))==(6<<3))
						{
							/* OUT (C),0 */
							fprintf(fh,"_OUT(0);\r\n");
						}
						else
						{
							fprintf(fh,"_OUT(%s);\r\n",RegA[RegIndex]);
						}
					}
					break;

					case 2:
					{
						unsigned char *Instruction;

						if ((Opcode & 0x08)!=0)
						{
							/* ADC HL,ss - 01ss1010 */
							fprintf(fh,"ADC_HL_rr(%s);\r\n",RegB[((Opcode>>4) & 0x03)]);
						}
						else
						{
							/* SBC HL,ss - 01ss0010 */
							fprintf(fh,"SBC_HL_rr(%s);\r\n",RegB[((Opcode>>4) & 0x03)]);
						}

						
//						Diss_strcat(Instruction);
//						Diss_space();
//						Diss_strcat(RegB[2]);
//						Diss_comma();
//						Diss_strcat(RegB[((Opcode>>4) & 0x03)]);
					}
					break;

					case 3:
					{
						/* LD dd,(nn) - 01dd1011 */
						/* LD (nn),dd - 01dd0011 */

						if ((Opcode & 0x08)!=0)
						{
							fprintf(fh,"LD_RR_nnnn(%s);\r\n",RegB[((Opcode>>4) & 0x03)]);

//							sprintf(OutputString, "LD %s,(#%04x)",
//								RegB[((Opcode>>4) & 0x03)],
//								Diss_GetWord(DisAddr));
						}
						else
						{
							fprintf(fh,"LD_nnnn_RR(%s);\r\n",RegB[((Opcode>>4) & 0x03)]);
//							sprintf(OutputString," LD (#%04x),%s",
//								Diss_GetWord(DisAddr),
//								RegB[((Opcode>>4) & 0x03)]);
						}
					}
					break;

					case 4:
					{
						/* NEG - 01xxx100 */
						fprintf(fh,"NEG();\r\n");	//OutputString,"NEG");
					}
					break;

					case 5:
					{
						/* RETI - 01xx1110 */
						/* RETN - 01xx0110 */
						if ((Opcode & 0x08)!=0)
						{
							fprintf(fh,"RETI();\r\n");

//							sprintf(OutputString,"RETI");
						}
						else
						{
							fprintf(fh,"RETN();\r\n");
//							sprintf(OutputString,"RETN");
						}
					}
					break;

					case 6:
					{
						unsigned char IM_Type;

						/* IM 0 - 01x00110 */
						/* IM ? - 01x01110 */
						/* IM 1 - 01x10110 */
						/* IM 2 - 01x11110 */

						switch ((Opcode>>3) & 0x03)
						{
							case 0:
							{
								IM_Type = 0;
							}
							break;

							case 1:
							{
								IM_Type = 0;
							}
							break;

							case 2:
							{
								IM_Type = 1;
							}
							break;

							case 3:
							{
								IM_Type = 2;
							}
							break;
						}

						fprintf(fh,"SET_IM(%d);\r\n",IM_Type);

//						Diss_strcat("IM");
//						Diss_space();
//						pDissassemblyString[0] = IM_Type;
//						pDissassemblyString++;
//						Diss_endstring();
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

						switch ((Opcode>>3) & 0x07)
						{
							case 0:
							{
								fprintf(fh,"LD_I_A();\r\n");
							}
							break;
							
							case 1:
							{
								fprintf(fh,"LD_R_A();\r\n");
							}
							break;

							case 2:
							{
								fprintf(fh,"LD_A_I();\r\n");
							}
							break;

							case 3:
							{
								fprintf(fh,"LD_A_R();\r\n");
							}
							break;

							case 4:
							{
								fprintf(fh,"RRD();\r\n");
							}
							break;

							case 5:
							{
								fprintf(fh,"RLD();\r\n");
							}
							break;


							default:
								break;
						}
						

//						sprintf(OutputString,"%s",
//							MiscMneumonics4[((Opcode>>3) & 0x07)]);


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

						switch ((Opcode>>3) & 0x03)
						{
							case 0:
							{
								fprintf(fh,"LDI();\r\n");
							}
							break;

							case 1:
							{
								fprintf(fh,"LDD();\r\n");
							}
							break;

							case 2:
							{


								fprintf(fh,"LDI();\r\n");

							//	fprintf(fh,"LDIR();\r\n");
							}
							break;

							case 3:
							{
								fprintf(fh,"LDD();\r\n");
							//	fprintf(fh,"LDDR();\r\n");
							}
							break;
						}

						switch ((Opcode>>3) & 0x03)
						{
							case 0:
							case 1:
								break;

							case 2:
							case 3:
							{
								fprintf(fh,"if (FLAGS_REG & Z80_PARITY_FLAG)\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"Z80_UpdateCycles(6);\r\n");
								fprintf(fh,"}\r\n");
								fprintf(fh,"else\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"ADD_PC(2);\r\n");
								fprintf(fh,"Z80_UpdateCycles(5);\r\n");
								fprintf(fh,"}\r\n");
							}
							break;
						}

//						sprintf(OutputString,"%s",
//							MiscMneumonics5[((Opcode>>3) & 0x03)]);

					}
					break;
					
					case 1:
					{
						/* 10100001 - CPI */
						/* 10111001 - CPDR */
						/* 10110001 - CPIR */
						/* 10101001 - CPD */

						switch ((Opcode>>3) & 0x03)
						{
							case 0:
							{
								fprintf(fh,"CPI();\r\n");
							}
							break;

							case 1:
							{
								fprintf(fh,"CPD();\r\n");
							}
							break;

							case 2:
							{
								fprintf(fh,"CPI();\r\n");
							//	fprintf(fh,"LDIR();\r\n");
							}
							break;

							case 3:
							{
								fprintf(fh,"CPD();\r\n");
							//	fprintf(fh,"LDDR();\r\n");
							}
							break;
						}

						switch ((Opcode>>3) & 0x03)
						{
							case 0:
							case 1:
								break;

							case 2:
							case 3:
							{
								fprintf(fh,"if ((FLAGS_REG & (Z80_PARITY_FLAG | Z80_ZERO_FLAG))==Z80_PARITY_FLAG)\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"Z80_UpdateCycles(5);\r\n");
								fprintf(fh,"}\r\n");
								fprintf(fh,"else\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"ADD_PC(2);\r\n");
								fprintf(fh,"Z80_UpdateCycles(6);\r\n");
								fprintf(fh,"}\r\n");
							}
							break;
						}


		//				sprintf(OutputString,"%s",
		//					MiscMneumonics6[((Opcode>>3) & 0x03)]);

					}
					break;

					case 2:
					{
						/* 10100010 - INI */
						/* 10110010 - INIR */
						/* 10101010 - IND */
						/* 10111010 - INDR */
						switch ((Opcode>>3) & 0x03)
						{
							case 0:
							{
								fprintf(fh,"INI();\r\n");
							}
							break;

							case 1:
							{
								fprintf(fh,"IND();\r\n");
							}
							break;

							case 2:
							{
								fprintf(fh,"INI();\r\n");
							//	fprintf(fh,"LDIR();\r\n");
							}
							break;

							case 3:
							{
								fprintf(fh,"IND();\r\n");
							//	fprintf(fh,"LDDR();\r\n");
							}
							break;
						}

						switch ((Opcode>>3) & 0x03)
						{
							case 0:
							case 1:
								break;

							case 2:
							case 3:
							{
								fprintf(fh,"if (FLAGS_REG & Z80_ZERO_FLAG)\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"ADD_PC(2);\r\n");
								fprintf(fh,"Z80_UpdateCycles(1);\r\n");
								fprintf(fh,"}\r\n");
								fprintf(fh,"else\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"}\r\n");
							}
							break;
						}



		//				sprintf(OutputString,"%s",
		//					MiscMneumonics7[((Opcode>>3) & 0x03)]);

					}
					break;

					case 3:
					{
						/* 10100011 - OUTI */
						/* 10110011 - OTIR */
						/* 10101011 - outd */
						/* 10111011 - otdr */
						switch ((Opcode>>3) & 0x03)
						{
							case 0:
							{
								fprintf(fh,"OUTI();\r\n");
							}
							break;

							case 1:
							{
								fprintf(fh,"OUTD();\r\n");
							}
							break;

							case 2:
							{
								fprintf(fh,"OUTI();\r\n");
							//	fprintf(fh,"LDIR();\r\n");
							}
							break;

							case 3:
							{
								fprintf(fh,"OUTD();\r\n");
							//	fprintf(fh,"LDDR();\r\n");
							}
							break;
						}
		
						switch ((Opcode>>3) & 0x03)
						{
							case 0:
							case 1:
								break;

							case 2:
							case 3:
							{
								fprintf(fh,"if (FLAGS_REG & Z80_ZERO_FLAG)\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"ADD_PC(2);\r\n");
								fprintf(fh,"Z80_UpdateCycles(1);\r\n");
								fprintf(fh,"}\r\n");
								fprintf(fh,"else\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"}\r\n");
							}
							break;
						}

		//				sprintf(OutputString,"%s",
		//					MiscMneumonics8[((Opcode>>3) & 0x03)]);

					}
					break;

				}
			}
			else
			{
		//		Diss_strcat("DEFB");
		//		Diss_space();
		//		Diss_WriteHexByte(0x0ed);
		//		Diss_comma()
				;
		//		Diss_WriteHexByte(Opcode);
		//		Diss_endstring();
			}
		}
		break;

		case 0x0fd:
		{

			Z80_Index_GenerateCode(DisAddr,'Y',fh);
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
	
								fprintf(fh,"if (");

								switch ((Opcode>>3) & 0x03)
								{
									case 0:
									{
										fprintf(fh,"ZERO_NOT_SET");
									}
									break;

									case 1:
									{
										fprintf(fh,"ZERO_SET");
									}
									break;

									case 2:
									{
										fprintf(fh,"CARRY_NOT_SET");
									}
									break;

									case 3:
									{
										fprintf(fh,"CARRY_SET");
									}
									break;
								}
				
								fprintf(fh,")\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"JR();\r\n");
								fprintf(fh,"Z80_UpdateCycles(3);\r\n");

								fprintf(fh,"}\r\nelse\r\n{\r\n");
								fprintf(fh,"ADD_PC(2);\r\n");
								fprintf(fh,"Z80_UpdateCycles(2);\r\n");
								fprintf(fh,"}\r\n");

					//			sprintf(OutputString,"JR %s,#%04x",
					//				ConditionCodes[((Opcode>>3) & 0x03)],
					//				Diss_GetRelativeAddr(DisAddr));

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
										fprintf(fh,"DJNZ_dd();\r\n");

//										Instruction = "DJNZ";
									}
									else
									{
										fprintf(fh,"JR();\r\n");
//
//										Instruction = "JR";
									}

					//				sprintf(OutputString,"%s #%04x",
					//					Instruction, 
					//					Diss_GetRelativeAddr(DisAddr));
								}
								else
								{
									/* 00000000 - NOP */
									/* 00001000 - EX AF,AF */

									if (Opcode==0x08)
									{
										fprintf(fh,"SWAP(R.AF.W,R.altAF.W);\r\n");
									}
					//				sprintf(OutputString,"%s",
					//					MiscMneumonics2[((Opcode>>3) & 0x07)]);
								}

							}
						}
						break;

						case 1:
						{
							if ((Opcode & 0x08)!=0)
							{
								/* 00ss1001 - ADD HL,ss */
								fprintf(fh,"ADD_RR_rr(R.HL.W,%s);\r\n",RegB[((Opcode>>4) & 0x03)]);

					//			sprintf(OutputString,"ADD HL,%s",
					//				RegB[((Opcode>>4) & 0x03)]);
							}
							else
							{
								/* 00dd0001 - LD dd,nn */
								fprintf(fh,"LD_RR_nn(%s);\r\n",RegB[((Opcode>>4) & 0x03)]);

					//			sprintf(OutputString,"LD %s,#%04x",
					//				RegB[((Opcode>>4) & 0x03)],
					//				Diss_GetWord(DisAddr));
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
										fprintf(fh,"LD_A_RR(R.BC.W);\r\n");
								//		sprintf(OutputString,"LD A,(BC)");
									}
									else
									{
										fprintf(fh,"LD_RR_A(R.BC.W);\r\n");
								//		sprintf(OutputString,"LD (BC),A");
									}
							
								}
								break;

								case 1:
								{
									/* 00010010 - LD (DE),A */
									/* 00011010 - LD A,(DE) */
				
									if ((Opcode & (1<<3))!=0)
									{
										fprintf(fh,"LD_A_RR(R.DE.W);\r\n");
								//		sprintf(OutputString,"LD A,(DE)");
									}
									else
									{
										fprintf(fh,"LD_RR_A(R.DE.W);\r\n");
								//		sprintf(OutputString,"LD (DE),A");
									}
								}
								break;

								case 2:
								{
									/* 00100010 - LD (nnnn),HL */
									/* 00101010 - LD HL,(nn) */
				
									if ((Opcode & (1<<3))==0)
									{
										fprintf(fh,"LD_nnnn_HL();\r\n");
										
										//		sprintf(OutputString,"LD (#%04x),HL",
								//			Diss_GetWord(DisAddr));
	
									}
									else
									{
										fprintf(fh,"LD_HL_nnnn();\r\n");
								//		sprintf(OutputString,"LD HL,(#%04x)",
								//			Diss_GetWord(DisAddr));
									}
								}
								break;

								case 3:
								{
									/* 00110010 - LD (nnnn),A */
									/* 00111010 - LD A,(nnnn) */
									
									if ((Opcode & (1<<3))==0)
									{
										fprintf(fh,"LD_nnnn_A();\r\n");
								//		sprintf(OutputString,"LD (#%04x),A",
								//			Diss_GetWord(DisAddr));
									}
									else
									{
										fprintf(fh,"LD_A_nnnn();\r\n");
								//		sprintf(OutputString,"LD A,(#%04x)",
								//			Diss_GetWord(DisAddr));
									}
								}
								break;
							}

						}
						break;

						case 3:
						{
							unsigned char *Instruction;
							unsigned char RegIndex = ((Opcode>>4) & 0x03);

							if ((Opcode & 0x08)==0)
							{
								/* 00ss0011 - INC ss */
							//	Instruction = "INC";
								fprintf(fh,"INC_rp(%s);\r\n",RegB[RegIndex]);
							}
							else
							{
								/* 00ss1011 - DEC ss */
								//Instruction = "DEC";
								fprintf(fh,"DEC_rp(%s);\r\n",RegB[RegIndex]);
							}

							//Diss_strcat(Instruction);
							//Diss_space();
							//Diss_strcat_only(RegB[((Opcode>>4) & 0x03)]);
						}
						break;

						case 4:
						{
							/* 00rrr100 - INC r */
							
							unsigned char Reg;

							Reg = ((Opcode>>3) & 0x07);

							if (Reg!=6)
							{
								fprintf(fh,"INC_R(%s);\r\n",RegA[Reg]);
							}
							else
							{
								fprintf(fh,"INC_HL_();\r\n");
							}

							//	sprintf(OutputString,"INC %s",
						//		RegA[((Opcode>>3) & 0x07)]);
							
						}
						break;

						case 5:
						{
							/* 00rrr101 - DEC r */
							unsigned char Reg;

							Reg = ((Opcode>>3) & 0x07);

						
							if (Reg!=6)
							{
								fprintf(fh,"DEC_R(%s);\r\n",RegA[Reg]);
							}
							else
							{
								fprintf(fh,"DEC_HL_();\r\n");
							}

						//	sprintf(OutputString,"DEC %s",
						//		RegA[((Opcode>>3) & 0x07)]);
							
						}
						break;

						case 6:
						{
							/* LD r,n - 00rrr110 */
							
							unsigned char Reg;

							Reg = ((Opcode>>3) & 0x07);

							if (Reg!=6)
							{
								fprintf(fh,"LD_R_n(%s);\r\n",RegA[Reg]);
							}
							else
							{
								fprintf(fh,"LD_HL_n();\r\n");
							}

							/* LD r,n - 00rrr110 */
						//	sprintf(OutputString,"LD %s,#%02x",
						//		RegA[(Opcode>>3) & 0x07],
						//		Data);

						}
						break;

						case 7:
						{

							/* 00000111 - RLCA */
							/* 00001111 - RRCA */
							/* 00010111 - RLA */
							/* 00011111 - RRA */

							switch ((Opcode>>3) & 0x07)
							{
								case 0:
									{
										fprintf(fh,"RLCA();\r\n");
									}
									break;

								case 1:
									{
										fprintf(fh,"RRCA();\r\n");
									}
									break;

								case 2:
									{
										fprintf(fh,"RLA();\r\n");
									}
									break;
								case 3:
									{
										fprintf(fh,"RRA();\r\n");
									}
									break;

								case 4:
									{
										fprintf(fh,"DAA();\r\n");
									}
									break;

								case 5:
									{
										fprintf(fh,"CPL();\r\n");
									}
									break;

								case 6:
									{
										fprintf(fh,"SCF();\r\n");
									}
									break;

								case 7:
									{
										fprintf(fh,"CCF();\r\n");
									}
									break;
							}

						//	Diss_strcat_only(MneumonicsList[((Opcode>>3) & 0x03)]);
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
						fprintf(fh,"HALT();\r\n");
					}
					else
					{
						int Reg1,Reg2;

						Reg1 = ((Opcode>>3) & 0x07);
						Reg2 = (Opcode & 0x07);

						if ((Reg1!=6) && (Reg2!=6))
						{
							if (Reg1!=Reg2)
							{
								fprintf(fh,"mov ah,byte ptr [%s]\r\n",RegA[Reg2]);
								fprintf(fh,"mov byte ptr [%s],ah\r\n",RegA[Reg1]);

//								fprintf(fh,"LD_R_R(%s,%s);\r\n",RegA[Reg1],
//									RegA[Reg2]);
							}
						}
						else
						{
							if (Reg1==6)
							{
								fprintf(fh,"LD_HL_R(%s);\r\n",RegA[Reg2]);
							}
							else
							{
								fprintf(fh,"LD_R_HL(%s);\r\n",RegA[Reg1]);
							}
						}

						/* 01rrrRRR - LD r,R */
				//		Diss_strcat("LD");
				//		Diss_space();
				//		Diss_strcat(RegA[((Opcode>>3) & 0x07)]);
				//		Diss_comma();
				//		Diss_strcat(RegA[(Opcode & 0x07)]);
				//		Diss_endstring();
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

					unsigned char RegIndex;

					RegIndex = (Opcode & 0x07);

					switch (((Opcode>>3) & 0x07))
					{
						case 0:
						{
							if (RegIndex==6)
							{
								fprintf(fh,"ADD_A_HL();\r\n");
							}
							else
							{
								//if (RegIndex!=7)
								//{
									fprintf(fh,"ADD_A_R(%s);\r\n",RegA[RegIndex]);
								//}
							}
						}
						break;

						case 1:
						{	
							if (RegIndex==6)
							{
								fprintf(fh,"ADC_A_HL();\r\n");
							}
							else
							{
								fprintf(fh,"ADC_A_R(%s);\r\n",RegA[RegIndex]);
							}
						}
						break;
						
						case 2:
						{
							if (RegIndex==6)
							{
								fprintf(fh,"SUB_A_HL();\r\n");
							}
							else
							{
								if (RegIndex!=7)
								{
									fprintf(fh,"SUB_A_R(%s);\r\n",RegA[RegIndex]);
								}
								else
								{
									// to be checked!!!!!!
									fprintf(fh,"Z80_BYTE Flags;\r\n");
									fprintf(fh,"%s = 0;\r\n",RegA[RegIndex]);
									fprintf(fh,"Flags = Z80_ZERO_FLAG | Z80_SUBTRACT_FLAG;\r\n");
									fprintf(fh,"FLAGS_REG = Flags;\r\n");
								}
							}
						}
						break;

						case 3:
						{
							if (RegIndex==6)
							{
								fprintf(fh,"SBC_A_HL();\r\n");
							}
							else
							{
								fprintf(fh,"SBC_A_R(%s);\r\n",RegA[RegIndex]);
							}
						}
						break;

						case 4:
						{
							if (RegIndex==6)
							{
								fprintf(fh,"AND_A_HL();\r\n");
							}
							else
							{
								if (RegIndex!=7)
								{
									fprintf(fh,"AND_A_R(%s);\r\n",RegA[RegIndex]);
								}
								else
								{
									fprintf(fh,"Z80_BYTE Flags;\r\n");
									fprintf(fh,"Flags = R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);\r\n");
									fprintf(fh,"Flags |= Z80_HALFCARRY_FLAG;\r\n");
									fprintf(fh,"Flags |= ZeroSignParityTable[R.AF.B.h];\r\n");
									fprintf(fh,"FLAGS_REG = Flags;\r\n");
								}
							}

						}
						break;

						case 5:
						{
							if (RegIndex==6)
							{
								fprintf(fh,"XOR_A_HL();\r\n");
							}
							else
							{
								if (RegIndex!=7)
								{
									fprintf(fh,"XOR_A_R(%s);\r\n",RegA[RegIndex]);
								}
								else
								{
									/* XOR A,A - A = 0, S = 0 (positive), Z = 1 (zero), 
									H = 0, P/V = 1 (even parity), N = 0, C = 0, F5 = 0, F3 = 0 */
									fprintf(fh,"Z80_BYTE Flags;\r\n");
									fprintf(fh,"%s=0;\r\n",RegA[RegIndex]);
									fprintf(fh,"Flags = Z80_ZERO_FLAG | Z80_PARITY_FLAG;\r\n");
									fprintf(fh,"FLAGS_REG = Flags;\r\n");
								}

							}
						}
						break;
						case 6:
						{
							if (RegIndex==6)
							{
								fprintf(fh,"OR_A_HL();\r\n");
							}
							else
							{
								if (RegIndex!=7)
								{
									fprintf(fh,"OR_A_R(%s);\r\n",RegA[RegIndex]);
								}
								else
								{
									fprintf(fh,"Z80_BYTE Flags;\r\n");
									fprintf(fh,"Flags = R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);\r\n");
									fprintf(fh,"Flags |= ZeroSignParityTable[R.AF.B.h];\r\n");
									fprintf(fh,"FLAGS_REG = Flags;\r\n");
								}
							}
						}
						break;
						case 7:
						{
							if (RegIndex==6)
							{
								fprintf(fh,"CP_A_HL();\r\n");
							}
							else
							{
								if (RegIndex!=7)
								{
									fprintf(fh,"CP_A_R(%s);\r\n",RegA[RegIndex]);
								}
								else
								{
									// in this case, Z80_UNUSED_FLAG1, Z80_UNUSED_FLAG2 are A before subtraction.
									fprintf(fh,"Z80_BYTE Flags;\r\n");
									fprintf(fh,"Flags = %s & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);\r\n",RegA[RegIndex]);
									fprintf(fh,"Flags |= Z80_ZERO_FLAG | Z80_SUBTRACT_FLAG;\r\n");
									fprintf(fh,"FLAGS_REG = Flags;\r\n");
								}
							}
						}
						break;
					}

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
								fprintf(fh,"if (");

								switch ((Opcode>>3) & 0x07)
								{
									case 0:
									{
										fprintf(fh,"ZERO_NOT_SET");
									}
									break;

									case 1:
									{
										fprintf(fh,"ZERO_SET");
									}
									break;

									case 2:
									{
										fprintf(fh,"CARRY_NOT_SET");
									}
									break;

									case 3:
									{
										fprintf(fh,"CARRY_SET");
									}
									break;

									case 4:
									{
										fprintf(fh,"PARITY_ODD");
									}
									break;

									case 5:
									{
										fprintf(fh,"PARITY_EVEN");
									}
									break;

									case 6:
									{
										fprintf(fh,"POSITIVE");
									}
									break;

									case 7:
									{
										fprintf(fh,"MINUS");
									}
									break;
								}
				
								fprintf(fh,")\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"RETURN();\r\n");
								fprintf(fh,"Z80_UpdateCycles(4);\r\n");
								fprintf(fh,"}\r\nelse\r\n{\r\n");
								fprintf(fh,"ADD_PC(1);\r\n");
								fprintf(fh,"Z80_UpdateCycles(2);\r\n");
								fprintf(fh,"}\r\n");
				//			sprintf(OutputString,"RET %s",
				//				ConditionCodes[((Opcode>>3) & 0x07)]);

						}
						break;

						case 1:
						{
							if ((Opcode & (1<<3))==0)
							{
								/* 11qq0001 - POP qq */
				
								fprintf(fh,"%s = POP();\r\n",RegC[((Opcode>>4) & 0x03)]);

								//				sprintf(OutputString,"POP %s",
				//					RegC[((Opcode>>4) & 0x03)]);
							}
							else
							{	
								/* 11001001 - RET */
								/* 11011001 - EXX */
								/* 11101001 - JP (HL) */
								/* 11111001 - LD SP,HL */

								if (Opcode==0x0c9)
								{
									fprintf(fh,"RETURN();\r\n");
								}

								if (Opcode==0x0d9)
								{
									fprintf(fh,"SWAP(R.DE.W, R.altDE.W);\r\n");
									fprintf(fh,"SWAP(R.HL.W, R.altHL.W);\r\n");
									fprintf(fh,"SWAP(R.BC.W, R.altBC.W);\r\n");
								}

								if (Opcode==0x0e9)
								{
									fprintf(fh,"JP_rp(R.HL.W);\r\n");
								}

								if (Opcode==0x0f9)
								{
									fprintf(fh,"LD_SP_rp(R.HL.W);\r\n");
								}

				//				sprintf(OutputString,
				//					MiscMneumonics3[((Opcode>>4) & 0x03)]);
							}
						}
						break;
				
						case 2:
						{
							/* 11 ccc 010 - JP cc,nnnn */
								fprintf(fh,"if (");

								switch ((Opcode>>3) & 0x07)
								{
									case 0:
									{
										fprintf(fh,"ZERO_NOT_SET");
									}
									break;

									case 1:
									{
										fprintf(fh,"ZERO_SET");
									}
									break;

									case 2:
									{
										fprintf(fh,"CARRY_NOT_SET");
									}
									break;

									case 3:
									{
										fprintf(fh,"CARRY_SET");
									}
									break;

									case 4:
									{
										fprintf(fh,"PARITY_ODD");
									}
									break;

									case 5:
									{
										fprintf(fh,"PARITY_EVEN");
									}
									break;

									case 6:
									{
										fprintf(fh,"POSITIVE");
									}
									break;

									case 7:
									{
										fprintf(fh,"MINUS");
									}
									break;
								}
				
								fprintf(fh,")\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"JP();\r\n");
								fprintf(fh,"}\r\nelse\r\n{\r\n");
								fprintf(fh,"ADD_PC(3);\r\n");
								fprintf(fh,"}\r\n");
								fprintf(fh,"Z80_UpdateCycles(3);\r\n");
				//			sprintf(OutputString,"JP %s,#%04x",
				//				ConditionCodes[((Opcode>>3) & 0x07)],
				//				Diss_GetWord(DisAddr));
								
						}
						break;

						case 3:
						{
							/* 11001011 - CB prefix */
							if ((Opcode & (3<<4))==(1<<4))
							{
//								unsigned char PortByte;

//								PortByte = Z80_RD_MEM(DisAddr);

								/* 11011011 - IN A,(n) */
								/* 11010011 - OUT (n),A */

								if ((Opcode & (1<<3))!=0)
								{
									/* 11011011 - IN A,(n) */
						
									fprintf(fh,"IN_A_n();\r\n");

						//			sprintf(OutputString,"IN A,(#%02x)",
						//				PortByte);
								}
								else
								{
									fprintf(fh,"OUT_n_A();\r\n");
									/* 11010011 - OUT (n),A */
						//			sprintf(OutputString,"OUT (#%02x),A",
						//				PortByte);
								}
		
							}	
							else if (Opcode == 0x0c3) 
							{
								/* 11000011 - JP nn */
								fprintf(fh,"JP();\r\n");
						//		sprintf(OutputString,"JP #%04x",
						//			Diss_GetWord(DisAddr));
							}
							else
							{
								/* 11100011 - EX (SP).HL */
								/* 11101011 - EX DE,HL */
							
								/* 11110011 - DI */
								/* 11111011 - EI */

								switch (Opcode)
								{
									case 0x0f3:
									{
										fprintf(fh,"DI();\r\n");
									}
									break;

									case 0x0fb:
									{
										fprintf(fh,"EI();\r\n");
									}
									break;

									case 0x0eb:
									{
										fprintf(fh,"SWAP(R.HL.W,R.DE.W);\r\n");
									}
									break;

									case 0x0e3:
									{
										fprintf(fh,"EX_SP_rr(R.HL.W);\r\n");
									}
									break;
								}


						//		sprintf(OutputString,"%s",
						//			MiscMneumonics9[(((Opcode>>3) & 0x07)-4)]);
							}

						}
						break;

						case 4:
						{
							/* 11 ccc 100 - CALL cc,nnnn */
								fprintf(fh,"if (");

								switch ((Opcode>>3) & 0x07)
								{
									case 0:
									{
										fprintf(fh,"ZERO_NOT_SET");
									}
									break;

									case 1:
									{
										fprintf(fh,"ZERO_SET");
									}
									break;

									case 2:
									{
										fprintf(fh,"CARRY_NOT_SET");
									}
									break;

									case 3:
									{
										fprintf(fh,"CARRY_SET");
									}
									break;

									case 4:
									{
										fprintf(fh,"PARITY_ODD");
									}
									break;

									case 5:
									{
										fprintf(fh,"PARITY_EVEN");
									}
									break;

									case 6:
									{
										fprintf(fh,"POSITIVE");
									}
									break;

									case 7:
									{
										fprintf(fh,"MINUS");
									}
									break;
								}
				
								fprintf(fh,")\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"CALL();\r\n");
								fprintf(fh,"Z80_UpdateCycles(5);\r\n");
								fprintf(fh,"}\r\nelse\r\n{\r\n");
								fprintf(fh,"ADD_PC(3);\r\n");
								fprintf(fh,"Z80_UpdateCycles(3);\r\n");
								fprintf(fh,"}\r\n");
						//	sprintf(OutputString,"CALL %s,#%04x",
						//		ConditionCodes[((Opcode>>3) & 0x07)],
						//		Diss_GetWord(DisAddr));
						}
						break;


						case 5:
						{
							/* 11qq0101 - PUSH qq */
							if ((Opcode & (1<<3))==0)
							{
								/* 11qq0101 - PUSH qq */
						
								fprintf(fh,"PUSH(%s);\r\n",RegC[((Opcode>>4) & 0x03)]);

								//		sprintf(OutputString,"PUSH %s",
						//			RegC[((Opcode>>4) & 0x03)]);

							}
							else
							{
								/* 11001101 - CALL nn */
								/* 11011101 - DD */
								/* 11101101 - ED */
								/* 11111101 - FD */

								fprintf(fh,"CALL();\r\n");

						//		sprintf(OutputString,"CALL #%04x",
						//			Diss_GetWord(DisAddr));
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

						switch (((Opcode>>3) & 0x07))
						{
							case 0:
							{
								fprintf(fh,"ADD_A_n();\r\n");
							}
							break;

							case 1:
							{	
								fprintf(fh,"ADC_A_n();\r\n");
							}
							break;
							
							case 2:
							{
								fprintf(fh,"SUB_A_n();\r\n");
							}
							break;

							case 3:
							{
								fprintf(fh,"SBC_A_n();\r\n");
							}
							break;

							case 4:
							{
								fprintf(fh,"AND_A_n();\r\n");
							}
							break;

							case 5:
							{
								fprintf(fh,"XOR_A_n();\r\n");
							}
							break;
							case 6:
							{
								fprintf(fh,"OR_A_n();\r\n");
							}
							break;
							case 7:
							{
								fprintf(fh,"CP_A_n();\r\n");
							}
							break;
						}

						//	Diss_strcat(ArithmeticMneumonics[((Opcode>>3) & 0x07)]);
						//	Diss_space();
						//	Diss_strcat(RegA[7]);
						//	Diss_comma();
						//	Diss_WriteHexByte(Data);
						//	Diss_endstring();
						}
						break;

						case 7:
						{
							/* 11ttt111 - RST */
							fprintf(fh,"RST(0x0%04x);\r\n",(Opcode & 0x038));
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
