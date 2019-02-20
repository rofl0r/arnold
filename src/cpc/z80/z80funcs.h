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
/***** the following are used to speed up the emulation! */

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

/* read a opcode word */
INLINE static Z80_WORD	Z80_RD_OPCODE_WORD(int Offset)
{
	return (Z80_RD_OPCODE_BYTE(Offset) | (Z80_RD_OPCODE_BYTE(Offset+1))<<8);
}

/* in interrupt mode 0 (IM0) the program counter is not incremented */
/* therefore the same byte is fetched twice to construct the address */
/* the initial opcode byte is provided by the interrupting device with */
/* subsequent bytes taken from PC location */
INLINE static Z80_WORD	Z80_RD_OPCODE_WORD_IM0(void)
{
	Z80_BYTE Byte;

	Byte = Z80_RD_OPCODE_BYTE(0);

	return (Byte | (Byte<<8));
}

#if 0
INLINE static void LD_HL_n(void)
{
        /*Z80_BYTE      Data;*/

        R.TempByte = Z80_RD_OPCODE_BYTE(1);	

        Z80_WR_BYTE(R.HL.W,R.TempByte);
}
#endif

/*---------------------------------------------------------*/
/* pop a word from the stack */

INLINE static Z80_WORD POP(void)
{
        Z80_WORD        Data;

        /*R.TempWord*/ Data = Z80_RD_WORD(R.SP.W);

        R.SP.W+=2;

        return Data;    
}

#define RETN()			\
{						\
        R.IFF1 = R.IFF2;\
						\
        R.PC.W.l = POP();	\
}						\

#define RETI()			\
{						\
        Z80_Reti();		\
						\
        R.PC.W.l = POP();	\
}

#define NEG() \
{ \
	Z80_BYTE	Flags;	\
	Z80_BYTE	AReg; \
						\
	AReg = R.AF.B.h;		\
    Flags = Z80_SUBTRACT_FLAG;	\
													\
    if (AReg == 0x080)									\
    {												\
          Flags |= Z80_PARITY_FLAG;					\
    }												\
													\
    if (AReg != 0x000)									\
    {												\
        Flags |= Z80_CARRY_FLAG;					\
    }												\
													\
	if ((AReg & 0x0f)!=0)								\
	{												\
		Flags |= Z80_HALFCARRY_FLAG;				\
	}												\
													\
    R.AF.B.h = -AReg;							\
													\
	Flags |= ZeroSignTable[R.AF.B.h];				\
	Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	\
	Z80_FLAGS_REG = Flags;								\
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

INLINE static int OUT_n_A(void)       
{
        Z80_WORD Port;

        /* A in upper byte of port, Data in lower byte of port */

        Port = (Z80_WORD)Z80_RD_OPCODE_BYTE(1) /*Z80_RD_BYTE((Z80_WORD)(R.PC.W+1))*/ | ((Z80_WORD)(R.AF.B.h)<<8);

/*        Z80_UpdateCycles(2); */

        Z80_DoOut(Port, R.AF.B.h);

  /*      Z80_UpdateCycles(1); */

		return 3;
}

INLINE static int IN_A_n(void)        
{
        Z80_WORD Port;

        
        Port = (Z80_WORD)Z80_RD_OPCODE_BYTE(1)	/*Z80_RD_BYTE((Z80_WORD)(R.PC.W+1))*/ | ((Z80_WORD)(R.AF.B.h)<<8);

      /*  Z80_UpdateCycles(2); */

        /* a in upper byte of port, data in lower byte of port */
        R.AF.B.h = Z80_DoIn(Port);

	/*	Z80_UpdateCycles(1); */
		return 3;
}


#if 0
INLINE void RLA()
{                                                          
        RL(R.AF.B.h);	
        A_SHIFTING_FLAGS;	                                       
}
#endif

INLINE void RRA() 
{                               
        RR(R.AF.B.h);
                                                                
        A_SHIFTING_FLAGS;                                       
}                               

#if 0
INLINE void RLCA(void)
{                                                                       
       RLC(R.AF.B.h);

        A_SHIFTING_FLAGS;                                               
}
#endif

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

/*---------------------------------------------------------*/
/* CPL */

#define CPL()				\
{							\
	Z80_BYTE Flags;			\
        /* complement */	\
        R.AF.B.h = (Z80_BYTE)(~R.AF.B.h);	\
											\
		Flags = Z80_FLAGS_REG;					\
		Flags = Flags & (Z80_SIGN_FLAG | Z80_ZERO_FLAG | Z80_PARITY_FLAG | Z80_CARRY_FLAG);	\
		Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	\
		Flags |= Z80_SUBTRACT_FLAG | Z80_HALFCARRY_FLAG;			\
        Z80_FLAGS_REG = Flags;											\
}

/*---------------------------------------------------------------------------*/
/* SCF */
#define SCF()					\
{								\
	Z80_BYTE	Flags;			\
								\
	Flags = Z80_FLAGS_REG;			\
								\
	Flags = Flags & (Z80_ZERO_FLAG | Z80_PARITY_FLAG | Z80_SIGN_FLAG);	\
    Flags = Flags | Z80_CARRY_FLAG;										\
	Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);			\
																		\
	Z80_FLAGS_REG = Flags;													\
}

