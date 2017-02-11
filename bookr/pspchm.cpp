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

#include <pspkernel.h>
#include <pspgu.h>
#include <psputility.h>
#include <psputility_htmlviewer.h>

#include <sys/stat.h>

#include <malloc.h>

#include "fzscreen.h"
#include "tinyxml.h"
#include "bkuser.h"
#include "bklogo.h"
#include "bkpopup.h"

#include "chmlib/chm_lib.h"
#include "vnconv/vnconv.h"

extern int ConvertFileToWebVIQR(const char *inFile, const char *outFile,
		int inCharset);

#define BROWSER_MEMORY (6*1024*1024)

int g_UpdateThreadId = -1;
string g_ChmPath;
string g_CachePath;
bool g_ExtractHTMDone = false;
bool g_ConvertToVN = false;

void convertFileToVN(string basePath, string filePath) {
	string infile = basePath + filePath;
	string outfile = infile + ".htm";

	char line[1024];
	FILE *fin, *fout;

	fin = fopen(infile.c_str(), "r");
	if (fin == (FILE *) 0)
		return;

	fout = fopen(outfile.c_str(), "w");
	if (fout == (FILE *) 0) {
		fclose(fin);
		return;
	}

	//	int charset = -1;
	int charset = CONV_CHARSET_UNIUTF8;

	while ((fgets(line, 1024, fin)) != NULL) {
		string tmp = line;

		if ((strstr(line, "charset=windows-1258") != NULL) || (strstr(line,
				"charset=windows-1252") != NULL)) {
			charset = -1;
			tmp.replace(tmp.find("charset=") + 8, 12, "iso-8859-1");
		} else if (strstr(line, "charset=utf-8") != NULL) {
			tmp.replace(tmp.find("charset=") + 8, 5, "iso-8859-1");
			//			charset = CONV_CHARSET_UNIUTF8;
		}

		fputs(tmp.c_str(), fout);
	}

	fclose(fout);
	fclose(fin);

	//	if (charset != -1) {
	ConvertFileToWebVIQR(outfile.c_str(), infile.c_str(), charset);
	//	}
	sceIoRemove(outfile.c_str());
}

void convertDirToVN(string basePath, string dir) {
	//	BKLogo::show("Converting dir: " + dir);
	vector<FZDirent> dirFiles;
	string dirPath = basePath + dir;

	FZScreen::dirContents((char*) dirPath.c_str(), dirFiles);
	int n = dirFiles.size();
	for (int i = 0; i < n; i++) {
		string s = dir + "/" + dirFiles[i].name;
		if (dirFiles[i].stat & FZ_STAT_IFDIR) {
			convertDirToVN(basePath, s);
		} else {
			string tmp = dirFiles[i].name.substr(dirFiles[i].name.find_last_of(
					"."));
			if (stricmp(tmp.c_str(), ".htm") == 0 || stricmp(tmp.c_str(), ".html")
					== 0) {
				//				BKLogo::show("Converting file: " + dirFiles[i].name);
				convertFileToVN(basePath, s);
			}
		}
	}

}

