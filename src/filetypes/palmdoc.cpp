/*
 * bookr-modern: a graphics based document reader 
 * Copyright (C) 2019 pathway27 (Sree)
 * IS A MODIFICATION OF THE ORIGINAL
 * Bookr and bookr-mod for PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com),
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com),
 * AND VARIOUS OTHER FORKS, See Forks in README.md
 * Licensed under GPLv3+, see LICENSE
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