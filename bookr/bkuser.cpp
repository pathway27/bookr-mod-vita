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

#include "tinyxml.h"
 
#include "fzscreen.h"
#include "bkuser.h"

#include <string>

BKUser::Controls BKUser::controls;
BKUser::Options BKUser::options;

int csSize = 0;

void BKUser::init() {
	setDefaultControls();
	setDefaultOptions();
	load();
}

void BKUser::setDefaultControls() {
	// set in-book default controls
	controls.previousPage     	= FZ_REPS_SQUARE;
	controls.nextPage         	= FZ_REPS_TRIANGLE;
	controls.previous10Pages  	= FZ_REPS_CIRCLE;
	controls.next10Pages      	= FZ_REPS_CROSS;
	controls.screenUp         	= FZ_REPS_UP;
	controls.screenDown       	= FZ_REPS_DOWN;
	controls.screenLeft       	= FZ_REPS_LEFT;
	controls.screenRight      	= FZ_REPS_RIGHT;
	controls.zoomIn           	= FZ_REPS_RTRIGGER;
	controls.zoomOut          	= FZ_REPS_LTRIGGER;
	controls.showMainMenu		= FZ_REPS_START;
	controls.showToolbar		= FZ_REPS_SELECT;
	
	// set menu default controls
	controls.select				= FZ_REPS_CIRCLE;//FZ_REPS_CIRCLE;
	controls.cancel				= FZ_REPS_CROSS;//FZ_REPS_CROSS;
	controls.alternate			= FZ_REPS_TRIANGLE;
	controls.details			= FZ_REPS_SQUARE;	
	controls.menuUp				= FZ_REPS_UP;
	controls.menuDown			= FZ_REPS_DOWN;
	controls.menuLeft			= FZ_REPS_LEFT;
	controls.menuRight			= FZ_REPS_RIGHT;	
	controls.resume				= FZ_REPS_START;
}

void BKUser::setDefaultOptions() {
	// set default options
	options.pdfFastScroll = false;
	options.txtRotation = 0;
	options.txtFont = "bookr:builtin";
	options.txtSize = 11;
	options.txtHeightPct = 100;

	options.colorSchemes.clear();
	
	ColorScheme aScheme;
	aScheme.txtBGColor = 0xffffff;
	aScheme.txtFGColor = 0;
	options.colorSchemes.push_back(aScheme);
	
	aScheme.txtBGColor = 0;
	aScheme.txtFGColor = 0xffffff;
	options.colorSchemes.push_back(aScheme);
	
	options.currentScheme = 0;

	options.txtJustify = true;
	options.pspSpeed = 0;
	options.pspMenuSpeed = 0;
	options.displayLabels = true;
	options.pdfInvertColors = false;
	options.lastFolder = FZScreen::basePath();
	options.lastFontFolder = FZScreen::basePath();
	options.loadLastFile = false;
	options.txtWrapCR = 0;
}

