/*
 * Bookr % VITA: document reader for the Sony PS Vita
 *   A fork of Bookr for PSP
 *
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com),
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com),
 *               2017 Sreekara C. (pathway27 at gmail dot com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>

#include "graphics/fzscreen.h"

//#include "bookrconfig.h"

// start with screen!
//#include "graphics/fzscreen.h"


//#include "bkdocument.h"

// res 960 × 544

int main(int argc, char* argv[]) {
    //BKDocument *documentLayer = 0;

    std::cout << "Hi" << std::endl;
    FZScreen::open(argc, argv);
    
    return 0;
}
