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
/* TZX tape image support */
/* WRITING OF TZX IS NOT COMPLETE */
#include "cpcglob.h"
#include "tzx.h"
#include "cpc.h"
#include "audioevent.h"
#include "host.h"
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef TZX_DEBUGGING
#include "..\debug.h"
extern char DebugString[256];
extern DEBUG_HANDLE TZX_Debug;
#endif
unsigned char *pGretBigSample = NULL;
unsigned char *pPos;


static char *TZX_TapeHeader = "ZXTape!\26";

/* spectrum clock hz */
#define SPECTRUM_CLOCK_HZ		3500000

/* T-States in TZX file are stored for a Spectrum 3.5Mhz clock. CPC uses a 4Mhz
clock, so adjust for that. (T_SCALE) */
#define T_SCALE 			((40<<16)/35)

/* pointer to TZX in memory */
static unsigned char *pTapeImage = NULL;
/* size of TZX file in memory */
static unsigned long TapeImageSize = 0;
/* pointer to end of TZX file in memory */
static unsigned char *pTapeImageEnd = NULL;
/* flags - see below */
static unsigned long TapeImage_Flags;
/* pointer to current tzx block */
static unsigned char *pTapeBlockPtr = NULL;

static void	TapeImage_GetNextDataBlock(void);
int		TapeImage_GetBlockLength(unsigned char *pBlock);
//static void	TapeImage_HandleBlock(unsigned char *);


INLINE int	TapeImage_GetByte(unsigned char *pPtr)
{
	int Byte = pPtr[0] & 0x0ff;

	return Byte;
}

/* get word from tape image */
INLINE int	TapeImage_GetWord(unsigned char *pPtr)
{
	int Word;

	Word = TapeImage_GetByte(pPtr);
	Word |= (TapeImage_GetByte(pPtr+1)<<8);

	return Word;
}

/* get value from tape image */
INLINE	int TapeImage_GetValue(unsigned char *pPtr, int NoOfBytes)
{
	int Value = 0;
	int i;
	int Shift = 0;

	for (i=0; i<NoOfBytes; i++)
	{
		
		Value |= (TapeImage_GetByte(pPtr+i)<<Shift);

		Shift = Shift+8;
	}

	return Value;
}



/* true if block is a data or pause block */
static BOOL TapeImage_IsADataOrPauseBlock(unsigned char BlockIndex)
{
	if ((BlockIndex == 0x010) || (BlockIndex == 0x011) 
			|| (BlockIndex == 0x012) || (BlockIndex == 0x013) 
			|| (BlockIndex == 0x014) || (BlockIndex == 0x015) 
			|| (BlockIndex == 0x020))
	{
		return TRUE;
	}

	return FALSE;
}


/* t-states remaining in this block */

static unsigned long TapeImage_ExpandedByte[256];

void	TapeImage_Init(void)
{
	int i;
	
	for (i=0; i<256; i++)
	{
		unsigned char DataByte = (unsigned char)i;
		unsigned long ExpandedByte = 0;
		int b;

		for (b=0; b<8; b++)
		{
			ExpandedByte = ExpandedByte<<2;
	
			if (DataByte & 0x080)
			{
				ExpandedByte |= 0x03;
			}

			DataByte = (unsigned char)(DataByte<<1);

		}

		TapeImage_ExpandedByte[i] = ExpandedByte;
	}
}

/* set if we have reached end of tape */
#define TAPE_IMAGE_REACHED_END	0x0001
/* set if the tape motor is active and play is pressed */
#define TAPE_IMAGE_MOTOR_ON		0x0002

#if 0
static unsigned char Cycle=0x00;
static unsigned char BaseCycle = 0x00;


static unsigned long TapeImage_Global_TStatesRemaining = 0;
static BOOL TapeImage_LowLevelActive;
static BOOL TapeImage_HighLevelActive;
#endif

/* rewind back to start of tape */
void	TapeImage_RewindToStart(void)
{

	/* it's not reached the end, because it is now at the start */
	TapeImage_Flags &= ~TAPE_IMAGE_REACHED_END;

	if (pTapeImage!=NULL)
	{
		unsigned char BlockIndex;
		
		/* first block in tzx */
		pTapeBlockPtr = pTapeImage+10;

		pPos = pGretBigSample;
#if 0
		Cycle = 0x00;
		BaseCycle = (unsigned char)Cycle;

		TapeImage_Global_TStatesRemaining = 0;

		TapeImage_LowLevelActive  = FALSE;
		TapeImage_HighLevelActive = FALSE;
#endif
		/* get ID of first block */
		BlockIndex = (unsigned char)TapeImage_GetByte(pTapeBlockPtr);
	
		/* if first block is not a pause or data block, skip until we get data block */
		if (!TapeImage_IsADataOrPauseBlock(BlockIndex))
		{
			TapeImage_GetNextDataBlock(); 
		}

//		TapeImage_HandleBlock(pTapeBlockPtr);
	}
}

/* Does the equivalent of press or release play on tape deck */
void	TapeImage_PlayStatus(BOOL Status)
{
	if (Status)
	{
		/* play pressed */

		/* have we reached end of tape? */
		if (!(TapeImage_Flags & TAPE_IMAGE_REACHED_END))
		{
			/* no, so we can play */
			TapeImage_Flags |= TAPE_IMAGE_MOTOR_ON;
		}
	}
	else
	{
		/* play released */
		TapeImage_Flags &= ~TAPE_IMAGE_MOTOR_ON;
	}
}

