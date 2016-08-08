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

#ifndef BKMAINMENU_H
#define BKMAINMENU_H

#include "fzscreen.h"

using namespace std;

#include "bklayer.h"

class BKMainMenu : public BKLayer {
	#define BKMM_MAIN 0
	#define BKMM_CONTROLS 1
	#define BKMM_OPTIONS 2

	int mode;
	bool captureButton;
	vector<BKMenuItem> mainItems;
	vector<BKMenuItem> controlItems;
	vector<BKMenuItem> optionItems;
	int updateMain(unsigned int buttons);
	int updateControls(unsigned int buttons);
	int updateOptions(unsigned int buttons);
	void buildMainMenu();
	void buildControlMenu();
	void buildOptionMenu();

	string popupText;
	int popupMode;
	int frames;

	protected:
	BKMainMenu();	
	~BKMainMenu();

	public:
	virtual int update(unsigned int buttons);
	virtual void render();
	
	static BKMainMenu* create();	

	string getPopupText();
	int getPopupMode();
	void rebuildMenu();

	typedef vector<BKMenuItem>::iterator bkMenuItemIt;
	
};

#endif

