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

#include "tinyxml.h"

#include "bkbookmark.h"
#include <set>

void BKBookmark::clear() {
	char xmlfilename[1024];
	snprintf(xmlfilename, 1024, BOOKMARK_XML_BASE, FZScreen::basePath(), BOOKMARK_XML);

	FILE* f = fopen(xmlfilename, "w");
	if (f == NULL)
		return;
	fclose(f);
}

void BKBookmark::set(string& filename, int position, bool lastview) {
	char xmlfilename[1024];
	snprintf(xmlfilename, 1024, BOOKMARK_XML_BASE, FZScreen::basePath(), BOOKMARK_XML);

	if (filename.compare(xmlfilename) == 0) {
		return;
	}

	bkBookmarks bookmarks, bookmarksLastview;
	load(bookmarks, false);
	load(bookmarksLastview, true);

	setPos(lastview ? bookmarksLastview : bookmarks, filename, position, true);
	save(bookmarks, bookmarksLastview);

	destroy(bookmarks);
	destroy(bookmarksLastview);
}

void BKBookmark::setPos(bkBookmarks& bookmarks, string& filename, int p, bool pushback) {
	bkBookmarksIt it(bookmarks.find(filename));
	bkBookmarkPos *pos = NULL;

	if (it == bookmarks.end()) {
		pos = new bkBookmarkPos;
		bookmarks[filename] = pos;
	} else {
		pos = it->second;
	}

	if (pushback)
		pos->push_back(p);
	else
		pos->insert(pos->begin(), p);
}

int BKBookmark::getLastView(string& filename) {
	char xmlfilename[1024];
	snprintf(xmlfilename, 1024, BOOKMARK_XML_BASE, FZScreen::basePath(), BOOKMARK_XML);

	if (filename.compare(xmlfilename) == 0) {
		return -1;
	}

	int pos = -1;
	bkBookmarks bookmarks;
	load(bookmarks, true);
	
	bkBookmarksIt it(bookmarks.find(filename));
	if (it != bookmarks.end())
		if (it->second->size() > 0)
			pos = it->second->front();

	destroy(bookmarks);
	return pos;
}

void BKBookmark::getBookmarks(string& filename, bkBookmarkPos &pos) {
	char xmlfilename[1024];
	snprintf(xmlfilename, 1024, BOOKMARK_XML_BASE, FZScreen::basePath(), BOOKMARK_XML);

	if (filename.compare(xmlfilename) == 0) {
		return;
	}

	bkBookmarks bookmarks;
	load(bookmarks, false);

	bkBookmarksIt it(bookmarks.find(filename));
	if (it != bookmarks.end())
		pos = *(it->second);

	destroy(bookmarks);
}

void BKBookmark::save(bkBookmarks &bookmarks, bkBookmarks& lastViewBookmarks) {
	char xmlfilename[1024];
	snprintf(xmlfilename, 1024, BOOKMARK_XML_BASE, FZScreen::basePath(), BOOKMARK_XML);

	FILE* f = fopen(xmlfilename, "w");
	if (f == NULL)
		return;

	fprintf(f, "<?xml version=\"1.0\" standalone=\"no\" ?>\n");
	fprintf(f, "<bookmarks>\n");

	bkBookmarksIt it(bookmarks.begin());
	bkBookmarksIt end(bookmarks.end());

	while (it != end) {
		// newest bookmarks are saved at the end
		vector<int>::reverse_iterator posIt(it->second->rbegin());
		vector<int>::reverse_iterator posEnd(it->second->rend());
		std::set<int> saved;

		while (posIt != posEnd) {
			if (saved.find((*posIt)) == saved.end()) {
				saved.insert((*posIt));

				// we will save only the latest 5 bookmarks
				if (saved.size() > MAX_BOOKMARKS_PER_FILE)
					break;
				fprintf(f, "\t<bookmark filename=\"%s\" position=\"%d\" />\n", it->first.c_str(), (*posIt));
			}
			++posIt;
		}
		++it;
	}

	it = lastViewBookmarks.begin();
	end = lastViewBookmarks.end();

	while (it != end) {
		vector<int>::reverse_iterator posIt(it->second->rbegin());
		vector<int>::reverse_iterator posEnd(it->second->rend());

		while (posIt != posEnd) {
			fprintf(f, "\t<bookmark filename=\"%s\" position=\"%d\" lastview=\"1\"/>\n", it->first.c_str(), (*posIt));
			break;
		}
		++it;
	}

	fprintf(f, "</bookmarks>\n");

	fclose(f);
}

void BKBookmark::load(bkBookmarks &bookmarks, bool wantLastview) {
	char xmlfilename[1024];
	snprintf(xmlfilename, 1024, BOOKMARK_XML_BASE, FZScreen::basePath(), BOOKMARK_XML);
	FILE* f = fopen(xmlfilename, "r");
	// no existing preferences, probably the first time run of the app
	if (f == NULL)
		return;

	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	char* buffer = (char*)malloc(size + 1);
	memset((void*)buffer, 0, size + 1);
	fseek(f, 0, SEEK_SET);
	fread(buffer, size, 1, f);
	fclose(f);
	buffer[size] = 0;

	TiXmlDocument *doc = new TiXmlDocument();
	doc->Parse(buffer);

	if(doc->Error()) {
		// Probably file not found
		//printf("invalid %s, cannot load bookmarks: %s, %d\n", xmlfilename, doc->ErrorDesc(), size);
		delete doc;
		return;
	}

	TiXmlElement* root = doc->RootElement();
	TiXmlElement* bookmark = root->FirstChildElement("bookmark");

	while (bookmark) {
		const char* filename = bookmark->Attribute("filename");
		const char* position = bookmark->Attribute("position");
		const char* lastview = bookmark->Attribute("lastview");
		bookmark = bookmark->NextSiblingElement("bookmark"); 
		
		if (filename == 0 || position == 0) {			
			continue;
		}

		string fn(filename);
		if (wantLastview) {
			if (lastview) {
				// found what we want
				setPos(bookmarks, fn, atoi(position), true);
			}
		} else if (!lastview) {
			setPos(bookmarks, fn, atoi(position), false);
		}
	}

	doc->Clear();
	delete doc;
}

void BKBookmark::destroy(bkBookmarks &bookmarks) {
	bkBookmarksIt it(bookmarks.begin());
	bkBookmarksIt end(bookmarks.end());
	
	while (it != end) {
		delete it->second;
		++it;
	}
	
	bookmarks.clear();
}
