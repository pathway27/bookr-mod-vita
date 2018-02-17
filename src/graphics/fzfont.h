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

#ifdef __vita__
  #include <vita2d.h>
#endif
#include <ft2build.h>
#include <string>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "fztexture.h"
using namespace std;

struct FZCharMetrics {
  short x, y, width, height, xoffset, yoffset, xadvance;
};

/**
 * Represents a font.
 */
class FZFont : public FZTexture {
  FZCharMetrics* metrics;
  int lineHeight;
  int maxAscent;
  bool isUTF;
  string fileName;
  int fontSize;
  bool autohint;
  unsigned char * buffer;
  int bufferSize;
  FT_Library ftlib;
  FT_Face ftface;

  static FZFont* createProto(FT_Library& library, FT_Face& face, int fontSize, bool autohint);

protected:
  FZFont();


public:
  #ifdef __vita__
    vita2d_font* v_font;
  #endif

  ~FZFont();

  /**
   * Get line height
   */
  int getLineHeight();

  /**
   * Get char metrics. Array is always for 256 chars, for the time being...
   */
  FZCharMetrics* getMetrics();

  int getSingleMetrics(unsigned long idx, FZCharMetrics* metrics, FZTexture** tpp);
  int initUTFFont();
  void doneUTFFont();
  static int get_next_utf8_char(unsigned long*, const char*, int);
  FZTexture* getTextureFromString(const char* t,int char_gap);
  
  /**
   * Create a new font texture with Freetype.
   */
  static FZFont* createFromFile(char* fileName, int fontSize);
  static FZFont* createFromFile(char* fileName, int fontSize, bool autohint);
  static FZFont* createFromMemory(unsigned char* buffer, int bufferSize);
  static FZFont* createFromMemory(unsigned char* buffer, int bufferSize, int fontSize, bool autohint);

  static FZFont* createUTFFromFile(char* fileName, int fontSize, bool autohint);
  static FZFont* createUTFFromMemory(unsigned char* buffer, int bufferSize, int fontSize, bool autohint);
  
};

#define IMAGE_MAX_WIDTH 512

#endif

