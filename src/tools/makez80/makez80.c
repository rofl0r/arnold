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

/* This file will generate z80.c */
#include <stdio.h>
#include <stdlib.h>
const char *Z80_INC_REFRESH = "\
        R.R+=%d;\r\n \
";

typedef unsigned short Z80_WORD;
typedef unsigned char  Z80_BYTE;
typedef int BOOL;
extern int	Z80_GetOpcodeCountForInstruction(int Addr);
int	Diss_Index_Get8BitRegCount(unsigned char OpcodeIndex);
static BOOL Diss_Index_IsRegIndex(unsigned char RegIndex);
static BOOL	Diss_Index_IsReg8Bit(unsigned char RegIndex);


#ifndef TRUE
#define TRUE (1==1)
#endif

#ifndef FALSE
#define FALSE (1==0)
#endif

const char *Z80_ADD_PC = "\
		R.PC.W.l+=%d; \
";

unsigned char Memory[256];

Z80_BYTE	Z80_RD_MEM(Z80_WORD Addr)
{
	return Memory[Addr];
}

BOOL InterruptMode;
int InterruptAddr;

void	DoPrefixIgnore(FILE *fh)
{
	fprintf(fh,"R.Flags &= ~Z80_CHECK_INTERRUPT_FLAG;\r\n");
	fprintf(fh,Z80_ADD_PC,1);
	fprintf(fh,Z80_INC_REFRESH,1);
	fprintf(fh,"Cycles = 1;\r\n");
//	Z80_UpdateCycles(1)
}


/* overflow caused, when both are + or -, and result is different. */
#define SET_OVERFLOW_FLAG_A_ADD(Reg, Result) \
        Z80_FLAGS_REG &= (0x0ff^Z80_PARITY_FLAG);           \
        Z80_FLAGS_REG |= (((R.AF.B.h^Reg^0x080)&(Reg^Result)&0x080)>>(7-Z80_PARITY_FLAG_BIT))

#define SET_OVERFLOW_FLAG_A_SUB(Reg, Result) \
        Z80_FLAGS_REG &= (0x0ff^Z80_PARITY_FLAG);                   \
        Z80_FLAGS_REG |= (((Reg^R.AF.B.h)&(Reg^Result)&0x080)>>(7-Z80_PARITY_FLAG_BIT))

#define SET_HALFCARRY(Reg, Result)              \
        Z80_FLAGS_REG &= (0x0ff^Z80_HALFCARRY_FLAG);        \
        Z80_FLAGS_REG |= ((Reg^R.AF.B.h^Result) & Z80_HALFCARRY_FLAG)

/*
#define SET_OVERFLOW_INC(Register, Reg,Result) \
        Z80_FLAGS_REG &= (0x0ff^Z80_PARITY_FLAG);           \
        Z80_FLAGS_REG |= (((Register^Reg^0x080)&(Reg^Result)&0x080)>>5)
*/

/*#define SET_OVERFLOW_DEC(Register,Reg, Result) \
        Z80_FLAGS_REG &= (0x0ff^Z80_PARITY_FLAG);                   \
        Z80_FLAGS_REG |= (((Register^Reg)&(Reg^Result)&0x080)>>5)
*/

/* halfcarry carry out of bit 11 */
#define SET_HALFCARRY_HL_ADD(Reg, Result)       \
        Z80_FLAGS_REG &= (0x0ff^Z80_HALFCARRY_FLAG);        \
        Z80_FLAGS_REG |= ((R.HL.W & 0x08000)>>(8+4))


#define SET_OVERFLOW_FLAG_HL_ADD(Reg, Result) \
        Z80_FLAGS_REG &= (0x0ff^Z80_PARITY_FLAG);           \
        Z80_FLAGS_REG |= (((Reg^R.HL.W^0x08000)&(Reg^Result)&0x08000)>>(15-Z80_PARITY_FLAG_BIT))

#define SET_OVERFLOW_FLAG_HL_SUB(Reg, Result) \
        Z80_FLAGS_REG &= (0x0ff^Z80_PARITY_FLAG);           \
        Z80_FLAGS_REG |= (((Reg^R.HL.W)&(Reg^Result)&0x8000)>>(15-Z80_PARITY_FLAG_BIT))


#if 0
#define SET_ZERO_FLAG(Register)         \
{                                                                       \
Z80_FLAGS_REG = Z80_FLAGS_REG & (0x0ff^Z80_ZERO_FLAG);          \
                                                                \
if (Register==0)                \
{                                                               \
        Z80_FLAGS_REG |= Z80_ZERO_FLAG;             \
}                                                               \
}
#endif

#define SET_ZERO_FLAG(Register) \
{                                                                               \
        Z80_FLAGS_REG = Z80_FLAGS_REG & (0x0ff^(Z80_ZERO_FLAG | Z80_SIGN_FLAG)); \
        Z80_FLAGS_REG = Z80_FLAGS_REG | (ZeroSignTable[Register & 0x0ff] & Z80_ZERO_FLAG); \
}


#define SET_ZERO_FLAG16(Register)               \
{                                                               \
Z80_FLAGS_REG &= (0x0ff^Z80_ZERO_FLAG);     \
                                                                \
        if ((Register & 0x0ffff)==0)            \
        {                                                               \
                Z80_FLAGS_REG |= Z80_ZERO_FLAG;             \
        }                                                               \
}
#define SET_UNDOC_BITS() \
{ \
Z80_FLAGS_REG = Z80_FLAGS_REG & ((1<<5)|(1<<3)); \
Z80_FLAGS_REG = Z80_FLAGS_REG|(Z80.Memptr.B.h&((1<<5)|(1<<3))); \
}

#define SET_SIGN_FLAG(Register)         \
{                                                                       \
Z80_FLAGS_REG = Z80_FLAGS_REG & (0x0ff^Z80_SIGN_FLAG);          \
Z80_FLAGS_REG = Z80_FLAGS_REG | (Register & 0x080);     \
}

#define SET_SIGN_FLAG16(Register)       \
{                                                                       \
Z80_FLAGS_REG = Z80_FLAGS_REG & (0x0ff^Z80_SIGN_FLAG);          \
Z80_FLAGS_REG = Z80_FLAGS_REG | ((Register & 0x08000)>>8); \
}

#define SET_CARRY_FLAG_ADD(Result)              \
{                                                                       \
Z80_FLAGS_REG = Z80_FLAGS_REG & (0x0ff^Z80_CARRY_FLAG); \
Z80_FLAGS_REG = Z80_FLAGS_REG | ((Result>>8) & 0x01); \
}

#define SET_CARRY_FLAG_ADD16(Result)    \
{                                                                       \
Z80_FLAGS_REG = Z80_FLAGS_REG & (0x0ff^Z80_CARRY_FLAG); \
Z80_FLAGS_REG = Z80_FLAGS_REG | ((Result>>16) & 0x01); \
}

#define SET_CARRY_FLAG_SUB(Result)      \
{                                                                               \
Z80_FLAGS_REG = Z80_FLAGS_REG & (0x0ff^Z80_CARRY_FLAG); \
Z80_FLAGS_REG = Z80_FLAGS_REG | ((Result>>8) & 0x01); \
}

#define SET_CARRY_FLAG_SUB16(Result)    \
{                                                                               \
Z80_FLAGS_REG = Z80_FLAGS_REG & (0x0ff^Z80_CARRY_FLAG); \
Z80_FLAGS_REG = Z80_FLAGS_REG | ((Result>>16) & 0x01); \
}


/* set zero and sign flag */
#define SET_ZERO_SIGN(Register)                 \
{                                                                               \
        Z80_FLAGS_REG = Z80_FLAGS_REG & (0x0ff^(Z80_ZERO_FLAG | Z80_SIGN_FLAG)); \
        Z80_FLAGS_REG = Z80_FLAGS_REG | ZeroSignTable[Register & 0x0ff]; \
}

/* set zero, sign and parity flag */
#define SET_ZERO_SIGN_PARITY(Register)  \
{                                                                               \
        Z80_FLAGS_REG = Z80_FLAGS_REG & (0x0ff^(Z80_ZERO_FLAG | Z80_SIGN_FLAG | Z80_PARITY_FLAG)); \
        Z80_FLAGS_REG = Z80_FLAGS_REG | ZeroSignParityTable[Register & 0x0ff]; \
}


#define SET_PARITY_FLAG(Register)               \
{                                                                               \
        Z80_FLAGS_REG &= (0x0ff^Z80_PARITY_FLAG); /*(UNUSED1_FLAG | UNUSED2_FLAG | CARRY_FLAG | ZERO_FLAG | SIGN_FLAG | SUBTRACT_FLAG | HALFCARRY_FLAG);*/          \
        Z80_FLAGS_REG |= ParityTable[Register & 0x0ff];     \
}







/*------------------*/
/* RES */

#define RES(AndMask,Register) \
        Register = Register & (~AndMask); \

#define RES_REG(AndMask, Register) \
{                                                                       \
        RES(AndMask, Register);                 \
                                                                        \
}

#define RES_HL(AndMask)                 \
{                                                               \
        /*Z80_BYTE Data;        */                      \
                                                                \
        R.TempByte = Z80_RD_BYTE(R.HL.W);       \
                                                                \
        RES(AndMask, R.TempByte);                       \
                                                                \
        Z80_WR_BYTE(R.HL.W, R.TempByte); \
}

#define RES_INDEX(AndMask,IndexReg)     \
{                                                               \
        /*Z80_BYTE Data;*/                              \
		SETUP_INDEXED_ADDRESS(IndexReg); \
                                                                \
        R.TempByte = RD_BYTE_INDEX();   \
                                                                \
        RES(AndMask, R.TempByte);                       \
                                                                \
        WR_BYTE_INDEX(R.TempByte);     \
}



/* SET */

#define SET(OrMask, Register) \
        Register = Register | OrMask;   \

#define SET_REG(OrMask, Register) \
{                                                                \
        SET(OrMask, Register)           \
                                                                \
}

#define SET_HL(OrMask)                  \
{                                                               \
        /*Z80_BYTE Data;*/                              \
                                                                \
        R.TempByte = Z80_RD_BYTE(R.HL.W); \
                                                                \
        SET(OrMask, R.TempByte);                        \
                                                                \
        Z80_WR_BYTE(R.HL.W, R.TempByte);  \
}


#define SET_INDEX(OrMask, IndexReg)     \
{                                                               \
        /*Z80_BYTE      Data;*/                         \
                                                                \
		SETUP_INDEXED_ADDRESS(IndexReg); \
        R.TempByte = RD_BYTE_INDEX();   \
                                                                \
        SET(OrMask, R.TempByte);                        \
                                                                \
        WR_BYTE_INDEX(R.TempByte);     \
}



/*------------------*/
#define SHIFTING_FLAGS(Register)        \
        SET_ZERO_SIGN_PARITY(Register)  \
        Z80_FLAGS_REG &= Z80_SIGN_FLAG | Z80_ZERO_FLAG | Z80_PARITY_FLAG | Z80_CARRY_FLAG


#define SET_CARRY_LEFT_SHIFT(Register)          \
Z80_FLAGS_REG = Z80_FLAGS_REG & (0x0ff^Z80_CARRY_FLAG); \
Z80_FLAGS_REG = Z80_FLAGS_REG | ((Register>>7) & 0x01)



#define SET_CARRY_RIGHT_SHIFT(Register)         \
Z80_FLAGS_REG = Z80_FLAGS_REG & (0x0ff^Z80_CARRY_FLAG); \
Z80_FLAGS_REG = Z80_FLAGS_REG | (Register & 0x001)




#define RL(Register)                            \
{                                                                       \
        Z80_BYTE Carry;                                 \
                                                                        \
        Carry = (Z80_FLAGS_REG & (Z80_BYTE)Z80_CARRY_FLAG); \
                                                                        \
        SET_CARRY_LEFT_SHIFT(Register); \
                                                                        \
        Register=Register<<1;                   \
                                                                        \
        Register=Register | Carry;                              \
}

#define RL_WITH_FLAGS(Register)         \
{                                                                       \
        RL(Register);                                   \
                                                                        \
        SHIFTING_FLAGS(Register);               \
}

#define RL_REG(Register)                        \
{                                                                       \
        RL_WITH_FLAGS(Register);                \
}

#define RL_HL()								\
{											\
	/*Z80_BYTE      Data;*/				\
											\
        R.TempByte = Z80_RD_BYTE(R.HL.W);	\
											\
        RL_WITH_FLAGS(R.TempByte);			\
											\
        Z80_WR_BYTE(R.HL.W,R.TempByte);		\
}