/* get next block if available */
static void TapeImage_NextBlock(void)
{
	if (!(TapeImage_Flags & TAPE_IMAGE_REACHED_END))
	{
		/* not reached end of tape */
		
		/* skip the current block */
		pTapeBlockPtr += TapeImage_GetBlockLength(pTapeBlockPtr);

		/* have we reached end of file? */
		if (pTapeBlockPtr>=pTapeImageEnd)
		{
			/* reached end of tape. */
			TapeImage_Flags |= TAPE_IMAGE_REACHED_END;
		}
	}
}


static unsigned long TapeImage_ScaleTStates(unsigned long TStateCount)
{
	return (TStateCount*T_SCALE)>>16;
}


/* convert a time in milliseconds to a t-state value */
static unsigned long TapeImage_MillisecondsToTStates(unsigned long Milliseconds)
{
	/* Spectrum clock specifies T states per second. 1000 milliseconds in a second */
	return ((SPECTRUM_CLOCK_HZ/1000)*Milliseconds);
}

#if 0
enum
{
	TZX_STATE_BASIC_PULSE = 1,
	TZX_STATE_BASIC_PAUSE
} TZX_LOW_LEVEL_STATE_ENUM;

/* a basic tone is a cycle of the square wave, which may be repeated.
This is the basic block for tones and data */
typedef struct TZX_BASIC_PULSE
{
	/* number of cycles to repeat this tone */
	unsigned long NumCycles;
	/* width of pulse in T-States (scaled) */
	unsigned long PulseWidthInTStates;		
	/* scaled number of T-States remaining */
	unsigned long TStatesRemaining;
	/* number of t-states passed so far */
	unsigned long TStatesSoFar;

} TZX_BASIC_PULSE;

/* a basic pause block */
typedef struct TZX_BASIC_PAUSE
{
	/* length of pause in T-States (scaled) */
	unsigned long PauseInTStates;
	unsigned long pad0;
	/* scaled number of T-States remaining */
	unsigned long TStatesRemaining;
	/* number of t-states passed so far */
	unsigned long TStatesSoFar;
} TZX_BASIC_PAUSE;

static void	TapeImage_UpdateCycleState(TZX_BASIC_PULSE *pBasicTone)
{
	unsigned long Cycles;

	/* number of cycles so far */
	Cycles = pBasicTone->TStatesSoFar/pBasicTone->PulseWidthInTStates;

	if (Cycles & 1)
	{
		/* odd */

		Cycle = (unsigned char)(BaseCycle ^ 0x01);
	}
	else
	{
		/* even */
		Cycle = BaseCycle; 
	}
}




/* current low-level state */
static unsigned long TapeImage_LowLevelState;
static void *TapeImage_LowLevelStateData;

/* high level state */
static unsigned long TapeImage_HighLevelState;
static BOOL  (*TapeImage_HighLevelStateData)(void);
static unsigned char *TapeImage_HighLevelStateBlockPtr;

/* low-level states, pause and tone */
static void	TapeImage_SetLowLevelState(unsigned long StateType, void *StateData)
{
	/* calc T-States remaining for low level state */
	switch (StateType)
	{
		case TZX_STATE_BASIC_PULSE:
		{
			TZX_BASIC_PULSE *pBasicTone = (TZX_BASIC_PULSE *)StateData;

			/* calc t-states for pulse */
			pBasicTone->TStatesRemaining = 
					(pBasicTone->PulseWidthInTStates)*pBasicTone->NumCycles;

			/* number of t-states executed of this tone */
			pBasicTone->TStatesRemaining -= TapeImage_Global_TStatesRemaining;

			pBasicTone->TStatesSoFar = TapeImage_Global_TStatesRemaining;

			TapeImage_UpdateCycleState(pBasicTone);			
		}
		break;

		case TZX_STATE_BASIC_PAUSE:
		{
			TZX_BASIC_PAUSE *pBasicPause = (TZX_BASIC_PAUSE *)StateData;

			BaseCycle = 0x0;
			Cycle = 0x0;

			/* pause is already defined in T-States */
			pBasicPause->TStatesRemaining = pBasicPause->PauseInTStates;

			pBasicPause->TStatesRemaining -= TapeImage_Global_TStatesRemaining;
		
			pBasicPause->TStatesSoFar = TapeImage_Global_TStatesRemaining;
		}
		break;
	}

	/* set state type */
	TapeImage_LowLevelState = StateType;
	
	/* set state data */
	TapeImage_LowLevelStateData = StateData;

	TapeImage_LowLevelActive = TRUE;

	
}

/* high level states - the handlers for each block type */
static void TapeImage_SetHighLevelState(BOOL (*StateData)(void), unsigned char *pBlockPtr)
{
	/* set initial state */
	TapeImage_HighLevelState = 0;
	/* set handler */
	TapeImage_HighLevelStateData = StateData;
	TapeImage_HighLevelStateBlockPtr = pBlockPtr;


	TapeImage_HighLevelActive = TRUE;
}
#endif

/***************************************************/
/** TZX LOOP BLOCK HANDLING **/
typedef struct TZX_LOOP
{
	unsigned char *pBlockPtr;
	unsigned long LoopCount;
} TZX_LOOP;

#define TZX_MAX_NESTED_LOOPS	16

static TZX_LOOP	TZX_LoopStack[TZX_MAX_NESTED_LOOPS];
static unsigned long TZX_LoopStack_NumEntries = 0;
static TZX_LOOP *TZX_CurrentLoop = NULL;

