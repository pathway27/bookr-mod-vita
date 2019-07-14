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


#if defined(__vita__)
  #include <psp2/display.h>
  #include <vita2d.h>
#elif defined(SWITCH)
  #include <glad/glad.h>
#else
  #include <stdio.h>
  #define GLEW_STATIC
  #include <GL/glew.h>
  #include <GLFW/glfw3.h>
#endif

#include <cstddef>
#include <iostream>

#include "fzscreen_defs.h"
#include "texture.hpp"
#include "screen.hpp"

namespace bookr {

Texture::Texture() {

}

Texture::~Texture() {

}


Texture* Texture::createFromVitaTexture(vita2d_texture* v_texture) {
    #ifdef DEBUG
      printf("create from vita\n");
    #endif
    Texture* texture = new Texture();
    texture->vita_texture = v_texture;
    //psp2shell_print("%p\n", (void *) &(texture->vita_texture));
    return texture;
}

Texture* Texture::createFromBuffer(const void * buffer) {
  Texture* texture = new Texture();
  texture->vita_texture = vita2d_load_PNG_buffer(buffer);
  return texture;
}

Texture* Texture::createFromSOIL(char* filename) {
    std::cout <<"create from SOIL" << std::endl;
    Texture* texture = new Texture();
    return texture;
}

}
