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

#include "bklayer.h"

// need only one - UI font
FZFont* BKLayer::fontBig = 0;
FZFont* BKLayer::fontSmall = 0;
FZFont* BKLayer::fontUTF = 0;
FZTexture* BKLayer::texUI = 0;
FZTexture* BKLayer::texUI2 = 0;
FZTexture* BKLayer::texLogo = 0;

static FZTexture* bk_memory_icon;
static FZTexture* bk_battery_icon;
static FZTexture* bk_clock_icon;
static FZTexture* bk_circle_icon;
static FZTexture* bk_cross_icon;
static FZTexture* bk_triangle_icon;

static const unsigned int TITLE_FONT_SIZE = 28;

extern "C" {
  extern unsigned int size_res_logo;
  extern unsigned char res_logo[];
  extern unsigned int size_res_uitex;
  extern unsigned char res_uitex[];
  extern unsigned int size_res_uitex2;
  extern unsigned char res_uitex2[];
  extern unsigned char pdf_font_DroidSansFallback_ttf_buf[];
  extern unsigned int  pdf_font_DroidSansFallback_ttf_len;

  extern unsigned char res_uifont[];
  extern unsigned int size_res_uifont;

  extern unsigned char _binary_data_icons_memory_png_start;
  extern unsigned char _binary_data_icons_battery_outline_png_start;
  extern unsigned char _binary_data_icons_clock_png_start;
  extern unsigned char _binary_data_icons_circle_outline_png_start;
  extern unsigned char _binary_data_icons_close_box_outline_png_start;
  extern unsigned char _binary_data_icons_triangle_outline_png_start;
  
  extern unsigned char _binary_sce_sys_icon0_t_png_start;
  extern unsigned int _binary_sce_sys_icon0_t_png_size;
};

void BKLayer::load() {
  #ifdef DEBUG
    printf("bklayer load\n");
  #endif
  texLogo = FZTexture::createFromVitaTexture(vita2d_load_PNG_buffer(&_binary_sce_sys_icon0_t_png_start));

  bk_memory_icon = FZTexture::createFromVitaTexture(vita2d_load_PNG_buffer(&_binary_data_icons_memory_png_start));
  bk_battery_icon = FZTexture::createFromVitaTexture(vita2d_load_PNG_buffer(&_binary_data_icons_battery_outline_png_start));
  bk_clock_icon = FZTexture::createFromVitaTexture(vita2d_load_PNG_buffer(&_binary_data_icons_clock_png_start));

  bk_circle_icon = FZTexture::createFromVitaTexture(vita2d_load_PNG_buffer(&_binary_data_icons_circle_outline_png_start));
  bk_cross_icon = FZTexture::createFromVitaTexture(vita2d_load_PNG_buffer(&_binary_data_icons_close_box_outline_png_start));
  bk_triangle_icon = FZTexture::createFromVitaTexture(vita2d_load_PNG_buffer(&_binary_data_icons_triangle_outline_png_start));

  if (!fontBig){
    fontBig = FZFont::createFromMemory(res_uifont, size_res_uifont);
  }
}

void BKLayer::unload(){
  // do i need to do this?
  // crashes if i do...
  texLogo->release();
  bk_memory_icon->release();
  bk_battery_icon->release();
  bk_clock_icon->release();
  bk_circle_icon->release();
  bk_cross_icon->release();
  fontBig->release();
}

void BKLayer::drawImage(int x, int y) {

}

void BKLayer::drawImage(int x, int y, int w, int h, int tx, int ty) {

}

void BKLayer::drawImageScale(int x, int y, int w, int h, int tx, int ty, int tw, int th) {
}

void BKLayer::drawTPill(int x, int y, int w, int h, int r, int tx, int ty) {
}

void BKLayer::drawRect(int x, int y, int w, int h, int r, int tx, int ty) {
}

int BKLayer::textWidthRange(char* t, int n, FZFont* font) {
  return 0;
}

int BKLayer::textW(char* t, FZFont* font) {
  return 0;
}

void BKLayer::drawTextHC(char* t, FZFont* font, int y) {
  int w = textW(t, font);
  drawText(t, font, (480 - w) / 2, y);
}


