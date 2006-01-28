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

#include "bkdocument.h"
#include "bkpdf.h"

static bool isPDF(string& file) {
	char header[4];
	memset((void*)header, 0, 4);
	FILE* f = fopen(file.c_str(), "r");
	fread(header, 4, 1, f);
	fclose(f);
	return header[0] == 0x25 && header[1] == 0x50 && header[2] == 0x44 && header[3] == 0x46;
}

BKDocument* BKDocument::create(string& filePath) {
	BKDocument* doc = 0;
	if (isPDF(filePath)) {
		doc = BKPDF::create(filePath);
	} else {
		//doc = BKPlainText::create(filePath);
	}
	if (doc != 0)
		doc->buildToolbarMenus();
	return doc;
}

BKDocument::BKDocument() : 	mode(BKDOC_VIEW), bannerFrames(0), banner(""), 	toolbarSelMenu(0), toolbarSelMenuItem(0) {
	lastSuspendSerial = FZScreen::getSuspendSerial();
}

BKDocument::~BKDocument() {
}

void BKDocument::setBanner(char* b) {
	banner = b;
	bannerFrames = 60;
}

int BKDocument::update(unsigned int buttons) {
	// let the view quit update processing early for some special events
	if (lastSuspendSerial != FZScreen::getSuspendSerial()) {
		lastSuspendSerial = FZScreen::getSuspendSerial();
		int r = resume();
		if (r != 0)
			return r;
	}
	int r = updateContent();
	if (r != 0)
		return r;

	bannerFrames--;
	if (bannerFrames < 0)
		bannerFrames = 0;

	// banner fade
	if (bannerFrames > 0)
		return BK_CMD_MARK_DIRTY;

	if (mode == BKDOC_VIEW)
		return processEventsForView();

	return processEventsForToolbar();
}

int BKDocument::processEventsForView() {
	int* b = FZScreen::ctrlReps();

	// button handling - pagination
	if (isPaginated()) {
		int n = getTotalPages();
		int p = getCurrentPage();
		if (b[BKUser::controls.nextPage] == 1) {
			p++;
		}
		if (b[BKUser::controls.previousPage] == 1) {
			p--;
		}
		if (b[BKUser::controls.next10Pages] == 1) {
			p += 10;
		}
		if (b[BKUser::controls.previous10Pages] == 1) {
			p -= 10;
		}
		/*if (b[BKUser::controls.firstPage] == 1) {
			p = 1;
		}
		if (b[BKUser::controls.lastPage] == 1) {
			p = n;
		}*/
		/*if (b[BKUser::controls.screenfulUp] == 1) {
			?
		}
		if (b[BKUser::controls.screenfulDown] == 1) {
			?
		}*/
		int r = setCurrentPage(p);
		if (r != 0)
			return r;
	}

	// button handling - zoom
	if (isZoomable()) {
		vector<ZoomLevel> zooms;
		getZoomLevels(zooms);
		int z = getCurrentZoomLevel();
		if (b[BKUser::controls.zoomIn] == 1) {
			z++;
		}
		if (b[BKUser::controls.zoomOut] == 1) {
			z--;
		}
		/*if (b[BKUser::controls.zoomFitWidth] == 1) {
			z ?
		}
		if (b[BKUser::controls.zoomFitHeight] == 1) {
			z ?
		}
		*/
		int r = setZoomLevel(z);
		if (r != 0)
			return r;
	}

	// button handling - analog pad panning
	{
		int ax = 0, ay = 0;
		FZScreen::getAnalogPad(ax, ay);
		int r = pan(ax, ay);
		if (r != 0)
			return r;
	}

	// button handling - rotation - TO DO
	/*
	virtual bool isRotable() = 0;
	virtual int getRotation() = 0;
	virtual void setRotation(int) = 0;
	*/

	// bookmarks and other features are not supported by mapeable keys

	// main menu
	if (b[FZ_REPS_START] == 1) {
		return BK_CMD_INVOKE_MENU;
	}

	// toolbar
	if (b[FZ_REPS_SELECT] == 1) {
		mode = BKDOC_TOOLBAR;
		return BK_CMD_MARK_DIRTY;
	}

	return 0;
}

void BKDocument::buildToolbarMenus() {
	toolbarMenus[0].clear();
	if (isBookmarkable()) {
/*		string fn;
		getFileName(fn);
		BKBookmarkList bl;
		BKBookmarksManager::getBookmarks(fn, bl);
*/
		ToolbarItem i;
		i.label = "Add bookmark";
		i.iconX = 19;
		i.iconY = 0;
		i.iconW = 22;
		i.iconH = 26;
		i.circleLabel = "Select";
		toolbarMenus[0].push_back(i);

		i.label = "First page";
		toolbarMenus[1].push_back(i);
		i.label = "Last page";
		toolbarMenus[1].push_back(i);
		i.label = "Previous 10 pages";
		toolbarMenus[1].push_back(i);
		i.label = "Next 10 pages";
		toolbarMenus[1].push_back(i);
		i.label = "Go to page";
		toolbarMenus[1].push_back(i);

		i.label = "Fit height";
		toolbarMenus[2].push_back(i);
		i.label = "Fit width";
		toolbarMenus[2].push_back(i);
		i.label = "Zoom out";
		toolbarMenus[2].push_back(i);
		i.label = "Zoom in";
		toolbarMenus[2].push_back(i);

	} else {
		ToolbarItem i;
		i.label = "No bookmark support";
		toolbarMenus[0].push_back(i);
	}
}

