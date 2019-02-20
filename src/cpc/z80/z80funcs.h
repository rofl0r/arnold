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

extern unsigned char *pReadRamPtr[8];

/* read a opcode byte */
INLINE static Z80_BYTE	Z80_RD_OPCODE_BYTE(unsigned long Offset)
{
	unsigned long MemBlock;
	unsigned char *pAddr;
	unsigned short Addr;
	
	Addr = (R.PC.W.l+Offset);
	MemBlock = (Addr>>13);
	pAddr = (unsigned char *)((unsigned long)pReadRamPtr[MemBlock] + (unsigned long)Addr);

	return pAddr[0];
}
/*---------------------------------------------------------*/
/* jump relative to a memory location */

INLINE static void JR(void)
{
        Z80_BYTE_OFFSET Offset;

        Offset = (Z80_BYTE_OFFSET)Z80_RD_OPCODE_BYTE(1);	

        R.PC.W.l = R.PC.W.l + (Z80_LONG)2 + Offset;
}


/* read a opcode word */
INLINE static Z80_WORD	Z80_RD_OPCODE_WORD(int Offset)
{
	return (Z80_RD_OPCODE_BYTE(Offset) | (Z80_RD_OPCODE_BYTE(Offset+1))<<8);
}

INLINE static Z80_WORD POP(void)
{
    Z80_WORD        Data;

    Data = Z80_RD_WORD(R.SP.W);

    R.SP.W+=2;

    return Data;    
}


INLINE static void RETURN()				
{								
    /* get return address from stack */	
    R.MemPtr.W = POP();				
	R.PC.W.l = R.MemPtr.W;
	/* no flags changed */
}
						

INLINE static void CALL_I(Z80_WORD Addr)
{
	// for now we don't store return address on stack here...
	// we do it in the appropiate routine
	//
    /* set program counter to sub-routine address */
    R.MemPtr.W = Addr;
	R.PC.W.l = R.MemPtr.W;
}


INLINE static void CALL(void)
{
    /* store return address on stack */
    PUSH((Z80_WORD)(R.PC.W.l+3));

	CALL_I(Z80_RD_OPCODE_WORD(1));
	/* no flags changed */
}


INLINE static void CALL_IM0(void)
{
    /* store return address on stack */
    PUSH((Z80_WORD)(R.PC.W.l));

	CALL_I(Z80_RD_WORD_IM0());
		/* no flags changed */

}

INLINE static int DJNZ_dd(void)
{
        /* decrement B */
        R.BC.B.h--;

        /* if zero */
        if (R.BC.B.h==0)
        {

                /* continue */
                ADD_PC(2);	
                
				return 3;

        }
        else
        {
                /* branch */
                JR();

				return 4;
		}
		/* no flags changed */
}


INLINE static int DJNZ_dd_IM0(void)
{
        /* decrement B */
        R.BC.B.h--;

        /* if zero */
        if (R.BC.B.h==0)
        {

				return 3;	

        }
        else
        {
                /* branch */
                JR();

				return 4;	
		}
		/* no flags changed */
}

INLINE static void SETUP_INDEXED_ADDRESS(Z80_WORD Index)	
{												
	Z80_BYTE_OFFSET Offset;						
							
	// -127->129
    Offset = (Z80_BYTE_OFFSET)Z80_RD_OPCODE_BYTE(2);
												
	R.MemPtr.W = Index+Offset;			
}

INLINE static Z80_BYTE  RD_BYTE_INDEX()
{
     return Z80_RD_BYTE(R.MemPtr.W);       
}

INLINE static WR_BYTE_INDEX(Z80_BYTE Data)	
{	
	Z80_WR_BYTE(R.MemPtr.W, Data); 
} 





INLINE static void ADD_A_HL(void)
{

        R.TempByte = Z80_RD_BYTE(R.HL.W);

        ADD_A_X(R.TempByte);
}
#if 0
INLINE static void ADD_A_n(void)
{

        R.TempByte = Z80_RD_OPCODE_BYTE(1);	

        ADD_A_X(R.TempByte);
}
#endif

