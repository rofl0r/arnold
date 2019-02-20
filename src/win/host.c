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
#include "precomp.h"
//#include <windows.h>

#include "win.h"
#include "../cpc/host.h"
#include "directx/dd.h"
#include "directx/graphlib.h"
#include "directx/ds.h"
#include <direct.h>
#include "../cpc/arnold.h"
#include "cpcemu.h"

extern APP_DATA AppData;


static GRAPHICS_BUFFER_INFO BufferInfo;
static GRAPHICS_BUFFER_COLOUR_FORMAT BufferColourFormat;
static SOUND_PLAYBACK_FORMAT SoundFormat;
static 	DDSURFACEDESC SurfaceDesc;

BOOL	Host_SetDisplay(int Type, int Width, int Height, int Depth)
{
	if (Type == DISPLAY_TYPE_WINDOWED)
	{
		MyApp_SetWindowed(Width, Height);
	}
	else
	{
		MyApp_SetFullScreen(Width, Height);
	}

	return DD_SetVideoMode(Width, Height, Depth, (Type!=DISPLAY_TYPE_WINDOWED));
}


BOOL	Host_LockGraphicsBuffer(void);
GRAPHICS_BUFFER_INFO	*Host_GetGraphicsBufferInfo(void);
void	Host_UnlockGraphicsBuffer(void);
void	Host_SetPaletteEntry(int, unsigned char, unsigned char, unsigned char);
BOOL	Host_SetDisplay(int Type, int Width, int Height, int Depth);


GRAPHICS_BUFFER_COLOUR_FORMAT *Host_GetGraphicsBufferColourFormat()
{
//		DDSURFACEDESC SurfaceDesc;

		MODE_DETAILS ModeDetails;

		DD_ExamineMode(&ModeDetails);

//		BufferInfo.Height = SurfaceDesc.dwHeight;
//		BufferInfo.Width = SurfaceDesc.dwWidth;
//#ifdef _MSC_VER
//		BufferInfo.Pitch = SurfaceDesc.lPitch;
//#else
//		BufferInfo.Pitch = SurfaceDesc.u1.lPitch;
//#endif
//		BufferInfo.pSurface = SurfaceDesc.lpSurface;

		BufferColourFormat.BPP = ModeDetails.BPP;

		BufferColourFormat.Red.BPP = ModeDetails.RedBPP;
		BufferColourFormat.Red.Mask = ModeDetails.RedMask;
		BufferColourFormat.Red.Shift = ModeDetails.RedShift;

		BufferColourFormat.Green.BPP = ModeDetails.GreenBPP;
		BufferColourFormat.Green.Mask = ModeDetails.GreenMask;
		BufferColourFormat.Green.Shift = ModeDetails.GreenShift;

		BufferColourFormat.Blue.BPP = ModeDetails.BlueBPP;
		BufferColourFormat.Blue.Mask = ModeDetails.BlueMask;
		BufferColourFormat.Blue.Shift = ModeDetails.BlueShift;

		return &BufferColourFormat;
}

GRAPHICS_BUFFER_INFO *Host_GetGraphicsBufferInfo()
{
	return &BufferInfo;
}

BOOL	Host_LockGraphicsBuffer(void)
{
	BOOL State;

	State = DD_GetSurfacePtr(&SurfaceDesc);

	if (State)
	{
		MODE_DETAILS ModeDetails;

		DD_ExamineMode(&ModeDetails);

		BufferInfo.Height = SurfaceDesc.dwHeight;
		BufferInfo.Width = SurfaceDesc.dwWidth;
//#ifdef _MSC_VER
		BufferInfo.Pitch = SurfaceDesc.lPitch;
//#else
//		BufferInfo.Pitch = SurfaceDesc.u1.lPitch;
//#endif
		BufferInfo.pSurface = SurfaceDesc.lpSurface;

		BufferColourFormat.BPP = ModeDetails.BPP;

		BufferColourFormat.Red.BPP = ModeDetails.RedBPP;
		BufferColourFormat.Red.Mask = ModeDetails.RedMask;
		BufferColourFormat.Red.Shift = ModeDetails.RedShift;

		BufferColourFormat.Green.BPP = ModeDetails.GreenBPP;
		BufferColourFormat.Green.Mask = ModeDetails.GreenMask;
		BufferColourFormat.Green.Shift = ModeDetails.GreenShift;

		BufferColourFormat.Blue.BPP = ModeDetails.BlueBPP;
		BufferColourFormat.Blue.Mask = ModeDetails.BlueMask;
		BufferColourFormat.Blue.Shift = ModeDetails.BlueShift;



	}

	return State;
}