#define RL_INDEX(IndexReg)                      \
{                                                                       \
        /*Z80_BYTE Data;        */                              \
                                                                        \
 		SETUP_INDEXED_ADDRESS(IndexReg); \
       R.TempByte = RD_BYTE_INDEX();   \
                                                                        \
        RL_WITH_FLAGS(R.TempByte);                                              \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);             \
}                                                                       \


#define RR(Register)                            \
{                                                                       \
        Z80_BYTE Carry;                                         \
                                                                        \
        Carry = Z80_FLAGS_REG & Z80_CARRY_FLAG;     \
                                                                        \
        SET_CARRY_RIGHT_SHIFT(Register);                        \
                                                                        \
        Register=Register>>1;                   \
                                                                        \
        Register=Register | (Carry<<7); \
                                                                        \
}

#define RR_WITH_FLAGS(Register)         \
{                                                                       \
        RR(Register);                                   \
                                                                        \
        SHIFTING_FLAGS(Register);               \
}

#define RR_REG(Register)                        \
{                                                                       \
        RR_WITH_FLAGS(Register);                \
}

#define RR_HL()									\
{												\
        /*Z80_BYTE      Data;	*/				\
												\
        R.TempByte = Z80_RD_BYTE(R.HL.W);		\
												\
        RR_WITH_FLAGS(R.TempByte);				\
												\
        Z80_WR_BYTE(R.HL.W,R.TempByte);			\
}


#define RR_INDEX(IndexReg)                      \
{                                                                       \
        /*Z80_BYTE Data;*/                                      \
                                                                        \
 		SETUP_INDEXED_ADDRESS(IndexReg); \
       R.TempByte = RD_BYTE_INDEX();   \
                                                                        \
        RR_WITH_FLAGS(R.TempByte);                                              \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);             \
}                                                                       \

/* rol doesn't set sign or zero! */



#define RLC(Register)                           \
{                                                                       \
        Z80_BYTE        OrByte;                         \
                                                                        \
        OrByte = (Register>>7) & 0x01;  \
                                                                        \
        SET_CARRY_LEFT_SHIFT(Register);                 \
                                                                        \
        Register=Register<<1;                   \
        Register=Register | OrByte;                             \
}

#define RLC_WITH_FLAGS(Register)        \
{                                                                       \
        RLC(Register);                                  \
                                                                        \
        SHIFTING_FLAGS(Register);               \
}

#define RLC_REG(Register)                       \
{                                                                       \
        RLC_WITH_FLAGS(Register);       \
}

#define RLC_HL()						\
{											\
        R.TempByte = Z80_RD_BYTE(R.HL.W);	\
											\
        RLC_WITH_FLAGS(R.TempByte);			\
											\
        Z80_WR_BYTE(R.HL.W,R.TempByte);		\
}


#define RLC_INDEX(IndexReg)                     \
{                                                                       \
        /*Z80_BYTE      Data;   */                              \
                                                                        \
		SETUP_INDEXED_ADDRESS(IndexReg); \
        R.TempByte = RD_BYTE_INDEX();   \
                                                                        \
        RLC_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);             \
}                                                                       \

#define RRC(Register)                           \
{                                                                       \
        Z80_BYTE        OrByte;                                 \
                                                                        \
        OrByte = Register & 0x001;              \
                                                                        \
        SET_CARRY_RIGHT_SHIFT(Register);                        \
                                                                        \
        Register=Register>>1;                   \
                                                                        \
        Register=Register | (OrByte<<7);                                \
}

#define RRC_WITH_FLAGS(Register)        \
{                                                                       \
        RRC(Register);                                  \
                                                                        \
        SHIFTING_FLAGS(Register);               \
}

#define RRC_REG(Register)                       \
{                                                                       \
        RRC_WITH_FLAGS(Register);       \
}

#define  RRC_HL()			\
{							\
        R.TempByte = Z80_RD_BYTE(R.HL.W);	\
											\
        RRC_WITH_FLAGS(R.TempByte);			\
											\
        Z80_WR_BYTE(R.HL.W,R.TempByte);		\
}


#define RRC_INDEX(IndexReg)                     \
{                                                                       \
        /*Z80_BYTE      Data;   */                      \
                                                                        \
		SETUP_INDEXED_ADDRESS(IndexReg); \
        R.TempByte = RD_BYTE_INDEX();   \
                                                                        \
        RRC_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);             \
}                                                                       \

#define SLA(Register)							\
{												\
	Z80_BYTE	Reg;							\
	Z80_BYTE	Flags;							\
	Reg = Register;								\
	Flags = ((Reg>>7) & 0x01);	/* carry */				\
	Reg = (Reg<<1);								\
	Flags = Flags | ZeroSignParityTable[Reg];	/* sign, zero, parity, f5, f3 */ \
	Register = Reg;								\
	Z80_FLAGS_REG = Flags;							\
}




#define SLA_WITH_FLAGS(Register)        \
{                                                                       \
        SLA(Register);                                  \
}

#define SLA_REG(Register)                       \
{                                                                       \
        SLA_WITH_FLAGS(Register);               \
}

#define SLA_HL()								\
{												\
        R.TempByte = Z80_RD_BYTE(R.HL.W);		\
        SLA_WITH_FLAGS(R.TempByte);				\
        Z80_WR_BYTE(R.HL.W,R.TempByte);			\
}


#define SLA_INDEX(IndexReg)                     \
{                                                                       \
        /*Z80_BYTE      Data;   */                      \
                                                                        \
		SETUP_INDEXED_ADDRESS(IndexReg); \
        R.TempByte = RD_BYTE_INDEX();   \
                                                                        \
        SLA_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);     \
}                                                                       \


#define SRA(Register)							\
{												\
	Z80_BYTE	Reg;							\
	Z80_BYTE	Flags;							\
	Z80_BYTE	OrByte;							\
	Reg = Register;								\
	Flags = Reg & 0x01;	/* carry */				\
	OrByte = Reg & 0x080;						\
	Reg = (Reg>>1) | OrByte;					\
	Flags = Flags | ZeroSignParityTable[Reg];	/* sign, zero, parity, f5, f3 */ \
	Register = Reg;								\
	Z80_FLAGS_REG = Flags;							\
}



#define SRA_WITH_FLAGS(Register)        \
{                                                                       \
        SRA(Register);                                  \
}

#define SRA_REG(Register)                       \
{                                                                       \
        SRA_WITH_FLAGS(Register);               \
}

#define SRA_HL()							\
{											\
        R.TempByte = Z80_RD_BYTE(R.HL.W);	\
        SRA_WITH_FLAGS(R.TempByte);			\
        Z80_WR_BYTE(R.HL.W,R.TempByte);		\
}

#define SRA_INDEX(Index)                \
{                                                                       \
        /*Z80_BYTE      Data;*/                                 \
                                                                        \
		SETUP_INDEXED_ADDRESS(Index); \
        R.TempByte = RD_BYTE_INDEX();      \
                                                                        \
        SRA_WITH_FLAGS(R.TempByte);                                             \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);                \
}                                                                       \




#define SRL(Register)                           \
{												\
	Z80_BYTE	Reg;							\
	Z80_BYTE	Flags;							\
	Reg = Register;								\
	Flags = Reg & 0x001; /* carry */			\
	Reg = Reg>>1;								\
	Flags = Flags | ZeroSignParityTable[Reg];	/* sign, zero, parity, f5, f3 */	\
	Register = Reg;								\
	Z80_FLAGS_REG = Flags;							\
}

#define SRL_WITH_FLAGS(Register)        \
{                                                                       \
        SRL(Register);                                  \
                                                                        \
}

#define SRL_REG(Register)                       \
{                                                                       \
        SRL_WITH_FLAGS(Register);               \
}

#define SRL_HL()								\
{												\
        R.TempByte = Z80_RD_BYTE(R.HL.W);		\
        SRL_WITH_FLAGS(R.TempByte);				\
        Z80_WR_BYTE(R.HL.W,R.TempByte);			\
}

#define SRL_INDEX(Index)                        \
{                                                                       \
        /*Z80_BYTE      Data;*/                         \
                                                                        \
		SETUP_INDEXED_ADDRESS(Index); \
        R.TempByte = RD_BYTE_INDEX();      \
                                                                        \
        SRL_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);                \
}                                                                       \


#define SLL(Register)							\
{												\
	Z80_BYTE	Reg;							\
	Z80_BYTE	Flags;							\
	Reg = Register;								\
	Flags = (Reg>>7) & 0x01;	/* carry */				\
	Reg = ((Reg<<1) | 1);								\
	Flags = Flags | ZeroSignParityTable[Reg];	/* sign, zero, parity, f5, f3 */ \
	Register = Reg;								\
	Z80_FLAGS_REG = Flags;							\
}




#define SLL_WITH_FLAGS(Register)        \
{                                                                       \
        SLL(Register);                                  \
}

#define SLL_REG(Register)                       \
{                                                                       \
        SLL_WITH_FLAGS(Register);               \
}

#define SLL_HL()							\
{											\
        R.TempByte = Z80_RD_BYTE(R.HL.W);	\
        SLL_WITH_FLAGS(R.TempByte);			\
        Z80_WR_BYTE(R.HL.W,R.TempByte);		\
}

#define SLL_INDEX(IndexReg)                     \
{                                                                       \
        /*Z80_BYTE      Data;   */                      \
                                                                        \
		SETUP_INDEXED_ADDRESS(IndexReg); \
        R.TempByte = RD_BYTE_INDEX();   \
                                                                        \
        SLL_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);     \
}                                                                       \

/*-----------------*/

#define A_SHIFTING_FLAGS \
        Z80_FLAGS_REG = Z80_FLAGS_REG & (Z80_SIGN_FLAG | Z80_ZERO_FLAG | Z80_PARITY_FLAG | Z80_CARRY_FLAG)

/*---------------*/

#define SET_LOGIC_FLAGS                         \
        SET_ZERO_SIGN_PARITY(R.AF.B.h); \
        Z80_FLAGS_REG = Z80_FLAGS_REG & (Z80_ZERO_FLAG | Z80_PARITY_FLAG | Z80_SIGN_FLAG)


#define AND_A_X(Register)					\
{											\
	Z80_BYTE	Flags;						\
											\
	R.AF.B.h = R.AF.B.h & Register;			\
	Flags = ZeroSignParityTable[R.AF.B.h];	\
	Flags = Flags | Z80_HALFCARRY_FLAG;		\
	Z80_FLAGS_REG = Flags;						\
}


#define AND_A_R(Register)                       \
{                                                                       \
        AND_A_X(Register);                              \
}


#define AND_A_INDEX(Index)              \
{                                                               \
        /*Z80_BYTE      Data;*/                         \
                                                                \
                                                                \
 		SETUP_INDEXED_ADDRESS(Index); \
       R.TempByte = RD_BYTE_INDEX(Index);      \
                                                                                \
        AND_A_X(R.TempByte);                                    \
}





#define XOR_A_X(Register)					\
{											\
	Z80_BYTE	Flags;						\
											\
	R.AF.B.h = R.AF.B.h ^ Register;			\
	Flags = ZeroSignParityTable[R.AF.B.h];	\
	Z80_FLAGS_REG = Flags;						\
}


#define XOR_A_R(Register)                       \
{                                                                       \
        XOR_A_X(Register);                              \
}


#define XOR_A_INDEX(Index)              \
{                                                               \
        /*Z80_BYTE      Data;*/                         \
                                                                \
		SETUP_INDEXED_ADDRESS(Index); \
        R.TempByte = RD_BYTE_INDEX(Index);      \
                                                                        \
        XOR_A_X(R.TempByte);                                    \
}



#define OR_A_X(Register)					\
{											\
	Z80_BYTE	Flags;						\
											\
	R.AF.B.h = R.AF.B.h | Register;			\
	Flags = ZeroSignParityTable[R.AF.B.h];	\
	Z80_FLAGS_REG = Flags;						\
}


#define OR_A_R(Register)                        \
{                                                                       \
        OR_A_X(Register);                               \
}



#define OR_A_INDEX(Index)               \
{                                                               \
        /*Z80_BYTE      Data;*/                         \
                                                                \
		SETUP_INDEXED_ADDRESS(Index); \
        R.TempByte = RD_BYTE_INDEX(Index);      \
                                                                        \
        OR_A_X(R.TempByte);                                     \
}

/*--------------*/