/* push new loop onto stack */
static BOOL TZX_PushLoop(unsigned char *pBlockPtr, unsigned long LoopCount)
{
	/* as long as the number of nested loops doesn't
	exceed the amount we allow */
	if ((TZX_LoopStack_NumEntries+1)!=TZX_MAX_NESTED_LOOPS)
	{
		/* put loop information onto the loop stack */
		TZX_LOOP *pNewLoop;

		pNewLoop = &TZX_LoopStack[TZX_LoopStack_NumEntries];
		
		TZX_LoopStack_NumEntries++;

		pNewLoop->pBlockPtr = pBlockPtr;
		pNewLoop->LoopCount = LoopCount;

		/* set current loop to be the new one just pushed */
		TZX_CurrentLoop = pNewLoop;

		/* ok, we put the loop on the stack */
		return TRUE;
	}
	else
	{
		/* too many nested loops */
		return FALSE;
	}
}

/* we've completed this loop, pop the next loop off */
static void	TZX_PopLoop(void)
{
	TZX_LoopStack_NumEntries--;

	if (TZX_LoopStack_NumEntries!=0)
	{
		/* get previous loop from stack */
		TZX_CurrentLoop = &TZX_LoopStack[TZX_LoopStack_NumEntries];
	}
	else
	{
		/* indicate no loops are active */
		TZX_CurrentLoop = NULL;
	}
}

static unsigned char *TZX_DoLoop(void)
{
	return TZX_CurrentLoop->pBlockPtr;
}

/* call here if loop end found */
static BOOL	TZX_UpdateLoop(void)
{
	if (TZX_CurrentLoop==NULL)
	{
		/* found a loop end without a loop start */
		return FALSE;
	}

	/* loop end found, loop is active */
	TZX_CurrentLoop->LoopCount--;

	if (TZX_CurrentLoop->LoopCount==0)
	{
		/* finished this loop, pop previous loop */
		TZX_PopLoop();

		/* do not loop */
		return FALSE;
	}
		
	/* do loop */
	return TRUE;
}

/***************************************************/

#if 0








/* returns TRUE if low-level state has completed, FALSE otherwise */
static BOOL TapeImage_DoLowLevelState(int TStatesPassed)
{
	switch (TapeImage_LowLevelState)
	{
		/* tone */
		case TZX_STATE_BASIC_PULSE:
		{
			TZX_BASIC_PULSE *pBasicTone = (TZX_BASIC_PULSE *)TapeImage_LowLevelStateData;


			/* if t-states passed has exceeded the number of t-states
			remaining for this basic tone */
			if ((unsigned long)TStatesPassed>=pBasicTone->TStatesRemaining)
			{
				/* tone has completed within T-States that have Passed */

				/* global t states remaining hold number of T-States
				over end of tone we have done */
				TapeImage_Global_TStatesRemaining = TStatesPassed - pBasicTone->TStatesRemaining;

				/* no low level state is active */
				TapeImage_LowLevelActive = FALSE;

				{
					unsigned long Cycles;

					pBasicTone->TStatesSoFar+=TStatesPassed;

					/* number of cycles so far */
					Cycles = pBasicTone->TStatesSoFar/pBasicTone->PulseWidthInTStates;

					if (Cycles & 1)
					{
						/* odd */

						BaseCycle = (unsigned char)(BaseCycle ^ 0x01);
					}
					Cycle = BaseCycle;
				}

				return TRUE;
			}

			/* update t-states remaining */
			pBasicTone->TStatesRemaining-=TStatesPassed;

			/* calc new state for Cycle */
			pBasicTone->TStatesSoFar += TStatesPassed;



			TapeImage_UpdateCycleState(pBasicTone);	

			/* tone not complete */
			return FALSE;
		}
		break;

		/* pause */
		case TZX_STATE_BASIC_PAUSE:
		{
			TZX_BASIC_PAUSE *pBasicPause = (TZX_BASIC_PAUSE *)TapeImage_LowLevelStateData;

			/* if t-states passed has exceeded the number of t-states
			remaining for this basic tone */
			if ((unsigned long)TStatesPassed>=pBasicPause->TStatesRemaining)
			{
				/* tone has completed within T-States that have Passed */

				/* global t states remaining hold number of T-States
				over end of tone we have done */
				TapeImage_Global_TStatesRemaining = TStatesPassed - pBasicPause->TStatesRemaining;

				/* no low level state is active */
				TapeImage_LowLevelActive = FALSE;

				return TRUE;
			}

			/* update t-states remaining */
			pBasicPause->TStatesRemaining-=TStatesPassed;

			/* calc new state for Cycle */
			pBasicPause->TStatesSoFar += TStatesPassed;

			/* pause not complete */
			return FALSE;
		}
		break;
	}

	return TRUE;
}

static BOOL TapeImage_DoHighLevelState(void)
{
	BOOL (*pHandler)(void) = TapeImage_HighLevelStateData;

	if (pHandler!=NULL)
	{
		return pHandler();
	}

	return TRUE;
}


/* update state */
void	TapeImage_UpdateState(int TStatesPassed)
{
	/* is motor active? */
	if	((TapeImage_Flags & TAPE_IMAGE_REACHED_END)==0)
	{
		/* is there a low level state active? */

		if (TapeImage_LowLevelActive)
		{
			/* low level is active */
			if (TapeImage_DoLowLevelState(TStatesPassed))
			{
				if (TapeImage_DoHighLevelState())
				{
					TapeImage_GetNextDataBlock();
				
					TapeImage_HandleBlock(pTapeBlockPtr);
				}

			}
		}
		else
		{
			if (TapeImage_DoHighLevelState())
			{
				TapeImage_GetNextDataBlock();

				TapeImage_HandleBlock(pTapeBlockPtr);
			
			}
			else
			{
				TapeImage_DoLowLevelState(TStatesPassed);
			}

		}

	}
}