void BKUser::save() {
	char filename[1024];
	snprintf(filename, 1024, "%s/%s", FZScreen::basePath(), "user.xml");
	FILE* f = fopen(filename, "w");
	if (f == NULL) {
		printf("cannot save prefs to %s\n", filename);
		return;
	}

	fprintf(f, "<?xml version=\"1.0\" standalone=\"no\" ?>\n");
	fprintf(f, "<user>\n");
	fprintf(f, "\t<controls>\n");

	fprintf(f, "\t\t<bind action=\"controls.previousPage\" button=\"%d\" />\n", controls.previousPage);
	fprintf(f, "\t\t<bind action=\"controls.nextPage\" button=\"%d\" />\n", controls.nextPage);
	fprintf(f, "\t\t<bind action=\"controls.previous10Pages\" button=\"%d\" />\n", controls.previous10Pages);
	fprintf(f, "\t\t<bind action=\"controls.next10Pages\" button=\"%d\" />\n", controls.next10Pages);
	fprintf(f, "\t\t<bind action=\"controls.screenUp\" button=\"%d\" />\n", controls.screenUp);
	fprintf(f, "\t\t<bind action=\"controls.screenDown\" button=\"%d\" />\n", controls.screenDown);
	fprintf(f, "\t\t<bind action=\"controls.screenLeft\" button=\"%d\" />\n", controls.screenLeft);
	fprintf(f, "\t\t<bind action=\"controls.screenRight\" button=\"%d\" />\n", controls.screenRight);
	fprintf(f, "\t\t<bind action=\"controls.zoomIn\" button=\"%d\" />\n", controls.zoomIn);
	fprintf(f, "\t\t<bind action=\"controls.zoomOut\" button=\"%d\" />\n", controls.zoomOut);
	
	fprintf(f, "\t</controls>\n");

	fprintf(f, "\t<options>\n");
	fprintf(f, "\t\t<set option=\"txtSize\" value=\"%d\" />\n", options.txtSize);
	fprintf(f, "\t\t<set option=\"menuControlStyle\" value=\"%s\" />\n", controls.select == FZ_REPS_CIRCLE ? "asian" : "western");
	fprintf(f, "\t\t<set option=\"txtHeightPct\" value=\"%d\" />\n", options.txtHeightPct);
	for (unsigned int i = 0; i < options.colorSchemes.size(); i++) {
		fprintf(f, "\t\t<set option=\"colorScheme\" id=\"%d\" foreground=\"%d\" background=\"%d\" />\n", i, options.colorSchemes[i].txtFGColor, options.colorSchemes[i].txtBGColor);
	}
	fprintf(f, "\t\t<set option=\"currentScheme\" value=\"%d\" />\n", options.currentScheme);
	fprintf(f, "\t\t<set option=\"txtJustify\" value=\"%d\" />\n", options.txtJustify ? 1 : 0);
	fprintf(f, "\t\t<set option=\"pspSpeed\" value=\"%d\" />\n", options.pspSpeed);
	fprintf(f, "\t\t<set option=\"pspMenuSpeed\" value=\"%d\" />\n", options.pspMenuSpeed);
	fprintf(f, "\t\t<set option=\"displayLabels\" value=\"%d\" />\n", options.displayLabels ? 1 : 0);
	fprintf(f, "\t\t<set option=\"lastFolder\" value=\"%s\" />\n", options.lastFolder.c_str());
	fprintf(f, "\t\t<set option=\"lastFontFolder\" value=\"%s\" />\n", options.lastFontFolder.c_str());
	fprintf(f, "\t\t<set option=\"loadLastFile\" value=\"%d\" />\n", options.loadLastFile ? 1 : 0);
	fprintf(f, "\t\t<set option=\"txtWrapCR\" value=\"%d\" />\n", options.txtWrapCR);



	fprintf(f, "\t</options>\n");
	fprintf(f, "</user>\n");

	fclose(f);
}

