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

#ifndef FZINSTREAMSIZED_H
#define FZINSTREAMSIZED_H

#include "fzinputstream.h"

/**
 * Sub-stream based input stream.
 * Reads up to a given size from a given input stream,
 */
class FZInputStreamSized : public FZInputStream {
	FZInputStream* in;
	int size;
protected:
	FZInputStreamSized(FZInputStream* s, int n);
	virtual ~FZInputStreamSized();
public:
	/**
	 * Check for end of stream.
	 */
	virtual bool eos();
	/**
	 * Get a byte from the stream.
	 */
	virtual char get();
	/**
	 * Get a block of bytes byte from the stream.
	 * Returns if true the whole block was read ok, false otherwise.
	 */
	virtual int getBlock(void* where, int s);
	/**
	 * Create a new sub-stream from s, reading a maximum of n bytes.
	 */
	static FZInputStreamSized* create(FZInputStream* s, int n);
};

#endif

