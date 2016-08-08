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

#ifndef BKPOPUP_H
#define BKPOPUP_H

#include "fzscreen.h"

using namespace std;

#include "bklayer.h"

#define BKPOPUP_WARNING		1
#define BKPOPUP_ERROR		2
#define BKPOPUP_INFO		3

class BKPopup : public BKLayer {
	int mode;
	string text;

	protected:
	BKPopup(int m, string t);	
	~BKPopup();

	public:
	virtual int update(unsigned int buttons);
	virtual void render();
	
	static BKPopup* create(int m, string t);	
};

#endif

