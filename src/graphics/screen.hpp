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

#ifndef SCREEN_H
#define SCREEN_H

#include <vector>
#include <algorithm>
#include <string>

#include "font.hpp"

using std::string;
using std::vector;

class Texture;

#define FZ_STAT_IFDIR 0x1000
#define FZ_STAT_IFREG 0x2000
struct Dirent {
  string name;
  string sname;
  int stat;
  int size;
  Dirent(string n, int st, int s) : name(n), stat(st), size(s) { }
  Dirent() : name(""), sname(""), stat(0), size(0) { }
  Dirent(string n, string sn, int st, int s) : name(n), sname(sn), stat(st), size(s) { }
};

/*! \brief An abstraction over a platform's basic functionality and graphics.
 *
 *  Each platform will have it's own implementation file.
 *  TODO: Convert to namespace; add simple draw methods.
 */
namespace bookr {

namespace Screen {
  int setupCallbacks();

  /**
   * Open the screen for rendering.
   */
  void open(int argc, char** argv);

  /**
   * Close the screen for rendering.
   */
  void close();

  void exit();

  /**
   * Start list for inmediate display.
   */
  void startDirectList();

  /**
   * End list and display it.
   */
  void endAndDisplayList();

  /**
   * Commit dirty cache.
   */

  void commitAll();

  void commitRange(const void* p, unsigned int size);

  /**
   * Swap frame and draw buffer.
   */
  void swapBuffers();
  void waitVblankStart();

  void* getListMemory(int s);

  void enable(int m);

  void disable(int m);

  void checkEvents(int buttons);

  void color(unsigned int c);

  void ambientColor(unsigned int c);

  void clear(unsigned int c, int b);

  void matricesFor2D(int rotation = 0);

  void drawArray(int prim, int vtype, int count, void* indices, void* vertices);

  void drawArray(int prim, int count, void* indices, void* vertices);

  void drawArray();

  void setTextSize(float x, float y);
  void drawText(int x, int y, unsigned int color, float scale, const char *text);

  void copyImage(int psm, int sx, int sy, int width, int height, int srcw, void *src,
    int dx, int dy, int destw, void *dest);

  void drawPixel(float x, float y, unsigned int color);
  void drawRectangle(float x, float y, float w, float h, unsigned int color);

  // TODO: Refactor to FZFont?
  void drawFontText(Font *font, int x, int y, unsigned int color, unsigned int size, const char *text);
  void drawFontTextf(Font *font, int x, int y, unsigned int color, unsigned int size, const char *text, ...);

  // TODO: Refactor to Texture

  void drawTextureScale(const Texture *texture, float x, float y, float x_scale, float y_scale);
  void drawTextureTintScale(const Texture *texture, float x, float y, float x_scale, float y_scale, unsigned int color);
  void drawTextureTintScaleRotate(const Texture *texture, float x, float y, float x_scale, float y_scale, float rad, unsigned int color);

  void* framebuffer();

  void blendFunc(int op, int src, int dst);
  void shadeModel(int mode);

  void setupCtrl();

  int readCtrl();

  bool isClosing();

  void getAnalogPad(int& x, int& y);
  void resetReps();
  int* ctrlReps();
  const char* nameForButton(int mask);
  const char* nameForButtonReps(int button);
  int repsForButtonMask(int mask);

  void dcacheWritebackAll();

  void setBoundTexture(Texture *);

  string basePath();
  int dirContents(const char* path, vector<Dirent>& a);

  int getSuspendSerial();

  static char *speedLabels[14];
  static int speedValues;
  void setSpeed(int v);
  int getSpeed();

  void getTime(int &h, int &m);
  int getBattery();
  int getUsedMemory();
  void setBrightness(int);

	static const char* browserTextSizes;
	static const char* browserDisplayModes;
	static const char* browserInterfaceModes;
} // Screen

#ifdef MAC
  #include <map>
  using std::map;
  map<string, GLuint> VBOs, VAOs, EBOs, textures;
#endif

} // ::bookr

#endif