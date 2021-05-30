/*
 * bookr-modern: a graphics based document reader 
 * Copyright (C) 2019 pathway27 (Sree)
 * IS A MODIFICATION OF THE ORIGINAL
 * Bookr and bookr-mod for PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com),
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com),
 * AND VARIOUS OTHER FORKS, See Forks in README.md
 * Licensed under GPLv3+, see LICENSE
*/

#include <string.h>
#include <stdlib.h>
#include "image.hpp"
#ifdef __APPLE__
static void* memalign(int t, int s) {
	return malloc(s);
}
#else
#include <malloc.h>
#endif

namespace bookr {

Image::Image(unsigned int w, unsigned int h, Image::Format f) : data(0), clut(0), clutSize(0) {
	/*assert(w > 0);
	assert(h > 0);
	assert(f == mono8 || f == mono16 || f == dual16 || f == rgb24 || f == rgb32 ||
		f == rgba32 || f == rgb56 || f == rgb64 || f == rgba64);*/
	width = w;
	height = h;
	imageFormat = f;
}

Image::~Image() {
	if (clut != NULL) {
		free(clut);
		clut = NULL;
	}
	if (data != NULL) {
		free(data);
		data = NULL;
	}
}

unsigned int Image::getBytesPerPixel() const {
	unsigned int bytesPP = 1;
	switch (imageFormat) {
		case mono8:
		break;
		case mono16:
		case dual16:
			bytesPP = 2;
		break;
		case rgb24:
			bytesPP = 3;
		break;
		case rgb32:
		case rgba32:
			bytesPP = 4;
		break;
	}
	return bytesPP;
}

Image::Format Image::getFormat() const {
	return imageFormat;
}

void Image::getDimensions(unsigned int& w, unsigned int &h) const {
	w = width;
	h = height;
}
	
char* Image::getData() {
	return data;
}

unsigned int* Image::getCLUT() {
	return clut;
}

unsigned int Image::getCLUTSize() {
	return clutSize;
}

Image* Image::createEmpty(unsigned int w, unsigned int h, unsigned int cs, Format f) {
	Image* image = new Image(w, h, f);
	/*image->data = (char*)memalign(16, h * w  * image->getBytesPerPixel());
	memset(image->data, 0, h * w  * image->getBytesPerPixel());
	if (cs > 0) {
		image->clutSize = cs;
		image->clut = (unsigned int*)memalign(16, cs * 4);
		memset(image->clut, 0, cs * 4);
	}*/
	return image;
}

Image* Image::createWithData(unsigned int w, unsigned int h, char* data) {
	Image* image = new Image(w, h, rgba32);
	image->data = data;
	return image;
}

void Image::swizzle(int sx, int sy) {
	//Image* from = createEmpty(width, height, 0, imageFormat);
	//char* fromData = from->getData();
	//int bpp = getBytesPerPixel();
	//char* fromData = (char*)memalign(16, width * height  * bpp);
	//char* toData = getData();
	//memcpy(fromData, toData, width * height * bpp);
	//int p = width * bpp;

	//for (unsigned int j = 0; j < height; j += sy) {
	//	for (int i = 0; i < p; i += sx) {
	//		char* base = &fromData[i + j*p];
	//		for (int k = 0; k < sy; k++) {
	//			memcpy(toData, base, sx);
	//			toData += sx;
	//			base += p;
	//		}
	//	}
	//}
	////from->release();
	//free(fromData);
}

Image* Image::createRGB24FromRGB32(Image* from) {
	if(from->getFormat() != Image::rgb32)
		return 0;

	unsigned int w, h;
	from->getDimensions(w, h);
	Image* to = createEmpty(w, h, 0, Image::rgb24);
        unsigned char* fromData = (unsigned char*)from->getData();
	unsigned char* toData = (unsigned char*)to->getData();
	unsigned char* fromP;
	unsigned char* fromQ = fromData;
	unsigned char* toP;
	unsigned char* toQ = toData;
	for (unsigned int j = 0; j < h; ++j) {
		fromP = fromQ;
		toP = toQ;
		for (unsigned int i = 0; i < w*3; ++i) {
			if ((i & 3) == 2) {
				++fromP;
			}
			*toP = *fromP;
			++toP;
			++fromP;
		}
		fromQ += w  * from->getBytesPerPixel();
		toQ += w  * to->getBytesPerPixel();
	}
	return to;
}

//Image* Image::createMipMap(Image* from, int level) {
//}

}