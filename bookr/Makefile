# to fix: compile manually the mupdf libs first...
# cd mupdf; make

PSP_EBOOT_ICON=data/icon0.png
TARGET=bookr
OBJS:=bookr.o bkbook.o bkpdf.o bklayer.o bkmainmenu.o bkfilechooser.o bklogo.o bkpdf.o bkmemcpy.o bkuser.o bkbookmark.o bkpopup.o bkcolorchooser.o
OBJS+=fzrefcount.o fzinstreammem.o fzinstreamstd.o fzimage.o fzimagepng.o fztexture.o fzfont.o fzscreenpsp.o fzscreencommon.o
OBJS+=tinystr.o tinyxmlerror.o tinyxml.o tinyxmlparser.o
OBJS+=res_uifont.o res_txtfont.o res_uitex.o res_logo.o

res_uifont.c: data/urwgothicb.pfb
	bin2c $< temp res_uifont
	sed s/static// temp > $@
	rm -f temp

res_txtfont.c: data/vera.ttf
	bin2c $< temp res_txtfont
	sed s/static// temp > $@
	rm -f temp

res_uitex.c: data/ui.png
	bin2c $< temp res_uitex
	sed s/static// temp > $@
	rm -f temp

res_logo.c: data/logo.png
	bin2c $< temp res_logo
	sed s/static// temp > $@
	rm -f temp

INCDIR =
CFLAGS = -Imupdf/include -G0 -Wall -O2 -I/usr/local/pspdev/psp/include/freetype2
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS=-Lmupdf/libs -lmupdf -lraster -lworld -lfonts -lstream -lbase -lpspgu -lpsppower -lpng -lz -ljpeg -lm -lfreetype -lstdc++ -lsupc++


EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Bookr - Book Reader

PSPSDK=$(shell psp-config --pspsdk-path)
#USE_PSPSDK_LIBC=1
include $(PSPSDK)/lib/build.mak

