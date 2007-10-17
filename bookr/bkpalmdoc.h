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

#ifndef BKPALMDOC_H
#define BKPALMDOC_H

#include "fzscreen.h"

using namespace std;

#include "bkfancytext.h"

class BKPalmDoc : public BKFancyText {
	private:
	string fileName;
	string title;
	char* buffer;

	protected:
	BKPalmDoc();
	~BKPalmDoc();

	public:
	virtual void getFileName(string&);
	virtual void getTitle(string&);
	virtual void getType(string&);

	static BKPalmDoc* create(string& file);
	static bool isPalmDoc(string& file);
};

#endif

