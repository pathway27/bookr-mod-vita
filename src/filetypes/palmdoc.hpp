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

#ifndef BKPALMDOC_H
#define BKPALMDOC_H

#include "../graphics/screen.hpp"
#include "../fancytext.h"

using std::string;

namespace bookr {

class PalmDoc : public FancyText {
private:
	string fileName;
	string title;
	char* buffer;

protected:
	PalmDoc();
	~PalmDoc();

public:
	virtual void getFileName(string&);
	virtual void getTitle(string&,int type = 0);
	virtual void getType(string&);

	static PalmDoc* create(string& file,string& longFileName);
	static bool isPalmDoc(string& file);
};

}

#endif