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
#include "../precomp.h"
//#define WIN32_LEAN_AND_MEAN
//#define WIN32_EXTRA_LEAN
#define INITGUID
#include <windows.h>
#include <ddraw.h>
#include "..\general\lnklist\lnklist.h"
#include "dd.h"
#include "graphlib.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
	GUID		Guid;
	LPTSTR		pDriverDescription;
	LPTSTR		pDriverName;
} DIRECT_DRAW_DEVICES_NODE;

typedef struct 
{
	DDSURFACEDESC	SurfaceDescription;
} MODE_LIST_NODE;

/* direct draw interfaces */
static LPDIRECTDRAW			pDirectDraw = NULL;
static LPDIRECTDRAW			pDirectDraw2 = NULL;

/* clipper interface */
static LPDIRECTDRAWCLIPPER	pDirectDrawClipper = NULL;

/* front surface interfaces */
static LPDIRECTDRAWSURFACE	pPrimarySurface=NULL;
static LPDIRECTDRAWSURFACE2	pPrimarySurface2=NULL;
static LPDIRECTDRAWSURFACE3	pPrimarySurface3=NULL;

/* paletted primary surface */
static PALETTEENTRY			Palette[256];
static LPDIRECTDRAWPALETTE	pPalette = NULL;

/* back surface interfaces */
static LPDIRECTDRAWSURFACE	pBackSurface=NULL;
static LPDIRECTDRAWSURFACE2	pBackSurface2=NULL;
static LPDIRECTDRAWSURFACE3	pBackSurface3=NULL;

static BOOL	Windowed;
static HWND DDrawHwnd;

LIST_HEADER			*pDirectDrawDevices=NULL;
LIST_HEADER			*pModeList=NULL;

static RECT	WindowRect;
static RECT ClientRect;

//static void DD_KillSurfaces(void);
void	DD_FullScreenToWindow(void);
void	DD_RestoreSurfaces(void);

static void	DD_GetInterfaces(void);

static void	DD_ReleaseBackInterfaces(void);
static void	DD_ReleasePrimaryInterfaces(void);

static void	DD_ReleaseInterfaces(void);
static void	DD_RestoreInterfaces(void);

static BOOL	DD_WindowedDisplay(HWND);
static BOOL	DD_FullScreenDisplay(HWND,int,int,int);


static void	DD_FlipScreens(void);
static void	DD_BlitScreens(RECT *);

static BOOL	WINAPI pDirectDrawEnumerateCallback(GUID FAR *lpGuid, LPTSTR lpDriverDescription, LPTSTR lpDriverName, LPVOID pData);
static HRESULT WINAPI	pModeListCallback(LPDDSURFACEDESC lpDDModeDesc,LPVOID pData);


BOOL UseFlip = FALSE;
static HINSTANCE hModule = NULL;

// DEBUG
//static const TCHAR *ErrorToString(HRESULT error);
static void	DD_ErrorMessage(HRESULT hResult);

//#define DD_DEBUG_MESSAGE(x) \
//	DD_ErrorMessage(ErrorToString(x))


/*----------------------------------------------------------------------------------*/

void	DD_ClearBothSurfaces()
{
	DDSURFACEDESC SurfaceDesc;

	if ((pBackSurface==NULL) || (pPrimarySurface==NULL))
		return;

	// need a clear that takes flipping into account!

	memset(&SurfaceDesc, 0, sizeof(DDSURFACEDESC));
	SurfaceDesc.dwSize = sizeof(DDSURFACEDESC);
	
	if (IDirectDrawSurface_GetSurfaceDesc(pPrimarySurface, &SurfaceDesc)==DD_OK)
	{
		// got surface desc
		if (SurfaceDesc.dwFlags & DDSD_BACKBUFFERCOUNT)
		{
			if (SurfaceDesc.dwBackBufferCount!=0)
			{
				// complex surface.

				// clear back buffer
				DDBLTFX	BltFx;

				memset(&BltFx,0,sizeof(DDBLTFX));
				BltFx.dwSize = sizeof(DDBLTFX);

				IDirectDrawSurface_Blt(pBackSurface,NULL,NULL,NULL,DDBLT_COLORFILL | DDBLT_WAIT,&BltFx);

				// flip
				DD_Flip();

				// clear back buffer
				memset(&BltFx,0,sizeof(DDBLTFX));
				BltFx.dwSize = sizeof(DDBLTFX);

				IDirectDrawSurface_Blt(pBackSurface,NULL,NULL,NULL,DDBLT_COLORFILL | DDBLT_WAIT,&BltFx);

				// flip
				DD_Flip();
			}
		}
		else
		{
					
			// works on matrox
			if (pBackSurface!=NULL)
			{

				DDBLTFX	BltFx;

				memset(&BltFx,0,sizeof(DDBLTFX));
				BltFx.dwSize = sizeof(DDBLTFX);

				IDirectDrawSurface_Blt(pBackSurface,NULL,NULL,NULL,DDBLT_COLORFILL | DDBLT_WAIT,&BltFx);
			}

			DD_Flip();
/*
			if (pPrimarySurface!=NULL)
			{

				DDBLTFX	BltFx;

				memset(&BltFx,0,sizeof(DDBLTFX));
				BltFx.dwSize = sizeof(DDBLTFX);

				IDirectDrawSurface_Blt(pPrimarySurface,NULL,NULL,NULL,DDBLT_COLORFILL,&BltFx);
			}
			*/
		}
	}

}


