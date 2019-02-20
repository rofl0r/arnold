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
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include "ds.h"
#include "..\general\lnklist\lnklist.h"

void	DS_ClearBuffer();

BOOL	AudioActive = FALSE;

void	WriteBufferForSoundPlayback(char *pBuffer,int BufferSize);

typedef struct 
{
	GUID		Guid;
	LPSTR		pDescription;
	LPSTR		pModule;
} DIRECT_SOUND_DEVICES_NODE;

static LPDIRECTSOUND	pDirectSound=NULL;

LIST_HEADER			*pDirectSoundDevices=NULL;


HRESULT WINAPI pDirectSoundEnumCallback(GUID FAR *lpGuid, LPSTR lpstrDescription,LPSTR lpstrModule,LPVOID pData)
{
	LIST_HEADER *pList= (LIST_HEADER *)pData;
	DIRECT_SOUND_DEVICES_NODE	*pDirectSoundDeviceNode;

	pDirectSoundDeviceNode = (DIRECT_SOUND_DEVICES_NODE *)malloc(sizeof(DIRECT_SOUND_DEVICES_NODE) + strlen(lpstrDescription) + 1 + strlen(lpstrModule) + 1);
	memcpy(&pDirectSoundDeviceNode->Guid,lpGuid,sizeof(GUID));
	
	pDirectSoundDeviceNode->pDescription = (char *)pDirectSoundDeviceNode + sizeof(DIRECT_SOUND_DEVICES_NODE);
	pDirectSoundDeviceNode->pModule = (char *)pDirectSoundDeviceNode->pModule + strlen(lpstrDescription) + 1;
	memcpy(pDirectSoundDeviceNode->pDescription,lpstrDescription,strlen(lpstrDescription) + 1);
	memcpy(pDirectSoundDeviceNode->pModule,lpstrModule,strlen(lpstrModule) + 1);

	LinkList_AddItemToListEnd(pList,pDirectSoundDeviceNode);

	
	return TRUE;
}

LPDIRECTSOUNDBUFFER pPrimaryBuffer = NULL;
LPDIRECTSOUNDBUFFER pBuffer1 = NULL;
LPDIRECTSOUNDBUFFER pBuffer2 = NULL;
LPDIRECTSOUNDBUFFER pBuffer3 = NULL;

//#define BufferSize 16384

int BufferSize;

//#define SAMPLE_RATE	10000		// 10000 Hz sample rate
//#define SAMPLE_BITS	8			// 8-bit samples

	WAVEFORMATEX	BufferFormat;

typedef struct	
{
	int Frequency;
	int BitsPerSample;
	int NoOfChannels;
	int Supported;
} SND_FORMAT;

static SND_FORMAT	SoundFormatsTable[16] = 
{
	{8000,8,1,0},
	{8000,8,2,0},
	{8000,16,1,0},
	{8000,16,2,0},
	{11025,8,1,0},
	{11025,8,2,0},
	{11025,16,1,0},
	{11025,16,2,0},
	{22050,8,1,0},
	{22050,8,2,0},
	{22050,16,1,0},
	{22050,16,2,0},
	{44100,8,1,0},
	{44100,8,2,0},
	{44100,16,1,0},
	{44100,16,2,0}
};

#define NUM_SOUND_FORMATS (sizeof(SoundFormatsTable)/sizeof(SND_FORMAT))

