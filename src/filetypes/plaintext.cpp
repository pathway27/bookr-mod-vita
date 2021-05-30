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

#include <stdio.h>
#include <list>

#include "plaintext.hpp"
#include "../utils.hpp"

namespace bookr {

PlainText::PlainText() : buffer(0) { }
PlainText::~PlainText() {
  saveLastView();
  if (buffer)
    free(buffer);
}

PlainText* PlainText::create(string& file) {
  #ifdef DEBUG
    printf("PlainText::create\n");
  #endif
  PlainText* r = new PlainText();
  r->fileName = file;

  // read file to memory
  #ifdef DEBUG
    printf("pre fopen\n");
  #endif
  FILE* f = fopen(file.c_str(), "r");
  if (f == NULL) {
    #ifdef DEBUG
      printf("fopen null\n");
    #endif
    delete r;
    return NULL;
  }
  #ifdef DEBUG
    printf("post fopen\n");
  #endif

  long length = 0;
  fseek(f, 0, SEEK_END);
  length = ftell(f);
  fseek(f, 0, SEEK_SET);
  if (length > 4*1024*1024)
    length = 4*1024*1024;
  char* b = (char*)malloc(length);
  fread(b, length, 1, f);
  fclose(f);

  bool isHTML = false;
  // FIX: make the heuristic a bit more advanced than that...
  const char* fc = file.c_str();
  int fs = file.size();
  if (
    ((fc[fs - 1] | 0x20) == 'l') &&
    ((fc[fs - 2] | 0x20) == 'm') &&
    ((fc[fs - 3] | 0x20) == 't') &&
    ((fc[fs - 4] | 0x20) == 'h')
  ) {
    isHTML = true;
  }
  if (
    ((fc[fs - 1] | 0x20) == 'm') &&
    ((fc[fs - 2] | 0x20) == 't') &&
    ((fc[fs - 3] | 0x20) == 'h')
  ) {
    isHTML = true;
  }
  
  if (isHTML) {
    //r->buffer = FancyText::parseHTML(r, b, length);
  } else {
    r->buffer = FancyText::parseText(r, b, length);
  }

  #ifdef DEBUG
    printf("post parse\n");
  #endif
  
  //r->resetFonts();
  #ifdef PSP
    r->resizeView(480, 272);
  #elif defined(__vita__)
    r->resizeView(960, 544);
  #endif

  return r;
}

void PlainText::getFileName(string& fn) {
  fn = fileName;
}

void PlainText::getTitle(string& t) {
  t = "FIX PLAIN TEXT TITLES";
}

void PlainText::getType(string& t) {
  t = "Plain text";
}

bool PlainText::isPlainText(string& file) {
  // Read First 4 bytes
  FILE* f = fopen(file.c_str(), "r");
  const char* ext = get_ext(file.c_str());
  fclose(f);

  // Trusting the user for now...
  return strcmp(ext, ".txt") == 0;
}

}