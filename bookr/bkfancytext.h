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

#ifndef BKFANCYTEXT_H
#define BKFANCYTEXT_H

#include <string>
#include <vector>

#include "fzscreen.h"
#include "fzfont.h"

using namespace std;

#include "bkdocument.h"

#define BKFT_CONT_NONE			0
#define BKFT_CONT_LF			1
#define BKFT_CONT_P				2

#define BKFT_STYLE_PLAIN		0
#define BKFT_STYLE_BOLD			1
#define BKFT_STYLE_ITALIC		2
#define BKFT_STYLE_BOLD_ITALIC	3

#define BKFT_FONT_FIXED			0
#define BKFT_FONT_SANS			1

// A block is a run of text with the same visual style. It can span
// multiple lines.
struct BKFancyBlock {
	string text;
	int continuation;
	int style;
	int font;
	int bgcolor;
	int fgcolor;
};

typedef vector<BKFancyBlock> BKFancyBlockList;
typedef BKFancyBlockList::iterator BKFancyBlockListIt;

class BKFancyText : public BKDocument {
	private:
	BKFancyBlockList blocks;
	int topVisibleBlock;
	int topVisiblePosition;
	string filePath;

	protected:
	BKKFancyText(string& file);
	~BKKFancyText();

	public:
	virtual void renderContent();

	virtual void getFilePath(string&);
	virtual void getTitle(string&);
	virtual void getType(string&);

	virtual bool isPaginated();
	virtual int getTotalPages();
	virtual int getCurrentPage();
	virtual void setCurrentPage(int);

	virtual bool isZoomable();
	virtual void getZoomLevels(vector<ZoomLevel>&);
	virtual int getCurrentZoomLevel();
	virtual void setZoomLevel(int);

	virtual void pan(int, int);

	virtual bool isRotable();
	virtual int getRotation();
	virtual void setRotation(int);

	virtual bool isBookmarkable();
	virtual void getBookmarkPosition(map<string, int>&);
	virtual void setBookmarkPosition(const map<string, int>&);

	static BKFancyText* create(string& file);
};

#endif

