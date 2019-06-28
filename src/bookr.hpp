/*
 * Bookr Modern: A document reader for the Sony PS Vita, Nintendo Switch and PC
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

#ifndef BOOKR_HPP
#define BOOKR_HPP

#include "bkdocument.h"
#include "bkmainmenu.h"
#include "bkfilechooser.h"

namespace bookr {
  extern BKDocument *documentLayer;
  // bkLayers layers;                   // iterator over all gui obj. that are initalsed
  // BKMainMenu *mm;                    // Main Menu, only opens when pressed start on opening screen
  // BKFileChooser *fs;                 // file chooser, only opens when Open File in mainmenu

  // Swapping buffers based on dirty variable feels dirty.
  // bool dirty = true;
  // bool exitApp = false;
  // int reloadTimer = 0;

  void initalise(int argc, char *argv[]);
  void mainloop();
  void command_handler(int command);
  void cleanup();
}

#endif