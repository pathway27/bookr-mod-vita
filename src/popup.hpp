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

#ifndef BKPOPUP_H
#define BKPOPUP_H

#include "graphics/screen.hpp"
#include "layer.hpp"


using std::string;

namespace bookr {

#define BKPOPUP_WARNING		1
#define BKPOPUP_ERROR		2
#define BKPOPUP_INFO		3

class Popup : public Layer {
	int mode;
	string text;

protected:
	Popup(int m, string t);	
	~Popup();

public:
	virtual int update(unsigned int buttons);
	virtual void render();
	
	static Popup* create(int m, string t);
};

}

#endif

