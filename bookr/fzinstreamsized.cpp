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

#include "fzinstreamsized.h"

FZInputStreamSized::FZInputStreamSized(FZInputStream* s, int n) : in(s), size(n) {
	assert(size >= 0);
	in->retain();
}

FZInputStreamSized::~FZInputStreamSized() {
	in->release();
	in = 0;
	size = -1;
}

FZInputStreamSized* FZInputStreamSized::create(FZInputStream* s, int n) {
	FZInputStreamSized* z = new FZInputStreamSized(s, n);
	return z;
}

bool FZInputStreamSized::eos() {
	return size <= 0 || in->eos();
}

char FZInputStreamSized::get() {
	--size;
	return in->get();
}

int FZInputStreamSized::getBlock(void* where, int s) {
	size -= s;
	if (size < s)
		s = size;
	return in->getBlock(where, s);
}

