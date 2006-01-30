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

#include "bkfancytext.h"

BKFancyText::BKFancyText() : lines(0), nLines(0), topLine(0), runs(0), nRuns(0) { }
BKFancyText::~BKFancyText() { }

void BKFancyText::reflow(int width) {
	free(lines);
	// temporal, slow line array
	vector<BKLine> tlines(30*500);
	BKLine line;
	int iline;
	bool clear = true;
	int x = 0;
	int y = 0;
	BKRun* run = runs;
	int irun = 0;
	FZCharMetrics* fontChars = fontBig->getMetrics();
	while (irun < nRuns) {
		// consume chars from the run until EOL or the end of the run arrives
		int lastSpace = -1;
		for (int i = 0; i < run->n; ++i) {
			int idx = run->text[i];
			if (x > width || idx == 10) {
				// new line
				x = 0;
				line.firstRun = irun;
				line.firstRunOffset = i;
				tlines.push_back(line);
			}
			// printable & white space
			if (idx >= 32)
				x += fontChars[idx].xadvance;
		}
		++irun;
		++run;
	}

	vector<BKLine>::iterator it(tlines.begin());
	int i = 0;
	while (it != tlines.end()) {
		const BKLine& l = *it;
		printf("line %d: fR = %d, fRO = %d\n", i, l.firstRun, l.firstRunOffset);
		++i;
		++it;
	}
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
	return 0;
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

