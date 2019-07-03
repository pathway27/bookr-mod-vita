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

#include "refcount.hpp"

#ifdef DEBUG_REFCOUNT
  #ifdef PSP
    #include <pspdebug.h>
    #define printf pspDebugScreenPrintf
  #else
  	#include <stdio.h>
  #endif
#endif

namespace bookr {

void RefCounted::tidy() {
	if (references < 0) {
#ifdef DEBUG_REFCOUNT
		printf("Instance %p: RefCounted::tidy() called with references = %d\n", this, references);
#endif
	} else if (references == 0) {
#ifdef DEBUG_REFCOUNT
		printf("Instance %p: delete() call chain begins...\n", this);
#endif
		delete this;
#ifdef DEBUG_REFCOUNT
		printf("Instance %p: delete() call chain ends\n", this);
#endif
	}
}

RefCounted::RefCounted() : references(1) {
}

RefCounted::~RefCounted() {
}

void RefCounted::retain() {
	++references;
#ifdef DEBUG_REFCOUNT
	printf("Instance %p: retained, now references = %d\n", this, references);
#endif
}

void RefCounted::release() {
	--references;
#ifdef DEBUG_REFCOUNT
	printf("Instance %p: released, now references = %d\n", this, references);
#endif
	tidy();
}

}