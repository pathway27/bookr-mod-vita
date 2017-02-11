#include "bklocalization.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "tinyxml.h"
#include "fzscreen.h"
#include "bklayer.h"

extern "C" 
{
	extern unsigned int size_res_uifont;
	extern unsigned char res_uifont[];
};

BKLocalization::Resources BKLocalization::current;
vector<BKLocalization::Resources> BKLocalization::availableLanguages;

bool isLanguage(string& file) 
{
	string tmp = file.substr(file.find_last_of("."));
	return stricmp(tmp.c_str(), ".lan") == 0 ;
}

void BKLocalization::loadMenuFont()
{
	//printf("BKMainMenu::rebuildMenuFont\n");

	// Delete Font
	if(BKLayer::fontBig!=NULL)
	{
		delete BKLayer::fontBig; BKLayer::fontBig = NULL;
	}
	if(BKLayer::fontSmall!=NULL)
	{
		delete BKLayer::fontSmall; BKLayer::fontSmall = NULL;
	}

	char fontPath[1024];
	snprintf(fontPath, 1024, "%s/%s/%s", FZScreen::basePath(), "languages", BKLocalization::current.fontFile.c_str());

	// Load Big Font
	BKLayer::fontBig = FZFont::createFromFile(fontPath, BKLocalization::current.fontBigSize, false);

	if(BKLayer::fontBig == NULL)
		BKLayer::fontBig = FZFont::createFromMemory(res_uifont, size_res_uifont, 14, false);
	BKLayer::fontBig->texEnv(FZ_TEX_MODULATE);
	BKLayer::fontBig->filter(FZ_NEAREST, FZ_NEAREST);

	// Load Small Font
	BKLayer::fontSmall = FZFont::createFromFile(fontPath, BKLocalization::current.fontSmallSize, false);

	if(BKLayer::fontSmall == NULL)
		BKLayer::fontSmall = FZFont::createFromMemory(res_uifont, size_res_uifont, 11, false);
	BKLayer::fontSmall->texEnv(FZ_TEX_MODULATE);
	BKLayer::fontSmall->filter(FZ_NEAREST, FZ_NEAREST);
}

void BKLocalization::init()
{
	vector<FZDirent> dirFiles;

	char langDirPath[1024];
	snprintf(langDirPath, 1024, "%s/%s", FZScreen::basePath(), "languages");

	int err = FZScreen::dirContents(langDirPath, dirFiles);

	int n = dirFiles.size();

	availableLanguages.push_back(BKLocalization::getDefault());

	int currentIndex = 0;

	for (int i = 0; i < n; i++) 
	{
		FZDirent item = dirFiles[i];

		if (!(item.stat & FZ_STAT_IFDIR) && isLanguage(item.name))
		{
			// Load Language File
			BKLocalization::Resources resources = BKLocalization::getDefault();

			char filePath[1024];
			snprintf(filePath, 1024, "%s/%s", langDirPath, item.name.c_str());

			if(BKLocalization::loadResourcesFromFile(filePath, resources))
			{
				availableLanguages.push_back(resources);

				if(resources.language == BKUser::options.menuLanguage)
					currentIndex = availableLanguages.size()-1;
			}
		}
	}

	//printf("BKLocalization::init currentIndex = %d", currentIndex);
	BKLocalization::current = availableLanguages[currentIndex];

	BKUser::options.menuLanguage = BKLocalization::current.language;

	loadMenuFont();
}