void	Host_UnlockGraphicsBuffer(void)
{
	DD_ReturnSurfacePtr(&SurfaceDesc);
}

void	Host_SwapGraphicsBuffers(void)
{
	DD_Flip();
}

void	Host_SetPaletteEntry(int Index, unsigned char R, unsigned char G, unsigned char B)
{
	DD_SetPaletteEntry(Index, R, G, B);
}

//void	Host_WriteDataToSoundBuffer(unsigned char *pData, unsigned long Length)
//{
//		DS_WriteBufferForSoundPlayback(pData,Length);
//}

BOOL	Host_AudioPlaybackPossible(void)
{
	return DS_AudioActive();
}

unsigned long	Host_GetCurrentTimeInMilliseconds(void)
{
	return timeGetTime();
}


SOUND_PLAYBACK_FORMAT *Host_GetSoundPlaybackFormat(void)
{
	SoundFormat.NumberOfChannels = DS_GetSampleChannels();
	SoundFormat.BitsPerSample = DS_GetSampleBits();
	SoundFormat.Frequency = DS_GetSampleRate();

	return &SoundFormat;
}

BOOL	Host_ProcessSystemEvents(void)
{
	/* process system events. If QUIT has been selected, then break out of loop */
	return WinApp_ProcessSystemEvents();
}

static void	DoDriveLEDIndicator(int Drive, BOOL State)
{
	if (State)
	{
//		ScrollLock_Set(TRUE);
	}
	else
	{
//		ScrollLock_Set(FALSE);
	}
}


/*void	Host_SetDirectory(char *Directory)
{
	_tchdir(Directory);
}
*/

BOOL Host_LockAudioBuffer(unsigned char **ppBlock1, unsigned long *pBlock1Size, unsigned char **ppBlock2, unsigned long *pBlock2Size, int BlockSize)
{
	return DS_LockAudioBuffer(ppBlock1, pBlock1Size, ppBlock2, pBlock2Size, BlockSize);
}

void	Host_UnLockAudioBuffer(void)
{
	DS_UnLockAudioBuffer();
}

extern BOOL bWin;
static unsigned long PreviousTime=0;
int Host_LockSpeed = FALSE;
unsigned long TimeError = 0;
extern BOOL DoNotScanKeyboard;

void	Host_Throttle(void)
{
	if (Host_LockSpeed)
	{
		/* use this to throttle speed */
		unsigned long	TimeDifference;
		unsigned long	Time;

		do
		{
			/* get current time */
			Time = timeGetTime();

			/* calc time difference */
			TimeDifference = Time - (PreviousTime-TimeError);
		}
		while (TimeDifference<(1000/50));

		TimeError = (TimeDifference - (1000/50)) % (1000/50);

		PreviousTime = Time;
	}


//		#ifdef AY_OUTPUT
//			/* if enabled, writes PSG registers to temp file */
//			YMOutput_WriteRegs();
//		#endif

  /* disc drive light indicator*/
   DoDriveLEDIndicator(0, FDD_LED_GetState(0));

	if (bWin)
	{
		CPC_UpdateAudio();

		/* auto type active? */
		if (AutoType_Active())
		{
			/* update it */
			AutoType_Update();
		}
		else
		{

			CPC_ClearKeyboard();

			/* scan keyboard/joysticks */
			DoKeyboard();
		}
	}
//	else Sleep(2); /* YIELD TO WINDOWS - TROELS */
}


HOST_FILE_HANDLE	Host_OpenFile(const char *Filename, int Access)
{
	HOST_FILE_HANDLE fh;

	if (Access == HOST_FILE_ACCESS_READ)
	{
		fh = (HOST_FILE_HANDLE)fopen(Filename,"rb");
	}
	else
	{
		fh = (HOST_FILE_HANDLE)fopen(Filename,"wb");
	}

	return fh;
}

void	Host_CloseFile(HOST_FILE_HANDLE Handle)
{
	if (Handle!=0)
	{
		fclose((FILE *)Handle);
	}
}

int	Host_GetFileSize(HOST_FILE_HANDLE Handle)
{
	if (Handle!=0)
	{
		int fno = _fileno((FILE *)Handle);

		return _filelength(fno);
	}

	return 0;
}



void	Host_ReadData(HOST_FILE_HANDLE Handle, unsigned char *pData, unsigned long Size)
{
	if (Handle!=0)
	{
		fread(pData, Size, 1, (FILE *)Handle);
	}
}

void	Host_WriteData(HOST_FILE_HANDLE Handle, unsigned char *pData, unsigned long Size)
{
	if (Handle!=0)
	{
		fwrite(pData, Size, 1, (FILE *)Handle);
	}
}

