/*
 * bkpagechooser: random page accessing extension for bookr 
 * Copyright (C) 2007 Yang.Hu (findreams at gmail dot com)
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

#include "bkpagechooser.h"
#include "bkuser.h"

static int getNumOfDigit(int num) {
	int ret;
	if(num == 0)
		return 1;
	for(ret = 0; num != 0; ret++){
		num /= 10;
	}
	return ret;
}

BKPageChooser::BKPageChooser(int t, int c, int r) : totalPage(t), currentPage(c), ret(r) {
	if (currentPage < 1){
		currentPage = 1;
	}
	if (currentPage > totalPage){
		currentPage = totalPage;
	}
	numOfDigit = getNumOfDigit(totalPage);
	currentPosition = 0;
	invalidPage = 0;
}

BKPageChooser::~BKPageChooser() {
}

int BKPageChooser::getCurrentPage() {
	return currentPage;
}

int BKPageChooser::update(unsigned int buttons) {
	int* b = FZScreen::ctrlReps();
	if (b[BKUser::controls.menuLeft] == 1 || b[BKUser::controls.menuLeft] > 20) {
		if(++currentPosition >= numOfDigit)
			currentPosition = 0;
		return BK_CMD_MARK_DIRTY;
	}
	if (b[BKUser::controls.menuRight] == 1 || b[BKUser::controls.menuRight] > 20) {
		if(--currentPosition < 0)
			currentPosition = numOfDigit-1;
		return BK_CMD_MARK_DIRTY;
	}
	if (b[BKUser::controls.menuUp] == 1 || b[BKUser::controls.menuUp] > 20) {
		int numChange = currentPage;
		int newPage = 0;
		for(int i = 0; i < currentPosition; ++i)
			numChange /= 10;
		numChange %= 10;
		if(--numChange < 0) numChange = 9;
		int power = 1;
		for(int i = 0; i < numOfDigit; ++i) {
			if(i != currentPosition)
				newPage += (currentPage / power) % 10 * power;
			else
				newPage += numChange * power;
			power *= 10;
		}
		currentPage = newPage;
		if(currentPage == 0 || currentPage > totalPage) {
			invalidPage = 1;
		}else{
			invalidPage = 0;
		}
		return BK_CMD_MARK_DIRTY;
	}
	if (b[BKUser::controls.menuDown] == 1 || b[BKUser::controls.menuDown] > 20) {
		int numChange = currentPage;
		int newPage = 0;
		for(int i = 0; i < currentPosition; ++i)
			numChange /= 10;
		numChange %= 10;
		if(++numChange > 9) numChange = 0;
		int power = 1;
		for(int i = 0; i < numOfDigit; ++i) {
			if(i != currentPosition)
				newPage += (currentPage / power) % 10 * power;
			else
				newPage += numChange * power;
			power *= 10;
		}
		currentPage = newPage;
		if(currentPage == 0 || currentPage > totalPage) {
			invalidPage = 1;
		}else{
			invalidPage = 0;
		}
		return BK_CMD_MARK_DIRTY;
	}
	if (b[BKUser::controls.cancel] == 1) {
		if(currentPage == 0 || currentPage > totalPage) {
			return 0;
		}
		return ret;
	}
	if (b[BKUser::controls.select] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}
	if (b[BKUser::controls.showToolbar] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}
	if (b[BKUser::controls.showMainMenu] == 1) {
		return BK_CMD_INVOKE_MENU;
	}
	return 0;
}

void BKPageChooser::render() {
	string title("Select page");
	string tl("");
	string cl("Open page");
	drawDialogFrame(title, tl, cl, 0);
	
	fontBig->bindForDisplay();
	
	int textY = 140;
	int textX = 280;
	int textXstep = 10;
	char strNumber[20];
	
	if(invalidPage) {
		memcpy(strNumber, "Invalid page Number", 20);
		FZScreen::ambientColor(0xffffffff);
		drawText(strNumber, fontBig, textX-120, textY - 50);
	}
	
	memcpy(strNumber, "Go to:", 7);
	FZScreen::ambientColor(0xffffffff);
	drawText(strNumber, fontBig, textX-180, textY);
	
	
	snprintf(strNumber, 10, "%d", currentPage);
	int numOfCurrent = getNumOfDigit(currentPage);
	char tempStr[2];
	tempStr[1] = '\0';
	for(int position = 0; position < numOfDigit; ++position) {
		FZScreen::ambientColor(0xffffffff);
		int i = numOfCurrent - 1 - position;
		if(i >= 0){
			tempStr[0] = strNumber[i];
		}else{
			tempStr[0] = '0';
		}
		if(position == currentPosition){
			FZScreen::ambientColor(0xff0000ff);
		}
		drawText(tempStr, fontBig, textX - 20 - textXstep*position, textY);
	}
	
	memcpy(strNumber, "of ", 3);
	snprintf(strNumber+3, 10, "%d", totalPage);
	memcpy(strNumber+strlen(strNumber), " pages", 7);
	FZScreen::ambientColor(0xffffffff);
	drawText(strNumber, fontBig, textX, textY);
}

BKPageChooser* BKPageChooser::create(int t, int c, int r) {
	BKPageChooser* p = new BKPageChooser(t, c, r);
	FZScreen::resetReps();
	return p;
}

