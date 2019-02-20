ARNOLD
======

A Amstrad/Schneider CPC, Plus/CPC+ and VEB Mikroelektronik KC Compact emulation.

The program and source are (c) Copyright Kevin Thacker 1995-2002.
<URL:mailto:amstrad@aiind.upv.es>

The ROMs are (c) Copyright Amstrad plc and Locomotive Software Ltd, All 
rights reserved.

Software:

Games:
<URL:ftp://ftp.nvg.ntnu.no/pub/cpc>

Demos, utilities:
<URL:ftp://ftp.lip6.fr/pub/amstrad>

Information:

Technical documentation:
<URL:http://andercheran.aiind.upv.es/~amstrad/>

Arnold Homepage:
<URL:http://arnold.emuunlim.com/>

MacOS port of Arnold:
<URL:http://www.bannister.org/>

Unix/Linux port of Arnold:
<URL:http://www.cpcszene.de/~nurgle/>

The roms have been distributed with this program with permission
of Amstrad plc.

This source is released under the GNU Public License. See the file "Copying"
for the full license.

License:

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


The MacOS version is maintained by Richard Bannister. The MacOS parts of the source code
are (c) Copyright Richard Bannister and are not distributed with this source archive.
I have given Richard permission to exclude them from the archive. His source
is not distributable and is not covered by the GNU Public License.

If you make a new port to another system, I would *prefer* if you would make your source
freely available under the GNU Public License and allow me to distribute it along with
the Arnold source.

NOTE:

Arnold is not completely bug free and therefore there might be some bugs in this 
version which could cause crashes or lockups.
YOU USE THIS EMULATOR AT YOUR OWN RISK. I have used this emulator a lot without
problem. Some features may not work or are not implemented fully, I am working towards
fixing these.

I know there are bugs in the Windows interface and I am working to fix these.
But, if you find a bug in the CPC emulation please tell me.

If there is a demo or program which doesn't display the screen correct, or the keyboard
doesn't respond when it should, or the sound is corrupted, please give me a description
of the problem and I will attempt to fix it.


Background
==========

What is the Amstrad/Schneider CPC?:
-----------------------------------

The Amstrad Colour Personal Computer (Amstrad CPC) is a 8-bit 
computer made by Amstrad plc in 1984. This computer was sold under the Schneider
brand name in Germany).

There were 3 Amstrad CPC systems:
- CPC 464 had 64K RAM, built in cassette-drive and the Amstrad CPC hardware design.
This system was released in 1984.
- CPC 664 had 64K RAM, built in disk drive and the Amstrad CPC hardware design
This system was released in 1985.
- CPC6128 had 128K RAM, built in disk drive and the Amstrad CPC hardware design
This system was released in 1985.

The Amstrad CPC design had the following features:
- a Z80 CPU running at 4Mhz
- a AY-3-8912 programmable sound generator running at 1Mhz
- a 6845 CRTC display processor generated the screen in partnership with the Gate-Array
- "Gate Array" custom chip providing:
	- 27 colours (3 intensities of R,G and B),
	- 3 display modes 
		- low resolution with 16 colours, 160x200 at standard screen size
		- medium resolution with 4 colours, 320x200 at standard screen size
		- high resolution with 2 colours, 640x200 at standard screen size
	- logic for ROM paging
	- logic for interrupt generation
- a 8255 Parallel Peripheral Interface connected to keyboard, AY-3-8912, joysticks and cassette
- digital joysticks

The Amstrad systems required a monitor to be used (because the power supply was hidden in the base
of the monitor), but this was a great idea because it meant there was only 1 plug for the whole lot.
The Amstrad CPC came with a green-screen monitor (shades of green), or a colour monitor.

What is the Amstrad Plus?:
--------------------------

In 1990, Amstrad produced a upgraded model, called the Amstrad Plus. This system
was backwards compatible with the Amstrad CPC. This design had the following features:
- the "CPC+ ASIC" combining the functions of the 6845, 8255 and "Gate Array"
- 4096 colour palette
- 16 hardware sprites
	- 16x16 pixels in dimension
	- can be magnified to x2 or x4 their original size
	- pixels have the same resolution as high resolution mode
	- 16 colours (independant palette to the background display)
- support for cartridges up to 512K 
- DMA list which can feed data to the sound chip
- programmable scan-line interrupts
- pixel smooth soft scroll
- screen split
- analogue joysticks

This model was produced too late, and by this time the Atari ST and Amiga had
taken the market.

