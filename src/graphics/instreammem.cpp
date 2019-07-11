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

#include <cstddef>
#include <cstring>

#include "instreammem.hpp"

namespace bookr {

InputStreamMem::InputStreamMem(char* b, int s) : InputStream(), base(b), size(s), position(0) {
}

InputStreamMem::~InputStreamMem() {
	base = 0;
}

InputStreamMem* InputStreamMem::create(char* b, int s) {
	return new InputStreamMem(b, s);
}

bool InputStreamMem::eos() {
	return position < size;
}

char InputStreamMem::get() {
	char r = *base;
	++position;
	++base;
	return r;
}

int InputStreamMem::getBlock(char* where, int size) {
	memcpy(where, base, size);
	base += size;
	position += size;
	return size;
}

}