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

#include <cstdlib>
#include <cstring>
#include <cstdio>
#if defined(MAC) || defined(WIN32)
static void* memalign(int t, int s) {
	return malloc(s);
}
#else
#include <cmalloc>
#endif

#include "font.hpp"
#include "user.hpp"

namespace bookr {

Font::Font() : metrics(0), isUTF(false), ftlib(0), ftface(0) {
}

Font::~Font() {
	if (metrics)
		free(metrics);
	metrics = 0;
	doneUTFFont();
}

// based on http://gpwiki.org/index.php/OpenGL_Font_System

Font* Font::createFromFile(char* fileName, int fontSize, bool autohint) {
	// This initializes FreeType
	FT_Library library;
	if (FT_Init_FreeType(&library) != 0) {
	  //		printf("cannot init freetype\n");
		return 0;
	}

	// Load the font
	FT_Face face;
	if (FT_New_Face(library, fileName, 0, &face) != 0) {
	  //	printf("cannot load font\n");
	  FT_Done_FreeType(library);
		return 0;
	}

	return createProto(library, face, fontSize, autohint);
}

Font* Font::createFromMemory(unsigned char* buffer, int bufferSize, int fontSize, bool autohint) {
	// This initializes FreeType
	FT_Library library;
	if (FT_Init_FreeType(&library) != 0) {
	  //	printf("cannot init freetype\n");
		return 0;
	}

	// Load the font
	FT_Face face;
	if (FT_New_Memory_Face(library, buffer, bufferSize, 0, &face) != 0) {
	  //	printf("cannot load font\n");
	  FT_Done_FreeType(library);
		return 0;
	}

	return createProto(library, face, fontSize, autohint);
}

Font* Font::createUTFFromFile(char* fileName, int fontSize, bool autohint) {
	// This initializes FreeType
	FT_Library library;
	if (FT_Init_FreeType(&library) != 0) {
	  //	printf("cannot init freetype\n");
		return 0;
	}

	// Load the font
	FT_Face face;
	if (FT_New_Face(library, fileName, 0, &face) != 0) {
	  //printf("cannot load font\n");
	  FT_Done_FreeType(library);
		return 0;
	}
	Font* ret = createProto(library, face, fontSize, autohint);
	ret->isUTF = true;
	ret->fileName = fileName;
	ret->bufferSize = 0;
	ret->fontSize = fontSize;
	ret->autohint = autohint;
	return ret;
}

Font* Font::createUTFFromMemory(unsigned char* buffer, int bufferSize, int fontSize, bool autohint) {
	// This initializes FreeType
	FT_Library library;
	if (FT_Init_FreeType(&library) != 0) {
	  //printf("cannot init freetype\n");
		return 0;
	}

	// Load the font
	FT_Face face;
	if (FT_New_Memory_Face(library, buffer, bufferSize, 0, &face) != 0) {
	  //printf("cannot load font\n");
	  FT_Done_FreeType(library);
		return 0;
	}

	Font* ret = createProto(library, face, fontSize, autohint);
	ret->buffer = buffer;
	ret->bufferSize = bufferSize;
	ret->fontSize = fontSize;
	ret->autohint = autohint;
	ret->isUTF = true;
	return ret;
}

Font* Font::createProto(FT_Library& library, FT_Face& face, int fontSize, bool autohint) {
	// Margins around characters to prevent them from 'bleeding' into
	// each other.
	int margin = 3;
	int image_height = 0, image_width = 256;

	// Abort if this is not a 'true type', scalable font.
	if (!(face->face_flags & FT_FACE_FLAG_SCALABLE) or !(face->face_flags & FT_FACE_FLAG_HORIZONTAL)) {
	  //printf("lame font\n");
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
	Image* image = Image::createEmpty(image_width, image_height, 256, Image::mono8);
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
	struct CharMetrics* fontMetrics = (CharMetrics*)memalign(16, 256 * sizeof(CharMetrics));
	memset(fontMetrics, 0, 256 * sizeof(CharMetrics));

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
				fontTexture[(fontMetrics[i].x + face->glyph->bitmap_left + pixel) +
					(fontMetrics[i].y + max_ascent - face->glyph->bitmap_top + row) * image_width] =
					face->glyph->bitmap.buffer[pixel + row * face->glyph->bitmap.pitch];
			}
		}
		x += advance;    
	}


	FT_Done_FreeType(library);
	Font* font = new Font();
	if (!initFromImage(font, image, false)) {
		font->release();
		font = 0;
	}
	else{
	  font->metrics = fontMetrics;
	  font->lineHeight = fontLineHeight;
	  font->maxAscent = max_ascent;
	}
	image->release();

	return font;
}

