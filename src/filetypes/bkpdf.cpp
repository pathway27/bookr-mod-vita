/*
 * Bookr % VITA: document reader for the Sony PS Vita
 * Copyright (C) 2017 Sreekara C. (pathway27 at gmail dot com)
 *
 * IS A MODIFICATION OF THE ORIGINAL
 *
 * Bookr and bookr-mod for PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com),
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com),

 * AND VARIOUS OTHER FORKS.
 * See Forks in the README for more info
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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


#ifdef MAC
	static void* memalign(int t, int s) {
		return malloc(s);
	}
#else
	#include <malloc.h>
#endif

#include "../bkbookmark.h"
#include "bkpdf.h"
#include "../utils.c"

extern "C" {
	#include <mupdf/fitz.h>
}

static const float zoomLevels[] = { 0.1f, 0.15f, 0.2f, 0.25f, 0.3f, 0.35f, 0.4f, 0.45f, 0.5f, 0.6f, 0.7f, 0.8f, 0.90f, 1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f,
	1.6f, 1.7f, 1.8f, 1.9f, 2.0f, 2.25f, 2.5f, 2.75f, 3.0f, 3.5f, 4.0f, 5.0f, 7.5f, 10.0f, 16.0f };

static const float rotateLevels[] = { 0.0f, 90.0f, 180.0f, 270.0f };

// singleton
static unsigned int* bounceBuffer = NULL;
//static unsigned int* backBuffer = NULL;
static fz_pixmap* fullPageBuffer = NULL;
static fz_pixmap* cachePageBuffer = NULL;

struct PDFContext {
		// remove these
		void *xref;
		void *outline;
		void *pages;

		// int rotate;
		// fz_pixmap *image;

		int pageno;
		void *page;
		float zoom;
		int zoomLevel;
		float rotate;
		int rotateLevel;

		void *rast;
};

static int pdfInit() {

	if (bounceBuffer == NULL) {
		bounceBuffer = (unsigned int*)memalign(16, 480*272*4);
	}
	//	if (backBuffer == NULL) {
	//		backBuffer = (unsigned int*)memalign(16, 480*272*4);
	//	}
	return 0;
}

static PDFContext* pdfOpen(char *filename) {
	PDFContext* ctx = new PDFContext();
	memset(ctx, 0, sizeof(PDFContext));
	return ctx;
}

static void pdfClose(PDFContext* ctx);


int BKPDF::pdfReopenFile(){
    return 0;
}


static char lastPageError[1024];

BKPDF::BKPDF(string& f) : ctx(0), fileName(f), panX(0), panY(0), neg_panX(-1), neg_panY(-1),loadNewPage(false), pageError(false) {
    #ifdef DEBUG
      printf("new BKPDF::BKPDF");
    #endif
    resetPanXY = false;
    retryLoadPageWhenError = false;
		ctitle = NULL;


		// m_page_text = nullptr;
		m_curPageLoaded = false;
		m_page_number = 0;
		m_fitWidth = true;
		m_scale = 1.0f;
		m_width = FZ_SCREEN_WIDTH;
		m_height = FZ_SCREEN_HEIGHT;
    m_rotate = 0.0f;

  	m_ctx = fz_new_context(nullptr, nullptr, FZ_STORE_UNLIMITED);
  	if (m_ctx) {
  			fz_register_document_handlers(m_ctx);
  	} else {
				printf("MuPDF context allocation problem");
		}

		fz_try(m_ctx) {
				m_doc = fz_open_document(m_ctx, f.c_str());
				if (fz_needs_password(m_ctx, m_doc)) {
						int okay = 0;
						char *password;
						char defaultValue = '\0';
						// input for password
						if (!okay)
							fz_throw(m_ctx, FZ_ERROR_GENERIC, "no pass");
				}
		} fz_catch(m_ctx) {
				fz_throw(m_ctx, FZ_ERROR_GENERIC, "opening error");
		}

		fz_try(m_ctx) {
				m_page_count = fz_count_pages(m_ctx, m_doc);
		} fz_catch(m_ctx) {
				fz_throw(m_ctx, FZ_ERROR_GENERIC, "page_count error");
		}



}

static BKPDF* singleton = 0;
//static void bkfree_all();
BKPDF::~BKPDF() {
    singleton = 0;
		fz_drop_pixmap(m_ctx, m_pix);
		fz_drop_document(m_ctx, m_doc);
		fz_drop_context(m_ctx);
}

static bool lastScrollFlag = false;
BKPDF* BKPDF::create(string& file) {
  #ifdef DEBUG
    printf("BKPDF::create");
  #endif
	if (singleton != 0) {
		printf("cannot open more than 1 pdf at the same time\n");
		return singleton;
	}

	//reset_allocs();
	BKPDF* b = new BKPDF(file);
	singleton = b;
	/*
		b->longFileName = longFileName;
		b->xpos_mode = 0;
		//	fz_setmemorycontext(&bkmem);
		fz_cpudetect();
		fz_accelerate();

		pdfInit(); // init bouncebuffer
		PDFContext* ctx = pdfOpen((char*)file.c_str());
		if (ctx == 0) {
			delete b;
			return 0;
		}
		if ((int)ctx == -1) {
		  delete b;
		  return (BKPDF*) -1;
		}
		b->ctx = ctx;
		int lastSlash = -1;
		int n = b->longFileName.size();
		for (int i = 0; i < n; ++i) {
			if (b->longFileName[i] == '\\')
				lastSlash = i;
			else if (b->longFileName[i] == '/')
				lastSlash = i;
		}
		b->title.assign(b->longFileName, lastSlash+1, n - 1 - lastSlash);
		b->ctitle = NULL;
		if (ctx->xref->info) {
			fz_error error;
			fz_obj *obj;
			obj = fz_dictgets(ctx->xref->info, "Title");
			if (obj) {
				error = pdf_toutf8(&b->ctitle, obj);
				if (error == NULL) {
					b->title = b->ctitle;
				}
			}
		}
		// Add bookmark support
		//int position = BKBookmark::getLastView(b->filePath);
		//b->setPage(position);


		//load from bookmark.xml if available
		if (b->isBookmarkable()){
		    BKBookmark bm;
		    string fn;
		    b->getFileName(fn);
		    if (BKBookmarksManager::getLastView(fn, bm)) {
		      b->setBookmarkPosition(bm.viewData);
		      b->setZoom(bm.zoom);
		    }
		}

	 	b->pageError = pdfLoadPage(ctx) != 0;
	 	if (!b->pageError){
	 	    b->retryLoadPageWhenError = true;
	 	}
	  else if(b->retryLoadPageWhenError){
		  b->pdfReopenFile();
		  b->pageError = pdfLoadPage(ctx) != 0;
		  if (!b->pageError){
	 	    b->retryLoadPageWhenError = true;
		  }
		  else{
		    delete b;
		    return NULL;
		  }
		}
		// 	else{
		// 	  delete b;
		// 	  return NULL;
		// 	}
		//print_allocs();
	 	FZScreen::resetReps();
		lastScrollFlag = BKUser::options.pdfFastScroll;
		b->loadNewPage = false;
		char t[256];
		snprintf(t, 256, "Page %d of %d", ctx->pageno, pdf_getpagecount(ctx->pages));
		b->setBanner(t);
	*/
	b->redrawBuffer(false);
	return b;
}

