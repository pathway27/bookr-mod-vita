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
 
 * AND VARIOUS OTHER FORKS. See CREDITS in README.md
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


#ifdef PSP
  #include <pspthreadman.h>
#elif defined(__vita__)
  #include <psp2/kernel/threadmgr.h> 
  #include <vita2d.h>
#endif

#include <string.h>

#include "bklogo.h"

BKLogo::BKLogo() : loading(false), error(false), text("") {
}

BKLogo::~BKLogo() {
}

void BKLogo::setLoading(bool v) {
    loading = v;
}

void BKLogo::setError(bool v) {
    error = v;
}

int BKLogo::update(unsigned int buttons) {
    int* b = FZScreen::ctrlReps();
    if (b[BKUser::controls.showMainMenu] == 1) {
        return BK_CMD_INVOKE_MENU;
    }
    return 0;
}

void BKLogo::render() {
  #ifdef DEBUG
    psp2shell_print("bklogo render");
  #endif
    FZScreen::clear(0xffffff, FZ_COLOR_BUFFER);
    FZScreen::color(0xffffffff);

    FZScreen::matricesFor2D();
    FZScreen::enable(FZ_TEXTURE_2D);
    FZScreen::disable(FZ_BLEND);
  
  #ifdef __vita__
    #ifdef DEBUG
      psp2shell_print("bklogo draw texture");
    #endif

    vita2d_draw_texture(texLogo->vita_texture, 350, 150);
    if (loading)
        FZScreen::drawText(380, 400, RGBA8(0,0,0,255), 1.0f, "Loading...");
    else
        FZScreen::drawText(380, 400, RGBA8(0,0,0,255), 1.0f, "Only TXTs for now.");

    vita2d_draw_rectangle(96, 504, 768, 40, RGBA8(105,105,105,255)); // my cheapo drawPill
    FZScreen::drawText(255, 527, RGBA8(255,255,255,255), 1.0f, "Press Start");
    
  #else
    genLogo->bindForDisplay();
    drawImage(0, 0, FZ_SCREEN_WIDTH, FZ_SCREEN_HEIGHT, 0, 0);
    texLogo->bindForDisplay();
    drawImage(76, 360, 128, 128, 0, 0);
  #endif

    // FZScreen::enable(FZ_BLEND);
    // FZScreen::blendFunc(FZ_ADD, FZ_SRC_ALPHA, FZ_ONE_MINUS_SRC_ALPHA);
    
    // texUI->bindForDisplay();
    
    // FZScreen::ambientColor(0xf0222222);
    // // drawPill(150, 240, 180, 20, 6, 31, 1);
    // drawPill(20, 240, 430, 20, 6, 31, 1);

    // fontBig->bindForDisplay();
    // FZScreen::ambientColor(0xff000000);
    // drawTextHC("PDF - TXT - PalmDoc - DJVU - CHM - HTML", fontBig, 180);
    // FZScreen::ambientColor(0xffffffff);
    // if (loading)
    //     drawTextHC("Loading...", fontBig, 244);
    // else if (text.length() > 0)
    //     drawTextHC((char*) text.c_str(), fontBig, 244);
    // else {
    //     if (error) {
    //         texUI->bindForDisplay();
    //         FZScreen::ambientColor(0xf06060ff);
    //         drawRect(0, 126, 480, 26, 6, 31, 1);
    //         fontBig->bindForDisplay();
    //         FZScreen::ambientColor(0xff222222);
    //         drawTextHC("Error: invalid or corrupted file", fontBig, 130);
    //     }
        // FZScreen::ambientColor(0xffffffff);
        // drawTextHC("Press Start", fontBig, 244);
    // }
}

BKLogo* BKLogo::create() {
    BKLogo* f = new BKLogo();
    FZScreen::resetReps();
    return f;
}

void BKLogo::show(string text) {
    show(text, 0);
}

void BKLogo::show(string text, int delaySeconds) {
    BKLogo* l = BKLogo::create();
    l->text = text;
    FZScreen::startDirectList();
    l->render();
    FZScreen::endAndDisplayList();
    FZScreen::waitVblankStart();
    FZScreen::swapBuffers();
    //FZScreen::checkEvents();
    l->release();
    if (delaySeconds > 0) {
        #if defined(PSP) || defined(__vita__)
          sceKernelDelayThread(delaySeconds * 1000000);
        #endif
    }
}

