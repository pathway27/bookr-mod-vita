/*
 * bookr-modern: a graphics based document reader 
 * Copyright (C) 2019 pathway27 (Sree)
 * IS A MODIFICATION OF THE ORIGINAL
 * Bookr and bookr-mod for PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com),
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com),
 * AND VARIOUS OTHER FORKS, See Forks in README.md
 * Licensed under GPLv3+, see LICENSE
*/

#ifndef BKFILECHOOSER_H
#define BKFILECHOOSER_H

#include "graphics/screen.hpp"
#include "layer.hpp"

using std::string;

namespace bookr {

class FileChooser : public Layer {
	bool convertToVN;
	string path;
	string title;
	int ret;
	vector<Dirent> dirFiles;
	void updateDirFiles();

	protected:
	FileChooser(string& t, int r);
	~FileChooser();

	public:
	virtual int update(unsigned int buttons);
	virtual void render();

	void getCurrentDirent(Dirent& de);
	void getFullPath(string& s);
	void getFileName(string& s);
	bool isConvertToVN();

	static FileChooser* create(string& t, int r);
};

}

#endif