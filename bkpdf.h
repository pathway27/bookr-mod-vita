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

#ifndef BKPDF_H
#define BKPDF_H

#include <string>

#include "fzscreen.h"
#include "fzfont.h"

using namespace std;

#include "bkdocument.h"

struct PDFContext;
class BKPDF : public BKDocument {
	PDFContext* ctx;

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
	virtual void getTitle(string&, int type = 0);
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
	static BKPDF* create(string& file,string& longFileName);
	static bool isPDF(string& file);
};

#define GLYPHCACHE_SIZE 524288
#define GLYPHCACHE_SLOTS (GLYPHCACHE_SIZE / 32)
#endif
