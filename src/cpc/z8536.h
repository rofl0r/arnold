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
#ifndef __Z8536_HEADER_INCLUDED__
#define __Z8536_HEADER_INCLUDED__


enum
{
	Z8536_RESET_STATE,
	Z8536_STATE_0,
	Z8536_STATE_1
} Z8536_STATES;

typedef struct Z8536
{
	unsigned char State;
	unsigned char PointerRegister;
	unsigned char Registers[64];
	/* outputs as seen by hardware connected to z8536 */
	unsigned char final_output[3];

	/* previous input data provided by hardware connected to z8536 */
	unsigned char previous_inputs[3];
	/* input data provided by hardware connected to z8536 */
	unsigned char inputs[3];

	/* the counters can be linked. This allows them to be linked together */
	/* inputs to each counter */
	unsigned char counter_inputs[3];
	/* previous inputs to counter */
	unsigned char counter_previous_inputs[3];
	/* counter outputs */
	unsigned char counter_outputs[3];

} Z8536;	

enum
{
	Z8536_REGISTER_MASTER_INTERRUPT_CONTROL = 0,
	Z8536_REGISTER_CONFIGURATION_CONTROL,
	Z8536_REGISTER_PORT_A_INTERRUPT_VECTOR,
	Z8536_REGISTER_PORT_B_INTERRUPT_VECTOR,
	Z8536_REGISTER_COUNTER_TIMER_INTERRUPT_VECTOR,
	Z8536_REGISTER_PORT_C_DATA_PATH_POLARITY,
	Z8536_REGISTER_PORT_C_DATA_DIRECTION,
	Z8536_REGISTER_PORT_C_SPECIAL_IO_CONTROL,
	Z8536_REGISTER_PORT_A_COMMAND_AND_STATUS,
	Z8536_REGISTER_PORT_B_COMMAND_AND_STATUS,
	Z8536_REGISTER_COUNTER_TIMER_1_COMMAND_AND_STATUS,
	Z8536_REGISTER_COUNTER_TIMER_2_COMMAND_AND_STATUS,
	Z8536_REGISTER_COUNTER_TIMER_3_COMMAND_AND_STATUS,
	Z8536_REGISTER_PORT_A_DATA,
	Z8536_REGISTER_PORT_B_DATA,
	Z8536_REGISTER_PORT_C_DATA,
	Z8536_REGISTER_COUNTER_TIMER_1_CURRENT_COUNT_MSB,
	Z8536_REGISTER_COUNTER_TIMER_1_CURRENT_COUNT_LSB,
	Z8536_REGISTER_COUNTER_TIMER_2_CURRENT_COUNT_MSB,
	Z8536_REGISTER_COUNTER_TIMER_2_CURRENT_COUNT_LSB,
	Z8536_REGISTER_COUNTER_TIMER_3_CURRENT_COUNT_MSB,
	Z8536_REGISTER_COUNTER_TIMER_3_CURRENT_COUNT_LSB,
	Z8536_REGISTER_COUNTER_TIMER_1_TIME_CONSTANT_MSB,
	Z8536_REGISTER_COUNTER_TIMER_1_TIME_CONSTANT_LSB,
	Z8536_REGISTER_COUNTER_TIMER_2_TIME_CONSTANT_MSB,
	Z8536_REGISTER_COUNTER_TIMER_2_TIME_CONSTANT_LSB,
	Z8536_REGISTER_COUNTER_TIMER_3_TIME_CONSTANT_MSB,
	Z8536_REGISTER_COUNTER_TIMER_3_TIME_CONSTANT_LSB,
	Z8536_REGISTER_COUNTER_TIMER_1_MODE_SPECIFICATION,
	Z8536_REGISTER_COUNTER_TIMER_2_MODE_SPECIFICATION,
	Z8536_REGISTER_COUNTER_TIMER_3_MODE_SPECIFICATION,
	Z8536_REGISTER_CURRENT_VECTOR,
	Z8536_REGISTER_PORT_A_MODE_SPECIFICATION,
	Z8536_REGISTER_PORT_A_HANDSHAKE_SPECIFICATION,
	Z8536_REGISTER_PORT_A_DATA_PATH_POLARITY,
	Z8536_REGISTER_PORT_A_DATA_DIRECTION,
	Z8536_REGISTER_PORT_A_SPECIAL_IO_CONTROL,
	Z8536_REGISTER_PORT_A_PATTERN_POLARITY,
	Z8536_REGISTER_PORT_A_PATTERN_TRANSITION,
	Z8536_REGISTER_PORT_A_PATTERN_MASK,
	Z8536_REGISTER_PORT_B_MODE_SPECIFICATION,
	Z8536_REGISTER_PORT_B_HANDSHAKE_SPECIFICATION,
	Z8536_REGISTER_PORT_B_DATA_PATH_POLARITY,
	Z8536_REGISTER_PORT_B_DATA_DIRECTION,
	Z8536_REGISTER_PORT_B_SPECIAL_IO_CONTROL,
	Z8536_REGISTER_PORT_B_PATTERN_POLARITY,
	Z8536_REGISTER_PORT_B_PATTERN_TRANSITION,
	Z8536_REGISTER_PORT_B_PATTERN_MASK,
} Z8536_REGISTER_IDS;


void Z8536_WriteData(unsigned char Address, unsigned char Data);
unsigned char Z8536_ReadData(unsigned char Address);
void	Z8536_Update(void);

#endif

