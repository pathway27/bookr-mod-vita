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

#include <stddef.h>

#include "fztexture.h"
#include "fzscreen.h"

#ifdef PSP

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspgu.h>
#define printf	pspDebugScreenPrintf

#else

#include <stdio.h>

#ifdef MAC
#include <GLUT/glut.h>
#elif defined (__CYGWIN__)
#include "cygwin/freeglut.h"
#else
#include <GL/freeglut.h>
#endif

#endif

#ifdef PSP

FZTexture::FZTexture() : texImage(0) {
}

FZTexture::~FZTexture() {
	if (texImage)
		texImage->release();
	texImage = NULL;
}

void FZTexture::bind() {
	FZScreen::setBoundTexture((FZTexture*)this);
}

void FZTexture::bindForDisplay() {
	// support only for main ram textures - very slow
	// TODO: investigate vram textures: when and how to
	// transfer?

	if (texImage->getCLUT() != NULL) {
		int n = texImage->getCLUTSize();
		sceGuClutMode(GU_PSM_8888, 0, n - 1, 0); // 32-bit palette
		sceGuClutLoad((n/8), texImage->getCLUT()); // upload entries (8 colors base unit)
	}
	sceGuTexFunc(texenv, pixelComponent);
	//sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
	sceGuTexMode(pixelFormat, 0, 0, GU_TRUE);
	//sceGuTexMode(pixelFormat, 0, 0, GU_FALSE);
	sceGuTexImage(0, width, height, width, texImage->getData());
	sceGuTexScale(1.0f, 1.0f);
	sceGuTexOffset(0.0f, 0.0f);
	sceGuTexFilter(texMin, texMag);
	FZScreen::setBoundTexture((FZTexture*)this);
}

#else

FZTexture::FZTexture() : texImage(0) {
	glGenTextures(1, &textureObject);
}

FZTexture::~FZTexture() {
	glDeleteTextures(1, &textureObject);
}

void FZTexture::bind() {
	glBindTexture(GL_TEXTURE_2D, textureObject);
	FZScreen::setBoundTexture((FZTexture*)this);
}

void FZTexture::bindForDisplay() {
	bind();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texenv);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texMag);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texMin);
}

#endif

static bool validatePow2(unsigned int x, unsigned int maxPow) {
	bool valid = false;
	unsigned int i, s;
	for (i = 0; i < maxPow; ++i) {
		s = 1 << i;
		if (x == s) {
			valid = true;
			break;
		}
	}
	return valid;
}

#ifdef PSP

bool FZTexture::validateFormat(FZImage* image) {
	FZImage::Format imageFormat = image->getFormat();
	pixelFormat = GU_PSM_T8;
	pixelComponent = GU_TCC_RGBA;
	switch (imageFormat) {
		case FZImage::mono8:
		break;
#if 0
		case FZImage::mono16:
		break;
		case FZImage::dual16:
		break;
#endif
		case FZImage::rgb24:
			pixelFormat = GU_PSM_T8;		// not
			pixelComponent = GU_TCC_RGB;
		break;
		case FZImage::rgb32:
			pixelFormat = GU_PSM_8888;
			pixelComponent = GU_TCC_RGB;
		break;
		case FZImage::rgba32:
			pixelFormat = GU_PSM_8888;
		break;
		default:
			return false;
	}
	return true;
}

#else

bool FZTexture::validateFormat(FZImage* image) {
	FZImage::Format imageFormat = image->getFormat();
	internalFormat = GL_ALPHA8;
	textureFormat = GL_ALPHA;
	textureType = GL_UNSIGNED_BYTE;
	switch (imageFormat) {
		case FZImage::mono8:
		break;
#if 0
		case FZImage::mono16:
			internalFormat = GL_ALPHA16;
			textureType = GL_UNSIGNED_SHORT;
		break;
		case FZImage::dual16:
			internalFormat = GL_LUMINANCE8_ALPHA8;
			textureFormat = GL_LUMINANCE_ALPHA;
		break;
#endif
		case FZImage::rgb24:
			internalFormat = GL_RGB8;
			textureFormat = GL_RGB;
		break;
		case FZImage::rgb32:
			internalFormat = GL_RGB8;
			textureFormat = GL_RGB;
		break;
		case FZImage::rgba32:
			internalFormat = GL_RGBA8;
			textureFormat = GL_RGBA;
		break;
		default:
			return false;
		break;
	}
	return true;
}
#endif