The Amstrad Plus required a cartridge to run and a system cartridge was provided which
included BASIC, AMSDOS and the Burnin' Rubber game.

The Amstrad Plus could be bought with a colour monitor or a black and white monitor.


What is the KC Compact?:
------------------------

The KC Compact is a East-German clone of the Amstrad CPC. This system was made in 1989
to celebrate 40 years of the GDR (German Democractic Republic). Later in 1989, the Berlin
wall was taken down and East and West Germany merged. As a result production of the KC
Compact was halted. 

This system was compatible to the Amstrad CPC design but had the following differences:
- a Zilog Z8536 CIO chip was used to generate the interrupts allowing various frequencies
of interrupts
- the functions of the "Gate Array" were "emulated" using standard TTL logic.


What is emulated?
=================

Arnold is very accurate and there is a small number of programs (mainly demos) that do not display correctly
(especially those which use "horizontal splitting" technique). Arnold should run 99% of all programs
and will run these as they would on a real CPC.

Arnold has been written to be portable and to be the most accurate Amstrad CPC and KC Compact emulation,
and emulates many undocumented features.

- Full 1us cycle emulation
- undocumented features of the CPC+ asic 
- undocumented features of the various CRTC types
- undocumented opcodes of the Z80
- full support for Z80 interrupt mode 0, 1 and 2; interrupt mode 0 and mode 2 work as they would on the CPC+ 
(vectored interrupts supported, and the weird instruction execution of IM0)
- all the features of the CPC+ hardware (including the incompatibilities between the CPC+ and the CPC)
- all the features of the CPC hardware
- Genius mouse
- Digital joysticks
- different ram expansions: DK'tronics 64k or Dk'Tronics 256K Silicon Disc
- Multiface 2 (the final model with automatic switch)
- Digiblaster 8-bit D/A sound device
- Inicron RAMROM device
- tape emulation using tape images or sampled/digitised sounds
- disk emulation using disk images
- CPC+ cartridge emulation using cartridge image files
- scan-line interrupts from the Z8536 CIO for the KC Compact
- undocumented operation of the AY-3-8912 I/O ports.

What is not emulated?
=====================

- undocumented mode 3 in KC Compact mode
- analogue joysticks
- not all operations of the Z8536 in the KC Compact
- not all operations of the 8255 in the CPC are supported
- interrupt vector bug of CPC+ ASIC.
- keyboard "clash"
- undocumented flags are not perfect for all commands.
- cycle-level disc access; programs do not load at real speed.
- other Amstrad hardware... (send me some info and I will implement it, or add the code for yourself)
- the sound emulation could do with more work
- the effects of the monitor hsync and vsync which produces bending patterns
with some programmed values
- monitor brightness control
- still many bugs with the CRTC emulations.


SPEED:
------

The speed of the emulator is slow in comparison to other emulators.
Why? 
1) The program has been written in C. This was done so it could be ported
to other systems and to make development of the code quicker for me.
2) The emulation is very accurate and allows timing dependant programs
to run as they would on a real system (split rasters and 99% of demo
effects are emulated exact).

The speed of the emulator in CPC+ mode is slower when the CPC+
features are activated because the emulator must perform extra
functions to emulate this system.


Usage:
======


Windows version
---------------

To use this emulator you require:
- a fast PC (a Pentium II or better)
- Win95 with DirectX 5.0 or greater installed
- Win98
- Windows NT4 with at least Service Pack3
- Windows 2000
- Windows ME
 
You can download the DirectX re-distributable from Microsoft's web-site.
<URL:http://www.microsoft.com/directx/>

If the emulator doesn't run properly, you might not have DirectX
installed properly. Test on a game that you know requires DirectX
to run. If that game works correctly, then please tell me about your configuration.

The emulator will with 16-bit, 24-bit or 32-bit desktop resolution only.

The speed of the emulator is slow in comparison to other emulators.
Why? 
1) The program has been written in C. This was done so it could be ported
to other systems and to make development of the code quicker for me.
2) The emulation is very accurate and allows timing dependant programs
to run as they would on a real system (split rasters and 99% of demo
effects are emulated exact).

The speed of the emulator in CPC+ mode is slower when the CPC+
features are activated because the emulator must perform extra
functions to emulate this system.

Unix/Linux version
------------------

To use this system you require:
- a fast system
- X-Windows display (16-bit depth, 24-bit or 32-bit)
- for SDL version you require libsdl installed. You can get it from http://www.libsdl.org/
- for GTK+ version you require libgtk installed.