void	CheckSupportedSoundFormats(LPDIRECTSOUND pDirectSound)
{
	LPDIRECTSOUNDBUFFER pPrimaryBuffer;
	DSBUFFERDESC		DirectSoundBufferDesc;

	if (pDirectSound==NULL)
		return;

	// get primary buffer
	memset(&DirectSoundBufferDesc, 0, sizeof(DSBUFFERDESC));
	DirectSoundBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	DirectSoundBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	
	// get access to primary buffer
	if (IDirectSound_CreateSoundBuffer(pDirectSound, &DirectSoundBufferDesc, &pPrimaryBuffer, NULL)==DS_OK)
	{
		WAVEFORMATEX	CurrentFormat;
		int				SizeWritten;

		IDirectSoundBuffer_GetFormat(pPrimaryBuffer, &CurrentFormat, sizeof(WAVEFORMATEX), &SizeWritten);

		if (SizeWritten<=sizeof(WAVEFORMATEX))
		{
			int i;

			for (i=0; i<NUM_SOUND_FORMATS; i++)
			{
				WAVEFORMATEX FormatToSet;

				if (IDirectSoundBuffer_SetFormat(pPrimaryBuffer, &FormatToSet)!=DS_OK)
				{
					/* error setting format */

					FormatToSet.wFormatTag = WAVE_FORMAT_PCM;
					FormatToSet.nChannels = SoundFormatsTable[i].NoOfChannels;
					FormatToSet.nSamplesPerSec = SoundFormatsTable[i].Frequency;
					FormatToSet.nBlockAlign = (SoundFormatsTable[i].BitsPerSample>>3) * FormatToSet.nChannels;
					FormatToSet.wBitsPerSample = SoundFormatsTable[i].BitsPerSample;
					FormatToSet.nAvgBytesPerSec = FormatToSet.nBlockAlign*FormatToSet.nSamplesPerSec;
					FormatToSet.cbSize = 0;

					if (IDirectSoundBuffer_SetFormat(pPrimaryBuffer, &FormatToSet)!=DS_OK)
					{

						SoundFormatsTable[i].Supported = FALSE;
					}
					else
					{
						SoundFormatsTable[i].Supported = TRUE;
					}
				}
			}
		}

		/* restore original format */

		IDirectSoundBuffer_SetFormat(pPrimaryBuffer, &CurrentFormat);
	}
}


BOOL	InitDirectSound(HWND hwnd)
{
	DSBUFFERDESC	DirectSoundBufferDesc;

	AudioActive = FALSE;

	// get direct sound devices
//	DirectSoundEnumerate((LPDSENUMCALLBACK)pDirectSoundEnumCallback,pDirectSoundDevices);

	// create a direct sound interface. If this fails, no sound hardware is available
	if (DirectSoundCreate(NULL,&pDirectSound,NULL)!=DS_OK)
		return FALSE;

	// initialise normal co-operation level (means that sound device can be shared 
	// with other programs.
	if (IDirectSound_SetCooperativeLevel(pDirectSound,hwnd,DSSCL_PRIORITY)!=DS_OK)	//;	//DSSCL_NORMAL)!=DS_OK)
		return FALSE;

	// get primary buffer
	memset(&DirectSoundBufferDesc, 0, sizeof(DSBUFFERDESC));
	DirectSoundBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	DirectSoundBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	
	// get access to primary buffer
	if (IDirectSound_CreateSoundBuffer(pDirectSound, &DirectSoundBufferDesc, &pPrimaryBuffer, NULL)==DS_OK)
	{
		DWORD	SizeWritten;
		WAVEFORMATEX PrimaryBufferFormat;

		// start it playing
		IDirectSoundBuffer_Play(pPrimaryBuffer,0,0,DSBPLAY_LOOPING);

		IDirectSoundBuffer_GetFormat(pPrimaryBuffer, &PrimaryBufferFormat, sizeof(WAVEFORMATEX), &SizeWritten);

		memcpy(&BufferFormat, &PrimaryBufferFormat, sizeof(WAVEFORMATEX));


		{
			WAVEFORMATEX FormatToSet;
			int NoOfChannels = 2;		//1;
			int Frequency = 22050;	//44100;	//22050;	//;;44100;
			int BitsPerSample = 8;	//8;	//16;

			FormatToSet.wFormatTag = WAVE_FORMAT_PCM;
			FormatToSet.nChannels = NoOfChannels;
			FormatToSet.nSamplesPerSec = Frequency;
			FormatToSet.nBlockAlign = (BitsPerSample>>3) * FormatToSet.nChannels;
			FormatToSet.wBitsPerSample = BitsPerSample;
			FormatToSet.nAvgBytesPerSec = FormatToSet.nBlockAlign*FormatToSet.nSamplesPerSec;
			FormatToSet.cbSize = 0;

			IDirectSoundBuffer_SetFormat(pPrimaryBuffer, &FormatToSet);
	
			memcpy(&BufferFormat, &FormatToSet, sizeof(WAVEFORMATEX));
		}


		// release it
		IDirectSoundBuffer_Release(pPrimaryBuffer);

		pPrimaryBuffer = NULL;
	}


//	// initialise buffer formats for secondary buffer
//	BufferFormat.wFormatTag = WAVE_FORMAT_PCM;
//	BufferFormat.nChannels = 1;
//	BufferFormat.nSamplesPerSec = 10000;
//	BufferFormat.nAvgBytesPerSec = 0;
//	BufferFormat.wBitsPerSample = 0;
//	BufferFormat.cbSize = 0;

	BufferSize = ((BufferFormat.nSamplesPerSec/50)*10)*((BufferFormat.wBitsPerSample>>3)*BufferFormat.nChannels);


	// create buffer for channel A
	memset(&DirectSoundBufferDesc, 0, sizeof(DSBUFFERDESC));
	DirectSoundBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	DirectSoundBufferDesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
	DirectSoundBufferDesc.dwBufferBytes = BufferSize;
	DirectSoundBufferDesc.lpwfxFormat = &BufferFormat;

	
	// channel A buffer
	IDirectSound_CreateSoundBuffer(pDirectSound, &DirectSoundBufferDesc, &pBuffer1, NULL);

	DS_ClearBuffer();

/*
	// create buffer for channel B
	memset(&DirectSoundBufferDesc, 0, sizeof(DSBUFFERDESC));
	DirectSoundBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	DirectSoundBufferDesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
	DirectSoundBufferDesc.dwBufferBytes = BufferSize;
	DirectSoundBufferDesc.lpwfxFormat = &BufferFormat;

	// channel B buffer
	IDirectSound_CreateSoundBuffer(pDirectSound, &DirectSoundBufferDesc, &pBuffer2, NULL);

	// create buffer for channel C
	memset(&DirectSoundBufferDesc, 0, sizeof(DSBUFFERDESC));
	DirectSoundBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	DirectSoundBufferDesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
	DirectSoundBufferDesc.dwBufferBytes = BufferSize;
	DirectSoundBufferDesc.lpwfxFormat = &BufferFormat;

	// channel C buffer
	IDirectSound_CreateSoundBuffer(pDirectSound, &DirectSoundBufferDesc, &pBuffer3, NULL);
*/
	IDirectSoundBuffer_Play(pBuffer1,0,0,DSBPLAY_LOOPING);

	AudioActive = TRUE;

	return TRUE;
}

