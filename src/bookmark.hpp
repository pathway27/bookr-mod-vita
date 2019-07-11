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
