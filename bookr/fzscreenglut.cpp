/*
 * Bookr: document reader for the Sony PSP 
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com)
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
#include <sys/malloc.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#ifdef MAC
#include <GLUT/glut.h>
#include <Carbon/Carbon.h>
#elif defined (__CYGWIN__)
#include "cygwin/freeglut.h"
#else
#include <GL/freeglut.h>
#endif

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
	// dummy display func
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
	}
}

static void keyboardup(unsigned char key, int x, int y) {
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
void FZScreen::open(int argc, char** argv) {
	getcwd(psp_full_path, 1024);

	glutInit(&argc, argv);
	glutInitWindowSize(480, 272);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("Bookr");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutMouseFunc(mousepress);
	glutMotionFunc(mousemotion);
	glViewport(0, 0, 480, 272);

	string path(psp_full_path);
	path += "/fonts";
	setenv("BOOKRFONTDIR", path.c_str(), 1);
	setenv("BOOKRCMAPDIR", path.c_str(), 1);
}

void FZScreen::setBoundTexture(FZTexture *t) {
	boundTexture = t;
}

int FZScreen::setupCallbacks() {
	return 0;
}

void FZScreen::close() {
}

void FZScreen::exit() {
	::exit(0);
}

void FZScreen::setupCtrl() {
	resetReps();
}

int FZScreen::readCtrl() {
	updateReps();
	return keyState;
}

void FZScreen::getAnalogPad(int& x, int& y) {
	if (mouseDrag) {
		x = lastAnalogX;
		y = lastAnalogY;
	} else {
		x = 0;
		y = 0;
	}
}

void FZScreen::resetReps() {
	stickyKeys = true;
}

int* FZScreen::ctrlReps() {
	return breps;
}

void FZScreen::startDirectList() {
	topList = 0;
}

void FZScreen::endAndDisplayList() {
}

#ifndef MAC
#ifndef __CYGWIN__
extern "C" {
	extern int glXGetVideoSyncSGI(unsigned int *count);
	extern int glXWaitVideoSyncSGI(int divisor, int remainder, unsigned int *count);
};
#endif
#endif

void FZScreen::swapBuffers() {
	glutSwapBuffers();
}

void FZScreen::waitVblankStart() {
#ifndef MAC
#ifndef __CYGWIN__
	unsigned int c = 0;
	glXGetVideoSyncSGI(&c);
	glXWaitVideoSyncSGI(1, 0, &c);
#endif
#endif
}

void FZScreen::color(unsigned int c) {
}

void FZScreen::clear(unsigned int c, int b) {
	glClearColor(((float)(c & 0xff))/255.0f,
		((float)((c & 0xff00) >> 8))/255.0f,
		((float)((c & 0xff0000) >> 16))/255.0f,
		((float)((c & 0xff000000) >> 24))/255.0f);
	int m = 0;
	if (b & FZ_COLOR_BUFFER)
		m |= GL_COLOR_BUFFER_BIT;
	if (b & FZ_DEPTH_BUFFER)
		m |= GL_DEPTH_BUFFER_BIT;
	glClear(m);
}

void FZScreen::checkEvents() {
#ifdef MAC
	glutCheckLoop();
#else
	glutMainLoopEvent();
#endif
}

void FZScreen::matricesFor2D(int rotation) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, 480.0f, 272.0f, 0.0f, 0.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	switch (rotation) {
		case 1:
			glTranslatef(480.0f, 0.0f, 0.0f);
			glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
		break;
		case 2:
			glTranslatef(480.0f, 272.0f, 0.0f);
			glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
		break;
		case 3:
			glTranslatef(0.0f, 272.0f, 0.0f);
			glRotatef(270.0f, 0.0f, 0.0f, 1.0f);
		break;
	}
}

static int toGLBlendMode(int v) {
	int r = 0;
	switch (v) {
		case FZ_SRC_COLOR: r = GL_SRC_COLOR; break;
		case FZ_ONE_MINUS_SRC_COLOR: r = GL_ONE_MINUS_SRC_COLOR; break;
		case FZ_SRC_ALPHA: r = GL_SRC_ALPHA; break; 
		case FZ_ONE_MINUS_SRC_ALPHA: r = GL_ONE_MINUS_SRC_ALPHA; break;
		//case FZ_DST_COLOR: r = GL_DST_COLOR; break;
		//case FZ_ONE_MINUS_DST_COLOR: r = GL_ONE_MINUS_DST_COLOR; break;
	}
	return r;
}

void FZScreen::blendFunc(int op, int src, int dst) {
#ifndef __CYGWIN__	
	int gop = GL_FUNC_ADD;
	switch (op) {
		case FZ_SUBTRACT: gop = GL_FUNC_SUBTRACT; break;
	}
	// unused
	//glBlendEquation(gop);
#endif	
	glBlendFunc(toGLBlendMode(src), toGLBlendMode(dst));
}

static int toGLState(int v) {
	int r = 0;
	switch (v) {
		case FZ_ALPHA_TEST: r = GL_ALPHA_TEST; break;
		case FZ_DEPTH_TEST: r = GL_DEPTH_TEST; break;
		case FZ_SCISSOR_TEST: r = GL_SCISSOR_TEST; break;
		case FZ_STENCIL_TEST: r = GL_STENCIL_TEST; break;
		case FZ_BLEND: r = GL_BLEND; break;
		case FZ_CULL_FACE: r = GL_CULL_FACE; break;
		case FZ_DITHER: r = GL_DITHER; break;
		case FZ_FOG: r = GL_FOG; break;
		//case FZ_CLIP_PLANES: r = GL_CLIP_PLANES; break;
		case FZ_TEXTURE_2D: r = GL_TEXTURE_2D; break;
		case FZ_LIGHTING: r = GL_LIGHTING; break;
		case FZ_LIGHT0: r = GL_LIGHT0; break;
		case FZ_LIGHT1: r = GL_LIGHT1; break;
		case FZ_LIGHT2: r = GL_LIGHT2; break;
		case FZ_LIGHT3: r = GL_LIGHT3; break;
		/*case FZ_COLOR_TEST:
		case FZ_COLOR_LOGIC_OP:
		case FZ_FACE_NORMAL_REVERSE:
		case FZ_PATCH_FACE:
		case FZ_FRAGMENT_2X:*/
	}
	return r;
}

