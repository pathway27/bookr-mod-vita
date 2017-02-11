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

#include <string.h>
#include "fzscreen.h"

#include "bkmainmenu.h"
#include "bkdocument.h"
#include "bkbookmark.h"
#include "bkpopup.h"

#include "bklocalization.h"

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
#define OPTIONS_MENU_ITEM_SET_MENU_LANGUAGE		1
#define OPTIONS_MENU_ITEM_SET_CONTROL_STYLE		2
#define OPTIONS_MENU_ITEM_PDF_FAST_IMAGES		3
#define OPTIONS_MENU_ITEM_PDF_INVERT_COLORS		4
#define OPTIONS_MENU_ITEM_PLAIN_CHOOSE_FONT		5
#define OPTIONS_MENU_ITEM_PLAIN_FONT_SIZE		6
#define OPTIONS_MENU_ITEM_PLAIN_SET_LINE_HEIGHT	7
#define OPTIONS_MENU_ITEM_PLAIN_JUSTIFY_TEXT	8
#define OPTIONS_MENU_ITEM_PLAIN_WRAP_TEXT		9
#define OPTIONS_MENU_ITEM_BROWSER_TEXTSIZE		10
#define OPTIONS_MENU_ITEM_BROWSER_DISPLAYMODE	11
#define OPTIONS_MENU_ITEM_BROWSER_ENABLEFLASH	12
#define OPTIONS_MENU_ITEM_BROWSER_INTERFACEMODE	13
#define OPTIONS_MENU_ITEM_BROWSER_CONFIRMEXIT	14
#define OPTIONS_MENU_ITEM_BROWSER_SHOWCURSOR	15
#define OPTIONS_MENU_ITEM_COLOR_SCHEMES			16
#define OPTIONS_MENU_ITEM_DISPLAY_LABELS		17
#define OPTIONS_MENU_ITEM_LOAD_LAST_FILE		18
#define OPTIONS_MENU_ITEM_CPU_BUS_SPEED			19
#define OPTIONS_MENU_ITEM_CPU_MENU_SPEED		20
#define OPTIONS_MENU_ITEM_CLEAR_BOOKMARKS		21

BKMainMenu::BKMainMenu() : mode(BKMM_MAIN), captureButton(false), frames(0) 
{
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
	//mainItems.push_back(BKMenuItem("Open file", "Select", 0));
	mainItems.clear();

	mainItems.push_back(BKMenuItem(BKLocalization::current.mainMenuItemOpenFile, BKLocalization::current.mainMenuItemSelect, 0));
	mainItems.push_back(BKMenuItem(BKLocalization::current.mainMenuItemBrowseChmCache, BKLocalization::current.mainMenuItemSelect, 0));
	mainItems.push_back(BKMenuItem(BKLocalization::current.mainMenuItemControls, BKLocalization::current.mainMenuItemSelect, 0));
	mainItems.push_back(BKMenuItem(BKLocalization::current.mainMenuItemOptions, BKLocalization::current.mainMenuItemSelect, 0));
	mainItems.push_back(BKMenuItem(BKLocalization::current.mainMenuItemAbout, BKLocalization::current.mainMenuItemSelect, 0));
	mainItems.push_back(BKMenuItem(BKLocalization::current.mainMenuItemExit, BKLocalization::current.mainMenuItemSelect, 0));
}

void BKMainMenu::buildControlMenu() {
	string cb(BKLocalization::current.controlMenuItemChangeButton);

	controlItems.clear();

	controlItems.push_back(BKMenuItem(BKLocalization::current.controlMenuItemRestoreDefaults, BKLocalization::current.controlMenuItemSelect, 0));

	string t(BKLocalization::current.controlMenuItemPreviousPage);
	t += FZScreen::nameForButtonReps(BKUser::controls.previousPage);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = BKLocalization::current.controlMenuItemNextPage;
	t += FZScreen::nameForButtonReps(BKUser::controls.nextPage);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = BKLocalization::current.controlMenuItemPrevious10Pages;
	t += FZScreen::nameForButtonReps(BKUser::controls.previous10Pages);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = BKLocalization::current.controlMenuItemNext10pages;
	t += FZScreen::nameForButtonReps(BKUser::controls.next10Pages);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = BKLocalization::current.controlMenuItemScreenUp;
	t += FZScreen::nameForButtonReps(BKUser::controls.screenUp);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = BKLocalization::current.controlMenuItemScreenDown;
	t += FZScreen::nameForButtonReps(BKUser::controls.screenDown);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = BKLocalization::current.controlMenuItemScreenLeft;
	t += FZScreen::nameForButtonReps(BKUser::controls.screenLeft);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = BKLocalization::current.controlMenuItemScreenRight;
	t += FZScreen::nameForButtonReps(BKUser::controls.screenRight);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = BKLocalization::current.controlMenuItemZoomIn;
	t += FZScreen::nameForButtonReps(BKUser::controls.zoomIn);
	controlItems.push_back(BKMenuItem(t, cb, 0));

	t = BKLocalization::current.controlMenuItemZoomOut;
	t += FZScreen::nameForButtonReps(BKUser::controls.zoomOut);
	controlItems.push_back(BKMenuItem(t, cb, 0));
}

