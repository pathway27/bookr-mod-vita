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
 
 * AND VARIOUS OTHER FORKS. See CREDITS in README.md
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


#include "graphics/fzinstreammem.h"
#include "graphics/fzscreen.h"

#include "bklayer.h"

#include <vita2d.h>
#include <psp2/kernel/threadmgr.h>

#include <cmath>

FZFont* BKLayer::fontBig = 0;
FZFont* BKLayer::fontSmall = 0;
FZFont* BKLayer::fontUTF = 0;
FZTexture* BKLayer::texUI = 0;
FZTexture* BKLayer::texUI2 = 0;
FZTexture* BKLayer::texLogo = 0;
FZTexture* BKLayer::genLogo = 0;

extern "C" {
  extern unsigned int size_res_logo;
  extern unsigned char res_logo[];
  extern unsigned int size_res_uitex;
  extern unsigned char res_uitex[];
  extern unsigned int size_res_uitex2;
  extern unsigned char res_uitex2[];
  extern unsigned int size_res_uifont;
  extern unsigned char res_uifont[];
  extern unsigned char pdf_font_DroidSansFallback_ttf_buf[];
  extern unsigned int  pdf_font_DroidSansFallback_ttf_len;
	extern unsigned char _binary_image_png_start;
	extern unsigned char _binary_icon0_t_png_start;
};

void BKLayer::load() {
	#ifdef __vita__
		#ifdef DEBUG
			psp2shell_print("bklayer load");
		#endif
		genLogo = FZTexture::createFromVitaTexture( vita2d_load_PNG_buffer(&_binary_image_png_start)   );
		texLogo = FZTexture::createFromVitaTexture( vita2d_load_PNG_buffer(&_binary_icon0_t_png_start) );
	#elif defined(PSP)
		// if (!fontBig){
		// fontBig = FZFont::createFromMemory(res_uifont, size_res_uifont, 14, false);
		// fontBig->texEnv(FZ_TEX_MODULATE);
		// fontBig->filter(FZ_NEAREST, FZ_NEAREST);
		// }
		// if (!fontUTF){
		//   fontUTF = FZFont::createUTFFromFile("utf.font",14,false);
		//   if(!fontUTF)
		//     if(pdf_font_DroidSansFallback_ttf_len)
		// fontUTF = FZFont::createUTFFromMemory(pdf_font_DroidSansFallback_ttf_buf, pdf_font_DroidSansFallback_ttf_len, 14, false);
		//     else
		// fontUTF = FZFont::createUTFFromFile("data.fnt",14,false);
		//   if(fontUTF){
		//     fontUTF->texEnv(FZ_TEX_MODULATE);
		//     fontUTF->filter(FZ_NEAREST, FZ_NEAREST);
		//   }
		// }
		// if (!fontSmall){
		// fontSmall = FZFont::createFromMemory(res_uifont, size_res_uifont, 11, false);
		// fontSmall->texEnv(FZ_TEX_MODULATE);
		// fontSmall->filter(FZ_NEAREST, FZ_NEAREST);
		// }
		// if (!texUI){
		// FZInputStreamMem* ins = FZInputStreamMem::create((char*)res_uitex, size_res_uitex);
		// FZImage* image = FZImage::createFromPNG(ins);
		// ins->release();
		// ins = 0;
		// texUI = FZTexture::createFromImage(image, false);
		// texUI->texEnv(FZ_TEX_MODULATE);
		// texUI->filter(FZ_NEAREST, FZ_NEAREST);
		// image->release();
		// }
		// if (!texUI2){
		// FZInputStreamMem* ins = FZInputStreamMem::create((char*)res_uitex2, size_res_uitex2);
		// FZImage* image = FZImage::createFromPNG(ins);
		// ins->release();
		// ins = 0;
		// texUI2 = FZTexture::createFromImage(image, false);
		// texUI2->texEnv(FZ_TEX_MODULATE);
		// texUI2->filter(FZ_NEAREST, FZ_NEAREST);
		// image->release();
		// }
	#endif
	
}

void BKLayer::unload(){
}

struct T32FV32F2D {
	float u,v;
	float x,y,z;
};

void BKLayer::drawImage(int x, int y, int w, int h, int tx, int ty) {
	struct T32FV32F2D vertices[2] = {

		{ tx, ty, x, y, 0 },
		{ tx + w, ty + h, x + w, y + h, 0 }
	};
	T32FV32F2D* verts = (T32FV32F2D*)FZScreen::getListMemory(2*sizeof(struct T32FV32F2D));
	memcpy(verts, vertices, 2 * sizeof(struct T32FV32F2D));
	FZScreen::drawArray(FZ_SPRITES,FZ_TEXTURE_32BITF|FZ_VERTEX_32BITF|FZ_TRANSFORM_2D,2,0,verts);
}

void BKLayer::drawImageScale(int x, int y, int w, int h, int tx, int ty, int tw, int th) {
	struct T32FV32F2D vertices[2] = {
		{ tx, ty, x, y, 0 },
		{ tx + tw, ty + th, x + w, y + h, 0 }
	};
	T32FV32F2D* verts = (T32FV32F2D*)FZScreen::getListMemory(2*sizeof(struct T32FV32F2D));
	memcpy(verts, vertices, 2 * sizeof(struct T32FV32F2D));
	FZScreen::drawArray(FZ_SPRITES,FZ_TEXTURE_32BITF|FZ_VERTEX_32BITF|FZ_TRANSFORM_2D,2,0,verts);
}

