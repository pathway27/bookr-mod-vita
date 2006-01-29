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
#include "bkpdf.h"
#include "bkbook.h"
#include "bkbookmark.h"
#include "bkpopup.h"

BKMainMenu::BKMainMenu(bool isPdeff, BKLayer* pdfOrBookLayer) : mode(BKMM_MAIN), captureButton(false) {
	mainItems.push_back(BKMenuItem("Open file", "Select", 0));
	mainItems.push_back(BKMenuItem("Controls", "Select", 0));	
	mainItems.push_back(BKMenuItem("Options", "Select", 0));
	mainItems.push_back(BKMenuItem("About", "Select", 0));
	mainItems.push_back(BKMenuItem("Exit", "Select", 0));
	buildControlMenu();
	buildOptionMenu();
}

BKMainMenu::~BKMainMenu() {
}

string BKMainMenu::getPopupText() {
	return popupText;
}

int BKMainMenu::getPopupMode() {
	return popupMode;
}

void BKMainMenu::rebuildMenu() {
	if (mode == BKMM_CONTROLS) {
		buildControlMenu();
	} else if (mode == BKMM_OPTIONS) {
		buildOptionMenu();
	}
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

void BKMainMenu::buildOptionMenu() {
	optionItems.clear();
	optionItems.push_back(BKMenuItem("Restore defaults", "Select", 0));

	string t("PDF - Fast scroll (zoom limited to 2x): ");
	t += BKUser::options.pdfFastScroll ? "Enabled" : "Disabled";
	optionItems.push_back(BKMenuItem(t, "Toggle", 0));

	t = "Plain text - Font file: ";
	if (BKUser::options.txtFont == "bookr:builtin") {
		t += "built-in";
	} else {
		int s = BKUser::options.txtFont.size();
		if (s > 25) {
			t += "...";
			t += string(BKUser::options.txtFont, s - 25, 25);
		} else {
			t += BKUser::options.txtFont;
		}
	}
	BKMenuItem mi = BKMenuItem(t, "Select font file", BK_MENU_ITEM_OPTIONAL_TRIANGLE_LABEL);
	mi.triangleLabel = "Use built-in font";
	optionItems.push_back(mi);

	char txt[1024];
	snprintf(txt, 1024, "Plain text - Font size: %d pixels", BKUser::options.txtSize);
	t = txt;
	optionItems.push_back(BKMenuItem(t, "Change", BK_MENU_ITEM_USE_LR_ICON));

	t = "Plain text - Justify text: ";
	t += BKUser::options.txtJustify ? "Enabled" : "Disabled";
	optionItems.push_back(BKMenuItem(t, "Toggle", 0));

	t = "Plain text - Foreground color: ";
	mi = BKMenuItem(t, "Select", BK_MENU_ITEM_COLOR_RECT);
	mi.color = BKUser::options.txtFGColor;
	optionItems.push_back(mi);

	t = "Plain text - Background color: ";
	mi = BKMenuItem(t, "Select", BK_MENU_ITEM_COLOR_RECT);
	mi.color = BKUser::options.txtBGColor;
	optionItems.push_back(mi);

	snprintf(txt, 1024, "CPU/Bus speed: %s", FZScreen::speedLabels[BKUser::options.pspSpeed]);
	t = txt;
	optionItems.push_back(BKMenuItem(t, "Change", BK_MENU_ITEM_USE_LR_ICON));

	t = "Display page loading and numbering labels: ";
	t += BKUser::options.displayLabels ? "Enabled" : "Disabled";
	optionItems.push_back(BKMenuItem(t, "Toggle", 0));

	t = "PDF - Invert colors: ";
	t += BKUser::options.pdfInvertColors ? "Enabled" : "Disabled";
	optionItems.push_back(BKMenuItem(t, "Toggle", 0));

	t = "PDF - Background color: ";
	mi = BKMenuItem(t, "Select", BK_MENU_ITEM_COLOR_RECT);
	mi.color = BKUser::options.pdfBGColor;
	optionItems.push_back(mi);

	optionItems.push_back(BKMenuItem("Clear bookmarks", "Select", 0));

	/*char txt[1024];
	snprintf(txt, 1024, "Plain text - Rotation: %d\260", BKUser::options.txtRotation);
	t = txt;
	optionItems.push_back(BKMenuItem(t, "Toggle", 0));*/
}

int BKMainMenu::update(unsigned int buttons) {
	if (mode == BKMM_CONTROLS) {
		return updateControls(buttons);
	} else if (mode == BKMM_OPTIONS) {
		return updateOptions(buttons);
	}
	return updateMain(buttons);
}

int BKMainMenu::updateMain(unsigned int buttons) {
	menuCursorUpdate(buttons, mainItems.size());

	int* b = FZScreen::ctrlReps();

	if (b[FZ_REPS_CIRCLE] == 1) {
		if (selItem == 0) {
			return BK_CMD_INVOKE_OPEN_FILE;
		}
		if (selItem == 1) {
			selItem = 0;
			topItem = 0;
			mode = BKMM_CONTROLS;
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == 2) {
			selItem = 0;
			topItem = 0;
			mode = BKMM_OPTIONS;
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == 3) {
			popupText = "Bookr - a document viewer for the Sony PSP.\nProgramming by Carlos and Edward.\nVisit http://bookr.sf.net for new versions.\nThis program is licensed under the terms of the GPL v2.\nUses the MuPDF library under the terms of the AFPL.";
			popupMode = BKPOPUP_INFO;
			return BK_CMD_MAINMENU_POPUP;
		}
		if (selItem == 4) {
#if 0
			if (reader != NULL) {
				// Set bookmark now
				printf("FIX1\n");
				/*if (isPdf)
					((BKPDF*)reader)->setBookmark(true);
				else
					((BKBook*)reader)->setBookmark(true);
				*/
			}
#endif
			return BK_CMD_EXIT;
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

int BKMainMenu::updateOptions(unsigned int buttons) {
	menuCursorUpdate(buttons, optionItems.size());

	int* b = FZScreen::ctrlReps();

	if (b[FZ_REPS_CIRCLE] == 1) {
		if (selItem == 0) {
			BKUser::setDefaultOptions();
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == 1) {
			BKUser::options.pdfFastScroll = !BKUser::options.pdfFastScroll;
			buildOptionMenu();
			popupText = "Fast scroll will cause instability with many PDF files.\nWhen reporting a bug make it very clear if you were using\nfast scroll or not. Also try the same file without fast scroll\nmode before reporting a bug.";
			popupMode = BKPOPUP_WARNING;
			return BKUser::options.pdfFastScroll ? BK_CMD_MAINMENU_POPUP : BK_CMD_MARK_DIRTY;
		}
		if (selItem == 2) {
			return BK_CMD_INVOKE_OPEN_FONT;
		}
		if (selItem == 4) {
			BKUser::options.txtJustify = !BKUser::options.txtJustify;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == 5) {
			return BK_CMD_INVOKE_COLOR_CHOOSER_TXTFG;
		}
		if (selItem == 6) {
			return BK_CMD_INVOKE_COLOR_CHOOSER_TXTBG;
		}
		if (selItem == 8) {
			BKUser::options.displayLabels = !BKUser::options.displayLabels;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == 9) {
			BKUser::options.pdfInvertColors = !BKUser::options.pdfInvertColors;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == 10) {
			return BK_CMD_INVOKE_COLOR_CHOOSER_PDFBG;
		}
		if (selItem == 11) {
			BKBookmarksManager::clear();
			popupText = "Bookmarks cleared.";
			popupMode = BKPOPUP_INFO;
			return BK_CMD_MAINMENU_POPUP;
		}
		/*if (selItem == 2) {
			if (BKUser::options.txtRotation == 0) {
				BKUser::options.txtRotation = 90;
			} else if (BKUser::options.txtRotation == 90) {
				BKUser::options.txtRotation = 180;
			} else if (BKUser::options.txtRotation == 180) {
				BKUser::options.txtRotation = 270;
			} else {
				BKUser::options.txtRotation = 0;
			}
			BKUser::save();
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}*/
	}

	if (b[FZ_REPS_TRIANGLE] == 1) {
		if (selItem == 2) {
			BKUser::options.txtFont = "bookr:builtin";
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
	}

	if (b[FZ_REPS_CROSS] == 1) {
		selItem = 0;
		topItem = 0;
		mode = BKMM_MAIN;
		BKUser::save();
		return BK_CMD_MARK_DIRTY;
	}

	if (b[FZ_REPS_START] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}

	if (b[FZ_REPS_LEFT] == 1) {
		if (selItem == 3) {
			--BKUser::options.txtSize;
			if (BKUser::options.txtSize < 6) {
				BKUser::options.txtSize = 6;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == 7) {
			--BKUser::options.pspSpeed;
			if (BKUser::options.pspSpeed < 0) {
				BKUser::options.pspSpeed = 0;
			} else {
				if (BKUser::options.pspSpeed == 0)
					FZScreen::setSpeed(5);
				else
					FZScreen::setSpeed(BKUser::options.pspSpeed);
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		}
	}

	if (b[FZ_REPS_RIGHT] == 1) {
		if (selItem == 3) {
			++BKUser::options.txtSize;
			if (BKUser::options.txtSize > 20) {
				BKUser::options.txtSize = 20;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == 7) {
			++BKUser::options.pspSpeed;
			if (BKUser::options.pspSpeed > 6) {
				BKUser::options.pspSpeed = 6;
			} else {
				if (BKUser::options.pspSpeed == 0)
					FZScreen::setSpeed(5);
				else
					FZScreen::setSpeed(BKUser::options.pspSpeed);
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		}
	}

	return 0;
}

void BKMainMenu::render() {
	string t("");
	if (mode == BKMM_MAIN) {
		string title("Main Menu");
		drawMenu(title, t, mainItems);
		string e;
		drawClockAndBattery(e);
	} else if (mode == BKMM_CONTROLS) {
		string title("Customize controls");
		drawMenu(title, t, controlItems);
	} else if (mode == BKMM_OPTIONS) {
		string title("Options");
		drawMenu(title, t, optionItems);
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

BKMainMenu* BKMainMenu::create(bool isPdf, BKLayer* pdfOrBookLayer) {
	BKMainMenu* f = new BKMainMenu(isPdf, pdfOrBookLayer);
	FZScreen::resetReps();
	return f;
}

