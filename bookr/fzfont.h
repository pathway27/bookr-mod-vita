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

#ifndef FZFONT_H
#define FZFONT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "fztexture.h"

struct FZCharMetrics {
	short x, y, width, height, xoffset, yoffset, xadvance;
};

/**
 * Represents a font.
 */
class FZFont : public FZTexture {

	FZCharMetrics* metrics;
	int lineHeight;
	static FZFont* createProto(FT_Library& library, FT_Face& face, int fontSize, bool autohint);

protected:
	FZFont();
	~FZFont();

public:
	/**
	 * Get line height
	 */
	int getLineHeight();

	/**
	 * Get char metrics. Array is always for 256 chars, for the time being...
	 */
	FZCharMetrics* getMetrics();

	/**
	 * Create a new font texture with Freetype.
	 */
	static FZFont* createFromFile(char* fileName, int fontSize, bool autohint);
	static FZFont* createFromMemory(unsigned char* buffer, int bufferSize, int fontSize, bool autohint);
};

#endif

