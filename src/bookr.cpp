/*
 * Original Bookr and bookr-mod for PSP
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com),
 *               2007 Christian Payeur (christian dot payeur at gmail dot com),
 *               2009 Nguyen Chi Tam (nguyenchitam at gmail dot com),
 * 
 * Modified Bookr % VITA: document reader for the Sony PS Vita
 * Copyright (C) 2017 Sreekara C. (pathway27 at gmail dot com)
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
#include <psp2/kernel/threadmgr.h>
#include "bkuser.h"

int main(int argc, char* argv[]) {
    //BKDocument *documentLayer = 0;
    BKUser::init();
    psp2shell_init(3333, 0);
    sceKernelDelayThread(2*1000000);

    psp2shell_print_color(COL_GREEN, "main\n");
    FZScreen::open(argc, argv);
    

    // Swapping buffers based on dirty variable feels dirty.
    bool dirty = true;
    bool exitApp = false;
    while (!exitApp) {
        //std::cout << "while" << std::endl;
        FZScreen::waitVblankStart();

        if (dirty) {
            //std::cout << "dirty" << std::endl;
            FZScreen::swapBuffers();
            dirty = false;

        }

        //FZScreen::checkEvents();
        //FZ_DEBUG_SCREEN_SET00
        int buttons = FZScreen::readCtrl();
        
        //std::cout << buttons << std::endl;
        //dirty = buttons != 0;

        if (buttons == FZ_CTRL_LTRIGGER || FZScreen::isClosing())
            exitApp = true;
        else {
            FZScreen::checkEvents(buttons);
        }
    }
    
    psp2shell_exit();
    FZScreen::close();
    FZScreen::exit();

    return 0;
}