/* INC */
#define INC_X(Register)                                                         \
{                                                                                                       \
        Z80_BYTE Flags;						\
		Flags = Z80_FLAGS_REG;					\
		Flags  = Flags & Z80_CARRY_FLAG;	 \
        if ((Z80_BYTE)(Register & 0x0f)==(Z80_BYTE)0x0f)                                        \
        {                                                                                               \
                Flags |= Z80_HALFCARRY_FLAG;                        \
        }                                                                                               \
                                                                                                        \
        if ((Z80_BYTE)Register==(Z80_BYTE)0x07f)                                        \
        {                                                                                               \
                Flags |= Z80_OVERFLOW_FLAG;                         \
        }                                                                                               \
        Register++;                                                                                     \
		Flags |= ZeroSignTable[Register];								\
		Flags |= Register & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);		\
		Z80_FLAGS_REG = Flags;												\
        \
}



#define INC_R(Register)                         \
{                                                                       \
        INC_X(Register);                                \
                                                                        \
}

#define INC_HL_()		\
{											\
        R.TempByte = Z80_RD_BYTE(R.HL.W);	\
        INC_X(R.TempByte);					\
        Z80_WR_BYTE(R.HL.W,R.TempByte);		\
}

#define _INC_INDEX_(Index)                      \
{                                                                       \
        /*Z80_BYTE      Data;*/                         \
                                                                        \
 		SETUP_INDEXED_ADDRESS(Index); \
       R.TempByte = RD_BYTE_INDEX();      \
                                                                        \
        INC_X(R.TempByte);                                      \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);                \
}


/* INC */
#define DEC_X(Register)                                                         \
{                                                                                                       \
        Z80_BYTE Flags;						\
		Flags = Z80_FLAGS_REG;					\
		Flags  = Flags & Z80_CARRY_FLAG;	 \
		Flags = Flags | Z80_SUBTRACT_FLAG;	\
        if ((Z80_BYTE)(Register & 0x0f)==0)                                        \
        {                                                                                               \
                Flags |= Z80_HALFCARRY_FLAG;                        \
        }                                                                                               \
                                                                                                        \
        if ((Z80_BYTE)Register==(Z80_BYTE)0x080)                                        \
        {                                                                                               \
                Flags |= Z80_OVERFLOW_FLAG;                         \
        }                                                                                               \
        Register--;                                                                                     \
		Flags |= ZeroSignTable[Register];								\
		Flags |= Register & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);		\
		Z80_FLAGS_REG = Flags;												\
        \
}

#if 0
/* DEC */
#define DEC_X(Register)                                                                 \
{       Z80_BYTE		Flags;                                                                                                        \
        register Z80_BYTE		Reg;                                                                                                        \
		Reg=Register;	\
		Flags = Z80_FLAGS_REG;	\
        Flags = Flags & ~(Z80_HALFCARRY_FLAG | Z80_OVERFLOW_FLAG);             \
        if ((Z80_BYTE)Reg==(Z80_BYTE)0x080)                                                                \
        {                                                                                                       \
                Flags |= Z80_OVERFLOW_FLAG;                                 \
        }                                                                                                       \
        if ((Z80_BYTE)(Reg & 0x0f)==(Z80_BYTE)0x00)                                                \
        {                                                                                                       \
                Flags|= Z80_HALFCARRY_FLAG;                                \
        }                                                                                                       \
                                                                                                                \
		Flags |= Z80_SUBTRACT_FLAG;	\
		Z80_FLAGS_REG = Flags;			\
		Register = Reg-1;	\
        SET_ZERO_SIGN(Register);                                                        \
}
#endif

#define DEC_R(Register)                         \
{                                                                       \
        DEC_X(Register);                                \
                                                                        \
}

#define DEC_HL_()				\
{												\
        R.TempByte = Z80_RD_BYTE(R.HL.W);		\
        DEC_X(R.TempByte);						\
        Z80_WR_BYTE(R.HL.W,R.TempByte);			\
}


#define _DEC_INDEX_(Index)                      \
{                                                                       \
        /*Z80_BYTE      Data;*/                                 \
                                                                        \
		SETUP_INDEXED_ADDRESS(Index); \
        R.TempByte = RD_BYTE_INDEX();      \
                                                                        \
        DEC_X(R.TempByte);                                      \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);                \
}

/*-----------------*/







/*------------*/
/* Macros */


#define ADD_A_X(Register2)      \
{                                                                       \
        Z80_WORD        Result=0;                                       \
                                                                        \
        Result = (Z80_WORD)R.AF.B.h + (Z80_WORD)Register2;\
                                                                        \
        SET_OVERFLOW_FLAG_A_ADD(Register2,Result); \
        SET_CARRY_FLAG_ADD(Result);                     \
        SET_HALFCARRY(Register2, Result);       \
        R.AF.B.h = (Z80_BYTE)Result;            \
                                                                        \
        SET_ZERO_SIGN(R.AF.B.h);                \
                                                                        \
        Z80_FLAGS_REG = Z80_FLAGS_REG & (~Z80_SUBTRACT_FLAG); \
}

#define ADD_A_R(Register)                       \
{                                                                       \
        ADD_A_X(Register);                              \
}

#if 0
#define ADD_A_INDEX8(Register)          \
{                                                                       \
        ADD_A_X(Register);                              \
                                                                        \
        R.PC.W+=2;                                              \
}
#endif



#define ADC_A_X(Register)                       \
{                                                                       \
        Z80_WORD        Result=0;                                       \
                                                                        \
        Result = (Z80_WORD)R.AF.B.h + (Z80_WORD)Register + (Z80_WORD)(Z80_FLAGS_REG & Z80_CARRY_FLAG);      \
                                                                        \
        SET_OVERFLOW_FLAG_A_ADD(Register,Result); \
        SET_CARRY_FLAG_ADD(Result);                     \
        SET_HALFCARRY(Register, Result);        \
                                                                        \
        R.AF.B.h = (Z80_BYTE)Result;            \
                                                                        \
        SET_ZERO_SIGN(R.AF.B.h);                \
                                                                        \
        Z80_FLAGS_REG = Z80_FLAGS_REG & (~Z80_SUBTRACT_FLAG); \
}

#define ADC_A_R(Register)                       \
{                                                                       \
        ADC_A_X(Register);                              \
}

#if 0
#define ADC_A_INDEX8(Register)          \
{                                                                       \
        ADC_A_X(Register);                      \
                                                                        \
        R.PC.W+=2;                                              \
}
#endif

#define SUB_A_X(Register)                       \
{                                                                       \
        Z80_WORD        Result=0;                                       \
                                                                        \
        Result = (Z80_WORD)R.AF.B.h - (Z80_WORD)Register;       \
                                                                        \
        SET_OVERFLOW_FLAG_A_SUB(Register,Result); \
        SET_CARRY_FLAG_SUB(Result);                     \
        SET_HALFCARRY(Register, Result);        \
                                                                        \
        R.AF.B.h = (Z80_BYTE)Result;            \
                                                                        \
        SET_ZERO_SIGN(R.AF.B.h);                \
                                                                        \
        Z80_FLAGS_REG = Z80_FLAGS_REG | Z80_SUBTRACT_FLAG;              \
}

#define SUB_A_R(Register)                       \
{                                                                       \
        SUB_A_X(Register);                              \
}

#if 0
#define SUB_A_INDEX8(Register)          \
{                                                                       \
        SUB_A_X(Register);                              \
                                                                        \
        R.PC.W+=2;                                              \
}
#endif

#define CP_A_X(Register)                        \
{                                                                       \
        Z80_WORD        Result=0;                                       \
                                                                        \
        Result = (Z80_WORD)R.AF.B.h - (Z80_WORD)Register;       \
                                                                        \
        SET_OVERFLOW_FLAG_A_SUB(Register,Result); \
        SET_CARRY_FLAG_SUB(Result);                     \
        SET_HALFCARRY(Register, Result);        \
                                                                        \
        SET_ZERO_SIGN(Result);                  \
                                                                        \
        Z80_FLAGS_REG = Z80_FLAGS_REG | Z80_SUBTRACT_FLAG;              \
}

#define CP_A_R(Register)                        \
{                                                                       \
        CP_A_X(Register);                               \
}

#if 0
#define CP_A_INDEX8(Register)           \
{                                                                       \
        CP_A_X(Register);                               \
                                                                        \
        R.PC.W+=2;                                              \
}
#endif

#define SBC_A_X(Register)                       \
{                                                                       \
        Z80_WORD                Result=0;                                       \
                                                                        \
        Result = (Z80_WORD)R.AF.B.h - (Z80_WORD)Register - (Z80_WORD)(Z80_FLAGS_REG & Z80_CARRY_FLAG);      \
                                                                        \
        SET_OVERFLOW_FLAG_A_SUB(Register,Result);  \
        SET_CARRY_FLAG_SUB(Result);                     \
        SET_HALFCARRY(Register, Result);        \
                                                                        \
        R.AF.B.h = (Z80_BYTE)Result;            \
                                                                        \
        SET_ZERO_SIGN(R.AF.B.h);                \
                                                                        \
        Z80_FLAGS_REG = Z80_FLAGS_REG | Z80_SUBTRACT_FLAG;              \
}

#define SBC_A_R(Register)                       \
{                                                                       \
        SBC_A_X(Register);                              \
}

#if 0
#define SBC_A_INDEX8(Register)          \
{                                                                       \
        SBC_A_X(Register);                              \
                                                                        \
        R.PC.W+=2;                                              \
}
#endif


/* BIT n,r */
const char *Z80_BIT = " \r\n\
{\r\n\                                                                       \r\n\
	Z80_BYTE	Flags;					\r\n\
	const Z80_BYTE	Mask = (1<<BitIndex);				\r\n\
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ \r\n\
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ \r\n\
\r\n\
	R.TempByte = %s & Mask;		/* perform AND operation */ \r\n\
	/* handle SF,YF,XF */ \r\n\
	/* there will be 1 in the place of the bit if it is set */ \r\n\
	/* if bit 7 was tested there will be a 1 there, but not in 5 or 3 */ \r\n\
	/* if bit 5 was tested there will be a 1 there, but not in 7 or 3 */ \r\n\
	/* if bit 3 was tested there will be a 1 there, but not in 7 or 5 */ \r\n\
	Flags |= Result & (Z80_SIGN_FLAG | Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	\r\n\
	Result = (~Result) & Mask; /* if bit is 1, then ZF is reset */ \r\n\
	Flags |= ((Result>>BitIndex)<<Z80_ZERO_FLAG_BIT); /* ZF */ \r\n\
	Flags |= ((Result>>BitIndex)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ \r\n\
	Z80_FLAGS_REG = Flags; \r\n\
}\r\n";

/* BIT n,(HL), BIT n,(IX+d)*/
const Z80_BIT_MP = "\r\n\
{\r\n\
	Z80_BYTE	Flags;						\r\n\
	const Z80_BYTE	Mask = (1<<%d);				\r\n\
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ \r\n\
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ \r\n\
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ \r\n\
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); \r\n\
	Flags |= ((R.TempByte>>%d)<<Z80_ZERO_FLAG_BIT); /* ZF */ \r\n\
	Flags |= ((R.TempByte>>%d)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ \r\n\
	Z80_FLAGS_REG = Flags; \r\n\
}\r\n";


const char *ADD_RR_rr = "\
        Z80_LONG                Result=0;                               \
                                                                        \
        Result = (Z80_LONG)Register1 + (Z80_LONG)Register2;             \
                                                                        \
        SET_CARRY_FLAG_ADD16(Result);           \
                                                                        \
        Register1 = (Z80_WORD)Result;           \
                                                                        \
        Z80_FLAGS_REG = Z80_FLAGS_REG & (Z80_CARRY_FLAG | Z80_ZERO_FLAG | Z80_PARITY_FLAG | Z80_SIGN_FLAG); \
";


const char *ADC_HL_rr = "\
        Z80_LONG                Result=0;                                       \
                                                                        \
        Result = (Z80_LONG)R.HL.W + (Z80_LONG)Register + (Z80_LONG)(Z80_FLAGS_REG & Z80_CARRY_FLAG);                \
                                                                        \
        SET_OVERFLOW_FLAG_HL_ADD(Register,Result); \
        SET_CARRY_FLAG_ADD16(Result);           \
        /*SET_HALFCARRY_HL_ADD(Register, Result);*/     \
                                                                        \
        R.HL.W = (Z80_WORD)Result;              \
                                                                        \
        SET_SIGN_FLAG16(R.HL.W);                \
        SET_ZERO_FLAG16(R.HL.W);                \
                                                                        \
        Z80_FLAGS_REG = Z80_FLAGS_REG & (Z80_CARRY_FLAG | Z80_ZERO_FLAG | Z80_PARITY_FLAG | Z80_SIGN_FLAG); \
";


