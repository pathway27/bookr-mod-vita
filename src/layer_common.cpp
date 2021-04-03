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

#include "layer.hpp"
#include "graphics/resolutions.hpp"
#include "graphics/controls.hpp"
#include "ui/colours.hpp"

namespace bookr {

Layer::Layer() : topItem(0), selItem(0), skipChars(0), maxSkipChars(-1) {
}

Layer::~Layer() {
}

void Layer::drawImage(int x, int y) {}
void Layer::drawImage(int x, int y, int w, int h, int tx, int ty) {}
void Layer::drawImageScale(int x, int y, int w, int h, int tx, int ty, int tw, int th) {}
void Layer::drawTPill(int x, int y, int w, int h, int r, int tx, int ty) {}
void Layer::drawPill(int x, int y, int w, int h, int r, int tx, int ty) {}
void Layer::drawRect(int x, int y, int w, int h, int r, int tx, int ty) {}
int Layer::textWidthRange(char* t, int n, Font* font) {  return 0; }
int Layer::drawUTFMenuItem(MenuItem* item, Font* font, int x, int y, int skipPixels, int maxWidth) { return 0; }
int Layer::drawText(char* t, Font* font, int x, int y, int n, bool useLF, bool usePS, float ps, bool use3D) { return 0; }

void Layer::drawTextHC(char* t, Font* font, int y) {
  int w = textW(t, font);
  drawText(t, font, (480 - w) / 2, y);
}

int Layer::drawUTFText(const char* t, Font* font, int x, int y, int skipUTFChars, int maxWidth) {  return 0; }


void Layer::drawMenu(string& title, string& triangleLabel, vector<MenuItem>& items) {
  drawMenu(title, triangleLabel, items, false);
}

void Layer::drawMenu(string& title, string& triangleLabel, vector<MenuItem>& items, string& upperBreadCrumb) {
  drawMenu(title, triangleLabel, items, false);
  drawText(300, 83, BLACK, 1.0f, upperBreadCrumb.c_str());
}

static int countLines(string& t) {
  int lines = 1;
  char* c = (char*)t.c_str();
  while (*c != 0) {
    if (*c == 0xa) ++lines;
    ++c;
  }
  return lines;
}

void Layer::menuCursorUpdate(unsigned int buttons, int max) {
  // printf("MainMenu::updateMain buttons: %i\n", buttons);
  int* b = Screen::ctrlReps();
  // printf("MainMenu::updateMain b[User::controls.menuDown]: %i\n", b[User::controls.menuDown]);
  
  if (b[User::controls.menuUp] == 1 || (b[User::controls.menuUp] > 10 && b[User::controls.menuUp] % 5 == 0)) {
    selItem--;
    if (selItem < 0) {
      selItem = max - 1;
    }
    skipChars = 0;
    maxSkipChars = -1;
  }
  if (b[User::controls.menuDown] == 1 || (b[User::controls.menuDown] > 10 && b[User::controls.menuDown] % 5 == 0)) {
    selItem++;
    if (selItem >= max) {
      selItem = 0;
    }
    skipChars = 0;
    maxSkipChars = -1;
  }
  if (b[User::controls.menuLeft] == 1 || (b[User::controls.menuLeft] > 10 && b[User::controls.menuLeft] % 5 == 0)) {
    skipChars--;
    if (skipChars < 0) {
      skipChars = 0;
    }
  }
  if (b[User::controls.menuRight] == 1 || (b[User::controls.menuRight] > 10 && b[User::controls.menuRight] % 5 == 0)) {
    skipChars++;
    if (maxSkipChars >= 0 && skipChars>maxSkipChars)
      skipChars = maxSkipChars;
  }

  // printf("MainMenu::updateMain selItem %i\n", selItem);
}

}
