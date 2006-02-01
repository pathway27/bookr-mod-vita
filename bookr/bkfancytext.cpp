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

#include <list>
using namespace std;
#include "bkfancytext.h"

BKFancyText::BKFancyText() : lines(0), nLines(0), topLine(0), font(0), runs(0), nRuns(0) { }
BKFancyText::~BKFancyText() {
	if (runs)
		delete[] runs;
	if (lines)
		delete[] lines;
	if (font)
		font->release();
}

struct BKRunsIterator {
	BKRun* runs;
	int currentRun;
	int currentChar;
	int maxRuns;
	int lastContinuation;
	int globalPos;
	bool end() {
		return currentRun >= maxRuns ||
			((currentRun == maxRuns - 1) && currentChar >= runs[currentRun].n);
	}
	inline unsigned char forward() {
		if (end()) return 0;
		unsigned char c = runs[currentRun].text[currentChar];
		++currentChar;
		++globalPos;
		if (currentChar >= runs[currentRun].n) {
			lastContinuation = runs[currentRun].continuation;
			++currentRun;
			currentChar = 0;
		}
		return c;
	}
	inline unsigned char backward() {
		if (currentRun <= 0 && currentChar < 0) return 0;
		unsigned char c = runs[currentRun].text[currentChar];
		--currentChar;
		--globalPos;
		if (currentChar < 0) {
			//lastContinuation = runs[currentRun].continuation;
			--currentRun;
			currentChar = runs[currentRun].n - 1;
		}
		return c;
	}
	BKRunsIterator(BKRun* r, int cr, int cc, int mr) : 	runs(r), currentRun(cr), currentChar(cc), maxRuns(mr), lastContinuation(0), globalPos(0) { }
	BKRunsIterator(const BKRunsIterator& s) :
		runs(s.runs),
		currentRun(s.currentRun),
		currentChar(s.currentRun),
		maxRuns(s.currentRun),
		lastContinuation(s.lastContinuation),
		globalPos(s.globalPos) { }
};

void BKFancyText::reflow(int width) {
	if (lines)
		delete[] lines;
	list<BKLine> tempLines;

	int lineFirstRun = 0;
	int lineFirstRunOffset = 0;
	int lineSpaces = 0;
	int lineStartGlobalPos = 0;
	int currentWidth = 0;
	float spaceWidth = 0.0f;
	BKRunsIterator rit(runs, 0, 0, nRuns);
	BKRunsIterator lastSpace = rit;
	FZCharMetrics* fontChars = font->getMetrics();

	while (!rit.end()) {
		int c = rit.forward();
		if (c == 32) {
			lastSpace = rit;
			++lineSpaces;
		}
		currentWidth += fontChars[c].xadvance;
		if (currentWidth > width || rit.lastContinuation != BKFT_CONT_NONE) {
			int vSpace = font->getLineHeight();
			if (rit.lastContinuation == BKFT_CONT_EXTRALF) {
				vSpace *= 2;
			}
			if (rit.lastContinuation != BKFT_CONT_NONE) {
				 rit.lastContinuation = BKFT_CONT_NONE;
			} else if (lineSpaces > 0) {
				rit = lastSpace;
				spaceWidth = (float(width - currentWidth) + float(lineSpaces * fontChars[32].xadvance)) / float(lineSpaces);
			} else {
				rit.backward();			// consume the overflowing char
				spaceWidth = float(fontChars[32].xadvance);
			}
			tempLines.push_back(BKLine(lineFirstRun, lineFirstRunOffset, rit.globalPos - lineStartGlobalPos, spaceWidth, vSpace));
			lineFirstRun = rit.currentRun;
			lineFirstRunOffset = rit.currentChar;
			lineStartGlobalPos = rit.globalPos;
			lineSpaces = 0;
			currentWidth = 0;
		}
	}

	nLines = tempLines.size();
	lines = new BKLine[nLines];
	list<BKLine>::iterator it(tempLines.begin());
	int i = 0;
	while (it != tempLines.end()) {
		const BKLine& l = *it;
		lines[i] = l;
		++i;
		++it;
	}
}

void BKFancyText::resizeView(int width, int height) {
	reflow(width);			// FIX - sub margins
	linesPerPage = (272 / font->getLineHeight()) - 1;
	totalPages = (nLines / linesPerPage) + 1;
}


