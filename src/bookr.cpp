/*
 * Bookr % VITA: document reader for the Sony PS Vita
 * Copyright (C) 2017 Sreekara C. (pathway27 at gmail dot com)
 *
 * IS A MODIFICATION OF THE ORIGINAL
 *
 * Bookr and bookr-mod for PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com),
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com),

 * AND VARIOUS OTHER FORKS.
 * See Forks in the README for more info
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "bookr.hpp"
#include "graphics/screen.hpp"
//#include "user.hpp"
//#include "layer.h"
//#include "logo.h"
//#include "mainmenu.h"
//#include "popup.h"
//#include "filechooser.h"

namespace bookr {

void initalise(int argc, char *argv[])
{
  Screen::open(argc, argv);    // GPU init and initalDraw
  Screen::setupCtrl();         // initalise control sampling, TODO: put in ::open

  #ifdef DEBUG
    printf("Debug Started: in bookr::initalise\n");
  #endif

  // BKUser::init(); // get app settings from user.xml

  // BKLayer::load();                       // make textures
  // mm = BKMainMenu::create(); // Main Menu, only opens when pressed start on opening screen
  // layers.push_back(BKLogo::create());    // Logo thats displayed with text at the back, first layer, then everything else draw on top
  // layers.push_back(mm);                  // Main Menu
}

#if 0
  void mainloop() {
    // Event Loop
    while (!exitApp) {
      // draw state to back buffer and swap
      if (dirty)
      {
        FZScreen::startDirectList();
        bkLayersIt it(layers.begin());
        bkLayersIt end(layers.end());
        while (it != end)
        {
          (*it)->render();
          ++it;
        }
        FZScreen::endAndDisplayList();
        FZScreen::swapBuffers();
      }

      int buttons = FZScreen::readCtrl();

      dirty = buttons != 0;

      #if defined(MAC) || defined(WIN32)
        if (buttons == FZ_CTRL_LTRIGGER || FZScreen::isClosing())
          break;
      #endif

      // // the last layer always owns the input focus
      bkLayersIt it(layers.end());
      --it;
      int command = 0;

      if ((*it) == nullptr)
        continue;

      // These take up most of the stdout
      #ifdef DEBUG_BUTTONS
        printf("pre update-buttons\n");
      #endif

      command = (*it)->update(buttons);
      if (command == BK_CMD_OPEN_FILE) {
        #ifdef DEBUG
          printf("Got BK_CMD_OPEN_FILE\n");
        #endif
      }

      #ifdef DEBUG_BUTTONS
        printf("post update-buttons\n");
      #endif
      // dont proc events while in the reload timer
      std::cout << command << std::endl;

      // pusedo message passing
      command_handler(command);

      #if defined(MAC) || defined(WIN32)
        if (buttons == FZ_CTRL_LTRIGGER || FZScreen::isClosing()) break;
      #endif
      #ifdef DEBUG
        // printf("powerResumed %i\n", FZScreen::getSuspendSerial());
        // Quick close
        if ((buttons == (FZ_CTRL_LTRIGGER | FZ_CTRL_CIRCLE)) ||
            FZScreen::isClosing())
          break;
        else
          FZScreen::checkEvents(buttons);
      #endif
    }
  }

  void cleanup() {
    #ifdef DEBUG
      printf("exiting...\n");
    #endif

    // Get rid of all gui layers
    bkLayersIt it(layers.begin());
    bkLayersIt end(layers.end());
    while (it != end) {
      (*it)->release();
      ++it;
    }
    layers.clear();

    FZScreen::close(); // deinit graphics layer
    BKLayer::unload(); // free textures
    FZScreen::exit();
  }

  void command_handler(int command) {
    switch (command)
    {
      case BK_CMD_MARK_DIRTY:
      {
        dirty = true;
        // mm->rebuildMenu();
      }
      break;
      case BK_CMD_CLOSE_TOP_LAYER:
      {
        bkLayersIt it(layers.end());
        --it;
        (*it)->release();
        layers.erase(it);

        if (command == BK_CMD_CLOSE_TOP_LAYER_RELOAD)
        {
          // repaint
          dirty = true;
        }
      }
      break;
      case BK_CMD_INVOKE_MENU:
      {
        mm = BKMainMenu::create();
        layers.push_back(mm);
      }
      break;
      case BK_CMD_MAINMENU_POPUP:
      {
        layers.push_back(BKPopup::create(
            mm->getPopupMode(),
            mm->getPopupText()));
      }
      break;
      case BK_CMD_EXIT:
      {
        exitApp = true;
        break;
      }
      case BK_CMD_INVOKE_OPEN_FILE:
      {
        // add a file chooser layer
        // string title("Open (use SQUARE to open Vietnamese chm/html)");
        string title("Open File");
        fs = BKFileChooser::create(title, BK_CMD_OPEN_FILE);
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
        bkLayersIt it(layers.begin());
        bkLayersIt end(layers.end());
        while (it != end)
        {
          (*it)->release();
          ++it;
        }
        layers.clear();
        #ifdef DEBUG
          printf("getFullPath post layer clear %s\n", fileName.c_str());
        #endif
        // little hack to display a loading screen
        BKLogo *l = BKLogo::create();
        l->setLoading(true);
        FZScreen::startDirectList();
        l->render();
        FZScreen::endAndDisplayList();
        FZScreen::waitVblankStart();
        FZScreen::swapBuffers();
        // FZScreen::checkEvents();
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
          documentLayer = BKDocument::create(fnCpy.c_str());
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
          BKLogo *l = BKLogo::create();
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
        // render filetype content i.e. bouncebuffer with FZScreen::copyImage
        // wait for event, redraw bouncebuffer responding to events

        // FZScreen::setSpeed(BKUser::options.pspSpeed);

        dirty = true;
        break;
      }
    }
  }
#endif

}