/* pointer to data */
static unsigned char *TapeImage_pData;
/* length of a one pulse in t states */
static unsigned long TapeImage_LengthOfOneBitPulseInTStates;
/* length of a zero pulse in t states */
static unsigned long TapeImage_LengthOfZeroBitPulseInTStates;
/* current data byte from block */


static unsigned long TapeImage_CurrentExpandedDataByte;


/* number of bits used in last byte */
static unsigned long TapeImage_NumBitsUsedInLastByte;
/* bits remaining in current byte */
static unsigned int TapeImage_BitsRemainingInCurrentByte;
/* bytes remaining in block */
static unsigned int	TapeImage_BytesRemaining;

static TZX_BASIC_PULSE BasicToneData;

/* get next byte of data from block. TRUE if block is finished,
else FALSE */
static BOOL TapeImage_GetNextDataByte(void)
{
	if (TapeImage_BytesRemaining!=0)
	{
		if (TapeImage_BytesRemaining==1)
		{
			/* we are going to transfer bits from the last byte in the block */
			/* setup last bit index for number of bits used in this byte */
			TapeImage_BitsRemainingInCurrentByte = TapeImage_NumBitsUsedInLastByte<<1;
		}
		else
		{
			/* we are going to transfer a whole byte of data*/
			TapeImage_BitsRemainingInCurrentByte = 16;
		}

		/* get new byte and update pointer */
		TapeImage_CurrentExpandedDataByte = 
			TapeImage_ExpandedByte[TapeImage_GetByte(TapeImage_pData)];
		
		TapeImage_pData++;

		TapeImage_BytesRemaining--;

		return FALSE;
	}

	return TRUE;
}


/* get next bit of data from data byte, TRUE if byte finished,
else FALSE */
static BOOL	TapeImage_GetNextBitFromDataByte(void)
{
	if (TapeImage_BitsRemainingInCurrentByte==0)
	{
		return TRUE;
	}
	else
	{
		/* get bit state */
		if ((TapeImage_CurrentExpandedDataByte & 0x08000)!=0)
		{
			/* this is a "1" data bit */
			BasicToneData.PulseWidthInTStates = TapeImage_LengthOfOneBitPulseInTStates;
			BasicToneData.NumCycles = 1;

			/* do tone */
			TapeImage_SetLowLevelState(TZX_STATE_BASIC_PULSE, &BasicToneData);
		}
		else
		{
			/* this is a "0" data bit */
			BasicToneData.PulseWidthInTStates = TapeImage_LengthOfZeroBitPulseInTStates;
			BasicToneData.NumCycles = 1;
		
			/* do tone */
			TapeImage_SetLowLevelState(TZX_STATE_BASIC_PULSE, &BasicToneData);
		}

		/* update data byte for next bit to be fetched */
		TapeImage_CurrentExpandedDataByte = TapeImage_CurrentExpandedDataByte<<1;

		/* update current bit index */
		TapeImage_BitsRemainingInCurrentByte--;
	}

	return FALSE;
}

/* t-states per byte. The total number of t-states
required to transfer the whole byte. */
static unsigned long TStates_PerByte[256];

/* generate a table of t-state values for all bytes */
static void	TapeImage_InitTStatesForData(void)
{
	int i,b;
	unsigned long TStateCount;

	for (i=0; i<255; i++)
	{
		unsigned char DataByte = (unsigned char)i;
		
		TStateCount = 0;

		for (b=0; b<8; b++)
		{
			unsigned long TStatesPerBitPulse;

			/* get data bit */
			if (DataByte & 0x080)
			{
				/* 1 bit */
				TStatesPerBitPulse = TapeImage_LengthOfOneBitPulseInTStates;
			}
			else
			{
				/* 0 bit */
				TStatesPerBitPulse = TapeImage_LengthOfZeroBitPulseInTStates;
			}

			/* add on t-states for complete wave. */
			TStateCount+=(TStatesPerBitPulse<<1);

			/* next bit in byte */
			DataByte = (unsigned char)(DataByte<<1);
		}
	}
}


/* TRUE if completed data, else FALSE */
static BOOL	TapeImage_HandleData(void)
{
	/* get next bit */
	if (TapeImage_GetNextBitFromDataByte())
	{
		/* completed current byte */

		/* get next data byte if available */
		if (TapeImage_GetNextDataByte())
		{
			/* completed block */

			TapeImage_HighLevelState++;

			return TRUE;
		}
		else
		{
			/* not completed block */

			/* get next data bit */
			TapeImage_GetNextBitFromDataByte();
		}
	}

	return FALSE;
}

static void	TapeImage_InitForHandleData(void)
{
	TapeImage_BitsRemainingInCurrentByte = 0;
}

/****************************************************/


static TZX_BASIC_PULSE	BasicTone;
static TZX_BASIC_PAUSE	BasicPause;
static TZX_BASIC_PULSE	BasicToneData;




/*
int LengthOfPilotPulse;
int LengthOfSyncFirstPulse;
int LengthOfSyncSecondPulse;
int LengthOfPilotToneInPilotPulses;

int LengthOfZeroBitPulse;
int LengthOfOneBitPulse;
int NumBitsUsedInLastByte;
int LengthOfDataFollowing;

int PauseAfterBlockInMs;


int PilotPulsesRemaining;

*/
/* current stage */

/* sample rate */


#endif
unsigned long CurrentLevel = 0;

typedef struct
{
	unsigned long emp;
} SAMPLE_BUFFER;

/* on Amstrad */
/* 4 t-states = 1us */