This version is more basic compared to the Windows version.
Some of the extra features in the Windows version have not yet been 
made available in the Unix version. Please be patient.

I have compiled and tested this version with Redhat Linux v7.0 on
my Pentium II system.

With the GTK/SDL version you will see a window where you can choose to open
a disc image (for drive A or B), reset the computer or Quit.

On the SDL version you can also choose a different rendering accuracy and full-screen mode.

F1 = reset
F2 = toggle between full-screen and windowed.

Please note that the Linux/Unix port is a work in progress and as a result there may
be bugs. However, Andreas Mickeli is working on it. He has done a great job so far,
and I know he will do a great job into the future.

There are various command-line options available:

-drivea <filename>	= specify disk image to insert into drive A
-driveb <filename>	= specify disk image to insert into drive B
-cart <filename>		= specify CPC+ cartridge to insert (will restart in CPC+ mode if not selected)
-frameskip <number>	= specify frame skip (0=none, 1=draw a frame, miss a frame, draw a frame etc
-crtctype <number>	= specify crtc type to use in the emulation (0=hd6845s/um6845, 1=um6845r, 2=mc6845, 3=CPC+ ASIC)
-tape <filename>		= specify .tzx/.cdt tape image to use
-cpctype <number>		= specify cpc type (0=cpc464, 1=cpc664,2=cpc6128,3=cpc464+,4=cpc6128+,5=kc compact
-snapshot <filename>	= specify snapshot (.sna) file to auto-start

For this version to start the rom files must be in a sub-directory below the directory
that the executable is in:

e.g.

arnold
roms/...


Software
========

To use software with this emulator it must be in a emulator friendly file format.

1) A disk image
This is a single file containing all the data from a Amstrad 3" disc in a special
layout. Disk images have the ".dsk" filename extension. Standard CPCEMU style disk 
images and Extended CPCEMU style disk images are supported.

2) A tape image
This is a single file containing a description of the sound waves that are stored
on a tape. Tape images have the ".tzx" or ".cdt" filename extension.

3) A sound sample
This is a file containing a digitised/sampled sound. The supported formats are uncompressed VOC
and uncompressed WAV. (not currently supported by the Unix version)

4) A cartridge image
This is a file containing all the data from a Amstrad CPC+ cartridge. A cartridge
is only useable with the Amstrad CPC+ emulation. A cartridge image has the ".cpr"
filename extension.

5) A snapshot
This is a file produced by the emulator and contains a snapshot of the Amstrad memory
and hardware state at the point the snapshot was taken. You can use a snapshot to save
your progress in a game so you can restart from the same point in the future.

A version 3 snapshot is written by default. This will also store CPC+ hardware state and
can be used to snapshot CPC+ demos and programs.

Quick Start guide
=================

Amstrad CPC commands
====================

NOTE: The "|" symbol is obtained by pressing SHIFT and @ together on the emulated Amstrad keyboard.
NOTE: [return] signifies that you press the RETURN key.
NOTE: Once the program has started to load follow on-screen prompts.

Disks
-----

Insert a disk into the drive and type:

CAT [return]

On most disks you will see a listing of the files on the disc. On some disks you may get the
following error message "Drive A: read fail, Retry Ignore or Cancel?". If you get an error
message then the disc may be copyprotected and may use the "|CPM" boot method to start it (see below).

To view a directory you can also type:

|DIR [return]

When a directory is displayed, you will see the filenames, their sizes, and the amount of free space
remaining on the disc.

On some discs you can run a program with one of the following names:

DISC.BAS
DISC.BIN
DISK.BAS
DISK.BIN
MENU.BAS
MENU.BIN

on other discs, these names will not exist, so it is trial and error to find the loader file, but the
following rules might help you:

1. look for a file with the extension ".BAS", ".BIN" or ".   " (3 spaces)
2. the file will often be one of the smallest (maybe a couple of k in size).
3. the file will be named similar to the name of the game you are trying e.g. CYBERND.BIN for Cybernoid

Try different files and you should find the correct one to run.
Once you have identified the filename type:

RUN"<filename> [return]

NOTE: replace <filename> with a name from the disk.
NOTE: if a file has a ".BAS", ".BIN" or ".   " extension, then you do not need to type this.
e.g. RUN"disc will run the disc.bas, disc.bin or disc.   file.

If you have chosen the correct file, then the program will load and run. If not the Amstrad
might crash or give an error message "Load failed...".

For some disks you might need to type:

|CPM [return]

This will boot the disc and run the program.

Tapes (samples and tape-images)
-----