int BKLayer::drawUTFText(const char* t, FZFont* font, int x, int y, int skipUTFChars, int maxWidth) {
  return 0;
}

int BKLayer::drawUTFMenuItem(BKMenuItem* item, FZFont* font, int x, int y, int skipPixels, int maxWidth) {
  return 0;
}

int BKLayer::drawText(char* t, FZFont* font, int x, int y, int n, bool useLF, bool usePS, float ps, bool use3D) {
  return 0;
}

#define DIALOG_ICON_SCALE 1.0
#define DIALOG_OFFSET_X 96
#define DIALOG_OFFSET_Y 40
#define DIALOG_WIDTH 768
#define DIALOG_HEIGHT 504
#define DIALOG_BG_COLOR RGBA8(47, 47, 47, 240) // Very Dark Transparent Gray

#define DIALOG_ITEM_OFFSET_X DIALOG_OFFSET_X + 10
#define DIALOG_ITEM_WIDTH DIALOG_WIDTH - 20
#define DIALOG_ITEM_HEIGHT 50

#define DIALOG_TITLE_OFFSET_Y DIALOG_OFFSET_Y + 10
#define DIALOG_TITLE_BG_COLOR RGBA8(170, 170, 170, 255) // Very Light Gray
#define DIALOG_TITLE_TEXT_OFFSET_X DIALOG_ITEM_OFFSET_X + 10
#define DIALOG_TITLE_TEXT_OFFSET_Y DIALOG_TITLE_OFFSET_Y + 35

#define DIALOG_CONTEXT_OFFSET_Y FZ_SCREEN_HEIGHT - DIALOG_ITEM_HEIGHT
#define DIALOG_CONTEXT_BG_COLOR RGBA8(85, 85, 85, 255) // Dark Gray

#define COLOR_WHITE RGBA8(255, 255, 255, 255)
#define COLOR_BLACK RGBA8(0, 0, 0, 255)

void BKLayer::drawDialogFrame(string& title, string& triangleLabel, string& circleLabel, int flags) {
  int scrY = 0;
  char *t =(char*)circleLabel.c_str();
  // int tw = textW(t, fontBig);

  #ifdef DEBUG_RENDER
    printf("draw dialog frame\n");
  #endif
  // 960
  // 920
  // 544
  // backs
  vita2d_draw_rectangle(DIALOG_OFFSET_X, DIALOG_OFFSET_Y, DIALOG_WIDTH, DIALOG_HEIGHT, DIALOG_BG_COLOR); // my cheapo drawTPill

  //title
  vita2d_draw_rectangle(DIALOG_ITEM_OFFSET_X, DIALOG_TITLE_OFFSET_Y, DIALOG_ITEM_WIDTH, DIALOG_ITEM_HEIGHT, DIALOG_TITLE_BG_COLOR);

  //context label
  vita2d_draw_rectangle(DIALOG_ITEM_OFFSET_X, DIALOG_CONTEXT_OFFSET_Y, DIALOG_ITEM_WIDTH, DIALOG_ITEM_HEIGHT, DIALOG_CONTEXT_BG_COLOR);

  //circle or other context
  // circleLabel
  vita2d_font_draw_text(fontBig->v_font, DIALOG_ITEM_WIDTH - 70,
    DIALOG_CONTEXT_OFFSET_Y + 35, COLOR_WHITE, TITLE_FONT_SIZE, t);

  switch(BKUser::controls.select) {
    case FZ_REPS_CROSS:
      vita2d_draw_texture_scale(bk_cross_icon->vita_texture, DIALOG_ITEM_WIDTH - 130, DIALOG_CONTEXT_OFFSET_Y + 7, 
        DIALOG_ICON_SCALE, DIALOG_ICON_SCALE);
      break;
    case FZ_REPS_CIRCLE:
      vita2d_draw_texture_scale(bk_circle_icon->vita_texture, DIALOG_ITEM_WIDTH - 130, DIALOG_CONTEXT_OFFSET_Y + 7,
        DIALOG_ICON_SCALE, DIALOG_ICON_SCALE);
    default:
      break;
  }

  //title
  // (255, 255, 255, 255)
  vita2d_font_draw_text(fontBig->v_font, DIALOG_TITLE_TEXT_OFFSET_X, DIALOG_TITLE_TEXT_OFFSET_Y, COLOR_WHITE, TITLE_FONT_SIZE, title.c_str());

  // triangle labels
  if (triangleLabel.size() > 0 || (flags & BK_MENU_ITEM_OPTIONAL_TRIANGLE_LABEL)) {
    vita2d_draw_texture_scale(bk_triangle_icon->vita_texture, DIALOG_TITLE_TEXT_OFFSET_X, DIALOG_CONTEXT_OFFSET_Y + 7, 
      DIALOG_ICON_SCALE, DIALOG_ICON_SCALE);
    vita2d_font_draw_text(fontBig->v_font, DIALOG_TITLE_TEXT_OFFSET_X + 60,
      DIALOG_CONTEXT_OFFSET_Y + 35, COLOR_WHITE, TITLE_FONT_SIZE, triangleLabel.c_str());
  }
}

