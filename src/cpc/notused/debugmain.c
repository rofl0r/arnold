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
#include "debugmain.h"



void	DebugMain_Initialise(void)
{
#ifdef DEBUG_MODE

#ifdef FDC_DEBUG
	FDC_Debug = Debug_Start("fdc-dbg.txt");
	Debug_Enable(FDC_Debug,FALSE);
#endif

#ifdef CRTC_DEBUG
	CRTC_Debug = Debug_Start("crtc-dbg.txt");
	Debug_Enable(CRTC_Debug,FALSE);
#endif

#ifdef ASIC_DEBUGGING
	ASIC_Debug = Debug_Start("asic-dbg.txt");
	Debug_Enable(ASIC_Debug,FALSE);
#endif

#ifdef AUDIOEVENT_DEBUG
	AudioEvent_Debug = Debug_Start("audio-dbg.txt");
	Debug_Enable(AudioEvent_Debug,FALSE);
#endif


#endif
}


void	DebugMain_Finish(void)
{
#ifdef DEBUG_MODE


#ifdef FDC_DEBUG
	Debug_End(FDC_Debug);
#endif

#ifdef CRTC_DEBUG
	Debug_End(CRTC_Debug);
#endif

#ifdef ASIC_DEBUGGING
	Debug_End(ASIC_Debug);
#endif

#ifdef AUDIOEVENT_DEBUG
	Debug_End(AudioEvent_Debug);
#endif

#endif
}