"Insert" the sample or tape and type:

|TAPE [return]
RUN" [return]

you should see the following message:

"Press PLAY then any key:"

press the RETURN key and wait.
After some time you should see:

"Loading <file> block 1"
where <file> is the name of a file from the tape.
If you do not see this message "rewind" the tape and wait again.

Be prepared to wait for a long time, tapes are emulated in real time,
so if a program took 8 mins to load on a real CPC, it will also
take 8 mins to load under emulation!!!!

For some tape images you might need to "rewind tape to start" just after
you have issued this load command otherwise the initial loader file on
the cassette might be missed. The reason you need to do this is that
the tape-image might not have a pause at the beginning. The Amstrad waits
a short delay for the tape motor to achieve full speed and as a result
could miss files early in the tape image if this is not done.


Cartridge
---------

Insert the cartridge, the game should start automatically.

Snapshot
--------

Insert the snapshot, the program will restart automatically.

Quick start guide
=================

Windows version
---------------

File->Disk Drive A->Blank disc->Unformatted
	This will insert a unformatted disk image into Amstrad disk drive A.
	You can use a program to format the disc for use.

File->Disk Drive A->Insert Disk...
	This will display a file selector where you can choose the disk
	image to insert into Amstrad disk drive A.

File->Disk Drive A->Remove disk
	This will remove the disk image from Amstrad disk drive A.
	If you try drive functions then it will report the disc
	as missing. If there isn't a disk in the drive then this
	will not have any effect.

File->Disk Drive A->Turn over disk
	If you have a disk image which describes a double sided disk
	you can turn the disk over using this function. If the disk
	is single sided then this will not have any effect.

File->Disk Drive B
	see info for File->Disk Drive A

File->Cartridge->Insert Cartridge...
	This will display a file selector where you can choose to
	insert a cartridge image. If the CPC type is not CPC Plus,
	then CPC Plus will automatically be selected and the game
	will start automatically.

File->Cartridge->Insert System Cartridge
	This will insert the system cartridge into the computer
	so you can use BASIC.

File->Snapshot->Open Snapshot....
	Insert a snapshot the computer will restart using the snapshot
	information.

File->Snapshot->Write 64K snapshot...
	Save 64k of the Amstrad ram to a snapshot.

File->Snapshot->Write 128k snapshot...
	Save 128k of the Amstrad ram to a snapshot.

File->Snapshot->Set version to write... (2 or 3)
	Save a snapshot of the selected version. Version 2 is compatible with all
	emulators. Version 2 does not support CPC+ or KC Compact hardware.
	Version 3 is new and not supported by many emulators, but this
	version can be used to capture CPC+ hardware information.
	This version is not supported at this time but will be in a future
	release.

File->Tape->Insert Tape Image...
	With this option you can select the tape image to be used
	with the tape emulation (a tape image has the extension ".cdt" or ".tzx").
File->Tape->Remove Tape
	With this option you can remove the tape image and the emulated
	cassette player will be empty
File->Tape->Rewind to start
	This is the same as rewinding a tape to the beginning
File->Tape->Play button is pressed on tape recorder
	This is the same as pressing play on the tape player
File->Tape->Insert Sample...
	With this option you can select the WAV or VOC sound sample
	which is used for the tape emulation.


File->Rom Settings...
	With this option you can install additional rom's. 

	Select "Load" to load the rom-file into the selected slot.
	You can click on the white box to enable/disable the rom.
	When enabled a check-mark is shown, when disabled the box is
	empty.

	For these settings to take effect you must reset the Amstrad.
	Example rom's are Maxam (an assembler) and Protext (a word processor)


File->Write Screen Snapshot...
	With this option you can write the display to a Windows BMP picture file.


------------------------------------------

Misc->Reset
	Reset the Amstrad (same as switching it off and then on again)

Settings->Cheats->Cheat Database

	With this option you can use a CPCEMU style cheat database. You can
	use the cheat database to enter pokes so you can cheat.

Settings->Emulation Settings->Full Screen
	Switch the display into full-screen mode. (press F4 to return to
	windowed mode). You can also press F4 at any time to change between
	fullscreen and windowed mode)

Settings->Emulation Settings->Use joystick/joypad
	Select this option to use a PC joystick/joypad to simulate the Amstrad
	joystick, If this option is disabled then you can use the number pad
	to simulate the joystick (8 up, 2 down, 4 left, 6 right and 5 fire)

