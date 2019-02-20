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
	LPSTR		pDriverDescription;
	LPSTR		pDriverName;
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

static BOOL	WINAPI pDirectDrawEnumerateCallback(GUID FAR *lpGuid, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID pData);
static HRESULT WINAPI	pModeListCallback(LPDDSURFACEDESC lpDDModeDesc,LPVOID pData);


BOOL UseFlip = FALSE;

// DEBUG
static char *ErrorToString(HRESULT error);
static void	DD_ErrorMessage(char *ErrorText);

#define DD_DEBUG_MESSAGE(x) \
	DD_ErrorMessage(ErrorToString(x))


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

	DDrawHwnd = hwnd;

	// create the default one
	if (DirectDrawCreate(pDirectDrawDriver,&pDirectDraw,NULL)!=DD_OK)
		return FALSE;

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
static BOOL	WINAPI pDirectDrawEnumerateCallback(GUID FAR *lpGuid, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID pData)
{
	LIST_HEADER	*pList = (LIST_HEADER *)pData;
	DIRECT_DRAW_DEVICES_NODE *pDirectDrawDevicesNode;

	pDirectDrawDevicesNode = (DIRECT_DRAW_DEVICES_NODE *)malloc(sizeof(DIRECT_DRAW_DEVICES_NODE) + strlen(lpDriverDescription) + strlen(lpDriverName) + 2);

	if (lpGuid!=NULL)
	{
			memcpy(&pDirectDrawDevicesNode->Guid,lpGuid,sizeof(GUID));
	}
	else
	{
		memset(&pDirectDrawDevicesNode->Guid,0,sizeof(GUID));
	}

	pDirectDrawDevicesNode->pDriverDescription = (char *)pDirectDrawDevicesNode + sizeof(DIRECT_DRAW_DEVICES_NODE);
	pDirectDrawDevicesNode->pDriverName = pDirectDrawDevicesNode->pDriverDescription + strlen(lpDriverDescription) + 1;

	memcpy(pDirectDrawDevicesNode->pDriverDescription,lpDriverDescription,strlen(lpDriverDescription) + 1);
	memcpy(pDirectDrawDevicesNode->pDriverName,lpDriverName,strlen(lpDriverName) + 1);

	LinkList_AddItemToListEnd(pList,pDirectDrawDevicesNode);

	return DDENUMRET_OK;
}

