ARNOLD
======

A Amstrad/Schneider CPC, Plus/CPC+ and VEB Mikroelektronik KC Compact emulation.

The program and source are (c) Copyright Kevin Thacker 1995-2002.
<URL:mailto:amstrad@aiind.upv.es>

The ROMs are (c) Copyright Amstrad plc and Locomotive Software Ltd, All 
rights reserved.

How to use CPC:
./arnold -drivea foo.dsk
list files:
CAT
mode 2:cat
|DIR
execute .bas or .bin
run"batman.bas
(note single quote)
the file extension .bas or .bin can be omitted

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
<URL:http://arnold.berlios.de/>

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