int htmlViewerInit(char *url, pspUtilityHtmlViewerParam &params,
		char* lastErrorMsg, int maxErrLen) {
	int res;

	memset(&params, 0, sizeof(pspUtilityHtmlViewerParam));

	params.base.size = sizeof(pspUtilityHtmlViewerParam);

	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE,
	&params.base.language);

	params.base.buttonSwap
			= (BKUser::controls.select == FZ_REPS_CIRCLE ? PSP_UTILITY_ACCEPT_CIRCLE
					: PSP_UTILITY_ACCEPT_CROSS);

	params.base.graphicsThread = 17;
	params.base.accessThread = 19;
	params.base.fontThread = 18;
	params.base.soundThread = 16;
	params.memsize = BROWSER_MEMORY;
	params.initialurl = url;
	params.numtabs = 2;
	params.cookiemode = PSP_UTILITY_HTMLVIEWER_COOKIEMODE_DISABLED;
	params.homeurl = url;
	params.textsize = BKUser::options.browserTextSize;
	params.displaymode = BKUser::options.browserDisplayMode;
	params.options = PSP_UTILITY_HTMLVIEWER_DISABLE_STARTUP_LIMITS;
	if (BKUser::options.browserEnableFlash)
		params.options |= PSP_UTILITY_HTMLVIEWER_ENABLE_FLASH;
	if (!BKUser::options.browserConfirmExit)
		params.options |= PSP_UTILITY_HTMLVIEWER_DISABLE_EXIT_DIALOG;
	if (!BKUser::options.browserShowCursor)
		params.options |= PSP_UTILITY_HTMLVIEWER_DISABLE_CURSOR;
	params.interfacemode = BKUser::options.browserInterfaceMode;
	params.connectmode = PSP_UTILITY_HTMLVIEWER_CONNECTMODE_MANUAL_ALL;

	// Note the lack of 'ms0:' on the paths
	params.dldirname = (char*) "/PSP/PHOTO";

	params.memaddr = (void*) malloc(BROWSER_MEMORY + 256);

	res = sceUtilityHtmlViewerInitStart(&params);

	if (res < 0) {
		snprintf(lastErrorMsg, maxErrLen,
				"Cannot open %s\nError 0x%08X initialising browser.\n", url,
				res);
		if (params.memaddr) {
			free(params.memaddr);
		}
		return res;
	}

	return 0;
}

int updateHtmlViewer() {
	FZScreen::startDirectList();
	FZScreen::clear(0xFFFFFF, GU_COLOR_BUFFER_BIT|GU_STENCIL_BUFFER_BIT |
	GU_DEPTH_BUFFER_BIT);
	FZScreen::endAndDisplayList();

	switch (sceUtilityHtmlViewerGetStatus()) {
		case PSP_UTILITY_DIALOG_VISIBLE:
		sceUtilityHtmlViewerUpdate(1);
		break;

		case PSP_UTILITY_DIALOG_QUIT:
		sceUtilityHtmlViewerShutdownStart();
		break;

		case PSP_UTILITY_DIALOG_NONE:
		return 0;
		break;

		default:
		break;
	}

	return 1;
}

struct extract_context {
	const char *base_path;
	int htmOnly;
	int hhcOnly;
	int showExtracting;
};

static int dir_exists(const char *path) {
	struct stat statbuf;
	if (stat(path, &statbuf) != -1)
		return 1;
	else
		return 0;
}

static int rmkdir(char *path) {
	/*
	 * strip off trailing components unless we can stat the directory, or we
	 * have run out of components
	 */

	char *i = rindex(path, '/');

	if (path[0] == '\0' || dir_exists(path))
		return 0;

	if (i != NULL) {
		*i = '\0';
		rmkdir(path);
		*i = '/';
		mkdir(path, 0777);
	}

	if (dir_exists(path))
		return 0;
	else
		return -1;
}

/*
 * callback function for enumerate API
 */