void BKUser::load() {
	char filename[1024];
	snprintf(filename, 1024, "%s/%s", FZScreen::basePath(), "user.xml");
	
	TiXmlDocument *doc = new TiXmlDocument();
	doc->LoadFile(filename);

	if(doc->Error()) {
		printf("invalid %s, cannot load preferences: %s\n", filename, doc->ErrorDesc());
		delete doc;
		return;
	}

	TiXmlElement* root = doc->RootElement();
	TiXmlElement* controls = root->FirstChildElement("controls");
	if (controls != 0) {
		TiXmlElement* bind = controls->FirstChildElement("bind");
	
		while (bind) {
			const char* action = bind->Attribute("action");
			const char* button = bind->Attribute("button");
			if (action == 0 || button == 0) {
				printf("invalid user.xml in line %d\n", bind->Row());
				break;
			}
			int b = atoi(button);
			     if (strncmp(action, "controls.previousPage",    128) == 0) BKUser::controls.previousPage    = b;
			else if (strncmp(action, "controls.nextPage",        128) == 0) BKUser::controls.nextPage        = b;
			else if (strncmp(action, "controls.previous10Pages", 128) == 0) BKUser::controls.previous10Pages = b;
			else if (strncmp(action, "controls.next10Pages",     128) == 0) BKUser::controls.next10Pages     = b;
			else if (strncmp(action, "controls.screenUp",        128) == 0) BKUser::controls.screenUp        = b;
			else if (strncmp(action, "controls.screenDown",      128) == 0) BKUser::controls.screenDown      = b;
			else if (strncmp(action, "controls.screenLeft",      128) == 0) BKUser::controls.screenLeft      = b;
			else if (strncmp(action, "controls.screenRight",     128) == 0) BKUser::controls.screenRight     = b;
			else if (strncmp(action, "controls.zoomIn",          128) == 0) BKUser::controls.zoomIn          = b;
			else if (strncmp(action, "controls.zoomOut",         128) == 0) BKUser::controls.zoomOut         = b;
	
			bind = bind->NextSiblingElement("bind");
		}
	} else {
		printf("no controls found in user.xml\n");
	}

	TiXmlElement* eoptions = root->FirstChildElement("options");
	
	bool isFirstColorSchemeLoad = true;
	
	if (eoptions != 0) {
		TiXmlElement* eset = eoptions->FirstChildElement("set");
	
		while (eset) {
			const char* option = eset->Attribute("option");
			const char* value = eset->Attribute("value");
			if (option == 0 || (value == 0 && (strncmp(option, "colorScheme", 128) != 0))) {
				printf("invalid user.xml in line %d\n", eset->Row());
				break;
			}
			else if (strncmp(option, "menuControlStyle",128) == 0) {
				// default is asian so we only do the western case
				if(strncmp(value, "western", 128) == 0) {
					BKUser::controls.select = FZ_REPS_CROSS;
					BKUser::controls.cancel = FZ_REPS_CIRCLE;
				} else if (strncmp(value, "asian", 128) != 0) {
					printf("invalid user.xml in line %d\n", eset->Row());
					break;
				}
			}
			else if (strncmp(option, "txtSize",         128) == 0) options.txtSize         = atoi(value);
			else if (strncmp(option, "txtHeightPct",    128) == 0) options.txtHeightPct    = atoi(value);
			else if (strncmp(option, "txtSize",         128) == 0) options.txtSize         = atoi(value);
			else if (strncmp(option, "colorScheme",		128) == 0) {
				
				// Get the scheme number and color values
				const char* id = eset->Attribute("id");
				const char* foreground = eset->Attribute("foreground");
				const char* background = eset->Attribute("background");
				
				if (id == 0 || foreground == 0 || background == 0) {
					printf("invalid user.xml in line %d\n", eset->Row());
					break;
				}
				
				// This is needed to ensure that a user who wishes to have a single
				// color profile can do so. Not clearing would leave the second default
				// scheme active.
				if (isFirstColorSchemeLoad) {
					options.colorSchemes.clear();
					isFirstColorSchemeLoad = false;
				}
				
				int iId = atoi(id);
				if (iId >= options.colorSchemes.size()) {
					options.colorSchemes.resize(iId+1);
				}
				
				options.colorSchemes[iId].txtBGColor = atoi(background);
				options.colorSchemes[iId].txtFGColor = atoi(foreground);	 
			}
			else if (strncmp(option, "currentScheme",   128) == 0) options.currentScheme   = atoi(value);
			else if (strncmp(option, "pspSpeed",        128) == 0) options.pspSpeed        = atoi(value);
			else if (strncmp(option, "pspMenuSpeed",    128) == 0) options.pspMenuSpeed    = atoi(value);
			else if (strncmp(option, "displayLabels",   128) == 0) options.displayLabels   = atoi(value) != 0;
			else if (strncmp(option, "pdfInvertColors", 128) == 0) options.pdfInvertColors = atoi(value) != 0;
			else if (strncmp(option, "lastFolder",      128) == 0) options.lastFolder      = value;
			else if (strncmp(option, "lastFontFolder",  128) == 0) options.lastFontFolder  = value;
			else if (strncmp(option, "loadLastFile",    128) == 0) options.loadLastFile    = atoi(value) != 0;
			else if (strncmp(option, "txtWrapCR",       128) == 0) options.txtWrapCR       = atoi(value);
	
			eset = eset->NextSiblingElement("set"); 
		}
	} else {
		printf("no options found in user.xml\n");
	}

	doc->Clear();
	delete doc;

	// fix some possible errors before they crash the app
	bool operror = false;
	if (options.txtRotation != 0 && options.txtRotation != 90 && options.txtRotation != 180 && options.txtRotation != 270) {
		options.txtRotation = 0;
		operror = true;
	}
	
	if (options.txtSize < 6 || options.txtSize > 20) {
		options.txtSize = 11;
		operror = true;
	}
	if (options.txtHeightPct < 50 || options.txtHeightPct > 150) {
		options.txtHeightPct = 100;
		operror = true;
	}
	
	vector<ColorScheme>::iterator thisColor = options.colorSchemes.begin();
	while (thisColor != options.colorSchemes.end()) {

		if ((thisColor->txtFGColor & 0xff000000) != 0) {
			thisColor->txtFGColor &= 0xffffff;
			operror = true;
		}
		
		if ((thisColor->txtBGColor & 0xff000000) != 0) {
			thisColor->txtBGColor &= 0xffffff;
			operror = true;
		}
		thisColor++;
	}

	if (options.pspSpeed < 0 || options.pspSpeed > 6) {
		options.pspSpeed = 0;
		operror = true;
	}
	if (options.pspMenuSpeed < 0 || options.pspMenuSpeed > 6) {
		options.pspMenuSpeed = 0;
		operror = true;
	}
	if (operror)
		BKUser::save();

	//free(buffer);
}