/*
// kill all surfaces
void	DD_KillSurfaces()
{
	// release IDirectDrawSurface2 and IDirectDrawSurface3 interfaces
	//DD_ReleaseInterfaces();

	if (pPrimarySurface!=NULL)
	{
		IDirectDrawSurface_Release(pPrimarySurface);
		pPrimarySurface = NULL;
	}

	if (pBackSurface!=NULL)
	{
		IDirectDrawSurface_Release(pBackSurface);
		pBackSurface = NULL;
	}
}
*/
// restore front and back surfaces
void	DD_RestoreSurfaces()
{
	// restore primary surface
	if (pPrimarySurface!=NULL)
	{
		if (IDirectDrawSurface_IsLost(pPrimarySurface))
			IDirectDrawSurface_Restore(pPrimarySurface);
	}

	// restore back surface
	if (pBackSurface!=NULL)
	{
		if (IDirectDrawSurface_IsLost(pBackSurface))
			IDirectDrawSurface_Restore(pBackSurface);
	}

//	DD_ClearBothSurfaces();
	
//	DD_RestoreInterfaces();

}

/*-----------------------------------------------------------------------------------*/
// switch between full screen and windowed

void	DD_ReleasePrimaryInterfaces()
{
	// PRIMARY SURFACE
	if (pPrimarySurface3!=NULL)
	{
		IDirectDrawSurface3_Release(pPrimarySurface3);
		pPrimarySurface3=NULL;
	}

	if (pPrimarySurface2!=NULL)
	{
		IDirectDrawSurface2_Release(pPrimarySurface2);
		pPrimarySurface2=NULL;
	}
}

void	DD_ReleaseBackInterfaces()
{
	// BACK SURFACE
	if (pBackSurface3!=NULL)
	{
		IDirectDrawSurface3_Release(pBackSurface3);
		pBackSurface3=NULL;
	}

	if (pBackSurface2!=NULL)
	{
		IDirectDrawSurface2_Release(pBackSurface2);
		pBackSurface2=NULL;
	}
}

void	DD_RemoveClipper(LPDIRECTDRAWSURFACE pSurface)
{
	/* remove clipper from this surface */
	if (pSurface!=NULL)
	{
		LPDIRECTDRAWCLIPPER pClipper;
		
		/* attempt to get clipper */
		if (IDirectDrawSurface_GetClipper(pSurface, &pClipper)==DD_OK)
		{
			if (pClipper!=NULL)
			{
				/* got it. Now remove it */
				IDirectDrawSurface_SetClipper(pSurface, NULL);
			}
		}
	}
}



void	DD_ReleaseSurfaces()
{
	DD_RemoveClipper(pPrimarySurface);

	DD_RemoveClipper(pBackSurface);

	/* palette setup? */
	if (pPalette!=NULL)
	{
		/* release palette */
		IDirectDrawPalette_Release(pPalette);
		pPalette = NULL;
	}

	DD_ReleaseBackInterfaces();
		
	DD_ReleasePrimaryInterfaces();

	/* release surfaces */
	if (pBackSurface!=NULL)
	{
		IDirectDrawSurface_Release(pBackSurface);
		pBackSurface = NULL;
	}

	if (pPrimarySurface!=NULL)
	{
		IDirectDrawSurface_Release(pPrimarySurface);
		pPrimarySurface = NULL;
	}


//	if (pBackSurface!=NULL)
//	{
//		IDirectDrawSurface_Release(pBackSurface);
//		pBackSurface = NULL;
//	}
//
//	if (pPrimarySurface!=NULL)
//	{
//		IDirectDrawSurface_Release(pPrimarySurface);
//		pPrimarySurface = NULL;
//	}


}

void	DD_ReleaseSurfacesAndRestoreVideoMode()
{
	DD_ReleaseSurfaces();

	// This must be done after the surfaces are release, otherwise,
	// on a Riva TNT board, the BLT's to the primary surface come out in the wrong place!

	// restore display mode
	IDirectDraw2_RestoreDisplayMode(pDirectDraw2);
}

/*
void	DD_FullScreenToWindow()
{
	DD_ReleaseSurfaces();





}
*/
/*
void	DD_ShutdownWindowed()
{
	DD_ReleaseSurfaces();
}
*/

/*-----------------------------------------------------------------------------------*/

// get IDirectDrawSurface2 and IDirectDrawSurface3 on primary and back surfaces
void	DD_GetInterfaces()
{
	// PRIMARY SURFACE 

	// query interface
	if (IDirectDrawSurface_QueryInterface(pPrimarySurface,&IID_IDirectDrawSurface2,(LPVOID *)&pPrimarySurface2)==DD_OK)
	{
		IDirectDrawSurface2_QueryInterface(pPrimarySurface2,&IID_IDirectDrawSurface3,(LPVOID *)&pPrimarySurface3);
	}

	// BACK SURFACE

	// query interface
	if (IDirectDrawSurface_QueryInterface(pBackSurface,&IID_IDirectDrawSurface2,(LPVOID *)&pBackSurface2)==DD_OK)
	{
		IDirectDrawSurface2_QueryInterface(pBackSurface2,&IID_IDirectDrawSurface3,(LPVOID *)&pBackSurface3);
	}
}



// release surface interfaces
void	DD_ReleaseInterfaces()
{
	DD_ReleaseSurfaces();

//	if (pPalette!=NULL)
//	{	
//		IDirectDrawPalette_Release(pPalette);
//		pPalette=NULL;
//	}


}

// restore surface interfaces
void	DD_RestoreInterfaces()
{
	// PRIMARY SURFACE
	if (pPrimarySurface3!=NULL)
		IDirectDrawSurface3_Restore(pPrimarySurface3);

	if (pPrimarySurface2!=NULL)
		IDirectDrawSurface2_Restore(pPrimarySurface2);

	// BACK SURFACE
	if (pBackSurface3!=NULL)
		IDirectDrawSurface3_Restore(pBackSurface3);

	if (pBackSurface2!=NULL)
		IDirectDrawSurface2_Restore(pBackSurface2);
}

/*-----------------------------------------------------------------------------------*/

void	DeleteModeListNode(void *pData)
{
	unsigned char *pNode = (unsigned char *)pData;

	if (pNode!=NULL)
	{
		free(pNode);
	}
}

void	DeleteDevicesListNode(void *pData)
{
	unsigned char *pNode = (unsigned char *)pData;

	if (pNode!=NULL)
	{
		free(pNode);
	}
}


