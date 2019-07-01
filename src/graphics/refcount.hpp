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
