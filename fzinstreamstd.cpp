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

#include <assert.h>

#include "fzinstreamstd.h"

#ifdef WIN32
#define STDMODE "rb"
#define STDBADSEP '/'
#define STDSEP '\\'
#else
#define STDMODE "r"
#define STDBADSEP '\\'
#define STDSEP '/'
#endif

static int mystrncpy(const char* from, char* to, int size) {
	int c = 0;
	while (*from != '\0' && c < (size - 1)) {
		*to = *from;
		++from;
		++to;
		++c;
	}
	*to = '\0';
	++c;
	return c;
}

FZInputStreamStd::FZInputStreamStd(const char* name) : FZInputStream(), file(NULL) {
	char sname[4096];
	int bytes = mystrncpy(name, sname, 4096);
	if (bytes > 4096) {
		return;
	}
	char *p;
	for (p = sname; *p != 0; ++p) {
		if (*p == STDBADSEP) {
			*p = STDSEP;
		}
	} 
	file = fopen(sname, STDMODE);
}

FZInputStreamStd::~FZInputStreamStd() {
	if (file != NULL)
		fclose(file);
	file = NULL;
}

FZInputStreamStd* FZInputStreamStd::create(const char* name) {
	FZInputStreamStd* s = new FZInputStreamStd(name);
	if (s->file == NULL) {
		s->release();
		return 0;
	}
	return s;
}

bool FZInputStreamStd::eos() {
	return feof(file) != 0 ? true : false;
}

char FZInputStreamStd::get() {
	return fgetc(file);
}

int FZInputStreamStd::getBlock(char* where, int size) {
	return fread(where, 1, size, file);
}