void	DS_StartSound()
{
	if (pBuffer1!=NULL)
	{
		IDirectSoundBuffer_Play(pBuffer1,0,0,DSBPLAY_LOOPING);
	}
}


void	DS_StopSound()
{
	if (pBuffer1!=NULL)
	{
		IDirectSoundBuffer_Stop(pBuffer1);	
	}
}


void	FinishDirectSound()
{
	if (pBuffer1!=NULL)
	{
		IDirectSoundBuffer_Stop(pBuffer1);
		IDirectSoundBuffer_Release(pBuffer1);
		pBuffer1 = NULL;
	}

	if (pBuffer2!=NULL)
	{
		IDirectSoundBuffer_Stop(pBuffer2);
		IDirectSoundBuffer_Release(pBuffer2);
		pBuffer2 = NULL;
	}

	if (pBuffer3!=NULL)
	{
		IDirectSoundBuffer_Stop(pBuffer3);
		IDirectSoundBuffer_Release(pBuffer3);
		pBuffer3 = NULL;
	}

	if (pPrimaryBuffer!=NULL)
	{
		IDirectSoundBuffer_Release(pPrimaryBuffer);
		pPrimaryBuffer = NULL;
	}

	if (pDirectSound!=NULL)
	{
		IDirectSound_Release(pDirectSound);
	}
}

void	DS_Init(HWND hWnd)
{
	InitDirectSound(hWnd);
}

void	DS_Close()
{
	FinishDirectSound();
}


static unsigned char *pAudioBufferPtr1;
static unsigned char *pAudioBufferPtr2;
static unsigned long AudioBufferBlock1Size;
static unsigned long AudioBufferBlock2Size;

static unsigned long BufferIndex = 0;
static unsigned long BufferPositions[100];
static unsigned long PrevPos = 0;

