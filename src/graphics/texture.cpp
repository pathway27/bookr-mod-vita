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
  #include <pspkernel.h>
  #include <pspdisplay.h>
  #include <pspdebug.h>
  #include <pspgu.h>
  #define printf  pspDebugScreenPrintf
#elif defined(__vita__)
  #include <psp2/display.h>
  #include <vita2d.h>
#elif defined(SWITCH)
#else
  #include <stdio.h>
  #define GLEW_STATIC
  #include <GL/glew.h>
  #include <GLFW/glfw3.h>
  #include <SOIL.h>
#endif

#include <cstddef>
#include <iostream>

#include "fzscreen_defs.h"
#include "texture.hpp"
#include "screen.hpp"

namespace bookr {

Texture::Texture() : texImage(0) {
    glGenTextures(1, &textureObject);
}

Texture::~Texture() {
    glDeleteTextures(1, &textureObject);
}

void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, textureObject);
    Screen::setBoundTexture((Texture*)this);
}

void Texture::bindForDisplay() {
    bind();
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texenv);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texMag);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texMin);
}

static bool validatePow2(unsigned int x, unsigned int maxPow) {
    bool valid = false;
    unsigned int i, s;
    for (i = 0; i <= maxPow; ++i) {
        s = 1 << i;
        if (x == s) {
            valid = true;
            break;
        }
    }
    return valid;
}


bool Texture::validateFormat(Image* image) {
    Image::Format imageFormat = image->getFormat();
    internalFormat = GL_ALPHA8;
    textureFormat = GL_ALPHA;
    textureType = GL_UNSIGNED_BYTE;
    switch (imageFormat) {
        case Image::mono8:
        break;
        case Image::rgb24:
            internalFormat = GL_RGB8;
            textureFormat = GL_RGB;
        break;
        case Image::rgb32:
            internalFormat = GL_RGB8;
            textureFormat = GL_RGB;
        break;
        case Image::rgba32:
            internalFormat = GL_RGBA8;
            textureFormat = GL_RGBA;
        break;
        default:
            return false;
        break;
    }
    return true;
}


Texture* Texture::createFromImage(Image* image, bool buildMipmaps) {
    Texture* texture = new Texture();
    if (!initFromImage(texture, image, buildMipmaps)) {
        texture->release();
        texture = 0;
    }
    return texture;
}

#ifdef __vita__
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
#elif defined(SWITCH)
  Texture* Texture::createFromBuffer(const void * buffer) {
    Texture* texture = new Texture();
    return texture;
  }
#endif

#if defined(MAC) || defined(WIN32)
  Texture* Texture::createFromSOIL(char* filename) {
      std::cout <<"create from SOIL" << std::endl;
      int width, height;
      char* soil_image = (char *) SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);

      Image* image = Image::createWithData(width, height, soil_image);

      Texture* texture = new Texture();
      texture->texImage = image;

    
      glBindTexture(GL_TEXTURE_2D, texture->textureObject);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, 
        GL_RGB, GL_UNSIGNED_BYTE, soil_image);
        glGenerateMipmap(GL_TEXTURE_2D);

        SOIL_free_image_data((unsigned char*) soil_image);
      glBindTexture(GL_TEXTURE_2D, 0);
      std::cout << texture->textureObject << std::endl;
      return texture;
  }
#endif
    
bool Texture::initFromImage(Texture* texture, Image* image, bool buildMipmaps) {

}

void Texture::texEnv(int op) {
  #if defined(PSP) || defined(__vita__) || defined(SWITCH)
    texenv = op;
  #else
    texenv = GL_REPLACE;
    switch (op) {
        case FZ_TEX_MODULATE: texenv = GL_MODULATE; break;
        case FZ_TEX_DECAL: texenv = GL_DECAL; break;
        case FZ_TEX_BLEND: texenv = GL_BLEND; break;
        case FZ_TEX_ADD: texenv = GL_ADD; break;
    }
  #endif  
}

void Texture::filter(int min, int mag) {
  #if defined(PSP) || defined(__vita__) || defined(SWITCH)
    texMin = min;
    texMag = mag;
  #else
    texMin = GL_NEAREST;
    switch (min) {
        case FZ_LINEAR: texMin = GL_LINEAR; break;
        case FZ_NEAREST_MIPMAP_NEAREST: texMin = GL_NEAREST_MIPMAP_NEAREST; break;
        case FZ_LINEAR_MIPMAP_NEAREST: texMin = GL_LINEAR_MIPMAP_NEAREST; break;
        case FZ_NEAREST_MIPMAP_LINEAR: texMin = GL_NEAREST_MIPMAP_LINEAR; break;
        case FZ_LINEAR_MIPMAP_LINEAR: texMin = GL_LINEAR_MIPMAP_LINEAR; break;
    }
    texMag = mag == FZ_LINEAR ? GL_LINEAR : GL_NEAREST;
  #endif
}

}