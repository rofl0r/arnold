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
#include "../cpc/arnold.h"
#include "../cpc/cpc.h"
#include "../cpc/host.h"
//#include "../cpc/dirstuff.h"
#include "ifacegen.h"
#include "configfile.h"
#include "gtkui.h"
#include "../cpc/messages.h"
#include "roms.h"
#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "sound.h"

#ifdef HAVE_SDL
#include <SDL.h>
#endif

#include "display.h"
#include <string.h>
#include <unistd.h>
#include <getopt.h>

/* FIXME: Move declarations to header file */
extern void	Host_InitDriveLEDIndicator();
extern void	Host_FreeDriveLEDIndicator();
#ifdef HAVE_GTK
extern GtkWidget *btn_double;
#endif
#ifdef HAVE_SDL
extern BOOL toggleFullscreenLater;
#endif

/* Forward declarations */
void init_main();
//char *getLocalIfNull(char *s);


/* 464 base system -> cassette only, 64k only */
void ConfigCPC464()
{
	CPC_SetOSRom(roms_cpc464.os.start);
	CPC_SetBASICRom(roms_cpc464.basic.start);
	Amstrad_DiscInterface_DeInstall();
	Amstrad_RamExpansion_DeInstall();
	CPC_SetHardware(CPC_HW_CPC);
  CPC_Reset();
}

/* 664 base system -> disc, 64k only */
void ConfigCPC664()
{
	CPC_SetOSRom(roms_cpc664.os.start);
	CPC_SetBASICRom(roms_cpc664.basic.start);
	CPC_SetDOSRom(rom_amsdos.start);
	Amstrad_DiscInterface_Install();
	Amstrad_RamExpansion_DeInstall();
	CPC_SetHardware(CPC_HW_CPC);
  CPC_Reset();
}

/* 6128 base system -> disc, 128k only */
void ConfigCPC6128()
{
	CPC_SetOSRom(roms_cpc6128.os.start);
	CPC_SetBASICRom(roms_cpc6128.basic.start);
	CPC_SetDOSRom(rom_amsdos.start);
	Amstrad_DiscInterface_Install();
	Amstrad_RamExpansion_Install();
	CPC_SetHardware(CPC_HW_CPC);
  CPC_Reset();
}

/* 6128 spanish base system -> disc, 128k only */
void ConfigCPC6128s()
{
	CPC_SetOSRom(roms_cpc6128s.os.start);
	CPC_SetBASICRom(roms_cpc6128s.basic.start);
	CPC_SetDOSRom(rom_amsdos.start);
	Amstrad_DiscInterface_Install();
	Amstrad_RamExpansion_Install();
	CPC_SetHardware(CPC_HW_CPC);
  CPC_Reset();
}

/* 464+ base system -> tape, 64k only */
void Config464Plus()
{
	CPC_SetHardware(CPC_HW_CPCPLUS);
	Cartridge_Insert(cartridge_cpcplus.start, cartridge_cpcplus.size);
	Amstrad_DiscInterface_DeInstall();
	Amstrad_RamExpansion_DeInstall();
  CPC_Reset();
}

/* 6128+ base system-> disc, 128k only */
void Config6128Plus()
{
	CPC_SetHardware(CPC_HW_CPCPLUS);
	Cartridge_Insert(cartridge_cpcplus.start, cartridge_cpcplus.size);
	Amstrad_DiscInterface_Install();
	Amstrad_RamExpansion_Install();
  CPC_Reset();
}

/* kc compact base system -> tape only, 64k */
void ConfigKCCompact()
{
	CPC_SetOSRom(roms_kcc.os.start);
	CPC_SetBASICRom(roms_kcc.basic.start);
	Amstrad_DiscInterface_DeInstall();
	Amstrad_RamExpansion_DeInstall();
	CPC_SetHardware(CPC_HW_KCCOMPACT);
  CPC_Reset();
}