int extract_chm_callback(struct chmFile *h, struct chmUnitInfo *ui,
		void *context) {
	char buffer[32768];
	struct extract_context *ctx = (struct extract_context *) context;
	char *i;

	if (ui->flags & CHM_ENUMERATE_SPECIAL)
		return CHM_ENUMERATOR_CONTINUE;

	if (ui->path[0] != '/')
		return CHM_ENUMERATOR_CONTINUE;

	if (ctx->hhcOnly) {
		if (strstr(ui->path, ".hhc") == NULL)
			return CHM_ENUMERATOR_CONTINUE;
	}

	/* quick hack for security hole mentioned by Sven Tantau */
	if (strstr(ui->path, "/../") != NULL) {
		/* fprintf(stderr, "Not extracting %s (dangerous path)\n", ui->path); */
		return CHM_ENUMERATOR_CONTINUE;
	}

	if (snprintf(buffer, sizeof(buffer), "%s%s", ctx->base_path, ui->path)
			> 1024)
		return CHM_ENUMERATOR_FAILURE;

	if (ui->length != 0) {
		if (ctx->hhcOnly) {
			if (snprintf(buffer, sizeof(buffer), "%s/_hhc.xml", ctx->base_path)
					> 1024)
				return CHM_ENUMERATOR_FAILURE;
		} else {
			if (ctx->htmOnly) {
				if (strstr(ui->path, ".htm") == NULL)
					return CHM_ENUMERATOR_CONTINUE;
			} else {
				if (strstr(ui->path, ".htm") != NULL)
					return CHM_ENUMERATOR_CONTINUE;
			}
		}

		FILE *fout;
		LONGINT64 len, remain = ui->length;
		LONGUINT64 offset = 0;

		if (ctx->showExtracting) {
			string s = "Extracting file ";
			BKLogo::show(s + ui->path);
		}

		if ((fout = fopen(buffer, "wb")) == NULL) {
			/* make sure that it isn't just a missing directory before we abort */
			char newbuf[32768];
			strcpy(newbuf, buffer);
			i = rindex(newbuf, '/');
			*i = '\0';
			rmkdir(newbuf);
			if ((fout = fopen(buffer, "wb")) == NULL)
				return CHM_ENUMERATOR_FAILURE;
		}

		while (remain != 0) {
			len = chm_retrieve_object(h, ui, (unsigned char *) buffer, offset,
					32768);
			if (len > 0) {
				fwrite(buffer, 1, (size_t) len, fout);
				offset += len;
				remain -= len;
			} else {
				fprintf(stderr, "incomplete file: %s\n", ui->path);
				break;
			}
		}

		fclose(fout);

		if (ctx->hhcOnly) {
			return CHM_ENUMERATOR_SUCCESS;
		}
	} else {
		if (rmkdir(buffer) == -1)
			return CHM_ENUMERATOR_FAILURE;
	}

	return CHM_ENUMERATOR_CONTINUE;
}

static int extract_chm(const char *chmPath, const char *outPath, int htmOnly,
		int showExtracting = 0) {
	struct chmFile *h;
	struct extract_context ec;

	h = chm_open(chmPath);
	if (h == NULL) {
		chm_close(h);
		return -1;
	}

	ec.base_path = outPath;
	ec.htmOnly = htmOnly;
	ec.hhcOnly = 0;
	ec.showExtracting = showExtracting;
	if (!chm_enumerate(h, CHM_ENUMERATE_ALL, extract_chm_callback, (void *) &ec)) {
		chm_close(h);
		return -2;
	}

	chm_close(h);
	return 0;
}

int extract_hhc(const char *chmPath, const char *outPath) {
	struct chmFile *h;
	struct extract_context ec;

	h = chm_open(chmPath);
	if (h == NULL) {
		chm_close(h);
		return -1;
	}

	ec.base_path = outPath;
	ec.hhcOnly = 1;
	ec.showExtracting = 1;
	if (!chm_enumerate(h, CHM_ENUMERATE_ALL, extract_chm_callback, (void *) &ec)) {
		chm_close(h);
		return -2;
	}

	chm_close(h);
	return 0;
}

bool isCHM(string& file) {
	char header[4];
	memset((void*) header, 0, 4);
	FILE* f = fopen(file.c_str(), "r");
	fread(header, 4, 1, f);
	fclose(f);
	return header[0] == 'I' && header[1] == 'T' && header[2] == 'S'
			&& header[3] == 'F';
}

void copyFile(const char* from, const char* to) {

	char line[80 + 1];
	FILE *fout, *fin;

	fout = fopen(to, "w");
	if (fout == (FILE *) 0)
		return;

	fin = fopen(from, "r");
	if (fin == (FILE *) 0) {
		fclose(fout);
		return;
	}

	while ((fgets(line, 80, fin)) != NULL) {

		fputs(line, fout);

	}

	fclose(fout);
	fclose(fin);
}

