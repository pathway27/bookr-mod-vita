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

#include <cstring>

#include "popup.hpp"

namespace bookr {

Popup::Popup(int m, string t) : mode(m), text(t) {
}

Popup::~Popup() {
}

int Popup::update(unsigned int buttons) {
	int* b = Screen::ctrlReps();

	if (b[User::controls.cancel] == 1) {
			return BK_CMD_CLOSE_TOP_LAYER;
	}

	return 0;
}

void Popup::render() {
	string title;
	int bg1 = 0;
	int bg2 = 0;
	int fg = 0;
	if (mode == BKPOPUP_WARNING) {
		bg1 = 0xf02020a0;
		bg2 = 0xf06060ff;
		fg = 0xffffffff;
		title = "Warning";
	} else if (mode == BKPOPUP_INFO) {
		bg1 = 0xf0a02020;
		bg2 = 0xf0ff6060;
		fg = 0xffffffff;
		title = "Info";
	} else {
		bg1 = 0xf02020a0;
		bg2 = 0xf06060ff;
		fg = 0xffffffff;
		title = "Error";
	}
	drawPopup(text, title, bg1, bg2, fg);
}

Popup* Popup::create(int m, string t) {
	Popup* f = new Popup(m, t);
	Screen::resetReps();
	return f;
}

}