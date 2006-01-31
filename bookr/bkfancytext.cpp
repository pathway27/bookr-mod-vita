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

BKFancyText::BKFancyText() : lines(0), nLines(0), topLine(0), runs(0), nRuns(0) { }
BKFancyText::~BKFancyText() {
	if (runs)
		delete[] runs;
	if (lines)
		delete[] lines;
}

struct BKRunsIterator {
	BKRun* runs;
	int currentRun;
	int currentChar;
	int maxRuns;
	bool end() {
		return currentRun >= maxRuns;
	}
	inline unsigned char forward() {
		if (currentChar >= runs[currentRun].n) {
			++currentRun;
			currentChar = -1;
		}
		++currentChar;
		return end() ? 0 : runs[currentRun].text[currentChar];
	}
	inline unsigned char backward() {
		if (currentChar <= 0) {
			--currentRun;
			currentChar = runs[currentRun].n;
		}
		--currentChar;
		return currentRun <= 0 && currentChar < 0 ? 0 : runs[currentRun].text[currentChar];
	}
	BKRunsIterator(BKRun* r, int cr, int cc, int mr) : 	runs(r), currentRun(cr), currentChar(cc), maxRuns(mr) { }
};

void BKFancyText::reflow(int width) {
	if (lines)
		delete[] lines;
	list<BKLine> tempLines;

	int lineFirstRun = 0;
	int lineFirstRunOffset = 0;
	int lineChars = 0;
	int lineSpaces = 0;
	int currentWidth = 0;
	float spaceWidth = 0.0f;
	BKRunsIterator rit(runs, 0, -1, nRuns);
	FZCharMetrics* fontChars = fontBig->getMetrics();

	while (!rit.end()) {
		int c = rit.forward();
		++lineChars;
		if (c == 32) ++lineSpaces;
		currentWidth += fontChars[c].xadvance;
		if (currentWidth > width) {
			while (c != 32 && lineSpaces > 0) {
				c = rit.backward();
				--lineChars;
				currentWidth -= fontChars[c].xadvance;
			}
			if (lineSpaces != 0)
				spaceWidth = (float(width - currentWidth) + float(lineSpaces * fontChars[32].xadvance)) / float(lineSpaces);
			else
				spaceWidth = float(fontChars[32].xadvance);
			tempLines.push_back(BKLine(lineFirstRun, lineFirstRunOffset, lineChars, spaceWidth));
			printf("line: fR = %d, fRO = %d\n", lineFirstRun, lineFirstRunOffset);
			lineFirstRun = rit.currentRun;
			lineFirstRunOffset = rit.currentChar;
			lineChars = 0;
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
		printf("line %d: fR = %d, fRO = %d\n", i, l.firstRun, l.firstRunOffset);
		++i;
		++it;
	}
}
#if 0
void BKFancyText::reflow(int width) {
	list<BKLine> tlines;
	BKLine line, lastLine;
	int iline;
	bool clear = true;
	int x = 0;
	int y = 0;
	BKRun* run = runs;
	int irun = 0;
	FZCharMetrics* fontChars = fontBig->getMetrics();
	nLines = 0;
	if (nRuns > 0) {
		// first line
		lastLine.firstRun = 0;
		lastLine.firstRunOffset = 0;
		++nLines;
	}
	while (irun < nRuns) {
		// consume chars from the run until EOL or the end of the run arrives
		int lastSpace = 0;
		int nspaces = 0;
		int totalchars = 0;
		for (int i = 0; i < run->n; ++i) {
			int idx = run->text[i];
			++totalchars;
			if (idx == 0x20) {
				lastSpace = i;
				++nspaces;
			}
			if (x > width || idx == 10) {
				// new line starts at the last space
				line.firstRun = irun;
				line.firstRunOffset = lastSpace;
				// NO, this is for the previous line
				if (idx != 10 || nspaces == 0)
					lastLine.spaceWidth = (float(480 - x) + float(nspaces * fontChars[32].xadvance)) / float(nspaces);
				else
					lastLine.spaceWidth = float(fontChars[32].xadvance);
				lastLine.totalChars = totalchars - (i - lastSpace);
				// start from the char just after the last space
				i = lastSpace + 1;
				x = 0;
				// add new line
				tlines.push_back(lastLine);
				lastLine = line;
				++nLines;
			}
			// printable & white space
			if (idx >= 32)
				x += fontChars[idx].xadvance;
		}
		++irun;
		++run;
	}
}
#endif

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

	fontBig->bindForDisplay();
	FZScreen::ambientColor(0xff000000);
	int y = 0;
	for (int i = topLine; i < nLines; i++) {
		BKRun& run = runs[lines[i].firstRun];
		drawText(&run.text[lines[i].firstRunOffset], fontBig, 0, y, lines[i].totalChars, false);
		y += fontBig->getLineHeight();
		if (y > 272)
			break;
	}
}

bool BKFancyText::isPaginated() {
	return false;
}

int BKFancyText::getTotalPages() {
	return 0;
}

int BKFancyText::getCurrentPage() {
	return 0;
}

int BKFancyText::setCurrentPage(int p) {
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

int BKFancyText::pan(int x, int y) {
	int oldTL = topLine;
	topLine += y>>4;
	if (topLine < 0)
		topLine = 0;
	if (topLine >= nLines)
		topLine = nLines - 1;
	return oldTL != topLine ? BK_CMD_MARK_DIRTY : 0;
}

int BKFancyText::screenUp() {
	return 0;
}

int BKFancyText::screenDown() {
	return 0;
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