BKLocalization::Resources BKLocalization::getDefault()
{
	BKLocalization::Resources retVal;

	retVal.language = "English";

	//// Font Settings
	retVal.fontFile;
	retVal.fontBigSize = 14;
	retVal.fontSmallSize = 11;

	
	// Buttons
	retVal.buttonSelect = "Select";
	retVal.buttonStart = "Start";
	retVal.buttonUp = "Up";
	retVal.buttonRight = "Right";
	retVal.buttonDown = "Down";
	retVal.buttonLeft = "Left";
	retVal.buttonLeftTrigger = "Left trigger";
	retVal.buttonRightTrigger = "Right trigger";
	retVal.buttonTriangle = "Triangle";
	retVal.buttonCircle = "Circle";
	retVal.buttonCross = "Cross";
	retVal.buttonSquare = "Square";
	retVal.buttonHome = "Home";
	retVal.buttonHome = "Hold";
	retVal.buttonNote = "Note";

	//// Main Menu Items
	retVal.mainMenuTitle = "Main Menu";
	retVal.mainMenuItemSelect = "Select";
	retVal.mainMenuItemOpenFile = "Open file";
	retVal.mainMenuItemBrowseChmCache = "Browse chm cache";
	retVal.mainMenuItemControls = "Controls";
	retVal.mainMenuItemOptions = "Options";
	retVal.mainMenuItemAbout = "About";
	retVal.mainMenuItemExit = "Exit";

	//// Control Menu Items
	retVal.controlMenuTitle = "Customize controls";
	retVal.controlMenuItemRestoreDefaults =  "Restore defaults";
	retVal.controlMenuItemPreviousPage = "Previous page: "; 
	retVal.controlMenuItemNextPage = "Next page:"; 
	retVal.controlMenuItemPrevious10Pages = "Previous 10 pages: ";
	retVal.controlMenuItemNext10pages = "Next 10 pages: ";
	retVal.controlMenuItemScreenUp = "Screen up: ";
	retVal.controlMenuItemScreenDown = "Screen down: ";
	retVal.controlMenuItemScreenLeft = "Screen left: ";
	retVal.controlMenuItemScreenRight = "Screen right: ";
	retVal.controlMenuItemZoomIn = "Zoom in: ";
	retVal.controlMenuItemZoomOut = "Zoom out: ";
	retVal.controlMenuItemChangeButton = "Change button";
	retVal.controlMenuItemSelect = "Select";

	// Options Menu
	retVal.optionMenuTitle = "Options";
	retVal.optionMenuItemRestoreDefaults = "Restore defaults";
	retVal.optionMenuItemMenuLanguage= "Menu language: ";
	retVal.optionMenuItemMenuControlStyle= "Menu control style: ";
	retVal.optionMenuItemMenuControlStyleAsian= "Asian";
	retVal.optionMenuItemMenuControlStyleWestern= "Western";
	retVal.optionMenuItemPdfFastImage= "PDF - Fast images (zoom limited to 2x): ";
	retVal.optionMenuItemPdfInvertColors= "PDF - Invert colors: ";
	retVal.optionMenuItemPlainTextFontFile= "Plain text - Font file: ";
	retVal.optionMenuItemPlainTextBuiltIn = "built-in";
	retVal.optionMenuItemSelectFontFile = "Select font file";
	retVal.optionMenuItemUseBuiltInFont = "Use built-in font";
	retVal.optionMenuItemPlainTextFontSize = "Plain text - Font size: ";
	retVal.optionMenuItemPlainTextLineHeight = "Plain text - Line Height: ";
	retVal.optionMenuItemPlainTextJustifyText = "Plain text - Justify text: ";
	retVal.optionMenuItemPlainTextWrapCRs = "Plain text - Wrap CRs: ";
	retVal.optionMenuItemBrowserTextSize = "Browser - Text size: ";
	retVal.optionMenuItemBrowserDisplayMode = "Browser - Display mode: ";
	retVal.optionMenuItemBrowserFlashContent = "Browser - Flash content:  ";
	retVal.optionMenuItemBrowserInterfaceMode = "Browser - Interface mode: ";
	retVal.optionMenuItemBrowserExitConfirmation = "Browser - Exit confirmation: ";
	retVal.optionMenuItemBrowserShowCursor = "Browser - Show cursor: ";
	retVal.optionMenuItemColorSchemes = "Color schemes: ";

	retVal.optionMenuItemPixels = " pixels";
	retVal.optionMenuItemPct = " Pct";
	retVal.optionMenuItemEnabled = "Enabled";
	retVal.optionMenuItemDisabled = "Disabled";

	retVal.optionMenuItemManageSchemes = "Manage schemes";
	retVal.optionMenuItemDisplayPageLoading = "Display page loading and numbering labels: ";
	retVal.optionMenuItemAutoLoadLastFile = "Autoload last file: ";
	retVal.optionMenuItemCPUSpeed = "CPU/Bus speed: ";
	retVal.optionMenuItemMenuSpeed = "Menu speed: ";
	retVal.optionMenuItemClearBookmarks = "Clear bookmarks";

	retVal.optionMenuItemChoose = "Choose";
	retVal.optionMenuItemChange = "Change";
	retVal.optionMenuItemToggle = "Toggle";
	retVal.optionMenuItemSelect = "Select";

	retVal.bookmarksClearedWarning = "Bookmarks cleared.";
	retVal.pdfFastWarning = "Fast images will cause instability with many PDF files.\nWhen reporting a bug make it very clear if you were using\nfast images or not. Also try the same file without fast\nimages mode before reporting a bug.";
	retVal.pressNewButtonWarning = "Press the new button";

	retVal.pressStart = "Press Start";
	retVal.loading = "Loading...";
	retVal.addBookmark = "Add bookmark";
	retVal.page = "Page ";
	retVal.buttonJumpTo = "Jump to";
	retVal.buttonDelete = "Delete";
	retVal.noBookmarksSupport = "No bookmark support";
	retVal.firstPage = "First Page";
	retVal.lastPage = "Last Page";
	retVal.previous10Pages = "Previous 10 pages";
	retVal.next10Pages = "Next 10 pages";
	retVal.goToPage = "Go to page";
	retVal.noPaginationSupport = "No pagination support";
	retVal.fitHeight = "Fit height";
	retVal.fitWidth = "Fit width";
	retVal.zoomOut = "Zoom out";
	retVal.zoomIn = "Zoom in";
	retVal.noZoomSupport = "No zoom support";
	retVal.rotate90Clockwise = "Rotate 90° clockwise";
	retVal.rotate90Counterclockwise = "Rotate 90° counterclockwise";
	retVal.noRotationSupport = "No rotation support";
	retVal.of = " of ";

	retVal.buttonYes = "Yes";
	retVal.buttonNo = "No";
	retVal.buttonOpen = "Open";
	retVal.clearCache = "Clear cache";
	retVal.deletingCacheWarning = "Deleting cache: ";
	retVal.deleteCacheWarning = "Are you sure to delete ";
	retVal.clearCacheWarning = "Are you sure to clear all caches?";
	retVal.cacheMenuTitle = "Cache";
	retVal.buttonDeleteCache = "Delete cache";
	retVal.buttonModify = "Modify";
	retVal.colorSchemesMenuItemColorScheme = "Color Scheme";
	retVal.colorSchemesMenuItemAddColorScheme = "Add a color scheme...";
	retVal.buttonAdd = "Add";
	retVal.emptyFolder = "<Empty folder>";
	retVal.parentFolder = "Parent folder";
	retVal.buttonSelectFile = "Select file";
	retVal.buttonOpenFolder = "Open folder";
	retVal.selectTextColor = "Select text color (plain text only)";
	retVal.selectBackgroundColor = "Select background color (plain text and PDF)";
	retVal.buttonSelectColor = "Select this color";
	retVal.buttonSwitchToSaturation = "Switch to saturation/value selection";
	retVal.buttonSwitchToHue = "Switch to hue selection";
	retVal.selectedColor = "Selected color";
	retVal.openFileMenuTitle = "Open (use SQUARE to open Vietnamese chm/html)";
	retVal.openFontMenuTitle = "Select font file to open";
	retVal.invalidFontFileWarning = "The selected file is not a valid TrueType font.";
	retVal.manageColorSchemesMenuTitle = "Manage color schemes";

	retVal.browserTextSizeLarge = "Large";
	retVal.browserTextSizeNormal = "Normal";
	retVal.browserTextSizeSmall = "Small";

	retVal.browserDisplayModesNormal = "Normal";
	retVal.browserDisplayModesFit = "Fit";
	retVal.browserDisplayModesSmartFit = "Smart-Fit";

	retVal.browserInterfaceModesFull = "Full";
	retVal.browserInterfaceModesLimited = "Limited";
	retVal.browserInterfaceModesNone = "None";

	return retVal;
}