BOOL	DS_LockAudioBuffer(unsigned char **ppBuffer, unsigned long *pBuf1Size, unsigned char **ppBuffer2, unsigned long *pBuf2Size, int BuffSize)
{
	unsigned long playpos, writepos;

	*ppBuffer = NULL;
	*ppBuffer2 = NULL;
	*pBuf1Size = 0;
	*pBuf2Size = 0;

	
	if (IDirectSoundBuffer_GetCurrentPosition(pBuffer1, &playpos, &writepos)==DS_OK)
	{
		int Pos;
		
		if (playpos<PrevPos)
		{
			Pos = playpos+BufferSize - PrevPos;
		}
		else
		{
			Pos = playpos - PrevPos;
		}

		Pos = Pos/((8*2)>>3);

		BufferPositions[BufferIndex] = Pos;

		BufferIndex++;

		BufferIndex = BufferIndex % 100;

		PrevPos = playpos;
	}


	if (IDirectSoundBuffer_Lock(pBuffer1, 0, BuffSize, ppBuffer, pBuf1Size, ppBuffer2, pBuf2Size, DSBLOCK_FROMWRITECURSOR)==DS_OK)
	{
		pAudioBufferPtr1 = *ppBuffer;
		pAudioBufferPtr2 = *ppBuffer2;
		AudioBufferBlock1Size = *pBuf1Size;
		AudioBufferBlock2Size = *pBuf2Size;

		return TRUE;
	}

	return FALSE;
}

void	DS_UnLockAudioBuffer()
{
	
	/* unlock the buffer */
	IDirectSoundBuffer_Unlock(pBuffer1, pAudioBufferPtr1, AudioBufferBlock1Size, pAudioBufferPtr2, AudioBufferBlock2Size);
}

/*
void	DS_WriteBufferForSoundPlayback(char *pBuf,int BuffSize)
{

	// lock the buffer 
	if (IDirectSoundBuffer_Lock(pBuffer1, 0, LockSize, &pAudioPtr1, &AudioBytes1, &pAudioPtr2, &AudioBytes2, DSBLOCK_FROMWRITECURSOR)==DS_OK)
	{
		unsigned long AudioBytes1Written;
		unsigned long AudioBytes2Written;

		if (pAudioPtr1!=NULL)
		{
			if (BuffSize>=AudioBytes1)
			{
				AudioBytes1Written = AudioBytes1;
			}	
			else
			{
				AudioBytes1Written = BuffSize;
			}
			
			memcpy(pAudioPtr1, pBuf, AudioBytes1Written);
		}

		AudioBytes2Written = 0;

		if ((pAudioPtr2!=NULL) && (BuffSize>=AudioBytes1Written))
		{
			AudioBytes2Written = BuffSize - AudioBytes1Written;
			memcpy(pAudioPtr2, pBuf + AudioBytes1Written, AudioBytes2Written);
		
		}
			
		// unlock the buffer 
		IDirectSoundBuffer_Unlock(pBuffer1, pAudioPtr1, AudioBytes1Written, pAudioPtr2, AudioBytes2Written);
	}
}
*/

BOOL	DS_AudioActive()
{
	return AudioActive;
}

int	DS_GetSampleRate()
{
	return BufferFormat.nSamplesPerSec;
}

int	DS_GetSampleBits()
{
	return BufferFormat.wBitsPerSample;
}


int	DS_GetSampleChannels()
{
	return BufferFormat.nChannels;
}

void	DS_ClearBuffer()
{
	DSBCAPS	BufferCaps;

	if (pBuffer1!=NULL)
	{

		BufferCaps.dwSize = sizeof(DSBCAPS);

		// channel A buffer
		if (IDirectSoundBuffer_GetCaps(pBuffer1, &BufferCaps)==DS_OK)
		{
			char	*pAudioPtr1, *pAudioPtr2;
			unsigned long	AudioBytes1, AudioBytes2;
		
			/* lock the buffer */
			if (IDirectSoundBuffer_Lock(pBuffer1, 0, BufferCaps.dwBufferBytes, &pAudioPtr1, &AudioBytes1, &pAudioPtr2, &AudioBytes2, 0)==DS_OK)
			{
				if (pAudioPtr1!=NULL)
				{
					memset(pAudioPtr1, 0x080, AudioBytes1);
				}

				if (pAudioPtr2!=NULL)
				{
					memset(pAudioPtr2, 0x080, AudioBytes2);
				}


				IDirectSoundBuffer_Unlock(pBuffer1, pAudioPtr1, AudioBytes1, pAudioPtr2, AudioBytes2);
			}
		}
	}
}