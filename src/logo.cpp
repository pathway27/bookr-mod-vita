/*
 * bookr-modern: a graphics based document reader 
 * Copyright (C) 2019 pathway27 (Sree)
 * IS A MODIFICATION OF THE ORIGINAL
 * Bookr and bookr-mod for PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com),
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com),
 * AND VARIOUS OTHER FORKS, See Forks in README.md
 * Licensed under GPLv3+, see LICENSE
*/

#ifdef __vita__
  #include <psp2/kernel/threadmgr.h>
  #include <vita2d.h>
#else
  #include <glm/glm.hpp>
  #include <glm/gtc/matrix_transform.hpp>
  #include <glm/gtc/type_ptr.hpp>

  #include "resource_manager.hpp"
#endif

#include <cstring>
#include <iostream>

#include "graphics/fzscreen_defs.h"
#include "logo.hpp"

namespace bookr {

using std::cout;
using std::endl;

Logo::Logo() : loading(false), error(false), text("") {
}

Logo::~Logo() {
}

void Logo::setLoading(bool v) {
  loading = v;
}

void Logo::setError(bool v) {
  error = v;
}

void Logo::setError(bool v, string message) {
  error = v;
  text = message;
}

int Logo::update(unsigned int buttons) {
  int* b = Screen::ctrlReps();
  if (b[User::controls.showMainMenu] == 1) {
    return BK_CMD_INVOKE_MENU;
  }
  return 0;
}

void Logo::render() {
  #ifdef DEBUG_RENDER
    printf("bklogo render");
  #endif

  drawLogo(loading, text, error);
}

Logo* Logo::create() {
  Logo* f = new Logo();
  Screen::resetReps();
  return f;
}

}