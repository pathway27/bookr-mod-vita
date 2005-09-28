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

#include "fzscreen.h"
#include "bkbook.h"
#include "bkpdf.h"
#include "bkfilechooser.h"
#include "bkmainmenu.h"
#include "bklogo.h"

#ifdef PSP
#include <pspkernel.h>
PSP_MODULE_INFO("Bookr", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

extern "C" {
	int main(int argc, char* argv[]);
};

static bool isPDF(string& file) {
	char header[4];
	memset((void*)header, 0, 4);
	FILE* f = fopen(file.c_str(), "r");
	fread(header, 4, 1, f);
	fclose(f);
	return header[0] == 0x25 && header[1] == 0x50 && header[2] == 0x44 && header[3] == 0x46;
}

int main(int argc, char* argv[]) {
	FZScreen::setupCallbacks();
	FZScreen::open(argc, argv);
	FZScreen::setupCtrl();
	BKUser::init();
	FZ_DEBUG_SCREEN_INIT

	BKLayer::load();
	bkLayers layers;
	BKFileChooser* fs = 0;
	BKMainMenu* mm = BKMainMenu::create();
	layers.push_back(BKLogo::create());
	layers.push_back(mm);

	FZScreen::dcacheWritebackAll();

	bool dirty = true;
	for (;;) {
		if (dirty) {
			FZScreen::startDirectList();
			// render layers back to front
			bkLayersIt it(layers.begin());
			bkLayersIt end(layers.end());
			while (it != end) {
				(*it)->render();
				++it;
			}
			FZScreen::endAndDisplayList();
		}

		FZScreen::waitVblankStart();

		if (dirty) {
			FZScreen::swapBuffers();
		}

		FZScreen::checkEvents();

		FZ_DEBUG_SCREEN_SET00

		int buttons = FZScreen::readCtrl();
		dirty = buttons != 0;

		// hack!
		/*if (buttons & FZ_CTRL_SQUARE)
			break;*/

		// the last layer always owns the input focus
		bkLayersIt it(layers.end());
		--it;
		int command = (*it)->update(buttons);
		switch (command) {
			case BK_CMD_CLOSE_TOP_LAYER: {
				bkLayersIt it(layers.end());
				--it;
				(*it)->release();
				layers.erase(it);
			} break;
			case BK_CMD_MARK_DIRTY:
				dirty = true;
			break;
			case BK_CMD_INVOKE_OPEN_FILE:
				// add a file chooser layer
				fs = BKFileChooser::create();
				layers.push_back(fs);
			break;
			case BK_CMD_OPEN_FILE: {
				// open a file as a document
				// get selected file
				FZDirent de;
				fs->getCurrentDirent(de);
				string s;
				fs->getFullPath(s);
				fs = 0;
				// clear layers
				bkLayersIt it(layers.begin());
				bkLayersIt end(layers.end());
				while (it != end) {	
					(*it)->release();
					++it;
				}
				layers.clear();
				// little hack to display a loading screen
				BKLogo* l = BKLogo::create();
				l->setLoading(true);
				FZScreen::startDirectList();
				l->render();
				FZScreen::endAndDisplayList();
				FZScreen::waitVblankStart();
				FZScreen::swapBuffers();
				FZScreen::checkEvents();
				l->release();
				// detect file type and add a new display layer
				if (isPDF(s)) {
					layers.push_back(BKPDF::create(s));
				} else {
					layers.push_back(BKBook::create(s, de.size));
				}
			} break;
			case BK_CMD_INVOKE_MENU:
				// add a main menu layer
				mm = BKMainMenu::create();
				layers.push_back(mm);
			break;
			//BK_CMD_OPEN_MENU
			//BK_CMD_OPEN_FONT
			//BK_CMD_INVOKE_OPEN_FONT
		}
	}

	bkLayersIt it(layers.begin());
	bkLayersIt end(layers.end());
	while (it != end) {	
		(*it)->release();
		++it;
	}
	layers.clear();

	FZScreen::close();
	FZScreen::exit();

	return 0;
}

