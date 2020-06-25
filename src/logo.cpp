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

#include "graphics/screen.hpp"
#include "graphics/fzscreen_defs.h"
#include "logo.hpp"

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

static const unsigned int TITLE_FONT_SIZE = 28;
static const unsigned int TEXT_PADDED_WIDTH = 20;
static const char *LOADING_TEXT = "Loading...";
static const char *DEFAULT_TEXT = "Press Start";

static const unsigned int LOGO_SIZE = 128;
static const unsigned int CENTER_X = (Screen::WIDTH / 2) - (LOGO_SIZE / 2);
static const unsigned int CENTER_Y = (Screen::HEIGHT / 2) - (LOGO_SIZE / 2);

void Logo::render() {
  #ifdef DEBUG_RENDER
    printf("bklogo render");
  #endif

  #ifdef __vita__
    vita2d_draw_texture(texLogo->vita_texture, CENTER_X, CENTER_Y);
  #else  
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ResourceManager::getSpriteRenderer()->DrawSprite(ResourceManager::GetTexture("logo"),
      glm::vec2(CENTER_X, CENTER_Y),
      glm::vec2(128, 128));
  #endif
  drawText(CENTER_X - 100, CENTER_Y + LOGO_SIZE + 10, RGBA8(0,0,0,255), TITLE_FONT_SIZE, "TXT - PDF - CBZ - HTML - EPUB - FB2");

  drawRectangle(Screen::WIDTH * 0.1, Screen::HEIGHT * 0.9, Screen::WIDTH * 0.8, Screen::HEIGHT * 0.1, RGBA8(105,105,105,155));

  if (loading)
    drawText(Screen::WIDTH * 0.3, Screen::HEIGHT * 0.95, RGBA8(255,255,255,255), TITLE_FONT_SIZE, LOADING_TEXT);
  else if (text.length() > 0 && !(error))
    drawText(Screen::WIDTH * 0.3, Screen::HEIGHT * 0.95, RGBA8(255,255,255,255), TITLE_FONT_SIZE, text.c_str());
  else if (error)
    drawText(Screen::WIDTH * 0.3, Screen::HEIGHT * 0.95, RGBA8(255,255,255,255), TITLE_FONT_SIZE, text.c_str());
  else
    drawText(Screen::WIDTH * 0.3, Screen::HEIGHT * 0.95, RGBA8(255,255,255,255), TITLE_FONT_SIZE, DEFAULT_TEXT);
}

Logo* Logo::create() {
  Logo* f = new Logo();
  Screen::resetReps();
  return f;
}

void Logo::show(string text) {
  show(text, 0);
}

void Logo::show(string text, int delaySeconds) {
  Logo* l = Logo::create();
  l->text = text;
  Screen::startDirectList();
  l->render();
  Screen::endAndDisplayList();
  Screen::waitVblankStart();
  Screen::swapBuffers();
  //Screen::checkEvents();
  l->release();
}

}