int BKDocument::processEventsForToolbar() {
	int* b = FZScreen::ctrlReps();

	if (b[FZ_REPS_UP] == 1 || b[FZ_REPS_UP] > 20) {
		toolbarSelMenuItem++;
	}
	if (b[FZ_REPS_DOWN] == 1 || b[FZ_REPS_DOWN] > 20) {
		toolbarSelMenuItem--;
	}
	if (b[FZ_REPS_LEFT] == 1 || b[FZ_REPS_LEFT] > 20) {
		toolbarSelMenu--;
	}
	if (b[FZ_REPS_RIGHT] == 1 || b[FZ_REPS_RIGHT] > 20) {
		toolbarSelMenu++;
	}

	// wrap menu indexes
	if (toolbarSelMenu >= 3)
		toolbarSelMenu = 0;
	if (toolbarSelMenu < 0)
		toolbarSelMenu = 2;
	if (toolbarSelMenuItem >= (int)toolbarMenus[toolbarSelMenu].size())
		toolbarSelMenuItem = 0;
	if (toolbarSelMenuItem < 0)
		toolbarSelMenuItem = toolbarMenus[toolbarSelMenu].size() - 1;

	// main menu
	if (b[FZ_REPS_START] == 1) {
		return BK_CMD_INVOKE_MENU;
	}

	// view
	if (b[FZ_REPS_SELECT] == 1) {
		mode = BKDOC_VIEW;
		return BK_CMD_MARK_DIRTY;
	}

	return 0;
}

void BKDocument::render() {
	// content
	renderContent();
	// label
	if (bannerFrames > 0 && BKUser::options.displayLabels) {
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
			drawTextHC((char*)banner.c_str(), fontBig, 244);
		}
	}

	if (mode != BKDOC_TOOLBAR)
		return;

	// all of the icons menus must have at least one item

	// wrap menu indexes
	if (toolbarSelMenu >= 3)
		toolbarSelMenu = 0;
	if (toolbarSelMenu < 0)
		toolbarSelMenu = 2;
	if (toolbarSelMenuItem >= (int)toolbarMenus[toolbarSelMenu].size())
		toolbarSelMenuItem = 0;
	if (toolbarSelMenuItem < 0)
		toolbarSelMenuItem = toolbarMenus[toolbarSelMenu].size() - 1;

	// background
	texUI->bindForDisplay();
	FZScreen::ambientColor(0xf0222222);
	drawTPill(20, 272 - 75, 480 - 46, 272, 6, 31, 1);

	// context label
	FZScreen::ambientColor(0xff555555);
	//drawTPill(25, 272 - 40, 480 - 46 - 11, 40, 6, 31, 1);
	drawTPill(25, 272 - 30, 480 - 46 - 11, 30, 6, 31, 1);

	// highlight icon column
	FZScreen::ambientColor(0xf0555555);
	drawPill(
		25 + toolbarSelMenu*55,
		272 - 156 - toolbarMenus[toolbarSelMenu].size()*35+70,
		40,
		toolbarMenus[toolbarSelMenu].size()*35+45,
		6,
		31, 1
	);

	// selected icon item row
	FZScreen::ambientColor(0xf0cccccc);
	int iw = textW((char*)toolbarMenus[toolbarSelMenu][toolbarSelMenuItem].label.c_str(), fontBig);
	int mw = toolbarMenus[toolbarSelMenu][toolbarSelMenuItem].minWidth;
	if (iw < mw)
		iw = mw;
	drawPill(
		30 + toolbarSelMenu*55,
		272 - 156 - toolbarSelMenuItem*35+40,
		iw + 10 + 35,
		30,
		6, 31, 1);

	// status icons
	FZScreen::ambientColor(0xffffffff);
	drawImage(400, 222, 16, 16, 100, 0);
	drawImage(350, 226, 16, 16, 100, 20);

	// icon bar
	texUI2->bindForDisplay();
	FZScreen::ambientColor(0xffffffff);
	// menu row
	for (int i = 0; i < 3; i++) {
		drawImage(38 + i*55, 205, 18, 26, 0, 0);
	}
	// selected column
	for (int i = 0; i < (int)toolbarMenus[toolbarSelMenu].size(); i++) {
		const ToolbarItem& it = toolbarMenus[toolbarSelMenu][i];
		if (i == toolbarSelMenuItem)
			FZScreen::ambientColor(0xff000000);
		else
			FZScreen::ambientColor(0xffffffff);
		if (it.iconW > 0)
			drawImage(
				40 + toolbarSelMenu*55,
				272 - 156 - i*35+45,
				it.iconW, it.iconH, it.iconX, it.iconY);
	}

	fontBig->bindForDisplay();
	// label for selected item
	FZScreen::ambientColor(0xff000000);
	const ToolbarItem& it = toolbarMenus[toolbarSelMenu][toolbarSelMenuItem];
	drawText((char*)it.label.c_str(), fontBig, 40 + toolbarSelMenu*55 + 35, 272 - 156 - toolbarSelMenuItem*35+48);

	fontSmall->bindForDisplay();
	FZScreen::ambientColor(0xffeeeeee);
	drawText("Page 23 of 102", fontSmall, 370, 205);
	drawText("to:do", fontSmall, 420, 224);
	drawText("TD%", fontSmall, 370, 224);
	/*FZScreen::ambientColor(0xff000000);
	drawText("timetable.pdf", fontSmall, 38+22+15, 125);
	drawText("Page 6        09:34 11/20/2005", fontSmall, 38+22+15, 139);*/
}