// release data concerning a specific direct draw
void	DD_CloseDD()
{
//	if (!(Windowed))
//	{
//		DD_FullScreenToWindow();
//	}

//	DD_KillSurfaces();

	DD_ReleaseSurfaces();

	// free clipper
	if (pDirectDrawClipper!=NULL)
	{
		IDirectDrawClipper_Release(pDirectDrawClipper);
		pDirectDrawClipper = NULL;
	}

	// free direct draw 2 interface
	if (pDirectDraw2!=NULL)
	{
		IDirectDraw2_Release(pDirectDraw2);
		pDirectDraw2 = NULL;
	}

	// free direct draw interface
	if (pDirectDraw!=NULL)
	{
		IDirectDraw_Release(pDirectDraw);
		pDirectDraw = NULL;
	}

	// delete list of modes for this direct draw device
	if (pModeList!=NULL)
		LinkList_DeleteList(&pModeList,DeleteModeListNode);
}

/*-----------------------------------------------------------------------------------*/

// callback used when creating a specific direct draw instance
static HRESULT WINAPI	pModeListCallback(LPDDSURFACEDESC lpDDModeDesc,LPVOID pData)
{
	LIST_HEADER *pList = (LIST_HEADER *)pData;
	MODE_LIST_NODE *pModeListNode;

	pModeListNode = (MODE_LIST_NODE *)malloc(sizeof(MODE_LIST_NODE));

	memcpy(&pModeListNode->SurfaceDescription,lpDDModeDesc,sizeof(DDSURFACEDESC));
	
	LinkList_AddItemToListEnd(pList,pModeListNode);

	return DDENUMRET_OK;
}


// create a specific direct draw
BOOL	DD_SetupDD(HWND hwnd,void *pDriver)
{
	LPGUID	pDirectDrawDriver = (LPGUID)pDriver;


/* TROELS BEGIN */   
   typedef HRESULT (WINAPI * PFNDIRECTDRAWCREATE)( GUID FAR *, LPDIRECTDRAW FAR *, IUnknown FAR *);

   PFNDIRECTDRAWCREATE pfn;

	DDrawHwnd = hwnd;
   pfn = (PFNDIRECTDRAWCREATE)GetProcAddress(hModule, "DirectDrawCreate");
   if (pfn == NULL)
		return FALSE;
	if ((*pfn)(pDirectDrawDriver,&pDirectDraw,NULL)!=DD_OK)
		return FALSE;
/* TROELS END */  

//	// create the default one
//	if (DirectDrawCreate(pDirectDrawDriver,&pDirectDraw,NULL)!=DD_OK)
//		return FALSE;

	// query interface for pDirectDraw2
	if (IDirectDraw_QueryInterface(pDirectDraw,&IID_IDirectDraw2,(LPVOID *)&pDirectDraw2)!=DD_OK)
		return FALSE;

	// create clipper
	if (IDirectDraw_CreateClipper(pDirectDraw,0,&pDirectDrawClipper,NULL)!=DD_OK)
		return FALSE;

	// initialise list of modes for this direct draw
	LinkList_InitialiseList(&pModeList);

	if (pModeList!=NULL)
	{
		// get list of display modes for selected direct draw
		if (IDirectDraw_EnumDisplayModes(pDirectDraw,0,NULL,pModeList,pModeListCallback)!=DD_OK)
			return FALSE;
	}

	return TRUE;
}

/*---------------------------------------------------------------------------------------------*/
// callback used in init to get list of direct draw devices
static BOOL	WINAPI pDirectDrawEnumerateCallback(GUID FAR *lpGuid, LPTSTR lpDriverDescription, LPTSTR lpDriverName, LPVOID pData)
{
	LIST_HEADER	*pList = (LIST_HEADER *)pData;
	DIRECT_DRAW_DEVICES_NODE *pDirectDrawDevicesNode;

	pDirectDrawDevicesNode = (DIRECT_DRAW_DEVICES_NODE *)malloc(sizeof(DIRECT_DRAW_DEVICES_NODE) + _tcslen(lpDriverDescription) + _tcslen(lpDriverName) + 2);

	if (lpGuid!=NULL)
	{
			memcpy(&pDirectDrawDevicesNode->Guid,lpGuid,sizeof(GUID));
	}
	else
	{
		memset(&pDirectDrawDevicesNode->Guid,0,sizeof(GUID));
	}

	pDirectDrawDevicesNode->pDriverDescription = (char *)pDirectDrawDevicesNode + sizeof(DIRECT_DRAW_DEVICES_NODE);
	pDirectDrawDevicesNode->pDriverName = pDirectDrawDevicesNode->pDriverDescription + _tcslen(lpDriverDescription) + 1;

	memcpy(pDirectDrawDevicesNode->pDriverDescription,lpDriverDescription,_tcslen(lpDriverDescription) + 1);
	memcpy(pDirectDrawDevicesNode->pDriverName,lpDriverName,_tcslen(lpDriverName) + 1);

	LinkList_AddItemToListEnd(pList,pDirectDrawDevicesNode);

	return DDENUMRET_OK;
}

// get list of direct draw drivers
BOOL	DD_Init()
{
   typedef HRESULT (WINAPI* PFNDIRECTDRAWENUMERATEW)( LPDDENUMCALLBACKW, LPVOID);
   typedef HRESULT (WINAPI* PFNDIRECTDRAWENUMERATEA)( LPDDENUMCALLBACKA, LPVOID);
#ifdef _UNICODE
   #define PFNDIRECTDRAWENUMERATE PFNDIRECTDRAWENUMERATEW
   #define FNNAME "DirectDrawEnumerateW"
#else
   #define PFNDIRECTDRAWENUMERATE PFNDIRECTDRAWENUMERATEA
   #define FNNAME "DirectDrawEnumerateA"
#endif
   PFNDIRECTDRAWENUMERATE pfn;

	// initialise list of direct draw drivers
	LinkList_InitialiseList(&pDirectDrawDevices);
	
	if (pDirectDrawDevices==NULL)
		return FALSE;


/* TROELS BEGIN */
   if (hModule == NULL) hModule = LoadLibrary(_T("ddraw.dll"));
   pfn = (PFNDIRECTDRAWENUMERATE)GetProcAddress(hModule, FNNAME);
   if (pfn == NULL)
		return FALSE;
   (*pfn)(pDirectDrawEnumerateCallback,pDirectDrawDevices);
/* TROELS END */   
   // enumerate direct draw drivers
//	DirectDrawEnumerate(pDirectDrawEnumerateCallback,pDirectDrawDevices);

	return TRUE;
}

