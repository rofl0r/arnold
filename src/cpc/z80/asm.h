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
/* ASM version of the macros */


	mov eax, [R.PC.L]
	mov ebx, eax
	shl ebx,13
	mov cl, [ebx*4]


#define BIT(BitIndex, Register) \
{                                                                       \
__asm mov al, byte ptr [Register]       \
__asm and al, (1<<BitIndex) \
__asm lahf                                                              \
__asm mov al, byte ptr [R.AF.B.l]               \
__asm and al, Z80_CARRY_FLAG                    \
__asm or al, Z80_HALFCARRY_FLAG                 \
__asm and ah, X86_ZERO_FLAG                             \
__asm or ah,al                                                  \
__asm mov byte ptr [R.AF.B.l],ah                \
}


// rl with flags
#define RL_WITH_FLAGS_ASM(Register)     \
{                                                                       \
__asm mov al, byte ptr  [R.AF.B.l]                              \
__asm ror al,1                                                  \
__asm mov al, byte ptr [Register]                               \
__asm rcl al, 1                                                 \
__asm lahf                                                              \
__asm mov byte ptr [Register], al                               \
__asm and ah,X86_CARRY_FLAG                             \
__asm mov byte ptr [R.AF.B.l], ah                               \
__asm or al,al                                                  \
__asm lahf                                                              \
__asm and ah, X86_SIGN_FLAG | X86_ZERO_FLAG | X86_PARITY_FLAG   \
__asm or  byte ptr [R.AF.B.l], ah                               \
}

// rr with flags
#define RR_WITH_FLAGS_ASM(Register)     \
{                                                                       \
__asm mov al, byte ptr [R.AF.B.l]                               \
__asm ror al,1                                                  \
__asm mov al, byte ptr [Register]                               \
__asm rcr al, 1                                                 \
__asm lahf                                                              \
__asm mov byte ptr [Register], al                               \
__asm and ah,X86_CARRY_FLAG                             \
__asm mov byte ptr [R.AF.B.l], ah                               \
__asm or al,al                                                  \
__asm lahf                                                              \
__asm and ah, X86_SIGN_FLAG | X86_ZERO_FLAG | X86_PARITY_FLAG   \
__asm or  byte ptr [R.AF.B.l], ah                               \
}

// rlc with flags
#define RLC_WITH_FLAGS_ASM(Register)                                                    \
{       \
_asm mov al,byte ptr [Register] /* get reg */                                                   \
_asm rol al,1                           /* rotate it */                                         \
_asm lahf                                       /* get flags */                                         \
_asm mov byte ptr [Register], al        /* store reg */                                         \
_asm and ah,X86_CARRY_FLAG      /* get flags we are interested in */    \
_asm mov byte ptr [R.AF.B.l], ah        /* store flags - CARRY */                               \
_asm or al,al                           /* set sign, zero and parity */                 \
_asm lahf                                       /* get flags */                                                 \
_asm and ah, X86_SIGN_FLAG | X86_ZERO_FLAG | X86_PARITY_FLAG /* get ones we are interested in */        \
_asm or byte ptr [R.AF.B.l], ah /* store remaining flags */                             \
}


// rrc with flags
#define RRC_WITH_FLAGS_ASM(Register)                                                    \
{       \
_asm mov al,byte ptr [Register] /* get reg */                                                   \
_asm ror al,1                           /* rotate it */                                         \
_asm lahf                                       /* get flags */                                         \
_asm mov byte ptr [Register], al        /* store reg */                                         \
_asm and ah,X86_CARRY_FLAG      /* get flags we are interested in */    \
_asm mov byte ptr [R.AF.B.l], ah        /* store flags - CARRY */                               \
_asm or al,al                           /* set sign, zero and parity */                 \
_asm lahf                                       /* get flags */                                                 \
_asm and ah, X86_SIGN_FLAG | X86_ZERO_FLAG | X86_PARITY_FLAG /* get ones we are interested in */        \
_asm or byte ptr [R.AF.B.l], ah /* store remaining flags */                             \
}

// sla with flags
#define SLA_WITH_FLAGS_ASM(Register)                                                    \
{       \
_asm mov al, byte ptr [Register] /* get reg */                                                  \
_asm sal al,1                           /* rotate it */                                         \
_asm lahf                                       /* get flags */                                         \
_asm mov byte ptr [Register], al        /* store reg */                                         \
_asm and ah,X86_CARRY_FLAG      /* get flags we are interested in */    \
_asm mov byte ptr [R.AF.B.l], ah        /* store flags - CARRY */                               \
_asm or al,al                           /* set sign, zero and parity */                 \
_asm lahf                                       /* get flags */                                                 \
_asm and ah, X86_SIGN_FLAG | X86_ZERO_FLAG | X86_PARITY_FLAG /* get ones we are interested in */        \
_asm or byte ptr [R.AF.B.l], ah /* store remaining flags */                             \
}