void BKLayer::drawPill(int x, int y, int w, int h, int r, int tx, int ty) {
	struct T32FV32F2D vertices[7*2] = {
		{ tx, ty, x, y, 0 },
		{ tx + r, ty + r, x + r, y + r, 0 },
	
		{ tx + r, ty, x + w, y, 0 },
		{ tx, ty + r, x + w + r, y + r, 0 },
	
		{ tx, ty + r, x, y + h, 0 },
		{ tx + r, ty, x + r, y + h + r, 0 },
	
		{ tx + r, ty + r, x + w, y + h, 0 },
		{ tx, ty, x + w + r, y + h + r, 0 },
	
		{ tx + r, ty + r, x + r, y, 0 },
		{ tx + r, ty + r, x + w, y + h + r, 0 },
	
		{ tx + r, ty + r, x, y + r, 0 },
		{ tx + r, ty + r, x + r, y + h, 0 },
	
		{ tx + r, ty + r, x + w, y + r, 0 },
		{ tx + r, ty + r, x + r + w, y + h, 0 }
	};
	T32FV32F2D* verts = (T32FV32F2D*)FZScreen::getListMemory(2 * 7 * sizeof(struct T32FV32F2D));
	memcpy(verts, vertices, 2 * 7 * sizeof(struct T32FV32F2D));
	FZScreen::drawArray(FZ_SPRITES,FZ_TEXTURE_32BITF|FZ_VERTEX_32BITF|FZ_TRANSFORM_2D,14,0,verts);
}

void BKLayer::drawTPill(int x, int y, int w, int h, int r, int tx, int ty) {
	struct T32FV32F2D vertices[5*2] = {
		{ tx, ty, x, y, 0 },
		{ tx + r, ty + r, x + r, y + r, 0 },
	
		{ tx + r, ty, x + w, y, 0 },
		{ tx, ty + r, x + w + r, y + r, 0 },
	
		/*{ tx, ty + r, x, y + h, 0 },
		{ tx + r, ty, x + r, y + h + r, 0 },
	
		{ tx + r, ty + r, x + w, y + h, 0 },
		{ tx, ty, x + w + r, y + h + r, 0 },*/
	
		{ tx + r, ty + r, x + r, y, 0 },
		{ tx + r, ty + r, x + w, y + h + r, 0 },
	
		{ tx + r, ty + r, x, y + r, 0 },
		{ tx + r, ty + r, x + r, y + h, 0 },
	
		{ tx + r, ty + r, x + w, y + r, 0 },
		{ tx + r, ty + r, x + r + w, y + h, 0 }
	};
	T32FV32F2D* verts = (T32FV32F2D*)FZScreen::getListMemory(2 * 5 * sizeof(struct T32FV32F2D));
	memcpy(verts, vertices, 2 * 5 * sizeof(struct T32FV32F2D));
	FZScreen::drawArray(FZ_SPRITES,FZ_TEXTURE_32BITF|FZ_VERTEX_32BITF|FZ_TRANSFORM_2D,10,0,verts);
}

void BKLayer::drawRect(int x, int y, int w, int h, int r, int tx, int ty) {
	struct T32FV32F2D vertices[1*2] = {
		{ tx + r, ty + r, x, y, 0 },
		{ tx + r, ty + r, x + w, y + h, 0 }
	};
	T32FV32F2D* verts = (T32FV32F2D*)FZScreen::getListMemory(2 * 1 * sizeof(struct T32FV32F2D));
	memcpy(verts, vertices, 2 * 1 * sizeof(struct T32FV32F2D));
	FZScreen::drawArray(FZ_SPRITES,FZ_TEXTURE_32BITF|FZ_VERTEX_32BITF|FZ_TRANSFORM_2D,2,0,verts);
}

int BKLayer::textWidthRange(char* t, int n, FZFont* font) {
	FZCharMetrics* fontChars = font->getMetrics();
	int baseX = 0;
	for (unsigned char *p = (unsigned char*)t; *p != 0; p++) {
		int idx = *p;
		// printable & white space
		if (idx >= 32)
			baseX += fontChars[idx].xadvance;
	}
	return baseX;
}

int BKLayer::textW(char* t, FZFont* font) {
	FZCharMetrics* fontChars = font->getMetrics();
	// precalc vertex count
	int vc = 0;
	for (unsigned char *p = (unsigned char*)t; *p != 0; p++) {
		if (*p <= 32)
			continue;
		vc += 2;
	}
	// lame
	if (vc == 0)
		return 0;
	int baseX = 0;
	for (unsigned char *p = (unsigned char*)t; *p != 0; p++) {
		int idx = *p;
		// new line
		if (idx == 10) {
			return baseX;
		}
		// white space
		if (idx == 32) {
			baseX += fontChars[idx].xadvance;
			continue;
		}
		// printable
		if (idx > 32) {
			baseX += fontChars[idx].xadvance;
			continue;
		}
	}
	return baseX;
}
	
void BKLayer::drawTextHC(char* t, FZFont* font, int y) {
	int w = textW(t, font);
	drawText(t, font, (480 - w) / 2, y);
}