FZTexture* FZTexture::createFromImage(FZImage* image, bool buildMipmaps) {
	FZTexture* texture = new FZTexture();
	if (!initFromImage(texture, image, buildMipmaps)) {
		texture->release();
		texture = 0;
	}
	return texture;
}
	
bool FZTexture::initFromImage(FZTexture* texture, FZImage* image, bool buildMipmaps) {

	unsigned int width = 0, height = 0;

	image->getDimensions(width, height);

	// \todo query max texture size
	// 2 pow 10 == 1024
	if (!validatePow2(width, 10) || !validatePow2(height, 10)) {
		//printf("invalid dimensions %d x %d\n", width, height);
		return false;
	}

	texture->width = width;
	texture->height = height;

	if (!texture->validateFormat(image)) {
		//printf("invalid format\n");
		return false;
	}

	void* data = image->getData();
	FZImage* buffer = 0;
	if (image->getFormat() == FZImage::rgb32) { 
		buffer = FZImage::createRGB24FromRGB32(image);
		data = buffer->getData();
	}


#ifdef PSP
	image->retain();
	// BUG: swizzle is not working above 8bpp
	image->swizzle(16, 8);	// swizzle is always 16x8 bytes
	texture->texImage = image;
#else
	texture->bind();
	glEnable(GL_TEXTURE_2D);

	//glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment);
	if (buildMipmaps) {
		glGetError();
		gluBuild2DMipmaps(GL_TEXTURE_2D, texture->internalFormat, width, height,
			texture->textureFormat, texture->textureType, data);
		GLenum er = glGetError();
		if (er != GL_NO_ERROR) {
			printf("FZTextureData2D::createFromImage: %s\n", gluErrorString(er));
		}
	} else {
		glGetError();
		glTexImage2D(GL_TEXTURE_2D, 0, texture->internalFormat, width, height, 0,
			texture->textureFormat, texture->textureType, data);
		GLenum er = glGetError();
		if (er != GL_NO_ERROR) {
			printf("FZTextureData2D::createFromImage: %s\n", gluErrorString(er));
		}
	}
#endif

	if (buffer != 0) {
		buffer->release();
	}

	return true;
}

void FZTexture::texEnv(int op) {
#ifdef PSP
	texenv = op;
#else
	texenv = GL_REPLACE;
	switch (op) {
		case FZ_TEX_MODULATE: texenv = GL_MODULATE; break;
		case FZ_TEX_DECAL: texenv = GL_DECAL; break;
		case FZ_TEX_BLEND: texenv = GL_BLEND; break;
		case FZ_TEX_ADD: texenv = GL_ADD; break;
	}
#endif	
}

void FZTexture::filter(int min, int mag) {
#ifdef PSP
	texMin = min;
	texMag = mag;
#else
	texMin = GL_NEAREST;
	switch (min) {
		case FZ_LINEAR: texMin = GL_LINEAR; break;
		case FZ_NEAREST_MIPMAP_NEAREST: texMin = GL_NEAREST_MIPMAP_NEAREST; break;
		case FZ_LINEAR_MIPMAP_NEAREST: texMin = GL_LINEAR_MIPMAP_NEAREST; break;
		case FZ_NEAREST_MIPMAP_LINEAR: texMin = GL_NEAREST_MIPMAP_LINEAR; break;
		case FZ_LINEAR_MIPMAP_LINEAR: texMin = GL_LINEAR_MIPMAP_LINEAR; break;
	}
	texMag = mag == FZ_LINEAR ? GL_LINEAR : GL_NEAREST;
#endif
}

