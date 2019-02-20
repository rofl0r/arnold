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
#ifndef __CPCDEFS_HEADER_INCLUDED__
#define __CPCDEFS_HEADER_INCLUDED__

/* defines to control parts of CPC emulation etc */

/* define for "least-significant-byte" first/little endian machines */ 
#define CPC_LSB_FIRST

/*#define DEBUG_MODE*/

#ifdef DEBUG_MODE
/* enable debug hooks */
/*#define DEBUG_HOOKS*/
/* enable fdc debug output */
#define FDC_DEBUG
/* enable crtc debug output */
#define CRTC_DEBUG
/* enable asic debug output */
/*#define ASIC_DEBUGGING */
#define KEY_DEBUG

#define TZX_DEBUG
#endif

/* enable output of ay data to file */
#define	AY_OUTPUT
/* enable tape image/tzx loading */
#define TZX
/* enable audio event */
#define AUDIOEVENT
/* SPO256, DK'Tronics speech emulation enable */
#define SPEECH


#define INTERNAL
/*#define USE_INLINE */
#ifndef UNIX
#endif

#define MULTIFACE

#define SIMPLE_MONITOR_EMULATION

/*#define TIME_EMULATION */

/* use pow (yiq.c monitor brightness stuff) */
/*#define POW*/
#endif