int BKLayer::drawUTFText(const char* t, FZFont* font, int x, int y, int skipUTFChars, int maxWidth) {
  if(maxWidth<=0 ||maxWidth>480){
    maxWidth = 480;
  }
  if (skipUTFChars < 0){
    skipUTFChars = 0;
  }
  int length = strlen(t);
  int nextUTFIndex = 0;
  int nextUTFLength = 0;
  unsigned long utf = 0;

  // skip first <skipUTFChars> UTF chars.
  int skipc = skipUTFChars;
  while (skipc > 0 && (nextUTFLength = FZFont::get_next_utf8_char(&utf, t+nextUTFIndex, length - nextUTFIndex)) > 0){
    skipc--;
    nextUTFIndex+=nextUTFLength;
  }

  // read at most 64 UTF chars
  int vc = 0;
  int textWidth = 0;
  int baseX = x;
  int baseY = y;
  FZTexture* utf_texture = 0;
  FZCharMetrics met;

  while((nextUTFLength = FZFont::get_next_utf8_char(&utf, t+nextUTFIndex, length - nextUTFIndex)) > 0){
    nextUTFIndex+=nextUTFLength;
    
    if(font->getSingleMetrics(utf,&met,&utf_texture)>0 && utf_texture){
      textWidth+=met.xadvance;
      if(textWidth < maxWidth){
	vc++;

	utf_texture->bindForDisplay();
	T32FV32F2D* vertices = (T32FV32F2D*)FZScreen::getListMemory(2 * sizeof(struct T32FV32F2D));
	vertices[0].u = met.x;
	vertices[0].v = met.y;
	vertices[0].x = baseX + met.xoffset;
	vertices[0].y = baseY + met.yoffset;
	vertices[0].z = 0;
	
	vertices[1].u = met.x + met.width;
	vertices[1].v = met.y + met.height;
	vertices[1].x = baseX + met.xoffset + met.width;
	vertices[1].y = baseY + met.yoffset + met.height;
	vertices[1].z = 0;

	// vertices[0].u = 0;
	// vertices[0].v = 0;
	// vertices[0].x = baseX + met.xoffset;
	// vertices[0].y = baseY + met.yoffset;
	// vertices[0].z = 0;
	
	// vertices[1].u = utf_texture->getWidth();
	// vertices[1].v = utf_texture->getHeight();
	// vertices[1].x = baseX + met.xoffset + utf_texture->getWidth();
	// vertices[1].y = baseY + met.yoffset + utf_texture->getHeight();
	// vertices[1].z = 0;
			
	baseX += met.xadvance;

	FZScreen::drawArray(FZ_SPRITES,FZ_TEXTURE_32BITF|FZ_VERTEX_32BITF|FZ_TRANSFORM_2D,2,0,vertices);
	utf_texture->release();
      }
      else{
	utf_texture->release();
	font->doneUTFFont();
	return 1;
      }
    }
  }
  font->doneUTFFont();
  return 0;
}

int BKLayer::drawUTFMenuItem(BKMenuItem* item, FZFont* font, int x, int y, int skipPixels, int maxWidth) {
  if(item->tex && font == item->currentTexFont){
    // texture is okay, draw it directly.
  }
  else{
    if (item->tex){
      item->tex->release();
      item->tex = 0;
    }
    item->currentTexFont = font;
    // create new texture for this menuitem.
    item->tex = item->currentTexFont->getTextureFromString(item->label.c_str(),0);
  }

  if(item->tex && font == item->currentTexFont){
    /*FZScreen::commitAll();*/
    item->tex->bindForDisplay();
    FZCharMetrics* met = ((FZFont*)(item->tex))->getMetrics();
    int w = met->width - skipPixels;
    int ret = 0;
    if(w > maxWidth ){
      w = maxWidth;
      ret = 1;
    }
    //drawImage(x, y + met->yoffset, w, met->height, met->x+skipPixels, met->y);
    int x_first = met->x+skipPixels;
    int x_last = met->x+skipPixels+w-1;
    int line_no_first = x_first / IMAGE_MAX_WIDTH;
    int line_no_last = x_last / IMAGE_MAX_WIDTH;
    int y_first = met->y + ((FZFont*)(item->tex))->getLineHeight() * line_no_first;
    if (line_no_first == line_no_last){
      drawImage(x, y + met->yoffset, w, met->height, x_first, y_first);
    }
    else{
      int w_first = (line_no_first+1) * IMAGE_MAX_WIDTH - x_first;
      drawImage(x, y + met->yoffset, w_first, met->height, x_first, y_first);
      drawImage(x+w_first, y + met->yoffset, w-w_first, met->height, 0, y_first+((FZFont*)(item->tex))->getLineHeight());
    }

    return ret;
  }
  else{
    drawText("<Invalid Item>",font,x,y);
    return 0;
  }
}

