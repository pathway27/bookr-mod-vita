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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef MAC
static void* memalign(int t, int s) {
	return malloc(s);
}
#else
#include <malloc.h>
#endif

#include "bkbook.h"
#include "fzfont.h"

BKBook::BKBook() : 
	text(0), textLen(0), baseLine(0), page(1),
	screenLines(0), totalLines(0),
	bannerFrames(0) {
}

BKBook::~BKBook() {
	if (text != 0) {
		free(text);
	}
	for (int i = 0; i < 4096; i++) {
		if (blocks[i] != NULL) {
			free(blocks[i]);
		}
	}
}

BKBook* BKBook::create(string& file, int size) {
	BKBook* b = new BKBook();
	FILE* f = fopen(file.c_str(), "r");
	b->textLen = size + 1;
	b->text = (char*)memalign(16, size + 2);
	fread(b->text, size, 1, f);
	b->text[size] = 0xa;
	b->text[size+1] = 0;
	fclose(f);

	memset(b->blocks, 0, sizeof(struct TextLineMetricsBlock*)*4096);

	b->textLineMetrics(450);
	b->screenLines = 257 / fontText->getLineHeight();

	FZScreen::resetReps();

	return b;
}

void BKBook::textLineMetrics(int width) {
	FZFont* font = fontText;
	char* str = text;
	int n = textLen; 
	int baseX = 0;
	int blockI = 0;
	int line = 0;
	int lineB = 0;
	struct TextLineMetrics tlm = { NULL, 0, 0, 0 };
	struct TextLineMetrics tlmSaved = { NULL, 0, 0, 0 };
	FZCharMetrics* fontChars = font->getMetrics();
	int i;
	int spaces = 0;
	//int lastSpace = 0;
	int savedW = 0;
	bool newLine = true;
	//bool seenPrintable = false;
	totalLines = 0;
	for (i = 0; i < n; i++) {
		unsigned char c = str[i];
		//printf("%d, %d\n", i, n);

		// alloc new block if needed
		if (blocks[blockI] == NULL) {
			blocks[blockI] = (TextLineMetricsBlock*)memalign(16, sizeof(TextLineMetricsBlock));
			memset(blocks[blockI], 0, sizeof(struct TextLineMetricsBlock));
		}

		// init new line if needed
		if (newLine) {
			tlm.base = &str[i];
			tlm.size = 0;
			tlm.nVertices = 0;
			tlm.spaceWidth = 0;
			baseX = 0;
			newLine = false;
			spaces = 0;
			savedW = 0;
			++totalLines;
			//seenPrintable = false;
		}

		tlm.size++;

		// calculate current line inside the block
		lineB = line & 0xfff;

		// space
		if (c == 32) {
			savedW = baseX;
			baseX += fontChars[32].xadvance;
			tlmSaved = tlm;
			spaces++;
			continue;
		}

		// line break
		if (c == 0xa) {
			tlm.spaceWidth = fontChars[32].xadvance;
			blocks[blockI]->lines[lineB] = tlm;
			line++;
			blocks[blockI]->top++;
			// 4096 lines: new block needed
			if (blocks[blockI]->top == 4096)
				blockI++;
			newLine = true;
			continue;
		}

		// printable
		if (c > 32) {
			//seenPrintable = true;
			tlm.nVertices += 2;
			baseX += fontChars[c].xadvance;
			// find if we must force a line break
			if (baseX > width) {
				int ws = tlm.size - tlmSaved.size;
				if (ws > 50)
					ws = 50;
				i -= ws;
				float r = width - savedW;
				float adv = fontChars[32].xadvance;
				spaces--;
				if (spaces > 0) {
					adv += r / spaces;
				}
				tlmSaved.spaceWidth = adv;
				blocks[blockI]->lines[lineB] = tlmSaved;
				line++;
				blocks[blockI]->top++;
				// 4096 lines: new block needed
				if (blocks[blockI]->top == 4096)
					blockI++;
				newLine = true;
			}
			continue;
		}
		// the rest: ignored
	}
}

int BKBook::countVerticesForLines(int from, int to) {
	// precalc vertex count for the selected range
	int i;
	int vn = 0;
	for (i = from; i < to; i++) {
		int blockI = i >> 12;
		int lineB = i & 0xfff;

		if (blocks[blockI] == 0)
			return vn;

		if (blocks[blockI]->top < lineB)
			return vn;

		vn += blocks[blockI]->lines[lineB].nVertices;
	}
	return vn;
}

