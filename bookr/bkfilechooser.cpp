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
	if( r == BK_CMD_SET_FONT )
		path = BKUser::options.lastFontFolder;
	else
		path = BKUser::options.lastFolder;
	updateDirFiles();
}

BKFileChooser::~BKFileChooser() {
		BKUser::save();
}

void BKFileChooser::getCurrentDirent(FZDirent& de) {
	de = dirFiles[selItem];
}

void BKFileChooser::getFullPath(string& s) {
	s = path + "/" + dirFiles[selItem].name;
}

void BKFileChooser::updateDirFiles() {
	dirFiles.clear();
	int err = FZScreen::dirContents((char*)path.c_str(), dirFiles);
	if (err < 0) {
		path = FZScreen::basePath();
		FZScreen::dirContents((char*)path.c_str(), dirFiles);
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
			path += "/" + dirFiles[selItem].name;
			selItem = 0;
			topItem = 0;
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
#ifdef PSP
		if (path == "")
			path = "ms0:/";
#else
		if (path == "")
			path = "/";
#endif
		selItem = 0;
		topItem = 0;
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
	vector<BKMenuItem> items;
	int n = dirFiles.size();
	for (int i = 0; i < n; i++) {
		string cl("Select file");
		int f = 0;
		if (dirFiles[i].stat & FZ_STAT_IFDIR) {
			cl = "Open folder";
			f = BK_MENU_ITEM_FOLDER;
		}
		items.push_back(BKMenuItem(dirFiles[i].name, cl, f)); 
	}
	string tl("Parent folder");
	drawMenu(title, tl, items);
}

BKFileChooser* BKFileChooser::create(string& t, int r) {
	BKFileChooser* f = new BKFileChooser(t, r);
	FZScreen::resetReps();
	return f;
}

