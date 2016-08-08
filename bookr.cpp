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
#include "bkzoomin.h"
#include "bkoutline.h"

#ifdef PSP
#include <pspkernel.h>

#ifdef FW150
PSP_MODULE_INFO("Bookr150", PSP_MODULE_KERNEL, 2, 0);
#else
PSP_MODULE_INFO("Bookr3xx", PSP_MODULE_USER, 2, 0);
#endif
PSP_HEAP_SIZE_MAX();
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
	BKZoomIn* zi = 0;
	BKOutline* outlineLayer = 0;
	BKMainMenu* mm = BKMainMenu::create();
	layers.push_back(BKLogo::create());
	layers.push_back(mm);
	
	
	char tt[5] = {'\0','\0','\0', '\0','\0'};
	snprintf(tt,5,"%d",BKUser::options.maxTreeHeight);
	setenv("MAXTREELEVEL",tt,1);

#ifdef FW150
	if(BKUser::options.screenBrightness>0){
	  FZScreen::setBrightness(BKUser::options.screenBrightness>100?100:BKUser::options.screenBrightness);
	}
#endif

	switch (BKUser::options.pdfImageQuality){
	case 0:
	  setenv("BOOKR_MAX_IMAGE_SCALE_DENOM","1",1);
	  break;
	case 1:
	  setenv("BOOKR_MAX_IMAGE_SCALE_DENOM","2",1);
	  break;
	case 2:
	  setenv("BOOKR_MAX_IMAGE_SCALE_DENOM","4",1);
	  break;
	case 3:
	default:
	  setenv("BOOKR_MAX_IMAGE_SCALE_DENOM","8",1);
	  break;
	}
	
	char tt3[3] = {'\0','\0','\0'};
	snprintf(tt3,3,"%d",BKUser::options.pdfImageBufferSizeM);
	setenv("BOOKR_MAX_IMAGE_BUFFER_SIZE_M",tt3,1);

	if (BKUser::options.pdfOptimizeForSmallImages)
	  setenv("BOOKR_IMAGE_SCALE_DENOM_ALWAYS_MAX","1",1);
	else
	  setenv("BOOKR_IMAGE_SCALE_DENOM_ALWAYS_MAX","0",1);
	
	if(BKUser::options.autoPruneBookmarks){
	  BKBookmarksManager::prune();
	}

	extern int jpc_decode_mode;
	jpc_decode_mode = BKUser::options.jpeg2000Decoder?1:0;

	FZScreen::dcacheWritebackAll();

	/* we take the first command line argument as filename, and ignore other ones */
