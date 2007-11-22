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

#include <string.h>
#include "fzscreen.h"

#include "bkmainmenu.h"
#include "bkdocument.h"
#include "bkbookmark.h"
#include "bkpopup.h"

// Main menu layout
#define MAIN_MENU_ITEM_OPEN_FILE				0
#define MAIN_MENU_ITEM_CONTROLS					1
#define MAIN_MENU_ITEM_OPTIONS					2
#define MAIN_MENU_ITEM_ABOUT					3
#define MAIN_MENU_ITEM_EXIT						4

// Controls menu layout
#define CONTROLS_MENU_ITEM_RESTORE_DEFAULTS		0
#define CONTROLS_MENU_ITEM_PREVIOUS_PAGE		1
#define CONTROLS_MENU_ITEM_NEXT_PAGE			2
#define CONTROLS_MENU_ITEM_PREVIOUS_10_PAGES	3
#define CONTROLS_MENU_ITEM_NEXT_10_PAGES		4
#define CONTROLS_MENU_ITEM_SCREEN_UP			5
#define CONTROLS_MENU_ITEM_SCREEN_DOWN			6
#define CONTROLS_MENU_ITEM_SCREEN_LEFT			7
#define CONTROLS_MENU_ITEM_SCREEN_RIGHT			8
#define CONTROLS_MENU_ITEM_ZOOM_IN				9
#define CONTROLS_MENU_ITEM_ZOOM_OUT				10

// Options menu layout
#define OPTIONS_MENU_ITEM_RESTORE_DEFAULTS		0
#define OPTIONS_MENU_ITEM_SET_CONTROL_STYLE		1
#define OPTIONS_MENU_ITEM_PDF_FAST_IMAGES		2
#define OPTIONS_MENU_ITEM_CHOOSE_FONT			3
#define OPTIONS_MENU_ITEM_FONT_SIZE				4
#define OPTIONS_MENU_ITEM_SET_LINE_HEIGHT		5
#define OPTIONS_MENU_ITEM_JUSTIFY_TEXT			6
#define OPTIONS_MENU_ITEM_COLOR_SCHEMES			7
#define OPTIONS_MENU_ITEM_CPU_BUS_SPEED			8
#define OPTIONS_MENU_ITEM_CPU_MENU_SPEED		9
#define OPTIONS_MENU_ITEM_DISPLAY_LABELS		10
#define OPTIONS_MENU_ITEM_INVERT_COLORS_PDF		11
#define OPTIONS_MENU_ITEM_CLEAR_BOOKMARKS		12
#define OPTIONS_MENU_ITEM_LOAD_LAST_FILE		13
#define OPTIONS_MENU_ITEM_WRAP_TEXT				14

