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
#include "filetypes/bkpdf.h"
#include "filetypes/bkdjvu.h"
#include "filetypes/bkpalmdoc.h"
#include "filetypes/bkplaintext.h"

BKDocument* BKDocument::create(string& filePath,string& longFileName) {
  FILE* f = fopen(filePath.c_str(),"r");
  if(f){
    fclose(f);
  }
  else{
    return (BKDocument*)-2;
  }
	BKDocument* doc = 0;
	bool skipBookmark = false;
	if (BKPDF::isPDF(filePath)) {
		skipBookmark = true;
		doc = BKPDF::create(filePath,longFileName);
	} else if (BKDJVU::isDJVU(filePath)) {
		skipBookmark = true;
		doc = BKDJVU::create(filePath,longFileName);
	} else if (BKPalmDoc::isPalmDoc(filePath)) {
	  doc = BKPalmDoc::create(filePath,longFileName);
	} else {
	  doc = BKPlainText::create(filePath,longFileName);
	}
	if (doc != 0 && (int)doc != -1 )
	{
		doc->title_skippix = 0;
		doc->title_skippix_max = -1;
		doc->buildToolbarMenus();
		if (doc->isBookmarkable() && ! skipBookmark ) {
			BKBookmark b;
			string fn;
			doc->getFileName(fn);
			if (BKBookmarksManager::getLastView(fn, b)) {
				doc->setBookmarkPosition(b.viewData);
				doc->setZoom(b.zoom);
			}
		}
		doc->outline_selItem = 0;
		doc->outline_topItem = 0;
		doc->title_type = BKUser::options.defaultTitleMode;
		string stitle;
		doc->getTitle(stitle, doc->title_type % 5);
		doc->miTitle = new BKMenuItem(stitle,"",0);
		BKUser::options.t_ignore_x = BKUser::options.ignoreXInOutlineOnSquare;
	}
  	return doc;
}

BKDocument::BKDocument() : 	mode(BKDOC_VIEW), bannerFrames(0), banner(""), 	tipFrames(120), toolbarSelMenu(0), toolbarSelMenuItem(0), frames(0) {
	lastSuspendSerial = FZScreen::getSuspendSerial();
	thumbnailFrames = BKUser::options.thumbnail;
	miTitle = 0;
}

BKDocument::~BKDocument() {
  delete miTitle;
}

void BKDocument::saveLastView() {
	if (isBookmarkable()) {
		string fn, t;
		getFileName(fn);
		getTitle(t);
		BKBookmark b;
		b.title = t;
		b.page = isPaginated() ? getCurrentPage() : 0;
		b.createdOn = "to do: creation date";
		b.lastView = true;
		b.zoom = isZoomable()? getCurrentZoom() : 0.0f;
		getBookmarkPosition(b.viewData);
		BKBookmarksManager::addBookmark(fn, b);
		BKBookmarksManager::setLastFile(fn,longFileName);
	}
}

void BKDocument::setBanner(char* b) {
	banner = b;
	bannerFrames = 60;
}

void BKDocument::setThumbnail(int pagew, int pageh, int screenw, int screenh, int screenx, int screeny) {

    float ratio = 1.0f;
    int tn_size = 100;
    if (pagew > tn_size || pageh > tn_size){
	if (pagew > pageh)
	    ratio = tn_size * 1.0f / pagew;
	else
	    ratio = tn_size * 1.0f / pageh;
    }

/*    
    printf("Set TB: pw: %d\n",pagew);
    printf("Set TB: ph: %d\n",pageh);
    printf("Set TB: sw: %d\n",screenw);
    printf("Set TB: sh: %d\n",screenh);
    printf("Set TB: sx: %d\n",screenx);
    printf("Set TB: sy: %d\n",screeny);
    printf("Set TB: ratio: %f\n",ratio);

*/

    tn_pagew = (int) pagew * ratio;
    tn_pageh = (int) pageh * ratio;
    tn_screenw = (int) screenw * ratio + 1;
    tn_screenh = (int) screenh * ratio + 1;
    tn_screenx = (int) screenx * ratio;
    tn_screeny = (int) screeny * ratio;
    if (tn_screenw + tn_screenx > tn_pagew) 
	tn_screenw = tn_pagew - tn_screenx;

    if (tn_screenh + tn_screeny > tn_pageh)
        tn_screenh = tn_pageh - tn_screeny;
    
    thumbnailFrames = BKUser::options.thumbnail;
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
	tipFrames--;
	thumbnailFrames--;
	if (bannerFrames < 0)
		bannerFrames = 0;
	if (tipFrames < 0)
		tipFrames = 0;
	if (thumbnailFrames < 0)
	        thumbnailFrames =0;

	// banner fade - this blocks event input during the fade
	//if (bannerFrames > 0)
	//	return BK_CMD_MARK_DIRTY;

	r = 0;
	if (mode == BKDOC_VIEW)
		r = processEventsForView();
	else
		r = processEventsForToolbar();

	// banner fade - this allows events during the fade
	if (bannerFrames > 0 && r == 0)
		r = BK_CMD_MARK_DIRTY;
	if (tipFrames > 0 && r == 0)
		r = BK_CMD_MARK_DIRTY;
	if (thumbnailFrames > 0 && r == 0)
	        r = BK_CMD_MARK_DIRTY;


	// clock tick
	frames++;
	if (frames % 60 == 0 && r == 0 && mode != BKDOC_VIEW)
		r = BK_CMD_MARK_DIRTY;

	return r;
}

