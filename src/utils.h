// TODO: Find a place for this

#ifndef UTILS_H
#define UTILS_H

#include <map>
#include <stdio.h>
#include <string.h>

#ifdef __vita__
#include <vita2d.h>

vita2d_texture* _vita2d_load_pixmap_generic(fz_pixmap *pixmap);
#endif

const char *get_ext (const char *fspec);
float get_or(std::map<std::string, float>, std::string k, float default_value);

#endif