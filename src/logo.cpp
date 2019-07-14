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

#include "graphics/fzscreen_defs.h"
#include "logo.hpp"

#ifdef __vita__
  #include <psp2/kernel/threadmgr.h> 
  #include <vita2d.h>
#else
  #include "resource_manager.hpp"
  #include <glm/glm.hpp>
  #include <glm/gtc/matrix_transform.hpp>
  #include <glm/gtc/type_ptr.hpp>
#endif

#include <cstring>
#include <iostream>

using std::cout;
using std::endl;

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

void Logo::render() {
  #ifdef DEBUG_RENDER
    printf("bklogo render");
  #endif

  // Screen::clear(0xffffff, FZ_COLOR_BUFFER);
  // Screen::color(0xffffffff);


  #ifdef __vita__
    vita2d_draw_texture(texLogo->vita_texture, 350, 150);
    vita2d_font_draw_text(fontBig->v_font, 260, 440, RGBA8(0,0,0,255), TITLE_FONT_SIZE, "TXT - PDF - CBZ - HTML - EPUB - FB2");

    vita2d_draw_rectangle(96, 494, 768, 40, RGBA8(105,105,105,255)); // my cheapo drawPill

    if (loading)
      vita2d_font_draw_textf(fontBig->v_font, 350, 524, RGBA8(255,255,255,255), TITLE_FONT_SIZE,
        "%*s", TEXT_PADDED_WIDTH / 2 + strlen(LOADING_TEXT) / 2 , LOADING_TEXT);
    else if (text.length() > 0 && !(error))
      vita2d_font_draw_textf(fontBig->v_font, 350, 524, RGBA8(255,255,255,255), TITLE_FONT_SIZE,
        "%*s", TEXT_PADDED_WIDTH / 2 + strlen(text.c_str()) / 2 , text.c_str());
    else {
      if (error) {
        vita2d_font_draw_textf(fontBig->v_font, 350, 524, RGBA8(200,0,0,255), TITLE_FONT_SIZE,
          "Error: %*s", TEXT_PADDED_WIDTH / 2 + strlen(text.c_str()) / 2 , text.c_str());
      }
      else
        vita2d_font_draw_textf(fontBig->v_font, 350, 524, RGBA8(255,255,255,255), TITLE_FONT_SIZE,
          "%*s", TEXT_PADDED_WIDTH / 2 + strlen(DEFAULT_TEXT) / 2 , DEFAULT_TEXT);
    }
  #else
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ResourceManager::getSpriteRenderer()->DrawSprite(
      ResourceManager::GetTexture("logo"),
      glm::vec2(380, 150),
      glm::vec2(128, 128));
    ResourceManager::getSpriteRenderer()->DrawQuad(
      glm::vec2(350, 150),
      glm::vec2(128, 128),
      0.0f, glm::vec4(47/255.0, 47/255.0, 47/255.0, 240/255.0));

    ResourceManager::getSpriteRenderer()->DrawQuad(
      glm::vec2(450, 150),
      glm::vec2(128, 128),
      0.0f, glm::vec4(170/255.0, 170/255.0, 170/255.0, 255/255.0));

    ResourceManager::getTextRenderer()->RenderText("Testing:", 5.0f, 5.0f, 1.0f, glm::vec3(0.0f,0.0f,0.0f));
  #endif
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
  #if defined(PSP) || defined(__vita__)
  if (delaySeconds > 0) {
    sceKernelDelayThread(delaySeconds * 1000000);
  }
  #endif
}

}