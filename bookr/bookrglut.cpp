/*
 * Bookr - Book Reader
 * Copyright (c) 2005 ccm
 */

#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fzimage.h"
#include "fzscreen.h"

#include "bkstrlines.h"

extern "C" {
	int bookr(int argc, char* argv[]);
};

static FZFont* font = 0;

static char* text = NULL;
static int textLen = 0;

static void loadFile() {
	FILE* f = fopen("data/book.txt", "r");
	text = (char*)malloc(300000);
	fread(text, 300000, 1, f);
	text[299999] = 0;
	textLen = 300000;
	fclose(f);
}

static int baseLine = 0;
static int screenLines;
static int holdKeyUp = 0;
static int holdKeyDown = 0;

int bookr(int argc, char* argv[]) {
	unsigned int i,j;

	FZScreen::setupCallbacks();

	loadFile();

	FZScreen::open(argc, argv);

	font = FZFont::createFromFile("data/vera.ttf", 11, false);
	font->texEnv(FZ_TEX_MODULATE);
	font->filter(FZ_NEAREST, FZ_NEAREST);
	FZScreen::enable(FZ_TEXTURE_2D);

	bkInitPrint();
	bkTextLineMetrics(font, text, textLen, 450);
	screenLines = 257 / font->getLineHeight();

	// run sample
	for(;;)	{

			FZScreen::startDirectList();
			FZScreen::clear(0xffffffff, FZ_COLOR_BUFFER);
			FZScreen::matricesFor2D();
			font->bindForDisplay();

			FZScreen::enable(FZ_BLEND);
			FZScreen::blendFunc(FZ_ADD, FZ_SRC_ALPHA, FZ_ONE_MINUS_SRC_ALPHA);
			//glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			int totalVertices = bkCountVerticesForLines(baseLine, baseLine + screenLines);
			struct BKVertex* vertices = (struct BKVertex*)malloc(sizeof(struct BKVertex) * totalVertices);
			bkSpritesForLines(font, 15, 10, baseLine, baseLine + screenLines, vertices);

			FZScreen::color(0xff000000);

			glBegin(GL_QUADS);
			int n = totalVertices/2;
			for (int i = 0; i < n; i++) {
				struct BKVertex* topleft = &vertices[i*2];
				struct BKVertex* botright = &vertices[i*2+1];

				topleft->u /= 256.0f;
				topleft->v /= 256.0f;
				botright->u /= 256.0f;
				botright->v /= 256.0f;

				glTexCoord2f(topleft->u, topleft->v);
				glVertex2f(topleft->x, topleft->y);

				glTexCoord2f(botright->u, topleft->v);
				glVertex2f(botright->x, topleft->y);

				glTexCoord2f(botright->u, botright->v);
				glVertex2f(botright->x, botright->y);

				glTexCoord2f(topleft->u, botright->v);
				glVertex2f(topleft->x, botright->y);

				/*glTexCoord2f(0.0f, 0.0f);
				glVertex2f(0.0f, 0.0f);

				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(256.0f, 0.0f);

				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(256.0f, 256.0f);

				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(0.0f, 256.0f);*/
			}
			glEnd();

			free(vertices);

		FZScreen::swapBuffers();
		FZScreen::checkEvents();

		int buttons = FZScreen::readCtrl();
		if (buttons & FZ_CTRL_UP) {
			holdKeyUp++;
		}  else {
			holdKeyUp = 0;
		}
		if (holdKeyUp == 1 || holdKeyUp > 20) {
			baseLine-=screenLines;
			//page--;
			if (baseLine < 0) {
				baseLine = 0;
				//page = 1;
			}
		}
		if (buttons & FZ_CTRL_DOWN) {
			holdKeyDown++;
		}  else {
			holdKeyDown = 0;
		}
		if (holdKeyDown == 1 || holdKeyDown > 20) {
			baseLine+=screenLines;
			//page++;
		} 
	}

	return 0;
}

