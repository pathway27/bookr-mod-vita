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

#include "fzinstreamstd.h"
#include "fzscreen.h"

#include "bklayer.h"

FZFont* BKLayer::fontBig = 0;
FZFont* BKLayer::fontText = 0;
FZTexture* BKLayer::texUI = 0;
FZTexture* BKLayer::texLogo = 0;

void BKLayer::load() {
	char filename[1024];
	snprintf(filename, 1024, "%s/data/%s", FZScreen::basePath(), "vera.ttf");
	fontText = FZFont::createFromFile(filename, 11, false);
	fontText->texEnv(FZ_TEX_MODULATE);
	fontText->filter(FZ_NEAREST, FZ_NEAREST);
	snprintf(filename, 1024, "%s/data/%s", FZScreen::basePath(), "urwgothicb.pfb");
	fontBig = FZFont::createFromFile(filename, 14, false);
	fontBig->texEnv(FZ_TEX_MODULATE);
	fontBig->filter(FZ_NEAREST, FZ_NEAREST);
	snprintf(filename, 1024, "%s/data/%s", FZScreen::basePath(), "ui.png");
	FZInputStreamStd* ins = FZInputStreamStd::create(filename);
	FZImage* image = FZImage::createFromPNG(ins);
	ins->release();
	ins = 0;
	texUI = FZTexture::createFromImage(image, false);
	texUI->texEnv(FZ_TEX_MODULATE);
	texUI->filter(FZ_NEAREST, FZ_NEAREST);
	image->release();
	snprintf(filename, 1024, "%s/data/%s", FZScreen::basePath(), "logo.png");
	ins = FZInputStreamStd::create(filename);
	image = FZImage::createFromPNG(ins);
	ins->release();
	ins = 0;
	texLogo = FZTexture::createFromImage(image, false);
	texLogo->texEnv(FZ_TEX_REPLACE);
	texLogo->filter(FZ_NEAREST, FZ_NEAREST);
	image->release();
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

static int textW(char* t, FZFont* font) {
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

void BKLayer::drawText(char* t, FZFont* font, int x, int y) {
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
		return;
	// build varray
	T32FV32F2D* vertices = (T32FV32F2D*)FZScreen::getListMemory(vc * sizeof(struct T32FV32F2D));
	// fill varray
	int iv = 0;
	int baseX = x;
	int baseY = y;
	for (unsigned char *p = (unsigned char*)t; *p != 0; p++) {
		int idx = *p;
		// new line
		if (idx == 10) {
			baseY += font->getLineHeight();
			baseX = x;
		}
		// white space
		if (idx == 32) {
			baseX += fontChars[idx].xadvance;
			continue;
		}
		// printable
		if (idx > 32) {
			int topleft = iv;
			int botright = topleft + 1;

			vertices[topleft].u = fontChars[idx].x;
			vertices[topleft].v = fontChars[idx].y;
			vertices[topleft].x = baseX + fontChars[idx].xoffset;
			vertices[topleft].y = baseY + fontChars[idx].yoffset;
			vertices[topleft].z = 0;

			vertices[botright].u = fontChars[idx].x + fontChars[idx].width;
			vertices[botright].v = fontChars[idx].y + fontChars[idx].height;
			vertices[botright].x = baseX + fontChars[idx].xoffset + fontChars[idx].width;
			vertices[botright].y = baseY + fontChars[idx].yoffset + fontChars[idx].height;
			vertices[botright].z = 0;

			baseX += fontChars[idx].xadvance;
			iv+=2;
			continue;
		}
	}
	FZScreen::drawArray(FZ_SPRITES,FZ_TEXTURE_32BITF|FZ_VERTEX_32BITF|FZ_TRANSFORM_2D,vc,0,vertices);
}

void BKLayer::drawMenu(string& title, string& triangleLabel, vector<BKMenuItem>& items) {
	int selPos = selItem - topItem;
	int scrY = 0;

	if (selPos < 0) {
		topItem += selPos;
		selPos = 0;
	}

	if (selPos > 7) {
		topItem += selPos - 7;
		selPos = 7;
	}

	char *t =(char*)items[selItem].circleLabel.c_str(); 
	int tw = textW(t, fontBig);

	texUI->bindForDisplay();
	FZScreen::ambientColor(0xf0222222);
	// back
	drawTPill(20, 20 + scrY, 480 - 46, 272, 6, 31, 1);
	// title
	FZScreen::ambientColor(0xffaaaaaa);
	drawPill(25, 25 + scrY, 480 - 46 - 10, 20, 6, 31, 1);
	// help
	FZScreen::ambientColor(0xff555555);
	drawTPill(25, 272 - 30 + scrY, 480 - 46 - 11, 30, 6, 31, 1);
	// icons
	FZScreen::ambientColor(0xff000000);
	//drawImage(430, 30 + scrY, 20, 18, 9, 53); tri!
	drawImage(430, 29 + scrY, 20, 17, 31, 53);
	FZScreen::ambientColor(0xffcccccc);
	drawImage(480 - tw - 65, 248 + scrY, 20, 20, 31, 70);
	if (triangleLabel.size() > 0) {
		//drawImage(37, 248 + scrY, 20, 20, 107, 5);
		drawImage(37, 248 + scrY, 20, 18, 9, 53);
	}
	// folder icons
	FZScreen::ambientColor(0xffffffff);
	for (int i = 0; i < 8; ++i) {
		if (i + topItem == selItem)
			continue;
		if ((60 + (i+1)*fontBig->getLineHeight()) > 250)
			break;
		if ((i + topItem) >= (int)(items.size()))
			break;
		if (items[i + topItem].flags & BK_MENU_ITEM_FOLDER) {
			//drawImage(40, 60 + i*fontBig->getLineHeight() + scrY, 20, 20, 84, 52);
			drawImage(40, 60 + i*fontBig->getLineHeight() + scrY, 20, 20, 58, 81);
		}
	}
	drawPill(25, 57 + scrY + selPos*fontBig->getLineHeight(), 480 - 46 - 10, 19, 6, 31, 1);
	if (items[selItem].flags & BK_MENU_ITEM_FOLDER) {
		FZScreen::ambientColor(0xff000000);
		//drawImage(40, 60 + scrY + selPos*fontBig->getLineHeight(), 20, 20, 84, 52);
		drawImage(40, 60 + scrY + selPos*fontBig->getLineHeight(), 20, 20, 58, 81);
	}

	fontBig->bindForDisplay();

	// title
	FZScreen::ambientColor(0xffffffff);
	drawText((char*)title.c_str(), fontBig, 31, 28 + scrY);
	// contents
	for (int i = 0; i < 8; ++i) {
		if (i + topItem == selItem)
			continue;
		if ((60 + (i+1)*fontBig->getLineHeight()) > 250)
			break;
		if ((i + topItem) >= (int)(items.size()))
			break;
		drawText((char*)items[i + topItem].label.c_str(), fontBig, 40 + 25, 60 + i*fontBig->getLineHeight() + scrY);
	}
	FZScreen::ambientColor(0xff000000);
	drawText((char*)items[selItem].label.c_str(), fontBig, 40 + 25, 60 + scrY + selPos*fontBig->getLineHeight());
	// labels
	FZScreen::ambientColor(0xffcccccc);
	if (triangleLabel.size() > 0) {
		drawText((char*)triangleLabel.c_str(), fontBig, 37 + 25, 248 + scrY);
	}
	drawText(t, fontBig, 480 - tw - 40, 248 + scrY);
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
	FZScreen::ambientColor(fg);
	drawImage(410, 9 + y, 20, 17, 31, 53);

	fontBig->bindForDisplay();

	FZScreen::ambientColor(fg);
	drawText((char*)title.c_str(), fontBig, 51, y + 9);
	drawText((char*)text.c_str(), fontBig, 51, y + 35);
/*
	// contents
	for (int i = 0; i < 8; ++i) {
		if (i + topItem == selItem)
			continue;
		if ((60 + (i+1)*fontBig->getLineHeight()) > 250)
			break;
		if ((i + topItem) >= (int)(items.size()))
			break;
		drawText((char*)items[i + topItem].label.c_str(), fontBig, 40 + 25, 60 + i*fontBig->getLineHeight() + scrY);
	}
	FZScreen::ambientColor(0xff000000);
	drawText((char*)items[selItem].label.c_str(), fontBig, 40 + 25, 60 + scrY + selPos*fontBig->getLineHeight());
	// labels
	FZScreen::ambientColor(0xffcccccc);
	if (triangleLabel.size() > 0) {
		drawText((char*)triangleLabel.c_str(), fontBig, 37 + 25, 248 + scrY);
	}
	drawText(t, fontBig, 480 - tw - 40, 248 + scrY);
*/
}

void BKLayer::menuCursorUpdate(unsigned int buttons, int max) {
	int* b = FZScreen::ctrlReps();
	if (b[FZ_REPS_UP] == 1 || b[FZ_REPS_UP] > 20) {
		selItem--;
		if (selItem < 0) {
			selItem = 0;
		}
	}
	if (b[FZ_REPS_DOWN] == 1 || b[FZ_REPS_DOWN] > 20) {
		selItem++;
		if (selItem >= max) {
			selItem = max - 1;
		}
	}
}

BKLayer::BKLayer() : topItem(0), selItem(0) {
}

BKLayer::~BKLayer() {
}

