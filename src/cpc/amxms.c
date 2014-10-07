#include "amxms.h"
#include "cpc.h"

static unsigned long ActiveDelay;
static int AmxMouse_DelayActive = 0;
static unsigned char AmxMouse_Current;
static unsigned char AmxMouse_Next;
static unsigned char AmxMouse_MovementData;

static int AmxMouse_PreviousNopCount = 0;

unsigned char AmxMouse_ReadJoystickPort(void)
{
	int CurrentNopCount = CPC_GetNopCount();
	int NopsPassed;

	NopsPassed = CurrentNopCount - AmxMouse_PreviousNopCount;

	AmxMouse_UpdateMovement(NopsPassed);

	AmxMouse_PreviousNopCount = CurrentNopCount;

	return AmxMouse_Current;
}

void	AmxMouse_UpdateMovement(unsigned long NopsPassed)
{
	if (AmxMouse_DelayActive)
	{
		if (NopsPassed>=ActiveDelay)
		{
			/* turn off directions */

			AmxMouse_Current |= AmxMouse_MovementData;

			AmxMouse_DelayActive = 0;
		}
		
		ActiveDelay = ActiveDelay - NopsPassed;
	}


}



void	AmxMouse_Update(int DeltaX, int DeltaY, int LeftButton, int RightButton)
{
	AmxMouse_MovementData = 0x00;

	/* left */
	if (DeltaX<0)
	{
		ActiveDelay = (-DeltaX)*3328;

		AmxMouse_MovementData |= 1<<(CPC_KEY_JOY_LEFT & 0x07);
	}
	else if (DeltaX>0)
	{
		ActiveDelay = (DeltaX)*3328;

		AmxMouse_MovementData |= 1<<(CPC_KEY_JOY_RIGHT & 0x07);
	}

	/* up */
	if (DeltaY<0)
	{
		ActiveDelay = (-DeltaY)*3328;

		AmxMouse_MovementData |= 1<<(CPC_KEY_JOY_UP & 0x07);
	}
	else if (DeltaY>0)
	{
		ActiveDelay = (DeltaY)*3328;

		AmxMouse_MovementData |= 1<<(CPC_KEY_JOY_DOWN & 0x07);
	}

	if ((AmxMouse_MovementData & 0x0f)!=0)
	{
		AmxMouse_DelayActive = 1;
	}


	AmxMouse_Current = 0x0ff^AmxMouse_MovementData;

	
	/* fire */
	if (LeftButton)
	{
		AmxMouse_MovementData &= ~(1<<4);
		AmxMouse_Current &= ~(1<<4);
	}
	
	/* fire 2 */
	if (RightButton)
	{
		AmxMouse_MovementData &= ~(1<<5);
		AmxMouse_Current &= ~(1<<5);
	}
}

void	AmxMouse_Reset(void)
{
	AmxMouse_PreviousNopCount = 0;
}