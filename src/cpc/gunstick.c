#include "gunstick.h"
#include "cpc.h"

//static unsigned long SpanishLightGun_CyclesToTrigger = 0;
//static BOOL		SpanishLightGun_DoTrigger = FALSE;
//static unsigned long SpanishLightGun_PreviousNopCount = 0;
//extern unsigned char CRTCRegisters[32];
//extern CRTC_INTERNAL_STATE CRTC_InternalState;

/**** SPANISH LIGHT GUN ****/
void	SpanishLightGun_Update(int X, int Y, int Button)
{
	
	if (Button)
	{
//		CRTC_LightPen_Trigger(X,Y);

//		SpanishLightGun_CyclesToTrigger = Nops;
//		SpanishLightGun_PreviousNopCount = CPC_GetNopCount();
//		SpanishLightGun_DoTrigger = TRUE;
	}		

}
#if 0
unsigned char SpanishLightGun_ReadJoystickPort(void)
{
	unsigned char Data = 0x0ff;

	if (SpanishLightGun_DoTrigger)
	{
	    int CurrentNopCount = CPC_GetNopCount();
	    int NopsPassed;

	    NopsPassed = CurrentNopCount - SpanishLightGun_PreviousNopCount;
		
		if (NopsPassed>=SpanishLightGun_CyclesToTrigger)
		{
			SpanishLightGun_DoTrigger = 0;

			Data &= ~2;
		}

		SpanishLightGun_CyclesToTrigger -= NopsPassed;


		SpanishLightGun_PreviousNopCount = CurrentNopCount;
	
		Data &= ~(16|32);
	}

	return Data;
}
#endif

void	SpanishLightGun_Reset(void)
{
//	SpanishLightGun_PreviousNopCount = 0;
}
