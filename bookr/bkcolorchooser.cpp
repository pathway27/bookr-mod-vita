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

#include <string.h>
#include <math.h>
#include "fzscreen.h"

#include "bkcolorchooser.h"
#include "bkuser.h"

//static void RGBtoHSV(float r, float g, float b, float *h, float *s, float *v);
static void HSVtoRGB(float *r, float *g, float *b, float h, float s, float v);
static unsigned int HSVtoRGBi(float h, float s, float v);
struct HSV {
	float h, s, v;
};
struct COLOUR {
	float r, g, b;
	COLOUR(float _r, float _g, float _b) : r(_r), g(_g), b(_b) { }
};
static HSV RGB2HSV(COLOUR c1);

BKColorChooser::BKColorChooser(int cs, int c, int r) : colorScheme(cs), color(c), hueY(0), svX(0), svY(0), hueTex(0), hueMode(false), ret(r) {	
}

BKColorChooser::~BKColorChooser() {
	hueTex->release();
}

unsigned int BKColorChooser::getColor() {
	return color;
}

void BKColorChooser::recalcColor() {
	const float v = float(svX) / 144.0f;
	const float s = 1.0f - (float(svY) / 144.0f);
	const float h = 359.0f * (float(hueY) / 144.0f);
	color = HSVtoRGBi(h, s, v);
}

int BKColorChooser::update(unsigned int buttons) {
	int* b = FZScreen::ctrlReps();
	if (b[BKUser::controls.menuDown] == 1 || b[BKUser::controls.menuDown] > 20) {
		if (hueMode) {
			++hueY;
			if (hueY > 144) {
				hueY = 144;
			}
		} else {
			++svY;
			if (svY > 144) {
				svY = 144;
			}
		}
		recalcColor();
		return BK_CMD_MARK_DIRTY;
	}
	if (b[BKUser::controls.menuUp] == 1 || b[BKUser::controls.menuUp] > 20) {
		if (hueMode) {
			--hueY;
			if (hueY < 0) {
				hueY = 0;
			}
		} else {
			--svY;
			if (svY < 0) {
				svY = 0;
			}
		}
		recalcColor();
		return BK_CMD_MARK_DIRTY;
	}
	if (b[BKUser::controls.menuRight] == 1 || b[BKUser::controls.menuRight] > 20) {
		if (!hueMode) {
			++svX;
			if (svX > 144) {
				svX = 144;
			}
			recalcColor();
			return BK_CMD_MARK_DIRTY;
		}
	}
	if (b[BKUser::controls.menuLeft] == 1 || b[BKUser::controls.menuLeft] > 20) {
		if (!hueMode) {
			--svX;
			if (svX < 0) {
				svX = 0;
			}
			recalcColor();
			return BK_CMD_MARK_DIRTY;
		}
	}
	if (b[BKUser::controls.select] == 1) {  
		return ret;
	}
	if (b[BKUser::controls.alternate] == 1) {
		hueMode = !hueMode;
		return BK_CMD_MARK_DIRTY;
	}
	if (b[BKUser::controls.cancel] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}
	return 0;
}

