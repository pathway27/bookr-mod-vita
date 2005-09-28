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

#ifndef FZTEXTURE_H
#define FZTEXTURE_H

#include "fzimage.h"

#define FZ_TEX_MODULATE  0
#define FZ_TEX_DECAL     1
#define FZ_TEX_BLEND     2
#define FZ_TEX_REPLACE   3
#define FZ_TEX_ADD       4

#define FZ_NEAREST                 0
#define FZ_LINEAR                  1
#define FZ_NEAREST_MIPMAP_NEAREST  4
#define FZ_LINEAR_MIPMAP_NEAREST   5
#define FZ_NEAREST_MIPMAP_LINEAR   6
#define FZ_LINEAR_MIPMAP_LINEAR    7

/**
 * Represents a 2D texture.
 */
class FZTexture : public FZRefCounted {
	// common
	unsigned int width, height;
	int texenv, texMin, texMag;

	// psp
	unsigned int pixelFormat;
	unsigned int pixelComponent;
	FZImage* texImage;
	//void* imageData;
	//void* clutData;

	// ogl
	unsigned int textureObject;
	int internalFormat;
	int textureFormat;
	int textureType;

	bool validateFormat(FZImage*);

protected:
	FZTexture();
	~FZTexture();
	static bool initFromImage(FZTexture* texture, FZImage* image, bool buildMipmaps);

public:

	/**
	 * Set texture op for combining with the current fragment color.
	 */
	void texEnv(int op);

	/**
	 * Set texture filtering.
	 */
	void filter(int min, int max);

	/**
	 * Make the texture the current texture. NOTE: remove this method ASAP
	 */
	void bind();

	/**
	 * Make the texture the current texture and reissue the declaration/upload.
	 */
	void bindForDisplay();

	/**
	 * Create a new 2D texture object from an image.
	 */
	static FZTexture* createFromImage(FZImage* image, bool buildMipmaps);

	unsigned int getWidth() const { return width; }
	unsigned int getHeight() const { return height; }
};

#endif
