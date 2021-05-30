/*
 * bookr-modern: a graphics based document reader for switch, vita and desktop
 * Copyright (C) 2019 pathway27 (Sree)
 * IS A MODIFICATION OF THE ORIGINAL
 * Bookr and bookr-mod for PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com),
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com),
 * AND VARIOUS OTHER FORKS, See Forks in README.md
 * Licensed under GPLv3+, see LICENSE
*/

#include <tinyxml2.h>
#include <string>

#include "graphics/controls.hpp"
#include "graphics/screen.hpp"
#include "user.hpp"

using namespace tinyxml2;

namespace bookr {

Controls User::controls;
Options User::options;

int csSize = 0;

void User::init() {
  setDefaultControls();
  setDefaultOptions();
  load();
}

void User::setDefaultControls() {
  // set in-book default controls
  controls.previousPage     = FZ_REPS_SQUARE;
  controls.nextPage         = FZ_REPS_TRIANGLE;
  controls.previous10Pages  = FZ_REPS_CIRCLE;
  controls.next10Pages      = FZ_REPS_CROSS;
  controls.screenUp         = FZ_REPS_UP;
  controls.screenDown       = FZ_REPS_DOWN;
  controls.screenLeft       = FZ_REPS_LEFT;
  controls.screenRight      = FZ_REPS_RIGHT;
  controls.zoomIn           = FZ_REPS_RTRIGGER;
  controls.zoomOut          = FZ_REPS_LTRIGGER;
  controls.showMainMenu     = FZ_REPS_START;
  controls.showToolbar      = FZ_REPS_SELECT;
  
  // set menu default controls
  controls.select           = FZ_REPS_CROSS;
  controls.cancel           = FZ_REPS_CIRCLE;
  controls.alternate        = FZ_REPS_TRIANGLE;
  controls.details          = FZ_REPS_SQUARE;	
  controls.menuUp           = FZ_REPS_UP;
  controls.menuDown         = FZ_REPS_DOWN;
  controls.menuLeft         = FZ_REPS_LEFT;
  controls.menuRight        = FZ_REPS_RIGHT;
  controls.menuLTrigger     = FZ_REPS_LTRIGGER;
  controls.menuRTrigger     = FZ_REPS_RTRIGGER;
  controls.resume           = FZ_REPS_START;
}

void User::setDefaultOptions() {
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
  options.lastFolder = Screen::basePath();
  options.lastFontFolder = Screen::basePath();
  options.loadLastFile = false;
  options.txtWrapCR = 0;
  options.hScroll = 50;
  options.vScroll = 20;
  options.thumbnail = 0;

  ColorScheme tnScheme;
  tnScheme.txtBGColor = 0x000000;
  tnScheme.txtFGColor = 0x0000ff;
  options.thumbnailColorSchemes.push_back(tnScheme);
  
  options.currentThumbnailScheme = 0;
  options.pdfImageQuality = 3;
  options.pdfImageBufferSizeM = 4;
  options.analogRateX = 100;
  options.analogRateY = 100;
  options.maxTreeHeight = 100;
  options.screenBrightness = 0; /* disable */
  options.autoPruneBookmarks = false;
  options.pdfOptimizeForSmallImages = false;
  options.defaultTitleMode = 0;
  options.evictGlyphCacheOnNewPage = false;
  options.pageScrollCacheMode = 0;
  options.ignoreXInOutlineOnSquare = false;
  options.jpeg2000Decoder = true;
}

void User::save() {
  #ifdef DEBUG
    printf("User::save\n");
  #endif

  char filename[1024];
  #ifdef __vita__
    snprintf(filename, 1024, "%s%s", Screen::basePath().c_str(), "data/Bookr/user.xml");
  #else
    snprintf(filename, 1024, "%s/%s", Screen::basePath().c_str(), "user.xml");
  #endif
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

  //fprintf(f, "\t\t<set option=\"pdfFastScroll\" value=\"%d\" />\n", options.pdfFastScroll ? 1 : 0);
  fprintf(f, "\t\t<set option=\"pageScrollCacheMode\" value=\"%d\" />\n", options.pageScrollCacheMode);
  fprintf(f, "\t\t<set option=\"txtRotation\" value=\"%d\" />\n", options.txtRotation);
  fprintf(f, "\t\t<set option=\"txtFont\" value=\"%s\" />\n", options.txtFont.c_str());
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
  fprintf(f, "\t\t<set option=\"pdfInvertColors\" value=\"%d\" />\n", options.pdfInvertColors ? 1 : 0);
  fprintf(f, "\t\t<set option=\"lastFolder\" value=\"%s\" />\n", options.lastFolder.c_str());
  fprintf(f, "\t\t<set option=\"lastFontFolder\" value=\"%s\" />\n", options.lastFontFolder.c_str());
  fprintf(f, "\t\t<set option=\"loadLastFile\" value=\"%d\" />\n", options.loadLastFile ? 1 : 0);
  fprintf(f, "\t\t<set option=\"txtWrapCR\" value=\"%d\" />\n", options.txtWrapCR);
  fprintf(f, "\t\t<set option=\"hScroll\" value=\"%d\" />\n", options.hScroll);
  fprintf(f, "\t\t<set option=\"vScroll\" value=\"%d\" />\n", options.vScroll);
  fprintf(f, "\t\t<set option=\"thumbnail\" value=\"%d\" />\n", options.thumbnail);

  for (unsigned int i = 0; i < options.thumbnailColorSchemes.size(); i++) {
    fprintf(f, "\t\t<set option=\"thumbnailColorScheme\" id=\"%d\" foreground=\"%d\" background=\"%d\" />\n", i, options.thumbnailColorSchemes[i].txtFGColor, options.thumbnailColorSchemes[i].txtBGColor);
  }
  fprintf(f, "\t\t<set option=\"currentThumbnailScheme\" value=\"%d\" />\n", options.currentThumbnailScheme);
  fprintf(f, "\t\t<set option=\"pdfImageQuality\" value=\"%d\" />\n", options.pdfImageQuality);
  fprintf(f, "\t\t<set option=\"pdfImageBufferSizeM\" value=\"%d\" />\n", options.pdfImageBufferSizeM);

  fprintf(f, "\t\t<set option=\"analogRateX\" value=\"%d\" />\n", options.analogRateX);
  fprintf(f, "\t\t<set option=\"analogRateY\" value=\"%d\" />\n", options.analogRateY);
  fprintf(f, "\t\t<set option=\"maxTreeHeight\" value=\"%d\" />\n", options.maxTreeHeight);
  fprintf(f, "\t\t<set option=\"screenBrightness\" value=\"%d\" />\n", options.screenBrightness);
  fprintf(f, "\t\t<set option=\"autoPruneBookmarks\" value=\"%d\" />\n", options.autoPruneBookmarks ? 1 : 0);
  fprintf(f, "\t\t<set option=\"pdfOptimizeForSmallImages\" value=\"%d\" />\n", options.pdfOptimizeForSmallImages ? 1 : 0);
  fprintf(f, "\t\t<set option=\"defaultTitleMode\" value=\"%d\" />\n", options.defaultTitleMode);
  fprintf(f, "\t\t<set option=\"evictGlyphCacheOnNewPage\" value=\"%d\" />\n", options.evictGlyphCacheOnNewPage ? 1 : 0);
  fprintf(f, "\t\t<set option=\"ignoreXInOutlineOnSquare\" value=\"%d\" />\n", options.ignoreXInOutlineOnSquare ? 1 : 0);
  fprintf(f, "\t\t<set option=\"jpeg2000Decoder\" value=\"%d\" />\n", options.jpeg2000Decoder ? 1 : 0);

  fprintf(f, "\t</options>\n");
  fprintf(f, "</user>\n");

  fclose(f);
}

void User::load() {
  #ifdef DEBUG
    printf("User::load\n");
  #endif
  
  char filename[1024];
  #ifdef __vita__
    snprintf(filename, 1024, "%s%s", Screen::basePath().c_str(), "data/Bookr/user.xml");
  #else
    snprintf(filename, 1024, "%s/%s", Screen::basePath().c_str(), "user.xml");
  #endif

  FILE *file = fopen(filename, "r");

  if (!file) {
    #ifndef DEBUG
      printf("%s doesn't exist; creating.\n", filename);
    #endif
    User::save();
  }
  
  XMLDocument doc;
  doc.LoadFile(filename);

  if(doc.Error()) {
    #ifndef DEBUG
      printf("invalid %s, cannot load preferences: %s\n", filename, doc.GetErrorStr1());
    #endif
    return;
  }

  XMLElement* root = doc.RootElement();
  XMLElement* controls = root->FirstChildElement("controls");
  if (controls != 0) {
    XMLElement* bind = controls->FirstChildElement("bind");

    while (bind) {
      const char* action = bind->Attribute("action");
      const char* button = bind->Attribute("button");
      if (action == 0 || button == 0) {
        //printf("invalid user.xml in line %d\n", bind->Row());
        break;
      }
      int b = atoi(button);
           if (strncmp(action, "controls.previousPage",    128) == 0) User::controls.previousPage    = b;
      else if (strncmp(action, "controls.nextPage",        128) == 0) User::controls.nextPage        = b;
      else if (strncmp(action, "controls.previous10Pages", 128) == 0) User::controls.previous10Pages = b;
      else if (strncmp(action, "controls.next10Pages",     128) == 0) User::controls.next10Pages     = b;
      else if (strncmp(action, "controls.screenUp",        128) == 0) User::controls.screenUp        = b;
      else if (strncmp(action, "controls.screenDown",      128) == 0) User::controls.screenDown      = b;
      else if (strncmp(action, "controls.screenLeft",      128) == 0) User::controls.screenLeft      = b;
      else if (strncmp(action, "controls.screenRight",     128) == 0) User::controls.screenRight     = b;
      else if (strncmp(action, "controls.zoomIn",          128) == 0) User::controls.zoomIn          = b;
      else if (strncmp(action, "controls.zoomOut",         128) == 0) User::controls.zoomOut         = b;

      bind = bind->NextSiblingElement("bind");
    }
  } else {
    printf("no controls found in user.xml\n");
  }

  XMLElement* eoptions = root->FirstChildElement("options");
  
  bool isFirstColorSchemeLoad = true;
  bool isFirstThumbnailColorSchemeLoad = true;

  if (eoptions != 0) {
    XMLElement* eset = eoptions->FirstChildElement("set");

    while (eset) {
      const char* option = eset->Attribute("option");
      const char* value = eset->Attribute("value");
      if (option == 0 || (value == 0 && (strncmp(option, "thumbnailColorScheme", 128) != 0) && (strncmp(option, "colorScheme", 128) != 0))) {
        //printf("invalid user.xml in line %d\n", eset->Row());
        break;
      }
      else if (strncmp(option, "menuControlStyle", 128) == 0) {
        // default is western so we only do the asian case
        if (strncmp(value, "western", 128) == 0) {
            User::controls.select = FZ_REPS_CROSS;
            User::controls.cancel = FZ_REPS_CIRCLE;
        } else if (strncmp(value, "asian", 128) == 0) {
            User::controls.select = FZ_REPS_CIRCLE;
            User::controls.cancel = FZ_REPS_CROSS;
        }
      }
      else if (strncmp(option, "txtRotation",     128) == 0) options.txtRotation     = atoi(value);
      else if (strncmp(option, "txtFont",         128) == 0) options.txtFont         = value;
      else if (strncmp(option, "txtHeightPct",    128) == 0) options.txtHeightPct    = atoi(value);
      else if (strncmp(option, "txtSize",         128) == 0) options.txtSize         = atoi(value);
      else if (strncmp(option, "txtJustify",      128) == 0) options.txtJustify      = atoi(value) != 0;
      else if (strncmp(option, "pdfFastScroll",   128) == 0) options.pdfFastScroll   = atoi(value) != 0;
      else if (strncmp(option, "pageScrollCacheMode",   128) == 0) options.pageScrollCacheMode   = atoi(value);
      else if (strncmp(option, "colorScheme",	    128) == 0) {
          
        // Get the scheme number and color values
        const char* id = eset->Attribute("id");
        const char* foreground = eset->Attribute("foreground");
        const char* background = eset->Attribute("background");
        
        if (id == 0 || foreground == 0 || background == 0) {
            //printf("invalid user.xml in line %d\n", eset->Row());
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
      else if (strncmp(option, "hScroll",         128) == 0) options.hScroll         = atoi(value);
      else if (strncmp(option, "vScroll",         128) == 0) options.vScroll         = atoi(value);
      else if (strncmp(option, "thumbnail",       128) == 0) options.thumbnail       = atoi(value);
      else if (strncmp(option, "thumbnailColorScheme", 128) == 0) {
          
        // Get the scheme number and color values
        const char* id = eset->Attribute("id");
        const char* foreground = eset->Attribute("foreground");
        const char* background = eset->Attribute("background");
        
        if (id == 0 || foreground == 0 || background == 0) {
            //printf("invalid user.xml in line %d\n", eset->Row());
            break;
        }
        
        // This is needed to ensure that a user who wishes to have a single
        // color profile can do so. Not clearing would leave the second default
        // scheme active.
        if (isFirstThumbnailColorSchemeLoad) {
            options.thumbnailColorSchemes.clear();
            isFirstThumbnailColorSchemeLoad = false;
        }
        
        int iId = atoi(id);
        if (iId >= options.thumbnailColorSchemes.size()) {
            options.thumbnailColorSchemes.resize(iId+1);
        }
        
        options.thumbnailColorSchemes[iId].txtBGColor = atoi(background);
        options.thumbnailColorSchemes[iId].txtFGColor = atoi(foreground);	 
      }
      else if (strncmp(option, "currentThumbnailScheme",   128) == 0) options.currentThumbnailScheme   = atoi(value);
      else if (strncmp(option, "pdfImageQuality",         128) == 0) options.pdfImageQuality         = atoi(value);
      else if (strncmp(option, "pdfImageBufferSizeM",         128) == 0) options.pdfImageBufferSizeM         = atoi(value);

      else if (strncmp(option, "analogRateX",         128) == 0) options.analogRateX         = atoi(value);
      else if (strncmp(option, "analogRateY",         128) == 0) options.analogRateY         = atoi(value);
      else if (strncmp(option, "maxTreeHeight",         128) == 0) options.maxTreeHeight         = atoi(value);
      else if (strncmp(option, "screenBrightness",         128) == 0) options.screenBrightness         = atoi(value);
      else if (strncmp(option, "autoPruneBookmarks",         128) == 0) options.autoPruneBookmarks         = atoi(value)!=0;
      else if (strncmp(option, "pdfOptimizeForSmallImages",         128) == 0) options.pdfOptimizeForSmallImages         = atoi(value)!=0;
      else if (strncmp(option, "defaultTitleMode",         128) == 0) options.defaultTitleMode         = atoi(value);
      else if (strncmp(option, "evictGlyphCacheOnNewPage",         128) == 0) options.evictGlyphCacheOnNewPage         = atoi(value)!=0;
      else if (strncmp(option, "ignoreXInOutlineOnSquare",         128) == 0) options.ignoreXInOutlineOnSquare         = atoi(value)!=0;
      else if (strncmp(option, "jpeg2000Decoder",         128) == 0) options.jpeg2000Decoder         = atoi(value)!=0;

      eset = eset->NextSiblingElement("set"); 
    }
  } else {
    printf("no options found in user.xml\n");
  }

  doc.Clear();

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


  vector<ColorScheme>::iterator thisThumbnailColor = options.thumbnailColorSchemes.begin();
  while (thisThumbnailColor != options.thumbnailColorSchemes.end()) {
    if ((thisThumbnailColor->txtFGColor & 0xff000000) != 0) {
      thisThumbnailColor->txtFGColor &= 0xffffff;
      operror = true;
    }
    
    if ((thisThumbnailColor->txtBGColor & 0xff000000) != 0) {
      thisThumbnailColor->txtBGColor &= 0xffffff;
      operror = true;
    }
    thisThumbnailColor++;
  }

  if (operror)
    User::save();

  if (options.pdfFastScroll || options.pageScrollCacheMode == 3){
    options.pageScrollCacheMode = 3;
    options.pdfFastScroll = true;
  }
  else if (options.pageScrollCacheMode>2||options.pageScrollCacheMode<0){
    options.pageScrollCacheMode = 0;
  }

  if (file != NULL)
    fclose(file);
}

}