// sla with flags
#define SRA_WITH_FLAGS_ASM(Register)                                                    \
{       \
_asm mov al, byte ptr [Register] /* get reg */                                                  \
_asm sar al,1                           /* rotate it */                                         \
_asm lahf                                       /* get flags */                                         \
_asm mov byte ptr [Register], al        /* store reg */                                         \
_asm and ah,X86_CARRY_FLAG      /* get flags we are interested in */    \
_asm mov byte ptr [R.AF.B.l], ah        /* store flags - CARRY */                               \
_asm or al,al                           /* set sign, zero and parity */                 \
_asm lahf                                       /* get flags */                                                 \
_asm and ah, X86_SIGN_FLAG | X86_ZERO_FLAG | X86_PARITY_FLAG /* get ones we are interested in */        \
_asm or byte ptr [R.AF.B.l], ah /* store remaining flags */                             \
}

// srl with flags
#define SRL_WITH_FLAGS_ASM(Register)                                                    \
{       \
_asm mov al, byte ptr [Register] /* get reg */                                                  \
_asm shr al,1                           /* rotate it */                                         \
_asm lahf                                       /* get flags */                                         \
_asm mov byte ptr [Register], al        /* store reg */                                         \
_asm and ah,X86_CARRY_FLAG      /* get flags we are interested in */    \
_asm mov byte ptr [R.AF.B.l], ah        /* store flags - CARRY */                               \
_asm or al,al                           /* set sign, zero and parity */                 \
_asm lahf                                       /* get flags */                                                 \
_asm and ah, X86_SIGN_FLAG | X86_ZERO_FLAG | X86_PARITY_FLAG /* get ones we are interested in */        \
_asm or byte ptr [R.AF.B.l], ah /* store remaining flags */                             \
}

// sll with flags
#define SLL_WITH_FLAGS_ASM(Register)                                                    \
{       \
_asm mov al, byte ptr [Register] /* get reg */                                                  \
_asm stc                                        /* set carry */                                                         \
_asm rcl al,1                           /* rotate it */                                         \
_asm lahf                                       /* get flags */                                         \
_asm mov byte ptr [Register], al        /* store reg */                                         \
_asm and ah,X86_CARRY_FLAG      /* get flags we are interested in */    \
_asm mov byte ptr [R.AF.B.l], ah        /* store flags - CARRY */                               \
_asm or al,al                           /* set sign, zero and parity */                 \
_asm lahf                                       /* get flags */                                                 \
_asm and ah, X86_SIGN_FLAG | X86_ZERO_FLAG | X86_PARITY_FLAG /* get ones we are interested in */        \
_asm or byte ptr [R.AF.B.l], ah /* store remaining flags */                             \
}

#define AND_A_X_ASM(Register)           \
{                                                                       \
_asm mov al, byte ptr [R.AF.B.h]        \
_asm and al, byte ptr [Register]        \
_asm lahf                                                       \
_asm mov byte ptr [R.AF.B.h], al        \
_asm and ah, X86_ZERO_FLAG | X86_SIGN_FLAG | X86_PARITY_FLAG    \
_asm or ah, Z80_HALFCARRY_FLAG          \
_asm mov byte ptr [R.AF.B.l], ah        \
}

#define XOR_A_X_ASM(Register)           \
{                                                                       \
_asm mov al, byte ptr [R.AF.B.h]        \
_asm xor al, byte ptr [Register]        \
_asm lahf                                                       \
_asm mov byte ptr [R.AF.B.h], al        \
_asm and ah, X86_ZERO_FLAG | X86_SIGN_FLAG | X86_PARITY_FLAG    \
_asm mov byte ptr [R.AF.B.l], ah        \
}

#define OR_A_X_ASM(Register)            \
{                                                                       \
_asm mov al, byte ptr [R.AF.B.h]        \
_asm or al, byte ptr [Register] \
_asm lahf                                                       \
_asm mov byte ptr [R.AF.B.h], al        \
_asm and ah, X86_ZERO_FLAG | X86_SIGN_FLAG | X86_PARITY_FLAG    \
_asm mov byte ptr [R.AF.B.l], ah        \
}

#define INC_X_ASM(Register)                                     \
{                       \
__asm mov cl,byte ptr [Register]                                           \
__asm inc cl	\
__asm lahf                                                                      \
__asm mov byte ptr [Register],cl		\
__asm and ah,X86_ZERO_FLAG | X86_SIGN_FLAG | X86_AUXILIARY_FLAG \
__asm mov byte ptr [R.AF.B.l], ah               \
}

#define DEC_X_ASM(Register)                                     \
{                       \
__asm dec byte ptr [Register]               \
__asm lahf                                                                 \
__asm and ah,X86_ZERO_FLAG | X86_SIGN_FLAG | X86_AUXILIARY_FLAG \
__asm mov byte ptr [R.AF.B.l], ah       \
}