void BKLayer::drawMenu(string& title, string& triangleLabel, vector<BKMenuItem>& items) {
  drawMenu(title, triangleLabel, items, false);
}

void BKLayer::drawMenu(string& title, string& triangleLabel, vector<BKMenuItem>& items, string& upperBreadCrumb) {
  drawMenu(title, triangleLabel, items, false);
  FZScreen::drawText(300, 83, RGBA8(255, 255, 255, 255), 1.0f, upperBreadCrumb.c_str());
}

#define DIALOG_MENU_FIRST_ITEM_OFFSET_Y DIALOG_TITLE_OFFSET_Y + DIALOG_ITEM_HEIGHT + 15
#define DIALOG_MENU_ITEM_TEXT_OFFSET_X DIALOG_ITEM_OFFSET_X + 60
#define DIALOG_MENU_ITEM_HEIGHT 40

void BKLayer::drawMenu(string& title, string& triangleLabel, vector<BKMenuItem>& items, bool useUTFFont) {
  int maxItemNum = 8;
  int selPos = selItem - topItem;
  FZFont* itemFont;
  itemFont = fontBig;

  if (selPos < 0) {
    topItem += selPos;
    selPos = 0;
  }

  if (selPos > maxItemNum - 1) {
    topItem += selPos - maxItemNum + 1;
    selPos = maxItemNum - 1;
  }

  bool scrollbar = items.size() > maxItemNum;

  string tl(triangleLabel);
  if (items[selItem].flags & BK_MENU_ITEM_OPTIONAL_TRIANGLE_LABEL) {
    tl = items[selItem].triangleLabel;
  }
  #ifdef DEBUG_RENDER
    printf("drawmenu\n");
  #endif
  drawDialogFrame(title, tl, items[selItem].circleLabel, items[selItem].flags);

  // selectedItem
  int wSelBox = scrollbar ? DIALOG_ITEM_WIDTH - 50: DIALOG_ITEM_WIDTH;
  vita2d_draw_rectangle(DIALOG_ITEM_OFFSET_X,
    (DIALOG_MENU_FIRST_ITEM_OFFSET_Y + (selPos*DIALOG_MENU_ITEM_HEIGHT)),
    wSelBox, DIALOG_MENU_ITEM_HEIGHT, COLOR_WHITE);

  // check if folder

  // scrollbar
  if (scrollbar) {
    float barh = 8.0f / float(items.size());
    barh *= 73.0f;
    if (barh < 15.0f)
      barh = 15.0f;
    float trel = float(topItem) / float(items.size());
    trel *= 73.0f;

    vita2d_draw_rectangle(DIALOG_OFFSET_X + wSelBox + 20,
      DIALOG_MENU_FIRST_ITEM_OFFSET_Y,
      40,
      DIALOG_CONTEXT_OFFSET_Y - DIALOG_MENU_FIRST_ITEM_OFFSET_Y - 200 - 30,
    0xff555555);

    vita2d_draw_rectangle(DIALOG_OFFSET_X + wSelBox + 20,
      DIALOG_MENU_FIRST_ITEM_OFFSET_Y + int(trel),
      40,
      DIALOG_CONTEXT_OFFSET_Y - DIALOG_MENU_FIRST_ITEM_OFFSET_Y - 200 - int(barh),
    0xffaaaaaa);
  }


  for (int i = 0; i < maxItemNum; ++i) {
    if ((i + topItem) >= (int)(items.size()))
      break;

    if ((i + topItem) == selItem)
      vita2d_font_draw_text(fontBig->v_font, DIALOG_MENU_ITEM_TEXT_OFFSET_X,
        (DIALOG_MENU_FIRST_ITEM_OFFSET_Y + ((i+1)*DIALOG_MENU_ITEM_HEIGHT) - 10),
        COLOR_BLACK, TITLE_FONT_SIZE, items[i + topItem].label.c_str());
    else
      vita2d_font_draw_text(fontBig->v_font, DIALOG_MENU_ITEM_TEXT_OFFSET_X,
        (DIALOG_MENU_FIRST_ITEM_OFFSET_Y + ((i+1)*DIALOG_MENU_ITEM_HEIGHT) - 10),
        COLOR_WHITE, TITLE_FONT_SIZE, items[i + topItem].label.c_str());
  }
}

