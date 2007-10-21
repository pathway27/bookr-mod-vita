/*
 * bkdjvu: djvu extension for bookr
 * Copyright (C) 2007 Yang.Hu (findreams at gmail dot com)
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
 
#ifdef PSP
#include <pspsysmem.h>
#endif

#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "bkbookmark.h"

#ifdef MAC
static void* memalign(int t, int s) {
	return malloc(s);
}
#else
#include <malloc.h>
#endif

#include "bkdjvu.h"

#include <ddjvuapi.h>
#include <miniexp.h>

static const char* bookrName = "bookr";

static const int cacheSize = 4*1024*1024;		//MB

static const float zoomLevels[] = { 0.1f, 0.15f, 0.2f, 0.25f, 0.3f, 0.35f, 0.4f, 0.5f, 0.75f, 0.90f, 1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f,
	1.6f, 1.7f, 1.8f, 1.9f, 2.0f, 2.25f, 2.5f, 2.75f, 3.0f };

static const ddjvu_page_rotation_t rotateLevels[] = { DDJVU_ROTATE_0, DDJVU_ROTATE_270, DDJVU_ROTATE_180, DDJVU_ROTATE_90 };

static char* bounceBuffer = NULL;

struct DJVUContext {
	/* current document params */
	//string docTitle;
	ddjvu_context_t *context;
	ddjvu_document_t *document;
	ddjvu_page_t *page;
	
	/* current page params */
	int pageno;
	int djvupageno;
	float zoom;
	int zoomLevel;
	ddjvu_page_rotation_t rotate;
	int rotateLevel;
};

static int djvuHandle(ddjvu_context_t* const ctx, int wait)
{
	const ddjvu_message_t *msg;
	int retval = 1;
	if(!ctx) {
		retval = 0;
		return retval;
	}
	if(wait)
		msg = ddjvu_message_wait(ctx);
	while ((msg = ddjvu_message_peek(ctx))){
		switch(msg->m_any.tag){ 
			case DDJVU_ERROR:
				printf("ddjvu error: %s, %s, %s, %d\n", msg->m_error.message, msg->m_error.function, msg->m_error.filename, msg->m_error.lineno);
				retval = 0;
				break;

			default:
				break;
		}
		ddjvu_message_pop(ctx);
	}
	return retval;
}

 static int djvuInit() {
 	if (bounceBuffer == NULL) {
		bounceBuffer = (char*)memalign(16, 480*272*4);
	}
 	return bounceBuffer==NULL?0:1;
 }

static DJVUContext* djvuOpen(char *filename) {
	DJVUContext* ctx = new DJVUContext();
	if(ctx == NULL) {
		return NULL;
	}
	memset(ctx, 0, sizeof(DJVUContext));

	ctx->pageno = 1;
	ctx->djvupageno = 0;
	ctx->zoom = 0.2f;
	ctx->zoomLevel = 2;
	ctx->rotate = DDJVU_ROTATE_0;
	ctx->rotateLevel = 0;

	ctx->context = ddjvu_context_create(bookrName);
	if(ctx->context == NULL) {
		delete ctx;
		return NULL;
	}
	ddjvu_cache_set_size(ctx->context, cacheSize);
	ctx->document = ddjvu_document_create_by_filename(ctx->context, filename, true);
	djvuHandle(ctx->context, TRUE);
	if(ctx->document == NULL) {
		ddjvu_context_release(ctx->context);
		delete ctx;
		return NULL;
	}
	ctx->page = ddjvu_page_create_by_pageno(ctx->document, ctx->djvupageno);	 
	//djvuHandle(ctx->context, TRUE);

	return ctx;
}

static void djvuClose(DJVUContext* ctx) {
	if (ctx->page)
		ddjvu_page_release(ctx->page);
	ctx->page = 0;
	if (ctx->document)
		ddjvu_document_release(ctx->document);
	ctx->document = 0;
	if (ctx->context)
		ddjvu_context_release(ctx->context);
	ctx->context = 0;
}

static void djvuFree() {
	if(bounceBuffer)
		free(bounceBuffer);
	bounceBuffer = 0;
}

