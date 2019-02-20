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
#include "arnold.h"
#include "cpc.h"
#include "cpcdefs.h"
#include "audioevent.h"
#include "host.h"
#include "debugmain.h"
#include "z80/z80.h"
#include "config.h"

#define FRAME_SKIP_MIN	0
#define FRAME_SKIP_MAX	10

#include "debugger/gdebug.h"


#ifdef SHOW_TIME

/* the following are for timing the emulation */
#define FRAME_TIME_IN_MS	(1000/50)
	
#define NUM_FRAMES_TO_TIME	10
static unsigned long FrameTimesInMs[NUM_FRAMES_TO_TIME];
static unsigned long FrameIndexForFrameTimes = 0;
static unsigned long Arnold_PreviousTimeInMs = 0;
static unsigned int Arnold_PercentRelativeSpeed = 0;
#endif

/* true if not to render anything to screen buffer, FALSE to render */
static BOOL DontRender=FALSE;
/* current frame index, compared against frame skip value */
static int CurrentFrameIndex = 0;
/* frame skip value */
static int FrameSkip = FRAME_SKIP_MIN;
/* number of nops executed in this frame so far */
static int NopCountToDate = NOPS_PER_MONITOR_SCREEN;
/* TRUE if audio is enabled, FALSE otherwise */
static BOOL AudioActiveFlag = FALSE;
/* TRUE if debugger is enabled, FALSE otherwise */
static BOOL DebuggerIsActive = FALSE;

#if 0
void	CPC_DoFrameFunc(void)
{


#ifdef AY_OUTPUT
	/* if enabled, writes PSG registers to temp file */
	YMOutput_WriteRegs();
#endif

	/* Frame Skip Control code */

	/* update frame skip */
	CurrentFrameIndex++;

	if (CurrentFrameIndex==FrameSkip+1)
	{
		CurrentFrameIndex = 0;
		
		/* is rendering possible? */
		if (Render_IsRendererActive())
		{
			/* dump whole display to screen */
			Render_DumpDisplay();
		}
	}

	/* call speed throttle function - use this to 
	throttle speed at 100% */
	Host_Throttle();

	FDD_UpdateLEDState();

/*	Host_ProcessSystemEvents(); */


	/* dont render */
	DontRender = TRUE;
	CRTC_SetRenderState(DontRender);

	/* should we render graphics? */
	if (CurrentFrameIndex == FrameSkip)
	{
		/* is rendering possible? */
		if (Render_IsRendererActive())
		{
			/* allow it to render */
			DontRender = FALSE;
			CRTC_SetRenderState(DontRender);
		}
	}

#ifdef SHOW_TIME
	/* works out the speed of the emulation based on the previous 10 
	frames. This method stores a frame time in ms for each frame in
	a table, when 10 frame times have been recorded, it works out
	the average of those times and updates the percentage speed. */
	{
		unsigned long CurrentTimeInMs;
		unsigned long MsPassed;
		int first = Arnold_PreviousTimeInMs;

		/* get time for this frame in ms */
		CurrentTimeInMs = Host_GetCurrentTimeInMilliseconds();

		/* work out ms passed based on previous ms value */
		MsPassed = CurrentTimeInMs - Arnold_PreviousTimeInMs;
		
		/* store this time */
		Arnold_PreviousTimeInMs = CurrentTimeInMs;
		
		if (!first)
		{
			/* store this frame time */
			FrameTimesInMs[FrameIndexForFrameTimes] = MsPassed;
			FrameIndexForFrameTimes++;

			if (FrameIndexForFrameTimes == NUM_FRAMES_TO_TIME)
			{
				int i;
				unsigned long AverageTime = 0;

				/* recalc relative speed */
				FrameIndexForFrameTimes = 0;

				for (i=0; i<NUM_FRAMES_TO_TIME; i++)
				{
					/* add on time for this frame */
					AverageTime+=FrameTimesInMs[i];
				}

				AverageTime = AverageTime/NUM_FRAMES_TO_TIME;

				Arnold_PercentRelativeSpeed = (FRAME_TIME_IN_MS*100)/AverageTime;
			}
		}
	}
#endif

}
#endif

#ifdef SHOW_TIME
unsigned long CPCEmulation_GetPercentRelativeSpeed(void)
{
	return Arnold_PercentRelativeSpeed;
}
#endif


/* set frame skip for emulation */
void	CPC_SetFrameSkip(int FrameSkipCount)
{
	if (FrameSkipCount<FRAME_SKIP_MIN)
		FrameSkipCount = FRAME_SKIP_MIN;

	if (FrameSkipCount>FRAME_SKIP_MAX)
		FrameSkipCount = FRAME_SKIP_MAX;

	FrameSkip = FrameSkipCount;

	CurrentFrameIndex = 0;
	DontRender = FALSE;

}