int BKDocument::processEventsForView() {
	int* b = FZScreen::ctrlReps();

	// button handling - pagination
	if (isPaginated()) {
		int n = getTotalPages();
		int p = getCurrentPage();
		int op = p;
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
		int r = 0;
		if (op != p)
			r = setCurrentPage(p);
		if (r != 0)
			return r;
	}

	// button handling - zoom
	if (isZoomable()) {
		vector<ZoomLevel> zooms;
		getZoomLevels(zooms);
		int z = getCurrentZoomLevel();
		bool zoommed = false;
		if (b[BKUser::controls.zoomIn] == 1) {
			z++;
			zoommed = true;
		}
		if (b[BKUser::controls.zoomOut] == 1) {
			z--;
			zoommed = true;
		}
		/*if (b[BKUser::controls.zoomFitWidth] == 1) {
			z ?
		}
		if (b[BKUser::controls.zoomFitHeight] == 1) {
			z ?
		}
		*/
		if(zoommed){
		  int r = setZoomLevel(z);
		  if (r != 0)
		    return r;
		}
	}

	// button handling - analog pad panning
	if(b[FZ_REPS_HOLD] == 0)
	{
		int ax = 0, ay = 0;
		FZScreen::getAnalogPad(ax, ay);
#ifndef PSP
		//fprintf(stderr,"analog: %d,%d\n",ax,ay);
#endif
		int r = pan(BKUser::options.analogRateX * ax / 100, BKUser::options.analogRateY * ay / 100);
		if (r != 0)
			return r;
	}

	// button handling - digital panning
	{
		if (b[BKUser::controls.screenUp] == 1 || b[BKUser::controls.screenUp] > 20) {
			int r = screenUp();
			if (r != 0)
				return r;
		}
		if (b[BKUser::controls.screenDown] == 1 || b[BKUser::controls.screenDown] > 20) {
			int r = screenDown();
			if (r != 0)
				return r;
		}
		if (b[BKUser::controls.screenLeft] == 1 || b[BKUser::controls.screenLeft] > 20) {
			int r = screenLeft();
			if (r != 0)
				return r;
		}
		if (b[BKUser::controls.screenRight] == 1 || b[BKUser::controls.screenRight] > 20) {
			int r = screenRight();
			if (r != 0)
				return r;
		}
	}
	
	// button handling - rotation - TO DO
	/*
	virtual bool isRotable() = 0;
	virtual int getRotation() = 0;
	virtual void setRotation(int) = 0;
	*/

	// bookmarks and other features are not supported by mapeable keys

	// main menu
	if (b[BKUser::controls.showMainMenu] == 1) {
		return BK_CMD_INVOKE_MENU;
	}

	// toolbar
	if (b[BKUser::controls.showToolbar] == 1) {
		mode = BKDOC_TOOLBAR;
		return BK_CMD_MARK_DIRTY;
	}

	return 0;
}

