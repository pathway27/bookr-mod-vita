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

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include <pspgu.h>

#include "fzimage.h"
#include "fzinstreammem.h"

#include "bkfont.h"
#include "bkstrlines.h"


PSP_MODULE_INFO("Bookr", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

extern "C" {
	int bookr(int argc, char* argv[]);
};

#define printf	pspDebugScreenPrintf

static unsigned int __attribute__((aligned(16))) list[262144];

int SetupCallbacks();

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
	FILE* f = fopen(psp_filename, "r");
	text = (char*)malloc(300000);
	fread(text, 300000, 1, f);
	text[299999] = 0;
	textLen = 300000;
	fclose(f);
}

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define PIXEL_SIZE (4) /* change this if you change to another screenmode */
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE (BUF_WIDTH SCR_HEIGHT * 2) /* zbuffer seems to be 16-bit? */

//unsigned int __attribute__((aligned(16))) clut256[256];
//unsigned char __attribute__((aligned(16))) tex256[256*256];

static int baseLine = 0;
static int page = 1;
static int screenLines;
static int holdKeyUp = 0;
static int holdKeyDown = 0;
int bookr(int argc, char* argv[]) {
	unsigned int i,j;

	setup(argc, argv);

	SetupCallbacks();
/*
	SceIoDirent findData;
	int findStatus;
	
	SceUID hFind = sceIoDopen(pFindPattern);
	DBGASSERT(hFind >= 0, "Couldnt start find..");
	
	findStatus = sceIoDread(hFind, &findData);
	
	DBGASSERT(false, STR("hmmmm %d.. i crash before i ever get here...", findStatus));
*/
	loadFile();

	char psp_filename[1024 + 1];
	snprintf(psp_filename, sizeof(psp_filename), "%s/%s", psp_full_path, "vera.ttf");
	bkFontPrecalc(psp_filename, 11);

	bkInitPrint();
	bkTextLineMetrics(text, textLen, 450);
	screenLines = 257 / bkFontLineHeight();

	unsigned char* tex = bkFontGetTexture();
	int h = bkFontGetTextureHeight();
	//printf("h %d\n", h);
	unsigned char* tex256 = (unsigned char*)memalign(16, 256*h);
	for (j = 0; j < h; ++j) {
		for (i = 0; i < 256; ++i, ++tex) {
			tex256[i + j * 256] = 255 - *tex;
		}
	}
	// grayscale palette
	unsigned int* clut256 = (unsigned int*)memalign(16, 256);
	unsigned int* clut = clut256;
	for (i = 0; i < 256; ++i) {
		unsigned int j = i;
		*(clut++) = (j << 24)|(j << 16)|(j << 8)|(j);
	}

	// controller
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

	sceKernelDcacheWritebackAll();

	pspDebugScreenInit();

	// setup GU

	sceGuInit();
	sceGuStart(GU_DIRECT,list);

	sceGuDrawBuffer(GU_PSM_8888,(void*)0,BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)FRAME_SIZE,BUF_WIDTH);
	sceGuDepthBuffer((void*)(FRAME_SIZE*2),BUF_WIDTH);
	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	sceGuDepthRange(0xc350,0x2710);
	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuFrontFace(GU_CW);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
	sceGuFinish();
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	// run sample
	for(;;)	{
		sceGuStart(GU_DIRECT,list);

		// clear screen
		sceGuClearColor(0xffffff);
		sceGuClear(GU_COLOR_BUFFER_BIT);

		// setup CLUT texture
		sceGuClutMode(GU_PSM_8888,0,0xff,0); // 32-bit palette
		sceGuClutLoad((256/8),clut256); // upload 32*8 entries (256)
		sceGuTexMode(GU_PSM_T8,0,0,0); // 8-bit image
		sceGuTexImage(0,256,h,256,tex256);
		sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGB);
		sceGuTexFilter(GU_NEAREST,GU_NEAREST);
		sceGuTexScale(1.0f,1.0f);
		sceGuTexOffset(0.0f,0.0f);
		sceGuAmbientColor(0xffffffff);

		// render text
		sceGuColor(0xffffffff);
		/*int totalVertices = bkCountVerticesForLines(0, 7);
		struct BKVertex* vertices = (struct BKVertex*)sceGuGetMemory(totalVertices * sizeof(struct BKVertex));
		bkSpritesForLines(15, 10, 0, 7, vertices);*/

		int totalVertices = bkCountVerticesForLines(baseLine, baseLine + screenLines);
		struct BKVertex* vertices = (struct BKVertex*)sceGuGetMemory(sizeof(struct BKVertex) * totalVertices);
		bkSpritesForLines(15, 10, baseLine, baseLine + screenLines, vertices);

		sceGuDrawArray(GU_SPRITES,GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_2D,totalVertices,0,vertices);
		/*vertices = (struct BKVertex*)sceGuGetMemory(2 * sizeof(struct BKVertex));
		vertices[0].u = 0; vertices[0].v = 0;
		vertices[0].x = 0; vertices[0].y = 0; vertices[0].z = 0;
		vertices[1].u = 255; vertices[1].v = 255;
		vertices[1].x = 255; vertices[1].y = 255; vertices[1].z = 0;
		sceGuDrawArray(GU_SPRITES,GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_2D,2,0,vertices);*/

		// wait for next frame
		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		sceGuSwapBuffers();

		pspDebugScreenSetXY(0,0);
		pspDebugScreenPrintf("%d", page);

		SceCtrlData pad;
    		sceCtrlReadBufferPositive(&pad, 1); 
		if (pad.Buttons & PSP_CTRL_UP) {
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
		if (pad.Buttons & PSP_CTRL_DOWN) {
			holdKeyDown++;
		}  else {
			holdKeyDown = 0;
		}
		if (holdKeyDown == 1 || holdKeyDown > 20) {
			baseLine+=screenLines;
			page++;
		} 
	}

	sceGuTerm();

	sceKernelExitGame();
	return 0;
}

/* Exit callback */
/*int exit_callback(void)
{
	sceKernelExitGame();
	return 0;
}*/

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
        sceKernelExitGame();
        return 0;
}



/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}


