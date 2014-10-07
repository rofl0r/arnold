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
#include "headers.h"

#include "messages.h"

static int tzx_current_block = 0;
static int tzx_num_blocks = 0;
static tzx_block_info *tzx_blocks = NULL;


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
//static unsigned long TapeImage_Flags;

/* pointer to current tzx block */
static unsigned long CurrentBlockIndex = 0;
static unsigned char *pCurrentBlockPtr = NULL;
static unsigned long NextBlockIndex = 0;

static void	TapeImage_GetNext(void);
static int		TapeImage_GetBlockLength(unsigned char *pBlock);
static BOOL TapeImage_HandleBlock(void);


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
			|| (BlockIndex == 0x016) || (BlockIndex == 0x017) 
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
//#define TAPE_IMAGE_REACHED_END	0x0001

static unsigned char Cycle=0x00;
static unsigned char BaseCycle = 0x00;


static unsigned long TapeImage_Global_TStatesRemaining = 0;
static BOOL TapeImage_LowLevelActive;
static BOOL TapeImage_HighLevelActive;


void	TapeImage_SetBlockByIndex(int Index)
{
	NextBlockIndex = Index;
	TapeImage_GetNext();
}


/* rewind back to start of tape */
void	TapeImage_RewindToStart(void)
{
	/* it's not reached the end, because it is now at the start */
//	TapeImage_Flags &= ~TAPE_IMAGE_REACHED_END;

	if (pTapeImage!=NULL)
	{		
		Cycle = 0x00;
		BaseCycle = (unsigned char)Cycle;

		TapeImage_Global_TStatesRemaining = 0;

		TapeImage_LowLevelActive  = FALSE;
		TapeImage_HighLevelActive = FALSE;
	
		TapeImage_SetBlockByIndex(0);

//		TapeImage_GetNextDataBlock(); 

//		TapeImage_HandleBlock(pTapeBlockPtr);
	}
}



