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

void BKBookmark::clear() {
	char xmlfilename[1024];
	snprintf(xmlfilename, 1024, "%s/data/%s", FZScreen::basePath(), "bookmark.xml");

	FILE* f = fopen(xmlfilename, "w");
	if (f == NULL)
		return;
	fclose(f);
}

void BKBookmark::set(string& filename, int position) {
	char xmlfilename[1024];
	snprintf(xmlfilename, 1024, "%s/data/%s", FZScreen::basePath(), "bookmark.xml");

	if (filename.compare(xmlfilename) == 0) {
		return;
	}
		
	bkBookmarks bookmarks;
	load(bookmarks);
	
	FILE* f = fopen(xmlfilename, "w");	
	if (f == NULL)
		return;
	
	bookmarks[filename] = position;

	fprintf(f, "<?xml version=\"1.0\" standalone=\"no\" ?>\n");
	fprintf(f, "<bookmarks>\n");	
	
	bkBookmarksIt it(bookmarks.begin());
	bkBookmarksIt end(bookmarks.end());
	
	while (it != end) {		
		fprintf(f, "\t<bookmark filename=\"%s\" position=\"%d\" />\n", it->first.c_str(), it->second);
		++it;
	}	

	fprintf(f, "</bookmarks>\n");	

	fclose(f);
}

int BKBookmark::get(string& filename) {
	char xmlfilename[1024];
	snprintf(xmlfilename, 1024, "%s/data/%s", FZScreen::basePath(), "bookmark.xml");
	
	if (filename.compare(xmlfilename) == 0) {
		return -1;
	}

	bkBookmarks bookmarks;
	load(bookmarks);
	
	bkBookmarksIt it(bookmarks.find(filename));
	if (it != bookmarks.end())
		return it->second;
	else
		return -1;
}

void BKBookmark::load(bkBookmarks &bookmarks) {
	char xmlfilename[1024];
	snprintf(xmlfilename, 1024, "%s/data/%s", FZScreen::basePath(), "bookmark.xml");
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
		bookmark = bookmark->NextSiblingElement("bookmark"); 
		
		if (filename == 0 || position == 0) {			
			continue;
		}
		int p = atoi(position);
		bookmarks[filename] = p;		
	}

	doc->Clear();
	delete doc;
}