BKMainMenu::BKMainMenu() : mode(BKMM_MAIN), captureButton(false), frames(0) {
	buildMainMenu();
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

void BKMainMenu::buildMainMenu() {
	mainItems.push_back(BKMenuItem("Open file", "Select", 0));
	mainItems.push_back(BKMenuItem("Controls", "Select", 0));	
	mainItems.push_back(BKMenuItem("Options", "Select", 0));
	mainItems.push_back(BKMenuItem("About", "Select", 0));
	mainItems.push_back(BKMenuItem("Exit", "Select", 0));	
}

void BKMainMenu::buildControlMenu() {
	string cb("Change button");

	controlItems.clear();

	controlItems.push_back(BKMenuItem("Restore defaults", "Select", 0));
	
	string t("Previous page: ");
	t += FZScreen::nameForButtonReps(BKUser::controls.previousPage);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "Next page: ";
	t += FZScreen::nameForButtonReps(BKUser::controls.nextPage);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "Previous 10 pages: ";
	t += FZScreen::nameForButtonReps(BKUser::controls.previous10Pages);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "Next 10 pages: ";
	t += FZScreen::nameForButtonReps(BKUser::controls.next10Pages);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "Screen up: ";
	t += FZScreen::nameForButtonReps(BKUser::controls.screenUp);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "Screen down: ";
	t += FZScreen::nameForButtonReps(BKUser::controls.screenDown);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "Screen left: ";
	t += FZScreen::nameForButtonReps(BKUser::controls.screenLeft);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "Screen right: ";
	t += FZScreen::nameForButtonReps(BKUser::controls.screenRight);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "Zoom in: ";
	t += FZScreen::nameForButtonReps(BKUser::controls.zoomIn);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = "Zoom out: ";
	t += FZScreen::nameForButtonReps(BKUser::controls.zoomOut);
	controlItems.push_back(BKMenuItem(t, cb, 0));
}

void BKMainMenu::buildOptionMenu() {
	optionItems.clear();
	optionItems.push_back(BKMenuItem("Restore defaults", "Select", 0));

	string t("Menu control style: ");
	switch(BKUser::controls.select) {
	case FZ_REPS_CIRCLE:	t.append("Asian");	break;
	case FZ_REPS_CROSS:		t.append("Western"); break;
	}
	optionItems.push_back(BKMenuItem(t, "Choose", BK_MENU_ITEM_USE_LR_ICON));
	
	t = ("PDF - Fast images (zoom limited to 2x): ");
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

	snprintf(txt, 1024, "Plain text - Line Height: %d Pct", BKUser::options.txtHeightPct);
	t = txt;
	optionItems.push_back(BKMenuItem(t, "Change", BK_MENU_ITEM_USE_LR_ICON));

	t = "Plain text - Justify text: ";
	t += BKUser::options.txtJustify ? "Enabled" : "Disabled";
	optionItems.push_back(BKMenuItem(t, "Toggle", 0));

	t = "Color schemes";
	mi = BKMenuItem(t, "Choose", BK_MENU_ITEM_OPTIONAL_TRIANGLE_LABEL | BK_MENU_ITEM_USE_LR_ICON | BK_MENU_ITEM_COLOR_RECT);
	mi.triangleLabel = "Manage schemes";
	mi.bgcolor = BKUser::options.colorSchemes[BKUser::options.currentScheme].txtBGColor;
	mi.fgcolor = BKUser::options.colorSchemes[BKUser::options.currentScheme].txtFGColor;
	optionItems.push_back(mi);

	snprintf(txt, 1024, "CPU/Bus speed: %s", FZScreen::speedLabels[BKUser::options.pspSpeed]);
	t = txt;
	optionItems.push_back(BKMenuItem(t, "Change", BK_MENU_ITEM_USE_LR_ICON));
	snprintf(txt, 1024, "Menu speed: %s", FZScreen::speedLabels[BKUser::options.pspMenuSpeed]);
	t = txt;
	optionItems.push_back(BKMenuItem(t, "Change", BK_MENU_ITEM_USE_LR_ICON));

	t = "Display page loading and numbering labels: ";
	t += BKUser::options.displayLabels ? "Enabled" : "Disabled";
	optionItems.push_back(BKMenuItem(t, "Toggle", 0));

	t = "PDF - Invert colors: ";
	t += BKUser::options.pdfInvertColors ? "Enabled" : "Disabled";
	optionItems.push_back(BKMenuItem(t, "Toggle", 0));

	optionItems.push_back(BKMenuItem("Clear bookmarks", "Select", 0));

	t = "Autoload last file: ";
	t += BKUser::options.loadLastFile ? "Enabled" : "Disabled";
	optionItems.push_back(BKMenuItem(t, "Toggle", 0));

	snprintf(txt, 1024, "Plain text - Wrap CRs: %d", BKUser::options.txtWrapCR);
	t = txt;
	optionItems.push_back(BKMenuItem(t, "Change", BK_MENU_ITEM_USE_LR_ICON));

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

	if (b[BKUser::controls.select] == 1) {
		if (selItem == MAIN_MENU_ITEM_OPEN_FILE) {
			return BK_CMD_INVOKE_OPEN_FILE;
		}
		if (selItem == MAIN_MENU_ITEM_CONTROLS) {
			selItem = 0;
			topItem = 0;
			mode = BKMM_CONTROLS;
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == MAIN_MENU_ITEM_OPTIONS) {
			selItem = 0;
			topItem = 0;
			mode = BKMM_OPTIONS;
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == MAIN_MENU_ITEM_ABOUT) {
			popupText = "Bookr - a document viewer for the Sony PSP.\nProgramming by Carlos, Edward and Chris.\nVisit http://bookr.sf.net for new versions.\nThis program is licensed under the terms of the GPL v2.\nUses the MuPDF and djvulibre libraries under the terms \nof their respective licenses. \nDjVu format support by Yang Hu.\nSeveral features by Paul Murray.";
			popupMode = BKPOPUP_INFO;
			return BK_CMD_MAINMENU_POPUP;
		}
		if (selItem == MAIN_MENU_ITEM_EXIT) {
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

	if (b[BKUser::controls.cancel] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}

	if (b[BKUser::controls.showMainMenu] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}

	frames++;
	if (frames % 60 == 0)
		return BK_CMD_MARK_DIRTY;

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
			case CONTROLS_MENU_ITEM_PREVIOUS_PAGE: 		BKUser::controls.previousPage = repsCode; break;
			case CONTROLS_MENU_ITEM_NEXT_PAGE: 			BKUser::controls.nextPage = repsCode; break;
			case CONTROLS_MENU_ITEM_PREVIOUS_10_PAGES: 	BKUser::controls.previous10Pages = repsCode; break;
			case CONTROLS_MENU_ITEM_NEXT_10_PAGES: 		BKUser::controls.next10Pages = repsCode; break;
			case CONTROLS_MENU_ITEM_SCREEN_UP: 			BKUser::controls.screenUp = repsCode; break;
			case CONTROLS_MENU_ITEM_SCREEN_DOWN: 		BKUser::controls.screenDown = repsCode; break;
			case CONTROLS_MENU_ITEM_SCREEN_LEFT: 		BKUser::controls.screenLeft = repsCode; break;
			case CONTROLS_MENU_ITEM_SCREEN_RIGHT: 		BKUser::controls.screenRight = repsCode; break;
			case CONTROLS_MENU_ITEM_ZOOM_IN: 			BKUser::controls.zoomIn = repsCode; break;
			case CONTROLS_MENU_ITEM_ZOOM_OUT: 			BKUser::controls.zoomOut = repsCode; break;
		}
		BKUser::save();
		buildControlMenu();
		captureButton = false;
		return BK_CMD_MARK_DIRTY;
	}

	menuCursorUpdate(buttons, controlItems.size());

	int* b = FZScreen::ctrlReps();

	if (b[BKUser::controls.select] == 1) {
		if (selItem == CONTROLS_MENU_ITEM_RESTORE_DEFAULTS) {
			BKUser::setDefaultControls();
			BKUser::save();
			buildControlMenu();			
			return BK_CMD_MARK_DIRTY;
		}
		buttonsHack = 0;
		captureButton = true;
		return BK_CMD_MARK_DIRTY;
	}

	if (b[BKUser::controls.cancel] == 1) {
		selItem = 0;
		topItem = 0;
		mode = BKMM_MAIN;
		return BK_CMD_MARK_DIRTY;
	}

	if (b[BKUser::controls.showMainMenu] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}

	return 0;
}

int BKMainMenu::updateOptions(unsigned int buttons) {
	menuCursorUpdate(buttons, optionItems.size());

	int* b = FZScreen::ctrlReps();

	if (b[BKUser::controls.select] == 1) {
		if (selItem == OPTIONS_MENU_ITEM_RESTORE_DEFAULTS) {
			BKUser::setDefaultOptions();
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_PDF_FAST_IMAGES) {
			BKUser::options.pdfFastScroll = !BKUser::options.pdfFastScroll;
			buildOptionMenu();
			popupText = "Fast images will cause instability with many PDF files.\nWhen reporting a bug make it very clear if you were using\nfast images or not. Also try the same file without fast\nimages mode before reporting a bug.";
			popupMode = BKPOPUP_WARNING;
			return BKUser::options.pdfFastScroll ? BK_CMD_MAINMENU_POPUP : BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_CHOOSE_FONT) {
			return BK_CMD_INVOKE_OPEN_FONT;
		}
		if (selItem == OPTIONS_MENU_ITEM_JUSTIFY_TEXT) {
			BKUser::options.txtJustify = !BKUser::options.txtJustify;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_COLOR_SCHEMES) {
			return BK_CMD_INVOKE_COLOR_SCHEME_MANAGER;//_TXTFG;
		}
		if (selItem == OPTIONS_MENU_ITEM_DISPLAY_LABELS) {
			BKUser::options.displayLabels = !BKUser::options.displayLabels;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_INVERT_COLORS_PDF) {
			BKUser::options.pdfInvertColors = !BKUser::options.pdfInvertColors;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_CLEAR_BOOKMARKS) {
			BKBookmarksManager::clear();
			popupText = "Bookmarks cleared.";
			popupMode = BKPOPUP_INFO;
			return BK_CMD_MAINMENU_POPUP;
		}
		if (selItem == OPTIONS_MENU_ITEM_LOAD_LAST_FILE) {
			BKUser::options.loadLastFile = !BKUser::options.loadLastFile;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY; //?
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

	if (b[BKUser::controls.alternate] == 1) {
		if (selItem == OPTIONS_MENU_ITEM_CHOOSE_FONT) {
			BKUser::options.txtFont = "bookr:builtin";
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_COLOR_SCHEMES) {
			return BK_CMD_INVOKE_COLOR_SCHEME_MANAGER;
		}
	}

	if (b[BKUser::controls.cancel] == 1) {
		selItem = 0;
		topItem = 0;
		mode = BKMM_MAIN;
		BKUser::save();
		return BK_CMD_MARK_DIRTY;
	}

	if (b[BKUser::controls.showMainMenu] == 1) {
		selItem = 0;
		topItem = 0;
		mode = BKMM_MAIN;
		BKUser::save();
		return BK_CMD_CLOSE_TOP_LAYER;
	}

	if (b[BKUser::controls.menuLeft] == 1) {
		if (selItem == OPTIONS_MENU_ITEM_SET_CONTROL_STYLE) {
			switch(BKUser::controls.select) {
			case FZ_REPS_CIRCLE:
				BKUser::controls.select = FZ_REPS_CROSS;
				BKUser::controls.cancel = FZ_REPS_CIRCLE;
				break;
			case FZ_REPS_CROSS:
			default:
				BKUser::controls.select = FZ_REPS_CIRCLE;
				BKUser::controls.cancel = FZ_REPS_CROSS;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_FONT_SIZE) {
			--BKUser::options.txtSize;
			if (BKUser::options.txtSize < 6) {
				BKUser::options.txtSize = 6;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_SET_LINE_HEIGHT) {
			BKUser::options.txtHeightPct -= 5;
			if (BKUser::options.txtHeightPct < 50) {
				BKUser::options.txtHeightPct = 50;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		}  else if (selItem == OPTIONS_MENU_ITEM_COLOR_SCHEMES) {
			if (BKUser::options.currentScheme == 0) {
				BKUser::options.currentScheme = BKUser::options.colorSchemes.size()-1;
			} else {
				BKUser::options.currentScheme--;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_CPU_BUS_SPEED) {
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
		} else if (selItem == OPTIONS_MENU_ITEM_CPU_MENU_SPEED) {
			--BKUser::options.pspMenuSpeed;
			if (BKUser::options.pspMenuSpeed < 0) {
				BKUser::options.pspMenuSpeed = 0;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_WRAP_TEXT) {
			--BKUser::options.txtWrapCR;
			if (BKUser::options.txtWrapCR < 0) {
				BKUser::options.txtWrapCR = 0;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		}
	}

	if (b[BKUser::controls.menuRight] == 1) {
		if (selItem == OPTIONS_MENU_ITEM_SET_CONTROL_STYLE) {
			switch(BKUser::controls.select) {
			case FZ_REPS_CIRCLE:
				BKUser::controls.select = FZ_REPS_CROSS;
				BKUser::controls.cancel = FZ_REPS_CIRCLE;
				break;
			case FZ_REPS_CROSS:
			default:
				BKUser::controls.select = FZ_REPS_CIRCLE;
				BKUser::controls.cancel = FZ_REPS_CROSS;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_FONT_SIZE) {
			++BKUser::options.txtSize;
			if (BKUser::options.txtSize > 20) {
				BKUser::options.txtSize = 20;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_SET_LINE_HEIGHT) {
			BKUser::options.txtHeightPct += 5;
			if (BKUser::options.txtHeightPct > 150) {
				BKUser::options.txtHeightPct = 150;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_COLOR_SCHEMES) {
			BKUser::options.currentScheme ++;
			if ((unsigned int) BKUser::options.currentScheme >= BKUser::options.colorSchemes.size()) {
				BKUser::options.currentScheme = 0;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;	
		} else if (selItem == OPTIONS_MENU_ITEM_CPU_BUS_SPEED) {
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
		} else if (selItem == OPTIONS_MENU_ITEM_CPU_MENU_SPEED) {
			++BKUser::options.pspMenuSpeed;
			if (BKUser::options.pspMenuSpeed > 6) {
				BKUser::options.pspMenuSpeed = 6;
			} else {
//				if (BKUser::options.pspMenuSpeed == 0)
//					FZScreen::setSpeed(5);
//				else
//					FZScreen::setSpeed(BKUser::options.pspMenuSpeed);
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_WRAP_TEXT) {
			++BKUser::options.txtWrapCR;
			if (BKUser::options.txtWrapCR > 3) {
				BKUser::options.txtWrapCR = 3;
			} else {
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

BKMainMenu* BKMainMenu::create() {
	BKMainMenu* f = new BKMainMenu();
	FZScreen::resetReps();
	return f;
}

