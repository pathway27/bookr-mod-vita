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

#ifndef IMAGE_H
#define IMAGE_H

#include "refcount.h"
#include "inputstream.h"

namespace bookr {

/**
 * Static, application accesible images.
 * Image contains a contiguous area of memory that is readable/writable by the application,
 * and represents an image.
 */
class Image : public RefCounted {
private:
	char* data;
	unsigned int* clut;
	unsigned int clutSize;

	unsigned int width, height;
	Format imageFormat;

protected:
	Image(unsigned int w, unsigned int h, Format f);
	~Image();

public:
	enum Format
	{
		rgba32,
		rgb32,
		rgb24,
		dual16,
		mono16,
		mono8
	};

	/**
	 * Get the image logical format.
	 */
	Format getFormat() const;
	/**
	 * Get the image bytes per pixel.
	 */
	unsigned int getBytesPerPixel() const;
	/**
	 * Get the image logical dimensions.
	 */
	void getDimensions(unsigned int& w, unsigned int &h) const;
	/**
	 * Get a pointer to the image data.
	 */
	char* getData();
	/**
	 * Get a pointer to the color table data; CLUT are always 32bpp RGBA
	 */
	unsigned int* getCLUT();
	/**
	 * Get color table size
	 */
	unsigned int getCLUTSize();

	void swizzle(int sx, int sy);

	/**
	 * Create an image from a PNG file.
	 */
	static Image* createFromPNG(InputStream* in);
	/**
	 * Create an image from a JPEG file.
	 */
	//static Image* createFromJPEG(InputStream* in);
	/**
	 * Create an empty image.
	 * An image is created with the specified parameters, and its data is
	 * initialized to 0
	 */
	static Image* createEmpty(unsigned int w, unsigned int h, unsigned int cl, Format f);

	static Image* createWithData(unsigned int w, unsigned int h, char* data);

	/**
	 * Create a rgb24 format image from a rgb32 format image.
	 */
	static Image* createRGB24FromRGB32(Image* from);
	/**
	 * Create an N 2-exponent lower resolution image.
	 * Requires a power of two image size on both axis. Returns a new image with N 2-exponent
	 * less width and height.
	 */
	//static Image* createMipMap(Image* from, int level);
};

}

#endif
