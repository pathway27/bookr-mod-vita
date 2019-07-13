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

#ifndef BKFANCYTEXT_H
#define BKFANCYTEXT_H

#include <string>
#include <vector>

#include "../graphics/screen.hpp"
#include "../document.hpp"

using std::string;

#define BKFT_CONT_NONE			0
#define BKFT_CONT_LF			1
#define BKFT_CONT_EXTRALF		2

#define BKFT_STYLE_PLAIN		0
#define BKFT_STYLE_BOLD			1
//#define BKFT_STYLE_ITALIC		2
//#define BKFT_STYLE_BOLD_ITALIC	3

#define BKFT_FONT_SANS			0
//#define BKFT_FONT_FIXED			1

namespace bookr {

struct Run {
  char* text;
  bool lineBreak;
  int n;
  int style;
  int font;
  int bgcolor;
  int fgcolor;
};

struct Line {
  int firstRun;
  int firstRunOffset;
  int totalChars;
  float spaceWidth;
  Line(int fr, int fro, int tc, float sw) : firstRun(fr), firstRunOffset(fro), totalChars(tc), spaceWidth(sw) { }
  Line() { }
};

class FancyText : public Document {
  private:
  Line* lines;
  int nLines;
  int topLine;
  int maxY;
  Font* font;
  int rotation;
  int lastFontSize;
  string lastFontFace;
  int	lastHeightPct;
  int lastWrapCR;

  int linesPerPage;
  int totalPages;
  void reflow(int width);

  protected:
  Run* runs;
  int nRuns;
  FancyText();
  ~FancyText();

  void resizeView(int widht, int height);
  void resetFonts();
  int setLine(int l);
  int runForLine(int l);
  int lineForRun(int r);

  bool holdScroll;

  // a lot of ebook formats use HTML as a display format, on top of a
  // container format. so it makes sense to put the parser/tokenizer in
  // the base class
  static char* parseHTML(FancyText* r, char* in, int n);

  // same with plain text
  static char* parseText(FancyText* r, char* b, int length);

  public:
  virtual int updateContent();
  virtual int resume();
  virtual void renderContent();

  virtual void getFileName(string&) = 0;
  virtual void getTitle(string&) = 0;
  virtual void getType(string&) = 0;

  virtual bool isPaginated();
  virtual int getTotalPages();
  virtual int getCurrentPage();
  virtual int setCurrentPage(int);

  virtual bool isZoomable();
  virtual void getZoomLevels(vector<Document::ZoomLevel>& v);
  virtual int getCurrentZoomLevel();
  virtual int setZoomLevel(int);
  virtual bool hasZoomToFit();
  virtual int setZoomToFitWidth();
  virtual int setZoomToFitHeight();

  virtual int pan(int, int);

  virtual int screenUp();
  virtual int screenDown();
  virtual int screenLeft();
  virtual int screenRight();

  virtual bool isRotable();
  virtual int getRotation();
  virtual int setRotation(int, bool bForce=false);

  virtual bool isBookmarkable();
  virtual void getBookmarkPosition(map<string, float>&);
  virtual int setBookmarkPosition(map<string, float>&);
};

}

#endif