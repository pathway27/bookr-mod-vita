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

#include <map>
#include <fstream>
#include <chrono>
#include <iostream>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include <errno.h>

#include <psp2/io/fcntl.h>

extern "C" {
  #include <mupdf/fitz.h>
}

#include "bkmudocument.h"
#include "../bkbookmark.h"
#include "../utils.h"

using namespace std;

// make sure only one document is open, do we need this?
// vita can probably do multiple.
static BKMUDocument* mudoc_singleton = nullptr;
// texture of current pixmap, TODO: generic fztexture
static vita2d_texture *texture;
static const float zoomLevels[] = { 0.25f, 0.5f, 0.75f, 0.90f, 1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f,
  1.6f, 1.7f, 1.8f, 1.9f, 2.0f, 2.25f, 2.5f, 2.75f, 3.0f, 3.5f, 4.0f, 5.0f, 7.5f, 10.0f, 16.0f };

BKMUDocument::BKMUDocument(string& f) : 
  filename(f), m_ctx(nullptr), m_doc(nullptr), m_page(nullptr), loadNewPage(false), zooming(false),
  m_pageText(nullptr), m_links(nullptr), panX(0), panY(0), m_current_page(0),
  m_curPageLoaded(false), m_fitWidth(true), zoomLevel(4)
{
  #ifdef DEBUG
    printf("BKMUDocument::BKMUDocument\n");
  #endif

  m_scale = zoomLevels[zoomLevel];
  m_rotate = 0.0f;
  m_width = FZ_SCREEN_WIDTH;
  m_height = FZ_SCREEN_HEIGHT;

  // Initalize fitz context
  m_ctx = fz_new_context(nullptr, nullptr, FZ_STORE_UNLIMITED);
  if (m_ctx)
    fz_register_document_handlers(m_ctx);
  else
    printf("MuPDF context allocation problem");

  fz_set_use_document_css(m_ctx, 1);

  // Open Document; TODO: Implement keyboard password
  fz_try(m_ctx) {
    m_doc = fz_open_document(m_ctx, f.c_str());
    if (fz_needs_password(m_ctx, m_doc)) {
      int okay = 0;
      char *password;
      char defaultValue;
      // input for password
      if (!okay)
        fz_throw(m_ctx, FZ_ERROR_GENERIC, "no pass");
    }
  } fz_catch(m_ctx) {
    printf("opening error: %s\n", fz_caught_message(m_ctx));
    fz_drop_context(m_ctx);
    fz_throw(m_ctx, FZ_ERROR_GENERIC, "opening error:");
  }

  m_pdf = pdf_specifics(m_ctx, m_doc);

  // Set page count
  fz_try(m_ctx) {
    m_pages = fz_count_pages(m_ctx, m_doc);
  } fz_catch(m_ctx) {
    printf("page_count error");
    fz_throw(m_ctx, FZ_ERROR_GENERIC, "page_count error");
  }

  #ifdef DEBUG
    printf("BKMUDocument::BKMUDocument end\n");
  #endif
}

BKMUDocument::~BKMUDocument() {
  #ifdef DEBUG
    printf("BKMUDocument::~BKMUDocument\n");
  #endif
  
  saveLastView();
  mudoc_singleton = nullptr;
  fz_drop_pixmap(m_ctx, m_pix);
  fz_drop_document(m_ctx, m_doc);
  fz_drop_context(m_ctx);
}

BKMUDocument* BKMUDocument::create(string& file) {
  #ifdef DEBUG
    printf("BKMUDocument::create\n");
  #endif
  
  if (mudoc_singleton) {
    printf("cannot open more than 1 pdf at the same time\n");
    return mudoc_singleton;
  }

  BKMUDocument* b = new BKMUDocument(file);
  mudoc_singleton = b;


  b->redrawBuffer();
  return b;
}

