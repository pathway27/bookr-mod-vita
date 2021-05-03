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

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <switch.h>
#include <malloc.h>
#include <dirent.h>

#include <EGL/egl.h>    // EGL library
#include <EGL/eglext.h> // EGL extensions
#include <glad/glad.h>  // glad library (OpenGL loader)

#include "screen.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "../resource_manager.hpp"

#include "textures_frag.h"
#include "textures_vert.h"
#include "text_vert.h"
#include "text_frag.h"

#include "NotoSans-Regular_ttf.h"
#include "icon0_t_png.h"
#include "circle-outline_png.h"
#include "close-box-outline_png.h"
#include "triangle-outline_png.h"
#include "clock_png.h"
#include "battery_outline_png.h"
#include "memory_png.h"

#include <filesystem>
#include <iostream>

//-----------------------------------------------------------------------------
// nxlink support
//-----------------------------------------------------------------------------
#ifndef DEBUG
  #define TRACE(fmt, ...) ((void)0)
#else
  #include <unistd.h>
  #define TRACE(fmt, ...) printf("%s: " fmt "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__)

  static int s_nxlinkSock = -1;

  static void initNxLink()
  {
    if (R_FAILED(socketInitializeDefault()))
      return;

    s_nxlinkSock = nxlinkStdio();
    if (s_nxlinkSock >= 0)
      TRACE("printf output now goes to nxlink server");
    else
      socketExit();
  }

  static void deinitNxLink()
  {
    if (s_nxlinkSock >= 0)
    {
      close(s_nxlinkSock);
      socketExit();
      s_nxlinkSock = -1;
    }
  }

  extern "C" void userAppInit()
  {
    initNxLink();
  }

  extern "C" void userAppExit()
  {
    deinitNxLink();
  }
#endif