void BKMainMenu::buildOptionMenu() 
{
	optionItems.clear();
	optionItems.push_back(BKMenuItem(BKLocalization::current.optionMenuItemRestoreDefaults, BKLocalization::current.optionMenuItemSelect, 0));

	// OZ Addon
	string strMenuLanguage(BKLocalization::current.optionMenuItemMenuLanguage);
	strMenuLanguage += BKUser::options.menuLanguage;

	optionItems.push_back(BKMenuItem(strMenuLanguage, BKLocalization::current.optionMenuItemChoose, BK_MENU_ITEM_USE_LR_ICON));
	//

	string t(BKLocalization::current.optionMenuItemMenuControlStyle);
	switch(BKUser::controls.select) 
	{
	case FZ_REPS_CIRCLE:	t.append(BKLocalization::current.optionMenuItemMenuControlStyleAsian);	break;
	case FZ_REPS_CROSS:		t.append(BKLocalization::current.optionMenuItemMenuControlStyleWestern); break;
	}
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemChoose, BK_MENU_ITEM_USE_LR_ICON));

	t = BKLocalization::current.optionMenuItemPdfFastImage;
	t += BKUser::options.pdfFastScroll ? BKLocalization::current.optionMenuItemEnabled : BKLocalization::current.optionMenuItemDisabled;
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemToggle, 0));

	t = BKLocalization::current.optionMenuItemPdfInvertColors;
	t += BKUser::options.pdfInvertColors ? BKLocalization::current.optionMenuItemEnabled : BKLocalization::current.optionMenuItemDisabled;
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemToggle, 0));

	t =  BKLocalization::current.optionMenuItemPlainTextFontFile;
	if (BKUser::options.txtFont == "bookr:builtin") 
	{
		t +=  BKLocalization::current.optionMenuItemPlainTextBuiltIn;
	} else 
	{
		int s = BKUser::options.txtFont.size();
		if (s > 25) 
		{
			t += "...";
			t += string(BKUser::options.txtFont, s - 25, 25);
		} else 
		{
			t += BKUser::options.txtFont;
		}
	}
	BKMenuItem mi = BKMenuItem(t,  BKLocalization::current.optionMenuItemSelectFontFile, BK_MENU_ITEM_OPTIONAL_TRIANGLE_LABEL);
	mi.triangleLabel = BKLocalization::current.optionMenuItemUseBuiltInFont;
	optionItems.push_back(mi);

	char txt[1024];
	snprintf(txt, 1024, "%s%d%s", BKLocalization::current.optionMenuItemPlainTextFontSize.c_str(), BKUser::options.txtSize, BKLocalization::current.optionMenuItemPixels.c_str());
	t = txt;
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemChoose, BK_MENU_ITEM_USE_LR_ICON));

	snprintf(txt, 1024, "%s%d%s", BKLocalization::current.optionMenuItemPlainTextLineHeight.c_str(), BKUser::options.txtHeightPct, BKLocalization::current.optionMenuItemPct.c_str());
	t = txt;
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemChange, BK_MENU_ITEM_USE_LR_ICON));

	t = BKLocalization::current.optionMenuItemPlainTextJustifyText;
	t += BKUser::options.txtJustify ? BKLocalization::current.optionMenuItemEnabled : BKLocalization::current.optionMenuItemDisabled;
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemToggle, 0));

	snprintf(txt, 1024, "%s%d", BKLocalization::current.optionMenuItemPlainTextWrapCRs.c_str(), BKUser::options.txtWrapCR);
	t = txt;
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemChange, BK_MENU_ITEM_USE_LR_ICON));

	/*char txt[1024];
	snprintf(txt, 1024, "Plain text - Rotation: %d\260", BKUser::options.txtRotation);
	t = txt;
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemToggle, 0));*/

	const char* browserTextSizes[3] = {BKLocalization::current.browserTextSizeLarge.c_str(),
		BKLocalization::current.browserTextSizeNormal.c_str(), 
		BKLocalization::current.browserTextSizeSmall.c_str()};

	snprintf(txt, 1024, "%s%s", BKLocalization::current.optionMenuItemBrowserTextSize.c_str(),  
		browserTextSizes[BKUser::options.browserTextSize]);
	t = txt;
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemChange, BK_MENU_ITEM_USE_LR_ICON));

	const char* browserDisplayModes[3] = {BKLocalization::current.browserDisplayModesNormal.c_str(),
		BKLocalization::current.browserDisplayModesFit.c_str(), 
		BKLocalization::current.browserDisplayModesSmartFit.c_str()};

	snprintf(txt, 1024, "%s%s", BKLocalization::current.optionMenuItemBrowserDisplayMode.c_str(),  
		browserDisplayModes[BKUser::options.browserDisplayMode]);
	t = txt;
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemChange, BK_MENU_ITEM_USE_LR_ICON));

	t = BKLocalization::current.optionMenuItemBrowserFlashContent;
	t += BKUser::options.browserEnableFlash ? BKLocalization::current.optionMenuItemEnabled : BKLocalization::current.optionMenuItemDisabled;
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemToggle, 0));

	const char* browserInterfaceModes[3] = {BKLocalization::current.browserInterfaceModesFull.c_str(),
		BKLocalization::current.browserInterfaceModesLimited.c_str(), 
		BKLocalization::current.browserInterfaceModesNone.c_str()};

	snprintf(txt, 1024, "%s%s", BKLocalization::current.optionMenuItemBrowserInterfaceMode.c_str(), 
		browserInterfaceModes[BKUser::options.browserInterfaceMode]);
	t = txt;
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemChange, BK_MENU_ITEM_USE_LR_ICON));

	t = BKLocalization::current.optionMenuItemBrowserExitConfirmation;
	t += BKUser::options.browserConfirmExit ? BKLocalization::current.optionMenuItemEnabled : BKLocalization::current.optionMenuItemDisabled;
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemToggle, 0));

	t = BKLocalization::current.optionMenuItemBrowserShowCursor;
	t += BKUser::options.browserShowCursor ? BKLocalization::current.optionMenuItemEnabled : BKLocalization::current.optionMenuItemDisabled;
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemToggle, 0));

	t = BKLocalization::current.optionMenuItemColorSchemes;
	mi = BKMenuItem(t, BKLocalization::current.optionMenuItemChoose, BK_MENU_ITEM_OPTIONAL_TRIANGLE_LABEL | BK_MENU_ITEM_USE_LR_ICON | BK_MENU_ITEM_COLOR_RECT);
	mi.triangleLabel = BKLocalization::current.optionMenuItemManageSchemes;
	mi.bgcolor = BKUser::options.colorSchemes[BKUser::options.currentScheme].txtBGColor;
	mi.fgcolor = BKUser::options.colorSchemes[BKUser::options.currentScheme].txtFGColor;
	optionItems.push_back(mi);

	t = BKLocalization::current.optionMenuItemDisplayPageLoading;
	t += BKUser::options.displayLabels ? BKLocalization::current.optionMenuItemEnabled : BKLocalization::current.optionMenuItemDisabled;
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemToggle, 0));

	t = BKLocalization::current.optionMenuItemAutoLoadLastFile;
	t += BKUser::options.loadLastFile ? BKLocalization::current.optionMenuItemEnabled : BKLocalization::current.optionMenuItemDisabled;
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemToggle, 0));

	snprintf(txt, 1024, "%s%s", BKLocalization::current.optionMenuItemCPUSpeed.c_str(), FZScreen::speedLabels[BKUser::options.pspSpeed]);
	t = txt;
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemChange, BK_MENU_ITEM_USE_LR_ICON));
	snprintf(txt, 1024, "%s%s", BKLocalization::current.optionMenuItemMenuSpeed.c_str(), FZScreen::speedLabels[BKUser::options.pspMenuSpeed]);
	t = txt;
	optionItems.push_back(BKMenuItem(t, BKLocalization::current.optionMenuItemChange, BK_MENU_ITEM_USE_LR_ICON));

	optionItems.push_back(BKMenuItem(BKLocalization::current.optionMenuItemClearBookmarks, BKLocalization::current.optionMenuItemSelect, 0));

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
			popupText = "Bookr - a document viewer for the Sony PSP.\nOriginal version by Carlos and Edward.\nV8.2 OZ Edition by Oleg Zhuk <zhukoo@gmail.com>\nThis program is licensed under the terms of the GPL v2.\nThis program uses the following libraries/projects:\n MuPDF library under the terms of the AFPL.\n chmlib 0.39 by Jed Wing <jedwin@ugcs.caltech.edu>\n VnConv by Pham Kim Long <longp@cslab.felk.cvut.cz>\n DjVuLibre 3.5 by Leon Bottou and Yann Le Cun";
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