/* main start for Arnold CPC emulator for linux */
int main(int argc, char *argv[])
{
	configInit();	//FIXME: disabled for debug

	/* print welcome message */
	printf("Arnold Emulator (c) Kevin Thacker\n");
	printf("Linux Port maintained by Andreas Micklei\n");
	roms_init();
	//printrom();

	if (!CPCEmulation_CheckEndianness())
	{
		printf("%s", Messages[72]);
		exit(1);
	}

//	/* check display */
//	if (!XWindows_CheckDisplay())
//	{
//		printf("Failed to open display. Or display depth is  8-bit\n");
//		exit(-1);
//	}

	 /* initialise cpc hardware */
	CPC_Initialise();

	Multiface_Install();

	/* done before parsing command line args. Command line args
	will take priority */
	loadConfigFile(); //FIXME: disabled for debug

	init_main(argc, argv);

	CPC_Finish();

	Multiface_DeInstall();

	//printf("heello");

	saveConfigFile(); //FIXME: disabled for debug

	configFree(); //FIXME: disabled for debug

	exit(0);

	return 0;	/* Never reached */
}


	void help_exit() {
		printf("Switches supported:\n");
		printf("-drivea <string> = specify disk image to insert into drive A\n");
		printf("-driveb <string> = specify disk image to insert into drive B\n");
		printf("-cart <string> = specify CPC+ cartridge to insert\n");
		printf("-frameskip <integer> = specify frame skip (0-5)\n");
		printf("-crtctype <integer> = specify crtc type (0,1,2,3,4)\n");
		printf("-tape <string> = specify tape image\n");
		printf("-cpctype <integer> = specify CPC type (0=CPC464, 1=CPC664, 2=CPC6128, 3=CPC464+, 4=CPC6128+\n");
		printf("-snapshort <string> = specify snapshot to load\n");
		printf("-kbdtype <integer> = specify keyboard type (0=QWERTY, 1=QWERTZ, 2=AZERTY)\n");
		printf("-soundplugin <integer> = specify sound output plugin\n                         (0=NONE, 1=OSS, 2=ALSA, 3=ALSA_MMAP, 4=SDL)\n");
#ifdef HAVE_SDL
		printf("-doublesize double the size of the emulator screen\n");
		printf("-fullscreen fullscreen mode\n");
#endif
		exit(0);
	}


void sdl_InitialiseJoysticks(void);

void init_main(int argc, char *argv[]) {
	int kbd = -1;
	/* name, has_arg, flag, val */
	static struct option long_options[] = {
		{"tape", 1, 0, 't'},
		{"drivea", 1, 0, 'a'},
		{"driveb", 1, 0, 'b'},
		{"cart", 1, 0, 'c'},
		{"frameskip", 1, 0, 'f'},
		{"crtctype", 1, 0, 'r'},
		{"cpctype", 1, 0, 'p'},
		{"snapshot", 1, 0, 's'},
		{"kbdtype", 1, 0, 'k'},
		{"soundplugin", 1, 0, 'o'},
#ifdef HAVE_SDL
		{"doublesize", 0, 0, 'd'},
		{"fullscreen", 0, 0, 'u'},
#endif
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0}
	};
	int c;
	int digit_optind = 0;
	char *tape = NULL;
	char *drivea = NULL;
	char *driveb = NULL;
	char *cart = NULL;
	char *frameskip = NULL;
	char *crtctype = NULL;
	char *cpctype = NULL;
	char *snapshot = NULL;
	char *kbdtype = NULL;
	char *soundplugin = NULL;
#ifdef HAVE_SDL
	BOOL doubled = FALSE;
	BOOL fullscreen = FALSE;
