/*
 * bookr-modern: a graphics based document reader 
 * Copyright (C) 2019 pathway27 (Sree)
 * IS A MODIFICATION OF THE ORIGINAL
 * Bookr and bookr-mod for PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com),
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com),
 * AND VARIOUS OTHER FORKS, See Forks in README.md
 * Licensed under GPLv3+, see LICENSE
*/

#ifndef BKMAINMENU_H
#define BKMAINMENU_H

#include "graphics/screen.hpp"
#include "layer.hpp"

using std::string;

namespace bookr {

class MainMenu : public Layer {
	#define BKMM_MAIN 0
	#define BKMM_CONTROLS 1
	#define BKMM_OPTIONS 2

	int mode;
	bool captureButton;
	vector<MenuItem> mainItems;
	vector<MenuItem> controlItems;
	vector<MenuItem> optionItems;
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
	MainMenu();	
	~MainMenu();

public:
	virtual int update(unsigned int buttons);
	virtual void render();
	
	static MainMenu* create();	

	string getPopupText();
	int getPopupMode();
	void rebuildMenu();

	typedef vector<MenuItem>::iterator MenuItemIt;
	
};

}

#endif
