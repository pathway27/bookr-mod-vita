PACKAGE_NAME = djvulibre
PACKAGE_VERSION = 3.5.19

DEFS = -DHAVE_CONFIG_H
OPTS = -DNDEBUG -DNO_MMX=1 -O2 -G0 -Wall -Wno-non-virtual-dtor
CXXRPOFLAGS = 
JPEG_LIBS=-ljpeg
JPEG_CFLAGS=
THREAD_LIBS = 
#THREAD_CFLAGS = -DTHREADMODEL=COTHREADS
#THREAD_CFLAGS = -DTHREADMODEL=POSIXTHREADS
THREAD_CFLAGS = -DTHREADMODEL=NOTHREADS

srcdir = .
configdir = ./config
INCS = -I${srcdir} -I${configdir}
FLAGS = ${DEFS} ${INCS} ${OPTS} ${JPEG_CFLAGS} ${THREAD_CFLAGS}
LIBS=  ${JPEG_LIBS} ${THREAD_LIBS} -lm 
CFLAGS = ${FLAGS}   
CXXFLAGS =  ${FLAGS} ${CXXPICFLAGS} ${CXXRPOFLAGS}   

PSPSDK=$(shell psp-config --pspsdk-path)
PSPDIR=$(shell psp-config --psp-prefix)

TARGET_LIB = libdjvulibre.a
OBJS = \
	DjVuGlobal.o DjVuGlobalMemory.o GOS.o GException.o GContainer.o \
	GString.o GThreads.o GRect.o GSmartPointer.o  MMX.o Arrays.o \
	GBitmap.o GPixmap.o GScaler.o DjVuPalette.o ByteStream.o \
	IFFByteStream.o BSByteStream.o BSEncodeByteStream.o ZPCodec.o \
	JB2Image.o JB2EncodeCodec.o IW44Image.o IW44EncodeCodec.o \
	MMRDecoder.o JPEGDecoder.o DataPool.o GURL.o DjVuFileCache.o \
	DjVuPort.o GMapAreas.o DjVuAnno.o GIFFManager.o DjVuText.o \
	DjVuInfo.o DjVuFile.o DjVuImage.o DjVuDocument.o \
	DjVmDir.o DjVmDoc.o DjVmDir0.o DjVmNav.o DjVuNavDir.o \
	DjVuErrorList.o DjVuDocEditor.o DjVuToPS.o DjVuDumpHelper.o \
	DjVuMessageLite.o DjVuMessage.o GUnicode.o UnicodeByteStream.o \
	XMLParser.o XMLTags.o miniexp.o ddjvuapi.o debug.o

include $(PSPSDK)/lib/build.mak

install: $(TARGET_LIB)
	@echo "Installing libdjvulibre into $(PSPDIR)"
	@mkdir -p $(PSPDIR)/include $(PSPDIR)/lib
	@cp ddjvuapi.h miniexp.h $(PSPDIR)/include
	@cp libdjvulibre.a  $(PSPDIR)/lib
	@echo "Done"

# Dependencies
include Makefile.dep

