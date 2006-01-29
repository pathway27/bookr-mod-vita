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

#include "tinyxml.h"
 
#include "fzscreen.h"
#include "bkuser.h"

BKUser::Controls BKUser::controls;
BKUser::Options BKUser::options;

void BKUser::init() {
	setDefaultControls();
	setDefaultOptions();
	load();
}

void BKUser::setDefaultControls() {
	// set default controls for pdf
	controls.previousPage     = FZ_REPS_SQUARE;
	controls.nextPage         = FZ_REPS_TRIANGLE;
	controls.previous10Pages  = FZ_REPS_CIRCLE;
	controls.next10Pages      = FZ_REPS_CROSS;
	controls.screenUp         = FZ_REPS_UP;
	controls.screenDown       = FZ_REPS_DOWN;
	controls.zoomIn           = FZ_REPS_RTRIGGER;
	controls.zoomOut          = FZ_REPS_LTRIGGER;
}

void BKUser::setDefaultOptions() {
	// set default options
	options.pdfFastScroll = false;
	options.txtRotation = 0;
	options.txtFont = "bookr:builtin";
	options.txtSize = 11;
	options.txtFGColor = 0;
	options.txtBGColor = 0xffffff;
	options.txtJustify = true;
	options.pspSpeed = 0;
	options.displayLabels = true;
	options.pdfInvertColors = false;
	options.pdfBGColor = 0x505050;
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
	fprintf(f, "\t\t<bind action=\"controls.zoomIn\" button=\"%d\" />\n", controls.zoomIn);
	fprintf(f, "\t\t<bind action=\"controls.zoomOut\" button=\"%d\" />\n", controls.zoomOut);

	fprintf(f, "\t</controls>\n");

	fprintf(f, "\t<options>\n");

	fprintf(f, "\t\t<set option=\"pdfFastScroll\" value=\"%d\" />\n", options.pdfFastScroll ? 1 : 0);
	fprintf(f, "\t\t<set option=\"txtRotation\" value=\"%d\" />\n", options.txtRotation);
	fprintf(f, "\t\t<set option=\"txtFont\" value=\"%s\" />\n", options.txtFont.c_str());
	fprintf(f, "\t\t<set option=\"txtSize\" value=\"%d\" />\n", options.txtSize);
	fprintf(f, "\t\t<set option=\"txtFGColor\" value=\"%d\" />\n", options.txtFGColor);
	fprintf(f, "\t\t<set option=\"txtBGColor\" value=\"%d\" />\n", options.txtBGColor);
	fprintf(f, "\t\t<set option=\"txtJustify\" value=\"%d\" />\n", options.txtJustify ? 1 : 0);
	fprintf(f, "\t\t<set option=\"pspSpeed\" value=\"%d\" />\n", options.pspSpeed);
	fprintf(f, "\t\t<set option=\"displayLabels\" value=\"%d\" />\n", options.displayLabels ? 1 : 0);
	fprintf(f, "\t\t<set option=\"pdfInvertColors\" value=\"%d\" />\n", options.pdfInvertColors ? 1 : 0);
	fprintf(f, "\t\t<set option=\"pdfBGColor\" value=\"%d\" />\n", options.pdfBGColor);

	fprintf(f, "\t</options>\n");
	fprintf(f, "</user>\n");

	fclose(f);
}

void BKUser::load() {
	char filename[1024];
	snprintf(filename, 1024, "%s/%s", FZScreen::basePath(), "user.xml");
	FILE* f = fopen(filename, "r");
	// no existing preferences, probably the first time run of the app
	if (f == NULL)
		return;
	
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
			else if (strncmp(action, "controls.zoomIn",          128) == 0) BKUser::controls.zoomIn          = b;
			else if (strncmp(action, "controls.zoomOut",         128) == 0) BKUser::controls.zoomOut         = b;
	
			bind = bind->NextSiblingElement("bind");
		}
	} else {
		printf("no controls found in user.xml\n");
	}

	TiXmlElement* eoptions = root->FirstChildElement("options");
	if (eoptions != 0) {
		TiXmlElement* eset = eoptions->FirstChildElement("set");
	
		while (eset) {
			const char* option = eset->Attribute("option");
			const char* value = eset->Attribute("value");
			if (option == 0 || value == 0) {
				printf("invalid user.xml in line %d\n", eset->Row());
				break;
			}
			     if (strncmp(option, "pdfFastScroll",   128) == 0) options.pdfFastScroll   = atoi(value) != 0;
			else if (strncmp(option, "txtRotation",     128) == 0) options.txtRotation     = atoi(value);
			else if (strncmp(option, "txtFont",         128) == 0) options.txtFont         = value;
			else if (strncmp(option, "txtSize",         128) == 0) options.txtSize         = atoi(value);
			else if (strncmp(option, "txtFGColor",      128) == 0) options.txtFGColor      = atoi(value);
			else if (strncmp(option, "txtBGColor",      128) == 0) options.txtBGColor      = atoi(value);
			else if (strncmp(option, "txtJustify",      128) == 0) options.txtJustify      = atoi(value) != 0;
			else if (strncmp(option, "pspSpeed",        128) == 0) options.pspSpeed        = atoi(value);
			else if (strncmp(option, "displayLabels",   128) == 0) options.displayLabels   = atoi(value) != 0;
			else if (strncmp(option, "pdfInvertColors", 128) == 0) options.pdfInvertColors = atoi(value) != 0;
			else if (strncmp(option, "pdfBGColor",      128) == 0) options.pdfBGColor      = atoi(value);
	
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
	if (options.txtSize < 6 && options.txtSize > 20) {
		options.txtSize = 11;
		operror = true;
	}
	if ((options.txtFGColor & 0xff000000) != 0) {
		options.txtFGColor &= 0xffffff;
		operror = true;
	}
	if ((options.txtBGColor & 0xff000000) != 0) {
		options.txtBGColor &= 0xffffff;
		operror = true;
	}
	if ((options.pdfBGColor & 0xff000000) != 0) {
		options.pdfBGColor &= 0xffffff;
		operror = true;
	}
	if (options.pspSpeed < 0 || options.pspSpeed > 6) {
		options.pspSpeed = 0;
		operror = true;
	}
	if (operror)
		BKUser::save();

	//free(buffer);
}