/*---------------------------------------------------------------------------*/
/* CCF */
#define CCF()					\
{								\
	Z80_BYTE Flags;				\
								\
	Flags = Z80_FLAGS_REG;			\
    Flags &= (Z80_CARRY_FLAG | Z80_ZERO_FLAG | Z80_PARITY_FLAG | Z80_SIGN_FLAG);	\
	Flags |= ((Flags & Z80_CARRY_FLAG)<<Z80_HALFCARRY_FLAG_BIT);					\
	Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);						\
    Z80_FLAGS_REG = Flags ^ Z80_CARRY_FLAG;												\
}

/*---------------------------------------------------------*/
/* LD R,A */
#define LD_R_A()	\
{					\
	Z80_BYTE	A;	\
					\
	A = R.AF.B.h;	\
					\
    /* store bit 7 */	\
    R.RBit7 = A & 0x080; \
							\
    /* store refresh register */	\
    R.R = A & 0x07f;				\
}

/*---------------------------------------------------------*/
/* LD A,I */

#define LD_A_I()	\
{					\
        R.AF.B.h = R.I;	\
		{				\
			Z80_BYTE	Flags;	\
								\
			Flags = Z80_FLAGS_REG;	\
			Flags &= Z80_CARRY_FLAG;	\
			Flags |= ((R.IFF2 & 0x01)<<Z80_PARITY_FLAG_BIT);	\
			Flags |= ZeroSignTable2[R.AF.B.h];	\
			Z80_FLAGS_REG = Flags;	\
		}	\
}

/*---------------------------------------------------------*/
/* LD A,R */
#define LD_A_R()			\
{							\
        R.AF.B.h = Z80_GET_R;	\
							\
		{					\
			Z80_BYTE	Flags;	\
								\
			Flags = Z80_FLAGS_REG;	\
			Flags &= Z80_CARRY_FLAG;		\
			Flags |= ((R.IFF2 & 0x01)<<Z80_PARITY_FLAG_BIT);	\
			Flags |= ZeroSignTable2[R.AF.B.h];		\
			Z80_FLAGS_REG = Flags;		\
		}					\
}


INLINE static void RRD(void)
{

        R.TempByte = Z80_RD_BYTE(R.HL.W);

        Z80_WR_BYTE(R.HL.W, (Z80_BYTE)(((R.TempByte>>4) | (R.AF.B.h<<4))));

        R.AF.B.h = (R.AF.B.h & 0x0f0) | (R.TempByte & 0x0f);

		{
			Z80_BYTE	Flags;

			Flags = Z80_FLAGS_REG;
			Flags &= Z80_CARRY_FLAG;
			Flags |= ZeroSignParityTable[R.AF.B.h];
			Z80_FLAGS_REG = Flags;
		}
}

