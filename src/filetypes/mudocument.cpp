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

#ifdef __vita__
#include <psp2/io/fcntl.h>
#endif

#include <map>
#include <fstream>
#include <chrono>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <ctime>
#include <cerrno>

#include "mudocument.hpp"
#include "../graphics/resolutions.hpp"
#include "../bookmark.hpp"
#include "../utils.h"
#include "../graphics/fzscreen_defs.h"
#include "../graphics/controls.hpp"

extern int _newlib_heap_size_user;

namespace bookr {

// make sure only one document is open, do we need this?
// vita can probably do multiple.
static MUDocument* mudoc_singleton = nullptr;
// texture of current pixmap, TODO: generic fztexture
#ifdef __vita__
static vita2d_texture *texture;
#elif defined(SWITCH)
#endif

// These will crash...
//, 2.5f, 2.75f, 3.0f, 3.5f, 4.0f, 5.0f, 7.5f, 10.0f, 16.0f };

static const float zoomLevels[] = { 0.25f, 0.5f, 0.75f, 0.90f, 1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f,
  1.6f, 1.7f, 1.8f, 1.9f, 2.0f, 2.25f };
static const float rotateLevels[] = { 0.0f, 90.0f, 180.0f, 270.0f };


MUDocument::MUDocument(string& f) : 
  m_ctx(nullptr), m_doc(nullptr), m_page(nullptr), loadNewPage(false), zooming(false),
  m_pageText(nullptr), m_links(nullptr), panX(0), panY(0), m_current_page(0),
  m_curPageLoaded(false), m_fitWidth(true), m_fitHeight(false), zoomLevel(8)
{
  #ifdef DEBUG
    printf("MUDocument::MUDocument f: %s, filename: %s\n", f.c_str(), filename.c_str());
  #endif

  filename = string(f);
  m_rotate = 0.0f;
  rotateLevel = 0;
  m_width = FZ_SCREEN_WIDTH;
  m_height = FZ_SCREEN_HEIGHT;

  // Initalize fitz context
  m_ctx = fz_new_context(nullptr, nullptr,
  #ifdef __vita__
    _newlib_heap_size_user
  #elif defined(SWITCH)
    FZ_STORE_DEFAULT
  #endif
  );

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
    printf("MUDocument::MUDocument end\n");
  #endif
}

MUDocument::~MUDocument() {
  #ifdef DEBUG
    printf("MUDocument::~MUDocument\n");
  #endif
  
  saveLastView();
  mudoc_singleton = nullptr;
  fz_drop_pixmap(m_ctx, m_pix);
  fz_drop_document(m_ctx, m_doc);
  fz_drop_context(m_ctx);
}

MUDocument* MUDocument::create(string& file) {
  #ifdef DEBUG
    printf("MUDocument::create\n");
  #endif
  
  if (mudoc_singleton) {
    printf("cannot open more than 1 pdf at the same time\n");
    return mudoc_singleton;
  }

  MUDocument* b = new MUDocument(file);
  mudoc_singleton = b;


  b->redrawBuffer();
  return b;
}

// Draws current page into texture using pixmap
bool MUDocument::redrawBuffer() {
  #ifdef DEBUG
    printf("MUDocument::redrawBuffer pp\n");
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

  #ifdef DEBUG
    printf("fz_load\n");
  #endif

  // bounds for inital window size
  m_bounds = fz_bound_page(m_ctx, m_page);
  #ifdef DEBUG
    printf("bound_page; (%f, %f) - (%f, %f)\n", m_bounds.x0, m_bounds.y0, m_bounds.y0, m_bounds.y1);
  #endif

  fz_matrix rotation_matrix;
  fz_matrix scaling_matrix;
  fz_matrix translation_matrix;

  // Rotate first since co-ords can be negative
  rotation_matrix = fz_rotate(m_rotate); // m_t = rotate * scaling_matrix
  fz_transform_rect(m_bounds, rotation_matrix);

  // Translate to positive coords to figure out fit to width/height scale easily
  translation_matrix = fz_translate(-m_bounds.x0, -m_bounds.y0); // matrix = translation matrix (for rect)
  fz_transform_rect(m_bounds, translation_matrix);

  if (m_fitWidth) {
    m_scale = m_width / (m_bounds.x1 - m_bounds.x0);
    vector<float> vec(std::begin(zoomLevels), std::end(zoomLevels));
    auto const it = std::lower_bound(vec.begin(), vec.end(), m_scale);
    if (it != vec.end())
      zoomLevel = it - vec.begin();
  }
  else if (m_fitHeight) {
    m_scale = m_height / (m_bounds.y1 - m_bounds.y0);
    vector<float> vec(std::begin(zoomLevels), std::end(zoomLevels));
    auto const it = std::lower_bound(vec.begin(), vec.end(), m_scale);
    if (it != vec.end())
      zoomLevel = it - vec.begin();
  }

  #ifdef DEBUG
    printf("bound_page; m_scale: %2.3gx, zoomLevel: %i\n", m_scale, zoomLevel);
  #endif

  // Scaling is then always positive so do it last
  scaling_matrix = fz_scale(m_scale, m_scale);
  fz_transform_rect(m_bounds, scaling_matrix);

  // Create final transformation matrix in the correct order (Rotation x Scaling x Translation)
  m_transform = fz_concat(rotation_matrix, scaling_matrix);
  // fz_concat(&m_transform, &m_transform, &translation_matrix); // dont really need to translate the page...?

  #ifdef DEBUG
    printf("bound_page; (%f, %f) - (%f, %f)\n", m_bounds.x0, m_bounds.y0, m_bounds.y0, m_bounds.y1);
  #endif

  #ifdef DEBUG
    printf("scale/transform\n");
  #endif

  // TODO: Is display list or bbox better?
  // This is currently the longest operation
  pdf_annot *annot;
  fz_try(m_ctx)
    m_pix = fz_new_pixmap_from_page_contents(m_ctx, m_page, m_transform, fz_device_rgb(m_ctx), 0);
  fz_catch(m_ctx) {
    printf("cannot render page: %s\n", fz_caught_message(m_ctx));
  }

  #ifdef DEBUG
    printf("new_pixmap n: %i \n", m_pix->n);
  #endif

  #ifdef __vita__
    // Crashes due to GPU memory use without this.
    vita2d_free_texture(texture);

    #ifdef DEBUG
      printf("post vita2d_free_texture\n");
    #endif

    texture = _vita2d_load_pixmap_generic(m_pix);

  #endif

  #ifdef DEBUG
    printf("post _vita2d_load_pixmap_generic\n");
  #endif

  fz_drop_pixmap(m_ctx, m_pix);
  // load annotations

  return true;
}

int MUDocument::updateContent() {
  if (loadNewPage) {
    panY = 0;
    redrawBuffer();
    saveLastView();

    loadNewPage = false;
    char t[256];

    snprintf(t, 256, "Page %d of %d", m_current_page + 1, m_pages);
    setBanner(t);
    
    return BK_CMD_MARK_DIRTY;
  } else if (zooming) {
    panX = 0;
    panY = 0;
    redrawBuffer();

    zooming = false;
    char t[256];

    snprintf(t, 256, "Zoomed...");
    setBanner(t);
    
    return BK_CMD_MARK_DIRTY;
  }
  return 0;
}

int MUDocument::resume() {
  // mupdf leaves open file descriptors around. they don't survive a suspend.
  // Is this still the case? yes
  // TODO: Don't need to reload for epub/html based docs, all kept in memory and retained
  return BK_CMD_RELOAD;
}

void MUDocument::renderContent() {
  #ifdef DEBUG_RENDER
    printf("MUDocument::renderContent %i / %i\n", m_current_page, m_pages);
  #endif

  Screen::clear(0xefefef, FZ_COLOR_BUFFER);
  #ifdef __vita__
    vita2d_draw_texture(texture, panX, panY);
  #endif

  // TODO: Show Page Error, don"t draw texture then.
  // if (pageError) {
  // 	texUI->bindForDisplay();
  // 	Screen::ambientColor(0xf06060ff);
  // 	drawRect(0, 126, 480, 26, 6, 31, 1);
  // 	fontBig->bindForDisplay();
  // 	Screen::ambientColor(0xff222222);
  // 	Screen::drawText(380, 400, RGBA8(0,0,0,255), 1.0f, "Error in page...");
  // }
}

int MUDocument::getTotalPages() {
  return m_pages - 1;
}

int MUDocument::getCurrentPage() {
  return m_current_page;
}

int MUDocument::setCurrentPage(int page_number) {
  // TOOD: Don't change page if changing to same page we'r on.
  if (page_number < 0 || page_number >= m_pages)
    // TODO(UI): Some visual notice of start or end
    setBanner("Invalid");
  else {
    loadNewPage = true;
    m_current_page = page_number;

    char t[256];
    snprintf(t, 256, "Loading page %d", m_current_page + 1);
    setBanner(t);
    
    
  }
  return 0;
}

bool MUDocument::isMUDocument(string& file) {
  // Read First 4 bytes
  char header[4];
  memset((void*)header, 0, 4);
  
  #ifdef __vita__
    int fd = sceIoOpen(file.c_str(), SCE_O_RDONLY, 0777);
    if (fd < 0) {
      printf("sceIoOpen fail: %d, %s\n", errno, strerror(errno));
      return false;
    }

    sceIoRead(fd, header, 4);
    sceIoClose(fd);
  #else
    FILE* f = fopen(file.c_str(), "r");
    if(f == NULL) {
      #ifdef DEBUG
        printf("fopen fail: %d, %s\n", errno, strerror(errno));
      #endif
      throw "failed opening file; try again";
    }

    fread(header, 4, 1, f);
    fclose(f);
  #endif

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

void MUDocument::getFileName(string& s) {
  s = filename;
}

bool MUDocument::isPaginated() {
  return true;
}

#define D_PAD_SPEED 250
int MUDocument::screenUp() {
  float potentialY = panY + D_PAD_SPEED;
  
  #ifdef DEBUG
    printf("bounds y0: %f y1: %f\n", m_bounds.y0, m_bounds.y1);
    printf("panY: %f potentialY: %f\n", panY, potentialY);
  #endif

  if (potentialY >= m_bounds.y0)
    panY = m_bounds.y0;
  else
    panY = potentialY;
}

int MUDocument::screenDown() {
  float potentialY = panY - D_PAD_SPEED;
  
  #ifdef DEBUG
    printf("bounds y0: %f y1: %f\n", m_bounds.y0, m_bounds.y1);
    printf("panY: %f potentialY: %f\n", panY, potentialY);
  #endif

  int bottomBounds = (m_bounds.y1 - FZ_SCREEN_HEIGHT);
  if (-potentialY >= bottomBounds)
    panY = -bottomBounds;
  else
    panY = potentialY;
}

// TODO: Move this to user.
static float speed = 0.5f;
int MUDocument::pan(int x, int y) {
  #ifdef DEBUG_BUTTONS
    printf("INPUT x %i y %i\n", x, y);
  #endif
  
  if (abs(x) <= FZ_ANALOG_THRESHOLD &&
      abs(y) <= FZ_ANALOG_THRESHOLD)
    return 0;

  #ifdef DEBUG
    printf("panX: %f panY: %f\n x1:%f y1:%f\n x0:%f y0:%f\n", panX, panY, m_bounds.x1, m_bounds.y1, m_bounds.x0, m_bounds.y0);
  #endif
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

bool MUDocument::isBookmarkable() {
  return true;
}

// -------------------- NOT IMPLEMENTED YET --------------------

bool MUDocument::isZoomable() {
  return true;
}

void MUDocument::getZoomLevels(vector<Document::ZoomLevel>& v) {
  int n = User::options.pdfFastScroll ? 15 : sizeof(zoomLevels)/sizeof(float);
  for (int i = 0; i < n; ++i)
    v.push_back(Document::ZoomLevel(BKDOCUMENT_ZOOMTYPE_ABSOLUTE, "FIX ZOOM LABELS"));
}

int MUDocument::getCurrentZoomLevel() {
  return zoomLevel;
}

int MUDocument::setZoomLevel(int z) {
  #ifdef DEBUG_BUTTONS
    printf("setZoomLevel");
  #endif
  
  if (z == zoomLevel)
    return 0;

  m_fitWidth = false;
  m_fitHeight = false;
  zooming = true;
  int n = sizeof(zoomLevels)/sizeof(float);
  zoomLevel = z;

  if (zoomLevel < 0)
    zoomLevel = 0;
  if (zoomLevel >= n)
    zoomLevel = n - 1;
  if (User::options.pdfFastScroll && zoomLevel > 14) {
    zoomLevel = 14;
    m_scale = 2.0f;
  }

  #ifdef DEBUG
    printf("setZoomLevel: z %i\n", z);
  #endif

  m_scale = zoomLevels[zoomLevel];

  #ifdef DEBUG
    printf("setZoomLevel: z %i m_scale %2.3gx\n", z, m_scale);
  #endif

  char t[256];
  snprintf(t, 256, "Zooming %2.3gx...", m_scale);
  setBanner(t);

  return 0;
}

bool MUDocument::hasZoomToFit() {
  return true;
}

int MUDocument::setZoomToFitWidth() {
  m_fitWidth = true;
  m_fitHeight = false;
  redrawBuffer();
  return 0;
}

int MUDocument::setZoomToFitHeight() {
  m_fitWidth = false;
  m_fitHeight = true;
  redrawBuffer();
  return 0;
}

int MUDocument::screenLeft() {
  return 0;
}

int MUDocument::screenRight() {
  #ifdef DEBUG
    panX = 0;
    panY = 0;
  #endif
}

bool MUDocument::isRotable() {
  return true;
}

int MUDocument::getRotation() {
  return rotateLevel;
}

int MUDocument::setRotation(int r, bool bForce) {
  #ifdef DEBUG

  #endif

  if (r == rotateLevel)
    return 0;

  if (r < 0)
    r = 3;
  if (r >= 4)
    r = 0;

  rotateLevel = r;
  m_rotate = rotateLevels[rotateLevel];

  // reset zoom?
  // float nx = float(panX);
  // float ny = float(panY);
  // clipCoords(nx, ny);
  // panX = int(nx);
  // panY = int(ny);

  char t[256];
  snprintf(t, 256, "Rotate to %3.3g°", m_rotate);
  setBanner(t);

  panX = 0;
  panY = 0;

  // if (User::options.pdfFastScroll) {
  // //pdfRenderFullPage(ctx);
  //   loadNewPage = true;
  //   return BK_CMD_MARK_DIRTY;
  // }

  // redrawBuffer();
  // return BK_CMD_MARK_DIRTY;
  redrawBuffer();

  return 0;
}


void MUDocument::getBookmarkPosition(map<string, float>& m) {
  m["page"] = m_current_page;
  
  m["panX"] = panX;
  m["panY"] = panY;

  m["scale"] = m_scale;
  m["fitWidth"] = m_fitWidth; 
  m["fitHeight"] = m_fitHeight;
  m["rotate"] = m_rotate;
}

int MUDocument::setBookmarkPosition(map<string, float>& m) {
  #ifdef DEBUG
    printf("setBookmarkPosition: page %i, panX %i, panY %i", m["page"], m["panX"], m["panY"]);
  #endif
  setCurrentPage(m["page"]);
  loadNewPage = false;

  panX = m["panX"];
  panY = m["panY"];

  // Can't be sure these exist in XML
  m_scale = get_or(m, "scale", 1);
  m_fitWidth = get_or(m, "fitWidth", true);
  m_fitHeight = get_or(m, "fitHeight", false);
  m_rotate = get_or(m, "rotate", 0);

  redrawBuffer();

  return BK_CMD_MARK_DIRTY;
}

void MUDocument::getTitle(string& t) {
  t = "title";
}

void MUDocument::getType(string& t) {
  t = "MUDoc";
}

}