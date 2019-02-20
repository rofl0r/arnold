# Makefile for Arnold

-include config.mak

include objects.mak
include roms.mak

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
