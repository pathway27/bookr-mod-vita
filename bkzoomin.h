/*
 * bkzoomin: zoom in to specified left and right borders.
 * Copyright (C) 2009
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

#ifndef BKZOOMIN_H
#define BKZOOMIN_H

#include "fzscreen.h"

using namespace std;

#include "bklayer.h"

class BKZoomIn : public BKLayer {
	int leftPos;
	int rightPos;
	int ret;
	bool isLeftSelected;
	bool showUsage;
	protected:
	BKZoomIn(int leftPos, int rightPos, int ret);
	~BKZoomIn();

	public:
	virtual int update(unsigned int buttons);
	virtual void render();
	int getLeftPos();
	int getRightPos();
	static BKZoomIn* create(int leftPos, int rightPos, int ret);
};

#define BK_IMG_LARROW_XSIZE 5
#define BK_IMG_LARROW_YSIZE 9
#define BK_IMG_LARROW_X 100
#define BK_IMG_LARROW_Y 87

#define BK_IMG_RARROW_XSIZE 5
#define BK_IMG_RARROW_YSIZE 9
#define BK_IMG_RARROW_X 108
#define BK_IMG_RARROW_Y 87

#endif

