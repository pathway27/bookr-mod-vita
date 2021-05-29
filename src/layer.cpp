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

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ui/colours.hpp"
#include "graphics/shader.hpp"
#include "resource_manager.hpp"
#include "layer.hpp"
#include "resource_manager.hpp"
#include "graphics/controls.hpp"

namespace bookr {

void Layer::load() {
  #ifdef DEBUG
    printf("bklayer load\n");
  #endif
}

void Layer::unload(){
  #ifdef DEBUG
    printf("bklayer unload\n");
  #endif
}

struct T32FV32F2D {
  float u,v;
  float x,y,z;
};

void Layer::drawLogo(bool loading, string text, bool error) {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  ResourceManager::getSpriteRenderer()->DrawSprite(
      ResourceManager::GetTexture("logo"),
      glm::vec2(380, 150),
      glm::vec2(128, 128));

  ResourceManager::getSpriteRenderer()->DrawQuad(
      glm::vec2(96, 494),
      glm::vec2(768, 40),
      0.0f, glm::vec4(105 / 255.0, 105 / 255.0, 105 / 255.0, 240 / 255.0));

  if (loading)
    ResourceManager::getTextRenderer()->RenderText(text, 350.0f, 524.0f, 1.0f, glm::vec3(255 / 255.0, 255 / 255.0, 255.0 / 255.0));
  else if (text.length() > 0 && !(error))
    ResourceManager::getTextRenderer()->RenderText(text, 350.0f, 524.0f, 1.0f, glm::vec3(255 / 255.0, 255 / 255.0, 255.0 / 255.0));
  else
  {
    if (error)
      ResourceManager::getTextRenderer()->RenderText(text, 350.0f, 524.0f, 1.0f, glm::vec3(255 / 255.0, 255 / 255.0, 255.0 / 255.0));
    else
      ResourceManager::getTextRenderer()->RenderText(text, 350.0f, 524.0f, 1.0f, glm::vec3(255 / 255.0, 255 / 255.0, 255.0 / 255.0));
  }
}

int Layer::textW(char* t, Font* font) {
  return 0;
}

#define DIALOG_OFFSET_X Screen::WIDTH * 0.1
#define DIALOG_OFFSET_Y Screen::HEIGHT * 0.09
#define DIALOG_WIDTH Screen::WIDTH * 0.8

#define DIALOG_ITEM_OFFSET_X Screen::WIDTH * 0.11
#define DIALOG_ITEM_WIDTH Screen::WIDTH * 0.78
#define DIALOG_ITEM_HEIGHT Screen::HEIGHT * 0.07

#define DIALOG_TITLE_OFFSET_Y Screen::HEIGHT * 0.12
#define DIALOG_TITLE_TEXT_OFFSET_X DIALOG_ITEM_OFFSET_X + Screen::WIDTH * 0.01
#define DIALOG_TITLE_TEXT_OFFSET_Y DIALOG_TITLE_OFFSET_Y + Screen::HEIGHT * 0.01

#define DIALOG_CONTEXT_OFFSET_Y Screen::HEIGHT - DIALOG_ITEM_HEIGHT

inline constexpr unsigned int LOGO_SIZE = 32;

void Layer::drawMenu(string& title, string& triangleLabel, vector<MenuItem>& items, string& upperBreadCrumb) {
  drawMenu(title, triangleLabel, items, false);
  drawText(Screen::WIDTH * 0.12, Screen::HEIGHT * 0.14, BLACK, 1.0f, upperBreadCrumb.c_str());
}

void Layer::drawDialogFrame(string& title, string& triangleLabel, string& circleLabel, int flags) {
  int scrY = 0;
  char *t = (char*)circleLabel.c_str();
  // int tw = textW(t, fontBig);

  // back
  drawRectangle(DIALOG_OFFSET_X, DIALOG_OFFSET_Y, DIALOG_WIDTH, Screen::HEIGHT, NIGHT_RIDER);

  // title
  drawRectangle(DIALOG_ITEM_OFFSET_X, DIALOG_TITLE_OFFSET_Y, DIALOG_ITEM_WIDTH, DIALOG_ITEM_HEIGHT, LIGHT_GREY);

  // context label
  drawRectangle(Screen::WIDTH * 0.11, Screen::HEIGHT * 0.91, Screen::WIDTH * 0.78, Screen::HEIGHT * 0.1, MORTAR);

  // circleLabel or other context
  drawText(Screen::WIDTH * 0.9 - 120, Screen::HEIGHT * 0.9 + 29, WHITE, 1.0f, t);

  switch (User::controls.select) {
    case FZ_REPS_CROSS:
      // drawText(Screen::WIDTH * 0.1 + 210, Screen::HEIGHT * 0.9 + 10, BLACK, 1.0f, "INSERT CROSS HERE");
      ResourceManager::getSpriteRenderer()->DrawSprite(ResourceManager::GetTexture("bk_cross_icon"),
        glm::vec2(Screen::WIDTH * 0.9 - 200, Screen::HEIGHT * 0.9 + 25),
        glm::vec2(LOGO_SIZE, LOGO_SIZE));
      break;
    case FZ_REPS_CIRCLE:
    default:
      // drawText(Screen::WIDTH * 0.1 + 210, Screen::HEIGHT * 0.9 + 10, BLACK, 1.0f, "INSERT CIRCLE HERE");
      ResourceManager::getSpriteRenderer()->DrawSprite(ResourceManager::GetTexture("bk_circle_icon"),
        glm::vec2(Screen::WIDTH * 0.9 - 200, Screen::HEIGHT * 0.9 + 25),
        glm::vec2(LOGO_SIZE, LOGO_SIZE));
      break;
  }

  // title
  drawText(Screen::WIDTH * 0.12, Screen::HEIGHT * 0.14, WHITE, 1.0f, title.c_str());

  // triangle colors
  // Screen::ambientColor(0xffcccccc);
  // circle or other context icon
  // if (flags & BK_MENU_ITEM_FLAG::USE_LR_ICON) {
  // 	drawImage(480 - tw - 65, 248 + scrY, BK_IMG_LRARROWS_XSIZE, BK_IMG_LRARROWS_YSIZE, BK_IMG_LRARROWS_X, BK_IMG_LRARROWS_Y);
  // } else {
  //   if(circleLabel.size() > 0) {
  // 	switch(User::controls.select) {
  // 	case FZ_REPS_CROSS:
  // 		drawImage(480 - tw - 65, 248 + scrY, BK_IMG_CROSS_XSIZE, BK_IMG_CROSS_YSIZE, BK_IMG_CROSS_X, BK_IMG_CROSS_Y);
  // 		break;
  // 	case FZ_REPS_CIRCLE:
  // 	default:
  // 		drawImage(480 - tw - 65, 248 + scrY, BK_IMG_CIRCLE_XSIZE, BK_IMG_CIRCLE_YSIZE, BK_IMG_CIRCLE_X, BK_IMG_CIRCLE_Y);
  // 		break;
  // 	}
  // 	// drawImage(480 - tw - 65, 248 + scrY, BK_IMG_CROSS_XSIZE, BK_IMG_CROSS_YSIZE, BK_IMG_CROSS_X, BK_IMG_CROSS_Y);
  //   }
  // }
  // if (triangleLabel.size() > 0 || flags & BK_MENU_ITEM_FLAG::OPTIONAL_TRIANGLE_LABEL) {
  // 	//drawImage(37, 248 + scrY, 20, 20, 107, 5);
  // 	// drawImage(37, 248 + scrY, BK_IMG_TRIANGLE_XSIZE, BK_IMG_TRIANGLE_YSIZE, BK_IMG_TRIANGLE_X, BK_IMG_TRIANGLE_Y);
  // 	drawImage(37, 248 + scrY, BK_IMG_TRIANGLE_XSIZE, BK_IMG_TRIANGLE_YSIZE, BK_IMG_TRIANGLE_X, BK_IMG_TRIANGLE_Y);
  // }
 
  // labels
  // Screen::ambientColor(0xffcccccc);
  // if (triangleLabel.size() > 0) {
  // 	drawText((char*)triangleLabel.c_str(), fontBig, 37 + 25, 248 + scrY);
  // }
  // if (circleLabel.size() > 0)
  //   drawText(t, fontBig, 480 - tw - 40, 248 + scrY);

  // icons
  // Screen::ambientColor(0xff000000);
  // drawImage(430, 30 + scrY, BK_IMG_TRIANGLE_XSIZE, BK_IMG_TRIANGLE_YSIZE, BK_IMG_TRIANGLE_X, BK_IMG_TRIANGLE_Y); tri!
  // drawImage(430, 29 + scrY, BK_IMG_CIRCLE_XSIZE, BK_IMG_CIRCLE_YSIZE, BK_IMG_CIRCLE_X, BK_IMG_CIRCLE_Y); // close handle
}

void Layer::drawMenu(string& title, string& triangleLabel, vector<MenuItem>& items, bool useUTFFont) {
  int maxItemNum = 8;
  int selPos = selItem - topItem;

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
  if (items[selItem].flags & BK_MENU_ITEM_FLAG::OPTIONAL_TRIANGLE_LABEL) {
    tl = items[selItem].triangleLabel;
  }
  #ifdef DEBUG_RENDER
    printf("drawmenu\n");
  #endif
  drawDialogFrame(title, tl, items[selItem].circleLabel, items[selItem].flags);

  // selected item
  int wSelBox = scrollbar ? DIALOG_ITEM_WIDTH - 50: DIALOG_ITEM_WIDTH;
  int intial = Screen::HEIGHT * 0.21;
  int size = Screen::HEIGHT * 0.07;
  
  drawRectangle(Screen::WIDTH * 0.11,
    intial + (selPos * size) - 1,
    Screen::WIDTH * 0.77,
    Screen::HEIGHT * 0.05, WHITE);

  // scrollbar
  if (scrollbar) {
    float barh = 8.0f / float(items.size());
    barh *= 73.0f;
    if (barh < 15.0f)
      barh = 15.0f;
    float trel = float(topItem) / float(items.size());
    trel *= 73.0f;

    drawRectangle(DIALOG_OFFSET_X + wSelBox + 20,
      intial + (Screen::HEIGHT * 0.009),
      40,
      DIALOG_CONTEXT_OFFSET_Y - 10 - 200 - 30,
      0xff555555);

    drawRectangle(DIALOG_OFFSET_X + wSelBox + 20,
      intial + (Screen::HEIGHT * 0.009) + int(trel),
      40,
      DIALOG_CONTEXT_OFFSET_Y - 10 - 200 - int(barh),
      0xffaaaaaa);
  }
  
  // contents
  for (int i = 0; i < maxItemNum; ++i) {
    if ((i + topItem) >= (int)(items.size()))
      break;

    unsigned int color = WHITE;

    if ((i + topItem) == selItem)
      color = BLACK;

    drawText(Screen::WIDTH * 0.12,
        intial + (Screen::HEIGHT * 0.01) + (i * size),
        color, 1.0f, items[i + topItem].label.c_str());
  }
}

void Layer::drawOutlinePrefix(string prefix, int x, int y, int w, int h, int ws){
  for (int i = 0;i<prefix.length();i++){
    switch (prefix[i]){
    case '0':
      break;
    case '1':
      drawRect(x+i*w+(w-ws)/2, y+(h-ws)/2,ws,1,6,31,1);
      drawRect(x+i*w+(w-ws)/2, y+(h+ws)/2-1,ws,1,6,31,1);

      drawRect(x+i*w+(w-ws)/2,y+(h-ws)/2,1,ws,6,31,1);
      drawRect(x+i*w+(w+ws)/2-1,y+(h-ws)/2,1,ws,6,31,1);

      drawRect(x+i*w+(w-ws)/2+2, y+h/2,ws-4,1,6,31,1);
      drawRect(x+i*w+w/2,y+(h-ws)/2+2,1,ws-4,6,31,1);
      break;
    case '2':
      drawRect(x+i*w+(w-ws)/2, y+(h-ws)/2,ws,1,6,31,1);
      drawRect(x+i*w+(w-ws)/2, y+(h+ws)/2-1,ws,1,6,31,1);

      drawRect(x+i*w+(w-ws)/2,y+(h-ws)/2,1,ws,6,31,1);
      drawRect(x+i*w+(w+ws)/2-1,y+(h-ws)/2,1,ws,6,31,1);

      drawRect(x+i*w+(w-ws)/2+2, y+h/2,ws-4,1,6,31,1);
      //drawRect(x+i*w+w/2,y+(h-ws)/2+2,1,ws-4,6,31,1);
      break;
    case '3':
      drawRect(x+i*w+w/2,y,1,(h+1)/2,6,31,1);
      drawRect(x+i*w+w/2,y+h/2,(w)/2, 1, 6,31,1);
      break;
    case '4':
      drawRect(x+i*w+w/2,y,1,(h),6,31,1);
      drawRect(x+i*w+w/2,y+h/2,(w)/2, 1, 6,31,1);
      break;
    case '5':
      drawRect(x+i*w+w/2,y,1,(h),6,31,1);
      break;
    case '6':
      drawRect(x+i*w+(w-ws)/2, y+(h-ws)/2,ws,1,6,31,1);
      drawRect(x+i*w+(w-ws)/2, y+(h+ws)/2-1,ws,1,6,31,1);

      drawRect(x+i*w+(w-ws)/2,y+(h-ws)/2,1,ws,6,31,1);
      drawRect(x+i*w+(w+ws)/2-1,y+(h-ws)/2,1,ws,6,31,1);
      break;
    }
  }
}

void Layer::drawOutline(string& title, string& triangleLabel, vector<OutlineItem>& items, bool useUTFFont) {
  int maxItemNum = 8;
  int selPos = selItem - topItem;
  int scrY = 0;
  Font* itemFont;
  bool hasOutline = true;
  //Screen::ambientColor(0xFF0000FF);
  //drawOutlinePrefix("1",0,0,20,20);

  // if (items.size()==0){
  //   string cl = "";
  //   string prefix = "";

  //   items.push_back(OutlineItem("<No Outlines>", cl, (void*)0, prefix , false));
  //   hasOutline = false;
  //   selItem = 0;
  //   topItem = 0;
  //   selPos = 0;
  // }

  if( items.size()== 1 && items[0].circleLabel == ""){
    hasOutline = false;
  }

  if (selPos < 0) {
    topItem += selPos;
    selPos = 0;
  }

  if (selPos > maxItemNum - 1) {
    topItem += selPos - maxItemNum + 1;
    selPos = maxItemNum - 1;
  }

  bool scrollbar = items.size() > maxItemNum;

  string tl;
  if (items[selItem].flags & BK_OUTLINE_ITEM_HAS_TRIANGLE_LABEL) {
    tl = triangleLabel;
  }
  drawDialogFrame(title, tl, items[selItem].circleLabel, items[selItem].flags);
  if (hasOutline){
    Screen::ambientColor(0xffcccccc);
    drawImage(190, 248, BK_IMG_SQUARE_XSIZE, BK_IMG_SQUARE_YSIZE, BK_IMG_SQUARE_X, BK_IMG_SQUARE_Y);
    // fontBig->bindForDisplay();
    //if (User::options.t_ignore_x)
    //  drawText(const_cast<char*>("Goto (ignore zoom&X)"), fontBig, 190+BK_IMG_SQUARE_XSIZE+8, 248);
    //else
    //  drawText(const_cast<char*>("Goto (ignore zoom)"), fontBig, 190+BK_IMG_SQUARE_XSIZE+8, 248);
  }
  int ITEMHEIGHT = 60;
  if(useUTFFont)
    ITEMHEIGHT = 62;
  Screen::ambientColor(0xffffffff);

  // selected item
  int wSelBox = scrollbar ? 480 - 46 - 10 - 24: 480 - 46 - 10;
  //drawPill(25, ITEMHEIGHT - 3 + scrY + selPos*itemFont->getLineHeight(), wSelBox, 19, 6, 31, 1);
  if (items[selItem].flags & BK_MENU_ITEM_FLAG::FOLDER) {
    Screen::ambientColor(0xff000000);
    //drawImage(40, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight(), 20, 20, 84, 52);
    //drawImage(40, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight(), BK_IMG_FOLDER_XSIZE, BK_IMG_FOLDER_YSIZE, BK_IMG_FOLDER_X, BK_IMG_FOLDER_Y);
  }

  // scrollbar
  if (scrollbar) {
    float barh = 1.0f * maxItemNum / float(items.size());
    barh *= 173.0f;
    if (barh < 15.0f)
      barh = 15.0f;
    float trel = float(topItem) / float(items.size());
    trel *= 173.0f;
    Screen::ambientColor(0xff555555);
    drawPill(436, 57, 12, 173, 6, 31, 1);
    Screen::ambientColor(0xffaaaaaa);
    drawPill(436, 57 + int(trel), 12, int(barh), 6, 31, 1);
  }

  // not support now
  // color rects items
  // for (int i = 0; i < maxItemNum; ++i) {
  // 	if ((ITEMHEIGHT + (i+1)*itemFont->getLineHeight()) > 250)
  // 		break;
  // 	if ((i + topItem) >= (int)(items.size()))
  // 		break;
  // 	if (items[i + topItem].flags & BK_MENU_ITEM_FLAG::COLOR_RECT) {
  // 		int tw = textW((char*)items[i + topItem].label.c_str(), itemFont);
  // 		Screen::ambientColor(items[i + topItem].bgcolor | 0xff000000);
  // 		drawRect(40 + 25 + tw + 10, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY, 30, 15, 6, 31, 1);
  // 		Screen::ambientColor(items[i + topItem].fgcolor | 0xff000000);
  // 		drawRect(40 + 25 + tw + 15, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY + 4, 30, 15, 6, 31, 1);
  // 	}
  // }

  // itemFont->bindForDisplay();

  Screen::ambientColor(0xffffffff);
  // contents
  int yoff = 3;
  int x_left = 40;
  int text_right = 415;
  int mark_width = 11;
  int text_left;
  for (int i = 0; i < maxItemNum; ++i) {
    if (i + topItem == selItem)
      continue;
    /*if ((ITEMHEIGHT + (i+1)*itemFont->getLineHeight()) > 250)
      break;
    if ((i + topItem) >= (int)(items.size()))
      break;*/

    //drawOutlinePrefix(items[i + topItem].prefix, x_left, ITEMHEIGHT + i * itemFont->getLineHeight() + scrY, mark_width, itemFont->getLineHeight(),9);

    text_left = x_left +  mark_width * items[i + topItem].prefix.length();
    //if(useUTFFont){
    //  //int tooLong = drawUTFMenuItem(&(items[i + topItem]), itemFont, text_left, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY + yoff, 0, text_right - text_left);
    //  if(tooLong){
    //    //drawUTFText("...", itemFont, 416, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY + yoff, 0, 480);
    //    texUI->bindForDisplay();
    //    //drawImage(text_right, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY + yoff,12,12,7,112);
    //  }
    //}
    //else{
    //  //drawText((char*)items[i + topItem].label.c_str(), itemFont, text_left, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY);
    //}
  }
  Screen::ambientColor(0xff000000);
  //drawOutlinePrefix(items[selItem].prefix, x_left, ITEMHEIGHT + selPos * itemFont->getLineHeight() + scrY, mark_width, itemFont->getLineHeight(),9);
  text_left = x_left +  mark_width * items[selItem].prefix.length();

  //if(useUTFFont){
  //  int tooLong;
  //  if(skipChars==0)
  //    //tooLong = drawUTFMenuItem(&(items[selItem]), itemFont, text_left, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff, 0, text_right-text_left);
  //  else {
  //    //drawUTFText("...", itemFont, 40+25, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff, 0, 480);
  //    texUI->bindForDisplay();
  //    //drawImage(text_left,ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff,12,12,7,112);
  //    //tooLong = drawUTFMenuItem(&(items[selItem]), itemFont, text_left + 14, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff, skipChars*10+14, text_right-text_left-14);
  //  }
  //  if(tooLong){
  //    texUI->bindForDisplay();
  //    //drawImage(text_right, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff, 12,12,7,112);
  //  }
  //  else{
  //    maxSkipChars = skipChars;
  //  }
  //  itemFont->doneUTFFont();
  //}
  //else
    //drawText((char*)items[selItem].label.c_str(), itemFont, text_left, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight());

  // if(!hasOutline)
  //   items.clear();
}

#define CLOCK_X_OFFSET Screen::WIDTH * 0.45
#define CLOCK_Y_OFFSET Screen::HEIGHT * 0.86

void Layer::drawClockAndBattery(string& extra) {

  ResourceManager::getSpriteRenderer()->DrawSprite(
      ResourceManager::GetTexture("bk_memory_icon"),
      glm::vec2(CLOCK_X_OFFSET + 100, CLOCK_Y_OFFSET - 5),
      glm::vec2(LOGO_SIZE, LOGO_SIZE));

  ResourceManager::getSpriteRenderer()->DrawSprite(
      ResourceManager::GetTexture("bk_battery_icon"),
      glm::vec2(CLOCK_X_OFFSET + 250, CLOCK_Y_OFFSET - 5),
      glm::vec2(LOGO_SIZE, LOGO_SIZE));

  ResourceManager::getSpriteRenderer()->DrawSprite(
      ResourceManager::GetTexture("bk_clock_icon"),
      glm::vec2(CLOCK_X_OFFSET + 400, CLOCK_Y_OFFSET - 5),
      glm::vec2(LOGO_SIZE, LOGO_SIZE));

  int h = 0, m = 0;
  Screen::getTime(h, m);
  int b = Screen::getBattery();
  int mem = Screen::getUsedMemory();
  int speed = Screen::getSpeed();
  char t1[20];
  snprintf(t1, 20, "%02d:%02d", h, m);
  char t2[20];
  snprintf(t2, 20, "%d%%", b);
  char t3[20];
  snprintf(t3, 20, "%.1fM", ((float)(mem)) / (1024.0f*1024.0f));
  char t4[20];
  snprintf(t4, 20, "%dMHz", speed);

  drawText(CLOCK_X_OFFSET, CLOCK_Y_OFFSET, SILVER, 1.0f, t4);
  drawText(CLOCK_X_OFFSET + 150, CLOCK_Y_OFFSET, SILVER, 1.0f, t3);
  drawText(CLOCK_X_OFFSET + 300, CLOCK_Y_OFFSET, SILVER, 1.0f, t2);
  drawText(CLOCK_X_OFFSET + 450, CLOCK_Y_OFFSET, SILVER, 1.0f, t1);
  drawText(CLOCK_X_OFFSET + 380, CLOCK_Y_OFFSET - 40, SILVER, 1.0f, extra.c_str());
}

static glm::vec3 colorToRGB(unsigned int c) {
  float red = (float)((c & 0x00ff0000) >> 16);
  float green = (float)((c & 0x0000ff00) >> 8);
  float blue = (float)(c & 0x000000ff);
  return glm::vec3(red, green, blue);
}

static glm::vec4 colorToRGBA(unsigned int c) {
  glm::vec3 rgb = colorToRGB(c);
  float alpha = (float)((c & 0xff000000) >> 24);
  return glm::vec4(rgb, alpha);
}

void Layer::drawText(int x, int y, unsigned int color, float scale, const char *text) {
  // TODO: Do something about this.
  string str(text);
  ResourceManager::getTextRenderer()->RenderText(str, x, y, 1.0f, (colorToRGB(color)));
}

void Layer::drawRectangle(float x, float y, float w, float h, unsigned int color) {
  ResourceManager::getSpriteRenderer()->DrawQuad(
      glm::vec2(x, y),
      glm::vec2(w, h),
      0.0f, colorToRGBA(color) / 255.0f);
}

}
