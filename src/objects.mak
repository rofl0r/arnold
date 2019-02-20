CPC_O=	cpc/arnold.o cpc/asic.o cpc/audioevent.o cpc/bmp.o cpc/cpc.o \
	cpc/crtc.o cpc/dumpym.o cpc/fdc.o cpc/fdd.o cpc/fdi.o \
	cpc/garray.o cpc/multface.o cpc/printer.o cpc/psgplay.o \
	cpc/psg.o cpc/render.o cpc/render5.o \
	cpc/snapshot.o cpc/sampload.o cpc/spo256.o cpc/pal.o \
	cpc/voc.o cpc/tzxold.o cpc/wav.o cpc/westpha.o cpc/yiq.o \
	cpc/z8536.o cpc/csw.o cpc/cassette.o cpc/amsdos.o \
	cpc/debugger/gdebug.o cpc/debugger/breakpt.o \
	cpc/diskimage/diskimg.o cpc/ramrom.o \
	cpc/diskimage/dsk.o cpc/diskimage/extdsk.o \
	cpc/diskimage/iextdsk.o \
	cpc/z80/z80.o cpc/debugger/memdump.o \
	cpc/riff.o cpc/snapv3.o \
	cpc/messages.o

UNIX_O= unix/main.o unix/host.o unix/global.o unix/display.o \
	unix/display_sdl.o unix/gtkui.o unix/sdlsound.o unix/configfile.o \
	unix/roms.o unix/ifacegen.o unix/alsasound.o unix/alsasound-mmap.o \
	unix/alsasound-common.o unix/osssound.o unix/sound.o

