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

#ifndef BKBOOKMARK_H
#define BKBOOKMARK_H

#include "graphics/fzscreen.h"

#ifdef PSP
	#include <pspiofilemgr.h>
#elif __vita__
	#include <psp2/io/fcntl.h> 
#else
	#include <fcntl.h>
#endif

using namespace std;

struct BKBookmark {
	// common data fields for the menu. only the file parameter is actualy used by
	// the views
	string title;
	int page;
	string createdOn;
	int *bm_thumbnail;
	bool lastView;
	float zoom;
	// the view-specific data. this is a black box for the bookmarks manager
	map<string, int> viewData;
	BKBookmark() : page(0), bm_thumbnail(0), lastView(false) { }
};

typedef vector<BKBookmark> BKBookmarkList;
typedef BKBookmarkList::iterator BKBookmarkListIt;

class BKBookmarksManager {

	#define MAX_BOOKMARKS_PER_FILE 20
	#define BOOKMARK_XML "bookmark.xml"
	#define BOOKMARK_XML_BASE "%s/%s"

	public:
	
	// find the last file used
	  static void getLastFile(string& f,string& lfn);
	    static void	setLastFile( string& filename, string& lfn );

	// find the last read bookmark for a given file
	static bool getLastView(string& filename, BKBookmark&);
	// load all the bookmarks for a given file
	static void getBookmarks(string& filename, BKBookmarkList &);
	// save all the bookmarks for a given file, overwriting the existing ones
	static void setBookmarks(string& filename, BKBookmarkList&);
	// add a new bookmark for a file
	static void addBookmark(string& filename, BKBookmark& b);
	// clears everything
	static void clear();
	static void free();
	static int prune();
};

#endif

