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

int    Z80_ExecuteInterrupt()
{
	int Cycles;

	Cycles = 0;
    if (R.IFF1)
    {
        R.IFF1 = 0;
        R.IFF2 = 0;

         if (R.Flags & Z80_EXECUTING_HALT_FLAG)	
         {
              ADD_PC(1);
         }

		 R.Flags &=~Z80_EXECUTING_HALT_FLAG;

		 Z80_AcknowledgeInterrupt();
		 
        switch (R.IM)
        {
            case 0x00:
			{
				Cycles = Z80_ExecuteIM0();
			}
			break;

            case 0x01:
            {
                    /* The number of cycles required to complete the instruction
                    is two more than normal due to the two added wait states */

					Cycles = 5;	/*Z80_UpdateCycles(5); */

					/* push return address onto stack */
					PUSH(R.PC.W.l);

					/* set program counter address */
					R.PC.W.l = 0x0038;
            }
			break;
    
			case 0x02:
            {
                    Z80_WORD            Vector;
                    Z80_WORD            Address;

                    /* 19 clock cycles for this mode. 8 for vector, six for program counter, six to obtain jump address */
                    Cycles = 7;	/*Z80_UpdateCycles(7); */

                    PUSH(R.PC.W.l);

                    Vector = (R.I<<8) | (R.InterruptVectorBase);

                    Address = Z80_RD_WORD(Vector);

                    R.PC.W.l = Address;
            }
            break;
		}
    }
	return Cycles;
}



/*
 A value has even parity when all the binary digits added together give an even
 number. (result = 0). A value has an odd parity when all the digits added
 together give an odd number. (result = 1)
*/

static void    Z80_BuildParityTable(void)
{
        int     i,j;
        int     sum;

        for (i=0; i<256; i++)
        {
                Z80_BYTE        data;
                
                sum = 0;                                /* will hold sum of all bits */

                data = i;                               /* data byte to find sum of */

                for (j=0; j<8; j++)
                {
                        sum+=data & 0x01;       /* isolate bit and add */
                        data=data>>1;           /* shift for next bit */
                }

                /* in flags register, if result has even parity, then
                 bit is set to 1, if result has odd parity, then bit
                 is set to 0.
				*/

                /* check bit 0 of sum. If 1, then odd parity, else even parity. */
                if ((sum & 0x01)!=0)    
                {
                        /* odd parity */
                        ParityTable[i] = 0;
                }
                else
                {       
                        /* even parity */
                        ParityTable[i] = Z80_PARITY_FLAG;
                }

        }

        for (i=0; i<256; i++)
        {
                ZeroSignTable[i] = 0;
                        
                if ((i & 0x0ff)==0)
                {
                        ZeroSignTable[i] |= Z80_ZERO_FLAG;
                }

                if (i & 0x080)
                {
                        ZeroSignTable[i] |= Z80_SIGN_FLAG;
                }
        }

        for (i=0; i<256; i++)
        {
			unsigned char Data;

            Data = 0;
			            
                if ((i & 0x0ff)==0)
                {
                        Data |= Z80_ZERO_FLAG;
                }

                if (i & 0x080)
                {
                        Data |= Z80_SIGN_FLAG;
                }

				Data |= (i & (Z80_UNUSED_FLAG1 | Z80_UNUSED_FLAG2));
			
				ZeroSignTable2[i] = Data; 

        }

        for (i=0; i<256; i++)
        {
                ZeroSignParityTable[i] = 0;

                if ((i & 0x0ff)==0)
                {
                        ZeroSignParityTable[i] |= Z80_ZERO_FLAG;
                }

                if ((i & 0x080)==0x080)
                {
                        ZeroSignParityTable[i] |= Z80_SIGN_FLAG;
                }

				/* included unused flag1 and 2 for AND,XOR,OR, IN, RLD, RRD */
                ZeroSignParityTable[i] |= ParityTable[i] | (i & Z80_UNUSED_FLAG1) | (i & Z80_UNUSED_FLAG2);
        }
}

