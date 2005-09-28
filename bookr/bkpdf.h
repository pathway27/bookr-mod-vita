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

#include "bklayer.h"

struct PDFContext;
class BKPDF : public BKLayer {
	string path;
	PDFContext* ctx;

	int bannerFrames;
	string banner;

	int panX;
	int panY;
	bool loadNewPage;
	void panBuffer(int nx, int ny);
	void redrawBuffer();

	protected:
	BKPDF(string& f);
	~BKPDF();

	public:
	virtual int update(unsigned int buttons);
	virtual void render();

	static BKPDF* create(string& file);
};

#endif

