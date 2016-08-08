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

#include <string.h>
#include "fzscreen.h"

#include "bkfilechooser.h"
#include "bkuser.h"

BKFileChooser::BKFileChooser(string& t, int r) : title(t), ret(r) {
  if( r == BK_CMD_SET_FONT ){
    path = BKUser::options.lastFontFolder;
    lpath = BKUser::options.lastFontFolder;
  }
  else{
    path = BKUser::options.lastFolder;
    lpath = BKUser::options.lastFolder;
  }
	updateDirFiles();
}

BKFileChooser::~BKFileChooser() {
		BKUser::save();
}

void BKFileChooser::getCurrentDirent(FZDirent& de) {
	de = dirFiles[selItem];
}

void BKFileChooser::getFullPath(string& s) {
  if ( path.c_str()[(path.length()-1)] == '/' ){
    s = path + dirFiles[selItem].sname;
  }
  else{
    s = path + "/" + dirFiles[selItem].sname;
  }
}

void BKFileChooser::getLongFileName(string& s){

    s = dirFiles[selItem].name;

}

void BKFileChooser::updateDirFiles() {

	dirFiles.clear();
	items.clear();
	#define MAX_SHORT_PATH_LENGTH 512
	char* spath = (char*) malloc(MAX_SHORT_PATH_LENGTH);
	if(!spath){
	  dirFiles.push_back(FZDirent("<Cannot open this folder>", 0, 0));
	  return;
	}
	memset(spath,'\0',MAX_SHORT_PATH_LENGTH);
	strncpy(spath,path.c_str(),MAX_SHORT_PATH_LENGTH - 1);

	int err = FZScreen::dirContents(lpath.c_str(), spath, dirFiles);
	if (err < 0) {
	  //path = FZScreen::basePath();
		lpath = FZScreen::basePath();
		strncpy(spath, lpath.c_str(),MAX_SHORT_PATH_LENGTH -1 );
		FZScreen::dirContents(lpath.c_str(), spath, dirFiles);
	}
	path = spath;

	//	if(spath != bkup_spath)
	free(spath);
	
#ifdef PSP
	unsigned int rootPathLen = 5;
#else
	unsigned int rootPathLen = 1;
#endif

	while ( path.length() > rootPathLen && path.c_str()[(path.length()-1)] == '/' ){
	  //remove trailing '/' added by dirContents.
	  path.resize(path.length()-1);
	}
	while ( lpath.length() > rootPathLen && lpath.c_str()[(lpath.length()-1)] == '/' ){
	  //remove trailing '/' added by dirContents.
	  lpath.resize(lpath.length()-1);
	}

	if (dirFiles.size() == 0)
		dirFiles.push_back(FZDirent("<Empty folder>", 0, 0));
	if( ret == BK_CMD_SET_FONT )
		BKUser::options.lastFontFolder = path;
	else
		BKUser::options.lastFolder = path;

}

int BKFileChooser::update(unsigned int buttons) {
	menuCursorUpdate(buttons, (int)dirFiles.size());
	int* b = FZScreen::ctrlReps();
	if (b[BKUser::controls.select] == 1) {
		//printf("selected %s\n", dirFiles[selItem].name.c_str());
		if (dirFiles[selItem].stat & FZ_STAT_IFDIR ) {
		  if ( path.c_str()[(path.length()-1)] == '/' ){
		    path += dirFiles[selItem].sname;
		    lpath += dirFiles[selItem].name;
		  }
		  else{
			path += "/" + dirFiles[selItem].sname;
			lpath += "/" + dirFiles[selItem].name;
		  }
			selItem = 0;
			topItem = 0;
			skipChars = 0;
			maxSkipChars = -1;
			updateDirFiles();
		} else if (dirFiles[selItem].stat & FZ_STAT_IFREG) {
			return ret;
		}
	}
	if (b[BKUser::controls.alternate] == 1) {
		// try to remove one dir
		int lastSlash = path.rfind('/');
		if (lastSlash != -1) {
			path.resize(lastSlash);
		}
		int llastSlash = lpath.rfind('/');
		if (llastSlash != -1) {
			lpath.resize(llastSlash);
		}
#ifdef PSP
		if (path == "" || path == "ms0:"){
			path = "ms0:/";
			lpath = "ms0:/";
		}
#else
		if (path == ""){
			path = "/";
			lpath = "/";
		}
#endif
		selItem = 0;
		topItem = 0;
		skipChars = 0;
		maxSkipChars = -1;
		updateDirFiles();
	}
	if (b[BKUser::controls.cancel] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}
	if (b[BKUser::controls.showMainMenu] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}
	return 0;
}

void BKFileChooser::render() {
  if(items.empty()){
	int n = dirFiles.size();
	for (int i = 0; i < n; i++) {
		string cl("Select file");
		int f = 0;
		if (dirFiles[i].stat & FZ_STAT_IFDIR) {
			cl = "Open folder";
			f = BK_MENU_ITEM_FOLDER;
		}
		else if(dirFiles[i].stat == 0){
		  cl = "";
		}
		items.push_back(BKMenuItem(dirFiles[i].name, cl, f)); 
	}
  }
	string tl("Parent folder");
	drawMenu(title, tl, items, true);
}

BKFileChooser* BKFileChooser::create(string& t, int r) {
	BKFileChooser* f = new BKFileChooser(t, r);
	FZScreen::resetReps();
	return f;
}

