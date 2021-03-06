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

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <map>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <iostream>
#include <ctime>
#include <malloc/malloc.h>
#include <cinttypes>

#include <dirent.h>

#include "screen.hpp"
#include "controls.hpp"
#include "texture.hpp"
#include "../resource_manager.hpp"

using std::cout;
using std::endl;

namespace bookr { namespace Screen {

unsigned int WIDTH = 1280;
unsigned int HEIGHT = 720;

static int powerSerial = 0;

static GLFWwindow* window;
static string psv_full_path;

static void keyboard(GLFWwindow* window, int key, int scancode, int action, int mode);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void loadShaders();

void open(int argc, char** argv) {
  psv_full_path = std::string(std::filesystem::current_path());
  std::cout << "psv_full_path: " << psv_full_path << std::endl;

  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  #endif

  window = glfwCreateWindow(WIDTH, HEIGHT, "Bookr", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
  }

  glfwSetKeyCallback(window, key_callback);

  // OpenGL configuration
  glViewport(0, 0, WIDTH, HEIGHT);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  std::cout << glGetString(GL_VERSION) << std::endl;

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  loadShaders();
}

static volatile int last_key = 0;
static bool stickyKeys = false;
static int breps[16];
static volatile int lastAnalogX = 0;
static volatile int lastAnalogY = 0;
static void updateReps(int keyState, int action) {
  cout << "updatereps keystate " << last_key << endl;

  if (action == GLFW_PRESS) {
    if (keyState == GLFW_KEY_LEFT_SHIFT  ) breps[FZ_REPS_SELECT  ]++; else breps[FZ_REPS_SELECT  ] = 0;
    if (keyState == GLFW_KEY_ENTER  ) breps[FZ_REPS_START   ]++; else breps[FZ_REPS_START   ] = 0;
    if (keyState == GLFW_KEY_W  ) breps[FZ_REPS_UP      ]++; else breps[FZ_REPS_UP      ] = 0;
    if (keyState == GLFW_KEY_D  ) breps[FZ_REPS_RIGHT   ]++; else breps[FZ_REPS_RIGHT   ] = 0;
    if (keyState == GLFW_KEY_S  ) breps[FZ_REPS_DOWN    ]++; else breps[FZ_REPS_DOWN    ] = 0;
    if (keyState == GLFW_KEY_A  ) breps[FZ_REPS_LEFT    ]++; else breps[FZ_REPS_LEFT    ] = 0;
    if (keyState == GLFW_KEY_Q  ) breps[FZ_REPS_LTRIGGER]++; else breps[FZ_REPS_LTRIGGER] = 0;
    if (keyState == GLFW_KEY_E  ) breps[FZ_REPS_RTRIGGER]++; else breps[FZ_REPS_RTRIGGER] = 0;
    if (keyState == GLFW_KEY_I  ) breps[FZ_REPS_TRIANGLE]++; else breps[FZ_REPS_TRIANGLE] = 0;
    if (keyState == GLFW_KEY_L  ) breps[FZ_REPS_CIRCLE  ]++; else breps[FZ_REPS_CIRCLE  ] = 0;
    if (keyState == GLFW_KEY_M  ) breps[FZ_REPS_CROSS   ]++; else breps[FZ_REPS_CROSS   ] = 0;
    if (keyState == GLFW_KEY_J  ) breps[FZ_REPS_SQUARE  ]++; else breps[FZ_REPS_SQUARE  ] = 0;
    if (keyState == GLFW_KEY_T  ) breps[FZ_REPS_HOME    ]++; else breps[FZ_REPS_HOME    ] = 0;
    if (keyState == GLFW_KEY_V  ) breps[FZ_REPS_HOLD    ]++; else breps[FZ_REPS_HOLD    ] = 0;
    if (keyState == GLFW_KEY_B  ) breps[FZ_REPS_NOTE    ]++; else breps[FZ_REPS_NOTE    ] = 0;
    if (keyState == GLFW_KEY_UP )   lastAnalogY += 20000;
    if (keyState == GLFW_KEY_DOWN ) lastAnalogY -= 20000;
    if (keyState == GLFW_KEY_LEFT )  lastAnalogX += 20000;
    if (keyState == GLFW_KEY_RIGHT ) lastAnalogX -= 20000;
  } else if (action == GLFW_RELEASE) {
    if (keyState == GLFW_KEY_LEFT_SHIFT  ) breps[FZ_REPS_SELECT  ]--; else breps[FZ_REPS_SELECT  ] = 0;
    if (keyState == GLFW_KEY_ENTER  ) breps[FZ_REPS_START   ]--; else breps[FZ_REPS_START   ] = 0;
    if (keyState == GLFW_KEY_W  ) breps[FZ_REPS_UP      ]--; else breps[FZ_REPS_UP      ] = 0;
    if (keyState == GLFW_KEY_D  ) breps[FZ_REPS_RIGHT   ]--; else breps[FZ_REPS_RIGHT   ] = 0;
    if (keyState == GLFW_KEY_S  ) breps[FZ_REPS_DOWN    ]--; else breps[FZ_REPS_DOWN    ] = 0;
    if (keyState == GLFW_KEY_A  ) breps[FZ_REPS_LEFT    ]--; else breps[FZ_REPS_LEFT    ] = 0;
    if (keyState == GLFW_KEY_Q  ) breps[FZ_REPS_LTRIGGER]--; else breps[FZ_REPS_LTRIGGER] = 0;
    if (keyState == GLFW_KEY_E  ) breps[FZ_REPS_RTRIGGER]--; else breps[FZ_REPS_RTRIGGER] = 0;
    if (keyState == GLFW_KEY_I  ) breps[FZ_REPS_TRIANGLE]--; else breps[FZ_REPS_TRIANGLE] = 0;
    if (keyState == GLFW_KEY_L  ) breps[FZ_REPS_CIRCLE  ]--; else breps[FZ_REPS_CIRCLE  ] = 0;
    if (keyState == GLFW_KEY_M  ) breps[FZ_REPS_CROSS   ]--; else breps[FZ_REPS_CROSS   ] = 0;
    if (keyState == GLFW_KEY_J  ) breps[FZ_REPS_SQUARE  ]--; else breps[FZ_REPS_SQUARE  ] = 0;
    if (keyState == GLFW_KEY_T  ) breps[FZ_REPS_HOME    ]--; else breps[FZ_REPS_HOME    ] = 0;
    if (keyState == GLFW_KEY_V  ) breps[FZ_REPS_HOLD    ]--; else breps[FZ_REPS_HOLD    ] = 0;
    if (keyState == GLFW_KEY_B  ) breps[FZ_REPS_NOTE    ]--; else breps[FZ_REPS_NOTE    ] = 0;
    if (keyState == GLFW_KEY_UP )   lastAnalogY = 0;
    if (keyState == GLFW_KEY_DOWN ) lastAnalogY = 0;
  }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  last_key = key;
  updateReps(key, action);

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GLFW_TRUE);
}


int readCtrl() {
  glfwWaitEvents();
  // glfwPollEvents();
  return last_key;
}

void resetReps() {
  stickyKeys = true;
}

int* ctrlReps() {
  return breps;
}

void setupCtrl() {
  resetReps();
}

static void loadShaders() {
  ResourceManager::LoadShader("shaders/textures.vert", "shaders/textures.frag", nullptr, "sprite", true);
  ResourceManager::CreateSpriteRenderer(ResourceManager::GetShader("sprite"));

  glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(WIDTH), static_cast<GLfloat>(HEIGHT), 0.0f, -1.0f, 1.0f);
  // glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

  // ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
  ResourceManager::GetShader("sprite").SetMatrix4("projection", projection, true);

  ResourceManager::LoadTexture("data/logos/icon0_t.png", GL_TRUE, "logo");
  ResourceManager::LoadTexture("data/icons/circle-outline.png", GL_TRUE, "bk_circle_icon");
  ResourceManager::LoadTexture("data/icons/close-box-outline.png", GL_TRUE, "bk_cross_icon");
  ResourceManager::LoadTexture("data/icons/triangle-outline.png", GL_TRUE, "bk_triangle_icon");

  ResourceManager::LoadTexture("data/icons/clock.png", GL_TRUE, "bk_clock_icon");
  ResourceManager::LoadTexture("data/icons/battery-outline.png", GL_TRUE, "bk_battery_icon");
  ResourceManager::LoadTexture("data/icons/memory.png", GL_TRUE, "bk_memory_icon");

  ResourceManager::LoadTexture("data/icons/collections_bookmark_white-fs8.png", GL_TRUE, "bk_bookmark_icon");
  ResourceManager::LoadTexture("data/icons/content_copy_white-fs8.png", GL_TRUE, "bk_copy_icon");
  ResourceManager::LoadTexture("data/icons/search_white-fs8.png", GL_TRUE, "bk_search_icon");
  ResourceManager::LoadTexture("data/icons/rotate_left_white-fs8.png", GL_TRUE, "bk_rotate_left_icon");
  ResourceManager::LoadTexture("data/icons/rotate_right_white-fs8.png", GL_TRUE, "bk_rotate_right_icon");

  ResourceManager::LoadTexture("data/icons/bookmark_add_white-fs8.png", GL_TRUE, "bk_add_bookmark_icon");
  ResourceManager::LoadTexture("data/icons/first_page-fs8.png", GL_TRUE, "bk_first_page_icon");
  ResourceManager::LoadTexture("data/icons/last_page-fs8.png", GL_TRUE, "bk_last_page_icon");
  ResourceManager::LoadTexture("data/icons/previous_ten-fs8.png", GL_TRUE, "bk_prev_ten_icon");
  ResourceManager::LoadTexture("data/icons/next_ten-fs8.png", GL_TRUE, "bk_next_ten_icon");
  ResourceManager::LoadTexture("data/icons/go_to_page-fs8.png", GL_TRUE, "bk_go_to_page_icon");

