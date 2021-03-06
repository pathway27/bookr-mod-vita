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

  #include "../resource_manager.hpp"
#endif

#include <cstring>
#include <iostream>

#include "../graphics/fzscreen_defs.h"
#include "logo.hpp"
#include "colours.hpp"

namespace bookr {

#include "../utils/debug_vita.hpp"

static unsigned int VBO, VAO, EBO, texture;
static int width, height, nrChannels;

Logo::Logo() : loading(false), error(false), text("")
{
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

inline constexpr const char *LOADING_TEXT = "Loading...";
inline constexpr const char *DEFAULT_TEXT = "Press Start";

inline constexpr unsigned int LOGO_SIZE = 256;

static const unsigned int CENTER_X = (Screen::WIDTH / 2) - (LOGO_SIZE / 2);
static const unsigned int CENTER_Y = (Screen::HEIGHT / 2) - (LOGO_SIZE / 2);

void Logo::render() {
  #ifdef DEBUG_RENDER
    printf("bklogo render\n");
  #endif

  #ifdef __vita__
    Screen::drawTexture(texLogo, CENTER_X, CENTER_Y);
  #else
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ResourceManager::getSpriteRenderer()->DrawSprite(ResourceManager::GetTexture("logo"),
      glm::vec2(CENTER_X, CENTER_Y),
      glm::vec2(LOGO_SIZE, LOGO_SIZE));
  #endif

  drawText(CENTER_X - (Screen::WIDTH * 0.1), CENTER_Y + LOGO_SIZE + 10, BLACK, 1.0f, "TXT - PDF - CBZ - HTML - ePub - FB2");

  drawRectangle(Screen::WIDTH * 0.1, Screen::HEIGHT * 0.9, Screen::WIDTH * 0.8, Screen::HEIGHT * 0.08, GONDOLA);

  unsigned int bottom_text_color = WHITE;
  const char * bottom_text = DEFAULT_TEXT;

  if (loading)
    bottom_text = LOADING_TEXT;
  else if (text.length() > 0) {
    bottom_text = text.c_str();
    if (error)
      bottom_text_color = ERROR_RED;
  }

  #ifdef __vita__
    #define TEXT_HEIGHT_OFFSET 0.95
  #else
    #define TEXT_HEIGHT_OFFSET 0.925
  #endif

  drawText(CENTER_X + (Screen::WIDTH * 0.03), Screen::HEIGHT * TEXT_HEIGHT_OFFSET, bottom_text_color, 1.0f, bottom_text);

  #if DEBUG
    #include "bookrconfig.h"
    #include <mupdf/fitz/version.h>
    string version_text = string("MuPDF v") + string(FZ_VERSION);
    drawText(0, Screen::HEIGHT * 0.93, LIGHT_RED, 1.0f, version_text.c_str());
    version_text = string("Bookr DEBUG ") + string(GIT_VERSION);
    drawText(0, Screen::HEIGHT * 0.97, LIGHT_RED, 1.0f, version_text.c_str());
  #endif
}

Logo* Logo::create() {
  Logo* f = new Logo();
  Screen::resetReps();
  return f;
}

}