Settings->Emulation Settings->Frame Skip
	None
	1
	2
	3
	4

	You can use this option to speed up the emulation.
	None - all frames are drawn
	1 - one frame is drawn, one frame is skipped, one frame is drawn...
	2 - one frame is drawn, two frames are skipped, one frame is drawn...
	etc

Settings->Emulation Settings->Lock Speed
	If the emulation is running to fast use this option to lock the speed
	at 100% speed of a real CPC.

Settings->Emulation Settings->Task Priority
	Normal
	Priority

	You can use this option to speed up the emulation.
	Normal is the default setting. When Priority is selected
	Windows will give more time to the emulation, but other Windows
	functions will be slowed down or stop functioning.


Settings->Emulation Settings->Rendering Accuracy
	Low 
	High
	Higher

	In this mode you can choose the display accuracy.
	
	In "low" every other pixel in the horizontal is not plotted.
	With this rendering accuracy, mode 2 does not display correct.

	In "high" every pixel is drawn, and there are black lines between
	each line. This is used to simulate the effect of a tv screen.

	In "higher" all the pixels are drawn and the lines are doubled.


Settings->Emulation Settings->Sound Recording->Record sound as WAV
	Start recording
	Stop recording

	You can use these functions to record the sound to a Windows WAV sound file.

Settings->Emulation Settings->Sound Recording->ST Sound output
	Start Recording
	Stop Recording

	ST Sound is another program that can reply sounds made by the AY-3-8912
	sound chip (the sound chip inside the CPC). Using this you can record
	a tune from a game and then hear it on your PC without having to run
	the game on Arnold.


Settings->Amstrad Settings->CPC Type
	CPC 464,
	CPC 664,
	CPC 6128,
	CPC 464+
	CPC 6128+
	KC Compact

	This menu allows you to choose the CPC type to emulate. 
	At this time, CPC464 and CPC664 and CPC464+ all have 128k memory.
	This will be fixed in a future version.


	NOTE:

	- The CPC+ is not fully compatible to the CPC, and when CPC+ is emulated,
	some games will not work; the same as if you tried the real game on a real
	CPC+!

	- The KC Compact is not fully compatible to the CPC. When this mode
	is selected interrupts will be generated differently.

	- When CPC+ mode is selected, CRTC Type 3 is also automatically selected.
	This was the only CRTC type available for this machine.
	- When KC Compact mode is selected, CRTC Type 0 is also automatically selected.
	This was the only CRTC type available for this machine.

Settings->Amstrad Settings->CRTC Type
	Type 0 (HD6845S/UM6845) - CPC only
	Type 1 (UM6845R) - CPC only
	Type 2 (MC6845) - CPC only
	Type 3 (CPC+ ASIC) - CPC+ only
	Type 4 (Pre-ASIC) - CPC only

	This menu allows you to choose the CRTC type that is emulated.
	The CRTC type is often only important when looking at demos
	that require a specific version. 
	
	Some demos will auto-detect the CRTC type used and adapt themselves
	to work with it, however some demos will not work with all CRTC types!

	Although following the same design, each CRTC is slightly different
	in operation when programmed "to-the-limit".

	CRTC type 0 is the most compatible and will work with 90% of all
	demos, so it is advisable to choose this when running in CPC mode.

	NOTE:
		- You can't choose CRTC type 3 when in CPC mode.
		The CPC system was only available with types 0,1,2 and 4.
		- You can't choose CRTC type in CPC+ mode. CRTC type 3 is the only allowed type.
		- You can't choose CRTC type in KC Compact mode. CRTC type 0 is the only allowed type.


Settings->Amstrad Settings->Multiface->Multiface Stop
	This is the same as pressing the stop button of the multiface. For this
	option to work the multiface emulation must be enabled.

Settings->Amstrad Settings->Multiface->Multiface Settings...
	Here you can enable/disable the multiface emulation, and if it is detected
	after a reset (like in the real Multiface).

	Here you can also set the Multiface ROM to be used. There are two roms
	one for CPC and one for CPC+. The CPC version of the ROM does not work on the
	CPC+ and vice versa.

	The Multiface is a hardware device which can be connected to the back
	of the Amstrad CPC/Plus. This device has two buttons; a reset button and a stop
	button. When the stop button is pressed the Multiface takes control and a
	menu is shown.From this menu you can save the ram and hardware state to a file
	or edit the state of the hardware. You can also return back to the emulation
	at the point it was halted.
	If you have saved the state of the memory and hardware you can re-load this
	at a later date and restart your program from the point it was halted.