int CPC_GetFrameSkip(void)
{
	return FrameSkip;
}

BOOL	CPC_IsAudioActive(void)
{
	return AudioActiveFlag;
}

void	CPC_SetAudioActive(BOOL State)
{
	if ((AudioActiveFlag==FALSE) && (State==TRUE))
	{
		/* setup for audio playback */

		/* is host system able to playback sound? */
		if (Host_AudioPlaybackPossible())
		{
			/* yes it is */
			SOUND_PLAYBACK_FORMAT *pSoundPlaybackFormat;

			/* get playback format */
			pSoundPlaybackFormat = Host_GetSoundPlaybackFormat();

			if (pSoundPlaybackFormat!=NULL)
			{
				/* audio was previously disabled, but we want it to be enabled */
				AudioEvent_SetFormat(pSoundPlaybackFormat->Frequency, pSoundPlaybackFormat->BitsPerSample, pSoundPlaybackFormat->NumberOfChannels);
	
				/* playing audio */
				AudioActiveFlag = TRUE;
			}
		}
		else
		{
			AudioActiveFlag = FALSE;
		}

	}

	if ((AudioActiveFlag==TRUE) && (State==FALSE))
	{
		/* audio was previously enabled, but we want it to be disabled */

		/* stop audio playback */
	}
}

/* this is called after every Z80 opcode executed */
/*void Z80_OpcodeFunc(int NopCount)
{
}
*/
/* initialise default CPC Emulation settings */
void	CPCEmulation_InitialiseDefaultSetup(void)
{
	/* set initial CRTC type */
	CPC_SetCRTCType(0);

	/* set initial CPC type */
	CPC_SetCPCType(CPC_TYPE_CPC6128);

	/* set initial printer output method */
	Printer_SetOutputMethod(PRINTER_OUTPUT_TO_DIGIBLASTER);

	/* set monitor to colour */
    CPC_SetMonitorType(CPC_MONITOR_COLOUR);

	/* set brightness to max */
/*   CPC_SetMonitorBrightness(MONITOR_BRIGHTNESS_MAX); */
}

void	CPC_ResetTiming(void)
{
	NopCountToDate = NOPS_PER_MONITOR_SCREEN;
}

BOOL CPCEmulation_Initialise(void)
{

	Debug_Init();

	DebugMain_Initialise();

	/* disable debugger */
	CPCEmulation_EnableDebugger(FALSE);

	/* attempt to initialise CPC core - initialised */
	if (CPC_Initialise())
	{
		/* load cpc 464 config */
	/*	Config_LoadConfiguration("cpc464"); */
		/* load cpc 664 config */
	/*	Config_LoadConfiguration("cpc664"); */
		/* load cpc 6128 config */
	/*	Config_LoadConfiguration("cpc6128"); */
		/* load cpc 464+ config */
	/*	Config_LoadConfiguration("464+"); */
		/* load cpc 6128+ config */
	/*	Config_LoadConfiguration("6128+"); */
		/* load kc compact config */
	/*	Config_LoadConfiguration("kc-compact"); */

		/* initialise z80 emulation */
		Z80_Init();

		/* set acknowledge interrupt callback function */
/*		Z80_SetUserAckInterruptFunction(CPC_AcknowledgeInterrupt); */

		/* CPC initialised, so set default setup */
		CPCEmulation_InitialiseDefaultSetup();

		return TRUE;
	}

	return FALSE;
}


extern unsigned char *pAudioBuffer;
extern unsigned int AudioBufferSize;
#if 0
void	CPC_UpdateAudio(void)
{
	int CPCNopCount = CPC_GetNopCount();
	int NopsReached = CPCNopCount;

	if (AudioActiveFlag)
	{
		/* update sound buffer with events in audio events buffer */
		AudioEvent_TraverseAudioEventsAndBuildSampleData();
	
		/* convert PSG vol/Digiblaster data to sample data */
		AudioEvent_ConvertToOutputFormat();
	}

	/* restart buffer ready to fill with new data */
	AudioEvent_RestartEventBuffer();
}
#endif


extern unsigned char *pAudioBuffer;
extern unsigned int AudioBufferSize;

void	CPC_UpdateAudio(void)
{
	if (AudioActiveFlag)
	{
		Digiblaster_EndFrame();
	}

	{
		int CPCNopCount = CPC_GetNopCount();
		int NopsReached = CPCNopCount;

		if (AudioActiveFlag)
		{
			/* update sound buffer with events in audio events buffer */
			NopsReached = AudioEvent_TraverseAudioEventsAndBuildSampleData(CPCNopCount,19968);
		}

		/* restart buffer ready to fill with new data */
		AudioEvent_RestartEventBuffer(NopsReached);
	}
}

void	CPCEmulation_EnableDebugger(BOOL State)
{
	DebuggerIsActive = State;
}