#define A_MICROSECOND (1e-06)
#define SAMPLE_RATE (22050)

unsigned long TStatesToSamples(SAMPLE_BUFFER *pBuffer, unsigned long TStates)
{
	unsigned long ScaledTStates = TapeImage_ScaleTStates(TStates);

	/* scaled t states will be in 4 */
	return TStates/64;
}

void	SampleBuffer_Fill(SAMPLE_BUFFER *pBuffer, unsigned long NumSamples)
{
	int i;

	for (i=0; i<NumSamples; i++)
	{
		pPos[0] = CurrentLevel;
		pPos++;
	}
}

void	SampleBuffer_ChangeLevel(SAMPLE_BUFFER *pBuffer)
{
	if (CurrentLevel==1)
	{
		CurrentLevel = 0;
	}
	else if (CurrentLevel == 0)
	{
		CurrentLevel = 1;
	}
}


void	OutputPause(SAMPLE_BUFFER *pBuffer, unsigned long TimeInMilliseconds)
{
	if (TimeInMilliseconds!=0)
	{	
		unsigned long SpectrumTStates = TapeImage_MillisecondsToTStates(TimeInMilliseconds);
		unsigned long NumSamples = TStatesToSamples(pBuffer,SpectrumTStates);

		CurrentLevel  = 0;

		SampleBuffer_Fill(pBuffer, NumSamples);
	}
}

void	OutputPulse(SAMPLE_BUFFER *pBuffer, unsigned long PulseWidthInTStates)
{
	unsigned long NumSamples = TStatesToSamples(pBuffer,PulseWidthInTStates);

	SampleBuffer_Fill(pBuffer, NumSamples);

	SampleBuffer_ChangeLevel(pBuffer);
}

void	OutputWave(SAMPLE_BUFFER *pBuffer, unsigned long PulseWidthInTStates)
{
	OutputPulse(pBuffer, PulseWidthInTStates);
	OutputPulse(pBuffer, PulseWidthInTStates);
}

void	OutputWaves(SAMPLE_BUFFER *pBuffer, unsigned long PulseWidthInTStates, unsigned long NumWaves)
{
	int i;

	for (i=0; i<NumWaves; i++)
	{
		OutputWave(pBuffer, PulseWidthInTStates);
	}
}


void	OutputPulses(SAMPLE_BUFFER *pBuffer, unsigned long PulseWidthInTStates, unsigned long NumPulses)
{
	int i;

	for (i=0; i<NumPulses; i++)
	{
		OutputPulse(pBuffer, PulseWidthInTStates);
	}
}

typedef struct
{
	unsigned long LengthOfOneBitPulseInTStates;
	unsigned long LengthOfZeroBitPulseInTStates;
	unsigned long Length;
	unsigned char *pData;
	unsigned long NumberOfBitsUsedInLastByte;
} TZX_DATA_SIGNAL_DESCRIPTION;

void	OutputData(SAMPLE_BUFFER *pBuffer, TZX_DATA_SIGNAL_DESCRIPTION *pDataDesc)
{
	unsigned long NumBitsRemaining;
	unsigned char CurrentByte;
	unsigned long NumBytesRemaining;
	unsigned char *pData;

	pData = pDataDesc->pData;
	NumBytesRemaining = pDataDesc->Length;

	while (NumBytesRemaining!=0)
	{
		if (NumBytesRemaining==1)
		{
			NumBitsRemaining = pDataDesc->NumberOfBitsUsedInLastByte;
		}
		else
		{
			NumBitsRemaining = 8;
		}

		NumBytesRemaining--;

		CurrentByte = pData[0];
		pData++;

		do
		{
			/* get current bit */
			if ((CurrentByte & 0x080)!=0)
			{
				/* 1 bit */
				OutputWave(pBuffer, pDataDesc->LengthOfOneBitPulseInTStates);
			}
			else
			{
				/* 0 bit */
				OutputWave(pBuffer, pDataDesc->LengthOfZeroBitPulseInTStates);
			}
			
			CurrentByte = CurrentByte<<1;
			
			NumBitsRemaining--;
		}
		while (NumBitsRemaining!=0);
	}

}

void TapeImage_HandleBlock0x010(SAMPLE_BUFFER *pBuffer, char *pBlockDataPtr)
{
	TZX_DATA_SIGNAL_DESCRIPTION DataDescription;

	/* setup pilot tone */
	OutputWaves(pBuffer, 2168, 3220);
	
	/* sync pulse 1 */
	OutputPulse(pBuffer, 667);

	/* sync pulse 2 */
	OutputPulse(pBuffer, 735);

	/* data */
	DataDescription.LengthOfOneBitPulseInTStates = 1710;
	DataDescription.LengthOfZeroBitPulseInTStates = 855;
	DataDescription.pData = pBlockDataPtr + 0x04;
	DataDescription.Length = TapeImage_GetWord(pBlockDataPtr+0x02);
	DataDescription.NumberOfBitsUsedInLastByte = 8;
	
	OutputData(pBuffer, &DataDescription);

	/* pause after block */
	OutputPause(pBuffer, TapeImage_GetWord(pBlockDataPtr + 0x00));
}


