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


#ifndef FZSCREEN_H
#define FZSCREEN_H

#include <vector>
#include <algorithm>
#include <string>
#include <cstring>
#include <map>

// TODO: Move these includes to implementation
#if defined(MAC) || defined(WIN32)
  #define GLEW_STATIC
  #include <GL/glew.h>
  #include <SOIL.h>

  #include <glm/glm.hpp>
  #include <glm/gtc/matrix_transform.hpp>
  #include <glm/gtc/type_ptr.hpp>

  #include <GLFW/glfw3.h>
#elif defined(__vita__)
  #include <psp2/kernel/processmgr.h>
  #include <psp2/ctrl.h>
  #include <psp2/power.h>
  #include <psp2/rtc.h>

  #include <psp2/pvf.h>
  #include <psp2/pgf.h>
  #include <psp2/io/dirent.h>
  #include <psp2/io/fcntl.h>

  #include <vita2d.h>

  #include <malloc.h>
  #include <string.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
#elif defined(SWITCH)
  #include <switch.h>
#endif

#include "fzfont.h"

using namespace std;

/**
 * Control the screen and rendering. This class is very biased
 * towards libgu on the PSP. The GL backend is just for devel;
 * it is slow and incomplete.
 */

// 100% replaceable with pure C, kinda sucks right now, but does
// its job of isolating libGU and GL

// TODO: Move these to fz graphics header
#define FZ_COLOR_BUFFER          1
#define FZ_DEPTH_BUFFER          2

#define FZ_SRC_COLOR             0
#define FZ_ONE_MINUS_SRC_COLOR   1
#define FZ_SRC_ALPHA             2
#define FZ_ONE_MINUS_SRC_ALPHA   3
#define FZ_DST_COLOR             0
#define FZ_ONE_MINUS_DST_COLOR   1

#define FZ_ADD                  0
#define FZ_SUBTRACT             1

#define FZ_FLAT                 0
#define FZ_SMOOTH               1

#define FZ_ALPHA_TEST           0
#define FZ_DEPTH_TEST           1
#define FZ_SCISSOR_TEST         2
#define FZ_STENCIL_TEST         3
#define FZ_GL_BLEND             4
#define FZ_CULL_FACE            5
#define FZ_DITHER               6
#define FZ_FOG                  7
#define FZ_CLIP_PLANES          8
#define FZ_TEXTURE_2D           9
#define FZ_LIGHTING             10
#define FZ_LIGHT0               11
#define FZ_LIGHT1               12
#define FZ_LIGHT2               13
#define FZ_LIGHT3               14
//#define GU_UNKNOWN_15           (15)
//#define GU_UNKNOWN_16           (16)
#define FZ_COLOR_TEST           17
#define FZ_COLOR_LOGIC_OP       18
#define FZ_FACE_NORMAL_REVERSE  19
#define FZ_PATCH_FACE           20
#define FZ_FRAGMENT_2X          21

// TODO: Move these to fz controls header
#define FZ_CTRL_SELECT          0x000001
#define FZ_CTRL_START           0x000008
#define FZ_CTRL_UP              0x000010
#define FZ_CTRL_RIGHT           0x000020
#define FZ_CTRL_DOWN            0x000040
#define FZ_CTRL_LEFT            0x000080
#define FZ_CTRL_LTRIGGER        0x000100
#define FZ_CTRL_RTRIGGER        0x000200
#define FZ_CTRL_TRIANGLE        0x001000
#define FZ_CTRL_CIRCLE          0x002000
#define FZ_CTRL_CROSS           0x004000
#define FZ_CTRL_SQUARE          0x008000
#define FZ_CTRL_HOME            0x010000
#define FZ_CTRL_HOLD            0x020000
#define FZ_CTRL_NOTE            0x800000

#define FZ_ANALOG_CENTER 128
#define FZ_ANALOG_THRESHOLD 50
#define FZ_ANALOG_SENSITIVITY 16

#ifdef __vita__
  #define FZ_CTRL_L3          0x000002
  #define FZ_CTRL_R3          0x000004
  #define FZ_CTRL_INTERCEPTED 0x010000
  #define FZ_CTRL_VOLUP       0x100000
  #define FZ_CTRL_VOLDOWN     0x200000
#elif defined(PSP)
  #define FZ_CTRL_L3
  #define FZ_CTRL_R3
  #define FZ_CTRL_INTERCEPTED
  #define FZ_CTRL_VOLUP
  #define FZ_CTRL_VOLDOWN
#endif


#define FZ_REPS_SELECT     1
#define FZ_REPS_START      2
#define FZ_REPS_UP         3
#define FZ_REPS_RIGHT      4
#define FZ_REPS_DOWN       5
#define FZ_REPS_LEFT       6
#define FZ_REPS_LTRIGGER   7
#define FZ_REPS_RTRIGGER   8
#define FZ_REPS_TRIANGLE   9
#define FZ_REPS_CIRCLE     10
#define FZ_REPS_CROSS      11
#define FZ_REPS_SQUARE     12
#define FZ_REPS_HOME       13
#define FZ_REPS_HOLD       14
#define FZ_REPS_NOTE       15