const char * SBC_HL_rr = "\
        Z80_LONG                Result=0;                                       \
                                                                        \
        Result = (Z80_LONG)R.HL.W - (Z80_LONG)Register - (Z80_LONG)(Z80_FLAGS_REG & Z80_CARRY_FLAG);                \
                                                                        \
        SET_OVERFLOW_FLAG_HL_SUB(Register,Result); \
        SET_CARRY_FLAG_SUB16(Result);                   \
        /*SET_HALFCARRY_HL_ADD(Register, Result);*/     \
                                                                        \
        R.HL.W = (Z80_WORD)Result;              \
                                                                        \
        SET_SIGN_FLAG16(R.HL.W);                \
        SET_ZERO_FLAG16(R.HL.W);                \
                                                                        \
        Z80_FLAGS_REG = Z80_FLAGS_REG & (Z80_CARRY_FLAG | Z80_ZERO_FLAG | Z80_PARITY_FLAG | Z80_SIGN_FLAG); \
        Z80_FLAGS_REG = Z80_FLAGS_REG | Z80_SUBTRACT_FLAG;              \
";

/* do a sla of index and copy into reg specified */
#define INDEX_CB_SLA_REG(IndexReg, Reg)         \
{                                                                       \
        /*Z80_BYTE      Data;*/                         \
                                                                        \
		SETUP_INDEXED_ADDRESS(IndexReg); \
        R.TempByte = RD_BYTE_INDEX();   \
                                                                        \
        SLA_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);     \
                                                                        \
        Reg = R.TempByte;                                               \
}

/* do a sra of index and copy into reg specified */
#define INDEX_CB_SRA_REG(IndexReg, Reg)         \
{                                                                       \
        /*Z80_BYTE      Data;*/                         \
                                                                        \
 		SETUP_INDEXED_ADDRESS(IndexReg); \
       R.TempByte = RD_BYTE_INDEX();   \
                                                                        \
        SRA_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);     \
                                                                        \
        Reg = R.TempByte;                                               \
}

/* do a sll of index and copy into reg specified */
#define INDEX_CB_SLL_REG(IndexReg, Reg)         \
{                                                                       \
        /*Z80_BYTE      Data;*/                         \
                                                                        \
 		SETUP_INDEXED_ADDRESS(IndexReg); \
       R.TempByte =  RD_BYTE_INDEX();  \
                                                                        \
        SLL_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);     \
                                                                        \
        Reg = R.TempByte;                                               \
}

/* do a srl of index and copy into reg specified */
#define INDEX_CB_SRL_REG(IndexReg, Reg)         \
{                                                                       \
        /*Z80_BYTE      Data;   */                      \
                                                                        \
 		SETUP_INDEXED_ADDRESS(IndexReg); \
       R.TempByte = RD_BYTE_INDEX();   \
                                                                        \
        SRL_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);     \
                                                                        \
        Reg = R.TempByte;                                               \
}

#define INDEX_CB_RLC_REG(IndexReg, Reg)         \
{                                                                       \
        /*Z80_BYTE      Data;   */                      \
                                                                        \
 		SETUP_INDEXED_ADDRESS(IndexReg); \
       R.TempByte = RD_BYTE_INDEX();   \
                                                                        \
        RLC_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);     \
                                                                        \
        Reg = R.TempByte;                                               \
}

#define INDEX_CB_RRC_REG(IndexReg, Reg)         \
{                                                                       \
        /*Z80_BYTE      Data;*/                         \
                                                                        \
		SETUP_INDEXED_ADDRESS(IndexReg); \
        R.TempByte = RD_BYTE_INDEX();   \
                                                                        \
        RRC_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);     \
                                                                        \
        Reg = R.TempByte;                                               \
}


#define INDEX_CB_RR_REG(IndexReg, Reg)          \
{                                                                       \
        /*Z80_BYTE      Data;*/                         \
                                                                        \
		SETUP_INDEXED_ADDRESS(IndexReg); \
        R.TempByte = RD_BYTE_INDEX();   \
                                                                        \
        RR_WITH_FLAGS(R.TempByte);                      \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);     \
                                                                        \
        Reg = R.TempByte;                                               \
                                                                        \
}


#define INDEX_CB_RL_REG(IndexReg, Reg)          \
{                                                                       \
        /*Z80_BYTE      Data;*/                         \
                                                                        \
		SETUP_INDEXED_ADDRESS(IndexReg); \
        R.TempByte = RD_BYTE_INDEX();   \
                                                                        \
        RL_WITH_FLAGS(R.TempByte);                      \
                                                                        \
        WR_BYTE_INDEX(R.TempByte);     \
                                                                        \
        Reg = R.TempByte;                                               \
                                                                        \
}

#define INDEX_CB_SET_REG(OrMask, IndexReg, Reg) \
{                                                               \
        /*Z80_BYTE      Data;*/                         \
                                                                \
		SETUP_INDEXED_ADDRESS(IndexReg); \
        R.TempByte = RD_BYTE_INDEX();   \
                                                                \
        SET(OrMask, R.TempByte);                        \
                                                                \
        WR_BYTE_INDEX(R.TempByte);     \
                                                               \
        Reg = R.TempByte;                                               \
}

#define INDEX_CB_RES_REG(AndMask,IndexReg, Reg) \
{                                                               \
        /*Z80_BYTE Data;*/                              \
                                                                \
		SETUP_INDEXED_ADDRESS(IndexReg); \
        R.TempByte = RD_BYTE_INDEX();   \
                                                                \
        RES(AndMask, R.TempByte);                       \
                                                                \
        WR_BYTE_INDEX(R.TempByte);     \
        Reg = R.TempByte;                                               \
}

#define PrefixIgnore()		\
	R.Flags &= ~Z80_CHECK_INTERRUPT_FLAG;		\
	R.PC.W.l++;	/*R.PC.W++;*/									\
	R.R++; \
	Z80_UpdateCycles(1)


/*---------------------------------------------------------*/
/* read a byte */

#define Z80_RD_BYTE(Addr)	Z80_RD_MEM(Addr)

/*---------------------------------------------------------*/
/* write a byte */

#define Z80_WR_BYTE(Addr,Data)	Z80_WR_MEM(Addr,Data)

/*---------------------------------------------------------*/
/* read a word from memory */
#define Z80_RD_WORD(Addr) 	(Z80_WORD)((Z80_WORD)Z80_RD_BYTE(Addr) | (((Z80_WORD)Z80_RD_BYTE((Z80_WORD)(Addr+1)))<<8))


#define Z80_WR_WORD(Addr,Data)	\
{													\
	/* write low byte */							\
        Z80_WR_BYTE(Addr,(Z80_BYTE)Data);			\
													\
        /* write high byte */						\
        Z80_WR_BYTE((Z80_WORD)(Addr+1),(Z80_BYTE)(Data>>8));	\
}


/*---------------------------------------------------------*/
/* pop a word from the stack */



/*---------------------------------------------------------*/
/* put a word on the stack */


#define PUSH(Data)						\
{										\
        R.SP.W-=2;						\
										\
        Z80_WR_WORD(R.SP.W,Data);		\
}




#define HALT()			    R.Flags |=Z80_EXECUTING_HALT_FLAG


/* swap two words */
#define SWAP(Reg1,Reg2) \
{                                               \
        Z80_WORD        tempR;  \
                                                \
        tempR = Reg1;           \
        Reg1 = Reg2;            \
        Reg2 = tempR;           \
}


const char *Z80_LD_R_INDEX = " \
		SETUP_INDEXED_ADDRESS(%s);	\r\n\
		%s = Z80_RD_BYTE(R.MemPtr.W);   \r\n\
";

const char *Z80_LD_INDEX_R = " \
		SETUP_INDEXED_ADDRESS(%s);	\r\n\
		Z80_WR_BYTE(R.MemPtr.W, %s); \r\n\
";


const char *Z80_LD_INDEX_n = " \
        SETUP_INDEXED_ADDRESS(%s); \r\n\
        R.TempByte = Z80_RD_OPCODE_BYTE(3);	\r\n\
        WR_BYTE_INDEX(R.TempByte);                                \r\n\
";


const char *Z80_IN = " \
		R.MemPtr.W = R.BC.W; \r\n\
		%s = Z80_DoIn(R.MemPtr.W);            \r\n\
		++R.MemPtr.W; \r\n\
		{											\r\n\
			Z80_BYTE	Flags;						\r\n\
			Flags = Z80_FLAGS_REG;						\r\n\
			Flags = Flags & Z80_CARRY_FLAG;			\r\n\
			Flags = ZeroSignParityTable[%s];	\r\n\
			Z80_FLAGS_REG = Flags;						\r\n\
		}											\r\n\
	Cycles = 4; \r\n\
";


const char *Z80_SET_IM = " \
        R.IM = %d; \r\n\
";

const char *Z80_DI = "\r\n\
        R.IFF1 = R.IFF2 = 0; \r\n\
        R.Flags &=~Z80_CHECK_INTERRUPT_FLAG;	\r\n\
";


const char *Z80_EI = "\r\n\
        R.IFF1 = R.IFF2 = 1; \r\n\
        R.Flags &=~Z80_CHECK_INTERRUPT_FLAG; \r\n\
";

const char *Z80_LD_A_nnnn = "\r\n\
	/* get memory address to read from */ \r\n\
	R.MemPtr.W = Z80_RD_OPCODE_WORD(1); \r\n\
 \r\n\
	/* read byte */ \r\n\
	R.AF.B.h = Z80_RD_BYTE(R.MemPtr.W); \r\n\
 \r\n\
	/* increment memptr */ \r\n\
	++R.MemPtr.W; \r\n\
";


const char *Z80_LD_A_nnnn_IM0 = "\r\n\
	/* get memory address to read from */ \r\n\
	R.MemPtr.W = Z80_RD_WORD_IM0(); \r\n\
 \r\n\
	/* read byte */ \r\n\
	R.AF.B.h = Z80_RD_BYTE(R.MemPtr.W); \r\n\
 \r\n\
	/* increment memptr */ \r\n\
	++R.MemPtr.W; \r\n\
";

const char *Z80_LD_nnnn_A = "\r\n\
	/* get memory address to read from and store in memptr */ \r\n\
	R.MemPtr.W = Z80_RD_OPCODE_WORD(1); \r\n\
	/* write byte */ \r\n\
	Z80_WR_BYTE(R.MemPtr.W, R.AF.B.h); \r\n\
	/* increment memory pointer */ \r\n\
	R.MemPtr.B.l++; \r\n\
	/* and store a in upper byte */ \r\n\
	R.MemPtr.B.h = R.AF.B.h; \r\n\
";

const char *Z80_LD_nnnn_A_IM0 = "\r\n\
	/* get memory address to read from and store in memptr */ \r\n\
	R.MemPtr.W = Z80_RD_WORD_IM0(); \r\n\
	/* write byte */ \r\n\
	Z80_WR_BYTE(R.MemPtr.W, R.AF.B.h); \r\n\
	/* increment memory pointer */ \r\n\
	R.MemPtr.B.l++; \r\n\
	/* and store a in upper byte */ \r\n\
	R.MemPtr.B.h = R.AF.B.h; \r\n\
";

const char *Z80_LD_HL_nn = " \
	R.MemPtr.W = Z80_RD_OPCODE_WORD(1);\r\n \
	R.HL.W = Z80_RD_WORD(R.MemPtr.W);\r\n \
	++R.MemPtr.W;\r\n \
";

const char *Z80_LD_HL_nn_IM0 = " \
   R.MemPtr.W = Z80_RD_WORD_IM0();\r\n \
	R.HL.W = Z80_RD_WORD(R.MemPtr.W);\r\n \
	++R.MemPtr.W;\r\n\
";


const char *Z80_LD_nn_HL = " \
	R.MemPtr.W = Z80_RD_OPCODE_WORD(1);\r\n \
	Z80_WR_WORD(R.MemPtr.W, R.HL.W);\r\n \
	++R.MemPtr.W;\r\n\
";

const char *Z80_LD_nn_HL_IM0 = " \
   R.MemPtr.W = Z80_RD_WORD_IM0();\r\n \
	Z80_WR_WORD(R.MemPtr.W, R.HL.W);\r\n \
	++R.MemPtr.W;\r\n\
";



const char *Z80_LD_INDEXRR_nnnn = " \r\n\
        R.MemPtr.W = Z80_RD_OPCODE_WORD(2);	\r\n\
        %s = Z80_RD_WORD(R.MemPtr.W);              \r\n\
		++R.MemPtr.W;	\r\n\
";