void TapeImage_HandleBlock0x011(SAMPLE_BUFFER *pBuffer, char *pBlockDataPtr)
{
	TZX_DATA_SIGNAL_DESCRIPTION DataDescription;

	/* setup pilot tone */
	OutputWaves(pBuffer, TapeImage_GetWord(pBlockDataPtr+0), TapeImage_GetWord(pBlockDataPtr+0x0a));

	/* sync pulse 1 */
	OutputPulse(pBuffer, TapeImage_GetWord(pBlockDataPtr+2));

	/* sync pulse 2 */
	OutputPulse(pBuffer, TapeImage_GetWord(pBlockDataPtr+4));

	/* data */
	DataDescription.LengthOfOneBitPulseInTStates = TapeImage_GetWord(pBlockDataPtr + 0x08);
	DataDescription.LengthOfZeroBitPulseInTStates = TapeImage_GetWord(pBlockDataPtr + 0x06);
	DataDescription.pData = pBlockDataPtr + 0x012;
	DataDescription.Length = TapeImage_GetValue(pBlockDataPtr + 0x0f, 3);
	DataDescription.NumberOfBitsUsedInLastByte = TapeImage_GetByte(pBlockDataPtr + 0x0c);

	OutputData(pBuffer, &DataDescription);

	/* pause after block */
	OutputPause(pBuffer, TapeImage_GetWord(pBlockDataPtr + 0x0d));
}

/* Handle Block 0x012 - Pure Tone */
void TapeImage_HandleBlock0x012(SAMPLE_BUFFER *pBuffer, char *pBlockDataPtr)
{
	/* setup pilot tone */
	OutputPulses(pBuffer, TapeImage_GetWord(pBlockDataPtr+0), TapeImage_GetWord(pBlockDataPtr+2));
}

/* Handle Block 0x013 - Sequence of Pulses of different lengths */
void TapeImage_HandleBlock0x013(SAMPLE_BUFFER *pBuffer, char *pBlockDataPtr)
{
	unsigned long NumPulses;
	int i;

	/* get number of pulses */
	NumPulses = TapeImage_GetByte(pBlockDataPtr);

	for (i=0; i<NumPulses; i++)
	{
		OutputPulse(pBuffer, TapeImage_GetWord(pBlockDataPtr+(i<<1)+1));
	}
}


/* Handle Block 0x014 - Pure Data*/
void TapeImage_HandleBlock0x014(SAMPLE_BUFFER *pBuffer, char *pBlockDataPtr)
{
	TZX_DATA_SIGNAL_DESCRIPTION DataDescription;
	/* data */
	DataDescription.LengthOfOneBitPulseInTStates = TapeImage_GetWord(pBlockDataPtr + 0x02);
	DataDescription.LengthOfZeroBitPulseInTStates = TapeImage_GetWord(pBlockDataPtr + 0x00);
	DataDescription.pData = pBlockDataPtr + 0x0a;
	DataDescription.Length = TapeImage_GetValue(pBlockDataPtr + 0x07, 3);
	DataDescription.NumberOfBitsUsedInLastByte = TapeImage_GetByte(pBlockDataPtr + 0x04);

	OutputData(pBuffer, &DataDescription);

	/* pause after block */
	OutputPause(pBuffer, TapeImage_GetWord(pBlockDataPtr + 0x05));
}



/* pause */
void TapeImage_HandleBlock0x020(SAMPLE_BUFFER *pBuffer, char *pBlockDataPtr)
{
	/* pause */
	OutputPause(pBuffer, TapeImage_GetWord(pBlockDataPtr + 0x00));
}

		

/* returns TRUE if completed block, else FALSE */
void TapeImage_HandleBlock0x015(void)
{
}

unsigned long PosUpd = 0;


/* update tape position based on tstates passed and return a bit */
int TapeImage_GetBit(int TStatesPassed)
{
	PosUpd += (TStatesPassed>>2);

	pPos+=PosUpd>>4;
	PosUpd&=0x0f;
	return pPos[0];

#if 0
	Cycle = 0x01;

	if (pTapeImage!=NULL)
	{
		if ((TapeImage_Flags & TAPE_IMAGE_REACHED_END)==0)
		{
			int i;

			/* not quite correct for any value of t-states */
			for (i=0; i<TStatesPassed>>2; i++)
			{
				/* update tape position etc */
				TapeImage_UpdateState(4);
			}
		}
	}


	/* send new bit of data */
	return Cycle;
#endif

	return 1;
}



static char	*TapeImage_ArchiveInfo_GetTextID(int Byte)
{
	switch (Byte)
	{
		case 00:
		{
			return "Full Title";
		}

		case 01:
		{
			return "Software House/Publisher";
		}

		case 02:
		{
			return "Author(s)";
		}

		case 03:
		{
			return "Year of Publication";
		}

		case 04:
		{
			return "Language";
		}

		case 0x0ff:
		{
			return "Comments";
		}

		default:
		{
			return "Unknown";
		}
	}
}
#if 0
/******************/
/* Handle a block */
static void TapeImage_HandleBlock(unsigned char *pBlock)
{
	unsigned char *pBlockPtr = pBlock;
	int	BlockIndex;

	/* get block ID */
	BlockIndex = TapeImage_GetByte(pBlockPtr);
	
	pBlockPtr++;

	switch (BlockIndex)
	{
		/* PLAY BLOCKS */
		
		/* standard speed data block */
		case 0x010:
		{
			TapeImage_SetHighLevelState(TapeImage_HandleBlock0x010, pBlockPtr);
		}
		break;

		/* Turbo loading data block */
		case 0x011:
		{
			TapeImage_SetHighLevelState(TapeImage_HandleBlock0x011,pBlockPtr);
		}
		break;

		/* Pure Tone */
		case 0x012:
		{
			TapeImage_SetHighLevelState(TapeImage_HandleBlock0x012, pBlockPtr);
		}
		break;

		/* Sequence of pulses of different lengths */
		case 0x013:
		{
			TapeImage_SetHighLevelState(TapeImage_HandleBlock0x013, pBlockPtr);
		}
		break;

		/* Pure Data */
		case 0x014:
		{
			TapeImage_SetHighLevelState(TapeImage_HandleBlock0x014, pBlockPtr);
		}
		break;
		
		/* direct recording */
		case 0x015:
		{
			TapeImage_SetHighLevelState(TapeImage_HandleBlock0x015, pBlockPtr);
		}
		break;

		/* pause/stop the tape */
		case 0x020:
		{
			TapeImage_SetHighLevelState(TapeImage_HandleBlock0x020, pBlockPtr);
		}
		break;

		/* CONTROL BLOCKS */

		/* Loop Start */
		case 0x024:
		{
			int RepetitionCount;
			
			RepetitionCount = TapeImage_GetWord(pBlockPtr);
			pBlockPtr+=2;

			TZX_PushLoop(pBlockPtr, RepetitionCount);
		}
		break;

		/* Loop End */
		case 0x025:
		{
			if (TZX_UpdateLoop())
			{
				/* set new block ptr */
				pBlockPtr = TZX_DoLoop();
			}
		}
		break;



	}
}
#endif

