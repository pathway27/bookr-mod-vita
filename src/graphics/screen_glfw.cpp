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

#include <glad/glad.h>
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
#include "SOIL.h"

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
                Shader recShader("src/graphics/shaders/rectangle.vert", 
                                 "src/graphics/shaders/rectangle.frag");
                
                GLuint vao;
                glGenVertexArrays(1, &vao);
                glBindVertexArray(vao);

                // Create a Vertex Buffer Object and copy the vertex data to it
                GLuint vbo;
                glGenBuffers(1, &vbo);

                GLfloat vertices[] = {
                    -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
                     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
                     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
                    -0.5f, -0.5f, 1.0f, 1.0f, 1.0f  // Bottom-left
                };
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                // Create an element array
                GLuint ebo;
                glGenBuffers(1, &ebo);

                GLuint elements[] = {
                    0, 1, 2,
                    2, 3, 0
                };

                recShader.Use();
                // Specify the layout of the vertex data
                GLint posAttrib = glGetAttribLocation(recShader.Program, "position");
                glEnableVertexAttribArray(posAttrib);
                glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

                GLint colAttrib = glGetAttribLocation(recShader.Program, "color");
                glEnableVertexAttribArray(colAttrib);
                glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
                

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                break;
            }
            case GLFW_KEY_D: {
                keyState |= FZ_CTRL_RIGHT;
                Shader ourShader("src/graphics/shaders/textures.vert", 
                                 "src/graphics/shaders/textures.frag");
                
                GLfloat vertices[] = {
                    // Positions          // Colors           // Texture Coords
                    1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Top Right
                    1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Bottom Right
                    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Bottom Left
                    -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Top Left 
                };
                GLuint indices[] = {  // Note that we start from 0!
                    0, 1, 3, // First Triangle
                    1, 2, 3  // Second Triangle
                };
                GLuint VBO, VAO, EBO;
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                glGenBuffers(1, &EBO);

                glBindVertexArray(VAO);
                  glBindBuffer(GL_ARRAY_BUFFER, VBO);
                  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

                  // Position attribute
                  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
                  glEnableVertexAttribArray(0);
                  // Color attribute
                  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
                  glEnableVertexAttribArray(1);
                  // TexCoord attribute
                  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
                  glEnableVertexAttribArray(2);
                glBindVertexArray(0); // Unbind VAO


                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                  unsigned char* image = SOIL_load_image("sce_sys/icon0.png", &width, &height, 0, SOIL_LOAD_RGB);
                  glClearColor(0.0, 0.0, 0.0, 0.0);
                  glClear(GL_COLOR_BUFFER_BIT);

                  cout << SOIL_last_result() << endl; 
                  cout << "null: " << !image << endl;
                  cout << "Max size: " << GL_MAX_TEXTURE_SIZE << endl;
                  cout << "Width: " <<  width << endl;
                  cout << "Height: " << height << endl;
                  cout << "Obj: " << texture << endl;

                  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, 
                  GL_RGB, GL_UNSIGNED_BYTE, image);
                  glGenerateMipmap(GL_TEXTURE_2D);

                  SOIL_free_image_data(image);
                glBindTexture(GL_TEXTURE_2D, 0);

                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                ourShader.Use();
                glBindTexture(GL_TEXTURE_2D, texture);
                
                glBindVertexArray(VAO);
                  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);

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

static Shader* texture_shader;
static void loadShaders() {
    // For textures
    // load shaders
    texture_shader = new Shader("src/graphics/shaders/textures.vert", 
                                "src/graphics/shaders/textures.frag");

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
void open(int argc, char** argv) {
  #ifdef DEBUG
    printf("open\n");
  #endif
  //getcwd(psp_full_path, 1024);
  glfwInit();
  if (!glfwInit()) {
      cout << "Failed to init GLFW" << endl;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  #ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  #endif

  // glfw window creation
  // --------------------
  GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
  }
  glfwMakeContextCurrent(window);
  // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
  }    

  std::cout << glGetString(GL_VERSION) << std::endl;

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  glfwSetKeyCallback(window, key_callback);

  // glfwSwapInterval(0);

  // glClearColor(0.5f, 0.3f, 0.3f, 1.0f);
  // glClear(GL_COLOR_BUFFER_BIT);
  // swapBuffers();

  // glfwWaitEvents();
}

void close() {
}

void exit() {
    glfwTerminate();
}

bool isClosing() {
    cout << "glfwWindowShouldClose(window)" << endl;
    // return !!glfwWindowShouldClose(window);
    // return true;
}

int readCtrl() {
    glfwWaitEvents();
    // updateReps();
    return 0;
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
