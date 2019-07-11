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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "controls.hpp"
#include "resolutions.hpp"
#include "shaders/shader.hpp"
#include "screen.hpp"

using std::cout;
using std::endl;

namespace bookr { namespace Screen {

static int keyState = 0;
static bool stickyKeys = false;
static int powerSerial = 0;
static int breps[16];
static void updateReps() {
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

//unsigned char key, int x, int y
int width, height;
GLuint texture;

static void keyboard(GLFWwindow* window, int key, int scancode, int action, int mode) {
    cout << key << endl;
    // swap this to some mapping?
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
            case GLFW_KEY_W:
                keyState |= FZ_CTRL_UP;
                glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                break;
            case GLFW_KEY_S:
                keyState |= FZ_CTRL_DOWN; 
                glClearColor(0.0f, 1.0f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                break;
            case GLFW_KEY_A: {
                keyState |= FZ_CTRL_LEFT;
                
                break;
            }
            case GLFW_KEY_D: {
                keyState |= FZ_CTRL_RIGHT;
                break;
            }
            case GLFW_KEY_K: {
                keyState |= FZ_CTRL_SQUARE; 
                break;
            }
            case GLFW_KEY_L: keyState |= FZ_CTRL_CROSS; break;
            case GLFW_KEY_O: keyState |= FZ_CTRL_TRIANGLE; break;
            case GLFW_KEY_P: keyState |= FZ_CTRL_CIRCLE; break;
            case GLFW_KEY_V: keyState |= FZ_CTRL_SELECT; break;
            case GLFW_KEY_B: keyState |= FZ_CTRL_START; break;
            case GLFW_KEY_X: keyState |= FZ_CTRL_LTRIGGER; break;
            case GLFW_KEY_C: keyState |= FZ_CTRL_RTRIGGER; break;
            case GLFW_KEY_H: keyState |= FZ_CTRL_HOLD;break;
        }
        // swapBuffers();
    } else if (action == GLFW_RELEASE ) {
        switch (key) {
            case 27:
                std::exit(0);
            break;
            case GLFW_KEY_W: keyState &= ~FZ_CTRL_UP; break;
            case GLFW_KEY_S: keyState &= ~FZ_CTRL_DOWN; break;
            case GLFW_KEY_A: keyState &= ~FZ_CTRL_LEFT; break;
            case GLFW_KEY_D: keyState &= ~FZ_CTRL_RIGHT; break;
            case GLFW_KEY_K: keyState &= ~FZ_CTRL_SQUARE; break;
            case GLFW_KEY_L: keyState &= ~FZ_CTRL_CROSS; break;
            case GLFW_KEY_O: keyState &= ~FZ_CTRL_TRIANGLE; break;
            case GLFW_KEY_P: keyState &= ~FZ_CTRL_CIRCLE; break;
            case GLFW_KEY_V: keyState &= ~FZ_CTRL_SELECT; break;
            case GLFW_KEY_B: keyState &= ~FZ_CTRL_START; break;
            case GLFW_KEY_X: keyState &= ~FZ_CTRL_LTRIGGER; break;
            case GLFW_KEY_C: keyState &= ~FZ_CTRL_RTRIGGER; break;
            case GLFW_KEY_H: keyState &= ~FZ_CTRL_HOLD; break;
            //case '6': powerSerial++; break;
      }
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    cout << key << endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
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

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


static Shader* texture_shader;
static void loadShaders() {
    // For textures
    // load shaders
    texture_shader = new Shader("shaders/textures.vert", 
                                "shaders/textures.frag");

    //shaders["texture"] = (*)texture_shader;
    // bind vertex buffers' and get id
    // Set up vertex data (and buffer(s)) and attribute pointers
    GLuint VAO, VBO;
    GLfloat vertices[] = { 
        // Pos      // Tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 
    
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //VBOs["texture"] = VBO;
    //VAOs["texture"] = VAO;

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static GLFWwindow* window;
static char psp_full_path[1024 + 1];
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
void open(int argc, char** argv) {
  // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bookr", NULL, NULL);
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

    glfwSetKeyCallback(window, keyboard);


    std::cout << glGetString(GL_VERSION) << std::endl;

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void close() {
}

void exit() {
    glfwTerminate();
}

bool isClosing() {
  return (bool)glfwWindowShouldClose(window);
}

int readCtrl() {
    glfwWaitEvents();
    updateReps();
    return keyState;
}

void getAnalogPad(int& x, int& y) {
  x = 128;
  y = 128;
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
    cout << "fzscreen drawArray" << endl;
    texture_shader->Use();
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(glm::vec2(200, 200), 0.0f));
    glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);
  
    glUniformMatrix4fv(glGetUniformLocation(texture_shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    //glUniform3f(glGetUniformLocation(texture_shader->Program, "spriteColor"), 1, GL_FALSE, glm::value_ptr(color)); 
    glUniform3f(glGetUniformLocation(texture_shader->Program, "spriteColor"), 0.0f, 1.0f, 0.0f);

    //cout << "vao texture id" + VAOs["texture"] << endl;

    //glBindVertexArray(VAOs["texture"]);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    //glBindVertexArray(0);
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

void resetReps() {
    stickyKeys = true;
}

int* ctrlReps() {
    return breps;
}

void setupCtrl() {
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

string basePath() {
    return psp_full_path;
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
}

int getBattery() {
  return 0;
}

int getUsedMemory() {
    //struct mallinfo mi = mallinfo();
    //return mi.uordblks;
    //return mi.arena;
  return 0;
}

void* getListMemory(int s) {
    //return sceGuGetMemory(s);
  return 0;
}


void setBrightness(int b){
}

void waitVblankStart() {
}

void color(unsigned int c) {
    float red = (float)((c & 0xff000000) >> 24);
    float green = (float)((c & 0x00ff0000) >> 16);
    float blue = (float)((c & 0x0000ff00) >> 8);
    float alpha = (float)(c & 0x000000ff);
    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT);
}

void ambientColor(unsigned int c) {
    // sceGuAmbientColor(c);
}

void clear(unsigned int color, int b) {
    glClearColor(0.0f, 0.0f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // sceGuClearColor(color);
    // int m = 0;
    // if (b & FZ_COLOR_BUFFER)
    //     m |= GU_COLOR_BUFFER_BIT;
    // if (b & FZ_DEPTH_BUFFER)
    //     m |= GU_DEPTH_BUFFER_BIT;
    // sceGuClear(m);
}

void startDirectList() {
    // sceGuStart(GU_DIRECT, list);
}

void endAndDisplayList() {
    // sceGuFinish();
    // sceKernelDcacheWritebackAll();  
    // sceGuSync(0,0);
}

} }
