/*
 * Original Bookr and bookr-mod for PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com),
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com),
 *                  
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

/*   ----------     GLUT is deprecated on macOS 10.9+    ------------   */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cstring>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#ifdef MAC
  #include <OpenGL/gl.h>
  #include <GLUT/glut.h>
//  #include <Cocoa/Cocoa.h>
#elif defined (__CYGWIN__)
 #include "cygwin/freeglut.h"
#else
 #include <GL/freeglut.h>
#endif

//#include fzscreencommon.
#include "fzscreen.h"
#include "fztexture.h"

using namespace std;

FZScreen::FZScreen() {
}

FZScreen::~FZScreen() {
}

// default display list
static unsigned char __attribute__((aligned(16))) list[262144*4];
static unsigned int topList = 0;

static FZTexture* boundTexture = 0;

static void display(void) {
	//clear white, draw with black
    glClearColor(0, 0, 0, 0);
    glColor3d(255, 0, 255);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //this draws a square using vertices
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(0, 128);
    glVertex2i(128, 128);
    glVertex2i(128, 0);
    glEnd();

    //a more useful helper
    glRecti(200, 200, 250, 250);

    glutSwapBuffers();
}


static int keyState = 0;
static bool stickyKeys = false;
static int powerSerial = 0;

static int breps[16];

// update reps

static void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 27:
			exit(0);
		break;
		case 'w': keyState |= FZ_CTRL_UP; break;
		case 's': keyState |= FZ_CTRL_DOWN; break;
		case 'a': keyState |= FZ_CTRL_LEFT; break;
		case 'd': keyState |= FZ_CTRL_RIGHT; break;
		case 'k': keyState |= FZ_CTRL_SQUARE; break;
		case 'l': keyState |= FZ_CTRL_CROSS; break;
		case 'o': keyState |= FZ_CTRL_TRIANGLE; break;
		case 'p': keyState |= FZ_CTRL_CIRCLE; break;
		case 'v': keyState |= FZ_CTRL_SELECT; break;
		case 'b': keyState |= FZ_CTRL_START; break;
		case 'x': keyState |= FZ_CTRL_LTRIGGER; break;
		case 'c': keyState |= FZ_CTRL_RTRIGGER; break;
		case 'h': keyState |= FZ_CTRL_HOLD;break;
	}
}

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

static bool mouseDrag = false;
static int originAnalogX = 0;
static int originAnalogY = 0;
static int lastAnalogX = 0;
static int lastAnalogY = 0;
static void mousepress(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		mouseDrag = true;
		originAnalogX = x;
		originAnalogY = y;
	} else {
		mouseDrag = false;
	}	
}

static void mousemotion(int x, int y) {
	lastAnalogX = x - originAnalogX;
	lastAnalogY = y - originAnalogY;
}

static char psp_full_path[1024 + 1];

void idle(void) {
    glutPostRedisplay();
}

void FZScreen::open(int argc, char** argv) {
	//getcwd(psp_full_path, 1024);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(480, 272);
	
	glutCreateWindow("Bookr");
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutMouseFunc(mousepress);
	glutMotionFunc(mousemotion);
	
	glClearColor(0.0, 0.0, 0.2, 0.0);
	glViewport(0, 0, 480, 272);

	glutMainLoop();
	//string path(psp_full_path);
	//path += "/fonts";
	//setenv("BOOKRFONTDIR", path.c_str(), 1);
	//setenv("BOOKRCMAPDIR", path.c_str(), 1);
}