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

#ifndef BKUSER_H
#define BKUSER_H

class BKUser {
	private:
	static void load();

	public:
	static void init();
	static void save();
	static void setDefaultControls();
	static void setDefaultOptions();

	struct Controls {
		int previousPage;
		int nextPage;
		int previous10Pages;
		int next10Pages;
		int panUp;
		int panDown;
		int panLeft;
		int panRight;
		int zoomIn;
		int zoomOut;
	};
	static Controls pdfControls;
	static Controls txtControls;

	struct Options {
		bool pdfFastScroll;
	};
	static Options options;
};

#endif