// Draws current page into texture using pixmap
bool BKMUDocument::redrawBuffer() {
  #ifdef DEBUG
    printf("BKMUDocument::redrawBuffer pp\n");
    chrono::steady_clock::time_point begin = chrono::steady_clock::now(); 
  #endif
  // fz_scale(&m_transform, m_scale / 72, m_scale / 72);
  // fz_pre_rotate(&m_transform, m_rotate);
  
  fz_drop_stext_page(m_ctx, m_pageText);
  m_pageText = nullptr;
  fz_drop_link(m_ctx, m_links);
  m_links = nullptr;
  fz_drop_page(m_ctx, m_page);
  m_page = nullptr;


  m_page = fz_load_page(m_ctx, m_doc, m_current_page);
  m_links = fz_load_links(m_ctx, m_page);
  m_pageText = fz_new_stext_page_from_page(m_ctx, m_page, nullptr);


  // bounds for inital window size
  fz_bound_page(m_ctx, m_page, &m_bounds);
  if (m_fitWidth)
    m_scale = m_width / (m_bounds.x1 - m_bounds.x0);

  fz_scale(&m_transform, m_scale, m_scale);
  fz_pre_rotate(&m_transform, m_rotate);
  fz_transform_rect(&m_bounds, &m_transform);

  // TODO: Is display list or bbox better?
  // This is currently the longest operation
  fz_annot *annot;
  fz_try(m_ctx)
    m_pix = fz_new_pixmap_from_page_contents(m_ctx, m_page, &m_transform, fz_device_rgb(m_ctx), 0);
  fz_catch(m_ctx) {
    printf("cannot render page: %s\n", fz_caught_message(m_ctx));
  }

  // Crashes due to GPU memory use without this.
  vita2d_free_texture(texture);
  texture = _vita2d_load_pixmap_generic(m_pix);

  fz_drop_pixmap(m_ctx, m_pix);
  // load annotations
  
  return true;
}

int BKMUDocument::updateContent() {
  if (loadNewPage) {
    panY = 0;
    redrawBuffer();
    saveLastView();

    loadNewPage = false;
    char t[256];

    snprintf(t, 256, "Page %d of %d", m_current_page + 1, m_pages);
    setBanner(t);
    
    return BK_CMD_MARK_DIRTY;
  }
  return 0;
}

int BKMUDocument::resume() {
  // mupdf leaves open file descriptors around. they don't survive a suspend.
  // Is this still the case?
  // pdfReopenFile();
  return 0;
}

void BKMUDocument::renderContent() {
  #ifdef DEBUG_RENDER
    printf("BKMUDocument::renderContent %i / %i\n", m_current_page, m_pages);
  #endif

  FZScreen::clear(0xefefef, FZ_COLOR_BUFFER);
  vita2d_draw_texture(texture, panX, panY);

  // TODO: Show Page Error, don"t draw texture then.
  // if (pageError) {
  // 	texUI->bindForDisplay();
  // 	FZScreen::ambientColor(0xf06060ff);
  // 	drawRect(0, 126, 480, 26, 6, 31, 1);
  // 	fontBig->bindForDisplay();
  // 	FZScreen::ambientColor(0xff222222);
  // 	FZScreen::drawText(380, 400, RGBA8(0,0,0,255), 1.0f, "Error in page...");
  // }
}

int BKMUDocument::getTotalPages() {
  return m_pages;
}

int BKMUDocument::getCurrentPage() {
  return m_current_page;
}

int BKMUDocument::setCurrentPage(int page_number) {
  if (page_number < 0 || page_number >= m_pages)
    // TODO(UI): Some visual notice of start or end
    return 1;
  else {
    loadNewPage = true;
    m_current_page = page_number;

    char t[256];
    snprintf(t, 256, "Loading page %d", m_current_page + 1);
    setBanner(t);
    
    return 0;
  }
}

bool BKMUDocument::isMUDocument(string& file) {
  // Read First 4 bytes
  char header[4];
  memset((void*)header, 0, 4);
  
  // int fd = sceIoOpen(file.c_str(), SCE_O_RDONLY, 0777);
  // if(fd == NULL) {
  //   printf("fopen fail: %d, %s\n", errno, strerror(errno));
  //   return false;
  // }
  FILE* f = fopen(file.c_str(), "r");
  if(f == NULL) {
    #ifdef DEBUG
      printf("fopen fail: %d, %s\n", errno, strerror(errno));
    #endif
    throw "failed opening file; try again";
  }

  // sceIoRead(fd, header, 4);
  // sceIoClose(fd);

  fread(header, 4, 1, f);
  fclose(f);

  const char* ext = get_ext(file.c_str());

  #ifdef DEBUG
    printf("scefile opened\n", ext);
    printf("ismu; ext: %s\n", ext);
  #endif

  // TODO: get libmagic or something?
  // Trusting the user for now...
  return ((header[0] == 0x25 && header[1] == 0x50 && header[2] == 0x44 && header[3] == 0x46) ||
          (strcmp(ext, ".xps") == 0) ||
          (strcmp(ext, ".svg") == 0) ||
          (strcmp(ext, ".cbz") == 0) ||
          (strcmp(ext, ".img") == 0) ||
          (strcmp(ext, ".tiff") == 0) ||
          (strcmp(ext, ".htm") == 0) ||
          (strcmp(ext, ".html") == 0) ||
          (strcmp(ext, ".epub") == 0) ||
          (strcmp(ext, ".fb2") == 0)
  );
}

void BKMUDocument::getFileName(string& s) {
  s = filename;
}

