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
#ifndef __Z80_HEADER_INCLUDED__
#define __Z80_HEADER_INCLUDED__

/* When int handler is called, ints are effectively disabled!!! */

/* Flag definitions and how to calculate flags:

  SIGN:

  8-bit values: Bit 7 state.
  16-bit values: Bit 16 state.

  PARITY:

  Odd: Sum of all binary digits is odd.
  Even: Sum of all binary digits is even.

  ZERO:

  8-bit values: If all 8-bits are 0.
  16-bit values: If all 16-bits are 0.

  OVERFLOW:

  This refers to numbers in signed arithmetic.

  0x000..0x07f are positive numbers,
  0x080..0x0ff are negative numbers.

  Addition:


  +ve   +  +ve  yielding +ve  = no overflow.
  +ve   +  +ve  yielding -ve  = overflow.
  +ve   +  -ve  yielding +ve  = no overflow.
  +ve   +  -ve  yielding -ve  = no overflow.
  -ve   +  +ve  yielding +ve  = no overflow.
  -ve   +  +ve  yielding -ve  = no overflow
  -ve   +  -ve  yielding +ve  = overflow.
  -ve   +  -ve  yielding -ve  = no overflow.


  Subtraction:

  check
  +ve   -  +ve  yielding +ve  = no overflow.
  +ve   -  +ve  yielding -ve  = no overflow.
  +ve   -  -ve  yielding +ve  = no overflow.
  +ve   -  -ve  yielding -ve  = overflow.
  -ve   -  +ve  yielding +ve  = overflow.
  -ve   -  +ve  yielding -ve  = no overflow.
  -ve   -  -ve  yielding +ve  = no overflow.
  -ve   -  -ve  yielding -ve  = no overflow.


  CARRY:

  Addition:

  7f + 7f       = no carry
  7f + 80       = no carry
  80 + 7f       = no carry
  80 + 80       = carry

  Subtraction:

  7f - 7f       = no carry
  7f - 80   = carry
  80 - 7f   = no carry
  80 - 80   = no carry

*/
  

/*
Flags Register (F)

Bit 7: Sign Flag
Bit 6: Zero Flag
Bit 5: Unused
Bit 4: Half Carry Flag
Bit 3: Unused
Bit 2: Parity/Overflow Flag
Bit 1: Subtract
Bit 0: Carry
*/


#include "../cpcdefs.h"
#include "../cpcglob.h"

/*
#define CPC_LSB_FIRST

#ifdef USE_INLINE
#define INLINE  __inline
#else
#define INLINE
#endif
*/
#define Z80_ZERO_FLAG_BIT                       6
#define Z80_HALFCARRY_FLAG_BIT          4
#define Z80_PARITY_FLAG_BIT                     2

#define Z80_SIGN_FLAG                           0x080
#define Z80_ZERO_FLAG                           0x040
#define Z80_UNUSED_FLAG1                        0x020
#define Z80_HALFCARRY_FLAG                      0x010
#define Z80_UNUSED_FLAG2                        0x008

#define Z80_PARITY_FLAG                         0x004
#define Z80_OVERFLOW_FLAG                       0x004

#define Z80_SUBTRACT_FLAG                       0x002
#define Z80_CARRY_FLAG                          0x001

#define X86_SIGN_FLAG                           0x080
#define X86_ZERO_FLAG                           0x040
#define X86_UNUSED_FLAG3                        0x020
#define X86_AUXILIARY_FLAG                      0x010
#define X86_UNUSED_FLAG2                        0x008
#define X86_PARITY_FLAG                         0x004
#define X86_UNUSED_FLAG1                        0x002
#define X86_CARRY_FLAG                          0x001


/* size defines */
typedef unsigned char  Z80_BYTE;
typedef unsigned short	Z80_WORD;
typedef signed char        Z80_BYTE_OFFSET;
typedef signed short   Z80_WORD_OFFSET;
typedef unsigned long   Z80_LONG;

#include "../cpc.h"

#ifndef NULL
#define NULL 0
#endif

#ifndef FALSE
#define FALSE (1==0)
#endif

#ifndef TRUE
#define TRUE (1==1)
#endif

/* opcode emulation function definition */
typedef void (*Z80_OPCODE_FUNCTION)(void);

#ifdef CPC_LSB_FIRST 
/* register pair definition */
typedef union
{
                /* read as a word */
				Z80_WORD W;
                /* read as seperate bytes, l for low, h for high bytes */
                struct
                {
                        Z80_BYTE l;
                        Z80_BYTE h;
				} B;
} Z80_REGISTER_PAIR;
#else
/* register pair definition */
typedef union
{
                /* read as a word */
                Z80_WORD W;
        
                /* read as seperate bytes, l for low, h for high bytes */
                struct
                {
                        Z80_BYTE h;
                        Z80_BYTE l;
                } B;
} Z80_REGISTER_PAIR;
#endif

#ifdef CPC_LSB_FIRST
typedef union
{
	Z80_LONG	L;

	struct
	{
		Z80_WORD	l;
		Z80_WORD	h;
	} W;

	struct
	{
		Z80_BYTE	l;
		Z80_BYTE	h;
		Z80_BYTE	pad[2];
	} B;
} Z80_REGISTER_LONG;
#else
typedef union
{
	Z80_LONG	L;

	struct
	{
		Z80_WORD	l;
		Z80_WORD	h;
	} W;

	struct
	{
		Z80_BYTE	pad[2];
		Z80_BYTE	h;
		Z80_BYTE	l;
	} B;
} Z80_REGISTER_LONG;
#endif

