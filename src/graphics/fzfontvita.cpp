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
#include <malloc.h>

#include "fzfont.h"
#include "bkuser.h"

#include <vita2d.h>


FZFont::FZFont() : metrics(0),isUTF(false),ftlib(0),ftface(0) {
}

FZFont::~FZFont() {

}

FZFont* FZFont::createFromFile(char* fileName, int fontSize, bool autohint) {
    FZFont* font = new FZFont();
	  font->font = vita2d_load_font_file(fileName);
	  return font;
}

FZFont* FZFont::createFromMemory(unsigned char* buffer, int bufferSize, int fontSize, bool autohint) {
    FZFont* font = new FZFont();
	  font->font = vita2d_load_font_mem(buffer, fontSize);
	  return font;
}