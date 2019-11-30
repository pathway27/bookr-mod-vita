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

#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/io/dirent.h> 
#include <psp2/io/stat.h>
#include <psp2/ctrl.h>
#include <psp2/power.h> 
#include <psp2/rtc.h>

#include <malloc.h>

#include "screen.hpp"
#include "texture.hpp"
#include "controls.hpp"

namespace bookr { namespace Screen {

unsigned int WIDTH = 960;
unsigned int HEIGHT = 544;
static bool closing = false;

// most stuff here comes directly from pspdev sdk examples
// default display list
//static unsigned int __attribute__((aligned(16))) list[262144];
static unsigned int* list;

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common) {
  return sceKernelExitProcess(0);
}


// yeah yeah yeah no mutex whatever whatever whatever
static volatile int powerResumed = 0;

#define VITA_RESUMING 0x00A00000

/* Power Callback */
int power_callback(int notifyId, int notifyCount, int powerInfo, void *common) {
  // callback doesn't get called without a file handle...?
  FILE *fd = fopen("ux0:data/Bookr/test_power_cb.txt", "a");

  #ifdef DEBUG
    // very hard to debug power callback messages without file
    // psp2shell disconnects during
    printf("notifyId %i, notifyCount %i, powerInfo 0x%08X\n", notifyId, notifyCount, powerInfo);
    printf("powerResumed %i\n", powerResumed);

    if (fd == NULL)
      return 0;

    SceDateTime time;
    sceRtcGetCurrentClockLocalTime(&time);
    fprintf(fd, "%04d/%02d/%02d %02d:%02d:%02d:%i notifyId %i, notifyCount %i, powerInfo 0x%08X\n", 
      sceRtcGetYear(&time), sceRtcGetMonth(&time), sceRtcGetDay(&time),
      sceRtcGetHour(&time), sceRtcGetMinute(&time), sceRtcGetSecond(&time), sceRtcGetMicrosecond(&time),
      notifyId, notifyCount, powerInfo);
  #endif

  
  // TODO: add some indication of re-loading file
  if (powerInfo & VITA_RESUMING) {
    powerResumed++;
  }
  
  return fclose(fd);
}

int CallbackThread(SceSize args, void *argp) {
  int cbid;
  cbid = sceKernelCreateCallback("Power Callback", 0, power_callback, NULL);

  #ifdef DEBUG
    printf("cbid %i,", cbid);
  #endif

  int registered = scePowerRegisterCallback(cbid);

  #ifdef DEBUG
    printf(" registered %i\n", registered);
  #endif

  while (1) {
    sceKernelDelayThreadCB(10 * 1000 * 1000);
  }

  return 0;
}

/* Sets up the callback thread and returns its thread id */
int setupCallbacks(void) {
  int thid = 0;

  thid = sceKernelCreateThread("update_thread", CallbackThread, 0x10000100, 0x10000, 0, 0, NULL);
  if (thid >= 0)
    sceKernelStartThread(thid, 0, 0);

  #ifdef DEBUG
    printf("thread created %i\n", thid);
  #endif

  return thid;
}

// from pspgl
static void * ptr_align64_uncached(unsigned long ptr) {
  // uncached access suck. better to use cached access and
  // flush dcache just before dlist issue
  //return (void *) (((ptr + 0x0f) & ~0x0f) | 0x40000000);
  return (void *) ((ptr + 0x3f) & ~0x3f);
}


static string psv_full_path;
static vita2d_pgf *pgf;
static void initalDraw() {
    vita2d_start_drawing();
    vita2d_clear_screen();

    vita2d_pgf_draw_text(pgf, 700, 30, RGBA8(255,255,255,255), 1.0f, "Hello in PGF");
    #ifdef __vita__
      vita2d_pgf_draw_text(pgf, 700, 207, RGBA8(255,255,255,255), 1.0f, "UTF-8 しません");
      //vita2d_pgf_draw_text(pgf, 700, 514, RGBA8(255,255,255,255), 1.0f, GIT_VERSION);
    #endif

    vita2d_pgf_draw_text(pgf, 0, 514, RGBA8(255,255,255,255), 1.0f, "Press L Trigger to Quit!");

    vita2d_end_drawing();
    vita2d_swap_buffers();
}

// Move this to constructor?
void open(int argc, char** argv) {
  setupCallbacks();

  vita2d_init();
  vita2d_set_clear_color(RGBA8(0, 0, 0, 255));

  pgf = vita2d_load_default_pgf();

  psv_full_path = "ux0:";

  struct SceIoStat st = {0};
  if (sceIoGetstat("ux0:data/Bookr", &st) == -1) {
      sceIoMkdir("ux0:data/Bookr", 0700);
  }

  #ifdef DEBUG
      initalDraw();
  #endif
}

void close() {
  vita2d_fini();
  vita2d_free_pgf(pgf);
}

void exit() {
  sceKernelExitProcess(0);
}

void drawText(int x, int y, unsigned int color, float scale, const char *text) {
  vita2d_pgf_draw_text(pgf, x, y, color, scale, text);
}

void setTextSize(float x, float y) {

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


void resetReps() {
  stickyKeys = true;
}

int* ctrlReps() {
  return breps;
}

void setupCtrl() {
  sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
  resetReps();
}

static volatile int lastAnalogX = 0;
static volatile int lastAnalogY = 0;
int readCtrl() {
  SceCtrlData pad;
  sceCtrlPeekBufferPositive(0, &pad, 1);
  updateReps(pad.buttons);
  lastAnalogX = pad.lx;
  lastAnalogY = pad.ly;
  return pad.buttons;
}

void getAnalogPad(int& x, int& y) {
  x = lastAnalogX - FZ_ANALOG_CENTER;
  y = lastAnalogY - FZ_ANALOG_CENTER;
}

void startDirectList() {
  #ifdef DEBUG_RENDER
    printf("start drawing");
  #endif
  vita2d_start_drawing();
}

void endAndDisplayList() {
  #ifdef DEBUG_RENDER
    printf("end drawing\n");
  #endif
  vita2d_end_drawing();
}

static void* lastFramebuffer = NULL;
void swapBuffers() {
    lastFramebuffer = vita2d_get_current_fb();
  #ifdef DEBUG_RENDER
    printf("swapBuffers\n");
  #endif
    vita2d_swap_buffers();
}

void waitVblankStart() {
  // vita2d_wait_rendering_done();
}

// void* getListMemory(int s) {
//   //return sceGuGetMemory(s);
// }

void shadeModel(int mode) {
  //sceGuShadeModel(mode);
}

void color(unsigned int c) {
  //sceGuColor(c);
}

void ambientColor(unsigned int c) {
  //sceGuAmbientColor(c);
}

void clear(unsigned int color, int b) {
  vita2d_set_clear_color(color);
  vita2d_clear_screen();
}

void checkEvents(int buttons) {
}

void matricesFor2D(int rotation) {
}

struct T32FV32F2D {
    float u,v;
    float x,y,z;
};

static Texture* boundTexture = 0;
void setBoundTexture(Texture *t) {
    boundTexture = t;
}

void drawRectangle(float x, float y, float w, float h, unsigned int color) {
  vita2d_draw_rectangle(x, y, w, h, color);
}

void drawFontText(Font *font, int x, int y, unsigned int color, unsigned int size, const char *text) {
  vita2d_font_draw_text(font->v_font, x, y, color, size, text);
}

void drawTextureScale(const Texture *texture, float x, float y, float x_scale, float y_scale) {
  vita2d_draw_texture_scale(texture->vita_texture, x, y, x_scale, y_scale);
}

void drawTextureTintScale(const Texture *texture, float x, float y, float x_scale, float y_scale, unsigned int color) {
  vita2d_draw_texture_tint_scale(texture->vita_texture, x, y, x_scale, y_scale, color);
}

void drawTextureTintScaleRotate(const Texture *texture, float x, float y, float x_scale, float y_scale, float rad, unsigned int color) {
  vita2d_draw_texture_tint_scale_rotate(texture->vita_texture, x, y, x_scale, y_scale, rad, color);
}



/*  Active Shader
    bind correct vertex array
  */
void drawArray(int prim, int vtype, int count, void* indices, void* vertices) {
  // vita2d_color_vertex *vertices = (vita2d_color_vertex *)vita2d_pool_memalign(
  // 	2 * sizeof(vita2d_color_vertex), // 2 vertices
  // 	sizeof(vita2d_color_vertex));

  // uint16_t *indices = (uint16_t *)vita2d_pool_memalign(
  // 	2 * sizeof(uint16_t), // 2 indices
  // 	sizeof(uint16_t));

  // vertices[0].x = x0;
  // vertices[0].y = y0;
  // vertices[0].z = +0.5f;
  // vertices[0].color = color;

  // vertices[1].x = x1;
  // vertices[1].y = y1;
  // vertices[1].z = +0.5f;
  // vertices[1].color = color;

  // indices[0] = 0;
  // indices[1] = 1;

  // sceGxmSetVertexProgram(_vita2d_context, _vita2d_colorVertexProgram);
  // sceGxmSetFragmentProgram(_vita2d_context, _vita2d_colorFragmentProgram);

  // void *vertexDefaultBuffer;
  // sceGxmReserveVertexDefaultUniformBuffer(_vita2d_context, &vertexDefaultBuffer);
  // sceGxmSetUniformDataF(vertexDefaultBuffer, _vita2d_colorWvpParam, 0, 16, _vita2d_ortho_matrix);

  // sceGxmSetVertexStream(_vita2d_context, 0, vertices);
  // sceGxmSetFrontPolygonMode(_vita2d_context, SCE_GXM_POLYGON_MODE_LINE);
  // sceGxmDraw(_vita2d_context, SCE_GXM_PRIMITIVE_LINES, SCE_GXM_INDEX_FORMAT_U16, indices, 2);
  // sceGxmSetFrontPolygonMode(_vita2d_context, SCE_GXM_POLYGON_MODE_TRIANGLE_FILL);
}

void copyImage(int psm, int sx, int sy, int width, int height, int srcw, void *src,
    int dx, int dy, int destw, void *dest) {
  //sceGuCopyImage(psm, sx, sy, width, height, srcw, src, dx, dy, destw, dest);
}

void drawPixel(float x, float y, unsigned int color) {
  vita2d_draw_pixel(x, y, color);
}

void* framebuffer() {
  return lastFramebuffer;
}

void blendFunc(int op, int src, int dst) {
  //sceGuBlendFunc(op, src, dst, 0, 0);
}

void enable(int m) {
  //sceGuEnable(m);
}

void disable(int m) {
  //sceGuDisable(m);
}

void dcacheWritebackAll() {
  // ksceKernelCpuDcacheWritebackAll();
}

string basePath() {
  return psv_full_path;
}

struct CompareDirent {
  bool operator()(const Dirent& a, const Dirent& b) {
      if ((a.stat & FZ_STAT_IFDIR) == (b.stat & FZ_STAT_IFDIR))
          return a.name < b.name;
      if (b.stat & FZ_STAT_IFDIR)
          return false;
      return true;
  }
};

int dirContents(const char* path, vector<Dirent>& a) {
  SceUID fd;
  SceIoDirent *findData;
  findData = (SceIoDirent*)memalign(16, sizeof(SceIoDirent));	// dont ask me WHY...
  memset((void*)findData, 0, sizeof(SceIoDirent));
  //a.push_back(Dirent("books/udhr.pdf", FZ_STAT_IFREG, 0));
  //a.push_back(Dirent("books/1984.txt", FZ_STAT_IFREG, 587083));
  fd = sceIoDopen(path);
  if (fd < 0)
      return -1;
  while (sceIoDread(fd, findData) > 0) {
      if (findData->d_name[0] != 0 && findData->d_name[0] != '.') {
          a.push_back(Dirent(findData->d_name, findData->d_stat.st_mode, findData->d_stat.st_size));
      }
  }
  sceIoDclose(fd);
  free(findData);
  sort(a.begin(), a.end(), CompareDirent());
  return 1;
}

int getSuspendSerial() {
  return powerResumed;
}

// static int speedValues[14] = {
//  0, 0,
//  10, 5,
//  25, 12,
//  50, 25,
//  111, 55, 
//  222, 111,
//  333, 166
// };
void setSpeed(int v) {
  if (v <= 0 || v > 6)
      return;

  //scePowerSetClockFrequency(speedValues[v*2], speedValues[v*2], speedValues[v*2+1]);

  // scePowerSetArmClockFrequency(speedValues[v*2]);
  //scePowerSetCpuClockFrequency(speedValues[v*2]);

  // scePowerSetBusClockFrequency(speedValues[v*2+1]);
}

int getSpeed() {
  return scePowerGetArmClockFrequency();
}

void getTime(int &h, int &m) {
  SceDateTime time;
  // same
  if (sceRtcGetCurrentClockLocalTime(&time) >= 0) {
      h = time.hour;
      m = time.minute;
  }
}

int getBattery() {
  return scePowerGetBatteryLifePercent();
}

int getUsedMemory() {
  struct mallinfo mi = mallinfo();
  return mi.uordblks;
  //return mi.arena;
}

void setBrightness(int b){
  return;
}

bool isClosing() {
  return closing;
}

} // namespace Screen 
} // namespace bookr