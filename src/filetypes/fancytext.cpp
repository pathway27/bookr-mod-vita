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

#include <cmath>
#include <cstring>
#include <list>
#include <algorithm>
#include <vector>

#include "fancytext.hpp"

namespace bookr {

FancyText::FancyText() : lines(0), nLines(0), topLine(0), maxY(0), font(0), rotation(0), runs(0), nRuns(0), holdScroll(false) {
  lastFontSize  = User::options.txtSize;
  lastFontFace  = User::options.txtFont;
  lastHeightPct = User::options.txtHeightPct;
  lastWrapCR    = User::options.txtWrapCR;
}

FancyText::~FancyText() {
  if (runs)
    delete[] runs;
  if (lines)
    delete[] lines;
  if (font)
    font->release();
}

static inline bool isBlank(int c) {
    return c == 32 || c == 10 || c == 9;
}

struct RunsIterator {
    Run* runs;
    int currentRun;
    int currentChar;
    int currentWidth;
    int maxRuns;
    int lineBreakMark;
    int globalPos;
    bool end() {
      return currentRun >= maxRuns ||
        ((currentRun == maxRuns - 1) && currentChar >= runs[currentRun].n);
    }
    inline unsigned char forward() {
      if (end()) return 0;
      if (currentChar >= runs[currentRun].n) {
        lineBreakMark += runs[currentRun].lineBreak ? 1 : 0;
        ++currentRun;
        currentChar = 0;
        return forward();
      }
      unsigned char c = runs[currentRun].text[currentChar];
      ++currentChar;
      ++globalPos;
      return c;
    }
    inline unsigned char backward() {
      if (currentRun <= 0 && currentChar < 0) return 0;
      if (currentChar < 0) {
        --currentRun;
        currentChar = runs[currentRun].n - 1;
        return backward();
      }
      unsigned char c = runs[currentRun].text[currentChar];
      --currentChar;
      --globalPos;
      return c;
    }
    RunsIterator(Run* r, int cr, int cc, int mr) : 	runs(r), currentRun(cr), currentChar(cc), maxRuns(mr), lineBreakMark(false), globalPos(0) { }
    RunsIterator(const RunsIterator& s) :
      runs(s.runs),
      currentRun(s.currentRun),
      currentChar(s.currentRun),
      maxRuns(s.currentRun),
      lineBreakMark(s.lineBreakMark),
      globalPos(s.globalPos) { }
};

void FancyText::reflow(int width) {
    if (lines)
      delete[] lines;
    list<Line> tempLines;

    int lineFirstRun = 0;
    int lineFirstRunOffset = 0;
    int lineSpaces = 0;
    int lineStartGlobalPos = 0;
    int currentWidth = 0;
    float spaceWidth = 0.0f;
    RunsIterator rit(runs, 0, 0, nRuns);
    RunsIterator lastSpace = rit;

    #ifdef PSP
      FZCharMetrics* fontChars = font->getMetrics();
      const int spaceWidthC = fontChars[32].xadvance;
      const float spaceWidthCF = float(spaceWidthC);
    #elif defined(__vita__) || defined(MAC) || defined(WIN32) || defined(SWITCH)
      const int spaceWidthC = 10;
      const float spaceWidthCF = 10;
    #endif

    while (!rit.end()) {
      if (currentWidth > width || rit.lineBreakMark > 0) {
        spaceWidth = spaceWidthCF;
        int tl = rit.lineBreakMark;
        if (rit.lineBreakMark > 0) {
          rit.lineBreakMark = 0;
        } else if (lineSpaces > 0) {
          rit = lastSpace;
          --lineSpaces;
          if (lineSpaces > 0)
            spaceWidth = spaceWidthCF + (float(width - rit.currentWidth) / float(lineSpaces));
          else
            spaceWidth = spaceWidthCF;
        } else {
          rit.backward();			// consume the overflowing char
        }
        --tl;
        //printf("line n %d\n", rit.globalPos - lineStartGlobalPos);
        tempLines.push_back(Line(lineFirstRun, lineFirstRunOffset, rit.globalPos - lineStartGlobalPos, spaceWidth));
        while (tl > 0) {
          tempLines.push_back(Line(lineFirstRun, lineFirstRunOffset, 0, spaceWidth));
          --tl;
        }
        lineFirstRun = rit.currentRun;
        lineFirstRunOffset = rit.currentChar;
        lineStartGlobalPos = rit.globalPos;
        lineSpaces = 0;
        currentWidth = 0;
      }
      int c = rit.forward();
      if (isBlank(c)) {
        lastSpace = rit;
        ++lineSpaces;
      }
      #ifdef PSP
        currentWidth += fontChars[c].xadvance;
      #elif defined(__vita__)
        currentWidth += 10;
      #endif
      rit.currentWidth = currentWidth;
    }

    nLines = tempLines.size();
    lines = new Line[nLines];
    list<Line>::iterator it(tempLines.begin());
    int i = 0;
    while (it != tempLines.end()) {
      const Line& l = *it;
      lines[i] = l;
      ++i;
      ++it;
    }
}

void FancyText::resizeView(int width, int height) {
    reflow(width - 10 - 10);

    #ifdef PSP
      linesPerPage = (height - 10) / (font->getLineHeight()*(User::options.txtHeightPct/100.0));
    #elif defined(__vita__)
      linesPerPage = 25;
    #endif

    maxY = height - 10;
    totalPages = (nLines / linesPerPage) + 1;
}

// a lot of ebook formats use HTML as a display format, on top of a
// container format. so it makes sense to put the parser/tokenizer in
// the base class

struct StrIt {
    char* p;
    int i;
    int n;
    StrIt(char* _p, int _i, int _n) : p(_p), i(_i), n(_n) { }
    bool end() {
      return i >= n;
    }
    bool matches(const char* s) {				// does NOT advance the iteration
      char* q = p;
      int j = i;
      while (*s != 0 && j < n) {
        char c = *s >= 'A' && *s <= 'Z' ? *s | 0x20 : *s;		// tolower for ascii
        if (c != *q)
          return false;
        ++q;
        ++s;
        ++j;
      }
      return true;
    }
    void skipTo(const char* s) {
      while (!matches(s)) {
        forward();
      }
      if (!end()) {
        int l = strlen(s);
        p += l;
        i += l;
      }
    }
    unsigned char forward() {
      unsigned char c = 0;
      if (!end()) {
        c = (unsigned char)*p;
        ++p;
        ++i;
      }
      return c;
    }
};

char* FancyText::parseHTML(FancyText* r, char* in, int n) {
    // detokenize html text
    // <head*>*</head> --> strip
    // <p*> --> run break, EXTRALF
    // <br*> --> run break, LF
    // <*> --> strip
    // ascii < 32 --> strip
    // &*; --> to do...

    list<Run> tempRuns;
    Run run;

    StrIt it(in, 0, n);

    char* out = (char*)malloc(n);
    memset(out, 0, n);
    char* q = out;
    char* lastQ = out;
    int i = 0;
    int li = 0;

    run.lineBreak = true;

    bool lastBlank = false;
    while (!it.end()) {
      if (it.matches("<head")) {			// skip html header
        it.skipTo("</head>");
        continue;
      }
      if (it.matches("<p")) {
        it.skipTo(">");
        // close the previous run
        run.text = lastQ;
        run.n = i - li;
        //printf("p %d\n", run.n);
        li = i;
        lastQ = q;
        tempRuns.push_back(run);

        // the continuation for the next run
        run.lineBreak = true;
        continue;
      }
      if (it.matches("<h") ) {
        it.skipTo(">");
        // close the previous run
        run.text = lastQ;
        run.n = i - li;
        li = i;
        lastQ = q;
        tempRuns.push_back(run);

        // the continuation for the next run
        run.lineBreak = true;
        /**q = '>'; ++q;
        *q = '>'; ++q;
        *q = '>'; ++q;
        *q = ' '; ++q;
        i += 4;*/
        continue;
      }
      if (it.matches("<br")) {
        it.skipTo(">");
        // close the previous run
        run.text = lastQ;
        run.n = i - li;
        //printf("br %d\n", run.n);
        li = i;
        lastQ = q;
        tempRuns.push_back(run);

        // the continuation for the next run
        run.lineBreak = true;
        continue;
      }
      if (it.matches("<li") || it.matches("<dt") || it.matches("<dl")) {
        it.skipTo(">");
        // close the previous run
        run.text = lastQ;
        run.n = i - li;
        li = i;
        lastQ = q;
        tempRuns.push_back(run);

        // the continuation for the next run
        run.lineBreak = true;
        *q = '*'; ++q;
        *q = ' '; ++q;
        i += 2;
        continue;
      }
      if (it.matches("<")) {				// any other tag - ignore it
        it.skipTo(">");
        continue;
      }
      unsigned char c = it.forward();
      if (!isBlank(c) && c < 32)			// skip non-blanks, non-printables
        continue;
      if (isBlank(c) && !lastBlank) {		// consolidate 1 to N blanks into a single space
        *q = 32;
        ++q;
        ++i;
        lastBlank = true;
      }
      if (c > 32) {						// passthru any other char
        *q = c;
        ++q;
        ++i;
        lastBlank = false;
      }
    }
    // last run
    run.text = lastQ;
    run.n = i - li;
    tempRuns.push_back(run);

    // create fast fixed size run array	
    r->runs = new Run[tempRuns.size()];
    r->nRuns = tempRuns.size();
    list<Run>::iterator jt(tempRuns.begin());
    i = 0;
    while (jt != tempRuns.end()) {
      const Run& l = *jt;
      r->runs[i] = l;
      ++i;
      ++jt;
    }

    free(in);

    return out;
}

static vector<string> sRuns;
static int pageNumber = 0;
static int maxPageNumber = 0;
static int linesPerPage = 25;
char* FancyText::parseText(FancyText* r, char* b, int length) {
    sRuns.clear();
    pageNumber = 0;
    maxPageNumber = 0;

    // tokenize text file
    list<Run> tempRuns;
    int li = 0;
    string ss(b);
    Run run;
    // int lastbreak = 0;
    for (int i = 0; i < length; ++i) {
      if (b[i] == '\n') {
        bool bBreak = true;
        if( User::options.txtWrapCR > 0 )
        {
          // if( i-lastbreak < 100 )
          // {
          // 	b[i] = 'X';
          // 	bBreak = false;
          // } else
          // 	lastbreak = i;
          bBreak = true;
          for( int j = 1 ; j <= User::options.txtWrapCR+1 ; j++ )
          {
            if( i+j >= length || b[i+j] != 10 )
              bBreak = false;
          }
          if( !bBreak )
          {
            for( int j = 1 ; j <= User::options.txtWrapCR+1 ; j++ )
            {
              if( i+j < length && b[i+j] == 10 )
                b[i+j] = 32;
            }
          }
        }
        if( bBreak )
        {
          run.text = &b[li];
          string s = ss.substr(li, (i - li + 1));
          // psp2shell_print("string %i to  %i cstr: %s\n", li, (i - li), s.c_str());
          // sceKernelDelayThread(2*1000000);
          run.n = i - li;
          li = i+1;
          run.lineBreak = true;
          tempRuns.push_back(run);
          sRuns.push_back(s);
          
        }
      }
    }
    
    // last run
    run.text = &b[li];
    run.n = length - li;
    run.lineBreak = true;
    tempRuns.push_back(run);

    // create fast fixed size run array	
    r->runs = new Run[tempRuns.size()];
    r->nRuns = tempRuns.size();
    list<Run>::iterator it(tempRuns.begin());
    int i = 0;
    while (it != tempRuns.end()) {
      const Run& l = *it;
      r->runs[i] = l;
      //psp2shell_print("run[%i] - %s\n", i, (*it).text);
      ++i;
      ++it;
    }

    return b;
}

// extern "C" {
// extern unsigned int size_res_txtfont;
// extern unsigned char res_txtfont[];
// };

void FancyText::resetFonts() {
    if (font)
      font->release();
    // load font
    // bool useBuiltin = User::options.txtFont == "bookr:builtin";
    // if (!useBuiltin) {
    // 	font = FZFont::createFromFile((char*)User::options.txtFont.c_str(), User::options.txtSize, false);
    // 	useBuiltin = font == 0;
    // }
    // if (useBuiltin) {
    // 	font = FZFont::createFromMemory(res_txtfont, size_res_txtfont, User::options.txtSize, false);
    // }
    // font->texEnv(FZ_TEX_MODULATE);
    // font->filter(FZ_NEAREST, FZ_NEAREST);
}

int FancyText::updateContent() {
    if (lastFontSize != User::options.txtSize
    || lastFontFace != User::options.txtFont 
    || lastHeightPct != User::options.txtHeightPct // should be able to just resize view here
    || lastWrapCR != User::options.txtWrapCR )
      return _CMD_RELOAD;
    return 0;
}

int FancyText::resume() {
    return 0;
}

static int strpos(string haystack, char needle, int nth)
{// Will return position of n-th occurence of a char in a string.
    if (nth == 0)
      return 0;
    string read;    // A string that will contain the read part of the haystack
    for (int i=1 ; i<nth+1 ; ++i)
    {
            std::size_t found = haystack.find(needle);
            read += haystack.substr(0,found+1); // the read part of the haystack is stocked in the read string
            haystack.erase(0, found+1);     // remove the read part of the haystack up to the i-th needle
            if (i == nth)
            {
                    return read.size();
            }
    }
    return -1;
}

void FancyText::renderContent() {
    FZScreen::clear(User::options.colorSchemes[User::options.currentScheme].txtBGColor & 0xffffff, FZ_COLOR_BUFFER);
    FZScreen::color(0xffffffff);
    FZScreen::matricesFor2D(rotation);
    FZScreen::enable(FZ_TEXTURE_2D);
    FZScreen::enable(FZ_GL_BLEND);
    FZScreen::blendFunc(FZ_ADD, FZ_SRC_ALPHA, FZ_ONE_MINUS_SRC_ALPHA);

    #ifdef PSP
      font->bindForDisplay();
      //bool txtJustify; ??
      FZScreen::ambientColor(0xff000000 | User::options.colorSchemes[User::options.currentScheme].txtFGColor);
    #endif

    #ifdef __vita__
      // psp2shell_print("sruns size: %i", );
      if (maxPageNumber == 0)
        maxPageNumber = (int) floor( (float)sRuns.size() / float(linesPerPage) );

      int condition;
      if (pageNumber >= maxPageNumber)
        condition = (int)sRuns.size();
      else
        condition = ((pageNumber+1) * linesPerPage);
      
      // psp2shell_print("currentpage :   %i\n", pageNumber);
      // psp2shell_print("maxPageNumber : %i\n", maxPageNumber);
      // psp2shell_print("condition :     %i\n", condition);

      for (int i = (pageNumber * linesPerPage); i < condition; i++) {
        FZScreen::drawText(20, 40 + (20 * (i % linesPerPage)), RGBA8(0, 0, 0, 255), 1.0f, sRuns[i].c_str());
      }

      
      
    #endif

    //bool txtJustify; ??

    /* Seems to address justification
      FZScreen::ambientColor(0xff000000 | User::options.colorSchemes[User::options.currentScheme].txtFGColor);
      int y = 10;
      int bn = topLine + linesPerPage;
      if (bn >= nLines) bn = nLines;
      psp2shell_print("render::content - topLine: %i\n", topLine);
      psp2shell_print("render::content - linesPerPage: %i\n", linesPerPage);
      psp2shell_print("render::content - bn: %i\n", bn);
      for (int i = topLine; i < bn; i++) {
        Run* run = &runs[lines[i].firstRun];
        int offset = lines[i].firstRunOffset;
        int x = 10;
        int n = lines[i].totalChars;
        do {
          int pn = n < run->n ? n : run->n;
          if (pn > 0) {
            #ifdef PSP
              x = drawText(&run->text[offset], font, x, y, pn, false, User::options.txtJustify, lines[i].spaceWidth, true);
            #elif defined(__vita__)
              psp2shell_print("render::content ------- start\n");
                psp2shell_print("run->text[offset] %s\n", &run->text[offset]);
                psp2shell_print("x,y : %i, %i\n", x, y);
              psp2shell_print("render::content ------- end\n");
              //FZScreen::drawText(x, y, RGBA8(0, 0, 0, 255), 1.0f, &run->text[offset]);
            #endif
          }
          n -= pn;
          offset = 0;
          ++run;
        } while (n > 0);
        
        y += 10*(User::options.txtHeightPct/100.0);

        //y += lines[i].vSpace;
        //if (y > maxY)
        //	break;
      }
    */

    FZScreen::matricesFor2D();
}

int FancyText::setLine(int l) {
    int oldP = getCurrentPage();
    int oldTL = topLine;
    topLine = l;
    if (topLine >= nLines)
      topLine = nLines - 1;
    if (topLine < 0)
      topLine = 0;
    int cp = getCurrentPage();
    if (cp != oldP) {
      char t[256];
      snprintf(t, 256, "Page %d", cp);
      setBanner(t);
    }
    return oldTL != topLine ? _CMD_MARK_DIRTY : 0;
}

int FancyText::runForLine(int l) {
    if (l >= nLines || l < 0)
      return 0;
    return lines[l].firstRun;
}

int FancyText::lineForRun(int r) {
    int l = 0;
    for (; l < nLines; l++) {
      if (lines[l].firstRun >= r)
        break;
    }
    return l;
}

bool FancyText::isPaginated() {
    return true;
}

int FancyText::getTotalPages() {
    return totalPages;
}

int FancyText::getCurrentPage() {
    return (topLine / linesPerPage) + 1;
}

int FancyText::setCurrentPage(int p) {
    if (p <= 0)
      p = 1;
    if (p > totalPages)
      p = totalPages;
    --p;
    return setLine(p * linesPerPage);
}

int FancyText::pan(int x, int y) {
  if (y > -32 && y < 32)
    y = 0;
  if (y == 0) {
    holdScroll = false;
    return 0;
  }
  if (y != 0 && !holdScroll) {
    holdScroll = true;
    return setLine(topLine + ((y < 0) ? -1 : 1));
  }
  return 0;
}

int FancyText::screenUp() {
    if (pageNumber > 0)
      pageNumber--;
    return setCurrentPage(getCurrentPage() - 1);
}

int FancyText::screenDown() {
    if (pageNumber < maxPageNumber)
      pageNumber++;
    return setCurrentPage(getCurrentPage() + 1);
}

int FancyText::screenLeft() {
    return setCurrentPage(getCurrentPage() - 10);
}

int FancyText::screenRight() {
    return setCurrentPage(getCurrentPage() + 10);
}

bool FancyText::isRotable() {
    return true;
}

int FancyText::getRotation() {
    return rotation;
}

int FancyText::setRotation(int r, bool bForce) {
    if (r == rotation && !bForce)
      return 0;
    rotation = r;
    int run = runForLine(topLine);
    if (rotation < 0)
      rotation = 3;
    if (rotation >= 4)
      rotation = 0;
    if (rotation == 0 || rotation == 2) {
      resizeView(480, 272);
      setLine(lineForRun(run));
    } else {
      resizeView(272, 480);
      setLine(lineForRun(run));
    }
    return _CMD_MARK_DIRTY;
}

bool FancyText::isBookmarkable() {
    return true;
}

void FancyText::getBookmarkPosition(map<string, float>& m) {
    m["topLineFirstRun"] = runForLine(topLine);
    m["zoom"] = 0;
    m["rotation"] = rotation;
}

int FancyText::setBookmarkPosition(map<string, float>& m) {
    setRotation(m["rotation"]);
    setLine(lineForRun(m["topLineFirstRun"]));
    return _CMD_MARK_DIRTY;
}

bool FancyText::isZoomable() {
    return false;
}

void FancyText::getZoomLevels(vector<Document::ZoomLevel>& v) {
}

int FancyText::getCurrentZoomLevel() {
    return 0;
}

int FancyText::setZoomLevel(int l) {
    return 0;
}

bool FancyText::hasZoomToFit() {
    return false;
}

int FancyText::setZoomToFitWidth() {
    return 0;
}

int FancyText::setZoomToFitHeight() {
    return 0;
}

}