#define ADD_A_X_ASM(Register)   \
{                                                                       \
__asm mov al,byte ptr [R.AF.B.h]        \
__asm add al,byte ptr [Register]        \
__asm lahf                                                      \
__asm mov byte ptr [R.AF.B.h], al       \
__asm and ah,X86_CARRY_FLAG | X86_ZERO_FLAG | X86_SIGN_FLAG | X86_AUXILIARY_FLAG \
__asm mov byte ptr [R.AF.B.l], ah       \
}

#define ADC_A_X_ASM(Register2)  \
{                                                                       \
__asm mov al, byte ptr [R.AF.B.l]       \
__asm ror al,1                                          \
__asm mov al,byte ptr [R.AF.B.h]        \
__asm adc al,byte ptr [Register2]       \
__asm lahf                                                      \
__asm mov byte ptr [R.AF.B.h], al       \
__asm and ah,X86_CARRY_FLAG | X86_ZERO_FLAG | X86_SIGN_FLAG | X86_AUXILIARY_FLAG \
__asm mov byte ptr [R.AF.B.l], ah       \
}


#define SUB_A_X_ASM(Register)   \
{                                                                       \
__asm mov al,byte ptr [R.AF.B.h]        \
__asm sub al,byte ptr [Register]        \
__asm lahf                                                      \
__asm mov byte ptr [R.AF.B.h], al       \
__asm and ah,X86_CARRY_FLAG | X86_ZERO_FLAG | X86_SIGN_FLAG | X86_AUXILIARY_FLAG \
__asm or ah, Z80_SUBTRACT_FLAG          \
__asm mov byte ptr [R.AF.B.l], ah       \
}


#define CP_A_X_ASM(Register)    \
{                                                                       \
__asm mov al,byte ptr [R.AF.B.h]        \
__asm sub al,byte ptr [Register]        \
__asm lahf                                                      \
__asm and ah,X86_CARRY_FLAG | X86_ZERO_FLAG | X86_SIGN_FLAG | X86_AUXILIARY_FLAG \
__asm or ah, Z80_SUBTRACT_FLAG          \
__asm mov byte ptr [R.AF.B.l], ah       \
}


#define SBC_A_X_ASM(Register2)  \
{                                                                       \
__asm mov al, byte ptr [R.AF.B.l]       \
__asm ror al,1                                          \
__asm mov al,byte ptr [R.AF.B.h]        \
__asm sbb al,byte ptr [Register2]       \
__asm lahf                                                      \
__asm mov byte ptr [R.AF.B.h], al       \
__asm and ah,X86_CARRY_FLAG | X86_ZERO_FLAG | X86_SIGN_FLAG | X86_AUXILIARY_FLAG \
__asm or ah, Z80_SUBTRACT_FLAG          \
__asm mov byte ptr [R.AF.B.l], ah       \
}

#define ADD_RR_rr_ASM(Register1, Register2)             \
{               \
__asm mov cx, word ptr [Register1]              \
__asm mov bx, word ptr [Register2]              \
__asm add cx,bx                                                 \
__asm lahf                                                              \
__asm mov word ptr [Register1],cx               \
__asm and ah, X86_CARRY_FLAG                    \
__asm and byte ptr [R.AF.B.l], (Z80_ZERO_FLAG | Z80_PARITY_FLAG | Z80_SIGN_FLAG)        \
__asm or byte ptr [R.AF.B.l],ah                 \
}
#define ADC_HL_rr_ASM(Register)         \
{               \
__asm mov al, byte ptr [R.AF.B.l]               \
__asm ror al, 1                                         \
__asm mov cx, word ptr [R.HL.W]         \
__asm mov bx, word ptr [Register]               \
__asm adc cx,bx                                                 \
__asm lahf                                                              \
__asm mov word ptr [R.HL.W],cx          \
__asm and byte ptr [R.AF.B.l], Z80_PARITY_FLAG  \
__asm and ah, X86_CARRY_FLAG | X86_ZERO_FLAG | X86_SIGN_FLAG                    \
__asm or byte ptr [R.AF.B.l],ah                 \
}

#define SBC_HL_rr_ASM(Register)         \
{               \
__asm mov al, byte ptr [R.AF.B.l]               \
__asm ror al, 1                                         \
__asm mov cx, word ptr [R.HL.W]         \
__asm mov bx, word ptr [Register]               \
__asm sbb cx,bx                                                 \
__asm lahf                                                              \
__asm mov word ptr [R.HL.W],cx          \
__asm and byte ptr [R.AF.B.l], Z80_PARITY_FLAG                          \
__asm and ah, X86_CARRY_FLAG | X86_ZERO_FLAG | X86_SIGN_FLAG                    \
__asm or ah, Z80_SUBTRACT_FLAG                          \
__asm or byte ptr [R.AF.B.l],ah                 \
}
