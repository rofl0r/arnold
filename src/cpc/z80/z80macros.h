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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *
 */
#include "z80.h"

#define INC_REFRESH(Count)                              \
        R.R+=Count

#define SETUP_INDEXED_ADDRESS(Index)	\
{												\
	Z80_BYTE_OFFSET Offset;						\
												\
    Offset = (Z80_BYTE_OFFSET)Z80_RD_OPCODE_BYTE(2);/*((Z80_WORD)(R.PC.W+2));*/   \
												\
	R.IndexPlusOffset = Index+Offset;			\
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
        Z80_FLAGS_REG |= (((Reg^R.HL.W)&(Reg^Result)&0x8000)>>13)
                

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


/* was 2,2 */
#define _OUT(Register)                                  \
{                                                                               \
/*        Z80_UpdateCycles(2);  */                    \
		Z80_DoOut(R.BC.W,Register);                     \
                                                                                \
/*        Z80_UpdateCycles(2);      */                \
Cycles = 4; \
}


#define _IN(Register)                                   \
{                                                                               \
/*        Z80_UpdateCycles(2);    */                  \
		Register = Z80_DoIn(R.BC.W);            \
		{											\
			Z80_BYTE	Flags;						\
			Flags = Z80_FLAGS_REG;						\
			Flags = Flags & Z80_CARRY_FLAG;			\
			Flags = ZeroSignParityTable[Register];	\
			Z80_FLAGS_REG = Flags;						\
		}											\
 /*       Z80_UpdateCycles(2);		*/				\
	Cycles = 4; \
}


#define LD_R_n(Register) \
        Register = Z80_RD_OPCODE_BYTE(1)

#define LD_RI_n(Register) \
        Register = Z80_RD_OPCODE_BYTE(2)


#define LD_R_HL(Register) \
        Register = Z80_RD_BYTE(R.HL.W); \

#define LD_R_INDEX(Index,Register)                      \
{                                                                                       \
        Register = RD_BYTE_INDEX(Index);                \
                                                                                        \
}

#define LD_INDEX_R(Index,Register)                      \
{                                                                                       \
        WR_BYTE_INDEX_OLD(Index,Register);                  \
                                                                                        \
}


#define LD_HL_R(Register) \
        Z80_WR_BYTE(R.HL.W,Register); \

#define LD_A_RR(Register) \
        R.AF.B.h = Z80_RD_BYTE(Register)

#define LD_RR_A(Register) \
        Z80_WR_BYTE(Register,R.AF.B.h) 

#define LD_INDEX_n(Index)                       \
{                                                                       \
        /*Z80_BYTE      Data;*/                                 \
                                                                        \
        R.TempByte = Z80_RD_OPCODE_BYTE(3);	/*Z80_RD_BYTE((Z80_WORD)(R.PC.W+3));*/ \
                                                                                        \
        WR_BYTE_INDEX_OLD(Index,R.TempByte);                                \
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
                                                                \
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                \
        RES(AndMask, R.TempByte);                       \
                                                                \
        WR_BYTE_INDEX(IndexReg,R.TempByte);     \
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
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                \
        SET(OrMask, R.TempByte);                        \
                                                                \
        WR_BYTE_INDEX(IndexReg,R.TempByte);     \
}


/* BIT */
#define BIT(BitIndex,Register)          \
{                                                                       \
	Z80_BYTE	Flags;						\
	Z80_BYTE	Reg753;					\
	Z80_BYTE	RegShift;				\
	Flags = Z80_FLAGS_REG;						\
	Reg753 = Register;			\
	RegShift = Reg753;			\
	Reg753 = Reg753 & (Z80_SIGN_FLAG | Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	\
	RegShift = ((((~RegShift) & (1<<BitIndex))>>BitIndex)<<Z80_ZERO_FLAG_BIT);	\
  Flags = Flags & Z80_CARRY_FLAG; \
  Flags = Flags | RegShift; \
  Flags = Flags | Reg753;		\
  Z80_FLAGS_REG = Flags | Z80_HALFCARRY_FLAG; \
}

#define BIT_REG(BitIndex, Register)     \
{    \
        BIT(BitIndex, Register);                \
                                                                        \
}

#define BIT_HL(BitIndex)                                \
{                                                                       \
        /*Z80_BYTE      Data;*/                                 \
                                                                        \
        R.TempByte = Z80_RD_BYTE(R.HL.W);               \
                                                                        \
        BIT(BitIndex,R.TempByte);                               \
                                                                        \
}                                                       

#define BIT_INDEX(BitIndex, IndexReg)                           \
{                                                                       \
        /*Z80_BYTE Data;*/                                      \
                                                                        \
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                        \
        BIT(BitIndex,R.TempByte);                               \
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
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                        \
        RL_WITH_FLAGS(R.TempByte);                                              \
                                                                        \
        WR_BYTE_INDEX(IndexReg,R.TempByte);             \
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
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                        \
        RR_WITH_FLAGS(R.TempByte);                                              \
                                                                        \
        WR_BYTE_INDEX(IndexReg,R.TempByte);             \
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
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                        \
        RLC_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(IndexReg,R.TempByte);             \
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
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                        \
        RRC_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(IndexReg,R.TempByte);             \
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
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                        \
        SLA_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(IndexReg,R.TempByte);     \
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
        R.TempByte = RD_BYTE_INDEX(Index);      \
                                                                        \
        SRA_WITH_FLAGS(R.TempByte);                                             \
                                                                        \
        WR_BYTE_INDEX(Index,R.TempByte);                \
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
        R.TempByte = RD_BYTE_INDEX(Index);      \
                                                                        \
        SRL_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(Index,R.TempByte);                \
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
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                        \
        SLL_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(IndexReg,R.TempByte);     \
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
#if 0
#define INC_INDEX8(Register)            \
{                                                                       \
        INC_X(Register);                                \
                                                                        \
        R.PC.W+=2;                                              \
}
#endif

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
        R.TempByte = RD_BYTE_INDEX(Index);      \
                                                                        \
        INC_X(R.TempByte);                                      \
                                                                        \
        WR_BYTE_INDEX(Index,R.TempByte);                \
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

#if 0
#define DEC_INDEX8(Register)            \
{                                                                       \
        DEC_X(Register);                                \
                                                                       \
        R.PC.W+=2;                                              \
}
#endif

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
        R.TempByte = RD_BYTE_INDEX(Index);      \
                                                                        \
        DEC_X(R.TempByte);                                      \
                                                                        \
        WR_BYTE_INDEX(Index,R.TempByte);                \
}

/*-----------------*/

#define LD_RR_nn(Register) \
        Register = Z80_RD_OPCODE_WORD(1);	/*Z80_RD_WORD((Z80_WORD)(R.PC.W+1));*/ \

#define LD_INDEXRR_nn(Index) \
        Index = Z80_RD_OPCODE_WORD(2)	/*Z80_RD_WORD((Z80_WORD)(R.PC.W+2))*/
     
#define LD_INDEXRR_nnnn(Index)          \
{                                                                       \
        Z80_WORD        Addr;                                   \
                                                                        \
        Addr = Z80_RD_OPCODE_WORD(2);	/*Z80_RD_WORD((Z80_WORD)(R.PC.W+2));*/       \
                                                                        \
        Index = Z80_RD_WORD(Addr);              \
}

#define LD_nnnn_INDEXRR(Index)          \
{                                                                       \
        Z80_WORD                Addr;                                   \
                                                                        \
        Addr = Z80_RD_OPCODE_WORD(2);	/*Z80_RD_WORD((Z80_WORD)(R.PC.W+2));*/       \
                                                                        \
        Z80_WR_WORD(Addr,Index);                \
}

#define LD_RR_nnnn(Register)            \
{                                                                       \
        Z80_WORD                Addr;                                   \
                                                                        \
        Addr = Z80_RD_OPCODE_WORD(2);	/*Z80_RD_WORD((Z80_WORD)(R.PC.W+2));*/       \
                                                                        \
        Register = Z80_RD_WORD(Addr);   \
                                                                        \
}

#define LD_nnnn_RR(Register)            \
{                                                                       \
        Z80_WORD        Addr;                                   \
                                                                        \
        Addr = Z80_RD_OPCODE_WORD(2);	/*Z80_RD_WORD((Z80_WORD)(R.PC.W+2));*/       \
                                                                        \
        Z80_WR_WORD(Addr, Register);    \
                                                                        \
}


        


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



#define ADD_RR_rr(Register1, Register2)                 \
{                                                                       \
        Z80_LONG                Result=0;                               \
                                                                        \
        Result = (Z80_LONG)Register1 + (Z80_LONG)Register2;             \
                                                                        \
        SET_CARRY_FLAG_ADD16(Result);           \
                                                                        \
        Register1 = (Z80_WORD)Result;           \
                                                                        \
        Z80_FLAGS_REG = Z80_FLAGS_REG & (Z80_CARRY_FLAG | Z80_ZERO_FLAG | Z80_PARITY_FLAG | Z80_SIGN_FLAG); \
}


/*#ifndef ASM */
#define ADC_HL_rr(Register)                     \
{                                                                       \
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
}


#define SBC_HL_rr(Register)                     \
{                                                                       \
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
}

/* do a sla of index and copy into reg specified */
#define INDEX_CB_SLA_REG(IndexReg, Reg)         \
{                                                                       \
        /*Z80_BYTE      Data;*/                         \
                                                                        \
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                        \
        SLA_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(IndexReg,R.TempByte);     \
                                                                        \
        Reg = R.TempByte;                                               \
}

/* do a sra of index and copy into reg specified */
#define INDEX_CB_SRA_REG(IndexReg, Reg)         \
{                                                                       \
        /*Z80_BYTE      Data;*/                         \
                                                                        \
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                        \
        SRA_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(IndexReg,R.TempByte);     \
                                                                        \
        Reg = R.TempByte;                                               \
}

/* do a sll of index and copy into reg specified */
#define INDEX_CB_SLL_REG(IndexReg, Reg)         \
{                                                                       \
        /*Z80_BYTE      Data;*/                         \
                                                                        \
        R.TempByte =  RD_BYTE_INDEX(IndexReg);  \
                                                                        \
        SLL_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(IndexReg,R.TempByte);     \
                                                                        \
        Reg = R.TempByte;                                               \
}

/* do a srl of index and copy into reg specified */
#define INDEX_CB_SRL_REG(IndexReg, Reg)         \
{                                                                       \
        /*Z80_BYTE      Data;   */                      \
                                                                        \
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                        \
        SRL_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(IndexReg,R.TempByte);     \
                                                                        \
        Reg = R.TempByte;                                               \
}

#define INDEX_CB_RLC_REG(IndexReg, Reg)         \
{                                                                       \
        /*Z80_BYTE      Data;   */                      \
                                                                        \
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                        \
        RLC_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(IndexReg,R.TempByte);     \
                                                                        \
        Reg = R.TempByte;                                               \
}

#define INDEX_CB_RRC_REG(IndexReg, Reg)         \
{                                                                       \
        /*Z80_BYTE      Data;*/                         \
                                                                        \
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                        \
        RRC_WITH_FLAGS(R.TempByte);                     \
                                                                        \
        WR_BYTE_INDEX(IndexReg,R.TempByte);     \
                                                                        \
        Reg = R.TempByte;                                               \
}


#define INDEX_CB_RR_REG(IndexReg, Reg)          \
{                                                                       \
        /*Z80_BYTE      Data;*/                         \
                                                                        \
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                        \
        RR_WITH_FLAGS(R.TempByte);                      \
                                                                        \
        WR_BYTE_INDEX(IndexReg,R.TempByte);     \
                                                                        \
        Reg = R.TempByte;                                               \
                                                                        \
}


#define INDEX_CB_RL_REG(IndexReg, Reg)          \
{                                                                       \
        /*Z80_BYTE      Data;*/                         \
                                                                        \
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                        \
        RL_WITH_FLAGS(R.TempByte);                      \
                                                                        \
        WR_BYTE_INDEX(IndexReg,R.TempByte);     \
                                                                        \
        Reg = R.TempByte;                                               \
                                                                        \
}

#define INDEX_CB_SET_REG(OrMask, IndexReg, Reg) \
{                                                               \
        /*Z80_BYTE      Data;*/                         \
                                                                \
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                \
        SET(OrMask, R.TempByte);                        \
                                                                \
        WR_BYTE_INDEX(IndexReg,R.TempByte);     \
                                                               \
        Reg = R.TempByte;                                               \
}

#define INDEX_CB_RES_REG(AndMask,IndexReg, Reg) \
{                                                               \
        /*Z80_BYTE Data;*/                              \
                                                                \
        R.TempByte = RD_BYTE_INDEX(IndexReg);   \
                                                                \
        RES(AndMask, R.TempByte);                       \
                                                                \
        WR_BYTE_INDEX(IndexReg,R.TempByte);     \
        Reg = R.TempByte;                                               \
}

#define PrefixIgnore()		\
	R.Flags &= ~Z80_CHECK_INTERRUPT_FLAG;		\
	R.PC.W.l++;	/*R.PC.W++;*/									\
	INC_REFRESH(1);								\
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

/*---------------------------------------------------------*/
/* perform a RST which is equivalent to a 1 byte CALL */

#define RST(Addr)						\
{										\
        /* push return address on stack */	\
        PUSH((Z80_WORD)(R.PC.W.l+1));			\
											\
        /* set program counter to address */	\
        R.PC.W.l = Addr;							\
}

#define LD_R_R(Reg1,Reg2)				\
		Reg1 = Reg2

#define SET_IM(x)				\
        R.IM = x;				\

#define HALT()			    R.Flags |=Z80_EXECUTING_HALT_FLAG
           

#define LD_I_A() R.I = R.AF.B.h

/* increment register pair */
#define INC_rp(x)	(x++)
/* decrement register pair */
#define DEC_rp(x)	(x--)


/* swap two words */
#define SWAP(Reg1,Reg2) \
{                                               \
        Z80_WORD        tempR;  \
                                                \
        tempR = Reg1;           \
        Reg1 = Reg2;            \
        Reg2 = tempR;           \
}

#define JP_rp(x)	(R.PC.W.l=x)

#define LD_SP_rp(x)	(R.SP.W=x)


/* EX (SP), HL */
#define EX_SP_rr(Register)                              \
{                                                                               \
        Z80_WORD        temp;                                   \
                                                                                \
        temp = Z80_RD_WORD(R.SP.W);                     \
        Z80_WR_WORD(R.SP.W, Register);          \
        Register = temp;                                        \
}

#define ADD_PC(x)	(R.PC.W.l+=x)

