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

#include "fzscreen.h"

using namespace std;

typedef vector<int> bkBookmarkPos;
typedef vector<int>::iterator bkBookmarkPosIt;

class BKBookmark {

	#define MAX_BOOKMARKS_PER_FILE 5
	#define BOOKMARK_XML "bookmark.xml"
	#define BOOKMARK_XML_BASE "%s/%s"

	private:
	typedef map<string, bkBookmarkPos*> bkBookmarks;
	typedef map<string, bkBookmarkPos*>::iterator bkBookmarksIt;

	static void load(bkBookmarks& bookmarks, bool lastview);
	static void save(bkBookmarks& bookmarks, bkBookmarks& lastViewBookmarks);
	static void destroy(bkBookmarks& bookmarks);
	static void setPos(bkBookmarks& bookmarks, string& filename, int pos, bool pushback);

	public:
	// returns -1 if not found
	static int getLastView(string& filename);		
	static void getBookmarks(string& filename, bkBookmarkPos &pos);		
	static void set(string& filename, int position, bool lastview=false);

	// clears everything
	static void clear();
};

#endif

