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

#ifndef IMAGE_H
#define IMAGE_H

#include "refcount.hpp"
#include "inputstream.hpp"

namespace bookr {

/**
 * Static, application accesible images.
 * Image contains a contiguous area of memory that is readable/writable by the application,
 * and represents an image.
 */
class Image : public RefCounted {
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
