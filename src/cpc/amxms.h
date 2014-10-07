#ifndef __AMX_MOUSE_INCLUDED__
#define __AMX_MOUSE_INCLUDED__

void	AmxMouse_UpdateMovement(unsigned long);
unsigned char AmxMouse_ReadJoystickPort(void);
void	AmxMouse_Update(int, int, int, int);

#endif