int BKLayer::drawText(char* t, FZFont* font, int x, int y, int n, bool useLF, bool usePS, float ps, bool use3D) {
	if (n < 0) {
		n = strlen(t);
	}
	FZCharMetrics* fontChars = font->getMetrics();
	// precalc vertex count
	int vc = 0;
	int i = 0;
	for (unsigned char *p = (unsigned char*)t; i < n; i++, p++) {
		if (*p <= 32)
			continue;
		vc += 2;
	}
	// lame
	if (vc == 0)
		return 0;
	// build varray
	T32FV32F2D* vertices = (T32FV32F2D*)FZScreen::getListMemory(vc * sizeof(struct T32FV32F2D));
	// fill varray
	int iv = 0;
	int baseX = x;
	int baseY = y;

	i = 0;
	float fx = 0.0f;
	for (unsigned char *p = (unsigned char*)t; i < n; i++, p++) {
		int idx = *p;
		// new line
		if (idx == 10 && useLF) {
			baseY += font->getLineHeight()*(BKUser::options.txtHeightPct/100.0);
			baseX = x;
			fx = 0.0f;
		}
		// white space
		if (idx == 32) {
			if (usePS)
				fx += ps;
			else
				baseX += fontChars[idx].xadvance;
			continue;
		}
		// printable
		if (idx > 32) {
			int topleft = iv;
			int botright = topleft + 1;
			int cx = usePS ? x + int(floor(fx)) : baseX;

			vertices[topleft].u = fontChars[idx].x;
			vertices[topleft].v = fontChars[idx].y;
			vertices[topleft].x = cx + fontChars[idx].xoffset;
			vertices[topleft].y = baseY + fontChars[idx].yoffset;
			vertices[topleft].z = 0;

			vertices[botright].u = fontChars[idx].x + fontChars[idx].width;
			vertices[botright].v = fontChars[idx].y + fontChars[idx].height;
			vertices[botright].x = cx + fontChars[idx].xoffset + fontChars[idx].width;
			vertices[botright].y = baseY + fontChars[idx].yoffset + fontChars[idx].height;
			vertices[botright].z = 0;

			baseX += fontChars[idx].xadvance;
			if (usePS)
				fx += float(fontChars[idx].xadvance);
			iv+=2;
			continue;
		}
	}
	if (use3D) {
		// no quads on psp so we need triangles
		T32FV32F2D* vertices3d = (T32FV32F2D*)FZScreen::getListMemory(vc * sizeof(struct T32FV32F2D) * 3);
		int i3d = 0;
		for (iv = 0; iv < vc; iv+=2, i3d+=6) {
			int topleft = iv;
			int botright = topleft + 1;

			// tri 1
			vertices3d[i3d] = vertices[topleft];

			vertices3d[i3d+1].u = vertices[botright].u;
			vertices3d[i3d+1].v = vertices[topleft].v;
			vertices3d[i3d+1].x = vertices[botright].x;
			vertices3d[i3d+1].y = vertices[topleft].y;
			vertices3d[i3d+1].z = 0;

			vertices3d[i3d+2].u = vertices[topleft].u;
			vertices3d[i3d+2].v = vertices[botright].v;
			vertices3d[i3d+2].x = vertices[topleft].x;
			vertices3d[i3d+2].y = vertices[botright].y;
			vertices3d[i3d+2].z = 0;

			// tri 2
			vertices3d[i3d+3] = vertices3d[i3d+1];
			vertices3d[i3d+4] = vertices[botright];
			vertices3d[i3d+5] = vertices3d[i3d+2];
		}
		FZScreen::drawArray(FZ_TRIANGLES,FZ_TEXTURE_32BITF|FZ_VERTEX_32BITF|FZ_TRANSFORM_3D,vc * 3,0,vertices3d);
	} else {
		FZScreen::drawArray(FZ_SPRITES,FZ_TEXTURE_32BITF|FZ_VERTEX_32BITF|FZ_TRANSFORM_2D,vc,0,vertices);
	}
	return baseX;
}

void BKLayer::drawDialogFrame(string& title, string& triangleLabel, string& circleLabel, int flags) {
	int scrY = 0;
	char *t =(char*)circleLabel.c_str(); 
	int tw = textW(t, fontBig);
	
	texUI->bindForDisplay();
	FZScreen::ambientColor(0xf0222222);
	// back
	drawTPill(20, 20 + scrY, 480 - 46, 272, 6, 31, 1);
	// title
	FZScreen::ambientColor(0xffaaaaaa);
	drawPill(25, 25 + scrY, 480 - 46 - 10, 20, 6, 31, 1);
	// context label
	FZScreen::ambientColor(0xff555555);
	drawTPill(25, 272 - 30 + scrY, 480 - 46 - 11, 30, 6, 31, 1);
	// icons
	FZScreen::ambientColor(0xff000000);
	// drawImage(430, 30 + scrY, BK_IMG_TRIANGLE_XSIZE, BK_IMG_TRIANGLE_YSIZE, BK_IMG_TRIANGLE_X, BK_IMG_TRIANGLE_Y); tri!
	// drawImage(430, 29 + scrY, BK_IMG_CIRCLE_XSIZE, BK_IMG_CIRCLE_YSIZE, BK_IMG_CIRCLE_X, BK_IMG_CIRCLE_Y); // close handle
	switch (BKUser::controls.select) {
	case FZ_REPS_CROSS:	
		drawImage(430, 29 + scrY, BK_IMG_CIRCLE_XSIZE, BK_IMG_CIRCLE_YSIZE, BK_IMG_CIRCLE_X, BK_IMG_CIRCLE_Y); 
		break;
	case FZ_REPS_CIRCLE:
	default:
		drawImage(430, 29 + scrY, BK_IMG_CROSS_XSIZE, BK_IMG_CROSS_YSIZE, BK_IMG_CROSS_X, BK_IMG_CROSS_Y);
		break;
	}
	
	FZScreen::ambientColor(0xffcccccc);
	// circle or other context icon
	if (flags & BK_MENU_ITEM_USE_LR_ICON) {
		drawImage(480 - tw - 65, 248 + scrY, BK_IMG_LRARROWS_XSIZE, BK_IMG_LRARROWS_YSIZE, BK_IMG_LRARROWS_X, BK_IMG_LRARROWS_Y);
	} else {
	  if(circleLabel.size() > 0){
		switch(BKUser::controls.select) {
		case FZ_REPS_CROSS:
			drawImage(480 - tw - 65, 248 + scrY, BK_IMG_CROSS_XSIZE, BK_IMG_CROSS_YSIZE, BK_IMG_CROSS_X, BK_IMG_CROSS_Y);
			break;
		case FZ_REPS_CIRCLE:
		default:
			drawImage(480 - tw - 65, 248 + scrY, BK_IMG_CIRCLE_XSIZE, BK_IMG_CIRCLE_YSIZE, BK_IMG_CIRCLE_X, BK_IMG_CIRCLE_Y);
			break;
		}
		// drawImage(480 - tw - 65, 248 + scrY, BK_IMG_CROSS_XSIZE, BK_IMG_CROSS_YSIZE, BK_IMG_CROSS_X, BK_IMG_CROSS_Y);
	  }
	}
	if (triangleLabel.size() > 0 || flags & BK_MENU_ITEM_OPTIONAL_TRIANGLE_LABEL) {
		//drawImage(37, 248 + scrY, 20, 20, 107, 5);
		// drawImage(37, 248 + scrY, BK_IMG_TRIANGLE_XSIZE, BK_IMG_TRIANGLE_YSIZE, BK_IMG_TRIANGLE_X, BK_IMG_TRIANGLE_Y);
		drawImage(37, 248 + scrY, BK_IMG_TRIANGLE_XSIZE, BK_IMG_TRIANGLE_YSIZE, BK_IMG_TRIANGLE_X, BK_IMG_TRIANGLE_Y);
	}

	fontBig->bindForDisplay();

	// title
	FZScreen::ambientColor(0xffffffff);
	drawText((char*)title.c_str(), fontBig, 31, 28 + scrY);
	// labels
	FZScreen::ambientColor(0xffcccccc);
	if (triangleLabel.size() > 0) {
		drawText((char*)triangleLabel.c_str(), fontBig, 37 + 25, 248 + scrY);
	}
	if (circleLabel.size() > 0)
	  drawText(t, fontBig, 480 - tw - 40, 248 + scrY);
}