#endif
	do {
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		c = getopt_long_only (argc, argv, "",
			long_options, &option_index);
		printf("c: %i %c\n", c, c);
		switch(c) {
			case 'h':
			case '?':
				help_exit();
				break;
			case 't':
				tape = optarg;
				break;
			case 'a':
				drivea = optarg;
				break;
			case 'b':
				driveb = optarg;
				break;
			case 'c':
				cart = optarg;
				break;
#ifdef HAVE_SDL
			case 'd':
				doubled = TRUE;
				break;
			case 'u':
				fullscreen = TRUE;
				break;
#endif
			case 'f':
				frameskip = optarg;
				break;
			case 'r':
				crtctype = optarg;
				break;
			case 'p':
				cpctype = optarg;
				break;
			case 's':
				snapshot = optarg;
				break;
			case 'k':
				kbdtype = optarg;
				break;
			case 'o':
				soundplugin = optarg;
				break;

		}
	} while (c != -1);
	printf("tape: %s\n", tape);

	CPCEmulation_InitialiseDefaultSetup();

	ConfigCPC6128();

		if (tape) {
			if (!GenericInterface_InsertTape(tape)) {
				printf(Messages[73], tape);
			}
		}

		if (drivea) {
			if (!GenericInterface_InsertDiskImage(0, drivea)) {
				printf(Messages[74],
					drivea);
			}
		}

		if (driveb) {
			if (!GenericInterface_InsertDiskImage(1, driveb)) {
				printf(Messages[74],
					driveb);
			}
		}

		if (cart) {
			if (!GenericInterface_InsertCartridge(cart)) {
				printf(Messages[75], cart);
			}
		}

		if (frameskip) {
			int fskip;
			fskip =  atoi(frameskip);
			CPC_SetFrameSkip(fskip);
		}

		if (crtctype) {
			int crtc;
			crtc = atoi(crtctype);
			CRTC_SetType(crtc);
		}

		if (cpctype) {
			int cpc;
			cpc = atoi(cpctype);
			switch (cpc)
			{
						case 0:
						{
				ConfigCPC464();
				}
				break;
				case 1:
				{
				ConfigCPC664();
				}
				break;
				case 2:
				{
				ConfigCPC6128();
				}
				break;
				case 3:
				{
				ConfigCPC6128s();
				}
				break;
				case 4:
				{
				Config464Plus();
				}
				break;
				case 5:
				{
				Config6128Plus();
				}
				break;
				case 6:
				{
				ConfigKCCompact();
				}
				break;

				default:
				{
				ConfigCPC6128();
}
				break;

			}
		}

		if (snapshot) {
			if (!GenericInterface_LoadSnapshot(snapshot)) {
				printf(Messages[78],
					snapshot);
			}
		}

		if ( kbdtype ) kbd = atoi(kbdtype);
		printf("kbdtype: %i\n",kbd);

		if ( soundplugin ) sound_plugin = atoi(soundplugin);
		printf("soundplugin: %i (%s)\n",sound_plugin,soundpluginNames[sound_plugin]);


#ifdef HAVE_GTK
		fprintf(stderr, "Initializing GTK+\n");
		gtkui_init(argc, argv);
#endif

#ifdef HAVE_SDL
		fprintf(stderr, "Initializing SDL\n");
		if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_TIMER
			|SDL_INIT_JOYSTICK) < 0 ) {
			fprintf(stderr, "SDL could not be initialized: %s\n", SDL_GetError());
			exit(1);
		}
		sdl_InitialiseKeyboardMapping(0);
		sdl_InitialiseJoysticks();
		atexit(SDL_Quit);
#endif

		Host_InitDriveLEDIndicator();

		Render_SetDisplayWindowed();

#ifdef HAVE_SDL
		if (doubled) {
#ifdef HAVE_GTK
			gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON (btn_double), TRUE );
#else
			sdl_SetDoubled(doubled);
#endif
		}
		if (fullscreen) {
			toggleFullscreenLater = TRUE;
		}
		if (kbd != -1) sdl_InitialiseKeyboardMapping(kbd);
#endif
		printf("%s", Messages[76]);

		CPC_SetAudioActive(TRUE);

		printf("%s", Messages[77]);

		/* Enter GTK+ event loop when GTK+ is compiled in. Use own main loop
		 * otherwise. */
#ifdef HAVE_GTK
		gtkui_run();
#else
	    CPCEmulation_Run();
#endif

	//printf("aaaa");
	    //CPCEmulation_Finish();
	//printf("bbbb");
		Host_FreeDriveLEDIndicator();
}