namespace bookr { namespace Screen {

unsigned int WIDTH = 1280;
unsigned int HEIGHT = 720;

static EGLDisplay s_display;
static EGLContext s_context;
static EGLSurface s_surface;

static bool initEgl(NWindow *win);
static void loadShaders();

static PadState pad;
// Move this to constructor?
void open(int argc, char **argv)
{
  #ifdef DEBUG
    printf("Screen::open\n");
    // Set mesa configuration (useful for debugging)
    // setMesaConfig();
  #endif
  // Initialize EGL on the default window
  if (!initEgl(nwindowGetDefault())) {
    #ifdef DEBUG
      printf("!initEgl(nwindowGetDefault())\n");
    #endif
    return;
  }

  gladLoadGL();
  #ifdef DEBUG
    printf("gladLoadGL()\n");
  #endif

  glViewport(0, 0, WIDTH, HEIGHT);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  std::cout << glGetString(GL_VERSION) << std::endl;

  loadShaders();

  padConfigureInput(8, HidNpadStyleSet_NpadStandard);

  padInitializeAny(&pad);
}

//-----------------------------------------------------------------------------
// EGL initialization
//-----------------------------------------------------------------------------

static bool initEgl(NWindow *win)
{
  // Connect to the EGL default display
  s_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (!s_display)
  {
    TRACE("Could not connect to display! error: %d", eglGetError());
    goto _fail0;
  }

  // Initialize the EGL display connection
  eglInitialize(s_display, nullptr, nullptr);

  // Select OpenGL (Core) as the desired graphics API
  if (eglBindAPI(EGL_OPENGL_API) == EGL_FALSE)
  {
    TRACE("Could not set API! error: %d", eglGetError());
    goto _fail1;
  }

  // Get an appropriate EGL framebuffer configuration
  EGLConfig config;
  EGLint numConfigs;
  static const EGLint framebufferAttributeList[] =
      {
          EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
          EGL_RED_SIZE, 8,
          EGL_GREEN_SIZE, 8,
          EGL_BLUE_SIZE, 8,
          EGL_ALPHA_SIZE, 8,
          EGL_DEPTH_SIZE, 24,
          EGL_STENCIL_SIZE, 8,
          EGL_NONE};
  eglChooseConfig(s_display, framebufferAttributeList, &config, 1, &numConfigs);
  if (numConfigs == 0)
  {
    TRACE("No config found! error: %d", eglGetError());
    goto _fail1;
  }

  // Create an EGL window surface
  s_surface = eglCreateWindowSurface(s_display, config, win, nullptr);
  if (!s_surface)
  {
    TRACE("Surface creation failed! error: %d", eglGetError());
    goto _fail1;
  }

  // Create an EGL rendering context
  static const EGLint contextAttributeList[] =
      {
          EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
          EGL_CONTEXT_MAJOR_VERSION_KHR, 4,
          EGL_CONTEXT_MINOR_VERSION_KHR, 3,
          EGL_NONE};
  s_context = eglCreateContext(s_display, config, EGL_NO_CONTEXT, contextAttributeList);
  if (!s_context)
  {
    TRACE("Context creation failed! error: %d", eglGetError());
    goto _fail2;
  }

  // Connect the context to the surface
  eglMakeCurrent(s_display, s_surface, s_surface, s_context);
  return true;

_fail2:
  eglDestroySurface(s_display, s_surface);
  s_surface = nullptr;
_fail1:
  eglTerminate(s_display);
  s_display = nullptr;
_fail0:
  return false;
}

static void deinitEgl()
{
  if (s_display)
  {
    eglMakeCurrent(s_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (s_context)
    {
      eglDestroyContext(s_display, s_context);
      s_context = nullptr;
    }
    if (s_surface)
    {
      eglDestroySurface(s_display, s_surface);
      s_surface = nullptr;
    }
    eglTerminate(s_display);
    s_display = nullptr;
  }
}

static const char *const vertexShaderSource = R"text(
  #version 330 core
  layout (location = 0) in vec3 aPos;
  layout (location = 1) in vec3 aColor;
  out vec3 ourColor;
  void main()
  {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    ourColor = aColor;
  }
)text";

static const char *const fragmentShaderSource = R"text(
  #version 330 core
  in vec3 ourColor;
  out vec4 fragColor;
  void main()
  {
    fragColor = vec4(ourColor, 1.0f);
  }
)text";

static TextRenderer* ui_text_renderer;
static void loadShaders() {
  ResourceManager::LoadShader((const char*)textures_vert, (const char*)textures_frag, nullptr, "sprite", false, textures_vert_size, textures_frag_size);
  ResourceManager::CreateSpriteRenderer(ResourceManager::GetShader("sprite"));

  glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(WIDTH), static_cast<GLfloat>(HEIGHT), 0.0f, -1.0f, 1.0f);

  // ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
  ResourceManager::GetShader("sprite").SetMatrix4("projection", projection, true);

  ResourceManager::LoadTexture((const char*)icon0_t_png, GL_TRUE, "logo", false, icon0_t_png_size);
  ResourceManager::LoadTexture((const char*)circle-outline_png, GL_TRUE, "bk_circle_icon", false, circle-outline_png_size);
  ResourceManager::LoadTexture((const char*)close-box-outline_png, GL_TRUE, "bk_cross_icon", false, close-box-outline_png_size);
  ResourceManager::LoadTexture((const char*)triangle-outline_png, GL_TRUE, "bk_triangle_icon", false, triangle-outline_png_size);
  
  ResourceManager::LoadTexture((const char*)clock_png, GL_TRUE, "bk_clock_icon", false, circle-outline_png_size);
  ResourceManager::LoadTexture((const char*)battery_outline_png, GL_TRUE, "bk_battery_icon", false, close-box-outline_png_size);
  ResourceManager::LoadTexture((const char*)memory_png, GL_TRUE, "bk_memory_icon", false, triangle-outline_png_size);

  // text
  ResourceManager::LoadShader((const char*)text_vert, (const char*)text_frag, nullptr, "text", false,
    text_vert_size, text_frag_size);