void BKLayer::drawMenu(string& title, string& triangleLabel, vector<BKMenuItem>& items) {
  drawMenu(title, triangleLabel, items, false);
}
void BKLayer::drawMenu(string& title, string& triangleLabel, vector<BKMenuItem>& items, bool useUTFFont) {
	int maxItemNum = 8;
	int selPos = selItem - topItem;
	int scrY = 0;
	FZFont* itemFont;
	if(useUTFFont&&fontUTF){
	  itemFont = fontUTF;
	  maxItemNum = 9;
	}
	else{
	  itemFont = fontBig;
	  maxItemNum = 8;
	  useUTFFont = false;
	}

	if (selPos < 0) {
		topItem += selPos;
		selPos = 0;
	}

	if (selPos > maxItemNum - 1) {
		topItem += selPos - maxItemNum + 1;
		selPos = maxItemNum - 1;
	}

	bool scrollbar = items.size() > maxItemNum;

	string tl(triangleLabel);
	if (items[selItem].flags & BK_MENU_ITEM_OPTIONAL_TRIANGLE_LABEL) {
		tl = items[selItem].triangleLabel; 
	}
	drawDialogFrame(title, tl, items[selItem].circleLabel, items[selItem].flags);

	texUI->bindForDisplay();
	// folder icons
	int ITEMHEIGHT = 60;
	if(useUTFFont)
	  ITEMHEIGHT = 62;
	FZScreen::ambientColor(0xffffffff);
	for (int i = 0; i < maxItemNum; ++i) {
		if (i + topItem == selItem)
			continue;
		if ((ITEMHEIGHT + (i+1)*itemFont->getLineHeight()) > 250)
			break;
		if ((i + topItem) >= (int)(items.size()))
			break;
		if (items[i + topItem].flags & BK_MENU_ITEM_FOLDER) {
			//drawImage(40, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY, 20, 20, 84, 52);
			drawImage(40, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY, 20, 20, 58, 81);
		}
	}
	// selected item
	int wSelBox = scrollbar ? 480 - 46 - 10 - 24: 480 - 46 - 10;
	drawPill(25, ITEMHEIGHT - 3 + scrY + selPos*itemFont->getLineHeight(), wSelBox, 19, 6, 31, 1);
	if (items[selItem].flags & BK_MENU_ITEM_FOLDER) {
		FZScreen::ambientColor(0xff000000);
		//drawImage(40, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight(), 20, 20, 84, 52);
		drawImage(40, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight(), BK_IMG_FOLDER_XSIZE, BK_IMG_FOLDER_YSIZE, BK_IMG_FOLDER_X, BK_IMG_FOLDER_Y);
	}

	// scrollbar
	if (scrollbar) {
		float barh = 1.0f * maxItemNum / float(items.size());
		barh *= 173.0f;
		if (barh < 15.0f)
			barh = 15.0f;
		float trel = float(topItem) / float(items.size());
		trel *= 173.0f;
		FZScreen::ambientColor(0xff555555);
		drawPill(436, 57, 12, 173, 6, 31, 1);
		FZScreen::ambientColor(0xffaaaaaa);
		drawPill(436, 57 + int(trel), 12, int(barh), 6, 31, 1);
	}

	// color rects items
	for (int i = 0; i < maxItemNum; ++i) {
		if ((ITEMHEIGHT + (i+1)*itemFont->getLineHeight()) > 250)
			break;
		if ((i + topItem) >= (int)(items.size()))
			break;
		if (items[i + topItem].flags & BK_MENU_ITEM_COLOR_RECT) {
			int tw = textW((char*)items[i + topItem].label.c_str(), itemFont);
			FZScreen::ambientColor(items[i + topItem].bgcolor | 0xff000000);
			drawRect(40 + 25 + tw + 10, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY, 30, 15, 6, 31, 1);
			FZScreen::ambientColor(items[i + topItem].fgcolor | 0xff000000);
			drawRect(40 + 25 + tw + 15, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY + 4, 30, 15, 6, 31, 1);		
		}
	}

	itemFont->bindForDisplay();

	FZScreen::ambientColor(0xffffffff);
	// contents
	int yoff = 3;
	for (int i = 0; i < maxItemNum; ++i) {
		if (i + topItem == selItem)
			continue;
		if ((ITEMHEIGHT + (i+1)*itemFont->getLineHeight()) > 250)
			break;
		if ((i + topItem) >= (int)(items.size()))
			break;
		if(useUTFFont){
		  int tooLong = drawUTFMenuItem(&(items[i + topItem]), itemFont, 40 + 25, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY + yoff, 0, 350);
		  if(tooLong){
		    //drawUTFText("...", itemFont, 416, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY + yoff, 0, 480);
		    texUI->bindForDisplay();
		    drawImage(415,ITEMHEIGHT + i*itemFont->getLineHeight() + scrY + yoff,12,12,7,112);
		  }
		}
		else{
		  drawText((char*)items[i + topItem].label.c_str(), itemFont, 40 + 25, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY);
		}
	}
	FZScreen::ambientColor(0xff000000);
	if(useUTFFont){
	  int tooLong;
	  if(skipChars==0)
	    tooLong = drawUTFMenuItem(&(items[selItem]), itemFont, 40 + 25, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff, 0, 350);
	  else {
	    //drawUTFText("...", itemFont, 40+25, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff, 0, 480);
	    texUI->bindForDisplay();
	    drawImage(40+25,ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff,12,12,7,112);
	    tooLong = drawUTFMenuItem(&(items[selItem]), itemFont, 40 + 25 + 14, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff, skipChars*10+14, 336);
	  }
	  if(tooLong){
	    texUI->bindForDisplay();
	    drawImage(415, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff, 12,12,7,112);
	  }
	  else{
	    maxSkipChars = skipChars;
	  }
	  itemFont->doneUTFFont();
	}
	else
	  drawText((char*)items[selItem].label.c_str(), itemFont, 40 + 25, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight());
}

