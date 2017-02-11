#pragma once

#include <vector>
#include <string>

using namespace std;

class BKLocalization
{
private:
	static int currentIndex;
public:
	struct Resources
	{
		// Font Settings
		string language;

		// Font Settings
		string fontFile;
		int fontBigSize;
		int fontSmallSize;

		// Buttons
		string buttonSelect; //"Select"
		string buttonStart; //"Start"
		string buttonUp; //"Up"
		string buttonRight; //"Right"
		string buttonDown; //"Down"
		string buttonLeft; //"Left"
		string buttonLeftTrigger; //"Left trigger"
		string buttonRightTrigger; //"Right trigger"
		string buttonTriangle; //"Triangle"
		string buttonCircle; //"Circle"
		string buttonCross; //"Cross"
		string buttonSquare; //"Square"
		string buttonHome; //"Home"
		string buttonHold; //"Hold"
		string buttonNote; //"Note"

		// Main Menu Items
		string mainMenuItemSelect; //"Select"
		string mainMenuItemOpenFile; // "Open file"
		string mainMenuItemBrowseChmCache; // Browse chm cache
		string mainMenuItemControls; // Controls
		string mainMenuItemOptions; // Options
		string mainMenuItemAbout; // About
		string mainMenuItemExit; // Exit

		// Control Menu
		string controlMenuItemRestoreDefaults; // Restore defaults
		string controlMenuItemPreviousPage; // Previous page: 
		string controlMenuItemNextPage; // Next page: 
		string controlMenuItemPrevious10Pages; // Previous 10 pages: 
		string controlMenuItemNext10pages; // Next 10 pages: 
		string controlMenuItemScreenUp; // Screen up: 
		string controlMenuItemScreenDown; // Screen down: 
		string controlMenuItemScreenLeft; // Screen left: 
		string controlMenuItemScreenRight; // Screen right: 
		string controlMenuItemZoomIn; // Zoom in: 
		string controlMenuItemZoomOut; // Zoom out: 
		string controlMenuItemChangeButton; // Change button
		string controlMenuItemSelect; // Select

		// Option Menu
		string optionMenuItemRestoreDefaults; // Restore defaults
		string optionMenuItemMenuLanguage; // Menu language: 
		string optionMenuItemMenuControlStyle; // Menu control style: 
		string optionMenuItemMenuControlStyleAsian; // Asian
		string optionMenuItemMenuControlStyleWestern; // Western
		string optionMenuItemPdfFastImage; // PDF - Fast images (zoom limited to 2x): 
		string optionMenuItemPdfInvertColors; // PDF - Invert colors: 
		string optionMenuItemPlainTextFontFile; // Plain text - Font file: 
		string optionMenuItemPlainTextBuiltIn; // built-in
		string optionMenuItemSelectFontFile; // Select font file
		string optionMenuItemUseBuiltInFont; // Use built-in font
		string optionMenuItemPlainTextFontSize; // Plain text - Font size: 
		string optionMenuItemPlainTextLineHeight; // Plain text - Line Height: 
		string optionMenuItemPlainTextJustifyText; // Plain text - Justify text: 
		string optionMenuItemPlainTextWrapCRs; // Plain text - Wrap CRs: 
		string optionMenuItemBrowserTextSize; // Browser - Text size:
		string optionMenuItemBrowserDisplayMode; // Browser - Display mode: 
		string optionMenuItemBrowserFlashContent; // Browser - Flash content:  
		string optionMenuItemBrowserInterfaceMode; // Browser - Interface mode: 
		string optionMenuItemBrowserExitConfirmation; // Browser - Exit confirmation: 
		string optionMenuItemBrowserShowCursor; // Browser - Show cursor: 
		string optionMenuItemColorSchemes; // Color schemes : 

		string optionMenuItemPixels; // pixels
		string optionMenuItemPct; // Pct
		string optionMenuItemEnabled; // Enabled
		string optionMenuItemDisabled; // Disabled

		string optionMenuItemManageSchemes; // Manage schemes
		string optionMenuItemDisplayPageLoading; // Display page loading and numbering labels: 
		string optionMenuItemAutoLoadLastFile; // Autoload last file: 
		string optionMenuItemCPUSpeed; // CPU/Bus speed: 
		string optionMenuItemMenuSpeed; // Menu speed: 
		string optionMenuItemClearBookmarks; // Clear bookmarks

		string optionMenuItemChoose; // Choose
		string optionMenuItemChange; // Change
		string optionMenuItemToggle; // Toggle
		string optionMenuItemSelect; // Select

		string mainMenuTitle;
		string controlMenuTitle;
		string optionMenuTitle;

		string bookmarksClearedWarning;
		string pdfFastWarning;
		string pressNewButtonWarning;

		string pressStart;
		string loading;
		string addBookmark;
		string page;
		string buttonJumpTo;
		string buttonDelete;
		string noBookmarksSupport;
		string firstPage;
		string lastPage;
		string previous10Pages;
		string next10Pages;
		string goToPage;
		string noPaginationSupport;
		string fitHeight;
		string fitWidth;
		string zoomOut;
		string zoomIn;
		string noZoomSupport;
		string rotate90Clockwise;
		string rotate90Counterclockwise;
		string noRotationSupport;
		string of;

		string buttonYes; //"Yes"
		string buttonNo; //"No"
		string buttonOpen; //"Open"
		string clearCache;
		string deletingCacheWarning;
		string deleteCacheWarning;
		string clearCacheWarning;
		string cacheMenuTitle;
		string buttonDeleteCache;

		string colorSchemesMenuItemColorScheme;
		string colorSchemesMenuItemAddColorScheme;
		string buttonModify;
		string buttonAdd;
		
		string emptyFolder;
		string parentFolder;
		string buttonSelectFile;
		string buttonOpenFolder;

		string selectTextColor;
		string selectBackgroundColor;
		string buttonSelectColor;
		string buttonSwitchToSaturation;
		string buttonSwitchToHue;
		string selectedColor;

		string openFileMenuTitle;
		string openFontMenuTitle;
		string invalidFontFileWarning;
		string manageColorSchemesMenuTitle;

		string browserTextSizeLarge;
		string browserTextSizeNormal;
		string browserTextSizeSmall;

		string browserDisplayModesNormal;
		string browserDisplayModesFit;
		string browserDisplayModesSmartFit;

		string browserInterfaceModesFull;
		string browserInterfaceModesLimited;
		string browserInterfaceModesNone;
	};

	static BKLocalization::Resources getDefault();
	static void init();
	static void loadMenuFont();
	static bool loadResourcesFromFile(const char* filename, BKLocalization::Resources &resource);

	static vector<BKLocalization::Resources> availableLanguages;
	static Resources current;
};


