// TODO: Find a place for this

#include "utils.h"

vita2d_texture* _vita2d_load_pixmap_generic(fz_pixmap *pixmap)
{
  int width = pixmap->w;
  int height = pixmap->h;

  vita2d_texture *texture = vita2d_create_empty_texture(width, height);
  void *texture_data = vita2d_texture_get_datap(texture);
  unsigned int tex_stride = vita2d_texture_get_stride(texture);


  for (int y = 0; y < height; ++y) {
    unsigned int *tex_pointer = (unsigned int *)((unsigned int *)texture_data + y*tex_stride);
    unsigned char *pixels_row = &pixmap->samples[y * pixmap->stride];

    for (int x = 0; x < width; ++x) {
      *tex_pointer = RGBA8(pixels_row[0], pixels_row[1], pixels_row[2], 255);
      tex_pointer++;
      pixels_row += pixmap->n;
    }
  }

  return texture;
}

const char *get_ext (const char *fspec) {
    char *e = strrchr (fspec, '.');
    if (e == NULL)
        e = (char *)""; // fast method, could also use &(fspec[strlen(fspec)]).
    return e;
}
