/*
 * Bookr: document reader for the Sony PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com)
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

#include <cstring>
#include <mupdf/fitz/version.h>

#include "graphics/screen.hpp"
#include "mainmenu.hpp"
#include "document.hpp"
#include "bookmark.hpp"
#include "popup.hpp"
#include "bookrconfig.hpp"

// Main menu layout
#define MAIN_MENU_ITEM_OPEN_FILE				0
#define MAIN_MENU_ITEM_BROWSE_CACHE				1
#define MAIN_MENU_ITEM_CONTROLS					2
#define MAIN_MENU_ITEM_OPTIONS					3
#define MAIN_MENU_ITEM_ABOUT					4
#define MAIN_MENU_ITEM_EXIT						5

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
#define OPTIONS_MENU_ITEM_PDF_INVERT_COLORS		3
#define OPTIONS_MENU_ITEM_PLAIN_CHOOSE_FONT		4
#define OPTIONS_MENU_ITEM_PLAIN_FONT_SIZE		5
#define OPTIONS_MENU_ITEM_PLAIN_SET_LINE_HEIGHT	6
#define OPTIONS_MENU_ITEM_PLAIN_JUSTIFY_TEXT	7
#define OPTIONS_MENU_ITEM_PLAIN_WRAP_TEXT		8
#define OPTIONS_MENU_ITEM_BROWSER_TEXTSIZE		9
#define OPTIONS_MENU_ITEM_BROWSER_DISPLAYMODE	10
#define OPTIONS_MENU_ITEM_BROWSER_ENABLEFLASH	11
#define OPTIONS_MENU_ITEM_BROWSER_INTERFACEMODE	12
#define OPTIONS_MENU_ITEM_BROWSER_CONFIRMEXIT	13
#define OPTIONS_MENU_ITEM_BROWSER_SHOWCURSOR	14
#define OPTIONS_MENU_ITEM_COLOR_SCHEMES			15
#define OPTIONS_MENU_ITEM_DISPLAY_LABELS		16
#define OPTIONS_MENU_ITEM_LOAD_LAST_FILE		17
#define OPTIONS_MENU_ITEM_CPU_BUS_SPEED			18
#define OPTIONS_MENU_ITEM_CPU_MENU_SPEED		19
#define OPTIONS_MENU_ITEM_CLEAR_BOOKMARKS		20

namespace bookr {

MainMenu::MainMenu() : mode(BKMM_MAIN), captureButton(false), frames(0) {
	buildMainMenu();
	buildControlMenu();
	buildOptionMenu();
}

MainMenu::~MainMenu() {
}

string MainMenu::getPopupText() {
	return popupText;
}

int MainMenu::getPopupMode() {
	return popupMode;
}

void MainMenu::rebuildMenu() {
	if (mode == BKMM_CONTROLS) {
		buildControlMenu();
	} else if (mode == BKMM_OPTIONS) {
		buildOptionMenu();
	}
}

void MainMenu::buildMainMenu() {
	mainItems.push_back(MenuItem("Open File", "Select", 0));
	mainItems.push_back(MenuItem("Browse chm Cache", "Select", 0));
	mainItems.push_back(MenuItem("Controls", "Select", 0));
	mainItems.push_back(MenuItem("Options", "Select", 0));
	mainItems.push_back(MenuItem("About", "Select", 0));
	mainItems.push_back(MenuItem("Exit", "Select", 0));
}

void MainMenu::buildControlMenu() {
	string cb("Change button");

	controlItems.clear();

	controlItems.push_back(MenuItem("Restore defaults", "Select", 0));

	string t("Previous page: ");
	t += Screen::nameForButtonReps(User::controls.previousPage);
	controlItems.push_back(MenuItem(t, cb, 0));

	t = "Next page: ";
	t += Screen::nameForButtonReps(User::controls.nextPage);
	controlItems.push_back(MenuItem(t, cb, 0));

	t = "Previous 10 pages: ";
	t += Screen::nameForButtonReps(User::controls.previous10Pages);
	controlItems.push_back(MenuItem(t, cb, 0));

	t = "Next 10 pages: ";
	t += Screen::nameForButtonReps(User::controls.next10Pages);
	controlItems.push_back(MenuItem(t, cb, 0));

	t = "Screen up: ";
	t += Screen::nameForButtonReps(User::controls.screenUp);
	controlItems.push_back(MenuItem(t, cb, 0));

	t = "Screen down: ";
	t += Screen::nameForButtonReps(User::controls.screenDown);
	controlItems.push_back(MenuItem(t, cb, 0));

	t = "Screen left: ";
	t += Screen::nameForButtonReps(User::controls.screenLeft);
	controlItems.push_back(MenuItem(t, cb, 0));

	t = "Screen right: ";
	t += Screen::nameForButtonReps(User::controls.screenRight);
	controlItems.push_back(MenuItem(t, cb, 0));

	t = "Zoom in: ";
	t += Screen::nameForButtonReps(User::controls.zoomIn);
	controlItems.push_back(MenuItem(t, cb, 0));

	t = "Zoom out: ";
	t += Screen::nameForButtonReps(User::controls.zoomOut);
	controlItems.push_back(MenuItem(t, cb, 0));
}

void MainMenu::buildOptionMenu() {
	optionItems.clear();
	optionItems.push_back(MenuItem("Restore defaults", "Select", 0));

	string t("Menu control style: ");
	switch(User::controls.select) {
	case FZ_REPS_CIRCLE:	t.append("Asian");	break;
	case FZ_REPS_CROSS:		t.append("Western"); break;
	}
	optionItems.push_back(MenuItem(t, "Choose", BK_MENU_ITEM_USE_LR_ICON));

	t = ("PDF - Fast images (zoom limited to 2x): ");
	t += User::options.pdfFastScroll ? "Enabled" : "Disabled";
	optionItems.push_back(MenuItem(t, "Toggle", 0));

	t = "PDF - Invert colors: ";
	t += User::options.pdfInvertColors ? "Enabled" : "Disabled";
	optionItems.push_back(MenuItem(t, "Toggle", 0));

	t = "Plain text - Font file: ";
	if (User::options.txtFont == "bookr:builtin") {
		t += "built-in";
	} else {
		int s = User::options.txtFont.size();
		if (s > 25) {
			t += "...";
			t += string(User::options.txtFont, s - 25, 25);
		} else {
			t += User::options.txtFont;
		}
	}
	MenuItem mi = MenuItem(t, "Select font file", BK_MENU_ITEM_OPTIONAL_TRIANGLE_LABEL);
	mi.triangleLabel = "Use built-in font";
	optionItems.push_back(mi);

	char txt[1024];
	snprintf(txt, 1024, "Plain text - Font size: %d pixels", User::options.txtSize);
	t = txt;
	optionItems.push_back(MenuItem(t, "Change", BK_MENU_ITEM_USE_LR_ICON));

	snprintf(txt, 1024, "Plain text - Line Height: %d Pct", User::options.txtHeightPct);
	t = txt;
	optionItems.push_back(MenuItem(t, "Change", BK_MENU_ITEM_USE_LR_ICON));

	t = "Plain text - Justify text: ";
	t += User::options.txtJustify ? "Enabled" : "Disabled";
	optionItems.push_back(MenuItem(t, "Toggle", 0));

	snprintf(txt, 1024, "Plain text - Wrap CRs: %d", User::options.txtWrapCR);
	t = txt;
	optionItems.push_back(MenuItem(t, "Change", BK_MENU_ITEM_USE_LR_ICON));

	/*char txt[1024];
	snprintf(txt, 1024, "Plain text - Rotation: %d\260", User::options.txtRotation);
	t = txt;
	optionItems.push_back(MenuItem(t, "Toggle", 0));*/

	snprintf(txt, 1024, "Browser - Text size: %s", Screen::browserTextSizes[User::options.browserTextSize]);
	t = txt;
	optionItems.push_back(MenuItem(t, "Change", BK_MENU_ITEM_USE_LR_ICON));

	snprintf(txt, 1024, "Browser - Display mode: %s", Screen::browserDisplayModes[User::options.browserDisplayMode]);
	t = txt;
	optionItems.push_back(MenuItem(t, "Change", BK_MENU_ITEM_USE_LR_ICON));

	t = "Browser - Flash content: ";
	t += User::options.browserEnableFlash ? "Enabled" : "Disabled";
	optionItems.push_back(MenuItem(t, "Toggle", 0));

	snprintf(txt, 1024, "Browser - Interface mode: %s", Screen::browserInterfaceModes[User::options.browserInterfaceMode]);
	t = txt;
	optionItems.push_back(MenuItem(t, "Change", BK_MENU_ITEM_USE_LR_ICON));

	t = "Browser - Exit confirmation: ";
	t += User::options.browserConfirmExit ? "Enabled" : "Disabled";
	optionItems.push_back(MenuItem(t, "Toggle", 0));

	t = "Browser - Show cursor: ";
	t += User::options.browserShowCursor ? "Enabled" : "Disabled";
	optionItems.push_back(MenuItem(t, "Toggle", 0));

	t = "Color schemes";
	mi = MenuItem(t, "Choose", BK_MENU_ITEM_OPTIONAL_TRIANGLE_LABEL | BK_MENU_ITEM_USE_LR_ICON | BK_MENU_ITEM_COLOR_RECT);
	mi.triangleLabel = "Manage schemes";
	mi.bgcolor = User::options.colorSchemes[User::options.currentScheme].txtBGColor;
	mi.fgcolor = User::options.colorSchemes[User::options.currentScheme].txtFGColor;
	optionItems.push_back(mi);

	t = "Display page loading and numbering labels: ";
	t += User::options.displayLabels ? "Enabled" : "Disabled";
	optionItems.push_back(MenuItem(t, "Toggle", 0));

	t = "Autoload last file: ";
	t += User::options.loadLastFile ? "Enabled" : "Disabled";
	optionItems.push_back(MenuItem(t, "Toggle", 0));

	snprintf(txt, 1024, "CPU/Bus speed: %s", Screen::speedLabels[User::options.pspSpeed]);
	t = txt;
	optionItems.push_back(MenuItem(t, "Change", BK_MENU_ITEM_USE_LR_ICON));
	snprintf(txt, 1024, "Menu speed: %s", Screen::speedLabels[User::options.pspMenuSpeed]);
	t = txt;
	optionItems.push_back(MenuItem(t, "Change", BK_MENU_ITEM_USE_LR_ICON));

	optionItems.push_back(MenuItem("Clear bookmarks", "Select", 0));

}

