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

#include <cstdio>
#include <list>
using namespace std;
#include "utf8.h"
#include "bkplaintext.h"
#include "bklogging.h"

int MAX_FILE_SIZE_TO_LOAD = 4 * 1024 * 1024;

unsigned char ISO_8859_7_INV_CHARMAP(uint32_t byte) {
	char result = -1;
	switch(byte) {
		case 0x0000:	result = 0x00; break;	//	NULL
		case 0x0001:	result = 0x01; break;	//	START OF HEADING
		case 0x0002:	result = 0x02; break;	//	START OF TEXT
		case 0x0003:	result = 0x03; break;	//	END OF TEXT
		case 0x0004:	result = 0x04; break;	//	END OF TRANSMISSION
		case 0x0005:	result = 0x05; break;	//	ENQUIRY
		case 0x0006:	result = 0x06; break;	//	ACKNOWLEDGE
		case 0x0007:	result = 0x07; break;	//	BELL
		case 0x0008:	result = 0x08; break;	//	BACKSPACE
		case 0x0009:	result = 0x09; break;	//	HORIZONTAL TABULATION
		case 0x000A:	result = 0x0A; break;	//	LINE FEED
		case 0x000B:	result = 0x0B; break;	//	VERTICAL TABULATION
		case 0x000C:	result = 0x0C; break;	//	FORM FEED
		case 0x000D:	result = 0x0D; break;	//	CARRIAGE RETURN
		case 0x000E:	result = 0x0E; break;	//	SHIFT OUT
		case 0x000F:	result = 0x0F; break;	//	SHIFT IN
		case 0x0010:	result = 0x10; break;	//	DATA LINK ESCAPE
		case 0x0011:	result = 0x11; break;	//	DEVICE CONTROL ONE
		case 0x0012:	result = 0x12; break;	//	DEVICE CONTROL TWO
		case 0x0013:	result = 0x13; break;	//	DEVICE CONTROL THREE
		case 0x0014:	result = 0x14; break;	//	DEVICE CONTROL FOUR
		case 0x0015:	result = 0x15; break;	//	NEGATIVE ACKNOWLEDGE
		case 0x0016:	result = 0x16; break;	//	SYNCHRONOUS IDLE
		case 0x0017:	result = 0x17; break;	//	END OF TRANSMISSION BLOCK
		case 0x0018:	result = 0x18; break;	//	CANCEL
		case 0x0019:	result = 0x19; break;	//	END OF MEDIUM
		case 0x001A:	result = 0x1A; break;	//	SUBSTITUTE
		case 0x001B:	result = 0x1B; break;	//	ESCAPE
		case 0x001C:	result = 0x1C; break;	//	FILE SEPARATOR
		case 0x001D:	result = 0x1D; break;	//	GROUP SEPARATOR
		case 0x001E:	result = 0x1E; break;	//	RECORD SEPARATOR
		case 0x001F:	result = 0x1F; break;	//	UNIT SEPARATOR
		case 0x0020:	result = 0x20; break;	//	SPACE
		case 0x0021:	result = 0x21; break;	//	EXCLAMATION MARK
		case 0x0022:	result = 0x22; break;	//	QUOTATION MARK
		case 0x0023:	result = 0x23; break;	//	NUMBER SIGN
		case 0x0024:	result = 0x24; break;	//	DOLLAR SIGN
		case 0x0025:	result = 0x25; break;	//	PERCENT SIGN
		case 0x0026:	result = 0x26; break;	//	AMPERSAND
		case 0x0027:	result = 0x27; break;	//	APOSTROPHE
		case 0x0028:	result = 0x28; break;	//	LEFT PARENTHESIS
		case 0x0029:	result = 0x29; break;	//	RIGHT PARENTHESIS
		case 0x002A:	result = 0x2A; break;	//	ASTERISK
		case 0x002B:	result = 0x2B; break;	//	PLUS SIGN
		case 0x002C:	result = 0x2C; break;	//	COMMA
		case 0x002D:	result = 0x2D; break;	//	HYPHEN-MINUS
		case 0x002E:	result = 0x2E; break;	//	FULL STOP
		case 0x002F:	result = 0x2F; break;	//	SOLIDUS
		case 0x0030:	result = 0x30; break;	//	DIGIT ZERO
		case 0x0031:	result = 0x31; break;	//	DIGIT ONE
		case 0x0032:	result = 0x32; break;	//	DIGIT TWO
		case 0x0033:	result = 0x33; break;	//	DIGIT THREE
		case 0x0034:	result = 0x34; break;	//	DIGIT FOUR
		case 0x0035:	result = 0x35; break;	//	DIGIT FIVE
		case 0x0036:	result = 0x36; break;	//	DIGIT SIX
		case 0x0037:	result = 0x37; break;	//	DIGIT SEVEN
		case 0x0038:	result = 0x38; break;	//	DIGIT EIGHT
		case 0x0039:	result = 0x39; break;	//	DIGIT NINE
		case 0x003A:	result = 0x3A; break;	//	COLON
		case 0x003B:	result = 0x3B; break;	//	SEMICOLON
		case 0x003C:	result = 0x3C; break;	//	LESS-THAN SIGN
		case 0x003D:	result = 0x3D; break;	//	EQUALS SIGN
		case 0x003E:	result = 0x3E; break;	//	GREATER-THAN SIGN
		case 0x003F:	result = 0x3F; break;	//	QUESTION MARK
		case 0x0040:	result = 0x40; break;	//	COMMERCIAL AT
		case 0x0041:	result = 0x41; break;	//	LATIN CAPITAL LETTER A
		case 0x0042:	result = 0x42; break;	//	LATIN CAPITAL LETTER B
		case 0x0043:	result = 0x43; break;	//	LATIN CAPITAL LETTER C
		case 0x0044:	result = 0x44; break;	//	LATIN CAPITAL LETTER D
		case 0x0045:	result = 0x45; break;	//	LATIN CAPITAL LETTER E
		case 0x0046:	result = 0x46; break;	//	LATIN CAPITAL LETTER F
		case 0x0047:	result = 0x47; break;	//	LATIN CAPITAL LETTER G
		case 0x0048:	result = 0x48; break;	//	LATIN CAPITAL LETTER H
		case 0x0049:	result = 0x49; break;	//	LATIN CAPITAL LETTER I
		case 0x004A:	result = 0x4A; break;	//	LATIN CAPITAL LETTER J
		case 0x004B:	result = 0x4B; break;	//	LATIN CAPITAL LETTER K
		case 0x004C:	result = 0x4C; break;	//	LATIN CAPITAL LETTER L
		case 0x004D:	result = 0x4D; break;	//	LATIN CAPITAL LETTER M
		case 0x004E:	result = 0x4E; break;	//	LATIN CAPITAL LETTER N
		case 0x004F:	result = 0x4F; break;	//	LATIN CAPITAL LETTER O
		case 0x0050:	result = 0x50; break;	//	LATIN CAPITAL LETTER P
		case 0x0051:	result = 0x51; break;	//	LATIN CAPITAL LETTER Q
		case 0x0052:	result = 0x52; break;	//	LATIN CAPITAL LETTER R
		case 0x0053:	result = 0x53; break;	//	LATIN CAPITAL LETTER S
		case 0x0054:	result = 0x54; break;	//	LATIN CAPITAL LETTER T
		case 0x0055:	result = 0x55; break;	//	LATIN CAPITAL LETTER U
		case 0x0056:	result = 0x56; break;	//	LATIN CAPITAL LETTER V
		case 0x0057:	result = 0x57; break;	//	LATIN CAPITAL LETTER W
		case 0x0058:	result = 0x58; break;	//	LATIN CAPITAL LETTER X
		case 0x0059:	result = 0x59; break;	//	LATIN CAPITAL LETTER Y
		case 0x005A:	result = 0x5A; break;	//	LATIN CAPITAL LETTER Z
		case 0x005B:	result = 0x5B; break;	//	LEFT SQUARE BRACKET
		case 0x005C:	result = 0x5C; break;	//	REVERSE SOLIDUS
		case 0x005D:	result = 0x5D; break;	//	RIGHT SQUARE BRACKET
		case 0x005E:	result = 0x5E; break;	//	CIRCUMFLEX ACCENT
		case 0x005F:	result = 0x5F; break;	//	LOW LINE
		case 0x0060:	result = 0x60; break;	//	GRAVE ACCENT
		case 0x0061:	result = 0x61; break;	//	LATIN SMALL LETTER A
		case 0x0062:	result = 0x62; break;	//	LATIN SMALL LETTER B
		case 0x0063:	result = 0x63; break;	//	LATIN SMALL LETTER C
		case 0x0064:	result = 0x64; break;	//	LATIN SMALL LETTER D
		case 0x0065:	result = 0x65; break;	//	LATIN SMALL LETTER E
		case 0x0066:	result = 0x66; break;	//	LATIN SMALL LETTER F
		case 0x0067:	result = 0x67; break;	//	LATIN SMALL LETTER G
		case 0x0068:	result = 0x68; break;	//	LATIN SMALL LETTER H
		case 0x0069:	result = 0x69; break;	//	LATIN SMALL LETTER I
		case 0x006A:	result = 0x6A; break;	//	LATIN SMALL LETTER J
		case 0x006B:	result = 0x6B; break;	//	LATIN SMALL LETTER K
		case 0x006C:	result = 0x6C; break;	//	LATIN SMALL LETTER L
		case 0x006D:	result = 0x6D; break;	//	LATIN SMALL LETTER M
		case 0x006E:	result = 0x6E; break;	//	LATIN SMALL LETTER N
		case 0x006F:	result = 0x6F; break;	//	LATIN SMALL LETTER O
		case 0x0070:	result = 0x70; break;	//	LATIN SMALL LETTER P
		case 0x0071:	result = 0x71; break;	//	LATIN SMALL LETTER Q
		case 0x0072:	result = 0x72; break;	//	LATIN SMALL LETTER R
		case 0x0073:	result = 0x73; break;	//	LATIN SMALL LETTER S
		case 0x0074:	result = 0x74; break;	//	LATIN SMALL LETTER T
		case 0x0075:	result = 0x75; break;	//	LATIN SMALL LETTER U
		case 0x0076:	result = 0x76; break;	//	LATIN SMALL LETTER V
		case 0x0077:	result = 0x77; break;	//	LATIN SMALL LETTER W
		case 0x0078:	result = 0x78; break;	//	LATIN SMALL LETTER X
		case 0x0079:	result = 0x79; break;	//	LATIN SMALL LETTER Y
		case 0x007A:	result = 0x7A; break;	//	LATIN SMALL LETTER Z
		case 0x007B:	result = 0x7B; break;	//	LEFT CURLY BRACKET
		case 0x007C:	result = 0x7C; break;	//	VERTICAL LINE
		case 0x007D:	result = 0x7D; break;	//	RIGHT CURLY BRACKET
		case 0x007E:	result = 0x7E; break;	//	TILDE
		case 0x007F:	result = 0x7F; break;	//	DELETE
		case 0x0080:	result = 0x80; break;	//	<control>
		case 0x0081:	result = 0x81; break;	//	<control>
		case 0x0082:	result = 0x82; break;	//	<control>
		case 0x0083:	result = 0x83; break;	//	<control>
		case 0x0084:	result = 0x84; break;	//	<control>
		case 0x0085:	result = 0x85; break;	//	<control>
		case 0x0086:	result = 0x86; break;	//	<control>
		case 0x0087:	result = 0x87; break;	//	<control>
		case 0x0088:	result = 0x88; break;	//	<control>
		case 0x0089:	result = 0x89; break;	//	<control>
		case 0x008A:	result = 0x8A; break;	//	<control>
		case 0x008B:	result = 0x8B; break;	//	<control>
		case 0x008C:	result = 0x8C; break;	//	<control>
		case 0x008D:	result = 0x8D; break;	//	<control>
		case 0x008E:	result = 0x8E; break;	//	<control>
		case 0x008F:	result = 0x8F; break;	//	<control>
		case 0x0090:	result = 0x90; break;	//	<control>
		case 0x0091:	result = 0x91; break;	//	<control>
		case 0x0092:	result = 0x92; break;	//	<control>
		case 0x0093:	result = 0x93; break;	//	<control>
		case 0x0094:	result = 0x94; break;	//	<control>
		case 0x0095:	result = 0x95; break;	//	<control>
		case 0x0096:	result = 0x96; break;	//	<control>
		case 0x0097:	result = 0x97; break;	//	<control>
		case 0x0098:	result = 0x98; break;	//	<control>
		case 0x0099:	result = 0x99; break;	//	<control>
		case 0x009A:	result = 0x9A; break;	//	<control>
		case 0x009B:	result = 0x9B; break;	//	<control>
		case 0x009C:	result = 0x9C; break;	//	<control>
		case 0x009D:	result = 0x9D; break;	//	<control>
		case 0x009E:	result = 0x9E; break;	//	<control>
		case 0x009F:	result = 0x9F; break;	//	<control>
		case 0x00A0:	result = 0xA0; break;	//	NO-BREAK SPACE
		case 0x2018:	result = 0xA1; break;	//	LEFT SINGLE QUOTATION MARK
		case 0x2019:	result = 0xA2; break;	//	RIGHT SINGLE QUOTATION MARK
		case 0x00A3:	result = 0xA3; break;	//	POUND SIGN
		case 0x20AC:	result = 0xA4; break;	//	EURO SIGN
		case 0x20AF:	result = 0xA5; break;	//	DRACHMA SIGN
		case 0x00A6:	result = 0xA6; break;	//	BROKEN BAR
		case 0x00A7:	result = 0xA7; break;	//	SECTION SIGN
		case 0x00A8:	result = 0xA8; break;	//	DIAERESIS
		case 0x00A9:	result = 0xA9; break;	//	COPYRIGHT SIGN
		case 0x037A:	result = 0xAA; break;	//	GREEK YPOGEGRAMMENI
		case 0x00AB:	result = 0xAB; break;	//	LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
		case 0x00AC:	result = 0xAC; break;	//	NOT SIGN
		case 0x00AD:	result = 0xAD; break;	//	SOFT HYPHEN
		case 0x2015:	result = 0xAF; break;	//	HORIZONTAL BAR
		case 0x00B0:	result = 0xB0; break;	//	DEGREE SIGN
		case 0x00B1:	result = 0xB1; break;	//	PLUS-MINUS SIGN
		case 0x00B2:	result = 0xB2; break;	//	SUPERSCRIPT TWO
		case 0x00B3:	result = 0xB3; break;	//	SUPERSCRIPT THREE
		case 0x0384:	result = 0xB4; break;	//	GREEK TONOS
		case 0x0385:	result = 0xB5; break;	//	GREEK DIALYTIKA TONOS
		case 0x0386:	result = 0xB6; break;	//	GREEK CAPITAL LETTER ALPHA WITH TONOS
		case 0x00B7:	result = 0xB7; break;	//	MIDDLE DOT
		case 0x0388:	result = 0xB8; break;	//	GREEK CAPITAL LETTER EPSILON WITH TONOS
		case 0x0389:	result = 0xB9; break;	//	GREEK CAPITAL LETTER ETA WITH TONOS
		case 0x038A:	result = 0xBA; break;	//	GREEK CAPITAL LETTER IOTA WITH TONOS
		case 0x00BB:	result = 0xBB; break;	//	RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
		case 0x038C:	result = 0xBC; break;	//	GREEK CAPITAL LETTER OMICRON WITH TONOS
		case 0x00BD:	result = 0xBD; break;	//	VULGAR FRACTION ONE HALF
		case 0x038E:	result = 0xBE; break;	//	GREEK CAPITAL LETTER UPSILON WITH TONOS
		case 0x038F:	result = 0xBF; break;	//	GREEK CAPITAL LETTER OMEGA WITH TONOS
		case 0x0390:	result = 0xC0; break;	//	GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
		case 0x0391:	result = 0xC1; break;	//	GREEK CAPITAL LETTER ALPHA
		case 0x0392:	result = 0xC2; break;	//	GREEK CAPITAL LETTER BETA
		case 0x0393:	result = 0xC3; break;	//	GREEK CAPITAL LETTER GAMMA
		case 0x0394:	result = 0xC4; break;	//	GREEK CAPITAL LETTER DELTA
		case 0x0395:	result = 0xC5; break;	//	GREEK CAPITAL LETTER EPSILON
		case 0x0396:	result = 0xC6; break;	//	GREEK CAPITAL LETTER ZETA
		case 0x0397:	result = 0xC7; break;	//	GREEK CAPITAL LETTER ETA
		case 0x0398:	result = 0xC8; break;	//	GREEK CAPITAL LETTER THETA
		case 0x0399:	result = 0xC9; break;	//	GREEK CAPITAL LETTER IOTA
		case 0x039A:	result = 0xCA; break;	//	GREEK CAPITAL LETTER KAPPA
		case 0x039B:	result = 0xCB; break;	//	GREEK CAPITAL LETTER LAMDA
		case 0x039C:	result = 0xCC; break;	//	GREEK CAPITAL LETTER MU
		case 0x039D:	result = 0xCD; break;	//	GREEK CAPITAL LETTER NU
		case 0x039E:	result = 0xCE; break;	//	GREEK CAPITAL LETTER XI
		case 0x039F:	result = 0xCF; break;	//	GREEK CAPITAL LETTER OMICRON
		case 0x03A0:	result = 0xD0; break;	//	GREEK CAPITAL LETTER PI
		case 0x03A1:	result = 0xD1; break;	//	GREEK CAPITAL LETTER RHO
		case 0x03A3:	result = 0xD3; break;	//	GREEK CAPITAL LETTER SIGMA
		case 0x03A4:	result = 0xD4; break;	//	GREEK CAPITAL LETTER TAU
		case 0x03A5:	result = 0xD5; break;	//	GREEK CAPITAL LETTER UPSILON
		case 0x03A6:	result = 0xD6; break;	//	GREEK CAPITAL LETTER PHI
		case 0x03A7:	result = 0xD7; break;	//	GREEK CAPITAL LETTER CHI
		case 0x03A8:	result = 0xD8; break;	//	GREEK CAPITAL LETTER PSI
		case 0x03A9:	result = 0xD9; break;	//	GREEK CAPITAL LETTER OMEGA
		case 0x03AA:	result = 0xDA; break;	//	GREEK CAPITAL LETTER IOTA WITH DIALYTIKA
		case 0x03AB:	result = 0xDB; break;	//	GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA
		case 0x03AC:	result = 0xDC; break;	//	GREEK SMALL LETTER ALPHA WITH TONOS
		case 0x03AD:	result = 0xDD; break;	//	GREEK SMALL LETTER EPSILON WITH TONOS
		case 0x03AE:	result = 0xDE; break;	//	GREEK SMALL LETTER ETA WITH TONOS
		case 0x03AF:	result = 0xDF; break;	//	GREEK SMALL LETTER IOTA WITH TONOS
		case 0x03B0:	result = 0xE0; break;	//	GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
		case 0x03B1:	result = 0xE1; break;	//	GREEK SMALL LETTER ALPHA
		case 0x03B2:	result = 0xE2; break;	//	GREEK SMALL LETTER BETA
		case 0x03B3:	result = 0xE3; break;	//	GREEK SMALL LETTER GAMMA
		case 0x03B4:	result = 0xE4; break;	//	GREEK SMALL LETTER DELTA
		case 0x03B5:	result = 0xE5; break;	//	GREEK SMALL LETTER EPSILON
		case 0x03B6:	result = 0xE6; break;	//	GREEK SMALL LETTER ZETA
		case 0x03B7:	result = 0xE7; break;	//	GREEK SMALL LETTER ETA
		case 0x03B8:	result = 0xE8; break;	//	GREEK SMALL LETTER THETA
		case 0x03B9:	result = 0xE9; break;	//	GREEK SMALL LETTER IOTA
		case 0x03BA:	result = 0xEA; break;	//	GREEK SMALL LETTER KAPPA
		case 0x03BB:	result = 0xEB; break;	//	GREEK SMALL LETTER LAMDA
		case 0x03BC:	result = 0xEC; break;	//	GREEK SMALL LETTER MU
		case 0x03BD:	result = 0xED; break;	//	GREEK SMALL LETTER NU
		case 0x03BE:	result = 0xEE; break;	//	GREEK SMALL LETTER XI
		case 0x03BF:	result = 0xEF; break;	//	GREEK SMALL LETTER OMICRON
		case 0x03C0:	result = 0xF0; break;	//	GREEK SMALL LETTER PI
		case 0x03C1:	result = 0xF1; break;	//	GREEK SMALL LETTER RHO
		case 0x03C2:	result = 0xF2; break;	//	GREEK SMALL LETTER FINAL SIGMA
		case 0x03C3:	result = 0xF3; break;	//	GREEK SMALL LETTER SIGMA
		case 0x03C4:	result = 0xF4; break;	//	GREEK SMALL LETTER TAU
		case 0x03C5:	result = 0xF5; break;	//	GREEK SMALL LETTER UPSILON
		case 0x03C6:	result = 0xF6; break;	//	GREEK SMALL LETTER PHI
		case 0x03C7:	result = 0xF7; break;	//	GREEK SMALL LETTER CHI
		case 0x03C8:	result = 0xF8; break;	//	GREEK SMALL LETTER PSI
		case 0x03C9:	result = 0xF9; break;	//	GREEK SMALL LETTER OMEGA
		case 0x03CA:	result = 0xFA; break;	//	GREEK SMALL LETTER IOTA WITH DIALYTIKA
		case 0x03CB:	result = 0xFB; break;	//	GREEK SMALL LETTER UPSILON WITH DIALYTIKA
		case 0x03CC:	result = 0xFC; break;	//	GREEK SMALL LETTER OMICRON WITH TONOS
		case 0x03CD:	result = 0xFD; break;	//	GREEK SMALL LETTER UPSILON WITH TONOS
		case 0x03CE:	result = 0xFE; break;	//	GREEK SMALL LETTER OMEGA WITH TONOS
	};

	return result;
}

