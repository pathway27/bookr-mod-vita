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

#include <stdio.h>
#include <list>
using namespace std;
#include "bkplaintext.h"

BKPlainText::BKPlainText() : buffer(0) { }
BKPlainText::~BKPlainText() {
  if (buffer){
    saveLastView();
    free(buffer);
  }
}

BKPlainText* BKPlainText::create(string& file, string& longFileName) {
	BKPlainText* r = new BKPlainText();
	r->fileName = file;
	r->longFileName = longFileName;
	// read file to memory
	FILE* f = fopen(file.c_str(), "r");
	if (f == NULL) {
		delete r;
		return NULL;
	}
	long length = 0;
	fseek(f, 0, SEEK_END);
	length = ftell(f);
	fseek(f, 0, SEEK_SET);
	if (length > 4*1024*1024)
		length = 4*1024*1024;
	char* b = (char*)malloc(length);
	fread(b, length, 1, f);
	fclose(f);

	bool isHTML = false;
	// FIX: make the heuristic a bit more advanced than that...
	const char* fc = file.c_str();
	int fs = file.size();
	if (
		((fc[fs - 1] | 0x20) == 'l') &&
		((fc[fs - 2] | 0x20) == 'm') &&
		((fc[fs - 3] | 0x20) == 't') &&
		((fc[fs - 4] | 0x20) == 'h')
	) {
		isHTML = true;
	}
	if (
		((fc[fs - 1] | 0x20) == 'm') &&
		((fc[fs - 2] | 0x20) == 't') &&
		((fc[fs - 3] | 0x20) == 'h')
	) {
		isHTML = true;
	}
	
	if (isHTML) {
		r->buffer = BKFancyText::parseHTML(r, b, length);
	} else {
		r->buffer = BKFancyText::parseText(r, b, length);
	}

	r->resetFonts();
	r->resizeView(480, 272);
	return r;
}

void BKPlainText::getFileName(string& fn) {
	fn = fileName;
}

void BKPlainText::getTitle(string& s, int type) {
  switch(type){
    
  case 1:
    s = "<No Title Info>";
    break;
  case 2:
    s = longFileName;
    break;
  case 3:
    s = "No Title Info"; 
    s += " ["; 
    s += longFileName;
    s += "]";
    break;
  case 4:
    s = longFileName;
    s += " [";
    s += "No Title Info";
    s += "]";
    break;
  default:
    int n = longFileName.size();
    int lastSlash = -1;
    for (int i = 0; i < n; ++i) {
      if (longFileName[i] == '\\')
	lastSlash = i;
      else if (longFileName[i] == '/')
	lastSlash = i;
    }
    s.assign(longFileName, lastSlash+1, n - 1 - lastSlash);
    break;
  }

}

void BKPlainText::getType(string& t) {
	t = "Plain text";
}

