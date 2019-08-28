/*
 * bookr-modern: a graphics based document reader for switch, vita and desktop
 * Copyright (C) 2019 pathway27 (Sree)
 * IS A MODIFICATION OF THE ORIGINAL
 * Bookr and bookr-mod for PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com),
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com),
 * AND VARIOUS OTHER FORKS, See Forks in README.md
 * Licensed under GPLv3+, see LICENSE
*/

#ifndef BKUSER_H
#define BKUSER_H

#include <vector>

using std::vector;

namespace bookr {

struct Controls {
  // in-book controls
  int previousPage;
  int nextPage;
  int previous10Pages;
  int next10Pages;
  int screenUp;
  int screenDown;
  int screenLeft;
  int screenRight;
  int zoomIn;
  int zoomOut;
  int showToolbar;  // Start
  int showMainMenu; // Select   
  
  // menu controls
  int select;   // Circle or Cross
  int cancel;   // Cross or Circle
  int alternate;  // Triangle
  int details;  // Square - not currently used
  int menuUp;
  int menuDown;
  int menuLeft;
  int menuRight;
  int menuLTrigger;
  int menuRTrigger;
  int resume;   // Start
};

struct ColorScheme {
  int txtFGColor;
  int txtBGColor;
};
  
struct Options {
  bool pdfFastScroll;

  // why it is a pref and not just a button command: the current text viewer
  // needs to repaginate the whole document when rotating it, so it is an
  // expensive operation. by offering the feature as an option we make it
  // clear to the user that it is just not possible to rotate on the fly,
  // unlike with the pdf viewer.
  int txtRotation;

  string txtFont;
  int txtSize;
  vector<ColorScheme> colorSchemes;
  int currentScheme;
  bool txtJustify;
  int pspSpeed;
  int pspMenuSpeed;
  bool displayLabels;
  bool pdfInvertColors;
  string lastFolder;
  string lastFontFolder;
  int txtHeightPct;
  bool loadLastFile;
  int txtWrapCR;

  int browserTextSize;
  int browserDisplayMode;
  bool browserEnableFlash;
  int browserInterfaceMode;
  bool browserConfirmExit;
  bool browserShowCursor;

  int hScroll;
  int vScroll;
  int thumbnail;
  vector<ColorScheme> thumbnailColorSchemes;
  int currentThumbnailScheme;
  /*
  pdfImageQuality:
  max scale denom while scale num is 1.
  0: 1, no scale
  1: 1/2
  2: 1/4
  3: 1/8
   */
  int pdfImageQuality;
  int pdfImageBufferSizeM;
  bool pdfOptimizeForSmallImages;
  int analogRateX;
  int analogRateY;
  int maxTreeHeight;
  int screenBrightness;
  bool autoPruneBookmarks;
  int defaultTitleMode;
  bool evictGlyphCacheOnNewPage;

  // 0 disabled 
  // 1 one screen
  // 2 four screen
  // 3 legacy full page buffer.
  int pageScrollCacheMode;
  bool ignoreXInOutlineOnSquare;
  bool t_ignore_x;
  bool jpeg2000Decoder;
};

class User {
  static void load();

public:
  static void init();
  static void save();
  static void setDefaultControls();
  static void setDefaultOptions();

  static Controls controls;
  static Options options;
};

}

#endif

