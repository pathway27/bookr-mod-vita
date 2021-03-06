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

#ifdef PSP
  #include <pspthreadman.h>
#elif defined(__vita__)
  #include <psp2/kernel/threadmgr.h> 
  #include <vita2d.h>
#endif
#include <cstring>

#include "graphics/fzscreen_defs.h"
#include "logo.hpp"

namespace bookr {

static unsigned int VBO, VAO, EBO;

Logo::Logo() : loading(false), error(false), text(""),
  recShader("shaders/rectangle.vert", "shaders/rectangle.frag")
{
  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
       0.5f,  0.5f, 0.0f,  // top right
       0.5f, -0.5f, 0.0f,  // bottom right
      -0.5f, -0.5f, 0.0f,  // bottom left
      -0.5f,  0.5f, 0.0f   // top left 
  };
  unsigned int indices[] = {  // note that we start from 0!
      0, 1, 3,  // first Triangle
      1, 2, 3   // second Triangle
  };
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0); 

  // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
  // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
  glBindVertexArray(0); 
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
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw our first triangle
    recShader.Use();
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // glBindVertexArray(0); // no need to unbind it every time
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