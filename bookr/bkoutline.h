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

#ifndef BKOUTLINE_H
#define BKOUTLINE_H

#include "fzscreen.h"

using namespace std;

#include "bklayer.h"
#include "fitz.h"
#include "mupdf.h"



class BKOutline : public BKLayer {
	string title;
	int ret;
	int outlineType;
	void* outlines;
	void* selected_outline;
	bool menuDirty;
	vector<BKOutlineItem> items;
	void buildOutlineMenus(void* outline, string prefix, bool topLayer);

	protected:
	BKOutline(const char* t, int r);
	~BKOutline();

	public:
	virtual int update(unsigned int buttons);
	virtual void render();

	void setOutlines(int type, void* o);
	void* getSelectedOutline();
	
	static BKOutline* create(const char*  t, int r, int top, int sel);
	void getSelection(int& top, int& sel);
	bool ignoreZoom;
};

#endif
