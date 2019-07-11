/*
 * Bookr: document reader for the Sony PSP 
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com)
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com)
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

#ifndef BKLOGO_H
#define BKLOGO_H

#include "layer.hpp"
#include "graphics/screen.hpp"
#include "graphics/shaders/shader.hpp"

using std::string;

namespace bookr {

class Logo : public Layer {
	bool loading;
	bool error;
	string text;
	Shader recShader;

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
