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

#ifndef BKLAYER_H
#define BKLAYER_H

#include <vector>
#include <string>

using namespace std;

#include "fzfont.h"
#include "bkuser.h"

#define BK_CMD_CLOSE_TOP_LAYER 1
#define BK_CMD_MARK_DIRTY 2
#define BK_CMD_EXIT 3
#define BK_CMD_RELOAD 4
#define BK_CMD_MAINMENU_POPUP 5
#define BK_CMD_CLOSE_TOP_LAYER_RELOAD 6

#define BK_CMD_OPEN_FILE 20
#define BK_CMD_SET_FONT 21
#define BK_CMD_SET_TXTFG 22
#define BK_CMD_SET_TXTBG 23
#define BK_CMD_SET_PDFBG 24
#define BK_CMD_OPEN_PAGE 25

#define BK_CMD_INVOKE_MENU 100
#define BK_CMD_INVOKE_OPEN_FILE 101
#define BK_CMD_INVOKE_OPEN_FONT 102
#define BK_CMD_INVOKE_COLOR_CHOOSER_TXTFG 103
#define BK_CMD_INVOKE_COLOR_CHOOSER_TXTBG 104
#define BK_CMD_INVOKE_COLOR_SCHEME_MANAGER 105
#define BK_CMD_INVOKE_PAGE_CHOOSER 106

#define BK_IMG_TRIANGLE_X 9
#define BK_IMG_TRIANGLE_Y 53
#define BK_IMG_CIRCLE_X 31
#define BK_IMG_CIRCLE_Y 70
#define BK_IMG_CROSS_X 31
#define BK_IMG_CROSS_Y 53
#define BK_IMG_LRARROWS_X 7
#define BK_IMG_LRARROWS_Y 92
#define BK_IMG_CLOCK_X 100
#define BK_IMG_CLOCK_Y 0
#define BK_IMG_BATTERY_X 100
#define BK_IMG_BATTERY_Y 20
#define BK_IMG_MEMORY_X 76
#define BK_IMG_MEMORY_Y 18
#define BK_IMG_FOLDER_X 58
#define BK_IMG_FOLDER_Y 81

#define BK_IMG_TRIANGLE_XSIZE 20
#define BK_IMG_TRIANGLE_YSIZE 18
#define BK_IMG_CIRCLE_XSIZE 20
#define BK_IMG_CIRCLE_YSIZE 20
#define BK_IMG_CROSS_XSIZE 20
#define BK_IMG_CROSS_YSIZE 17
#define BK_IMG_LRARROWS_XSIZE 20
#define BK_IMG_LRARROWS_YSIZE 17
#define BK_IMG_CLOCK_XSIZE 16
#define BK_IMG_CLOCK_YSIZE 16
#define BK_IMG_BATTERY_XSIZE 16
#define BK_IMG_BATTERY_YSIZE 16
#define BK_IMG_MEMORY_XSIZE 16
#define BK_IMG_MEMORY_YSIZE 16
#define BK_IMG_FOLDER_XSIZE 20
#define BK_IMG_FOLDER_YSIZE 20

class BKLayer : public FZRefCounted {
	protected:
	static FZFont* fontBig;
	static FZFont* fontSmall;
	static FZTexture* texUI;
	static FZTexture* texUI2;
	static FZTexture* texLogo;

	int textW(char* t, FZFont* font);
	int textWidthRange(char* t, int n, FZFont* font);
	void drawRect(int x, int y, int w, int h, int r, int tx, int ty);
	void drawPill(int x, int y, int w, int h, int r, int tx, int ty);
	void drawTPill(int x, int y, int w, int h, int r, int tx, int ty);
	int drawText(char* t, FZFont* font, int x, int y, int n = -1, bool useLF = true, bool usePS = false, float ps = 0.0f, bool use3D = false);

	void drawTextHC(char* t, FZFont* font, int y);
	void drawImage(int x, int y, int w, int h, int tx, int ty);
	void drawImageScale(int x, int y, int w, int h, int tx, int ty, int tw, int th);

	BKLayer();
	~BKLayer();

	// "flexible" menu
	#define BK_MENU_ITEM_FOLDER			1
	#define BK_MENU_ITEM_USE_LR_ICON	2
	#define BK_MENU_ITEM_COLOR_RECT		4
	#define BK_MENU_ITEM_OPTIONAL_TRIANGLE_LABEL 8
	struct BKMenuItem {
		string label;
		string circleLabel;
		string triangleLabel;
		int flags;
		unsigned int fgcolor;
		unsigned int bgcolor;
		BKMenuItem() : flags(0) { }
		BKMenuItem(string& l, string& cl, int f) : label(l), circleLabel(cl), flags(f) { }
		BKMenuItem(char* l, string& cl, int f) : label(l), circleLabel(cl), flags(f) { }
		BKMenuItem(string& l, char* cl, int f) : label(l), circleLabel(cl), flags(f) { }
		BKMenuItem(char* l, char* cl, int f) : label(l), circleLabel(cl), flags(f) { }
	};
	int topItem;
	int selItem;
	void drawDialogFrame(string& title, string& triangleLabel, string& circleLabel, int flags);
	void drawMenu(string& title, string& triangleLabel, vector<BKMenuItem>& items);
	void menuCursorUpdate(unsigned int buttons, int max);

	void drawPopup(string& text, string& title, int bg1, int bg2, int fg);

	void drawClockAndBattery(string& extra);

	public:
	virtual int update(unsigned int buttons) = 0;
	virtual void render() = 0;

	static void load();
};

typedef vector<BKLayer*> bkLayers;
typedef vector<BKLayer*>::iterator bkLayersIt;

#endif

