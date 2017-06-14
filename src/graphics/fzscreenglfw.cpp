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
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cstring>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include <iostream>

//#include fzscreencommon.
#include "fzscreen.h"
#include "fztexture.h"
#include "shaders/shader.h"

using namespace std;

FZScreen::FZScreen() {
    cout << "FZScreen()" << endl;
}

FZScreen::~FZScreen() {
    cout << "~FZScreen()" << endl;
}

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
    // swap this to some mapping?
    if (action == GLFW_PRESS) {
        cout << keyState << endl;
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_W:
                keyState |= FZ_CTRL_UP;
                glClearColor(0.2f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                break;
            case GLFW_KEY_S:
                keyState |= FZ_CTRL_DOWN; 
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                break;
            case GLFW_KEY_A:
                keyState |= FZ_CTRL_LEFT;
                break;
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

                  unsigned char* image = SOIL_load_image("image.png", &width, &height, 0, SOIL_LOAD_RGB);
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
                
                // Configure VAO/VBO
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
                
                
                glBindVertexArray(VAO);
                  glBindBuffer(GL_ARRAY_BUFFER, VBO);
                  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                  glEnableVertexAttribArray(0);
                  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
                glBindVertexArray(0);
                
                Shader ourShader("src/graphics/shaders/textures.vert.orig", 
                                 "src/graphics/shaders/textures.frag.orig");
                glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(940), 
                    static_cast<GLfloat>(544), 0.0f, -1.0f, 1.0f);

                glUniform1i(glGetUniformLocation(ourShader.Program, "image"), 0);
                glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

                // Load textures
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                  unsigned char* image = SOIL_load_image("sce_sys/icon0_t.png", &width, &height, 0, SOIL_LOAD_RGB);
                  glClearColor(0.0, 0.0, 0.0, 0.0);
                  glClear(GL_COLOR_BUFFER_BIT);
                  
                  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
                  glGenerateMipmap(GL_TEXTURE_2D);

                  SOIL_free_image_data(image);
                glBindTexture(GL_TEXTURE_2D, 0);

                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                //glBindTexture(GL_TEXTURE_2D, texture);
                // Texture2D &texture,                                       glm::vec2 position, glm::vec2 size, GLfloat rotate, glm::vec3 color
                // Renderer->DrawSprite(ResourceManager::GetTexture("face"), glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
                glBindTexture(GL_TEXTURE_2D, texture);
                ourShader.Use();
                // glm::mat4 model;
                
                // model = glm::translate(model, glm::vec3(glm::vec2(0, 0), 0.0f));  

                // glm::vec2 size = glm::vec2(300, 400);
                // model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); 
                // model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 0.0f, 1.0f)); 
                // model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

                // model = glm::scale(model, glm::vec3(size, 1.0f)); 
              
                // this->shader.SetMatrix4("model", model);
                // glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
                // this->shader.SetVector3f("spriteColor", color);
                // glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);
                // glUniform3f(glGetUniformLocation(ourShader.Program, "spriteColor"), color.x, color.y, color.z);
              
                //glActiveTexture(GL_TEXTURE0);
                

                glBindVertexArray(VAO);
                  glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindVertexArray(0);
                
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
        FZScreen::swapBuffers();
    } else if (action == GLFW_RELEASE ) {
        cout << keyState << endl;
        switch (key) {
            case 27:
                exit(0);
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
    VBOs["texture"] = VBO;
    VAOs["texture"] = VAO;

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
void FZScreen::open(int argc, char** argv) {
    //getcwd(psp_full_path, 1024);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  #ifdef MAC
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  #endif

    window = glfwCreateWindow(FZ_SCREEN_WIDTH, FZ_SCREEN_HEIGHT, "Bookr GLFW", nullptr, nullptr);
    if (window == nullptr) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        cout << "Failed to initialize GLEW" << endl;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glfwSetKeyCallback(window, keyboard);

    glfwSwapInterval(0);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    loadShaders();
}

void FZScreen::close() {
}

void FZScreen::exit() {
    glfwTerminate();
}

bool FZScreen::isClosing() {
    return glfwWindowShouldClose(window);
}

int FZScreen::readCtrl() {
    glfwWaitEvents();
    updateReps();
    return keyState;
}

void FZScreen::getAnalogPad(int& x, int& y) {
	x = 128;
	y = 128;
}

void FZScreen::swapBuffers() {
    glfwSwapBuffers(window);
}

void FZScreen::checkEvents(int buttons) {
    //glfwPollEvents();
    
}

void FZScreen::matricesFor2D(int rotation) {
}

struct T32FV32F2D {
    float u,v;
    float x,y,z;
};

static FZTexture* boundTexture = 0;
void FZScreen::setBoundTexture(FZTexture *t) {
    boundTexture = t;
}

/*  Active Shader
    bind correct vertex array
*/
void FZScreen::drawArray() {
    cout << "fzscreen drawArray" << endl;
    texture_shader->Use();
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(glm::vec2(200, 200), 0.0f));
    glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);
  
    glUniformMatrix4fv(glGetUniformLocation(texture_shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    //glUniform3f(glGetUniformLocation(texture_shader->Program, "spriteColor"), 1, GL_FALSE, glm::value_ptr(color)); 
    glUniform3f(glGetUniformLocation(texture_shader->Program, "spriteColor"), 0.0f, 1.0f, 0.0f);

    cout << "vao texture id" + VAOs["texture"] << endl;

    glBindVertexArray(VAOs["texture"]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void FZScreen::drawArray(int prim, int count, void* indices, void* vertices) {
    //glUseProgram
    //glBindVertextArray(verticies)
    //glDrawArrays(prim, 0, count);
    //glBindVertexArray(0);
}

void FZScreen::drawArray(int prim, int vtype, int count, void* indices, void* vertices) {
    // blah
}

void FZScreen::copyImage(int psm, int sx, int sy, int width, int height, int srcw, void *src,
  int dx, int dy, int destw, void *dest) {
    //sceGuCopyImage(psm, sx, sy, width, height, srcw, src, dx, dy, destw, dest);
}

void FZScreen::resetReps() {
    stickyKeys = true;
}

int* FZScreen::ctrlReps() {
    return breps;
}

void FZScreen::setupCtrl() {
    // sceCtrlSetSamplingCycle(0);
    // sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    // resetReps();
}


void FZScreen::blendFunc(int op, int src, int dst) {
    //sceGuBlendFunc(op, src, dst, 0, 0);
}

void FZScreen::enable(int m) {
    //sceGuEnable(m);
}

void FZScreen::disable(int m) {
    //sceGuDisable(m);
}

void FZScreen::dcacheWritebackAll() {
    //sceKernelDcacheWritebackAll();
}

string FZScreen::basePath() {
    return psp_full_path;
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
}

int FZScreen::getSuspendSerial() {
    //return powerResumed;
}

void FZScreen::setSpeed(int v) {
    if (v <= 0 || v > 6)
        return;
    
    //scePowerSetClockFrequency(speedValues[v*2], speedValues[v*2], speedValues[v*2+1]);
    
    //scePowerSetArmClockFrequency(speedValues[v*2]);
    //scePowerSetCpuClockFrequency(speedValues[v*2]);
    
    //scePowerSetBusClockFrequency(speedValues[v*2+1]);
}

int FZScreen::getSpeed() {
}

void FZScreen::getTime(int &h, int &m) {
}

int FZScreen::getBattery() {
}

int FZScreen::getUsedMemory() {
    //struct mallinfo mi = mallinfo();
    //return mi.uordblks;
    //return mi.arena;
}

void* FZScreen::getListMemory(int s) {
    //return sceGuGetMemory(s);
}


void FZScreen::setBrightness(int b){
}

void FZScreen::waitVblankStart() {
}

void FZScreen::color(unsigned int c) {
    // sceGuColor(c);
}

void FZScreen::ambientColor(unsigned int c) {
    // sceGuAmbientColor(c);
}

void FZScreen::clear(unsigned int color, int b) {
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

void FZScreen::startDirectList() {
    // sceGuStart(GU_DIRECT, list);
}

void FZScreen::endAndDisplayList() {
    // sceGuFinish();
    // sceKernelDcacheWritebackAll();  
    // sceGuSync(0,0);
}