static void pdfClose(PDFContext* ctx) {
}

void BKPDF::clipCoords(float& nx, float& ny) {
	return;
}

bool BKPDF::isZoomable() {
	return true;
}

void BKPDF::getZoomLevels(vector<BKDocument::ZoomLevel>& v) {
  //int n = BKUser::options.pdfFastScroll ? 15 : sizeof(zoomLevels)/sizeof(float);
	for (int i = 0; i < sizeof(zoomLevels)/sizeof(float); ++i){
	  if(BKUser::options.pdfFastScroll && zoomLevels[i]>2.01)
	    break;
	  v.push_back(BKDocument::ZoomLevel(BKDOCUMENT_ZOOMTYPE_ABSOLUTE, "FIX ZOOM LABELS"));
	}
}

void BKPDF::gotoOutline(void *o, bool ignoreZoom) {
	return;
}

int BKPDF::getCurrentZoomLevel() {
	int zl = ctx->zoomLevel;
	// 	if(zoomLevels[zl] != ctx->zoom){
	// 	  zl = 0;
	// 	  while (zoomLevels[zl]<ctx->zoom){
	// 	    zl++;
	// 	  }
	// 	}
	return zl;
}

int BKPDF::setZoomLevel2(int z){
	if (z == ctx->zoomLevel)
		return 0;
	int n = sizeof(zoomLevels)/sizeof(float);
	ctx->zoomLevel = z;

	if (ctx->zoomLevel < 0)
		ctx->zoomLevel = 0;
	if (ctx->zoomLevel >= n)
		ctx->zoomLevel = n - 1;
  	ctx->zoom = zoomLevels[ctx->zoomLevel];
	return ctx->zoomLevel;
}

