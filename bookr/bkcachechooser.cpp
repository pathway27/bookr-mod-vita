/*
 * Bookr: document reader for the Sony PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com)
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com)
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

#include <pspiofilemgr.h>
#include <pspthreadman.h>

#include <string.h>
#include <malloc.h>

#include "fzscreen.h"
#include "bkcachechooser.h"
#include "bkuser.h"
#include "bklogo.h"

BKCacheChooser::BKCacheChooser(string& t, int r) :
	title(t), ret(r), mode(0), lastSelItem(0) {
	cachePath = FZScreen::basePath();
	cachePath += "/CACHE";
	yesnoItems.clear();
	yesnoItems.push_back(BKMenuItem("Yes", "Select", 0));
	yesnoItems.push_back(BKMenuItem("No", "Select", 0));
	buildCacheMenu();
}

BKCacheChooser::~BKCacheChooser() {
}

void BKCacheChooser::getFullPath(string& s) {
	s = cachePath + "/" + curItem + "/_hhc.htm";
}

void BKCacheChooser::buildCacheMenu() {
	cacheItems.clear();

	vector<FZDirent> dirFiles;

	FZScreen::dirContents((char*) cachePath.c_str(), dirFiles);
	int n = dirFiles.size();
	for (int i = 0; i < n; i++) {
		if (dirFiles[i].stat & FZ_STAT_IFDIR) {
			cacheItems.push_back(BKMenuItem(dirFiles[i].name, "Open", 0));
		}
	}

	cacheItems.push_back(BKMenuItem("Clear cache", "Select", 0));
}

int BKCacheChooser::update(unsigned int buttons) {
	if (mode == MODE_DELETECACHE) {
		return updateDeleteCache(buttons);
	} else if (mode == MODE_CLEARCACHE) {
		return updateClearCache(buttons);
	}
	return updateCache(buttons);
}

int BKCacheChooser::updateCache(unsigned int buttons) {
	menuCursorUpdate(buttons, cacheItems.size());

	int* b = FZScreen::ctrlReps();

	if (b[BKUser::controls.select] == 1) {
		if (selItem == cacheItems.size() - 1) {
			lastSelItem = selItem;
			selItem = 0;
			mode = MODE_CLEARCACHE;
			return BK_CMD_MARK_DIRTY;
		} else {
			curItem = cacheItems[selItem].label;
			return ret;
		}
	}

	if (b[BKUser::controls.cancel] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}

	if (b[FZ_REPS_START] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}

	if (b[FZ_REPS_TRIANGLE] == 1) {
		if (selItem < cacheItems.size() - 1) {
			curItem = cacheItems[selItem].label;
			lastSelItem = selItem;
			selItem = 0;
			mode = MODE_DELETECACHE;
			return BK_CMD_MARK_DIRTY;
		}
	}

	return 0;
}

int BKCacheChooser::updateDeleteCache(unsigned int buttons) {
	menuCursorUpdate(buttons, yesnoItems.size());

	int* b = FZScreen::ctrlReps();

	if (b[BKUser::controls.select] == 1) {
		if (selItem == 0) {
			BKLogo::show("Deleting cache: " + curItem);
			// delete cache
			string toDelete = cachePath + "/" + curItem;
			recursiveDelete(toDelete.c_str());
			buildCacheMenu();
		}
		selItem = lastSelItem;
		mode = MODE_CACHE;
		return BK_CMD_MARK_DIRTY;
	}

	if (b[BKUser::controls.cancel] == 1) {
		selItem = lastSelItem;
		mode = MODE_CACHE;
		return BK_CMD_MARK_DIRTY;
	}

	if (b[FZ_REPS_START] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}

	return 0;
}

int BKCacheChooser::updateClearCache(unsigned int buttons) {
	menuCursorUpdate(buttons, yesnoItems.size());

	int* b = FZScreen::ctrlReps();

	if (b[BKUser::controls.select] == 1) {
		if (selItem == 0) {
			// remove all caches
//			BKLogo::show("Clearing cache...");
//			recursiveDelete(cachePath.c_str());

			vector<FZDirent> dirFiles;

			FZScreen::dirContents((char*) cachePath.c_str(), dirFiles);
			int n = dirFiles.size();
			for (int i = 0; i < n; i++) {
				if (dirFiles[i].stat & FZ_STAT_IFDIR) {
					string toDelete = cachePath + "/" + dirFiles[i].name;
					BKLogo::show("Deleting cache: " + dirFiles[i].name);
					recursiveDelete(toDelete.c_str());
				}
			}

			return BK_CMD_CLOSE_TOP_LAYER;
		} else {
			selItem = lastSelItem;
			mode = MODE_CACHE;
			return BK_CMD_MARK_DIRTY;
		}
	}

	if (b[BKUser::controls.cancel] == 1) {
		selItem = lastSelItem;
		mode = MODE_CACHE;
		return BK_CMD_MARK_DIRTY;
	}

	if (b[FZ_REPS_START] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}

	return 0;
}

void BKCacheChooser::render() {

	if (mode == MODE_DELETECACHE) {
		string title("Are you sure to delete " + curItem + "?");
		string t = "";
		drawMenu(title, t, yesnoItems);
	} else if (mode == MODE_CLEARCACHE) {
		string title("Are you sure to clear all caches?");
		string t = "";
		drawMenu(title, t, yesnoItems);
	} else {
		string title("Cache");
		string t = "Delete cache";
		drawMenu(title, t, cacheItems);
	}
}

BKCacheChooser* BKCacheChooser::create(string& t, int r) {
	BKCacheChooser* f = new BKCacheChooser(t, r);
	FZScreen::resetReps();
	return f;
}

void BKCacheChooser::recursiveDelete(const char *dir) {
	SceUID fd;
	SceIoDirent *findData;
	findData = (SceIoDirent*)memalign(16, sizeof(SceIoDirent));
	memset((void*)findData, 0, sizeof(SceIoDirent));
	fd = sceIoDopen(dir);
	if (fd < 0)
		return;
	while (sceIoDread(fd, findData) > 0) {
		if (findData->d_name[0] != 0 && findData->d_name[0] != '.') {
			string fullname(dir);
			fullname += "/";
			fullname += findData->d_name;
			if (findData->d_stat.st_mode & FZ_STAT_IFDIR) {
				recursiveDelete(fullname.c_str());
			} else {
				sceIoRemove(fullname.c_str());
			}
		}
	}
	sceIoDclose(fd);
	free(findData);
	sceIoRmdir(dir);
}

