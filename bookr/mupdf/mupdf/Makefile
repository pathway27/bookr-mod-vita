TARGET_LIB=libmupdf.a
OBJS:=pdf_crypt.o pdf_debug.o pdf_doctor.o pdf_lex.o pdf_nametree.o pdf_open.o pdf_parse.o pdf_repair.o pdf_save.o pdf_stream.o pdf_xref.o
OBJS+=pdf_annot.o pdf_outline.o
OBJS+=pdf_fontagl.o pdf_fontenc.o pdf_cmap.o pdf_unicode.o pdf_font.o pdf_type3.o pdf_fontfile.o
OBJS+=pdf_function.o pdf_colorspace1.o pdf_colorspace2.o pdf_image.o pdf_pattern.o pdf_shade.o pdf_shade1.o pdf_shade4.o pdf_xobject.o
OBJS+=pdf_build.o pdf_interpret.o pdf_page.o pdf_pagetree.o pdf_resources.o pdf_store.o

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
