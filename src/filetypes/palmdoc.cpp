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

#include <cstdio>
#include <list>

#include "palmdoc.h"
#include "palmdoc/palmdoc.h"

using std::string;

namespace bookr {

PalmDoc::PalmDoc() : buffer(0) { }
PalmDoc::~PalmDoc() {
	saveLastView();
	if (buffer)
		free(buffer);
}

PalmDoc* PalmDoc::create(string& file, string& longFileName) {
	PalmDoc* r = new PalmDoc();
	r->fileName = file;
	r->longFileName = longFileName;
	int length = 0;
	int isMobi = 0;
	char ctitle[dmDBNameLength];

	// convert file to plain text
	char* b = palmdoc_decode(file.c_str(), &length, &isMobi, ctitle);
	if (b == NULL) {
		return 0;
	}

	r->title = ctitle;

	if (isMobi) {
		r->buffer = FancyText::parseHTML(r, b, length);
	} else {
		r->buffer = FancyText::parseText(r, b, length);
	}

	r->resetFonts();
	r->resizeView(480, 272);
	return r;
}

void PalmDoc::getFileName(string& fn) {
	fn = fileName;
}

void PalmDoc::getTitle(string& s, int type) {
  switch(type){
  case 1:
    s = title;
    break;
  case 2:
    s = longFileName;
    break;
  case 3:
    s = title; 
    s += " ["; 
    s += longFileName;
    s += "]";
    break;
  case 4:
    s = longFileName;
    s += " [";
    s += title;
    s += "]";
    break;
  default:
    s = title;
    break;
  }

}

void PalmDoc::getType(string& t) {
	t = "PalmDoc";
}

bool PalmDoc::isPalmDoc(string& file) {
	return palmdoc_is_palmdoc(file.c_str()) != 0;
}

}