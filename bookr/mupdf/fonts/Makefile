TARGET_LIB=libfonts.a
OBJS:=Dingbats.o             NimbusMonL-ReguObli.o     NimbusRomNo9L-ReguItal.o  NimbusSanL-ReguItal.o
OBJS+=NimbusMonL-Bold.o      NimbusRomNo9L-Medi.o      NimbusSanL-Bold.o         StandardSymL.o
OBJS+=NimbusMonL-BoldObli.o  NimbusRomNo9L-MediItal.o  NimbusSanL-BoldItal.o     URWChanceryL-MediItal.o
OBJS+=NimbusMonL-Regu.o      NimbusRomNo9L-Regu.o      NimbusSanL-Regu.o

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


