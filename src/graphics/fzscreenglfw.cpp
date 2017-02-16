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
#elif define (__WIN32__)
 #include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>

//#include fzscreencommon.
#include "fzscreen.h"
#include "fztexture.h"

using namespace std;

FZScreen::FZScreen() {
    std::cout << 'FZScreen()' << endl;
}

FZScreen::~FZScreen() {
    std::cout << '~FZScreen()' << endl;
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
static void keyboard(GLFWwindow* window, int key, int scancode, int action, int mode) {
    std::cout << key << endl;
    
    // swap this to some mapping?
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_W: keyState |= FZ_CTRL_UP; break;
            case GLFW_KEY_S: keyState |= FZ_CTRL_DOWN; break;
            case GLFW_KEY_A: keyState |= FZ_CTRL_LEFT; break;
            case GLFW_KEY_D: keyState |= FZ_CTRL_RIGHT; break;
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
    }
}

/*
static void keyboardup(unsigned char key, int x, int y) {
  printf("Pressed: %i", key);
  switch (key) {
    
    case 27:
      exit(0);
      break;
    case 'w': keyState &= ~FZ_CTRL_UP; break;
    case 's': keyState &= ~FZ_CTRL_DOWN; break;
    case 'a': keyState &= ~FZ_CTRL_LEFT; break;
    case 'd': keyState &= ~FZ_CTRL_RIGHT; break;
    case 'k': keyState &= ~FZ_CTRL_SQUARE; break;
    case 'l': keyState &= ~FZ_CTRL_CROSS; break;
    case 'o': keyState &= ~FZ_CTRL_TRIANGLE; break;
    case 'p': keyState &= ~FZ_CTRL_CIRCLE; break;
    case 'v': keyState &= ~FZ_CTRL_SELECT; break;
    case 'b': keyState &= ~FZ_CTRL_START; break;
    case 'x': keyState &= ~FZ_CTRL_LTRIGGER; break;
    case 'c': keyState &= ~FZ_CTRL_RTRIGGER; break;
    case 'h': keyState &= ~FZ_CTRL_HOLD;break;

    case '6': powerSerial++; break;
  }
}
*/

static GLFWwindow* window;
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

    window = glfwCreateWindow(800, 600, "Bookr GLFW", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);


    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        std::cout << "Failed to initialize GLEW" << std::endl;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glfwSetKeyCallback(window, keyboard);

    //while(!glfwWindowShouldClose(window))
    //{
    //glfwPollEvents();

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
    //updateReps();
    //std::cout << keyState << std::endl;
    return keyState;
}

void FZScreen::swapBuffers() {
    glfwSwapBuffers(window);
}

void FZScreen::checkEvents() {
    glfwPollEvents();
}