void	DD_Close()
{
	
	// delete list of direct draw devices
	if (pDirectDrawDevices!=NULL)
		LinkList_DeleteList(&pDirectDrawDevices,DeleteDevicesListNode);
	
}

/*---------------------------------------------------------------------------------------------*/

HCURSOR	hPreviousCursor = NULL;

void	MyApp_SetWindowDimensions(int,int);

//void	DD_FromWindowed()
//{
//	DD_KillSurfaces();
//}


void	DD_SetPalette(LPDIRECTDRAWSURFACE pPrimarySurface)
{
	if (pPrimarySurface!=NULL)
	{
		DDSURFACEDESC SurfaceDescription;

		/* get primary surface description */
		memset(&SurfaceDescription, 0, sizeof(DDSURFACEDESC));
		SurfaceDescription.dwSize = sizeof(DDSURFACEDESC);

		if (IDirectDrawSurface_GetSurfaceDesc(pPrimarySurface, &SurfaceDescription)!=DD_OK)
			return;

		/* RGB or palette surface */
		if ((SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)!=0)
		{
			/* surface is paletted */

		//	if (pPalette!=NULL)
		//	{
		//		IDirectDrawPalette_Release(pPalette);
		//		pPalette = NULL;
		//	}

			/* create a palette */
			if (IDirectDraw_CreatePalette(pDirectDraw, DDPCAPS_8BIT, &Palette[0], &pPalette, NULL)==DD_OK)
			{
				int i;
				
				for (i=0; i<255; i++)
				{
					Palette[i].peRed = 0;
					Palette[i].peGreen = 0;
					Palette[i].peBlue = 0;
					Palette[i].peFlags = 0;
				}
				
				/* palette created */

				/* set palette */
				if (pPrimarySurface!=NULL)
				{
					IDirectDrawSurface_SetPalette(pPrimarySurface, pPalette);
				}
				
				if (pBackSurface!=NULL)
				{
					IDirectDrawSurface_SetPalette(pBackSurface, pPalette);
				}
			}
		}
	}
}


void	DD_SurfacesCreated(HWND hwnd)
{
	// set hwnd for clipper
	if (IDirectDrawClipper_SetHWnd(pDirectDrawClipper,0,hwnd)!=DD_OK)
		return;

	if (pPrimarySurface!=NULL)
	{
		IDirectDrawSurface_SetClipper(pPrimarySurface, pDirectDrawClipper);
	}

	// set palette
	DD_SetPalette(pPrimarySurface);

	// clear both surfaces
	DD_ClearBothSurfaces();
}



BOOL	DD_WindowedDisplay(HWND hwnd)
{
	DDSURFACEDESC	SurfaceDescription;
	RECT			WindowRect;
	int				Width,Height;

	Windowed = TRUE;
	ShowCursor(TRUE);
	UseFlip = FALSE;

	MyApp_GetWindowRect(&WindowRect);

	Width = WindowRect.right - WindowRect.left;
	Height = WindowRect.bottom - WindowRect.top;

	DD_ReleaseSurfaces();

	// set co-operative level
	if (IDirectDraw_SetCooperativeLevel(pDirectDraw,hwnd,DDSCL_NORMAL)!=DD_OK)
		return FALSE;

	memset(&SurfaceDescription,0,sizeof(DDSURFACEDESC));
	SurfaceDescription.dwSize = sizeof(DDSURFACEDESC);
	SurfaceDescription.dwFlags = DDSD_CAPS;
	SurfaceDescription.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	
	// create primary surface
	if (IDirectDraw_CreateSurface(pDirectDraw,&SurfaceDescription,&pPrimarySurface,NULL)!=DD_OK)
		return FALSE;

	memset(&SurfaceDescription, 0, sizeof(DDSURFACEDESC));
	SurfaceDescription.dwSize = sizeof(DDSURFACEDESC);
	SurfaceDescription.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	SurfaceDescription.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN; 
	SurfaceDescription.dwHeight = Height;
	SurfaceDescription.dwWidth = Width;

	/* create back surface */
	if (IDirectDraw2_CreateSurface(pDirectDraw2,&SurfaceDescription,&pBackSurface,NULL)!=DD_OK)
		return FALSE;

	DD_SurfacesCreated(hwnd);

	if (pPalette!=NULL)
	{
		int i;

		/* initialise palette for windowed mode */
		for (i=0; i<10; i++)
		{
			Palette[i].peRed = (unsigned char)i;
			Palette[i].peGreen = 0;
			Palette[i].peBlue = 0;
			Palette[i].peFlags = PC_EXPLICIT;
		
			Palette[i+246].peRed = (unsigned char)(i + 246);
			Palette[i+246].peGreen = 0;
			Palette[i+246].peBlue = 0;
			Palette[i+246].peFlags = PC_EXPLICIT;
		}

		/* initialise remaining for our use */
		for (i=10; i<246; i++)
		{
			Palette[i].peRed = 0;
			Palette[i].peGreen = 0;
			Palette[i].peBlue = 0;
			Palette[i].peFlags = PC_RESERVED | PC_NOCOLLAPSE;
		}
	}



	return TRUE;
}

