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

#ifndef __SOUND_HEADER_INCLUDED__
#define __SOUND_HEADER_INCLUDED__

#include "../cpc/cpcglob.h"
#include "../cpc/host.h"

enum {
	SOUND_PLUGIN_NONE=0,
	SOUND_PLUGIN_OSS,
	SOUND_PLUGIN_ALSA,
	SOUND_PLUGIN_ALSA_MMAP,
	SOUND_PLUGIN_SDL,
	SOUND_PLUGIN_AUTO,
	SOUND_PLUGIN_MAX = SOUND_PLUGIN_AUTO
};

extern const char *soundpluginNames[];

extern int sound_plugin;

static SOUND_PLAYBACK_FORMAT SoundFormat;
void convert8to16bit(signed short *ptr, int cptr);
int getSoundplugin(const char *s);
int autoDetectSoundplugin();
BOOL sound_throttle(void);
#endif

