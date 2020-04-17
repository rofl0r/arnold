# Makefile for Arnold

-include config.mak

include objects.mak
include roms.mak

X11LIBS = $(X11LIB) -lX11 -lXext

INCLUDES = $(GTKINC) $(SDLINC) $(X11INC)
LIBS = $(GTKLIB) $(SDLLIB) $(X11LIBS) $(ALSALIB)

CFLAGS = $(CFLAGS_CONFIGURE) $(INCLUDES) -I. -DUNIX -DCPC_LSB_FIRST -Wall -Wno-unused -Wno-pointer-sign
LDFLAGS = $(LDFLAGS_CONFIGURE)

ELF_FMT := $(shell $(CC) -Wl,--print-output-format 2>/dev/null || true | head -n 1)

arnold: roms $(CPC_O) $(UNIX_O)
	$(CC) -Wl,-b,binary,$(ROMS_BIN),-b,$(ELF_FMT) -o arnold \
	$(LDFLAGS) $(CPC_O) $(UNIX_O) $(LIBS)

unix/display_sdl.o: unix/keyboard_sdl.c

roms:
	ln -s ../roms .

clean:
	rm -f $(UNIX_O) $(CPC_O)
