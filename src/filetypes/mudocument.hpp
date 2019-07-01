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

#ifndef BKMUPDFDOCUMENT_H
#define BKMUPDFDOCUMENT_H

#include <mupdf/fitz.h>
#include <mupdf/pdf.h>

#include "../document.hpp"
#include "../graphics/screen.hpp"

using std::string;

namespace bookr {

class MUDocument : public Document {
private:
  fz_context *m_ctx;
  fz_document *m_doc;
  fz_page *m_page;
  fz_pixmap *m_pix;
  fz_rect m_bounds;
  fz_matrix m_transform;
  fz_stext_page *m_pageText;
  fz_rect m_matches[512];
  fz_link *m_links;
  pdf_document *m_pdf;
  
  int m_current_page;
  int m_pages;
  bool m_curPageLoaded;
  bool m_fitWidth;
  bool m_fitHeight;

  bool loadNewPage;
  bool zooming;
  
  int zoomLevel;
  int rotateLevel;
  float m_scale;
  float m_rotate;
  int m_width;
  int m_height;
  float panX;
	float panY;

  string filename;

  bool redrawBuffer();

protected:
  MUDocument(string& f);
  ~MUDocument();

public:
	virtual int updateContent();
	virtual int resume();
	virtual void renderContent();

  virtual bool isPaginated();
  virtual int getTotalPages();
	virtual int getCurrentPage();
	virtual int setCurrentPage(int);

  virtual int pan(int, int);
  virtual int screenUp();
	virtual int screenDown();
	virtual int screenLeft();
	virtual int screenRight();

  virtual void getFileName(string&);
  virtual void getTitle(string&);
  virtual void getType(string&);

  static MUDocument* create(string& file);
  static bool isMUDocument(string& file);

	virtual bool isZoomable();
	virtual void getZoomLevels(vector<Document::ZoomLevel>& v);
	virtual int getCurrentZoomLevel();
	virtual int setZoomLevel(int);
	virtual bool hasZoomToFit();
	virtual int setZoomToFitWidth();
	virtual int setZoomToFitHeight();

	virtual bool isRotable();
	virtual int getRotation() ;
	virtual int setRotation(int, bool bForce=false);

	virtual bool isBookmarkable();
	virtual void getBookmarkPosition(map<string, float>&);
	virtual int setBookmarkPosition(map<string, float>&);
};

}

#endif