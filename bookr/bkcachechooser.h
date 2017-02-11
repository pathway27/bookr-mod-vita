/*
 * Bookr: document reader for the Sony PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com)
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com)
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

#ifndef BKCACHECHOOSER_H
#define BKCACHECHOOSER_H

#include "fzscreen.h"

using namespace std;

#include "bklayer.h"

class BKCacheChooser : public BKLayer {
	#define MODE_CACHE 0
	#define MODE_DELETECACHE 1
	#define MODE_CLEARCACHE 2

	int mode;

	string cachePath;
	string curItem;
	int lastSelItem;
	string title;
	int ret;
	vector<BKMenuItem> cacheItems;
	vector<BKMenuItem> yesnoItems;

	void buildCacheMenu();

	protected:
	BKCacheChooser(string& t, int r);
	~BKCacheChooser();
	int updateCache(unsigned int buttons);
	int updateDeleteCache(unsigned int buttons);
	int updateClearCache(unsigned int buttons);

	public:
	virtual int update(unsigned int buttons);
	virtual void render();

	void getFullPath(string& s);

	static BKCacheChooser* create(string& t, int r);
	static void recursiveDelete(const char *dir);
};

#endif

