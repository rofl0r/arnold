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
#include "cpc.h"
#include "printer.h"
#include "host.h"


static CPC_PRINTER_HW printer;

void	Printer_Initialise(void)
{
	/* initialise bit 7 state for CPC */
	printer.CurrentBit7State = 0;
	printer.PreviousBit7State = printer.CurrentBit7State;

	printer.CurrentDataByte = 0;
	printer.PreviousDataByte = printer.CurrentDataByte;
}


/* following two functions are to support CPC+ full 8-bit
printer port. Bit 3 of CRTC register 12 holds the value
of data bit 7. Or these functions can also support a CPC
with a 8-bit printer port modification */

void	Printer_SetDataBit7State(int BitState)
{
	printer.PreviousBit7State = printer.CurrentBit7State;
	printer.CurrentBit7State = 0;
	if (BitState)
		printer.CurrentBit7State |=(1<<7);
}

int	Printer_GetDataBit7State(void)
{
	return (printer.CurrentBit7State>>7);
}

unsigned char Printer_GetDataByte(void)
{
	return printer.CurrentDataByte;
}

unsigned char Printer_Get8BitData(void)
{
	return ((printer.CurrentDataByte&0x07f)|printer.CurrentBit7State);
}

unsigned char Printer_GetPrevious8BitData(void)
{
	return ((printer.PreviousDataByte&0x07f)|printer.PreviousBit7State);
}

int		Printer_GetStrobeState(void)
{
	/* /strobe is inverted by the hardware */
	/* put state of strobe into bit 0 */
	return ((printer.CurrentDataByte^0x080)>>7);
}


int		Printer_GetPreviousStrobeState(void)
{
	/* /strobe is inverted by the hardware */
	/* put state of strobe into bit 0 */
	return ((printer.PreviousDataByte^0x080)>>7);
}

void	Printer_SetBusyInput(int BusyState)
{
	/* to be implemented */
}

/* write a byte of data to printer port 0x0efxx */
void	Printer_WriteDataByte(int DataByte)
{
	printer.PreviousDataByte = printer.CurrentDataByte;
	printer.CurrentDataByte = DataByte;

	Host_HandlePrinterOutput();
}

void	Printer_Finish(void)
{
}

			
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

