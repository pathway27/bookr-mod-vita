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

#include "fzscreen.h"

const char* FZScreen::nameForButton(int mask) {
	if (mask & FZ_CTRL_SELECT  ) return "Select";
	if (mask & FZ_CTRL_START   ) return "Start";
	if (mask & FZ_CTRL_UP      ) return "Up";
	if (mask & FZ_CTRL_RIGHT   ) return "Right";
	if (mask & FZ_CTRL_DOWN    ) return "Down";
	if (mask & FZ_CTRL_LEFT    ) return "Left";
	if (mask & FZ_CTRL_LTRIGGER) return "Left trigger";
	if (mask & FZ_CTRL_RTRIGGER) return "Right trigger";
	if (mask & FZ_CTRL_TRIANGLE) return "Triangle";
	if (mask & FZ_CTRL_CIRCLE  ) return "Circle";
	if (mask & FZ_CTRL_CROSS   ) return "Cross";
	if (mask & FZ_CTRL_SQUARE  ) return "Square";
	if (mask & FZ_CTRL_HOME    ) return "Home";
	if (mask & FZ_CTRL_HOLD    ) return "Hold";
	if (mask & FZ_CTRL_NOTE    ) return "Note";
	return "Unknow button";
}

const char* FZScreen::nameForButtonReps(int button) {
	if (button == FZ_REPS_SELECT  ) return "Select";
	if (button == FZ_REPS_START   ) return "Start";
	if (button == FZ_REPS_UP      ) return "Up";
	if (button == FZ_REPS_RIGHT   ) return "Right";
	if (button == FZ_REPS_DOWN    ) return "Down";
	if (button == FZ_REPS_LEFT    ) return "Left";
	if (button == FZ_REPS_LTRIGGER) return "Left trigger";
	if (button == FZ_REPS_RTRIGGER) return "Right trigger";
	if (button == FZ_REPS_TRIANGLE) return "Triangle";
	if (button == FZ_REPS_CIRCLE  ) return "Circle";
	if (button == FZ_REPS_CROSS   ) return "Cross";
	if (button == FZ_REPS_SQUARE  ) return "Square";
	if (button == FZ_REPS_HOME    ) return "Home";
	if (button == FZ_REPS_HOLD    ) return "Hold";
	if (button == FZ_REPS_NOTE    ) return "Note";
	return "Unknow button";
}

int FZScreen::repsForButtonMask(int mask) {
	if (mask & FZ_CTRL_SELECT  ) return FZ_REPS_SELECT;
	if (mask & FZ_CTRL_START   ) return FZ_REPS_START;
	if (mask & FZ_CTRL_UP      ) return FZ_REPS_UP;
	if (mask & FZ_CTRL_RIGHT   ) return FZ_REPS_RIGHT;
	if (mask & FZ_CTRL_DOWN    ) return FZ_REPS_DOWN;
	if (mask & FZ_CTRL_LEFT    ) return FZ_REPS_LEFT;
	if (mask & FZ_CTRL_LTRIGGER) return FZ_REPS_LTRIGGER;
	if (mask & FZ_CTRL_RTRIGGER) return FZ_REPS_RTRIGGER;
	if (mask & FZ_CTRL_TRIANGLE) return FZ_REPS_TRIANGLE;
	if (mask & FZ_CTRL_CIRCLE  ) return FZ_REPS_CIRCLE;
	if (mask & FZ_CTRL_CROSS   ) return FZ_REPS_CROSS;
	if (mask & FZ_CTRL_SQUARE  ) return FZ_REPS_SQUARE;
	if (mask & FZ_CTRL_HOME    ) return FZ_REPS_HOME;
	if (mask & FZ_CTRL_HOLD    ) return FZ_REPS_HOLD;
	if (mask & FZ_CTRL_NOTE    ) return FZ_REPS_NOTE;
	return 0;
}

char* FZScreen::speedLabels[7] = {
	"Default",
	"10Mhz/5Mhz",
	"25Mhz/12Mhz",
	"50Mhz/25Mhz",
	"111Mhz/55Mhz",
	"222Mhz/111Mhz",
	"333Mhz/166Mhz"
};

int FZScreen::speedValues[14] = {
	0, 0,
	10, 5,
	25, 12,
	50, 25,
	111, 55, 
	222, 111,
	333, 166
};

