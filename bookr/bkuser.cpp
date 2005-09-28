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

BKUser::Controls BKUser::pdfControls;
BKUser::Controls BKUser::txtControls;

void BKUser::init() {
	setDefaultControls();
	load();
}

void BKUser::setDefaultControls() {
	// set default controls for txt. the non-set controls are unused
	memset((void*)&BKUser::txtControls, 0, sizeof(BKUser::Controls));
	txtControls.previousPage = FZ_REPS_UP;
	txtControls.nextPage     = FZ_REPS_DOWN;
	// set default controls for pdf
	pdfControls.previousPage     = FZ_REPS_SQUARE;
	pdfControls.nextPage         = FZ_REPS_TRIANGLE;
	pdfControls.previous10Pages  = FZ_REPS_CIRCLE;
	pdfControls.next10Pages      = FZ_REPS_CROSS;
	pdfControls.panUp            = FZ_REPS_UP;
	pdfControls.panDown          = FZ_REPS_DOWN;
	pdfControls.panLeft          = FZ_REPS_LEFT;
	pdfControls.panRight         = FZ_REPS_RIGHT;
	pdfControls.zoomIn           = FZ_REPS_RTRIGGER;
	pdfControls.zoomOut          = FZ_REPS_LTRIGGER;
}

void BKUser::save() {
	char filename[1024];
	snprintf(filename, 1024, "%s/data/%s", FZScreen::basePath(), "user.xml");
	FILE* f = fopen(filename, "w");

	fprintf(f, "<?xml version=\"1.0\" standalone=\"no\" ?>\n");
	fprintf(f, "<user>\n");
	fprintf(f, "\t<controls>\n");

	fprintf(f, "\t\t<bind action=\"txtControls.previousPage\" button=\"%d\" />\n", txtControls.previousPage);
	fprintf(f, "\t\t<bind action=\"txtControls.nextPage\" button=\"%d\" />\n", txtControls.nextPage);

	fprintf(f, "\t\t<bind action=\"pdfControls.previousPage\" button=\"%d\" />\n", pdfControls.previousPage);
	fprintf(f, "\t\t<bind action=\"pdfControls.nextPage\" button=\"%d\" />\n", pdfControls.nextPage);
	fprintf(f, "\t\t<bind action=\"pdfControls.previous10Pages\" button=\"%d\" />\n", pdfControls.previous10Pages);
	fprintf(f, "\t\t<bind action=\"pdfControls.next10Pages\" button=\"%d\" />\n", pdfControls.next10Pages);
	fprintf(f, "\t\t<bind action=\"pdfControls.panUp\" button=\"%d\" />\n", pdfControls.panUp);
	fprintf(f, "\t\t<bind action=\"pdfControls.panDown\" button=\"%d\" />\n", pdfControls.panDown);
	fprintf(f, "\t\t<bind action=\"pdfControls.panLeft\" button=\"%d\" />\n", pdfControls.panLeft);
	fprintf(f, "\t\t<bind action=\"pdfControls.panRight\" button=\"%d\" />\n", pdfControls.panRight);
	fprintf(f, "\t\t<bind action=\"pdfControls.zoomIn\" button=\"%d\" />\n", pdfControls.zoomIn);
	fprintf(f, "\t\t<bind action=\"pdfControls.zoomOut\" button=\"%d\" />\n", pdfControls.zoomOut);

	fprintf(f, "\t</controls>\n");
	fprintf(f, "</user>\n");

	fclose(f);
}

void BKUser::load() {
	char filename[1024];
	snprintf(filename, 1024, "%s/data/%s", FZScreen::basePath(), "user.xml");
	FILE* f = fopen(filename, "r");
	// no existing preferences, probably the first time run of the app
	if (f == NULL)
		return;
	
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	char* buffer = (char*)malloc(size);
	memset((void*)buffer, 0, size);
	fseek(f, 0, SEEK_SET);
	fread(buffer, size, 1, f);
	fclose(f);

	TiXmlDocument *doc = new TiXmlDocument();
	doc->Parse(buffer);

	if(doc->Error()) {
		printf("invalid %s, cannot load preferences: %s, %d\n", filename, doc->ErrorDesc(), size);
		delete doc;
		return;
	}

	TiXmlElement* root = doc->RootElement();
	TiXmlElement* controls = root->FirstChildElement("controls");
	TiXmlElement* bind = controls->FirstChildElement("bind");

	while (bind) {
		const char* action = bind->Attribute("action");
		const char* button = bind->Attribute("button");
		if (action == 0 || button == 0) {
			printf("invalid user.xml in line %d\n", bind->Row());
			break;
		}
		int b = atoi(button);
		     if (strncmp(action, "txtControls.previousPage",    128) == 0) txtControls.previousPage    = b;
		else if (strncmp(action, "txtControls.nextPage",        128) == 0) txtControls.nextPage        = b;
		else if (strncmp(action, "pdfControls.previousPage",    128) == 0) pdfControls.previousPage    = b;
		else if (strncmp(action, "pdfControls.nextPage",        128) == 0) pdfControls.nextPage        = b;
		else if (strncmp(action, "pdfControls.previous10Pages", 128) == 0) pdfControls.previous10Pages = b;
		else if (strncmp(action, "pdfControls.next10Pages",     128) == 0) pdfControls.next10Pages     = b;
		else if (strncmp(action, "pdfControls.panUp",           128) == 0) pdfControls.panUp           = b;
		else if (strncmp(action, "pdfControls.panDown",         128) == 0) pdfControls.panDown         = b;
		else if (strncmp(action, "pdfControls.panLeft",         128) == 0) pdfControls.panLeft         = b;
		else if (strncmp(action, "pdfControls.panRight",        128) == 0) pdfControls.panRight        = b;
		else if (strncmp(action, "pdfControls.zoomIn",          128) == 0) pdfControls.zoomIn          = b;
		else if (strncmp(action, "pdfControls.zoomOut",         128) == 0) pdfControls.zoomOut         = b;

		bind = bind->NextSiblingElement("bind"); 
	}

	doc->Clear();
	delete doc;
}