  ResourceManager::LoadTexture("data/icons/fit_height-fs8.png", GL_TRUE, "bk_fit_height_icon");
  ResourceManager::LoadTexture("data/icons/fit_width-fs8.png", GL_TRUE, "bk_fit_width_icon");
  ResourceManager::LoadTexture("data/icons/zoom_out_white-fs8.png", GL_TRUE, "bk_zoom_out_icon");
  ResourceManager::LoadTexture("data/icons/zoom_in_white-fs8.png", GL_TRUE, "bk_zoom_in_icon");

  ResourceManager::CreateTextRenderer(WIDTH, HEIGHT);
}


void startDirectList() {
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}


void close() {
}

void exit() {
    glfwTerminate();
}

bool isClosing() {
  return (bool)glfwWindowShouldClose(window);
}


void getAnalogPad(int& x, int& y) {
  x = lastAnalogX;
  y = lastAnalogY;
}

void swapBuffers() {
    glfwSwapBuffers(window);
}

void checkEvents(int buttons) {
    glfwPollEvents();
    
}

void matricesFor2D(int rotation) {
}

static Texture* boundTexture = 0;
void setBoundTexture(Texture *t) {
    boundTexture = t;
}

/*  Active Shader
    bind correct vertex array
*/
void drawArray() {
}

void drawArray(int prim, int count, void* indices, void* vertices) {
    //glUseProgram
    //glBindVertextArray(verticies)
    //glDrawArrays(prim, 0, count);
    //glBindVertexArray(0);
}

void drawArray(int prim, int vtype, int count, void* indices, void* vertices) {
    // blah
}

void copyImage(int psm, int sx, int sy, int width, int height, int srcw, void *src,
  int dx, int dy, int destw, void *dest) {
    //sceGuCopyImage(psm, sx, sy, width, height, srcw, src, dx, dy, destw, dest);
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
    //sceKernelDcacheWritebackAll();
}

std::string basePath() {
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

int dirContents(const char* path, std::vector<Dirent>& a) {
  for (const auto & entry : std::filesystem::directory_iterator(path)) {
    std::cout << entry.path() << std::endl;
    std::string filename(entry.path().filename());

    int type;

    if (entry.is_regular_file())
      type = FZ_STAT_IFREG;
    else if (entry.is_directory())
      type = FZ_STAT_IFDIR;

    a.push_back(Dirent(filename, type, 0));
  }

  return 0;
}

int getSuspendSerial() {
    //return powerResumed;
  return 0;
}

void setSpeed(int v) {
    if (v <= 0 || v > 6)
        return;
    
    //scePowerSetClockFrequency(speedValues[v*2], speedValues[v*2], speedValues[v*2+1]);
    
    //scePowerSetArmClockFrequency(speedValues[v*2]);
    //scePowerSetCpuClockFrequency(speedValues[v*2]);
    
    //scePowerSetBusClockFrequency(speedValues[v*2+1]);
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
  return 100;
}

int getUsedMemory() {
  auto data = mstats();
  return data.bytes_used;
}

void* getListMemory(int s) {
    //return sceGuGetMemory(s);
  return 0;
}

void setBrightness(int b){
}

void waitVblankStart() {
}

glm::vec4 colorToRGBA(unsigned int c) {
  float red = (float)((c & 0xff000000) >> 24);
  float green = (float)((c & 0x00ff0000) >> 16);
  float blue = (float)((c & 0x0000ff00) >> 8);
  float alpha = (float)(c & 0x000000ff);
  return glm::vec4(red, green, blue, alpha);
} 

void ambientColor(unsigned int c) {
    // sceGuAmbientColor(c);
}

// for glClear
void clear(unsigned int color, int b) {
  glClear(GL_COLOR_BUFFER_BIT);
}

// TODO: Remove all but this.
void clear(float r, float g, float b, float a, int buffer = GL_COLOR_BUFFER_BIT) {
  glClearColor(r, g, b, a);
  glClear(buffer);
}

// glClearColor
void color(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
}

void color(unsigned int c) {
  float red = (float)((c & 0xff000000) >> 24);
  float green = (float)((c & 0x00ff0000) >> 16);
  float blue = (float)((c & 0x0000ff00) >> 8);
  float alpha = (float)(c & 0x000000ff);
  glClearColor(red, green, blue, alpha);
}

void endAndDisplayList() {
    // sceGuFinish();
    // sceKernelDcacheWritebackAll();  
    // sceGuSync(0,0);
}

void drawTexture(const Texture *texture, float x, float y) {}

} }
