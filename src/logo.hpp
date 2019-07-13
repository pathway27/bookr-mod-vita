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

#ifndef BKLOGO_H
#define BKLOGO_H

#include "layer.hpp"
#include "graphics/screen.hpp"

using std::string;

namespace bookr {

class Logo : public Layer {
	bool loading;
	bool error;
	string text;

protected:
	Logo();
	~Logo();

public:
	virtual int update(unsigned int buttons);
	virtual void render();
	void setLoading(bool v);
	void setError(bool err);
	void setError(bool err, string message);

	static Logo* create();
	static void show(string text);
	static void show(string text, int delaySeconds);
};

}

#endif