int MainMenu::update(unsigned int buttons) {
	if (mode == BKMM_CONTROLS) {
		return updateControls(buttons);
	} else if (mode == BKMM_OPTIONS) {
		return updateOptions(buttons);
	}
	return updateMain(buttons);
}

int MainMenu::updateMain(unsigned int buttons) {
	menuCursorUpdate(buttons, mainItems.size());

	int* b = Screen::ctrlReps();

	if (b[User::controls.select] == 1) {
		if (selItem == MAIN_MENU_ITEM_OPEN_FILE) {
			return BK_CMD_INVOKE_OPEN_FILE;
		}
		if (selItem == MAIN_MENU_ITEM_BROWSE_CACHE) {
			return BK_CMD_INVOKE_BROWSE_CACHE;
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
			popupText = "\
			Bookr Mod Vita (" + std::string(GIT_VERSION) + ") - by pathway27 (Sreekara C.)\n\n\
			IS A MODIFICATION OF THE ORIGINAL\n\
			Bookr and bookr-mod - a document viewer for the Sony PSP\n\
			  by Carlos and Edward\n\
			  V8.1 by Nguyen Chi Tam <nguyenchitam@gmail.com>\n\
			  Modified for Official Firmware by the HomebrewStore Development Team\n\
			AND VARIOUS OTHER FORKS\n\n\
			This program is licensed under the terms of the GNU GPLv3+.\n\n\
			This program uses the following libraries/projects:\n\
			  MuPDF v" + FZ_VERSION + " by ArtifexSoftware under AGPL v3\n\
			  vita2d by xerpi under MIT\n\
			  psp2shell by Cpasjuste under GPLv3\n\n\
			Thanks to:\n\
			  Team Molecule for Henkaku\n\
			  VITA SDK Contibutors\n\
			  People on freenode#vitasdk#henkaku and Discord\n\
			  ebraminio for help with harbuzz\n";
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

	if (b[User::controls.cancel] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}

	if (b[User::controls.showMainMenu] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}

	frames++;
	if (frames % 60 == 0)
		return BK_CMD_MARK_DIRTY;

	return 0;
}

static int buttonsHack = 0;
int MainMenu::updateControls(unsigned int buttons) {
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
		int repsCode = Screen::repsForButtonMask(buttons);
		switch (selItem) {
			case CONTROLS_MENU_ITEM_PREVIOUS_PAGE: 		User::controls.previousPage = repsCode; break;
			case CONTROLS_MENU_ITEM_NEXT_PAGE: 			User::controls.nextPage = repsCode; break;
			case CONTROLS_MENU_ITEM_PREVIOUS_10_PAGES: 	User::controls.previous10Pages = repsCode; break;
			case CONTROLS_MENU_ITEM_NEXT_10_PAGES: 		User::controls.next10Pages = repsCode; break;
			case CONTROLS_MENU_ITEM_SCREEN_UP: 			User::controls.screenUp = repsCode; break;
			case CONTROLS_MENU_ITEM_SCREEN_DOWN: 		User::controls.screenDown = repsCode; break;
			case CONTROLS_MENU_ITEM_SCREEN_LEFT: 		User::controls.screenLeft = repsCode; break;
			case CONTROLS_MENU_ITEM_SCREEN_RIGHT: 		User::controls.screenRight = repsCode; break;
			case CONTROLS_MENU_ITEM_ZOOM_IN: 			User::controls.zoomIn = repsCode; break;
			case CONTROLS_MENU_ITEM_ZOOM_OUT: 			User::controls.zoomOut = repsCode; break;
		}
		User::save();
		buildControlMenu();
		captureButton = false;
		return BK_CMD_MARK_DIRTY;
	}

	menuCursorUpdate(buttons, controlItems.size());

	int* b = Screen::ctrlReps();

	if (b[User::controls.select] == 1) {
		if (selItem == CONTROLS_MENU_ITEM_RESTORE_DEFAULTS) {
			User::setDefaultControls();
			User::save();
			buildControlMenu();
			return BK_CMD_MARK_DIRTY;
		}
		buttonsHack = 0;
		captureButton = true;
		return BK_CMD_MARK_DIRTY;
	}

	if (b[User::controls.cancel] == 1) {
		selItem = 0;
		topItem = 0;
		mode = BKMM_MAIN;
		return BK_CMD_MARK_DIRTY;
	}

	if (b[User::controls.showMainMenu] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}

	return 0;
}

int MainMenu::updateOptions(unsigned int buttons) {
	menuCursorUpdate(buttons, optionItems.size());

	int* b = Screen::ctrlReps();

	if (b[User::controls.select] == 1) {
		if (selItem == OPTIONS_MENU_ITEM_RESTORE_DEFAULTS) {
			User::setDefaultOptions();
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_PDF_FAST_IMAGES) {
			User::options.pdfFastScroll = !User::options.pdfFastScroll;
			buildOptionMenu();
			popupText = "Fast images will cause instability with many PDF files.\nWhen reporting a bug make it very clear if you were using\nfast images or not. Also try the same file without fast\nimages mode before reporting a bug.";
			popupMode = BKPOPUP_WARNING;
			return User::options.pdfFastScroll ? BK_CMD_MAINMENU_POPUP : BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_PLAIN_CHOOSE_FONT) {
			return BK_CMD_INVOKE_OPEN_FONT;
		}
		if (selItem == OPTIONS_MENU_ITEM_PLAIN_JUSTIFY_TEXT) {
			User::options.txtJustify = !User::options.txtJustify;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_COLOR_SCHEMES) {
			return BK_CMD_INVOKE_COLOR_SCHEME_MANAGER;//_TXTFG;
		}
		if (selItem == OPTIONS_MENU_ITEM_DISPLAY_LABELS) {
			User::options.displayLabels = !User::options.displayLabels;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_PDF_INVERT_COLORS) {
			User::options.pdfInvertColors = !User::options.pdfInvertColors;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_CLEAR_BOOKMARKS) {
			//BKBookmarksManager::clear();
			popupText = "Bookmarks cleared.";
			popupMode = BKPOPUP_INFO;
			return BK_CMD_MAINMENU_POPUP;
		}
		if (selItem == OPTIONS_MENU_ITEM_LOAD_LAST_FILE) {
			User::options.loadLastFile = !User::options.loadLastFile;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY; //?
		}
		/*if (selItem == 2) {
			if (User::options.txtRotation == 0) {
				User::options.txtRotation = 90;
			} else if (User::options.txtRotation == 90) {
				User::options.txtRotation = 180;
			} else if (User::options.txtRotation == 180) {
				User::options.txtRotation = 270;
			} else {
				User::options.txtRotation = 0;
			}
			User::save();
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}*/

		if (selItem == OPTIONS_MENU_ITEM_BROWSER_ENABLEFLASH) {
			User::options.browserEnableFlash = !User::options.browserEnableFlash;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_BROWSER_CONFIRMEXIT) {
			User::options.browserConfirmExit = !User::options.browserConfirmExit;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_BROWSER_SHOWCURSOR) {
			User::options.browserShowCursor = !User::options.browserShowCursor;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
	}

	if (b[User::controls.alternate] == 1) {
		if (selItem == OPTIONS_MENU_ITEM_PLAIN_CHOOSE_FONT) {
			User::options.txtFont = "bookr:builtin";
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_COLOR_SCHEMES) {
			return BK_CMD_INVOKE_COLOR_SCHEME_MANAGER;
		}
	}

	if (b[User::controls.cancel] == 1) {
		selItem = 0;
		topItem = 0;
		mode = BKMM_MAIN;
		User::save();
		return BK_CMD_MARK_DIRTY;
	}

	if (b[User::controls.showMainMenu] == 1) {
		selItem = 0;
		topItem = 0;
		mode = BKMM_MAIN;
		User::save();
		return BK_CMD_CLOSE_TOP_LAYER;
	}

	if (b[User::controls.menuLeft] == 1) {
		if (selItem == OPTIONS_MENU_ITEM_SET_CONTROL_STYLE) {
			switch(User::controls.select) {
			case FZ_REPS_CIRCLE:
				User::controls.select = FZ_REPS_CROSS;
				User::controls.cancel = FZ_REPS_CIRCLE;
				break;
			case FZ_REPS_CROSS:
			default:
				User::controls.select = FZ_REPS_CIRCLE;
				User::controls.cancel = FZ_REPS_CROSS;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_PLAIN_FONT_SIZE) {
			--User::options.txtSize;
			if (User::options.txtSize < 6) {
				User::options.txtSize = 6;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_PLAIN_SET_LINE_HEIGHT) {
			User::options.txtHeightPct -= 5;
			if (User::options.txtHeightPct < 50) {
				User::options.txtHeightPct = 50;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		}  else if (selItem == OPTIONS_MENU_ITEM_COLOR_SCHEMES) {
			if (User::options.currentScheme == 0) {
				User::options.currentScheme = User::options.colorSchemes.size()-1;
			} else {
				User::options.currentScheme--;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_CPU_BUS_SPEED) {
			--User::options.pspSpeed;
			if (User::options.pspSpeed < 0) {
				User::options.pspSpeed = 0;
			} else {
				if (User::options.pspSpeed == 0)
					Screen::setSpeed(5);
				else
					Screen::setSpeed(User::options.pspSpeed);
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_CPU_MENU_SPEED) {
			--User::options.pspMenuSpeed;
			if (User::options.pspMenuSpeed < 0) {
				User::options.pspMenuSpeed = 0;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_PLAIN_WRAP_TEXT) {
			--User::options.txtWrapCR;
			if (User::options.txtWrapCR < 0) {
				User::options.txtWrapCR = 0;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_BROWSER_TEXTSIZE) {
			--User::options.browserTextSize;
			if (User::options.browserTextSize < 0) {
				User::options.browserTextSize = 2;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_BROWSER_DISPLAYMODE) {
			--User::options.browserDisplayMode;
			if (User::options.browserDisplayMode < 0) {
				User::options.browserDisplayMode = 2;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_BROWSER_INTERFACEMODE) {
			--User::options.browserInterfaceMode;
			if (User::options.browserInterfaceMode < 0) {
				User::options.browserInterfaceMode = 2;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
	}


	if (b[User::controls.menuRight] == 1) {
		if (selItem == OPTIONS_MENU_ITEM_SET_CONTROL_STYLE) {
			switch(User::controls.select) {
			case FZ_REPS_CIRCLE:
				User::controls.select = FZ_REPS_CROSS;
				User::controls.cancel = FZ_REPS_CIRCLE;
				break;
			case FZ_REPS_CROSS:
			default:
				User::controls.select = FZ_REPS_CIRCLE;
				User::controls.cancel = FZ_REPS_CROSS;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_PLAIN_FONT_SIZE) {
			++User::options.txtSize;
			if (User::options.txtSize > 20) {
				User::options.txtSize = 20;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_PLAIN_SET_LINE_HEIGHT) {
			User::options.txtHeightPct += 5;
			if (User::options.txtHeightPct > 150) {
				User::options.txtHeightPct = 150;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_COLOR_SCHEMES) {
			User::options.currentScheme ++;
			if ((unsigned int) User::options.currentScheme >= User::options.colorSchemes.size()) {
				User::options.currentScheme = 0;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_CPU_BUS_SPEED) {
			++User::options.pspSpeed;
			if (User::options.pspSpeed > 6) {
				User::options.pspSpeed = 6;
			} else {
				if (User::options.pspSpeed == 0)
					Screen::setSpeed(5);
				else
					Screen::setSpeed(User::options.pspSpeed);
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_CPU_MENU_SPEED) {
			++User::options.pspMenuSpeed;
			if (User::options.pspMenuSpeed > 6) {
				User::options.pspMenuSpeed = 6;
			} else {
				// if (User::options.pspMenuSpeed == 0)
				// 	Screen::setSpeed(5);
				// else
				// 	Screen::setSpeed(User::options.pspMenuSpeed);
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_PLAIN_WRAP_TEXT) {
			++User::options.txtWrapCR;
			if (User::options.txtWrapCR > 3) {
				User::options.txtWrapCR = 3;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_BROWSER_TEXTSIZE) {
			++User::options.browserTextSize;
			if (User::options.browserTextSize > 2) {
				User::options.browserTextSize = 0;
		}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_BROWSER_DISPLAYMODE) {
			++User::options.browserDisplayMode;
			if (User::options.browserDisplayMode > 2) {
				User::options.browserDisplayMode = 0;
	}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_BROWSER_INTERFACEMODE) {
			++User::options.browserInterfaceMode;
			if (User::options.browserInterfaceMode > 2) {
				User::options.browserInterfaceMode = 0;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
	}


	return 0;
}

void MainMenu::render() {
	#ifdef DEBUG_RENDER
    printf("mainmenu render\n");
  #endif
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
	// if (captureButton) {
	// 	texUI->bindForDisplay();
	// 	Screen::ambientColor(0xf0dddddd);
	// 	drawPill(150, 126, 180, 20, 6, 31, 1);
	// 	fontBig->bindForDisplay();
	// 	Screen::ambientColor(0xff222222);
	// 	drawTextHC("Press the new button", fontBig, 130);
	// }
}

MainMenu* MainMenu::create() {
	MainMenu* f = new MainMenu();
	Screen::resetReps();
	return f;
}

}