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

#include "layer.hpp"
#include "graphics/resolutions.hpp"
#include "graphics/controls.hpp"

using std::make_pair;

namespace bookr {

// Yeah, ok.
#ifdef __vita__
  #define drawFontTextf(font, x, y, color, size, text, ...) vita2d_font_draw_textf(font->v_font, x, y, color, size, text, __VA_ARGS__)
#else
  #define drawFontTextf Screen::drawFontTextf
#endif

// need only one - UI font
Font* Layer::fontBig = 0;
Font* Layer::fontSmall = 0;
Font* Layer::fontUTF = 0;
Texture* Layer::texUI = 0;
Texture* Layer::texUI2 = 0;
Texture* Layer::texLogo = 0;

const auto& createTexFromBuffer = Texture::createFromBuffer;
map<string, Texture*> Layer::bk_icons;

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

  extern unsigned char _binary_data_icons_collections_bookmark_white_png_start;
  extern unsigned char _binary_data_icons_content_copy_white_png_start;
  extern unsigned char _binary_data_icons_search_white_png_start;
  extern unsigned char _binary_data_icons_rotate_left_white_png_start;
  extern unsigned char _binary_data_icons_rotate_right_white_png_start;

  extern unsigned char _binary_data_icons_bookmark_add_white_png_start;
  extern unsigned char _binary_data_icons_first_page_png_start;
  extern unsigned char _binary_data_icons_last_page_png_start;
  extern unsigned char _binary_data_icons_previous_ten_png_start;
  extern unsigned char _binary_data_icons_next_ten_png_start;
  extern unsigned char _binary_data_icons_go_to_page_png_start;

  extern unsigned char _binary_data_icons_fit_height_png_start;
  extern unsigned char _binary_data_icons_fit_width_png_start;
  extern unsigned char _binary_data_icons_zoom_out_white_png_start;
  extern unsigned char _binary_data_icons_zoom_in_white_png_start;

  extern unsigned char _binary_data_logos_icon0_t_png_start;
  extern unsigned int _binary_data_logos_icon0_t_png_size;
};

void Layer::load() {
  #ifdef DEBUG
    printf("bklayer load\n");
  #endif
  
  texLogo = Texture::createFromBuffer(&_binary_data_logos_icon0_t_png_size);

  // TODO: fix serious uglyness, replace with old spritesheet code? IDK.
  bk_icons.insert(make_pair("bk_memory_icon", createTexFromBuffer(&_binary_data_icons_memory_png_start)));
  bk_icons.insert(make_pair("bk_battery_icon", createTexFromBuffer(&_binary_data_icons_battery_outline_png_start)));
  bk_icons.insert(make_pair("bk_clock_icon", createTexFromBuffer(&_binary_data_icons_clock_png_start)));

  bk_icons.insert(make_pair("bk_circle_icon", createTexFromBuffer(&_binary_data_icons_circle_outline_png_start)));
  bk_icons.insert(make_pair("bk_cross_icon", createTexFromBuffer(&_binary_data_icons_close_box_outline_png_start)));
  bk_icons.insert(make_pair("bk_triangle_icon", createTexFromBuffer(&_binary_data_icons_triangle_outline_png_start)));

  bk_icons.insert(make_pair("bk_bookmark_icon", createTexFromBuffer(&_binary_data_icons_collections_bookmark_white_png_start)));
  bk_icons.insert(make_pair("bk_copy_icon", createTexFromBuffer(&_binary_data_icons_content_copy_white_png_start)));
  bk_icons.insert(make_pair("bk_search_icon", createTexFromBuffer(&_binary_data_icons_search_white_png_start)));
  bk_icons.insert(make_pair("bk_rotate_left_icon", createTexFromBuffer(&_binary_data_icons_rotate_left_white_png_start)));
  bk_icons.insert(make_pair("bk_rotate_right_icon", createTexFromBuffer(&_binary_data_icons_rotate_right_white_png_start)));

  bk_icons.insert(make_pair("bk_add_bookmark_icon", createTexFromBuffer(&_binary_data_icons_bookmark_add_white_png_start)));
  bk_icons.insert(make_pair("bk_first_page_icon", createTexFromBuffer(&_binary_data_icons_first_page_png_start)));
  bk_icons.insert(make_pair("bk_last_page_icon", createTexFromBuffer(&_binary_data_icons_last_page_png_start)));
  bk_icons.insert(make_pair("bk_prev_ten_icon", createTexFromBuffer(&_binary_data_icons_previous_ten_png_start)));
  bk_icons.insert(make_pair("bk_next_ten_icon", createTexFromBuffer(&_binary_data_icons_next_ten_png_start)));
  bk_icons.insert(make_pair("bk_go_to_page_icon", createTexFromBuffer(&_binary_data_icons_go_to_page_png_start)));
  
  bk_icons.insert(make_pair("bk_fit_height_icon", createTexFromBuffer(&_binary_data_icons_fit_height_png_start)));
  bk_icons.insert(make_pair("bk_fit_width_icon", createTexFromBuffer(&_binary_data_icons_fit_width_png_start)));
  bk_icons.insert(make_pair("bk_zoom_out_icon", createTexFromBuffer(&_binary_data_icons_zoom_out_white_png_start)));
  bk_icons.insert(make_pair("bk_zoom_in_icon", createTexFromBuffer(&_binary_data_icons_zoom_in_white_png_start)));

  fontBig = Font::createFromMemory(res_uifont, size_res_uifont);
  fontSmall = Font::createFromMemory(res_uifont, size_res_uifont);
}

