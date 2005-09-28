/*
 * Bookr: document reader for the Sony PSP 
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "fzimage.h"
#include <string.h>
#include <stdlib.h>
//#include <assert.h>

#ifdef MAC
static void* memalign(int t, int s) {
	return malloc(s);
}
#else
#include <malloc.h>
#endif

FZImage::FZImage(unsigned int w, unsigned int h, FZImage::Format f) : data(0), clut(0), clutSize(0) {
	/*assert(w > 0);
	assert(h > 0);
	assert(f == mono8 || f == mono16 || f == dual16 || f == rgb24 || f == rgb32 ||
		f == rgba32 || f == rgb56 || f == rgb64 || f == rgba64);*/
	width = w;
	height = h;
	imageFormat = f;
}

FZImage::~FZImage() {
	if (clut != NULL) {
		free(clut);
		clut = NULL;
	}
	if (data != NULL) {
		free(data);
		data = NULL;
	}
}

unsigned int FZImage::getBytesPerPixel() const {
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

FZImage::Format FZImage::getFormat() const {
	return imageFormat;
}

void FZImage::getDimensions(unsigned int& w, unsigned int &h) const {
	w = width;
	h = height;
}
	
char* FZImage::getData() {
	return data;
}

unsigned int* FZImage::getCLUT() {
	return clut;
}

unsigned int FZImage::getCLUTSize() {
	return clutSize;
}

FZImage* FZImage::createEmpty(unsigned int w, unsigned int h, unsigned int cs, Format f) {
	FZImage* image = new FZImage(w, h, f);
	image->data = (char*)memalign(16, h * w  * image->getBytesPerPixel());
	memset(image->data, 0, h * w  * image->getBytesPerPixel());
	if (cs > 0) {
		image->clutSize = cs;
		image->clut = (unsigned int*)memalign(16, cs * 4);
		memset(image->clut, 0, cs * 4);
	}
	return image;
}

void FZImage::swizzle(int sx, int sy) {
	FZImage* from = createEmpty(width, height, 0, imageFormat);
	char* fromData = from->getData();
	char* toData = getData();
	int bpp = getBytesPerPixel();
	memcpy(fromData, toData, width * height * bpp);
	int p = width * bpp;
	for (unsigned int j = 0; j < height; j += sy) {
		for (int i = 0; i < p; i += sx) {
			char* base = &fromData[i + j*p];
			for (int k = 0; k < sy; k++) {
				memcpy(toData, base, sx);
				toData += sx;
				base += p;
			}
		}
	}
	from->release();
}

FZImage* FZImage::createRGB24FromRGB32(FZImage* from) {
	if(from->getFormat() != FZImage::rgb32)
		return 0;

	unsigned int w, h;
	from->getDimensions(w, h);
	FZImage* to = createEmpty(w, h, 0, FZImage::rgb24);
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
			if (i & 3 == 2) {
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

//FZImage* FZImage::createMipMap(FZImage* from, int level) {
//}


