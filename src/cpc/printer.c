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
#include "audioevent.h"
#include "host.h"

static HOST_FILE_HANDLE fhPrinterOutput = 0;

static int Bit7State = 0;
static int PrinterOutputMethod = PRINTER_OUTPUT_DO_NOTHING;
static unsigned char Digiblaster_Data=0x07f;

void	Printer_Initialise(void)
{
	/* initialise bit 7 state for CPC */
	Bit7State = 0;
	/* set initial printer output method */
	PrinterOutputMethod = PRINTER_OUTPUT_DO_NOTHING;
	/* initialise file handle for printing to file */
	fhPrinterOutput = 0;
}


/* following two functions are to support CPC+ full 8-bit
printer port. Bit 3 of CRTC register 12 holds the value
of data bit 7. Or these functions can also support a CPC
with a 8-bit printer port modification */

void	Printer_SetDataBit7State(int BitState)
{
	Bit7State = (BitState & 1)<<7;
}

int	Printer_GetDataBit7State(void)
{
	return (Bit7State>>7);
}

static void	Printer_CloseOutputFile(void)
{
	if (fhPrinterOutput!=0)
	{

		Host_CloseFile(fhPrinterOutput);
		fhPrinterOutput = 0;
	}
}

static void	Printer_OpenOutputFile(void)
{
	fhPrinterOutput = Host_OpenFile("printer.txt", HOST_FILE_ACCESS_WRITE);
}

static void	Printer_WriteToOutputFile(unsigned char DataByte)
{
	if (fhPrinterOutput!=0)
		Host_WriteData(fhPrinterOutput, &DataByte, sizeof(unsigned char));
}

int		Printer_GetOutputMethod(void)
{
	return PrinterOutputMethod;
}

void	Printer_SetOutputMethod(int Method)
{
	/* if the old printer output method is to a file, close
	the output file */
	if (PrinterOutputMethod == PRINTER_OUTPUT_TO_FILE)
	{
		Printer_CloseOutputFile();
	}

	/* select new output method */
	switch (Method)
	{
		case PRINTER_OUTPUT_TO_FILE:
		{
			Printer_OpenOutputFile();
		}
		break;

		case PRINTER_OUTPUT_DO_NOTHING:
			break;
	}

	PrinterOutputMethod = Method;
}

static unsigned char Printer_DataByte;

unsigned char Printer_GetDataByte(void)
{
	return Printer_DataByte;
}


/* write a byte of data to printer port 0x0efxx */
void	Printer_WriteDataByte(int DataByte)
{
	Printer_DataByte = DataByte;

	switch (PrinterOutputMethod)
	{
		case PRINTER_OUTPUT_TO_FILE:
		{
			int Strobe;
			unsigned char Data;

			Strobe = DataByte & 0x080;
			Data = (unsigned char)((DataByte & 0x07f) | Bit7State);

			if (Strobe!=0)
			{
				/* write databyte to printer text output file */
				Printer_WriteToOutputFile(Data);
			}
		}
		break;

		case PRINTER_OUTPUT_TO_PRINTER:
		{
			int Strobe;
			int Data;

			Strobe = DataByte & 0x080;
			Data = (DataByte & 0x07f) | Bit7State;
		}
		break;

		case PRINTER_OUTPUT_TO_DIGIBLASTER:
		{
			unsigned char DigiData;

			/* 8-th bit is sent with bit 7 inverted, because
			the CPC h/w automatically inverts it */
			DigiData = (unsigned char)(DataByte ^ 0x080);
		
			/* output from printer port after h/w has inverted it appears to be a unsigned 8-bit 
			number */

		
			Audio_Digiblaster_Write(DigiData);

		}
		break;

		case PRINTER_OUTPUT_DO_NOTHING:
			break;
	}


	Digiblaster_Data = (unsigned char)DataByte;
}

void	Printer_Finish(void)
{
	if (PrinterOutputMethod == PRINTER_OUTPUT_TO_FILE)
	{
		Printer_CloseOutputFile();
	}
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

