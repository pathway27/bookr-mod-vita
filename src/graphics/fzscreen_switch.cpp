/*
 * Bookr % VITA: document reader for the Sony PS Vita
 * Copyright (C) 2017 Sreekara C. (pathway27 at gmail dot com)
 *
 * IS A MODIFICATION OF THE ORIGINAL
 *
 * Bookr and bookr-mod for PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com),
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com),

 * AND VARIOUS OTHER FORKS.
 * See Forks in the README for more info
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "fzscreen.h"
#include "fztexture.h"

// Move this to constructor?
void FZScreen::open(int argc, char** argv) {
  
}


int FZScreen::setupCallbacks(void) {
  return 0;
}

static void initalDraw() {

}

void FZScreen::close() {

}

void FZScreen::exit() {

}

void FZScreen::drawText(int x, int y, unsigned int color, float scale, const char *text) {

}

void FZScreen::drawFontTextf(FZFont *font, int x, int y, unsigned int color, unsigned int size, const char *text, ...) {

}

void FZScreen::setTextSize(float x, float y) {

}

static bool stickyKeys = false;

static int breps[16];
static void updateReps(int keyState) {
  if (stickyKeys && keyState == 0) {
    stickyKeys = false;
  }
  if (stickyKeys) {
    memset((void*)breps, 0, sizeof(int)*16);
    return;
  }
  if (keyState & FZ_CTRL_SELECT  ) breps[FZ_REPS_SELECT  ]++; else breps[FZ_REPS_SELECT  ] = 0;
  if (keyState & FZ_CTRL_START   ) breps[FZ_REPS_START   ]++; else breps[FZ_REPS_START   ] = 0;
  if (keyState & FZ_CTRL_UP      ) breps[FZ_REPS_UP      ]++; else breps[FZ_REPS_UP      ] = 0;
  if (keyState & FZ_CTRL_RIGHT   ) breps[FZ_REPS_RIGHT   ]++; else breps[FZ_REPS_RIGHT   ] = 0;
  if (keyState & FZ_CTRL_DOWN    ) breps[FZ_REPS_DOWN    ]++; else breps[FZ_REPS_DOWN    ] = 0;
  if (keyState & FZ_CTRL_LEFT    ) breps[FZ_REPS_LEFT    ]++; else breps[FZ_REPS_LEFT    ] = 0;
  if (keyState & FZ_CTRL_LTRIGGER) breps[FZ_REPS_LTRIGGER]++; else breps[FZ_REPS_LTRIGGER] = 0;
  if (keyState & FZ_CTRL_RTRIGGER) breps[FZ_REPS_RTRIGGER]++; else breps[FZ_REPS_RTRIGGER] = 0;
  if (keyState & FZ_CTRL_TRIANGLE) breps[FZ_REPS_TRIANGLE]++; else breps[FZ_REPS_TRIANGLE] = 0;
  if (keyState & FZ_CTRL_CIRCLE  ) breps[FZ_REPS_CIRCLE  ]++; else breps[FZ_REPS_CIRCLE  ] = 0;
  if (keyState & FZ_CTRL_CROSS   ) breps[FZ_REPS_CROSS   ]++; else breps[FZ_REPS_CROSS   ] = 0;
  if (keyState & FZ_CTRL_SQUARE  ) breps[FZ_REPS_SQUARE  ]++; else breps[FZ_REPS_SQUARE  ] = 0;
  if (keyState & FZ_CTRL_HOME    ) breps[FZ_REPS_HOME    ]++; else breps[FZ_REPS_HOME    ] = 0;
  if (keyState & FZ_CTRL_HOLD    ) breps[FZ_REPS_HOLD    ]++; else breps[FZ_REPS_HOLD    ] = 0;
  if (keyState & FZ_CTRL_NOTE    ) breps[FZ_REPS_NOTE    ]++; else breps[FZ_REPS_NOTE    ] = 0;
}


void FZScreen::resetReps() {

}

int* FZScreen::ctrlReps() {
  return breps;
}

void FZScreen::setupCtrl() {

}

int FZScreen::readCtrl() {
  return 0;
}

void FZScreen::getAnalogPad(int& x, int& y) {

}

void FZScreen::startDirectList() {

}

void FZScreen::endAndDisplayList() {

}


void FZScreen::swapBuffers() {

}

void FZScreen::waitVblankStart() {

}

void* FZScreen::getListMemory(int s) {
  return 0;
}

void FZScreen::shadeModel(int mode) {

}

void FZScreen::color(unsigned int c) {

}

void FZScreen::ambientColor(unsigned int c) {

}

void FZScreen::clear(unsigned int color, int b) {

}

void FZScreen::checkEvents(int buttons) {
}

void FZScreen::matricesFor2D(int rotation) {
}

struct T32FV32F2D {
    float u,v;
    float x,y,z;
};

void FZScreen::setBoundTexture(FZTexture *t) {

}

void FZScreen::drawRectangle(float x, float y, float w, float h, unsigned int color) {

}

void FZScreen::drawFontText(FZFont *font, int x, int y, unsigned int color, unsigned int size, const char *text) {

}

void FZScreen::drawTextureScale(const FZTexture *texture, float x, float y, float x_scale, float y_scale) {

}

void FZScreen::drawTextureTintScale(const FZTexture *texture, float x, float y, float x_scale, float y_scale, unsigned int color) {

}

void FZScreen::drawTextureTintScaleRotate(const FZTexture *texture, float x, float y, float x_scale, float y_scale, float rad, unsigned int color) {

}

/*  Active Shader
    bind correct vertex array
  */
void FZScreen::drawArray(int prim, int vtype, int count, void* indices, void* vertices) {
}

void FZScreen::copyImage(int psm, int sx, int sy, int width, int height, int srcw, void *src,
    int dx, int dy, int destw, void *dest) {

}

void FZScreen::drawPixel(float x, float y, unsigned int color) {

}

void* FZScreen::framebuffer() {
  return 0;
}

void FZScreen::blendFunc(int op, int src, int dst) {

}

void FZScreen::enable(int m) {

}

void FZScreen::disable(int m) {

}

void FZScreen::dcacheWritebackAll() {

}

string FZScreen::basePath() {
  return "";
}

struct CompareDirent {
  bool operator()(const FZDirent& a, const FZDirent& b) {
      if ((a.stat & FZ_STAT_IFDIR) == (b.stat & FZ_STAT_IFDIR))
          return a.name < b.name;
      if (b.stat & FZ_STAT_IFDIR)
          return false;
      return true;
  }
};

int FZScreen::dirContents(const char* path, vector<FZDirent>& a) {
  return 0;
}

int FZScreen::getSuspendSerial() {
  return 0;
}

void FZScreen::setSpeed(int v) {

}

int FZScreen::getSpeed() {
  return 0;
}

void FZScreen::getTime(int &h, int &m) {

}

int FZScreen::getBattery() {
  return 0;
}

int FZScreen::getUsedMemory() {
  return 0;
}

void FZScreen::setBrightness(int b){

}

bool FZScreen::isClosing() {
  return false;
}
