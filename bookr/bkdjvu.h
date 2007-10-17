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
 
#ifndef BKDJVU_H
#define BKDJVU_H

#include <string>

#include "fzscreen.h"
#include "fzfont.h"

using namespace std;

#include "bkdocument.h"

struct DJVUContext;
class BKDJVU : public BKDocument {
	DJVUContext* ctx;

	string fileName;

	int panX;
	int panY;
	bool loadNewPage;
	bool pageError;
	void panBuffer(int nx, int ny);
	void clipCoords(float& nx, float& ny);
	void redrawBuffer();
	int prePan(int x, int y);

	string title;

	protected:
	BKDJVU(string& f);
	~BKDJVU();

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
	virtual int setZoomLevel(int);
	virtual bool hasZoomToFit();
	virtual int setZoomToFitWidth();
	virtual int setZoomToFitHeight();

	virtual int pan(int, int);

	virtual int screenUp();
	virtual int screenDown();
	virtual int screenLeft();
	virtual int screenRight();

	virtual bool isRotable();
	virtual int getRotation();
	virtual int setRotation(int, bool bForce=false);

	virtual bool isBookmarkable();
	virtual void getBookmarkPosition(map<string, int>&);
	virtual int setBookmarkPosition(map<string, int>&);

	static BKDJVU* create(string& file);
	static bool isDJVU(string& file);
};

#endif

