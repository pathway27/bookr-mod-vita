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

#include "bkstrlines.h"
#include "fzfont.h"

struct TextLineMetrics {
	char* base;			// base char of the line
	short size;			// total amount of bytes to parse
	short nVertices;		// final amount of vertices, to prealloc the array for the page
	float spaceWidth;	// white space width for the line, needed to implement justification
};

// alloc is based on blocks of 4096 TextLineMetrics 
struct TextLineMetricsBlock {
	int top;
	struct TextLineMetrics lines[4096];
};

// 4096*4096 lines max.. that's a lot of text. of course the right aproach is to use
// real dynamic arrays, but with alloc-on-demand of blocks of 4096 lines we cover any
// real word book. besides, at full size, it wouldnt fit in the RAM of the PSP :) 
static struct TextLineMetricsBlock* blocks[4096];

void bkInitPrint() {
	memset(blocks, 0, sizeof(struct TextLineMetricsBlock*)*4096);
}

void bkTextLineMetrics(FZFont* font, char* str, int n, int width) {
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
	for (i = 0; i < n; i++) {
		unsigned char c = str[i];

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
				i -= tlm.size - tlmSaved.size;
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

int bkCountVerticesForLines(int from, int to) {
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

void bkSpritesForLines(FZFont* font, int x, int y, int from, int to, BKVertex* vertices) {
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