Settings->Amstrad Settings->Inicron RAM-ROM Settings
	The Inicron RAM-ROM is a device which connects to the expansion port.
	This device has RAM which you can install rom programs into.
	This device is ideal for developing and prototyping rom programs.
	The device can hold the data for 16 ROMs and you can choose which
	of the slots is enabled. You can also decide if the device is on,
	and if writing to it's ram is disabled.


Settings->Amstrad Settings->Ram Config
	64k 
	256k ram expansion
	256k silicon disk

	This option allows you to choose the additional ram hardware.
	On a real Amstrad these hardware devices would be connected
	to the expansion port and would provide additional ram.

	the 128k of the Amstrad CPC6128 is the same as choosing the
	Amstrad CPC464 with the 64k ram expansion.

Settings->Amstrad Settings->Monitor Type
	Colour
	Green Screen
	Grey Scale

	You can use this option to see how the program/game would
	look if one of these monitors was used.

	Green Screen can't be used in CPC+ mode and Grey Scale will
	be chosen instead.


Settings->Amstrad Settings->Printer output
	File
	Digiblaster

	If File is selected, any data written to the printer by the Amstrad
	will be sent to a file.

	If Digiblaster is selected, any data written to the printer will
	be sent to the Digiblaster device.

	The Digiblaster is a device which is connected to the Amstrad parallel
	printer connector and allows 8-bit sampled/digitised sound to be played
	by the Amstrad.


Settings->Amstrad Settings->Hardware on Joystick Port
	Joystick
	AMX Mouse
	Spanish Lightgun

	You can use this to choose the hardware connected to the Joystick
	port of the Amstrad.

	Options are:

	Joystick - standard digital joystick
	AMX Mouse - an AMX Mouse
	Spanish Lightgun - the Lightgun made by a Spanish company (don't know the name)


Debug->Debug Display
	Show the built in debugger. This option is useful for the advanced
	user.

	View->Open Dissassembly View
		This will show a dissassembly of some of the memory. You can use
		the cursor keys to move up and down memory or page up and page down
		to move faster.
		
		Pressing V will change the view between Z80 dissassembly,
		or dissassembly of each of the CPC+ DMA channels.
		
		When stepping through program code, the current program counter
		is shown with the grey bar.

		You can also position the grey bar on any line and press B to 
		place a breakpoint (press B again to remove breakpoint).

		When a breakpoint is placed execution will stop at this point
	
		If right mouse button is pressed you can change the viewing address.

	View->Open Memory Dump
		This will show a dump of the memory.
		You can view the memory as hex bytes and ASCII or hex-words.
		Press V to toggle between the views.
		
		You can move around with the cursor keys and use page up and down
		as in the dissassembly view.
		The grey cursor shows the current position.
		
		You can type in a hex value when you are in the hex area of the display
		and change the memory.

		If you type in the ASCII area this will change the memory, but you
		can type words and sentences in.

		If you type in the memory address area you can change the viewing
		address.

		You can press TAB to switch between areas.

		In this view you can also press the right mouse button, and from
		here you can set the viewing address or search for bytes.

		When searching for bytes type each byte in with space to seperate.
	
	View->Open CPC Hardware View
		Here you can view the state of the CPC Hardware (Gate Array palette
		registers, gate array mode and rom control etc). You can click on a
		item and edit it.
	View->Open CPC PLUS Info
		Here you can view the state of the CPC+ hardware. (see the
		CPC+ extended palette, DMA address etc)
	View->Open CRTC View
		Here you can view the state of the CRTC (see the values of it's
		internal registers and comparisons)

	On the main display you can step through a program, run to an address
	and edit the CPU registers.


Modes
	This gives a list of the modes available in full-screen mode.

STSound recording feature
=========================

The allows you to record the PSG output in the YM3
format which can be played with STSound. The output
file, has the extension ".YM3". Before this file can
be played with STSound, it must be converted into
a ".YM" file. This is done, by compressing the file
using LHA.

lha -h0 <filename>.ym <filename>.ym3

To start recording the sound, select "STSound->Start
Recording". You will be asked for the name of the output
".YM3" file. Recording will start, and will continue
until you select "STSound->Stop Recording". While
recording, a temporary file holds the raw PSG sound
data. When "Stop Recording" is selected, this raw data
is read in and converted to the ".YM3" format.

You can start and stop capture of PSG sound at any time.

This feature, currently, will only capture normal 
musics. It cannot handle Digi-Tracker music, or sounds
with Digi-effects. Therefore, it will not be able
to capture some CPC sounds properly.
