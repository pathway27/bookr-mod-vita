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

#include <cstdio>
#include <cstdlib>
#include <png.h>

#ifdef MAC
static void* memalign(int t, int s) {
	return malloc(s);
}
#else
#include <malloc.h>
#endif

#include "fzimage.h"

static FZInputStream* inStream = 0;
//static void fz_png_read(png_structp t, png_bytep data, png_uint_32 size) {
#ifdef MAC
static void fz_png_read(png_structp t, png_bytep data, png_size_t size) {
	inStream->getBlock((char*)data, size);
#else
static void fz_png_read(png_structp t, png_bytep data, unsigned int size) {
	inStream->getBlock((char*)data, size);

#endif	
}

FZImage* FZImage::createFromPNG(FZInputStream* in) {
	png_byte header[8];
	// try to read the first 8 bytes
	if (in->getBlock((char*)header, 8) != 8)
		return 0;
	// check if the file is a PNG
	if (png_sig_cmp(header, 0, 8) != 0)
		return 0;

	// create a new png reader struct
	png_structp readStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		NULL, NULL, NULL);
	if (readStruct == NULL)
		return 0;

	// create a new png info struct
	png_infop infoStruct = png_create_info_struct(readStruct);
	if (infoStruct == NULL) {
		png_destroy_read_struct(&readStruct, (png_infopp)NULL,
			(png_infopp)NULL);
		return 0;
	}

	// read file
	//png_init_io(readStruct, fp);
	inStream = in;
	png_set_read_fn(readStruct, NULL, fz_png_read);
	png_set_sig_bytes(readStruct, 8);

	png_read_png(readStruct, infoStruct,
		PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING |
		PNG_TRANSFORM_SWAP_ENDIAN, NULL);

	// transform it!
/* only for low-level reads!!!!!!
	unsigned int bitDepth, colorType;
	colorType = png_get_color_type(readStruct, infoStruct);
	bitDepth  = png_get_bit_depth(readStruct, infoStruct);
	if (colorType == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(readStruct);
	if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
		png_set_gray_1_2_4_to_alpha(readStruct);
	if (colorType == PNG_COLOR_TYPE_RGB_ALPHA)
		png_set_swap_alpha(readStruct);
	if (bitDepth == 16)
		png_set_swap(readStruct);
	if (colorType == PNG_COLOR_TYPE_RGB ||
		colorType == PNG_COLOR_TYPE_RGB_ALPHA)
		png_set_bgr(readStruct);
	if (bitDepth == 8 && colorType = PNG_COLOR_TYPE_RGB)
		png_set_filler(readStruct, 255, PNG_FILLER_AFTER);
*/
	// interpret the header
	unsigned int w, h, bitDepth, colorType, channels, rowBytes;
	bool hasCLUT = false;
	FZImage::Format f = mono8;
	w         = png_get_image_width(readStruct, infoStruct);
	h         = png_get_image_height(readStruct, infoStruct);
	bitDepth  = png_get_bit_depth(readStruct, infoStruct);
	colorType = png_get_color_type(readStruct, infoStruct);
	channels  = png_get_channels(readStruct, infoStruct);
	rowBytes  = png_get_rowbytes(readStruct, infoStruct);

	// decide data format
	switch (colorType) {
		case PNG_COLOR_TYPE_GRAY:
			f = mono8;
		break;
		case PNG_COLOR_TYPE_PALETTE:
			f = mono8;
			hasCLUT = true;
		break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			f = dual16;
		break;
		case PNG_COLOR_TYPE_RGB:
			f = rgb24;
		break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			f = rgba32;
		break;
		default:
			// clean libpng structs
			png_destroy_read_struct(&readStruct, &infoStruct, (png_infopp)NULL);
			return 0;			
		break;
	}

	// allocate data buffer
	FZImage* image = new FZImage(w, h, f);
	//image->pitch = rowBytes;
	//image->data = (char*)malloc(w * image->getPitch() * image->getBytesPerPixel());
	image->data = (char*)memalign(16, h * w * image->getBytesPerPixel());
	memset(image->data, 0, h * w * image->getBytesPerPixel());

	// copy the CLUT
	// TODO: support single transparency for grayscale - is it useful?
	if (f == mono8 && hasCLUT) {
		png_colorp palette = NULL;
		png_bytep trans = NULL;
		int num_palette = 0;
		int num_trans = 0;
		png_color_16p trans_values; // for non-PLTE images
		png_get_PLTE(readStruct, infoStruct, &palette, &num_palette);
		png_get_tRNS(readStruct, infoStruct, &trans, &num_trans, &trans_values);
		//image->clut = (unsigned int*)malloc(num_palette * sizeof(unsigned int));
		image->clut = (unsigned int*)memalign(16, num_palette * sizeof(unsigned int));
		image->clutSize = num_palette;
		int i, col;
		for (i = 0; i < num_palette; i++) {
			col = (palette[i].red) | (palette[i].green << 8) | (palette[i].blue << 16);
			if (trans != NULL && i < num_trans) {
				col |= trans[i] << 24;
			} else {
				col |= 0xff000000;
			}
			image->clut[i] = col;
		}
	}

	// access & copy the data
	png_bytep *rowPointers = png_get_rows(readStruct, infoStruct);
	unsigned int j;
	unsigned char* cData = (unsigned char*)image->data;
	for (j = 0; j < image->height; j++) {
		memcpy(cData, rowPointers[j], rowBytes);
		cData += rowBytes;
	}

	// clean libpng structs
	png_destroy_read_struct(&readStruct, &infoStruct, (png_infopp)NULL);

	inStream = 0;

	return image;
}