void	DD_ResizeWindow(HWND hwnd)
{
	DD_ReleaseSurfaces();

	DD_WindowedDisplay(hwnd);
#if 0

	RECT	WindowRect;
	DDSURFACEDESC	SurfaceDescription;
	int	Width,Height;

	// release all back interfaces
//	DD_ReleaseBackInterfaces();

	// release back surface
	if (pBackSurface!=NULL)
	{
		IDirectDrawSurface_Release(pBackSurface);
	}
	
	GetWindowRect(hwnd,&WindowRect);

	/* width of rectangle */
	Width = WindowRect.right-WindowRect.left;
	Height = WindowRect.bottom-WindowRect.top;

	memset(&SurfaceDescription,0,sizeof(DDSURFACEDESC));
	SurfaceDescription.dwSize = sizeof(DDSURFACEDESC);

	if (IDirectDrawSurface_GetSurfaceDesc(pPrimarySurface,&SurfaceDescription)==DD_OK)
	{
		SurfaceDescription.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		SurfaceDescription.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN; // | DDSCAPS_SYSTEMMEMORY;
		SurfaceDescription.dwHeight = Width;
		SurfaceDescription.dwWidth = Height;
	
		// create back surface
		IDirectDraw2_CreateSurface(pDirectDraw2,&SurfaceDescription,&pBackSurface,NULL);
	}
#endif
}

// create surfaces for full-screen
BOOL DD_FullScreenCreateSurfaces(int Height, int Width)
{
	DDSURFACEDESC SurfaceDescription;

	DD_ReleaseSurfaces();
	
	// attempt to create a front/back flipping surface
	memset(&SurfaceDescription,0,sizeof(DDSURFACEDESC));
	SurfaceDescription.dwSize = sizeof(DDSURFACEDESC);
	SurfaceDescription.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	SurfaceDescription.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE;
	SurfaceDescription.dwBackBufferCount = 1;

	if (IDirectDraw2_CreateSurface(pDirectDraw2,&SurfaceDescription,&pPrimarySurface,NULL)==DD_OK)
	{
		// successfully created a complex flipping surface
		SurfaceDescription.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

		if (IDirectDrawSurface_GetAttachedSurface(pPrimarySurface,&SurfaceDescription.ddsCaps,&pBackSurface)==DD_OK)
		{
			UseFlip = TRUE;

			return TRUE;
		}

	}

	// create seperate surfaces, and allow them to go into system memory
	memset(&SurfaceDescription,0,sizeof(DDSURFACEDESC));
	SurfaceDescription.dwSize = sizeof(DDSURFACEDESC);
	SurfaceDescription.dwFlags = DDSD_CAPS;
	SurfaceDescription.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	if (IDirectDraw_CreateSurface(pDirectDraw,&SurfaceDescription,&pPrimarySurface,NULL)==DD_OK)
	{

		memset(&SurfaceDescription, 0, sizeof(DDSURFACEDESC));
		SurfaceDescription.dwSize = sizeof(DDSURFACEDESC);
		SurfaceDescription.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		SurfaceDescription.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN; 
		SurfaceDescription.dwHeight = Height;
		SurfaceDescription.dwWidth = Width;

		/* create back surface */
		if (IDirectDraw2_CreateSurface(pDirectDraw2,&SurfaceDescription,&pBackSurface,NULL)==DD_OK)
		{
			UseFlip = FALSE;
			return TRUE;
		}

	}
	return FALSE;
}

BOOL	DD_FullScreenDisplay(HWND hwnd,int Width,int Height,int BPP)
{
//	DDSURFACEDESC	SurfaceDescription;

	SetForegroundWindow(DDrawHwnd);

	
	ShowCursor(FALSE);
	Windowed = FALSE;

	// set cooperative level
	if (IDirectDraw_SetCooperativeLevel(pDirectDraw2,hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN)!=DD_OK)
		return FALSE;

	if (IDirectDraw_SetDisplayMode(pDirectDraw,Width,Height,BPP)!=DD_OK)
		return FALSE;

	// set it again just in case
	if (IDirectDraw_SetCooperativeLevel(pDirectDraw2,hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN)!=DD_OK)
		return FALSE;

	if (DD_FullScreenCreateSurfaces(Width, Height))
	{
		DD_SurfacesCreated(hwnd);

		return TRUE;
	}

	return FALSE;
}



BOOL	DD_SetVideoMode(int Width,int Height,int Depth, BOOL fFullScreen)
{
	if (fFullScreen)
	{
		RECT	ClientRect;
		int		Style, ExStyle;
		BOOL	HasMenu;

		// adjust client rect to be the width and height we want
		ClientRect.left = 0;
		ClientRect.top = 0;
		ClientRect.right = Width;
		ClientRect.bottom = Height;

		Style = GetWindowLong(DDrawHwnd, GWL_STYLE);
		ExStyle = GetWindowLong(DDrawHwnd, GWL_EXSTYLE);

		if (GetMenu(DDrawHwnd))
		{
			HasMenu = TRUE;
		}
		else
		{
			HasMenu = FALSE;
		}

		AdjustWindowRectEx(&ClientRect, Style, HasMenu, ExStyle);

		MyApp_SetWindowDimensions(ClientRect.right-ClientRect.left, ClientRect.bottom-ClientRect.top);

		// update window and size
		SetWindowPos(DDrawHwnd,NULL, 0, 0, ClientRect.right-ClientRect.left, ClientRect.bottom-ClientRect.top,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE/* | SWP_SHOWWINDOW*/);
		
		return	DD_FullScreenDisplay(DDrawHwnd,Width,Height,Depth);
	}
	else
	{
		RECT	ClientRect;
		int		Style, ExStyle;
		BOOL	HasMenu;

		// adjust client rect to be the width and height we want
		ClientRect.left = 0;
		ClientRect.top = 0;
		ClientRect.right = Width;
		ClientRect.bottom = Height;

		Style = GetWindowLong(DDrawHwnd, GWL_STYLE);
		ExStyle = GetWindowLong(DDrawHwnd, GWL_EXSTYLE);

		if (GetMenu(DDrawHwnd))
		{
			HasMenu = TRUE;
		}
		else
		{
			HasMenu = FALSE;
		}

		AdjustWindowRectEx(&ClientRect, Style, HasMenu, ExStyle);
/*		// adjust for toolbar
		ClientRect.top-=32;*/

		MyApp_SetWindowDimensions(ClientRect.right-ClientRect.left, ClientRect.bottom-ClientRect.top);

		// update window and size
		SetWindowPos(DDrawHwnd,NULL, 0, 0, ClientRect.right-ClientRect.left, ClientRect.bottom-ClientRect.top,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE/* | SWP_SHOWWINDOW*/);

		return	DD_WindowedDisplay(DDrawHwnd);
	}


	return FALSE;
}


