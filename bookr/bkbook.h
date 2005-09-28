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

#ifndef BKBOOK_H
#define BKBOOK_H

#include <string>

#include "fzscreen.h"
#include "fzfont.h"

using namespace std;

#include "bklayer.h"

class BKBook : public BKLayer {
	struct BKVertex {
		float u,v;
		float x,y,z;
	};

	struct TextLineMetrics {
		char* base;		// base char of the line
		short size;		// total amount of bytes to parse
		short nVertices;	// final amount of vertices, to prealloc the array for the page
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
	// and I don't like using the STL for large amounts of data that need fast
	// access and low overhead. that's the exact opposite of how current STL implementations
	// work :)
	struct TextLineMetricsBlock* blocks[4096];

	string path;

	char* text;
	int textLen;

	int baseLine;
	int page;
	int screenLines;
	int totalLines;

	int bannerFrames;

	protected:
	BKBook(string& file);
	~BKBook();
	void textLineMetrics(int width);
	int countVerticesForLines(int from, int to);
	void spritesForLines(int x, int y, int from, int to, BKVertex* vertices);
	void skipPages(int offset);

	public:
	virtual int update(unsigned int buttons);
	virtual void render();
	virtual void setBookmark();

	static BKBook* create(string& file, int size);
};

#endif