int Font::getLineHeight() {
	return lineHeight;
}

CharMetrics* Font::getMetrics() {
	return metrics;
}

int Font::initUTFFont(){
  if (ftlib == 0 && FT_Init_FreeType(&ftlib) != 0) {
    return 0;
  }
  
  if(this->bufferSize>0){
    if (ftface == 0 && FT_New_Memory_Face(ftlib, this->buffer, this->bufferSize, 0, &ftface) != 0) {
      return 0;
    }
  }
  else{
    if (ftface == 0 && FT_New_Face(ftlib, this->fileName.c_str(), 0, &ftface) != 0) {
      return 0;
    }
    
  }
  return 1;
}

void Font::doneUTFFont(){
  if(ftface){
    FT_Done_Face(ftface);
    ftface = 0;
  }
  if(ftlib){
    FT_Done_FreeType(ftlib);
    ftlib = 0;
  }
}

int Font::getSingleMetrics(unsigned long idx, CharMetrics* met, Texture** texturepp){
  int offset = 0;
  
  if(idx < 256){
    met->x = metrics[idx].x;
    met->y = metrics[idx].y;
    met->width = metrics[idx].width;
    met->height = metrics[idx].height;
    met->xoffset = metrics[idx].xoffset;
    met->yoffset = metrics[idx].yoffset;
    met->xadvance = metrics[idx].xadvance;
    this->retain();
    *texturepp = this;
    return 1;
  }

  if(!initUTFFont())
    return 0;

  
    int margin = 3;  // XXX may change this later XXX
    int image_height = 32, image_width = 32;
    
    // Abort if this is not a 'true type', scalable font.
    if (!(ftface->face_flags & FT_FACE_FLAG_SCALABLE) or !(ftface->face_flags & FT_FACE_FLAG_HORIZONTAL)) {
      //    printf("lame font\n");
      return 0;
    }
    
    // Set the font size
    FT_Set_Pixel_Sizes(ftface, fontSize, 0);
    
    
    //int max_descent = 0, max_ascent = 0;
    
    int loadMode = FT_LOAD_DEFAULT;
    if (autohint)
      loadMode |= FT_LOAD_FORCE_AUTOHINT;
    
    int char_index = FT_Get_Char_Index(ftface, idx);
    if (idx < 32)
      char_index = 0;
    
    // Render the glyph
    FT_Load_Glyph(ftface, char_index, loadMode);
    FT_Render_Glyph(ftface->glyph, FT_RENDER_MODE_NORMAL);

    //advance = (ftface->glyph->metrics.horiAdvance >> 6) + margin;
    //max_ascent = ftface->glyph->bitmap_top;
    //max_descent = ftface->glyph->bitmap.rows - ftface->glyph->bitmap_top;
    
    int needed_image_height = this->lineHeight + margin;
    while (image_height < needed_image_height)
      image_height *= 2;
    
    int needed_image_width = (ftface->glyph->metrics.horiAdvance >> 6) +2 * margin;
    while (image_width < needed_image_width)
      image_width *= 2;
    
    // Allocate memory for the texture
    int clutSize = 256;
    Image* image = Image::createEmpty(image_width, image_height, clutSize, Image::mono8);
    unsigned char* fontTexture = (unsigned char*)image->getData();
    // add clut to texture
    unsigned int* clut = image->getCLUT();
    for (int i = 0; i < clutSize; i++) {
      clut[i] = 0xffffff | (i << 24);
    }
    //for (int i = 0; i < image_height * image_width; ++i)
    //	fontTexture[i] = 0;
    //int fontLineHeight = max_ascent + max_descent + margin;
    
    // These are the position at which to draw the next glyph
    //int x = margin, y = margin + max_ascent;
    
    // Fill in the GlyphEntry
    met->width = (ftface->glyph->metrics.horiAdvance >> 6);
    met->xadvance = met->width;
    met->xoffset = 0;
    met->yoffset = 0;
    met->height = this->lineHeight - margin;
    met->x = margin;
    met->y = margin;
    
    // Copy the image gotten from FreeType onto the texture
    // at the correct position

    for (int row = 0; row < ftface->glyph->bitmap.rows; ++row){
      for (int pixel = 0; pixel < ftface->glyph->bitmap.width; ++pixel){
	fontTexture[(met->x + ftface->glyph->bitmap_left + pixel) +
		    (met->y + this->maxAscent - ftface->glyph->bitmap_top + row) * image_width] =
	  ftface->glyph->bitmap.buffer[pixel + row * ftface->glyph->bitmap.pitch];
      }
    }
    
    *texturepp = new Font();

    if( *texturepp == 0 || !initFromImage(*texturepp,image,false)){
      if (*texturepp){
	((Font*)(*texturepp)) -> release();
	*texturepp = 0;
      }
      image->release();
      return 0;
    }
    else {
       ((Font*)(*texturepp))->metrics = 0;
       ((Font*)(*texturepp))->metrics = (CharMetrics*)memalign(16,sizeof(CharMetrics));
       if (((Font*)(*texturepp))->metrics){
 	memset(((Font*)(*texturepp))->metrics, 0, sizeof(CharMetrics));
 	((Font*)(*texturepp))->metrics->width = met->width;
 	((Font*)(*texturepp))->metrics->height = met->height;
 	((Font*)(*texturepp))->metrics->xoffset = met->xoffset;
 	((Font*)(*texturepp))->metrics->yoffset = met->yoffset;
 	((Font*)(*texturepp))->metrics->x = met->x;
 	((Font*)(*texturepp))->metrics->y = met->y;
 	((Font*)(*texturepp))->metrics->xadvance = met->xadvance;
       }
       ((Font*)(*texturepp))->lineHeight = this->lineHeight;
       ((Font*)(*texturepp))->maxAscent = this->maxAscent;
      (*texturepp)->texEnv(_TEX_MODULATE);
      (*texturepp)->filter(_NEAREST, _NEAREST);
      image->release();
      return 1;
    }
}