/*---------------------------------------------------------------------------------*/

void	DD_ClearScreen()
{
	DDBLTFX	BltFx;

	memset(&BltFx,0,sizeof(DDBLTFX));
	BltFx.dwSize = sizeof(DDBLTFX);

	//IDirectDraw_WaitForVerticalBlank(pDirectDraw,DDWAITVB_BLOCKBEGIN,NULL);

	//IDirectDrawSurface3_PageLock(pBackSurface3,0);

	
	IDirectDrawSurface3_Blt(pBackSurface3,NULL,NULL,NULL,DDBLT_COLORFILL,&BltFx);
/*
	return;

	while (1==1)
	{
		HRESULT	Result;

		Result = IDirectDrawSurface3_Blt(pBackSurface3,NULL,NULL,NULL,DDBLT_COLORFILL | DDBLT_WAIT,&BltFx);

		if (Result==DD_OK)
			break;

		if (Result==DDERR_SURFACELOST)
		{
			DD_RestoreSurfaces();
		
		}
		
	}

	//IDirectDrawSurface3_PageUnlock(pBackSurface3,0);
*/
}

/*--------------------------------------------------------------------------------*/

void	DD_FlipScreens()
{
	HRESULT	Result;


	while (1==1)
	{
		Result = IDirectDrawSurface_Flip(pPrimarySurface,NULL,DDFLIP_WAIT);

		if (Result==DD_OK)
			break;

		if (Result==DDERR_SURFACELOST)
		{
			DD_RestoreSurfaces();
		}
	}
}

/*----------------------------------------------------------------------------------*/
// blit entire back surface to front surface. We could if wanted
// only blit those areas that have changed. But we don't bother
// with that wank.

void	DD_BlitScreens(RECT *pDestRect)
{
//	HRESULT	Result;
	DDBLTFX	DDBltFx;
	RECT	SourceRect;

	//IDirectDraw2_WaitForVerticalBlank(pDirectDraw2, DDWAITVB_BLOCKBEGIN, NULL);

	SourceRect.left = 0;
	SourceRect.top = 0;
	SourceRect.bottom = pDestRect->bottom-pDestRect->top;
	SourceRect.right = pDestRect->right-pDestRect->left;


	if ((SourceRect.bottom<=0) || (SourceRect.right<=0))
		return;

	if ((pDestRect->bottom<=0) || (pDestRect->right<=0))
		return;

	memset(&DDBltFx,0,sizeof(DDBltFx));
	DDBltFx.dwSize = sizeof(DDBltFx);
	
	
/*	pDestRect->top+=28;
	pDestRect->bottom+=28; */

	IDirectDrawSurface_Blt(pPrimarySurface,pDestRect,pBackSurface,&SourceRect,0,&DDBltFx);

	//IDirectDrawSurface_BltFast(pPrimarySurface,0, 0, pBackSurface,&SourceRect,DDBLTFAST_NOCOLORKEY);


	if (pPalette!=NULL)
	{
		IDirectDrawPalette_SetEntries(pPalette, 0, 0, 256, Palette);
	}

	return;
/*
//	IDirectDrawSurface3_PageLock(pBackSurface3,0);
	
	while (1==1)
	{
		Result = IDirectDrawSurface_Blt(pPrimarySurface,pDestRect,pBackSurface,&SourceRect,DDBLT_WAIT,&DDBltFx);

		if (Result==DD_OK)
			break;

		DD_DEBUG_MESSAGE(Result);
	}

//	IDirectDrawSurface3_PageUnlock(pBackSurface3,0);
*/}

/*--------------------------------------------------------------------------------*/


void	DD_FlipWindowed()
{
	RECT	ClientRect;

	GetClientRect(DDrawHwnd,&ClientRect);
	
	ClientToScreen(DDrawHwnd,(POINT *)&ClientRect.left);
	ClientToScreen(DDrawHwnd,(POINT *)&ClientRect.right);

	DD_BlitScreens(&ClientRect);
}

// flip buffers.
void	DD_Flip()
{
	if (!UseFlip)
//	if (Windowed)
	{
		// windowed

		DD_FlipWindowed();
	}
	else
	{
		// full-screen
		DD_FlipScreens();
	}
}


/////////////////////////////////////////////////////////////////////////////////////

/*
void	SetWindowProc(WNDPROC *pNewWndProc)
{
	WNDPROC	pWndProc;

	// get current window proc 
	pWndProc = GetWindowLong(hwnd,GWL_WNDPROC);
	
// store it 


	// set a new window proc which will handle ddraw messages
	SetWindowLong(hwnd, GWL_WNDPROC, (LONG)DDrawWndProc);
};
*/
//	if (!ApplicationWindowed)
//		IDirectDraw_RestoreDisplayMode(pDirectDraw);