BKDJVU::BKDJVU(string& f) : ctx(0), fileName(f), panX(0), panY(0), loadNewPage(false), pageError(false) {
}

static BKDJVU* singleton = 0;

BKDJVU::~BKDJVU() {
	if (ctx != 0) {
		saveLastView();
		djvuClose(ctx);
		delete ctx;
	}
	ctx = 0;
	djvuFree();
	singleton = 0;
}

BKDJVU* BKDJVU::create(string& file) {
	if (singleton != 0) {
		printf("cannot open more than 1 pdf at the same time\n");
		return singleton;
	}
	
	BKDJVU* b = new BKDJVU(file);
	singleton = b;

	if (!djvuInit())
		return 0;

	DJVUContext* ctx = djvuOpen((char*)file.c_str());
	if (ctx == 0) {
		delete b;
		return 0;
	}
	b->ctx = ctx;
	int lastSlash = 0;
	int n = file.size();
	for (int i = 0; i < n; ++i) {
		if (file[i] == '\\')
			lastSlash == i++;
		else if (file[i] == '/')
			lastSlash == i++;
	}
	b->title.assign(file, lastSlash, n - lastSlash);
 	b->redrawBuffer();
	return b;
}

void BKDJVU::clipCoords(float& nx, float& ny) {
		float w = (float)ddjvu_page_get_width(ctx->page) * ctx->zoom;
		float h = (float)ddjvu_page_get_height(ctx->page) * ctx->zoom;
		if (ny < 0.0f) {
			ny = 0.0f;
		}
		if (h <= 272.0f) {
			ny = 0.0f;
		} else if (ny >= h - 272.0f) {
			ny = h - 273.0f;
		}
		if (nx < 0.0f) {
			nx = 0.0f;
		}
		if (w <= 489.0f) {
			nx = 0.0f;
		} else if (nx >= w - 480.0f) {
			nx = w - 481.0f;
		}
}

bool BKDJVU::isZoomable() {
	return true;
}

void BKDJVU::getZoomLevels(vector<BKDocument::ZoomLevel>& v) {
	int n = sizeof(zoomLevels)/sizeof(float);
	for (int i = 0; i < n; ++i)
		v.push_back(BKDocument::ZoomLevel(BKDOCUMENT_ZOOMTYPE_ABSOLUTE, "FIX ZOOM LABELS"));
}

int BKDJVU::getCurrentZoomLevel() {
	return ctx->zoomLevel;
}

int BKDJVU::setZoomLevel(int z) {
	if (z == ctx->zoomLevel)
		return 0;
	int n = sizeof(zoomLevels)/sizeof(float);
	ctx->zoomLevel = z;

	if (ctx->zoomLevel < 0)
		ctx->zoomLevel = 0;
	if (ctx->zoomLevel >= n)
		ctx->zoomLevel = n - 1;

	ctx->zoom = zoomLevels[ctx->zoomLevel];
	float nx = float(panX)*ctx->zoom;
	float ny = float(panY)*ctx->zoom;
	clipCoords(nx, ny);
	panX = int(nx);
	panY = int(ny);
	char t[256];
	snprintf(t, 256, "Zoom %2.3gx", ctx->zoom);
	setBanner(t);
	redrawBuffer();
	return BK_CMD_MARK_DIRTY;
}


bool BKDJVU::hasZoomToFit() {
	return true;
}

int BKDJVU::setZoomToFitWidth() {
	if (ctx->page) {
		ctx->zoom = 480.0f/ddjvu_page_get_width(ctx->page);
	}
	float nx = float(panX)*ctx->zoom;
	float ny = float(panY)*ctx->zoom;
	clipCoords(nx, ny);
	panX = int(nx);
	panY = int(ny);
	char t[256];
	snprintf(t, 256, "Zoom %2.3gx", ctx->zoom);
	setBanner(t);
	redrawBuffer();
	return BK_CMD_MARK_DIRTY;
}