#if 0
/* get next block if available */
static unsigned char *TapeImage_NextBlock(void)
{
	tzx_current_block++;

	if (tzx_current_block>tzx_num


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
#endif


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

enum
{
	TZX_STATE_BASIC_PULSE = 1,
	TZX_STATE_BASIC_PAUSE
} TZX_LOW_LEVEL_STATE_ENUM;

/* a basic tone is a cycle of the square wave, which may be repeated.
This is the basic block for tones and data */
typedef struct
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
typedef struct
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

/***************************************************/
/** TZX LOOP BLOCK HANDLING **/
typedef struct
{
	unsigned char BlockIndex;
	unsigned long LoopCount;
} TZX_LOOP;

#define TZX_MAX_NESTED_LOOPS	16

static TZX_LOOP	TZX_LoopStack[TZX_MAX_NESTED_LOOPS];
static unsigned long TZX_LoopStack_NumEntries = 0;
static TZX_LOOP TZX_CurrentLoop;

#define TZX_MAX_PUSHED_BLOCKS	16

static unsigned long TZX_BlockStack_NumEntries = 0;
static unsigned long TZX_BlockStack[TZX_MAX_PUSHED_BLOCKS];


static void TZX_LoopStack_Init(void)
{
	TZX_LoopStack_NumEntries = 0;
	TZX_CurrentLoop.LoopCount = 0;
}

static void TZX_BlockStack_Init(void)
{
	TZX_BlockStack_NumEntries = 0;
}

static void TZX_BlockStack_Push(int BlockIndex)
{
	if (TZX_BlockStack_NumEntries!=TZX_MAX_PUSHED_BLOCKS)
	{
		TZX_BlockStack[TZX_BlockStack_NumEntries] = BlockIndex;
		TZX_BlockStack_NumEntries++;
	}
}

static int TZX_BlockStack_Pop(void)
{
	if (TZX_BlockStack_NumEntries!=0)
	{
		TZX_BlockStack_NumEntries--;
		return TZX_BlockStack[TZX_BlockStack_NumEntries];
	}

	return NULL;
}


static BOOL TZX_LoopStack_Push(void)
{
	if (TZX_LoopStack_NumEntries!=TZX_MAX_NESTED_LOOPS)
	{
		memcpy(&TZX_LoopStack[TZX_LoopStack_NumEntries], &TZX_CurrentLoop,sizeof(TZX_LOOP));
		TZX_LoopStack_NumEntries++;
		return TRUE;
	}

	return FALSE;
}

static BOOL TZX_LoopStack_Pop(void)
{
	if (TZX_LoopStack_NumEntries!=0)
	{
		TZX_LoopStack_NumEntries--;

		memcpy(&TZX_CurrentLoop, &TZX_LoopStack[TZX_LoopStack_NumEntries],sizeof(TZX_LOOP));
		return TRUE;
	}

	return FALSE;
}


/* push new loop onto stack */
static void TZX_PushLoop(unsigned long BlockIndex, unsigned long LoopCount)
{
	/* push existing loop onto the stack */
	TZX_LoopStack_Push();

	TZX_CurrentLoop.LoopCount = LoopCount;
	TZX_CurrentLoop.BlockIndex = BlockIndex;
}

/* call here if loop end found */
static int TZX_UpdateLoop(void)
{
	if (TZX_CurrentLoop.LoopCount==0)
		return -1;

	/* loop is active */

	/* decrement count */
	TZX_CurrentLoop.LoopCount--;

	/* loop over? */
	if (TZX_CurrentLoop.LoopCount==0)
	{
		/* pop from stack */
		TZX_LoopStack_Pop();
		return -1;
	}
	
	/* do the loop */
	return TZX_CurrentLoop.BlockIndex;
}

/***************************************************/

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
	if	(pCurrentBlockPtr)
	{
		/* is there a low level state active? */

		if (TapeImage_LowLevelActive)
		{
			/* low level is active */
			if (TapeImage_DoLowLevelState(TStatesPassed))
			{
				if (TapeImage_DoHighLevelState())
				{
					TapeImage_GetNext();

//					TapeImage_GetNextDataBlock();
				
//					TapeImage_HandleBlock(pTapeBlockPtr);
				}

			}
		}
		else
		{
			if (TapeImage_DoHighLevelState())
			{
				TapeImage_GetNext();

//				TapeImage_GetNextDataBlock();

//				TapeImage_HandleBlock(pTapeBlockPtr);
			
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


/* returns TRUE if completed block, else FALSE */
static BOOL	TapeImage_HandleBlock0x010(void)
{
	switch (TapeImage_HighLevelState)
	{
		/* Pilot */
		case 0:
		{
			/* setup pilot tone */
			int NumCycles;

			/* pilot tone pulse width */
			BasicTone.PulseWidthInTStates = TapeImage_ScaleTStates(2168);

			if (TapeImage_GetByte(TapeImage_HighLevelStateBlockPtr + 0x04)==0)
			{
				/* sync byte of 0 indicates a data-block */
				NumCycles = 3220;
			}
			else
			{
				NumCycles = 8064;
			}

			/* pilot tone num cycles */
			BasicTone.NumCycles = NumCycles;

			/* set low-level state */
			TapeImage_SetLowLevelState(TZX_STATE_BASIC_PULSE, &BasicTone);
		
			TapeImage_HighLevelState++;
		}
		break;

		/* sync pulse 1 */
		case 1:
		{
			/* setup sync pulse 1 tone */
			BasicTone.PulseWidthInTStates = TapeImage_ScaleTStates(667);
			/* 1 cycle */
			BasicTone.NumCycles = 1;
			
			TapeImage_SetLowLevelState(TZX_STATE_BASIC_PULSE, &BasicTone);
		
			TapeImage_HighLevelState++;
		}
		break;

		/* sync pulse 2*/
		case 2:
		{
			/* setup sync pulse 2 tone */
			BasicTone.PulseWidthInTStates = TapeImage_ScaleTStates(735);
			BasicTone.NumCycles = 1;

			TapeImage_SetLowLevelState(TZX_STATE_BASIC_PULSE, &BasicTone);
		
			TapeImage_HighLevelState++;
		
			/**** SETUP FOR DATA ****/

			/* length of one bit */
			TapeImage_LengthOfOneBitPulseInTStates = TapeImage_ScaleTStates(1710);	
			
			/* length of zero bit */
			TapeImage_LengthOfZeroBitPulseInTStates = TapeImage_ScaleTStates(855);	
			
			/* get length of data */
			TapeImage_BytesRemaining = 
				TapeImage_GetWord(TapeImage_HighLevelStateBlockPtr + 0x02);
			
			/* get number of bits used in last byte */
			TapeImage_NumBitsUsedInLastByte = 8;
			
			/* set data ptr */
			TapeImage_pData = TapeImage_HighLevelStateBlockPtr + 0x04;
			
			TapeImage_InitForHandleData();

		}
		break;

		
		/* data */
		case 3:
		{
			TapeImage_HandleData();
		}
		break;


		case 4:
		{
			unsigned long Pause;

			Pause = TapeImage_GetWord(TapeImage_HighLevelStateBlockPtr + 0x00);

			if (Pause!=0)
			{
				BasicPause.PauseInTStates = TapeImage_MillisecondsToTStates(Pause);
			
				TapeImage_SetLowLevelState(TZX_STATE_BASIC_PAUSE, &BasicPause);
			
				TapeImage_HighLevelState++;
			}
			else
			{
				return TRUE;
			}

		}
		break;

		/* completed block */
		case 5:
		{
			return TRUE;

		}
	}

	/* not handled yet! */
	return FALSE;
}





/* returns TRUE if completed block, else FALSE */
static BOOL TapeImage_HandleBlock0x011(void)
{
	switch (TapeImage_HighLevelState)
	{
		/* Pilot */
		case 0:
		{
			/* setup pilot tone */

			/* pilot tone pulse width */
			BasicTone.PulseWidthInTStates = 
				TapeImage_ScaleTStates(TapeImage_GetWord(TapeImage_HighLevelStateBlockPtr+0));
			/* pilot tone num cycles */
			BasicTone.NumCycles = 
				TapeImage_GetWord(TapeImage_HighLevelStateBlockPtr+0x0a);
			
			/* set low-level state */
			TapeImage_SetLowLevelState(TZX_STATE_BASIC_PULSE, &BasicTone);
		
			TapeImage_HighLevelState++;
		}
		break;

		/* sync pulse 1 */
		case 1:
		{
			/* setup sync pulse 1 tone */
			BasicTone.PulseWidthInTStates = 
				TapeImage_ScaleTStates(TapeImage_GetWord(TapeImage_HighLevelStateBlockPtr+2));
			/* 1 cycle */
			BasicTone.NumCycles = 1;
			
			TapeImage_SetLowLevelState(TZX_STATE_BASIC_PULSE, &BasicTone);
		
			TapeImage_HighLevelState++;
		}
		break;

		/* sync pulse 2*/
		case 2:
		{
			/* setup sync pulse 2 tone */
			BasicTone.PulseWidthInTStates = 
				TapeImage_ScaleTStates(TapeImage_GetWord(TapeImage_HighLevelStateBlockPtr + 4));
			BasicTone.NumCycles = 1;

			TapeImage_SetLowLevelState(TZX_STATE_BASIC_PULSE, &BasicTone);
		
			TapeImage_HighLevelState++;
		
			/**** SETUP FOR DATA ****/

			/* length of one bit */
			TapeImage_LengthOfOneBitPulseInTStates = 
				TapeImage_ScaleTStates(TapeImage_GetWord(TapeImage_HighLevelStateBlockPtr + 0x08));

			/* length of zero bit */
			TapeImage_LengthOfZeroBitPulseInTStates = 
				TapeImage_ScaleTStates(TapeImage_GetWord(TapeImage_HighLevelStateBlockPtr + 0x06));
			
			/* get length of data */
			TapeImage_BytesRemaining = 
				TapeImage_GetValue(TapeImage_HighLevelStateBlockPtr + 0x0f, 3);
			
			/* get number of bits used in last byte */
			TapeImage_NumBitsUsedInLastByte = 
				TapeImage_GetByte(TapeImage_HighLevelStateBlockPtr + 0x0c);
			
			/* set data ptr */
			TapeImage_pData = TapeImage_HighLevelStateBlockPtr + 0x012;
			
			TapeImage_InitForHandleData();

		}
		break;

		
		/* data */
		case 3:
		{
			TapeImage_HandleData();
		}
		break;

		/* pause */
		case 4:
		{
			unsigned long Pause;

			Pause = TapeImage_GetWord(TapeImage_HighLevelStateBlockPtr + 0x0d);

			if (Pause!=0)
			{
				BasicPause.PauseInTStates = TapeImage_MillisecondsToTStates(Pause);
			
				TapeImage_SetLowLevelState(TZX_STATE_BASIC_PAUSE, &BasicPause);
			
				TapeImage_HighLevelState++;
			}
			else
			{
				return TRUE;
			}

		}
		break;

		/* completed block */
		case 5:
		{
			return TRUE;
		}
		break;
	}

	return FALSE;
}

/* Handle Block 0x012 - Pure Tone */
static BOOL	TapeImage_HandleBlock0x012(void)
{
	switch (TapeImage_HighLevelState)
	{
		case 0:
		{
			/* set pulse width */
			BasicTone.PulseWidthInTStates = 
				TapeImage_ScaleTStates(TapeImage_GetWord(TapeImage_HighLevelStateBlockPtr+0));

			/* set num cycles */
			BasicTone.NumCycles = 
				TapeImage_GetWord(TapeImage_HighLevelStateBlockPtr+2);

			/* set low-level state */
			TapeImage_SetLowLevelState(TZX_STATE_BASIC_PULSE, &BasicTone);

			TapeImage_HighLevelState++;
		}
		break;

		case 1:
		{
			return TRUE;
		}
	}

	return FALSE;
}

/* Handle Block 0x013 - Sequence of Pulses of different lengths */
static BOOL	TapeImage_HandleBlock0x013(void)
{
	unsigned long NumPulses;

	/* get number of pulses */
	NumPulses = TapeImage_GetByte(TapeImage_HighLevelStateBlockPtr);

	if (TapeImage_HighLevelState==NumPulses)
	{
		return TRUE;
	}

	/* set pulse width */
	BasicTone.PulseWidthInTStates =  
		TapeImage_ScaleTStates(
		TapeImage_GetWord(TapeImage_HighLevelStateBlockPtr+(TapeImage_HighLevelState<<1)+1));

	/* set num cycles */
	BasicTone.NumCycles = 1;

	/* set low-level state */
	TapeImage_SetLowLevelState(TZX_STATE_BASIC_PULSE, &BasicTone);

	TapeImage_HighLevelState++;

	return FALSE;
}

/* Handle Block 0x014 - Pure Data*/
static BOOL	TapeImage_HandleBlock0x014(void)
{
	switch (TapeImage_HighLevelState)
	{

		case 0:
		{
		
			/**** SETUP FOR DATA ****/

			/* length of one bit */
			TapeImage_LengthOfOneBitPulseInTStates = 
				TapeImage_ScaleTStates(TapeImage_GetWord(TapeImage_HighLevelStateBlockPtr + 0x02));

			/* length of zero bit */
			TapeImage_LengthOfZeroBitPulseInTStates = 
				TapeImage_ScaleTStates(TapeImage_GetWord(TapeImage_HighLevelStateBlockPtr + 0x00));
			
			/* get length of data */
			TapeImage_BytesRemaining = 
				TapeImage_GetValue(TapeImage_HighLevelStateBlockPtr + 0x07, 3);
			
			/* get number of bits used in last byte */
			TapeImage_NumBitsUsedInLastByte = 
				TapeImage_GetByte(TapeImage_HighLevelStateBlockPtr + 0x04);
			
			/* set data ptr */
			TapeImage_pData = TapeImage_HighLevelStateBlockPtr + 0x0a;
			
			TapeImage_InitForHandleData();

			TapeImage_HandleData();

			TapeImage_HighLevelState++;
		}
		break;

		case 1:
		{
			TapeImage_HandleData();
		}
		break;

		case 2:
		{
			unsigned long Pause;

			Pause = TapeImage_GetWord(TapeImage_HighLevelStateBlockPtr + 0x05);

			if (Pause!=0)
			{
				BasicPause.PauseInTStates = TapeImage_MillisecondsToTStates(Pause);
			
				TapeImage_SetLowLevelState(TZX_STATE_BASIC_PAUSE, &BasicPause);
			
				TapeImage_HighLevelState++;
			}
			else
			{
				return TRUE;
			}

		}
		break;

		case 3:
		{
			return TRUE;

		}
	}

	return FALSE;
}

/* returns TRUE if completed block, else FALSE */
static BOOL TapeImage_HandleBlock0x015(void)
{
	switch (TapeImage_HighLevelState)
	{
	}
	return FALSE;
}


/* pause */
static BOOL TapeImage_HandleBlock0x020(void)
{
	switch (TapeImage_HighLevelState)
	{
		case 0:
		{
			unsigned long Pause;

			Pause = TapeImage_GetWord(TapeImage_HighLevelStateBlockPtr + 0);

			if (Pause!=0)
			{
				/* convert pause in milliseconds to t-states */
				BasicPause.PauseInTStates = TapeImage_MillisecondsToTStates(Pause);
			
				TapeImage_SetLowLevelState(TZX_STATE_BASIC_PAUSE, &BasicPause);
			
				TapeImage_HighLevelState++;
			}
			else
			{
				return TRUE;
			}
		}
		break;

		case 1:
		{
			return TRUE;
		}
	}

	return FALSE;
}

		


/* update tape position based on tstates passed and return a bit */
int TapeImage_GetBit(int TStatesPassed)
{
	Cycle = 0x01;

	if (pTapeImage!=NULL)
	{
		if (pCurrentBlockPtr)
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
}


/*
static char	*TapeImage_ArchiveInfo_GetTextID(int Byte)
{
	switch (Byte)
	{
		case 00:
		{
			return Messages[108];
		}

		case 01:
		{
			return Messages[109];
		}

		case 02:
		{
			return Messages[110];
		}

		case 03:
		{
			return Messages[111];
		}

		case 04:
		{
			return Messages[112];
		}

		case 0x0ff:
		{
			return Messages[113];
		}

		default:
		{
			return Messages[114];
		}
	}
}
*/
int GetBlockIndexFromSignedOffset(int Base, signed short Offset)
{
	unsigned long destination_block;

	destination_block=Base+Offset;

	if (destination_block<0)
	{
		destination_block = 0;
	}
	else
	if (destination_block>tzx_num_blocks)
	{
		destination_block = tzx_num_blocks-1;
	}

	return destination_block;
}


/******************/
/* Handle a block */
static BOOL TapeImage_HandleBlock(void)
{
	unsigned char *pBlockPtr;
	int	BlockID;
	unsigned long BlockLength;

	/* over end of file? */
	if (CurrentBlockIndex==tzx_num_blocks)
	{
		pCurrentBlockPtr = NULL;

		/* no continue */
		return FALSE;
	}

	/* setup ptr for this block */
	pBlockPtr = tzx_blocks[CurrentBlockIndex].pBlockPtr;
	pCurrentBlockPtr = pBlockPtr;

	/* get ID of this block */
	BlockID = TapeImage_GetByte(pBlockPtr);

	/* calculate index of next block; ignores jump's etc */
	NextBlockIndex++;

	/* get the length of this block */
	BlockLength = TapeImage_GetBlockLength(pBlockPtr);

	/* get pointer to actual block data */
	pBlockPtr++;

	switch (BlockID)
	{
		/* standard speed data block */
		case 0x010:
		{
			TapeImage_SetHighLevelState(TapeImage_HandleBlock0x010, pBlockPtr);
		
			/* no continue as this is a data block */
		}
		return FALSE;

		/* Turbo loading data block */
		case 0x011:
		{
			TapeImage_SetHighLevelState(TapeImage_HandleBlock0x011,pBlockPtr);
		
			/* no continue as this is a data block */
		}
		return FALSE;

		/* Pure Tone */
		case 0x012:
		{
			TapeImage_SetHighLevelState(TapeImage_HandleBlock0x012, pBlockPtr);	

			/* no continue as this is a data block */
		}
		return FALSE;

		/* Sequence of pulses of different lengths */
		case 0x013:
		{
			TapeImage_SetHighLevelState(TapeImage_HandleBlock0x013, pBlockPtr);

			/* no continue as this is a data block */
		}
		return FALSE;

		/* Pure Data */
		case 0x014:
		{
			TapeImage_SetHighLevelState(TapeImage_HandleBlock0x014, pBlockPtr);

			/* no continue as this is a data block */
		}
		return FALSE;
		
		/* direct recording */
		case 0x015:
		{
			TapeImage_SetHighLevelState(TapeImage_HandleBlock0x015, pBlockPtr);

			/* no continue as this is a data block */
		}
		return FALSE;

		/* not handled yet */
		case 0x016:
			return FALSE;
		case 0x017:
			return FALSE;

		/* pause/stop the tape */
		case 0x020:
		{
			TapeImage_SetHighLevelState(TapeImage_HandleBlock0x020, pBlockPtr);

			/* no continue as this is a data block */
		}
		return FALSE;

		/* jump to block */
		case 0x023:
		{
			signed short Offset = TapeImage_GetWord(pBlockPtr);
			
			/* infinite loop? */
			if (Offset==0)
			{
				/* stop here! */

				pCurrentBlockPtr = NULL;

				/* no continue */
				return FALSE;
			}
			else
			{
				/* otherwise continue */
				NextBlockIndex = GetBlockIndexFromSignedOffset(CurrentBlockIndex, Offset);
			}
		}
		break;

		/* Loop Start */
		case 0x024:
		{
			int RepetitionCount;

			RepetitionCount = TapeImage_GetWord(pBlockPtr);

			TZX_PushLoop(NextBlockIndex, RepetitionCount);

			/* continue */
		}
		break;

		/* Loop End */
		case 0x025:
		{
			int DestBlockIndex;

			DestBlockIndex = TZX_UpdateLoop();

			if (DestBlockIndex!=-1)
			{
				NextBlockIndex = DestBlockIndex;
			}

			/* continue */
		}
		break;

//		case 0x02a:
//			break;

//		case 0x030:
//			break;

//		case 0x031:
//			break;

//		case 0x032:
//			break;

//		case 0x033:
//			break;

//		case 0x034:
//			break;

//		case 0x035:
//			break;

//		case 0x040:
//			break;

		/* call sequence */
		case 0x026:
		{
			int i;
			int DestBlockIndex;
			unsigned short NumBlocks;
			
			NumBlocks = TapeImage_GetWord(pBlockPtr);

			TZX_BlockStack_Push(NextBlockIndex);

			for (i=NumBlocks-1; i>=0; i--)
			{
				DestBlockIndex = GetBlockIndexFromSignedOffset(CurrentBlockIndex, (signed short)TapeImage_GetWord(pBlockPtr+2+(i<<1)));

				/* push the blocks onto the stack */
				TZX_BlockStack_Push(DestBlockIndex);
			}
			

			/* pop next call from list */
			DestBlockIndex = TZX_BlockStack_Pop();

			if (DestBlockIndex!=-1)
			{
				NextBlockIndex = DestBlockIndex;
			}

			/* continue */
		}
		break;

		/* return from call sequence */
		case 0x027:
		{
			int DestBlockIndex;

			/* pop next call from list */
			DestBlockIndex = TZX_BlockStack_Pop();

			if (DestBlockIndex!=-1)
			{
				NextBlockIndex = DestBlockIndex;
			}

			/* continue */
		}
		break;

		case 0x021:
		case 0x022:
		default:
			break;

	}

	return TRUE;
}

static int		TapeImage_GetBlockLength(unsigned char *pBlock)
{
	unsigned char *pBlockPtr = pBlock;
	unsigned char BlockIndex;
	unsigned long BlockLength = 0;

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

		/* block's following the extension rule */
		default:
		{
			/* get block length */
			BlockLength = TapeImage_GetValue(pBlockPtr,4);

			return BlockLength + 4 + 1;
		}
		break;

	}
}

int		TapeImage_GetCurrentBlock(void)
{
	return CurrentBlockIndex;
}

int		TapeImage_GetNumBlocks(void)
{
	return tzx_num_blocks;
}

int		TapeImage_GetBlockID(int BlockIndex)
{
	unsigned char *pBlockPtr = tzx_blocks[BlockIndex].pBlockPtr;

	return 	TapeImage_GetByte(pBlockPtr);
}

const char *TapeImage_GetBlockDescription(int BlockID)
{
	switch (BlockID)
	{
		/* standard speed data block */
		case 0x010:
			return "Standard Speed Data Block";
		
		/* Turbo loading data block */
		case 0x011:
			return "Turbo Loading Data Block";
	
		/* pure tone */
		case 0x012:
			return "Pure Tone";

		/* pulses of different lengths */
		case 0x013:
			return "Pulses of Different Lengths";

		/* pure data */
		case 0x014:
			return "Pure Data";

		/* direct recording */
		case 0x015:
			return "Direct Recording";
		case 0x016:
			return "C64 ROM Type Data Block";
		case 0x017:
			return "C64 Turbo Tape Data Block";
		/* stop tape/pause */
		case 0x020:
			return "Pause or 'Stop the Tape'";
		/* group start */
		case 0x021:
			return "Group Start";
			
		/* group end */
		case 0x022:
			return "Group End";


		/* jump to block */
		case 0x023:
			return "Jump To Block";

		/* loop start */
		case 0x024:
			return "Loop Start";
		
		/* loop end */
		case 0x025:
			return "Loop End";
			
		/* call sequence */
		case 0x026:
			return "Call Sequence";

		/* return from sequence */
		case 0x027:
			return "Return from Sequence";

		/* select block */
		case 0x028:
			return "Select Block";

		/* stop tape if in 48k mode */
		case 0x02a:
			return "Spectrum: Stop tape if in 48k mode";
			
		/* Text Description */
		case 0x030:
			return "Text Description";

		/*  Message block */
		case 0x031:
			return "Message Block";

		/* "Archive Info" */
		case 0x032:
			return "Archive Info";
			
		/* "Hardware Type */
		case 0x033:
			return "Hardware Type";

		/* emulation info */
		case 0x034:
			return "Emulation Info";
			
		/* custom info block */
		case 0x035:
			return "Custom Info";
			
		/* snapshot block */
		case 0x040:
			return "Spectrum Snapshot Block";

		/* joined tape's */
		case 'Z':
			return "Header of joined tape";

		/* block's following the extension rule */
		default:
			return "Extension Block";
	}
}



/* a block is assumed to be valid if it's size is within the end
of the tape file */
static BOOL		TapeImage_BlockIsValid(unsigned char *pBlockPtr, unsigned char *pTapeImageEnd, unsigned long TapeImageSize)
{
	unsigned long BlockLength;

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

	return FALSE;
}




/* validate a tape image */
static BOOL	TapeImage_Validate(unsigned char *pTapeImage, unsigned long TapeImageSize)
{
	BOOL Validity = FALSE;

	if (memcmp(pTapeImage,TZX_TapeHeader,7)==0)
	{
		/* main header is valid */
		unsigned char *pBlockPtr;		

		/* calc end */
		pTapeImageEnd = pTapeImage + TapeImageSize;
		

		/* 1st block */
		pBlockPtr = pTapeImage + 10;

		/* if block has no data, then report an error */
		if (pBlockPtr==pTapeImageEnd)
			return FALSE;

		do
		{
			if (!(TapeImage_BlockIsValid(pBlockPtr,pTapeImageEnd,TapeImageSize)))
			{
				/* block appears to be in-valid */
				break;
			}
			else
			{
				/* go to next block */
				pBlockPtr += TapeImage_GetBlockLength(pBlockPtr);
			}		
		}
		while (pBlockPtr<pTapeImageEnd);

		/* if block ptr is not near end of tape, therefore
			this block must be invalid - it's size is too big
			and extends over the end of the tape image - attempting
			to read this tape image could cause problems */

		if (pBlockPtr>=pTapeImageEnd)
		{
			Validity = TRUE;
		}
	}

	return Validity;
}

static void	TapeImage_GetNext(void)
{
	BOOL bContinue = FALSE;

	do
	{
		/* setup index of current block */
		CurrentBlockIndex = NextBlockIndex;

		/* process block, and return TRUE if we should loop */
		bContinue = TapeImage_HandleBlock();
	}
	while (bContinue);
}

static void TapeImage_Setup(unsigned char *pTapeImage)
{
	/* main header is valid */
	unsigned char *pBlockPtr;		

	tzx_num_blocks = 0;

	/* calc end */
	pTapeImageEnd = pTapeImage + TapeImageSize;
	
	/* 1st block */
	pBlockPtr = pTapeImage + 10;

	/* if block has no data, then report an error */
	if (pBlockPtr!=pTapeImageEnd)
	{
		do
		{
			tzx_num_blocks++;

			/* go to next block */
			pBlockPtr += TapeImage_GetBlockLength(pBlockPtr);		
		}
		while (pBlockPtr<pTapeImageEnd);
	}

	if (tzx_num_blocks==0)
		return;

	tzx_blocks = (tzx_block_info *)malloc(sizeof(tzx_block_info)*tzx_num_blocks);
	
	if (tzx_blocks!=NULL)
	{
		tzx_num_blocks = 0;

		/* 1st block */
		pBlockPtr = pTapeImage + 10;

		do
		{
			tzx_blocks[tzx_num_blocks].pBlockPtr = pBlockPtr;
			tzx_num_blocks++;

			/* go to next block */
			pBlockPtr += TapeImage_GetBlockLength(pBlockPtr);		
		}
		while (pBlockPtr<pTapeImageEnd);
	}
	else
	{
		tzx_num_blocks = 0;
	}

}


/* insert a tape image */
int TapeImage_Insert(const unsigned char *pTapeImageData, const unsigned long TapeImageDataSize)
{
	pTapeImage = malloc(TapeImageDataSize);

	if (pTapeImage!=NULL)
	{
		memcpy(pTapeImage, pTapeImageData, TapeImageDataSize);
		TapeImageSize = TapeImageDataSize;

		/* attempt to load this tape image */
		if (TapeImage_Validate(pTapeImage,TapeImageSize))
		{		
			/* calc end */
			pTapeImageEnd = pTapeImage + TapeImageSize;

			TapeImage_Setup(pTapeImage);


			TapeImage_RewindToStart();

			CPC_SetCassetteType(CASSETTE_TYPE_TAPE_IMAGE);

			return ARNOLD_STATUS_OK;
		}
	}

	TapeImage_Remove();

	/* not valid, keep with current tape image */
	return ARNOLD_STATUS_UNRECOGNISED;
}


void	TapeImage_Remove(void)
{
	TZX_LoopStack_Init();

	TZX_BlockStack_Init();


	if (tzx_blocks!=NULL)
	{
		free(tzx_blocks);
		tzx_blocks = NULL;
	}

	tzx_num_blocks = 0;

	/* is tape image inserted? */
	if (pTapeImage!=NULL)
	{
		free(pTapeImage);
		pTapeImage = NULL;
	}
}

#if 0
static HOST_FILE_HANDLE TZX_Write_Handle;

void	TZX_Write_Initialise(char *pFilename)
{
	TZX_Write_Handle = Host_OpenFile(pFilename, HOST_FILE_ACCESS_WRITE);
}

void	TZX_Write(unsigned long Nops, unsigned long Bit)
{
	unsigned long Data;

	Data = Nops;

	Host_WriteData(TZX_Write_Handle, (unsigned char *)&Data, 4);
		
	Data = Bit;

	Host_WriteData(TZX_Write_Handle, (unsigned char *)&Data, 4);
}

void	TZX_Write_End(void)
{
	Host_CloseFile(TZX_Write_Handle);
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
#endif
