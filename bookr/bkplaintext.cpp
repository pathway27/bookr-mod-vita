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

BKPlainText::BKPlainText() { }
BKPlainText::~BKPlainText() {
	if (buffer)
		free(buffer);
}

BKPlainText* BKPlainText::create(string& file) {
	BKPlainText* r = new BKPlainText();
	r->fileName = file;

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

	// create fast fixed size array	
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

	r->reflow(480);

#if 0
	r->runs = new BKRun[1];
	r->nRuns = 1;
	r->runs[0].text = "1 22 333 4444 5555 666666 7777777 88888888 999999999 0000000000 11111111111 222222222222 3333333333333 44444444444444 1 22 333 4444 5555 666666 7777777 88888888 999999999 0000000000 11111111111 222222222222 3333333333333 44444444444444";
	r->runs[0].n = strlen(r->runs[0].text);
#endif
	return r;
}

void BKPlainText::getFileName(string& fn) {
	fn = fileName;
}

void BKPlainText::getTitle(string& t) {
	t = "FIX PLAIN TEXT TITLES";
}

void BKPlainText::getType(string& t) {
	t = "Plain text";
}

