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

#ifndef FZREFCOUNT_H
#define FZREFCOUNT_H

namespace bookr {

/**
 * Simple pure reference counted allocation cycle objects.
 * All the objects inheriting from FZRefCounted must be created by factories.
 * Initial reference count after creation must be always 1. Subclasses must
 * implement creation/destruction behaviour on constructors/destroyers.
 * FZRefCounted::retain() and FZRefCounted::release() are NOT overridable for
 * this reason.
 */
class RefCounted {
	int references;
	void tidy();

	protected:
	// Force factory construction
	RefCounted();
	// Force destruction by release
	virtual ~RefCounted();

	public:
	/**
	 * Increase reference count.
	 */
	void retain();
	/**
	 * Decrease reference count.
	 * It's only when calling this method that the object may get deleted.
	 */
	void release();
};

}

#endif
