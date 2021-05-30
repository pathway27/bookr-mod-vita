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

#ifndef BOOKR_HPP
#define BOOKR_HPP

namespace bookr {
  void initalise(int argc, char *argv[]);
  void mainloop();
  void cleanup();
}

#endif