int BKDJVU::setZoomToFitHeight() {

	if (ctx->page) {
		ctx->zoom = 272.0f/ddjvu_page_get_height(ctx->page);
	}
	float nx = float(panX)*ctx->zoom;
	float ny = float(panY)*ctx->zoom;
	clipCoords(nx, ny);
	panX = int(nx);
	panY = int(ny);
	char t[256];
	snprintf(t, 256, "Zoom %2.3gx", ctx->zoom);
	setBanner(t);
	redrawBuffer();
	return BK_CMD_MARK_DIRTY;
}

bool BKDJVU::isBookmarkable() {
	return true;
}

void BKDJVU::getBookmarkPosition(map<string, int>& m) {
	m["page"] = ctx->pageno;
	m["zoom"] = ctx->zoomLevel;
	m["panX"] = panX;
	m["panY"] = panY;
	m["rotation"] = ctx->rotateLevel;
}

int BKDJVU::setBookmarkPosition(map<string, int>& m) {
	setCurrentPage(m["page"]);
	setZoomLevel(m["zoom"]);
	setRotation(m["rotation"]);
	panX = m["panX"];
	panY = m["panY"];
	return BK_CMD_MARK_DIRTY;
}

bool BKDJVU::isPaginated() {
	return true;
}

int BKDJVU::getTotalPages() {
	return ddjvu_document_get_pagenum(ctx->document);
}

int BKDJVU::getCurrentPage() {
	return ctx->pageno;
}

int BKDJVU::setCurrentPage(int position) {
	ctx->djvupageno = ctx->pageno-1; 
	ctx->pageno = position;
	if (ctx->pageno < 1)
		ctx->pageno = 1;
	if (ctx->pageno > getTotalPages())
		ctx->pageno = getTotalPages();
	if (ctx->pageno-1 != ctx->djvupageno) {
		loadNewPage = true;
		char t[256];
		snprintf(t, 256, "Loading page %d", ctx->pageno);
		setBanner(t);
		panX = 0;
		panY = 0;
		return BK_CMD_MARK_DIRTY;
	}
	return 0;
}

bool BKDJVU::isRotable() {
	return true;
}

int BKDJVU::getRotation() {
	return ctx->rotateLevel;
}

int BKDJVU::setRotation(int z, bool bForce) {

	if (z == ctx->rotateLevel)
		return 0;
	int n = 4;
	ctx->rotateLevel = z;

	if (ctx->rotateLevel < 0)
		ctx->rotateLevel = 3;
	if (ctx->rotateLevel >= n)
		ctx->rotateLevel = 0;

	ctx->rotate = rotateLevels[ctx->rotateLevel];
	float nx = float(panX);
	float ny = float(panY);
	clipCoords(nx, ny);
	panX = int(nx);
	panY = int(ny);
	char t[256];
	snprintf(t, 256, "Rotate to %3.3g°", (float)ctx->rotate);
	setBanner(t);

	redrawBuffer();
	return BK_CMD_MARK_DIRTY;
}

void BKDJVU::getTitle(string& s) {
	s = title;
}

void BKDJVU::getType(string& s) {
	s = "DJVU";
}

void BKDJVU::getFileName(string& s) {
	s = fileName;
}

int BKDJVU::prePan(int x, int y) {
	float nx = float(panX + x);
	float ny = float(panY + y);
	clipCoords(nx, ny);
	if (panX == int(nx) && panY == int(ny))
		return 0;
	panBuffer(int(nx), int(ny));
	return BK_CMD_MARK_DIRTY;
}

int BKDJVU::pan(int x, int y) {
	if (x > -32 && x < 32)
		x = 0;
	if (y > -32 && y < 32)
		y = 0;
	if (x == 0 && y == 0)
		return 0;
	x >>= 2;
	y >>= 2;
	return prePan(x, y);
}

int BKDJVU::screenUp() {
	return prePan(0, -150);
}

int BKDJVU::screenDown() {
	return prePan(0, 150);
}

int BKDJVU::screenLeft() {
	return prePan(-250, 0);
}

int BKDJVU::screenRight() {
	return prePan(250, 0);
}