const char *Z80_LD_nnnn_INDEXRR = " \r\n\
        R.MemPtr.W = Z80_RD_OPCODE_WORD(2);	      \r\n\
		Z80_WR_WORD(R.MemPtr.W,%s);    \r\n\
		++R.MemPtr.W;	\r\n\
";

const char *Z80_LD_RI_n = " \r\n\
        %s = Z80_RD_OPCODE_BYTE(2); \r\n\
";


const char *Z80_OUT = " \r\n\
		R.MemPtr.W = R.BC.W; \r\n\
		Z80_DoOut(R.MemPtr.W,%s);                     \r\n\
        ++R.MemPtr.W;                                       \r\n\
		Cycles = 4;\r\n\
";

const char *Z80_LD_R_HL = " \r\n\
        %s = Z80_RD_BYTE(R.HL.W); \r\n\
";

const char *Z80_LD_HL_R = " \r\n\
        Z80_WR_BYTE(R.HL.W,%s); \r\n\
";

const char *Z80_LD_A_RR = " \r\n\
    R.AF.B.h = Z80_RD_BYTE(%s); \r\n\
	R.MemPtr.W = %s+1; \r\n\
";

const char *Z80_LD_RR_A = " \r\n\
    Z80_WR_BYTE(%s,R.AF.B.h); \r\n\
	R.MemPtr.B.l = (%s+1) & 0x0ff; \r\n\
	R.MemPtr.B.h = R.AF.B.h; \r\n\
";

const char *Z80_LD_SP_rp = "\r\n\
    R.SP.W=%s; \r\n\
";

const char *Z80_JP_rp = "\r\n\
    R.PC.W.l=%s; \r\n\
";

const char *Z80_RST = "	\r\n\
/* push return address on stack */	\r\n\
PUSH((Z80_WORD)(R.PC.W.l+1));			\r\n\
/* set memptr to address */	\r\n\
R.MemPtr.W = 0x%04x;	\r\n\
/* set program counter to memptr */ \r\n\
R.PC.W.l = R.MemPtr.W; \r\n\
";

const char *Z80_LD_R_R = " \r\n\
		%s = %s; \r\n\
";


const char *Z80_LD_RR_nn = " \r\n\
        %s = Z80_RD_OPCODE_WORD(1); \r\n\
";

const char *Z80_LD_INDEXRR_nn = " \r\n\
        %s = Z80_RD_OPCODE_WORD(2); \r\n\
";


const char *Z80_LD_RR_nnnn = " \r\n\
        /* read destination address into memptr */ \r\n\
        R.MemPtr.W = Z80_RD_OPCODE_WORD(2);	\r\n\
        /* read register from address */ \r\n\
        %s = Z80_RD_WORD(R.MemPtr.W);   \r\n\
		++R.MemPtr.W; \r\n\
";

const char *Z80_LD_nnnn_RR = " \r\n\
        /* read destination address into memptr */ \r\n\
        R.MemPtr.W = Z80_RD_OPCODE_WORD(2);	\r\n\
        /* write register to address */ \r\n\
        Z80_WR_WORD(R.MemPtr.W, %s);    \r\n\
        ++R.MemPtr.W; \r\n\
";

const char *Z80_LD_I_A=" \r\n\
	R.I = R.AF.B.h;        \r\n\
";


/* increment register pair */
const char *Z80_INC_rp=" \r\n\
    ++%s;                \r\n\
";

/* decrement register pair */
const char *Z80_DEC_rp=" \r\n\
	--%s;                \r\n\
";


/* EX (SP), HL */
const char *Z80_EX_SP_rr=" \r\n\
        R.MemPtr.W = Z80_RD_WORD(R.SP.W); \r\n\
        Z80_WR_WORD(R.SP.W, %s);    \r\n\
        %s = R.MemPtr.W; \r\n\
";


const char *Z80_LD_A_I = "\r\n\
        R.AF.B.h = R.I;	\r\n\
		{				\r\n\
			Z80_BYTE	Flags;	\r\n\
 \r\n\
			/* HF, NF = 0, CF not changed */ \r\n\
			Flags = Z80_FLAGS_REG;	\r\n\
			Flags &= Z80_CARRY_FLAG;	/* keep CF, zeroise everything else */ \r\n\
			Flags |= ((R.IFF2 & 0x01)<<Z80_PARITY_FLAG_BIT);	/* IFF2 into PV */ \r\n\
			Flags |= ZeroSignTable2[R.AF.B.h];	/* SF, ZF */ \r\n\
			Flags |= R.MemPtr.B.h & ((1<<5)|(1<<3)); /* Bits 5,3 from MemPtr */ \r\n\
			Z80_FLAGS_REG = Flags;	\r\n\
		}	\r\n\
";

/*---------------------------------------------------------*/
/* LD A,R */

const char *Z80_LD_R_A = " \r\n\
    /* store bit 7 */ \r\n\
    R.RBit7 = R.AF.B.h & 0x080; \r\n\
 \r\n\
    /* store refresh register */ \r\n\
    R.R = R.AF.B.h & 0x07f; \r\n\
	/* no flags changed */ \r\n\
";


const char *Z80_LD_A_R = "\r\n\
        R.AF.B.h = Z80_GET_R;	\r\n\
							\r\n\
		{					\r\n\
			Z80_BYTE	Flags;	\r\n\
								\r\n\
			/* HF, NF = 0, CF not changed */ \r\n\
			Flags = Z80_FLAGS_REG;	\r\n\
			Flags &= Z80_CARRY_FLAG;	/* keep CF, zeroise everything else */ \r\n\
			Flags |= ((R.IFF2 & 0x01)<<Z80_PARITY_FLAG_BIT);	/* IFF2 into PV */ \r\n\
			Flags |= ZeroSignTable2[R.AF.B.h];	/* SF, ZF */ \r\n\
			Flags |= R.MemPtr.B.h & ((1<<5)|(1<<3)); /* Bits 5,3 from MemPtr */ \r\n\
			Z80_FLAGS_REG = Flags;	\r\n\
		}					\r\n\
";

/*---------------------------------------------------------*/
/* CPL */

const char *Z80_CPL = "				\r\n\
	Z80_BYTE Flags;			\r\n\
        /* complement */	\r\n\
        R.AF.B.h = (Z80_BYTE)(~R.AF.B.h);	\r\n\
											\r\n\
		Flags = Z80_FLAGS_REG;					\r\n\
		Flags = Flags & (Z80_SIGN_FLAG | Z80_ZERO_FLAG | Z80_PARITY_FLAG | Z80_CARRY_FLAG);	\r\n\
		Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	\r\n\
		Flags |= Z80_SUBTRACT_FLAG | Z80_HALFCARRY_FLAG;			\r\n\
        Z80_FLAGS_REG = Flags;											\r\n\
";


/*---------------------------------------------------------------------------*/
/* SCF */
const char *Z80_SCF = "	\r\n\
	Z80_BYTE	Flags;			\r\n\
								\r\n\
	Flags = Z80_FLAGS_REG;			\r\n\
								\r\n\
	Flags = Flags & (Z80_ZERO_FLAG | Z80_PARITY_FLAG | Z80_SIGN_FLAG);	\r\n\
    Flags = Flags | Z80_CARRY_FLAG;										\r\n\
	Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);			\r\n\
																		\r\n\
	Z80_FLAGS_REG = Flags;													\r\n\
";

/*---------------------------------------------------------------------------*/
/* CCF */
const char *Z80_CCF = "	\r\n\
	Z80_BYTE Flags;				\r\n\
								\r\n\
	Flags = Z80_FLAGS_REG;			\r\n\
    Flags &= (Z80_CARRY_FLAG | Z80_ZERO_FLAG | Z80_PARITY_FLAG | Z80_SIGN_FLAG);	\r\n\
	Flags |= ((Flags & Z80_CARRY_FLAG)<<Z80_HALFCARRY_FLAG_BIT);					\r\n\
	Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);						\r\n\
    Z80_FLAGS_REG = Flags ^ Z80_CARRY_FLAG;												\r\n\
";


const char *Z80_RRD = " \r\n\
        R.TempByte = Z80_RD_BYTE(R.HL.W); \r\n\
        Z80_WR_BYTE(R.HL.W, (Z80_BYTE)(((R.TempByte>>4) | (R.AF.B.h<<4)))); \r\n\
        R.AF.B.h = (R.AF.B.h & 0x0f0) | (R.TempByte & 0x0f); \r\n\
		R.MemPtr.W = R.HL.W+1; \r\n\
		{ \r\n\
			Z80_BYTE	Flags; \r\n\
\r\n\
			Flags = Z80_FLAGS_REG; \r\n\
			Flags &= Z80_CARRY_FLAG; \r\n\
			Flags |= ZeroSignParityTable[R.AF.B.h]; \r\n\
			Z80_FLAGS_REG = Flags; \r\n\
		} \r\n\
";

const char *Z80_RLD = " \r\n\
	R.TempByte = Z80_RD_BYTE(R.HL.W); \r\n\
    Z80_WR_BYTE(R.HL.W,(Z80_BYTE)((R.TempByte<<4)|(R.AF.B.h & 0x0f))); \r\n\
    R.AF.B.h = (R.AF.B.h & 0x0f0) | (R.TempByte>>4); \r\n\
	R.MemPtr.W = R.HL.W+1; \r\n\
	{ \r\n\
		Z80_BYTE	Flags; \r\n\
\r\n\
		Flags = Z80_FLAGS_REG;\r\n\
		Flags &= Z80_CARRY_FLAG;\r\n\
		Flags |= ZeroSignParityTable[R.AF.B.h];\r\n\
		Z80_FLAGS_REG = Flags;\r\n\
	}\r\n\
";


const char *Z80_RETN = "\
   R.IFF1 = R.IFF2; \r\n\
   /* update memptr */ \r\n\
   R.MemPtr.W = POP(); \r\n\
   R.PC.W.l = R.MemPtr.W; \r\n\
   /* flags not changed */ \r\n\
";

const char *Z80_RETI = "\
    R.IFF1 = R.IFF2; \r\n\
    Z80_Reti(); \r\n\
	 /* update memptr */ \r\n\
    R.MemPtr.W = POP(); \r\n\
	R.PC.W.l = R.MemPtr.W; \r\n\
	/* flags not changed */ \r\n\
";


const char *Z80_OUT_n_A = "\
    /* A in upper byte of port, Data in lower byte of port */ \r\n\
    R.MemPtr.B.l = Z80_RD_OPCODE_BYTE(1); \r\n\
	R.MemPtr.B.h = R.AF.B.h; \r\n\
	/* perform out */ \r\n\
    Z80_DoOut(R.MemPtr.W, R.AF.B.h); \r\n\
	/* update mem ptr */ \r\n\
	R.MemPtr.B.l++; \r\n\
	R.MemPtr.B.h = R.AF.B.h; \r\n\
	/* no flags changed */ \r\n\
	Cycles = 3;\r\n\
";

const char *Z80_IN_A_n = " \r\n\
    /* A in upper byte of port, Data in lower byte of port */\r\n\
	R.MemPtr.B.l = Z80_RD_OPCODE_BYTE(1);\r\n\
	R.MemPtr.B.h = R.AF.B.h;\r\n\
    /* a in upper byte of port, data in lower byte of port */\r\n\
    R.AF.B.h = Z80_DoIn(R.MemPtr.W);\r\n\
	/* update mem ptr */\r\n\
	R.MemPtr.W++;\r\n\
	/* no flags changed */\r\n\
	Cycles = 3;\r\n\
";


const char *Z80_NEG = "\r\n\
	Z80_BYTE	Flags;	\r\n\
	Z80_BYTE	AReg; \r\n\
						\r\n\
	AReg = R.AF.B.h;		\r\n\
    Flags = Z80_SUBTRACT_FLAG;	\r\n\
													\r\n\
    if (AReg == 0x080)									\r\n\
    {												\r\n\
          Flags |= Z80_PARITY_FLAG;					\r\n\
    }												\r\n\
													\r\n\
    if (AReg != 0x000)									\r\n\
    {												\r\n\
        Flags |= Z80_CARRY_FLAG;					\r\n\
    }												\r\n\
													\r\n\
	if ((AReg & 0x0f)!=0)								\r\n\
	{												\r\n\
		Flags |= Z80_HALFCARRY_FLAG;				\r\n\
	}												\r\n\
													\r\n\
    R.AF.B.h = -AReg;							\r\n\
													\r\n\
	Flags |= ZeroSignTable[R.AF.B.h];				\r\n\
	Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	\r\n\
	Z80_FLAGS_REG = Flags;								\r\n\
";

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

