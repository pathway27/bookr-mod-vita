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

#ifndef BKPLAINTEXT_H
#define BKPLAINTEXT_H

#include "../graphics/screen.hpp"
#include "fancytext.hpp"

using std::string;

namespace bookr {

class PlainText : public FancyText {
private:
  string fileName;
  char* buffer;

protected:
  PlainText();
  ~PlainText();

public:
  virtual void getFileName(string&);
  virtual void getTitle(string&);
  virtual void getType(string&);

  static PlainText* create(string& file);
  static bool isPlainText(string& file);
};

}

#endif