void Layer::unload(){
  // do i need to do this?
  texLogo->release();

  map<string, Texture*>::iterator it = bk_icons.begin();
  while(it != bk_icons.end()) {
    it->second->release();
    it++;
  }
  #ifdef DEBUG
    printf("finish icons unload\n");
  #endif

  fontBig->release();
  #ifdef DEBUG
    printf("finish fontbig unload\n");
  #endif
  fontBig->release();
  fontSmall->release();
}

static const unsigned int TEXT_PADDED_WIDTH = 20;
static const char *LOADING_TEXT = "Loading...";
static const char *DEFAULT_TEXT = "Press Start";
void Layer::drawLogo(bool loading, string text, bool error) {
  vita2d_draw_texture(texLogo->vita_texture, 350, 150);
  vita2d_font_draw_text(fontBig->v_font, 260, 440, RGBA8(0, 0, 0, 255), TITLE_FONT_SIZE, "TXT - PDF - CBZ - HTML - EPUB - FB2");

  vita2d_draw_rectangle(96, 494, 768, 40, RGBA8(105, 105, 105, 255)); // my cheapo drawPill

  if (loading)
    vita2d_font_draw_textf(fontBig->v_font, 350, 524, RGBA8(255, 255, 255, 255), TITLE_FONT_SIZE,
                           "%*s", TEXT_PADDED_WIDTH / 2 + strlen(LOADING_TEXT) / 2, LOADING_TEXT);
  else if (text.length() > 0 && !(error))
    vita2d_font_draw_textf(fontBig->v_font, 350, 524, RGBA8(255, 255, 255, 255), TITLE_FONT_SIZE,
                           "%*s", TEXT_PADDED_WIDTH / 2 + strlen(text.c_str()) / 2, text.c_str());
  else
  {
    if (error)
    {
      vita2d_font_draw_textf(fontBig->v_font, 350, 524, RGBA8(200, 0, 0, 255), TITLE_FONT_SIZE,
                             "Error: %*s", TEXT_PADDED_WIDTH / 2 + strlen(text.c_str()) / 2, text.c_str());
    }
    else
      vita2d_font_draw_textf(fontBig->v_font, 350, 524, RGBA8(255, 255, 255, 255), TITLE_FONT_SIZE,
                             "%*s", TEXT_PADDED_WIDTH / 2 + strlen(DEFAULT_TEXT) / 2, DEFAULT_TEXT);
  }

}

void Layer::drawImage(int x, int y) {

}

void Layer::drawImage(int x, int y, int w, int h, int tx, int ty) {

}

void Layer::drawImageScale(int x, int y, int w, int h, int tx, int ty, int tw, int th) {
}

void Layer::drawTPill(int x, int y, int w, int h, int r, int tx, int ty) {
}

void Layer::drawRect(int x, int y, int w, int h, int r, int tx, int ty) {
}

int Layer::textWidthRange(char* t, int n, Font* font) {
  return 0;
}

int Layer::textW(char* t, Font* font) {
  return font->fontTextWidth(t);
}