bool BKLocalization::loadResourcesFromFile(const char* filename, BKLocalization::Resources &resource)
{
	TiXmlDocument *doc = new TiXmlDocument();
	doc->LoadFile(filename, TIXML_ENCODING_LEGACY);

	if(doc->Error()) 
	{
		printf("invalid %s, cannot load language: %s\n", filename, doc->ErrorDesc());
		delete doc;
		return true;
	}

	resource.language = filename;

	int startPos = resource.language.find_last_of("/")+1;
	int len = resource.language.find_last_of(".") - startPos;
	resource.language = resource.language.substr(startPos, len);

	TiXmlElement* root = doc->RootElement();
	if(root!=0)
	{
		TiXmlElement* data = root->FirstChildElement("data");

		while(data)
		{
			const char* name = data->Attribute("name");
			const char* value = data->Attribute("value");
			if (name == 0 || value == 0) 
			{
				printf("invalid %s in line %d\n", filename, data->Row());
				continue;
			}

			// Font
			if (strncmp(name, "fontFile",    128) == 0)
				resource.fontFile = value;
			// Buttons
			else if (strncmp(name, "buttonSelect",    128) == 0)
				resource.buttonSelect = value;
			else if (strncmp(name, "buttonStart",    128) == 0)
				resource.buttonStart = value;
			else if (strncmp(name, "buttonUp",    128) == 0)
				resource.buttonUp = value;
			else if (strncmp(name, "buttonDown",    128) == 0)
				resource.buttonDown = value;
			else if (strncmp(name, "buttonRight",    128) == 0)
				resource.buttonRight = value;
			else if (strncmp(name, "buttonLeft",    128) == 0)
				resource.buttonLeft = value;
			else if (strncmp(name, "buttonLeftTrigger",    128) == 0)
				resource.buttonLeftTrigger = value;
			else if (strncmp(name, "buttonRightTrigger",    128) == 0)
				resource.buttonRightTrigger = value;
			else if (strncmp(name, "buttonTriangle",    128) == 0)
				resource.buttonTriangle = value;
			else if (strncmp(name, "buttonCircle",    128) == 0)
				resource.buttonCircle = value;
			else if (strncmp(name, "buttonCross",    128) == 0)
				resource.buttonCross = value;
			else if (strncmp(name, "buttonSquare",    128) == 0)
				resource.buttonSquare = value;
			else if (strncmp(name, "buttonHome",    128) == 0)
				resource.buttonHome = value;
			else if (strncmp(name, "buttonHold",    128) == 0)
				resource.buttonHold = value;
			else if (strncmp(name, "buttonNote",    128) == 0)
				resource.buttonNote = value;
			// Main Menu
			else if (strncmp(name, "fontBigSize",    128) == 0)
				resource.fontBigSize = atoi(value);
			else if (strncmp(name, "fontSmallSize",    128) == 0)
				resource.fontSmallSize = atoi(value);
			else if (strncmp(name, "mainMenuItemOpenFile",    128) == 0)
				resource.mainMenuItemOpenFile = value;
			else if (strncmp(name, "mainMenuItemSelect",    128) == 0)
				resource.mainMenuItemSelect = value;
			else if (strncmp(name, "mainMenuItemBrowseChmCache",    128) == 0)
				resource.mainMenuItemBrowseChmCache = value;
			else if (strncmp(name, "mainMenuItemControls",    128) == 0)
				resource.mainMenuItemControls = value;
			else if (strncmp(name, "mainMenuItemOptions",    128) == 0)
				resource.mainMenuItemOptions = value;
			else if (strncmp(name, "mainMenuItemAbout",    128) == 0)
				resource.mainMenuItemAbout = value;
			else if (strncmp(name, "mainMenuItemExit",    128) == 0)
				resource.mainMenuItemExit = value;
			// Controls
			else if (strncmp(name, "controlMenuItemRestoreDefaults",    128) == 0)
				resource.controlMenuItemRestoreDefaults = value;
			else if (strncmp(name, "controlMenuItemPreviousPage",    128) == 0)
				resource.controlMenuItemPreviousPage = value;
			else if (strncmp(name, "controlMenuItemNextPage",    128) == 0)
				resource.controlMenuItemNextPage = value;
			else if (strncmp(name, "controlMenuItemPrevious10Pages",    128) == 0)
				resource.controlMenuItemPrevious10Pages = value;
			else if (strncmp(name, "controlMenuItemNext10pages",    128) == 0)
				resource.controlMenuItemNext10pages = value;
			else if (strncmp(name, "controlMenuItemScreenUp",    128) == 0)
				resource.controlMenuItemScreenUp = value;
			else if (strncmp(name, "controlMenuItemScreenDown",    128) == 0)
				resource.controlMenuItemScreenDown = value;
			else if (strncmp(name, "controlMenuItemScreenLeft",    128) == 0)
				resource.controlMenuItemScreenLeft = value;
			else if (strncmp(name, "controlMenuItemScreenRight",    128) == 0)
				resource.controlMenuItemScreenRight = value;
			else if (strncmp(name, "controlMenuItemZoomIn",    128) == 0)
				resource.controlMenuItemZoomIn = value;
			else if (strncmp(name, "controlMenuItemZoomOut",    128) == 0)
				resource.controlMenuItemZoomOut = value;
			else if (strncmp(name, "controlMenuItemChangeButton",    128) == 0)
				resource.controlMenuItemChangeButton = value;
			else if (strncmp(name, "controlMenuItemSelect",    128) == 0)
				resource.controlMenuItemSelect = value;
			// Options
			else if (strncmp(name, "optionMenuItemRestoreDefaults",    128) == 0)
				resource.optionMenuItemRestoreDefaults = value;
			else if (strncmp(name, "optionMenuItemMenuLanguage",    128) == 0)
				resource.optionMenuItemMenuLanguage = value;
			else if (strncmp(name, "optionMenuItemMenuControlStyle",    128) == 0)
				resource.optionMenuItemMenuControlStyle = value;
			else if (strncmp(name, "optionMenuItemMenuControlStyleAsian",    128) == 0)
				resource.optionMenuItemMenuControlStyleAsian = value;
			else if (strncmp(name, "optionMenuItemMenuControlStyleWestern",    128) == 0)
				resource.optionMenuItemMenuControlStyleWestern = value;
			else if (strncmp(name, "optionMenuItemPdfFastImage",    128) == 0)
				resource.optionMenuItemPdfFastImage = value;
			else if (strncmp(name, "optionMenuItemPdfInvertColors",    128) == 0)
				resource.optionMenuItemPdfInvertColors = value;
			else if (strncmp(name, "optionMenuItemPlainTextFontFile",    128) == 0)
				resource.optionMenuItemPlainTextFontFile = value;
			else if (strncmp(name, "optionMenuItemPlainTextBuiltIn",    128) == 0)
				resource.optionMenuItemPlainTextBuiltIn = value;
			else if (strncmp(name, "optionMenuItemSelectFontFile",    128) == 0)
				resource.optionMenuItemSelectFontFile = value;
			else if (strncmp(name, "optionMenuItemUseBuiltInFont",    128) == 0)
				resource.optionMenuItemUseBuiltInFont = value;
			else if (strncmp(name, "optionMenuItemPlainTextFontSize",    128) == 0)
				resource.optionMenuItemPlainTextFontSize = value;
			else if (strncmp(name, "optionMenuItemPlainTextLineHeight",    128) == 0)
				resource.optionMenuItemPlainTextLineHeight = value;
			else if (strncmp(name, "optionMenuItemPlainTextJustifyText",    128) == 0)
				resource.optionMenuItemPlainTextJustifyText = value;
			else if (strncmp(name, "optionMenuItemPlainTextWrapCRs",    128) == 0)
				resource.optionMenuItemPlainTextWrapCRs = value;
			else if (strncmp(name, "optionMenuItemBrowserTextSize",    128) == 0)
				resource.optionMenuItemBrowserTextSize = value;
			else if (strncmp(name, "optionMenuItemBrowserDisplayMode",    128) == 0)
				resource.optionMenuItemBrowserDisplayMode = value;
			else if (strncmp(name, "optionMenuItemBrowserFlashContent",    128) == 0)
				resource.optionMenuItemBrowserFlashContent = value;
			else if (strncmp(name, "optionMenuItemBrowserInterfaceMode",    128) == 0)
				resource.optionMenuItemBrowserInterfaceMode = value;
			else if (strncmp(name, "optionMenuItemBrowserExitConfirmation",    128) == 0)
				resource.optionMenuItemBrowserExitConfirmation = value;
			else if (strncmp(name, "optionMenuItemBrowserShowCursor",    128) == 0)
				resource.optionMenuItemBrowserShowCursor = value;
			else if (strncmp(name, "optionMenuItemColorSchemes",    128) == 0)
				resource.optionMenuItemColorSchemes = value;
			else if (strncmp(name, "optionMenuItemPixels",    128) == 0)
				resource.optionMenuItemPixels = value;
			else if (strncmp(name, "optionMenuItemPct",    128) == 0)
				resource.optionMenuItemPct = value;
			else if (strncmp(name, "optionMenuItemEnabled",    128) == 0)
				resource.optionMenuItemEnabled = value;
			else if (strncmp(name, "optionMenuItemDisabled",    128) == 0)
				resource.optionMenuItemDisabled = value;
			else if (strncmp(name, "optionMenuItemManageSchemes",    128) == 0)
				resource.optionMenuItemManageSchemes = value;
			else if (strncmp(name, "optionMenuItemDisplayPageLoading",    128) == 0)
				resource.optionMenuItemDisplayPageLoading = value;
			else if (strncmp(name, "optionMenuItemAutoLoadLastFile",    128) == 0)
				resource.optionMenuItemAutoLoadLastFile = value;
			else if (strncmp(name, "optionMenuItemCPUSpeed",    128) == 0)
				resource.optionMenuItemCPUSpeed = value;
			else if (strncmp(name, "optionMenuItemMenuSpeed",    128) == 0)
				resource.optionMenuItemMenuSpeed = value;
			else if (strncmp(name, "optionMenuItemClearBookmarks",    128) == 0)
				resource.optionMenuItemClearBookmarks = value;
			else if (strncmp(name, "optionMenuItemChoose",    128) == 0)
				resource.optionMenuItemChoose = value;
			else if (strncmp(name, "optionMenuItemChange",    128) == 0)
				resource.optionMenuItemChange = value;
			else if (strncmp(name, "optionMenuItemToggle",    128) == 0)
				resource.optionMenuItemToggle = value;
			else if (strncmp(name, "optionMenuItemSelect",    128) == 0)
				resource.optionMenuItemSelect = value;
			else if (strncmp(name, "mainMenuTitle",    128) == 0)
				resource.mainMenuTitle = value;
			else if (strncmp(name, "controlMenuTitle",    128) == 0)
				resource.controlMenuTitle = value;
			else if (strncmp(name, "optionMenuTitle",    128) == 0)
				resource.optionMenuTitle = value;
			else if (strncmp(name, "bookmarksClearedWarning",    128) == 0)
				resource.bookmarksClearedWarning = value;
			else if (strncmp(name, "pdfFastWarning",    128) == 0)
				resource.pdfFastWarning = value;
			else if (strncmp(name, "pressNewButtonWarning",    128) == 0)
				resource.pressNewButtonWarning = value;
			else if (strncmp(name, "pressStart",    128) == 0)
				resource.pressStart = value;
			else if (strncmp(name, "loading",    128) == 0)
				resource.loading = value;
			else if (strncmp(name, "addBookmark",    128) == 0)
				resource.addBookmark = value;
			else if (strncmp(name, "page",    128) == 0)
				resource.page = value;
			else if (strncmp(name, "buttonJumpTo",    128) == 0)
				resource.buttonJumpTo = value;
			else if (strncmp(name, "buttonDelete",    128) == 0)
				resource.buttonDelete = value;
			else if (strncmp(name, "noBookmarksSupport",    128) == 0)
				resource.noBookmarksSupport = value;
			else if (strncmp(name, "firstPage",    128) == 0)
				resource.firstPage = value;
			else if (strncmp(name, "lastPage",    128) == 0)
				resource.lastPage = value;
			else if (strncmp(name, "previous10Pages",    128) == 0)
				resource.previous10Pages = value;
			else if (strncmp(name, "next10Pages",    128) == 0)
				resource.next10Pages = value;
			else if (strncmp(name, "goToPage",    128) == 0)
				resource.goToPage = value;
			else if (strncmp(name, "noPaginationSupport",    128) == 0)
				resource.noPaginationSupport = value;
			else if (strncmp(name, "fitHeight",    128) == 0)
				resource.fitHeight = value;
			else if (strncmp(name, "fitWidth",    128) == 0)
				resource.fitWidth = value;
			else if (strncmp(name, "zoomOut",    128) == 0)
				resource.zoomOut = value;
			else if (strncmp(name, "zoomIn",    128) == 0)
				resource.zoomIn = value;
			else if (strncmp(name, "noZoomSupport",    128) == 0)
				resource.noZoomSupport = value;
			else if (strncmp(name, "rotate90Clockwise",    128) == 0)
				resource.rotate90Clockwise = value;
			else if (strncmp(name, "rotate90Counterclockwise",    128) == 0)
				resource.rotate90Counterclockwise = value;
			else if (strncmp(name, "noRotationSupport",    128) == 0)
				resource.noRotationSupport = value;
			else if (strncmp(name, "of",    128) == 0)
				resource.of = value;
			else if (strncmp(name, "buttonYes",    128) == 0)
				resource.buttonYes = value;
			else if (strncmp(name, "buttonNo",    128) == 0)
				resource.buttonNo = value;
			else if (strncmp(name, "buttonOpen",    128) == 0)
				resource.buttonOpen = value;
			else if (strncmp(name, "clearCache",    128) == 0)
				resource.clearCache = value;
			else if (strncmp(name, "deletingCacheWarning",    128) == 0)
				resource.deletingCacheWarning = value;
			else if (strncmp(name, "deleteCacheWarning",    128) == 0)
				resource.deleteCacheWarning = value;
			else if (strncmp(name, "clearCacheWarning",    128) == 0)
				resource.clearCacheWarning = value;
			else if (strncmp(name, "cacheMenuTitle",    128) == 0)
				resource.cacheMenuTitle = value;
			else if (strncmp(name, "buttonDeleteCache",    128) == 0)
				resource.buttonDeleteCache = value;
			else if (strncmp(name, "buttonModify",    128) == 0)
				resource.buttonModify = value;
			else if (strncmp(name, "colorSchemesMenuItemColorScheme",    128) == 0)
				resource.colorSchemesMenuItemColorScheme = value;
			else if (strncmp(name, "colorSchemesMenuItemAddColorScheme",    128) == 0)
				resource.colorSchemesMenuItemAddColorScheme = value;
			else if (strncmp(name, "buttonAdd",    128) == 0)
				resource.buttonAdd = value;
			else if (strncmp(name, "emptyFolder",    128) == 0)
				resource.emptyFolder = value;
			else if (strncmp(name, "parentFolder",    128) == 0)
				resource.parentFolder = value;
			else if (strncmp(name, "buttonSelectFile",    128) == 0)
				resource.buttonSelectFile = value;
			else if (strncmp(name, "buttonOpenFolder",    128) == 0)
				resource.buttonOpenFolder = value;
			else if (strncmp(name, "selectTextColor",    128) == 0)
				resource.selectTextColor = value;
			else if (strncmp(name, "selectBackgroundColor",    128) == 0)
				resource.selectBackgroundColor = value;
			else if (strncmp(name, "buttonSelectColor",    128) == 0)
				resource.buttonSelectColor = value;
			else if (strncmp(name, "buttonSwitchToSaturation",    128) == 0)
				resource.buttonSwitchToSaturation = value;
			else if (strncmp(name, "buttonSwitchToHue",    128) == 0)
				resource.buttonSwitchToHue = value;
			else if (strncmp(name, "selectedColor",    128) == 0)
				resource.selectedColor = value;
			else if (strncmp(name, "openFileMenuTitle",    128) == 0)
				resource.openFileMenuTitle = value;
			else if (strncmp(name, "openFontMenuTitle",    128) == 0)
				resource.openFontMenuTitle = value;
			else if (strncmp(name, "invalidFontFileWarning",    128) == 0)
				resource.invalidFontFileWarning = value;
			else if (strncmp(name, "manageColorSchemesMenuTitle",    128) == 0)
				resource.manageColorSchemesMenuTitle = value;
			else if (strncmp(name, "browserTextSizeLarge",    128) == 0)
				resource.browserTextSizeLarge = value;
			else if (strncmp(name, "browserTextSizeNormal",    128) == 0)
				resource.browserTextSizeNormal = value;
			else if (strncmp(name, "browserTextSizeSmall",    128) == 0)
				resource.browserTextSizeSmall = value;
			else if (strncmp(name, "browserDisplayModesNormal",    128) == 0)
				resource.browserDisplayModesNormal = value;
			else if (strncmp(name, "browserDisplayModesFit",    128) == 0)
				resource.browserDisplayModesFit = value;
			else if (strncmp(name, "browserDisplayModesSmartFit",    128) == 0)
				resource.browserDisplayModesSmartFit = value;
			else if (strncmp(name, "browserInterfaceModesFull",    128) == 0)
				resource.browserInterfaceModesFull = value;
			else if (strncmp(name, "browserInterfaceModesLimited",    128) == 0)
				resource.browserInterfaceModesLimited = value;
			else if (strncmp(name, "browserInterfaceModesNone",    128) == 0)
				resource.browserInterfaceModesNone = value;

			data = data->NextSiblingElement("data");
		}
	}
	else
	{
		printf("no controls localization in %s\n", filename);
		return false;
	}

	doc->Clear();
	delete doc;

	return true;
}
