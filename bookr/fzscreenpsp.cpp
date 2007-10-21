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

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <pspgum.h>
#include <psppower.h>
#include <psprtc.h>
#include <malloc.h>

#include "fzscreen.h"
#include "fztexture.h"

FZScreen::FZScreen() {
}

FZScreen::~FZScreen() {
}

// most stuff here comes directly from pspdev sdk examples

// default display list
//static unsigned int __attribute__((aligned(16))) list[262144];
static unsigned int* list;

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define PIXEL_SIZE (4) /* change this if you change to another screenmode */
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE (BUF_WIDTH SCR_HEIGHT * 2) /* zbuffer seems to be 16-bit? */

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common) {
        sceKernelExitGame();
        return 0;
}

// yeah yeah yeah no mutex whatever whatever whatever
static volatile int powerResumed = 0;

/* Power Callback */
int power_callback(int unknown, int pwrflags, void *common) {
    /* check for power switch and suspending as one is manual and the other automatic */
    if (pwrflags & PSP_POWER_CB_POWER_SWITCH || pwrflags & PSP_POWER_CB_SUSPENDING) {
        /*sprintf(powerCBMessage,
                "first arg: 0x%08X, flags: 0x%08X: suspending\n", unknown, pwrflags);*/
    } else if (pwrflags & PSP_POWER_CB_RESUMING) {
        /*sprintf(powerCBMessage,
                "first arg: 0x%08X, flags: 0x%08X: resuming from suspend mode\n",
                unknown, pwrflags);*/
		powerResumed++;
    } else if (pwrflags & PSP_POWER_CB_RESUME_COMPLETE) {
        /*sprintf(powerCBMessage,
                "first arg: 0x%08X, flags: 0x%08X: resume complete\n", unknown, pwrflags);*/
		powerResumed++;
    } else if (pwrflags & PSP_POWER_CB_STANDBY) {
        /*sprintf(powerCBMessage,
                "first arg: 0x%08X, flags: 0x%08X: entering standby mode\n", unknown, pwrflags);*/
    } else {
        /*sprintf(powerCBMessage, "first arg: 0x%08X, flags: 0x%08X: Unhandled power event\n", unknown);*/
	}
    sceDisplayWaitVblankStart();

	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp) {
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
    cbid = sceKernelCreateCallback("Power Callback", power_callback, NULL);
    scePowerRegisterCallback(0, cbid);

	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int FZScreen::setupCallbacks(void) {
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

// from pspgl
static void * ptr_align64_uncached(unsigned long ptr) {
	// uncached access suck. better to use cached access and
	// flush dcache just before dlist issue
	//return (void *) (((ptr + 0x0f) & ~0x0f) | 0x40000000);
	return (void *) ((ptr + 0x3f) & ~0x3f);
}


static char psp_full_path[1024 + 1];
void FZScreen::open(int argc, char** argv) {
	/* Get the full path to EBOOT.PBP. */
	char *psp_eboot_path;
	
	strncpy(psp_full_path, argv[0], sizeof(psp_full_path) - 1);
	psp_full_path[sizeof(psp_full_path) - 1] = '\0';
	
	psp_eboot_path = strrchr(psp_full_path, '/');
	if (psp_eboot_path != NULL) {
		*psp_eboot_path = '\0';
	}

	string path(psp_full_path);
	path += "/fonts";
	setenv("BOOKRFONTDIR", path.c_str(), 1);
	setenv("BOOKRCMAPDIR", path.c_str(), 1);

	// from pspgl. this memory block won't be freed
	list = (unsigned int*)malloc(4 * (256*1024));
	list = (unsigned int*)ptr_align64_uncached((unsigned long)list + 64);

	sceGuInit();
	sceGuStart(GU_DIRECT, list);
	sceGuDrawBuffer(GU_PSM_8888, (void*)0, BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, (void*)FRAME_SIZE, BUF_WIDTH);
	sceGuDepthBuffer((void*)(FRAME_SIZE*2), BUF_WIDTH);
	sceGuOffset(2048 - (SCR_WIDTH/2), 2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048, 2048, SCR_WIDTH,SCR_HEIGHT);
	sceGuDepthRange(0xc350, 0x2710);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDisable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	//sceGuEnable(GU_TEXTURE_2D);
	sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
	sceGuFinish();
	sceKernelDcacheWritebackAll();	
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);
}

void FZScreen::close() {
	sceGuTerm();
}

void FZScreen::exit() {
	sceKernelExitGame();
}

static bool stickyKeys = false;

static int breps[16];

static void updateReps(int keyState) {
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
		
void FZScreen::resetReps() {
	stickyKeys = true;
}
		
int* FZScreen::ctrlReps() {
	return breps;
}

void FZScreen::setupCtrl() {
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	resetReps();
}

static int lastAnalogX = 0;
static int lastAnalogY = 0;
int FZScreen::readCtrl() {
	SceCtrlData pad;
	sceCtrlPeekBufferPositive(&pad, 1);
	updateReps(pad.Buttons);
	lastAnalogX = pad.Lx;
	lastAnalogY = pad.Ly;
	return pad.Buttons;
}

void FZScreen::getAnalogPad(int& x, int& y) {
	x = lastAnalogX - 128;
	y = lastAnalogY - 128;
}

void FZScreen::startDirectList() {
	sceGuStart(GU_DIRECT, list);
}

void FZScreen::endAndDisplayList() {
	sceGuFinish();
	sceKernelDcacheWritebackAll();	
	sceGuSync(0,0);
}

static void* lastFramebuffer = NULL;
void FZScreen::swapBuffers() {
//	sceDisplayWaitVblankStart();
	lastFramebuffer = sceGuSwapBuffers();
}

void FZScreen::waitVblankStart() {
	sceDisplayWaitVblankStart();
}

void* FZScreen::getListMemory(int s) {
	return sceGuGetMemory(s);
}

void FZScreen::shadeModel(int mode) {
	sceGuShadeModel(mode);
}

void FZScreen::color(unsigned int c) {
	sceGuColor(c);
}

void FZScreen::ambientColor(unsigned int c) {
	sceGuAmbientColor(c);
}

void FZScreen::clear(unsigned int color, int b) {
	sceGuClearColor(color);
	int m = 0;
	if (b & FZ_COLOR_BUFFER)
		m |= GU_COLOR_BUFFER_BIT;
	if (b & FZ_DEPTH_BUFFER)
		m |= GU_DEPTH_BUFFER_BIT;
	sceGuClear(m);
}

void FZScreen::checkEvents() {
}

void FZScreen::matricesFor2D(int rotation) {
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumOrtho(0.0f, 480.0f, 272.0f, 0.0f, 0.0f, 1.0f);

	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();

	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();

	switch (rotation) {
		case 1: {
			ScePspFVector3 pos = { 480.0f, 0.0f, 0.0f };
			sceGumTranslate(&pos);
			sceGumRotateZ(90.0f * (GU_PI/180.0f));
		} break;
		case 2: {
			ScePspFVector3 pos = { 480.0f, 272.0f, 0.0f };
			sceGumTranslate(&pos);
			sceGumRotateZ(180.0f * (GU_PI/180.0f));
		} break;
		case 3: {
			ScePspFVector3 pos = { 0.0f, 272.0f, 0.0f };
			sceGumTranslate(&pos);
			sceGumRotateZ(270.0f * (GU_PI/180.0f));
		} break;
	}
}

struct T32FV32F2D {
	float u,v;
	float x,y,z;
};

static FZTexture* boundTexture = 0;
void FZScreen::setBoundTexture(FZTexture *t) {
	boundTexture = t;
}

void FZScreen::drawArray(int prim, int vtype, int count, void* indices, void* vertices) {
	if ((vtype & GU_TRANSFORM_2D) == 0) {
		// hack...
		struct T32FV32F2D* verts = (struct T32FV32F2D*)vertices;
		float w = 256.0f;
		float h = 256.0f;
		if (boundTexture != 0) {
			w = boundTexture->getWidth();
			h = boundTexture->getHeight();
		}
		for (int i = 0; i < count; i++) {
			verts[i].u /= w;
			verts[i].v /= h;
		}
		sceGumDrawArray(prim, vtype, count, indices, vertices);
	}
	else {
		sceGuDrawArray(prim, vtype, count, indices, vertices);
	}
}

void FZScreen::copyImage(int psm, int sx, int sy, int width, int height, int srcw, void *src,
	int dx, int dy, int destw, void *dest) {
	sceGuCopyImage(psm, sx, sy, width, height, srcw, src, dx, dy, destw, dest);
}

void* FZScreen::framebuffer() {
	return lastFramebuffer;
}

void FZScreen::blendFunc(int op, int src, int dst) {
	sceGuBlendFunc(op, src, dst, 0, 0);
}

void FZScreen::enable(int m) {
	sceGuEnable(m);
}

void FZScreen::disable(int m) {
	sceGuDisable(m);
}

void FZScreen::dcacheWritebackAll() {
	sceKernelDcacheWritebackAll();	
}

char* FZScreen::basePath() {
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

int FZScreen::dirContents(char* path, vector<FZDirent>& a) {
	SceUID fd;
	SceIoDirent *findData;
	findData = (SceIoDirent*)memalign(16, sizeof(SceIoDirent));	// dont ask me WHY...
	memset((void*)findData, 0, sizeof(SceIoDirent));
	//a.push_back(FZDirent("books/udhr.pdf", FZ_STAT_IFREG, 0));
	//a.push_back(FZDirent("books/1984.txt", FZ_STAT_IFREG, 587083));
	fd = sceIoDopen(path);
	if (fd < 0)
		return -1;
	while (sceIoDread(fd, findData) > 0) {
		if (findData->d_name[0] != 0 && findData->d_name[0] != '.') {
			a.push_back(FZDirent(findData->d_name, findData->d_stat.st_mode, findData->d_stat.st_size));
		}
	}
	sceIoDclose(fd);
	free(findData);
	sort(a.begin(), a.end(), CompareDirent());
	return 1;
}

int FZScreen::getSuspendSerial() {
	return powerResumed;
}

void FZScreen::setSpeed(int v) {
	if (v <= 0 || v > 6)
		return;
	scePowerSetCpuClockFrequency(speedValues[v*2]);
	scePowerSetBusClockFrequency(speedValues[v*2+1]);
}

int FZScreen::getSpeed() {
	return scePowerGetCpuClockFrequency();
}

void FZScreen::getTime(int &h, int &m) {
	pspTime time;
	if (sceRtcGetCurrentClockLocalTime(&time) >= 0) {
		h = time.hour;
		m = time.minutes;
	}
}

int FZScreen::getBattery() {
	if (scePowerIsBatteryExist()) {
		return scePowerGetBatteryLifePercent();
	}
	return 0;
}

int FZScreen::getUsedMemory() {
	struct mallinfo mi = mallinfo();
	return mi.uordblks;
	//return mi.arena;
}

