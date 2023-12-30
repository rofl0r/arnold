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
void init_main(int argc, char *argv[]);
//char *getLocalIfNull(char *s);

static void
ConfigRomOverrides()
{
	const char *rPath = NULL;
	char filename[512] = { 0, };
	char *pRomData = NULL;
	unsigned long RomSize = 0;

	rPath = getRomDirectory();
	/*
	 * If we have a ROM directory then check in each
	 * for the ROMs that we could load.
	 */
	if (rPath == NULL) {
		return;
	}

	/* OS ROM image */
	filename[0] = 0x0;
	snprintf(filename, sizeof(filename) - 1,
	    "%s/%s", rPath, "os.rom");
	LoadFile(filename, &pRomData, &RomSize);
	if (pRomData != NULL) {
		CPC_SetOSRom(pRomData);	/* XXX size? */
		free(pRomData);
	}
	pRomData = NULL;
	RomSize = 0;

	/* BASIC */
	filename[0] = 0x0;
	snprintf(filename, sizeof(filename) - 1,
	    "%s/%s", rPath, "basic.rom");
	LoadFile(filename, &pRomData, &RomSize);
	if (pRomData != NULL) {
		CPC_SetBASICRom(pRomData);	/* XXX size? */
		free(pRomData);
	}
	pRomData = NULL;
	RomSize = 0;

	/* DOS */
	filename[0] = 0x0;
	snprintf(filename, sizeof(filename) - 1,
	    "%s/%s", rPath, "amsdos.rom");
	LoadFile(filename, &pRomData, &RomSize);
	if (pRomData != NULL) {
		CPC_SetDOSRom(pRomData);	/* XXX size? */
		free(pRomData);
	}
	pRomData = NULL;
	RomSize = 0;
}

/* 464 base system -> cassette only, 64k only */
void ConfigCPC464()
{
	CPC_SetOSRom(roms_cpc464.os.start);
	CPC_SetBASICRom(roms_cpc464.basic.start);
	ConfigRomOverrides();
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
	ConfigRomOverrides();
	Amstrad_DiscInterface_Install();
	Amstrad_RamExpansion_DeInstall();
	CPC_SetHardware(CPC_HW_CPC);
  CPC_Reset();
}

/* 6128 base system -> disc, 128k only */
void ConfigCPC6128()
{
	/* Setup default ROMs first */
	CPC_SetOSRom(roms_cpc6128.os.start);
	CPC_SetBASICRom(roms_cpc6128.basic.start);
	CPC_SetDOSRom(rom_amsdos.start);
	ConfigRomOverrides();
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
	ConfigRomOverrides();
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
	ConfigRomOverrides();
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
		printf("-snapshot <string> = specify snapshot to load\n");
		printf("-kbdtype <integer> = specify keyboard type (0=QWERTY, 1=QWERTZ, 2=AZERTY)\n");
		printf("-soundplugin <string> = specify sound output plugin\n                         (NONE, OSS, ALSA, ALSAMMAP, SDL, PULSE, AUTO)\n");
#ifdef HAVE_SDL
		printf("-doublesize double the size of the emulator screen\n");
		printf("-fullscreen fullscreen mode\n");
#endif
		exit(0);
	}


void sdl_InitialiseJoysticks(void);

/* name, has_arg, flag, val */
static struct option long_options[] = {
	{"tape", required_argument, NULL, 't'},
	{"drivea", required_argument, NULL, 'a'},
	{"driveb", required_argument, NULL, 'b'},
	{"cart", required_argument, NULL, 'c'},
	{"frameskip", required_argument, NULL, 'f'},
	{"crtctype", required_argument, NULL, 'r'},
	{"cpctype", required_argument, NULL, 'p'},
	{"snapshot", required_argument, NULL, 's'},
	{"kbdtype", required_argument, NULL, 'k'},
	{"soundplugin", required_argument, NULL, 'o'},
#ifdef HAVE_SDL
	{"doublesize", no_argument, NULL, 'd'},
	{"fullscreen", no_argument, NULL, 'u'},
#endif
	{"help", no_argument, NULL, 'h'},
	{NULL, 0, NULL, 0}
};

void init_main(int argc, char *argv[]) {
	int kbd = -1;
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
	int option_index = 0;

	while((c = getopt_long_only (argc, argv, "",
			long_options, &option_index)) != -1) {
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
	}

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

		if ( soundplugin ) {
			sound_plugin = getSoundplugin(soundplugin);
		} else {
			sound_plugin = SOUND_PLUGIN_AUTO;
		}
		if ( sound_plugin == SOUND_PLUGIN_AUTO ) {
			sound_plugin = autoDetectSoundplugin();
		}
		printf("soundplugin: %i (%s)\n",sound_plugin,soundpluginNames[sound_plugin]);

#ifdef HAVE_XINITTHREADS
		extern void XInitThreads(void);
		XInitThreads();
#elif defined(HAVE_GTK) && defined(HAVE_SDL)
	#warning "XInitThreads not found, but having both SDL and GTK active-will probably crash at runtime"
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

#ifdef HAVE_GTK
		fprintf(stderr, "Initializing GTK+\n");
		gtkui_init(argc, argv);
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