/* structure holds all register data */
typedef struct  
{
        Z80_REGISTER_LONG       PC;                     
        Z80_REGISTER_PAIR       AF;                     
        Z80_REGISTER_PAIR       HL;                     
        Z80_REGISTER_PAIR       SP;                     
		Z80_REGISTER_PAIR       DE;                     
        Z80_REGISTER_PAIR       BC;                     
		
        
		Z80_REGISTER_PAIR       IX;                     
        Z80_REGISTER_PAIR       IY;                     
		Z80_WORD				IndexPlusOffset;

        Z80_REGISTER_PAIR       altHL;          
        Z80_REGISTER_PAIR       altDE;
        Z80_REGISTER_PAIR       altBC;
        Z80_REGISTER_PAIR       altAF;

        Z80_WORD                TempWordResult;
        Z80_LONG                TempLongResult;
        /* interrupt vector register. High byte of address */
        Z80_BYTE                I;

        /* refresh register */
        Z80_BYTE                R;

        /* interrupt status */
        Z80_BYTE                IFF1;
        Z80_BYTE                IFF2;

        /* bit 7 of R register */
        Z80_BYTE                RBit7;

        /* interrupt mode 0,1,2 */
        Z80_BYTE                IM;
        Z80_BYTE                TempByte;
        Z80_BYTE                TempWord;
        Z80_REGISTER_PAIR       TempRegister;
		Z80_BYTE				InterruptVectorBase;
		unsigned long			Flags;
} Z80_REGISTERS;

#define Z80_GET_R   (R.RBit7 | (R.R & 0x07f))

/* external defined functions */

/* read a byte of data from memory */
Z80_BYTE        Z80_RD_MEM(Z80_WORD Addr);

/* write a byte to memory */
void            Z80_WR_MEM(Z80_WORD Addr, Z80_BYTE Data);

Z80_WORD        Z80_RD_MEM_WORD(Z80_WORD Addr);

void            Z80_WR_MEM_WORD(Z80_WORD Addr, Z80_WORD Data);

INLINE Z80_BYTE  RD_BYTE_INDEX(Z80_WORD Index);
INLINE void		WR_BYTE_INDEX(Z80_WORD Index,Z80_BYTE Data);


/* write a byte to a I/O port */
void            Z80_DoOut(Z80_WORD Addr, Z80_BYTE Data);

/* read a byte from a I/O port */
Z80_BYTE        Z80_DoIn(Z80_WORD Addr);


/*--------*/





#define Z80_FLAGS_REG                       R.AF.B.l

#define Z80_TEST_CARRY_SET                       ((Z80_FLAGS_REG & Z80_CARRY_FLAG)!=0)
#define Z80_TEST_CARRY_NOT_SET           (Z80_FLAGS_REG & Z80_CARRY_FLAG)==0
#define Z80_TEST_ZERO_SET                        (Z80_FLAGS_REG & Z80_ZERO_FLAG)!=0
#define Z80_TEST_ZERO_NOT_SET            (Z80_FLAGS_REG & Z80_ZERO_FLAG)==0
#define Z80_TEST_MINUS                           (Z80_FLAGS_REG & Z80_SIGN_FLAG)!=0
#define Z80_TEST_POSITIVE                        (Z80_FLAGS_REG & Z80_SIGN_FLAG)==0

/* parity even. bit = 1, parity odd, bit = 0 */
#define Z80_TEST_PARITY_EVEN                 ((Z80_FLAGS_REG & Z80_PARITY_FLAG)!=0)
#define Z80_TEST_PARITY_ODD                  ((Z80_FLAGS_REG & Z80_PARITY_FLAG)==0)

#define Z80_KEEP_UNUSED_FLAGS       (Z80_UNUSED1_FLAG | Z80_UNUSED2_FLAG)


void    Z80_Init(void);
void    Z80_Reset(void);
int    Z80_ExecuteInterrupt(void);


void    Z80_NMI(void);


void    Z80_Reti(void);


/* set the interrupt vector - used by IM 0 and IM 2 */
void    Z80_SetInterruptVector(int);
/* return TRUE if a interrupt has been requested and not yet acknowledged */
BOOL	Z80_GetInterruptRequest(void);
/* clear a requested interrupt */
void    Z80_ClearInterruptRequest(void);
/* request a interrupt */
void    Z80_SetInterruptRequest(void);
/* Z80 executes this to acknowledge a requested interrupt */
void	Z80_AcknowledgeInterrupt(void);


void    Z80_PatchFunction();


/*static void	Z80_UpdateCycles(int); */
#define Z80_CHECK_INTERRUPT_FLAG				0x0001
#define Z80_EXECUTE_INTERRUPT_HANDLER_FLAG	0x0002
#define Z80_EXECUTING_HALT_FLAG				0x0004
#define Z80_INTERRUPT_FLAG					0x0008

/*static void Z80_FlushCycles(void); */

/* for debugging */

enum
{
	Z80_PC,
	Z80_SP,
	Z80_IX,
	Z80_IY,
	Z80_I,
	Z80_R,
	Z80_AF,
	Z80_BC,
	Z80_DE,
	Z80_HL,
	Z80_IFF1,
	Z80_IFF2,
	Z80_IM,
	Z80_AF2,
	Z80_BC2,
	Z80_DE2,
	Z80_HL2,
	Z80_F
} /*Z80_REG_ID */;

int Z80_GetReg(int RegID);
void Z80_SetReg(int RegID, int Value);

int	Z80_ExecuteInstruction(void);
int Z80_ExecuteIM0(void);
#endif
