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

#include "bkpopup.h"

BKPopup::BKPopup(int m, string t) : mode(m), text(t) {
}

BKPopup::~BKPopup() {
}

int BKPopup::update(unsigned int buttons) {
	int* b = FZScreen::ctrlReps();

	if (b[FZ_REPS_CROSS] == 1) {
			return BK_CMD_CLOSE_TOP_LAYER;
	}

	return 0;
}
		
void BKPopup::render() {
	string title;
	if (mode == BKPOPUP_WARNING)
		title = "Warning";
	else
		title = "Error";
	drawPopup(text, title, 0xf02020a0, 0xf06060ff, 0xffffffff);
}

BKPopup* BKPopup::create(int m, string t) {
	BKPopup* f = new BKPopup(m, t);
	FZScreen::resetReps();
	return f;
}