int		Z80_GetNopCountForInstruction(Z80_WORD Addr, Z80_BYTE Flags);
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
	BOOL bCheckInterrupt = TRUE;

	int OpcodeCount;

	OpcodeCount = Z80_GetOpcodeCountForInstruction(0);

	if ((IsPrefix) && (OpcodeCount==1))
	{
		DoPrefixIgnore(fh);
		//fprintf(fh,"PrefixIgnore();\r\n");
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
				/*(Memory[1]==0x0a2) || (Memory[1]==0x0aa) ||*/
			 /*(Memory[1]==0x0a3) || (Memory[1]==0x0ab) || */
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

		if (InterruptMode)
		{
			/* correct; prooved by tests */
			bIncrementPC = FALSE;

			/* not sure */
			bIncrementR = FALSE;
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
			fprintf(fh,Z80_INC_REFRESH,R_Increment);
		}


		/******************************/
		/* write PC increment */
		if (bIncrementPC)
		{
			int OpcodeCount;

			OpcodeCount = Z80_GetOpcodeCountForInstruction(0);

			fprintf(fh,Z80_ADD_PC,OpcodeCount);
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


		if (bCheckInterrupt)
		{
			fprintf(fh,"R.Flags |= Z80_CHECK_INTERRUPT_FLAG;\r\n");
		}



			if (doNopCycle)
			{
				/******************************/
				/* write NOP cycle timing */
				{
					int NOP_Count;

					NOP_Count = Z80_GetNopCountForInstruction(0,0);

					//fprintf(fh,"Z80_UpdateCycles(%d);\r\n", NOP_Count);
					fprintf(fh,"Cycles = %d;\r\n",NOP_Count);
				}
			}
	}
}


void	StartFunction(unsigned char *FunctionName, FILE *fh,BOOL Local)
{
	fprintf(fh,"/***************************************************************************/\r\n");

	if (Local)
	{
		fprintf(fh,"INLINE static ");
	}

	fprintf(fh,"int %s(void)\r\n",FunctionName);
	fprintf(fh,"{\r\n");
	/* write variables that will be used */
	fprintf(fh,"unsigned long Opcode;\r\n");	//Z80_BYTE	Opcode;\r\n");
	fprintf(fh,"unsigned long Cycles;\r\n");
}