#ifdef PSP
	if( argc > 1 ) {
#else
	if( argc > 1 && access(argv[1],R_OK) != -1 ) {
#endif
	  string s(argv[1]);

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
	  documentLayer = BKDocument::create(s,s);
	  if ((int)documentLayer == 0 || (int)documentLayer == -1) {
	    // error, back to logo screen
	    BKLogo* l = BKLogo::create();
	    l->setError(true);
	    l->setErrorType(-1*(int)documentLayer);
	    layers.push_back(l);
	    FZScreen::swapBuffers();
	    documentLayer = 0;
	  } else {
	    // file loads ok, add the layer
	    layers.push_back(documentLayer);
	  }
	}


	else if( BKUser::options.loadLastFile )
	{
	  string s;
	  string lfn;
	  BKBookmarksManager::getLastFile(s,lfn);
	    if (lfn.length() == 0)
	      lfn = s;
		  if( s.substr(0,5) == "ms0:/" || s.substr(0,1) == "/" )
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
				  documentLayer = BKDocument::create(s,lfn);
				  if ((int)documentLayer == 0||(int)documentLayer == -1||(int)documentLayer == -2) {
					// error, back to logo screen
					BKLogo* l = BKLogo::create();
					l->setError(true);
					l->setErrorType(-1*(int)documentLayer);
					layers.push_back(l);
					FZScreen::swapBuffers();
					documentLayer = 0;
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
//	int reloadTimer = 0;
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
//		if (reloadTimer <= 0)
		command = (*it)->update(buttons);
// 		if (command == BK_CMD_RELOAD) {
// 			reloadTimer = 60;
// 		}
// 		if (reloadTimer > 0) {
// 			reloadTimer--;
// 			if (reloadTimer == 0)
// 				command = BK_CMD_RELOAD;
// 		}
// 		// dont proc events while in the reload timer
// 		if (reloadTimer > 0)
// 			continue;
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
				break;
			case BK_CMD_MARK_MENU_DIRTY:
				dirty = true;
				mm->rebuildMenu();
				if(documentLayer){
				  documentLayer->buildToolbarMenus();
				}
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
			case BK_CMD_INVOKE_ZOOM_IN: {
			  if (documentLayer && documentLayer->isZoomable()) {
			    documentLayer->setMode(BKDOC_VIEW);
			    zi = BKZoomIn::create(79,399,BK_CMD_ZOOM_IN);
			    layers.push_back(zi);
			  }
			} break;
			case BK_CMD_ZOOM_IN: {
				if (documentLayer && documentLayer->isZoomable()) {
				  int r = documentLayer->setZoomIn(zi->getLeftPos(), zi->getRightPos());
					if (r != 0)
						command = r;
				}
				bkLayersIt it(layers.end());
				--it;
				(*it)->release();
				layers.erase(it);
			  
			} break;
			case BK_CMD_INVOKE_OUTLINES: {
			  if (documentLayer && documentLayer->getOutlineType()) {
			    int top,sel;
			    documentLayer->getOutlineSelection(top,sel);
			    outlineLayer = BKOutline::create("Outlines", BK_CMD_OUTLINE_GOTO, top, sel);
			    outlineLayer->setOutlines(documentLayer->getOutlineType(), documentLayer->getOutlines());
			    layers.push_back(outlineLayer);
			  }
			} break;
			case BK_CMD_OUTLINE_GOTO: {
			  if(outlineLayer){
			    int top, sel;
			    outlineLayer->getSelection(top,sel);
			    documentLayer->setOutlineSelection(top,sel);
			    documentLayer->gotoOutline(outlineLayer->getSelectedOutline(), outlineLayer->ignoreZoom);
			    dirty = true;
			  }
			  bkLayersIt it(layers.end());
			  --it;
			  (*it)->release();
			  layers.erase(it);
			  documentLayer->setMode(BKDOC_VIEW);
			} break;
			case BK_CMD_OPEN_FILE:
			case BK_CMD_RELOAD: {
				// open a file as a document
				string s;
				string lfn;
				
				FZScreen::setSpeed(BKUser::options.pspMenuSpeed);
				if (command == BK_CMD_RELOAD) {
					documentLayer->getFileName(s);
					documentLayer = 0;
				}
				if (command == BK_CMD_OPEN_FILE) {
					// open selected file
					fs->getFullPath(s);
					fs->getLongFileName(lfn);
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
				documentLayer = BKDocument::create(s,lfn);
				if ((int)documentLayer == 0||(int)documentLayer == -1) {
					// error, back to logo screen
					BKLogo* l = BKLogo::create();
					l->setError(true);
					l->setErrorType(-1*(int)documentLayer);
					layers.push_back(l);
					documentLayer = 0;
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

		        case BK_CMD_INVOKE_THUMBNAIL_COLOR_SCHEME_MANAGER: {//TNFG:
			    // add a thumbnail color scheme manager layer
			    string title("Manage thumbnail color schemes");
			    csm = BKColorSchemeManager::create(title, BK_CMD_INVOKE_THUMBNAIL_COLOR_SCHEME_MANAGER);
			    layers.push_back(csm);
			} break;

			
			case BK_CMD_INVOKE_COLOR_CHOOSER_TXTFG:
				// add a color chooser layer
				cs = BKColorChooser::create(csm->getColorScheme(), BK_CMD_SET_TXTFG);
				layers.push_back(cs);
			break;

		        case BK_CMD_INVOKE_COLOR_CHOOSER_TNFG:
			    // add a thumbnail color chooser layer
			    cs = BKColorChooser::create(csm->getColorScheme(), BK_CMD_SET_TNFG);
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

			case BK_CMD_SET_TNFG: {
				BKUser::options.thumbnailColorSchemes[csm->getColorScheme()].txtFGColor = cs->getColor();
				BKUser::save();
				mm->rebuildMenu();
				bkLayersIt it(layers.end());
				--it;
				(*it)->release();
				layers.erase(it);
				
				// After choosing the foreground color, choose the background one
				cs = BKColorChooser::create(csm->getColorScheme(), BK_CMD_SET_TNBG);
				layers.push_back(cs);
				
			} break;
			case BK_CMD_SET_TNBG: {
				BKUser::options.thumbnailColorSchemes[csm->getColorScheme()].txtBGColor = cs->getColor();
				BKUser::save();
				mm->rebuildMenu();
				bkLayersIt it(layers.end());
				--it;
				(*it)->release();
				layers.erase(it);
			} break;

			case BK_CMD_OPEN_LAST_FILE: {
			  string s;
			  string lfn;
			  FZScreen::setSpeed(BKUser::options.pspMenuSpeed);
			  BKBookmarksManager::getLastFile(s,lfn);
			  if (lfn.length() == 0)
			    lfn = s;
			  if( s.substr(0,5) == "ms0:/" || s.substr(0,1) == "/" )
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
			      documentLayer = BKDocument::create(s,lfn);
			      if ((int)documentLayer == 0||(int)documentLayer == -1||(int)documentLayer == -2) {
				// error, back to logo screen
				BKLogo* l = BKLogo::create();
				l->setError(true);
				l->setErrorType(-1*(int)documentLayer);
				layers.push_back(l);
				FZScreen::swapBuffers();
				documentLayer = 0;
			      } else {
				// file loads ok, add the layer
				layers.push_back(documentLayer);
			      }
			    }
			  FZScreen::setSpeed(BKUser::options.pspSpeed);
			  dirty = true;
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
	BKBookmarksManager::free();
	BKLayer::unload();
	FZScreen::close();
	FZScreen::exit();
	return 0;
}