int Font::get_next_utf8_char(unsigned long* ucs, const char* s, int n) {
	const char *src = s;
	char c;
	int extra;
	unsigned long result;

	if (n == 0)
		return 0;
    
	c = *src++;
	n--;
    
	if (!(c & 0x80)) {
		result = c;
		extra = 0;
	} 
	else if (!(c & 0x40)) {
		return -1;
	}
	else if (!(c & 0x20)) {
		result = c & 0x1f;
		extra = 1;
	}
	else if (!(c & 0x10)) {
		result = c & 0xf;
		extra = 2;
	}
	else if (!(c & 0x08)) {
		result = c & 0x07;
		extra = 3;
	}
	else if (!(c & 0x04)) {
		result = c & 0x03;
		extra = 4;
	}
	else if ( ! (c & 0x02)) {
		result = c & 0x01;
		extra = 5;
	}
	else {
		return -1;
	}
	if (extra > n)
		return -1;
    
	while (extra--) {
		result <<= 6;
		c = *src++;
	
		if ((c & 0xc0) != 0x80)
			return -1;
		result |= c & 0x3f;
	}
	*ucs = result;
	return src - s;
}

Texture* Font::getTextureFromString(const char* t,int cgap){
  int margin=3;
  //int cgap = 1; /* gap between adjust chars */
  int image_height = 16;
  int image_width = 16;
  int needed_width = margin; 
  int needed_height;
  int max_descent = 0, max_ascent = 0;
  
  if(!initUTFFont())
    return 0;
  
  if (!(ftface->face_flags & FT_FACE_FLAG_SCALABLE) or !(ftface->face_flags & FT_FACE_FLAG_HORIZONTAL)) {
    return 0;
  }

  FT_Set_Pixel_Sizes(ftface, fontSize, 0);
  int loadMode = FT_LOAD_DEFAULT;
  if (autohint)
    loadMode |= FT_LOAD_FORCE_AUTOHINT;
  
  int length = strlen(t);
  int nextUTFIndex = 0;
  int nextUTFLength = 0;
  unsigned long utf = 0;

  // run once to get image width and height.
  while ((nextUTFLength = get_next_utf8_char(&utf, t+nextUTFIndex, length - nextUTFIndex)) > 0){
    nextUTFIndex += nextUTFLength;
    if (utf < 32){
      utf = 32;
    }
    unsigned int char_index = FT_Get_Char_Index(ftface,utf);
    FT_Load_Glyph(ftface,char_index, loadMode);
    FT_Render_Glyph(ftface->glyph,FT_RENDER_MODE_NORMAL);
    
    needed_width += (ftface->glyph->metrics.horiAdvance >>6)+cgap;

    int t = ftface->glyph->bitmap_top;
    if (t > max_ascent)
      max_ascent = t;
    t = ftface->glyph->bitmap.rows - ftface->glyph->bitmap_top;
    if (t > max_descent)
      max_descent = t;
  }

  needed_width = needed_width - cgap + margin;
  needed_height = max_descent + max_ascent + 2* margin;

  while (image_width < needed_width)
    image_width *= 2;
  while (image_height < needed_height)
    image_height *= 2;

  int needed_lines = (image_width-1)/ IMAGE_MAX_WIDTH + 1;
  int single_line_height = image_height;
  if(needed_lines > 1){
    // we have to break this long text into multiple lines manually.
    image_width = IMAGE_MAX_WIDTH;
    image_height *= needed_lines;
  }

  int clutSize = 256;
  Image* image = Image::createEmpty(image_width, image_height, clutSize, Image::mono8);
  if(!image)
    return 0;
  unsigned char* fontTexture = (unsigned char*)image->getData();
  // add clut to texture
  unsigned int* clut = image->getCLUT();
  for (int i = 0; i < clutSize; i++) {
    clut[i] = 0xffffff | (i << 24);
  }

  struct CharMetrics* fontMetrics = (CharMetrics*)memalign(16, 1 * sizeof(CharMetrics));
  if(!fontMetrics)
    return 0;
  memset(fontMetrics, 0, 1 * sizeof(CharMetrics));
  
  nextUTFIndex = 0;
  nextUTFLength = 0;
  utf = 0;
  int x = margin;
  // draw chars into image
  if (needed_lines == 1 ){
    while ((nextUTFLength = get_next_utf8_char(&utf, t+nextUTFIndex, length - nextUTFIndex)) > 0){
      nextUTFIndex += nextUTFLength;
      if (utf<32){
	utf = 32;
      }
      unsigned int char_index = FT_Get_Char_Index(ftface,utf);
      FT_Load_Glyph(ftface,char_index, loadMode);
      FT_Render_Glyph(ftface->glyph,FT_RENDER_MODE_NORMAL);
      
      for (int row = 0; row < ftface->glyph->bitmap.rows; ++row){
	for (int pixel = 0; pixel < ftface->glyph->bitmap.width; ++pixel){
	  fontTexture[(x + ftface->glyph->bitmap_left + pixel) +
		      (margin + max_ascent - ftface->glyph->bitmap_top + row) * image_width] |=
	    ftface->glyph->bitmap.buffer[pixel + row * ftface->glyph->bitmap.pitch];
	}
      }
      
      x += (ftface->glyph->metrics.horiAdvance >>6)+cgap;
    }
  }
  else{
    int virt_x, virt_y, cur_line_no, real_x, real_y;
    while ((nextUTFLength = get_next_utf8_char(&utf, t+nextUTFIndex, length - nextUTFIndex)) > 0){
      nextUTFIndex += nextUTFLength;
      
      unsigned int char_index = FT_Get_Char_Index(ftface,utf);
      FT_Load_Glyph(ftface,char_index, loadMode);
      FT_Render_Glyph(ftface->glyph,FT_RENDER_MODE_NORMAL);
      
      for (int row = 0; row < ftface->glyph->bitmap.rows; ++row){
	for (int pixel = 0; pixel < ftface->glyph->bitmap.width; ++pixel){

	  virt_x = x + ftface->glyph->bitmap_left + pixel;
	  virt_y = margin + max_ascent - ftface->glyph->bitmap_top + row;
	  cur_line_no = virt_x / IMAGE_MAX_WIDTH;
	  
	  real_x = virt_x % IMAGE_MAX_WIDTH;
	  real_y = virt_y + cur_line_no * single_line_height; 

	  fontTexture[real_x +
		      real_y * IMAGE_MAX_WIDTH] |=
	    ftface->glyph->bitmap.buffer[pixel + row * ftface->glyph->bitmap.pitch];
	}
      }
      
      x += (ftface->glyph->metrics.horiAdvance >>6)+cgap;
    }
    
  }

  fontMetrics[0].width = needed_width - 2* margin;
  fontMetrics[0].xadvance = fontMetrics[0].width;
  fontMetrics[0].xoffset = 0;
  fontMetrics[0].yoffset = this->maxAscent-max_ascent;
  fontMetrics[0].height = max_ascent + max_descent;
  fontMetrics[0].x = margin;
  fontMetrics[0].y = margin;

  Font* font = new Font();
  if (!initFromImage(font, image, false)) {
    font->release();
    font = 0;
  }
  else{
    font->metrics = fontMetrics;
    //font->lineHeight = max_ascent + max_descent + margin;
    font->lineHeight = single_line_height;
    font->maxAscent = max_ascent;
    font->texEnv(_TEX_MODULATE);
    font->filter(_NEAREST, _NEAREST);

  }
  image->release();
  
  return font;
	
}

}