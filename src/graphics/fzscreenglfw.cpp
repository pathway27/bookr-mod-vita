/*
 * Bookr % VITA: document reader for the Sony PS Vita
 * Copyright (C) 2017 Sreekara C. (pathway27 at gmail dot com)
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

#ifdef MAC
  #define GLEW_STATIC
  #include <GL/glew.h>
  #include <SOIL.h>
#elif define (__WIN32__)
  #include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>

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
                Shader ourShader("src/graphics/shaders/textures.vert", 
                    "src/graphics/shaders/textures.frag");
                keyState |= FZ_CTRL_RIGHT;
                #ifdef MAC
                    // Set up vertex data (and buffer(s)) and attribute pointers
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
                #endif


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

                glBindTexture(GL_TEXTURE_2D, texture);
                ourShader.Use();

                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);

                break;
            }
            case GLFW_KEY_K: keyState |= FZ_CTRL_SQUARE; break;
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

    window = glfwCreateWindow(940, 544, "Bookr GLFW", nullptr, nullptr);
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

void FZScreen::swapBuffers() {
    glfwSwapBuffers(window);
}

void FZScreen::checkEvents(int buttons) {
    //glfwPollEvents();
    
}

static FZTexture* boundTexture = 0;
void FZScreen::setBoundTexture(FZTexture *t) {
    boundTexture = t;
}

void FZScreen::waitVblankStart() {
}

string FZScreen::basePath() {
    return psp_full_path;
}

void FZScreen::drawArray(int prim, int vtype, int count, void* indices, void* vertices) {
    //glUseProgram
    //glBindVertextArray(verticies)
    //glDrawArrays(prim, 0, count);
    //glBindVertexArray(0);
}