void BKBook::spritesForLines(int x, int y, int from, int to, BKVertex* vertices) {
	FZFont* font = fontText;
	int i, j, iv = 0;
	float baseX;
	int baseY = y;
	struct TextLineMetrics* tlm = NULL;
	FZCharMetrics* fontChars = font->getMetrics();
	// add line by line the sprites needed
	for (i = from; i < to; i++) {
		int blockI = i >> 12;
		int lineB = i & 0xfff;

		if (blocks[blockI] == 0)
			return;

		if (blocks[blockI]->top < lineB)
			return;

		tlm = &blocks[blockI]->lines[lineB];
		baseX = 0;

		for (j = 0; j < tlm->size; j++) {
			int idx = tlm->base[j] & 0xff;

			// space
			if (idx == 32) {
				baseX += tlm->spaceWidth;
				continue;
			}
	
			// printable
			if (idx > 32) {
				int topleft = iv;
				int botright = topleft + 1;
				// round subpixel X to an integer - ft AA is optimized for
				// rounded integer coords
				int bX = (int)baseX;

				vertices[topleft].u = fontChars[idx].x;
				vertices[topleft].v = fontChars[idx].y;
				vertices[topleft].x = x + bX + fontChars[idx].xoffset;
				vertices[topleft].y = baseY + fontChars[idx].yoffset;
				vertices[topleft].z = 0;
	
				vertices[botright].u = fontChars[idx].x + fontChars[idx].width;
				vertices[botright].v = fontChars[idx].y + fontChars[idx].height;
				vertices[botright].x = x + bX + fontChars[idx].xoffset + fontChars[idx].width;
				vertices[botright].y = baseY + fontChars[idx].yoffset + fontChars[idx].height;
				vertices[botright].z = 0;
	
				baseX += fontChars[idx].xadvance;
				iv+=2;
				continue;
			}
			// the rest: ignored
		}
		baseY += font->getLineHeight();
	}
}

void BKBook::render() {
	struct BKVertex* vertices = 0;

	FZScreen::clear(0xffffff, FZ_COLOR_BUFFER);
	FZScreen::color(0xffffffff);
	FZScreen::matricesFor2D();
	FZScreen::enable(FZ_TEXTURE_2D);
	FZScreen::enable(FZ_BLEND);
	FZScreen::blendFunc(FZ_ADD, FZ_SRC_ALPHA, FZ_ONE_MINUS_SRC_ALPHA);

	fontText->bindForDisplay();
	FZScreen::ambientColor(0xff000000);
	int totalVertices = countVerticesForLines(baseLine, baseLine + screenLines);
	vertices = (struct BKVertex*)FZScreen::getListMemory(sizeof(struct BKVertex) * totalVertices);
	spritesForLines(15, 10, baseLine, baseLine + screenLines, vertices);
	FZScreen::drawArray(FZ_SPRITES,FZ_TEXTURE_32BITF|FZ_VERTEX_32BITF|FZ_TRANSFORM_2D,totalVertices,0,vertices);

	if (bannerFrames > 0) {
		int alpha = 0xff;
		if (bannerFrames <= 32) {
			alpha = bannerFrames*(256/32) - 8;
		}
		if (alpha > 0) {
			texUI->bindForDisplay();
			FZScreen::ambientColor(0x222222 | (alpha << 24));
			drawPill(150, 240, 180, 20, 6, 31, 1);
			fontBig->bindForDisplay();
			FZScreen::ambientColor(0xffffff | (alpha << 24));
			char t[256];
			int total = 1;
			if (totalLines > 0 && screenLines > 0) {
				total = (totalLines / screenLines) + 1;
			}
			snprintf(t, 256, "Page %d of %d", page, total);
			drawTextHC(t, fontBig, 244);
		}
	}
}

int BKBook::update(unsigned int buttons) {
	bannerFrames--;
	if (bannerFrames < 0)
		bannerFrames = 0;
	int* b = FZScreen::ctrlReps();
	if (b[BKUser::txtControls.previousPage] == 1 || b[BKUser::txtControls.previousPage] > 20) {
		baseLine-=screenLines;
		page--;
		if (baseLine < 0) {
			baseLine = 0;
			page = 1;
		}
		bannerFrames = 60;
	}
	if (b[BKUser::txtControls.nextPage] == 1 || b[BKUser::txtControls.nextPage] > 20) {
		if ((baseLine + screenLines) <= totalLines) {
			baseLine+=screenLines;
			page++;
		}
		bannerFrames = 60;
	}
	if (b[FZ_REPS_START] == 1) {
		return BK_CMD_INVOKE_MENU;
	}
	if (bannerFrames > 0)
		return BK_CMD_MARK_DIRTY;
	return 0;
}

