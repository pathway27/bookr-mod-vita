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

using namespace std;

#include "bkdocument.h"

#define BKFT_CONT_NONE			0
#define BKFT_CONT_LF			1
#define BKFT_CONT_EXTRALF		2

#define BKFT_STYLE_PLAIN		0
#define BKFT_STYLE_BOLD			1
//#define BKFT_STYLE_ITALIC		2
//#define BKFT_STYLE_BOLD_ITALIC	3

#define BKFT_FONT_SANS			0
//#define BKFT_FONT_FIXED			1

struct BKRun {
	char* text;
	bool lineBreak;
	int n;
	int style;
	int font;
	int bgcolor;
	int fgcolor;
};

struct BKLine {
	int firstRun;
	int firstRunOffset;
	int totalChars;
	float spaceWidth;
	BKLine(int fr, int fro, int tc, float sw) : firstRun(fr), firstRunOffset(fro), totalChars(tc), spaceWidth(sw) { }
	BKLine() { }
};

class BKFancyText : public BKDocument {
	private:
	BKLine* lines;
	int nLines;
	int topLine;
	int maxY;
	FZFont* font;
	int rotation;
	int lastFontSize;
	string lastFontFace;
	int	lastHeightPct;
	int lastWrapCR;

	int linesPerPage;
	int totalPages;
	void reflow(int width);

	protected:
	BKRun* runs;
	int nRuns;
	BKFancyText();
	~BKFancyText();

	void resizeView(int widht, int height);
	void resetFonts();
	int setLine(int l);
	int runForLine(int l);
	int lineForRun(int r);

	bool holdScroll;

	// a lot of ebook formats use HTML as a display format, on top of a
	// container format. so it makes sense to put the parser/tokenizer in
	// the base class
	static char* parseHTML(BKFancyText* r, char* in, int n);

	// same with plain text
	static char* parseText(BKFancyText* r, char* b, int length);

	public:
	virtual int updateContent();
	virtual int resume();
	virtual void renderContent();

	virtual void getFileName(string&) = 0;
	virtual void getTitle(string&) = 0;
	virtual void getType(string&) = 0;

	virtual bool isPaginated();
	virtual int getTotalPages();
	virtual int getCurrentPage();
	virtual int setCurrentPage(int);

	virtual bool isZoomable();
	virtual void getZoomLevels(vector<BKDocument::ZoomLevel>& v);
	virtual int getCurrentZoomLevel();
	virtual int setZoomLevel(int);
	virtual bool hasZoomToFit();
	virtual int setZoomToFitWidth();
	virtual int setZoomToFitHeight();

	virtual int pan(int, int);

	virtual int screenUp();
	virtual int screenDown();
	virtual int screenLeft();
	virtual int screenRight();

	virtual bool isRotable();
	virtual int getRotation();
	virtual int setRotation(int, bool bForce=false);

	virtual bool isBookmarkable();
	virtual void getBookmarkPosition(map<string, int>&);
	virtual int setBookmarkPosition(map<string, int>&);
};

#endif