bool searchIndexFile(string dir) {
	vector<FZDirent> dirFiles;

	FZScreen::dirContents((char*) dir.c_str(), dirFiles);
	int n = dirFiles.size();
	for (int i = 0; i < n; i++) {
		if (!(dirFiles[i].stat & FZ_STAT_IFDIR) && (stricmp(dirFiles[i].name.c_str(), "index.htm") == 0 || stricmp(
				dirFiles[i].name.c_str(), "index.html") == 0)) {

			string from = dir + "/" + dirFiles[i].name;
			string to = dir + "/_hhc.htm";
			copyFile(from.c_str(), to.c_str());
			return true;
		}
	}

	return false;
}

void buildTOC(string &out, string dir, string baseDir) {
	vector<FZDirent> dirFiles;

	FZScreen::dirContents((char*) dir.c_str(), dirFiles);
	int n = dirFiles.size();
	for (int i = 0; i < n; i++) {
		string s = dir + "/" + dirFiles[i].name;
		if (dirFiles[i].stat & FZ_STAT_IFDIR) {
			out += "<ul>" + dirFiles[i].name + "\n";
			buildTOC(out, s, baseDir + "/" + dirFiles[i].name);
			out += "</ul>\n";
		} else {
			string tmp = dirFiles[i].name.substr(dirFiles[i].name.find_last_of(
					"."));
			if (stricmp(tmp.c_str(), ".htm") == 0 || stricmp(tmp.c_str(), ".html")
					== 0) {
				out += "<li><a href='" + baseDir + "/" + dirFiles[i].name
						+ "' target='content'>" + dirFiles[i].name + "</li>\n";
			}
		}
	}
}

void buildTOCFile(string cachePath, string title) {

	string s = "<html>\n<head>\n<title>";
	s += title;
	s += "</title></head>\n<body>\n";
	buildTOC(s, cachePath, ".");
	s += "</body>\n</html>\n";

	string path = cachePath + "/_hhc.htm";
	FILE *fout;

	fout = fopen(path.c_str(), "w");
	if (fout == (FILE *) 0) {
		return;
	}

	fputs(s.c_str(), fout);
	fclose(fout);
}

int extractCHMThread(SceSize args, void *argp) {
	FZScreen::setSpeed(6); // 333Mhz/166Mhz
	if (!g_ExtractHTMDone)
		extract_chm(g_ChmPath.c_str(), g_CachePath.c_str(), 1); // extract htm files first

	if (g_ConvertToVN)
		convertDirToVN(g_CachePath, "");

	extract_chm(g_ChmPath.c_str(), g_CachePath.c_str(), 0);

	FZScreen::setSpeed(BKUser::options.pspSpeed);

	g_UpdateThreadId = -1;

	return 0;
}

int asyncExtractCHM() {
	if (g_UpdateThreadId >= 0) {
		sceKernelTerminateDeleteThread(g_UpdateThreadId);
	}

	g_UpdateThreadId = sceKernelCreateThread("extract_thread",
			extractCHMThread, 0x11, 256 * 1024, PSP_THREAD_ATTR_USER, NULL);
	if (g_UpdateThreadId >= 0) {
		sceKernelStartThread(g_UpdateThreadId, 0, 0);
	}
	return g_UpdateThreadId;
}

void processElement(TiXmlElement* element) {
	if (element) {
		if (stricmp(element->Value(), "object") == 0) {
			const char *linkHref = 0;
			const char *linkText = 0;
			TiXmlElement* eParam = element->FirstChildElement("param");

			while (eParam) {
				const char* aName = eParam->Attribute("name");
				const char* aValue = eParam->Attribute("value");

				if (aName && aValue) {
					if (strncmp(aName, "Name", 128) == 0)
						linkText = aValue;
					else if (strncmp(aName, "Local", 128) == 0)
						linkHref = aValue;
				}

				eParam = eParam->NextSiblingElement("param");
			}

			if (linkText && linkHref) {
				TiXmlElement newnode("a");
				newnode.SetAttribute("href", linkHref);
				newnode.SetAttribute("target", "content");
				newnode.LinkEndChild(new TiXmlText(linkText));
				element->Parent()->ReplaceChild(element, newnode);
			}
		} else {
			if (element->FirstChildElement()) {
				processElement(element->FirstChildElement());
			}
		}

		if (element->NextSiblingElement()) {
			processElement(element->NextSiblingElement());
		}
	}
}

