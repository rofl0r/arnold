#ifndef __KEMPSTON_MOUSE_HEADER_INCLUDED__
#define __KEMPSTON_MOUSE_HEADER_INCLUDED__

#include "cpc.h"

unsigned char KempstonMouse_Read(Z80_WORD Port, Z80_BYTE Data);
void	KempstonMouse_Update(int DeltaX, int DeltaY, int LeftButton, int RightButton);



#endif