// get list of direct draw drivers
BOOL	DD_Init()
{
	// initialise list of direct draw drivers
	LinkList_InitialiseList(&pDirectDrawDevices);
	
	if (pDirectDrawDevices==NULL)
		return FALSE;

	// enumerate direct draw drivers
	DirectDrawEnumerate(pDirectDrawEnumerateCallback,pDirectDrawDevices);

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
	// clear both surfaces
	DD_ClearBothSurfaces();

	// set hwnd for clipper
	if (IDirectDrawClipper_SetHWnd(pDirectDrawClipper,0,hwnd)!=DD_OK)
		return;

	if (pPrimarySurface!=NULL)
	{
		IDirectDrawSurface_SetClipper(pPrimarySurface, pDirectDrawClipper);
	}

	if (pBackSurface!=NULL)
	{
		/* direct draw 8 moans if there is no clipper on the back surface */
		IDirectDrawSurface_SetClipper(pBackSurface,pDirectDrawClipper);
	}

	// set palette
	DD_SetPalette(pPrimarySurface);

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

	{
		DDSURFACEDESC PrimarySurfaceDesc;

		// get primary surface description.
		// we need to check the width of the primary surface
		// so that we can see if the requested width is possible.
		// if not, we create a surface the same size as the primary,
		// and then crop the image when we draw it.
		PrimarySurfaceDesc.dwSize = sizeof(DDSURFACEDESC);
		IDirectDrawSurface_GetSurfaceDesc(pPrimarySurface, &PrimarySurfaceDesc);

		{
			RECT	ClientRect;
			int		Style, ExStyle;
			BOOL	HasMenu;

			// adjust client rect to be the width and height we want
			ClientRect.left = 0;
			ClientRect.top = 0;
			
			if (PrimarySurfaceDesc.dwWidth>Width)
			{
				ClientRect.right = Width;
			}
			else
			{
				ClientRect.right = PrimarySurfaceDesc.dwWidth;
			}
			
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
		}

		// setup back surface.
		SurfaceDescription.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		SurfaceDescription.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN; 
		SurfaceDescription.dwHeight = Height;

		// is the primary surface width less than requested width?
		if (PrimarySurfaceDesc.dwWidth<Width)
		{
			// yes.

			// DX3 doesn't support surfaces wider than Primary
			// so we will have to create a surface the same size
			// as the primary.
			SurfaceDescription.dwWidth = PrimarySurfaceDesc.dwWidth;
		}
		else
		{
			// width of surface is ok!
			SurfaceDescription.dwWidth = Width;
		}
	}

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



BOOL	DD_SetVideoMode(int Type,int Width,int Height,int Depth)
{
	if (Type==GRAPHICS_FULLSCREEN)
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

	if (Type==GRAPHICS_WINDOWED)
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

void	ExamineMode(MODE_DETAILS *pModeDetails)
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

////////////////////////////////////////////////////

char *ErrorToString(HRESULT error)
{
    switch(error) {
        case DD_OK:
            return "No error.\0";
        case DDERR_ALREADYINITIALIZED:
            return "This object is already initialized.\0";
        case DDERR_BLTFASTCANTCLIP:
            return "Return if a clipper object is attached to the source surface passed into a BltFast call.\0";
        case DDERR_CANNOTATTACHSURFACE:
            return "This surface can not be attached to the requested surface.\0";
        case DDERR_CANNOTDETACHSURFACE:
            return "This surface can not be detached from the requested surface.\0";
        case DDERR_CANTCREATEDC:
            return "Windows can not create any more DCs.\0";
        case DDERR_CANTDUPLICATE:
            return "Can't duplicate primary & 3D surfaces, or surfaces that are implicitly created.\0";
        case DDERR_CLIPPERISUSINGHWND:
            return "An attempt was made to set a cliplist for a clipper object that is already monitoring an hwnd.\0";
        case DDERR_COLORKEYNOTSET:
            return "No src color key specified for this operation.\0";
        case DDERR_CURRENTLYNOTAVAIL:
            return "Support is currently not available.\0";
        case DDERR_DIRECTDRAWALREADYCREATED:
            return "A DirectDraw object representing this driver has already been created for this process.\0";
        case DDERR_EXCEPTION:
            return "An exception was encountered while performing the requested operation.\0";
        case DDERR_EXCLUSIVEMODEALREADYSET:
            return "An attempt was made to set the cooperative level when it was already set to exclusive.\0";
        case DDERR_GENERIC:
            return "Generic failure.\0";
        case DDERR_HEIGHTALIGN:
            return "Height of rectangle provided is not a multiple of reqd alignment.\0";
        case DDERR_HWNDALREADYSET:
            return "The CooperativeLevel HWND has already been set. It can not be reset while the process has surfaces or palettes created.\0";
        case DDERR_HWNDSUBCLASSED:
            return "HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state.\0";
        case DDERR_IMPLICITLYCREATED:
            return "This surface can not be restored because it is an implicitly created surface.\0";
        case DDERR_INCOMPATIBLEPRIMARY:
            return "Unable to match primary surface creation request with existing primary surface.\0";
        case DDERR_INVALIDCAPS:
            return "One or more of the caps bits passed to the callback are incorrect.\0";
        case DDERR_INVALIDCLIPLIST:
            return "DirectDraw does not support the provided cliplist.\0";
        case DDERR_INVALIDDIRECTDRAWGUID:
            return "The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier.\0";
        case DDERR_INVALIDMODE:
            return "DirectDraw does not support the requested mode.\0";
        case DDERR_INVALIDOBJECT:
            return "DirectDraw received a pointer that was an invalid DIRECTDRAW object.\0";
        case DDERR_INVALIDPARAMS:
            return "One or more of the parameters passed to the function are incorrect.\0";
        case DDERR_INVALIDPIXELFORMAT:
            return "The pixel format was invalid as specified.\0";
        case DDERR_INVALIDPOSITION:
            return "Returned when the position of the overlay on the destination is no longer legal for that destination.\0";
        case DDERR_INVALIDRECT:
            return "Rectangle provided was invalid.\0";
        case DDERR_LOCKEDSURFACES:
            return "Operation could not be carried out because one or more surfaces are locked.\0";
        case DDERR_NO3D:
            return "There is no 3D present.\0";
        case DDERR_NOALPHAHW:
            return "Operation could not be carried out because there is no alpha accleration hardware present or available.\0";
        case DDERR_NOBLTHW:
            return "No blitter hardware present.\0";
        case DDERR_NOCLIPLIST:
            return "No cliplist available.\0";
        case DDERR_NOCLIPPERATTACHED:
            return "No clipper object attached to surface object.\0";
        case DDERR_NOCOLORCONVHW:
            return "Operation could not be carried out because there is no color conversion hardware present or available.\0";
        case DDERR_NOCOLORKEY:
            return "Surface doesn't currently have a color key\0";
        case DDERR_NOCOLORKEYHW:
            return "Operation could not be carried out because there is no hardware support of the destination color key.\0";
        case DDERR_NOCOOPERATIVELEVELSET:
            return "Create function called without DirectDraw object method SetCooperativeLevel being called.\0";
        case DDERR_NODC:
            return "No DC was ever created for this surface.\0";
        case DDERR_NODDROPSHW:
            return "No DirectDraw ROP hardware.\0";
        case DDERR_NODIRECTDRAWHW:
            return "A hardware-only DirectDraw object creation was attempted but the driver did not support any hardware.\0";
        case DDERR_NOEMULATION:
            return "Software emulation not available.\0";
        case DDERR_NOEXCLUSIVEMODE:
            return "Operation requires the application to have exclusive mode but the application does not have exclusive mode.\0";
        case DDERR_NOFLIPHW:
            return "Flipping visible surfaces is not supported.\0";
        case DDERR_NOGDI:
            return "There is no GDI present.\0";
        case DDERR_NOHWND:
            return "Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND.\0";
        case DDERR_NOMIRRORHW:
            return "Operation could not be carried out because there is no hardware present or available.\0";
        case DDERR_NOOVERLAYDEST:
            return "Returned when GetOverlayPosition is called on an overlay that UpdateOverlay has never been called on to establish a destination.\0";
        case DDERR_NOOVERLAYHW:
            return "Operation could not be carried out because there is no overlay hardware present or available.\0";
        case DDERR_NOPALETTEATTACHED:
            return "No palette object attached to this surface.\0";
        case DDERR_NOPALETTEHW:
            return "No hardware support for 16 or 256 color palettes.\0";
        case DDERR_NORASTEROPHW:
            return "Operation could not be carried out because there is no appropriate raster op hardware present or available.\0";
        case DDERR_NOROTATIONHW:
            return "Operation could not be carried out because there is no rotation hardware present or available.\0";
        case DDERR_NOSTRETCHHW:
            return "Operation could not be carried out because there is no hardware support for stretching.\0";
        case DDERR_NOT4BITCOLOR:
            return "DirectDrawSurface is not in 4 bit color palette and the requested operation requires 4 bit color palette.\0";
        case DDERR_NOT4BITCOLORINDEX:
            return "DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette.\0";
        case DDERR_NOT8BITCOLOR:
            return "DirectDrawSurface is not in 8 bit color mode and the requested operation requires 8 bit color.\0";
        case DDERR_NOTAOVERLAYSURFACE:
            return "Returned when an overlay member is called for a non-overlay surface.\0";
        case DDERR_NOTEXTUREHW:
            return "Operation could not be carried out because there is no texture mapping hardware present or available.\0";
        case DDERR_NOTFLIPPABLE:
            return "An attempt has been made to flip a surface that is not flippable.\0";
        case DDERR_NOTFOUND:
            return "Requested item was not found.\0";
        case DDERR_NOTLOCKED:
            return "Surface was not locked.  An attempt to unlock a surface that was not locked at all, or by this process, has been attempted.\0";
        case DDERR_NOTPALETTIZED:
            return "The surface being used is not a palette-based surface.\0";
        case DDERR_NOVSYNCHW:
            return "Operation could not be carried out because there is no hardware support for vertical blank synchronized operations.\0";
        case DDERR_NOZBUFFERHW:
            return "Operation could not be carried out because there is no hardware support for zbuffer blitting.\0";
        case DDERR_NOZOVERLAYHW:
            return "Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays.\0";
        case DDERR_OUTOFCAPS:
            return "The hardware needed for the requested operation has already been allocated.\0";
        case DDERR_OUTOFMEMORY:
            return "DirectDraw does not have enough memory to perform the operation.\0";
        case DDERR_OUTOFVIDEOMEMORY:
            return "DirectDraw does not have enough memory to perform the operation.\0";
        case DDERR_OVERLAYCANTCLIP:
            return "The hardware does not support clipped overlays.\0";
        case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
            return "Can only have ony color key active at one time for overlays.\0";
        case DDERR_OVERLAYNOTVISIBLE:
            return "Returned when GetOverlayPosition is called on a hidden overlay.\0";
        case DDERR_PALETTEBUSY:
            return "Access to this palette is being refused because the palette is already locked by another thread.\0";
        case DDERR_PRIMARYSURFACEALREADYEXISTS:
            return "This process already has created a primary surface.\0";
        case DDERR_REGIONTOOSMALL:
            return "Region passed to Clipper::GetClipList is too small.\0";
        case DDERR_SURFACEALREADYATTACHED:
            return "This surface is already attached to the surface it is being attached to.\0";
        case DDERR_SURFACEALREADYDEPENDENT:
            return "This surface is already a dependency of the surface it is being made a dependency of.\0";
        case DDERR_SURFACEBUSY:
            return "Access to this surface is being refused because the surface is already locked by another thread.\0";
        case DDERR_SURFACEISOBSCURED:
            return "Access to surface refused because the surface is obscured.\0";
        case DDERR_SURFACELOST:
            return "Access to this surface is being refused because the surface memory is gone. The DirectDrawSurface object representing this surface should have Restore called on it.\0";
        case DDERR_SURFACENOTATTACHED:
            return "The requested surface is not attached.\0";
        case DDERR_TOOBIGHEIGHT:
            return "Height requested by DirectDraw is too large.\0";
        case DDERR_TOOBIGSIZE:
            return "Size requested by DirectDraw is too large, but the individual height and width are OK.\0";
        case DDERR_TOOBIGWIDTH:
            return "Width requested by DirectDraw is too large.\0";
        case DDERR_UNSUPPORTED:
            return "Action not supported.\0";
        case DDERR_UNSUPPORTEDFORMAT:
            return "FOURCC format requested is unsupported by DirectDraw.\0";
        case DDERR_UNSUPPORTEDMASK:
            return "Bitmask in the pixel format requested is unsupported by DirectDraw.\0";
        case DDERR_VERTICALBLANKINPROGRESS:
            return "Vertical blank is in progress.\0";
        case DDERR_WASSTILLDRAWING:
            return "Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete.\0";
        case DDERR_WRONGMODE:
            return "This surface can not be restored because it was created in a different mode.\0";
        case DDERR_XALIGN:
            return "Rectangle provided was not horizontally aligned on required boundary.\0";
/*
        case D3DERR_BADMAJORVERSION:
            return "D3DERR_BADMAJORVERSION\0";
        case D3DERR_BADMINORVERSION:
            return "D3DERR_BADMINORVERSION\0";
        case D3DERR_EXECUTE_LOCKED:
            return "D3DERR_EXECUTE_LOCKED\0";
        case D3DERR_EXECUTE_NOT_LOCKED:
            return "D3DERR_EXECUTE_NOT_LOCKED\0";
        case D3DERR_EXECUTE_CREATE_FAILED:
            return "D3DERR_EXECUTE_CREATE_FAILED\0";
        case D3DERR_EXECUTE_DESTROY_FAILED:
            return "D3DERR_EXECUTE_DESTROY_FAILED\0";
        case D3DERR_EXECUTE_LOCK_FAILED:
            return "D3DERR_EXECUTE_LOCK_FAILED\0";
        case D3DERR_EXECUTE_UNLOCK_FAILED:
            return "D3DERR_EXECUTE_UNLOCK_FAILED\0";
        case D3DERR_EXECUTE_FAILED:
            return "D3DERR_EXECUTE_FAILED\0";
        case D3DERR_EXECUTE_CLIPPED_FAILED:
            return "D3DERR_EXECUTE_CLIPPED_FAILED\0";
        case D3DERR_TEXTURE_NO_SUPPORT:
            return "D3DERR_TEXTURE_NO_SUPPORT\0";
        case D3DERR_TEXTURE_NOT_LOCKED:
            return "D3DERR_TEXTURE_NOT_LOCKED\0";
        case D3DERR_TEXTURE_LOCKED:
            return "D3DERR_TEXTURELOCKED\0";
        case D3DERR_TEXTURE_CREATE_FAILED:
            return "D3DERR_TEXTURE_CREATE_FAILED\0";
        case D3DERR_TEXTURE_DESTROY_FAILED:
            return "D3DERR_TEXTURE_DESTROY_FAILED\0";
        case D3DERR_TEXTURE_LOCK_FAILED:
            return "D3DERR_TEXTURE_LOCK_FAILED\0";
        case D3DERR_TEXTURE_UNLOCK_FAILED:
            return "D3DERR_TEXTURE_UNLOCK_FAILED\0";
        case D3DERR_TEXTURE_LOAD_FAILED:
            return "D3DERR_TEXTURE_LOAD_FAILED\0";
        case D3DERR_MATRIX_CREATE_FAILED:
            return "D3DERR_MATRIX_CREATE_FAILED\0";
        case D3DERR_MATRIX_DESTROY_FAILED:
            return "D3DERR_MATRIX_DESTROY_FAILED\0";
        case D3DERR_MATRIX_SETDATA_FAILED:
            return "D3DERR_MATRIX_SETDATA_FAILED\0";
        case D3DERR_SETVIEWPORTDATA_FAILED:
            return "D3DERR_SETVIEWPORTDATA_FAILED\0";
        case D3DERR_MATERIAL_CREATE_FAILED:
            return "D3DERR_MATERIAL_CREATE_FAILED\0";
        case D3DERR_MATERIAL_DESTROY_FAILED:
            return "D3DERR_MATERIAL_DESTROY_FAILED\0";
        case D3DERR_MATERIAL_SETDATA_FAILED:
            return "D3DERR_MATERIAL_SETDATA_FAILED\0";
        case D3DERR_LIGHT_SET_FAILED:
            return "D3DERR_LIGHT_SET_FAILED\0";
*/
		default:
            return "Unrecognized error value.\0";
    }
}

void	DD_ErrorMessage(char *ErrorText)
{
	MessageBox(DDrawHwnd,ErrorText,"Ddraw Error",MB_ICONERROR|MB_OK);
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
	DDrawLibrary = LoadLibrary("DDRAW.DLL");

	if (DDrawLibrary!=NULL)
	{
		HINSTANCE	DInputLibrary;

		/* load Direct Input Library DINPUT.DLL */
		DInputLibrary = LoadLibrary("DINPUT.DLL");

		if (DInputLibrary!=NULL)
		{
			HINSTANCE	DSoundLibrary;

			/* load Direct Sound Library DSOUND.DLL */
			DSoundLibrary = LoadLibrary("DSOUND.DLL");

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
