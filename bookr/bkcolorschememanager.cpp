/*
 * Bookr: document reader for the Sony PSP 
 * Copyright (C) 2007 Christian Payeur (christian dot payeur at gmail dot com)
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

#include "stdio.h"
#include "bkcolorschememanager.h"
#include "bkuser.h"

BKColorSchemeManager::BKColorSchemeManager(string& t) : title(t) {
    selectedColorSchemes = & (BKUser::options.colorSchemes);
    selectedCurrentScheme = & (BKUser::options.currentScheme);
//    csmType = 0;
    bk_cmd_invoke_color_chooser = BK_CMD_INVOKE_COLOR_CHOOSER_TXTFG;
}

BKColorSchemeManager::BKColorSchemeManager(string& t, int type) : title(t) {
//    csmType = type;
    switch (type){

	case BK_CMD_INVOKE_THUMBNAIL_COLOR_SCHEME_MANAGER:
	    selectedColorSchemes = &(BKUser::options.thumbnailColorSchemes);
	    selectedCurrentScheme = &(BKUser::options.currentThumbnailScheme);
	    bk_cmd_invoke_color_chooser = BK_CMD_INVOKE_COLOR_CHOOSER_TNFG;
	    break;

	case BK_CMD_INVOKE_COLOR_SCHEME_MANAGER:
	default:
	    selectedColorSchemes = &(BKUser::options.colorSchemes);
	    selectedCurrentScheme = &(BKUser::options.currentScheme);
	    bk_cmd_invoke_color_chooser = BK_CMD_INVOKE_COLOR_CHOOSER_TXTFG;
	    break;
    }



}


BKColorSchemeManager::~BKColorSchemeManager() {
		BKUser::save();
}

int BKColorSchemeManager::update(unsigned int buttons) {

	menuCursorUpdate(buttons, selectedColorSchemes->size()+1);
	
	int* b = FZScreen::ctrlReps();
	
	if (b[BKUser::controls.select] == 1) {
		if ((unsigned int) selItem == selectedColorSchemes->size() &&
			selItem < 100) {
			BKUser::ColorScheme newScheme;
			newScheme.txtBGColor = 0xDDDDDD;
			newScheme.txtFGColor = 0x333333;
			selectedColorSchemes->push_back(newScheme);
			BKUser::save();
		} else if (selItem < 100) {
			colorScheme = selItem;
			return (bk_cmd_invoke_color_chooser);
		}
	}
	if (b[BKUser::controls.alternate] == 1) {
		
		int numSchemes = selectedColorSchemes->size();
		if (numSchemes > 1 &&			// Must have at least one scheme
			selItem < numSchemes) {		// The last item in the menu is not a scheme
				
			// Locate and delete the scheme
			vector<BKUser::ColorScheme>::iterator csIt = selectedColorSchemes->begin();
			for (int i = 0; i < selItem; i++) {
				csIt++;
			}
			selectedColorSchemes->erase(csIt);
				
			// If the current scheme was the deleted scheme or above, 
			// we adjust the current scheme.
			if (*selectedCurrentScheme >= selItem &&
				selItem > 0) {// but we do nothing in case we just deleted the first scheme
				
			    *selectedCurrentScheme = *selectedCurrentScheme - 1;
			}
			
			BKUser::save();
			return BK_CMD_MARK_DIRTY;
		}
	}
	if (b[BKUser::controls.cancel] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}
	if (b[BKUser::controls.showMainMenu] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}
	return 0;
}

void BKColorSchemeManager::render() {
	vector<BKMenuItem> items;
	string cl("Modify");
	string tl("");

	int n = selectedColorSchemes->size();
	for (int i = 0; i < n; i++) {
		string itemName("Color Scheme");
		char index[6] = "";
		sprintf (index," %d: ", i+1);
		itemName += index;
		
		BKMenuItem menuItem = BKMenuItem(itemName, cl, BK_MENU_ITEM_OPTIONAL_TRIANGLE_LABEL | BK_MENU_ITEM_COLOR_RECT);
		menuItem.bgcolor = (*selectedColorSchemes)[i].txtBGColor;
		menuItem.fgcolor = (*selectedColorSchemes)[i].txtFGColor;
		menuItem.triangleLabel = "Delete";
		items.push_back(menuItem);
	}
	
	BKMenuItem addSchemeMenuItem = BKMenuItem("Add a color scheme...", "Add", 0);
	items.push_back(addSchemeMenuItem);
	drawMenu(title, tl, items);
}

int BKColorSchemeManager::getColorScheme() {
	return colorScheme;
}

BKColorSchemeManager* BKColorSchemeManager::create(string& t) {
	BKColorSchemeManager* f = new BKColorSchemeManager(t);
	FZScreen::resetReps();
	return f;
}

BKColorSchemeManager* BKColorSchemeManager::create(string& t, int type) {
        BKColorSchemeManager* f = new BKColorSchemeManager(t,type);
	FZScreen::resetReps();
	return f;
}