uint32_t ISO_8859_7_CHARMAP(unsigned char byte) {
	int result = -1;
	switch(byte) {
		case 0x00:	result = 0x0000; break;	//	NULL
		case 0x01:	result = 0x0001; break;	//	START OF HEADING
		case 0x02:	result = 0x0002; break;	//	START OF TEXT
		case 0x03:	result = 0x0003; break;	//	END OF TEXT
		case 0x04:	result = 0x0004; break;	//	END OF TRANSMISSION
		case 0x05:	result = 0x0005; break;	//	ENQUIRY
		case 0x06:	result = 0x0006; break;	//	ACKNOWLEDGE
		case 0x07:	result = 0x0007; break;	//	BELL
		case 0x08:	result = 0x0008; break;	//	BACKSPACE
		case 0x09:	result = 0x0009; break;	//	HORIZONTAL TABULATION
		case 0x0A:	result = 0x000A; break;	//	LINE FEED
		case 0x0B:	result = 0x000B; break;	//	VERTICAL TABULATION
		case 0x0C:	result = 0x000C; break;	//	FORM FEED
		case 0x0D:	result = 0x000D; break;	//	CARRIAGE RETURN
		case 0x0E:	result = 0x000E; break;	//	SHIFT OUT
		case 0x0F:	result = 0x000F; break;	//	SHIFT IN
		case 0x10:	result = 0x0010; break;	//	DATA LINK ESCAPE
		case 0x11:	result = 0x0011; break;	//	DEVICE CONTROL ONE
		case 0x12:	result = 0x0012; break;	//	DEVICE CONTROL TWO
		case 0x13:	result = 0x0013; break;	//	DEVICE CONTROL THREE
		case 0x14:	result = 0x0014; break;	//	DEVICE CONTROL FOUR
		case 0x15:	result = 0x0015; break;	//	NEGATIVE ACKNOWLEDGE
		case 0x16:	result = 0x0016; break;	//	SYNCHRONOUS IDLE
		case 0x17:	result = 0x0017; break;	//	END OF TRANSMISSION BLOCK
		case 0x18:	result = 0x0018; break;	//	CANCEL
		case 0x19:	result = 0x0019; break;	//	END OF MEDIUM
		case 0x1A:	result = 0x001A; break;	//	SUBSTITUTE
		case 0x1B:	result = 0x001B; break;	//	ESCAPE
		case 0x1C:	result = 0x001C; break;	//	FILE SEPARATOR
		case 0x1D:	result = 0x001D; break;	//	GROUP SEPARATOR
		case 0x1E:	result = 0x001E; break;	//	RECORD SEPARATOR
		case 0x1F:	result = 0x001F; break;	//	UNIT SEPARATOR
		case 0x20:	result = 0x0020; break;	//	SPACE
		case 0x21:	result = 0x0021; break;	//	EXCLAMATION MARK
		case 0x22:	result = 0x0022; break;	//	QUOTATION MARK
		case 0x23:	result = 0x0023; break;	//	NUMBER SIGN
		case 0x24:	result = 0x0024; break;	//	DOLLAR SIGN
		case 0x25:	result = 0x0025; break;	//	PERCENT SIGN
		case 0x26:	result = 0x0026; break;	//	AMPERSAND
		case 0x27:	result = 0x0027; break;	//	APOSTROPHE
		case 0x28:	result = 0x0028; break;	//	LEFT PARENTHESIS
		case 0x29:	result = 0x0029; break;	//	RIGHT PARENTHESIS
		case 0x2A:	result = 0x002A; break;	//	ASTERISK
		case 0x2B:	result = 0x002B; break;	//	PLUS SIGN
		case 0x2C:	result = 0x002C; break;	//	COMMA
		case 0x2D:	result = 0x002D; break;	//	HYPHEN-MINUS
		case 0x2E:	result = 0x002E; break;	//	FULL STOP
		case 0x2F:	result = 0x002F; break;	//	SOLIDUS
		case 0x30:	result = 0x0030; break;	//	DIGIT ZERO
		case 0x31:	result = 0x0031; break;	//	DIGIT ONE
		case 0x32:	result = 0x0032; break;	//	DIGIT TWO
		case 0x33:	result = 0x0033; break;	//	DIGIT THREE
		case 0x34:	result = 0x0034; break;	//	DIGIT FOUR
		case 0x35:	result = 0x0035; break;	//	DIGIT FIVE
		case 0x36:	result = 0x0036; break;	//	DIGIT SIX
		case 0x37:	result = 0x0037; break;	//	DIGIT SEVEN
		case 0x38:	result = 0x0038; break;	//	DIGIT EIGHT
		case 0x39:	result = 0x0039; break;	//	DIGIT NINE
		case 0x3A:	result = 0x003A; break;	//	COLON
		case 0x3B:	result = 0x003B; break;	//	SEMICOLON
		case 0x3C:	result = 0x003C; break;	//	LESS-THAN SIGN
		case 0x3D:	result = 0x003D; break;	//	EQUALS SIGN
		case 0x3E:	result = 0x003E; break;	//	GREATER-THAN SIGN
		case 0x3F:	result = 0x003F; break;	//	QUESTION MARK
		case 0x40:	result = 0x0040; break;	//	COMMERCIAL AT
		case 0x41:	result = 0x0041; break;	//	LATIN CAPITAL LETTER A
		case 0x42:	result = 0x0042; break;	//	LATIN CAPITAL LETTER B
		case 0x43:	result = 0x0043; break;	//	LATIN CAPITAL LETTER C
		case 0x44:	result = 0x0044; break;	//	LATIN CAPITAL LETTER D
		case 0x45:	result = 0x0045; break;	//	LATIN CAPITAL LETTER E
		case 0x46:	result = 0x0046; break;	//	LATIN CAPITAL LETTER F
		case 0x47:	result = 0x0047; break;	//	LATIN CAPITAL LETTER G
		case 0x48:	result = 0x0048; break;	//	LATIN CAPITAL LETTER H
		case 0x49:	result = 0x0049; break;	//	LATIN CAPITAL LETTER I
		case 0x4A:	result = 0x004A; break;	//	LATIN CAPITAL LETTER J
		case 0x4B:	result = 0x004B; break;	//	LATIN CAPITAL LETTER K
		case 0x4C:	result = 0x004C; break;	//	LATIN CAPITAL LETTER L
		case 0x4D:	result = 0x004D; break;	//	LATIN CAPITAL LETTER M
		case 0x4E:	result = 0x004E; break;	//	LATIN CAPITAL LETTER N
		case 0x4F:	result = 0x004F; break;	//	LATIN CAPITAL LETTER O
		case 0x50:	result = 0x0050; break;	//	LATIN CAPITAL LETTER P
		case 0x51:	result = 0x0051; break;	//	LATIN CAPITAL LETTER Q
		case 0x52:	result = 0x0052; break;	//	LATIN CAPITAL LETTER R
		case 0x53:	result = 0x0053; break;	//	LATIN CAPITAL LETTER S
		case 0x54:	result = 0x0054; break;	//	LATIN CAPITAL LETTER T
		case 0x55:	result = 0x0055; break;	//	LATIN CAPITAL LETTER U
		case 0x56:	result = 0x0056; break;	//	LATIN CAPITAL LETTER V
		case 0x57:	result = 0x0057; break;	//	LATIN CAPITAL LETTER W
		case 0x58:	result = 0x0058; break;	//	LATIN CAPITAL LETTER X
		case 0x59:	result = 0x0059; break;	//	LATIN CAPITAL LETTER Y
		case 0x5A:	result = 0x005A; break;	//	LATIN CAPITAL LETTER Z
		case 0x5B:	result = 0x005B; break;	//	LEFT SQUARE BRACKET
		case 0x5C:	result = 0x005C; break;	//	REVERSE SOLIDUS
		case 0x5D:	result = 0x005D; break;	//	RIGHT SQUARE BRACKET
		case 0x5E:	result = 0x005E; break;	//	CIRCUMFLEX ACCENT
		case 0x5F:	result = 0x005F; break;	//	LOW LINE
		case 0x60:	result = 0x0060; break;	//	GRAVE ACCENT
		case 0x61:	result = 0x0061; break;	//	LATIN SMALL LETTER A
		case 0x62:	result = 0x0062; break;	//	LATIN SMALL LETTER B
		case 0x63:	result = 0x0063; break;	//	LATIN SMALL LETTER C
		case 0x64:	result = 0x0064; break;	//	LATIN SMALL LETTER D
		case 0x65:	result = 0x0065; break;	//	LATIN SMALL LETTER E
		case 0x66:	result = 0x0066; break;	//	LATIN SMALL LETTER F
		case 0x67:	result = 0x0067; break;	//	LATIN SMALL LETTER G
		case 0x68:	result = 0x0068; break;	//	LATIN SMALL LETTER H
		case 0x69:	result = 0x0069; break;	//	LATIN SMALL LETTER I
		case 0x6A:	result = 0x006A; break;	//	LATIN SMALL LETTER J
		case 0x6B:	result = 0x006B; break;	//	LATIN SMALL LETTER K
		case 0x6C:	result = 0x006C; break;	//	LATIN SMALL LETTER L
		case 0x6D:	result = 0x006D; break;	//	LATIN SMALL LETTER M
		case 0x6E:	result = 0x006E; break;	//	LATIN SMALL LETTER N
		case 0x6F:	result = 0x006F; break;	//	LATIN SMALL LETTER O
		case 0x70:	result = 0x0070; break;	//	LATIN SMALL LETTER P
		case 0x71:	result = 0x0071; break;	//	LATIN SMALL LETTER Q
		case 0x72:	result = 0x0072; break;	//	LATIN SMALL LETTER R
		case 0x73:	result = 0x0073; break;	//	LATIN SMALL LETTER S
		case 0x74:	result = 0x0074; break;	//	LATIN SMALL LETTER T
		case 0x75:	result = 0x0075; break;	//	LATIN SMALL LETTER U
		case 0x76:	result = 0x0076; break;	//	LATIN SMALL LETTER V
		case 0x77:	result = 0x0077; break;	//	LATIN SMALL LETTER W
		case 0x78:	result = 0x0078; break;	//	LATIN SMALL LETTER X
		case 0x79:	result = 0x0079; break;	//	LATIN SMALL LETTER Y
		case 0x7A:	result = 0x007A; break;	//	LATIN SMALL LETTER Z
		case 0x7B:	result = 0x007B; break;	//	LEFT CURLY BRACKET
		case 0x7C:	result = 0x007C; break;	//	VERTICAL LINE
		case 0x7D:	result = 0x007D; break;	//	RIGHT CURLY BRACKET
		case 0x7E:	result = 0x007E; break;	//	TILDE
		case 0x7F:	result = 0x007F; break;	//	DELETE
		case 0x80:	result = 0x0080; break;	//	<control>
		case 0x81:	result = 0x0081; break;	//	<control>
		case 0x82:	result = 0x0082; break;	//	<control>
		case 0x83:	result = 0x0083; break;	//	<control>
		case 0x84:	result = 0x0084; break;	//	<control>
		case 0x85:	result = 0x0085; break;	//	<control>
		case 0x86:	result = 0x0086; break;	//	<control>
		case 0x87:	result = 0x0087; break;	//	<control>
		case 0x88:	result = 0x0088; break;	//	<control>
		case 0x89:	result = 0x0089; break;	//	<control>
		case 0x8A:	result = 0x008A; break;	//	<control>
		case 0x8B:	result = 0x008B; break;	//	<control>
		case 0x8C:	result = 0x008C; break;	//	<control>
		case 0x8D:	result = 0x008D; break;	//	<control>
		case 0x8E:	result = 0x008E; break;	//	<control>
		case 0x8F:	result = 0x008F; break;	//	<control>
		case 0x90:	result = 0x0090; break;	//	<control>
		case 0x91:	result = 0x0091; break;	//	<control>
		case 0x92:	result = 0x0092; break;	//	<control>
		case 0x93:	result = 0x0093; break;	//	<control>
		case 0x94:	result = 0x0094; break;	//	<control>
		case 0x95:	result = 0x0095; break;	//	<control>
		case 0x96:	result = 0x0096; break;	//	<control>
		case 0x97:	result = 0x0097; break;	//	<control>
		case 0x98:	result = 0x0098; break;	//	<control>
		case 0x99:	result = 0x0099; break;	//	<control>
		case 0x9A:	result = 0x009A; break;	//	<control>
		case 0x9B:	result = 0x009B; break;	//	<control>
		case 0x9C:	result = 0x009C; break;	//	<control>
		case 0x9D:	result = 0x009D; break;	//	<control>
		case 0x9E:	result = 0x009E; break;	//	<control>
		case 0x9F:	result = 0x009F; break;	//	<control>
		case 0xA0:	result = 0x00A0; break;	//	NO-BREAK SPACE
		case 0xA1:	result = 0x2018; break;	//	LEFT SINGLE QUOTATION MARK
		case 0xA2:	result = 0x2019; break;	//	RIGHT SINGLE QUOTATION MARK
		case 0xA3:	result = 0x00A3; break;	//	POUND SIGN
		case 0xA4:	result = 0x20AC; break;	//	EURO SIGN
		case 0xA5:	result = 0x20AF; break;	//	DRACHMA SIGN
		case 0xA6:	result = 0x00A6; break;	//	BROKEN BAR
		case 0xA7:	result = 0x00A7; break;	//	SECTION SIGN
		case 0xA8:	result = 0x00A8; break;	//	DIAERESIS
		case 0xA9:	result = 0x00A9; break;	//	COPYRIGHT SIGN
		case 0xAA:	result = 0x037A; break;	//	GREEK YPOGEGRAMMENI
		case 0xAB:	result = 0x00AB; break;	//	LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
		case 0xAC:	result = 0x00AC; break;	//	NOT SIGN
		case 0xAD:	result = 0x00AD; break;	//	SOFT HYPHEN
		case 0xAF:	result = 0x2015; break;	//	HORIZONTAL BAR
		case 0xB0:	result = 0x00B0; break;	//	DEGREE SIGN
		case 0xB1:	result = 0x00B1; break;	//	PLUS-MINUS SIGN
		case 0xB2:	result = 0x00B2; break;	//	SUPERSCRIPT TWO
		case 0xB3:	result = 0x00B3; break;	//	SUPERSCRIPT THREE
		case 0xB4:	result = 0x0384; break;	//	GREEK TONOS
		case 0xB5:	result = 0x0385; break;	//	GREEK DIALYTIKA TONOS
		case 0xB6:	result = 0x0386; break;	//	GREEK CAPITAL LETTER ALPHA WITH TONOS
		case 0xB7:	result = 0x00B7; break;	//	MIDDLE DOT
		case 0xB8:	result = 0x0388; break;	//	GREEK CAPITAL LETTER EPSILON WITH TONOS
		case 0xB9:	result = 0x0389; break;	//	GREEK CAPITAL LETTER ETA WITH TONOS
		case 0xBA:	result = 0x038A; break;	//	GREEK CAPITAL LETTER IOTA WITH TONOS
		case 0xBB:	result = 0x00BB; break;	//	RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
		case 0xBC:	result = 0x038C; break;	//	GREEK CAPITAL LETTER OMICRON WITH TONOS
		case 0xBD:	result = 0x00BD; break;	//	VULGAR FRACTION ONE HALF
		case 0xBE:	result = 0x038E; break;	//	GREEK CAPITAL LETTER UPSILON WITH TONOS
		case 0xBF:	result = 0x038F; break;	//	GREEK CAPITAL LETTER OMEGA WITH TONOS
		case 0xC0:	result = 0x0390; break;	//	GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
		case 0xC1:	result = 0x0391; break;	//	GREEK CAPITAL LETTER ALPHA
		case 0xC2:	result = 0x0392; break;	//	GREEK CAPITAL LETTER BETA
		case 0xC3:	result = 0x0393; break;	//	GREEK CAPITAL LETTER GAMMA
		case 0xC4:	result = 0x0394; break;	//	GREEK CAPITAL LETTER DELTA
		case 0xC5:	result = 0x0395; break;	//	GREEK CAPITAL LETTER EPSILON
		case 0xC6:	result = 0x0396; break;	//	GREEK CAPITAL LETTER ZETA
		case 0xC7:	result = 0x0397; break;	//	GREEK CAPITAL LETTER ETA
		case 0xC8:	result = 0x0398; break;	//	GREEK CAPITAL LETTER THETA
		case 0xC9:	result = 0x0399; break;	//	GREEK CAPITAL LETTER IOTA
		case 0xCA:	result = 0x039A; break;	//	GREEK CAPITAL LETTER KAPPA
		case 0xCB:	result = 0x039B; break;	//	GREEK CAPITAL LETTER LAMDA
		case 0xCC:	result = 0x039C; break;	//	GREEK CAPITAL LETTER MU
		case 0xCD:	result = 0x039D; break;	//	GREEK CAPITAL LETTER NU
		case 0xCE:	result = 0x039E; break;	//	GREEK CAPITAL LETTER XI
		case 0xCF:	result = 0x039F; break;	//	GREEK CAPITAL LETTER OMICRON
		case 0xD0:	result = 0x03A0; break;	//	GREEK CAPITAL LETTER PI
		case 0xD1:	result = 0x03A1; break;	//	GREEK CAPITAL LETTER RHO
		case 0xD3:	result = 0x03A3; break;	//	GREEK CAPITAL LETTER SIGMA
		case 0xD4:	result = 0x03A4; break;	//	GREEK CAPITAL LETTER TAU
		case 0xD5:	result = 0x03A5; break;	//	GREEK CAPITAL LETTER UPSILON
		case 0xD6:	result = 0x03A6; break;	//	GREEK CAPITAL LETTER PHI
		case 0xD7:	result = 0x03A7; break;	//	GREEK CAPITAL LETTER CHI
		case 0xD8:	result = 0x03A8; break;	//	GREEK CAPITAL LETTER PSI
		case 0xD9:	result = 0x03A9; break;	//	GREEK CAPITAL LETTER OMEGA
		case 0xDA:	result = 0x03AA; break;	//	GREEK CAPITAL LETTER IOTA WITH DIALYTIKA
		case 0xDB:	result = 0x03AB; break;	//	GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA
		case 0xDC:	result = 0x03AC; break;	//	GREEK SMALL LETTER ALPHA WITH TONOS
		case 0xDD:	result = 0x03AD; break;	//	GREEK SMALL LETTER EPSILON WITH TONOS
		case 0xDE:	result = 0x03AE; break;	//	GREEK SMALL LETTER ETA WITH TONOS
		case 0xDF:	result = 0x03AF; break;	//	GREEK SMALL LETTER IOTA WITH TONOS
		case 0xE0:	result = 0x03B0; break;	//	GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
		case 0xE1:	result = 0x03B1; break;	//	GREEK SMALL LETTER ALPHA
		case 0xE2:	result = 0x03B2; break;	//	GREEK SMALL LETTER BETA
		case 0xE3:	result = 0x03B3; break;	//	GREEK SMALL LETTER GAMMA
		case 0xE4:	result = 0x03B4; break;	//	GREEK SMALL LETTER DELTA
		case 0xE5:	result = 0x03B5; break;	//	GREEK SMALL LETTER EPSILON
		case 0xE6:	result = 0x03B6; break;	//	GREEK SMALL LETTER ZETA
		case 0xE7:	result = 0x03B7; break;	//	GREEK SMALL LETTER ETA
		case 0xE8:	result = 0x03B8; break;	//	GREEK SMALL LETTER THETA
		case 0xE9:	result = 0x03B9; break;	//	GREEK SMALL LETTER IOTA
		case 0xEA:	result = 0x03BA; break;	//	GREEK SMALL LETTER KAPPA
		case 0xEB:	result = 0x03BB; break;	//	GREEK SMALL LETTER LAMDA
		case 0xEC:	result = 0x03BC; break;	//	GREEK SMALL LETTER MU
		case 0xED:	result = 0x03BD; break;	//	GREEK SMALL LETTER NU
		case 0xEE:	result = 0x03BE; break;	//	GREEK SMALL LETTER XI
		case 0xEF:	result = 0x03BF; break;	//	GREEK SMALL LETTER OMICRON
		case 0xF0:	result = 0x03C0; break;	//	GREEK SMALL LETTER PI
		case 0xF1:	result = 0x03C1; break;	//	GREEK SMALL LETTER RHO
		case 0xF2:	result = 0x03C2; break;	//	GREEK SMALL LETTER FINAL SIGMA
		case 0xF3:	result = 0x03C3; break;	//	GREEK SMALL LETTER SIGMA
		case 0xF4:	result = 0x03C4; break;	//	GREEK SMALL LETTER TAU
		case 0xF5:	result = 0x03C5; break;	//	GREEK SMALL LETTER UPSILON
		case 0xF6:	result = 0x03C6; break;	//	GREEK SMALL LETTER PHI
		case 0xF7:	result = 0x03C7; break;	//	GREEK SMALL LETTER CHI
		case 0xF8:	result = 0x03C8; break;	//	GREEK SMALL LETTER PSI
		case 0xF9:	result = 0x03C9; break;	//	GREEK SMALL LETTER OMEGA
		case 0xFA:	result = 0x03CA; break;	//	GREEK SMALL LETTER IOTA WITH DIALYTIKA
		case 0xFB:	result = 0x03CB; break;	//	GREEK SMALL LETTER UPSILON WITH DIALYTIKA
		case 0xFC:	result = 0x03CC; break;	//	GREEK SMALL LETTER OMICRON WITH TONOS
		case 0xFD:	result = 0x03CD; break;	//	GREEK SMALL LETTER UPSILON WITH TONOS
		case 0xFE:	result = 0x03CE; break;	//	GREEK SMALL LETTER OMEGA WITH TONOS
	};

	return result;
};


