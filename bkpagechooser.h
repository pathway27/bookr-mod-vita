/*
 * bkpagechooser: random page accessing extension for bookr 
 * Copyright (C) 2007 Yang.Hu (findreams at gmail dot com)
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

#ifndef BKPAGECHOOSER_H
#define BKPAGECHOOSER_H

#include "fzscreen.h"

using namespace std;

#include "bklayer.h"

class BKPageChooser : public BKLayer {
	int totalPage;
	int currentPage;
	int ret;
	int numOfDigit;
	int currentPosition;
	int invalidPage;

	protected:
	BKPageChooser(int t, int c, int r);
	~BKPageChooser();

	public:
	virtual int update(unsigned int buttons);
	virtual void render();

	int getCurrentPage();

	static BKPageChooser* create(int t, int c, int r);
};

#endif