void BKMainMenu::rebuildMenuFontAndText()
{
	BKLocalization::loadMenuFont();

	buildOptionMenu();
	buildControlMenu();
	buildMainMenu();
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
			popupText = BKLocalization::current.pdfFastWarning;
			popupMode = BKPOPUP_WARNING;
			return BKUser::options.pdfFastScroll ? BK_CMD_MAINMENU_POPUP : BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_PLAIN_CHOOSE_FONT) {
			return BK_CMD_INVOKE_OPEN_FONT;
		}
		if (selItem == OPTIONS_MENU_ITEM_PLAIN_JUSTIFY_TEXT) {
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
		if (selItem == OPTIONS_MENU_ITEM_PDF_INVERT_COLORS) {
			BKUser::options.pdfInvertColors = !BKUser::options.pdfInvertColors;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_CLEAR_BOOKMARKS) {
			BKBookmarksManager::clear();
			popupText = BKLocalization::current.bookmarksClearedWarning;
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

		if (selItem == OPTIONS_MENU_ITEM_BROWSER_ENABLEFLASH) {
			BKUser::options.browserEnableFlash = !BKUser::options.browserEnableFlash;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_BROWSER_CONFIRMEXIT) {
			BKUser::options.browserConfirmExit = !BKUser::options.browserConfirmExit;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_BROWSER_SHOWCURSOR) {
			BKUser::options.browserShowCursor = !BKUser::options.browserShowCursor;
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
	}

	// controls.alternate
	if (b[BKUser::controls.alternate] == 1) {
		if (selItem == OPTIONS_MENU_ITEM_PLAIN_CHOOSE_FONT) {
			BKUser::options.txtFont = "bookr:builtin";
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
		if (selItem == OPTIONS_MENU_ITEM_COLOR_SCHEMES) {
			return BK_CMD_INVOKE_COLOR_SCHEME_MANAGER;
		}
	}

	// controls.cancel
	if (b[BKUser::controls.cancel] == 1) {
		selItem = 0;
		topItem = 0;
		mode = BKMM_MAIN;
		BKUser::save();
		return BK_CMD_MARK_DIRTY;
	}

	// controls.showMainMenu
	if (b[BKUser::controls.showMainMenu] == 1) {
		selItem = 0;
		topItem = 0;
		mode = BKMM_MAIN;
		BKUser::save();
		return BK_CMD_CLOSE_TOP_LAYER;
	}

	// controls.menuLeft
	if (b[BKUser::controls.menuLeft] == 1) {
		if(selItem == OPTIONS_MENU_ITEM_SET_MENU_LANGUAGE)
		{
			for(int index = 0; index<BKLocalization::availableLanguages.size(); index++)
			{
				BKLocalization::Resources resource = BKLocalization::availableLanguages[index];

				if(resource.language == BKUser::options.menuLanguage)
				{
					index--;
					if(index<0)
						index = BKLocalization::availableLanguages.size()-1;
					
					BKLocalization::current = BKLocalization::availableLanguages[index];
					BKUser::options.menuLanguage = BKLocalization::current.language;

					rebuildMenuFontAndText();
					break;
				}
			}
			return BK_CMD_MARK_DIRTY;
		}
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
		if (selItem == OPTIONS_MENU_ITEM_PLAIN_FONT_SIZE) {
			--BKUser::options.txtSize;
			if (BKUser::options.txtSize < 6) {
				BKUser::options.txtSize = 6;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_PLAIN_SET_LINE_HEIGHT) {
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
		} else if (selItem == OPTIONS_MENU_ITEM_PLAIN_WRAP_TEXT) {
			--BKUser::options.txtWrapCR;
			if (BKUser::options.txtWrapCR < 0) {
				BKUser::options.txtWrapCR = 0;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_BROWSER_TEXTSIZE) {
			--BKUser::options.browserTextSize;
			if (BKUser::options.browserTextSize < 0) {
				BKUser::options.browserTextSize = 2;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_BROWSER_DISPLAYMODE) {
			--BKUser::options.browserDisplayMode;
			if (BKUser::options.browserDisplayMode < 0) {
				BKUser::options.browserDisplayMode = 2;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_BROWSER_INTERFACEMODE) {
			--BKUser::options.browserInterfaceMode;
			if (BKUser::options.browserInterfaceMode < 0) {
				BKUser::options.browserInterfaceMode = 2;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
	}

	// controls.menuRight
	if (b[BKUser::controls.menuRight] == 1) {
		if(selItem == OPTIONS_MENU_ITEM_SET_MENU_LANGUAGE)
		{
			for(int index = 0; index<BKLocalization::availableLanguages.size(); index++)
			{
				BKLocalization::Resources resource = BKLocalization::availableLanguages[index];

				if(resource.language == BKUser::options.menuLanguage)
				{
					index++;
					if(index>=BKLocalization::availableLanguages.size())
						index = 0;
					
					BKLocalization::current = BKLocalization::availableLanguages[index];
					BKUser::options.menuLanguage = BKLocalization::current.language;

					rebuildMenuFontAndText();
					break;
				}
			}
			return BK_CMD_MARK_DIRTY;
		}
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
		if (selItem == OPTIONS_MENU_ITEM_PLAIN_FONT_SIZE) {
			++BKUser::options.txtSize;
			if (BKUser::options.txtSize > 20) {
				BKUser::options.txtSize = 20;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_PLAIN_SET_LINE_HEIGHT) {
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
		} else if (selItem == OPTIONS_MENU_ITEM_PLAIN_WRAP_TEXT) {
			++BKUser::options.txtWrapCR;
			if (BKUser::options.txtWrapCR > 3) {
				BKUser::options.txtWrapCR = 3;
			} else {
				buildOptionMenu();
			}
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_BROWSER_TEXTSIZE) {
			++BKUser::options.browserTextSize;
			if (BKUser::options.browserTextSize > 2) {
				BKUser::options.browserTextSize = 0;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_BROWSER_DISPLAYMODE) {
			++BKUser::options.browserDisplayMode;
			if (BKUser::options.browserDisplayMode > 2) {
				BKUser::options.browserDisplayMode = 0;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		} else if (selItem == OPTIONS_MENU_ITEM_BROWSER_INTERFACEMODE) {
			++BKUser::options.browserInterfaceMode;
			if (BKUser::options.browserInterfaceMode > 2) {
				BKUser::options.browserInterfaceMode = 0;
			}
			buildOptionMenu();
			return BK_CMD_MARK_DIRTY;
		}
	}


	return 0;
}

void BKMainMenu::render() {
	string t("");
	if (mode == BKMM_MAIN) {
		string title(BKLocalization::current.mainMenuTitle);
		drawMenu(title, t, mainItems);
		string e;
		drawClockAndBattery(e);
	} else if (mode == BKMM_CONTROLS) {
		string title(BKLocalization::current.controlMenuTitle);
		drawMenu(title, t, controlItems);
	} else if (mode == BKMM_OPTIONS) {
		string title(BKLocalization::current.optionMenuTitle);
		drawMenu(title, t, optionItems);
	}
	if (captureButton) 
	{
		texUI->bindForDisplay();
		FZScreen::ambientColor(0xf0dddddd);
		drawPill(150, 126, 180, 20, 6, 31, 1);
		fontBig->bindForDisplay();
		FZScreen::ambientColor(0xff222222);

		char txt[1024];
		snprintf(txt, 1024, "%s", BKLocalization::current.pressNewButtonWarning.c_str());
		drawTextHC(txt, fontBig, 130);
	}
}

BKMainMenu* BKMainMenu::create() {
	BKMainMenu* f = new BKMainMenu();
	FZScreen::resetReps();
	return f;
}

