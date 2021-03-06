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

#include "bookr.hpp"

#include <iostream>

#include "graphics/screen.hpp"
#include "graphics/controls.hpp"

//#include "user.hpp"
//#include "layer.hpp"
#include "ui/logo.hpp"
#include "ui/mainmenu.hpp"
#include "ui/popup.hpp"
#include "ui/filechooser.hpp"

#include "document.hpp"

namespace bookr {

static void command_handler(int command);

static Layers layers;                 // iterator over all gui obj. that are initalsed
static MainMenu *mm;                    // Main Menu, only opens when pressed start on opening screen
static FileChooser *fs;                 // file chooser, only opens when Open File in mainmenu
static Document *documentLayer;

// Swapping buffers based on dirty variable feels dirty.
static bool dirty = true;
static bool exitApp = false;

#include "utils/debug_vita.hpp"

void initalise(int argc, char *argv[]) {
  // #ifdef DEBUG
  //   printf("Debug Started: in bookr::initalise\n");
  // #endif
  Screen::open(argc, argv);    // GPU init and initalDraw
  Screen::setupCtrl();         // initalise control sampling, TODO: put in ::open
  User::init(); // get app settings from user.xml

  Layer::load();                       // make textures
  mm = MainMenu::create(); // Main Menu, only opens when pressed start on opening screen
  layers.push_back(Logo::create());    // Logo thats displayed with text at the back, first layer, then everything else draw on top
  layers.push_back(mm);                  // Main Menu
}

void mainloop() {
  #ifdef DEBUG
    printf("bookr::mainloop\n");
  #endif
  // Event Loop
  while (!exitApp) {
    // draw state to back buffer and swap
    if (dirty) {
      Screen::startDirectList();
      for (const auto &layer : layers)
        layer->render();
      Screen::endAndDisplayList();
      Screen::swapBuffers();
    }

    int buttons = Screen::readCtrl();
    #ifdef DEBUG_BUTTONS
      std::cout << "mainloop buttons " << buttons << std::endl;
    #endif
    dirty = buttons != 0;


    // // the last layer always owns the input focus
    auto &last_layer = layers.back();
    int command = 0;

    if (last_layer == nullptr)
      continue;

    // These take up most of the stdout
    #ifdef DEBUG_BUTTONS
      printf("pre update-buttons\n");
    #endif

    command = last_layer->update(buttons);
    if (command == BK_CMD_OPEN_FILE) {
      #ifdef DEBUG
        printf("Got BK_CMD_OPEN_FILE\n");
      #endif
    }

    #ifdef DEBUG_BUTTONS
      printf("post update-buttons\n");
    #endif
    // dont proc events while in the reload timer

    #ifdef DEBUG_BUTTONS
      std::cout << "mainloop command: " << command << std::endl;
    #endif
    // // pusedo message passing
    command_handler(command);

    if (Screen::isClosing())
      break;

    #ifdef DEBUG
      // printf("powerResumed %i\n", Screen::getSuspendSerial());
      // Quick close
      if ((buttons == (FZ_CTRL_LTRIGGER | FZ_CTRL_CIRCLE)) ||
          Screen::isClosing())
        break;
      // else
      // Screen::checkEvents(buttons);
    #endif
  }
}

void cleanup() {
  #ifdef DEBUG
    printf("exiting...\n");
  #endif

  for(auto &layer: layers) {
    layer->release();
  }
  layers.clear();

  Screen::close(); // deinit graphics layer
  Layer::unload(); // free textures
  Screen::exit();
}

static void command_handler(int command) {
  switch (command) {
    case BK_CMD_MARK_DIRTY:
    {
      dirty = true;
      // mm->rebuildMenu();
      break;
    }
    case BK_CMD_CLOSE_TOP_LAYER:
    {
      layers.pop_back();

      if (command == BK_CMD_CLOSE_TOP_LAYER_RELOAD)
      {
        // repaint
        dirty = true;
      }
      break;
    }
    case BK_CMD_INVOKE_MENU:
    {
      mm = MainMenu::create();
      layers.push_back(mm);
      break;
    }
    case BK_CMD_MAINMENU_POPUP:
    {
      layers.push_back(
        Popup::create(mm->getPopupMode(), mm->getPopupText())
      );
      break;
    }
    case BK_CMD_EXIT:
    {
      exitApp = true;
      break;
    }
    case BK_CMD_INVOKE_OPEN_FILE:
    {
      // add a file chooser layer
      string title("Open File");
      fs = FileChooser::create(title, BK_CMD_OPEN_FILE);
      layers.push_back(fs);
      break;
    }
    case BK_CMD_RELOAD:
    case BK_CMD_OPEN_FILE:
    {
      // open a file as a document
      #ifdef DEBUG
        printf("BK_CMD_OPEN_FILE\n");
      #endif
      string fileName; // this is being changed somewhere, don't trust it..
      string fnCpy;

      bool convertToVN = false;

      if (command == BK_CMD_RELOAD)
      {
        documentLayer->getFileName(fileName);
        documentLayer = 0;
      }
      if (command == BK_CMD_OPEN_FILE)
      {
        // open selected file
        fs->getFullPath(fileName);
        //convertToVN = fs->isConvertToVN();
        fs = 0;
        #ifdef DEBUG
          printf("getFullPath %s\n", fileName.c_str());
        #endif
      }
      fnCpy = string(fileName);

      // clear layers
      #ifdef DEBUG
        printf("getFullPath pre layer clear %s\n", fileName.c_str());
      #endif
      for (const auto &layer : layers)
        layer->release();
      layers.clear();
      #ifdef DEBUG
        printf("getFullPath post layer clear %s\n", fileName.c_str());
      #endif
      // little hack to display a loading screen
      Logo *l = Logo::create();
      l->setLoading(true);
      Screen::startDirectList();
      l->render();
      Screen::endAndDisplayList();
      Screen::waitVblankStart();
      Screen::swapBuffers();
      // Screen::checkEvents();
      l->release();
      #ifdef DEBUG
        printf("getFullPath copy %s\n", fnCpy.c_str());
      #endif
      #ifdef DEBUG
        printf("BKLogo Created\n");
        printf("Pre Document::create\n");
      #endif
      #ifdef DEBUG
        printf("getFullPath %s\n", fileName.c_str());
      #endif

      const char *error;
      // detect file type and add a new display layer
      try
      {
        #ifdef DEBUG
          printf("getFullPath %s\n", fileName.c_str());
        #endif
        documentLayer = Document::create(fnCpy.c_str());
      }
      catch (const char *e)
      {
        error = e;
        documentLayer = nullptr;
      }
      // specific create BKPDF::create
      //   init mupdf vars
      //   b->redrawBuffer(); sets bouncebuffer
      #ifdef DEBUG
        printf("Post Document::create\n");
      #endif
      if (documentLayer == nullptr)
      {
        // error, back to logo screen
        Logo *l = Logo::create();
        l->setError(true, error);
        // Still no way of getting exact error during file opening
        // or during class init... maybe throw string and try and catch
        layers.push_back(l);
      }
      else
      {
        // file loads ok, add the layer
        layers.push_back(documentLayer);
      }

      // render document
      // render filetype content i.e. bouncebuffer with Screen::copyImage
      // wait for event, redraw bouncebuffer responding to events

      // Screen::setSpeed(BKUser::options.pspSpeed);

      dirty = true;
      break;
    }
  }
}

}
