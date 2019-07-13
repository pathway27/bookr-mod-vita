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

#include <tinyxml2.h>

#include "bookmark.hpp"

/*
<bookmarks>
	<file filename="">
		<lastview ... />
			<viewdata key="" value=""/>
			<viewdata key="" value=""/>
			<viewdata key="" value=""/>
			<viewdata key="" value=""/>
		</lastview>
		<bookmark ... >
			<viewdata key="" value=""/>
			<viewdata key="" value=""/>
			<viewdata key="" value=""/>
			<viewdata key="" value=""/>
			...
		</bookmark>
		<bookmark ... >
			<viewdata key="" value=""/>
			<viewdata key="" value=""/>
			<viewdata key="" value=""/>
			<viewdata key="" value=""/>
			...
		</bookmark>
	</file>
	<lastfile filename=""/>
</bookmarks>

*/
using namespace tinyxml2;

namespace bookr {

static XMLDocument *doc = 0;
static XMLElement *root = 0;

static void clearXML() {
	#ifdef DEBUG
		printf("clearXML\n");
	#endif

	if (doc != 0)
		delete doc;
	doc = new XMLDocument();

	root = doc->NewElement("bookmarks");
	root->SetAttribute("version", "2");
	doc->InsertFirstChild(root);
	
	char xmlfilename[1024];
	#ifdef __vita__
		snprintf(xmlfilename, 1024, "%s%s%s", FZScreen::basePath().c_str(), "data/Bookr/", BOOKMARK_XML);
	#else
		snprintf(xmlfilename, 1024, BOOKMARK_XML_BASE, FZScreen::basePath().c_str(), BOOKMARK_XML);
	#endif

	doc->SaveFile(xmlfilename);
}

static void loadXML() {
	#ifdef DEBUG
		printf("loadXML\n");
	#endif

	char xmlfilename[1024];
	
	#ifdef __vita__
		snprintf(xmlfilename, 1024, "%s%s%s", FZScreen::basePath().c_str(), "data/Bookr/", BOOKMARK_XML);
	#else
		snprintf(xmlfilename, 1024, BOOKMARK_XML_BASE, FZScreen::basePath().c_str(), BOOKMARK_XML);
	#endif

	if (doc != 0)
		delete doc;

	doc = new XMLDocument();
	doc->LoadFile(xmlfilename);

	if(doc->Error()) {
		// probably file not found, create an empty one
		printf("doc Error\n");
		clearXML();
	}

	root = doc->FirstChildElement("bookmarks");
	// check basic file structure
	if (root == 0) {
		printf("WARNING: corrupted bookmarks file\n");
		clearXML();
	}
	int v = 0;
	root->QueryIntAttribute("version", &v);
	if (v < 2) {
		printf("WARNING: bookmarks file version too old\n");
		clearXML();
	}
}

static void saveXML() {
	#ifdef DEBUG
		printf("saveXML\n");
	#endif

	char xmlfilename[1024];
	#ifdef __vita__
		snprintf(xmlfilename, 1024, "%s%s%s", FZScreen::basePath().c_str(), "data/Bookr/", BOOKMARK_XML);
	#else
		snprintf(xmlfilename, 1024, BOOKMARK_XML_BASE, FZScreen::basePath().c_str(), BOOKMARK_XML);
	#endif

	if (doc != 0) {
		doc->SaveFile(xmlfilename);
	}
}

static XMLNode* fileNode(string& filename) {
	#ifdef DEBUG
		printf("fileNode(%s)\n", filename.c_str());
	#endif

	#ifdef DEBUG
		printf("fileNode - pre load xml\n");
	#endif
	if (doc == 0)
		loadXML();

	#ifdef DEBUG
		printf("fileNode - post load xml\n");
	#endif
	XMLElement* file = root->FirstChildElement("file");
	#ifdef DEBUG
		printf("fileNode - file ass. xml\n");
	#endif
	while (file) {
		const char* name = file->Attribute("filename");
		#ifdef DEBUG
			printf("fileNode - %s\n", name);
		#endif
		if (name != 0) {
			if (strncmp(filename.c_str(), name, 1024) == 0)
				return file;
		}
		file = file->NextSiblingElement("file");
	}
	return 0;
}

static XMLElement* lastFileNode() {
	#ifdef DEBUG
		printf("lastFileNode\n");
	#endif

	if (doc == 0)
		loadXML();
	XMLElement* file = root->FirstChildElement("lastfile");
	return file;
}

static XMLNode* loadOrAddFileNode(string& filename) {
	#ifdef DEBUG
		printf("loadOrAddFileNode\n");
	#endif
	
	if (doc == 0)
		loadXML();
	XMLNode* file = fileNode(filename);
	if (file != 0)
		return file;

	XMLElement *efile = doc->NewElement("file");
	efile->SetAttribute("filename", filename.c_str());
	file = root->InsertEndChild(efile);
	return file;
}

static void loadBookmark(XMLNode* _bn, Bookmark& b) {
	#ifdef DEBUG
		printf("loadBookmark\n");
	#endif

	XMLElement* bn = _bn->ToElement();
	if (strncmp(bn->Value(), "lastview", 1024) == 0) {
		b.lastView = true;
	}
	b.title = bn->Attribute("title");
	int p = 0;
	bn->QueryIntAttribute("page", &p);
	b.page = p;
	b.createdOn = bn->Attribute("createdon");
	//int *thumbnail;
	map<string, float> viewData;
	XMLElement* vd = bn->FirstChildElement("viewdata");
	while (vd) {
		const char* key = vd->Attribute("key");
		int value = 0;
		vd->QueryIntAttribute("value", &value);
		b.viewData.insert(pair<string, int>(key, value));
		vd = vd->NextSiblingElement("viewdata");
	}
}

// find the last read bookmark for a given file
string BookmarksManager::getLastFile() {
	#ifdef DEBUG
		printf("BookmarksManager::getLastFile\n");
	#endif

	XMLElement* file = lastFileNode();
	if (file == 0)
		return string();
	return file->Attribute("filename");
}

// save last use file
void BookmarksManager::setLastFile(string& filename) {
	#ifdef DEBUG
		printf("BookmarksManager::setLastFile\n");
	#endif

	XMLElement* file = lastFileNode();
	if (file != 0)
	{
		file->SetAttribute("filename", filename.c_str());
	} else {
		XMLElement* efile =  doc->NewElement("lastfile");
		efile->SetAttribute("filename", filename.c_str());
		root->InsertEndChild(efile);
	}
	saveXML();
}

// find the last read bookmark for a given file
bool BookmarksManager::getLastView(string& filename, Bookmark& b) {
	#ifdef DEBUG
		printf("BookmarksManager::getLastView\n");
	#endif

	XMLNode* file = fileNode(filename);
	if (file == 0)
		return false;
	XMLNode* lastview = file->FirstChildElement("lastview");
	if (lastview == 0)
		return false;
	loadBookmark(lastview, b);
	return true;
}

// add a new bookmark for a file
static void addBookmarkProto(string& filename, Bookmark& b, XMLNode* file) {
	#ifdef DEBUG
		printf("addBookmarkProto\n");
	#endif

	XMLElement* bookmark = doc->NewElement(b.lastView ? "lastview" : "bookmark");
	bookmark->SetAttribute("title", b.title.c_str());
	bookmark->SetAttribute("page", b.page);
	bookmark->SetAttribute("createdon", b.createdOn.c_str());
	//bookmark->SetAttribute("zoomvalue", b.zoom);
	//bookmark.SetAttribute("thumbnail", );
	map<string, float>::iterator it(b.viewData.begin());
	#ifdef DEBUG
		printf("addBookmarkProto: title: %s page: %i\n", b.title.c_str(), b.page);
	#endif
	while (it != b.viewData.end()) {
		XMLElement *vd = doc->NewElement("viewdata");
		vd->SetAttribute("key", (*it).first.c_str());
		vd->SetAttribute("value", (*it).second);
		bookmark->InsertEndChild(vd);
		++it;
	}
	file->InsertEndChild(bookmark);
}

void BookmarksManager::addBookmark(string& filename, Bookmark& b) {
	#ifdef DEBUG
		printf("BookmarksManager::addBookmark\n");
	#endif

	XMLNode* file = loadOrAddFileNode(filename);
	if (b.lastView) {
		// remove previous lastview
		XMLNode* lastviewnode = file->FirstChildElement("lastview");
		if (lastviewnode != 0)
			file->DeleteChild(lastviewnode);
	}
	addBookmarkProto(filename, b, file);
	saveXML();
}

void BookmarksManager::removeBookmark(string& filename, int index) {
	#ifdef DEBUG
		printf("BookmarksManager::removeBookmark\n");
	#endif

	XMLNode* file = loadOrAddFileNode(filename);
	#ifdef DEBUG
		printf("BookmarksManager::removeBookmark - loadOrAddFileNode\n");
	#endif
	XMLNode* nodeToDelete = file->FirstChildElement("bookmark");
	#ifdef DEBUG
		printf("BookmarksManager::removeBookmark - FirstChildElement\n");
	#endif
	int count = 0;
	while(count != index) {
		nodeToDelete = nodeToDelete->NextSibling();
		count++;
	}
	doc->DeleteNode(nodeToDelete);
	saveXML();
}

// load all the bookmarks for a given file
void BookmarksManager::getBookmarks(string& filename, BookmarkList &bl) {
	#ifdef DEBUG
		printf("BookmarksManager::getBookmarks\n");
	#endif

	XMLNode* file = loadOrAddFileNode(filename);
	#ifdef DEBUG
		printf("BookmarksManager::getBookmarks - post filenode\n");
	#endif
	if (file == 0){
		#ifdef DEBUG
			printf("BookmarksManager::getBookmarks - file == 0 %i\n", file);
		#endif
		return;
	}
	XMLElement* bookmark = file->FirstChildElement();
	while (bookmark != 0) {
		if (strncmp(bookmark->Value(), "bookmark", 1024) == 0) {
			Bookmark b;
			loadBookmark(bookmark, b);
			bl.push_back(b);
		}
		bookmark = bookmark->NextSiblingElement();
	}
}

// save all the bookmarks for a given file, overwriting the existing ones
void BookmarksManager::setBookmarks(string& filename, BookmarkList &bl) {
	#ifdef DEBUG
		printf("BookmarksManager::setBookmarks\n");
	#endif

	Bookmark lastView;
	bool lv = getLastView(filename, lastView);
	XMLNode* file = loadOrAddFileNode(filename);
	file->GetDocument()->Clear();
	if (lv) addBookmarkProto(filename, lastView, file);
	BookmarkListIt it(bl.begin());
	while (it != bl.end()) {
		addBookmarkProto(filename, *it, file);
		++it;
	}
	saveXML();
	doc = 0;
}

void BookmarksManager::clear() {
	#ifdef DEBUG
		printf("BookmarksManager::clear\n");
	#endif

	clearXML();
}

}