BOOL	DD_GetSurfacePtr(DDSURFACEDESC *SurfaceDescription)
{
	//DDSURFACEDESC SurfaceDescription;
	LPDIRECTDRAWSURFACE pSurface = pBackSurface;

	RECT	BackSurfaceRect;

	if (pSurface == NULL)
		return FALSE;

	memset(SurfaceDescription,0,sizeof(DDSURFACEDESC));
	SurfaceDescription->dwSize = sizeof(DDSURFACEDESC);

	if (IDirectDrawSurface_GetSurfaceDesc(pSurface,SurfaceDescription)!=DD_OK)
		return FALSE;

	BackSurfaceRect.left = 0;
	BackSurfaceRect.top = 0;
	BackSurfaceRect.right = SurfaceDescription->dwWidth;
	BackSurfaceRect.bottom = SurfaceDescription->dwHeight;


	if (IDirectDrawSurface_Lock(pSurface,&BackSurfaceRect,SurfaceDescription,DDLOCK_SURFACEMEMORYPTR/* | DDLOCK_WAIT*/ | DDLOCK_WRITEONLY,0)!=DD_OK)
		return FALSE;

	return TRUE;
}

void	DD_ReturnSurfacePtr(DDSURFACEDESC *SurfaceDescription)
{
	LPDIRECTDRAWSURFACE	pSurface = pBackSurface;

	IDirectDrawSurface_Unlock(pSurface,SurfaceDescription->lpSurface);
}

void	DD_ExamineMode(MODE_DETAILS *pModeDetails)
{
	int	i;

	DDSURFACEDESC	SurfaceDesc;
	int	RedShift,RedBPP,RedMask;
	int	GreenShift, GreenBPP,GreenMask;
	int	BlueShift, BlueBPP,BlueMask;
	int BPP;


	memset(&SurfaceDesc,0,sizeof(DDSURFACEDESC));
	SurfaceDesc.dwSize = sizeof(DDSURFACEDESC);
	
	// Get Surface Description
	if (IDirectDrawSurface_GetSurfaceDesc(pBackSurface,&SurfaceDesc)!=DD_OK)
		return;

#ifdef _MSC_VER
	BPP = SurfaceDesc.ddpfPixelFormat.dwRGBBitCount;
#else
	BPP = SurfaceDesc.ddpfPixelFormat.u1.dwRGBBitCount;
#endif
	// RED

	RedShift = 0;
#ifdef _MSC_VER
	RedMask = SurfaceDesc.ddpfPixelFormat.dwRBitMask;
#else
	RedMask = SurfaceDesc.ddpfPixelFormat.u2.dwRBitMask;
#endif

	for (i=0; i<BPP; i++)
	{
		if ((RedMask & 0x01)!=0)
			break;

		RedMask = RedMask>>1;

		RedShift++;
	}

	RedBPP = 0;

	for (i=0; i<BPP; i++)
	{
		if ((RedMask & 0x01)==0)
			break;

		RedMask = RedMask>>1;

		RedBPP++;
	}

	// GREEN
	GreenShift = 0;
#ifdef _MSC_VER
	GreenMask = SurfaceDesc.ddpfPixelFormat.dwGBitMask;
#else
	GreenMask = SurfaceDesc.ddpfPixelFormat.u3.dwGBitMask;
#endif
	for (i=0; i<BPP; i++)
	{
		if ((GreenMask & 0x01)!=0)
			break;

		GreenMask = GreenMask>>1;

		GreenShift++;
	}

	GreenBPP = 0;

	for (i=0; i<BPP; i++)
	{
		if ((GreenMask & 0x01)==0)
			break;

		GreenMask = GreenMask>>1;

		GreenBPP++;
	}

	// BLUE

	BlueShift = 0;
#ifdef _MSC_VER
	BlueMask = SurfaceDesc.ddpfPixelFormat.dwBBitMask;
#else
	BlueMask = SurfaceDesc.ddpfPixelFormat.u4.dwBBitMask;
#endif

	for (i=0; i<BPP; i++)
	{
		if ((BlueMask & 0x01)!=0)
			break;

		BlueMask = BlueMask>>1;

		BlueShift++;
	}

	
	BlueBPP = 0;

	for (i=0; i<BPP; i++)
	{
		if ((BlueMask & 0x01)==0)
			break;

		BlueMask = BlueMask>>1;

		BlueBPP++;
	}

	pModeDetails->BPP = BPP;
#ifdef _MSC_VER
	pModeDetails->RedMask = SurfaceDesc.ddpfPixelFormat.dwRBitMask;
	pModeDetails->GreenMask = SurfaceDesc.ddpfPixelFormat.dwGBitMask;
	pModeDetails->BlueMask = SurfaceDesc.ddpfPixelFormat.dwBBitMask;
#else
	pModeDetails->RedMask = SurfaceDesc.ddpfPixelFormat.u2.dwRBitMask;
	pModeDetails->GreenMask = SurfaceDesc.ddpfPixelFormat.u3.dwGBitMask;
	pModeDetails->BlueMask = SurfaceDesc.ddpfPixelFormat.u4.dwBBitMask;
#endif
	pModeDetails->RedShift = RedShift;
	pModeDetails->GreenShift = GreenShift;
	pModeDetails->BlueShift = BlueShift;
	pModeDetails->RedBPP = RedBPP;
	pModeDetails->GreenBPP = GreenBPP;
	pModeDetails->BlueBPP = BlueBPP;
}







/*
	RECT	ClientRect;
	int		WindowStyle;

	ClientRect.left = 0;
	ClientRect.top = 0;
	ClientRect.right = Width;
	ClientRect.bottom = Height;
	
	Windowed=TRUE;
	WindowStyle = GetWindowLong(hwnd, GWL_STYLE);
	AdjustWindowRect(&ClientRect,WindowStyle,TRUE);
	SetWindowPos(hwnd,HWND_TOPMOST,0,0,ClientRect.right-ClientRect.left,ClientRect.bottom-ClientRect.top,SWP_NOZORDER);
	ShowWindow(hwnd, TRUE );
    UpdateWindow( hwnd );
	WindowRect.left = 0;
	WindowRect.top = 0;
	WindowRect.bottom = Height;
	WindowRect.right = Width;
*/



void	DD_ErrorMessage(HRESULT hResult)
{
	LPVOID lpMsgBuf = NULL;

	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM,
					NULL,
					hResult, 
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR) &lpMsgBuf,
					0,
					NULL );

	MessageBox(DDrawHwnd,(LPTSTR)lpMsgBuf,_T("Ddraw Error"),MB_ICONERROR|MB_OK);

	LocalFree(lpMsgBuf);
}