void BKDocument::buildToolbarMenus() {
	toolbarMenus[0].clear();

	if (getOutlineType()){
	  ToolbarItem i;
	  i.circleLabel = "Select";
	  i.label = "Outlines";
	  i.iconX = 0;
	  i.iconY = 106;
	  i.iconW = 18;
	  i.iconH = 22;
	  toolbarMenus[0].push_back(i);
	}

	if (isBookmarkable()) {
		ToolbarItem i;
// 		i.label = "Prune bookmark";
// 		i.iconX = 82;
// 		i.iconY = 0;
// 		i.iconW = 22;
// 		i.iconH = 26;
// 		i.circleLabel = "Select";
// 		toolbarMenus[0].push_back(i);

		i.label = "Add bookmark";
		i.iconX = 19;
		i.iconY = 0;
		i.iconW = 22;
		i.iconH = 26;
		i.circleLabel = "Select";
		toolbarMenus[0].push_back(i);

		string fn;
		getFileName(fn);
		bookmarkList.clear();
		BKBookmarksManager::getBookmarks(fn, bookmarkList);
		BKBookmarkListIt it(bookmarkList.begin());
		char t[256];
		while (it != bookmarkList.end()) {
			const BKBookmark& b = *it;
			snprintf(t, 256, "Page %d", b.page);
			i.label = t;
			i.iconX = 0;
			i.iconY = 0;
			i.iconW = 18;
			i.iconH = 26;
			i.circleLabel = "Jump to";
			i.triangleLabel = "Delete";
			toolbarMenus[0].push_back(i);
			++it;
		}
	} else {
		ToolbarItem i;
		i.label = "No bookmark support";
		i.iconX = 57;
		i.iconY = 0;
		i.iconW = 18;
		i.iconH = 26;
		toolbarMenus[0].push_back(i);
	}

	toolbarMenus[1].clear();
	if (isPaginated()) {
		ToolbarItem i;
		i.label = "First page";
		i.iconX = 0;
		i.iconY = 26;
		i.iconW = 18;
		i.iconH = 26;
		i.circleLabel = "Select";
		toolbarMenus[1].push_back(i);

		i.label = "Last page";
		i.iconX = 19;
		i.iconY = 26;
		i.iconW = 18;
		i.iconH = 26;
		toolbarMenus[1].push_back(i);

		i.label = "Previous 10 pages";
		i.iconX = 95;
		i.iconY = 26;
		i.iconW = 18;
		i.iconH = 26;
		toolbarMenus[1].push_back(i);

		i.label = "Next 10 pages";
		i.iconX = 76;
		i.iconY = 26;
		i.iconW = 18;
		i.iconH = 26;
		toolbarMenus[1].push_back(i);

		i.label = "Go to page";
		i.iconX = 0;
		i.iconY = 53;
		i.iconW = 18;
		i.iconH = 26;
		toolbarMenus[1].push_back(i);
	} else {
		ToolbarItem i;
		i.label = "No pagination support";
		i.iconX = 57;
		i.iconY = 0;
		i.iconW = 18;
		i.iconH = 26;
		toolbarMenus[1].push_back(i);
	}

	toolbarMenus[2].clear();
	if (isZoomable()) {
		ToolbarItem i;
		i.circleLabel = "Select";

		if (hasZoomToFit()) {
			i.label = "Fit height";
			i.iconX = 0;
			i.iconY = 78;
			i.iconW = 18;
			i.iconH = 26;
			toolbarMenus[2].push_back(i);
	
			i.label = "Fit width";
			i.iconX = 95;
			i.iconY = 53;
			i.iconW = 18;
			i.iconH = 26;
			toolbarMenus[2].push_back(i);
		}

		i.label = "Zoom out";
		i.iconX = 76;
		i.iconY = 53;
		i.iconW = 18;
		i.iconH = 26;
		toolbarMenus[2].push_back(i);

		i.label = "Zoom in";
		i.iconX = 57;
		i.iconY = 53;
		i.iconW = 18;
		i.iconH = 26;
		toolbarMenus[2].push_back(i);

		i.label = "Zoom in ...";
		i.iconX = 76;
		i.iconY = 80;
		i.iconW = 18;
		i.iconH = 22;
		toolbarMenus[2].push_back(i);

	} else {
		ToolbarItem i;
		i.label = "No zoom support";
		i.iconX = 57;
		i.iconY = 0;
		i.iconW = 18;
		i.iconH = 26;
		toolbarMenus[2].push_back(i);
	}

	toolbarMenus[3].clear();
	if (isRotable()) {
		ToolbarItem i;
		i.circleLabel = "Select";
		i.label = "Rotate 90� clockwise";
		i.iconX = 39;
		i.iconY = 79;
		i.iconW = 17;
		i.iconH = 26;
		toolbarMenus[3].push_back(i);

		i.label = "Rotate 90� counterclockwise";
		i.iconX = 57;
		i.iconY = 79;
		i.iconW = 17;
		i.iconH = 26;
		toolbarMenus[3].push_back(i);
	} else {
		ToolbarItem i;
		i.label = "No rotation support";
		i.iconX = 57;
		i.iconY = 0;
		i.iconW = 18;
		i.iconH = 26;
		toolbarMenus[3].push_back(i);
	}
	if (isZoomable()){
	  ToolbarItem i;
	  i.circleLabel = "Switch";
	  i.label = "Even-Odd mode";
	  i.iconX = 96;
	  i.iconY = 105;
	  i.iconW = 18;
	  i.iconH = 22;
	  toolbarMenus[3].push_back(i);
	}
// 	if (getOutlineType()){
// 	  ToolbarItem i;
// 	  i.circleLabel = "Select";
// 	  i.label = "Outlines";
// 	  i.iconX = 0;
// 	  i.iconY = 106;
// 	  i.iconW = 18;
// 	  i.iconH = 22;
// 	  toolbarMenus[3].push_back(i);
// 	}
}

