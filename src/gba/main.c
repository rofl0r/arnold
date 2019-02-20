#include "../cpc/host.h"

BOOL	Host_SetDisplay(int Type, int Width, int Height, int Depth)
{
	return FALSE;
}

GRAPHICS_BUFFER_COLOUR_FORMAT *Host_GetGraphicsBufferColourFormat()
{
	return NULL;
}

GRAPHICS_BUFFER_INFO *Host_GetGraphicsBufferInfo()
{
	return NULL;
}

BOOL	Host_LockGraphicsBuffer(void)
{
}

void	Host_UnlockGraphicsBuffer(void)
{
}

void	Host_SwapGraphicsBuffers(void)
{
}

BOOL Host_AudioPlaybackPossible()
{
	return FALSE;
}

void Host_Throttle()
{
}

void	Host_SetPaletteEntry(int Index, unsigned char R, unsigned char G, unsigned char B)
{
}

BOOL Host_ProcessSystemEvents()
{
	return FALSE;
}

BOOL Host_LockAudioBuffer(unsigned char **ppBlock1, unsigned long *pBlock1Size, unsigned char **ppBlock2, unsigned long *pBlock2Size, int BlockSize)
{
	return FALSE;
}

void	Host_UnLockAudioBuffer(void)
{
}


SOUND_PLAYBACK_FORMAT *Host_GetSoundPlaybackFormat(void)
{
	return NULL;
}

void Host_HandlePrinterOutput()
{
}


void AgbMain()
{

}
