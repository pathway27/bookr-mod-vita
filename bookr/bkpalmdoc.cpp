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

#include "bkpalmdoc.h"
#include "palmdoc/palmdoc.h"

BKPalmDoc::BKPalmDoc() : buffer(0) { }
BKPalmDoc::~BKPalmDoc() {
	if (buffer)
		free(buffer);
}

BKPalmDoc* BKPalmDoc::create(string& file) {
	BKPalmDoc* r = new BKPalmDoc();
	r->fileName = file;
	int length = 0;

	// convert file to plain text
	char* b = palmdoc_decode(file.c_str(), &length);
	if (b == NULL) {
		return 0;
	}

	r->buffer = b;

	// tokenize text file
	list<BKRun> tempRuns;
	int li = 0;
	BKRun run;
	for (int i = 0; i < length; ++i) {
		if (b[i] == 10) {
			run.text = &b[li];
			run.n = i - li;
			li = i;
			run.continuation = BKFT_CONT_LF;
			tempRuns.push_back(run);
		}
	}
	// last run
	run.text = &b[li];
	run.n = length - li;
	run.continuation = BKFT_CONT_LF;
	tempRuns.push_back(run);

	// create fast fixed size run array	
	r->runs = new BKRun[tempRuns.size()];
	r->nRuns = tempRuns.size();
	list<BKRun>::iterator it(tempRuns.begin());
	int i = 0;
	while (it != tempRuns.end()) {
		const BKRun& l = *it;
		r->runs[i] = l;
		++i;
		++it;
	}

	r->resetFonts();
	r->resizeView(480, 272);
	return r;
}

void BKPalmDoc::getFileName(string& fn) {
	fn = fileName;
}

void BKPalmDoc::getTitle(string& t) {
	t = "FIX PALMDOC TITLES";
}

void BKPalmDoc::getType(string& t) {
	t = "PalmDoc";
}

bool BKPalmDoc::isPalmDoc(string& file) {
	return palmdoc_is_palmdoc(file.c_str()) != 0;
}