static void TapeImage_HandleBlock(SAMPLE_BUFFER *pBuffer, unsigned char *pBlock)
{
	unsigned char *pBlockPtr = pBlock;
	int	BlockIndex;

	/* get block ID */
	BlockIndex = TapeImage_GetByte(pBlockPtr);
	
	pBlockPtr++;

	switch (BlockIndex)
	{
		/* PLAY BLOCKS */
		
		/* standard speed data block */
		case 0x010:
		{
			TapeImage_HandleBlock0x010(pBuffer, pBlockPtr);
		}
		break;

		/* Turbo loading data block */
		case 0x011:
		{
			TapeImage_HandleBlock0x011(pBuffer, pBlockPtr);
		}
		break;

		/* Pure Tone */
		case 0x012:
		{
			TapeImage_HandleBlock0x012(pBuffer, pBlockPtr);
		}
		break;

		/* Sequence of pulses of different lengths */
		case 0x013:
		{
			TapeImage_HandleBlock0x013(pBuffer, pBlockPtr);
		}
		break;

		/* Pure Data */
		case 0x014:
		{
			TapeImage_HandleBlock0x014(pBuffer, pBlockPtr);
		}
		break;
		
		/* direct recording */
		case 0x015:
		{
//			TapeImage_HandleBlock0x015(pBuffer, pBlockPtr);
		}
		break;

		/* pause/stop the tape */
		case 0x020:
		{
			TapeImage_HandleBlock0x020(pBuffer, pBlockPtr);
		}
		break;

		/* CONTROL BLOCKS */

		/* Loop Start */
		case 0x024:
		{
			int RepetitionCount;
			
			RepetitionCount = TapeImage_GetWord(pBlockPtr);
			pBlockPtr+=2;

			TZX_PushLoop(pBlockPtr, RepetitionCount);
		}
		break;

		/* Loop End */
		case 0x025:
		{
			if (TZX_UpdateLoop())
			{
				/* set new block ptr */
				pBlockPtr = TZX_DoLoop();
			}
		}
		break;



	}
}

/****************/
/* skip a block */
static int		TapeImage_GetBlockLength(unsigned char *pBlock)
{
	unsigned char *pBlockPtr = pBlock;
	int	BlockIndex;
	int BlockLength = 0;

	/* get block ID */
	BlockIndex = TapeImage_GetByte(pBlockPtr);

	pBlockPtr++;

	switch (BlockIndex)
	{
		/* standard speed data block */
		case 0x010:
		{
			BlockLength = TapeImage_GetWord(pBlockPtr+0x02);

			return BlockLength + 4 + 1;
		}
		
		/* Turbo loading data block */
		case 0x011:
		{
			BlockLength = TapeImage_GetValue(pBlockPtr+0x0f,3);

			return BlockLength + 0x012 + 1;

		}
		break;
	
		/* pure tone */
		case 0x012:
		{
			return 4+1;
		}

		/* pulses of different lengths */
		case 0x013:
		{
			BlockLength = TapeImage_GetByte(pBlockPtr)<<1;

			return BlockLength+1+1;
		}

		/* pure data */
		case 0x014:
		{
			BlockLength = TapeImage_GetValue(pBlockPtr+0x07, 3);

			return BlockLength + 0x0a + 1;
		}

		/* direct recording */
		case 0x015:
		{
			BlockLength = TapeImage_GetValue(pBlockPtr+0x05, 3);

			return BlockLength + 0x08 + 1;
		}

		/* stop tape/pause */
		case 0x020:
		{
			return 0x02 + 1;
		}

		/* group start */
		case 0x021:
		{
			BlockLength = TapeImage_GetByte(pBlockPtr+0x00);

			return BlockLength + 1 + 1;
		}

		/* group end */
		case 0x022:
		{
			return 1;
		}


		/* jump to block */
		case 0x023:
		{
			return 0x02 + 1;
		}

		/* loop start */
		case 0x024:
		{
			return 0x02 + 1;
		}
		
		/* loop end */
		case 0x025:
		{
			return 1;
		}

		/* call sequence */
		case 0x026:
		{
			BlockLength = TapeImage_GetWord(pBlockPtr+0x00)<<1;

			return BlockLength + 2 + 1;
		}


		/* return from sequence */
		case 0x027:
		{
			return 1;
		}



		/* select block */
		case 0x028:
		{
			BlockLength = TapeImage_GetWord(pBlockPtr+0x00);

			return BlockLength + 2 + 1;
		}

		/* stop tape if in 48k mode */
		case 0x02a:
		{
			BlockLength = TapeImage_GetValue(pBlockPtr+0x00,4);

			return BlockLength + 4 + 1;
		}


		/* Text Description */
		case 0x030:
		{
			BlockLength = TapeImage_GetByte(pBlockPtr+0x00);

			return BlockLength + 1 + 1;
		}

		/*  Message block */
		case 0x031:
		{
			BlockLength = TapeImage_GetByte(pBlockPtr+0x01);

			return BlockLength+2+1;
		}

		/* "Archive Info" */
		case 0x032:
		{
			/* get block length */
			BlockLength = TapeImage_GetWord(pBlockPtr+0x00);

			return BlockLength + 2 + 1;
		}

		/* "Hardware Type */
		case 0x033:
		{
			BlockLength = TapeImage_GetByte(pBlockPtr+0x00)*3;

			return BlockLength + 0x01 + 1;
		}

		/* emulation info */
		case 0x034:
		{
			return 0x08+1;
		}

		/* custom info block */
		case 0x035:
		{
			BlockLength = TapeImage_GetValue(pBlockPtr+0x010, 4);

			return BlockLength + 0x014+1;
		}


		/* snapshot block */
		case 0x040:
		{
			BlockLength = TapeImage_GetValue(pBlockPtr+0x01, 3);

			return BlockLength+1+0x04;
		}


		/* joined tape's */
		case 'Z':
		{
			return 0x09+1;
		}

		/* block following the extension rule - block 0x017, 0x016 included here */
		default:
		{
			/* get block length */
			BlockLength = TapeImage_GetValue(pBlockPtr+0x00,4);

			return BlockLength + 4 + 1;
		}
		break;

	}
}