INLINE static void RLD(void)
{
	R.TempByte = Z80_RD_BYTE(R.HL.W);

        Z80_WR_BYTE(R.HL.W,(Z80_BYTE)((R.TempByte<<4)|(R.AF.B.h & 0x0f)));

        R.AF.B.h = (R.AF.B.h & 0x0f0) | (R.TempByte>>4);


		{
			Z80_BYTE	Flags;

			Flags = Z80_FLAGS_REG;
			Flags &= Z80_CARRY_FLAG;
			Flags |= ZeroSignParityTable[R.AF.B.h];
			Z80_FLAGS_REG = Flags;
		}
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
        Flags = Z80_FLAGS_REG & (Z80_CARRY_FLAG | Z80_ZERO_FLAG | Z80_SIGN_FLAG);	\
										\
        /* if BC==0, then PV =0, else PV = 1 */	\
        if (R.BC.W!=0)						\
        {									\
                Flags |= Z80_PARITY_FLAG;	\
        }								\
		Z80_FLAGS_REG = Flags;				\
}

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
        Flags = Z80_FLAGS_REG & (Z80_CARRY_FLAG | Z80_ZERO_FLAG | Z80_SIGN_FLAG);	\
										\
        /* if BC==0, then PV =0, else PV = 1 */	\
        if (R.BC.W!=0)						\
        {									\
                Flags |= Z80_PARITY_FLAG;	\
        }								\
		Z80_FLAGS_REG = Flags;				\
}

#if 0
/* LDIR */
INLINE static void      LDIR(void)
{
        LDI();

        NOP_Cycle = 5;
        
        if (R.BC.W!=0)
        {
                NOP_Cycle = 6;

                R.PC.W-=2;
        }
}

/* LDDR */
INLINE static int      LDDR(void)
{
        LDD();


        if (Z80_FLAGS_REG & Z80_PARITY_FLAG)
        {
                R.PC.W-=2;

				return 6;
				
        }
		else
		{
			return 5;
		}
}
#endif


static void CPI(void)
{
        Z80_BYTE        Result;

        R.TempByte = Z80_RD_BYTE(R.HL.W);
        Result = R.AF.B.h - R.TempByte;
        
        Z80_FLAGS_REG = Z80_FLAGS_REG | Z80_SUBTRACT_FLAG;
        SET_ZERO_SIGN(Result);

        R.HL.W++;
        R.BC.W--;

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

        /* as per Zilog docs */
        Z80_FLAGS_REG |= Z80_SUBTRACT_FLAG;

        R.HL.W--;

        /*Z80_UpdateCycles(3); */
		return 5;
}

#if 0
/* R incremented by 2 per loop */
static void OTDR(void)
{
        OUTD();

        if ((Z80_FLAGS_REG & Z80_ZERO_FLAG)==0)
        {
                R.PC.W-=2;
        }
}
#endif

static void INI(void)
{
        
        R.TempByte = Z80_DoIn(R.BC.W);

        Z80_WR_BYTE(R.HL.W,R.TempByte);

        R.HL.W++;
        R.BC.B.h--;

        SET_ZERO_FLAG(R.BC.B.h);

        Z80_FLAGS_REG |= Z80_SUBTRACT_FLAG;
}


static void IND(void)
{

        R.TempByte = Z80_DoIn(R.BC.W);          
                
        Z80_WR_BYTE(R.HL.W,R.TempByte);

        R.HL.W--;
        R.BC.B.h--;

        SET_ZERO_FLAG(R.BC.B.h);

        Z80_FLAGS_REG |= Z80_SUBTRACT_FLAG;
}

#if 0
#define LD_HL_nnnn()			\
{								\
        Z80_WORD Addr;			\
											\
        Addr =  Z80_RD_OPCODE_WORD(1);		\
											\
        R.HL.W = Z80_RD_WORD(Addr);			\
}	

#define LD_nnnn_HL()			\
{								\
        Z80_WORD Addr;			\
								\
        Addr =  Z80_RD_OPCODE_WORD(1);		\
											\
        Z80_WR_WORD(Addr,R.HL.W);			\
}

