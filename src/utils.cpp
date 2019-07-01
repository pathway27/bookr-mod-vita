// TODO: Find a place for this

#include "graphics/screen.hpp"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef __vita__
vita2d_texture* _vita2d_load_pixmap_generic(fz_pixmap *pixmap)
{
  srand(time(NULL));

  #ifdef DEBUG
    printf("_vita2d_load_pixmap_generic\n");
  #endif
  int width = pixmap->w;
  int height = pixmap->h;

  printf("creating empty texture w: %i h: %i\n", width, height);
  vita2d_texture *texture = vita2d_create_empty_texture(width, height);
  if (texture == NULL) {
    printf("failed to create empty texture\n");
    return NULL;
  }

  void *texture_data = vita2d_texture_get_datap(texture);
  unsigned int tex_stride = vita2d_texture_get_stride(texture);

  printf("got text data/stride %i\n", tex_stride);
  
  // Crashes on bad pdfs and too much zoom?
  for (int y = 0; y < height; ++y) {
    unsigned int *tex_pointer = (unsigned int *)(texture_data + y*tex_stride);
    unsigned char *pixels_row = &pixmap->samples[y * pixmap->stride];

    for (int x = 0; x < width; ++x) {
      *tex_pointer = RGBA8(pixels_row[0], pixels_row[1], pixels_row[2], 255);
      tex_pointer++;
      pixels_row += pixmap->n;
    }

    //printf("out of y loop\n");
  }

  #ifdef DEBUG
    printf("_vita2d_load_pixmap_generic end\n");
  #endif
  return texture;
}
#endif

const char *get_ext (const char *fspec) {
    char *e = strrchr (fspec, '.');
    if (e == NULL)
        e = (char *)""; // fast method, could also use &(fspec[strlen(fspec)]).
    return e;
}

// Put this in bookmark class?
float get_or(std::map<std::string, float> m, std::string key, float default_value) {
  auto it = m.find(key);
  if (it == m.end())
  {
    return default_value;
  }
  return it->second;
}