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
#define MAX_BOOKMARKS_PER_FILE 20
#define BOOKMARK_XML "bookmark.xml"
#define BOOKMARK_XML_BASE "%s/%s"

#include "graphics/screen.hpp"

using std::string;

namespace bookr {

struct Bookmark {
	// common data fields for the menu. only the file parameter is actualy used by
	// the views
	string title;
	int page;
	string createdOn;
	int *thumbnail;
	bool lastView;
	// the view-specific data. this is a black box for the bookmarks manager
	map<string, float> viewData;
	Bookmark() : page(0), thumbnail(0), lastView(false) { }
};

typedef vector<Bookmark> BookmarkList;
typedef BookmarkList::iterator BookmarkListIt;

class BookmarksManager {
public:
	// find the last file used
	static string getLastFile();
	static void	setLastFile( string& filename );

	// find the last read bookmark for a given file
	static bool getLastView(string& filename, Bookmark&);
	// load all the bookmarks for a given file
	static void getBookmarks(string& filename, BookmarkList &);
	// save all the bookmarks for a given file, overwriting the existing ones
	static void setBookmarks(string& filename, BookmarkList&);
	// add a new bookmark for a file
	static void addBookmark(string& filename, Bookmark& b);
	// remove a bookmark for a file
	static void removeBookmark(string& filename, int index);
	// clears everything
	static void clear();
};

}

#endif
