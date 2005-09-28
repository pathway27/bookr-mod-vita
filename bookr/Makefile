# to fix: compile manually the mupdf libs first...
# cd mupdf; make

TARGET=bookr
OBJS:=bookr.o bkbook.o bkpdf.o bklayer.o bkmainmenu.o bkfilechooser.o bklogo.o bkpdf.o bkmemcpy.o bkuser.o bkbookmark.o
OBJS+=fzrefcount.o fzinstreammem.o fzinstreamstd.o fzimage.o fzimagepng.o fztexture.o fzfont.o fzscreenpsp.o fzscreencommon.o
OBJS+=tinystr.o tinyxmlerror.o tinyxml.o tinyxmlparser.o

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

#kxhidden: kxploit
#	mv ${TARGET} `scripts/kxhidden2.sh ${TARGET}`
#	mv ${TARGET}% `scripts/kxhidden.sh ${TARGET}`

dist: kxploit
	cp data bookr
