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
#ifndef __AUDIO_EVENT_HEADER_INCLUDED__
#define __AUDIO_EVENT_HEADER_INCLUDED__

#define AUDIO_EVENT_PSG			0x0001
#define AUDIO_EVENT_DIGIBLASTER	0x0002
#define AUDIO_EVENT_TAPE		0x0004

typedef struct 
{
	unsigned long PSG_Register;
	unsigned long PSG_RegisterData;
} PSG_EVENT;

typedef struct 
{
	unsigned long Volume;
	unsigned long pad0;
} DIGIBLASTER_EVENT;

typedef struct 
{
	unsigned long Volume;
	unsigned long pad0;
} TAPE_EVENT;


typedef struct 
{
	unsigned long NopCount;
	unsigned long Type;

	union
	{
		PSG_EVENT	PSG_Event;
		DIGIBLASTER_EVENT	Digiblaster_Event;
		TAPE_EVENT	Tape_Event;
	} AudioEvent;

} AUDIO_EVENT;

void	AudioEvent_RestartEventBuffer(int);
void	AudioEvent_AddEventToBuffer(unsigned long, unsigned long, unsigned long);
void	AudioEvent_Initialise(void);
void	AudioEvent_Finish(void);

//void	AudioEvent_TraverseAudioEventsDebug(DEBUG_HANDLE DebugHandle);
int		AudioEvent_TraverseAudioEventsAndBuildSampleData(int,int);
void	AudioEvent_SetFormat(int SampleRate, int SampleBits, int SampleChannels);

void	Audio_Reset(void);

void	Digiblaster_Init();
/* write to digiblaster */
void	Audio_Digiblaster_Write(unsigned char);
/* update frame with data */
void	Digiblaster_EndFrame(void);
#endif