int BKDocument::processEventsForToolbar() {
	int* b = FZScreen::ctrlReps();

	if (b[BKUser::controls.menuUp] == 1 || b[BKUser::controls.menuUp] > 20) {
		toolbarSelMenuItem++;
	}
	if (b[BKUser::controls.menuDown] == 1 || b[BKUser::controls.menuDown] > 20) {
		toolbarSelMenuItem--;
	}
	if (b[BKUser::controls.menuLeft] == 1 || b[BKUser::controls.menuLeft] > 20) {
		toolbarSelMenu--;
	}
	if (b[BKUser::controls.menuRight] == 1 || b[BKUser::controls.menuRight] > 20) {
		toolbarSelMenu++;
	}

	// wrap menu indexes
	if (toolbarSelMenu >= 4)
		toolbarSelMenu = 0;
	if (toolbarSelMenu < 0)
		toolbarSelMenu = 3;
	if (toolbarSelMenuItem >= (int)toolbarMenus[toolbarSelMenu].size())
		toolbarSelMenuItem = 0;
	if (toolbarSelMenuItem < 0)
		toolbarSelMenuItem = toolbarMenus[toolbarSelMenu].size() - 1;

	int itemNumOfFirstBookmark = 1;
	if (getOutlineType()){
	  itemNumOfFirstBookmark = 2;
	}
	if (b[BKUser::controls.alternate] == 1) {
		// delete bookmark
		if (toolbarSelMenu == 0 && toolbarSelMenuItem >= itemNumOfFirstBookmark && isBookmarkable()) {
			string fn;
			getFileName(fn);
			BKBookmarkListIt it(bookmarkList.begin());
			int di =  toolbarSelMenuItem - itemNumOfFirstBookmark;
			for (int i = 0; i < di; i++, it++)
			  ;
			bookmarkList.erase(it);
			BKBookmarksManager::setBookmarks(fn, bookmarkList);
			buildToolbarMenus();
			return BK_CMD_MARK_DIRTY;
		}
	}
	if (b[BKUser::controls.details] == 1) {
	  // change title type;
	  title_type ++;
	  if (title_type > 4)
	    title_type = 0;
	  string stitle;
	  getTitle(stitle, title_type % 5);
	  delete miTitle;
	  title_skippix = 0;
	  title_skippix_max = -1;
	  miTitle = new BKMenuItem(stitle,"",0);
	  return BK_CMD_MARK_DIRTY;
	}
	if (b[BKUser::controls.select] == 1) {

		if (getOutlineType() && toolbarSelMenu == 0 && toolbarSelMenuItem == 0){
			return BK_CMD_INVOKE_OUTLINES;
		}

		if (toolbarSelMenu == 0 && toolbarSelMenuItem == (itemNumOfFirstBookmark-1) && isBookmarkable()) {
			string fn, t;
			getFileName(fn);
			getTitle(t);
			BKBookmark b;
			b.title = t;
			b.page = isPaginated() ? getCurrentPage() : 0;
			b.createdOn = "to do: creation date";
			b.zoom = isZoomable()? getCurrentZoom(): 0.0f;
			getBookmarkPosition(b.viewData);
			BKBookmarksManager::addBookmark(fn, b);
			buildToolbarMenus();
			return BK_CMD_MARK_DIRTY;
		}
		// jump to bookmark
		if (toolbarSelMenu == 0 && toolbarSelMenuItem >= itemNumOfFirstBookmark && isBookmarkable()) {
			int di =  toolbarSelMenuItem - itemNumOfFirstBookmark;
			int ret = setBookmarkPosition(bookmarkList[di].viewData);
			setZoom(bookmarkList[di].zoom);
			return ret;
		}
		// first page
		if (toolbarSelMenu == 1 && toolbarSelMenuItem == 0 && isPaginated()) {
			int r = setCurrentPage(0);
			if (r != 0)
				return r;
		}
		// last page
		if (toolbarSelMenu == 1 && toolbarSelMenuItem == 1 && isPaginated()) {
			int n = getTotalPages();
			int r = setCurrentPage(n);
			if (r != 0)
				return r;
		}
		// prev 10 pages
		if (toolbarSelMenu == 1 && toolbarSelMenuItem == 2 && isPaginated()) {
			int p = getCurrentPage();
			p -= 10;
			int r = setCurrentPage(p);
			if (r != 0)
				return r;
		}
		// next 10 pages
		if (toolbarSelMenu == 1 && toolbarSelMenuItem == 3 && isPaginated()) {
			int p = getCurrentPage();
			p += 10;
			int r = setCurrentPage(p);
			if (r != 0)
				return r;
		}
		// go to page
		if (toolbarSelMenu == 1 && toolbarSelMenuItem == 4 && isPaginated()) {
			return BK_CMD_INVOKE_PAGE_CHOOSER;
		}
		int szi = 4;
		int zi = 3;
		int zo = 2;
		if (hasZoomToFit()) {
			if (toolbarSelMenu == 2 && toolbarSelMenuItem == 1 && isZoomable()) {
				int r = setZoomToFitWidth();
				if (r != 0)
					return r;
			}
			if (toolbarSelMenu == 2 && toolbarSelMenuItem == 0 && isZoomable()) {
				int r = setZoomToFitHeight();
				if (r != 0)
					return r;
			}
		} else {
			szi = 2;
			zi = 1;
			zo = 0;
		}
		
		// zoom in
		if (toolbarSelMenu == 2 && toolbarSelMenuItem == zi && isZoomable()) {
			vector<ZoomLevel> zooms;
			getZoomLevels(zooms);
			int z = getCurrentZoomLevel();
			z++;
			int r = setZoomLevel(z);
			if (r != 0)
				return r;
		}
		// zoom out
		if (toolbarSelMenu == 2 && toolbarSelMenuItem == zo && isZoomable()) {
			vector<ZoomLevel> zooms;
			getZoomLevels(zooms);
			int z = getCurrentZoomLevel();
			z--;
			int r = setZoomLevel(z);
			if (r != 0)
				return r;
		}
		// specified zoom in
		if (toolbarSelMenu == 2 && toolbarSelMenuItem == szi && isZoomable()) {
		  thumbnailFrames = 0;
		  tipFrames = 0;
		  bannerFrames = 0;
		  return BK_CMD_INVOKE_ZOOM_IN;
		}
		// rotate cw
		if (toolbarSelMenu == 3 && toolbarSelMenuItem == 0 && isRotable()) {
			FZScreen::setSpeed(BKUser::options.pspMenuSpeed);
			int z = getRotation();
			z++;
			int r = setRotation(z);
			FZScreen::setSpeed(BKUser::options.pspSpeed);
			if (r != 0)
				return r;
		}
		// rotate ccw
		if (toolbarSelMenu == 3 && toolbarSelMenuItem == 1 && isRotable()) {
			FZScreen::setSpeed(BKUser::options.pspMenuSpeed);
			int z = getRotation();
			z--;
			int r = setRotation(z);
			FZScreen::setSpeed(BKUser::options.pspSpeed);
			if (r != 0)
				return r;
		}
		// even-odd mod
		if (toolbarSelMenu == 3 && isZoomable() &&((toolbarSelMenuItem == 1 && !isRotable())||(toolbarSelMenuItem == 2 && isRotable()))){

		  if(xpos_mode)
		    xpos_mode = 0;
		  else
		    xpos_mode = 1;

		  return 0;
		}
// 		// outlines
// 		int outline_icon_index = -1;
// 		if (isRotable()){
// 		  if(isZoomable())
// 		    outline_icon_index = 3;
// 		  else
// 		    outline_icon_index = 2;
// 		}
// 		else{
// 		  if(isZoomable())
// 		    outline_icon_index = 2;
// 		  else
// 		    outline_icon_index = 1;
// 		}
// 		if (toolbarSelMenu == 3 && getOutlineType() && toolbarSelMenuItem == outline_icon_index ){
// 		  return BK_CMD_INVOKE_OUTLINES;
// 		}

	}

	// main menu
	if (b[BKUser::controls.showMainMenu] == 1) {
		return BK_CMD_INVOKE_MENU;
	}

	// view
	if (b[BKUser::controls.showToolbar] == 1) {
		mode = BKDOC_VIEW;
		return BK_CMD_MARK_DIRTY;
	}

	if (b[BKUser::controls.menuLTrigger] == 1 || b[BKUser::controls.menuLTrigger] > 10) {
	  title_skippix -= 10;
	  if (title_skippix < 0)
	    title_skippix = 0;
	}
	if (b[BKUser::controls.menuRTrigger] == 1 || b[BKUser::controls.menuRTrigger] > 10) {
	  title_skippix += 10;
	  if(title_skippix_max>=0&&title_skippix_max<title_skippix)
	    title_skippix = title_skippix_max;
	}

	return 0;
}