void FZScreen::enable(int m) {
	glEnable(toGLState(m));
}

void FZScreen::disable(int m) {
	glDisable(toGLState(m));
}

void FZScreen::dcacheWritebackAll() {
}

void FZScreen::shadeModel(int mode) {
}

void FZScreen::ambientColor(unsigned int c) {
	glColor4ub(c & 0xff, (c & 0xff00) >> 8, (c & 0xff0000) >> 16, (c & 0xff000000) >> 24);
}

void* FZScreen::getListMemory(int s) {
	void *r = &list[topList];
	topList += s;
	return r;
}

struct T32FV32F2D {
	float u,v;
	float x,y,z;
};

struct C8888V32F2D {
	unsigned int color;
	float x,y,z;
};

void FZScreen::drawArray(int prim, int vtype, int count, void* indices, void* vertices) {
	if (prim == FZ_TRIANGLES && vtype == (FZ_COLOR_8888|FZ_VERTEX_32BITF|FZ_TRANSFORM_2D)) {
		struct C8888V32F2D* verts = (struct C8888V32F2D*)vertices;
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < count; i++) {
			glColor4ubv((GLubyte*)&verts[i].color);
			glVertex2f(verts[i].x, verts[i].y);
		}
		glEnd();
	} else if (prim == FZ_TRIANGLES && vtype == (FZ_TEXTURE_32BITF|FZ_VERTEX_32BITF|FZ_TRANSFORM_3D)) {
		struct T32FV32F2D* verts = (struct T32FV32F2D*)vertices;
		glBegin(GL_TRIANGLES);
		int n = count/3;
		float w = 256.0f;
		float h = 256.0f;
		if (boundTexture != 0) {
			w = boundTexture->getWidth();
			h = boundTexture->getHeight();
		}
		for (int i = 0; i < n; i++) {
			struct T32FV32F2D* c1 = &verts[i*3];
			struct T32FV32F2D* c2 = &verts[i*3+1];
			struct T32FV32F2D* c3 = &verts[i*3+2];

			c1->u /= w;
			c1->v /= h;
			c2->u /= w;
			c2->v /= h;
			c3->u /= w;
			c3->v /= h;

			glTexCoord2f(c1->u, c1->v);
			glVertex2f(c1->x, c1->y);

			glTexCoord2f(c2->u, c2->v);
			glVertex2f(c2->x, c2->y);

			glTexCoord2f(c3->u, c3->v);
			glVertex2f(c3->x, c3->y);
		}
		glEnd();
	} else if (prim == FZ_SPRITES && vtype == (FZ_TEXTURE_32BITF|FZ_VERTEX_32BITF|FZ_TRANSFORM_2D)) {
		struct T32FV32F2D* verts = (struct T32FV32F2D*)vertices;
		glBegin(GL_QUADS);
		int n = count/2;
		float w = 256.0f;
		float h = 256.0f;
		if (boundTexture != 0) {
			w = boundTexture->getWidth();
			h = boundTexture->getHeight();
		}
		for (int i = 0; i < n; i++) {
			struct T32FV32F2D* topleft = &verts[i*2];
			struct T32FV32F2D* botright = &verts[i*2+1];

			topleft->u /= w;
			topleft->v /= h;
			botright->u /= w;
			botright->v /= h;

			glTexCoord2f(topleft->u, topleft->v);
			glVertex2f(topleft->x, topleft->y);

			glTexCoord2f(botright->u, topleft->v);
			glVertex2f(botright->x, topleft->y);

			glTexCoord2f(botright->u, botright->v);
			glVertex2f(botright->x, botright->y);

			glTexCoord2f(topleft->u, botright->v);
			glVertex2f(topleft->x, botright->y);
		}
		glEnd();
	} else {
		printf("unsupported drawArray invocation\n");
	}
}