extern "C" {
extern unsigned int size_res_txtfont;
extern unsigned char res_txtfont[];
};

void BKFancyText::resetFonts() {
	if (font)
		font->release();
	// load font
	bool useBuiltin = BKUser::options.txtFont == "bookr:builtin";
	if (!useBuiltin) {
		font = FZFont::createFromFile((char*)BKUser::options.txtFont.c_str(), BKUser::options.txtSize, false);
		useBuiltin = font == 0;
	}
	if (useBuiltin) {
		font = FZFont::createFromMemory(res_txtfont, size_res_txtfont, BKUser::options.txtSize, false);
	}
	font->texEnv(FZ_TEX_MODULATE);
	font->filter(FZ_NEAREST, FZ_NEAREST);
}

int BKFancyText::updateContent() {
	return 0;
}

int BKFancyText::resume() {
	return 0;
}

void BKFancyText::renderContent() {
	FZScreen::clear(BKUser::options.txtBGColor & 0xffffff, FZ_COLOR_BUFFER);
	FZScreen::color(0xffffffff);
	FZScreen::matricesFor2D();
	FZScreen::enable(FZ_TEXTURE_2D);
	FZScreen::enable(FZ_BLEND);
	FZScreen::blendFunc(FZ_ADD, FZ_SRC_ALPHA, FZ_ONE_MINUS_SRC_ALPHA);

	font->bindForDisplay();
	FZScreen::ambientColor(0xff000000);
	int y = 0;
	for (int i = topLine; i < nLines; i++) {
		BKRun* run = &runs[lines[i].firstRun];
		int offset = lines[i].firstRunOffset;
		int x = 0;
		int n = lines[i].totalChars;
		do {
			int pn = n < run->n ? n : run->n;
			drawText(&run->text[offset], font, x, y, pn, false);
			n -= pn;
			offset = 0;
			++run;
		} while (n > 0);
		y += lines[i].vSpace;
		if (y > 272)
			break;
	}
}

int BKFancyText::setLine(int l) {
	int oldP = getCurrentPage();
	int oldTL = topLine;
	topLine = l;
	if (topLine >= nLines)
		topLine = nLines - 1;
	if (topLine < 0)
		topLine = 0;
	int cp = getCurrentPage();
	if (cp != oldP) {
		char t[256];
		snprintf(t, 256, "Page %d", cp);
		setBanner(t);
	}
	return oldTL != topLine ? BK_CMD_MARK_DIRTY : 0;
}


bool BKFancyText::isPaginated() {
	return true;
}

int BKFancyText::getTotalPages() {
	return totalPages;
}

// FIX PAGINATION: lines are of different sizes, depending on the style
// a precalc of page boundaries is required
int BKFancyText::getCurrentPage() {
	return (topLine / linesPerPage) + 1;
}

int BKFancyText::setCurrentPage(int p) {
	if (p <= 0)
		p = 1;
	if (p > totalPages)
		p = totalPages;
	--p;
	return setLine(p * linesPerPage);
}

int BKFancyText::pan(int x, int y) {
	return setLine(topLine + y);
}

int BKFancyText::screenUp() {
	return setCurrentPage(getCurrentPage() - 1);
}

int BKFancyText::screenDown() {
	return setCurrentPage(getCurrentPage() + 1);
}

int BKFancyText::screenLeft() {
	return 0;
}

int BKFancyText::screenRight() {
	return 0;
}

bool BKFancyText::isRotable() {
	return false;
}

int BKFancyText::getRotation() {
	return 0;
}

int BKFancyText::setRotation(int) {
	return 0;
}

bool BKFancyText::isBookmarkable() {
	return false;
}

void BKFancyText::getBookmarkPosition(map<string, int>& m) {
}

int BKFancyText::setBookmarkPosition(map<string, int>& m) {
	return 0;
}

bool BKFancyText::isZoomable() {
	return false;
}

void BKFancyText::getZoomLevels(vector<BKDocument::ZoomLevel>& v) {
}

int BKFancyText::getCurrentZoomLevel() {
	return 0;
}

int BKFancyText::setZoomLevel(int l) {
	return 0;
}

bool BKFancyText::hasZoomToFit() {
	return false;
}

int BKFancyText::setZoomToFitWidth() {
	return 0;
}

int BKFancyText::setZoomToFitHeight() {
	return 0;
}