#define ADD_A_INDEX(Index)              \
{                                                                       \
        /*Z80_BYTE      Data;*/                                 \
                                                                        \
 		SETUP_INDEXED_ADDRESS(Index); \
       R.TempByte = RD_BYTE_INDEX(Index);      \
                                                                        \
        ADD_A_X(R.TempByte);                                    \
}

INLINE static void ADC_A_HL(void) 
{

        R.TempByte = Z80_RD_BYTE(R.HL.W);

        ADC_A_X(R.TempByte);
}

#if 0
INLINE static void ADC_A_n(void) 
{

        R.TempByte = Z80_RD_OPCODE_BYTE(1);

        ADC_A_X(R.TempByte);

}
#endif

#define ADC_A_INDEX(Index)              \
{                                                               \
        /*Z80_BYTE      Data;*/                         \
                                                                \
		SETUP_INDEXED_ADDRESS(Index); \
        R.TempByte = RD_BYTE_INDEX(Index);      \
                                                                                \
        ADC_A_X(R.TempByte);                                    \
}


INLINE static void SUB_A_HL(void) 
{
 
        R.TempByte = Z80_RD_BYTE(R.HL.W);

        SUB_A_X(R.TempByte);
}

#if 0
INLINE static void SUB_A_n(void) 
{
/*      Z80_BYTE        Data;*/

        R.TempByte = Z80_RD_OPCODE_BYTE(1);	

        SUB_A_X(R.TempByte);
}
#endif

#define SUB_A_INDEX(Index)              \
{                                                               \
        /*Z80_BYTE      Data;           */              \
                                                                \
		SETUP_INDEXED_ADDRESS(Index); \
        R.TempByte = RD_BYTE_INDEX(Index);      \
                                                                        \
        SUB_A_X(R.TempByte);                                    \
}

INLINE static void SBC_A_HL(void) 
{
        /*Z80_BYTE      Data;*/

        R.TempByte = Z80_RD_BYTE(R.HL.W);

        SBC_A_X(R.TempByte);
}

#if 0
INLINE static void SBC_A_n(void) 
{
        /*Z80_BYTE      Data;*/

        R.TempByte = Z80_RD_OPCODE_BYTE(1);	

        SBC_A_X(R.TempByte);

}
#endif

#define SBC_A_INDEX(Index)              \
{                                                               \
        /*Z80_BYTE      Data;*/                         \
                                                                \
		SETUP_INDEXED_ADDRESS(Index); \
        R.TempByte = RD_BYTE_INDEX(Index);      \
                                                                        \
        SBC_A_X(R.TempByte);                                    \
}

INLINE static void CP_A_HL(void)
{
        /*Z80_BYTE      Data;*/

        R.TempByte = Z80_RD_BYTE(R.HL.W);

        CP_A_X(R.TempByte);
}

#if 0
INLINE static void CP_A_n(void)
{
        /*Z80_BYTE      Data;*/

        R.TempByte = Z80_RD_OPCODE_BYTE(1);	

        CP_A_X(R.TempByte);

}
#endif

#define CP_A_INDEX(Index)               \
{                                                                       \
        /*int   Data;   */                              \
                                                                        \
		SETUP_INDEXED_ADDRESS(Index); \
        R.TempByte = RD_BYTE_INDEX(Index);      \
                                                                        \
        CP_A_X(R.TempByte);                                     \
}

#if 0
INLINE static void AND_A_n(void) 
{

        R.TempByte = Z80_RD_OPCODE_BYTE(1);	

        AND_A_X(R.TempByte);
}
#endif

INLINE static void AND_A_HL(void) 
{

        R.TempByte = Z80_RD_BYTE(R.HL.W);

        AND_A_X(R.TempByte);
}

#if 0
INLINE static void XOR_A_n(void) 
{

        R.TempByte = Z80_RD_OPCODE_BYTE(1);	

        XOR_A_X(R.TempByte);
}
#endif 

INLINE static void XOR_A_HL(void) 
{

        R.TempByte = Z80_RD_BYTE(R.HL.W);

        XOR_A_X(R.TempByte);
}