#define LD_A_nnnn()				\
{								\
        Z80_WORD        Addr;	\
											\
        Addr = Z80_RD_OPCODE_WORD(1);		\
											\
        R.AF.B.h = Z80_RD_BYTE(Addr);		\
}

#define LD_nnnn_A()				\
{								\
        Z80_WORD        Addr;	\
									\
        Addr = Z80_RD_OPCODE_WORD(1);	\
										\
        Z80_WR_BYTE(Addr,R.AF.B.h);		\
}
#endif

unsigned long NOP_Cycle;


/*---------------------------------------------------------*/
/* jump to a memory location */

INLINE static void JP(void)
{
        /* set program counter to sub-routine address */
        R.PC.W.l = Z80_RD_OPCODE_WORD(1);	
}

INLINE static void JP_IM0(void)
{
        /* set program counter to sub-routine address */
        R.PC.W.l = Z80_RD_OPCODE_WORD_IM0();	
}

/*---------------------------------------------------------*/
/* jump relative to a memory location */

INLINE static void JR(void)
{
        Z80_BYTE_OFFSET Offset;

        Offset = (Z80_BYTE_OFFSET)Z80_RD_OPCODE_BYTE(1);	

        R.PC.W.l = R.PC.W.l + (Z80_LONG)2 + Offset;
}


INLINE static void JR_IM0(void)
{
        Z80_BYTE_OFFSET Offset;

        Offset = (Z80_BYTE_OFFSET)Z80_RD_OPCODE_BYTE(0);	

		/* need to check this... */
        R.PC.W.l = R.PC.W.l + Offset;
}


/*---------------------------------------------------------*/
/* call a sub-routine */

INLINE static void CALL(void)
{
        /* store return address on stack */
        PUSH((Z80_WORD)(R.PC.W.l+3));

        /* set program counter to sub-routine address */
        R.PC.W.l = Z80_RD_OPCODE_WORD(1);	
}


INLINE static void CALL_IM0(void)
{
		/* to check */
        /* store return address on stack */
        PUSH((Z80_WORD)(R.PC.W.l));

        /* set program counter to sub-routine address */
        R.PC.W.l = Z80_RD_OPCODE_WORD_IM0();	
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
}


/*---------------------------------------------------------*/
/* perform a return from sub-routine */
#define RETURN()				\
{								\
        /* get return address from stack */	\
        R.PC.W.l = POP();				\
}


/*
#define EX_DE_INDEX(Index)                      \
{                                                                       \
        SWAP(Index,R.DE.W);                             \
   R.PC.W+=2;                                           \
}

static void EX_DE_IX(void) { EX_DE_INDEX(R.IX.W); }
static void EX_DE_IY(void) { EX_DE_INDEX(R.IY.W); }
*/

#define DI()					\
{								\
        R.IFF1 = R.IFF2 = 0;	\
        R.Flags &=~Z80_CHECK_INTERRUPT_FLAG;	\
}

#define EI()					\
{								\
        R.IFF1 = R.IFF2 = 1;	\
        R.Flags &=~Z80_CHECK_INTERRUPT_FLAG;	\
}												\


INLINE Z80_BYTE  RD_BYTE_INDEX(Z80_WORD Index)
{
		SETUP_INDEXED_ADDRESS(Index);

        return Z80_RD_BYTE(R.IndexPlusOffset);       
}

/*---------------------------------------------------------*/
/* write a byte of data using index */

INLINE void WR_BYTE_INDEX_OLD(Z80_WORD Index,Z80_BYTE Data)
{
        Z80_BYTE_OFFSET Offset;

        Offset = (Z80_BYTE_OFFSET)Z80_RD_OPCODE_BYTE(2);	

        Z80_WR_BYTE((Z80_WORD)((Index+Offset)),Data);
}

#define WR_BYTE_INDEX(Index,Data)	\
{	\
	Z80_WR_BYTE(R.IndexPlusOffset, Data); \
} \

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

