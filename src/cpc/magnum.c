#include "magnum.h"
#include "cpc.h"

/**** MAGNUM LIGHT PHASER ****/

static int Magnum_FirePressed = FALSE;
static unsigned char Magnum_PreviousByteWritten = 0x07f;

void	Magnum_Update(int X, int Y, int FirePressed)
{
	Magnum_FirePressed = FirePressed;

	if (Magnum_FirePressed)
	{
//		CRTC_LightPen_Trigger(X,Y);
	}
}

void	Magnum_DoOut(Z80_WORD Port, Z80_BYTE Data)
{
	/* when a write is done to this port, it signifies that the
	magnum should trigger the LPEN input to signify the fire button is pressed */
	if (Port==0x0fbfe)
	{
		if (((Data^Magnum_PreviousByteWritten) & 0x080)!=0)
		{
			/* bit changed state */
			if ((Data & 0x080)!=0)
			{
				/* if the data is the same when read, LPEN has not been triggered,
				 and therefore the fire button is pressed.
				*/
				if (!(Magnum_FirePressed))
				{
					/* not pressed, trigger LPEN. */

					/* number of cycles before lpen is triggered... */
				//	CRTC_LightPen_Trigger(1);
				}
			}

		}
			
		Magnum_PreviousByteWritten = Data;
	}
}