/*
==18204== Conditional jump or move depends on uninitialised value(s)
==18204==    at 0x808CF3B: fz_dropimage (res_image.c:14)
==18204==    by 0x8089F77: fz_dropimagenode (node_misc2.c:287)
==18204==    by 0x808910B: fz_dropnode (node_misc1.c:56)
==18204==    by 0x80890B1: fz_dropnode (node_misc1.c:37)
==18204==
==18204== Conditional jump or move depends on uninitialised value(s)
==18204==    at 0x808CF3B: fz_dropimage (res_image.c:14)
==18204==    by 0x80627B4: dropitem (pdf_store.c:57)
==18204==    by 0x8062853: pdf_emptystore (pdf_store.c:79)
==18204==    by 0x80628D6: pdf_dropstore (pdf_store.c:97)
*/

static GLuint bounceTex = 0;
void FZScreen::copyImage(int psm, int sx, int sy, int width, int height, int srcw, void *src, int dx, int dy, int destw, void *dest) {
	if (bounceTex == 0) {
		glGenTextures(1, &bounceTex);
		glBindTexture(GL_TEXTURE_2D, bounceTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	} else {
		glBindTexture(GL_TEXTURE_2D, bounceTex);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	}
	if (psm != FZ_PSM_8888) {
		printf("non supported image format %d in copyImage\n", psm);
		return;
	}
	// texsubimage wants a non-strided array... so this call will fail
	// in many cases. Handle With Care.
	glTexSubImage2D(GL_TEXTURE_2D, 0,
		dx, dy, width, height,
		GL_RGBA, GL_UNSIGNED_BYTE, src);

	float w = 512.0f;
	float h = 512.0f;
	glDisable(GL_BLEND);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex2f(0, 0);

	glTexCoord2f(480/w, 0);
	glVertex2f(480, 0);

	glTexCoord2f(480/w, 272/h);
	glVertex2f(480, 272);

	glTexCoord2f(0, 272/h);
	glVertex2f(0, 272);

	glEnd();
	glEnable(GL_BLEND);
}

void* FZScreen::framebuffer() {
	return NULL;
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

int FZScreen::dirContents(char* path, vector<FZDirent>& a) {
	DIR *dp;
	struct dirent *ep;

	dp = opendir(path);
	if (dp == NULL)
		return -1;
	string pt(path);
	while ((ep = readdir(dp))) {
		if (ep->d_name[0] != 0 && ep->d_name[0] != '.') {
			unsigned int s = 0;
#ifndef __CYGWIN__
			if (ep->d_type == DT_REG)
				s |= FZ_STAT_IFREG;
			if (ep->d_type == DT_DIR)
				s |= FZ_STAT_IFDIR;
#else
			// TODO: This is incorrect
			s |= FZ_STAT_IFREG;
#endif			
			if (s == 0)
				continue;
			struct stat st;
			memset((void*)&st, 0, sizeof(struct stat));
			string ptf(pt);
			ptf += "/";
			ptf += ep->d_name;
			int r = stat(ptf.c_str(), &st);
			a.push_back(FZDirent(ep->d_name, s, st.st_size));
		}
	}
	closedir(dp);
	sort(a.begin(), a.end(), CompareDirent());
	return 1;
}
/*
void FZScreen::freeDirContents(char** a, int n) {
	for (int i = 0; i < n; ++i) {
		free(a[i]);
	}
	free(a);
}
*/
char* FZScreen::basePath() {
	return psp_full_path;
}

int FZScreen::getSuspendSerial() {
	return powerSerial;
}

void FZScreen::setSpeed(int v) {
}

int FZScreen::getSpeed() {
#ifdef MAC
	return CurrentProcessorSpeed();
#else
	return 0;
#endif
}

void FZScreen::getTime(int &h, int &m) {
	time_t t = time(NULL);
	struct tm* lt = localtime(&t);
	h = lt->tm_hour;
	m = lt->tm_min;
}

int FZScreen::getBattery() {
	return 100;
}

int FZScreen::getUsedMemory() {
#ifdef MAC
/*
	ProcessSerialNumber* PSN;
	ProcessInfoRec* info;

	GetCurrentProcess(PSN);
	GetProcessInformation(PSN, info);
	
	return info->processSize;
*/
	return 0;
#else
	struct mallinfo mi = mallinfo();
	return mi.uordblks;
	//return mi.arena;
#endif
}
	