  ui_text_renderer = new TextRenderer(ResourceManager::GetShader("text"), static_cast<GLfloat>(WIDTH), static_cast<GLfloat>(HEIGHT));

  std::string noto_font((const char *)NotoSans_Regular_ttf, NotoSans_Regular_ttf_size);
  ui_text_renderer->Load(noto_font, 24, true);

  ResourceManager::setTextRenderer(ui_text_renderer);
}

int setupCallbacks(void) {
  return 0;
}

void close() {
  #ifdef DEBUG
    printf("Screeh::close\n");
  #endif

  // Deinitialize EGL
  deinitEgl();
}

void exit() {
  #ifdef DEBUG
    printf("Screeh::exit\n");
  #endif
}

void drawText(int x, int y, unsigned int color, float scale, const char *text) {
  ui_text_renderer->RenderText(std::string(text), x, y, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
}

void drawRectangle(float x, float y, float w, float h, unsigned int color) {
  ResourceManager::getSpriteRenderer()->DrawQuad(
        glm::vec2(x, y),
        glm::vec2(w, h),
        0.0f, glm::vec4(170/255.0, 170/255.0, 170/255.0, 255/255.0));
}

void drawFontTextf(Font *font, int x, int y, unsigned int color, unsigned int size, const char *text, ...) {
}

void setTextSize(float x, float y) {

}

static u64 last_kDown = 0;
static u64 last_kHeld = 0;
static u64 last_kUp = 0;

static bool stickyKeys = false;
static int breps[16];
static void updateReps(u64 kDown, u64 kHeld, u64 kUp) {
  // hidScanInput();
  // u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
  // if (kDown & KEY_PLUS)
  //   break;
  
  if (kDown & KEY_MINUS  ) breps[FZ_REPS_SELECT  ]++; else breps[FZ_REPS_SELECT  ] = 0;
  if (kDown & KEY_DLEFT   ) breps[FZ_REPS_START   ]++; else breps[FZ_REPS_START   ] = 0;
  if (kDown & KEY_DUP      ) breps[FZ_REPS_UP      ]++; else breps[FZ_REPS_UP      ] = 0;
  if (kDown & KEY_DRIGHT   ) breps[FZ_REPS_RIGHT   ]++; else breps[FZ_REPS_RIGHT   ] = 0;
  if (kDown & KEY_DDOWN    ) breps[FZ_REPS_DOWN    ]++; else breps[FZ_REPS_DOWN    ] = 0;
  if (kDown & KEY_DLEFT    ) breps[FZ_REPS_LEFT    ]++; else breps[FZ_REPS_LEFT    ] = 0;
  if (kDown & KEY_L) breps[FZ_REPS_LTRIGGER]++; else breps[FZ_REPS_LTRIGGER] = 0;
  if (kDown & KEY_R) breps[FZ_REPS_RTRIGGER]++; else breps[FZ_REPS_RTRIGGER] = 0;
  if (kDown & KEY_X) breps[FZ_REPS_TRIANGLE]++; else breps[FZ_REPS_TRIANGLE] = 0;
  if (kDown & KEY_A  ) breps[FZ_REPS_CIRCLE  ]++; else breps[FZ_REPS_CIRCLE  ] = 0;
  if (kDown & KEY_B   ) breps[FZ_REPS_CROSS   ]++; else breps[FZ_REPS_CROSS   ] = 0;
  if (kDown & KEY_Y  ) breps[FZ_REPS_SQUARE  ]++; else breps[FZ_REPS_SQUARE  ] = 0;
  if (kDown & KEY_HOME    ) breps[FZ_REPS_HOME    ]++; else breps[FZ_REPS_HOME    ] = 0;
  if (kDown & KEY_CAPTURE    ) breps[FZ_REPS_HOLD    ]++; else breps[FZ_REPS_HOLD    ] = 0;
  if (kDown & KEY_ZL    ) breps[FZ_REPS_NOTE    ]++; else breps[FZ_REPS_NOTE    ] = 0;
}

void resetReps() {

}

int* ctrlReps() {
  return breps;
}

void setupCtrl() {
  resetReps();
}

int readCtrl() {
  padUpdate(&pad);
  u64 kDown = padGetButtonsDown(&pad);
  //hidKeysHeld returns information about which buttons have are held down in this frame
  u64 kHeld = padGetButtons(&pad);
  //hidKeysUp returns information about which buttons have been just released
  u64 kUp = padGetButtonsUp(&pad);


  updateReps(kDown, kHeld, kUp);
  // || kHeld != last_kHeld || kUp != last_kUp
  bool rerender = (kDown != last_kDown);

  last_kDown = kDown;
  // last_kHeld = kHeld;
  // last_kUp = kUp;

  return rerender;
}

void getAnalogPad(int& x, int& y) {
  //Read the joysticks' position
  HidAnalogStickState analog_stick_l = padGetStickPos(&pad, 0);
  // HidAnalogStickState analog_stick_r = padGetStickPos(&pad, 1);

  x = analog_stick_l.x - FZ_ANALOG_CENTER;
  y = analog_stick_l.y - FZ_ANALOG_CENTER;
}

void startDirectList() {

}

void swapBuffers() {
  eglSwapBuffers(s_display, s_surface);
}

void endAndDisplayList() {

}

void waitVblankStart() {

}

void* getListMemory(int s) {
  return 0;
}

void shadeModel(int mode) {

}

void color(unsigned int c) {

}

void ambientColor(unsigned int c) {

}

void clear(unsigned int color, int b) {

}

void checkEvents(int buttons) {
}

void matricesFor2D(int rotation) {
}

struct T32FV32F2D {
    float u,v;
    float x,y,z;
};

void setBoundTexture(Texture *t) {

}

void drawFontText(Font *font, int x, int y, unsigned int color, unsigned int size, const char *text) {

}

void drawTextureScale(const Texture *texture, float x, float y, float x_scale, float y_scale) {

}

void drawTextureTintScale(const Texture *texture, float x, float y, float x_scale, float y_scale, unsigned int color) {

}

void drawTextureTintScaleRotate(const Texture *texture, float x, float y, float x_scale, float y_scale, float rad, unsigned int color) {

}

/*  Active Shader
    bind correct vertex array
  */
void drawArray(int prim, int vtype, int count, void* indices, void* vertices) {
}

void copyImage(int psm, int sx, int sy, int width, int HEIGHT, int srcw, void *src,
    int dx, int dy, int destw, void *dest) {

}

void drawPixel(float x, float y, unsigned int color) {

}

void* framebuffer() {
  return 0;
}

void blendFunc(int op, int src, int dst) {

}

void enable(int m) {

}

void disable(int m) {

}

void dcacheWritebackAll() {

}

string basePath() {
  // TODO: Change this for switch.
  return "/Books";
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

int dirContents(const char* path, std::vector<Dirent>& a) {
  for (const auto & entry : std::filesystem::directory_iterator(path)) {
    std::cout << entry.path() << std::endl;

    std::string filename(entry.path().filename());

    // if (filename == std::string('.') || filename == std::string('..'))
    //   continue;

    a.push_back(Dirent(filename, FZ_STAT_IFREG, 0));
  }

  return 0;
}

int getSuspendSerial() {
  return 0;
}

void setSpeed(int v) {

}

int getSpeed() {
  return 0;
}

void getTime(int &h, int &m) {
  auto t = std::time(0);   // get time now
  auto now = std::localtime(&t);
  h = now->tm_hour;
  m = now->tm_min;
}

int getBattery() {
  unsigned int charge;
  psmGetBatteryChargePercentage(&charge);
  return charge;
}

int getUsedMemory() {
  struct mallinfo mi = mallinfo();
  return mi.uordblks;
}

void setBrightness(int b){

}

bool isClosing() {
  return !appletMainLoop();
}

} }
