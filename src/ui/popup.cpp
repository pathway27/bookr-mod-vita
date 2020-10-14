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

#include "../graphics/fzscreen_defs.h"
#include "popup.hpp"

#ifdef __vita__
  #include <psp2/kernel/threadmgr.h> 
  #include <vita2d.h>
#else
  #include <glm/glm.hpp>
  #include <glm/gtc/matrix_transform.hpp>
  #include <glm/gtc/type_ptr.hpp>
  #include "../graphics/shader.hpp"
  #include "../resource_manager.hpp"
#endif

#include <cstring>
#include <iostream>


namespace bookr {

Popup::Popup(int m, string t) : mode(m), text(t)
{

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

  // back
  drawRectangle(Screen::WIDTH * 0.15, Screen::HEIGHT * 0.15, Screen::WIDTH * 0.70, Screen::HEIGHT * 0.80, bg1);

  // title
  drawRectangle(Screen::WIDTH * 0.16, Screen::HEIGHT * 0.17, Screen::WIDTH * 0.68, Screen::HEIGHT * 0.05, bg2);
  drawText(Screen::WIDTH * 0.17, Screen::HEIGHT * 0.18, fg, 1.0f, title.c_str());

	// body
  drawText(Screen::WIDTH * 0.17, Screen::HEIGHT * 0.25, fg, 1.0f, text.c_str());
}

Popup* Popup::create(int m, string t) {
	Popup* f = new Popup(m, t);
	Screen::resetReps();
	return f;
}

}
