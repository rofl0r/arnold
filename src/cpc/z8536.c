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
/* Z8536 emulation */

#include "z8536.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
			
/* test transfer sequence 

  KCC Side:

	check inputs:
	/STROBE on expansion port = 1,
	DATA1 on expansion port = 1,
	->continue
	check inputs:
	/STROBE on expansion port = 1,
	DATA1 on expansion port = 0,
	->continue

	write 0x0f, wait:
	DATA1 = 1, DATA2 = 1, DATA7 = 0, /STROBE = 0
	
	repeat for 8 bits

		repeat
		
			write 0x0ff, wait:
			DATA1 = 1, DATA2 = 1, DATA7 = 1, /STROBE = 1

			check inputs:
			/STROBE on expansion port = 1
				-> wait... and continue
			/STROBE on expansion port = 0
				-> repeat loop
		end repeat
		
		 check inputs:
		/STROBE on expansion port = 0 
			->write 0x0ff, wait etc...

		/STROBE on expansion port = 1
			->DATA1 into carry. this is the bit of data.

	end repeat

	write 0x0f0, wait:
		DATA1 = 0, DATA2= 0, DATA7 = 1, /STROBE = 1
	check inputs:
			/STROBE on expansion port = 1
				-> wait... and continue
			/STROBE on expansion port = 0
				-> repeat loop
*/




Z8536 z8536;

void Z8536_WriteData(unsigned char Address, unsigned char Data)
{
	switch (Address)
	{
		case 0:
		{
			/* Port C Data register */
			z8536.Registers[Z8536_REGISTER_PORT_C_DATA] = Data;
		}
		break;

		case 1:
		{
			/* Port B Data register */
			z8536.Registers[Z8536_REGISTER_PORT_B_DATA] = Data;
		}
		break;

		case 2:
		{
			/* Port A Data register */
			z8536.Registers[Z8536_REGISTER_PORT_A_DATA] = Data;

			/* update port a output */
			z8536.final_output[0] = 
				(z8536.Registers[Z8536_REGISTER_PORT_A_DATA] ^ 
				z8536.Registers[Z8536_REGISTER_PORT_A_DATA_PATH_POLARITY]);

		}
		break;


		case 3:
		{
			/* Control registers */

			switch (z8536.State)
			{
				case Z8536_RESET_STATE:
				{
					if ((Data & 0x01)==0)
					{
						z8536.State = Z8536_STATE_0;
					}
				}
				break;

				case Z8536_STATE_0:
				{
					z8536.State = Z8536_STATE_1;
					z8536.PointerRegister = (Data & 0x03f);
				}
				break;

				case Z8536_STATE_1:
				{
					/* write data to internal registers */
					z8536.Registers[z8536.PointerRegister] = Data;

					/* write to "Master Interrupt Control Register" with Reset bit set? */
					if ((z8536.PointerRegister==0) && ((Data & 0x01)!=0))
					{
						/* go to reset state */
						z8536.State = Z8536_RESET_STATE;
				
						/* reset registers */
						memset(z8536.Registers, 0, 64);
					}
					else
					{
						/* go back to state 0 */
						z8536.State = Z8536_STATE_0;

					}
				}
				break;

				default:
					break;
			}


		}
		break;

		default:
			break;
	}
}

unsigned char Z8536_ReadData(unsigned char Address)
{
	switch (Address)
	{
		case 0:
			return z8536.Registers[Z8536_REGISTER_PORT_C_DATA];
		
		case 1:
			return z8536.Registers[Z8536_REGISTER_PORT_B_DATA];
		
		case 2:
			return z8536.Registers[Z8536_REGISTER_PORT_A_DATA];
		
		case 3:
		{
			switch (z8536.State)
			{

				case Z8536_RESET_STATE:
				{
					return 0x01;
				}

				case Z8536_STATE_0:
				case Z8536_STATE_1:
				{
					z8536.State = Z8536_STATE_0;
					return z8536.Registers[z8536.PointerRegister];
				}
			}
		}
		break;

		default:
			break;
	}

	return 0x0ff;
}


/* 
	Z8536 OPERATION IN KC COMPACT:

	Port C - Timer - Handles interrupts

	PC0 = Counter/Timer Output = AIC
	PC1 = Counter Input = CRTC HSYNC
	PC2 = Trigger Input = VSYV
	PC3 = Gate Input = 5V


  VSYV is TRUE if a HSYNC ended while VSYNC was active.


  */


