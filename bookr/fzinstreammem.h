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

#ifndef FZINSTREAMMEM_H
#define FZINSTREAMMEM_H

#include "fzinputstream.h"

/**
 * Memory based input stream.
 */
class FZInputStreamMem : public FZInputStream {
	char* base;
	int size;
	int position;
protected:
	FZInputStreamMem(char* b, int s);
	virtual ~FZInputStreamMem();
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
	 * Returns the number of bytes read.
	 */
	virtual int getBlock(char* where, int size);
	/**
	 * Create a stream from a memory buffer
	 */
	static FZInputStreamMem* create(char* b, int s);
};

#endif
