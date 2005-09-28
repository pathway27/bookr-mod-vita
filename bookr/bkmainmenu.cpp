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

#include "bkmainmenu.h"

BKMainMenu::BKMainMenu() : mode(BKMM_MAIN), captureButton(false) {
	mainItems.push_back(BKMenuItem("Open file", "Select", 0));
	mainItems.push_back(BKMenuItem("Controls", "Select", 0));
	//mainItems.push_back(BKMenuItem("Options", "Select", 0));
	buildControlMenu();
}

BKMainMenu::~BKMainMenu() {
}

void BKMainMenu::buildControlMenu() {
	string cb("Change button");

	controlItems.clear();

	controlItems.push_back(BKMenuItem("Restore defaults", "Select", 0));

	string t("Plain text - Previous page: ");
	t += FZScreen::nameForButtonReps(BKUser::txtControls.previousPage);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "Plain text - Next page: ";
	t += FZScreen::nameForButtonReps(BKUser::txtControls.nextPage);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "PDF - Previous page: ";
	t += FZScreen::nameForButtonReps(BKUser::pdfControls.previousPage);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "PDF - Next page: ";
	t += FZScreen::nameForButtonReps(BKUser::pdfControls.nextPage);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "PDF - Previous 10 pages: ";
	t += FZScreen::nameForButtonReps(BKUser::pdfControls.previous10Pages);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "PDF - Next 10 pages: ";
	t += FZScreen::nameForButtonReps(BKUser::pdfControls.next10Pages);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "PDF - Pan up: ";
	t += FZScreen::nameForButtonReps(BKUser::pdfControls.panUp);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "PDF - Pan down: ";
	t += FZScreen::nameForButtonReps(BKUser::pdfControls.panDown);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "PDF - Pan left: ";
	t += FZScreen::nameForButtonReps(BKUser::pdfControls.panLeft);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "PDF - Pan right: ";
	t += FZScreen::nameForButtonReps(BKUser::pdfControls.panRight);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "PDF - Zoom in: ";
	t += FZScreen::nameForButtonReps(BKUser::pdfControls.zoomIn);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "PDF - Zoom out: ";
	t += FZScreen::nameForButtonReps(BKUser::pdfControls.zoomOut);
	controlItems.push_back(BKMenuItem(t, cb, 0));
}

int BKMainMenu::update(unsigned int buttons) {
	if (mode == BKMM_CONTROLS) {
		return updateControls(buttons);
	}
	return updateMain(buttons);
}

int BKMainMenu::updateMain(unsigned int buttons) {
	menuCursorUpdate(buttons, mainItems.size());

	int* b = FZScreen::ctrlReps();

	if (b[FZ_REPS_CIRCLE] == 1) {
		if (selItem == 0)
			return BK_CMD_INVOKE_OPEN_FILE;
		if (selItem == 1) {
			selItem = 0;
			topItem = 0;
			mode = BKMM_CONTROLS;
			return BK_CMD_MARK_DIRTY;
		}
	}

	if (b[FZ_REPS_CROSS] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}

	if (b[FZ_REPS_START] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}

	return 0;
}

static int buttonsHack = 0;
int BKMainMenu::updateControls(unsigned int buttons) {
	const int validMask = FZ_CTRL_UP | FZ_CTRL_RIGHT | FZ_CTRL_DOWN | FZ_CTRL_LEFT | FZ_CTRL_LTRIGGER |
		FZ_CTRL_RTRIGGER | FZ_CTRL_TRIANGLE | FZ_CTRL_CIRCLE | FZ_CTRL_CROSS | FZ_CTRL_SQUARE;
	if (captureButton) {
		// not pretty... wait for the mask to become 0 at least once
		if (buttons != 0 && buttonsHack == 0)
			return 0;
		buttonsHack = 1;
		// capture only non-system buttons
		if ((buttons & validMask) == 0)
			return 0;
		int repsCode = FZScreen::repsForButtonMask(buttons);
		switch (selItem) {
			case  1: BKUser::txtControls.previousPage = repsCode; break;
			case  2: BKUser::txtControls.nextPage = repsCode; break;
			case  3: BKUser::pdfControls.previousPage = repsCode; break;
			case  4: BKUser::pdfControls.nextPage = repsCode; break;
			case  5: BKUser::pdfControls.previous10Pages = repsCode; break;
			case  6: BKUser::pdfControls.next10Pages = repsCode; break;
			case  7: BKUser::pdfControls.panUp = repsCode; break;
			case  8: BKUser::pdfControls.panDown = repsCode; break;
			case  9: BKUser::pdfControls.panLeft = repsCode; break;
			case 10: BKUser::pdfControls.panRight = repsCode; break;
			case 11: BKUser::pdfControls.zoomIn = repsCode; break;
			case 12: BKUser::pdfControls.zoomOut = repsCode; break;
		}
		BKUser::save();
		buildControlMenu();
		captureButton = false;
		return BK_CMD_MARK_DIRTY;
	}

	menuCursorUpdate(buttons, controlItems.size());

	int* b = FZScreen::ctrlReps();

	if (b[FZ_REPS_CIRCLE] == 1) {
		if (selItem == 0) {
			BKUser::setDefaultControls();
			BKUser::save();
			buildControlMenu();			
			return BK_CMD_MARK_DIRTY;
		}
		buttonsHack = 0;
		captureButton = true;
		return BK_CMD_MARK_DIRTY;
	}

	if (b[FZ_REPS_CROSS] == 1) {
		selItem = 0;
		topItem = 0;
		mode = BKMM_MAIN;
		return BK_CMD_MARK_DIRTY;
	}

	if (b[FZ_REPS_START] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}

	return 0;
}

void BKMainMenu::render() {
	string t("");
	if (mode == BKMM_MAIN) {
		string title("Main Menu");
		drawMenu(title, t, mainItems);
	} else if (mode == BKMM_CONTROLS) {
		string title("Customize controls");
		drawMenu(title, t, controlItems);
	}
	if (captureButton) {
		texUI->bindForDisplay();
		FZScreen::ambientColor(0xf0dddddd);
		drawPill(150, 126, 180, 20, 6, 31, 1);
		fontBig->bindForDisplay();
		FZScreen::ambientColor(0xff222222);
		drawTextHC("Press the new button", fontBig, 130);
	}
}

BKMainMenu* BKMainMenu::create() {
	BKMainMenu* f = new BKMainMenu();
	FZScreen::resetReps();
	return f;
}