void    Z80_Init()
{
        Z80_BuildParityTable();
}



void    Z80_Reset(void)
{
        R.PC.L=0;
        R.I=0;
        R.IM=0;
        R.IFF1=0;
        R.IFF2=0;
        R.RBit7=0;
        R.R = 0;
        R.Flags &=~
			(Z80_EXECUTING_HALT_FLAG | 
			Z80_CHECK_INTERRUPT_FLAG | 
			Z80_EXECUTE_INTERRUPT_HANDLER_FLAG |
			Z80_INTERRUPT_FLAG);
}

void    Z80_NMI(void)
{
        /* disable maskable ints */
        R.IFF1 = 0;

        /* push return address on stack */
        PUSH(R.PC.W.l);

        /* set program counter address */
        R.PC.W.l = 0x0066;
}
/*
Z80_REGISTERS   *Z80_GetReg(void)
{
        return &R;
}
*/

int		Z80_GetReg(int RegID)
{
	switch (RegID)
	{
		case Z80_PC:
			return R.PC.W.l;
		case Z80_HL:
			return R.HL.W;
		case Z80_DE:
			return R.DE.W;
		case Z80_BC:
			return R.BC.W;
		case Z80_AF:
			return R.AF.W;
		case Z80_SP:
			return R.SP.W;
		case Z80_IX:
			return R.IX.W;
		case Z80_IY:
			return R.IY.W;
		case Z80_IM:
			return R.IM;
		case Z80_IFF1:
			return R.IFF1;
		case Z80_IFF2:
			return R.IFF2;
		case Z80_I:
			return R.I;
		case Z80_R:
			return Z80_GET_R;
		case Z80_AF2:
			return R.altAF.W;
		case Z80_BC2:
			return R.altBC.W;
		case Z80_DE2:
			return R.altDE.W;
		case Z80_HL2:
			return R.altHL.W;
		default:
			break;
	}

	return 0x0;
}


void		Z80_SetReg(int RegID, int Value)
{
	switch (RegID)
	{
		case Z80_PC:
			R.PC.W.l = Value;
			return;

		case Z80_HL:
			R.HL.W = Value;
			return;

		case Z80_DE:
			R.DE.W = Value;
			return;

		case Z80_BC:
			R.BC.W = Value;
			return;

		case Z80_AF:
			R.AF.W = Value;
			return;

		case Z80_SP:
			R.SP.W = Value;
			return;

		case Z80_IX:
			R.IX.W = Value;
			return;

		case Z80_IY:
			R.IY.W = Value;
			return;

		case Z80_IM:
			R.IM = Value;
			return;

		case Z80_IFF1:
			R.IFF1 = Value;
			return;

		case Z80_IFF2:
			R.IFF2 = Value;
			return;

		case Z80_I:
			R.I = Value;
			return;

		case Z80_R:
			R.R = Value;
			R.RBit7 = Value & 0x080;
			return;

		case Z80_AF2:
			R.altAF.W = Value;
			return;

		case Z80_BC2:
			R.altBC.W = Value;
			return;

		case Z80_DE2:
			R.altDE.W = Value;
			return;

		case Z80_HL2:
			R.altHL.W = Value;
			return;

		default:
			break;
	}
}


void    Z80_SetInterruptVector(int Base)
{
        R.InterruptVectorBase = Base & 0x0ff;
}

void    Z80_SetInterruptRequest(void)
{
        R.Flags |=Z80_INTERRUPT_FLAG;
		R.Flags |=Z80_EXECUTE_INTERRUPT_HANDLER_FLAG;
}

void    Z80_ClearInterruptRequest(void)
{
        R.Flags &=~Z80_INTERRUPT_FLAG;
		R.Flags &=~Z80_EXECUTE_INTERRUPT_HANDLER_FLAG;
}

BOOL	Z80_GetInterruptRequest(void)
{
	if (R.Flags & Z80_INTERRUPT_FLAG)
		return TRUE;
	
	return FALSE;
}



