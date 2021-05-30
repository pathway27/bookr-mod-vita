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

#ifndef FZINPUTSTREAM_H
#define FZINPUTSTREAM_H

#include "refcount.hpp"

namespace bookr {

/**
 * Base interface for input streams.
 */
class InputStream : public RefCounted {
protected:
	InputStream() { }
	virtual ~InputStream() { }
public:
	/**
	 * Check for end of stream.
	 */
	virtual bool eos() = 0;
	/**
	 * Get a byte from the stream.
	 */
	virtual char get() = 0;
	/**
	 * Get a block of bytes byte from the stream.
	 * Returns the number of bytes read.
	 */
	virtual int getBlock(char* where, int size) = 0;
};

}

#endif