int convert_hhc(string cachePath) {

	string filename = cachePath + "/_hhc.xml";

	TiXmlDocument *doc = new TiXmlDocument();
	doc->LoadFile(filename.c_str());

	if (doc->Error()) {
		//		BKLogo::show(doc->ErrorDesc(), 5);
		delete doc;
		return -1;
	}

	processElement(doc->RootElement());

	filename = cachePath + "/_hhc.htm";
	doc->SaveFile(filename.c_str());

	doc->Clear();
	delete doc;

	return 0;
}

int processCHMHTM(bkLayers& layers, string filepath, bool convertToVN) {
	char lastErrorMsg[256];

	g_ConvertToVN = convertToVN;

	int iErr = 0;
	if (isCHM(filepath)) {
		if (g_UpdateThreadId >= 0) {
			sceKernelTerminateDeleteThread(g_UpdateThreadId);
			sceKernelWaitThreadEnd(g_UpdateThreadId, NULL);
			g_UpdateThreadId = -1;
		}

		string filename = filepath.substr(filepath.find_last_of("/") + 1);

		g_CachePath = FZScreen::basePath();
		g_CachePath += "/CACHE/";
		g_CachePath += filename;

		g_ChmPath = filepath;

		BKLogo::show("Extracting hhc file...");
		iErr = extract_hhc(g_ChmPath.c_str(), g_CachePath.c_str());

		if (iErr < 0) {
			snprintf(lastErrorMsg, 256, "Invalid chm file: %s\n",
					g_ChmPath.c_str());
			layers.push_back(BKPopup::create(BKPOPUP_ERROR,
			lastErrorMsg));
			return iErr;
		}

		filepath = g_CachePath;
		filepath += "/_hhc.htm";

		BKLogo::show("Converting hhc file...");
		iErr = convert_hhc(g_CachePath);

		if (iErr < 0) {
			BKLogo::show("Extracting all html files to cache...");
			FZScreen::setSpeed(6); // 333Mhz/166Mhz
			extract_chm(g_ChmPath.c_str(), g_CachePath.c_str(), 1, 1); // extract htm files first
			FZScreen::setSpeed(BKUser::options.pspSpeed);

			if (iErr < 0) {
				BKLogo::show("Error in hhc, searching for Index file...");
				if (searchIndexFile(g_CachePath) == false) {
					BKLogo::show("Index file not found, building file list...");
					buildTOCFile(g_CachePath, filename);
				}
			}

			g_ExtractHTMDone = true;
			asyncExtractCHM();

		} else {
			g_ExtractHTMDone = false;
			asyncExtractCHM();
		}
	} else {
		if (convertToVN) {
			convertFileToVN("", filepath);
		}
	}

	string tmp = filepath.substr(0, filepath.find(":"));

	if (strcmp(tmp.c_str(), "ms0") == 0) {
		filepath = "file" + filepath.substr(3);
	}

	pspUtilityHtmlViewerParam params;

	iErr = htmlViewerInit((char*) filepath.c_str(), params, lastErrorMsg, 256);

	if (iErr < 0) {
		layers.push_back(BKPopup::create(BKPOPUP_ERROR, lastErrorMsg));
		return iErr;
	}

	while (updateHtmlViewer()) {
		FZScreen::waitVblankStart();
		FZScreen::swapBuffers();
	}

	if (params.memaddr) {
		free(params.memaddr);
	}

	return 0;
}