void BKLayer::drawOutlinePrefix(string prefix, int x, int y, int w, int h, int ws){
  for (int i = 0;i<prefix.length();i++){
    texUI->bindForDisplay();
    switch (prefix[i]){
    case '0':
      break;
    case '1':
      drawRect(x+i*w+(w-ws)/2, y+(h-ws)/2,ws,1,6,31,1);
      drawRect(x+i*w+(w-ws)/2, y+(h+ws)/2-1,ws,1,6,31,1);

      drawRect(x+i*w+(w-ws)/2,y+(h-ws)/2,1,ws,6,31,1);
      drawRect(x+i*w+(w+ws)/2-1,y+(h-ws)/2,1,ws,6,31,1);

      drawRect(x+i*w+(w-ws)/2+2, y+h/2,ws-4,1,6,31,1);
      drawRect(x+i*w+w/2,y+(h-ws)/2+2,1,ws-4,6,31,1);
      break;
    case '2':
      drawRect(x+i*w+(w-ws)/2, y+(h-ws)/2,ws,1,6,31,1);
      drawRect(x+i*w+(w-ws)/2, y+(h+ws)/2-1,ws,1,6,31,1);

      drawRect(x+i*w+(w-ws)/2,y+(h-ws)/2,1,ws,6,31,1);
      drawRect(x+i*w+(w+ws)/2-1,y+(h-ws)/2,1,ws,6,31,1);

      drawRect(x+i*w+(w-ws)/2+2, y+h/2,ws-4,1,6,31,1);
      //drawRect(x+i*w+w/2,y+(h-ws)/2+2,1,ws-4,6,31,1);
      break;
    case '3':
      drawRect(x+i*w+w/2,y,1,(h+1)/2,6,31,1);
      drawRect(x+i*w+w/2,y+h/2,(w)/2, 1, 6,31,1);
      break;
    case '4':
      drawRect(x+i*w+w/2,y,1,(h),6,31,1);
      drawRect(x+i*w+w/2,y+h/2,(w)/2, 1, 6,31,1);
      break;
    case '5':
      drawRect(x+i*w+w/2,y,1,(h),6,31,1);
      break;
    case '6':
      drawRect(x+i*w+(w-ws)/2, y+(h-ws)/2,ws,1,6,31,1);
      drawRect(x+i*w+(w-ws)/2, y+(h+ws)/2-1,ws,1,6,31,1);

      drawRect(x+i*w+(w-ws)/2,y+(h-ws)/2,1,ws,6,31,1);
      drawRect(x+i*w+(w+ws)/2-1,y+(h-ws)/2,1,ws,6,31,1);
      break;
    }
  }
}