bool BKMUDocument::isPaginated() {
  return true;
}

// TODO: Move this to bkuser.
static float speed = 0.5f;
int BKMUDocument::pan(int x, int y) {
  #ifdef DEBUG_BUTTONS
    printf("INPUT x %i y %i\n", x, y);
  #endif
  
  // printf("%f %f %f %f\n", m_bounds.x0, m_bounds.x1, m_bounds.y0, m_bounds.y1);
  
  if (abs(x) <= FZ_ANALOG_THRESHOLD &&
      abs(y) <= FZ_ANALOG_THRESHOLD)
    return 0;

  // TODO: Choose invert analog settings
  // if settings.invertAnalog
  // x = -x

  // bounds checked pan
  if (abs(x) > FZ_ANALOG_THRESHOLD) {
    if (panX > m_bounds.x0) { // left bounds
      if (x > 0) panX -= x/10;     // only going right allowed
    } else if (-panX > m_bounds.x1 - m_width) { // right bounds
      if (x < 0) panX -= x/10;                       // only going left
    } else {        // inside box
      panX -= x/10;
    }
  }

  if (abs(y) > FZ_ANALOG_THRESHOLD) {
    if (panY > m_bounds.y0) {
      if (y > 0) panY -= y/10;
    } else if (-panY > m_bounds.y1 - m_height) {
      if (y < 0) panY -= y/10;
    } else {
      panY -= y/10;
    }
  }


  #ifdef DEBUG_BUTTONS
    printf("OUTPUT x %i y %i\n", panX, panY);
  #endif
  return BK_CMD_MARK_DIRTY;
}

bool BKMUDocument::isBookmarkable() {
  return true;
}

// -------------------- NOT IMPLEMENTED YET --------------------

bool BKMUDocument::isZoomable() {
  return true;
}

void BKMUDocument::getZoomLevels(vector<BKDocument::ZoomLevel>& v) {
  int n = BKUser::options.pdfFastScroll ? 15 : sizeof(zoomLevels)/sizeof(float);
  for (int i = 0; i < n; ++i)
    v.push_back(BKDocument::ZoomLevel(BKDOCUMENT_ZOOMTYPE_ABSOLUTE, "FIX ZOOM LABELS"));
}

int BKMUDocument::getCurrentZoomLevel() {
  return zoomLevel;
}

int BKMUDocument::setZoomLevel(int z) {
  if (z == zoomLevel)
    return 0;

  m_fitWidth = false;
  int n = sizeof(zoomLevels)/sizeof(float);
  zoomLevel = z;

  if (zoomLevel < 0)
    zoomLevel = 0;
  if (zoomLevel >= n)
    zoomLevel = n - 1;
  if (BKUser::options.pdfFastScroll && zoomLevel > 14) {
    zoomLevel = 14;
    m_scale = 2.0f;
  }

  m_scale = zoomLevels[zoomLevel];

  char t[256];
  snprintf(t, 256, "Zooming %2.3gx...", m_scale);
  setBanner(t);

  redrawBuffer();

  snprintf(t, 256, "Zoomed %2.3gx...", m_scale);
  setBanner(t);
    
  return BK_CMD_MARK_DIRTY;
}

bool BKMUDocument::hasZoomToFit() {
  return false;
}

int BKMUDocument::setZoomToFitWidth() {
  return 0;
}

int BKMUDocument::setZoomToFitHeight() {
  return 0;
}

int BKMUDocument::screenUp() {
  panY += 250;
}

int BKMUDocument::screenDown() {
  panY -= 250;
}

int BKMUDocument::screenLeft() {
  return 0;
}

int BKMUDocument::screenRight() {
  #ifdef DEBUG
    panX = 0;
    panY = 0;
  #endif
}

bool BKMUDocument::isRotable() {
  return false;
}

int BKMUDocument::getRotation() {
  return 0;
}

int BKMUDocument::setRotation(int r, bool bForce) {
  return 0;
}


void BKMUDocument::getBookmarkPosition(map<string, int>& m) {
  m["page"] = m_current_page;
  
  m["panX"] = panX;
  m["panY"] = panY;
}

int BKMUDocument::setBookmarkPosition(map<string, int>& m) {
  #ifdef DEBUG
    printf("setBookmarkPosition: page %i, panX %i, panY %i", m["page"], m["panX"], m["panY"]);
  #endif
  setCurrentPage(m["page"]);
  loadNewPage = false;

  panX = m["panX"];
  panY = m["panY"];

  redrawBuffer();

  return BK_CMD_MARK_DIRTY;
}

void BKMUDocument::getTitle(string&) {}
void BKMUDocument::getType(string&) {}