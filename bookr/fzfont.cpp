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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef MAC
static void* memalign(int t, int s) {
	return malloc(s);
}
#else
#include <malloc.h>
#endif

#include "fzfont.h"

FZFont::FZFont() : metrics(0) {
}

FZFont::~FZFont() {
	if (metrics)
		free(metrics);
	metrics = 0;
}

// based on http://gpwiki.org/index.php/OpenGL_Font_System

FZFont* FZFont::createFromFile(char* fileName, int fontSize, bool autohint) {
	// This initializes FreeType
	FT_Library library;
	if (FT_Init_FreeType(&library) != 0) {
		printf("cannot init freetype\n");
		return 0;
	}

	// Load the font
	FT_Face face;
	if (FT_New_Face(library, fileName, 0, &face) != 0) {
		printf("cannot load font\n");
		return 0;
	}

	return createProto(library, face, fontSize, autohint);
}

FZFont* FZFont::createFromMemory(unsigned char* buffer, int bufferSize, int fontSize, bool autohint) {
	// This initializes FreeType
	FT_Library library;
	if (FT_Init_FreeType(&library) != 0) {
		printf("cannot init freetype\n");
		return 0;
	}

	// Load the font
	FT_Face face;
	if (FT_New_Memory_Face(library, buffer, bufferSize, 0, &face) != 0) {
		printf("cannot load font\n");
		return 0;
	}

	return createProto(library, face, fontSize, autohint);
}

FZFont* FZFont::createProto(FT_Library& library, FT_Face& face, int fontSize, bool autohint) {
	// Margins around characters to prevent them from 'bleeding' into
	// each other.
	int margin = 3;
	int image_height = 0, image_width = 256;

	// Abort if this is not a 'true type', scalable font.
	if (!(face->face_flags & FT_FACE_FLAG_SCALABLE) or !(face->face_flags & FT_FACE_FLAG_HORIZONTAL)) {
		printf("lame font\n");
		return 0;
	}

	// Set the font size
	FT_Set_Pixel_Sizes(face, fontSize, 0);

	// First we go over all the characters to find the max descent
	// and ascent (space required above and below the base of a
	// line of text) and needed image size. There are simpler methods
	// to obtain these with FreeType but they are unreliable.
	int max_descent = 0, max_ascent = 0;
	int space_on_line = image_width - margin, lines = 1;

	int loadMode = FT_LOAD_DEFAULT;
	if (autohint)
		loadMode |= FT_LOAD_FORCE_AUTOHINT;
	for (int i = 0; i < 256; ++i) {
		// Look up the character in the font file.
		int char_index = FT_Get_Char_Index(face, i);
		if (i < 32)
			char_index = 0;
	
		// Render the current glyph.
		FT_Load_Glyph(face, char_index, loadMode);
		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
	
		int advance = (face->glyph->metrics.horiAdvance >> 6) + margin;
		if (advance > space_on_line) {
			space_on_line = image_width - margin;
			++lines;
		}
		space_on_line -= advance;

		int t = face->glyph->bitmap_top;
		if (t > max_ascent)
			max_ascent = t;
		t = face->glyph->bitmap.rows - face->glyph->bitmap_top;
		if (t > max_descent)
			max_descent = t;
	}

	// Compute how high the texture has to be.
	int needed_image_height = (max_ascent + max_descent + margin) * lines + margin;
	// Get the first power of two in which it fits.
	image_height = 16;
	while (image_height < needed_image_height)
		image_height *= 2;

	// Allocate memory for the texture
	FZImage* image = FZImage::createEmpty(image_width, image_height, 256, FZImage::mono8);
	unsigned char* fontTexture = (unsigned char*)image->getData();
	// add clut to texture
	unsigned int* clut = image->getCLUT();
	for (int i = 0; i < 256; i++) {
		clut[i] = 0xffffff | (i << 24);
	}
	//for (int i = 0; i < image_height * image_width; ++i)
	//	fontTexture[i] = 0;
	int fontLineHeight = max_ascent + max_descent + margin;

	// Allocate space for the GlyphEntries
	//std::vector<GlyphEntry> entries(chars.size());
	struct FZCharMetrics* fontMetrics = (FZCharMetrics*)memalign(16, 256 * sizeof(FZCharMetrics));
	memset(fontMetrics, 0, 256 * sizeof(FZCharMetrics));

	// These are the position at which to draw the next glyph
	int x = margin, y = margin + max_ascent;

	// Drawing loop
	for (int i = 0; i < 256; ++i) {
		int char_index = FT_Get_Char_Index(face, i);
		if (i < 32)
			char_index = 0;
		
		// Render the glyph
		FT_Load_Glyph(face, char_index, FT_LOAD_DEFAULT);
		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		
		// See whether the character fits on the current line
		int advance = (face->glyph->metrics.horiAdvance >> 6) + margin;
		if (advance > image_width - x){
			x = margin;
			y += (max_ascent + max_descent + margin);
		}
		
		// Fill in the GlyphEntry
		fontMetrics[i].width = advance - 3;
		fontMetrics[i].xadvance = advance - 3;
		fontMetrics[i].xoffset = 0;
		fontMetrics[i].yoffset = 0;
		fontMetrics[i].height = max_ascent + max_descent;
		fontMetrics[i].x = x;
		fontMetrics[i].y = y - max_ascent;
		
		// Copy the image gotten from FreeType onto the texture
		// at the correct position
		for (int row = 0; row < face->glyph->bitmap.rows; ++row){
			for (int pixel = 0; pixel < face->glyph->bitmap.width; ++pixel){
				fontTexture[(x + face->glyph->bitmap_left + pixel) +
					(y - face->glyph->bitmap_top + row) * image_width] =
					face->glyph->bitmap.buffer[pixel + row * face->glyph->bitmap.pitch];
			}
		}
		x += advance;    
	}

	FT_Done_FreeType(library);

	FZFont* font = new FZFont();
	if (!initFromImage(font, image, false)) {
		font->release();
		font = 0;
	}
	image->release();
	font->metrics = fontMetrics;
	font->lineHeight = fontLineHeight;

	return font;
}

int FZFont::getLineHeight() {
	return lineHeight;
}

FZCharMetrics* FZFont::getMetrics() {
	return metrics;
}

