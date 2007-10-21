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

#ifndef BKUSER_H
#define BKUSER_H

#include <vector>

class BKUser {
	private:
	static void load();

	public:
	static void init();
	static void save();
	static void setDefaultControls();
	static void setDefaultOptions();

	struct Controls {
		// in-book controls
		int previousPage;
		int nextPage;
		int previous10Pages;
		int next10Pages;
		int screenUp;
		int screenDown;
		int screenLeft;
		int screenRight;
		int zoomIn;
		int zoomOut;
		int showToolbar;	// Start
		int showMainMenu;	// Select		
		
		// menu controls
		int select;		// Circle or Cross
		int cancel;		// Cross or Circle
		int alternate;	// Triangle
		int details;	// Square - not currently used
		int menuUp;
		int menuDown;
		int menuLeft;
		int menuRight;
		int resume;		// Start
	};
	
	static Controls controls;
	
	struct ColorScheme {
		int	txtFGColor;
		int txtBGColor;
	};
	
	struct Options {
		bool pdfFastScroll;

		// why it is a pref and not just a button command: the current text viewer
		// needs to repaginate the whole document when rotating it, so it is an
		// expensive operation. by offering the feature as an option we make it
		// clear to the user that it is just not possible to rotate on the fly,
		// unlike with the pdf viewer.
		int txtRotation;

		string txtFont;
		int txtSize;
		vector<ColorScheme> colorSchemes;
		int currentScheme;
		bool txtJustify;
		int pspSpeed;
		int pspMenuSpeed;
		bool displayLabels;
		bool pdfInvertColors;
		string lastFolder;
		string lastFontFolder;
		int txtHeightPct;
		bool loadLastFile;
		int txtWrapCR;
	};
	static Options options;
};

#endif

