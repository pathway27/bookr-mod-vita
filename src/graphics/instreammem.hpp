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

#ifndef FZINSTREAMMEM_H
#define FZINSTREAMMEM_H

#include "inputstream.hpp"

namespace bookr {

/**
 * Memory based input stream.
 */
class InputStreamMem : public InputStream {
	char* base;
	int size;
	int position;
protected:
	InputStreamMem(char* b, int s);
	virtual ~InputStreamMem();
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
	static InputStreamMem* create(char* b, int s);
};

}

#endif