void BKLayer::drawOutlinePrefix(string prefix, int x, int y, int w, int h, int ws){

}

void BKLayer::drawOutline(string& title, string& triangleLabel, vector<BKOutlineItem>& items, bool useUTFFont) {
}

static int countLines(string& t) {
  int lines = 1;
  char* c = (char*)t.c_str();
  while (*c != 0) {
    if (*c == 0xa) ++lines;
    ++c;
  }
  return lines;
}

void BKLayer::drawPopup(string& text, string& title, int bg1, int bg2, int fg) {  
  int l = countLines(text);

  int h = 22 + (22*l);
  int y;
  if (h >= 544)
    y = 0;
  else
    y = (544 - h) / 2;

  // back
  vita2d_draw_rectangle(80, y, 960 - 156, h, bg1);

  // title
  vita2d_draw_rectangle(90, 10 + y, 960 - 176, 30, bg2);


  // // icons
  // FZScreen::ambientColor(bg1|0xff000000);
  // // drawImage(410, 9 + y, BK_IMG_CIRCLE_XSIZE, BK_IMG_CIRCLE_YSIZE, BK_IMG_CIRCLE_X, BK_IMG_CIRCLE_Y);
  // switch (BKUser::controls.select) {
  // case FZ_REPS_CIRCLE:
  // 	drawImage(410, 9 + y, BK_IMG_CROSS_XSIZE, BK_IMG_CROSS_YSIZE, BK_IMG_CROSS_X, BK_IMG_CROSS_Y);
  // 	break;
  // case FZ_REPS_CROSS:
  // default:
  // 	drawImage(410, 9 + y, BK_IMG_CIRCLE_XSIZE, BK_IMG_CIRCLE_YSIZE, BK_IMG_CIRCLE_X, BK_IMG_CIRCLE_Y);
  // 	break;
  // }

  //fontBig->bindForDisplay();


  FZScreen::drawText(102, y + 30, fg, 1.0f, title.c_str());
  FZScreen::drawText(102, y + 65, fg, 1.0f, text.c_str());
}

#define _PI_OVER_180 0.0174532925199432957692369076849f
#define _180_OVER_PI 57.2957795130823208767981548141f

#define DEG_TO_RAD(x) (x * _PI_OVER_180)
#define RAD_TO_DEG(x) (x * _180_OVER_PI)

// RGBA8(75,75,75,255)
#define DIALOG_ICON_COLOR 0xffbbbbbb
#define DIALOG_ICON_TEXT_SIZE TITLE_FONT_SIZE - 6


#define DIALOG_ICON_OFFSET_Y DIALOG_CONTEXT_OFFSET_Y - 35
#define DIALOG_ICON_TEXT_OFFSET_Y DIALOG_CONTEXT_OFFSET_Y - 10


