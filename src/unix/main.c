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
#include "../cpc/dirstuff.h"
#include "../ifacegen/ifacegen.h"
#include "gtkui.h"

#ifdef HAVE_SDL
#include <SDL.h>
#endif

#include "display.h"
#include <string.h>
#include <unistd.h>

/* FIXME: Move declarations to header file */
extern void	Host_InitDriveLEDIndicator();
extern void	Host_FreeDriveLEDIndicator();

/* main start for Arnold CPC emulator for linux */
int main(int argc, char *argv[])
{
	/* print welcome message */
	printf("Arnold Emulator (c) Kevin Thacker\r\n");

	if (!CPCEmulation_CheckEndianness())
	{
		printf("Program Compiled with wrong Endian settings.\r\n");
		exit(1);
	}
	
//	/* check display */
//	if (!XWindows_CheckDisplay())
//	{
//		printf("Failed to open display. Or display depth is  8-bit\r\n");
//		exit(-1);
//	}
	
	/* display ok, continue with emulation */

	DirStuff_Initialise();

	{
		char LocalDirectory[1024];
		char ProgramDirectory[1024]="";
		
		getcwd(ProgramDirectory, 1024);

sprintf(LocalDirectory,"%s/roms/amsdose/",ProgramDirectory);
	
	SetDirectoryForLocation(EMULATOR_ROM_CPCAMSDOS_DIR,
LocalDirectory);
	sprintf(LocalDirectory,"%s/roms/cpc464e/",ProgramDirectory);
		SetDirectoryForLocation(EMULATOR_ROM_CPC464_DIR,
LocalDirectory);
		
	sprintf(LocalDirectory,"%s/roms/cpc664e/",ProgramDirectory);
		SetDirectoryForLocation(EMULATOR_ROM_CPC664_DIR,
LocalDirectory);

	sprintf(LocalDirectory,"%s/roms/cpc6128e/",ProgramDirectory);
		SetDirectoryForLocation(EMULATOR_ROM_CPC6128_DIR,
LocalDirectory);

	sprintf(LocalDirectory,"%s/roms/cpcplus/",ProgramDirectory);
		SetDirectoryForLocation(EMULATOR_ROM_CPCPLUS_DIR,
LocalDirectory);

	sprintf(LocalDirectory,"%s/roms/kcc/",ProgramDirectory);
	SetDirectoryForLocation(EMULATOR_ROM_KCCOMPACT_DIR,
LocalDirectory);	


	GenericInterface_Initialise();

	if (CPCEmulation_Initialise())
	  {
		chdir(ProgramDirectory);

		CPC_SetCPCType(CPC_TYPE_CPC6128);

		if (argc!=0)
		{

			int argindex = 0;

			do
			{
				if (strcmp("-tape",argv[argindex])==0)
				{
					if ((argindex+1)<argc)
					{
						if
(!TapeImage_Insert(argv[argindex+1]))
{
	printf("Failed to open tape image %s.\r\n",argv[argindex+1]);
}
					}
				}

				if (strcmp("-drivea",argv[argindex])==0)
				{
					if ((argindex+1)<argc)
					{

if (!GenericInterface_InsertDiskImage(0, argv[argindex+1]))
{
	printf("Failed to open disk image %s.\r\n",argv[argindex+1]);
} 
					}
				}

				if (strcmp("-driveb",argv[argindex])==0)	
				{
					if ((argindex+1)<argc)
					{

if (!GenericInterface_InsertDiskImage(1, argv[argindex+1]))
{	
	printf("Failed to open disk image %s.\r\n",argv[argindex+1]);
}
					}
				}

				if (strcmp("-cart",argv[argindex])==0)
				{
					if ((argindex+1)<argc)
					{

if (!GenericInterface_InsertCartridge(argv[argindex+1]))
{
	printf("Failed to insert cartridge %s\r\n", argv[argindex+1]);
}
					}
				}
				
				if (strcmp("-frameskip",argv[argindex])==0)
				{
					int frameskip;

					frameskip =  atoi(argv[argindex+1]);

					CPC_SetFrameSkip(frameskip);
				}

				if (strcmp("-crtctype",argv[argindex])==0)
				{
					int crtc;
				
					crtc = atoi(argv[argindex+1]);
				
					CRTC_SetType(crtc);
				}
			
				if (strcmp("-cpctype", argv[argindex])==0)
				{
					int cpctype;

					cpctype = atoi(argv[argindex+1]);

					switch (cpctype)
					{
						case 0:
						{	
				CPC_SetCPCType(CPC_TYPE_CPC464);
				}
				break;
				case 1:
				{
				CPC_SetCPCType(CPC_TYPE_CPC664);
				}
				break;
				case 2:
				{
				CPC_SetCPCType(CPC_TYPE_CPC6128);
				}
				break;
				case 3:
				{
				CPC_SetCPCType(CPC_TYPE_464PLUS);
				}
				break;
				case 4:
				{
				CPC_SetCPCType(CPC_TYPE_6128PLUS);
				}
				break;

				case 5:
				{
				CPC_SetCPCType(CPC_TYPE_KCCOMPACT);
				}
				break;

				default:
				{
				CPC_SetCPCType(CPC_TYPE_CPC6128);
				}
				break;

				}
				}

				if (strcmp("-snapshot",argv[argindex])==0)
				{
if (!Snapshot_Load(argv[argindex+1]))
{	
	printf("Failed to open snapshot %s.\r\n",argv[argindex+1]);
}

	

				}

				if (strcmp("-help",argv[argindex])==0)
				{
					printf("Switches supported:\r\n");
					printf("-drivea <string> = specify disk image to insert into drive A\r\n");
					printf("-driveb <string> = specify disk image to insert into drive B\r\n");
					printf("-cart <string> = specify CPC+ cartridge to insert\r\n");				
					printf("-frameskip <integer> = specify frame skip (0-5)\r\n");
					printf("-crtctype <integer> = specify crtc type (0,1,2,3,4)\r\n");
					printf("-tape <string> = specify tape image\r\n");
					printf("-cpctype <integer> = specify CPC type (0=CPC464, 1=CPC664, 2=CPC6128, 3=CPC464+, 4=CPC6128+\r\n");
					printf("-snapshort <string> = specify snapshot to load\r\n");

				}
				argindex++;
			}
			while (argindex<argc);
		}

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
		atexit(SDL_Quit);
#endif

		Host_InitDriveLEDIndicator();

		Render_SetDisplayWindowed();

		printf("Initialised CPC Emulation Core...\r\n");

		CPC_SetAudioActive(TRUE);

		printf("Initialised Audio...\r\n");

		/* Enter GTK+ event loop when GTK+ is compiled in. Use own main loop
		 * otherwise. */
#ifdef HAVE_GTK
		gtkui_run();
#else
	    CPCEmulation_Run();
#endif

	    CPCEmulation_Finish();

		Host_FreeDriveLEDIndicator();
	  }
	}
	GenericInterface_Finish();

	DirStuff_Finish();

	exit(0);
}
