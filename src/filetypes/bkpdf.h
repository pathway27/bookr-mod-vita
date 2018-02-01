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

#ifndef BKPDF_H
#define BKPDF_H

#include <string>
extern "C" {
	#include <mupdf/fitz.h>
}

#include "../graphics/fzscreen.h"
#include "../graphics/fzfont.h"
#include "../bkdocument.h"

struct PDFContext;
class BKPDF : public BKDocument {
	PDFContext* ctx;
	fz_context  *m_ctx;
	fz_document *m_doc;
	fz_page *m_page;
	fz_pixmap *m_pix;
	fz_rect m_bounds;
	fz_matrix m_transform;
	int m_page_number, m_page_count;
	// fz_text_page *m_pageText;
	fz_rect m_matches[512];
	bool m_curPageLoaded;
	bool m_fitWidth;
	float m_scale, m_rotate;

	int m_width, m_height;

	string fileName;

	int panX;
	int panY;
	int neg_panX;
	int neg_panY;
	bool loadNewPage;
	bool resetPanXY;
	bool pageError;
	bool retryLoadPageWhenError;
	int leftMargin;
	void panBuffer(int nx, int ny);
	void clipCoords(float& nx, float& ny);
	bool redrawBuffer(bool setSpeed = false);
	bool redrawBufferIncremental(int nx, int ny, bool setSpeed = false);
	int prePan(int x, int y);
	int pdfReopenFile();

	string title;
	char* ctitle;
	protected:
	BKPDF(string& f);
	~BKPDF();

	public:
	virtual int updateContent();
	virtual int resume();
	virtual void renderContent();

	virtual void getFileName(string&);
	virtual void getTitle(string&);
	virtual void getType(string&);

	virtual bool isPaginated();
	virtual int getTotalPages();
	virtual int getCurrentPage();
	virtual int setCurrentPage(int);

	virtual bool isZoomable();
	virtual void getZoomLevels(vector<BKDocument::ZoomLevel>& v);
	virtual int getCurrentZoomLevel();
	virtual int setZoomLevel2(int);
	virtual int setZoomLevel(int);
	virtual bool hasZoomToFit();
	virtual int setZoomToFitWidth();
	virtual int setZoomToFitHeight();
	virtual int setZoomIn(int,int);
	virtual void setZoom(float z);
	virtual int pan(int, int);

	virtual int screenUp();
	virtual int screenDown();
	virtual int screenLeft();
	virtual int screenRight();

	virtual bool isRotable();
	virtual int getRotation();
	virtual int setRotation2(int, bool bForce=false);
	virtual int setRotation(int, bool bForce=false);

	virtual bool isBookmarkable();
	virtual void getBookmarkPosition(map<string, int>&);
	virtual int setBookmarkPosition(map<string, int>&);
	virtual int getFastImageStatus();
	virtual float getCurrentZoom();
	virtual int getOutlineType();
	virtual void* getOutlines();
	virtual void gotoOutline(void *o, bool ignoreZoom = false);
	static BKPDF* create(string& file);
	static bool isPDF(string& file);
};

#define GLYPHCACHE_SIZE 524288
#define GLYPHCACHE_SLOTS (GLYPHCACHE_SIZE / 32)
#endif
