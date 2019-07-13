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
#include <iostream>

#include "graphics/fzscreen_defs.h"
#include "logo.hpp"
#include "SOIL.h"
#include "resource_manager.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using std::cout;
using std::endl;

namespace bookr {

static unsigned int VBO, VAO, EBO, texture;
static int width, height, nrChannels;

Logo::Logo() : loading(false), error(false), text("")
{
  // Configure VAO/VBO
  GLfloat vertices[] = {
    // Pos      // Tex
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f
  };

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(VAO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    unsigned char* image = SOIL_load_image("sce_sys/icon0.png", &width, &height, 0, SOIL_LOAD_RGB);
    
    cout << SOIL_last_result() << endl; 
    cout << "null: " << !image << endl;
    cout << "Max size: " << GL_MAX_TEXTURE_SIZE << endl;
    cout << "Width: " <<  width << endl;
    cout << "Height: " << height << endl;
    cout << "Obj: " << texture << endl;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, 
    GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);
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
    ResourceManager::GetShader("texture").Use();

    //glm::mat4 model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(glm::vec2(200, 200), 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)

    //glm::vec2 size = glm::vec2(10, 10);
    //model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
    //GLfloat rotate = 0.0f;
    //model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
    //model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back

    //model = glm::scale(model, glm::vec3(0.0f, 1.0f, 0.0f)); // Last scale
    //
    //
    //ResourceManager::GetShader("texture_ortho").SetMatrix4("model", model);
    //ResourceManager::GetShader("texture_ortho").SetVector3f("spriteColor", glm::vec3(1.0f));
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
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