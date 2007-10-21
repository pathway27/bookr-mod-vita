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

#include "bklogo.h"

BKLogo::BKLogo() : loading(false), error(false) {
}

BKLogo::~BKLogo() {
}

void BKLogo::setLoading(bool v) {
	loading = v;
}

void BKLogo::setError(bool v) {
	error = v;
}

int BKLogo::update(unsigned int buttons) {
	int* b = FZScreen::ctrlReps();
	if (b[BKUser::controls.showMainMenu] == 1) {
		return BK_CMD_INVOKE_MENU;
	}
	return 0;
}

void BKLogo::render() {
	FZScreen::clear(0xffffff, FZ_COLOR_BUFFER);
	FZScreen::color(0xffffffff);
	FZScreen::matricesFor2D();
	FZScreen::enable(FZ_TEXTURE_2D);
	FZScreen::disable(FZ_BLEND);
	texLogo->bindForDisplay();
	drawImage(480/2 - 64, 50, 128, 128, 0, 0);
	FZScreen::enable(FZ_BLEND);
	FZScreen::blendFunc(FZ_ADD, FZ_SRC_ALPHA, FZ_ONE_MINUS_SRC_ALPHA);
	texUI->bindForDisplay();
	FZScreen::ambientColor(0xf0222222);
	drawPill(150, 240, 180, 20, 6, 31, 1);
	fontBig->bindForDisplay();
	FZScreen::ambientColor(0xff000000);
	drawTextHC("Version 0.7.1 FG-PSM-DJVU", fontBig, 180);
	FZScreen::ambientColor(0xffffffff);
	if (loading)
		drawTextHC("Loading...", fontBig, 244);
	else {
		if (error) {
			texUI->bindForDisplay();
			FZScreen::ambientColor(0xf06060ff);
			drawRect(0, 126, 480, 26, 6, 31, 1);
			fontBig->bindForDisplay();
			FZScreen::ambientColor(0xff222222);
			drawTextHC("Error: invalid or corrupted file", fontBig, 130);
		}
		FZScreen::ambientColor(0xffffffff);
		drawTextHC("Press Start", fontBig, 244);
	}
}

BKLogo* BKLogo::create() {
	BKLogo* f = new BKLogo();
	FZScreen::resetReps();
	return f;
}

