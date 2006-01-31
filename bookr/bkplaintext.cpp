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

#include "bkplaintext.h"

BKPlainText::BKPlainText() { }
BKPlainText::~BKPlainText() { }

BKPlainText* BKPlainText::create(string& file) {
	BKPlainText* r = new BKPlainText();
	r->fileName = file;
	r->runs = new BKRun[1];
	r->nRuns = 1;

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
	r->runs[0].text = (char*)malloc(length);
	fread(r->runs[0].text, length, 1, f);
	r->runs[0].n = length;
	fclose(f);

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

