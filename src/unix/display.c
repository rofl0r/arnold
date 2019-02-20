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
/* display */
#ifndef HAVE_SDL
#include "display.h"

Display *display;
Window window;
char xkey[128];
Screen *screen;
Colormap colormap;
XImage *image;
GC gc;
  int myscreen;
Atom	Protocols[1];

static unsigned char *image_buffer;
static XVisualInfo myvisual;
static unsigned long black_xpixel;

static XColor	PaletteColours[256];

void XWindows_InitialiseKeyboardMapping();


/* check there is a display, and check what depth it is */
Bool	XWindows_CheckDisplay()
{
	Screen *displayScreen;

	/* open display */
	display = XOpenDisplay(NULL);

	/* if failed to open display, return false */
	if (display==NULL)
		return False;

	/* check display is TrueColour.Arnold requires it.*/
	displayScreen = DefaultScreenOfDisplay(display);

	/* close the display */	
	XCloseDisplay(display);
	display = NULL;

	return True;
}


void	XWindows_GotVisual(int Width, int Height)
{
  XSetWindowAttributes winattr;
  XGCValues xgcv;
  XEvent event;
  XSizeHints hints;
  XWMHints wm_hints;
  int black_xpixel;
  int root_window_id;
	int event_mask;

//     printf("matched visual info\r\n");
	
//	printf("%d\r\n",myvisual.map_entries);

//	if (myvisual.depth!=8)
	{
     		colormap = DefaultColormapOfScreen(screen);
//	}
//	else
//	{
			

//	colormap = XCreateColormap(display, window
	}

     black_xpixel = BlackPixelOfScreen(screen);

     winattr.background_pixel = black_xpixel;
     winattr.border_pixel = WhitePixelOfScreen(screen);
     winattr.bit_gravity = ForgetGravity;
     winattr.win_gravity = NorthWestGravity;
     winattr.backing_store = NotUseful;
     winattr.override_redirect = False;
     winattr.save_under = False;
     winattr.event_mask = 0;
     winattr.do_not_propagate_mask = 0;
     winattr.colormap = colormap;
     winattr.cursor = None;
     //if (root_window_id == 0)
     root_window_id = RootWindowOfScreen(screen);
     
     window = XCreateWindow(display, root_window_id, 0,0,
		       Width, Height,0, myvisual.depth, InputOutput,
			    myvisual.visual, (CWBorderPixel | CWBackPixel
| CWBitGravity | CWWinGravity | CWBackingStore | CWOverrideRedirect |
CWSaveUnder | CWEventMask | CWDontPropagate | CWColormap |
CWCursor),&winattr);
     
     if (window)
       {
//	 printf("created window\r\n");

	/* enable closing of window */
	Protocols[0] = XInternAtom(display, "WM_CLOSE_WINDOW", True);

	XSetWMProtocols(display, window, Protocols, 1);

	 gc = XCreateGC(display, window, 0, &xgcv);

	XMapRaised(display, window);

	wm_hints.input = True;
	wm_hints.flags = InputHint;
	XSetWMHints(display, window, &wm_hints);
	
	/* set window min, max and base dimensions */
	hints.flags = PSize | PMinSize | PMaxSize;
	hints.min_width = hints.max_width = hints.base_width = Width;
	hints.min_height = hints.max_height = hints.base_height = Height;


	XSetWMNormalHints(display, window, &hints);


	 XClearWindow(display, window);


	/* set window title */
	XStoreName(display, window, "Arnold (c) Kevin Thacker");

	event_mask = (FocusChangeMask | ExposureMask | KeyPressMask | 
KeyReleaseMask);

	
	XSelectInput(display, window, event_mask);

	/* the window is opened if this event is used!!!! */
	XWindowEvent(display, window, ExposureMask, &event);

	image_buffer = malloc(sizeof(unsigned long)*Width*Height);

//	image = XCreatePixmap(display, window, width, height, 
//		myvisual.depth;

	image = XCreateImage(display, myvisual.visual,
		myvisual.depth, ZPixmap,
		0, (char *)image_buffer, Width, Height, 32,0);

//	memset(image_buffer,  0x0ff,sizeof(unsigned long)*Width*Height);
}
}



