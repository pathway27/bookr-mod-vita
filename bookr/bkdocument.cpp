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
	if (isPDF(filePath)) {
		return BKPDF::create(filePath);
	} else {
		//return BKPlainText::create(filePath);
	}
	return 0;
}

BKDocument::BKDocument() {
	lastSuspendSerial = FZScreen::getSuspendSerial();
}

BKDocument::~BKDocument() {
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

	return processEventsForView();
	//processEventsForToolbar(); ?
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
		setZoomLevel(z);
	}
	virtual void setZoomLevel(int) = 0;

	// Analog pad paning - can be ignored
	virtual void pan(int, int) = 0;

	// Rotation (0 = 0deg, 1 = 90deg, 2 = 180deg, 3 = 240deg)
	virtual bool isRotable() = 0;
	virtual int getRotation() = 0;
	virtual void setRotation(int) = 0;

	// Bookmark support. The returned map is a black box
	// for the bookmarking system.
	virtual bool isBookmarkable() = 0;
	virtual void getBookmarkPosition(map<string, int>&) = 0;
	virtual void setBookmarkPosition(const map<string, int>&) = 0;


	float nx = panX;
	float ny = panY;
	bool fullRedraw = false;
	// pan
	if (b[BKUser::pdfControls.panUp] == 1 || b[BKUser::pdfControls.panUp] > 20) {
		ny -= 16.0f;
	}
	if (b[BKUser::pdfControls.panDown] == 1 || b[BKUser::pdfControls.panDown] > 20) {
		ny += 16.0f;
	}
	if (b[BKUser::pdfControls.panLeft] == 1 || b[BKUser::pdfControls.panLeft] > 20) {
		nx -= 16.0f;
	}
	if (b[BKUser::pdfControls.panRight] == 1 || b[BKUser::pdfControls.panRight] > 20) {
		nx += 16.0f;
	}
	// zoom
	if (b[BKUser::pdfControls.zoomOut] == 1) {
		--ctx->zoomLevel;
		if (ctx->zoomLevel < 0)
			ctx->zoomLevel = 0;
		if (BKUser::options.pdfFastScroll && ctx->zoomLevel > 14) {
			ctx->zoomLevel = 14;
			ctx->zoom = 2.0f;
		}
		ctx->zoom = zoomLevels[ctx->zoomLevel];
		fullRedraw = true;
		nx *= ctx->zoom;
		ny *= ctx->zoom;
		char t[256];
		snprintf(t, 256, "Zoom %2.3gx", ctx->zoom);
		banner = t;
		bannerFrames = 60;
	}
	if (b[BKUser::pdfControls.zoomIn] == 1) {
		++ctx->zoomLevel;
		int n = sizeof(zoomLevels)/sizeof(float);
		if (ctx->zoomLevel >= n)
			ctx->zoomLevel = n - 1;
		if (BKUser::options.pdfFastScroll && ctx->zoomLevel > 14) {
			ctx->zoomLevel = 14;
			ctx->zoom = 2.0f;
		}
		ctx->zoom = zoomLevels[ctx->zoomLevel];
		fullRedraw = true;
		nx *= ctx->zoom;
		ny *= ctx->zoom;
		char t[256];
		snprintf(t, 256, "Zoom %2.3gx", ctx->zoom);
		banner = t;
		bannerFrames = 60;
	}
	// clip coords
	if (!pageError) {
		fz_matrix ctm = pdfViewctm(ctx);
		fz_rect bbox = ctx->page->mediabox;
		bbox = fz_transformaabb(ctm, bbox);
		if (ny < 0.0f) {
			ny = 0.0f;
		}
		float h = bbox.y1 - bbox.y0;
		if (ny >= h - 272.0f) {
			ny = h - 273.0f;
		}
		if (nx < 0.0f) {
			nx = 0.0f;
		}
		float w = bbox.x1 - bbox.x0;
		if (nx >= w - 480.0f) {
			nx = w - 481.0f;
		}
#if 0
		if (ny < 0.0f) {
			ny = 0.0f;
		}
		float h = ctx->page->mediabox.y1 - ctx->page->mediabox.y0;
		h *= ctx->zoom;
		if (ny >= h - 272.0f) {
			ny = h - 273.0f;
		}
		if (nx < 0.0f) {
			nx = 0.0f;
		}
		float w = ctx->page->mediabox.x1 - ctx->page->mediabox.x0;
		w *= ctx->zoom;
		if (nx >= w - 480.0f) {
			nx = w - 481.0f;
		}
#endif
	}

	int inx = (int)nx;
	int iny = (int)ny;
	// redraw and/or pan
	if (fullRedraw) {
		panX = inx;
		panY = iny;
		if (BKUser::options.pdfFastScroll) {
			//pdfRenderFullPage(ctx);
			loadNewPage = true;
			return BK_CMD_MARK_DIRTY;
		}
		redrawBuffer();
		return BK_CMD_MARK_DIRTY;
	}
	if (inx != panX || iny != panY) {
		panBuffer(inx, iny);
		return BK_CMD_MARK_DIRTY;
	}

	// main menu
	if (b[FZ_REPS_START] == 1) {
		return BK_CMD_INVOKE_MENU;
	}

	// banner fade
	if (bannerFrames > 0)
		return BK_CMD_MARK_DIRTY;

	// next/prev page
	int oldpage = ctx->pageno;

	if (b[BKUser::pdfControls.nextPage] == 1) {
		ctx->pageno++;
	}
	if (b[BKUser::pdfControls.previousPage] == 1) {
		ctx->pageno--;
	}
	if (b[BKUser::pdfControls.next10Pages] == 1) {
		ctx->pageno+=10;
	}
	if (b[BKUser::pdfControls.previous10Pages] == 1) {
		ctx->pageno-=10;
	}

	return 0;
}


int BKDocument::updateToolbariew() {
}

void BKDocument::render() {
	renderContent();
	// toolbar...
}