#define FZ_POINTS               (0)
#define FZ_LINES                (1)
#define FZ_LINE_STRIP           (2)
#define FZ_TRIANGLES            (3)
#define FZ_TRIANGLE_STRIP       (4)
#define FZ_TRIANGLE_FAN         (5)
#define FZ_SPRITES              (6)

#define FZ_TEXTURE_SHIFT(n)     ((n)<<0)
#define FZ_TEXTURE_8BIT         FZ_TEXTURE_SHIFT(1)
#define FZ_TEXTURE_16BIT        FZ_TEXTURE_SHIFT(2)
#define FZ_TEXTURE_32BITF       FZ_TEXTURE_SHIFT(3)

#define FZ_COLOR_SHIFT(n)       ((n)<<2)
#define FZ_COLOR_RES1           FZ_COLOR_SHIFT(1)
#define FZ_COLOR_RES2           FZ_COLOR_SHIFT(2)
#define FZ_COLOR_RES3           FZ_COLOR_SHIFT(3)
#define FZ_COLOR_5650           FZ_COLOR_SHIFT(4)
#define FZ_COLOR_5551           FZ_COLOR_SHIFT(5)
#define FZ_COLOR_4444           FZ_COLOR_SHIFT(6)
#define FZ_COLOR_8888           FZ_COLOR_SHIFT(7)

#define FZ_NORMAL_SHIFT(n)      ((n)<<5)
#define FZ_NORMAL_8BIT          FZ_NORMAL_SHIFT(1)
#define FZ_NORMAL_16BIT         FZ_NORMAL_SHIFT(2)
#define FZ_NORMAL_32BITF        FZ_NORMAL_SHIFT(3)

#define FZ_VERTEX_SHIFT(n)      ((n)<<7)
#define FZ_VERTEX_8BIT          FZ_VERTEX_SHIFT(1)
#define FZ_VERTEX_16BIT         FZ_VERTEX_SHIFT(2)
#define FZ_VERTEX_32BITF        FZ_VERTEX_SHIFT(3)

#define FZ_WEIGHT_SHIFT(n)      ((n)<<9)
#define FZ_WEIGHT_8BIT          FZ_WEIGHT_SHIFT(1)
#define FZ_WEIGHT_16BIT         FZ_WEIGHT_SHIFT(2)
#define FZ_WEIGHT_32BITF        FZ_WEIGHT_SHIFT(3)

#define FZ_INDEX_SHIFT(n)       ((n)<<11)
#define FZ_INDEX_8BIT           FZ_INDEX_SHIFT(1)
#define FZ_INDEX_16BIT          FZ_INDEX_SHIFT(2)
#define FZ_INDEX_RES3           FZ_INDEX_SHIFT(3)

#define FZ_TRANSFORM_SHIFT(n)   ((n)<<23)
#define FZ_TRANSFORM_3D         FZ_TRANSFORM_SHIFT(0)
#define FZ_TRANSFORM_2D         FZ_TRANSFORM_SHIFT(1)

#define FZ_PSM_5650             (0) /* Display, Texture, Palette */
#define FZ_PSM_5551             (1) /* Display, Texture, Palette */
#define FZ_PSM_4444             (2) /* Display, Texture, Palette */
#define FZ_PSM_8888             (3) /* Display, Texture, Palette */
#define FZ_PSM_T4               (4) /* Texture */
#define FZ_PSM_T8               (5) /* Texture */

#define FZ_SCREEN_WIDTH					960
#define FZ_SCREEN_HEIGHT				544

#ifdef PSP
  #include <pspdebug.h>
  #define FZ_DEBUG_SCREEN_INIT pspDebugScreenInit();
  #define FZ_DEBUG_SCREEN_SET00 pspDebugScreenSetXY(0,0);
  #define printf pspDebugScreenPrintf
#elif defined(__vita__) && defined(DEBUG)
  #include <psp2/kernel/clib.h>
  #define printf sceClibPrintf
#else
  #define FZ_DEBUG_SCREEN_INIT
  #define FZ_DEBUG_SCREEN_SET00
#endif

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
class FZScreen {
protected:
  FZScreen();
  ~FZScreen();

public:
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

  static const char* speedLabels[];
  static int speedValues[];
  static void setSpeed(int v);
  static int getSpeed();

  static void getTime(int &h, int &m);
  static int getBattery();
  static int getUsedMemory();
  static void setBrightness(int);

	static const char* const browserTextSizes[];
	static const char* const browserDisplayModes[];
	static const char* const browserInterfaceModes[];
};

#ifdef MAC
  static map<string, GLuint> VBOs, VAOs, EBOs, textures;
#endif

#endif

struct T32FV32F2D
{
  float u, v;
  float x, y, z;
};