static BOOL	TapeImage_IsHandledBlock(int BlockIndex)
{
	switch (BlockIndex)
	{
		case 0x010:
		case 0x011:
		case 0x012:
		case 0x013:
		case 0x014:
		case 0x015:
		case 0x020:
		case 0x021:
		case 0x022:
		case 0x023:
		case 0x024:
		case 0x025:
		case 0x026:
		case 0x027:
		case 0x028:
		case 0x02a:
		case 0x030:
		case 0x031:
		case 0x032:
		case 0x033:
		case 0x034:
		case 0x035:
		case 0x040:
		case 'Z':
			return TRUE;

		default:
			break;
	}

	return FALSE;
}

/* a block is assumed to be valid if it's size is within the end
of the tape file */
static BOOL		TapeImage_BlockIsValid(unsigned char *pBlockPtr, unsigned char *pTapeImageEnd, unsigned long TapeImageSize)
{
	unsigned char BlockIndex;

	/* get block ID */
	BlockIndex = (unsigned char)TapeImage_GetByte(pBlockPtr);

	/* is it a handled block? */
	if (TapeImage_IsHandledBlock(BlockIndex))
	{
		/* yes */

		unsigned long BlockLength;
		
		/* get block length */
		BlockLength = TapeImage_GetBlockLength(pBlockPtr);
	
		/* check size does not exceed size of tape image */
		if (BlockLength<TapeImageSize)
		{
			/* check block size + current block position
			doesn't exceed end of tape */
			if ((pBlockPtr+BlockLength)<=pTapeImageEnd)
			{
				/* length is valid */
			
				return TRUE;
			}
		}
	}
	else
	{
		/* not handled block */

		unsigned long BlockLength;

		BlockLength = TapeImage_GetValue(pBlockPtr+1, 4);

		/* check size does not exceed size of tape image */
		if (BlockLength<TapeImageSize)
		{
			/* check block size + current block position
			doesn't exceed end of tape */
			if ((pBlockPtr+BlockLength)<=pTapeImageEnd)
			{
				/* length is valid */
	
				return TRUE;
			}
		}
	}

	return FALSE;
}



static HOST_FILE_HANDLE TZX_Write_Handle;

void	TZX_Write_Initialise(char *pFilename)
{
	TZX_Write_Handle = Host_OpenFile(pFilename, HOST_FILE_ACCESS_WRITE);
}

void	TZX_Write(unsigned long Nops, unsigned long Bit)
{
#if 0
	unsigned long Data;

	Data = Nops;

	Host_WriteData(TZX_Write_Handle, (unsigned char *)&Data, 4);
		
	Data = Bit;

	Host_WriteData(TZX_Write_Handle, (unsigned char *)&Data, 4);
#endif
}

void	TZX_Write_End(void)
{
#if 0
	Host_CloseFile(TZX_Write_Handle);
#endif
}



/***********************/
/* TZX write functions */
/***********************/

static unsigned char *pTZX_OutputFilename;
static HOST_FILE_HANDLE TZX_OutputFileHandle;

/* create a TZX file from data written to cassette */
void	TZX_OpenOutputFile(char *OutputFilename)
{
#if 0
	TZX_OutputFileHandle = Host_OpenFile(OutputFilename, HOST_FILE_ACCESS_WRITE);

	if (TZX_OutputFileHandle!=0)
	{
		unsigned char BlockType = 0x015;

		/* write main header */

		/* write ident text */
		Host_WriteData(TZX_OutputFileHandle, TZX_TapeHeader,8); 

		/* write block ident - Direct Recording */
		Host_WriteData(TZX_OutputFileHandle, &BlockType, 1);
		/* number of T-States per sample */

		/* pause after block in ms */

		/* used bits in last byte */

		/* data length */

		/* data */

	}
#endif
}

void	TZX_WriteData(unsigned char DataBit)
{




}