void Layer::drawTextHC(char* t, Font* font, int y) {
  int w = textW(t, font);
  drawText(t, font, (480 - w) / 2, y);
}


int Layer::drawUTFText(const char* t, Font* font, int x, int y, int skipUTFChars, int maxWidth) {
  return 0;
}

int Layer::drawUTFMenuItem(MenuItem* item, Font* font, int x, int y, int skipPixels, int maxWidth) {
  return 0;
}

int Layer::drawText(char* t, Font* font, int x, int y, int n, bool useLF, bool usePS, float ps, bool use3D) {
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

void Layer::drawDialogFrame(string& title, string& triangleLabel, string& circleLabel, int flags) {
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
  Screen::drawRectangle(DIALOG_OFFSET_X, DIALOG_OFFSET_Y, DIALOG_WIDTH, DIALOG_HEIGHT, DIALOG_BG_COLOR); // my cheapo drawTPill

  //title
  Screen::drawRectangle(DIALOG_ITEM_OFFSET_X, DIALOG_TITLE_OFFSET_Y, DIALOG_ITEM_WIDTH, DIALOG_ITEM_HEIGHT, DIALOG_TITLE_BG_COLOR);

  //context label
  Screen::drawRectangle(DIALOG_ITEM_OFFSET_X, DIALOG_CONTEXT_OFFSET_Y, DIALOG_ITEM_WIDTH, DIALOG_ITEM_HEIGHT, DIALOG_CONTEXT_BG_COLOR);

  //circle or other context
  // circleLabel
  Screen::drawFontText(fontBig, DIALOG_ITEM_WIDTH - 70,
    DIALOG_CONTEXT_OFFSET_Y + 35, COLOR_WHITE, TITLE_FONT_SIZE, t);

  switch(User::controls.select) {
    case FZ_REPS_CROSS:
      Screen::drawTextureScale(bk_icons["bk_cross_icon"], DIALOG_ITEM_WIDTH - 130, DIALOG_CONTEXT_OFFSET_Y + 7, 
        DIALOG_ICON_SCALE, DIALOG_ICON_SCALE);
      break;
    case FZ_REPS_CIRCLE:
      Screen::drawTextureScale(bk_icons["bk_circle_icon"], DIALOG_ITEM_WIDTH - 130, DIALOG_CONTEXT_OFFSET_Y + 7,
        DIALOG_ICON_SCALE, DIALOG_ICON_SCALE);
    default:
      break;
  }

  //title
  // (255, 255, 255, 255)
  Screen::drawFontText(fontBig, DIALOG_TITLE_TEXT_OFFSET_X, DIALOG_TITLE_TEXT_OFFSET_Y, COLOR_WHITE, TITLE_FONT_SIZE, title.c_str());

  // triangle labels
  if (triangleLabel.size() > 0 || (flags & BK_MENU_ITEM_OPTIONAL_TRIANGLE_LABEL)) {
    Screen::drawTextureScale(bk_icons["bk_triangle_icon"], DIALOG_TITLE_TEXT_OFFSET_X, DIALOG_CONTEXT_OFFSET_Y + 7, 
      DIALOG_ICON_SCALE, DIALOG_ICON_SCALE);
    Screen::drawFontText(fontBig, DIALOG_TITLE_TEXT_OFFSET_X + 60,
      DIALOG_CONTEXT_OFFSET_Y + 35, COLOR_WHITE, TITLE_FONT_SIZE, triangleLabel.c_str());
  }
}

void Layer::drawMenu(string& title, string& triangleLabel, vector<MenuItem>& items) {
  drawMenu(title, triangleLabel, items, false);
}

void Layer::drawMenu(string& title, string& triangleLabel, vector<MenuItem>& items, string& upperBreadCrumb) {
  drawMenu(title, triangleLabel, items, false);
  Screen::drawText(300, 83, RGBA8(255, 255, 255, 255), 1.0f, upperBreadCrumb.c_str());
}

#define DIALOGBK_MENU_FIRST_ITEM_OFFSET_Y DIALOG_TITLE_OFFSET_Y + DIALOG_ITEM_HEIGHT + 15
#define DIALOGBK_MENU_ITEM_TEXT_OFFSET_X DIALOG_ITEM_OFFSET_X + 60
#define DIALOGBK_MENU_ITEM_HEIGHT 40

void Layer::drawMenu(string& title, string& triangleLabel, vector<MenuItem>& items, bool useUTFFont) {
  int maxItemNum = 8;
  int selPos = selItem - topItem;
  Font* itemFont;
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
  Screen::drawRectangle(DIALOG_ITEM_OFFSET_X,
    (DIALOGBK_MENU_FIRST_ITEM_OFFSET_Y + (selPos*DIALOGBK_MENU_ITEM_HEIGHT)),
    wSelBox, DIALOGBK_MENU_ITEM_HEIGHT, COLOR_WHITE);

  // check if folder

  // scrollbar
  if (scrollbar) {
    float barh = 8.0f / float(items.size());
    barh *= 73.0f;
    if (barh < 15.0f)
      barh = 15.0f;
    float trel = float(topItem) / float(items.size());
    trel *= 73.0f;

    Screen::drawRectangle(DIALOG_OFFSET_X + wSelBox + 20,
      DIALOGBK_MENU_FIRST_ITEM_OFFSET_Y,
      40,
      DIALOG_CONTEXT_OFFSET_Y - DIALOGBK_MENU_FIRST_ITEM_OFFSET_Y - 200 - 30,
    0xff555555);

    Screen::drawRectangle(DIALOG_OFFSET_X + wSelBox + 20,
      DIALOGBK_MENU_FIRST_ITEM_OFFSET_Y + int(trel),
      40,
      DIALOG_CONTEXT_OFFSET_Y - DIALOGBK_MENU_FIRST_ITEM_OFFSET_Y - 200 - int(barh),
    0xffaaaaaa);
  }


  for (int i = 0; i < maxItemNum; ++i) {
    if ((i + topItem) >= (int)(items.size()))
      break;

    if ((i + topItem) == selItem)
      Screen::drawFontText(fontBig, DIALOGBK_MENU_ITEM_TEXT_OFFSET_X,
        (DIALOGBK_MENU_FIRST_ITEM_OFFSET_Y + ((i+1)*DIALOGBK_MENU_ITEM_HEIGHT) - 10),
        COLOR_BLACK, TITLE_FONT_SIZE, items[i + topItem].label.c_str());
    else
      Screen::drawFontText(fontBig, DIALOGBK_MENU_ITEM_TEXT_OFFSET_X,
        (DIALOGBK_MENU_FIRST_ITEM_OFFSET_Y + ((i+1)*DIALOGBK_MENU_ITEM_HEIGHT) - 10),
        COLOR_WHITE, TITLE_FONT_SIZE, items[i + topItem].label.c_str());
  }
}

void Layer::drawOutlinePrefix(string prefix, int x, int y, int w, int h, int ws){

}

void Layer::drawOutline(string& title, string& triangleLabel, vector<OutlineItem>& items, bool useUTFFont) {
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

void Layer::drawPopup(string& text, string& title, int bg1, int bg2, int fg) {  
  int l = countLines(text);

  int h = 22 + (22*l);
  int y;
  if (h >= 544)
    y = 0;
  else
    y = (544 - h) / 2;

  // back
  Screen::drawRectangle(80, y, 960 - 156, h, bg1);

  // title
  Screen::drawRectangle(90, 10 + y, 960 - 176, 30, bg2);


  // // icons
  // Screen::ambientColor(bg1|0xff000000);
  // // drawImage(410, 9 + y, _IMG_CIRCLE_XSIZE, _IMG_CIRCLE_YSIZE, _IMG_CIRCLE_X, _IMG_CIRCLE_Y);
  // switch (User::controls.select) {
  // case FZ_REPS_CIRCLE:
  // 	drawImage(410, 9 + y, _IMG_CROSS_XSIZE, _IMG_CROSS_YSIZE, _IMG_CROSS_X, _IMG_CROSS_Y);
  // 	break;
  // case FZ_REPS_CROSS:
  // default:
  // 	drawImage(410, 9 + y, _IMG_CIRCLE_XSIZE, _IMG_CIRCLE_YSIZE, _IMG_CIRCLE_X, _IMG_CIRCLE_Y);
  // 	break;
  // }

  //fontBig->bindForDisplay();


  Screen::drawText(102, y + 30, fg, 1.0f, title.c_str());
  Screen::drawText(102, y + 65, fg, 1.0f, text.c_str());
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


void Layer::drawClockAndBattery(string& extra) {
  // int ew = textW((char*)extra.c_str(), fontSmall);
  Screen::drawFontText(fontSmall, DIALOGBK_MENU_ITEM_TEXT_OFFSET_X + 565,
    DIALOG_ICON_TEXT_OFFSET_Y - 45,
    DIALOG_ICON_COLOR, DIALOG_ICON_TEXT_SIZE, extra.c_str());

  // cpu speed
  drawFontTextf(fontSmall, DIALOGBK_MENU_ITEM_TEXT_OFFSET_X + 255,
    DIALOG_ICON_TEXT_OFFSET_Y,
    DIALOG_ICON_COLOR, DIALOG_ICON_TEXT_SIZE, "%dMHz", Screen::getSpeed());

  // cpu icon
  Screen::drawTextureTintScale(bk_icons["bk_memory_icon"], DIALOGBK_MENU_ITEM_TEXT_OFFSET_X + 345, 
    DIALOG_ICON_OFFSET_Y, DIALOG_ICON_SCALE, DIALOG_ICON_SCALE, DIALOG_ICON_COLOR);

  // memory usage
  drawFontTextf(fontSmall, DIALOGBK_MENU_ITEM_TEXT_OFFSET_X + 395,
    DIALOG_ICON_TEXT_OFFSET_Y,
    DIALOG_ICON_COLOR, DIALOG_ICON_TEXT_SIZE, "%dK", Screen::getUsedMemory() / 1024);

  // battery icon
  Screen::drawTextureTintScaleRotate(bk_icons["bk_battery_icon"], DIALOGBK_MENU_ITEM_TEXT_OFFSET_X + 485,
    DIALOG_ICON_OFFSET_Y + 17, DIALOG_ICON_SCALE, DIALOG_ICON_SCALE,
    DEG_TO_RAD(90), DIALOG_ICON_COLOR);

  // battery %
  drawFontTextf(fontSmall, DIALOGBK_MENU_ITEM_TEXT_OFFSET_X + 510,
    DIALOG_ICON_TEXT_OFFSET_Y,
    DIALOG_ICON_COLOR, DIALOG_ICON_TEXT_SIZE, "%d%%", Screen::getBattery());

  // clock icon
  Screen::drawTextureTintScale(bk_icons["bk_clock_icon"], DIALOGBK_MENU_ITEM_TEXT_OFFSET_X + 565,
    DIALOG_ICON_OFFSET_Y + 5, DIALOG_ICON_SCALE, DIALOG_ICON_SCALE, DIALOG_ICON_COLOR);

  // time text
  int h = 0, m = 0;
  Screen::getTime(h, m);
  drawFontTextf(fontSmall, DIALOGBK_MENU_ITEM_TEXT_OFFSET_X + 600,
    DIALOG_ICON_TEXT_OFFSET_Y,
    DIALOG_ICON_COLOR, DIALOG_ICON_TEXT_SIZE, "%02d:%02d", h, m);
}

void Layer::menuCursorUpdate(unsigned int buttons, int max) {
  int* b = Screen::ctrlReps();
  if (b[User::controls.menuUp] == 1 || (b[User::controls.menuUp] > 10 && b[User::controls.menuUp] % 5 == 0)) {
    selItem--;
    if (selItem < 0) {
      selItem = max - 1;
    }
    skipChars = 0;
    maxSkipChars = -1;
  }
  if (b[User::controls.menuDown] == 1 || (b[User::controls.menuDown] > 10 && b[User::controls.menuDown] % 5 == 0)) {
    selItem++;
    if (selItem >= max) {
      selItem = 0;
    }
    skipChars = 0;
    maxSkipChars = -1;
  }
  if (b[User::controls.menuLeft] == 1 || (b[User::controls.menuLeft] > 10 && b[User::controls.menuLeft] % 5 == 0)) {
    skipChars--;
    if (skipChars < 0) {
      skipChars = 0;
    }
  }
  if (b[User::controls.menuRight] == 1 || (b[User::controls.menuRight] > 10 && b[User::controls.menuRight] % 5 == 0)) {
    skipChars++;
    if (maxSkipChars >= 0 && skipChars>maxSkipChars)
      skipChars = maxSkipChars;
  }
}

Layer::Layer() : topItem(0), selItem(0),skipChars(0),maxSkipChars(-1) {
}

Layer::~Layer() {
}

}