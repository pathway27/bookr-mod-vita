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

#ifndef FZFONT_H
#define FZFONT_H

#ifdef __vita__
  #include <vita2d.h>
#endif
#include <freetype2/ft2build.h>
#include <string>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "texture.hpp"

using std::string;

namespace bookr {

struct CharMetrics {
  short x, y, width, height, xoffset, yoffset, xadvance;
};

/**
 * Represents a font.
 */
class Font : public RefCounted {
  CharMetrics* metrics;
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

  static Font* createProto(FT_Library& library, FT_Face& face, int fontSize, bool autohint);

protected:
  Font();


public:
  #ifdef __vita__
    vita2d_font* v_font;
  #endif

  ~Font();

  /**
   * Get line height
   */
  int getLineHeight();

  /**
   * Get char metrics. Array is always for 256 chars, for the time being...
   */
  CharMetrics* getMetrics();

  int getSingleMetrics(unsigned long idx, CharMetrics* metrics, Texture** tpp);
  int initUTFFont();
  void doneUTFFont();
  static int get_next_utf8_char(unsigned long*, const char*, int);
  Texture* getTextureFromString(const char* t,int char_gap);
  int fontTextWidth(const char *text);
  
  /**
   * Create a new font texture with Freetype.
   */
  static Font* createFromFile(char* fileName, int fontSize);
  static Font* createFromFile(char* fileName, int fontSize, bool autohint);
  static Font* createFromMemory(unsigned char* buffer, int bufferSize);
  static Font* createFromMemory(unsigned char* buffer, int bufferSize, int fontSize, bool autohint);

  static Font* createUTFFromFile(char* fileName, int fontSize, bool autohint);
  static Font* createUTFFromMemory(unsigned char* buffer, int bufferSize, int fontSize, bool autohint);
  
};

#define IMAGE_MAX_WIDTH 512

}

#endif