void    XWindows_SetDisplayWindowed(int Width, int Height, int Depth)
{
	XWindows_InitialiseKeyboardMapping();


 	display = XOpenDisplay(NULL);

	if (display!=NULL)
	{

  	screen = DefaultScreenOfDisplay(display);
  	myscreen = DefaultScreen(display);

	if (XMatchVisualInfo(display, myscreen, 8, PseudoColor,
&myvisual))
	{
		printf("8-bit paletted!\r\n");
		XWindows_GotVisual(Width, Height);
	}
	else
	if (XMatchVisualInfo(display, myscreen, 8, TrueColor, &myvisual))
	{
		/* true colour 8-bit */
		XWindows_GotVisual(Width, Height);
	}
	else if (XMatchVisualInfo(display, myscreen, 15,TrueColor,&myvisual))
	{
		/* true colour 15- bit */
		XWindows_GotVisual(Width, Height);
	}
      	else if (XMatchVisualInfo(display, myscreen, 16, TrueColor,&myvisual))
	{
		/* true colour 16-bit */
		XWindows_GotVisual(Width, Height);
	}
	else if (XMatchVisualInfo(display, myscreen, 24, TrueColor,&myvisual))
	{	
		/* true colour 24-bit */
		XWindows_GotVisual(Width, Height);
	}
	else if (XMatchVisualInfo(display, myscreen, 32, TrueColor,&myvisual))
	{	
		/* true colour 32-bit */
		XWindows_GotVisual(Width, Height);
	}
   }
}


#include "cpc/packedimage.h"

void
XWindows_GetGraphicsBufferColourFormat(GRAPHICS_BUFFER_COLOUR_FORMAT 
*pFormat)
{
	unsigned long BPP;
	unsigned long Shift;
	unsigned long Mask;

	Mask = image->red_mask;

	CalcShiftAndBPPFromMask(Mask, &BPP, &Shift);
	pFormat->Red.Mask = Mask;
	pFormat->Red.BPP = BPP;
	pFormat->Red.Shift = Shift;

	Mask = image->green_mask;
	CalcShiftAndBPPFromMask(Mask, &BPP, &Shift);
	pFormat->Green.Mask = Mask;
	pFormat->Green.BPP = BPP;
	pFormat->Green.Shift = Shift;
	
	Mask = image->blue_mask;
	CalcShiftAndBPPFromMask(Mask, &BPP, &Shift);
	pFormat->Blue.Mask = Mask;
	pFormat->Blue.BPP = BPP;
	pFormat->Blue.Shift = Shift;

	pFormat->BPP = image->depth;
	printf("BPP: %d\r\n",pFormat->BPP);
	printf("Red: M: %08x B: %d S: %d\r\n",pFormat->Red.Mask,
pFormat->Red.BPP, pFormat->Red.Shift);
	printf("Green: M: %08x B: %d S: %d\r\n", pFormat->Green.Mask,
pFormat->Green.BPP, pFormat->Green.Shift);
	printf("Blue: M: %08x B: %d S: %d\r\n",pFormat->Blue.Mask,
pFormat->Blue.BPP, pFormat->Blue.Shift);

}

void	XWindows_GetGraphicsBufferInfo(GRAPHICS_BUFFER_INFO *pBufferInfo)
{
	pBufferInfo->pSurface = image_buffer;
	pBufferInfo->Width = image->width;
	pBufferInfo->Height = image->height;
	pBufferInfo->Pitch = image->bytes_per_line;

//	printf("get buffer info\r\n");
//	printf("W: %d H: %d P: %d\r\n",image->width, image->height,
//image->bytes_per_line);
}

void	XWindows_SwapGraphicsBuffers(void)
{	
/*	XLockDisplay(display);*/

/*	int whitepixel = WhitePixelOfScreen(screen);

	XSetForeground(display, gc, whitepixel);

	XDrawRectangle(display, window, gc,0,0,100,100);
*/

if(XPutImage(display,window,gc,image,0,0,0,0,image->width,image->height)!=0)
{
	printf("some error occured");
}

	XFlush(display);	//GC(display, gc);

/*	XUnlockDisplay(display);*/
	}

#include "keyboard.c"
#endif		/* HAVE_SDL */
