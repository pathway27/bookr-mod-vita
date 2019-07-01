/*
 * Bookr % VITA: document reader for the Sony PS Vita
 * Copyright (C) 2017 Sreekara C. (pathway27 at gmail dot com)
 *
 * IS A MODIFICATION OF THE ORIGINAL
 *
 * Bookr and bookr-mod for PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com),
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com),
 
 * AND VARIOUS OTHER FORKS.
 * See Forks in the README for more info
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// TODO: Figure out how to remove this.
#if defined(__vita__) && defined(DEBUG)
  #include <psp2/kernel/clib.h>
  #define printf sceClibPrintf
#endif

#include "font.hpp"

namespace bookr {

Font::Font() : metrics(0), isUTF(false), ftlib(0), ftface(0) {
  printf("Font()\n");
}

#ifdef __vita__
Font::~Font() {
  #ifdef DEBUG
    printf("~Font()\n");
  #endif

  if (v_font != NULL)
    vita2d_free_font(v_font);
}

Font* Font::createFromFile(char* fileName, int fontSize) {
  Font* font = new Font();
  font->v_font = vita2d_load_font_file(fileName);
  return font;
}

Font* Font::createFromMemory(unsigned char* buffer, int bufferSize) {
  #ifdef DEBUG
    printf("Font::createFromMemory()\n");
  #endif
  Font* font = new Font();
  font->v_font = vita2d_load_font_mem(buffer, bufferSize);
  font->fontSize = 28;
  return font;
}

int Font::fontTextWidth(const char *text)
{
  #ifdef DEBUG_BUTTONS
    printf("Font::fontTextWidth() - %i\n", vita2d_font_text_width(v_font, fontSize, text));
  #endif
  return vita2d_font_text_width(v_font, fontSize, text);
}
#elif defined(SWITCH)
Font::~Font() {
}

Font* Font::createFromFile(char* fileName, int fontSize) {
  Font* font = new Font();
  return font;
}

int Font::fontTextWidth(const char *text)
{
  return 1;
}

Font* Font::createFromMemory(unsigned char* buffer, int bufferSize) {
  Font* font = new Font();
  return font;
}
#endif

}