void BKLayer::drawOutline(string& title, string& triangleLabel, vector<BKOutlineItem>& items, bool useUTFFont) {
	int maxItemNum = 8;
	int selPos = selItem - topItem;
	int scrY = 0;
	FZFont* itemFont;
	bool hasOutline = true;
	//FZScreen::ambientColor(0xFF0000FF);
	//drawOutlinePrefix("1",0,0,20,20);
	if(useUTFFont&&fontUTF){
	  itemFont = fontUTF;
	  maxItemNum = 9;
	}
	else{
	  itemFont = fontBig;
	  maxItemNum = 8;
	  useUTFFont = false;
	}

	// if (items.size()==0){
	//   string cl = "";
	//   string prefix = "";

	//   items.push_back(BKOutlineItem("<No Outlines>", cl, (void*)0, prefix , false));
	//   hasOutline = false;
	//   selItem = 0;
	//   topItem = 0;
	//   selPos = 0;
	// }

	if( items.size()== 1 && items[0].circleLabel == ""){
	  hasOutline = false;
	}

	if (selPos < 0) {
		topItem += selPos;
		selPos = 0;
	}

	if (selPos > maxItemNum - 1) {
		topItem += selPos - maxItemNum + 1;
		selPos = maxItemNum - 1;
	}

	bool scrollbar = items.size() > maxItemNum;

	string tl;
	if (items[selItem].flags & BK_OUTLINE_ITEM_HAS_TRIANGLE_LABEL) {
		tl = triangleLabel; 
	}
	drawDialogFrame(title, tl, items[selItem].circleLabel, items[selItem].flags);
	if (hasOutline){
	  texUI->bindForDisplay();
	  FZScreen::ambientColor(0xffcccccc);
	  drawImage(190, 248, BK_IMG_SQUARE_XSIZE, BK_IMG_SQUARE_YSIZE, BK_IMG_SQUARE_X, BK_IMG_SQUARE_Y);
	  fontBig->bindForDisplay();
	  if (BKUser::options.t_ignore_x)
	    drawText("Goto (ignore zoom&X)", fontBig, 190+BK_IMG_SQUARE_XSIZE+8, 248);
	  else
	    drawText("Goto (ignore zoom)", fontBig, 190+BK_IMG_SQUARE_XSIZE+8, 248);
	}
	texUI->bindForDisplay();
	int ITEMHEIGHT = 60;
	if(useUTFFont)
	  ITEMHEIGHT = 62;
	FZScreen::ambientColor(0xffffffff);
	
	// selected item
	int wSelBox = scrollbar ? 480 - 46 - 10 - 24: 480 - 46 - 10;
	drawPill(25, ITEMHEIGHT - 3 + scrY + selPos*itemFont->getLineHeight(), wSelBox, 19, 6, 31, 1);
	if (items[selItem].flags & BK_MENU_ITEM_FOLDER) {
		FZScreen::ambientColor(0xff000000);
		//drawImage(40, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight(), 20, 20, 84, 52);
		drawImage(40, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight(), BK_IMG_FOLDER_XSIZE, BK_IMG_FOLDER_YSIZE, BK_IMG_FOLDER_X, BK_IMG_FOLDER_Y);
	}

	// scrollbar
	if (scrollbar) {
		float barh = 1.0f * maxItemNum / float(items.size());
		barh *= 173.0f;
		if (barh < 15.0f)
			barh = 15.0f;
		float trel = float(topItem) / float(items.size());
		trel *= 173.0f;
		FZScreen::ambientColor(0xff555555);
		drawPill(436, 57, 12, 173, 6, 31, 1);
		FZScreen::ambientColor(0xffaaaaaa);
		drawPill(436, 57 + int(trel), 12, int(barh), 6, 31, 1);
	}

	// not support now
	// color rects items
	// for (int i = 0; i < maxItemNum; ++i) {
	// 	if ((ITEMHEIGHT + (i+1)*itemFont->getLineHeight()) > 250)
	// 		break;
	// 	if ((i + topItem) >= (int)(items.size()))
	// 		break;
	// 	if (items[i + topItem].flags & BK_MENU_ITEM_COLOR_RECT) {
	// 		int tw = textW((char*)items[i + topItem].label.c_str(), itemFont);
	// 		FZScreen::ambientColor(items[i + topItem].bgcolor | 0xff000000);
	// 		drawRect(40 + 25 + tw + 10, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY, 30, 15, 6, 31, 1);
	// 		FZScreen::ambientColor(items[i + topItem].fgcolor | 0xff000000);
	// 		drawRect(40 + 25 + tw + 15, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY + 4, 30, 15, 6, 31, 1);		
	// 	}
	// }

	itemFont->bindForDisplay();

	FZScreen::ambientColor(0xffffffff);
	// contents
	int yoff = 3;
	int x_left = 40;
	int text_right = 415;
	int mark_width = 11;
	int text_left;
	for (int i = 0; i < maxItemNum; ++i) {
		if (i + topItem == selItem)
			continue;
		if ((ITEMHEIGHT + (i+1)*itemFont->getLineHeight()) > 250)
			break;
		if ((i + topItem) >= (int)(items.size()))
			break;

		drawOutlinePrefix(items[i + topItem].prefix, x_left, ITEMHEIGHT + i * itemFont->getLineHeight() + scrY, mark_width, itemFont->getLineHeight(),9);

		text_left = x_left +  mark_width * items[i + topItem].prefix.length();
		if(useUTFFont){
		  int tooLong = drawUTFMenuItem(&(items[i + topItem]), itemFont, text_left, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY + yoff, 0, text_right - text_left);
		  if(tooLong){
		    //drawUTFText("...", itemFont, 416, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY + yoff, 0, 480);
		    texUI->bindForDisplay();
		    drawImage(text_right, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY + yoff,12,12,7,112);
		  }
		}
		else{
		  drawText((char*)items[i + topItem].label.c_str(), itemFont, text_left, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY);
		}
	}
	FZScreen::ambientColor(0xff000000);
	drawOutlinePrefix(items[selItem].prefix, x_left, ITEMHEIGHT + selPos * itemFont->getLineHeight() + scrY, mark_width, itemFont->getLineHeight(),9);
	text_left = x_left +  mark_width * items[selItem].prefix.length();

	if(useUTFFont){
	  int tooLong;
	  if(skipChars==0)
	    tooLong = drawUTFMenuItem(&(items[selItem]), itemFont, text_left, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff, 0, text_right-text_left);
	  else {
	    //drawUTFText("...", itemFont, 40+25, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff, 0, 480);
	    texUI->bindForDisplay();
	    drawImage(text_left,ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff,12,12,7,112);
	    tooLong = drawUTFMenuItem(&(items[selItem]), itemFont, text_left + 14, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff, skipChars*10+14, text_right-text_left-14);
	  }
	  if(tooLong){
	    texUI->bindForDisplay();
	    drawImage(text_right, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff, 12,12,7,112);
	  }
	  else{
	    maxSkipChars = skipChars;
	  }
	  itemFont->doneUTFFont();
	}
	else
	  drawText((char*)items[selItem].label.c_str(), itemFont, text_left, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight());

	// if(!hasOutline)
	//   items.clear();
}