int BKPDF::setZoomLevel(int z) {
	return 0;
}


bool BKPDF::hasZoomToFit() {
	return true;
}

int BKPDF::setZoomToFitWidth() {
	return 0;
}

int BKPDF::setZoomToFitHeight() {
	return 0;
}

int BKPDF::setZoomIn(int left, int right) {
	return 0;
}

bool BKPDF::isBookmarkable() {
	return true;
}

void BKPDF::getBookmarkPosition(map<string, float>& m) {
	return;
}

int BKPDF::setBookmarkPosition(map<string, float>& m) {
	return 0;
}

bool BKPDF::isPaginated() {
	return true;
}

int BKPDF::getTotalPages() {
	return 0;
}

int BKPDF::getCurrentPage() {
	return 0;
  //ctx->pageno;
}

int BKPDF::setCurrentPage(int position) {
	return 0;
}

bool BKPDF::isRotable() {
	return true;
}

int BKPDF::getRotation() {
	return ctx->rotateLevel;
}

int BKPDF::setRotation2(int z, bool bForce) {
	return 0;
}

int BKPDF::setRotation(int z, bool bForce) {
	return 0;
}

void BKPDF::getTitle(string& s) {
  return;

}

void BKPDF::getType(string& s) {
	s = "PDF";
}

void BKPDF::getFileName(string& s) {
	s = fileName;
}

int BKPDF::prePan(int x, int y) {
	float nx = float(panX + x);
	float ny = float(panY + y);
	clipCoords(nx, ny);
	if (panX == int(nx) && panY == int(ny))
		return 0;
	panBuffer(int(nx), int(ny));
	return BK_CMD_MARK_DIRTY;
}

int BKPDF::pan(int x, int y) {

  int minValueX = 32 * BKUser::options.analogRateX / 100;
  int minValueY = 32 * BKUser::options.analogRateY / 100;
	if (x > -1 * minValueX && x < minValueX)
		x = 0;
	if (y > -1 * minValueY && y < minValueY)
		y = 0;
	if (x == 0 && y == 0)
		return 0;
	x >>= 2;
	y >>= 2;
	return prePan(x, y);
}

int BKPDF::screenUp() {
    return prePan(0, -1 * BKUser::options.vScroll);
}

int BKPDF::screenDown() {
    return prePan(0, BKUser::options.vScroll);
}

int BKPDF::screenLeft() {
    return prePan(-1 * BKUser::options.hScroll, 0);
}

int BKPDF::screenRight() {
    return prePan(BKUser::options.hScroll, 0);
}

static vita2d_texture *texture;
void BKPDF::renderContent() {
  printf("BKPDF::renderContent\n");

  FZScreen::clear(0xefefef, FZ_COLOR_BUFFER);
  FZScreen::color(0xffffffff);
  // FZScreen::matricesFor2D(rotation);
  FZScreen::enable(FZ_TEXTURE_2D);
  // FZScreen::enable(FZ_BLEND);
  FZScreen::blendFunc(FZ_ADD, FZ_SRC_ALPHA, FZ_ONE_MINUS_SRC_ALPHA);

  // char text[100];
  // sprintf(text, "Hi from PDF::RenderCOntent %d/%d", m_page_number, m_page_count);
  // FZScreen::drawText(100, 100, RGBA8(0,0,0,255), 1.0f, text);
  //
  // sprintf(text, "Pixmap info h:%d w:%d", m_pix->h, m_pix->w);
  // FZScreen::drawText(100, 200, RGBA8(0,0,0,255), 1.0f, text);
	// FZScreen::drawPixel(400, 400, RGBA8(0, 0, 0, 255));
  // FZScreen::drawPixel(400, 401, RGBA8(0, 0, 0, 255));
  // FZScreen::drawPixel(400, 402, RGBA8(0, 0, 0, 255));
  // FZScreen::drawPixel(400, 403, RGBA8(0, 0, 0, 255));
  // FZScreen::drawPixel(400, 404, RGBA8(0, 0, 0, 255));
  // FZScreen::drawPixel(400, 405, RGBA8(0, 0, 0, 255));

	// TODO: convert to texture
  printf("BKPDF::renderContent -- pre pixel\n");
  vita2d_draw_texture(texture, 0, 0);
  printf("BKPDF::renderContent -- post pixel\n");

	//FZScreen::copyImage(FZ_PSM_8888, 0, 0, 480, 272, 480, bounceBuffer, 0, 0, 512, (void*)(0x04000000+(unsigned int)FZScreen::framebuffer()));

	// if (pageError) {
	// 	texUI->bindForDisplay();
	// 	FZScreen::ambientColor(0xf06060ff);
	// 	drawRect(0, 126, 480, 26, 6, 31, 1);
	// 	fontBig->bindForDisplay();
	// 	FZScreen::ambientColor(0xff222222);
	// 	FZScreen::drawText(380, 400, RGBA8(0,0,0,255), 1.0f, "Error in page...");
	// }

	#ifdef PSP
		//int tfm = sceKernelTotalFreeMemSize();
		//int mfm = sceKernelMaxFreeMemSize();
		//printf("tfm %d, mfm %d\n", tfm, mfm);
	#endif
}

