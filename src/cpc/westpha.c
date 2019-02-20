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
/* THIS IS NOT COMPLETE - THE FORMAT OF THE DATA STREAM FROM THE WESTPHASER IS NOT
KNOWN AND THEREFORE IT DOES NOT WORK PROPERLY. IF ANYONE HAS ONE OF THESE LIGHTGUN'S
PLEASE COULD THEY CONTACT ME SO I CAN WRITE A PROGRAM TO DUMP THE INFORMATION AND GET
AN IDEA OF WHAT SHOULD HAPPEN! */
#include "cpc.h"
#include "westpha.h"
#include <memory.h>
#include <string.h>

/* NOP count before changing to next state */
static int StateCount;
/* we fill in the joystick line */
extern int KeyboardData[20];

static unsigned char WestPhaser_DataBuffer[80];

static int WestPhaser_DataBufferSize = 80;	
static int WestPhaser_DataBytesRemaining;
static unsigned char *pWestPhaser_Data;
static void WestPhaser_DoTrigger(void);
static int LightRegisteredDelay;

void	WestPhaser_SetCoordinates(int X, int Y)
{
	/* this delay between 0 and 19968 nops?? */
	LightRegisteredDelay = 19968;

/*	LightRegisteredDelay = (Y*64) + X;

	memset(WestPhaser_DataBuffer,0x01,64);
	WestPhaser_DataBuffer[80-79]&=0x0fe;
	WestPhaser_DataBuffer[80-62]&=0x0fe;
	WestPhaser_DataBuffer[80-46]&=0x0fe;
	WestPhaser_DataBuffer[80-30]&=0x0fe;
	WestPhaser_DataBuffer[80-16]&=0x0fe;
*/
}

#define JOYSTICK_LINE 9


unsigned long PreviousTriggerState = 0x0ff;

void	WestPhaser_Trigger()
{
	if (((PreviousTriggerState^KeyboardData[9]) & (1<<4))!=0)
	{
		if ((KeyboardData[9] & (1<<4))!=0)
		{
			/* released */
			WestPhaser_DoTrigger();
		}
	}

	PreviousTriggerState = KeyboardData[9];

}

typedef enum 
{
	WESTPHASER_IDLE = 0,
	WESTPHASER_TRIGGER_PRESSED,
	WESTPHASER_LIGHT_REGISTERED,
	WESTPHASER_DATA_DELAY,
	WESTPHASER_SYNC_1,
	WESTPHASER_SYNC_2,
	WESTPHASER_SEND_DATA
} WESTPHASER_ENUM;

static WESTPHASER_ENUM WestPhaserState = WESTPHASER_IDLE;

static void	WestPhaser_DoTrigger(void)
{
	if (WestPhaserState==WESTPHASER_IDLE)
	{
		CPC_SetKey(CPC_KEY_JOY_FIRE2);
		WestPhaserState = WESTPHASER_TRIGGER_PRESSED;
		StateCount = 0;
	}
}

static int	WestPhaser_GetState(void)
{
	return WestPhaserState;
}

static int	WestPhaser_GetStateCount(void)
{
	return StateCount;
}


static int WestPhaser_DBRem(void)
{
	return WestPhaser_DataBytesRemaining;
}

void	WestPhaser_Update(int NopCount)
{
	if (WestPhaserState!=WESTPHASER_IDLE)
	{
		if (StateCount>=NopCount)
		{
			StateCount-=NopCount;
		}
		else
		{
			int NopsRemaining = NopCount - StateCount;

			do
			{
	
				/* here state count is either 0 or -ve */

				/* change state to new one */
				switch (WestPhaserState)
				{
					case WESTPHASER_TRIGGER_PRESSED:
					{
						WestPhaserState = WESTPHASER_LIGHT_REGISTERED;

						/* just finished trigger */
					/*	WestPhaserState = WESTPHASER_SYNC_1; */
					}
					break;

					case WESTPHASER_LIGHT_REGISTERED:
					{
						WestPhaserState = WESTPHASER_SYNC_1;
					}
					break;


					case WESTPHASER_SYNC_1:
					{
						/* just finished sync 1 */
						WestPhaserState = WESTPHASER_SYNC_2;
					}
					break;

					case WESTPHASER_SYNC_2:
					{
						WestPhaser_DataBytesRemaining = WestPhaser_DataBufferSize;
						pWestPhaser_Data = WestPhaser_DataBuffer;
						
						/* just finished sync 2 */
						WestPhaserState = WESTPHASER_SEND_DATA;
					}
					break;

					case WESTPHASER_SEND_DATA:
					{
						if (WestPhaser_DataBytesRemaining==0)
						{
							WestPhaserState = WESTPHASER_IDLE;
						}
					}
					break;

                                        default:
                                                break;
				}

				/* setup for new state */
				switch (WestPhaserState)
				{
					case WESTPHASER_LIGHT_REGISTERED:
					{
						StateCount = LightRegisteredDelay;	
					}
					break;

					case WESTPHASER_SYNC_1:
					{
						/* bit 0 set to 1 */
						CPC_SetKey(CPC_KEY_JOY_UP);

						/* this bit is the problem!!!! How long does this stay active for? */
						StateCount = 35;
					}
					break;

					case WESTPHASER_SYNC_2:
					{
						/* bit 0 set to 0 */
						CPC_SetKey(CPC_KEY_JOY_UP);

						/* 
							.l1
							in a,(c)		[4]
							rrca			[1]
							jr nz,l1		[3] if taken [2] = not taken
							ld d,7			[2]
							.p1 dec d		[1]
							jr nc,p1		[3] if taken [2] = not taken
							
							in c,(c)		

							[4] + [1] + [2] + [2] + (([1]+[3])*6) + [1] + [2] = 36

						*/

						StateCount = 36;

					}
					break;

					case WESTPHASER_SEND_DATA:
					{
						KeyboardData[JOYSTICK_LINE] = pWestPhaser_Data[0];
						
						pWestPhaser_Data++;
						WestPhaser_DataBytesRemaining--;
						StateCount = 4;
					}
					break;

					case WESTPHASER_IDLE:
					{
						KeyboardData[JOYSTICK_LINE]=(KeyboardData[JOYSTICK_LINE] & 0x080) | 0x07f;
					}
					break;

                                        default:
                                                break;
				}

				if (NopsRemaining<StateCount)
				{
					StateCount -= NopsRemaining;
					break;
				}
				else
				{
					NopsRemaining -= StateCount;
				}
			}
			while (WestPhaserState!=WESTPHASER_IDLE);
		
		}
	}
}
static int Keyboard_PreviousNopCount;

unsigned char	WestPhaser_ReadJoystickPort(void)
{
    int CurrentNopCount = CPC_GetNopCount();
    int NopsPassed;

    NopsPassed = CurrentNopCount - Keyboard_PreviousNopCount;

    WestPhaser_Update(NopsPassed);

    Keyboard_PreviousNopCount = CurrentNopCount;

	return 0x0ff;
}
