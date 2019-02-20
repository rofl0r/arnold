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

#include "../cpc/cpcglob.h"
#include "sound.h"
#include "alsasound-common.h"
#include "osssound.h"
#include <string.h>

extern int sound_plugin;

const char *soundpluginNames[] = {
	[SOUND_PLUGIN_NONE] = "NONE",
	[SOUND_PLUGIN_OSS] = "OSS",
	[SOUND_PLUGIN_ALSA] ="ALSA",
	[SOUND_PLUGIN_ALSA_MMAP] = "ALSAMMAP",
	[SOUND_PLUGIN_SDL] = "SDL",
	[SOUND_PLUGIN_AUTO] = "AUTO"
};

void convert8to16bit(signed short *ptr, int cptr) {
	signed short *dest;
	unsigned char *src;
	int srcbytes;
	//fprintf(stderr,"convert8to16bit ptr: %i, cptr:%i\n", ptr, cptr);
	srcbytes = cptr*4;
	dest = ptr + (srcbytes-2);
	src = (unsigned char *) (ptr + (srcbytes/2-1));
	//src = ptr + (srcbytes/2-1);
	while(srcbytes-- > 0) {
		//fprintf(stderr,"convert8to16bit dest: %i, src:%i, d_dest: %i, d_src: %i\n", dest, src, (((long)dest)-((long)ptr)), (((long)src)-((long)ptr)));
		*dest-- = ((*src--)-128)<<8;
	}
}

int getSoundplugin(const char *s) {
	int i;
	for (i=0; i<SOUND_PLUGIN_MAX; i++) {
		if (!strcmp(soundpluginNames[i],s)) {
			return i;
		}
	}
	return SOUND_PLUGIN_AUTO;
}

int autoDetectSoundplugin() {
	if (alsa_AudioPlaybackPossible()) {
		return SOUND_PLUGIN_ALSA;
	}
	if (oss_AudioPlaybackPossible()) {
		return SOUND_PLUGIN_OSS;
	}
	return SOUND_PLUGIN_NONE;
}

BOOL sound_throttle(void) {
	switch(sound_plugin) {
		case SOUND_PLUGIN_OSS:
			return oss_Throttle();
		case SOUND_PLUGIN_ALSA:
			return alsa_Throttle();
		case SOUND_PLUGIN_ALSA_MMAP:
			return alsa_Throttle();
		case SOUND_PLUGIN_SDL: /* fall through */
		default:
			return FALSE;
	}
}

