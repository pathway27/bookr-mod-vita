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

#include "fzfont.h"
#include "bkuser.h"

#define BK_CMD_CLOSE_TOP_LAYER 1
#define BK_CMD_MARK_DIRTY 2
#define BK_CMD_EXIT 3

#define BK_CMD_OPEN_FILE 20
#define BK_CMD_OPEN_FONT 21

#define BK_CMD_INVOKE_MENU 100
#define BK_CMD_INVOKE_OPEN_FILE 101
#define BK_CMD_INVOKE_OPEN_FONT 102

class BKLayer : public FZRefCounted {
	protected:
	static FZFont* fontBig;
	static FZFont* fontText;
	static FZTexture* texUI;
	static FZTexture* texLogo;

	void drawRect(int x, int y, int w, int h, int r, int tx, int ty);
	void drawPill(int x, int y, int w, int h, int r, int tx, int ty);
	void drawTPill(int x, int y, int w, int h, int r, int tx, int ty);
	void drawText(char* t, FZFont* font, int x, int y);
	void drawTextHC(char* t, FZFont* font, int y);
	void drawImage(int x, int y, int w, int h, int tx, int ty);

	BKLayer();
	~BKLayer();

	// "flexible" menu
	#define BK_MENU_ITEM_FOLDER	1
	struct BKMenuItem {
		string label;
		string circleLabel;
		int flags;
		BKMenuItem() : flags(0) { }
		BKMenuItem(string& l, string& cl, int f) : label(l), circleLabel(cl), flags(f) { }
		BKMenuItem(char* l, string& cl, int f) : label(l), circleLabel(cl), flags(f) { }
		BKMenuItem(string& l, char* cl, int f) : label(l), circleLabel(cl), flags(f) { }
		BKMenuItem(char* l, char* cl, int f) : label(l), circleLabel(cl), flags(f) { }
	};
	int topItem;
	int selItem;
	void drawMenu(string& title, string& triangleLabel, vector<BKMenuItem>& items);
	void menuCursorUpdate(unsigned int buttons, int max);

	public:
	virtual int update(unsigned int buttons) = 0;
	virtual void render() = 0;

	static void load();
};

typedef vector<BKLayer*> bkLayers;
typedef vector<BKLayer*>::iterator bkLayersIt;

#endif