void BKColorChooser::render() {
	string title;
	switch (ret) {
		case BK_CMD_SET_TXTFG:	title="Select text color (plain text only)";	break;
		case BK_CMD_SET_TXTBG:	title="Select background color (plain text and PDF)"; 	break;
	default:	break;
	}
	string cl("Select this color");
	string tl(hueMode ? "Switch to saturation/value selection" : "Switch to hue selection");
	
	drawDialogFrame(title, tl, cl, 0);

	const int x = 75;
	const int y = 75;
	const int y2 = 120;
	const int side = 145;
	const int side2 = 145 - (y2 - y);
	//const float v = float(svX) / 144.0f;
	//const float s = 1.0f - (float(svY) / 144.0f);
	const float h = 359.0f * (float(hueY) / 144.0f);
	unsigned int hueColor = HSVtoRGBi(h, 1.0f, 1.0f);
	hueTex->bindForDisplay();
	drawImageScale(x + side + 25, y, 32, side, 0, 0, 32, 128);

	struct C8888V32F2D {
		unsigned int color;
		float x,y,z;
	};

	FZScreen::disable(FZ_TEXTURE_2D);
	FZScreen::shadeModel(FZ_SMOOTH);
	C8888V32F2D vertices[18] = {
		{ hueColor,   x,      y, 0 },
		{ hueColor,   x+side, y, 0 },
		{ 0xffffffff, x,      y+side, 0 },

		{ hueColor,   x+side, y, 0 },
		{ 0xffffffff, x+side, y+side, 0 },
		{ 0xffffffff, x,      y+side, 0 },

		{ 0xff000000, x,      y, 0 },
		{ 0x00000000, x+side, y, 0 },
		{ 0xff000000, x,      y+side, 0 },

		{ 0x00000000, x+side, y, 0 },
		{ 0x00000000, x+side, y+side, 0 },
		{ 0xff000000, x,      y+side, 0 },

		{ color, 300, y2, 0 },
		{ color, 400, y2, 0 },
		{ color, 300, y2+side2, 0 },

		{ color, 400, y2, 0 },
		{ color, 400, y2+side2, 0 },
		{ color, 300, y2+side2, 0 },
	};
	FZScreen::drawArray(FZ_TRIANGLES, FZ_COLOR_8888|FZ_VERTEX_32BITF|FZ_TRANSFORM_2D, 18, 0, vertices);
	FZScreen::shadeModel(FZ_FLAT);
	FZScreen::enable(FZ_TEXTURE_2D);

	texUI->bindForDisplay();

	FZScreen::ambientColor(hueMode ? 0xff707070 : 0xffffffff);
	drawImage(x + svX - 4, y + svY - 4, 9, 9, 32, 92);
	FZScreen::ambientColor(hueMode ? 0xffffffff : 0xff707070);
	drawImage(x + side + 25 + 4, y + hueY - 4, 4, 9, 32, 92);
	drawImage(x + side + 25 + 24, y + hueY - 4, 4, 9, 32 + 5, 92);

	fontBig->bindForDisplay();
	FZScreen::ambientColor(0xffffffff);
	drawText("Selected color", fontBig, 300, y + 10);
}

BKColorChooser* BKColorChooser::create(int cs, int re) {	
	int c;
	switch (re) {
	case BK_CMD_SET_TXTFG:	c = BKUser::options.colorSchemes[cs].txtFGColor;	
		break;
	case BK_CMD_SET_TXTBG:	
	default:				c = BKUser::options.colorSchemes[cs].txtBGColor;
	break;
	}

	BKColorChooser* f = new BKColorChooser(cs, c | 0xff000000, re);
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

	r = float(c & 0xff) / 255.0f;
	g = float((c >> 8)& 0xff) / 255.0f;
	b = float((c >> 16)& 0xff) / 255.0f;
	HSV c2 =	RGB2HSV(COLOUR(r, g, b));
	f->hueY = int((c2.h / 360.0f) * 144.0f);
	if (f->hueY < 0)
		f->hueY = 0;
	f->svX = int(c2.v * 144.0f);
	f->svY = int((1.0f - c2.s) * 144.0f);

	img->release();

	FZScreen::resetReps();
	return f;
}

static unsigned int HSVtoRGBi(float h, float s, float v) {
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	HSVtoRGB(&r, &g, &b, h, s, v);
	unsigned int ir = (unsigned int)(r*255.0f);
	unsigned int ig = (unsigned int)(g*255.0f);
	unsigned int ib = (unsigned int)(b*255.0f);
	return ir | (ig << 8) | (ib << 16) | 0xff000000;
}

// from http://www.cs.rit.edu/~ncs/color/t_convert.html

// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//		if s == 0, then h = -1 (undefined)
#define MIN2(a,b) (((a) < (b)) ? (a) : (b))
#define MIN3(a,b,c) MIN2(a,MIN2(b,c))
#define MAX2(a,b) (((a) > (b)) ? (a) : (b))
#define MAX3(a,b,c) MAX2(a,MAX2(b,c))
/*
   Calculate HSV from RGB
   Hue is in degrees
   Lightness is betweeen 0 and 1
   Saturation is between 0 and 1
*/
static HSV RGB2HSV(COLOUR c1) {
	float themin,themax,delta;
	HSV c2;
	
	themin = MIN2(c1.r,MIN2(c1.g,c1.b));
	themax = MAX2(c1.r,MAX2(c1.g,c1.b));
	delta = themax - themin;
	c2.v = themax;
	c2.s = 0.0f;
	if (themax > 0.0f)
		c2.s = delta / themax;
	c2.h = 0.0f;
	if (delta > 0.0f) {
		if (themax == c1.r && themax != c1.g)
			c2.h += (c1.g - c1.b) / delta;
		if (themax == c1.g && themax != c1.b)
			c2.h += (2.0f + (c1.b - c1.r) / delta);
		if (themax == c1.b && themax != c1.r)
			c2.h += (4.0f + (c1.r - c1.g) / delta);
		c2.h *= 60.0f;
	}
	return(c2);
}

/*
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
*/
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

