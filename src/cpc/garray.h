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
#ifndef __GATE_ARRAY_HEADER_INCLUDED__
#define __GATE_ARRAY_HEADER_INCLUDED__

/* reset gate array */
void    GateArray_Reset(void);

/* write data to gate array */
void    GateArray_Write(int);

/* re-setup memory pointers */
void    GateArray_RethinkMemory(void);

typedef enum 
{
        MONITOR_MODE_COLOUR = 0,                        /* colour = CTM640/CTM644/CM14 */
        MONITOR_MODE_GREEN_SCREEN,              /* green-screen = GT60/GT64 */
        MONITOR_MODE_GREY_SCALE /* black and white = MM14 */
} MONITOR_COLOUR_MODE;

void    GateArray_SetMonitorColourMode(MONITOR_COLOUR_MODE Mode);

void    GateArray_Initialise(void);

int             GateArray_GetRed(int);
int             GateArray_GetGreen(int);
int             GateArray_GetBlue(int);

/* used for Snapshot and Multiface */
int             GateArray_GetPaletteColour(int PenIndex);
int             GateArray_GetSelectedPen(void);
int             GateArray_GetRamConfiguration(void);
int             GateArray_GetMultiConfiguration(void);
void            GateArray_RethinkMemory(void);
void            GateArray_SetRethinkMemoryCallback(void (*)(void));
void			CPC_GateArray_RethinkMemory(void);
void			CPC_GateArray_Write(int);
void			GateArray_SetWriteCallback(void (*)(int));

#define CRTC_CLOCK_FREQUENCY 1000000
#define RAM_CLOCK 1000000
#define GATE_ARRAY_FREQUENCY 16000000
#define GATE_ARRAY_INT_PULSE_LENGTH_IN_NOPS 5
/* wait every 4th clock. */

typedef struct GATE_ARRAY_STATE
{
	unsigned long	InterruptLineCount;
	unsigned long	InterruptSyncCount;

	unsigned char   PenSelection;
	unsigned char   ColourSelection;
	unsigned char   RamConfiguration;
	unsigned char   RomConfiguration;

	unsigned char   PenColour[18];

	unsigned char	PenIndex;
	unsigned char	pad0;
	unsigned char	pad1;

	unsigned char **pChosenRamConfig;

	/* these are the addressess for ram r/w i.e. when rom is not paged in */
	unsigned char	*RamConfigRead[8];
	unsigned char	*RamConfigWrite[8];

	/* these are the addressess when rom is active */
	unsigned char	*RomConfigRead[8];
	unsigned char	*RomConfigWrite[8];

} GATE_ARRAY_STATE;


void	GateArray_SetInterruptLineCount(int Count);
int		GateArray_GetInterruptLineCount(void);

void    GateArray_AcknowledgeInterrupt(void);
void	GateArray_UpdateColours(void);
void	GateArray_Update();
void	GateArray_TriggerVsyncSynchronisation(void);


int	GateArray_GetVsyncSynchronisationCount(void);
void	GateArray_SetVsyncSynchronisationCount(int Count);

#endif
