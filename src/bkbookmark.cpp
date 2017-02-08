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

#include "deps/tinyxml/tinyxml.h"

#include "bkbookmark.h"

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
	</file>
	<lastfile filename=""/>
</bookmarks>

*/

static TiXmlDocument* doc = 0;

static void freedoc(){
  
  delete doc;
  doc = (TiXmlDocument*)0;
} 

static void clearXML() {
	if (doc != 0)
	  freedoc();
	doc = new TiXmlDocument();
	TiXmlElement e("bookmarks");
	e.SetAttribute("version", "2");
	doc->InsertEndChild(e);
	
	char xmlfilename[1024];
	snprintf(xmlfilename, 1024, BOOKMARK_XML_BASE, FZScreen::basePath().c_str(), BOOKMARK_XML);

	doc->SaveFile(xmlfilename);
}

static void loadXML() {
	char xmlfilename[1024];
	snprintf(xmlfilename, 1024, BOOKMARK_XML_BASE, FZScreen::basePath().c_str(), BOOKMARK_XML);

	if (doc != 0)
	  freedoc();

	doc = new TiXmlDocument();
	doc->LoadFile(xmlfilename);

	if(doc->Error()) {
		// probably file not found, create an empty one
		clearXML();
		return;
	}

	// check basic file structure
	TiXmlElement* root = doc->FirstChildElement("bookmarks");
	if (root == 0) {
		printf("WARNING: corrupted bookmarks file\n");
		clearXML();
	        return;
	}
	int v = 0;
	root->QueryIntAttribute("version", &v);
	if (v < 2) {
		printf("WARNING: bookmarks file version too old\n");
		clearXML();
	}
}

static void saveXML() {
	char xmlfilename[1024];
	snprintf(xmlfilename, 1024, BOOKMARK_XML_BASE, FZScreen::basePath().c_str(), BOOKMARK_XML);

	if (doc != 0) {
		doc->SaveFile(xmlfilename);
	}
}

static TiXmlNode* fileNode(string& filename) {
	if (doc == 0)
		loadXML();
	TiXmlElement* root = doc->RootElement();
	TiXmlElement* file = root->FirstChildElement("file");
	while (file) {
		const char* name = file->Attribute("filename");
		if (name != 0) {
			if (strncmp(filename.c_str(), name, 1024) == 0)
				return file;
		}
		file = file->NextSiblingElement("file");
	}
	return 0;
}

static TiXmlElement* lastFileNode() {
	if (doc == 0)
		loadXML();
	TiXmlElement* root = doc->RootElement();
	TiXmlElement* file = root->FirstChildElement("lastfile");
	return file;
}

static TiXmlNode* loadOrAddFileNode(string& filename) {
	if (doc == 0)
		loadXML();
	TiXmlNode* file = fileNode(filename);
	if (file != 0)
		return file;
	TiXmlElement* root = doc->RootElement();
	TiXmlElement efile("file");
	efile.SetAttribute("filename", filename.c_str());
	file = root->InsertEndChild(efile);
	return file;
}

static void loadBookmark(TiXmlNode* _bn, BKBookmark& b) {
	TiXmlElement* bn = (TiXmlElement*)_bn;
	if (strncmp(bn->Value(), "lastview", 1024) == 0) {
		b.lastView = true;
	}
	b.title = bn->Attribute("title");
	int p = 0;
	bn->QueryIntAttribute("page", &p);
	b.page = p;
	float z = 0.0f;
	bn->QueryFloatAttribute("zoomvalue",&z);
	b.zoom = z;
	b.createdOn = bn->Attribute("createdon");
	//int *thumbnail;
	map<string, int> viewData;
	TiXmlElement* vd = bn->FirstChildElement("viewdata");
	while (vd) {
		const char* key = vd->Attribute("key");
		int value = 0;
		vd->QueryIntAttribute("value", &value);
		b.viewData.insert(pair<string, int>(key, value));
		vd = vd->NextSiblingElement("viewdata");
	}
}

// find the last read bookmark for a given file
void BKBookmarksManager::getLastFile(string& filename, string& longFileName) {
	TiXmlElement* file = lastFileNode();
	  if (file == 0){
		longFileName = filename = string();
	  }
	filename =  file->Attribute("filename");
	const char* lfn = file->Attribute("longfilename");
	if (lfn)
	  longFileName = lfn;
	else
	  longFileName = filename;
}