static void Z8536_UpdateCounter(unsigned char CounterID)
{
	unsigned char counter_inputs_changed;

	/* get changed inputs */
	counter_inputs_changed = (z8536.counter_inputs[CounterID]^z8536.counter_previous_inputs[CounterID]) & 0x0f;

	/* store inputs */
	z8536.counter_previous_inputs[CounterID] = z8536.counter_inputs[CounterID];


	/* inputs have changed? */
	if (counter_inputs_changed==0)
		return;

	/* yes */

	/* counter trigger input changed? */
	if (counter_inputs_changed & 0x04)
	{
		/* rising edge? */
		if ((z8536.counter_inputs[CounterID] & 0x04)!=0)
		{
			/* counter trigger rising edge caught */
			if ((z8536.Registers[Z8536_REGISTER_COUNTER_TIMER_1_MODE_SPECIFICATION + CounterID] & 0x04)!=0)
			{
				/* retrigger enable bit */

				/* reload current count from stored time constant */
				z8536.Registers[Z8536_REGISTER_COUNTER_TIMER_1_CURRENT_COUNT_MSB + (CounterID<<1)] = 
					z8536.Registers[Z8536_REGISTER_COUNTER_TIMER_1_TIME_CONSTANT_MSB + (CounterID<<1)];

				z8536.Registers[Z8536_REGISTER_COUNTER_TIMER_1_CURRENT_COUNT_LSB + (CounterID<<1)] = 
					z8536.Registers[Z8536_REGISTER_COUNTER_TIMER_1_TIME_CONSTANT_LSB + (CounterID<<1)];

				/* pulse? */
				if ((z8536.Registers[Z8536_REGISTER_COUNTER_TIMER_1_MODE_SPECIFICATION + CounterID] & 0x03)==0)
				{
					/* set counter output */
					z8536.counter_outputs[2]|=0x01;
				}


				return;
			}
		}
	}

	/* counter input changed? */
	if (counter_inputs_changed & 0x02)
	{
		/* rising edge? */
		if ((z8536.counter_inputs[CounterID] & 0x02)!=0)
		{

			/* counter input changed */
			unsigned int CountLow,CountHigh;
			unsigned int CurrentCount;

			/* pulse? */
			if ((z8536.Registers[Z8536_REGISTER_COUNTER_TIMER_1_MODE_SPECIFICATION + CounterID] & 0x03)==0)
			{
				z8536.counter_outputs[2]&=~0x01;
			}

			/* counter high */
			CountHigh = z8536.Registers[Z8536_REGISTER_COUNTER_TIMER_1_CURRENT_COUNT_MSB + (CounterID<<1)];
			/* counter low */
			CountLow = z8536.Registers[Z8536_REGISTER_COUNTER_TIMER_1_CURRENT_COUNT_LSB + (CounterID<<1)];

			CurrentCount = (CountHigh<<8) | (CountLow);

			/* decrement count */
			CurrentCount--;

			/* count finished? */
			if (CurrentCount==0)
			{
				/* pulse? */
				if ((z8536.Registers[Z8536_REGISTER_COUNTER_TIMER_1_MODE_SPECIFICATION + CounterID] & 0x03)==0)
				{
					/* set counter output */
					z8536.counter_outputs[2]|=0x01;
				}

				if (z8536.Registers[Z8536_REGISTER_COUNTER_TIMER_1_MODE_SPECIFICATION + CounterID] & 0x080)
				{
					/* continuous */

					/* reload current count from stored time constant */
					z8536.Registers[Z8536_REGISTER_COUNTER_TIMER_1_CURRENT_COUNT_MSB + (CounterID<<1)] = 
						z8536.Registers[Z8536_REGISTER_COUNTER_TIMER_1_TIME_CONSTANT_MSB + (CounterID<<1)];

					z8536.Registers[Z8536_REGISTER_COUNTER_TIMER_1_CURRENT_COUNT_LSB + (CounterID<<1)] = 
						z8536.Registers[Z8536_REGISTER_COUNTER_TIMER_1_TIME_CONSTANT_LSB + (CounterID<<1)];
				}
			}
			else
			{
				z8536.Registers[Z8536_REGISTER_COUNTER_TIMER_1_CURRENT_COUNT_MSB + (CounterID<<1)] = CurrentCount>>8;
				z8536.Registers[Z8536_REGISTER_COUNTER_TIMER_1_CURRENT_COUNT_LSB + (CounterID<<1)] = CurrentCount;
			}
		}
	}
}



void	Z8536_Update(void)
{
	z8536.counter_inputs[2] = z8536.inputs[2]^z8536.Registers[Z8536_REGISTER_PORT_C_DATA_PATH_POLARITY];

	Z8536_UpdateCounter(2);

}