static inline void bk_memcpysr8(void* to, void* from, unsigned int n) {
	int* s = (int*)from;
	int* d = (int*)to;
	n >>= 2;
	for (unsigned int i = 0; i < n; ++i) {
		*d = *s >> 8;
		++s;
		++d;
	}
}

#ifdef PSP
extern "C" {
	extern int bk_memcpy(void*, void*, unsigned int);
	// not working...
	//extern int bk_memcpysr8(void*, void*, unsigned int);
};
#else
#define bk_memcpy memcpy
#endif

bool BKPDF::redrawBuffer(bool setSpeed) {
  printf("BKPDF::redrawBuffer\n");
  // if (m_pix != 0)
  //  fz_drop_pixmap(m_ctx, m_pix);
	// fz_drop_text_page(m_ctx, m_pageText;)
	// m_pageText = nullptr;

	// if (!m_curPageLoaded) {
	// 		fz_drop_page(m_ctx, m_page);
	// 		m_page = fz_load_page(m_ctx, m_doc, m_page_number);
	// 		m_curPageLoaded = true;
	// }
  //
	// fz_bound_page(m_ctx, m_page, &m_bounds);
	// if (m_fitWidth) {
	// 		m_scale = m_width / (m_bounds.x1 - m_bounds.x0);
	// }

	fz_scale(&m_transform, m_scale, m_scale);
  fz_pre_rotate(&m_transform, m_rotate);

	//fixBounds();

	fz_try(m_ctx)
		m_pix = fz_new_pixmap_from_page_number(m_ctx, m_doc, m_page_number, &m_transform, fz_device_rgb(m_ctx), 0);
	fz_catch(m_ctx) {
		printf("cannot render page: %s\n", fz_caught_message(m_ctx));
	}

  texture = _vita2d_load_pixmap_generic(m_pix);

  printf("BKPDF::redrawBuffer - end\n");
	return true;
}

bool BKPDF::redrawBufferIncremental(int newx, int newy, bool setSpeed) {
	return true;
}


void BKPDF::panBuffer(int nx, int ny) {
	return;
}

int BKPDF::resume() {
	// mupdf leaves open file descriptors around. they don't survive a suspend.
	//pdfReopenFile();
	return 0;
}

int BKPDF::updateContent() {
	return 0;
}

bool BKPDF::isPDF(string& file) {
	char header[4];
	memset((void*)header, 0, 4);
	FILE* f = fopen(file.c_str(), "r");
	if( !f )
		return false;
	fread(header, 4, 1, f);
	fclose(f);
	return header[0] == 0x25 && header[1] == 0x50 && header[2] == 0x44 && header[3] == 0x46;
}

int BKPDF::getFastImageStatus(){
	return 0;
}

float BKPDF::getCurrentZoom(){
	return 0.0f;
}

void BKPDF::setZoom(float z){
	return;
}

int
BKPDF::getOutlineType(){
  return 0;
}

void*
BKPDF::getOutlines(){
  return ctx?ctx->outline:0;
}


static void setXY(int rotateLevel, float x, float y, float zoom, int& lpanX, int& lpanY, int& lneg_panX, int& lneg_panY){
  switch (rotateLevel){
    case 0: //up
      if(x>=0)
	lpanX = x* zoom;
      if(y>=0)
	lneg_panY = y* zoom;
      break;
    case 1: //right
      if(x>=0)
	lpanY = x* zoom;
      if(y>=0)
	lpanX = y* zoom - 480;
      break;
    case 2: //down
      if(x>=0)
	lneg_panX = x* zoom+480;
      if(y>=0)
	lpanY = y* zoom-272;

      break;
    case 3: //left
      if(x>=0)
	lneg_panY = x* zoom+272;
      if(y>=0)
	lneg_panX = y* zoom;
      break;
    }
}