void BKLayer::drawClockAndBattery(string& extra) {
  int ew = textW((char*)extra.c_str(), fontSmall);
  // drawText((char*)extra.c_str(), fontSmall, 480 - 30 - ew, 205);

  // cpu speed
  vita2d_font_draw_textf(fontBig->v_font, DIALOG_MENU_ITEM_TEXT_OFFSET_X + 255,
    DIALOG_ICON_TEXT_OFFSET_Y,
    DIALOG_ICON_COLOR, DIALOG_ICON_TEXT_SIZE, "%dMHz", FZScreen::getSpeed());

  // cpu icon
  vita2d_draw_texture_tint_scale(bk_memory_icon->vita_texture, DIALOG_MENU_ITEM_TEXT_OFFSET_X + 345, 
    DIALOG_ICON_OFFSET_Y, DIALOG_ICON_SCALE, DIALOG_ICON_SCALE, DIALOG_ICON_COLOR);

  // memory usage
  vita2d_font_draw_textf(fontBig->v_font, DIALOG_MENU_ITEM_TEXT_OFFSET_X + 395,
    DIALOG_ICON_TEXT_OFFSET_Y,
    DIALOG_ICON_COLOR, DIALOG_ICON_TEXT_SIZE, "%dK", FZScreen::getUsedMemory() / 1024);

  // battery icon
  vita2d_draw_texture_tint_scale_rotate(bk_battery_icon->vita_texture, DIALOG_MENU_ITEM_TEXT_OFFSET_X + 485,
    DIALOG_ICON_OFFSET_Y + 17, DIALOG_ICON_SCALE, DIALOG_ICON_SCALE,
    DEG_TO_RAD(90), DIALOG_ICON_COLOR);

  // battery %
  vita2d_font_draw_textf(fontBig->v_font, DIALOG_MENU_ITEM_TEXT_OFFSET_X + 510,
    DIALOG_ICON_TEXT_OFFSET_Y,
    DIALOG_ICON_COLOR, DIALOG_ICON_TEXT_SIZE, "%d%%", FZScreen::getBattery());

  // clock icon
  vita2d_draw_texture_tint_scale(bk_clock_icon->vita_texture, DIALOG_MENU_ITEM_TEXT_OFFSET_X + 565,
    DIALOG_ICON_OFFSET_Y + 5, DIALOG_ICON_SCALE, DIALOG_ICON_SCALE, DIALOG_ICON_COLOR);

  // time text
  int h = 0, m = 0;
  FZScreen::getTime(h, m);
  vita2d_font_draw_textf(fontBig->v_font, DIALOG_MENU_ITEM_TEXT_OFFSET_X + 600,
    DIALOG_ICON_TEXT_OFFSET_Y,
    DIALOG_ICON_COLOR, DIALOG_ICON_TEXT_SIZE, "%02d:%02d", h, m);
}

void BKLayer::menuCursorUpdate(unsigned int buttons, int max) {
  int* b = FZScreen::ctrlReps();
  if (b[BKUser::controls.menuUp] == 1 || (b[BKUser::controls.menuUp] > 10 && b[BKUser::controls.menuUp] % 5 == 0)) {
    selItem--;
    if (selItem < 0) {
      selItem = max - 1;
    }
    skipChars = 0;
    maxSkipChars = -1;
  }
  if (b[BKUser::controls.menuDown] == 1 || (b[BKUser::controls.menuDown] > 10 && b[BKUser::controls.menuDown] % 5 == 0)) {
    selItem++;
    if (selItem >= max) {
      selItem = 0;
    }
    skipChars = 0;
    maxSkipChars = -1;
  }
  if (b[BKUser::controls.menuLeft] == 1 || (b[BKUser::controls.menuLeft] > 10 && b[BKUser::controls.menuLeft] % 5 == 0)) {
    skipChars--;
    if (skipChars < 0) {
      skipChars = 0;
    }
  }
  if (b[BKUser::controls.menuRight] == 1 || (b[BKUser::controls.menuRight] > 10 && b[BKUser::controls.menuRight] % 5 == 0)) {
    skipChars++;
    if (maxSkipChars >= 0 && skipChars>maxSkipChars)
      skipChars = maxSkipChars;
  }
}

BKLayer::BKLayer() : topItem(0), selItem(0),skipChars(0),maxSkipChars(-1) {
}

BKLayer::~BKLayer() {
}