// save last use file
void BKBookmarksManager::setLastFile(string& filename, string& longFileName) {
	TiXmlElement* file = lastFileNode();
	if (file != 0)
	{
		file->SetAttribute("filename", filename.c_str());
		file->SetAttribute("longfilename", longFileName.c_str());
	} else {
		TiXmlElement* root = doc->RootElement();
		TiXmlElement efile("lastfile");
		efile.SetAttribute("filename", filename.c_str());
		efile.SetAttribute("longfilename", longFileName.c_str());
		root->InsertEndChild(efile);
	}
	saveXML();
}

// find the last read bookmark for a given file
bool BKBookmarksManager::getLastView(string& filename, BKBookmark& b) {
	TiXmlNode* file = fileNode(filename);
	if (file == 0)
		return false;
	TiXmlNode* lastview = file->FirstChild("lastview");
	if (lastview == 0)
		return false;
	loadBookmark(lastview, b);
	return true;
}

// add a new bookmark for a file
static void addBookmarkProto(string& filename, BKBookmark& b, TiXmlNode* file) {
	TiXmlElement bookmark(b.lastView ? "lastview" : "bookmark");
	bookmark.SetAttribute("title", b.title.c_str());
	bookmark.SetAttribute("page", b.page);
	bookmark.SetAttribute("createdon", b.createdOn.c_str());
	bookmark.SetDoubleAttribute("zoomvalue", b.zoom);
	//bookmark.SetAttribute("thumbnail", );
	map<string, int>::iterator it(b.viewData.begin());
	while (it != b.viewData.end()) {
		TiXmlElement vd("viewdata");
		vd.SetAttribute("key", (*it).first.c_str());
		vd.SetAttribute("value", (*it).second);
		bookmark.InsertEndChild(vd);
		++it;
	}
	file->InsertEndChild(bookmark);
}
void BKBookmarksManager::addBookmark(string& filename, BKBookmark& b) {
	TiXmlNode* file = loadOrAddFileNode(filename);
	if (b.lastView) {
		// remove previous lastview
		TiXmlNode* lastviewnode = file->FirstChild("lastview");
		if (lastviewnode != 0)
			file->RemoveChild(lastviewnode);
	}
	addBookmarkProto(filename, b, file);
	saveXML();
}

// load all the bookmarks for a given file
void BKBookmarksManager::getBookmarks(string& filename, BKBookmarkList &bl) {
	TiXmlNode* file = fileNode(filename);
	if (file == 0)
		return;
	TiXmlElement* bookmark = file->FirstChildElement();
	while (bookmark != 0) {
		if (strncmp(bookmark->Value(), "bookmark", 1024) == 0) {
			BKBookmark b;
			loadBookmark(bookmark, b);
			bl.push_back(b);
		}
		bookmark = bookmark->NextSiblingElement();
	}
}

// save all the bookmarks for a given file, overwriting the existing ones
void BKBookmarksManager::setBookmarks(string& filename, BKBookmarkList &bl) {
	BKBookmark lastView;
	bool lv = getLastView(filename, lastView);
	TiXmlNode* file = loadOrAddFileNode(filename);
	file->Clear();
	if (lv) addBookmarkProto(filename, lastView, file);
	BKBookmarkListIt it(bl.begin());
	while (it != bl.end()) {
		addBookmarkProto(filename, *it, file);
		++it;
	}
	saveXML();
}

void BKBookmarksManager::clear() {
	clearXML();
}

void BKBookmarksManager::free(){
  freedoc();
}

static bool fileExists(const char* filename){
  if(!filename)
    return false;
#ifdef PSP
  int fd = sceIoOpen(filename,PSP_O_RDONLY,0666);
  if(fd <= 0){
    return false;
  }
  sceIoClose(fd);
  return true;
#elif __vita__
	int fd = sceIoOpen(filename,SCE_O_RDONLY,0666);
  if(fd <= 0){
    return false;
  }
  sceIoClose(fd);
  return true;
#else
  int fd = open(filename, 0, 0666);
  if(fd <= 0){
    return false;
  }
  close(fd);
  return true;
#endif
}

int BKBookmarksManager::prune(){
	int count = 0;
	if (doc == 0)
		loadXML();
	TiXmlElement* root = doc->RootElement();
	TiXmlElement* file = root->FirstChildElement("file");
	TiXmlElement* next;
	while (file) {
		next = file->NextSiblingElement("file");
		const char* name = file->Attribute("filename");
		if (!fileExists(name)) {
		  root->RemoveChild(file);
		  count++;
		}
		file = next;
	}
	saveXML();
	return count;
}

