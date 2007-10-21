/*
 * Bookr: document reader for the Sony PSP 
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com)
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
#include "bkdocument.h"
#include "bkfilechooser.h"
#include "bkcolorchooser.h"
#include "bkpagechooser.h"
#include "bkcolorschememanager.h"
#include "bkmainmenu.h"
#include "bklogo.h"
#include "bkpopup.h"

#ifdef PSP
#include <pspkernel.h>
PSP_MODULE_INFO("Bookr", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
#endif

extern "C" {
	int main(int argc, char* argv[]);
};

static bool isTTF(string& file) {
	char header[4];
	memset((void*)header, 0, 4);
	FILE* f = fopen(file.c_str(), "r");
	fread(header, 4, 1, f);
	fclose(f);
	return header[0] == 0x0 && header[1] == 0x1 && header[2] == 0x0 && header[3] == 0x0;
}

int main(int argc, char* argv[]) {
	BKDocument* documentLayer = 0;
	FZScreen::setupCallbacks();
	FZScreen::open(argc, argv);
	FZScreen::setupCtrl();
	BKUser::init();
	FZScreen::setSpeed(BKUser::options.pspMenuSpeed);
	FZ_DEBUG_SCREEN_INIT

	BKLayer::load();
	bkLayers layers;
	BKFileChooser* fs = 0;
	BKColorChooser* cs = 0;
	BKColorSchemeManager* csm = 0;
	BKPageChooser* ps = 0;
	BKMainMenu* mm = BKMainMenu::create();
	layers.push_back(BKLogo::create());
	layers.push_back(mm);

	FZScreen::dcacheWritebackAll();


	if( BKUser::options.loadLastFile )
	{
		string s = BKBookmarksManager::getLastFile();
		if( s.substr(0,5) == "ms0:/" )
		{
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
				documentLayer = BKDocument::create(s);
				if (documentLayer == 0) {
					// error, back to logo screen
					BKLogo* l = BKLogo::create();
					l->setError(true);
					layers.push_back(l);
					FZScreen::swapBuffers();
				} else {
					// file loads ok, add the layer
					layers.push_back(documentLayer);
				}
		}
	}

	FZScreen::setSpeed(BKUser::options.pspSpeed);

	bool dirty = true;
//	bool vdirty = true;
	bool exitApp = false;
	int reloadTimer = 0;
	while (!exitApp) {
//		if(vdirty) {
//			FZScreen::startDirectList();
//			// render layers back to front
//			bkLayersIt it(layers.begin());
//			bkLayersIt end(layers.end());
//			while (it != end) {
//				(*it)->update(0);
//				(*it)->render();
//				++it;
//			}
//			FZScreen::endAndDisplayList();
//		} else 
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

		// the last layer always owns the input focus
		bkLayersIt it(layers.end());
		--it;
		int command = 0;
		// the PSP hangs when doing file I/O just after a power resume, delay it a few vsyncs
		if (reloadTimer <= 0)
			command = (*it)->update(buttons);
		if (command == BK_CMD_RELOAD) {
			reloadTimer = 60;
		}
		if (reloadTimer > 0) {
			reloadTimer--;
			if (reloadTimer == 0)
				command = BK_CMD_RELOAD;
		}
		// dont proc events while in the reload timer
		if (reloadTimer > 0)
			continue;
		switch (command) {
			case BK_CMD_CLOSE_TOP_LAYER_RELOAD:
			case BK_CMD_CLOSE_TOP_LAYER: {
				bkLayersIt it(layers.end());
				--it;
				(*it)->release();
				layers.erase(it);

				if (command == BK_CMD_CLOSE_TOP_LAYER_RELOAD) {
					// repaint
					dirty = true;                                        
				}
			} break;
			case BK_CMD_MARK_DIRTY:
				dirty = true;
				mm->rebuildMenu();
			break;
			case BK_CMD_INVOKE_OPEN_FILE: {
				// add a file chooser layer
				string title("Select document to open");
				fs = BKFileChooser::create(title, BK_CMD_OPEN_FILE);
				layers.push_back(fs);
			} break;
			case BK_CMD_INVOKE_OPEN_FONT: {
				// add a file chooser layer
				string title("Select font file to open");
				fs = BKFileChooser::create(title, BK_CMD_SET_FONT);
				layers.push_back(fs);
			} break;
			case BK_CMD_INVOKE_PAGE_CHOOSER: {
				if (documentLayer && documentLayer->isPaginated()) {
					ps = BKPageChooser::create(documentLayer->getTotalPages(), 
							documentLayer->getCurrentPage(),
							BK_CMD_OPEN_PAGE);
					layers.push_back(ps);
				}
			} break;
			case BK_CMD_OPEN_PAGE: {
				if (documentLayer && documentLayer->isPaginated()) {
					int pagenum = ps->getCurrentPage();
					int r = documentLayer->setCurrentPage(pagenum);
					if (r != 0)
						command = r;
				}
				bkLayersIt it(layers.end());
				--it;
				(*it)->release();
				layers.erase(it);
			} break;
			case BK_CMD_OPEN_FILE:
			case BK_CMD_RELOAD: {
				// open a file as a document
				string s;
				
				FZScreen::setSpeed(BKUser::options.pspMenuSpeed);

				if (command == BK_CMD_RELOAD) {
					documentLayer->getFileName(s);
					documentLayer = 0;
				}
				if (command == BK_CMD_OPEN_FILE) {
					// open selected file
					fs->getFullPath(s);
					fs = 0;
				}

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
				documentLayer = BKDocument::create(s);
				if (documentLayer == 0) {
					// error, back to logo screen
					BKLogo* l = BKLogo::create();
					l->setError(true);
					layers.push_back(l);
				} else {
					// file loads ok, add the layer
					layers.push_back(documentLayer);
				}

				FZScreen::setSpeed(BKUser::options.pspSpeed);

				dirty = true;
			} break;
			case BK_CMD_SET_FONT: {
				// change font for plain text rendering
				string s;
				FZDirent de;
				// open selected file
				fs->getCurrentDirent(de);
				fs->getFullPath(s);
				// detect file type and display error if needed
				if (!isTTF(s)) {
					layers.push_back(BKPopup::create(
							BKPOPUP_ERROR,
							"The selected file is not a valid TrueType font."
						)
					);
					break;
				}
				// file ok
				bkLayersIt it(layers.end());
				--it;
				(*it)->release();
				layers.erase(it);
				fs = 0;
				BKUser::options.txtFont = s;
				BKUser::save();
				mm->rebuildMenu();
			} break;
			case BK_CMD_INVOKE_MENU:
				// add a main menu layer
				mm = BKMainMenu::create();
				layers.push_back(mm);
			break;
			case BK_CMD_MAINMENU_POPUP:
				layers.push_back(BKPopup::create(
						mm->getPopupMode(),
						mm->getPopupText()
					)
				);
			break;
			case BK_CMD_INVOKE_COLOR_SCHEME_MANAGER: {//TXTFG:
				// add a color scheme manager layer
				string title("Manage color schemes");
				csm = BKColorSchemeManager::create(title);
				layers.push_back(csm);
			} break;
			case BK_CMD_INVOKE_COLOR_CHOOSER_TXTFG:
				// add a color chooser layer
				cs = BKColorChooser::create(csm->getColorScheme(), BK_CMD_SET_TXTFG);
				layers.push_back(cs);
			break;

			case BK_CMD_SET_TXTFG: {
				BKUser::options.colorSchemes[csm->getColorScheme()].txtFGColor = cs->getColor();
				BKUser::save();
				mm->rebuildMenu();
				bkLayersIt it(layers.end());
				--it;
				(*it)->release();
				layers.erase(it);
				
				// After choosing the foreground color, choose the background one
				cs = BKColorChooser::create(csm->getColorScheme(), BK_CMD_SET_TXTBG);
				layers.push_back(cs);
				
			} break;
			case BK_CMD_SET_TXTBG: {
				BKUser::options.colorSchemes[csm->getColorScheme()].txtBGColor = cs->getColor();
				BKUser::save();
				mm->rebuildMenu();
				bkLayersIt it(layers.end());
				--it;
				(*it)->release();
				layers.erase(it);
			} break;
			case BK_CMD_EXIT: {
					FZScreen::setSpeed(BKUser::options.pspMenuSpeed);
					exitApp = true;
			}
			break;
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

