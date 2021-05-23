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

#ifdef __vita__
  #include <vita2d.h>
#else
  #include "resource_manager.hpp"
#endif

#include "ui/colours.hpp"
#include "graphics/resolutions.hpp"
#include "graphics/controls.hpp"
#include "document.hpp"
#include "filetypes/mudocument.hpp"
// #include "filetypes/djvu.hpp"
// #include "filetypes/palmdoc.hpp"
#include "filetypes/plaintext.hpp"

namespace bookr {

Document* Document::create(string filePath) {
  #ifdef DEBUG
    printf("Document::create %s\n", filePath.c_str());
  #endif
  Document* doc = nullptr;

  // TODO(refactor): open file handle only once and send
  if (MUDocument::isMUDocument(filePath)) {
      doc = MUDocument::create(filePath);
  // } else if (DJVU::isDJVU(filePath)) {
  // 	doc = DJVU::create(filePath);
  // } else if (PalmDoc::isPalmDoc(filePath)) {
  //  doc = PalmDoc::create(filePath);
  } else if (PlainText::isPlainText(filePath)) {
    doc = PlainText::create(filePath);
  } else {
    #ifdef DEBUG
      printf("not accepted type\n");
    #endif
    throw "File not supported";
  }
  
  if (doc == nullptr) {
    printf("NULLPTR\n");
    return doc;
  }
    

  #ifdef DEBUG
    printf("doc created, not nullptr \n");
  #endif
  
  doc->buildToolbarMenus();

  if (doc->isBookmarkable()) {
    Bookmark b;
    string fn;
    doc->getFileName(fn);
    if (BookmarksManager::getLastView(fn, b)) {
      #ifdef DEBUG
        printf("bookmark: page %i\n", b.page);
      #endif

      doc->setBookmarkPosition(b.viewData);
    }
  }

  return doc;
}

Document::Document() : 
  mode(BKDOC_VIEW), bannerFrames(0), banner(""), 	tipFrames(120), toolbarSelMenu(0),
  toolbarSelMenuItem(0), frames(0)
{
  lastSuspendSerial = Screen::getSuspendSerial();
}

Document::~Document() {
}

void Document::saveLastView() {
  if (isBookmarkable()) {
    string fn, t;
    getFileName(fn);
    getTitle(t);
    Bookmark b;
    b.title = t;
    b.page = isPaginated() ? getCurrentPage() : 0;
    b.createdOn = "to do: creation date";
    b.lastView = true;
    getBookmarkPosition(b.viewData);
    BookmarksManager::addBookmark(fn, b);
    BookmarksManager::setLastFile(fn);
  }
}

void Document::setBanner(char* b) {
  banner = b;
  bannerFrames = 60;
}

int Document::update(unsigned int buttons) {
  // let the view quit update processing early for some special events
  #ifdef DEBUG_RENDER
    printf("Document::update\n");
  #endif
  if (lastSuspendSerial != Screen::getSuspendSerial()) {
    #ifdef DEBUG_RENDER
      printf("lastSuspendSerial != Screen::getSuspendSerial()\n");
    #endif
    lastSuspendSerial = Screen::getSuspendSerial();
    int r = resume();
    if (r != 0)
      return r;
  }

  #ifdef DEBUG_RENDER
    printf("Document::updateContent pre\n");
  #endif

  int r = updateContent();
  if (r != 0)
    return r;
  #ifdef DEBUG_RENDER
    printf("Document::updateContent post\n");
  #endif

  bannerFrames--;
  tipFrames--;
  if (bannerFrames < 0)
    bannerFrames = 0;
  if (tipFrames < 0)
    tipFrames = 0;

  // banner fade - this blocks event input during the fade
  //if (bannerFrames > 0)
  //	return BK_CMD_MARK_DIRTY;

  r = 0;
  if (mode == BKDOC_VIEW)
    r = processEventsForView();
  else
    r = processEventsForToolbar();

  // banner fade - this allows events during the fade
  if (bannerFrames > 0 && r == 0)
    r = BK_CMD_MARK_DIRTY;
  if (tipFrames > 0 && r == 0)
    r = BK_CMD_MARK_DIRTY;

  // clock tick
  frames++;
  if (frames % 60 == 0 && r == 0 && mode != BKDOC_VIEW)
    r = BK_CMD_MARK_DIRTY;

  #ifdef DEBUG_RENDER
    printf("Document::updateContent - done\n");
  #endif

  return r;
}

int Document::processEventsForView() {
  #ifdef DEBUG_RENDER
    printf("Document::processEventsForView - start\n");
  #endif
  int* b = Screen::ctrlReps();

  // button handling - pagination
  if (isPaginated()) {
    #ifdef DEBUG_RENDER
      printf("Document::processEventsForView - paginated - start\n");
    #endif
    // int n = getTotalPages();
    int p = getCurrentPage();
    int op = p;
    if (b[User::controls.nextPage] == 1) {
      p++;
    }
    if (b[User::controls.previousPage] == 1) {
      p--;
    }
    if (b[User::controls.next10Pages] == 1) {
      p += 10;
    }
    if (b[User::controls.previous10Pages] == 1) {
      p -= 10;
    }
    int r = 0;
    if (op != p)
      setCurrentPage(p);
    #ifdef DEBUG_RENDER
      printf("Document::processEventsForView - paginated - end\n");
    #endif
    if (r != 0)
      return r;
  }

  // button handling - zoom
  if (isZoomable()) {
    vector<ZoomLevel> zooms;
    getZoomLevels(zooms);
    int z = getCurrentZoomLevel();
    if (b[User::controls.zoomIn] == 1) {
      z++;
    }
    if (b[User::controls.zoomOut] == 1) {
      z--;
    }
    int r = setZoomLevel(z);
    if (r != 0)
      return r;
  }

  // button handling - analog pad panning
  {
    int ax = 0, ay = 0;
    Screen::getAnalogPad(ax, ay);
    int r = pan(ax, ay);
    if (r != 0)
      return r;
  }

  // button handling - digital panning
  {
    if (b[User::controls.screenUp] == 1 || b[User::controls.screenUp] > 20) {
      int r = screenUp();
      if (r != 0)
        return r;
    }
    if (b[User::controls.screenDown] == 1 || b[User::controls.screenDown] > 20) {
      int r = screenDown();
      if (r != 0)
        return r;
    }
    if (b[User::controls.screenLeft] == 1 || b[User::controls.screenLeft] > 20) {
      int r = screenLeft();
      if (r != 0)
        return r;
    }
    if (b[User::controls.screenRight] == 1 || b[User::controls.screenRight] > 20) {
      int r = screenRight();
      if (r != 0)
        return r;
    }
  }

  // button handling - rotation - TO DO
  /**/

  // bookmarks and other features are not supported by mapeable keys

  // main menu
  if (b[User::controls.showMainMenu] == 1) {
    return BK_CMD_INVOKE_MENU;
  }

  // toolbar
  if (b[User::controls.showToolbar] == 1) {
    mode = BKDOC_TOOLBAR;
    return BK_CMD_MARK_DIRTY;
  }

  #ifdef DEBUG_RENDER
    printf("Document::processEventsForView - end\n");
  #endif
  return 0;
}

void Document::buildToolbarMenus() {
  #ifdef DEBUG
		printf("BookmarksManager::buildToolbarMenus\n");
	#endif

  toolbarMenus[0].clear();
  if (isBookmarkable()) {
    ToolbarItem i("Add bookmark", "bk_add_bookmark_icon", "Select");
    toolbarMenus[0].push_back(i);

    string fn;
    getFileName(fn);
    bookmarkList.clear();
    BookmarksManager::getBookmarks(fn, bookmarkList);
    BookmarkListIt it(bookmarkList.begin());
    char t[256];
    while (it != bookmarkList.end()) {
      const Bookmark& b = *it;
      snprintf(t, 256, "Page %d", b.page);
      i.label = t;
      i.circleLabel = "Jump to";
      i.triangleLabel = "Delete";
      toolbarMenus[0].push_back(i);
      ++it;
    }
  } else {
    ToolbarItem i("No bookmark support");
    toolbarMenus[0].push_back(i);
  }

  toolbarMenus[1].clear();
  if (isPaginated()) {
    ToolbarItem i = ToolbarItem("First page", "bk_first_page_icon", "Select");
    toolbarMenus[1].push_back(i);

    i = ToolbarItem("Last page", "bk_last_page_icon", "Select");
    toolbarMenus[1].push_back(i);

    i = ToolbarItem("Previous 10 pages", "bk_prev_ten_icon", "Select");
    toolbarMenus[1].push_back(i);

    i = ToolbarItem("Next 10 pages", "bk_next_ten_icon", "Select");
    toolbarMenus[1].push_back(i);

    // i = ToolbarItem("Go to page", "bk_go_to_page_icon", "Select");
    // toolbarMenus[1].push_back(i);
  } else {
    ToolbarItem i("No pagination support");
    toolbarMenus[1].push_back(i);
  }

  toolbarMenus[2].clear();
  if (isZoomable()) {
    ToolbarItem i;
    i.circleLabel = "Select";

    if (hasZoomToFit()) {
      i.label = "Fit height";
      i.iconName = "bk_fit_height_icon";
      toolbarMenus[2].push_back(i);

      i.label = "Fit width";
      i.iconName = "bk_fit_width_icon";
      toolbarMenus[2].push_back(i);
    }

    i.label = "Zoom out";
    i.iconName = "bk_zoom_out_icon";
    toolbarMenus[2].push_back(i);

    i.label = "Zoom in";
    i.iconName = "bk_zoom_in_icon";
    toolbarMenus[2].push_back(i);
  } else {
    ToolbarItem i("No zoom support");
    toolbarMenus[2].push_back(i);
  }

  toolbarMenus[3].clear();
  if (isRotable()) {
    ToolbarItem i;
    i.label = "Rotate 90° clockwise";
    i.circleLabel = "Select";
    i.iconName = "bk_rotate_right_icon";
    toolbarMenus[3].push_back(i);

    i.label = "Rotate 90° counterclockwise";
    i.iconName = "bk_rotate_left_icon";
    toolbarMenus[3].push_back(i);
  } else {
    ToolbarItem i("No rotation support");
    toolbarMenus[3].push_back(i);
  }
}

int Document::processEventsForToolbar() {
  int* b = Screen::ctrlReps();

  if (b[User::controls.menuUp] == 1 || b[User::controls.menuUp] > 20) {
    toolbarSelMenuItem++;
  }
  if (b[User::controls.menuDown] == 1 || b[User::controls.menuDown] > 20) {
    toolbarSelMenuItem--;
  }
  if (b[User::controls.menuLeft] == 1 || b[User::controls.menuLeft] > 20) {
    toolbarSelMenu--;
  }
  if (b[User::controls.menuRight] == 1 || b[User::controls.menuRight] > 20) {
    toolbarSelMenu++;
  }

  // wrap menu indexes
  if (toolbarSelMenu >= 4)
    toolbarSelMenu = 0;
  if (toolbarSelMenu < 0)
    toolbarSelMenu = 3;
  if (toolbarSelMenuItem >= (int)toolbarMenus[toolbarSelMenu].size())
    toolbarSelMenuItem = 0;
  if (toolbarSelMenuItem < 0)
    toolbarSelMenuItem = toolbarMenus[toolbarSelMenu].size() - 1;

  if (b[User::controls.alternate] == 1) {
    // delete bookmark
    // TODO: deleting first bookmark when there's more than one = crash
    //       deletes all bookmarks...?
    if (toolbarSelMenu == 0 && toolbarSelMenuItem > 0 && isBookmarkable()) {
      string fn;
      getFileName(fn);
      int di = toolbarSelMenuItem - 1;
      printf("delete bookmark: %i\n", di);
      BookmarksManager::removeBookmark(fn, di);
      buildToolbarMenus();
      return BK_CMD_MARK_DIRTY;
    }
  }

  if (b[User::controls.select] == 1) {
    // add bookmark
    if (toolbarSelMenu == 0 && toolbarSelMenuItem == 0 && isBookmarkable()) {
      string fn, t;
      getFileName(fn);
      getTitle(t);
      Bookmark b;
      b.title = t;
      b.page = isPaginated() ? getCurrentPage() : 0;
      b.createdOn = "to do: creation date";
      getBookmarkPosition(b.viewData);
      BookmarksManager::addBookmark(fn, b);
      buildToolbarMenus();
      return BK_CMD_MARK_DIRTY;
    }
    // jump to bookmark
    if (toolbarSelMenu == 0 && toolbarSelMenuItem > 0 && isBookmarkable()) {
      int di =  toolbarSelMenuItem - 1;
      return setBookmarkPosition(bookmarkList[di].viewData);
    }
    // first page
    if (toolbarSelMenu == 1 && toolbarSelMenuItem == 0 && isPaginated()) {
      int r = setCurrentPage(0);
      if (r != 0)
        return r;
    }
    // last page
    if (toolbarSelMenu == 1 && toolbarSelMenuItem == 1 && isPaginated()) {
      int n = getTotalPages();
      int r = setCurrentPage(n);
      if (r != 0)
        return r;
    }
    // prev 10 pages
    if (toolbarSelMenu == 1 && toolbarSelMenuItem == 2 && isPaginated()) {
      int p = getCurrentPage();
      p -= 10;
      int r = setCurrentPage(p);
      if (r != 0)
        return r;
    }
    // next 10 pages
    if (toolbarSelMenu == 1 && toolbarSelMenuItem == 3 && isPaginated()) {
      int p = getCurrentPage();
      p += 10;
      int r = setCurrentPage(p);
      if (r != 0)
        return r;
    }
    // go to page
    if (toolbarSelMenu == 1 && toolbarSelMenuItem == 4 && isPaginated()) {
      return BK_CMD_INVOKE_PAGE_CHOOSER;
    }
    int zi = 3;
    int zo = 2;
    if (hasZoomToFit()) {
      if (toolbarSelMenu == 2 && toolbarSelMenuItem == 1 && isZoomable()) {
        int r = setZoomToFitWidth();
        if (r != 0)
          return r;
      }
      if (toolbarSelMenu == 2 && toolbarSelMenuItem == 0 && isZoomable()) {
        int r = setZoomToFitHeight();
        if (r != 0)
          return r;
      }
    } else {
      zi = 1;
      zo = 0;
    }

    // zoom in
    if (toolbarSelMenu == 2 && toolbarSelMenuItem == zi && isZoomable()) {
      vector<ZoomLevel> zooms;
      getZoomLevels(zooms);
      int z = getCurrentZoomLevel();
      z++;
      int r = setZoomLevel(z);
      if (r != 0)
        return r;
    }
    // zoom out
    if (toolbarSelMenu == 2 && toolbarSelMenuItem == zo && isZoomable()) {
      vector<ZoomLevel> zooms;
      getZoomLevels(zooms);
      int z = getCurrentZoomLevel();
      z--;
      int r = setZoomLevel(z);
      if (r != 0)
        return r;
    }

    // rotate cw
    if (toolbarSelMenu == 3 && toolbarSelMenuItem == 0 && isRotable()) {
      // Screen::setSpeed(User::options.pspMenuSpeed);
      int z = getRotation();
      z++;
      int r = setRotation(z);
      // Screen::setSpeed(User::options.pspSpeed);
      if (r != 0)
        return r;
    }
    // rotate ccw
    if (toolbarSelMenu == 3 && toolbarSelMenuItem == 1 && isRotable()) {
      // Screen::setSpeed(User::options.pspMenuSpeed);
      int z = getRotation();
      z--;
      int r = setRotation(z);
      // Screen::setSpeed(User::options.pspSpeed);
      if (r != 0)
        return r;
    }
  }

  // main menu
  if (b[User::controls.showMainMenu] == 1) {
    return BK_CMD_INVOKE_MENU;
  }

  // view
  if (b[User::controls.showToolbar] == 1) {
    mode = BKDOC_VIEW;
    return BK_CMD_MARK_DIRTY;
  }

  return 0;
}

#define TOOLTIP_WIDTH 150
#define TOOLTIP_PADDING 10
#define TOOLTIP_ITEM_WIDTH 60
#define TOOLTIP_HEIGHT 50
#define DIALOG_ICON_SCALE 1.0f

#ifdef __vita__
#define TOOLTIP_ICONS_Y_OFFSET 544 - 150 + 10
#define TOOLTIP_TEXT_OFFSET_X 10
#define TOOLTIP_TEXT_OFFSET_Y 18
#define TOOLTIP_TEXT_OFFSET_X_2 72
#define TOOLTIP_TEXT_OFFSET_4 15
#define TOOLTIP_TEXT_OFFSET_X_3 75
#else
#define TOOLTIP_ICONS_Y_OFFSET 1280 - 150 + 10
#define TOOLTIP_TEXT_OFFSET_X 8
#define TOOLTIP_TEXT_OFFSET_Y 6
#define TOOLTIP_TEXT_OFFSET_X_2 72
#define TOOLTIP_TEXT_OFFSET_4 13
#define TOOLTIP_TEXT_OFFSET_X_3 75
#endif

#ifdef SWITCH
#define TOOLTIP_TOOLS_TEXT "+"
#define TOOLTIP_MENU_TEXT "-"
#elif defined(__vita__)
#define TOOLTIP_TOOLS_TEXT "Select"
#define TOOLTIP_MENU_TEXT "Start"
#else
#define TOOLTIP_TOOLS_TEXT "L-Shift"
#define TOOLTIP_MENU_TEXT "Enter"
#endif

#define TOOLTIP_OUTER_RECTANGLE_X DEFAULT_SCREEN_WIDTH - TOOLTIP_WIDTH
#define TOOLTIP_OUTER_RECTANGLE_Y DEFAULT_SCREEN_HEIGHT - TOOLTIP_HEIGHT

#define TOOLTIP_INNER_TOOLS_RECTANGLE_X TOOLTIP_OUTER_RECTANGLE_X + TOOLTIP_PADDING
#define TOOLTIP_INNER_TOOLS_RECTANGLE_Y TOOLTIP_OUTER_RECTANGLE_Y + (TOOLTIP_HEIGHT / 2)
#define TOOLTIP_INNER_MENU_RECTANGLE_X DEFAULT_SCREEN_WIDTH - (TOOLTIP_WIDTH / 2) + (TOOLTIP_PADDING/2)
#define TOOLTIP_INNER_MENU_RECTANGLE_Y TOOLTIP_INNER_TOOLS_RECTANGLE_Y

#define TOOLTIP_TOOLS_TEXT_OFFSET_X TOOLTIP_OUTER_RECTANGLE_X + TOOLTIP_TEXT_OFFSET_X
#define TOOLTIP_TOOLS_TEXT_OFFSET_Y TOOLTIP_OUTER_RECTANGLE_Y + TOOLTIP_TEXT_OFFSET_Y
#define TOOLTIP_MENU_TEXT_OFFSET_X TOOLTIP_TOOLS_TEXT_OFFSET_X + TOOLTIP_TEXT_OFFSET_X_2
#define TOOLTIP_MENU_TEXT_OFFSET_Y TOOLTIP_TOOLS_TEXT_OFFSET_Y

#define TOOLTIP_SELECT_TEXT_OFFSET_X TOOLTIP_OUTER_RECTANGLE_X + TOOLTIP_TEXT_OFFSET_4
#define TOOLTIP_SELECT_TEXT_OFFSET_Y TOOLTIP_OUTER_RECTANGLE_Y + (TOOLTIP_HEIGHT / 2) + TOOLTIP_TEXT_OFFSET_Y
#define TOOLTIP_START_TEXT_OFFSET_X TOOLTIP_SELECT_TEXT_OFFSET_X + TOOLTIP_TEXT_OFFSET_X_3
#define TOOLTIP_START_TEXT_OFFSET_Y TOOLTIP_SELECT_TEXT_OFFSET_Y

#ifdef __vita__
#define BANNER_X (DEFAULT_SCREEN_WIDTH / 2) - 180
#define BANNER_WIDTH (2*180)
#define BANNER_HEIGHT 30
#define BANNER_X_TEXT_X BANNER_X + 90
#define BANNER_X_TEXT_Y 21
#else
#define BANNER_X (DEFAULT_SCREEN_WIDTH / 2) - 180
#define BANNER_WIDTH (2*180)
#define BANNER_HEIGHT 30
#define BANNER_X_TEXT_X BANNER_X + 90
#define BANNER_X_TEXT_Y 6
#endif

#ifdef __vita__
#define TOOLBAR_OUTER_RECTANGLE_X 40
#define TOOLBAR_OUTER_RECTANGLE_Y 544 - 150
#define TOOLBAR_OUTER_RECTANGLE_WIDTH 960 - 92
#define TOOLBAR_OUTER_RECTANGLE_HEIGHT 544
#else
#define TOOLBAR_OUTER_RECTANGLE_X 40
#define TOOLBAR_OUTER_RECTANGLE_Y DEFAULT_SCREEN_HEIGHT * 0.7
#define TOOLBAR_OUTER_RECTANGLE_WIDTH DEFAULT_SCREEN_WIDTH * 0.93
#define TOOLBAR_OUTER_RECTANGLE_HEIGHT DEFAULT_SCREEN_HEIGHT
#endif

#ifdef __vita__
#define TOOLBAR_CONTEXT_LABEL_X 96
#define TOOLBAR_CONTEXT_LABEL_Y 494
#define TOOLBAR_CONTEXT_LABEL_WIDTH 768
#define TOOLBAR_CONTEXT_LABEL_HEIGHT 50
#else
#define TOOLBAR_CONTEXT_LABEL_X 96
#define TOOLBAR_CONTEXT_LABEL_Y DEFAULT_SCREEN_HEIGHT * 0.9
#define TOOLBAR_CONTEXT_LABEL_WIDTH DEFAULT_SCREEN_WIDTH * 0.8
#define TOOLBAR_CONTEXT_LABEL_HEIGHT DEFAULT_SCREEN_HEIGHT * 0.1
#endif

#ifdef __vita__
#define TOOLBAR_HIGHLIGHT_ICON_X 40
#define TOOLBAR_HIGHLIGHT_ICON_X_MULTIPLIER 75
#define TOOLBAR_HIGHLIGHT_ICON_Y 544 - 150
#define TOOLBAR_HIGHLIGHT_ICON_Y_MULTIPLIER 55
#define TOOLBAR_HIGHLIGHT_ICON_WIDTH 85
#define TOOLBAR_HIGHLIGHT_ICON_HEIGHT 65
#else
#define TOOLBAR_HIGHLIGHT_ICON_X 40
#define TOOLBAR_HIGHLIGHT_ICON_X_MULTIPLIER 75
#define TOOLBAR_HIGHLIGHT_ICON_Y DEFAULT_SCREEN_HEIGHT * 0.7
#define TOOLBAR_HIGHLIGHT_ICON_Y_MULTIPLIER 55
#define TOOLBAR_HIGHLIGHT_ICON_WIDTH 85
#define TOOLBAR_HIGHLIGHT_ICON_HEIGHT 65
#endif

#ifdef __vita__
#define TOOLBAR_SELECTED_ICON_X 50
#define TOOLBAR_SELECTED_ICON_Y 544 - 195
#define TOOLBAR_SELECTED_ICON_Y_MULTIPLIER 55
#define TOOLBAR_SELECTED_ICON_WIDTH 80
#define TOOLBAR_SELECTED_ICON_HEIGHT 50
#else
#define TOOLBAR_SELECTED_ICON_X 40
#define TOOLBAR_SELECTED_ICON_X_MULTIPLIER 75
#define TOOLBAR_SELECTED_ICON_Y DEFAULT_SCREEN_HEIGHT * 0.7
#define TOOLBAR_SELECTED_ICON_Y_MULTIPLIER 55
#define TOOLBAR_SELECTED_ICON_WIDTH 85
#define TOOLBAR_SELECTED_ICON_HEIGHT 65
#endif

void Document::render() {
  // content
  renderContent();

  // flash tip for menu/toolbar on load
  if (tipFrames > 0 && mode != BKDOC_TOOLBAR) {
    int alpha = 0xff;
    if (tipFrames <= 32) {
      alpha = tipFrames * (256/32) - 8;
    }

    if (alpha > 0) {
      drawRectangle(TOOLTIP_OUTER_RECTANGLE_X, TOOLTIP_OUTER_RECTANGLE_Y,
                    TOOLTIP_WIDTH, TOOLTIP_HEIGHT, WOOD_BARK | (alpha << 24));
      drawRectangle(TOOLTIP_INNER_TOOLS_RECTANGLE_X, TOOLTIP_INNER_TOOLS_RECTANGLE_Y,
                    TOOLTIP_ITEM_WIDTH, TOOLTIP_HEIGHT/2,WHITE_SMOKE | (alpha << 24));
      drawRectangle(TOOLTIP_INNER_MENU_RECTANGLE_X,  TOOLTIP_INNER_MENU_RECTANGLE_Y,
                    TOOLTIP_ITEM_WIDTH, TOOLTIP_HEIGHT/2, WHITE_SMOKE | (alpha << 24));

      drawText(TOOLTIP_TOOLS_TEXT_OFFSET_X, TOOLTIP_TOOLS_TEXT_OFFSET_Y,
               (WHITE | (alpha << 24)), 1.0f, "Tools");
      drawText(TOOLTIP_MENU_TEXT_OFFSET_X, TOOLTIP_MENU_TEXT_OFFSET_Y,
               (WHITE | (alpha << 24)), 1.0f, "Menu");
      drawText(TOOLTIP_SELECT_TEXT_OFFSET_X, TOOLTIP_SELECT_TEXT_OFFSET_Y,
               (WOOD_BARK | (alpha << 24)), 0.85f, TOOLTIP_TOOLS_TEXT);
      drawText(TOOLTIP_START_TEXT_OFFSET_X, TOOLTIP_START_TEXT_OFFSET_Y,
               (WHITE | (alpha << 24)), 0.85f, TOOLTIP_MENU_TEXT);
    }
  }

  // banner that shows page loading and current page number / number of pages
  if (bannerFrames > 0 && User::options.displayLabels) {
    int y = mode == BKDOC_TOOLBAR ? 10 : DEFAULT_SCREEN_HEIGHT - 50;
    int alpha = 0xff;
    if (bannerFrames <= 32) {
      alpha = bannerFrames*(256/32) - 8;
    }
    if (alpha > 0) {
      drawRectangle(BANNER_X, y,
                    BANNER_WIDTH, BANNER_HEIGHT, NERO | (alpha << 24));
      drawText(BANNER_X_TEXT_X, y + BANNER_X_TEXT_Y,
               WHITE | (alpha << 24), 1.0f, banner.c_str());
    }
  }

  if (mode != BKDOC_TOOLBAR)
    return;

  // all of the icons menus must have at least one item
  // wrap menu indexes
  if (toolbarSelMenu >= 4)
    toolbarSelMenu = 0;
  if (toolbarSelMenu < 0)
    toolbarSelMenu = 3;
  if (toolbarSelMenuItem >= (int)toolbarMenus[toolbarSelMenu].size())
    toolbarSelMenuItem = 0;
  if (toolbarSelMenuItem < 0)
    toolbarSelMenuItem = toolbarMenus[toolbarSelMenu].size() - 1;

  const ToolbarItem& it = toolbarMenus[toolbarSelMenu][toolbarSelMenuItem];

  // background
  drawRectangle(TOOLBAR_OUTER_RECTANGLE_X, TOOLBAR_OUTER_RECTANGLE_Y,
                TOOLBAR_OUTER_RECTANGLE_WIDTH, TOOLBAR_OUTER_RECTANGLE_HEIGHT, ALIZARIN);

  // context label
  drawRectangle(TOOLBAR_CONTEXT_LABEL_X, TOOLBAR_CONTEXT_LABEL_Y,
                TOOLBAR_CONTEXT_LABEL_WIDTH, TOOLBAR_CONTEXT_LABEL_HEIGHT, TOMATO);

  // selected column - decide if it overflows
  int ts = toolbarMenus[toolbarSelMenu].size();
  int init = 0;
  bool overflow = false;
  int cs = ts;
  if (ts > 5) { // overflow mode
    overflow = true;
    init = toolbarSelMenuItem - 4;
    if (init < 0)
      init = 0;
    ts = 5 + init;
    cs = 5;
  }

  // highlight icon column
  drawRectangle(TOOLBAR_HIGHLIGHT_ICON_X + (toolbarSelMenu*TOOLBAR_HIGHLIGHT_ICON_X_MULTIPLIER),
                TOOLBAR_HIGHLIGHT_ICON_Y - (cs * TOOLBAR_HIGHLIGHT_ICON_Y_MULTIPLIER),
                TOOLBAR_HIGHLIGHT_ICON_WIDTH,
                (cs * TOOLBAR_HIGHLIGHT_ICON_Y_MULTIPLIER) + TOOLBAR_HIGHLIGHT_ICON_HEIGHT,
                TOMATO);

  // selected icon item row
  int iw = 1;
  int mw = toolbarMenus[toolbarSelMenu][toolbarSelMenuItem].minWidth;
  if (iw < mw)
    iw = mw;
  int selItemI = overflow ?
    toolbarSelMenuItem > 4 ? 4 : toolbarSelMenuItem
    : toolbarSelMenuItem;
  drawRectangle(TOOLBAR_SELECTED_ICON_X + (toolbarSelMenu*TOOLBAR_HIGHLIGHT_ICON_X_MULTIPLIER),
                TOOLBAR_SELECTED_ICON_Y - (selItemI * TOOLBAR_SELECTED_ICON_Y_MULTIPLIER) - TOOLBAR_HIGHLIGHT_ICON_Y_MULTIPLIER,
                TOOLBAR_SELECTED_ICON_WIDTH + iw,
                TOOLBAR_SELECTED_ICON_HEIGHT,
                CHERUB);

  // button icons
  // if (it.circleLabel.size() > 0) {
  //   #ifdef PSP
  //     Screen::ambientColor(0xffcccccc);
  //     int tw = textW((char*)it.circleLabel.c_str(), fontBig);
  //     drawImage(480 - tw - 65, 248, _IMG_CROSS_XSIZE, _IMG_CROSS_YSIZE, _IMG_CROSS_X, _IMG_CROSS_Y);
  //   #elif defined(__vita__)
  //     // printf("here");
  //     switch (User::controls.select)  {
  //       case FZ_REPS_CROSS:
  //         vita2d_draw_texture_scale(bk_icons["bk_cross_icon"]->vita_texture, 768 - 20 - 130, DEFAULT_SCREEN_HEIGHT - 50 + 7,
  //                                   DIALOG_ICON_SCALE, DIALOG_ICON_SCALE);
  //         break;
  //       case FZ_REPS_CIRCLE:
  //         vita2d_draw_texture_scale(bk_icons["bk_circle_icon"]->vita_texture, 768 - 20 - 130, DEFAULT_SCREEN_HEIGHT - 50 + 7,
  //                                   DIALOG_ICON_SCALE, DIALOG_ICON_SCALE);
  //       default:
  //         break;
  //     }
  //   #endif
  // }

  // if (it.triangleLabel.size() > 0) {
  //   #ifdef PSP
  //     drawImage(37, 248, 20, 18, _IMG_TRIANGLE_X, _IMG_TRIANGLE_Y);
  //   #elif defined(__vita__)
  //     vita2d_draw_texture_scale(bk_icons["bk_triangle_icon"]->vita_texture, 20 + 130, DEFAULT_SCREEN_HEIGHT - 50 + 7,
  //                               DIALOG_ICON_SCALE, DIALOG_ICON_SCALE);
  //   #endif
  // }


  // menu row
  #ifdef __vita__
    vita2d_draw_texture_scale(bk_icons["bk_bookmark_icon"]->vita_texture, 60, TOOLTIP_ICONS_Y_OFFSET, 
      DIALOG_ICON_SCALE, DIALOG_ICON_SCALE);

    vita2d_draw_texture_scale(bk_icons["bk_copy_icon"]->vita_texture, 60 + 75, TOOLTIP_ICONS_Y_OFFSET, 
      DIALOG_ICON_SCALE, DIALOG_ICON_SCALE);

    vita2d_draw_texture_scale(bk_icons["bk_search_icon"]->vita_texture, 60 + 75 + 75 , TOOLTIP_ICONS_Y_OFFSET, 
      DIALOG_ICON_SCALE, DIALOG_ICON_SCALE);

    vita2d_draw_texture_scale(bk_icons["bk_rotate_left_icon"]->vita_texture, 60 + 75 + 75 + 75, TOOLTIP_ICONS_Y_OFFSET, 
      DIALOG_ICON_SCALE, DIALOG_ICON_SCALE);
  #else
    #define LOGO_SIZE 48

    ResourceManager::getSpriteRenderer()->DrawSprite(ResourceManager::GetTexture("bk_bookmark_icon"),
      glm::vec2(TOOLBAR_HIGHLIGHT_ICON_X + 20, TOOLBAR_HIGHLIGHT_ICON_Y + 10),
      glm::vec2(48, 48));

    ResourceManager::getSpriteRenderer()->DrawSprite(ResourceManager::GetTexture("bk_copy_icon"),
      glm::vec2(TOOLBAR_HIGHLIGHT_ICON_X + 20 + 70, TOOLBAR_HIGHLIGHT_ICON_Y + 10),
      glm::vec2(LOGO_SIZE, LOGO_SIZE));

    ResourceManager::getSpriteRenderer()->DrawSprite(ResourceManager::GetTexture("bk_search_icon"),
      glm::vec2(TOOLBAR_HIGHLIGHT_ICON_X + 20 + (70 * 2), TOOLBAR_HIGHLIGHT_ICON_Y + 10),
      glm::vec2(LOGO_SIZE, LOGO_SIZE));

    ResourceManager::getSpriteRenderer()->DrawSprite(ResourceManager::GetTexture("bk_rotate_left_icon"),
      glm::vec2(TOOLBAR_HIGHLIGHT_ICON_X + 20 + (70 * 3), TOOLBAR_HIGHLIGHT_ICON_Y + 10),
      glm::vec2(LOGO_SIZE, LOGO_SIZE));
  #endif

  // selected column
  for (int i = init, j = 0; i < ts; i++, j++) {
    const ToolbarItem& it2 = toolbarMenus[toolbarSelMenu][i];
    unsigned int color;
    if (i == toolbarSelMenuItem)
      color = 0xff000000;
    else
      color = 0xffffffff;
    if (it2.iconName.size() > 0) {
      // check map existance, just in case.
      #ifdef __vita__
        vita2d_draw_texture_tint_scale(bk_icons[it2.iconName]->vita_texture, 60 + toolbarSelMenu*75, 544 - 140 - (j*55) - 55,
          DIALOG_ICON_SCALE, DIALOG_ICON_SCALE, color);
      #else
        ResourceManager::getSpriteRenderer()->DrawSprite(ResourceManager::GetTexture(it2.iconName),
          glm::vec2(60 + toolbarSelMenu*75, 720 - 150 - (j*55) - 110),
          glm::vec2(48, 48));

      #endif
    } else {
      #ifdef __vita__
        vita2d_draw_texture_tint_scale(bk_icons["bk_rotate_left_icon"]->vita_texture, 60 + toolbarSelMenu*75, 544 - 140 - (j*55) - 55,
          DIALOG_ICON_SCALE, DIALOG_ICON_SCALE, color);
      #endif
    }
  }
  
  // item label for selected item
  #ifdef __vita__
    vita2d_font_draw_text(fontBig->v_font, 
      60 + toolbarSelMenu*75 - 10 + 70,
      544 - 140 - (selItemI*55) - 55 + 33,
      0xff000000, 28, it.label.c_str());
  #else
    drawText(60 + toolbarSelMenu*75 - 10 + 70 + 15,
      544 - 140 - (selItemI*55) + 55 + 15,
      BLACK, 1.0f, it.label.c_str());
  #endif

  // // // button labels
  // if (it.triangleLabel.size() > 0) {
  //   #ifdef __vita__
  //     vita2d_font_draw_text(fontBig->v_font, 20 + 130 + 45, DEFAULT_SCREEN_HEIGHT - 50 + 7 + 28, RGBA8(255, 255, 255, 255), 28, it.triangleLabel.c_str());
  //   #endif
  // }
  // if (it.circleLabel.size() > 0) {
  //   #ifdef __vita__
  //     vita2d_font_draw_text(fontBig->v_font, 768 - 20 - 130 + 45, DEFAULT_SCREEN_HEIGHT - 50 + 7 + 28, RGBA8(255, 255, 255, 255), 28, it.circleLabel.c_str());
  //   #endif
  // }

  // // // overflow indicators
  // /*
  //   if (overflow) {
  //     Screen::ambientColor(0xffffffff);
  //     drawText("...", fontBig, 43 + toolbarSelMenu*55, 0);
  //     drawText("...", fontBig, 43 + toolbarSelMenu*55, 272 - 92);
  //   }
  // */

  string t;
  if (isPaginated()) {
    char tp[256];
    snprintf(tp, 256, "Page %d of %d", getCurrentPage() + 1, getTotalPages());
    t = tp;
  }
  
  drawClockAndBattery(t);
}

}