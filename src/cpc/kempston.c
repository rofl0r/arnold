#include "kempston.h"
#include "cpc.h"

static unsigned char KempstonMouse_Y = 0;
static unsigned char KempstonMouse_X = 0;
static unsigned char KempstonMouse_Buttons = 0x0ff;

void	KempstonMouse_Update(int DeltaX, int DeltaY, int LeftButton, int RightButton)
{
	KempstonMouse_X += DeltaX;
	KempstonMouse_Y += -DeltaY;
	KempstonMouse_Buttons = 0x0ff;

	if (LeftButton)
	{
		KempstonMouse_Buttons &= ~(1<<0);
	}

	if (RightButton)
	{
		KempstonMouse_Buttons &= ~(1<<1);
	}

}

unsigned char KempstonMouse_ReadX(Z80_WORD Port)
{
	return KempstonMouse_X;
}

unsigned char KempstonMouse_ReadY(Z80_WORD Port)
{
	return KempstonMouse_Y;
}

unsigned char KempstonMouse_ReadButtons(Z80_WORD Port)
{
	return KempstonMouse_Buttons;
}


CPCPortRead kempstonReadPorts[3]=
{
	{0x0fbee, 0x0fbee, KempstonMouse_ReadX},
	{0x0fbef, 0x0fbef, KempstonMouse_ReadY},
	{0x0faef, 0x0faef, KempstonMouse_ReadButtons},
};

void	KempstonMouse_Install(void)
{
	int i;

	for (i=0; i<3; i++)
	{
		CPC_InstallReadPort(&kempstonReadPorts[i]);
	}
}