void	EndFunction(FILE *fh)
{

	fprintf(fh,"return Cycles;\r\n");
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

void	StartSwitch2(FILE *fh)
{
	fprintf(fh,"Opcode = R.InterruptVectorBase;\r\n");
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
			DoPrefixIgnore(fh);	//fprintf(fh,"PrefixIgnore();\r\n");
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
						fprintf(fh,"Cycles = Z80_DD_CB_ExecuteInstruction();\r\n");
					}
					else
					{
						fprintf(fh,"Cycles = Z80_FD_CB_ExecuteInstruction();\r\n");
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

int	main(int argc, char *argv[])
{
	int i;
	FILE *fh;

	fh = fopen("c:\\z80code.c","wb");

	if (fh!=NULL)
	{

		int Opcode;

		InterruptMode = FALSE;
		fprintf(fh,"/*\n");
		fprintf(fh," *  Arnold emulator (c) Copyright, Kevin Thacker 1995-2001\n");
		fprintf(fh," *\n");
		fprintf(fh," *  This file is part of the Arnold emulator source code distribution.\n");
		fprintf(fh," *\n");
		fprintf(fh," *  This program is free software; you can redistribute it and/or modify\n");
		fprintf(fh," *  it under the terms of the GNU General Public License as published by\n");
		fprintf(fh," *  the Free Software Foundation; either version 2 of the License, or\n");
		fprintf(fh," *  (at your option) any later version.\n");
		fprintf(fh," *\n");
		fprintf(fh," *  This program is distributed in the hope that it will be useful,\n");
		fprintf(fh," *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
		fprintf(fh," *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
		fprintf(fh," *  GNU General Public License for more details.\n");
		fprintf(fh," *\n");
		fprintf(fh," *  You should have received a copy of the GNU General Public License\n");
		fprintf(fh," *  along with this program; if not, write to the Free Software\n");
		fprintf(fh," *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA\n");
		fprintf(fh," *\n");
		fprintf(fh," */\n");

		fprintf(fh,"#include \"z80.h\"\r\n");
		fprintf(fh,"#include \"z80tables.h\"\r\n");
		fprintf(fh,"#include \"z80macros.h\"\r\n");

		fprintf(fh,"Z80_REGISTERS R;\r\n");

		fprintf(fh,"#include \"z80funcs.h\"\r\n");
		fprintf(fh,"#include \"z80funcs2.h\"\r\n");


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
		fprintf(fh,Z80_INC_REFRESH,2);
		fprintf(fh,Z80_ADD_PC,4);
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
		fprintf(fh,Z80_INC_REFRESH,2);
		fprintf(fh,Z80_ADD_PC,4);
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
		fprintf(fh,Z80_INC_REFRESH,2);

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
		fprintf(fh,Z80_INC_REFRESH,2);
		fprintf(fh,Z80_ADD_PC,2);
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
					fprintf(fh,"Cycles = Z80_CB_ExecuteInstruction();\r\n");
				}
				else if (Opcode==0x0ed)
				{
					fprintf(fh,"Cycles = Z80_ED_ExecuteInstruction();\r\n");
				}
				else if (Opcode==0x0dd)
				{
					fprintf(fh,"Cycles = Z80_DD_ExecuteInstruction();\r\n");
				}
				else if (Opcode==0x0fd)
				{
					fprintf(fh,"Cycles = Z80_FD_ExecuteInstruction();\r\n");
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
	fprintf(fh,"Cycles+=Z80_ExecuteInterrupt();\r\n");
	fprintf(fh,"}\r\n");

		EndFunction(fh);

		InterruptMode = TRUE;
		StartFunction("Z80_ExecuteIM0",fh,FALSE);
			StartSwitch2(fh);

			for (i=0; i<256; i++)
			{
				OpcodeProcessed[i] = FALSE;
			}

			NumProcessed++;
			for (i=0; i<256; i++)
			{
				if (i!=0x0ff)
				{
					switch (i & 0x07)
					{
						case 1:
						case 3:
						case 5:
						case 7:
						{
							BeginCaseNoBracket(fh,i);
							NumProcessed++;
							OpcodeProcessed[i] = TRUE;
						}
						break;
					}
				}
			}

			if (NumProcessed!=0)
			{
				fprintf(fh,"{\r\n");
				EndCase(fh);
			}

			for (i=0; i<256; i++)
			{
				if (OpcodeProcessed[i]==FALSE)
				{
						BeginCase(fh,i);
						Memory[0] = i;
						InterruptAddr = (i<<8) | i;
						DoOpcode(fh,FALSE);
						EndCase(fh);
				}
			}


			EndSwitch(fh);
		EndFunction(fh);
	}

	fclose(fh);

	return 0;
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

static char *RegASmall[]=
{
	"B",
	"C",
	"D",
	"E",
	"H",
	"L",
	"",
	"A"
};


const unsigned char *RegB[]=
{
	"R.BC.W",
	"R.DE.W",
	"R.HL.W",
	"R.SP.W"
};

const unsigned char *RegBSmall[]=
{
	"BC",
	"DE",
	"HL",
	"SP"
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

			DoPrefixIgnore(fh);	//fprintf(fh,"PrefixIgnore();\r\n");
		}
		break;

		case 0x0cb:
		{
		//	signed char Offset;

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
						char sReg[128];
						fprintf(fh,"/* BIT %d,(I%c+d) */\r\n", BitIndex, IndexCh);
						sprintf(sReg,"R.I%c.W",IndexCh);
						fprintf(fh,"SETUP_INDEXED_ADDRESS(%s);\r\n",sReg);
						fprintf(fh,"R.TempByte = Z80_RD_MEM(R.MemPtr.W);\r\n");
						fprintf(fh,Z80_BIT_MP,BitIndex, BitIndex, BitIndex);
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
								char sReg1[128];
								fprintf(fh,"/* LD I%c,nnnn */\r\n",IndexCh);
								sprintf(sReg1,"R.I%c.W",IndexCh);
								fprintf(fh,Z80_LD_INDEXRR_nn, sReg1);
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

									char sReg[128];
									sprintf(sReg,"R.I%c.W",IndexCh);
									if ((Opcode & (1<<3))==0)
									{
										fprintf(fh,"/* LD (nnnn),I%c */\r\n",IndexCh);
										fprintf(fh,Z80_LD_nnnn_INDEXRR, sReg);

									}
									else
									{
										fprintf(fh,"/* LD I%c,(nnnn) */\r\n",IndexCh);
										fprintf(fh,Z80_LD_INDEXRR_nnnn, sReg);
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
								char sReg[128];
								fprintf(fh,"/* INC I%c */\r\n",IndexCh);
								sprintf(sReg,"R.I%c.W", IndexCh);
								fprintf(fh,Z80_INC_rp,sReg);
							}
							else
							{
								/* 00ss1011 - DEC ss */
								char sReg[128];
								fprintf(fh,"/* DEC I%c */\r\n",IndexCh);
								sprintf(sReg,"R.I%c.W", IndexCh);
								fprintf(fh,Z80_DEC_rp,sReg);
							}
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
							char Reg1[128];

							RegIndex = ((Opcode>>3) & 0x07);

							if (RegIndex==4)
							{
								fprintf(fh,"/* LD HI%c, n */\r\n",IndexCh);
								sprintf(Reg1,"R.I%c.B.h",IndexCh);
								fprintf(fh,Z80_LD_RI_n,Reg1);
							}
							else if (RegIndex==5)
							{
								fprintf(fh,"/* LD LI%c, n */\r\n",IndexCh);
								sprintf(Reg1,"R.I%c.B.l",IndexCh);
								fprintf(fh,Z80_LD_RI_n,Reg1);
							}
							else if (RegIndex==6)
							{
								char sReg[128];
								fprintf(fh,"/* LD (I%c+d),n */\r\n", IndexCh);
								sprintf(sReg,"R.I%c.W",IndexCh);
								fprintf(fh,Z80_LD_INDEX_n, sReg);
							}
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
							char sReg[128];
							sprintf(sReg,"R.I%c.W", IndexCh);
							fprintf(fh,"/* LD (I%c+D),%s */\r\n",IndexCh,RegASmall[Reg2]);
							fprintf(fh,Z80_LD_INDEX_R, sReg,RegA[Reg2]);
						}

						if (Reg2==6)
						{
							char sReg[128];
							sprintf(sReg,"R.I%c.W", IndexCh);
							fprintf(fh,"/* LD %s,(I%c+D) */\r\n",RegASmall[Reg1],IndexCh);
							fprintf(fh,Z80_LD_R_INDEX, sReg,RegA[Reg1]);
						}
					}
					else
					{
						unsigned char Reg1Char='h', Reg2Char='h';

						if (Reg1 == 4)
						{
							Reg1Char = 'h';
						}
						else if (Reg1 == 5)
						{
							Reg1Char = 'l';
						}
						else
						{
							printf("warning Reg1 not valid %d!\r\n",Reg1);
						}

						if (Reg2 == 4)
						{
							Reg2Char = 'h';
						}
						else if (Reg2 == 5)
						{
							Reg2Char = 'l';
						}
						else
						{
							printf("warning Reg2 not valid %d!\r\n",Reg2);
						}

						if ((Reg1==4) || (Reg1==5))
						{
							if ((Reg2==4) || (Reg2==5))
							{
								if (Reg1!=Reg2)
								{
									/* reg 1,2 IXh type */
									char sReg1[128];
									char sReg2[128];
									fprintf(fh,"/* LD %cI%c,%cI%c */\r\n",Reg1Char, IndexCh, Reg2Char, IndexCh);
									sprintf(sReg1,"R.I%c.B.%c",IndexCh,Reg1Char);
									sprintf(sReg2,"R.I%c.B.%c",IndexCh,Reg2Char);
									fprintf(fh,Z80_LD_R_R, sReg1, sReg2);
								}
							}
							else
							{
								/* reg 1 is IXH, Reg 2 isn't */
								char sReg1[128];
								fprintf(fh,"/* LD %cI%c,%s */\r\n",Reg1Char, IndexCh, RegASmall[Reg2]);
								sprintf(sReg1,"R.I%c.B.%c",IndexCh,Reg1Char);
								fprintf(fh,Z80_LD_R_R,sReg1, RegA[Reg2]);
							}
						}
						else
						{
							if ((Reg2==4) || (Reg2==5))
							{
								/* reg 1 not IXH type */
								char sReg2[128];
								fprintf(fh,"/* LD %s,%cI%c */\r\n",RegASmall[Reg1], Reg2Char, IndexCh);
								sprintf(sReg2,"R.I%c.B.%c",IndexCh,Reg2Char);
								fprintf(fh,Z80_LD_R_R,RegA[Reg1],sReg2);
							}
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
								unsigned char RegPartCh='h';

								if (Reg==4)
								{
									RegPartCh = 'h';
								}
								else if (Reg==5)
								{
									RegPartCh = 'l';
								}
								else
								{
									printf("warning Reg not valid!\r\n");
								}

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
								unsigned char RegPartCh='h';

								if (Reg==4)
								{
									RegPartCh = 'h';
								}
								else if (Reg==5)
								{
									RegPartCh = 'l';
								}
								else
								{
									printf("warning Reg not valid!\r\n");
								}

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
								unsigned char RegPartCh='h';

								if (Reg==4)
								{
									RegPartCh = 'h';
								}
								else if (Reg==5)
								{
									RegPartCh = 'l';
								}
								else
								{
									printf("warning Reg not valid!\r\n");
								}

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
								unsigned char RegPartCh='h';

								if (Reg==4)
								{
									RegPartCh = 'h';
								}
								else if (Reg==5)
								{
									RegPartCh = 'l';
								}
								else
								{
									printf("warning Reg not valid!\r\n");
								}

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
								unsigned char RegPartCh='h';

								if (Reg==4)
								{
									RegPartCh = 'h';
								}
								else if (Reg==5)
								{
									RegPartCh = 'l';
								}
								else
								{
									printf("warning Reg not valid!\r\n");
								}
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
								unsigned char RegPartCh='h';

								if (Reg==4)
								{
									RegPartCh = 'h';
								}
								else if (Reg==5)
								{
									RegPartCh = 'l';
								}
								else
								{
									printf("warning Reg not valid!\r\n");
								}

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
								unsigned char RegPartCh='h';

								if (Reg==4)
								{
									RegPartCh = 'h';
								}
								else if (Reg==5)
								{
									RegPartCh = 'l';
								}
								else
								{
									printf("warning Reg not valid!\r\n");
								}

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
								unsigned char RegPartCh='h';

								if (Reg==4)
								{
									RegPartCh = 'h';
								}
								else if (Reg==5)
								{
									RegPartCh = 'l';
								}
								else
								{
									printf("warning Reg not valid!\r\n");
								}

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
								fprintf(fh,"/* POP I%c */\r\n",IndexCh);
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

                                char sReg1[128];
                                sprintf(sReg1,"R.I%c.W", IndexCh);

								if (Opcode==0x0e9)
								{
									fprintf(fh,"/* JP (I%c) */\r\n",IndexCh);
									fprintf(fh,Z80_JP_rp, sReg1);
								}

								if (Opcode==0x0f9)
								{
									fprintf(fh,"/* LD SP,I%c */\r\n",IndexCh);
									fprintf(fh,Z80_LD_SP_rp, sReg1);
								}
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
								    char sReg[128];
									fprintf(fh,"/* EX (SP),I%c */\r\n",IndexCh);
								    sprintf(sReg,"R.I%c.W",IndexCh);
									fprintf(fh,Z80_EX_SP_rr, sReg, sReg);
								}
							}

						}
						break;

						case 5:
						{
							/* 11qq0101 - PUSH qq */
							if ((Opcode & (1<<3))==0)
							{
								fprintf(fh,"/* PUSH I%c */\r\n",IndexCh);
								fprintf(fh,"PUSH(R.I%c.W);\r\n",IndexCh);
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
						    fprintf(fh,"/* BIT %d,%s */\r\n",BitIndex,RegASmall[Reg]);
							fprintf(fh,Z80_BIT,BitIndex, RegA[Reg], BitIndex, BitIndex);
						}
						else
						{
						    fprintf(fh,"/* BIT %d,(HL) */\r\n",BitIndex);
						    fprintf(fh,"R.TempByte = Z80_RD_BYTE(R.HL.W);\r\n");
							fprintf(fh,Z80_BIT_MP,BitIndex,BitIndex, BitIndex);
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
							fprintf(fh,"/* IN X,(C) */\r\n");
							fprintf(fh,Z80_IN,"R.TempByte","R.TempByte");

						}
						else
						{
							fprintf(fh,"/* IN %s,(C) */\r\n",RegASmall[RegIndex]);
							fprintf(fh,Z80_IN,RegA[RegIndex],RegA[RegIndex]);
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
							fprintf(fh," /* OUT (C),0 */\r\n");
							fprintf(fh,Z80_OUT,"0");
						}
						else
						{
							fprintf(fh," /* OUT (C),%s */\r\n", RegASmall[RegIndex]);
							fprintf(fh,Z80_OUT,RegA[RegIndex]);
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
						    fprintf(fh,"/* LD %s,(nnnn) */\r\n",RegBSmall[((Opcode>>4) & 0x03)]);
							fprintf(fh,Z80_LD_RR_nnnn,RegB[((Opcode>>4) & 0x03)]);
						}
						else
						{
						    fprintf(fh,"/* LD (nnnn),%s */\r\n",RegBSmall[((Opcode>>4) & 0x03)]);
							fprintf(fh,Z80_LD_nnnn_RR,RegB[((Opcode>>4) & 0x03)]);
						}
					}
					break;

					case 4:
					{
						/* NEG - 01xxx100 */
						fprintf(fh,"/* NEG */\r\n");
						fprintf(fh,Z80_NEG);
					}
					break;

					case 5:
					{
						/* RETI - 01xx1110 */
						/* RETN - 01xx0110 */
						if ((Opcode & 0x08)!=0)
						{
							fprintf(fh,"/* RETI */\r\n");
							fprintf(fh,Z80_RETI);
						}
						else
						{
							fprintf(fh,"/* RETN */\r\n");
							fprintf(fh,Z80_RETN);
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
							default:
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
						fprintf(fh,"/* IM %d */\r\n",IM_Type);
						fprintf(fh,Z80_SET_IM, IM_Type);
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
								fprintf(fh,"/* LD I,A */\r\n");
							    fprintf(fh,Z80_LD_I_A);
							}
							break;

							case 1:
							{
								fprintf(fh,"/* LD R,A */\r\n");
								fprintf(fh,Z80_LD_R_A);
							}
							break;

							case 2:
							{
								fprintf(fh,"/* LD A,I */\r\n");
								fprintf(fh,Z80_LD_A_I);
							}
							break;

							case 3:
							{
								fprintf(fh,"/* LD A,R */\r\n");
								fprintf(fh,Z80_LD_A_R);
							}
							break;

							case 4:
							{
								fprintf(fh,"/* RRD */\r\n");
								fprintf(fh,Z80_RRD);
							}
							break;

							case 5:
							{
								fprintf(fh,"/* RLD */\r\n");
								fprintf(fh,Z80_RLD);
							}
							break;


							default:
								break;
						}
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
								fprintf(fh,"if (Z80_TEST_PARITY_EVEN)\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"Cycles=6;\r\n");	//Z80_UpdateCycles(6);\r\n");
								fprintf(fh,"}\r\n");
								fprintf(fh,"else\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,Z80_ADD_PC,2);
								fprintf(fh,"Cycles=5;\r\n");	//Z80_UpdateCycles(5);\r\n");
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
						/* 10101001 - CPD */
						/* 10111001 - CPDR */
						/* 10110001 - CPIR */

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
								fprintf(fh,"if ((Z80_FLAGS_REG & (Z80_PARITY_FLAG | Z80_ZERO_FLAG))==Z80_PARITY_FLAG)\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"Cycles=5;\r\n");	//Z80_UpdateCycles(5);\r\n");
								fprintf(fh,"}\r\n");
								fprintf(fh,"else\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,Z80_ADD_PC,2);
								fprintf(fh,"Cycles=6;\r\n");	//Z80_UpdateCycles(6);\r\n");
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
						/* 10101010 - IND */
						/* 10110010 - INIR */
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
								fprintf(fh,"if (Z80_TEST_ZERO_SET)\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,Z80_ADD_PC,2);
								fprintf(fh,"Cycles=6;\r\n");	//Z80_UpdateCycles(1);\r\n");
								fprintf(fh,"}\r\n");
								fprintf(fh,"else\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"Cycles=5;\r\n");
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
						/* 10101011 - outd */
						/* 10110011 - OTIR */
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
								fprintf(fh,"if (Z80_TEST_ZERO_SET)\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,Z80_ADD_PC,2);
								fprintf(fh,"Cycles=6;\r\n");	//Z80_UpdateCycles(1);\r\n");
								fprintf(fh,"}\r\n");
								fprintf(fh,"else\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"Cycles=5;\r\n");	//Z80_UpdateCycles(1);\r\n");
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

								fprintf(fh, "/* JR cc,dd */\r\n");

								fprintf(fh, "Z80_BYTE_OFFSET Offset;\r\n");
								if (InterruptMode)
								{
									fprintf(fh,"Offset = Z80_RD_BYTE_IM0();\r\n");
								}
								else
								{
									fprintf(fh,"Offset = Z80_RD_OPCODE_BYTE(1);\r\n");
								}

								fprintf(fh,"R.MemPtr.W = R.PC.W.l + (Z80_LONG)2 + Offset;\r\n");
								fprintf(fh,"if (");


								switch ((Opcode>>3) & 0x03)
								{
									case 0:
									{
										fprintf(fh,"Z80_TEST_ZERO_NOT_SET");
									}
									break;

									case 1:
									{
										fprintf(fh,"Z80_TEST_ZERO_SET");
									}
									break;

									case 2:
									{
										fprintf(fh,"Z80_TEST_CARRY_NOT_SET");
									}
									break;

									case 3:
									{
										fprintf(fh,"Z80_TEST_CARRY_SET");
									}
									break;
								}

								fprintf(fh,")\r\n");
								fprintf(fh,"{\r\n");

								fprintf(fh,"R.PC.W.l = R.MemPtr.W;\r\n");
								fprintf(fh,"Cycles=3;\r\n");

								fprintf(fh,"}\r\nelse\r\n{\r\n");

								if (!InterruptMode)
								{
									fprintf(fh,Z80_ADD_PC,2);
								}
								fprintf(fh,"Cycles=2;\r\n");
								fprintf(fh,"}\r\n");
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
										if (InterruptMode)
										{
											fprintf(fh,"Cycles = DJNZ_dd_IM0();\r\n");
										}
										else
										{
											fprintf(fh,"Cycles = DJNZ_dd();\r\n");
										}

//										Instruction = "DJNZ";
									}
									else
									{
										fprintf(fh, "/* JR dd */\r\n");

										fprintf(fh, "Z80_BYTE_OFFSET Offset;\r\n");
										if (InterruptMode)
										{
											fprintf(fh,"Offset = Z80_RD_BYTE_IM0();\r\n");
										}
										else
										{
											fprintf(fh,"Offset = Z80_RD_OPCODE_BYTE(1);\r\n");
										}

										fprintf(fh,"R.MemPtr.W = R.PC.W.l + (Z80_LONG)2 + Offset;\r\n");
										fprintf(fh,"R.PC.W.l = R.MemPtr.W;\r\n");
									//	fprintf(fh,"Cycles = 3;\r\n");

									}
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
							}
							else
							{
								/* 00dd0001 - LD dd,nn */
								fprintf(fh,"/* LD %s,nnnn */\r\n",RegBSmall[((Opcode>>4) & 0x03)]);
								fprintf(fh,Z80_LD_RR_nn,RegB[((Opcode>>4) & 0x03)]);
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
										fprintf(fh,"/* LD A,(BC) */\r\n");
										fprintf(fh,Z80_LD_A_RR,"R.BC.W","R.BC.W");
									}
									else
									{
										fprintf(fh,"/* LD (BC),A */\r\n");
										fprintf(fh,Z80_LD_RR_A,"R.BC.W","R.BC.W");
									}

								}
								break;

								case 1:
								{
									/* 00010010 - LD (DE),A */
									/* 00011010 - LD A,(DE) */

									if ((Opcode & (1<<3))!=0)
									{
										fprintf(fh,"/* LD A,(DE) */\r\n");
										fprintf(fh,Z80_LD_A_RR, "R.DE.W","R.DE.W");
									}
									else
									{
										fprintf(fh,"/* LD (DE),A */\r\n");
										fprintf(fh,Z80_LD_RR_A,"R.DE.W","R.DE.W");
									}
								}
								break;

								case 2:
								{
									/* 00100010 - LD (nnnn),HL */
									/* 00101010 - LD HL,(nn) */

									if ((Opcode & (1<<3))==0)
									{
										/* LD (nnnn), HL */
										fprintf(fh,"/* LD (nnnn),HL */\r\n");
										if (InterruptMode)
										{
											fprintf(fh,Z80_LD_nn_HL_IM0);
										}
										else
										{
											fprintf(fh,Z80_LD_nn_HL);
										}
									}
									else
									{
										fprintf(fh,"/* LD HL,(nnnn) */\r\n");

										if (InterruptMode)
										{
											fprintf(fh,Z80_LD_HL_nn_IM0);
										}
										else
										{
											fprintf(fh,Z80_LD_HL_nn);
										}
									}
								}
								break;

								case 3:
								{
									/* 00110010 - LD (nnnn),A */
									/* 00111010 - LD A,(nnnn) */

									if ((Opcode & (1<<3))==0)
									{
										/* LD (nnnn),A */
										fprintf(fh,"/* LD (nnnn),A */\r\n");
										fprintf(fh,Z80_LD_nnnn_A);
									}
									else
									{
										/* LD A,(nnnn) */
										fprintf(fh,"/* LD A,(nnnn) */\r\n");
										fprintf(fh,Z80_LD_A_nnnn);
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
								fprintf(fh,"/* INC %s */\r\n",RegBSmall[RegIndex]);
								fprintf(fh,Z80_INC_rp,RegB[RegIndex]);
							}
							else
							{
								/* 00ss1011 - DEC ss */
								fprintf(fh,"/* DEC %s */\r\n",RegBSmall[RegIndex]);
								fprintf(fh,Z80_DEC_rp,RegB[RegIndex]);
							}
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
								/* LD r,n */
								fprintf(fh," /* LD %s,n */\r\n", RegASmall[Reg]);
								if (InterruptMode)
								{
									fprintf(fh,"%s = Z80_RD_OPCODE_BYTE(0);\r\n",RegA[Reg]);
								}
								else
								{
									fprintf(fh,"%s = Z80_RD_OPCODE_BYTE(1);\r\n",RegA[Reg]);
								}
							}
							else
							{
								fprintf(fh," /* LD (HL),n */\r\n", RegASmall[Reg]);
								if (InterruptMode)
								{
									fprintf(fh,"R.TempByte = Z80_RD_OPCODE_BYTE(0);\r\n");
								}
								else
								{
									fprintf(fh,"R.TempByte = Z80_RD_OPCODE_BYTE(1);\r\n");
								}

							    fprintf(fh,"Z80_WR_BYTE(R.HL.W,R.TempByte);\r\n");
							}
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
										fprintf(fh,"/* CPL */\r\n");
										fprintf(fh,Z80_CPL);
									}
									break;

								case 6:
									{
										fprintf(fh,"/* SCF */\r\n");
										fprintf(fh,Z80_SCF);
									}
									break;

								case 7:
									{
										fprintf(fh,"/* CCF */\r\n");
										fprintf(fh,Z80_CCF);
									}
									break;
							}
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
								fprintf(fh,"/* LD %s,%s */\r\n",RegASmall[Reg1],RegASmall[Reg2]);
								fprintf(fh,Z80_LD_R_R,RegA[Reg1],
									RegA[Reg2]);
							}
						}
						else
						{
							if (Reg1==6)
							{
								fprintf(fh,"/* LD (HL),%s */\r\n",RegASmall[Reg2]);
								fprintf(fh,Z80_LD_HL_R,RegA[Reg2]);
							}
							else
							{
								fprintf(fh,"/* LD %s,(HL) */\r\n",RegASmall[Reg1]);
								fprintf(fh,Z80_LD_R_HL,RegA[Reg1]);
							}
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
									fprintf(fh,"Z80_FLAGS_REG = Flags;\r\n");
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
									fprintf(fh,"Z80_FLAGS_REG = Flags;\r\n");
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
									fprintf(fh,"Z80_FLAGS_REG = Flags;\r\n");
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
									fprintf(fh,"Z80_FLAGS_REG = Flags;\r\n");
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
									fprintf(fh,"Z80_FLAGS_REG = Flags;\r\n");
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
										fprintf(fh,"Z80_TEST_ZERO_NOT_SET");
									}
									break;

									case 1:
									{
										fprintf(fh,"Z80_TEST_ZERO_SET");
									}
									break;

									case 2:
									{
										fprintf(fh,"Z80_TEST_CARRY_NOT_SET");
									}
									break;

									case 3:
									{
										fprintf(fh,"Z80_TEST_CARRY_SET");
									}
									break;

									case 4:
									{
										fprintf(fh,"Z80_TEST_PARITY_ODD");
									}
									break;

									case 5:
									{
										fprintf(fh,"Z80_TEST_PARITY_EVEN");
									}
									break;

									case 6:
									{
										fprintf(fh,"Z80_TEST_POSITIVE");
									}
									break;

									case 7:
									{
										fprintf(fh,"Z80_TEST_MINUS");
									}
									break;
								}

								fprintf(fh,")\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"RETURN();\r\n");
								fprintf(fh,"Cycles=4;\r\n");	//Z80_UpdateCycles(4);\r\n");
								fprintf(fh,"}\r\nelse\r\n{\r\n");
								if (!InterruptMode)
								{
									fprintf(fh,Z80_ADD_PC,1);
								}
								fprintf(fh,"Cycles=2;\r\n");	//Z80_UpdateCycles(2);\r\n");
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
									fprintf(fh,"/* JP (HL) */\r\n");
									fprintf(fh,Z80_JP_rp, "R.HL.W");
								}

								if (Opcode==0x0f9)
								{
									fprintf(fh,"/* LD SP,HL */\r\n");
									fprintf(fh,Z80_LD_SP_rp, "R.HL.W");
								}
							}
						}
						break;

						case 2:
						{
								/* 11 ccc 010 - JP cc,nnnn */
								fprintf(fh,"/* JP cc, nnnn */\r\n");

								if (InterruptMode)
								{
									fprintf(fh,"R.MemPtr.W = Z80_RD_WORD_IM0();\r\n");
								}
								else
								{
									fprintf(fh,"R.MemPtr.W = Z80_RD_OPCODE_WORD(1);\r\n");
								}

								fprintf(fh,"if (");

								switch ((Opcode>>3) & 0x07)
								{
									case 0:
									{
										fprintf(fh,"Z80_TEST_ZERO_NOT_SET");
									}
									break;

									case 1:
									{
										fprintf(fh,"Z80_TEST_ZERO_SET");
									}
									break;

									case 2:
									{
										fprintf(fh,"Z80_TEST_CARRY_NOT_SET");
									}
									break;

									case 3:
									{
										fprintf(fh,"Z80_TEST_CARRY_SET");
									}
									break;

									case 4:
									{
										fprintf(fh,"Z80_TEST_PARITY_ODD");
									}
									break;

									case 5:
									{
										fprintf(fh,"Z80_TEST_PARITY_EVEN");
									}
									break;

									case 6:
									{
										fprintf(fh,"Z80_TEST_POSITIVE");
									}
									break;

									case 7:
									{
										fprintf(fh,"Z80_TEST_MINUS");
									}
									break;
								}

								fprintf(fh,")\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"R.PC.W.l = R.MemPtr.W;\r\n");
								fprintf(fh,"}\r\nelse\r\n{\r\n");
								if (!InterruptMode)
								{
								fprintf(fh,Z80_ADD_PC,3);
								}
								fprintf(fh,"}\r\n");
								fprintf(fh,"Cycles=3;\r\n");	//Z80_UpdateCycles(3);\r\n");
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

									fprintf(fh,"/* IN A,(n) */\r\n");
									fprintf(fh,Z80_IN_A_n);
								}
								else
								{
									fprintf(fh,"/* OUT (n),A */\r\n");
									fprintf(fh,Z80_OUT_n_A);
								}

							}
							else if (Opcode == 0x0c3)
							{
								/* 11000011 - JP nn */
								fprintf(fh,"/* JP nnnn */\r\n");
								fprintf(fh,"R.MemPtr.W = Z80_RD_OPCODE_WORD(1);\r\n");
								fprintf(fh,"R.PC.W.l = R.MemPtr.W;\r\n");
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
										fprintf(fh,"/* DI */\r\n");
										fprintf(fh,Z80_DI);
									}
									break;

									case 0x0fb:
									{
										fprintf(fh,"/* EI */\r\n");
										fprintf(fh,Z80_EI);
									}
									break;

									case 0x0eb:
									{
										fprintf(fh,"SWAP(R.HL.W,R.DE.W);\r\n");
									}
									break;

									case 0x0e3:
									{
										fprintf(fh,Z80_EX_SP_rr,"R.HL.W","R.HL.W");
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

								fprintf(fh,"/* CALL cc,nnnn */\r\n");

								if (InterruptMode)
								{
									fprintf(fh,"R.MemPtr.W = Z80_RD_WORD_IM0();\r\n");
								}
								else
								{
									fprintf(fh,"R.MemPtr.W = Z80_RD_OPCODE_WORD(1);\r\n");
								}

								fprintf(fh,"if (");

								switch ((Opcode>>3) & 0x07)
								{
									case 0:
									{
										fprintf(fh,"Z80_TEST_ZERO_NOT_SET");
									}
									break;

									case 1:
									{
										fprintf(fh,"Z80_TEST_ZERO_SET");
									}
									break;

									case 2:
									{
										fprintf(fh,"Z80_TEST_CARRY_NOT_SET");
									}
									break;

									case 3:
									{
										fprintf(fh,"Z80_TEST_CARRY_SET");
									}
									break;

									case 4:
									{
										fprintf(fh,"Z80_TEST_PARITY_ODD");
									}
									break;

									case 5:
									{
										fprintf(fh,"Z80_TEST_PARITY_EVEN");
									}
									break;

									case 6:
									{
										fprintf(fh,"Z80_TEST_POSITIVE");
									}
									break;

									case 7:
									{
										fprintf(fh,"Z80_TEST_MINUS");
									}
									break;
								}

								fprintf(fh,")\r\n");
								fprintf(fh,"{\r\n");
								fprintf(fh,"PUSH((R.PC.W.l+3));\r\n");
								fprintf(fh,"R.PC.W.l = R.MemPtr.W;\r\n");
								fprintf(fh,"Cycles=5;\r\n");	//Z80_UpdateCycles(5);\r\n");
								fprintf(fh,"}\r\nelse\r\n{\r\n");
								if (!InterruptMode)
								{
								fprintf(fh,Z80_ADD_PC,3);
								}
								fprintf(fh,"Cycles=3;\r\n");	//Z80_UpdateCycles(3);\r\n");
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

								fprintf(fh,"/* CALL nnnn */\r\n");

								if (InterruptMode)
								{
									fprintf(fh,"R.MemPtr.W = Z80_RD_WORD_IM0();\r\n");
								}
								else
								{
									fprintf(fh,"R.MemPtr.W = Z80_RD_OPCODE_WORD(1);\r\n");
								}

								fprintf(fh,"PUSH((R.PC.W.l+3));\r\n");
								fprintf(fh,"R.PC.W.l = R.MemPtr.W;\r\n");
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

							if (InterruptMode)
							{
								fprintf(fh,"R.TempByte = Z80_RD_OPCODE_BYTE(0);\r\n");
							}
							else
							{
								fprintf(fh,"R.TempByte = Z80_RD_OPCODE_BYTE(1);\r\n");
							}


						switch (((Opcode>>3) & 0x07))
						{
							case 0:
							{
								fprintf(fh,"ADD_A_X(R.TempByte);\r\n");
							}
							break;

							case 1:
							{
								fprintf(fh,"ADC_A_X(R.TempByte);\r\n");
							}
							break;

							case 2:
							{
								fprintf(fh,"SUB_A_X(R.TempByte);\r\n");
							}
							break;

							case 3:
							{
								fprintf(fh,"SBC_A_X(R.TempByte);\r\n");
							}
							break;

							case 4:
							{
								fprintf(fh,"AND_A_X(R.TempByte);\r\n");
							}
							break;

							case 5:
							{
								fprintf(fh,"XOR_A_X(R.TempByte);\r\n");
							}
							break;
							case 6:
							{
								fprintf(fh,"OR_A_X(R.TempByte);\r\n");
							}
							break;
							case 7:
							{
								fprintf(fh,"CP_A_X(R.TempByte);\r\n");
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
							fprintf(fh,"/* RST 0x%04x */\r\n",(Opcode&0x038));
							fprintf(fh,Z80_RST,(Opcode & 0x038));
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