/*
long FAR PASCAL WindowProc( HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
	switch (iMsg)
	{
		case WM_ENTERMENULOOP:
			{



			}
			break;

		case WM_EXITMENULOOP:
			{


			}
			break;

		case WM_SETFOCUS:
			{
				// restore all surfaces

			}
			break;

		case WM_KILLFOCUS:
			{
				// 



			}
			break;
	}
}
*/

/* check Direct Draw, Direct Input and Direct Sound are present */
BOOL		DirectX_CheckComponentsArePresent()
{
	BOOL		AllComponentsExist = FALSE;

	HINSTANCE	DDrawLibrary;

	/* load Direct Draw library DDRAW.DLL */
	DDrawLibrary = LoadLibrary(_T("DDRAW.DLL"));

	if (DDrawLibrary!=NULL)
	{
		HINSTANCE	DInputLibrary;

		/* load Direct Input Library DINPUT.DLL */
		DInputLibrary = LoadLibrary(_T("DINPUT.DLL"));

		if (DInputLibrary!=NULL)
		{
			HINSTANCE	DSoundLibrary;

			/* load Direct Sound Library DSOUND.DLL */
			DSoundLibrary = LoadLibrary(_T("DSOUND.DLL"));

			if (DSoundLibrary!=NULL)
			{
				/* all components exist */
				AllComponentsExist = TRUE;

				/* free opened library */
				FreeLibrary(DSoundLibrary);
			}

			/* free opened library */
			FreeLibrary(DInputLibrary);
		}

		/* free opened library */
		FreeLibrary(DDrawLibrary);
	}

	return AllComponentsExist;
}


/*
#define DDRAW_NOT_INSTALLED	0x0001
#define DDRAW_WRONG_VERSION	0x0002


int		DetectDirectXVersionUnderWin95()
{	
	HINSTANCE				DDrawLibrary = NULL;
	LPDIRECTDRAW			pDirectDraw = NULL;
	LPDIRECTDRAW2			pDirectDraw2 = NULL;
	LPDIRECTINPUT			pDirectInput = NULL;
	LPDIRECTDRAWSURFACE3	pSurface3 = NULL;

	// attempt to load DirectDraw library 
    DDrawLibrary = LoadLibrary("DDRAW.DLL");
    
	if (DDrawLibrary == 0)
	{
		return DDRAW_NOT_INSTALLED;
	}
	else
	{
		DIRECTDRAWCREATE	DirectDrawCreate = NULL;

		// succeeded in loading direct draw 
    
		// See if we can create get the DirectDrawCreate function from 
		// the library 

		DirectDrawCreate = (DIRECTDRAWCREATE)GetProcAddress(DDrawLibrary, "DirectDrawCreate");
    
		if (DirectDrawCreate != NULL)
		{
			// got DirectDrawCreate function address //
			
			// create DirectDraw interface 
			if (DirectDrawCreate(NULL, &pDirectDraw,NULL)==DD_OK)
			{
				// succeeded in getting Direct Draw Interface 

				// attempt to get DirectDraw2 interface 
				if (IDirectDraw_QueryInterface(pDirectDraw, &IID_IDirectDraw2, (LPVOID *)&pDirectDraw2)==DD_OK)
				{





			FreeLibrary(DirectDrawCreate);
		}
*/


BOOL	ValidateModeIsAtLeast16Bits()
{
	DDSURFACEDESC SurfaceDesc;

	memset(&SurfaceDesc, 0, sizeof(DDSURFACEDESC));
	SurfaceDesc.dwSize = sizeof(DDSURFACEDESC);

	IDirectDraw_GetDisplayMode(pDirectDraw, &SurfaceDesc);

#ifdef _MSC_VER
	if (SurfaceDesc.ddpfPixelFormat.dwRGBBitCount>=16)
#else
	if (SurfaceDesc.ddpfPixelFormat.u1.dwRGBBitCount>=16)
#endif
	{
		return TRUE;
	}
	
	return FALSE;
}

void	DD_SetPaletteEntry(int index, unsigned char r, unsigned char g, unsigned char b)
{
	Palette[index].peRed = r;
	Palette[index].peGreen = g;
	Palette[index].peBlue = b;
	Palette[index].peFlags = PC_RESERVED | PC_NOCOLLAPSE;
}

void	DD_BuildModeMenu(void (*pCallback)(int, int,int))
{
	if (pModeList!=NULL)
	{
		LIST_NODE *pCurrentNode = pModeList->pFirstNode;
		
		while (pCurrentNode!=NULL)
		{
			MODE_LIST_NODE *pModeListNode = pCurrentNode->pNodeData;
			int BitDepth;

			if ((pModeListNode->SurfaceDescription.ddpfPixelFormat.dwFlags & (DDPF_PALETTEINDEXED1 | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXEDTO8))!=0)
			{
				/* paletted mode */

				if (pModeListNode->SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED1)
				{
					BitDepth = 1;
				}

				if (pModeListNode->SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED2)
				{
					BitDepth = 2;
				}

				if (pModeListNode->SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4)
				{
					BitDepth = 1;
				}

				if (pModeListNode->SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
				{
					BitDepth = 1;
				}
			}
			else
			{
#ifdef _MSC_VER
				BitDepth = pModeListNode->SurfaceDescription.ddpfPixelFormat.dwRGBBitCount;
#else
				BitDepth = pModeListNode->SurfaceDescription.ddpfPixelFormat.u1.dwRGBBitCount;
#endif
			}
		
			if (BitDepth>=8)
			{
				pCallback(pModeListNode->SurfaceDescription.dwWidth, 
						pModeListNode->SurfaceDescription.dwHeight,
						BitDepth);
			}

			pCurrentNode = (LIST_NODE *)pCurrentNode->pNextNode;
		}
	}
}