static int countLines(string& t) {
	int lines = 1;
	char* c = (char*)t.c_str();
	while (*c != 0) {
		if (*c == 0xa) ++lines;
		++c;
	}
	return lines;
}

void BKLayer::drawPopup(string& text, string& title, int bg1, int bg2, int fg) {
	texUI->bindForDisplay();
	int l = countLines(text);
	int h = 25 + 25*l;
	int y = (272 - h) /2;
	// back
	FZScreen::ambientColor(bg1);
	drawPill(40, y, 480 - 86, h, 6, 31, 1);
	// title
	FZScreen::ambientColor(bg2);
	drawPill(45, 5 + y, 480 - 86 - 10, 20, 6, 31, 1);
	// icons
	FZScreen::ambientColor(bg1|0xff000000);
	// drawImage(410, 9 + y, BK_IMG_CIRCLE_XSIZE, BK_IMG_CIRCLE_YSIZE, BK_IMG_CIRCLE_X, BK_IMG_CIRCLE_Y);
	switch (BKUser::controls.select) {
	case FZ_REPS_CIRCLE:
		drawImage(410, 9 + y, BK_IMG_CROSS_XSIZE, BK_IMG_CROSS_YSIZE, BK_IMG_CROSS_X, BK_IMG_CROSS_Y);
		break;
	case FZ_REPS_CROSS:
	default:
		drawImage(410, 9 + y, BK_IMG_CIRCLE_XSIZE, BK_IMG_CIRCLE_YSIZE, BK_IMG_CIRCLE_X, BK_IMG_CIRCLE_Y);
		break;
	}

	fontBig->bindForDisplay();

	FZScreen::ambientColor(fg);
	drawText((char*)title.c_str(), fontBig, 51, y + 9);
	drawText((char*)text.c_str(), fontBig, 51, y + 35);
}

void BKLayer::drawClockAndBattery(string& extra) {
	texUI->bindForDisplay();
	FZScreen::ambientColor(0xffbbbbbb);
	drawImage(350, 226, BK_IMG_BATTERY_XSIZE, BK_IMG_BATTERY_YSIZE, BK_IMG_BATTERY_X, BK_IMG_BATTERY_Y);
	drawImage(405, 222, BK_IMG_CLOCK_XSIZE, BK_IMG_CLOCK_YSIZE, BK_IMG_CLOCK_X, BK_IMG_CLOCK_Y);
	drawImage(292, 224, BK_IMG_MEMORY_XSIZE, BK_IMG_MEMORY_YSIZE, BK_IMG_MEMORY_X, BK_IMG_MEMORY_Y);
	fontSmall->bindForDisplay();
	FZScreen::ambientColor(0xffbbbbbb);
	int ew = textW((char*)extra.c_str(), fontSmall);
	drawText((char*)extra.c_str(), fontSmall, 480 - 30 - ew, 205);
	int h = 0, m = 0;
	FZScreen::getTime(h, m);
	int b = FZScreen::getBattery();
	int mem = FZScreen::getUsedMemory();
	int speed = FZScreen::getSpeed();
	char t1[20];
	snprintf(t1, 20, "%02d:%02d", h, m);
	char t2[20];
	snprintf(t2, 20, "%d%%", b);
	char t3[20];
	snprintf(t3, 20, "%.1fM", ((float)(mem)) / (1024.0f*1024.0f));
	char t4[20];
	snprintf(t4, 20, "%dMHz", speed);
	drawText(t1, fontSmall, 425, 224);
	drawText(t2, fontSmall, 370, 224);
	drawText(t3, fontSmall, 310, 224);
	drawText(t4, fontSmall, 240, 224);
}

void BKLayer::menuCursorUpdate(unsigned int buttons, int max) {
	int* b = FZScreen::ctrlReps();
	if (b[BKUser::controls.menuUp] == 1 || (b[BKUser::controls.menuUp] > 10 && b[BKUser::controls.menuUp] % 5 == 0)) {
		selItem--;
		if (selItem < 0) {
			selItem = max - 1;
		}
		skipChars = 0;
		maxSkipChars = -1;
	}
	if (b[BKUser::controls.menuDown] == 1 || (b[BKUser::controls.menuDown] > 10 && b[BKUser::controls.menuDown] % 5 == 0)) {
		selItem++;
		if (selItem >= max) {
			selItem = 0;
		}
		skipChars = 0;
		maxSkipChars = -1;
	}
	if (b[BKUser::controls.menuLeft] == 1 || (b[BKUser::controls.menuLeft] > 10 && b[BKUser::controls.menuLeft] % 5 == 0)) {
		skipChars--;
		if (skipChars < 0) {
			skipChars = 0;
		}
	}
	if (b[BKUser::controls.menuRight] == 1 || (b[BKUser::controls.menuRight] > 10 && b[BKUser::controls.menuRight] % 5 == 0)) {
		skipChars++;
		if (maxSkipChars >= 0 && skipChars>maxSkipChars)
		  skipChars = maxSkipChars;
	}
}

BKLayer::BKLayer() : topItem(0), selItem(0),skipChars(0),maxSkipChars(-1) {
}

BKLayer::~BKLayer() {
}