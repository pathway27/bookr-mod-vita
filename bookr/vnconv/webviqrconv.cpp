/*------------------------------------------------------------------------------
VnConv: Vietnamese Encoding Converter Library
UniKey Project: http://unikey.sourceforge.net
Copyleft (C) 1998-2002 Pham Kim Long
Contact: longp@cslab.felk.cvut.cz

Convert to WebVIQR on PSP by Nguyen Chi Tam (nguyenchitam@gmail.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
--------------------------------------------------------------------------------*/

#include "../fzscreen.h"
#include "bklogo.h"
#include <pspthreadman.h>
#include <stdio.h>

#include "charset.h"
#include "data.h"

extern UnicodeChar UnicodeTable[TOTAL_VNCHARS];

string Win1252WebVIQRMap = "";

string WebVIQRCodeTable[TOTAL_VNCHARS] = { "" };
int loadedWebVIQR = 0;

void LoadWebVIQRCodeTable() {
	char line[1024 + 1];
	FILE *fin;

	fin = fopen("WebVIQR.txt", "r");
	if (fin == (FILE *) 0)
		return;

	loadedWebVIQR = 0;
	Win1252WebVIQRMap = "";

	char buffer[256];

	while ((fgets(line, 1024, fin)) != NULL) {
		int len = strlen(line);

		while (len > 0 && (line[len - 1] == '\r' || line[len - 1] == '\n')) {
			line[--len] = 0;
		}

		if (len > 0) {
			WebVIQRCodeTable[loadedWebVIQR] = line;
			snprintf(buffer, sizeof(buffer), "&#%d;%s",
					UnicodeTable[loadedWebVIQR], line);
			Win1252WebVIQRMap += buffer;
		}

		if (++loadedWebVIQR > TOTAL_VNCHARS)
			break;

	}

	fclose(fin);
	Win1252WebVIQRMap += "&#;";
}

int WebVIQR_putChar(ByteOutStream & os, StdVnChar stdChar, int & outLen) {
	int ret = 0;
	if (stdChar >= VnStdCharOffset) {
		unsigned int offset = stdChar - VnStdCharOffset;
		if (WebVIQRCodeTable[offset].length() == 0) {
			outLen = 1;
			unsigned char ch = (stdChar == StdStartQuote) ? PadStartQuote
					: ((stdChar == StdEndQuote) ? PadEndQuote : ((stdChar
							== StdEllipsis) ? PadEllipsis : PadChar));
			ret = os.putB(ch);
		} else {
			outLen = WebVIQRCodeTable[offset].length();
			ret = os.puts(WebVIQRCodeTable[offset].c_str(), outLen);
		}
	} else {
		if (stdChar > 255) {
			//this character is missing in the charset
			// output padding character
			outLen = 1;
			ret = os.putB(PadChar);
		} else {
			outLen = 1;
			ret = os.putB((BYTE) stdChar);
		}
	}
	return ret;
}

int ConvertToWebVIQR(VnCharset & incs, ByteInStream & input,
		ByteOutStream & output) {
	StdVnChar stdChar;
	int bytesRead, bytesWritten;

	incs.startInput();
	//	outcs.startOutput();

	int ret = 1;
	while (!input.eos()) {
		if (incs.nextInput(input, stdChar, bytesRead)) {
			if (stdChar != INVALID_STD_CHAR) {
				//			  if (VnCharsetLibObj.m_options.toLower)
				//			    stdChar = StdVnToLower(stdChar);
				//			  else if (VnCharsetLibObj.m_options.toUpper)
				//			    stdChar = StdVnToUpper(stdChar);
				//			  if (VnCharsetLibObj.m_options.removeTone)
				//			    stdChar = StdVnRemoveTone(stdChar);
				ret = WebVIQR_putChar(output, stdChar, bytesWritten);
			}
		} else
			break;
	}
	return (ret ? 0 : VNCONV_OUT_OF_MEMORY);
}

int Win1252ToWebVIQR(FileBIStream is, FileBOStream os) {

	unsigned char ch;
	string s = "";

	while (!is.eos()) {
		is.getNext(ch);
		if (ch == '&') {
			if (s.length() > 0) {
				os.puts(s.c_str(), s.length());
			}
			s = ch;
		} else if (ch == ';') {
			if (s.length() > 0) {
				s += ch;
				string::size_type start = Win1252WebVIQRMap.find(s.c_str());
				if (start != string::npos) {
					start += s.length();
					string::size_type end = Win1252WebVIQRMap.find_first_of(
							"&", start);
					string tmp = Win1252WebVIQRMap.substr(start, end - start);
					os.puts(tmp.c_str(), tmp.length());
				} else {
					os.puts(s.c_str(), s.length());
				}
				s = "";
			} else {
				os.putB(ch);
			}
		} else if (s.length() > 0) {
			s += ch;
		} else {
			os.putB(ch);
		}
	}

	return 1;
}

int ConvertFileToWebVIQR(const char *inFile, const char *outFile, int inCharset) {
	FileBIStream is;
	FileBOStream os;

	if (is.open(inFile) == 0) {
		return 0;
	}

	if (os.open(outFile) == 0) {
		is.close();
		return 0;
	}

	if (loadedWebVIQR == 0) {
		LoadWebVIQRCodeTable();
	}

	if (inCharset == -1) {
		Win1252ToWebVIQR(is, os);
	} else {
		VnCharset *pInCharset = VnCharsetLibObj.getVnCharset(inCharset);
		ConvertToWebVIQR(*pInCharset, is, os);
	}

	is.close();
	os.close();

	return 1;
}