INLINE static void OR_A_HL(void) 
{

        R.TempByte = Z80_RD_BYTE(R.HL.W);

        OR_A_X(R.TempByte);
}

#if 0
INLINE static void OR_A_n(void) 
{

        R.TempByte = Z80_RD_OPCODE_BYTE(1);	

        OR_A_X(R.TempByte);
}
#endif



INLINE void RRA() 
{                               
        RR(R.AF.B.h);
                                                                
        A_SHIFTING_FLAGS;                                       
}                               

#define RLA() \
{ \
	Z80_BYTE	OrByte;	\
	Z80_BYTE	Flags;	\
	OrByte = Z80_FLAGS_REG & 0x01;	\
	Flags = Z80_FLAGS_REG;			\
	Flags = Flags & (Z80_SIGN_FLAG | Z80_ZERO_FLAG | Z80_PARITY_FLAG);	\
	Flags |= ((R.AF.B.h>>7) & 0x01); \
	{							\
		Z80_BYTE	Reg;		\
		Reg = R.AF.B.h;			\
		Reg = Reg<<1;			\
		Reg = Reg|OrByte;		\
		R.AF.B.h = Reg;			\
		Reg &= (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	\
		Flags = Flags | Reg;	\
	} \
	Z80_FLAGS_REG = Flags;			\
}

#define RLCA() \
{ \
	Z80_BYTE        OrByte; \
	Z80_BYTE		Flags; \
	OrByte = (R.AF.B.h>>7) & 0x01;	\
	Flags = Z80_FLAGS_REG; \
	Flags = Flags & (Z80_SIGN_FLAG | Z80_ZERO_FLAG | Z80_PARITY_FLAG);	\
    Flags |= OrByte;			\
	{							\
		Z80_BYTE Reg;			\
								\
		Reg = R.AF.B.h;			\
		Reg = Reg<<1;			\
		Reg = Reg|OrByte;		\
	    R.AF.B.h=Reg;            \
		Reg &= (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	\
		Flags = Flags | Reg;	\
	}		\
	Z80_FLAGS_REG = Flags;				\
}

#define RRCA() \
{               \
	Z80_BYTE	Flags;	\
	Z80_BYTE	OrByte;	\
	OrByte = R.AF.B.h & 0x01;			\
	Flags = Z80_FLAGS_REG;	\
	Flags = Flags & (Z80_SIGN_FLAG | Z80_ZERO_FLAG | Z80_PARITY_FLAG); \
	Flags |= OrByte; \
	OrByte = OrByte<<7;	\
	{					\
		Z80_BYTE Reg;	\
		Reg = R.AF.B.h; \
		Reg = Reg>>1; \
		Reg = Reg | OrByte; \
		R.AF.B.h = Reg;		\
		Reg = Reg & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	\
		Flags = Flags | Reg;								\
	} \
	Z80_FLAGS_REG = Flags; \
}

/* LDI */
#define LDI()		\
{					\
        Z80_BYTE      Data;	\
		Z80_BYTE		Flags;	\
							\
        Data  = Z80_RD_BYTE(R.HL.W);	\
        Z80_WR_BYTE(R.DE.W,Data);		\
										\
        R.HL.W++;						\
        R.DE.W++;						\
        R.BC.W--;						\
										\
        Flags = Z80_FLAGS_REG & (Z80_CARRY_FLAG | Z80_ZERO_FLAG | Z80_SIGN_FLAG);	/* Do not change CF, ZF, SF */ \
		/* HF, NF = 0 */ \
		/* according to Sean's z80 documentation: (Data+A) bit 1 is YF, bit 3 is XF */ \
		Flags |= ((Data + R.AF.B.h)&((1<<1)|(1<<3)))<<2; \
        /* if BC==0, then PV =0, else PV = 1 */	\
        if (R.BC.W!=0)						\
        {									\
                Flags |= Z80_PARITY_FLAG;	\
        }								\
		Z80_FLAGS_REG = Flags;				\
}

/* LDIR */
#define LDIR() \
{ \
    Z80_BYTE      Data;	\
	Z80_BYTE		Flags;	\
						\
    Data  = Z80_RD_BYTE(R.HL.W);	\
    Z80_WR_BYTE(R.DE.W,Data);		\
									\
    R.HL.W++;						\
    R.DE.W++;						\
    R.BC.W--;						\
									\
    Flags = Z80_FLAGS_REG & (Z80_CARRY_FLAG | Z80_ZERO_FLAG | Z80_SIGN_FLAG|Z80_PARITY_FLAG);	/* Do not change CF, ZF, SF */ \
	/* HF, NF = 0 */ \
	/* according to Sean's z80 documentation: (Data+A) bit 1 is YF, bit 3 is XF */ \
	Flags |= ((Data + R.AF.B.h)&((1<<1)|(1<<3)))<<2; \
    /* if BC==0, then PV =0 */	\
    if (R.BC.W==0)						\
    {									\
        Flags &= ~Z80_PARITY_FLAG;	\
		ADD_PC(2);					\
		Cycles=6;					\
    }								\
	else							\
	{								\
		R.MemPtr.W = R.PC.W.l+1;	\
		Cycles=5;					\
	}								\
	Z80_FLAGS_REG = Flags;				\
} \


/* LDD */
#define LDD()			\
{						\
        Z80_BYTE      Data,Flags;		\
										\
        Data  = Z80_RD_BYTE(R.HL.W);	\
        Z80_WR_BYTE(R.DE.W,Data);		\
										\
        R.HL.W--;						\
        R.DE.W--;						\
        R.BC.W--;						\
										\
        Flags = Z80_FLAGS_REG & (Z80_CARRY_FLAG | Z80_ZERO_FLAG | Z80_SIGN_FLAG);	/* Do not change CF, ZF, SF */ \
		/* HF, NF = 0 */ \
		/* according to Sean's z80 documentation: (Data+A) bit 1 is YF, bit 3 is XF
		Flags |= ((Data + R.AF.B.h)&((1<<1)|(1<<3)))<<2; \
        /* if BC==0, then PV =0, else PV = 1 */	\
        if (R.BC.W!=0)						\
        {									\
                Flags |= Z80_PARITY_FLAG;	\
        }								\
		Z80_FLAGS_REG = Flags;				\
}

	
/* LDDR */
#define LDDR()			\
{						\
        Z80_BYTE      Data,Flags;		\
										\
        Data  = Z80_RD_BYTE(R.HL.W);	\
        Z80_WR_BYTE(R.DE.W,Data);		\
										\
        R.HL.W--;						\
        R.DE.W--;						\
        R.BC.W--;						\
										\
    Flags = Z80_FLAGS_REG & (Z80_CARRY_FLAG | Z80_ZERO_FLAG | Z80_SIGN_FLAG|Z80_PARITY_FLAG);	/* Do not change CF, ZF, SF */ \
	/* HF, NF = 0 */ \
	/* according to Sean's z80 documentation: (Data+A) bit 1 is YF, bit 3 is XF */ \
	Flags |= ((Data + R.AF.B.h)&((1<<1)|(1<<3)))<<2; \
    /* if BC==0, then PV =0 */	\
    if (R.BC.W==0)						\
    {									\
        Flags &= ~Z80_PARITY_FLAG;	\
		ADD_PC(2);					\
		Cycles=6;					\
    }								\
	else							\
	{								\
		R.MemPtr.W = R.PC.W.l+1;	\
		Cycles=5;					\
	}								\
	Z80_FLAGS_REG = Flags;				\
} 

static void CPI(void)
{
        Z80_BYTE        Result;

        R.TempByte = Z80_RD_BYTE(R.HL.W);
        Result = R.AF.B.h - R.TempByte;
        
        Z80_FLAGS_REG = Z80_FLAGS_REG | Z80_SUBTRACT_FLAG;
        SET_ZERO_SIGN(Result);

        R.HL.W++;
        R.BC.W--;
		R.MemPtr.W++;


        Z80_FLAGS_REG = Z80_FLAGS_REG & (~Z80_PARITY_FLAG);
        if (R.BC.W!=0)
        {
                Z80_FLAGS_REG = Z80_FLAGS_REG | Z80_PARITY_FLAG;
        }
}

static void CPD(void)
{
        Z80_BYTE        Result;

        R.TempByte = Z80_RD_BYTE(R.HL.W);
        Result = R.AF.B.h - R.TempByte;
        
        Z80_FLAGS_REG = Z80_FLAGS_REG | Z80_SUBTRACT_FLAG;
        SET_ZERO_SIGN(Result);

        R.HL.W--;
        R.BC.W--;
		R.MemPtr.W--;

        Z80_FLAGS_REG = Z80_FLAGS_REG & (~Z80_PARITY_FLAG);
        if (R.BC.W!=0)
        {
                Z80_FLAGS_REG = Z80_FLAGS_REG | Z80_PARITY_FLAG;
        }
}

#if 0
static void CPIR(void)
{
        CPI();

        /* not zero and BC isn't zero */
        if (((Z80_FLAGS_REG & Z80_ZERO_FLAG)==0) && (R.BC.W!=0))
        {
                R.PC.W-=2;
        }
}

static void CPDR(void)
{
        CPD();

        /* if A=(HL) and BC!=0 */
        if (((Z80_FLAGS_REG & Z80_ZERO_FLAG)==0) && (R.BC.W!=0))
        {
                R.PC.W-=2;
        }
}
#endif

static int OUTI(void)
{

/*        Z80_UpdateCycles(2); */

        R.BC.B.h --;
		
        SET_ZERO_FLAG(R.BC.B.h);

        R.TempByte = Z80_RD_BYTE(R.HL.W);

        Z80_DoOut(R.BC.W,R.TempByte);                   
		R.MemPtr.W = R.BC.W+1;

        Z80_FLAGS_REG |= Z80_SUBTRACT_FLAG;

        R.HL.W++;

 /*       Z80_UpdateCycles(3); */
		return 5;
}

#if 0
/* two R refresh per instruction execution */
static void OTIR(void)
{
        OUTI();

        if ((Z80_FLAGS_REG & Z80_ZERO_FLAG)==0)
        {
                R.PC.W-=2;
        }
}
#endif

/* B is pre-decremented before execution */
static int OUTD(void)
{

        /*Z80_UpdateCycles(2); */

        R.BC.B.h--;

        SET_ZERO_FLAG(R.BC.B.h);

        R.TempByte = Z80_RD_BYTE(R.HL.W);

        Z80_DoOut(R.BC.W,R.TempByte);
		R.MemPtr.W = R.BC.W-1;

        /* as per Zilog docs */
        Z80_FLAGS_REG |= Z80_SUBTRACT_FLAG;

        R.HL.W--;

        /*Z80_UpdateCycles(3); */
		return 5;
}

static void INI(void)
{
        
        R.TempByte = Z80_DoIn(R.BC.W);

        Z80_WR_BYTE(R.HL.W,R.TempByte);

        R.HL.W++;
		R.MemPtr.W = R.BC.W+1;

        R.BC.B.h--;

        SET_ZERO_FLAG(R.BC.B.h);

        Z80_FLAGS_REG |= Z80_SUBTRACT_FLAG;
}


static void IND(void)
{

        R.TempByte = Z80_DoIn(R.BC.W);          
                
        Z80_WR_BYTE(R.HL.W,R.TempByte);

        R.HL.W--;
		R.MemPtr.W = R.BC.W-1;
        R.BC.B.h--;

        SET_ZERO_FLAG(R.BC.B.h);

        Z80_FLAGS_REG |= Z80_SUBTRACT_FLAG;
}

#include "z80daa.h"



/* half carry not set */
static void DAA(void)
{
 int i;
 i=R.AF.B.h;
 if (R.AF.B.l&Z80_CARRY_FLAG) i|=256;
 if (R.AF.B.l&Z80_HALFCARRY_FLAG) i|=512;
 if (R.AF.B.l&Z80_SUBTRACT_FLAG) i|=1024;
 R.AF.W=DAATable[i];
}

