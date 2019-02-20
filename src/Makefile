# Makefile for Arnold

-include config.mak

include objects.mak

BIND = $(CC)
#   CFLAGS    flags for C compile
#   LFLAGS1   flags after output file spec, before obj file list
#   LFLAGS2   flags after obj file list (libraries, etc)

# Use this for Solaris 2.5.1 with OpenWin
#X11INC = -I/usr/X11R6/include -I/usr/openwin/share/include
#X11LIB = -L/usr/X11R6/lib -L/usr/openwin/lib

X11LIBS = $(X11LIB) -lX11 -lXext

#-Wall for max warnings!
CFLAGS = $(CFLAGS_CONFIGURE) $(GTKINC) $(SDLINC)$(X11INC) -I. -DUNIX -DCPC_LSB_FIRST -Wall -Wno-unused -Wno-pointer-sign

LFLAGS1 = $(LDFLAGS_CONFIGURE) $(GTKLIB) $(SDLLIB) $(X11LIBS)

ROMS_BIN=	roms/amsdose/amsdos.rom,roms/cpc464e/os.rom,roms/cpc464e/basic.rom,roms/cpc664e/os.rom,roms/cpc664e/basic.rom,roms/cpc6128e/os.rom,roms/cpc6128e/basic.rom,roms/cpcplus/system.cpr,roms/kcc/kccos.rom,roms/kcc/kccbas.rom

ELF_FMT := $(shell $(CC) -Wl,--print-output-format 2>/dev/null || true | head -n 1)

arnold: roms $(CPC_O) $(UNIX_O)
	$(BIND) -Wl,-b,binary,$(ROMS_BIN),-b,$(ELF_FMT) -o arnold $(LFLAGS1) $(CPC_O) \
	$(UNIX_O)
	cp arnold ../arnold

unix/display_sdl.o: unix/keyboard_sdl.c

roms:
	ln -s ../roms .

ctags:
	ctags -R
clean:
	rm $(UNIX_O) $(CPC_O)
	rm -f cpc/*.o
	rm -f ifacegen/*.o
	rm -f unix/*.o
	rm -f cpc/debugger/*.o
	rm -f cpc/diskimage/*.o
	rm -f cpc/z80/*.o
	rm -rf tags
distclean: clean
	rm -rf *~ */*~
	rm Makefile
	rm config.cache
	rm config.log
	rm config.status
realclean: distclean
	rm aclocal.m4
	rm configure
archive:
	$(clean)
	cp makefile unix/makefile.unx
	zip -r cpccore.zip cpc
	zip -r ifacegen.zip ifacegen
	zip -r unix.zip unix
	zip -r roms.zip roms

# C compile rules
#.c.o:
#	$(CC) -c $(CFLAGS) $<

#CPC Core
#arnold.o: cpc/arnold.o
#asic.o: cpc/asic.o
#audioevent.o: cpc/audioevent.o
#bmp.o: cpc/bmp.o
#cpc.o: cpc/cpc.o
#crtc.o: cpc/crtc.o
#debugmain.o: cpc/debugmain.o
#dirstuff.o: cpc/dirstuff.o
#dumpym.o: cpc/dumpym.o
#fdc.o: cpc/fdc.o
#garray.o: cpc/garray.o
#multface.o: cpc/multface.o
#printer.o: cpc/printer.o
#psg.o: cpc/psg.o
#render.o: cpc/render.o
#render5.o: cpc/render5.o
#scrsnap.o: cpc/scrsnap.o
#snapshot.o: cpc/snapshot.o
#spo256.o: cpc/spo256.o
#tapefile.o: cpc/tapefile.o
#tzx.o: cpc/tzx.o
#wav.o: cpc/wav.o
#westpha.o: cpc/westpha.o
#yiq.o: cpc/yiq.o
#debug.o: cpc/debugger/debug.o
#gdebug.o: cpc/debugger/gdebug.o
#diskimg.o: cpc/diskimage/diskimg.o
#dsk.o: cpc/diskimage/dsk.o
#extdsk.o: cpc/diskimage/extdsk.o
#iextdsk.o: cpc/diskimage/iextdsk.o
#z80.o: cpc/myz80/z80.o
#riff.o: cpc/riff.o

#Generic Interface
#ifacegen.o: ifacegen/ifacegen.o
#fnp.o: ifacegen/fnp.o
#romfn.o: ifacegen/romfn.o
#filetool.o: ifacegen/filetool.o

#Linux specific host code
#main.o: unix/main.o
#host.o: unix/host.o
#global.o: unix/global.o
#settings.o: unix/settings.o
#display.o: unix/display.o