BKPlainText::BKPlainText() : buffer(0) { }
BKPlainText::~BKPlainText() {
	saveLastView();
	if (buffer)
		free(buffer);
}

BKPlainText* BKPlainText::create(string& file) {
	BKPlainText* r = new BKPlainText();
	r->fileName = file;

	// read file to memory
	FILE* f = fopen(file.c_str(), "r");
	logDebug("Attempting to open file: %s", file.c_str());
	if (f == NULL) {
		delete r;
		return NULL;
	}
	long length = 0;
	fseek(f, 0, SEEK_END);
	length = ftell(f);
	fseek(f, 0, SEEK_SET);
	if (length > MAX_FILE_SIZE_TO_LOAD)
		length = MAX_FILE_SIZE_TO_LOAD;
	char* b = (char*)malloc(length);
	fread(b, length, 1, f);
	fclose(f);

	uint32_t *utf32TextData = (uint32_t*) malloc(length * 4);
	logDebug("Started converting to ISO_8859_7");
	uint32_t *endOf32BitData = 
						utf8::unchecked::utf8to32(b, b + length, utf32TextData);


	memset(b, '\0', length);
	int decodedLength = endOf32BitData - utf32TextData;
	/* Change to ISO_8859_7 and also change any windows line endings to unix line endinds.
	 * The change of line endings will make the options.txtWrapCR option work correctly.
	 */
	for (int i = 0, outIndex = 0; i < decodedLength; i++) {

		unsigned char decoded = ISO_8859_7_INV_CHARMAP(utf32TextData[i]);

		if (	   decoded == '\n' 
				&& outIndex - 1 >= 0 
				&& b[outIndex - 1] == '\r') {
						b[outIndex - 1] = '\n';
		}
		else {
			b[outIndex++] = decoded;
		}

	}

	free(utf32TextData);
	b = (char*) realloc(b, decodedLength);
	logDebug("Done converting to ISO_8859_7");

	bool isHTML = false;
	// FIX: make the heuristic a bit more advanced than that...
	const char* fc = file.c_str();
	int fs = file.size();
	if (
		((fc[fs - 1] | 0x20) == 'l') &&
		((fc[fs - 2] | 0x20) == 'm') &&
		((fc[fs - 3] | 0x20) == 't') &&
		((fc[fs - 4] | 0x20) == 'h')
	) {
		isHTML = true;
	}
	if (
		((fc[fs - 1] | 0x20) == 'm') &&
		((fc[fs - 2] | 0x20) == 't') &&
		((fc[fs - 3] | 0x20) == 'h')
	) {
		isHTML = true;
	}
	
	if (isHTML) {
		r->buffer = BKFancyText::parseHTML(r, b, length);
	} else {
		/* parseText basically modifies "r", both by return value and inside
		 * the function. */
		r->buffer = BKFancyText::parseText(r, b, length);
	}

	r->resetFonts();
	r->resizeView(480, 272);
	return r;
}

void BKPlainText::getFileName(string& fn) {
	fn = fileName;
}

void BKPlainText::getTitle(string& t) {
	t = "FIX PLAIN TEXT TITLES";
}

void BKPlainText::getType(string& t) {
	t = "Plain text";
}

