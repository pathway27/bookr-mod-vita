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
#ifdef __vita__
  #include <psp2/kernel/threadmgr.h>
#endif

#include "graphics/fzscreen.h"
#include "bkuser.h"
#include "bklayer.h"
#include "bklogo.h"

int main(int argc, char* argv[]) {
    //BKDocument *documentLayer = 0;
    FZScreen::open(argc, argv);
    FZScreen::setupCtrl();

    BKUser::init();

  #ifdef DEBUG    
    psp2shell_init(3333, 0);
    sceKernelDelayThread(2*1000000);
    psp2shell_print_color(COL_GREEN, "Debug Started: in main\n");
  #endif

    BKLayer::load();
    bkLayers layers;
    layers.push_back(BKLogo::create());
    std::cout << "Hi" << std::endl;  

    // Swapping buffers based on dirty variable feels dirty.
    bool dirty = true;
    while (true) {
        // psp2shell_print("while entered");
        // draw state to back buffer
        if (dirty) {
            FZScreen::startDirectList();
            // vita2d_draw_rectangle(20, 20, 400, 250, RGBA8(255, 0, 0, 255));
            //sceKernelDelayThread(2*1000000);
            bkLayersIt it(layers.begin());
            bkLayersIt end(layers.end());
            while (it != end) {
                (*it)->render();
                ++it;
            }
            FZScreen::endAndDisplayList();
        }
        
        FZScreen::waitVblankStart();

        // draw it
        if (dirty) {
            FZScreen::swapBuffers();
            //dirty = false;
        }

        //FZScreen::checkEvents();
        //FZ_DEBUG_SCREEN_SET00
        int buttons = FZScreen::readCtrl();
        
        //std::cout << buttons << std::endl;
        //dirty = buttons != 0;

        if (buttons == FZ_CTRL_LTRIGGER || FZScreen::isClosing())
            break;
        else {
            FZScreen::checkEvents(buttons);
        }
    }

  #ifdef DEBUG  
    psp2shell_exit();
  #endif

    bkLayersIt it(layers.begin());
    bkLayersIt end(layers.end());
    while (it != end) {
    	(*it)->release();
    	++it;
    }
    layers.clear();

    BKLayer::unload();
    FZScreen::close();
    FZScreen::exit();

    return 0;
}

void bkInit() {}

void loadLastFile() {}

void clearLayers() {
  // bkLayersIt it(layers.begin());
	// bkLayersIt end(layers.end());
	// while (it != end) {
	// 	(*it)->release();
	// 	++it;
	// }
	// layers.clear();

}
