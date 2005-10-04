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
#include <math.h>
#include "fzscreen.h"

#include "bkcolorchooser.h"


BKColorChooser::BKColorChooser(int c) : color(c), hueY(0), svX(0), svY(0), hueTex(0) {
}

BKColorChooser::~BKColorChooser() {
	hueTex->release();
}

int BKColorChooser::getColor() {
	return color;
}

int BKColorChooser::update(unsigned int buttons) {
	int* b = FZScreen::ctrlReps();
	if (b[FZ_REPS_CROSS] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}
	return 0;
}

static void RGBtoHSV(float r, float g, float b, float *h, float *s, float *v);
static void HSVtoRGB(float *r, float *g, float *b, float h, float s, float v);

void BKColorChooser::render() {
	string title("Select color");
	string tl("");
	string cl("Switch to hue selection");
	drawDialogFrame(title, tl, cl, 0);

	const int x = 45;
	const int y = 75;
	const int s = 145;

	hueTex->bindForDisplay();
	drawImageScale(x + s + 25, y, 32, s, 0, 0, 32, 128);

	struct C8888V32F2D {
		unsigned int color;
		float x,y,z;
	};

	int hueColor = 0xff00ff00;
	FZScreen::disable(FZ_TEXTURE_2D);
	C8888V32F2D vertices[12] = {
		{ hueColor,   x,   y, 0 },
		{ hueColor,   x+s, y, 0 },
		{ 0xffffffff, x,   y+s, 0 },

		{ hueColor,   x+s, y, 0 },
		{ 0xffffffff, x+s, y+s, 0 },
		{ 0xffffffff, x,   y+s, 0 },

		{ 0xff000000, x,   y, 0 },
		{ 0x00000000, x+s, y, 0 },
		{ 0xff000000, x,   y+s, 0 },

		{ 0x00000000, x+s, y, 0 },
		{ 0x00000000, x+s, y+s, 0 },
		{ 0xff000000, x,   y+s, 0 }
	};
	FZScreen::drawArray(FZ_TRIANGLES, FZ_COLOR_8888|FZ_VERTEX_32BITF|FZ_TRANSFORM_2D, 12, 0, vertices);
	FZScreen::enable(FZ_TEXTURE_2D);
}

BKColorChooser* BKColorChooser::create(int c) {
	BKColorChooser* f = new BKColorChooser(c);
	FZImage* img = FZImage::createEmpty(32, 128, 0, FZImage::rgba32);
	unsigned int* p = (unsigned int*)img->getData();
	const float inc = 360.0f / 128.0f;
	float h = 0.0f;
	float s = 1.0f;
	float v = 1.0f;
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	for (int j = 0; j < 128; ++j, h += inc) {
		HSVtoRGB(&r, &g, &b, h, s, v);
		for (int i = 0; i < 32; ++i, ++p) {
			unsigned int ir = (unsigned int)(r*255.0f);
			unsigned int ig = (unsigned int)(g*255.0f);
			unsigned int ib = (unsigned int)(b*255.0f);
			*p = ir | (ig << 8) | (ib << 16) | 0xff000000;
		}
	}
	f->hueTex = FZTexture::createFromImage(img, false);
	f->hueTex->texEnv(FZ_TEX_MODULATE);
	f->hueTex->filter(FZ_LINEAR, FZ_LINEAR);

	img->release();

	FZScreen::resetReps();
	return f;
}

// from http://www.cs.rit.edu/~ncs/color/t_convert.html

// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//		if s == 0, then h = -1 (undefined)
#define MIN2(a,b) (((a) < (b)) ? (a) : (b))
#define MIN(a,b,c) MIN2(MIN2(MIN2(a,b),MIN2(b,c)),MIN2(a,c))
#define MAX2(a,b) (((a) > (b)) ? (a) : (b))
#define MAX(a,b,c) MAX2(MAX2(MAX2(a,b),MAX2(b,c)),MAX2(a,c))
static void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v) {
	float min, max, delta;

	min = MIN( r, g, b );
	max = MAX( r, g, b );
	*v = max;				// v

	delta = max - min;

	if( max != 0 )
		*s = delta / max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}

	if( r == max )
		*h = ( g - b ) / delta;		// between yellow & magenta
	else if( g == max )
		*h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		*h = 4 + ( r - g ) / delta;	// between magenta & cyan

	*h *= 60;				// degrees
	if( *h < 0 )
		*h += 360;
}

static void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v) {
	int i;
	float f, p, q, t;

	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}

	h /= 60;			// sector 0 to 5
	i = (int)floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}
}

