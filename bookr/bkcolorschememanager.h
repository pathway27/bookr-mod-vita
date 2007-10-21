/*
 * Bookr: document reader for the Sony PSP 
 * Copyright (C) 2007 Christian Payeur (christian dot payeur at gmail dot com)
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

#ifndef BKCOLORSCHEMEMANAGER_H
#define BKCOLORSCHEMEMANAGER_H

#include "fzscreen.h"

using namespace std;

#include "bklayer.h"



class BKColorSchemeManager : public BKLayer {
	string title;
	int	colorScheme;

	protected:
	BKColorSchemeManager(string& t);
	~BKColorSchemeManager();

	public:
	virtual int update(unsigned int buttons);
	virtual void render();
	virtual int getColorScheme();
	
	static BKColorSchemeManager* create(string& t);
	
};

#endif /*BKCOLORSCHEMEMANAGER_H*/
