/*
 * Bookr - Book Reader
 * Copyright (c) 2005 ccm
 */

/*
 * Based on sample code from PSPDEV and licensed under the same terms:
 *
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * Copyright (c) 2005 Jesper Svennevid
 */


/*
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspgu.h>
*/

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "fzscreen.h"

#include "fzimage.h"
#include "fzinstreammem.h"
#include "fzfont.h"

#include "bkstrlines.h"

#ifdef PSP
#include <pspkernel.h>
PSP_MODULE_INFO("Bookr", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

extern "C" {
	int bookr(int argc, char* argv[]);
};

static char* text;
static int textLen;

static char psp_full_path[1024 + 1];

void setup(int argc, char* argv[]) {
	/* Get the full path to EBOOT.PBP. */
	char *psp_eboot_path;
	
	strncpy(psp_full_path, argv[0], sizeof(psp_full_path) - 1);
	psp_full_path[sizeof(psp_full_path) - 1] = '\0';
	
	psp_eboot_path = strrchr(psp_full_path, '/');
	if (psp_eboot_path != NULL) {
		*psp_eboot_path = '\0';
	}
}

static void loadFile() {
	char psp_filename[1024 + 1];
	snprintf(psp_filename, sizeof(psp_filename), "%s/%s", psp_full_path, "book.txt");
	//printf("fugu %s\n", psp_filename);
	FILE* f = fopen(psp_filename, "r");
	text = (char*)memalign(16, 300000);
	fread(text, 300000, 1, f);
	text[299999] = 0;
	textLen = 300000;
	fclose(f);
}

/*
	SceIoDirent findData;
	int findStatus;
	
	SceUID hFind = sceIoDopen(pFindPattern);
	DBGASSERT(hFind >= 0, "Couldnt start find..");
	
	findStatus = sceIoDread(hFind, &findData);
	
	DBGASSERT(false, STR("hmmmm %d.. i crash before i ever get here...", findStatus));
*/

static int baseLine = 0;
static int page = 1;
static int screenLines;
static int holdKeyUp = 0;
static int holdKeyDown = 0;

static FZFont* font = 0;
int bookr(int argc, char* argv[]) {
	setup(argc, argv);

	FZScreen::setupCallbacks();

	loadFile();

	FZScreen::open(argc, argv);

	char psp_filename[1024 + 1];
	snprintf(psp_filename, sizeof(psp_filename), "%s/%s", psp_full_path, "vera.ttf");
	font = FZFont::createFromFile(psp_filename, 11, false);
	font->texEnv(FZ_TEX_MODULATE);
	font->filter(FZ_NEAREST, FZ_NEAREST);
	FZScreen::enable(FZ_TEXTURE_2D);

	bkInitPrint();
	bkTextLineMetrics(font, text, textLen, 450);
	screenLines = 257 / font->getLineHeight();

	// controller
	FZScreen::setupCtrl();

	FZScreen::dcacheWritebackAll();

	FZ_DEBUG_SCREEN_INIT

	// run sample
	for(;;)	{
		FZScreen::startDirectList();
		FZScreen::clear(0xffffff, FZ_COLOR_BUFFER);
		FZScreen::matricesFor2D();
		font->bindForDisplay();

		FZScreen::enable(FZ_BLEND);
		FZScreen::blendFunc(FZ_ADD, FZ_SRC_ALPHA, FZ_ONE_MINUS_SRC_ALPHA);

		FZScreen::ambientColor(0xffffffff);
		FZScreen::color(0xff000000);

		int totalVertices = bkCountVerticesForLines(baseLine, baseLine + screenLines);
		//struct BKVertex* vertices = (struct BKVertex*)sceGuGetMemory(sizeof(struct BKVertex) * totalVertices);
		struct BKVertex* vertices = (struct BKVertex*)FZScreen::getListMemory(sizeof(struct BKVertex) * totalVertices);
		bkSpritesForLines(font, 15, 10, baseLine, baseLine + screenLines, vertices);

		FZScreen::drawArray(FZ_SPRITES,FZ_TEXTURE_32BITF|FZ_VERTEX_32BITF|FZ_TRANSFORM_2D,totalVertices,0,vertices);

		/*struct BKVertex* vertices = (struct BKVertex*)sceGuGetMemory(2 * sizeof(struct BKVertex));
		vertices[0].u = 0; vertices[0].v = 0;
		vertices[0].x = 0; vertices[0].y = 0; vertices[0].z = 0;
		vertices[1].u = 255; vertices[1].v = 255;
		vertices[1].x = 255; vertices[1].y = 255; vertices[1].z = 0;
		sceGuDrawArray(GU_SPRITES,GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_2D,2,0,vertices);*/

		FZScreen::endAndDisplayList();

		FZScreen::swapBuffers();
		FZScreen::checkEvents();

		FZ_DEBUG_SCREEN_SET00
		//printf("p %d\n", page);

		int buttons = FZScreen::readCtrl();
		if (buttons & FZ_CTRL_UP) {
			holdKeyUp++;
		}  else {
			holdKeyUp = 0;
		}
		if (holdKeyUp == 1 || holdKeyUp > 20) {
			baseLine-=screenLines;
			page--;
			if (baseLine < 0) {
				baseLine = 0;
				page = 1;
			}
		}
		if (buttons & FZ_CTRL_DOWN) {
			holdKeyDown++;
		}  else {
			holdKeyDown = 0;
		}
		if (holdKeyDown == 1 || holdKeyDown > 20) {
			baseLine+=screenLines;
			page++;
		}
	}

	FZScreen::close();
	FZScreen::exit();

	return 0;
}

