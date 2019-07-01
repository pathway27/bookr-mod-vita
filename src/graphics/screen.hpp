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

#ifndef SCREEN_H
#define SCREEN_H

#include <vector>
#include <algorithm>
#include <string>

#include "font.hpp"

using namespace std;

class FZTexture;

#define FZ_STAT_IFDIR 0x1000
#define FZ_STAT_IFREG 0x2000
struct FZDirent {
  string name;
  string sname;
  int stat;
  int size;
  FZDirent(string n, int st, int s) : name(n), stat(st), size(s) { }
  FZDirent() : name(""), sname(""), stat(0), size(0) { }
  FZDirent(string n, string sn, int st, int s) : name(n), sname(sn), stat(st), size(s) { }
};

/*! \brief An abstraction over a platform's basic functionality and graphics.
 *
 *  Each platform will have it's own implementation file.
 *  TODO: Convert to namespace; add simple draw methods.
 */
namespace bookr {

namespace Screen {
  static int setupCallbacks();

  /**
   * Open the screen for rendering.
   */
  static void open(int argc, char** argv);

  /**
   * Close the screen for rendering.
   */
  static void close();

  static void exit();

  /**
   * Start list for inmediate display.
   */
  static void startDirectList();

  /**
   * End list and display it.
   */
  static void endAndDisplayList();

  /**
   * Commit dirty cache.
   */

  static void commitAll();

  static void commitRange(const void* p, unsigned int size);

  /**
   * Swap frame and draw buffer.
   */
  static void swapBuffers();
  static void waitVblankStart();

  static void* getListMemory(int s);

  static void enable(int m);

  static void disable(int m);

  static void checkEvents(int buttons);

  static void color(unsigned int c);

  static void ambientColor(unsigned int c);

  static void clear(unsigned int c, int b);

  static void matricesFor2D(int rotation = 0);

  static void drawArray(int prim, int vtype, int count, void* indices, void* vertices);

  static void drawArray(int prim, int count, void* indices, void* vertices);

  static void drawArray();

  static void setTextSize(float x, float y);
  static void drawText(int x, int y, unsigned int color, float scale, const char *text);

  static void copyImage(int psm, int sx, int sy, int width, int height, int srcw, void *src,
    int dx, int dy, int destw, void *dest);

  static void drawPixel(float x, float y, unsigned int color);
  static void drawRectangle(float x, float y, float w, float h, unsigned int color);

  // TODO: Refactor to FZFont?
  static void drawFontText(FZFont *font, int x, int y, unsigned int color, unsigned int size, const char *text);
  static void drawFontTextf(FZFont *font, int x, int y, unsigned int color, unsigned int size, const char *text, ...);

  // TODO: Refactor to FZTexture

  static void drawTextureScale(const FZTexture *texture, float x, float y, float x_scale, float y_scale);
  static void drawTextureTintScale(const FZTexture *texture, float x, float y, float x_scale, float y_scale, unsigned int color);
  static void drawTextureTintScaleRotate(const FZTexture *texture, float x, float y, float x_scale, float y_scale, float rad, unsigned int color);

  static void* framebuffer();

  static void blendFunc(int op, int src, int dst);
  static void shadeModel(int mode);

  static void setupCtrl();

  static int readCtrl();

  static bool isClosing();

  static void getAnalogPad(int& x, int& y);
  static void resetReps();
  static int* ctrlReps();
  static const char* nameForButton(int mask);
  static const char* nameForButtonReps(int button);
  static int repsForButtonMask(int mask);

  static void dcacheWritebackAll();

  static void setBoundTexture(FZTexture *);

  static string basePath();
  static int dirContents(const char* path, vector<FZDirent>& a);

  static int getSuspendSerial();

  // extern char *speedLabels[7];
  // static int speedValues;
  // static void setSpeed(int v);
  // static int getSpeed();

  static void getTime(int &h, int &m);
  static int getBattery();
  static int getUsedMemory();
  static void setBrightness(int);

	// static const char* browserTextSizes;
	// static const char* browserDisplayModes;
	// static const char* browserInterfaceModes;
} // Screen

#ifdef MAC
  #include <map>
  static map<string, GLuint> VBOs, VAOs, EBOs, textures;
#endif

} // ::bookr

#endif