void	CPCEmulation_Run(void)
{
	BOOL doBreak = FALSE;

	while (!doBreak)
	{
		int NopCount;
		int LocalNopCountToDate;

		/* execute the instruction */
/*		NopCount = Z80_ExecuteInstruction(); */
		NopCount = Debugger_Execute(); 

		/* update CPC nop count - used for other hardware */
		CPC_UpdateNopCount(NopCount);

		LocalNopCountToDate = NopCountToDate - NopCount;

		/* nop counter is used to define when we render the whole display */
		if (LocalNopCountToDate<=0)		
		{
			int NopsToFrameEnd;

			NopsToFrameEnd = NopCount + LocalNopCountToDate;

			/* update CRTC for NopCount cycles */
			if (NopsToFrameEnd!=0)
				CRTC_DoCycles(NopsToFrameEnd);	
		
			/* execute functions when a frame has been completed */


		#ifdef AY_OUTPUT
			/* if enabled, writes PSG registers to temp file */
			YMOutput_WriteRegs();
		#endif

			/* Frame Skip Control code */

			/* update frame skip */
			CurrentFrameIndex++;

			if (CurrentFrameIndex==FrameSkip+1)
			{
				CurrentFrameIndex = 0;
				
				/* is rendering possible? */
				if (Render_IsRendererActive())
				{
					/* dump whole display to screen */
					Render_DumpDisplay();
				}
			}

			/* call speed throttle function - use this to 
			throttle speed at 100% */
			Host_Throttle();

			FDD_UpdateLEDState();

			doBreak = Host_ProcessSystemEvents(); 


			/* dont render */
			DontRender = TRUE;
			CRTC_SetRenderState(DontRender);

			/* should we render graphics? */
			if (CurrentFrameIndex == FrameSkip)
			{
				/* is rendering possible? */
				if (Render_IsRendererActive())
				{
					/* allow it to render */
					DontRender = FALSE;
					CRTC_SetRenderState(DontRender);
				}
			}

	#ifdef SHOW_TIME
			/* works out the speed of the emulation based on the previous 10 
			frames. This method stores a frame time in ms for each frame in
			a table, when 10 frame times have been recorded, it works out
			the average of those times and updates the percentage speed. */
			{
				unsigned long CurrentTimeInMs;
				unsigned long MsPassed;
				int first = Arnold_PreviousTimeInMs;

				/* get time for this frame in ms */
				CurrentTimeInMs = Host_GetCurrentTimeInMilliseconds();

				/* work out ms passed based on previous ms value */
				MsPassed = CurrentTimeInMs - Arnold_PreviousTimeInMs;
				
				/* store this time */
				Arnold_PreviousTimeInMs = CurrentTimeInMs;
				
				if (!first)
				{
					/* store this frame time */
					FrameTimesInMs[FrameIndexForFrameTimes] = MsPassed;
					FrameIndexForFrameTimes++;

					if (FrameIndexForFrameTimes == NUM_FRAMES_TO_TIME)
					{
						int i;
						unsigned long AverageTime = 0;

						/* recalc relative speed */
						FrameIndexForFrameTimes = 0;

						for (i=0; i<NUM_FRAMES_TO_TIME; i++)
						{
							/* add on time for this frame */
							AverageTime+=FrameTimesInMs[i];
						}

						AverageTime = AverageTime/NUM_FRAMES_TO_TIME;

						Arnold_PercentRelativeSpeed = (FRAME_TIME_IN_MS*100)/AverageTime;
					}
				}
			}
	#endif
/*			CPC_DoFrameFunc(); */
		
			if (LocalNopCountToDate!=0)
				CRTC_DoCycles(-LocalNopCountToDate);
		
			/* a whole screen has been timed */
			LocalNopCountToDate += NOPS_PER_MONITOR_SCREEN;

		}
		else
		{
			/* update CRTC for NopCount cycles */
			CRTC_DoCycles(NopCount);
		}

		NopCountToDate = LocalNopCountToDate;
	}
}

void	CPCEmulation_Finish(void)
{
	CPC_Finish();
	
	Render_Finish();

	DebugMain_Finish();

	Debug_Finish();
}

BOOL	CPCEmulation_CheckEndianness(void)
{
	BOOL EndianCorrect;
	unsigned long TestLong = 0;
	unsigned char *pLongPtr = (unsigned char *)&TestLong;

	pLongPtr[0] = 1;

#ifdef CPC_LSB_FIRST
	EndianCorrect = FALSE;
	if (TestLong == 1)
	{	
		/* on little endian, long = 1 */
		EndianCorrect = TRUE;
	}
#else
	EndianCorrect = TRUE;
	if (TestLong == 1)
	{
		/* on little endian, long = 1 */
		EndianCorrect = FALSE;
	}
#endif
	return EndianCorrect;
}
