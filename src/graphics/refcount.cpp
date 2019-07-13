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
