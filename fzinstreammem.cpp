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

#include <stddef.h>
#include <string.h>

#include "fzinstreammem.h"

FZInputStreamMem::FZInputStreamMem(char* b, int s) : FZInputStream(), base(b), size(s), position(0) {
}

FZInputStreamMem::~FZInputStreamMem() {
	base = 0;
}

FZInputStreamMem* FZInputStreamMem::create(char* b, int s) {
	return new FZInputStreamMem(b, s);
}

bool FZInputStreamMem::eos() {
	return position < size;
}

char FZInputStreamMem::get() {
	char r = *base;
	++position;
	++base;
	return r;
}

int FZInputStreamMem::getBlock(char* where, int size) {
	memcpy(where, base, size);
	base += size;
	position += size;
	return size;
}

