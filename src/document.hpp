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

#ifndef BKDOCUMENT_H
#define BKDOCUMENT_H
#define BKDOC_VIEW 0
#define BKDOC_TOOLBAR 1
#define BKDOCUMENT_ZOOMTYPE_ABSOLUTE 0
#define BKDOCUMENT_ZOOMTYPE_LARGER_TEXT 1
#define BKDOCUMENT_ZOOMTYPE_SMALLER_TEXT 2

#include <map>
#include <string>

#include "layer.hpp"
#include "bookmark.hpp"

using std::string;

namespace bookr {

// BKDocument - an unified interface for any page-able, scroll-able,
// bookmark-able file.
class Document : public Layer {
private:
	int lastSuspendSerial;
	int mode;

	int processEventsForView();
	int processEventsForToolbar();

	int bannerFrames;
	string banner;
	int tipFrames;

	struct ToolbarItem {
		int lines;
		int minWidth;
		string label;
		string iconName;
		string circleLabel;
		string triangleLabel;
		string botLabelLeft;
		string botLabelRight;
		int iconX, iconY, iconW, iconH;
		ToolbarItem(string label = "", string iconName = "", string circleLabel = "", string triangleLabel = "")
		 : lines(1), minWidth(100),
		 label(label), iconName(iconName), circleLabel(circleLabel), triangleLabel(triangleLabel) { }
	};

	int toolbarSelMenu;
	int toolbarSelMenuItem;

	vector<ToolbarItem> toolbarMenus[4];
	BookmarkList bookmarkList;
	void buildToolbarMenus();

	int frames;

protected:
	Document();
	virtual void saveLastView();
	~Document();

public:
	// BKLayer::update is implemented outside the document viewers.
	// The viewers receive the "cooked" events via the is/get/set
	// APIs. Of course they can still override this method if neeeded
	// but then they must call BKDocument::update(buttons) before they
	// start processing.
	virtual int update(unsigned int buttons);

	// "Blind" update method for the viwers that need it.
	virtual int updateContent() = 0;

	// Notify the view of a power resume event
	virtual int resume() = 0;

	// BKDocument has its own UI shell for the toolbar and labels.
	virtual void render();

	// Render event for the viewers.
	virtual void renderContent() = 0;

	// Factory with file detection
	static Document* create(string filePath);

	// Document metadata
	virtual void getFileName(string&) = 0;
	virtual void getTitle(string&) = 0;
	virtual void getType(string&) = 0;

	// Pagination - a page is clearly defined for a PDF file, but
	// in the case of a text file is just an arbitrary division
	// based on screen size
	virtual bool isPaginated() = 0;
	virtual int getTotalPages() = 0;
	virtual int getCurrentPage() = 0;
	virtual int setCurrentPage(int) = 0;

	// Zoom
	// The type field is a hint for the shell UI to select an
	// apropiate icon.
	struct ZoomLevel {
		int type;
		string label;
		ZoomLevel(int t, const char* l) : type(t), label(l) { }
	};
	virtual bool isZoomable() = 0;
	virtual void getZoomLevels(vector<Document::ZoomLevel>& v) = 0;
	virtual int getCurrentZoomLevel() = 0;
	virtual int setZoomLevel(int) = 0;
	virtual bool hasZoomToFit() = 0;
	virtual int setZoomToFitWidth() = 0;
	virtual int setZoomToFitHeight() = 0;

	// Analog pad paning - can be ignored
	virtual int pan(int, int) = 0;

	// digital paning - 100% view defined, unlike paging
	virtual int screenUp() = 0;
	virtual int screenDown() = 0;
	virtual int screenLeft() = 0;
	virtual int screenRight() = 0;

	// Rotation (0 = 0deg, 1 = 90deg, 2 = 180deg, 3 = 240deg)
	virtual bool isRotable() = 0;
	virtual int getRotation() = 0;
	virtual int setRotation(int, bool bForce=false) = 0;

	// Bookmark support. The returned map is a black box
	// for the bookmarking system.
	virtual bool isBookmarkable() = 0;
	virtual void getBookmarkPosition(map<string, float>&) = 0;
	virtual int setBookmarkPosition(map<string, float>&) = 0;

	// banners
	void setBanner(char*);
};

}

#endif