void BKDocument::render() {
	// content
	renderContent();

	// thumbnail for position in current page
	if (thumbnailFrames > 0 && tn_pagew > 0 ) {
	    int init_alpha = 0x80;
		int alpha = init_alpha;
		if (thumbnailFrames <= 32) {
			alpha = (thumbnailFrames - 1) * init_alpha / 32;
		}

		if (alpha > 0){
		    texUI->bindForDisplay();
		    FZScreen::ambientColor(0x00FFFFFF & BKUser::options.thumbnailColorSchemes[BKUser::options.currentThumbnailScheme].txtBGColor | (alpha << 24));
                    drawRect(480 - 5 - tn_pagew, 5, tn_pagew, tn_pageh, 6, 31, 1);

		    FZScreen::ambientColor(0x00FFFFFF & BKUser::options.thumbnailColorSchemes[BKUser::options.currentThumbnailScheme].txtFGColor | (alpha << 24));
//		    drawRect(480 - 5 - tn_pagew + tn_screenx, 5+tn_screeny, tn_screenw, tn_screenh, 6, 31, 1);
		    
		    int tn_border = 2;
		    drawRect(480 - 5 - tn_pagew + tn_screenx, 5+tn_screeny, tn_screenw, tn_border, 6, 31, 1);
		    drawRect(480 - 5 - tn_pagew + tn_screenx, 5+tn_screeny + tn_border, tn_border, tn_screenh-2 * tn_border, 6, 31, 1);
		    drawRect(480 - 5 - tn_pagew + tn_screenx, 5+tn_screeny + tn_screenh - tn_border, tn_screenw, tn_border, 6, 31, 1);
		    drawRect(480 - 5 - tn_pagew + tn_screenx+ tn_screenw - tn_border, 5+tn_screeny + tn_border, tn_border, tn_screenh-2 * tn_border, 6, 31, 1);
	   
		}
	    
	}

	// flash tip for menu/toolbar on load
	if (tipFrames > 0 && mode != BKDOC_TOOLBAR) {
		int alpha = 0xff;
		if (tipFrames <= 32) {
			alpha = tipFrames*(256/32) - 8;
		}
		if (alpha > 0) {
			texUI->bindForDisplay();
			FZScreen::ambientColor(0x222222 | (alpha << 24));
			drawPill(480 - 37 - 37 - 8, 272 - 18 - 4,
				37*2 + 2, 50,
				6,
				31, 1);
			FZScreen::ambientColor(0xffffff | (alpha << 24));
			drawImage(480 - 37 - 2, 272 - 18,
				37, 18,
				75, 60);
			drawImage(480 - 37 - 2 - 2 - 37, 272 - 18,
				37, 18,
				75, 39);
		}
	}

	// label
	if (bannerFrames > 0 && BKUser::options.displayLabels) {
		int y = mode == BKDOC_TOOLBAR ? 10 : 240;
		int alpha = 0xff;
		if (bannerFrames <= 32) {
			alpha = bannerFrames*(256/32) - 8;
		}
		if (alpha > 0) {
			texUI->bindForDisplay();
			FZScreen::ambientColor(0x222222 | (alpha << 24));
			drawPill(150, y, 180, 20, 6, 31, 1);
			fontBig->bindForDisplay();
			FZScreen::ambientColor(0xffffff | (alpha << 24));
			drawTextHC((char*)banner.c_str(), fontBig, y + 4);
		}
	}

	if (mode != BKDOC_TOOLBAR)
		return;

	// all of the icons menus must have at least one item

	// wrap menu indexes
	if (toolbarSelMenu >= 4)
		toolbarSelMenu = 0;
	if (toolbarSelMenu < 0)
		toolbarSelMenu = 3;
	if (toolbarSelMenuItem >= (int)toolbarMenus[toolbarSelMenu].size())
		toolbarSelMenuItem = 0;
	if (toolbarSelMenuItem < 0)
		toolbarSelMenuItem = toolbarMenus[toolbarSelMenu].size() - 1;

	const ToolbarItem& it = toolbarMenus[toolbarSelMenu][toolbarSelMenuItem];

	// background
	texUI->bindForDisplay();
	FZScreen::ambientColor(0xf0222222);
	drawTPill(20, 272 - 75, 480 - 46, 272, 6, 31, 1);

	// context label
	FZScreen::ambientColor(0xff555555);
	//drawTPill(25, 272 - 40, 480 - 46 - 11, 40, 6, 31, 1);
	drawTPill(25, 272 - 30, 480 - 46 - 11, 30, 6, 31, 1);

	// selected column - decide if it overflows
	int ts = toolbarMenus[toolbarSelMenu].size();
	int init = 0;
	bool overflow = false;
	int cs = ts;
	if (ts > 5) {		// overflow mode
		overflow = true;
		init = toolbarSelMenuItem - 4;
		if (init < 0)
			init = 0;
		ts = 5 + init;
		cs = 5;
	}
	// highlight icon column
	FZScreen::ambientColor(0xf0555555);
	drawPill(
		25 + toolbarSelMenu*55,
		272 - 156 - cs*35+70,
		40,
		cs*35+45,
		6,
		31, 1
	);

	// selected icon item row
	FZScreen::ambientColor(0xf0cccccc);
	int iw = textW((char*)toolbarMenus[toolbarSelMenu][toolbarSelMenuItem].label.c_str(), fontBig);
	int mw = toolbarMenus[toolbarSelMenu][toolbarSelMenuItem].minWidth;
	if (iw < mw)
		iw = mw;
	int selItemI = overflow ?
		toolbarSelMenuItem > 4 ? 4 : toolbarSelMenuItem
		: toolbarSelMenuItem;
	drawPill(
		30 + toolbarSelMenu*55,
		272 - 156 - selItemI*35+40,
		iw + 10 + 35,
		30,
		6, 31, 1);

	// button icons
	FZScreen::ambientColor(0xffcccccc);
	int tw = textW((char*)it.circleLabel.c_str(), fontBig);
	//int tw1 = tw;
	if (it.circleLabel.size() > 0) {
	    if (BKUser::controls.select == FZ_REPS_CROSS)
		drawImage(480 - tw - 65, 248, BK_IMG_CROSS_XSIZE, BK_IMG_CROSS_YSIZE, BK_IMG_CROSS_X, BK_IMG_CROSS_Y);
	    else 
		drawImage(480 - tw - 65, 248, BK_IMG_CIRCLE_XSIZE, BK_IMG_CIRCLE_YSIZE, BK_IMG_CIRCLE_X, BK_IMG_CIRCLE_Y);
	}
	if (it.triangleLabel.size() > 0) {
		drawImage(37, 248, BK_IMG_TRIANGLE_XSIZE, BK_IMG_TRIANGLE_YSIZE, BK_IMG_TRIANGLE_X, BK_IMG_TRIANGLE_Y);
	}

	// icon bar
	texUI2->bindForDisplay();
	FZScreen::ambientColor(0xffffffff);

	// menu row
	drawImage(38 + 0*55, 205, 18, 26, 0, 0);
	drawImage(38 + 1*55, 205, 18, 26, 19, 53);
	drawImage(38 + 2*55, 205, 18, 26, 38, 53);
	drawImage(38 + 3*55, 205, 19, 26, 19, 79);
	// selected column
	for (int i = init, j = 0; i < ts; i++, j++) {
		const ToolbarItem& it2 = toolbarMenus[toolbarSelMenu][i];
		if (i == toolbarSelMenuItem)
			FZScreen::ambientColor(0xff000000);
		else
			FZScreen::ambientColor(0xffffffff);
		if (it2.iconW > 0)
			drawImage(
				40 + toolbarSelMenu*55,
				272 - 156 - j*35+45,
				it2.iconW, it2.iconH, it2.iconX, it2.iconY);
	}

	fontBig->bindForDisplay();
	// item label for selected item
	FZScreen::ambientColor(0xff000000);
	drawText((char*)it.label.c_str(), fontBig, 40 + toolbarSelMenu*55 + 35, 272 - 156 - selItemI*35+48);

	// button labels
	FZScreen::ambientColor(0xffcccccc);
	if (it.triangleLabel.size() > 0) {
		drawText((char*)it.triangleLabel.c_str(), fontBig, 37 + 25, 248);
	}
	if (it.circleLabel.size() > 0) {
		drawText((char*)it.circleLabel.c_str(), fontBig, 480 - tw - 40, 248);
	}

	// overflow indicators
	if (overflow) {
		FZScreen::ambientColor(0xffffffff);
		drawText("...", fontBig, 43 + toolbarSelMenu*55, 0);
		drawText("...", fontBig, 43 + toolbarSelMenu*55, 272 - 92);
	}

	
	string t;
	if (isPaginated()) {
		char tp[256];
		snprintf(tp, 256, "P: %d of %d", getCurrentPage(), getTotalPages());
		t = tp;
	}
	drawClockAndBattery(t);

        //draw fast image status
	int xfi = 480 - 30 - textW((char*)t.c_str(),fontSmall);
	string tfi = "FI: ";
	switch (getFastImageStatus()){
	case 3:
	  tfi += "full";
	  break;
	case 2:
	  tfi += "4 scrns";
	  break;
	case 1:
	  tfi += "1 scrn";
	  break;
	default:
	  tfi += "disabled";
	  break;
	}
	xfi -= 10+textW((char*)tfi.c_str(), fontSmall);
	drawText((char*)tfi.c_str(), fontSmall, xfi, 205);

	// draw zoom ratio
	string tz = "Z: ";
	if(isZoomable()){
	  float f = getCurrentZoom();
	  char zoom_value[7] = {'\0','\0','\0','\0','\0','\0','\0'};
	  snprintf(zoom_value,7,"%2.3gx",f);
	  tz += zoom_value;
	}
	else{
	  tz+="N/A";
	}
	xfi -= 10+ textW((char*)tz.c_str(), fontSmall);
	drawText((char*)tz.c_str(), fontSmall, xfi, 205);

	int titleLeftPos = 130;
	int titleYPos = 250;
	int titleRightPos = 330;
	int tooLong;

	if(isZoomable()){
	  titleLeftPos += 20;
	  // draw xpos mode icon
	  texUI2->bindForDisplay();
	  FZScreen::ambientColor(0xffffffff);
	  if (xpos_mode)
	    drawImage(titleLeftPos-20 ,titleYPos,15,15,68,110);
	  else
	    drawImage(titleLeftPos-20 ,titleYPos,15,15,48,110);
	}
	// draw title
	FZFont* titleFont = fontUTF;
	if (!titleFont)
	  titleFont = fontBig;
	FZScreen::ambientColor(0xffffffff);
	if (title_skippix){
	  texUI->bindForDisplay();
	  drawImage(titleLeftPos,titleYPos,12,12,7,112);
	  tooLong = drawUTFMenuItem(miTitle, titleFont, titleLeftPos+14, titleYPos, title_skippix+14, titleRightPos - titleLeftPos - 14);
	}
	else{
	  tooLong = drawUTFMenuItem(miTitle,titleFont,titleLeftPos,titleYPos,0,titleRightPos - titleLeftPos);
	}
	titleFont->doneUTFFont();
	if (tooLong){
	  texUI->bindForDisplay();
	  drawImage(titleRightPos+2,titleYPos,12,12,7,112);
	}
	else{
	  title_skippix_max = title_skippix;
	}

}

void BKDocument::setMode(int m){
  mode = m;
}
int BKDocument::getMode(){
  return mode;
}

int BKDocument::getFastImageStatus(){
  return false;
}

float BKDocument::getCurrentZoom(){
  return 0.0f;
}

void BKDocument::setLongFileName(string s){
  longFileName = s;
}

int BKDocument::getOutlineType(){
  return OUTLINE_NONE;
}

void* BKDocument::getOutlines(){
  return (void*)0;
}
void BKDocument::gotoOutline(void* o, bool ignoreZoom){

}
void BKDocument::getOutlineSelection(int& top, int& sel){
  top = outline_topItem;
  sel = outline_selItem;
}
void BKDocument::setOutlineSelection(int top, int sel){
  outline_topItem = top;
  outline_selItem = sel;
}
