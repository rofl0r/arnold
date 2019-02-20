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
#include "z80tables.h"
#include "z80macros.h"
Z80_REGISTERS R;
#include "z80funcs.h"
#include "z80funcs2.h"
/***************************************************************************/
INLINE static int Z80_FD_CB_ExecuteInstruction(void)
{
unsigned long Opcode;
unsigned long Cycles=0;
Opcode = Z80_RD_OPCODE_BYTE(3);
Opcode = Opcode & 0x0ff;
switch (Opcode)
{
case 0x000:
{
INDEX_CB_RLC_REG(R.IY.W,R.BC.B.h);
Cycles = 7;
}
break;
case 0x001:
{
INDEX_CB_RLC_REG(R.IY.W,R.BC.B.l);
Cycles = 7;
}
break;
case 0x002:
{
INDEX_CB_RLC_REG(R.IY.W,R.DE.B.h);
Cycles = 7;
}
break;
case 0x003:
{
INDEX_CB_RLC_REG(R.IY.W,R.DE.B.l);
Cycles = 7;
}
break;
case 0x004:
{
INDEX_CB_RLC_REG(R.IY.W,R.HL.B.h);
Cycles = 7;
}
break;
case 0x005:
{
INDEX_CB_RLC_REG(R.IY.W,R.HL.B.l);
Cycles = 7;
}
break;
case 0x006:
{
RLC_INDEX(R.IY.W);
Cycles = 7;
}
break;
case 0x007:
{
INDEX_CB_RLC_REG(R.IY.W,R.AF.B.h);
Cycles = 7;
}
break;
case 0x008:
{
INDEX_CB_RRC_REG(R.IY.W,R.BC.B.h);
Cycles = 7;
}
break;
case 0x009:
{
INDEX_CB_RRC_REG(R.IY.W,R.BC.B.l);
Cycles = 7;
}
break;
case 0x00a:
{
INDEX_CB_RRC_REG(R.IY.W,R.DE.B.h);
Cycles = 7;
}
break;
case 0x00b:
{
INDEX_CB_RRC_REG(R.IY.W,R.DE.B.l);
Cycles = 7;
}
break;
case 0x00c:
{
INDEX_CB_RRC_REG(R.IY.W,R.HL.B.h);
Cycles = 7;
}
break;
case 0x00d:
{
INDEX_CB_RRC_REG(R.IY.W,R.HL.B.l);
Cycles = 7;
}
break;
case 0x00e:
{
RRC_INDEX(R.IY.W);
Cycles = 7;
}
break;
case 0x00f:
{
INDEX_CB_RRC_REG(R.IY.W,R.AF.B.h);
Cycles = 7;
}
break;
case 0x010:
{
INDEX_CB_RL_REG(R.IY.W,R.BC.B.h);
Cycles = 7;
}
break;
case 0x011:
{
INDEX_CB_RL_REG(R.IY.W,R.BC.B.l);
Cycles = 7;
}
break;
case 0x012:
{
INDEX_CB_RL_REG(R.IY.W,R.DE.B.h);
Cycles = 7;
}
break;
case 0x013:
{
INDEX_CB_RL_REG(R.IY.W,R.DE.B.l);
Cycles = 7;
}
break;
case 0x014:
{
INDEX_CB_RL_REG(R.IY.W,R.HL.B.h);
Cycles = 7;
}
break;
case 0x015:
{
INDEX_CB_RL_REG(R.IY.W,R.HL.B.l);
Cycles = 7;
}
break;
case 0x016:
{
RL_INDEX(R.IY.W);
Cycles = 7;
}
break;
case 0x017:
{
INDEX_CB_RL_REG(R.IY.W,R.AF.B.h);
Cycles = 7;
}
break;
case 0x018:
{
INDEX_CB_RR_REG(R.IY.W,R.BC.B.h);
Cycles = 7;
}
break;
case 0x019:
{
INDEX_CB_RR_REG(R.IY.W,R.BC.B.l);
Cycles = 7;
}
break;
case 0x01a:
{
INDEX_CB_RR_REG(R.IY.W,R.DE.B.h);
Cycles = 7;
}
break;
case 0x01b:
{
INDEX_CB_RR_REG(R.IY.W,R.DE.B.l);
Cycles = 7;
}
break;
case 0x01c:
{
INDEX_CB_RR_REG(R.IY.W,R.HL.B.h);
Cycles = 7;
}
break;
case 0x01d:
{
INDEX_CB_RR_REG(R.IY.W,R.HL.B.l);
Cycles = 7;
}
break;
case 0x01e:
{
RR_INDEX(R.IY.W);
Cycles = 7;
}
break;
case 0x01f:
{
INDEX_CB_RR_REG(R.IY.W,R.AF.B.h);
Cycles = 7;
}
break;
case 0x020:
{
INDEX_CB_SLA_REG(R.IY.W,R.BC.B.h);
Cycles = 7;
}
break;
case 0x021:
{
INDEX_CB_SLA_REG(R.IY.W,R.BC.B.l);
Cycles = 7;
}
break;
case 0x022:
{
INDEX_CB_SLA_REG(R.IY.W,R.DE.B.h);
Cycles = 7;
}
break;
case 0x023:
{
INDEX_CB_SLA_REG(R.IY.W,R.DE.B.l);
Cycles = 7;
}
break;
case 0x024:
{
INDEX_CB_SLA_REG(R.IY.W,R.HL.B.h);
Cycles = 7;
}
break;
case 0x025:
{
INDEX_CB_SLA_REG(R.IY.W,R.HL.B.l);
Cycles = 7;
}
break;
case 0x026:
{
SLA_INDEX(R.IY.W);
Cycles = 7;
}
break;
case 0x027:
{
INDEX_CB_SLA_REG(R.IY.W,R.AF.B.h);
Cycles = 7;
}
break;
case 0x028:
{
INDEX_CB_SRA_REG(R.IY.W,R.BC.B.h);
Cycles = 7;
}
break;
case 0x029:
{
INDEX_CB_SRA_REG(R.IY.W,R.BC.B.l);
Cycles = 7;
}
break;
case 0x02a:
{
INDEX_CB_SRA_REG(R.IY.W,R.DE.B.h);
Cycles = 7;
}
break;
case 0x02b:
{
INDEX_CB_SRA_REG(R.IY.W,R.DE.B.l);
Cycles = 7;
}
break;
case 0x02c:
{
INDEX_CB_SRA_REG(R.IY.W,R.HL.B.h);
Cycles = 7;
}
break;
case 0x02d:
{
INDEX_CB_SRA_REG(R.IY.W,R.HL.B.l);
Cycles = 7;
}
break;
case 0x02e:
{
SRA_INDEX(R.IY.W);
Cycles = 7;
}
break;
case 0x02f:
{
INDEX_CB_SRA_REG(R.IY.W,R.AF.B.h);
Cycles = 7;
}
break;
case 0x030:
{
INDEX_CB_SLL_REG(R.IY.W,R.BC.B.h);
Cycles = 7;
}
break;
case 0x031:
{
INDEX_CB_SLL_REG(R.IY.W,R.BC.B.l);
Cycles = 7;
}
break;
case 0x032:
{
INDEX_CB_SLL_REG(R.IY.W,R.DE.B.h);
Cycles = 7;
}
break;
case 0x033:
{
INDEX_CB_SLL_REG(R.IY.W,R.DE.B.l);
Cycles = 7;
}
break;
case 0x034:
{
INDEX_CB_SLL_REG(R.IY.W,R.HL.B.h);
Cycles = 7;
}
break;
case 0x035:
{
INDEX_CB_SLL_REG(R.IY.W,R.HL.B.l);
Cycles = 7;
}
break;
case 0x036:
{
SLL_INDEX(R.IY.W);
Cycles = 7;
}
break;
case 0x037:
{
INDEX_CB_SLL_REG(R.IY.W,R.AF.B.h);
Cycles = 7;
}
break;
case 0x038:
{
INDEX_CB_SRL_REG(R.IY.W,R.BC.B.h);
Cycles = 7;
}
break;
case 0x039:
{
INDEX_CB_SRL_REG(R.IY.W,R.BC.B.l);
Cycles = 7;
}
break;
case 0x03a:
{
INDEX_CB_SRL_REG(R.IY.W,R.DE.B.h);
Cycles = 7;
}
break;
case 0x03b:
{
INDEX_CB_SRL_REG(R.IY.W,R.DE.B.l);
Cycles = 7;
}
break;
case 0x03c:
{
INDEX_CB_SRL_REG(R.IY.W,R.HL.B.h);
Cycles = 7;
}
break;
case 0x03d:
{
INDEX_CB_SRL_REG(R.IY.W,R.HL.B.l);
Cycles = 7;
}
break;
case 0x03e:
{
SRL_INDEX(R.IY.W);
Cycles = 7;
}
break;
case 0x03f:
{
INDEX_CB_SRL_REG(R.IY.W,R.AF.B.h);
Cycles = 7;
}
break;
case 0x040:
{
/* BIT 0,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<0);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>0)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>0)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x041:
{
/* BIT 0,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<0);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>0)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>0)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x042:
{
/* BIT 0,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<0);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>0)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>0)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x043:
{
/* BIT 0,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<0);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>0)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>0)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x044:
{
/* BIT 0,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<0);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>0)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>0)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x045:
{
/* BIT 0,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<0);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>0)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>0)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x046:
{
/* BIT 0,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<0);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>0)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>0)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x047:
{
/* BIT 0,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<0);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>0)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>0)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x048:
{
/* BIT 1,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<1);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>1)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>1)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x049:
{
/* BIT 1,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<1);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>1)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>1)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x04a:
{
/* BIT 1,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<1);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>1)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>1)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x04b:
{
/* BIT 1,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<1);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>1)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>1)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x04c:
{
/* BIT 1,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<1);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>1)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>1)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x04d:
{
/* BIT 1,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<1);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>1)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>1)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x04e:
{
/* BIT 1,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<1);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>1)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>1)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x04f:
{
/* BIT 1,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<1);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>1)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>1)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x050:
{
/* BIT 2,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<2);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>2)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>2)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x051:
{
/* BIT 2,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<2);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>2)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>2)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x052:
{
/* BIT 2,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<2);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>2)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>2)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x053:
{
/* BIT 2,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<2);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>2)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>2)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x054:
{
/* BIT 2,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<2);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>2)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>2)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x055:
{
/* BIT 2,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<2);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>2)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>2)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x056:
{
/* BIT 2,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<2);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>2)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>2)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x057:
{
/* BIT 2,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<2);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>2)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>2)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x058:
{
/* BIT 3,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<3);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>3)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>3)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x059:
{
/* BIT 3,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<3);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>3)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>3)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x05a:
{
/* BIT 3,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<3);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>3)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>3)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x05b:
{
/* BIT 3,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<3);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>3)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>3)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x05c:
{
/* BIT 3,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<3);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>3)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>3)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x05d:
{
/* BIT 3,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<3);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>3)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>3)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x05e:
{
/* BIT 3,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<3);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>3)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>3)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x05f:
{
/* BIT 3,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<3);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>3)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>3)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x060:
{
/* BIT 4,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<4);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>4)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>4)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x061:
{
/* BIT 4,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<4);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>4)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>4)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x062:
{
/* BIT 4,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<4);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>4)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>4)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x063:
{
/* BIT 4,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<4);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>4)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>4)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x064:
{
/* BIT 4,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<4);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>4)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>4)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x065:
{
/* BIT 4,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<4);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>4)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>4)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x066:
{
/* BIT 4,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<4);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>4)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>4)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x067:
{
/* BIT 4,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<4);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>4)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>4)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x068:
{
/* BIT 5,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<5);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>5)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>5)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x069:
{
/* BIT 5,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<5);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>5)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>5)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x06a:
{
/* BIT 5,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<5);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>5)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>5)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x06b:
{
/* BIT 5,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<5);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>5)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>5)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x06c:
{
/* BIT 5,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<5);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>5)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>5)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x06d:
{
/* BIT 5,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<5);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>5)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>5)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x06e:
{
/* BIT 5,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<5);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>5)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>5)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x06f:
{
/* BIT 5,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<5);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>5)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>5)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x070:
{
/* BIT 6,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<6);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>6)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>6)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x071:
{
/* BIT 6,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<6);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>6)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>6)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x072:
{
/* BIT 6,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<6);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>6)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>6)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x073:
{
/* BIT 6,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<6);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>6)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>6)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x074:
{
/* BIT 6,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<6);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>6)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>6)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x075:
{
/* BIT 6,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<6);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>6)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>6)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x076:
{
/* BIT 6,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<6);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>6)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>6)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x077:
{
/* BIT 6,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<6);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>6)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>6)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x078:
{
/* BIT 7,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<7);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>7)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>7)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x079:
{
/* BIT 7,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<7);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>7)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>7)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x07a:
{
/* BIT 7,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<7);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>7)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>7)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x07b:
{
/* BIT 7,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<7);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>7)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>7)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x07c:
{
/* BIT 7,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<7);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>7)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>7)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x07d:
{
/* BIT 7,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<7);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>7)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>7)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x07e:
{
/* BIT 7,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<7);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>7)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>7)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x07f:
{
/* BIT 7,(IY+d) */
SETUP_INDEXED_ADDRESS(R.IY.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<7);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>7)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>7)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x080:
{
INDEX_CB_RES_REG(0x001, R.IY.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x081:
{
INDEX_CB_RES_REG(0x001, R.IY.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x082:
{
INDEX_CB_RES_REG(0x001, R.IY.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x083:
{
INDEX_CB_RES_REG(0x001, R.IY.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x084:
{
INDEX_CB_RES_REG(0x001, R.IY.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x085:
{
INDEX_CB_RES_REG(0x001, R.IY.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x086:
{
RES_INDEX(0x01,R.IY.W);
Cycles = 7;
}
break;
case 0x087:
{
INDEX_CB_RES_REG(0x001, R.IY.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x088:
{
INDEX_CB_RES_REG(0x002, R.IY.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x089:
{
INDEX_CB_RES_REG(0x002, R.IY.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x08a:
{
INDEX_CB_RES_REG(0x002, R.IY.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x08b:
{
INDEX_CB_RES_REG(0x002, R.IY.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x08c:
{
INDEX_CB_RES_REG(0x002, R.IY.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x08d:
{
INDEX_CB_RES_REG(0x002, R.IY.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x08e:
{
RES_INDEX(0x02,R.IY.W);
Cycles = 7;
}
break;
case 0x08f:
{
INDEX_CB_RES_REG(0x002, R.IY.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x090:
{
INDEX_CB_RES_REG(0x004, R.IY.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x091:
{
INDEX_CB_RES_REG(0x004, R.IY.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x092:
{
INDEX_CB_RES_REG(0x004, R.IY.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x093:
{
INDEX_CB_RES_REG(0x004, R.IY.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x094:
{
INDEX_CB_RES_REG(0x004, R.IY.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x095:
{
INDEX_CB_RES_REG(0x004, R.IY.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x096:
{
RES_INDEX(0x04,R.IY.W);
Cycles = 7;
}
break;
case 0x097:
{
INDEX_CB_RES_REG(0x004, R.IY.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x098:
{
INDEX_CB_RES_REG(0x008, R.IY.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x099:
{
INDEX_CB_RES_REG(0x008, R.IY.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x09a:
{
INDEX_CB_RES_REG(0x008, R.IY.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x09b:
{
INDEX_CB_RES_REG(0x008, R.IY.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x09c:
{
INDEX_CB_RES_REG(0x008, R.IY.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x09d:
{
INDEX_CB_RES_REG(0x008, R.IY.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x09e:
{
RES_INDEX(0x08,R.IY.W);
Cycles = 7;
}
break;
case 0x09f:
{
INDEX_CB_RES_REG(0x008, R.IY.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0a0:
{
INDEX_CB_RES_REG(0x010, R.IY.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0a1:
{
INDEX_CB_RES_REG(0x010, R.IY.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0a2:
{
INDEX_CB_RES_REG(0x010, R.IY.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0a3:
{
INDEX_CB_RES_REG(0x010, R.IY.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0a4:
{
INDEX_CB_RES_REG(0x010, R.IY.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0a5:
{
INDEX_CB_RES_REG(0x010, R.IY.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0a6:
{
RES_INDEX(0x10,R.IY.W);
Cycles = 7;
}
break;
case 0x0a7:
{
INDEX_CB_RES_REG(0x010, R.IY.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0a8:
{
INDEX_CB_RES_REG(0x020, R.IY.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0a9:
{
INDEX_CB_RES_REG(0x020, R.IY.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0aa:
{
INDEX_CB_RES_REG(0x020, R.IY.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0ab:
{
INDEX_CB_RES_REG(0x020, R.IY.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0ac:
{
INDEX_CB_RES_REG(0x020, R.IY.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0ad:
{
INDEX_CB_RES_REG(0x020, R.IY.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0ae:
{
RES_INDEX(0x20,R.IY.W);
Cycles = 7;
}
break;
case 0x0af:
{
INDEX_CB_RES_REG(0x020, R.IY.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0b0:
{
INDEX_CB_RES_REG(0x040, R.IY.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0b1:
{
INDEX_CB_RES_REG(0x040, R.IY.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0b2:
{
INDEX_CB_RES_REG(0x040, R.IY.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0b3:
{
INDEX_CB_RES_REG(0x040, R.IY.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0b4:
{
INDEX_CB_RES_REG(0x040, R.IY.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0b5:
{
INDEX_CB_RES_REG(0x040, R.IY.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0b6:
{
RES_INDEX(0x40,R.IY.W);
Cycles = 7;
}
break;
case 0x0b7:
{
INDEX_CB_RES_REG(0x040, R.IY.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0b8:
{
INDEX_CB_RES_REG(0x080, R.IY.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0b9:
{
INDEX_CB_RES_REG(0x080, R.IY.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0ba:
{
INDEX_CB_RES_REG(0x080, R.IY.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0bb:
{
INDEX_CB_RES_REG(0x080, R.IY.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0bc:
{
INDEX_CB_RES_REG(0x080, R.IY.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0bd:
{
INDEX_CB_RES_REG(0x080, R.IY.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0be:
{
RES_INDEX(0x80,R.IY.W);
Cycles = 7;
}
break;
case 0x0bf:
{
INDEX_CB_RES_REG(0x080, R.IY.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0c0:
{
INDEX_CB_SET_REG(0x001, R.IY.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0c1:
{
INDEX_CB_SET_REG(0x001, R.IY.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0c2:
{
INDEX_CB_SET_REG(0x001, R.IY.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0c3:
{
INDEX_CB_SET_REG(0x001, R.IY.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0c4:
{
INDEX_CB_SET_REG(0x001, R.IY.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0c5:
{
INDEX_CB_SET_REG(0x001, R.IY.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0c6:
{
SET_INDEX(0x001,R.IY.W);
Cycles = 7;
}
break;
case 0x0c7:
{
INDEX_CB_SET_REG(0x001, R.IY.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0c8:
{
INDEX_CB_SET_REG(0x002, R.IY.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0c9:
{
INDEX_CB_SET_REG(0x002, R.IY.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0ca:
{
INDEX_CB_SET_REG(0x002, R.IY.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0cb:
{
INDEX_CB_SET_REG(0x002, R.IY.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0cc:
{
INDEX_CB_SET_REG(0x002, R.IY.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0cd:
{
INDEX_CB_SET_REG(0x002, R.IY.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0ce:
{
SET_INDEX(0x002,R.IY.W);
Cycles = 7;
}
break;
case 0x0cf:
{
INDEX_CB_SET_REG(0x002, R.IY.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0d0:
{
INDEX_CB_SET_REG(0x004, R.IY.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0d1:
{
INDEX_CB_SET_REG(0x004, R.IY.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0d2:
{
INDEX_CB_SET_REG(0x004, R.IY.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0d3:
{
INDEX_CB_SET_REG(0x004, R.IY.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0d4:
{
INDEX_CB_SET_REG(0x004, R.IY.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0d5:
{
INDEX_CB_SET_REG(0x004, R.IY.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0d6:
{
SET_INDEX(0x004,R.IY.W);
Cycles = 7;
}
break;
case 0x0d7:
{
INDEX_CB_SET_REG(0x004, R.IY.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0d8:
{
INDEX_CB_SET_REG(0x008, R.IY.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0d9:
{
INDEX_CB_SET_REG(0x008, R.IY.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0da:
{
INDEX_CB_SET_REG(0x008, R.IY.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0db:
{
INDEX_CB_SET_REG(0x008, R.IY.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0dc:
{
INDEX_CB_SET_REG(0x008, R.IY.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0dd:
{
INDEX_CB_SET_REG(0x008, R.IY.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0de:
{
SET_INDEX(0x008,R.IY.W);
Cycles = 7;
}
break;
case 0x0df:
{
INDEX_CB_SET_REG(0x008, R.IY.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0e0:
{
INDEX_CB_SET_REG(0x010, R.IY.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0e1:
{
INDEX_CB_SET_REG(0x010, R.IY.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0e2:
{
INDEX_CB_SET_REG(0x010, R.IY.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0e3:
{
INDEX_CB_SET_REG(0x010, R.IY.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0e4:
{
INDEX_CB_SET_REG(0x010, R.IY.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0e5:
{
INDEX_CB_SET_REG(0x010, R.IY.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0e6:
{
SET_INDEX(0x010,R.IY.W);
Cycles = 7;
}
break;
case 0x0e7:
{
INDEX_CB_SET_REG(0x010, R.IY.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0e8:
{
INDEX_CB_SET_REG(0x020, R.IY.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0e9:
{
INDEX_CB_SET_REG(0x020, R.IY.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0ea:
{
INDEX_CB_SET_REG(0x020, R.IY.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0eb:
{
INDEX_CB_SET_REG(0x020, R.IY.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0ec:
{
INDEX_CB_SET_REG(0x020, R.IY.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0ed:
{
INDEX_CB_SET_REG(0x020, R.IY.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0ee:
{
SET_INDEX(0x020,R.IY.W);
Cycles = 7;
}
break;
case 0x0ef:
{
INDEX_CB_SET_REG(0x020, R.IY.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0f0:
{
INDEX_CB_SET_REG(0x040, R.IY.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0f1:
{
INDEX_CB_SET_REG(0x040, R.IY.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0f2:
{
INDEX_CB_SET_REG(0x040, R.IY.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0f3:
{
INDEX_CB_SET_REG(0x040, R.IY.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0f4:
{
INDEX_CB_SET_REG(0x040, R.IY.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0f5:
{
INDEX_CB_SET_REG(0x040, R.IY.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0f6:
{
SET_INDEX(0x040,R.IY.W);
Cycles = 7;
}
break;
case 0x0f7:
{
INDEX_CB_SET_REG(0x040, R.IY.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0f8:
{
INDEX_CB_SET_REG(0x080, R.IY.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0f9:
{
INDEX_CB_SET_REG(0x080, R.IY.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0fa:
{
INDEX_CB_SET_REG(0x080, R.IY.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0fb:
{
INDEX_CB_SET_REG(0x080, R.IY.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0fc:
{
INDEX_CB_SET_REG(0x080, R.IY.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0fd:
{
INDEX_CB_SET_REG(0x080, R.IY.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0fe:
{
SET_INDEX(0x080,R.IY.W);
Cycles = 7;
}
break;
case 0x0ff:
{
INDEX_CB_SET_REG(0x080, R.IY.W, R.AF.B.h);
Cycles = 7;
}
break;
default:
/* the following tells MSDEV 6 to not generate */
/* code which checks if a input value to the  */
/* switch is not valid.*/
#ifdef _MSC_VER
#if (_MSC_VER>=1200)
__assume(0);
#endif
#endif
break;
}
        R.R+=2;
 		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
return Cycles;
}
/***************************************************************************/
INLINE static int Z80_FD_ExecuteInstruction(void)
{
unsigned long Opcode;
unsigned long Cycles=0;
Opcode = Z80_RD_OPCODE_BYTE(1);
Opcode = Opcode & 0x0ff;
switch (Opcode)
{
case 0x000:
case 0x001:
case 0x002:
case 0x003:
case 0x004:
case 0x005:
case 0x006:
case 0x007:
case 0x008:
case 0x00a:
case 0x00b:
case 0x00c:
case 0x00d:
case 0x00e:
case 0x00f:
case 0x010:
case 0x011:
case 0x012:
case 0x013:
case 0x014:
case 0x015:
case 0x016:
case 0x017:
case 0x018:
case 0x01a:
case 0x01b:
case 0x01c:
case 0x01d:
case 0x01e:
case 0x01f:
case 0x020:
case 0x027:
case 0x028:
case 0x02f:
case 0x030:
case 0x031:
case 0x032:
case 0x033:
case 0x037:
case 0x038:
case 0x03a:
case 0x03b:
case 0x03c:
case 0x03d:
case 0x03e:
case 0x03f:
case 0x040:
case 0x041:
case 0x042:
case 0x043:
case 0x047:
case 0x048:
case 0x049:
case 0x04a:
case 0x04b:
case 0x04f:
case 0x050:
case 0x051:
case 0x052:
case 0x053:
case 0x057:
case 0x058:
case 0x059:
case 0x05a:
case 0x05b:
case 0x05f:
case 0x076:
case 0x078:
case 0x079:
case 0x07a:
case 0x07b:
case 0x07f:
case 0x080:
case 0x081:
case 0x082:
case 0x083:
case 0x087:
case 0x088:
case 0x089:
case 0x08a:
case 0x08b:
case 0x08f:
case 0x090:
case 0x091:
case 0x092:
case 0x093:
case 0x097:
case 0x098:
case 0x099:
case 0x09a:
case 0x09b:
case 0x09f:
case 0x0a0:
case 0x0a1:
case 0x0a2:
case 0x0a3:
case 0x0a7:
case 0x0a8:
case 0x0a9:
case 0x0aa:
case 0x0ab:
case 0x0af:
case 0x0b0:
case 0x0b1:
case 0x0b2:
case 0x0b3:
case 0x0b7:
case 0x0b8:
case 0x0b9:
case 0x0ba:
case 0x0bb:
case 0x0bf:
case 0x0c0:
case 0x0c1:
case 0x0c2:
case 0x0c3:
case 0x0c4:
case 0x0c5:
case 0x0c6:
case 0x0c7:
case 0x0c8:
case 0x0c9:
case 0x0ca:
case 0x0cc:
case 0x0cd:
case 0x0ce:
case 0x0cf:
case 0x0d0:
case 0x0d1:
case 0x0d2:
case 0x0d3:
case 0x0d4:
case 0x0d5:
case 0x0d6:
case 0x0d7:
case 0x0d8:
case 0x0d9:
case 0x0da:
case 0x0db:
case 0x0dc:
case 0x0dd:
case 0x0de:
case 0x0df:
case 0x0e0:
case 0x0e2:
case 0x0e4:
case 0x0e6:
case 0x0e7:
case 0x0e8:
case 0x0ea:
case 0x0eb:
case 0x0ec:
case 0x0ed:
case 0x0ee:
case 0x0ef:
case 0x0f0:
case 0x0f1:
case 0x0f2:
case 0x0f3:
case 0x0f4:
case 0x0f5:
case 0x0f6:
case 0x0f7:
case 0x0f8:
case 0x0fa:
case 0x0fb:
case 0x0fc:
case 0x0fd:
case 0x0fe:
case 0x0ff:
{
R.Flags &= ~Z80_CHECK_INTERRUPT_FLAG;
		R.PC.W.l+=1;         R.R+=1;
 Cycles = 1;
}
break;
case 0x009:
{
ADD_RR_rr(R.IY.W,R.BC.W);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x019:
{
ADD_RR_rr(R.IY.W,R.DE.W);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x021:
{
/* LD IY,nnnn */
 
        R.IY.W = Z80_RD_OPCODE_WORD(2); 
        R.R+=2;
 		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x022:
{
/* LD (nnnn),IY */
 
        R.MemPtr.W = Z80_RD_OPCODE_WORD(2);	      
		Z80_WR_WORD(R.MemPtr.W,R.IY.W);    
		++R.MemPtr.W;	
        R.R+=2;
 		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x023:
{
/* INC IY */
 
    ++R.IY.W;                
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x024:
{
INC_R(R.IY.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x025:
{
DEC_R(R.IY.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x026:
{
/* LD HIY, n */
 
        R.IY.B.h = Z80_RD_OPCODE_BYTE(2); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x029:
{
ADD_RR_rr(R.IY.W,R.IY.W);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x02a:
{
/* LD IY,(nnnn) */
 
        R.MemPtr.W = Z80_RD_OPCODE_WORD(2);	
        R.IY.W = Z80_RD_WORD(R.MemPtr.W);              
		++R.MemPtr.W;	
        R.R+=2;
 		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x02b:
{
/* DEC IY */
 
	--R.IY.W;                
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x02c:
{
INC_R(R.IY.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x02d:
{
DEC_R(R.IY.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x02e:
{
/* LD LIY, n */
 
        R.IY.B.l = Z80_RD_OPCODE_BYTE(2); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x034:
{
_INC_INDEX_(R.IY.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x035:
{
_DEC_INDEX_(R.IY.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x036:
{
/* LD (IY+d),n */
         SETUP_INDEXED_ADDRESS(R.IY.W); 
        R.TempByte = Z80_RD_OPCODE_BYTE(3);	
        WR_BYTE_INDEX(R.TempByte);                                
        R.R+=2;
 		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x039:
{
ADD_RR_rr(R.IY.W,R.SP.W);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x044:
{
/* LD B,hIY */
 
		R.BC.B.h = R.IY.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x045:
{
/* LD B,lIY */
 
		R.BC.B.h = R.IY.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x046:
{
/* LD B,(IY+D) */
 		SETUP_INDEXED_ADDRESS(R.IY.W);	
		R.BC.B.h = Z80_RD_BYTE(R.MemPtr.W);   
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x04c:
{
/* LD C,hIY */
 
		R.BC.B.l = R.IY.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x04d:
{
/* LD C,lIY */
 
		R.BC.B.l = R.IY.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x04e:
{
/* LD C,(IY+D) */
 		SETUP_INDEXED_ADDRESS(R.IY.W);	
		R.BC.B.l = Z80_RD_BYTE(R.MemPtr.W);   
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x054:
{
/* LD D,hIY */
 
		R.DE.B.h = R.IY.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x055:
{
/* LD D,lIY */
 
		R.DE.B.h = R.IY.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x056:
{
/* LD D,(IY+D) */
 		SETUP_INDEXED_ADDRESS(R.IY.W);	
		R.DE.B.h = Z80_RD_BYTE(R.MemPtr.W);   
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x05c:
{
/* LD E,hIY */
 
		R.DE.B.l = R.IY.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x05d:
{
/* LD E,lIY */
 
		R.DE.B.l = R.IY.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x05e:
{
/* LD E,(IY+D) */
 		SETUP_INDEXED_ADDRESS(R.IY.W);	
		R.DE.B.l = Z80_RD_BYTE(R.MemPtr.W);   
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x060:
{
/* LD hIY,B */
 
		R.IY.B.h = R.BC.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x061:
{
/* LD hIY,C */
 
		R.IY.B.h = R.BC.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x062:
{
/* LD hIY,D */
 
		R.IY.B.h = R.DE.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x063:
{
/* LD hIY,E */
 
		R.IY.B.h = R.DE.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x064:
{
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x065:
{
/* LD hIY,lIY */
 
		R.IY.B.h = R.IY.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x066:
{
/* LD H,(IY+D) */
 		SETUP_INDEXED_ADDRESS(R.IY.W);	
		R.HL.B.h = Z80_RD_BYTE(R.MemPtr.W);   
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x067:
{
/* LD hIY,A */
 
		R.IY.B.h = R.AF.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x068:
{
/* LD lIY,B */
 
		R.IY.B.l = R.BC.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x069:
{
/* LD lIY,C */
 
		R.IY.B.l = R.BC.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x06a:
{
/* LD lIY,D */
 
		R.IY.B.l = R.DE.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x06b:
{
/* LD lIY,E */
 
		R.IY.B.l = R.DE.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x06c:
{
/* LD lIY,hIY */
 
		R.IY.B.l = R.IY.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x06d:
{
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x06e:
{
/* LD L,(IY+D) */
 		SETUP_INDEXED_ADDRESS(R.IY.W);	
		R.HL.B.l = Z80_RD_BYTE(R.MemPtr.W);   
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x06f:
{
/* LD lIY,A */
 
		R.IY.B.l = R.AF.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x070:
{
/* LD (IY+D),B */
 		SETUP_INDEXED_ADDRESS(R.IY.W);	
		Z80_WR_BYTE(R.MemPtr.W, R.BC.B.h); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x071:
{
/* LD (IY+D),C */
 		SETUP_INDEXED_ADDRESS(R.IY.W);	
		Z80_WR_BYTE(R.MemPtr.W, R.BC.B.l); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x072:
{
/* LD (IY+D),D */
 		SETUP_INDEXED_ADDRESS(R.IY.W);	
		Z80_WR_BYTE(R.MemPtr.W, R.DE.B.h); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x073:
{
/* LD (IY+D),E */
 		SETUP_INDEXED_ADDRESS(R.IY.W);	
		Z80_WR_BYTE(R.MemPtr.W, R.DE.B.l); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x074:
{
/* LD (IY+D),H */
 		SETUP_INDEXED_ADDRESS(R.IY.W);	
		Z80_WR_BYTE(R.MemPtr.W, R.HL.B.h); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x075:
{
/* LD (IY+D),L */
 		SETUP_INDEXED_ADDRESS(R.IY.W);	
		Z80_WR_BYTE(R.MemPtr.W, R.HL.B.l); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x077:
{
/* LD (IY+D),A */
 		SETUP_INDEXED_ADDRESS(R.IY.W);	
		Z80_WR_BYTE(R.MemPtr.W, R.AF.B.h); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x07c:
{
/* LD A,hIY */
 
		R.AF.B.h = R.IY.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x07d:
{
/* LD A,lIY */
 
		R.AF.B.h = R.IY.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x07e:
{
/* LD A,(IY+D) */
 		SETUP_INDEXED_ADDRESS(R.IY.W);	
		R.AF.B.h = Z80_RD_BYTE(R.MemPtr.W);   
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x084:
{
ADD_A_R(R.IY.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x085:
{
ADD_A_R(R.IY.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x086:
{
ADD_A_INDEX(R.IY.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x08c:
{
ADC_A_R(R.IY.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x08d:
{
ADC_A_R(R.IY.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x08e:
{
ADC_A_INDEX(R.IY.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x094:
{
SUB_A_R(R.IY.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x095:
{
SUB_A_R(R.IY.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x096:
{
SUB_A_INDEX(R.IY.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x09c:
{
SBC_A_R(R.IY.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x09d:
{
SBC_A_R(R.IY.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x09e:
{
SBC_A_INDEX(R.IY.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0a4:
{
AND_A_R(R.IY.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0a5:
{
AND_A_R(R.IY.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0a6:
{
AND_A_INDEX(R.IY.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0ac:
{
XOR_A_R(R.IY.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0ad:
{
XOR_A_R(R.IY.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0ae:
{
XOR_A_INDEX(R.IY.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0b4:
{
OR_A_R(R.IY.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0b5:
{
OR_A_R(R.IY.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0b6:
{
OR_A_INDEX(R.IY.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0bc:
{
CP_A_R(R.IY.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0bd:
{
CP_A_R(R.IY.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0be:
{
CP_A_INDEX(R.IY.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0cb:
{
Cycles = Z80_FD_CB_ExecuteInstruction();
}
break;
case 0x0e1:
{
/* POP IY */
R.IY.W = POP();
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x0e3:
{
/* EX (SP),IY */
 
        R.MemPtr.W = Z80_RD_WORD(R.SP.W); 
        Z80_WR_WORD(R.SP.W, R.IY.W);    
        R.IY.W = R.MemPtr.W; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 7;
}
break;
case 0x0e5:
{
/* PUSH IY */
PUSH(R.IY.W);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0e9:
{
/* JP (IY) */

    R.PC.W.l=R.IY.W; 
        R.R+=2;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0f9:
{
/* LD SP,IY */

    R.SP.W=R.IY.W; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
default:
/* the following tells MSDEV 6 to not generate */
/* code which checks if a input value to the  */
/* switch is not valid.*/
#ifdef _MSC_VER
#if (_MSC_VER>=1200)
__assume(0);
#endif
#endif
break;
}
return Cycles;
}
/***************************************************************************/
INLINE static int Z80_DD_CB_ExecuteInstruction(void)
{
unsigned long Opcode;
unsigned long Cycles=0;
Opcode = Z80_RD_OPCODE_BYTE(3);
Opcode = Opcode & 0x0ff;
switch (Opcode)
{
case 0x000:
{
INDEX_CB_RLC_REG(R.IX.W,R.BC.B.h);
Cycles = 7;
}
break;
case 0x001:
{
INDEX_CB_RLC_REG(R.IX.W,R.BC.B.l);
Cycles = 7;
}
break;
case 0x002:
{
INDEX_CB_RLC_REG(R.IX.W,R.DE.B.h);
Cycles = 7;
}
break;
case 0x003:
{
INDEX_CB_RLC_REG(R.IX.W,R.DE.B.l);
Cycles = 7;
}
break;
case 0x004:
{
INDEX_CB_RLC_REG(R.IX.W,R.HL.B.h);
Cycles = 7;
}
break;
case 0x005:
{
INDEX_CB_RLC_REG(R.IX.W,R.HL.B.l);
Cycles = 7;
}
break;
case 0x006:
{
RLC_INDEX(R.IX.W);
Cycles = 7;
}
break;
case 0x007:
{
INDEX_CB_RLC_REG(R.IX.W,R.AF.B.h);
Cycles = 7;
}
break;
case 0x008:
{
INDEX_CB_RRC_REG(R.IX.W,R.BC.B.h);
Cycles = 7;
}
break;
case 0x009:
{
INDEX_CB_RRC_REG(R.IX.W,R.BC.B.l);
Cycles = 7;
}
break;
case 0x00a:
{
INDEX_CB_RRC_REG(R.IX.W,R.DE.B.h);
Cycles = 7;
}
break;
case 0x00b:
{
INDEX_CB_RRC_REG(R.IX.W,R.DE.B.l);
Cycles = 7;
}
break;
case 0x00c:
{
INDEX_CB_RRC_REG(R.IX.W,R.HL.B.h);
Cycles = 7;
}
break;
case 0x00d:
{
INDEX_CB_RRC_REG(R.IX.W,R.HL.B.l);
Cycles = 7;
}
break;
case 0x00e:
{
RRC_INDEX(R.IX.W);
Cycles = 7;
}
break;
case 0x00f:
{
INDEX_CB_RRC_REG(R.IX.W,R.AF.B.h);
Cycles = 7;
}
break;
case 0x010:
{
INDEX_CB_RL_REG(R.IX.W,R.BC.B.h);
Cycles = 7;
}
break;
case 0x011:
{
INDEX_CB_RL_REG(R.IX.W,R.BC.B.l);
Cycles = 7;
}
break;
case 0x012:
{
INDEX_CB_RL_REG(R.IX.W,R.DE.B.h);
Cycles = 7;
}
break;
case 0x013:
{
INDEX_CB_RL_REG(R.IX.W,R.DE.B.l);
Cycles = 7;
}
break;
case 0x014:
{
INDEX_CB_RL_REG(R.IX.W,R.HL.B.h);
Cycles = 7;
}
break;
case 0x015:
{
INDEX_CB_RL_REG(R.IX.W,R.HL.B.l);
Cycles = 7;
}
break;
case 0x016:
{
RL_INDEX(R.IX.W);
Cycles = 7;
}
break;
case 0x017:
{
INDEX_CB_RL_REG(R.IX.W,R.AF.B.h);
Cycles = 7;
}
break;
case 0x018:
{
INDEX_CB_RR_REG(R.IX.W,R.BC.B.h);
Cycles = 7;
}
break;
case 0x019:
{
INDEX_CB_RR_REG(R.IX.W,R.BC.B.l);
Cycles = 7;
}
break;
case 0x01a:
{
INDEX_CB_RR_REG(R.IX.W,R.DE.B.h);
Cycles = 7;
}
break;
case 0x01b:
{
INDEX_CB_RR_REG(R.IX.W,R.DE.B.l);
Cycles = 7;
}
break;
case 0x01c:
{
INDEX_CB_RR_REG(R.IX.W,R.HL.B.h);
Cycles = 7;
}
break;
case 0x01d:
{
INDEX_CB_RR_REG(R.IX.W,R.HL.B.l);
Cycles = 7;
}
break;
case 0x01e:
{
RR_INDEX(R.IX.W);
Cycles = 7;
}
break;
case 0x01f:
{
INDEX_CB_RR_REG(R.IX.W,R.AF.B.h);
Cycles = 7;
}
break;
case 0x020:
{
INDEX_CB_SLA_REG(R.IX.W,R.BC.B.h);
Cycles = 7;
}
break;
case 0x021:
{
INDEX_CB_SLA_REG(R.IX.W,R.BC.B.l);
Cycles = 7;
}
break;
case 0x022:
{
INDEX_CB_SLA_REG(R.IX.W,R.DE.B.h);
Cycles = 7;
}
break;
case 0x023:
{
INDEX_CB_SLA_REG(R.IX.W,R.DE.B.l);
Cycles = 7;
}
break;
case 0x024:
{
INDEX_CB_SLA_REG(R.IX.W,R.HL.B.h);
Cycles = 7;
}
break;
case 0x025:
{
INDEX_CB_SLA_REG(R.IX.W,R.HL.B.l);
Cycles = 7;
}
break;
case 0x026:
{
SLA_INDEX(R.IX.W);
Cycles = 7;
}
break;
case 0x027:
{
INDEX_CB_SLA_REG(R.IX.W,R.AF.B.h);
Cycles = 7;
}
break;
case 0x028:
{
INDEX_CB_SRA_REG(R.IX.W,R.BC.B.h);
Cycles = 7;
}
break;
case 0x029:
{
INDEX_CB_SRA_REG(R.IX.W,R.BC.B.l);
Cycles = 7;
}
break;
case 0x02a:
{
INDEX_CB_SRA_REG(R.IX.W,R.DE.B.h);
Cycles = 7;
}
break;
case 0x02b:
{
INDEX_CB_SRA_REG(R.IX.W,R.DE.B.l);
Cycles = 7;
}
break;
case 0x02c:
{
INDEX_CB_SRA_REG(R.IX.W,R.HL.B.h);
Cycles = 7;
}
break;
case 0x02d:
{
INDEX_CB_SRA_REG(R.IX.W,R.HL.B.l);
Cycles = 7;
}
break;
case 0x02e:
{
SRA_INDEX(R.IX.W);
Cycles = 7;
}
break;
case 0x02f:
{
INDEX_CB_SRA_REG(R.IX.W,R.AF.B.h);
Cycles = 7;
}
break;
case 0x030:
{
INDEX_CB_SLL_REG(R.IX.W,R.BC.B.h);
Cycles = 7;
}
break;
case 0x031:
{
INDEX_CB_SLL_REG(R.IX.W,R.BC.B.l);
Cycles = 7;
}
break;
case 0x032:
{
INDEX_CB_SLL_REG(R.IX.W,R.DE.B.h);
Cycles = 7;
}
break;
case 0x033:
{
INDEX_CB_SLL_REG(R.IX.W,R.DE.B.l);
Cycles = 7;
}
break;
case 0x034:
{
INDEX_CB_SLL_REG(R.IX.W,R.HL.B.h);
Cycles = 7;
}
break;
case 0x035:
{
INDEX_CB_SLL_REG(R.IX.W,R.HL.B.l);
Cycles = 7;
}
break;
case 0x036:
{
SLL_INDEX(R.IX.W);
Cycles = 7;
}
break;
case 0x037:
{
INDEX_CB_SLL_REG(R.IX.W,R.AF.B.h);
Cycles = 7;
}
break;
case 0x038:
{
INDEX_CB_SRL_REG(R.IX.W,R.BC.B.h);
Cycles = 7;
}
break;
case 0x039:
{
INDEX_CB_SRL_REG(R.IX.W,R.BC.B.l);
Cycles = 7;
}
break;
case 0x03a:
{
INDEX_CB_SRL_REG(R.IX.W,R.DE.B.h);
Cycles = 7;
}
break;
case 0x03b:
{
INDEX_CB_SRL_REG(R.IX.W,R.DE.B.l);
Cycles = 7;
}
break;
case 0x03c:
{
INDEX_CB_SRL_REG(R.IX.W,R.HL.B.h);
Cycles = 7;
}
break;
case 0x03d:
{
INDEX_CB_SRL_REG(R.IX.W,R.HL.B.l);
Cycles = 7;
}
break;
case 0x03e:
{
SRL_INDEX(R.IX.W);
Cycles = 7;
}
break;
case 0x03f:
{
INDEX_CB_SRL_REG(R.IX.W,R.AF.B.h);
Cycles = 7;
}
break;
case 0x040:
{
/* BIT 0,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<0);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>0)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>0)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x041:
{
/* BIT 0,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<0);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>0)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>0)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x042:
{
/* BIT 0,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<0);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>0)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>0)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x043:
{
/* BIT 0,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<0);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>0)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>0)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x044:
{
/* BIT 0,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<0);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>0)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>0)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x045:
{
/* BIT 0,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<0);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>0)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>0)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x046:
{
/* BIT 0,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<0);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>0)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>0)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x047:
{
/* BIT 0,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<0);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>0)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>0)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x048:
{
/* BIT 1,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<1);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>1)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>1)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x049:
{
/* BIT 1,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<1);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>1)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>1)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x04a:
{
/* BIT 1,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<1);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>1)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>1)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x04b:
{
/* BIT 1,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<1);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>1)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>1)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x04c:
{
/* BIT 1,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<1);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>1)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>1)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x04d:
{
/* BIT 1,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<1);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>1)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>1)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x04e:
{
/* BIT 1,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<1);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>1)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>1)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x04f:
{
/* BIT 1,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<1);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>1)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>1)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x050:
{
/* BIT 2,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<2);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>2)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>2)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x051:
{
/* BIT 2,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<2);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>2)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>2)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x052:
{
/* BIT 2,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<2);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>2)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>2)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x053:
{
/* BIT 2,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<2);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>2)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>2)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x054:
{
/* BIT 2,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<2);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>2)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>2)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x055:
{
/* BIT 2,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<2);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>2)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>2)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x056:
{
/* BIT 2,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<2);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>2)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>2)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x057:
{
/* BIT 2,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<2);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>2)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>2)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x058:
{
/* BIT 3,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<3);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>3)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>3)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x059:
{
/* BIT 3,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<3);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>3)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>3)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x05a:
{
/* BIT 3,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<3);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>3)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>3)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x05b:
{
/* BIT 3,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<3);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>3)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>3)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x05c:
{
/* BIT 3,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<3);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>3)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>3)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x05d:
{
/* BIT 3,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<3);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>3)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>3)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x05e:
{
/* BIT 3,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<3);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>3)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>3)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x05f:
{
/* BIT 3,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<3);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>3)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>3)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x060:
{
/* BIT 4,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<4);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>4)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>4)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x061:
{
/* BIT 4,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<4);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>4)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>4)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x062:
{
/* BIT 4,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<4);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>4)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>4)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x063:
{
/* BIT 4,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<4);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>4)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>4)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x064:
{
/* BIT 4,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<4);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>4)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>4)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x065:
{
/* BIT 4,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<4);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>4)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>4)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x066:
{
/* BIT 4,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<4);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>4)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>4)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x067:
{
/* BIT 4,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<4);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>4)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>4)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x068:
{
/* BIT 5,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<5);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>5)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>5)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x069:
{
/* BIT 5,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<5);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>5)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>5)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x06a:
{
/* BIT 5,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<5);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>5)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>5)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x06b:
{
/* BIT 5,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<5);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>5)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>5)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x06c:
{
/* BIT 5,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<5);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>5)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>5)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x06d:
{
/* BIT 5,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<5);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>5)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>5)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x06e:
{
/* BIT 5,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<5);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>5)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>5)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x06f:
{
/* BIT 5,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<5);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>5)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>5)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x070:
{
/* BIT 6,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<6);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>6)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>6)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x071:
{
/* BIT 6,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<6);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>6)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>6)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x072:
{
/* BIT 6,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<6);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>6)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>6)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x073:
{
/* BIT 6,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<6);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>6)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>6)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x074:
{
/* BIT 6,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<6);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>6)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>6)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x075:
{
/* BIT 6,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<6);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>6)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>6)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x076:
{
/* BIT 6,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<6);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>6)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>6)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x077:
{
/* BIT 6,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<6);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>6)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>6)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x078:
{
/* BIT 7,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<7);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>7)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>7)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x079:
{
/* BIT 7,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<7);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>7)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>7)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x07a:
{
/* BIT 7,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<7);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>7)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>7)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x07b:
{
/* BIT 7,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<7);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>7)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>7)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x07c:
{
/* BIT 7,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<7);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>7)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>7)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x07d:
{
/* BIT 7,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<7);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>7)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>7)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x07e:
{
/* BIT 7,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<7);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>7)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>7)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x07f:
{
/* BIT 7,(IX+d) */
SETUP_INDEXED_ADDRESS(R.IX.W);
R.TempByte = Z80_RD_MEM(R.MemPtr.W);

{
	Z80_BYTE	Flags;						
	const Z80_BYTE	Mask = (1<<7);				
	Flags = Z80_FLAGS_REG & Z80_CARRY_FLAG;	/* CF not changed, NF set to zero */ 
	Flags |= Z80_HALFCARRY_FLAG;			/* HF set */ 
	R.TempByte = (~R.TempByte) & Mask;		/* perform AND operation, but swap bit result for ZF */ 
	Flags |= R.MemPtr.B.h & ((1<<5) | (1<<3)); 
	Flags |= ((R.TempByte>>7)<<Z80_ZERO_FLAG_BIT); /* ZF */ 
	Flags |= ((R.TempByte>>7)<<Z80_PARITY_FLAG_BIT); /* PF is a copy of ZF */ 
	Z80_FLAGS_REG = Flags; 
}
Cycles = 6;
}
break;
case 0x080:
{
INDEX_CB_RES_REG(0x001, R.IX.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x081:
{
INDEX_CB_RES_REG(0x001, R.IX.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x082:
{
INDEX_CB_RES_REG(0x001, R.IX.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x083:
{
INDEX_CB_RES_REG(0x001, R.IX.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x084:
{
INDEX_CB_RES_REG(0x001, R.IX.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x085:
{
INDEX_CB_RES_REG(0x001, R.IX.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x086:
{
RES_INDEX(0x01,R.IX.W);
Cycles = 7;
}
break;
case 0x087:
{
INDEX_CB_RES_REG(0x001, R.IX.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x088:
{
INDEX_CB_RES_REG(0x002, R.IX.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x089:
{
INDEX_CB_RES_REG(0x002, R.IX.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x08a:
{
INDEX_CB_RES_REG(0x002, R.IX.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x08b:
{
INDEX_CB_RES_REG(0x002, R.IX.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x08c:
{
INDEX_CB_RES_REG(0x002, R.IX.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x08d:
{
INDEX_CB_RES_REG(0x002, R.IX.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x08e:
{
RES_INDEX(0x02,R.IX.W);
Cycles = 7;
}
break;
case 0x08f:
{
INDEX_CB_RES_REG(0x002, R.IX.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x090:
{
INDEX_CB_RES_REG(0x004, R.IX.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x091:
{
INDEX_CB_RES_REG(0x004, R.IX.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x092:
{
INDEX_CB_RES_REG(0x004, R.IX.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x093:
{
INDEX_CB_RES_REG(0x004, R.IX.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x094:
{
INDEX_CB_RES_REG(0x004, R.IX.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x095:
{
INDEX_CB_RES_REG(0x004, R.IX.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x096:
{
RES_INDEX(0x04,R.IX.W);
Cycles = 7;
}
break;
case 0x097:
{
INDEX_CB_RES_REG(0x004, R.IX.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x098:
{
INDEX_CB_RES_REG(0x008, R.IX.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x099:
{
INDEX_CB_RES_REG(0x008, R.IX.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x09a:
{
INDEX_CB_RES_REG(0x008, R.IX.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x09b:
{
INDEX_CB_RES_REG(0x008, R.IX.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x09c:
{
INDEX_CB_RES_REG(0x008, R.IX.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x09d:
{
INDEX_CB_RES_REG(0x008, R.IX.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x09e:
{
RES_INDEX(0x08,R.IX.W);
Cycles = 7;
}
break;
case 0x09f:
{
INDEX_CB_RES_REG(0x008, R.IX.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0a0:
{
INDEX_CB_RES_REG(0x010, R.IX.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0a1:
{
INDEX_CB_RES_REG(0x010, R.IX.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0a2:
{
INDEX_CB_RES_REG(0x010, R.IX.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0a3:
{
INDEX_CB_RES_REG(0x010, R.IX.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0a4:
{
INDEX_CB_RES_REG(0x010, R.IX.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0a5:
{
INDEX_CB_RES_REG(0x010, R.IX.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0a6:
{
RES_INDEX(0x10,R.IX.W);
Cycles = 7;
}
break;
case 0x0a7:
{
INDEX_CB_RES_REG(0x010, R.IX.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0a8:
{
INDEX_CB_RES_REG(0x020, R.IX.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0a9:
{
INDEX_CB_RES_REG(0x020, R.IX.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0aa:
{
INDEX_CB_RES_REG(0x020, R.IX.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0ab:
{
INDEX_CB_RES_REG(0x020, R.IX.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0ac:
{
INDEX_CB_RES_REG(0x020, R.IX.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0ad:
{
INDEX_CB_RES_REG(0x020, R.IX.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0ae:
{
RES_INDEX(0x20,R.IX.W);
Cycles = 7;
}
break;
case 0x0af:
{
INDEX_CB_RES_REG(0x020, R.IX.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0b0:
{
INDEX_CB_RES_REG(0x040, R.IX.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0b1:
{
INDEX_CB_RES_REG(0x040, R.IX.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0b2:
{
INDEX_CB_RES_REG(0x040, R.IX.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0b3:
{
INDEX_CB_RES_REG(0x040, R.IX.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0b4:
{
INDEX_CB_RES_REG(0x040, R.IX.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0b5:
{
INDEX_CB_RES_REG(0x040, R.IX.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0b6:
{
RES_INDEX(0x40,R.IX.W);
Cycles = 7;
}
break;
case 0x0b7:
{
INDEX_CB_RES_REG(0x040, R.IX.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0b8:
{
INDEX_CB_RES_REG(0x080, R.IX.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0b9:
{
INDEX_CB_RES_REG(0x080, R.IX.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0ba:
{
INDEX_CB_RES_REG(0x080, R.IX.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0bb:
{
INDEX_CB_RES_REG(0x080, R.IX.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0bc:
{
INDEX_CB_RES_REG(0x080, R.IX.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0bd:
{
INDEX_CB_RES_REG(0x080, R.IX.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0be:
{
RES_INDEX(0x80,R.IX.W);
Cycles = 7;
}
break;
case 0x0bf:
{
INDEX_CB_RES_REG(0x080, R.IX.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0c0:
{
INDEX_CB_SET_REG(0x001, R.IX.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0c1:
{
INDEX_CB_SET_REG(0x001, R.IX.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0c2:
{
INDEX_CB_SET_REG(0x001, R.IX.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0c3:
{
INDEX_CB_SET_REG(0x001, R.IX.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0c4:
{
INDEX_CB_SET_REG(0x001, R.IX.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0c5:
{
INDEX_CB_SET_REG(0x001, R.IX.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0c6:
{
SET_INDEX(0x001,R.IX.W);
Cycles = 7;
}
break;
case 0x0c7:
{
INDEX_CB_SET_REG(0x001, R.IX.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0c8:
{
INDEX_CB_SET_REG(0x002, R.IX.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0c9:
{
INDEX_CB_SET_REG(0x002, R.IX.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0ca:
{
INDEX_CB_SET_REG(0x002, R.IX.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0cb:
{
INDEX_CB_SET_REG(0x002, R.IX.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0cc:
{
INDEX_CB_SET_REG(0x002, R.IX.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0cd:
{
INDEX_CB_SET_REG(0x002, R.IX.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0ce:
{
SET_INDEX(0x002,R.IX.W);
Cycles = 7;
}
break;
case 0x0cf:
{
INDEX_CB_SET_REG(0x002, R.IX.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0d0:
{
INDEX_CB_SET_REG(0x004, R.IX.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0d1:
{
INDEX_CB_SET_REG(0x004, R.IX.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0d2:
{
INDEX_CB_SET_REG(0x004, R.IX.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0d3:
{
INDEX_CB_SET_REG(0x004, R.IX.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0d4:
{
INDEX_CB_SET_REG(0x004, R.IX.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0d5:
{
INDEX_CB_SET_REG(0x004, R.IX.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0d6:
{
SET_INDEX(0x004,R.IX.W);
Cycles = 7;
}
break;
case 0x0d7:
{
INDEX_CB_SET_REG(0x004, R.IX.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0d8:
{
INDEX_CB_SET_REG(0x008, R.IX.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0d9:
{
INDEX_CB_SET_REG(0x008, R.IX.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0da:
{
INDEX_CB_SET_REG(0x008, R.IX.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0db:
{
INDEX_CB_SET_REG(0x008, R.IX.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0dc:
{
INDEX_CB_SET_REG(0x008, R.IX.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0dd:
{
INDEX_CB_SET_REG(0x008, R.IX.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0de:
{
SET_INDEX(0x008,R.IX.W);
Cycles = 7;
}
break;
case 0x0df:
{
INDEX_CB_SET_REG(0x008, R.IX.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0e0:
{
INDEX_CB_SET_REG(0x010, R.IX.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0e1:
{
INDEX_CB_SET_REG(0x010, R.IX.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0e2:
{
INDEX_CB_SET_REG(0x010, R.IX.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0e3:
{
INDEX_CB_SET_REG(0x010, R.IX.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0e4:
{
INDEX_CB_SET_REG(0x010, R.IX.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0e5:
{
INDEX_CB_SET_REG(0x010, R.IX.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0e6:
{
SET_INDEX(0x010,R.IX.W);
Cycles = 7;
}
break;
case 0x0e7:
{
INDEX_CB_SET_REG(0x010, R.IX.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0e8:
{
INDEX_CB_SET_REG(0x020, R.IX.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0e9:
{
INDEX_CB_SET_REG(0x020, R.IX.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0ea:
{
INDEX_CB_SET_REG(0x020, R.IX.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0eb:
{
INDEX_CB_SET_REG(0x020, R.IX.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0ec:
{
INDEX_CB_SET_REG(0x020, R.IX.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0ed:
{
INDEX_CB_SET_REG(0x020, R.IX.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0ee:
{
SET_INDEX(0x020,R.IX.W);
Cycles = 7;
}
break;
case 0x0ef:
{
INDEX_CB_SET_REG(0x020, R.IX.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0f0:
{
INDEX_CB_SET_REG(0x040, R.IX.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0f1:
{
INDEX_CB_SET_REG(0x040, R.IX.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0f2:
{
INDEX_CB_SET_REG(0x040, R.IX.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0f3:
{
INDEX_CB_SET_REG(0x040, R.IX.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0f4:
{
INDEX_CB_SET_REG(0x040, R.IX.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0f5:
{
INDEX_CB_SET_REG(0x040, R.IX.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0f6:
{
SET_INDEX(0x040,R.IX.W);
Cycles = 7;
}
break;
case 0x0f7:
{
INDEX_CB_SET_REG(0x040, R.IX.W, R.AF.B.h);
Cycles = 7;
}
break;
case 0x0f8:
{
INDEX_CB_SET_REG(0x080, R.IX.W, R.BC.B.h);
Cycles = 7;
}
break;
case 0x0f9:
{
INDEX_CB_SET_REG(0x080, R.IX.W, R.BC.B.l);
Cycles = 7;
}
break;
case 0x0fa:
{
INDEX_CB_SET_REG(0x080, R.IX.W, R.DE.B.h);
Cycles = 7;
}
break;
case 0x0fb:
{
INDEX_CB_SET_REG(0x080, R.IX.W, R.DE.B.l);
Cycles = 7;
}
break;
case 0x0fc:
{
INDEX_CB_SET_REG(0x080, R.IX.W, R.HL.B.h);
Cycles = 7;
}
break;
case 0x0fd:
{
INDEX_CB_SET_REG(0x080, R.IX.W, R.HL.B.l);
Cycles = 7;
}
break;
case 0x0fe:
{
SET_INDEX(0x080,R.IX.W);
Cycles = 7;
}
break;
case 0x0ff:
{
INDEX_CB_SET_REG(0x080, R.IX.W, R.AF.B.h);
Cycles = 7;
}
break;
default:
/* the following tells MSDEV 6 to not generate */
/* code which checks if a input value to the  */
/* switch is not valid.*/
#ifdef _MSC_VER
#if (_MSC_VER>=1200)
__assume(0);
#endif
#endif
break;
}
        R.R+=2;
 		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
return Cycles;
}
/***************************************************************************/
INLINE static int Z80_DD_ExecuteInstruction(void)
{
unsigned long Opcode;
unsigned long Cycles=0;
Opcode = Z80_RD_OPCODE_BYTE(1);
Opcode = Opcode & 0x0ff;
switch (Opcode)
{
case 0x000:
case 0x001:
case 0x002:
case 0x003:
case 0x004:
case 0x005:
case 0x006:
case 0x007:
case 0x008:
case 0x00a:
case 0x00b:
case 0x00c:
case 0x00d:
case 0x00e:
case 0x00f:
case 0x010:
case 0x011:
case 0x012:
case 0x013:
case 0x014:
case 0x015:
case 0x016:
case 0x017:
case 0x018:
case 0x01a:
case 0x01b:
case 0x01c:
case 0x01d:
case 0x01e:
case 0x01f:
case 0x020:
case 0x027:
case 0x028:
case 0x02f:
case 0x030:
case 0x031:
case 0x032:
case 0x033:
case 0x037:
case 0x038:
case 0x03a:
case 0x03b:
case 0x03c:
case 0x03d:
case 0x03e:
case 0x03f:
case 0x040:
case 0x041:
case 0x042:
case 0x043:
case 0x047:
case 0x048:
case 0x049:
case 0x04a:
case 0x04b:
case 0x04f:
case 0x050:
case 0x051:
case 0x052:
case 0x053:
case 0x057:
case 0x058:
case 0x059:
case 0x05a:
case 0x05b:
case 0x05f:
case 0x076:
case 0x078:
case 0x079:
case 0x07a:
case 0x07b:
case 0x07f:
case 0x080:
case 0x081:
case 0x082:
case 0x083:
case 0x087:
case 0x088:
case 0x089:
case 0x08a:
case 0x08b:
case 0x08f:
case 0x090:
case 0x091:
case 0x092:
case 0x093:
case 0x097:
case 0x098:
case 0x099:
case 0x09a:
case 0x09b:
case 0x09f:
case 0x0a0:
case 0x0a1:
case 0x0a2:
case 0x0a3:
case 0x0a7:
case 0x0a8:
case 0x0a9:
case 0x0aa:
case 0x0ab:
case 0x0af:
case 0x0b0:
case 0x0b1:
case 0x0b2:
case 0x0b3:
case 0x0b7:
case 0x0b8:
case 0x0b9:
case 0x0ba:
case 0x0bb:
case 0x0bf:
case 0x0c0:
case 0x0c1:
case 0x0c2:
case 0x0c3:
case 0x0c4:
case 0x0c5:
case 0x0c6:
case 0x0c7:
case 0x0c8:
case 0x0c9:
case 0x0ca:
case 0x0cc:
case 0x0cd:
case 0x0ce:
case 0x0cf:
case 0x0d0:
case 0x0d1:
case 0x0d2:
case 0x0d3:
case 0x0d4:
case 0x0d5:
case 0x0d6:
case 0x0d7:
case 0x0d8:
case 0x0d9:
case 0x0da:
case 0x0db:
case 0x0dc:
case 0x0dd:
case 0x0de:
case 0x0df:
case 0x0e0:
case 0x0e2:
case 0x0e4:
case 0x0e6:
case 0x0e7:
case 0x0e8:
case 0x0ea:
case 0x0eb:
case 0x0ec:
case 0x0ed:
case 0x0ee:
case 0x0ef:
case 0x0f0:
case 0x0f1:
case 0x0f2:
case 0x0f3:
case 0x0f4:
case 0x0f5:
case 0x0f6:
case 0x0f7:
case 0x0f8:
case 0x0fa:
case 0x0fb:
case 0x0fc:
case 0x0fd:
case 0x0fe:
case 0x0ff:
{
R.Flags &= ~Z80_CHECK_INTERRUPT_FLAG;
		R.PC.W.l+=1;         R.R+=1;
 Cycles = 1;
}
break;
case 0x009:
{
ADD_RR_rr(R.IX.W,R.BC.W);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x019:
{
ADD_RR_rr(R.IX.W,R.DE.W);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x021:
{
/* LD IX,nnnn */
 
        R.IX.W = Z80_RD_OPCODE_WORD(2); 
        R.R+=2;
 		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x022:
{
/* LD (nnnn),IX */
 
        R.MemPtr.W = Z80_RD_OPCODE_WORD(2);	      
		Z80_WR_WORD(R.MemPtr.W,R.IX.W);    
		++R.MemPtr.W;	
        R.R+=2;
 		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x023:
{
/* INC IX */
 
    ++R.IX.W;                
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x024:
{
INC_R(R.IX.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x025:
{
DEC_R(R.IX.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x026:
{
/* LD HIX, n */
 
        R.IX.B.h = Z80_RD_OPCODE_BYTE(2); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x029:
{
ADD_RR_rr(R.IX.W,R.IX.W);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x02a:
{
/* LD IX,(nnnn) */
 
        R.MemPtr.W = Z80_RD_OPCODE_WORD(2);	
        R.IX.W = Z80_RD_WORD(R.MemPtr.W);              
		++R.MemPtr.W;	
        R.R+=2;
 		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x02b:
{
/* DEC IX */
 
	--R.IX.W;                
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x02c:
{
INC_R(R.IX.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x02d:
{
DEC_R(R.IX.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x02e:
{
/* LD LIX, n */
 
        R.IX.B.l = Z80_RD_OPCODE_BYTE(2); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x034:
{
_INC_INDEX_(R.IX.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x035:
{
_DEC_INDEX_(R.IX.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x036:
{
/* LD (IX+d),n */
         SETUP_INDEXED_ADDRESS(R.IX.W); 
        R.TempByte = Z80_RD_OPCODE_BYTE(3);	
        WR_BYTE_INDEX(R.TempByte);                                
        R.R+=2;
 		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x039:
{
ADD_RR_rr(R.IX.W,R.SP.W);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x044:
{
/* LD B,hIX */
 
		R.BC.B.h = R.IX.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x045:
{
/* LD B,lIX */
 
		R.BC.B.h = R.IX.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x046:
{
/* LD B,(IX+D) */
 		SETUP_INDEXED_ADDRESS(R.IX.W);	
		R.BC.B.h = Z80_RD_BYTE(R.MemPtr.W);   
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x04c:
{
/* LD C,hIX */
 
		R.BC.B.l = R.IX.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x04d:
{
/* LD C,lIX */
 
		R.BC.B.l = R.IX.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x04e:
{
/* LD C,(IX+D) */
 		SETUP_INDEXED_ADDRESS(R.IX.W);	
		R.BC.B.l = Z80_RD_BYTE(R.MemPtr.W);   
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x054:
{
/* LD D,hIX */
 
		R.DE.B.h = R.IX.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x055:
{
/* LD D,lIX */
 
		R.DE.B.h = R.IX.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x056:
{
/* LD D,(IX+D) */
 		SETUP_INDEXED_ADDRESS(R.IX.W);	
		R.DE.B.h = Z80_RD_BYTE(R.MemPtr.W);   
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x05c:
{
/* LD E,hIX */
 
		R.DE.B.l = R.IX.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x05d:
{
/* LD E,lIX */
 
		R.DE.B.l = R.IX.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x05e:
{
/* LD E,(IX+D) */
 		SETUP_INDEXED_ADDRESS(R.IX.W);	
		R.DE.B.l = Z80_RD_BYTE(R.MemPtr.W);   
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x060:
{
/* LD hIX,B */
 
		R.IX.B.h = R.BC.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x061:
{
/* LD hIX,C */
 
		R.IX.B.h = R.BC.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x062:
{
/* LD hIX,D */
 
		R.IX.B.h = R.DE.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x063:
{
/* LD hIX,E */
 
		R.IX.B.h = R.DE.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x064:
{
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x065:
{
/* LD hIX,lIX */
 
		R.IX.B.h = R.IX.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x066:
{
/* LD H,(IX+D) */
 		SETUP_INDEXED_ADDRESS(R.IX.W);	
		R.HL.B.h = Z80_RD_BYTE(R.MemPtr.W);   
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x067:
{
/* LD hIX,A */
 
		R.IX.B.h = R.AF.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x068:
{
/* LD lIX,B */
 
		R.IX.B.l = R.BC.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x069:
{
/* LD lIX,C */
 
		R.IX.B.l = R.BC.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x06a:
{
/* LD lIX,D */
 
		R.IX.B.l = R.DE.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x06b:
{
/* LD lIX,E */
 
		R.IX.B.l = R.DE.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x06c:
{
/* LD lIX,hIX */
 
		R.IX.B.l = R.IX.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x06d:
{
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x06e:
{
/* LD L,(IX+D) */
 		SETUP_INDEXED_ADDRESS(R.IX.W);	
		R.HL.B.l = Z80_RD_BYTE(R.MemPtr.W);   
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x06f:
{
/* LD lIX,A */
 
		R.IX.B.l = R.AF.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x070:
{
/* LD (IX+D),B */
 		SETUP_INDEXED_ADDRESS(R.IX.W);	
		Z80_WR_BYTE(R.MemPtr.W, R.BC.B.h); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x071:
{
/* LD (IX+D),C */
 		SETUP_INDEXED_ADDRESS(R.IX.W);	
		Z80_WR_BYTE(R.MemPtr.W, R.BC.B.l); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x072:
{
/* LD (IX+D),D */
 		SETUP_INDEXED_ADDRESS(R.IX.W);	
		Z80_WR_BYTE(R.MemPtr.W, R.DE.B.h); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x073:
{
/* LD (IX+D),E */
 		SETUP_INDEXED_ADDRESS(R.IX.W);	
		Z80_WR_BYTE(R.MemPtr.W, R.DE.B.l); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x074:
{
/* LD (IX+D),H */
 		SETUP_INDEXED_ADDRESS(R.IX.W);	
		Z80_WR_BYTE(R.MemPtr.W, R.HL.B.h); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x075:
{
/* LD (IX+D),L */
 		SETUP_INDEXED_ADDRESS(R.IX.W);	
		Z80_WR_BYTE(R.MemPtr.W, R.HL.B.l); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x077:
{
/* LD (IX+D),A */
 		SETUP_INDEXED_ADDRESS(R.IX.W);	
		Z80_WR_BYTE(R.MemPtr.W, R.AF.B.h); 
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x07c:
{
/* LD A,hIX */
 
		R.AF.B.h = R.IX.B.h; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x07d:
{
/* LD A,lIX */
 
		R.AF.B.h = R.IX.B.l; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x07e:
{
/* LD A,(IX+D) */
 		SETUP_INDEXED_ADDRESS(R.IX.W);	
		R.AF.B.h = Z80_RD_BYTE(R.MemPtr.W);   
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x084:
{
ADD_A_R(R.IX.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x085:
{
ADD_A_R(R.IX.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x086:
{
ADD_A_INDEX(R.IX.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x08c:
{
ADC_A_R(R.IX.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x08d:
{
ADC_A_R(R.IX.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x08e:
{
ADC_A_INDEX(R.IX.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x094:
{
SUB_A_R(R.IX.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x095:
{
SUB_A_R(R.IX.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x096:
{
SUB_A_INDEX(R.IX.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x09c:
{
SBC_A_R(R.IX.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x09d:
{
SBC_A_R(R.IX.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x09e:
{
SBC_A_INDEX(R.IX.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0a4:
{
AND_A_R(R.IX.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0a5:
{
AND_A_R(R.IX.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0a6:
{
AND_A_INDEX(R.IX.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0ac:
{
XOR_A_R(R.IX.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0ad:
{
XOR_A_R(R.IX.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0ae:
{
XOR_A_INDEX(R.IX.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0b4:
{
OR_A_R(R.IX.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0b5:
{
OR_A_R(R.IX.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0b6:
{
OR_A_INDEX(R.IX.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0bc:
{
CP_A_R(R.IX.B.h);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0bd:
{
CP_A_R(R.IX.B.l);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0be:
{
CP_A_INDEX(R.IX.W);
        R.R+=2;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0cb:
{
Cycles = Z80_DD_CB_ExecuteInstruction();
}
break;
case 0x0e1:
{
/* POP IX */
R.IX.W = POP();
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x0e3:
{
/* EX (SP),IX */
 
        R.MemPtr.W = Z80_RD_WORD(R.SP.W); 
        Z80_WR_WORD(R.SP.W, R.IX.W);    
        R.IX.W = R.MemPtr.W; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 7;
}
break;
case 0x0e5:
{
/* PUSH IX */
PUSH(R.IX.W);
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0e9:
{
/* JP (IX) */

    R.PC.W.l=R.IX.W; 
        R.R+=2;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0f9:
{
/* LD SP,IX */

    R.SP.W=R.IX.W; 
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
default:
/* the following tells MSDEV 6 to not generate */
/* code which checks if a input value to the  */
/* switch is not valid.*/
#ifdef _MSC_VER
#if (_MSC_VER>=1200)
__assume(0);
#endif
#endif
break;
}
return Cycles;
}
/***************************************************************************/
INLINE static int Z80_ED_ExecuteInstruction(void)
{
unsigned long Opcode;
unsigned long Cycles=0;
        R.R+=2;
 Opcode = Z80_RD_OPCODE_BYTE(1);
Opcode = Opcode & 0x0ff;
switch (Opcode)
{
case 0x000:
case 0x001:
case 0x002:
case 0x003:
case 0x004:
case 0x005:
case 0x006:
case 0x007:
case 0x008:
case 0x009:
case 0x00a:
case 0x00b:
case 0x00c:
case 0x00d:
case 0x00e:
case 0x00f:
case 0x010:
case 0x011:
case 0x012:
case 0x013:
case 0x014:
case 0x015:
case 0x016:
case 0x017:
case 0x018:
case 0x019:
case 0x01a:
case 0x01b:
case 0x01c:
case 0x01d:
case 0x01e:
case 0x01f:
case 0x020:
case 0x021:
case 0x022:
case 0x023:
case 0x024:
case 0x025:
case 0x026:
case 0x027:
case 0x028:
case 0x029:
case 0x02a:
case 0x02b:
case 0x02c:
case 0x02d:
case 0x02e:
case 0x02f:
case 0x030:
case 0x031:
case 0x032:
case 0x033:
case 0x034:
case 0x035:
case 0x036:
case 0x037:
case 0x038:
case 0x039:
case 0x03a:
case 0x03b:
case 0x03c:
case 0x03d:
case 0x03e:
case 0x03f:
case 0x080:
case 0x081:
case 0x082:
case 0x083:
case 0x084:
case 0x085:
case 0x086:
case 0x087:
case 0x088:
case 0x089:
case 0x08a:
case 0x08b:
case 0x08c:
case 0x08d:
case 0x08e:
case 0x08f:
case 0x090:
case 0x091:
case 0x092:
case 0x093:
case 0x094:
case 0x095:
case 0x096:
case 0x097:
case 0x098:
case 0x099:
case 0x09a:
case 0x09b:
case 0x09c:
case 0x09d:
case 0x09e:
case 0x09f:
case 0x0a4:
case 0x0a5:
case 0x0a6:
case 0x0a7:
case 0x0ac:
case 0x0ad:
case 0x0ae:
case 0x0af:
case 0x0b4:
case 0x0b5:
case 0x0b6:
case 0x0b7:
case 0x0bc:
case 0x0bd:
case 0x0be:
case 0x0bf:
case 0x0c0:
case 0x0c1:
case 0x0c2:
case 0x0c3:
case 0x0c4:
case 0x0c5:
case 0x0c6:
case 0x0c7:
case 0x0c8:
case 0x0c9:
case 0x0ca:
case 0x0cb:
case 0x0cc:
case 0x0cd:
case 0x0ce:
case 0x0cf:
case 0x0d0:
case 0x0d1:
case 0x0d2:
case 0x0d3:
case 0x0d4:
case 0x0d5:
case 0x0d6:
case 0x0d7:
case 0x0d8:
case 0x0d9:
case 0x0da:
case 0x0db:
case 0x0dc:
case 0x0dd:
case 0x0de:
case 0x0df:
case 0x0e0:
case 0x0e1:
case 0x0e2:
case 0x0e3:
case 0x0e4:
case 0x0e5:
case 0x0e6:
case 0x0e7:
case 0x0e8:
case 0x0e9:
case 0x0ea:
case 0x0eb:
case 0x0ec:
case 0x0ed:
case 0x0ee:
case 0x0ef:
case 0x0f0:
case 0x0f1:
case 0x0f2:
case 0x0f3:
case 0x0f4:
case 0x0f5:
case 0x0f6:
case 0x0f7:
case 0x0f8:
case 0x0f9:
case 0x0fa:
case 0x0fb:
case 0x0fc:
case 0x0fd:
case 0x0fe:
case 0x0ff:
{
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x040:
{
/* IN B,(C) */
 		R.MemPtr.W = R.BC.W; 
		R.BC.B.h = Z80_DoIn(R.MemPtr.W);            
		++R.MemPtr.W; 
		{											
			Z80_BYTE	Flags;						
			Flags = Z80_FLAGS_REG;						
			Flags = Flags & Z80_CARRY_FLAG;			
			Flags = ZeroSignParityTable[R.BC.B.h];	
			Z80_FLAGS_REG = Flags;						
		}											
	Cycles = 4; 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x041:
{
 /* OUT (C),B */
 
		R.MemPtr.W = R.BC.W; 
		Z80_DoOut(R.MemPtr.W,R.BC.B.h);                     
        ++R.MemPtr.W;                                       
		Cycles = 4;
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x042:
{
SBC_HL_rr(R.BC.W);
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x043:
{
/* LD (nnnn),BC */
 
        /* read destination address into memptr */ 
        R.MemPtr.W = Z80_RD_OPCODE_WORD(2);	
        /* write register to address */ 
        Z80_WR_WORD(R.MemPtr.W, R.BC.W);    
        ++R.MemPtr.W; 
		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x044:
{
/* NEG */

	Z80_BYTE	Flags;	
	Z80_BYTE	AReg; 
						
	AReg = R.AF.B.h;		
    Flags = Z80_SUBTRACT_FLAG;	
													
    if (AReg == 0x080)									
    {												
          Flags |= Z80_PARITY_FLAG;					
    }												
													
    if (AReg != 0x000)									
    {												
        Flags |= Z80_CARRY_FLAG;					
    }												
													
	if ((AReg & 0x0f)!=0)								
	{												
		Flags |= Z80_HALFCARRY_FLAG;				
	}												
													
    R.AF.B.h = -AReg;							
													
	Flags |= ZeroSignTable[R.AF.B.h];				
	Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	
	Z80_FLAGS_REG = Flags;								
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x045:
{
/* RETN */
   R.IFF1 = R.IFF2; 
   /* update memptr */ 
   R.MemPtr.W = POP(); 
   R.PC.W.l = R.MemPtr.W; 
   /* flags not changed */ 
R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x046:
{
/* IM 0 */
         R.IM = 0; 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x047:
{
/* LD I,A */
 
	R.I = R.AF.B.h;        
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x048:
{
/* IN C,(C) */
 		R.MemPtr.W = R.BC.W; 
		R.BC.B.l = Z80_DoIn(R.MemPtr.W);            
		++R.MemPtr.W; 
		{											
			Z80_BYTE	Flags;						
			Flags = Z80_FLAGS_REG;						
			Flags = Flags & Z80_CARRY_FLAG;			
			Flags = ZeroSignParityTable[R.BC.B.l];	
			Z80_FLAGS_REG = Flags;						
		}											
	Cycles = 4; 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x049:
{
 /* OUT (C),C */
 
		R.MemPtr.W = R.BC.W; 
		Z80_DoOut(R.MemPtr.W,R.BC.B.l);                     
        ++R.MemPtr.W;                                       
		Cycles = 4;
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x04a:
{
ADC_HL_rr(R.BC.W);
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x04b:
{
/* LD BC,(nnnn) */
 
        /* read destination address into memptr */ 
        R.MemPtr.W = Z80_RD_OPCODE_WORD(2);	
        /* read register from address */ 
        R.BC.W = Z80_RD_WORD(R.MemPtr.W);   
		++R.MemPtr.W; 
		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x04c:
{
/* NEG */

	Z80_BYTE	Flags;	
	Z80_BYTE	AReg; 
						
	AReg = R.AF.B.h;		
    Flags = Z80_SUBTRACT_FLAG;	
													
    if (AReg == 0x080)									
    {												
          Flags |= Z80_PARITY_FLAG;					
    }												
													
    if (AReg != 0x000)									
    {												
        Flags |= Z80_CARRY_FLAG;					
    }												
													
	if ((AReg & 0x0f)!=0)								
	{												
		Flags |= Z80_HALFCARRY_FLAG;				
	}												
													
    R.AF.B.h = -AReg;							
													
	Flags |= ZeroSignTable[R.AF.B.h];				
	Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	
	Z80_FLAGS_REG = Flags;								
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x04d:
{
/* RETI */
    R.IFF1 = R.IFF2; 
    Z80_Reti(); 
	 /* update memptr */ 
    R.MemPtr.W = POP(); 
	R.PC.W.l = R.MemPtr.W; 
	/* flags not changed */ 
R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x04e:
{
/* IM 0 */
         R.IM = 0; 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x04f:
{
/* LD R,A */
 
    /* store bit 7 */ 
    R.RBit7 = R.AF.B.h & 0x080; 
 
    /* store refresh register */ 
    R.R = R.AF.B.h & 0x07f; 
	/* no flags changed */ 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x050:
{
/* IN D,(C) */
 		R.MemPtr.W = R.BC.W; 
		R.DE.B.h = Z80_DoIn(R.MemPtr.W);            
		++R.MemPtr.W; 
		{											
			Z80_BYTE	Flags;						
			Flags = Z80_FLAGS_REG;						
			Flags = Flags & Z80_CARRY_FLAG;			
			Flags = ZeroSignParityTable[R.DE.B.h];	
			Z80_FLAGS_REG = Flags;						
		}											
	Cycles = 4; 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x051:
{
 /* OUT (C),D */
 
		R.MemPtr.W = R.BC.W; 
		Z80_DoOut(R.MemPtr.W,R.DE.B.h);                     
        ++R.MemPtr.W;                                       
		Cycles = 4;
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x052:
{
SBC_HL_rr(R.DE.W);
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x053:
{
/* LD (nnnn),DE */
 
        /* read destination address into memptr */ 
        R.MemPtr.W = Z80_RD_OPCODE_WORD(2);	
        /* write register to address */ 
        Z80_WR_WORD(R.MemPtr.W, R.DE.W);    
        ++R.MemPtr.W; 
		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x054:
{
/* NEG */

	Z80_BYTE	Flags;	
	Z80_BYTE	AReg; 
						
	AReg = R.AF.B.h;		
    Flags = Z80_SUBTRACT_FLAG;	
													
    if (AReg == 0x080)									
    {												
          Flags |= Z80_PARITY_FLAG;					
    }												
													
    if (AReg != 0x000)									
    {												
        Flags |= Z80_CARRY_FLAG;					
    }												
													
	if ((AReg & 0x0f)!=0)								
	{												
		Flags |= Z80_HALFCARRY_FLAG;				
	}												
													
    R.AF.B.h = -AReg;							
													
	Flags |= ZeroSignTable[R.AF.B.h];				
	Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	
	Z80_FLAGS_REG = Flags;								
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x055:
{
/* RETN */
   R.IFF1 = R.IFF2; 
   /* update memptr */ 
   R.MemPtr.W = POP(); 
   R.PC.W.l = R.MemPtr.W; 
   /* flags not changed */ 
R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x056:
{
/* IM 1 */
         R.IM = 1; 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x057:
{
/* LD A,I */

        R.AF.B.h = R.I;	
		{				
			Z80_BYTE	Flags;	
 
			/* HF, NF = 0, CF not changed */ 
			Flags = Z80_FLAGS_REG;	
			Flags &= Z80_CARRY_FLAG;	/* keep CF, zeroise everything else */ 
			Flags |= ((R.IFF2 & 0x01)<<Z80_PARITY_FLAG_BIT);	/* IFF2 into PV */ 
			Flags |= ZeroSignTable2[R.AF.B.h];	/* SF, ZF */ 
			Flags |= R.MemPtr.B.h & ((1<<5)|(1<<3)); /* Bits 5,3 from MemPtr */ 
			Z80_FLAGS_REG = Flags;	
		}	
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x058:
{
/* IN E,(C) */
 		R.MemPtr.W = R.BC.W; 
		R.DE.B.l = Z80_DoIn(R.MemPtr.W);            
		++R.MemPtr.W; 
		{											
			Z80_BYTE	Flags;						
			Flags = Z80_FLAGS_REG;						
			Flags = Flags & Z80_CARRY_FLAG;			
			Flags = ZeroSignParityTable[R.DE.B.l];	
			Z80_FLAGS_REG = Flags;						
		}											
	Cycles = 4; 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x059:
{
 /* OUT (C),E */
 
		R.MemPtr.W = R.BC.W; 
		Z80_DoOut(R.MemPtr.W,R.DE.B.l);                     
        ++R.MemPtr.W;                                       
		Cycles = 4;
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x05a:
{
ADC_HL_rr(R.DE.W);
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x05b:
{
/* LD DE,(nnnn) */
 
        /* read destination address into memptr */ 
        R.MemPtr.W = Z80_RD_OPCODE_WORD(2);	
        /* read register from address */ 
        R.DE.W = Z80_RD_WORD(R.MemPtr.W);   
		++R.MemPtr.W; 
		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x05c:
{
/* NEG */

	Z80_BYTE	Flags;	
	Z80_BYTE	AReg; 
						
	AReg = R.AF.B.h;		
    Flags = Z80_SUBTRACT_FLAG;	
													
    if (AReg == 0x080)									
    {												
          Flags |= Z80_PARITY_FLAG;					
    }												
													
    if (AReg != 0x000)									
    {												
        Flags |= Z80_CARRY_FLAG;					
    }												
													
	if ((AReg & 0x0f)!=0)								
	{												
		Flags |= Z80_HALFCARRY_FLAG;				
	}												
													
    R.AF.B.h = -AReg;							
													
	Flags |= ZeroSignTable[R.AF.B.h];				
	Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	
	Z80_FLAGS_REG = Flags;								
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x05d:
{
/* RETI */
    R.IFF1 = R.IFF2; 
    Z80_Reti(); 
	 /* update memptr */ 
    R.MemPtr.W = POP(); 
	R.PC.W.l = R.MemPtr.W; 
	/* flags not changed */ 
R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x05e:
{
/* IM 2 */
         R.IM = 2; 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x05f:
{
/* LD A,R */

        R.AF.B.h = Z80_GET_R;	
							
		{					
			Z80_BYTE	Flags;	
								
			/* HF, NF = 0, CF not changed */ 
			Flags = Z80_FLAGS_REG;	
			Flags &= Z80_CARRY_FLAG;	/* keep CF, zeroise everything else */ 
			Flags |= ((R.IFF2 & 0x01)<<Z80_PARITY_FLAG_BIT);	/* IFF2 into PV */ 
			Flags |= ZeroSignTable2[R.AF.B.h];	/* SF, ZF */ 
			Flags |= R.MemPtr.B.h & ((1<<5)|(1<<3)); /* Bits 5,3 from MemPtr */ 
			Z80_FLAGS_REG = Flags;	
		}					
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x060:
{
/* IN H,(C) */
 		R.MemPtr.W = R.BC.W; 
		R.HL.B.h = Z80_DoIn(R.MemPtr.W);            
		++R.MemPtr.W; 
		{											
			Z80_BYTE	Flags;						
			Flags = Z80_FLAGS_REG;						
			Flags = Flags & Z80_CARRY_FLAG;			
			Flags = ZeroSignParityTable[R.HL.B.h];	
			Z80_FLAGS_REG = Flags;						
		}											
	Cycles = 4; 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x061:
{
 /* OUT (C),H */
 
		R.MemPtr.W = R.BC.W; 
		Z80_DoOut(R.MemPtr.W,R.HL.B.h);                     
        ++R.MemPtr.W;                                       
		Cycles = 4;
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x062:
{
SBC_HL_rr(R.HL.W);
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x063:
{
/* LD (nnnn),HL */
 
        /* read destination address into memptr */ 
        R.MemPtr.W = Z80_RD_OPCODE_WORD(2);	
        /* write register to address */ 
        Z80_WR_WORD(R.MemPtr.W, R.HL.W);    
        ++R.MemPtr.W; 
		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x064:
{
/* NEG */

	Z80_BYTE	Flags;	
	Z80_BYTE	AReg; 
						
	AReg = R.AF.B.h;		
    Flags = Z80_SUBTRACT_FLAG;	
													
    if (AReg == 0x080)									
    {												
          Flags |= Z80_PARITY_FLAG;					
    }												
													
    if (AReg != 0x000)									
    {												
        Flags |= Z80_CARRY_FLAG;					
    }												
													
	if ((AReg & 0x0f)!=0)								
	{												
		Flags |= Z80_HALFCARRY_FLAG;				
	}												
													
    R.AF.B.h = -AReg;							
													
	Flags |= ZeroSignTable[R.AF.B.h];				
	Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	
	Z80_FLAGS_REG = Flags;								
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x065:
{
/* RETN */
   R.IFF1 = R.IFF2; 
   /* update memptr */ 
   R.MemPtr.W = POP(); 
   R.PC.W.l = R.MemPtr.W; 
   /* flags not changed */ 
R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x066:
{
/* IM 0 */
         R.IM = 0; 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x067:
{
/* RRD */
 
        R.TempByte = Z80_RD_BYTE(R.HL.W); 
        Z80_WR_BYTE(R.HL.W, (Z80_BYTE)(((R.TempByte>>4) | (R.AF.B.h<<4)))); 
        R.AF.B.h = (R.AF.B.h & 0x0f0) | (R.TempByte & 0x0f); 
		R.MemPtr.W = R.HL.W+1; 
		{ 
			Z80_BYTE	Flags; 

			Flags = Z80_FLAGS_REG; 
			Flags &= Z80_CARRY_FLAG; 
			Flags |= ZeroSignParityTable[R.AF.B.h]; 
			Z80_FLAGS_REG = Flags; 
		} 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x068:
{
/* IN L,(C) */
 		R.MemPtr.W = R.BC.W; 
		R.HL.B.l = Z80_DoIn(R.MemPtr.W);            
		++R.MemPtr.W; 
		{											
			Z80_BYTE	Flags;						
			Flags = Z80_FLAGS_REG;						
			Flags = Flags & Z80_CARRY_FLAG;			
			Flags = ZeroSignParityTable[R.HL.B.l];	
			Z80_FLAGS_REG = Flags;						
		}											
	Cycles = 4; 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x069:
{
 /* OUT (C),L */
 
		R.MemPtr.W = R.BC.W; 
		Z80_DoOut(R.MemPtr.W,R.HL.B.l);                     
        ++R.MemPtr.W;                                       
		Cycles = 4;
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x06a:
{
ADC_HL_rr(R.HL.W);
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x06b:
{
/* LD HL,(nnnn) */
 
        /* read destination address into memptr */ 
        R.MemPtr.W = Z80_RD_OPCODE_WORD(2);	
        /* read register from address */ 
        R.HL.W = Z80_RD_WORD(R.MemPtr.W);   
		++R.MemPtr.W; 
		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x06c:
{
/* NEG */

	Z80_BYTE	Flags;	
	Z80_BYTE	AReg; 
						
	AReg = R.AF.B.h;		
    Flags = Z80_SUBTRACT_FLAG;	
													
    if (AReg == 0x080)									
    {												
          Flags |= Z80_PARITY_FLAG;					
    }												
													
    if (AReg != 0x000)									
    {												
        Flags |= Z80_CARRY_FLAG;					
    }												
													
	if ((AReg & 0x0f)!=0)								
	{												
		Flags |= Z80_HALFCARRY_FLAG;				
	}												
													
    R.AF.B.h = -AReg;							
													
	Flags |= ZeroSignTable[R.AF.B.h];				
	Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	
	Z80_FLAGS_REG = Flags;								
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x06d:
{
/* RETI */
    R.IFF1 = R.IFF2; 
    Z80_Reti(); 
	 /* update memptr */ 
    R.MemPtr.W = POP(); 
	R.PC.W.l = R.MemPtr.W; 
	/* flags not changed */ 
R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x06e:
{
/* IM 0 */
         R.IM = 0; 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x06f:
{
/* RLD */
 
	R.TempByte = Z80_RD_BYTE(R.HL.W); 
    Z80_WR_BYTE(R.HL.W,(Z80_BYTE)((R.TempByte<<4)|(R.AF.B.h & 0x0f))); 
    R.AF.B.h = (R.AF.B.h & 0x0f0) | (R.TempByte>>4); 
	R.MemPtr.W = R.HL.W+1; 
	{ 
		Z80_BYTE	Flags; 

		Flags = Z80_FLAGS_REG;
		Flags &= Z80_CARRY_FLAG;
		Flags |= ZeroSignParityTable[R.AF.B.h];
		Z80_FLAGS_REG = Flags;
	}
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x070:
{
/* IN X,(C) */
 		R.MemPtr.W = R.BC.W; 
		R.TempByte = Z80_DoIn(R.MemPtr.W);            
		++R.MemPtr.W; 
		{											
			Z80_BYTE	Flags;						
			Flags = Z80_FLAGS_REG;						
			Flags = Flags & Z80_CARRY_FLAG;			
			Flags = ZeroSignParityTable[R.TempByte];	
			Z80_FLAGS_REG = Flags;						
		}											
	Cycles = 4; 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x071:
{
 /* OUT (C),0 */
 
		R.MemPtr.W = R.BC.W; 
		Z80_DoOut(R.MemPtr.W,0);                     
        ++R.MemPtr.W;                                       
		Cycles = 4;
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x072:
{
SBC_HL_rr(R.SP.W);
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x073:
{
/* LD (nnnn),SP */
 
        /* read destination address into memptr */ 
        R.MemPtr.W = Z80_RD_OPCODE_WORD(2);	
        /* write register to address */ 
        Z80_WR_WORD(R.MemPtr.W, R.SP.W);    
        ++R.MemPtr.W; 
		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x074:
{
/* NEG */

	Z80_BYTE	Flags;	
	Z80_BYTE	AReg; 
						
	AReg = R.AF.B.h;		
    Flags = Z80_SUBTRACT_FLAG;	
													
    if (AReg == 0x080)									
    {												
          Flags |= Z80_PARITY_FLAG;					
    }												
													
    if (AReg != 0x000)									
    {												
        Flags |= Z80_CARRY_FLAG;					
    }												
													
	if ((AReg & 0x0f)!=0)								
	{												
		Flags |= Z80_HALFCARRY_FLAG;				
	}												
													
    R.AF.B.h = -AReg;							
													
	Flags |= ZeroSignTable[R.AF.B.h];				
	Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	
	Z80_FLAGS_REG = Flags;								
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x075:
{
/* RETN */
   R.IFF1 = R.IFF2; 
   /* update memptr */ 
   R.MemPtr.W = POP(); 
   R.PC.W.l = R.MemPtr.W; 
   /* flags not changed */ 
R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x076:
{
/* IM 1 */
         R.IM = 1; 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x077:
{
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x078:
{
/* IN A,(C) */
 		R.MemPtr.W = R.BC.W; 
		R.AF.B.h = Z80_DoIn(R.MemPtr.W);            
		++R.MemPtr.W; 
		{											
			Z80_BYTE	Flags;						
			Flags = Z80_FLAGS_REG;						
			Flags = Flags & Z80_CARRY_FLAG;			
			Flags = ZeroSignParityTable[R.AF.B.h];	
			Z80_FLAGS_REG = Flags;						
		}											
	Cycles = 4; 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x079:
{
 /* OUT (C),A */
 
		R.MemPtr.W = R.BC.W; 
		Z80_DoOut(R.MemPtr.W,R.AF.B.h);                     
        ++R.MemPtr.W;                                       
		Cycles = 4;
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x07a:
{
ADC_HL_rr(R.SP.W);
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x07b:
{
/* LD SP,(nnnn) */
 
        /* read destination address into memptr */ 
        R.MemPtr.W = Z80_RD_OPCODE_WORD(2);	
        /* read register from address */ 
        R.SP.W = Z80_RD_WORD(R.MemPtr.W);   
		++R.MemPtr.W; 
		R.PC.W.l+=4; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x07c:
{
/* NEG */

	Z80_BYTE	Flags;	
	Z80_BYTE	AReg; 
						
	AReg = R.AF.B.h;		
    Flags = Z80_SUBTRACT_FLAG;	
													
    if (AReg == 0x080)									
    {												
          Flags |= Z80_PARITY_FLAG;					
    }												
													
    if (AReg != 0x000)									
    {												
        Flags |= Z80_CARRY_FLAG;					
    }												
													
	if ((AReg & 0x0f)!=0)								
	{												
		Flags |= Z80_HALFCARRY_FLAG;				
	}												
													
    R.AF.B.h = -AReg;							
													
	Flags |= ZeroSignTable[R.AF.B.h];				
	Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	
	Z80_FLAGS_REG = Flags;								
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x07d:
{
/* RETI */
    R.IFF1 = R.IFF2; 
    Z80_Reti(); 
	 /* update memptr */ 
    R.MemPtr.W = POP(); 
	R.PC.W.l = R.MemPtr.W; 
	/* flags not changed */ 
R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x07e:
{
/* IM 2 */
         R.IM = 2; 
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x07f:
{
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0a0:
{
LDI();
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0a1:
{
CPI();
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0a2:
{
INI();
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0a3:
{
OUTI();
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0a8:
{
LDD();
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0a9:
{
CPD();
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0aa:
{
IND();
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0ab:
{
OUTD();
		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0b0:
{
LDI();
if (Z80_TEST_PARITY_EVEN)
{
Cycles=6;
}
else
{
		R.PC.W.l+=2; Cycles=5;
}
R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0b1:
{
CPI();
if ((Z80_FLAGS_REG & (Z80_PARITY_FLAG | Z80_ZERO_FLAG))==Z80_PARITY_FLAG)
{
Cycles=5;
}
else
{
		R.PC.W.l+=2; Cycles=6;
}
R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0b2:
{
INI();
if (Z80_TEST_ZERO_SET)
{
		R.PC.W.l+=2; Cycles=6;
}
else
{
Cycles=5;
}
R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0b3:
{
OUTI();
if (Z80_TEST_ZERO_SET)
{
		R.PC.W.l+=2; Cycles=6;
}
else
{
Cycles=5;
}
R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0b8:
{
LDD();
if (Z80_TEST_PARITY_EVEN)
{
Cycles=6;
}
else
{
		R.PC.W.l+=2; Cycles=5;
}
R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0b9:
{
CPD();
if ((Z80_FLAGS_REG & (Z80_PARITY_FLAG | Z80_ZERO_FLAG))==Z80_PARITY_FLAG)
{
Cycles=5;
}
else
{
		R.PC.W.l+=2; Cycles=6;
}
R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0ba:
{
IND();
if (Z80_TEST_ZERO_SET)
{
		R.PC.W.l+=2; Cycles=6;
}
else
{
Cycles=5;
}
R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0bb:
{
OUTD();
if (Z80_TEST_ZERO_SET)
{
		R.PC.W.l+=2; Cycles=6;
}
else
{
Cycles=5;
}
R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
default:
/* the following tells MSDEV 6 to not generate */
/* code which checks if a input value to the  */
/* switch is not valid.*/
#ifdef _MSC_VER
#if (_MSC_VER>=1200)
__assume(0);
#endif
#endif
break;
}
return Cycles;
}
/***************************************************************************/
INLINE static int Z80_CB_ExecuteInstruction(void)
{
unsigned long Opcode;
unsigned long Cycles=0;
Opcode = Z80_RD_OPCODE_BYTE(1);
Opcode = Opcode & 0x0ff;
switch (Opcode)
{
case 0x000:
{
RLC_REG(R.BC.B.h);
Cycles = 2;
}
break;
case 0x001:
{
RLC_REG(R.BC.B.l);
Cycles = 2;
}
break;
case 0x002:
{
RLC_REG(R.DE.B.h);
Cycles = 2;
}
break;
case 0x003:
{
RLC_REG(R.DE.B.l);
Cycles = 2;
}
break;
case 0x004:
{
RLC_REG(R.HL.B.h);
Cycles = 2;
}
break;
case 0x005:
{
RLC_REG(R.HL.B.l);
Cycles = 2;
}
break;
case 0x006:
{
RLC_HL();
Cycles = 4;
}
break;
case 0x007:
{
RLC_REG(R.AF.B.h);
Cycles = 2;
}
break;
case 0x008:
{
RRC_REG(R.BC.B.h);
Cycles = 2;
}
break;
case 0x009:
{
RRC_REG(R.BC.B.l);
Cycles = 2;
}
break;
case 0x00a:
{
RRC_REG(R.DE.B.h);
Cycles = 2;
}
break;
case 0x00b:
{
RRC_REG(R.DE.B.l);
Cycles = 2;
}
break;
case 0x00c:
{
RRC_REG(R.HL.B.h);
Cycles = 2;
}
break;
case 0x00d:
{
RRC_REG(R.HL.B.l);
Cycles = 2;
}
break;
case 0x00e:
{
RRC_HL();
Cycles = 4;
}
break;
case 0x00f:
{
RRC_REG(R.AF.B.h);
Cycles = 2;
}
break;
case 0x010:
{
RL_REG(R.BC.B.h);
Cycles = 2;
}
break;
case 0x011:
{
RL_REG(R.BC.B.l);
Cycles = 2;
}
break;
case 0x012:
{
RL_REG(R.DE.B.h);
Cycles = 2;
}
break;
case 0x013:
{
RL_REG(R.DE.B.l);
Cycles = 2;
}
break;
case 0x014:
{
RL_REG(R.HL.B.h);
Cycles = 2;
}
break;
case 0x015:
{
RL_REG(R.HL.B.l);
Cycles = 2;
}
break;
case 0x016:
{
RL_HL();
Cycles = 4;
}
break;
case 0x017:
{
RL_REG(R.AF.B.h);
Cycles = 2;
}
break;
case 0x018:
{
RR_REG(R.BC.B.h);
Cycles = 2;
}
break;
case 0x019:
{
RR_REG(R.BC.B.l);
Cycles = 2;
}
break;
case 0x01a:
{
RR_REG(R.DE.B.h);
Cycles = 2;
}
break;
case 0x01b:
{
RR_REG(R.DE.B.l);
Cycles = 2;
}
break;
case 0x01c:
{
RR_REG(R.HL.B.h);
Cycles = 2;
}
break;
case 0x01d:
{
RR_REG(R.HL.B.l);
Cycles = 2;
}
break;
case 0x01e:
{
RR_HL();
Cycles = 4;
}
break;
case 0x01f:
{
RR_REG(R.AF.B.h);
Cycles = 2;
}
break;
case 0x020:
{
SLA_REG(R.BC.B.h);
Cycles = 2;
}
break;
case 0x021:
{
SLA_REG(R.BC.B.l);
Cycles = 2;
}
break;
case 0x022:
{
SLA_REG(R.DE.B.h);
Cycles = 2;
}
break;
case 0x023:
{
SLA_REG(R.DE.B.l);
Cycles = 2;
}
break;
case 0x024:
{
SLA_REG(R.HL.B.h);
Cycles = 2;
}
break;
case 0x025:
{
SLA_REG(R.HL.B.l);
Cycles = 2;
}
break;
case 0x026:
{
SLA_HL();
Cycles = 4;
}
break;
case 0x027:
{
SLA_REG(R.AF.B.h);
Cycles = 2;
}
break;
case 0x028:
{
SRA_REG(R.BC.B.h);
Cycles = 2;
}
break;
case 0x029:
{
SRA_REG(R.BC.B.l);
Cycles = 2;
}
break;
case 0x02a:
{
SRA_REG(R.DE.B.h);
Cycles = 2;
}
break;
case 0x02b:
{
SRA_REG(R.DE.B.l);
Cycles = 2;
}
break;
case 0x02c:
{
SRA_REG(R.HL.B.h);
Cycles = 2;
}
break;
case 0x02d:
{
SRA_REG(R.HL.B.l);
Cycles = 2;
}
break;
case 0x02e:
{
SRA_HL();
Cycles = 4;
}
break;
case 0x02f:
{
SRA_REG(R.AF.B.h);
Cycles = 2;
}
break;
case 0x030:
{
SLL_REG(R.BC.B.h);
Cycles = 2;
}
break;
case 0x031:
{
SLL_REG(R.BC.B.l);
Cycles = 2;
}
break;
case 0x032:
{
SLL_REG(R.DE.B.h);
Cycles = 2;
}
break;
case 0x033:
{
SLL_REG(R.DE.B.l);
Cycles = 2;
}
break;
case 0x034:
{
SLL_REG(R.HL.B.h);
Cycles = 2;
}
break;
case 0x035:
{
SLL_REG(R.HL.B.l);
Cycles = 2;
}
break;
case 0x036:
{
SLL_HL();
Cycles = 4;
}
break;
case 0x037:
{
SLL_REG(R.AF.B.h);
Cycles = 2;
}
break;
case 0x038:
{
SRL_REG(R.BC.B.h);
Cycles = 2;
}
break;
case 0x039:
{
SRL_REG(R.BC.B.l);
Cycles = 2;
}
break;
case 0x03a:
{
SRL_REG(R.DE.B.h);
Cycles = 2;
}
break;
case 0x03b:
{
SRL_REG(R.DE.B.l);
Cycles = 2;
}
break;
case 0x03c:
{
SRL_REG(R.HL.B.h);
Cycles = 2;
}
break;
case 0x03d:
{
SRL_REG(R.HL.B.l);
Cycles = 2;
}
break;
case 0x03e:
{
SRL_HL();
Cycles = 4;
}
break;
case 0x03f:
{
SRL_REG(R.AF.B.h);
Cycles = 2;
}
break;
case 0x040:
{
BIT_REG(0,R.BC.B.h);
Cycles = 2;
}
break;
case 0x041:
{
BIT_REG(0,R.BC.B.l);
Cycles = 2;
}
break;
case 0x042:
{
BIT_REG(0,R.DE.B.h);
Cycles = 2;
}
break;
case 0x043:
{
BIT_REG(0,R.DE.B.l);
Cycles = 2;
}
break;
case 0x044:
{
BIT_REG(0,R.HL.B.h);
Cycles = 2;
}
break;
case 0x045:
{
BIT_REG(0,R.HL.B.l);
Cycles = 2;
}
break;
case 0x046:
{
BIT_HL(0);
Cycles = 3;
}
break;
case 0x047:
{
BIT_REG(0,R.AF.B.h);
Cycles = 2;
}
break;
case 0x048:
{
BIT_REG(1,R.BC.B.h);
Cycles = 2;
}
break;
case 0x049:
{
BIT_REG(1,R.BC.B.l);
Cycles = 2;
}
break;
case 0x04a:
{
BIT_REG(1,R.DE.B.h);
Cycles = 2;
}
break;
case 0x04b:
{
BIT_REG(1,R.DE.B.l);
Cycles = 2;
}
break;
case 0x04c:
{
BIT_REG(1,R.HL.B.h);
Cycles = 2;
}
break;
case 0x04d:
{
BIT_REG(1,R.HL.B.l);
Cycles = 2;
}
break;
case 0x04e:
{
BIT_HL(1);
Cycles = 3;
}
break;
case 0x04f:
{
BIT_REG(1,R.AF.B.h);
Cycles = 2;
}
break;
case 0x050:
{
BIT_REG(2,R.BC.B.h);
Cycles = 2;
}
break;
case 0x051:
{
BIT_REG(2,R.BC.B.l);
Cycles = 2;
}
break;
case 0x052:
{
BIT_REG(2,R.DE.B.h);
Cycles = 2;
}
break;
case 0x053:
{
BIT_REG(2,R.DE.B.l);
Cycles = 2;
}
break;
case 0x054:
{
BIT_REG(2,R.HL.B.h);
Cycles = 2;
}
break;
case 0x055:
{
BIT_REG(2,R.HL.B.l);
Cycles = 2;
}
break;
case 0x056:
{
BIT_HL(2);
Cycles = 3;
}
break;
case 0x057:
{
BIT_REG(2,R.AF.B.h);
Cycles = 2;
}
break;
case 0x058:
{
BIT_REG(3,R.BC.B.h);
Cycles = 2;
}
break;
case 0x059:
{
BIT_REG(3,R.BC.B.l);
Cycles = 2;
}
break;
case 0x05a:
{
BIT_REG(3,R.DE.B.h);
Cycles = 2;
}
break;
case 0x05b:
{
BIT_REG(3,R.DE.B.l);
Cycles = 2;
}
break;
case 0x05c:
{
BIT_REG(3,R.HL.B.h);
Cycles = 2;
}
break;
case 0x05d:
{
BIT_REG(3,R.HL.B.l);
Cycles = 2;
}
break;
case 0x05e:
{
BIT_HL(3);
Cycles = 3;
}
break;
case 0x05f:
{
BIT_REG(3,R.AF.B.h);
Cycles = 2;
}
break;
case 0x060:
{
BIT_REG(4,R.BC.B.h);
Cycles = 2;
}
break;
case 0x061:
{
BIT_REG(4,R.BC.B.l);
Cycles = 2;
}
break;
case 0x062:
{
BIT_REG(4,R.DE.B.h);
Cycles = 2;
}
break;
case 0x063:
{
BIT_REG(4,R.DE.B.l);
Cycles = 2;
}
break;
case 0x064:
{
BIT_REG(4,R.HL.B.h);
Cycles = 2;
}
break;
case 0x065:
{
BIT_REG(4,R.HL.B.l);
Cycles = 2;
}
break;
case 0x066:
{
BIT_HL(4);
Cycles = 3;
}
break;
case 0x067:
{
BIT_REG(4,R.AF.B.h);
Cycles = 2;
}
break;
case 0x068:
{
BIT_REG(5,R.BC.B.h);
Cycles = 2;
}
break;
case 0x069:
{
BIT_REG(5,R.BC.B.l);
Cycles = 2;
}
break;
case 0x06a:
{
BIT_REG(5,R.DE.B.h);
Cycles = 2;
}
break;
case 0x06b:
{
BIT_REG(5,R.DE.B.l);
Cycles = 2;
}
break;
case 0x06c:
{
BIT_REG(5,R.HL.B.h);
Cycles = 2;
}
break;
case 0x06d:
{
BIT_REG(5,R.HL.B.l);
Cycles = 2;
}
break;
case 0x06e:
{
BIT_HL(5);
Cycles = 3;
}
break;
case 0x06f:
{
BIT_REG(5,R.AF.B.h);
Cycles = 2;
}
break;
case 0x070:
{
BIT_REG(6,R.BC.B.h);
Cycles = 2;
}
break;
case 0x071:
{
BIT_REG(6,R.BC.B.l);
Cycles = 2;
}
break;
case 0x072:
{
BIT_REG(6,R.DE.B.h);
Cycles = 2;
}
break;
case 0x073:
{
BIT_REG(6,R.DE.B.l);
Cycles = 2;
}
break;
case 0x074:
{
BIT_REG(6,R.HL.B.h);
Cycles = 2;
}
break;
case 0x075:
{
BIT_REG(6,R.HL.B.l);
Cycles = 2;
}
break;
case 0x076:
{
BIT_HL(6);
Cycles = 3;
}
break;
case 0x077:
{
BIT_REG(6,R.AF.B.h);
Cycles = 2;
}
break;
case 0x078:
{
BIT_REG(7,R.BC.B.h);
Cycles = 2;
}
break;
case 0x079:
{
BIT_REG(7,R.BC.B.l);
Cycles = 2;
}
break;
case 0x07a:
{
BIT_REG(7,R.DE.B.h);
Cycles = 2;
}
break;
case 0x07b:
{
BIT_REG(7,R.DE.B.l);
Cycles = 2;
}
break;
case 0x07c:
{
BIT_REG(7,R.HL.B.h);
Cycles = 2;
}
break;
case 0x07d:
{
BIT_REG(7,R.HL.B.l);
Cycles = 2;
}
break;
case 0x07e:
{
BIT_HL(7);
Cycles = 3;
}
break;
case 0x07f:
{
BIT_REG(7,R.AF.B.h);
Cycles = 2;
}
break;
case 0x080:
{
RES_REG(0x01,R.BC.B.h);
Cycles = 2;
}
break;
case 0x081:
{
RES_REG(0x01,R.BC.B.l);
Cycles = 2;
}
break;
case 0x082:
{
RES_REG(0x01,R.DE.B.h);
Cycles = 2;
}
break;
case 0x083:
{
RES_REG(0x01,R.DE.B.l);
Cycles = 2;
}
break;
case 0x084:
{
RES_REG(0x01,R.HL.B.h);
Cycles = 2;
}
break;
case 0x085:
{
RES_REG(0x01,R.HL.B.l);
Cycles = 2;
}
break;
case 0x086:
{
RES_HL(0x01);
Cycles = 4;
}
break;
case 0x087:
{
RES_REG(0x01,R.AF.B.h);
Cycles = 2;
}
break;
case 0x088:
{
RES_REG(0x02,R.BC.B.h);
Cycles = 2;
}
break;
case 0x089:
{
RES_REG(0x02,R.BC.B.l);
Cycles = 2;
}
break;
case 0x08a:
{
RES_REG(0x02,R.DE.B.h);
Cycles = 2;
}
break;
case 0x08b:
{
RES_REG(0x02,R.DE.B.l);
Cycles = 2;
}
break;
case 0x08c:
{
RES_REG(0x02,R.HL.B.h);
Cycles = 2;
}
break;
case 0x08d:
{
RES_REG(0x02,R.HL.B.l);
Cycles = 2;
}
break;
case 0x08e:
{
RES_HL(0x02);
Cycles = 4;
}
break;
case 0x08f:
{
RES_REG(0x02,R.AF.B.h);
Cycles = 2;
}
break;
case 0x090:
{
RES_REG(0x04,R.BC.B.h);
Cycles = 2;
}
break;
case 0x091:
{
RES_REG(0x04,R.BC.B.l);
Cycles = 2;
}
break;
case 0x092:
{
RES_REG(0x04,R.DE.B.h);
Cycles = 2;
}
break;
case 0x093:
{
RES_REG(0x04,R.DE.B.l);
Cycles = 2;
}
break;
case 0x094:
{
RES_REG(0x04,R.HL.B.h);
Cycles = 2;
}
break;
case 0x095:
{
RES_REG(0x04,R.HL.B.l);
Cycles = 2;
}
break;
case 0x096:
{
RES_HL(0x04);
Cycles = 4;
}
break;
case 0x097:
{
RES_REG(0x04,R.AF.B.h);
Cycles = 2;
}
break;
case 0x098:
{
RES_REG(0x08,R.BC.B.h);
Cycles = 2;
}
break;
case 0x099:
{
RES_REG(0x08,R.BC.B.l);
Cycles = 2;
}
break;
case 0x09a:
{
RES_REG(0x08,R.DE.B.h);
Cycles = 2;
}
break;
case 0x09b:
{
RES_REG(0x08,R.DE.B.l);
Cycles = 2;
}
break;
case 0x09c:
{
RES_REG(0x08,R.HL.B.h);
Cycles = 2;
}
break;
case 0x09d:
{
RES_REG(0x08,R.HL.B.l);
Cycles = 2;
}
break;
case 0x09e:
{
RES_HL(0x08);
Cycles = 4;
}
break;
case 0x09f:
{
RES_REG(0x08,R.AF.B.h);
Cycles = 2;
}
break;
case 0x0a0:
{
RES_REG(0x10,R.BC.B.h);
Cycles = 2;
}
break;
case 0x0a1:
{
RES_REG(0x10,R.BC.B.l);
Cycles = 2;
}
break;
case 0x0a2:
{
RES_REG(0x10,R.DE.B.h);
Cycles = 2;
}
break;
case 0x0a3:
{
RES_REG(0x10,R.DE.B.l);
Cycles = 2;
}
break;
case 0x0a4:
{
RES_REG(0x10,R.HL.B.h);
Cycles = 2;
}
break;
case 0x0a5:
{
RES_REG(0x10,R.HL.B.l);
Cycles = 2;
}
break;
case 0x0a6:
{
RES_HL(0x10);
Cycles = 4;
}
break;
case 0x0a7:
{
RES_REG(0x10,R.AF.B.h);
Cycles = 2;
}
break;
case 0x0a8:
{
RES_REG(0x20,R.BC.B.h);
Cycles = 2;
}
break;
case 0x0a9:
{
RES_REG(0x20,R.BC.B.l);
Cycles = 2;
}
break;
case 0x0aa:
{
RES_REG(0x20,R.DE.B.h);
Cycles = 2;
}
break;
case 0x0ab:
{
RES_REG(0x20,R.DE.B.l);
Cycles = 2;
}
break;
case 0x0ac:
{
RES_REG(0x20,R.HL.B.h);
Cycles = 2;
}
break;
case 0x0ad:
{
RES_REG(0x20,R.HL.B.l);
Cycles = 2;
}
break;
case 0x0ae:
{
RES_HL(0x20);
Cycles = 4;
}
break;
case 0x0af:
{
RES_REG(0x20,R.AF.B.h);
Cycles = 2;
}
break;
case 0x0b0:
{
RES_REG(0x40,R.BC.B.h);
Cycles = 2;
}
break;
case 0x0b1:
{
RES_REG(0x40,R.BC.B.l);
Cycles = 2;
}
break;
case 0x0b2:
{
RES_REG(0x40,R.DE.B.h);
Cycles = 2;
}
break;
case 0x0b3:
{
RES_REG(0x40,R.DE.B.l);
Cycles = 2;
}
break;
case 0x0b4:
{
RES_REG(0x40,R.HL.B.h);
Cycles = 2;
}
break;
case 0x0b5:
{
RES_REG(0x40,R.HL.B.l);
Cycles = 2;
}
break;
case 0x0b6:
{
RES_HL(0x40);
Cycles = 4;
}
break;
case 0x0b7:
{
RES_REG(0x40,R.AF.B.h);
Cycles = 2;
}
break;
case 0x0b8:
{
RES_REG(0x80,R.BC.B.h);
Cycles = 2;
}
break;
case 0x0b9:
{
RES_REG(0x80,R.BC.B.l);
Cycles = 2;
}
break;
case 0x0ba:
{
RES_REG(0x80,R.DE.B.h);
Cycles = 2;
}
break;
case 0x0bb:
{
RES_REG(0x80,R.DE.B.l);
Cycles = 2;
}
break;
case 0x0bc:
{
RES_REG(0x80,R.HL.B.h);
Cycles = 2;
}
break;
case 0x0bd:
{
RES_REG(0x80,R.HL.B.l);
Cycles = 2;
}
break;
case 0x0be:
{
RES_HL(0x80);
Cycles = 4;
}
break;
case 0x0bf:
{
RES_REG(0x80,R.AF.B.h);
Cycles = 2;
}
break;
case 0x0c0:
{
SET_REG(0x01,R.BC.B.h);
Cycles = 2;
}
break;
case 0x0c1:
{
SET_REG(0x01,R.BC.B.l);
Cycles = 2;
}
break;
case 0x0c2:
{
SET_REG(0x01,R.DE.B.h);
Cycles = 2;
}
break;
case 0x0c3:
{
SET_REG(0x01,R.DE.B.l);
Cycles = 2;
}
break;
case 0x0c4:
{
SET_REG(0x01,R.HL.B.h);
Cycles = 2;
}
break;
case 0x0c5:
{
SET_REG(0x01,R.HL.B.l);
Cycles = 2;
}
break;
case 0x0c6:
{
SET_HL(0x01);
Cycles = 4;
}
break;
case 0x0c7:
{
SET_REG(0x01,R.AF.B.h);
Cycles = 2;
}
break;
case 0x0c8:
{
SET_REG(0x02,R.BC.B.h);
Cycles = 2;
}
break;
case 0x0c9:
{
SET_REG(0x02,R.BC.B.l);
Cycles = 2;
}
break;
case 0x0ca:
{
SET_REG(0x02,R.DE.B.h);
Cycles = 2;
}
break;
case 0x0cb:
{
SET_REG(0x02,R.DE.B.l);
Cycles = 2;
}
break;
case 0x0cc:
{
SET_REG(0x02,R.HL.B.h);
Cycles = 2;
}
break;
case 0x0cd:
{
SET_REG(0x02,R.HL.B.l);
Cycles = 2;
}
break;
case 0x0ce:
{
SET_HL(0x02);
Cycles = 4;
}
break;
case 0x0cf:
{
SET_REG(0x02,R.AF.B.h);
Cycles = 2;
}
break;
case 0x0d0:
{
SET_REG(0x04,R.BC.B.h);
Cycles = 2;
}
break;
case 0x0d1:
{
SET_REG(0x04,R.BC.B.l);
Cycles = 2;
}
break;
case 0x0d2:
{
SET_REG(0x04,R.DE.B.h);
Cycles = 2;
}
break;
case 0x0d3:
{
SET_REG(0x04,R.DE.B.l);
Cycles = 2;
}
break;
case 0x0d4:
{
SET_REG(0x04,R.HL.B.h);
Cycles = 2;
}
break;
case 0x0d5:
{
SET_REG(0x04,R.HL.B.l);
Cycles = 2;
}
break;
case 0x0d6:
{
SET_HL(0x04);
Cycles = 4;
}
break;
case 0x0d7:
{
SET_REG(0x04,R.AF.B.h);
Cycles = 2;
}
break;
case 0x0d8:
{
SET_REG(0x08,R.BC.B.h);
Cycles = 2;
}
break;
case 0x0d9:
{
SET_REG(0x08,R.BC.B.l);
Cycles = 2;
}
break;
case 0x0da:
{
SET_REG(0x08,R.DE.B.h);
Cycles = 2;
}
break;
case 0x0db:
{
SET_REG(0x08,R.DE.B.l);
Cycles = 2;
}
break;
case 0x0dc:
{
SET_REG(0x08,R.HL.B.h);
Cycles = 2;
}
break;
case 0x0dd:
{
SET_REG(0x08,R.HL.B.l);
Cycles = 2;
}
break;
case 0x0de:
{
SET_HL(0x08);
Cycles = 4;
}
break;
case 0x0df:
{
SET_REG(0x08,R.AF.B.h);
Cycles = 2;
}
break;
case 0x0e0:
{
SET_REG(0x10,R.BC.B.h);
Cycles = 2;
}
break;
case 0x0e1:
{
SET_REG(0x10,R.BC.B.l);
Cycles = 2;
}
break;
case 0x0e2:
{
SET_REG(0x10,R.DE.B.h);
Cycles = 2;
}
break;
case 0x0e3:
{
SET_REG(0x10,R.DE.B.l);
Cycles = 2;
}
break;
case 0x0e4:
{
SET_REG(0x10,R.HL.B.h);
Cycles = 2;
}
break;
case 0x0e5:
{
SET_REG(0x10,R.HL.B.l);
Cycles = 2;
}
break;
case 0x0e6:
{
SET_HL(0x10);
Cycles = 4;
}
break;
case 0x0e7:
{
SET_REG(0x10,R.AF.B.h);
Cycles = 2;
}
break;
case 0x0e8:
{
SET_REG(0x20,R.BC.B.h);
Cycles = 2;
}
break;
case 0x0e9:
{
SET_REG(0x20,R.BC.B.l);
Cycles = 2;
}
break;
case 0x0ea:
{
SET_REG(0x20,R.DE.B.h);
Cycles = 2;
}
break;
case 0x0eb:
{
SET_REG(0x20,R.DE.B.l);
Cycles = 2;
}
break;
case 0x0ec:
{
SET_REG(0x20,R.HL.B.h);
Cycles = 2;
}
break;
case 0x0ed:
{
SET_REG(0x20,R.HL.B.l);
Cycles = 2;
}
break;
case 0x0ee:
{
SET_HL(0x20);
Cycles = 4;
}
break;
case 0x0ef:
{
SET_REG(0x20,R.AF.B.h);
Cycles = 2;
}
break;
case 0x0f0:
{
SET_REG(0x40,R.BC.B.h);
Cycles = 2;
}
break;
case 0x0f1:
{
SET_REG(0x40,R.BC.B.l);
Cycles = 2;
}
break;
case 0x0f2:
{
SET_REG(0x40,R.DE.B.h);
Cycles = 2;
}
break;
case 0x0f3:
{
SET_REG(0x40,R.DE.B.l);
Cycles = 2;
}
break;
case 0x0f4:
{
SET_REG(0x40,R.HL.B.h);
Cycles = 2;
}
break;
case 0x0f5:
{
SET_REG(0x40,R.HL.B.l);
Cycles = 2;
}
break;
case 0x0f6:
{
SET_HL(0x40);
Cycles = 4;
}
break;
case 0x0f7:
{
SET_REG(0x40,R.AF.B.h);
Cycles = 2;
}
break;
case 0x0f8:
{
SET_REG(0x80,R.BC.B.h);
Cycles = 2;
}
break;
case 0x0f9:
{
SET_REG(0x80,R.BC.B.l);
Cycles = 2;
}
break;
case 0x0fa:
{
SET_REG(0x80,R.DE.B.h);
Cycles = 2;
}
break;
case 0x0fb:
{
SET_REG(0x80,R.DE.B.l);
Cycles = 2;
}
break;
case 0x0fc:
{
SET_REG(0x80,R.HL.B.h);
Cycles = 2;
}
break;
case 0x0fd:
{
SET_REG(0x80,R.HL.B.l);
Cycles = 2;
}
break;
case 0x0fe:
{
SET_HL(0x80);
Cycles = 4;
}
break;
case 0x0ff:
{
SET_REG(0x80,R.AF.B.h);
Cycles = 2;
}
break;
default:
/* the following tells MSDEV 6 to not generate */
/* code which checks if a input value to the  */
/* switch is not valid.*/
#ifdef _MSC_VER
#if (_MSC_VER>=1200)
__assume(0);
#endif
#endif
break;
}
        R.R+=2;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
return Cycles;
}
/***************************************************************************/
int Z80_ExecuteInstruction(void)
{
unsigned long Opcode;
unsigned long Cycles=0;
Opcode = Z80_RD_OPCODE_BYTE(0);
Opcode = Opcode & 0x0ff;
switch (Opcode)
{
case 0x000:
{
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x001:
{
/* LD BC,nnnn */
 
        R.BC.W = Z80_RD_OPCODE_WORD(1); 
        R.R+=1;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x002:
{
/* LD (BC),A */
 
    Z80_WR_BYTE(R.BC.W,R.AF.B.h); 
	R.MemPtr.B.l = (R.BC.W+1) & 0x0ff; 
	R.MemPtr.B.h = R.AF.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x003:
{
/* INC BC */
 
    ++R.BC.W;                
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x004:
{
INC_R(R.BC.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x005:
{
DEC_R(R.BC.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x006:
{
 /* LD B,n */
R.BC.B.h = Z80_RD_OPCODE_BYTE(1);
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x007:
{
RLCA();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x008:
{
SWAP(R.AF.W,R.altAF.W);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x009:
{
ADD_RR_rr(R.HL.W,R.BC.W);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x00a:
{
/* LD A,(BC) */
 
    R.AF.B.h = Z80_RD_BYTE(R.BC.W); 
	R.MemPtr.W = R.BC.W+1; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x00b:
{
/* DEC BC */
 
	--R.BC.W;                
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x00c:
{
INC_R(R.BC.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x00d:
{
DEC_R(R.BC.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x00e:
{
 /* LD C,n */
R.BC.B.l = Z80_RD_OPCODE_BYTE(1);
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x00f:
{
RRCA();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x010:
{
Cycles = DJNZ_dd();
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x011:
{
/* LD DE,nnnn */
 
        R.DE.W = Z80_RD_OPCODE_WORD(1); 
        R.R+=1;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x012:
{
/* LD (DE),A */
 
    Z80_WR_BYTE(R.DE.W,R.AF.B.h); 
	R.MemPtr.B.l = (R.DE.W+1) & 0x0ff; 
	R.MemPtr.B.h = R.AF.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x013:
{
/* INC DE */
 
    ++R.DE.W;                
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x014:
{
INC_R(R.DE.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x015:
{
DEC_R(R.DE.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x016:
{
 /* LD D,n */
R.DE.B.h = Z80_RD_OPCODE_BYTE(1);
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x017:
{
RLA();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x018:
{
/* JR dd */
Z80_BYTE_OFFSET Offset;
Offset = Z80_RD_OPCODE_BYTE(1);
R.MemPtr.W = R.PC.W.l + (Z80_LONG)2 + Offset;
R.PC.W.l = R.MemPtr.W;
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x019:
{
ADD_RR_rr(R.HL.W,R.DE.W);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x01a:
{
/* LD A,(DE) */
 
    R.AF.B.h = Z80_RD_BYTE(R.DE.W); 
	R.MemPtr.W = R.DE.W+1; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x01b:
{
/* DEC DE */
 
	--R.DE.W;                
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x01c:
{
INC_R(R.DE.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x01d:
{
DEC_R(R.DE.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x01e:
{
 /* LD E,n */
R.DE.B.l = Z80_RD_OPCODE_BYTE(1);
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x01f:
{
RRA();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x020:
{
/* JR cc,dd */
Z80_BYTE_OFFSET Offset;
Offset = Z80_RD_OPCODE_BYTE(1);
R.MemPtr.W = R.PC.W.l + (Z80_LONG)2 + Offset;
if (Z80_TEST_ZERO_NOT_SET)
{
R.PC.W.l = R.MemPtr.W;
Cycles=3;
}
else
{
		R.PC.W.l+=2; Cycles=2;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x021:
{
/* LD HL,nnnn */
 
        R.HL.W = Z80_RD_OPCODE_WORD(1); 
        R.R+=1;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x022:
{
/* LD (nnnn),HL */
 	R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
 	Z80_WR_WORD(R.MemPtr.W, R.HL.W);
 	++R.MemPtr.W;
        R.R+=1;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x023:
{
/* INC HL */
 
    ++R.HL.W;                
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x024:
{
INC_R(R.HL.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x025:
{
DEC_R(R.HL.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x026:
{
 /* LD H,n */
R.HL.B.h = Z80_RD_OPCODE_BYTE(1);
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x027:
{
DAA();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x028:
{
/* JR cc,dd */
Z80_BYTE_OFFSET Offset;
Offset = Z80_RD_OPCODE_BYTE(1);
R.MemPtr.W = R.PC.W.l + (Z80_LONG)2 + Offset;
if (Z80_TEST_ZERO_SET)
{
R.PC.W.l = R.MemPtr.W;
Cycles=3;
}
else
{
		R.PC.W.l+=2; Cycles=2;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x029:
{
ADD_RR_rr(R.HL.W,R.HL.W);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x02a:
{
/* LD HL,(nnnn) */
 	R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
 	R.HL.W = Z80_RD_WORD(R.MemPtr.W);
 	++R.MemPtr.W;
         R.R+=1;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x02b:
{
/* DEC HL */
 
	--R.HL.W;                
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x02c:
{
INC_R(R.HL.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x02d:
{
DEC_R(R.HL.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x02e:
{
 /* LD L,n */
R.HL.B.l = Z80_RD_OPCODE_BYTE(1);
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x02f:
{
/* CPL */
				
	Z80_BYTE Flags;			
        /* complement */	
        R.AF.B.h = (Z80_BYTE)(~R.AF.B.h);	
											
		Flags = Z80_FLAGS_REG;					
		Flags = Flags & (Z80_SIGN_FLAG | Z80_ZERO_FLAG | Z80_PARITY_FLAG | Z80_CARRY_FLAG);	
		Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);	
		Flags |= Z80_SUBTRACT_FLAG | Z80_HALFCARRY_FLAG;			
        Z80_FLAGS_REG = Flags;											
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x030:
{
/* JR cc,dd */
Z80_BYTE_OFFSET Offset;
Offset = Z80_RD_OPCODE_BYTE(1);
R.MemPtr.W = R.PC.W.l + (Z80_LONG)2 + Offset;
if (Z80_TEST_CARRY_NOT_SET)
{
R.PC.W.l = R.MemPtr.W;
Cycles=3;
}
else
{
		R.PC.W.l+=2; Cycles=2;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x031:
{
/* LD SP,nnnn */
 
        R.SP.W = Z80_RD_OPCODE_WORD(1); 
        R.R+=1;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x032:
{
/* LD (nnnn),A */

	/* get memory address to read from and store in memptr */ 
	R.MemPtr.W = Z80_RD_OPCODE_WORD(1); 
	/* write byte */ 
	Z80_WR_BYTE(R.MemPtr.W, R.AF.B.h); 
	/* increment memory pointer */ 
	R.MemPtr.B.l++; 
	/* and store a in upper byte */ 
	R.MemPtr.B.h = R.AF.B.h; 
        R.R+=1;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x033:
{
/* INC SP */
 
    ++R.SP.W;                
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x034:
{
INC_HL_();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x035:
{
DEC_HL_();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x036:
{
 /* LD (HL),n */
R.TempByte = Z80_RD_OPCODE_BYTE(1);
Z80_WR_BYTE(R.HL.W,R.TempByte);
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x037:
{
/* SCF */
	
	Z80_BYTE	Flags;			
								
	Flags = Z80_FLAGS_REG;			
								
	Flags = Flags & (Z80_ZERO_FLAG | Z80_PARITY_FLAG | Z80_SIGN_FLAG);	
    Flags = Flags | Z80_CARRY_FLAG;										
	Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);			
																		
	Z80_FLAGS_REG = Flags;													
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x038:
{
/* JR cc,dd */
Z80_BYTE_OFFSET Offset;
Offset = Z80_RD_OPCODE_BYTE(1);
R.MemPtr.W = R.PC.W.l + (Z80_LONG)2 + Offset;
if (Z80_TEST_CARRY_SET)
{
R.PC.W.l = R.MemPtr.W;
Cycles=3;
}
else
{
		R.PC.W.l+=2; Cycles=2;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x039:
{
ADD_RR_rr(R.HL.W,R.SP.W);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x03a:
{
/* LD A,(nnnn) */

	/* get memory address to read from */ 
	R.MemPtr.W = Z80_RD_OPCODE_WORD(1); 
 
	/* read byte */ 
	R.AF.B.h = Z80_RD_BYTE(R.MemPtr.W); 
 
	/* increment memptr */ 
	++R.MemPtr.W; 
        R.R+=1;
 		R.PC.W.l+=3; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x03b:
{
/* DEC SP */
 
	--R.SP.W;                
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x03c:
{
INC_R(R.AF.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x03d:
{
DEC_R(R.AF.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x03e:
{
 /* LD A,n */
R.AF.B.h = Z80_RD_OPCODE_BYTE(1);
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x03f:
{
/* CCF */
	
	Z80_BYTE Flags;				
								
	Flags = Z80_FLAGS_REG;			
    Flags &= (Z80_CARRY_FLAG | Z80_ZERO_FLAG | Z80_PARITY_FLAG | Z80_SIGN_FLAG);	
	Flags |= ((Flags & Z80_CARRY_FLAG)<<Z80_HALFCARRY_FLAG_BIT);					
	Flags |= R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);						
    Z80_FLAGS_REG = Flags ^ Z80_CARRY_FLAG;												
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x040:
{
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x041:
{
/* LD B,C */
 
		R.BC.B.h = R.BC.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x042:
{
/* LD B,D */
 
		R.BC.B.h = R.DE.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x043:
{
/* LD B,E */
 
		R.BC.B.h = R.DE.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x044:
{
/* LD B,H */
 
		R.BC.B.h = R.HL.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x045:
{
/* LD B,L */
 
		R.BC.B.h = R.HL.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x046:
{
/* LD B,(HL) */
 
        R.BC.B.h = Z80_RD_BYTE(R.HL.W); 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x047:
{
/* LD B,A */
 
		R.BC.B.h = R.AF.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x048:
{
/* LD C,B */
 
		R.BC.B.l = R.BC.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x049:
{
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x04a:
{
/* LD C,D */
 
		R.BC.B.l = R.DE.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x04b:
{
/* LD C,E */
 
		R.BC.B.l = R.DE.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x04c:
{
/* LD C,H */
 
		R.BC.B.l = R.HL.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x04d:
{
/* LD C,L */
 
		R.BC.B.l = R.HL.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x04e:
{
/* LD C,(HL) */
 
        R.BC.B.l = Z80_RD_BYTE(R.HL.W); 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x04f:
{
/* LD C,A */
 
		R.BC.B.l = R.AF.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x050:
{
/* LD D,B */
 
		R.DE.B.h = R.BC.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x051:
{
/* LD D,C */
 
		R.DE.B.h = R.BC.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x052:
{
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x053:
{
/* LD D,E */
 
		R.DE.B.h = R.DE.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x054:
{
/* LD D,H */
 
		R.DE.B.h = R.HL.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x055:
{
/* LD D,L */
 
		R.DE.B.h = R.HL.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x056:
{
/* LD D,(HL) */
 
        R.DE.B.h = Z80_RD_BYTE(R.HL.W); 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x057:
{
/* LD D,A */
 
		R.DE.B.h = R.AF.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x058:
{
/* LD E,B */
 
		R.DE.B.l = R.BC.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x059:
{
/* LD E,C */
 
		R.DE.B.l = R.BC.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x05a:
{
/* LD E,D */
 
		R.DE.B.l = R.DE.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x05b:
{
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x05c:
{
/* LD E,H */
 
		R.DE.B.l = R.HL.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x05d:
{
/* LD E,L */
 
		R.DE.B.l = R.HL.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x05e:
{
/* LD E,(HL) */
 
        R.DE.B.l = Z80_RD_BYTE(R.HL.W); 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x05f:
{
/* LD E,A */
 
		R.DE.B.l = R.AF.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x060:
{
/* LD H,B */
 
		R.HL.B.h = R.BC.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x061:
{
/* LD H,C */
 
		R.HL.B.h = R.BC.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x062:
{
/* LD H,D */
 
		R.HL.B.h = R.DE.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x063:
{
/* LD H,E */
 
		R.HL.B.h = R.DE.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x064:
{
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x065:
{
/* LD H,L */
 
		R.HL.B.h = R.HL.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x066:
{
/* LD H,(HL) */
 
        R.HL.B.h = Z80_RD_BYTE(R.HL.W); 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x067:
{
/* LD H,A */
 
		R.HL.B.h = R.AF.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x068:
{
/* LD L,B */
 
		R.HL.B.l = R.BC.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x069:
{
/* LD L,C */
 
		R.HL.B.l = R.BC.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x06a:
{
/* LD L,D */
 
		R.HL.B.l = R.DE.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x06b:
{
/* LD L,E */
 
		R.HL.B.l = R.DE.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x06c:
{
/* LD L,H */
 
		R.HL.B.l = R.HL.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x06d:
{
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x06e:
{
/* LD L,(HL) */
 
        R.HL.B.l = Z80_RD_BYTE(R.HL.W); 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x06f:
{
/* LD L,A */
 
		R.HL.B.l = R.AF.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x070:
{
/* LD (HL),B */
 
        Z80_WR_BYTE(R.HL.W,R.BC.B.h); 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x071:
{
/* LD (HL),C */
 
        Z80_WR_BYTE(R.HL.W,R.BC.B.l); 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x072:
{
/* LD (HL),D */
 
        Z80_WR_BYTE(R.HL.W,R.DE.B.h); 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x073:
{
/* LD (HL),E */
 
        Z80_WR_BYTE(R.HL.W,R.DE.B.l); 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x074:
{
/* LD (HL),H */
 
        Z80_WR_BYTE(R.HL.W,R.HL.B.h); 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x075:
{
/* LD (HL),L */
 
        Z80_WR_BYTE(R.HL.W,R.HL.B.l); 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x076:
{
HALT();
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x077:
{
/* LD (HL),A */
 
        Z80_WR_BYTE(R.HL.W,R.AF.B.h); 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x078:
{
/* LD A,B */
 
		R.AF.B.h = R.BC.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x079:
{
/* LD A,C */
 
		R.AF.B.h = R.BC.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x07a:
{
/* LD A,D */
 
		R.AF.B.h = R.DE.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x07b:
{
/* LD A,E */
 
		R.AF.B.h = R.DE.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x07c:
{
/* LD A,H */
 
		R.AF.B.h = R.HL.B.h; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x07d:
{
/* LD A,L */
 
		R.AF.B.h = R.HL.B.l; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x07e:
{
/* LD A,(HL) */
 
        R.AF.B.h = Z80_RD_BYTE(R.HL.W); 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x07f:
{
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x080:
{
ADD_A_R(R.BC.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x081:
{
ADD_A_R(R.BC.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x082:
{
ADD_A_R(R.DE.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x083:
{
ADD_A_R(R.DE.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x084:
{
ADD_A_R(R.HL.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x085:
{
ADD_A_R(R.HL.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x086:
{
ADD_A_HL();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x087:
{
ADD_A_R(R.AF.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x088:
{
ADC_A_R(R.BC.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x089:
{
ADC_A_R(R.BC.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x08a:
{
ADC_A_R(R.DE.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x08b:
{
ADC_A_R(R.DE.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x08c:
{
ADC_A_R(R.HL.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x08d:
{
ADC_A_R(R.HL.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x08e:
{
ADC_A_HL();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x08f:
{
ADC_A_R(R.AF.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x090:
{
SUB_A_R(R.BC.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x091:
{
SUB_A_R(R.BC.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x092:
{
SUB_A_R(R.DE.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x093:
{
SUB_A_R(R.DE.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x094:
{
SUB_A_R(R.HL.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x095:
{
SUB_A_R(R.HL.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x096:
{
SUB_A_HL();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x097:
{
Z80_BYTE Flags;
R.AF.B.h = 0;
Flags = Z80_ZERO_FLAG | Z80_SUBTRACT_FLAG;
Z80_FLAGS_REG = Flags;
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x098:
{
SBC_A_R(R.BC.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x099:
{
SBC_A_R(R.BC.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x09a:
{
SBC_A_R(R.DE.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x09b:
{
SBC_A_R(R.DE.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x09c:
{
SBC_A_R(R.HL.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x09d:
{
SBC_A_R(R.HL.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x09e:
{
SBC_A_HL();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x09f:
{
SBC_A_R(R.AF.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0a0:
{
AND_A_R(R.BC.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0a1:
{
AND_A_R(R.BC.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0a2:
{
AND_A_R(R.DE.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0a3:
{
AND_A_R(R.DE.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0a4:
{
AND_A_R(R.HL.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0a5:
{
AND_A_R(R.HL.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0a6:
{
AND_A_HL();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0a7:
{
Z80_BYTE Flags;
Flags = R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);
Flags |= Z80_HALFCARRY_FLAG;
Flags |= ZeroSignParityTable[R.AF.B.h];
Z80_FLAGS_REG = Flags;
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0a8:
{
XOR_A_R(R.BC.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0a9:
{
XOR_A_R(R.BC.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0aa:
{
XOR_A_R(R.DE.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0ab:
{
XOR_A_R(R.DE.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0ac:
{
XOR_A_R(R.HL.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0ad:
{
XOR_A_R(R.HL.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0ae:
{
XOR_A_HL();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0af:
{
Z80_BYTE Flags;
R.AF.B.h=0;
Flags = Z80_ZERO_FLAG | Z80_PARITY_FLAG;
Z80_FLAGS_REG = Flags;
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0b0:
{
OR_A_R(R.BC.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0b1:
{
OR_A_R(R.BC.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0b2:
{
OR_A_R(R.DE.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0b3:
{
OR_A_R(R.DE.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0b4:
{
OR_A_R(R.HL.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0b5:
{
OR_A_R(R.HL.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0b6:
{
OR_A_HL();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0b7:
{
Z80_BYTE Flags;
Flags = R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);
Flags |= ZeroSignParityTable[R.AF.B.h];
Z80_FLAGS_REG = Flags;
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0b8:
{
CP_A_R(R.BC.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0b9:
{
CP_A_R(R.BC.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0ba:
{
CP_A_R(R.DE.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0bb:
{
CP_A_R(R.DE.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0bc:
{
CP_A_R(R.HL.B.h);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0bd:
{
CP_A_R(R.HL.B.l);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0be:
{
CP_A_HL();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0bf:
{
Z80_BYTE Flags;
Flags = R.AF.B.h & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2);
Flags |= Z80_ZERO_FLAG | Z80_SUBTRACT_FLAG;
Z80_FLAGS_REG = Flags;
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0c0:
{
if (Z80_TEST_ZERO_NOT_SET)
{
RETURN();
Cycles=4;
}
else
{
		R.PC.W.l+=1; Cycles=2;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0c1:
{
R.BC.W = POP();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x0c2:
{
/* JP cc, nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
if (Z80_TEST_ZERO_NOT_SET)
{
R.PC.W.l = R.MemPtr.W;
}
else
{
		R.PC.W.l+=3; }
Cycles=3;
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0c3:
{
/* JP nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
R.PC.W.l = R.MemPtr.W;
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x0c4:
{
/* CALL cc,nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
if (Z80_TEST_ZERO_NOT_SET)
{
PUSH((R.PC.W.l+3));
R.PC.W.l = R.MemPtr.W;
Cycles=5;
}
else
{
		R.PC.W.l+=3; Cycles=3;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0c5:
{
PUSH(R.BC.W);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x0c6:
{
R.TempByte = Z80_RD_OPCODE_BYTE(1);
ADD_A_X(R.TempByte);
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0c7:
{
/* RST 0x0000 */
	
/* push return address on stack */	
PUSH((Z80_WORD)(R.PC.W.l+1));			
/* set memptr to address */	
R.MemPtr.W = 0x0000;	
/* set program counter to memptr */ 
R.PC.W.l = R.MemPtr.W; 
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x0c8:
{
if (Z80_TEST_ZERO_SET)
{
RETURN();
Cycles=4;
}
else
{
		R.PC.W.l+=1; Cycles=2;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0c9:
{
RETURN();
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x0ca:
{
/* JP cc, nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
if (Z80_TEST_ZERO_SET)
{
R.PC.W.l = R.MemPtr.W;
}
else
{
		R.PC.W.l+=3; }
Cycles=3;
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0cb:
{
Cycles = Z80_CB_ExecuteInstruction();
}
break;
case 0x0cc:
{
/* CALL cc,nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
if (Z80_TEST_ZERO_SET)
{
PUSH((R.PC.W.l+3));
R.PC.W.l = R.MemPtr.W;
Cycles=5;
}
else
{
		R.PC.W.l+=3; Cycles=3;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0cd:
{
/* CALL nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
PUSH((R.PC.W.l+3));
R.PC.W.l = R.MemPtr.W;
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 5;
}
break;
case 0x0ce:
{
R.TempByte = Z80_RD_OPCODE_BYTE(1);
ADC_A_X(R.TempByte);
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0cf:
{
/* RST 0x0008 */
	
/* push return address on stack */	
PUSH((Z80_WORD)(R.PC.W.l+1));			
/* set memptr to address */	
R.MemPtr.W = 0x0008;	
/* set program counter to memptr */ 
R.PC.W.l = R.MemPtr.W; 
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x0d0:
{
if (Z80_TEST_CARRY_NOT_SET)
{
RETURN();
Cycles=4;
}
else
{
		R.PC.W.l+=1; Cycles=2;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0d1:
{
R.DE.W = POP();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x0d2:
{
/* JP cc, nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
if (Z80_TEST_CARRY_NOT_SET)
{
R.PC.W.l = R.MemPtr.W;
}
else
{
		R.PC.W.l+=3; }
Cycles=3;
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0d3:
{
/* OUT (n),A */
    /* A in upper byte of port, Data in lower byte of port */ 
    R.MemPtr.B.l = Z80_RD_OPCODE_BYTE(1); 
	R.MemPtr.B.h = R.AF.B.h; 
	/* perform out */ 
    Z80_DoOut(R.MemPtr.W, R.AF.B.h); 
	/* update mem ptr */ 
	R.MemPtr.B.l++; 
	R.MemPtr.B.h = R.AF.B.h; 
	/* no flags changed */ 
	Cycles = 3;
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0d4:
{
/* CALL cc,nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
if (Z80_TEST_CARRY_NOT_SET)
{
PUSH((R.PC.W.l+3));
R.PC.W.l = R.MemPtr.W;
Cycles=5;
}
else
{
		R.PC.W.l+=3; Cycles=3;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0d5:
{
PUSH(R.DE.W);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x0d6:
{
R.TempByte = Z80_RD_OPCODE_BYTE(1);
SUB_A_X(R.TempByte);
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0d7:
{
/* RST 0x0010 */
	
/* push return address on stack */	
PUSH((Z80_WORD)(R.PC.W.l+1));			
/* set memptr to address */	
R.MemPtr.W = 0x0010;	
/* set program counter to memptr */ 
R.PC.W.l = R.MemPtr.W; 
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x0d8:
{
if (Z80_TEST_CARRY_SET)
{
RETURN();
Cycles=4;
}
else
{
		R.PC.W.l+=1; Cycles=2;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0d9:
{
SWAP(R.DE.W, R.altDE.W);
SWAP(R.HL.W, R.altHL.W);
SWAP(R.BC.W, R.altBC.W);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0da:
{
/* JP cc, nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
if (Z80_TEST_CARRY_SET)
{
R.PC.W.l = R.MemPtr.W;
}
else
{
		R.PC.W.l+=3; }
Cycles=3;
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0db:
{
/* IN A,(n) */
 
    /* A in upper byte of port, Data in lower byte of port */
	R.MemPtr.B.l = Z80_RD_OPCODE_BYTE(1);
	R.MemPtr.B.h = R.AF.B.h;
    /* a in upper byte of port, data in lower byte of port */
    R.AF.B.h = Z80_DoIn(R.MemPtr.W);
	/* update mem ptr */
	R.MemPtr.W++;
	/* no flags changed */
	Cycles = 3;
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0dc:
{
/* CALL cc,nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
if (Z80_TEST_CARRY_SET)
{
PUSH((R.PC.W.l+3));
R.PC.W.l = R.MemPtr.W;
Cycles=5;
}
else
{
		R.PC.W.l+=3; Cycles=3;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0dd:
{
Cycles = Z80_DD_ExecuteInstruction();
}
break;
case 0x0de:
{
R.TempByte = Z80_RD_OPCODE_BYTE(1);
SBC_A_X(R.TempByte);
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0df:
{
/* RST 0x0018 */
	
/* push return address on stack */	
PUSH((Z80_WORD)(R.PC.W.l+1));			
/* set memptr to address */	
R.MemPtr.W = 0x0018;	
/* set program counter to memptr */ 
R.PC.W.l = R.MemPtr.W; 
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x0e0:
{
if (Z80_TEST_PARITY_ODD)
{
RETURN();
Cycles=4;
}
else
{
		R.PC.W.l+=1; Cycles=2;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0e1:
{
R.HL.W = POP();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x0e2:
{
/* JP cc, nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
if (Z80_TEST_PARITY_ODD)
{
R.PC.W.l = R.MemPtr.W;
}
else
{
		R.PC.W.l+=3; }
Cycles=3;
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0e3:
{
 
        R.MemPtr.W = Z80_RD_WORD(R.SP.W); 
        Z80_WR_WORD(R.SP.W, R.HL.W);    
        R.HL.W = R.MemPtr.W; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 6;
}
break;
case 0x0e4:
{
/* CALL cc,nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
if (Z80_TEST_PARITY_ODD)
{
PUSH((R.PC.W.l+3));
R.PC.W.l = R.MemPtr.W;
Cycles=5;
}
else
{
		R.PC.W.l+=3; Cycles=3;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0e5:
{
PUSH(R.HL.W);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x0e6:
{
R.TempByte = Z80_RD_OPCODE_BYTE(1);
AND_A_X(R.TempByte);
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0e7:
{
/* RST 0x0020 */
	
/* push return address on stack */	
PUSH((Z80_WORD)(R.PC.W.l+1));			
/* set memptr to address */	
R.MemPtr.W = 0x0020;	
/* set program counter to memptr */ 
R.PC.W.l = R.MemPtr.W; 
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x0e8:
{
if (Z80_TEST_PARITY_EVEN)
{
RETURN();
Cycles=4;
}
else
{
		R.PC.W.l+=1; Cycles=2;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0e9:
{
/* JP (HL) */

    R.PC.W.l=R.HL.W; 
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0ea:
{
/* JP cc, nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
if (Z80_TEST_PARITY_EVEN)
{
R.PC.W.l = R.MemPtr.W;
}
else
{
		R.PC.W.l+=3; }
Cycles=3;
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0eb:
{
SWAP(R.HL.W,R.DE.W);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 1;
}
break;
case 0x0ec:
{
/* CALL cc,nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
if (Z80_TEST_PARITY_EVEN)
{
PUSH((R.PC.W.l+3));
R.PC.W.l = R.MemPtr.W;
Cycles=5;
}
else
{
		R.PC.W.l+=3; Cycles=3;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0ed:
{
Cycles = Z80_ED_ExecuteInstruction();
}
break;
case 0x0ee:
{
R.TempByte = Z80_RD_OPCODE_BYTE(1);
XOR_A_X(R.TempByte);
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0ef:
{
/* RST 0x0028 */
	
/* push return address on stack */	
PUSH((Z80_WORD)(R.PC.W.l+1));			
/* set memptr to address */	
R.MemPtr.W = 0x0028;	
/* set program counter to memptr */ 
R.PC.W.l = R.MemPtr.W; 
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x0f0:
{
if (Z80_TEST_POSITIVE)
{
RETURN();
Cycles=4;
}
else
{
		R.PC.W.l+=1; Cycles=2;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0f1:
{
R.AF.W = POP();
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 3;
}
break;
case 0x0f2:
{
/* JP cc, nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
if (Z80_TEST_POSITIVE)
{
R.PC.W.l = R.MemPtr.W;
}
else
{
		R.PC.W.l+=3; }
Cycles=3;
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0f3:
{
/* DI */

        R.IFF1 = R.IFF2 = 0; 
        R.Flags &=~Z80_CHECK_INTERRUPT_FLAG;	
        R.R+=1;
 		R.PC.W.l+=1; Cycles = 1;
}
break;
case 0x0f4:
{
/* CALL cc,nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
if (Z80_TEST_POSITIVE)
{
PUSH((R.PC.W.l+3));
R.PC.W.l = R.MemPtr.W;
Cycles=5;
}
else
{
		R.PC.W.l+=3; Cycles=3;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0f5:
{
PUSH(R.AF.W);
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x0f6:
{
R.TempByte = Z80_RD_OPCODE_BYTE(1);
OR_A_X(R.TempByte);
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0f7:
{
/* RST 0x0030 */
	
/* push return address on stack */	
PUSH((Z80_WORD)(R.PC.W.l+1));			
/* set memptr to address */	
R.MemPtr.W = 0x0030;	
/* set program counter to memptr */ 
R.PC.W.l = R.MemPtr.W; 
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
case 0x0f8:
{
if (Z80_TEST_MINUS)
{
RETURN();
Cycles=4;
}
else
{
		R.PC.W.l+=1; Cycles=2;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0f9:
{
/* LD SP,HL */

    R.SP.W=R.HL.W; 
        R.R+=1;
 		R.PC.W.l+=1; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0fa:
{
/* JP cc, nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
if (Z80_TEST_MINUS)
{
R.PC.W.l = R.MemPtr.W;
}
else
{
		R.PC.W.l+=3; }
Cycles=3;
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0fb:
{
/* EI */

        R.IFF1 = R.IFF2 = 1; 
        R.Flags &=~Z80_CHECK_INTERRUPT_FLAG; 
        R.R+=1;
 		R.PC.W.l+=1; Cycles = 1;
}
break;
case 0x0fc:
{
/* CALL cc,nnnn */
R.MemPtr.W = Z80_RD_OPCODE_WORD(1);
if (Z80_TEST_MINUS)
{
PUSH((R.PC.W.l+3));
R.PC.W.l = R.MemPtr.W;
Cycles=5;
}
else
{
		R.PC.W.l+=3; Cycles=3;
}
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
}
break;
case 0x0fd:
{
Cycles = Z80_FD_ExecuteInstruction();
}
break;
case 0x0fe:
{
R.TempByte = Z80_RD_OPCODE_BYTE(1);
CP_A_X(R.TempByte);
        R.R+=1;
 		R.PC.W.l+=2; R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 2;
}
break;
case 0x0ff:
{
/* RST 0x0038 */
	
/* push return address on stack */	
PUSH((Z80_WORD)(R.PC.W.l+1));			
/* set memptr to address */	
R.MemPtr.W = 0x0038;	
/* set program counter to memptr */ 
R.PC.W.l = R.MemPtr.W; 
        R.R+=1;
 R.Flags |= Z80_CHECK_INTERRUPT_FLAG;
Cycles = 4;
}
break;
default:
/* the following tells MSDEV 6 to not generate */
/* code which checks if a input value to the  */
/* switch is not valid.*/
#ifdef _MSC_VER
#if (_MSC_VER>=1200)
__assume(0);
#endif
#endif
break;
}
/* check interrupts? */
if ((R.Flags & (Z80_CHECK_INTERRUPT_FLAG | Z80_EXECUTE_INTERRUPT_HANDLER_FLAG))==(Z80_CHECK_INTERRUPT_FLAG | Z80_EXECUTE_INTERRUPT_HANDLER_FLAG))
{
Cycles+=Z80_ExecuteInterrupt();
}
return Cycles;
}
/***************************************************************************/
int Z80_ExecuteIM0(void)
{
unsigned long Opcode;
unsigned long Cycles=0;
Opcode = R.InterruptVectorBase;
Opcode = Opcode & 0x0ff;
switch (Opcode)
{
case 0x001:
case 0x003:
case 0x005:
case 0x007:
case 0x009:
case 0x00b:
case 0x00d:
case 0x00f:
case 0x011:
case 0x013:
case 0x015:
case 0x017:
case 0x019:
case 0x01b:
case 0x01d:
case 0x01f:
case 0x021:
case 0x023:
case 0x025:
case 0x027:
case 0x029:
case 0x02b:
case 0x02d:
case 0x02f:
case 0x031:
case 0x033:
case 0x035:
case 0x037:
case 0x039:
case 0x03b:
case 0x03d:
case 0x03f:
case 0x041:
case 0x043:
case 0x045:
case 0x047:
case 0x049:
case 0x04b:
case 0x04d:
case 0x04f:
case 0x051:
case 0x053:
case 0x055:
case 0x057:
case 0x059:
case 0x05b:
case 0x05d:
case 0x05f:
case 0x061:
case 0x063:
case 0x065:
case 0x067:
case 0x069:
case 0x06b:
case 0x06d:
case 0x06f:
case 0x071:
case 0x073:
case 0x075:
case 0x077:
case 0x079:
case 0x07b:
case 0x07d:
case 0x07f:
case 0x081:
case 0x083:
case 0x085:
case 0x087:
case 0x089:
case 0x08b:
case 0x08d:
case 0x08f:
case 0x091:
case 0x093:
case 0x095:
case 0x097:
case 0x099:
case 0x09b:
case 0x09d:
case 0x09f:
case 0x0a1:
case 0x0a3:
case 0x0a5:
case 0x0a7:
case 0x0a9:
case 0x0ab:
case 0x0ad:
case 0x0af:
case 0x0b1:
case 0x0b3:
case 0x0b5:
case 0x0b7:
case 0x0b9:
case 0x0bb:
case 0x0bd:
case 0x0bf:
case 0x0c1:
case 0x0c3:
case 0x0c5:
case 0x0c7:
case 0x0c9:
case 0x0cb:
case 0x0cd:
case 0x0cf:
case 0x0d1:
case 0x0d3:
case 0x0d5:
case 0x0d7:
case 0x0d9:
case 0x0db:
case 0x0dd:
case 0x0df:
case 0x0e1:
case 0x0e3:
case 0x0e5:
case 0x0e7:
case 0x0e9:
case 0x0eb:
case 0x0ed:
case 0x0ef:
case 0x0f1:
case 0x0f3:
case 0x0f5:
case 0x0f7:
case 0x0f9:
case 0x0fb:
case 0x0fd:
{
}
break;
case 0x000:
{
Cycles = 1;
}
break;
case 0x002:
{
/* LD (BC),A */
 
    Z80_WR_BYTE(R.BC.W,R.AF.B.h); 
	R.MemPtr.B.l = (R.BC.W+1) & 0x0ff; 
	R.MemPtr.B.h = R.AF.B.h; 
Cycles = 2;
}
break;
case 0x004:
{
INC_R(R.BC.B.h);
Cycles = 1;
}
break;
case 0x006:
{
 /* LD B,n */
R.BC.B.h = Z80_RD_OPCODE_BYTE(0);
Cycles = 2;
}
break;
case 0x008:
{
SWAP(R.AF.W,R.altAF.W);
Cycles = 1;
}
break;
case 0x00a:
{
/* LD A,(BC) */
 
    R.AF.B.h = Z80_RD_BYTE(R.BC.W); 
	R.MemPtr.W = R.BC.W+1; 
Cycles = 2;
}
break;
case 0x00c:
{
INC_R(R.BC.B.l);
Cycles = 1;
}
break;
case 0x00e:
{
 /* LD C,n */
R.BC.B.l = Z80_RD_OPCODE_BYTE(0);
Cycles = 2;
}
break;
case 0x010:
{
Cycles = DJNZ_dd_IM0();
}
break;
case 0x012:
{
/* LD (DE),A */
 
    Z80_WR_BYTE(R.DE.W,R.AF.B.h); 
	R.MemPtr.B.l = (R.DE.W+1) & 0x0ff; 
	R.MemPtr.B.h = R.AF.B.h; 
Cycles = 2;
}
break;
case 0x014:
{
INC_R(R.DE.B.h);
Cycles = 1;
}
break;
case 0x016:
{
 /* LD D,n */
R.DE.B.h = Z80_RD_OPCODE_BYTE(0);
Cycles = 2;
}
break;
case 0x018:
{
/* JR dd */
Z80_BYTE_OFFSET Offset;
Offset = Z80_RD_BYTE_IM0();
R.MemPtr.W = R.PC.W.l + (Z80_LONG)2 + Offset;
R.PC.W.l = R.MemPtr.W;
Cycles = 3;
}
break;
case 0x01a:
{
/* LD A,(DE) */
 
    R.AF.B.h = Z80_RD_BYTE(R.DE.W); 
	R.MemPtr.W = R.DE.W+1; 
Cycles = 2;
}
break;
case 0x01c:
{
INC_R(R.DE.B.l);
Cycles = 1;
}
break;
case 0x01e:
{
 /* LD E,n */
R.DE.B.l = Z80_RD_OPCODE_BYTE(0);
Cycles = 2;
}
break;
case 0x020:
{
/* JR cc,dd */
Z80_BYTE_OFFSET Offset;
Offset = Z80_RD_BYTE_IM0();
R.MemPtr.W = R.PC.W.l + (Z80_LONG)2 + Offset;
if (Z80_TEST_ZERO_NOT_SET)
{
R.PC.W.l = R.MemPtr.W;
Cycles=3;
}
else
{
Cycles=2;
}
}
break;
case 0x022:
{
/* LD (nnnn),HL */
    R.MemPtr.W = Z80_RD_WORD_IM0();
 	Z80_WR_WORD(R.MemPtr.W, R.HL.W);
 	++R.MemPtr.W;
Cycles = 5;
}
break;
case 0x024:
{
INC_R(R.HL.B.h);
Cycles = 1;
}
break;
case 0x026:
{
 /* LD H,n */
R.HL.B.h = Z80_RD_OPCODE_BYTE(0);
Cycles = 2;
}
break;
case 0x028:
{
/* JR cc,dd */
Z80_BYTE_OFFSET Offset;
Offset = Z80_RD_BYTE_IM0();
R.MemPtr.W = R.PC.W.l + (Z80_LONG)2 + Offset;
if (Z80_TEST_ZERO_SET)
{
R.PC.W.l = R.MemPtr.W;
Cycles=3;
}
else
{
Cycles=2;
}
}
break;
case 0x02a:
{
/* LD HL,(nnnn) */
    R.MemPtr.W = Z80_RD_WORD_IM0();
 	R.HL.W = Z80_RD_WORD(R.MemPtr.W);
 	++R.MemPtr.W;
Cycles = 5;
}
break;
case 0x02c:
{
INC_R(R.HL.B.l);
Cycles = 1;
}
break;
case 0x02e:
{
 /* LD L,n */
R.HL.B.l = Z80_RD_OPCODE_BYTE(0);
Cycles = 2;
}
break;
case 0x030:
{
/* JR cc,dd */
Z80_BYTE_OFFSET Offset;
Offset = Z80_RD_BYTE_IM0();
R.MemPtr.W = R.PC.W.l + (Z80_LONG)2 + Offset;
if (Z80_TEST_CARRY_NOT_SET)
{
R.PC.W.l = R.MemPtr.W;
Cycles=3;
}
else
{
Cycles=2;
}
}
break;
case 0x032:
{
/* LD (nnnn),A */

	/* get memory address to read from and store in memptr */ 
	R.MemPtr.W = Z80_RD_OPCODE_WORD(1); 
	/* write byte */ 
	Z80_WR_BYTE(R.MemPtr.W, R.AF.B.h); 
	/* increment memory pointer */ 
	R.MemPtr.B.l++; 
	/* and store a in upper byte */ 
	R.MemPtr.B.h = R.AF.B.h; 
Cycles = 4;
}
break;
case 0x034:
{
INC_HL_();
Cycles = 3;
}
break;
case 0x036:
{
 /* LD (HL),n */
R.TempByte = Z80_RD_OPCODE_BYTE(0);
Z80_WR_BYTE(R.HL.W,R.TempByte);
Cycles = 2;
}
break;
case 0x038:
{
/* JR cc,dd */
Z80_BYTE_OFFSET Offset;
Offset = Z80_RD_BYTE_IM0();
R.MemPtr.W = R.PC.W.l + (Z80_LONG)2 + Offset;
if (Z80_TEST_CARRY_SET)
{
R.PC.W.l = R.MemPtr.W;
Cycles=3;
}
else
{
Cycles=2;
}
}
break;
case 0x03a:
{
/* LD A,(nnnn) */

	/* get memory address to read from */ 
	R.MemPtr.W = Z80_RD_OPCODE_WORD(1); 
 
	/* read byte */ 
	R.AF.B.h = Z80_RD_BYTE(R.MemPtr.W); 
 
	/* increment memptr */ 
	++R.MemPtr.W; 
Cycles = 4;
}
break;
case 0x03c:
{
INC_R(R.AF.B.h);
Cycles = 1;
}
break;
case 0x03e:
{
 /* LD A,n */
R.AF.B.h = Z80_RD_OPCODE_BYTE(0);
Cycles = 2;
}
break;
case 0x040:
{
Cycles = 1;
}
break;
case 0x042:
{
/* LD B,D */
 
		R.BC.B.h = R.DE.B.h; 
Cycles = 1;
}
break;
case 0x044:
{
/* LD B,H */
 
		R.BC.B.h = R.HL.B.h; 
Cycles = 1;
}
break;
case 0x046:
{
/* LD B,(HL) */
 
        R.BC.B.h = Z80_RD_BYTE(R.HL.W); 
Cycles = 2;
}
break;
case 0x048:
{
/* LD C,B */
 
		R.BC.B.l = R.BC.B.h; 
Cycles = 1;
}
break;
case 0x04a:
{
/* LD C,D */
 
		R.BC.B.l = R.DE.B.h; 
Cycles = 1;
}
break;
case 0x04c:
{
/* LD C,H */
 
		R.BC.B.l = R.HL.B.h; 
Cycles = 1;
}
break;
case 0x04e:
{
/* LD C,(HL) */
 
        R.BC.B.l = Z80_RD_BYTE(R.HL.W); 
Cycles = 2;
}
break;
case 0x050:
{
/* LD D,B */
 
		R.DE.B.h = R.BC.B.h; 
Cycles = 1;
}
break;
case 0x052:
{
Cycles = 1;
}
break;
case 0x054:
{
/* LD D,H */
 
		R.DE.B.h = R.HL.B.h; 
Cycles = 1;
}
break;
case 0x056:
{
/* LD D,(HL) */
 
        R.DE.B.h = Z80_RD_BYTE(R.HL.W); 
Cycles = 2;
}
break;
case 0x058:
{
/* LD E,B */
 
		R.DE.B.l = R.BC.B.h; 
Cycles = 1;
}
break;
case 0x05a:
{
/* LD E,D */
 
		R.DE.B.l = R.DE.B.h; 
Cycles = 1;
}
break;
case 0x05c:
{
/* LD E,H */
 
		R.DE.B.l = R.HL.B.h; 
Cycles = 1;
}
break;
case 0x05e:
{
/* LD E,(HL) */
 
        R.DE.B.l = Z80_RD_BYTE(R.HL.W); 
Cycles = 2;
}
break;
case 0x060:
{
/* LD H,B */
 
		R.HL.B.h = R.BC.B.h; 
Cycles = 1;
}
break;
case 0x062:
{
/* LD H,D */
 
		R.HL.B.h = R.DE.B.h; 
Cycles = 1;
}
break;
case 0x064:
{
Cycles = 1;
}
break;
case 0x066:
{
/* LD H,(HL) */
 
        R.HL.B.h = Z80_RD_BYTE(R.HL.W); 
Cycles = 2;
}
break;
case 0x068:
{
/* LD L,B */
 
		R.HL.B.l = R.BC.B.h; 
Cycles = 1;
}
break;
case 0x06a:
{
/* LD L,D */
 
		R.HL.B.l = R.DE.B.h; 
Cycles = 1;
}
break;
case 0x06c:
{
/* LD L,H */
 
		R.HL.B.l = R.HL.B.h; 
Cycles = 1;
}
break;
case 0x06e:
{
/* LD L,(HL) */
 
        R.HL.B.l = Z80_RD_BYTE(R.HL.W); 
Cycles = 2;
}
break;
case 0x070:
{
/* LD (HL),B */
 
        Z80_WR_BYTE(R.HL.W,R.BC.B.h); 
Cycles = 2;
}
break;
case 0x072:
{
/* LD (HL),D */
 
        Z80_WR_BYTE(R.HL.W,R.DE.B.h); 
Cycles = 2;
}
break;
case 0x074:
{
/* LD (HL),H */
 
        Z80_WR_BYTE(R.HL.W,R.HL.B.h); 
Cycles = 2;
}
break;
case 0x076:
{
HALT();
Cycles = 1;
}
break;
case 0x078:
{
/* LD A,B */
 
		R.AF.B.h = R.BC.B.h; 
Cycles = 1;
}
break;
case 0x07a:
{
/* LD A,D */
 
		R.AF.B.h = R.DE.B.h; 
Cycles = 1;
}
break;
case 0x07c:
{
/* LD A,H */
 
		R.AF.B.h = R.HL.B.h; 
Cycles = 1;
}
break;
case 0x07e:
{
/* LD A,(HL) */
 
        R.AF.B.h = Z80_RD_BYTE(R.HL.W); 
Cycles = 2;
}
break;
case 0x080:
{
ADD_A_R(R.BC.B.h);
Cycles = 1;
}
break;
case 0x082:
{
ADD_A_R(R.DE.B.h);
Cycles = 1;
}
break;
case 0x084:
{
ADD_A_R(R.HL.B.h);
Cycles = 1;
}
break;
case 0x086:
{
ADD_A_HL();
Cycles = 2;
}
break;
case 0x088:
{
ADC_A_R(R.BC.B.h);
Cycles = 1;
}
break;
case 0x08a:
{
ADC_A_R(R.DE.B.h);
Cycles = 1;
}
break;
case 0x08c:
{
ADC_A_R(R.HL.B.h);
Cycles = 1;
}
break;
case 0x08e:
{
ADC_A_HL();
Cycles = 2;
}
break;
case 0x090:
{
SUB_A_R(R.BC.B.h);
Cycles = 1;
}
break;
case 0x092:
{
SUB_A_R(R.DE.B.h);
Cycles = 1;
}
break;
case 0x094:
{
SUB_A_R(R.HL.B.h);
Cycles = 1;
}
break;
case 0x096:
{
SUB_A_HL();
Cycles = 2;
}
break;
case 0x098:
{
SBC_A_R(R.BC.B.h);
Cycles = 1;
}
break;
case 0x09a:
{
SBC_A_R(R.DE.B.h);
Cycles = 1;
}
break;
case 0x09c:
{
SBC_A_R(R.HL.B.h);
Cycles = 1;
}
break;
case 0x09e:
{
SBC_A_HL();
Cycles = 2;
}
break;
case 0x0a0:
{
AND_A_R(R.BC.B.h);
Cycles = 1;
}
break;
case 0x0a2:
{
AND_A_R(R.DE.B.h);
Cycles = 1;
}
break;
case 0x0a4:
{
AND_A_R(R.HL.B.h);
Cycles = 1;
}
break;
case 0x0a6:
{
AND_A_HL();
Cycles = 2;
}
break;
case 0x0a8:
{
XOR_A_R(R.BC.B.h);
Cycles = 1;
}
break;
case 0x0aa:
{
XOR_A_R(R.DE.B.h);
Cycles = 1;
}
break;
case 0x0ac:
{
XOR_A_R(R.HL.B.h);
Cycles = 1;
}
break;
case 0x0ae:
{
XOR_A_HL();
Cycles = 2;
}
break;
case 0x0b0:
{
OR_A_R(R.BC.B.h);
Cycles = 1;
}
break;
case 0x0b2:
{
OR_A_R(R.DE.B.h);
Cycles = 1;
}
break;
case 0x0b4:
{
OR_A_R(R.HL.B.h);
Cycles = 1;
}
break;
case 0x0b6:
{
OR_A_HL();
Cycles = 2;
}
break;
case 0x0b8:
{
CP_A_R(R.BC.B.h);
Cycles = 1;
}
break;
case 0x0ba:
{
CP_A_R(R.DE.B.h);
Cycles = 1;
}
break;
case 0x0bc:
{
CP_A_R(R.HL.B.h);
Cycles = 1;
}
break;
case 0x0be:
{
CP_A_HL();
Cycles = 2;
}
break;
case 0x0c0:
{
if (Z80_TEST_ZERO_NOT_SET)
{
RETURN();
Cycles=4;
}
else
{
Cycles=2;
}
}
break;
case 0x0c2:
{
/* JP cc, nnnn */
R.MemPtr.W = Z80_RD_WORD_IM0();
if (Z80_TEST_ZERO_NOT_SET)
{
R.PC.W.l = R.MemPtr.W;
}
else
{
}
Cycles=3;
}
break;
case 0x0c4:
{
/* CALL cc,nnnn */
R.MemPtr.W = Z80_RD_WORD_IM0();
if (Z80_TEST_ZERO_NOT_SET)
{
PUSH((R.PC.W.l+3));
R.PC.W.l = R.MemPtr.W;
Cycles=5;
}
else
{
Cycles=3;
}
}
break;
case 0x0c6:
{
R.TempByte = Z80_RD_OPCODE_BYTE(0);
ADD_A_X(R.TempByte);
Cycles = 2;
}
break;
case 0x0c8:
{
if (Z80_TEST_ZERO_SET)
{
RETURN();
Cycles=4;
}
else
{
Cycles=2;
}
}
break;
case 0x0ca:
{
/* JP cc, nnnn */
R.MemPtr.W = Z80_RD_WORD_IM0();
if (Z80_TEST_ZERO_SET)
{
R.PC.W.l = R.MemPtr.W;
}
else
{
}
Cycles=3;
}
break;
case 0x0cc:
{
/* CALL cc,nnnn */
R.MemPtr.W = Z80_RD_WORD_IM0();
if (Z80_TEST_ZERO_SET)
{
PUSH((R.PC.W.l+3));
R.PC.W.l = R.MemPtr.W;
Cycles=5;
}
else
{
Cycles=3;
}
}
break;
case 0x0ce:
{
R.TempByte = Z80_RD_OPCODE_BYTE(0);
ADC_A_X(R.TempByte);
Cycles = 2;
}
break;
case 0x0d0:
{
if (Z80_TEST_CARRY_NOT_SET)
{
RETURN();
Cycles=4;
}
else
{
Cycles=2;
}
}
break;
case 0x0d2:
{
/* JP cc, nnnn */
R.MemPtr.W = Z80_RD_WORD_IM0();
if (Z80_TEST_CARRY_NOT_SET)
{
R.PC.W.l = R.MemPtr.W;
}
else
{
}
Cycles=3;
}
break;
case 0x0d4:
{
/* CALL cc,nnnn */
R.MemPtr.W = Z80_RD_WORD_IM0();
if (Z80_TEST_CARRY_NOT_SET)
{
PUSH((R.PC.W.l+3));
R.PC.W.l = R.MemPtr.W;
Cycles=5;
}
else
{
Cycles=3;
}
}
break;
case 0x0d6:
{
R.TempByte = Z80_RD_OPCODE_BYTE(0);
SUB_A_X(R.TempByte);
Cycles = 2;
}
break;
case 0x0d8:
{
if (Z80_TEST_CARRY_SET)
{
RETURN();
Cycles=4;
}
else
{
Cycles=2;
}
}
break;
case 0x0da:
{
/* JP cc, nnnn */
R.MemPtr.W = Z80_RD_WORD_IM0();
if (Z80_TEST_CARRY_SET)
{
R.PC.W.l = R.MemPtr.W;
}
else
{
}
Cycles=3;
}
break;
case 0x0dc:
{
/* CALL cc,nnnn */
R.MemPtr.W = Z80_RD_WORD_IM0();
if (Z80_TEST_CARRY_SET)
{
PUSH((R.PC.W.l+3));
R.PC.W.l = R.MemPtr.W;
Cycles=5;
}
else
{
Cycles=3;
}
}
break;
case 0x0de:
{
R.TempByte = Z80_RD_OPCODE_BYTE(0);
SBC_A_X(R.TempByte);
Cycles = 2;
}
break;
case 0x0e0:
{
if (Z80_TEST_PARITY_ODD)
{
RETURN();
Cycles=4;
}
else
{
Cycles=2;
}
}
break;
case 0x0e2:
{
/* JP cc, nnnn */
R.MemPtr.W = Z80_RD_WORD_IM0();
if (Z80_TEST_PARITY_ODD)
{
R.PC.W.l = R.MemPtr.W;
}
else
{
}
Cycles=3;
}
break;
case 0x0e4:
{
/* CALL cc,nnnn */
R.MemPtr.W = Z80_RD_WORD_IM0();
if (Z80_TEST_PARITY_ODD)
{
PUSH((R.PC.W.l+3));
R.PC.W.l = R.MemPtr.W;
Cycles=5;
}
else
{
Cycles=3;
}
}
break;
case 0x0e6:
{
R.TempByte = Z80_RD_OPCODE_BYTE(0);
AND_A_X(R.TempByte);
Cycles = 2;
}
break;
case 0x0e8:
{
if (Z80_TEST_PARITY_EVEN)
{
RETURN();
Cycles=4;
}
else
{
Cycles=2;
}
}
break;
case 0x0ea:
{
/* JP cc, nnnn */
R.MemPtr.W = Z80_RD_WORD_IM0();
if (Z80_TEST_PARITY_EVEN)
{
R.PC.W.l = R.MemPtr.W;
}
else
{
}
Cycles=3;
}
break;
case 0x0ec:
{
/* CALL cc,nnnn */
R.MemPtr.W = Z80_RD_WORD_IM0();
if (Z80_TEST_PARITY_EVEN)
{
PUSH((R.PC.W.l+3));
R.PC.W.l = R.MemPtr.W;
Cycles=5;
}
else
{
Cycles=3;
}
}
break;
case 0x0ee:
{
R.TempByte = Z80_RD_OPCODE_BYTE(0);
XOR_A_X(R.TempByte);
Cycles = 2;
}
break;
case 0x0f0:
{
if (Z80_TEST_POSITIVE)
{
RETURN();
Cycles=4;
}
else
{
Cycles=2;
}
}
break;
case 0x0f2:
{
/* JP cc, nnnn */
R.MemPtr.W = Z80_RD_WORD_IM0();
if (Z80_TEST_POSITIVE)
{
R.PC.W.l = R.MemPtr.W;
}
else
{
}
Cycles=3;
}
break;
case 0x0f4:
{
/* CALL cc,nnnn */
R.MemPtr.W = Z80_RD_WORD_IM0();
if (Z80_TEST_POSITIVE)
{
PUSH((R.PC.W.l+3));
R.PC.W.l = R.MemPtr.W;
Cycles=5;
}
else
{
Cycles=3;
}
}
break;
case 0x0f6:
{
R.TempByte = Z80_RD_OPCODE_BYTE(0);
OR_A_X(R.TempByte);
Cycles = 2;
}
break;
case 0x0f8:
{
if (Z80_TEST_MINUS)
{
RETURN();
Cycles=4;
}
else
{
Cycles=2;
}
}
break;
case 0x0fa:
{
/* JP cc, nnnn */
R.MemPtr.W = Z80_RD_WORD_IM0();
if (Z80_TEST_MINUS)
{
R.PC.W.l = R.MemPtr.W;
}
else
{
}
Cycles=3;
}
break;
case 0x0fc:
{
/* CALL cc,nnnn */
R.MemPtr.W = Z80_RD_WORD_IM0();
if (Z80_TEST_MINUS)
{
PUSH((R.PC.W.l+3));
R.PC.W.l = R.MemPtr.W;
Cycles=5;
}
else
{
Cycles=3;
}
}
break;
case 0x0fe:
{
R.TempByte = Z80_RD_OPCODE_BYTE(0);
CP_A_X(R.TempByte);
Cycles = 2;
}
break;
case 0x0ff:
{
/* RST 0x0038 */
	
/* push return address on stack */	
PUSH((Z80_WORD)(R.PC.W.l+1));			
/* set memptr to address */	
R.MemPtr.W = 0x0038;	
/* set program counter to memptr */ 
R.PC.W.l = R.MemPtr.W; 
Cycles = 4;
}
break;
default:
/* the following tells MSDEV 6 to not generate */
/* code which checks if a input value to the  */
/* switch is not valid.*/
#ifdef _MSC_VER
#if (_MSC_VER>=1200)
__assume(0);
#endif
#endif
break;
}
return Cycles;
}