void BKDJVU::renderContent() {
	FZScreen::color(0xffffffff);
	FZScreen::matricesFor2D();
	FZScreen::enable(FZ_TEXTURE_2D);
	FZScreen::enable(FZ_BLEND);
	FZScreen::blendFunc(FZ_ADD, FZ_SRC_ALPHA, FZ_ONE_MINUS_SRC_ALPHA);

	FZScreen::copyImage(FZ_PSM_8888, 0, 0, 480, 272, 480, bounceBuffer, 0, 0, 512, (void*)(0x04000000+(unsigned int)FZScreen::framebuffer()));

#ifdef PSP
	//int tfm = sceKernelTotalFreeMemSize();
	//int mfm = sceKernelMaxFreeMemSize();
	//printf("tfm %d, mfm %d\n", tfm, mfm);
#endif
}

void BKDJVU::redrawBuffer() {
	if (ctx->pageno-1 != ctx->djvupageno) {
		if (ctx->page)
			ddjvu_page_release(ctx->page);
		ctx->djvupageno = ctx->pageno-1;
		ctx->page = ddjvu_page_create_by_pageno(ctx->document, ctx->djvupageno);	
		//djvuHandle(ctx->context, TRUE);
	}
	if(ctx->page == NULL)
		return;
	ddjvu_page_set_rotation(ctx->page, ctx->rotate);
	
	char* bufferStart = bounceBuffer;
	
	ddjvu_rect_t zoomrect;
	zoomrect.x = zoomrect.y = 0;
	zoomrect.w = (unsigned int)((float)ddjvu_page_get_width(ctx->page) * ctx->zoom);
	zoomrect.h = (unsigned int)((float)ddjvu_page_get_height(ctx->page) * ctx->zoom);
	
	ddjvu_rect_t screenrect;
	screenrect.x = panX;
	screenrect.y = panY;
	screenrect.w = 480;
	screenrect.h = 272;
	
	/* while zoomrect is smaller than screenrect */
	if(screenrect.w > zoomrect.w) {
		bufferStart += (screenrect.w - zoomrect.w) / 2 * 4;
		screenrect.w = zoomrect.w;
		screenrect.x = panX = 0;
	}
	if(screenrect.h > zoomrect.h) {
		bufferStart += (screenrect.h - zoomrect.h) / 2 * 480 * 4;
		screenrect.h = zoomrect.h;
		screenrect.y = panY = 0;
	}
	if(bufferStart != bounceBuffer) {
		unsigned int *d = (unsigned int*)bounceBuffer;
		//const unsigned int c = BKUser::options.pdfBGColor;
		const unsigned int c = BKUser::options.colorSchemes[BKUser::options.currentScheme].txtBGColor;
		for (int i = 0; i < 480*272; i++) {
			*d++ = c;
		}
	}
	ddjvu_format_t* format = ddjvu_format_create(DDJVU_FORMAT_RGB32, 0, 0);
	ddjvu_format_set_row_order(format, 1);
	ddjvu_format_set_y_direction(format, 1);
	ddjvu_page_render( ctx->page,
			DDJVU_RENDER_COLOR,
			&zoomrect,
			&screenrect,
			format,
			480*4,
			bufferStart);
	ddjvu_format_release(format);
}

void BKDJVU::panBuffer(int nx, int ny) {
	panX = nx;
	panY = ny;
	redrawBuffer();
}

int BKDJVU::resume() {
	// mupdf leaves open file descriptors around. they don't survive a suspend.
	return BK_CMD_RELOAD;
}

int BKDJVU::updateContent() {
	if (loadNewPage) {
		//pageError = pdfLoadPage(ctx) != 0;
		redrawBuffer();
		loadNewPage = false;
		char t[256];
		snprintf(t, 256, "Page %d of %d", ctx->pageno, getTotalPages());
		setBanner(t);
		return BK_CMD_MARK_DIRTY;
	}

	return 0;
}

bool BKDJVU::isDJVU(string& file) {
	char header[4];
	memset((void*)header, 0, 4);
	FILE* f = fopen(file.c_str(), "r");
	fread(header, 4, 1, f);
	fclose(f);
	return header[0] == 0x41 && header[1] == 0x54 && header[2] == 0x26 && header[3] == 0x54;
}

