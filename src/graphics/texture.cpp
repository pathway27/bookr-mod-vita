/*
 * Bookr % VITA: document reader for the Sony PS Vita
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

#if defined(PSP) || defined(__vita__) || defined(SWITCH)
  Texture::Texture() : texImage(0) {
  }

  Texture::~Texture() {
      #ifdef DEBUG
        printf("~Texture\n");
      #endif

    #ifdef __vita__
    if (vita_texture != NULL) {
        vita2d_free_texture(vita_texture);
        // vita_texture = NULL;
    }
    #endif
  }

  void Texture::bind() {
      Screen::setBoundTexture((Texture*)this);
  }

  void Texture::bindForDisplay() {
      // support only for main ram textures - very slow
      // TODO: investigate vram textures: when and how to
      // transfer?

    #ifdef PSP
      if (texImage->getCLUT() != NULL) {
          int n = texImage->getCLUTSize();
          sceGuClutMode(GU_PSM_8888, 0, n - 1, 0); // 32-bit palette
          sceGuClutLoad((n/8), texImage->getCLUT()); // upload entries (8 colors base unit)
      }
      sceGuTexFunc(texenv, pixelComponent);
      //sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
      sceGuTexMode(pixelFormat, 0, 0, GU_TRUE);
      //sceGuTexMode(pixelFormat, 0, 0, GU_FALSE);
      sceGuTexImage(0, width, height, width, texImage->getData());
      sceGuTexScale(1.0f, 1.0f);
      sceGuTexOffset(0.0f, 0.0f);
      sceGuTexFilter(texMin, texMag);
    #elif __vita__
      //vita2d_load_PNG_buffer(texImage->getData());
      //sceGxmSetFragmentTexture(_vita2d_context, 0, &vita_texture->gxm_tex);
    #endif
      Screen::setBoundTexture((Texture*)this);
  }

#else
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
#endif

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

#ifdef PSP
    bool Texture::validateFormat(Image* image) {
        Image::Format imageFormat = image->getFormat();
        pixelFormat = GU_PSM_T8;
        pixelComponent = GU_TCC_RGBA;
        switch (imageFormat) {
            case Image::mono8:
            break;
      #if 0
            case Image::mono16:
            break;
            case Image::dual16:
            break;
      #endif
            case Image::rgb24:
                pixelFormat = GU_PSM_T8;		// not
                pixelComponent = GU_TCC_RGB;
            break;
            case Image::rgb32:
                pixelFormat = GU_PSM_8888;
                pixelComponent = GU_TCC_RGB;
            break;
            case Image::rgba32:
                pixelFormat = GU_PSM_8888;
            break;
            default:
                return false;
        }
        return true;
    }
#elif __vita__ || defined(SWITCH)
    bool Texture::validateFormat(Image* image) {
        Image::Format imageFormat = image->getFormat();
        return true;
    }
#else
    bool Texture::validateFormat(Image* image) {
        Image::Format imageFormat = image->getFormat();
        internalFormat = GL_ALPHA8;
        textureFormat = GL_ALPHA;
        textureType = GL_UNSIGNED_BYTE;
        switch (imageFormat) {
            case Image::mono8:
            break;
      #if 0
            case Image::mono16:
                internalFormat = GL_ALPHA16;
                textureType = GL_UNSIGNED_SHORT;
            break;
            case Image::dual16:
                internalFormat = GL_LUMINANCE8_ALPHA8;
                textureFormat = GL_LUMINANCE_ALPHA;
            break;
      #endif
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
#endif

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

    unsigned int width = 0, height = 0;

    image->getDimensions(width, height);

    // \todo query max texture size
    // 2 pow 10 == 1024
    if (!validatePow2(width, 10) || !validatePow2(height, 10)) {
        //printf("invalid dimensions %d x %d\n", width, height);
        return false;
    }

    texture->width = width;
    texture->height = height;

    if (!texture->validateFormat(image)) {
        //printf("invalid format\n");
        return false;
    }

    void* data = image->getData();
    Image* buffer = 0;
    if (image->getFormat() == Image::rgb32) { 
        buffer = Image::createRGB24FromRGB32(image);
        data = buffer->getData();
    }


  #if defined(PSP) || defined(__vita__)
    image->retain();
    // BUG: swizzle is not working above 8bpp
    image->swizzle(16, 8);	// swizzle is always 16x8 bytes
    texture->texImage = image;
  #elif defined(OLD)
    texture->bind();
    glEnable(GL_TEXTURE_2D);

    //glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment);
    if (buildMipmaps) {
        glGetError();
        gluBuild2DMipmaps(GL_TEXTURE_2D, texture->internalFormat, width, height,
            texture->textureFormat, texture->textureType, data);
        GLenum er = glGetError();
        if (er != GL_NO_ERROR) {
            printf("TextureData2D::createFromImage: %s\n", gluErrorString(er));
        }
    } else {
        glGetError();
        glTexImage2D(GL_TEXTURE_2D, 0, texture->internalFormat, width, height, 0,
            texture->textureFormat, texture->textureType, data);
        GLenum er = glGetError();
        if (er != GL_NO_ERROR) {
            printf("TextureData2D::createFromImage: %s\n", gluErrorString(er));
        }
    }
  #endif

    if (buffer != 0) {
        buffer->release();
    }

    return true;
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