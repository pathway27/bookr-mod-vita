TARGET_LIB=libapps.a
OBJS=pdfapp.o

INCDIR=../include
CFLAGS=-G0 -Wall -O2 -I/usr/local/pspdev/psp/include/freetype2
CXXFLAGS=$(CFLAGS) -fno-exceptions -fno-rtti -I/usr/local/pspdev/psp/include/freetype2
ASFLAGS=$(CFLAGS)

LIBDIR=
LDFLAGS=
LIBS=-lpspgu -lz -lm -lfreetype

#EXTRA_TARGETS = EBOOT.PBP
#PSP_EBOOT_TITLE = Bookr - Book Reader

PSPSDK=$(shell psp-config --pspsdk-path)
#USE_PSPSDK_LIBC=1
include $(PSPSDK)/lib/build.mak
