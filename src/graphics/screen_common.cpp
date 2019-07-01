/*
 * Bookr % VITA: document reader for the Sony PS Vita
 * Copyright (C) 2017 Sreekara C. (pathway27 at gmail dot com)
 *
 * IS A MODIFICATION OF THE ORIGINAL
 *
 * Bookr and bookr-mod for PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com),
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com),
 
 * AND VARIOUS OTHER FORKS.
 * See Forks in the README for more info
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "screen.hpp"
#include "controls.hpp"

namespace bookr { namespace Screen {

const char* nameForButton(int mask) {
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
#ifdef __vita__
	if (mask & FZ_CTRL_L3	      ) return "L3";
	if (mask & FZ_CTRL_R3	      ) return "R3";
	if (mask & FZ_CTRL_VOLUP      ) return "Vol UP";
	if (mask & FZ_CTRL_VOLDOWN    ) return "Vol Down";
	if (mask & FZ_CTRL_INTERCEPTED) return "Intercepted";
#endif
	return "Unknow button";
}

const char* nameForButtonReps(int button) {
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

int repsForButtonMask(int mask) {
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

// char *speedLabels = {
// 	"Default",
// 	"10Mhz/5Mhz",
// 	"25Mhz/12Mhz",
// 	"50Mhz/25Mhz",
// 	"111Mhz/55Mhz",
// 	"222Mhz/111Mhz",
// 	"333Mhz/166Mhz"
// };

// int speedValues[14] = {
// 	0, 0,
// 	10, 5,
// 	25, 12,
// 	50, 25,
// 	111, 55, 
// 	222, 111,
// 	333, 166
// };

// const char* browserTextSizes[] =
// {
// 	"Large", "Normal", "Small"
// };

// const char* browserDisplayModes[] =
// {
// 	"Normal", "Fit", "Smart-Fit"
// };

// const char* browserInterfaceModes[3] =
// {
// 	"Full", "Limited", "None"
// };

}}