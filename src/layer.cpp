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
#ifdef __vita__
  #include <vita2d.h>
  #include <psp2/kernel/threadmgr.h>
#elif MAC
  #include <SOIL.h>
  #include "graphics/shaders/shader.h"
#endif

#include "layer.hpp"

namespace bookr {

Font* Layer::fontBig = 0;
Font* Layer::fontSmall = 0;
Font* Layer::fontUTF = 0;
Texture* Layer::texUI = 0;
Texture* Layer::texUI2 = 0;
Texture* Layer::texLogo = 0;

extern "C" {
  extern unsigned int size_res_logo;
  extern unsigned char res_logo[];
  extern unsigned int size_res_uitex;
  extern unsigned char res_uitex[];
  extern unsigned int size_res_uitex2;
  extern unsigned char res_uitex2[];
  extern unsigned int size_res_uifont;
  extern unsigned char res_uifont[];
  extern unsigned char pdf_font_DroidSansFallback_ttf_buf[];
  extern unsigned int  pdf_font_DroidSansFallback_ttf_len;
  extern unsigned char _binary_icon0_t_png_start;
  extern unsigned int _binary_icon0_t_png_size;

  extern unsigned char fz_resources_fonts_urw_NimbusSans_Regular_cff[];
  extern unsigned int fz_resources_fonts_urw_NimbusSans_Regular_cff_size;
};

void Layer::load() {
  #ifdef __vita__
    #ifdef DEBUG
      printf("bklayer load\n");
    #endif
    texLogo = Texture::createFromVitaTexture(vita2d_load_PNG_buffer(&_binary_icon0_t_png_start));

    if (!fontBig){
      fontBig = Font::createFromMemory(fz_resources_fonts_urw_NimbusSans_Regular_cff, fz_resources_fonts_urw_NimbusSans_Regular_cff_size, 
        14, false);
    }
  #elif defined(MAC) || defined(WIN32)
    texLogo = Texture::createFromSOIL("sce_sys/icon0_t.png");
  #elif defined(PSP)

    // if (!fontUTF){
    //   fontUTF = Font::createUTFFromFile("utf.font",14,false);
    //   if(!fontUTF)
    //     if(pdf_font_DroidSansFallback_ttf_len)
    // fontUTF = Font::createUTFFromMemory(pdf_font_DroidSansFallback_ttf_buf, pdf_font_DroidSansFallback_ttf_len, 14, false);
    //     else
    // fontUTF = Font::createUTFFromFile("data.fnt",14,false);
    //   if(fontUTF){
    //     fontUTF->texEnv(FZ_TEX_MODULATE);
    //     fontUTF->filter(FZ_NEAREST, FZ_NEAREST);
    //   }
    // }
    // if (!fontSmall){
    // fontSmall = Font::createFromMemory(res_uifont, size_res_uifont, 11, false);
    // fontSmall->texEnv(FZ_TEX_MODULATE);
    // fontSmall->filter(FZ_NEAREST, FZ_NEAREST);
    // }
    // if (!texUI){
    // FZInputStreamMem* ins = FZInputStreamMem::create((char*)res_uitex, size_res_uitex);
    // FZImage* image = FZImage::createFromPNG(ins);
    // ins->release();
    // ins = 0;
    // texUI = Texture::createFromImage(image, false);
    // texUI->texEnv(FZ_TEX_MODULATE);
    // texUI->filter(FZ_NEAREST, FZ_NEAREST);
    // image->release();
    // }
    // if (!texUI2){
    // FZInputStreamMem* ins = FZInputStreamMem::create((char*)res_uitex2, size_res_uitex2);
    // FZImage* image = FZImage::createFromPNG(ins);
    // ins->release();
    // ins = 0;
    // texUI2 = Texture::createFromImage(image, false);
    // texUI2->texEnv(FZ_TEX_MODULATE);
    // texUI2->filter(FZ_NEAREST, FZ_NEAREST);
    // image->release();
    // }
  #endif

}

void Layer::unload(){
}

struct T32FV32F2D {
  float u,v;
  float x,y,z;
};

void Layer::drawImage(int x, int y) {
  // shaders["texture"].Use();
  // // vertex
  // glm::mat4 model;
  //   model = glm::translate(model, glm::vec3(x, y, 0.0f));

  //   this->shader.SetMatrix4("model", model);
  // glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, GL_FALSE, glm::value_ptr(model));

  // glBindVertexArray(VAOs["texture"]);
  //   	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  //   glBindVertexArray(0);
}

void Layer::drawImage(int x, int y, int w, int h, int tx, int ty) {
// #if defined(MAC) || defined(WIN32)
//   Shader ourShader("src/graphics/shaders/textures.vert",
//                    "src/graphics/shaders/textures.frag");

//   GLfloat vertices[] = {
//     // Positions          // Colors           // Texture Coords
//     1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Top Right
//     1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Bottom Right
//     -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Bottom Left
//     -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Top Left
//   };
//   GLuint indices[] = {  // Note that we start from 0!
//       0, 1, 3, // First Triangle
//       1, 2, 3  // Second Triangle
//   };
//   GLuint VBO, VAO, EBO;
//   glGenVertexArrays(1, &VAO);
//   glGenBuffers(1, &VBO);
//   glGenBuffers(1, &EBO);

//   glBindVertexArray(VAO);
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

//     // Position attribute
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
//     glEnableVertexAttribArray(0);
//     // Color attribute
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
//     glEnableVertexAttribArray(1);
//     // TexCoord attribute
//     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
//     glEnableVertexAttribArray(2);
//   glBindVertexArray(0); // Unbind VAO


//   // glGenTextures(1, &texture);
//   // glBindTexture(GL_TEXTURE_2D, texture);
//   //   //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//   //   //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//   //   //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//   //   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//   //   unsigned char* image = SOIL_load_image("sce_sys/icon0.png", &width, &height, 0, SOIL_LOAD_RGB);
//   //   glClearColor(0.0, 0.0, 0.0, 0.0);
//   //   glClear(GL_COLOR_BUFFER_BIT);

//   //   cout << SOIL_last_result() << endl;
//   //   cout << "null: " << !image << endl;
//   //   cout << "Max size: " << GL_MAX_TEXTURE_SIZE << endl;
//   //   cout << "Width: " <<  width << endl;
//   //   cout << "Height: " << height << endl;
//   //   cout << "Obj: " << texture << endl;

//   //   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//   //   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
//   //   GL_RGB, GL_UNSIGNED_BYTE, image);
//   //   glGenerateMipmap(GL_TEXTURE_2D);

//   //   SOIL_free_image_data(image);
//   // glBindTexture(GL_TEXTURE_2D, 0);

//   glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//   glClear(GL_COLOR_BUFFER_BIT);

//   ourShader.Use();
//   // glBindTexture(GL_TEXTURE_2D, texture);

//   glBindVertexArray(VAO);
//     glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//   glBindVertexArray(0);
// #endif
}

void Layer::drawImageScale(int x, int y, int w, int h, int tx, int ty, int tw, int th) {

}

void Layer::drawPill(int x, int y, int w, int h, int r, int tx, int ty) {

}

void Layer::drawTPill(int x, int y, int w, int h, int r, int tx, int ty) {
  
}

void Layer::drawRect(int x, int y, int w, int h, int r, int tx, int ty) {
  
}

int Layer::textWidthRange(char* t, int n, Font* font) {
  return 0;
}

int Layer::textW(char* t, Font* font) {
  return 0;
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

void Layer::drawDialogFrame(string& title, string& triangleLabel, string& circleLabel, int flags) {
  int scrY = 0;
  char *t =(char*)circleLabel.c_str();
  // int tw = textW(t, fontBig);

  #ifdef __vita__
    #ifdef DEBUG_RENDER
      printf("draw dialog frame\n");
    #endif
    // 960
    // 920
    // 544
    // backs
    vita2d_draw_rectangle(96, 40, 768, 504, RGBA8(34, 34, 34, 200)); // my cheapo drawTPill

    //title
    vita2d_draw_rectangle(106, 50, 748, 50, RGBA8(170, 170, 170, 255));

    //context label
    vita2d_draw_rectangle(106, 494, 748, 50, RGBA8(85, 85, 85, 255));

    //icons
    //(0, 0, 0, 255)
    //vita2d_draw_texture(genLogo->vita_texture, 0, 0);

    //circle or other context
    // (204, 204, 204, 255)

    //title
    // (255, 255, 255, 255)
    Screen::setTextSize(20.0f, 20.0f);
    Screen::drawText(116, 88, RGBA8(255, 255, 255, 255), 1.0f, title.c_str());

    //labels
    // if triangle/circle
  #endif

  // texUI->bindForDisplay();
  // // back
  // Screen::ambientColor(0xf0222222);
  // drawTPill(20, 20 + scrY, 480 - 46, 272, 6, 31, 1);
  // // title
  // Screen::ambientColor(0xffaaaaaa);
  // drawPill(25, 25 + scrY, 480 - 46 - 10, 20, 6, 31, 1);
  // // context label
  // Screen::ambientColor(0xff555555);
  // drawTPill(25, 272 - 30 + scrY, 480 - 46 - 11, 30, 6, 31, 1);
  // // icons
  // Screen::ambientColor(0xff000000);
  // // drawImage(430, 30 + scrY, BK_IMG_TRIANGLE_XSIZE, BK_IMG_TRIANGLE_YSIZE, BK_IMG_TRIANGLE_X, BK_IMG_TRIANGLE_Y); tri!
  // // drawImage(430, 29 + scrY, BK_IMG_CIRCLE_XSIZE, BK_IMG_CIRCLE_YSIZE, BK_IMG_CIRCLE_X, BK_IMG_CIRCLE_Y); // close handle
  // switch (User::controls.select) {
  // case FZ_REPS_CROSS:
  // 	drawImage(430, 29 + scrY, BK_IMG_CIRCLE_XSIZE, BK_IMG_CIRCLE_YSIZE, BK_IMG_CIRCLE_X, BK_IMG_CIRCLE_Y);
  // 	break;
  // case FZ_REPS_CIRCLE:
  // default:
  // 	drawImage(430, 29 + scrY, BK_IMG_CROSS_XSIZE, BK_IMG_CROSS_YSIZE, BK_IMG_CROSS_X, BK_IMG_CROSS_Y);
  // 	break;
  // }

  // Screen::ambientColor(0xffcccccc);
  // // circle or other context icon
  // if (flags & BK_MENU_ITEM_USE_LR_ICON) {
  // 	drawImage(480 - tw - 65, 248 + scrY, BK_IMG_LRARROWS_XSIZE, BK_IMG_LRARROWS_YSIZE, BK_IMG_LRARROWS_X, BK_IMG_LRARROWS_Y);
  // } else {
  //   if(circleLabel.size() > 0){
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
  // if (triangleLabel.size() > 0 || flags & BK_MENU_ITEM_OPTIONAL_TRIANGLE_LABEL) {
  // 	//drawImage(37, 248 + scrY, 20, 20, 107, 5);
  // 	// drawImage(37, 248 + scrY, BK_IMG_TRIANGLE_XSIZE, BK_IMG_TRIANGLE_YSIZE, BK_IMG_TRIANGLE_X, BK_IMG_TRIANGLE_Y);
  // 	drawImage(37, 248 + scrY, BK_IMG_TRIANGLE_XSIZE, BK_IMG_TRIANGLE_YSIZE, BK_IMG_TRIANGLE_X, BK_IMG_TRIANGLE_Y);
  // }

  // fontBig->bindForDisplay();

  // // title
  // Screen::ambientColor(0xffffffff);
  // drawText((char*)title.c_str(), fontBig, 31, 28 + scrY);
  // // labels
  // Screen::ambientColor(0xffcccccc);
  // if (triangleLabel.size() > 0) {
  // 	drawText((char*)triangleLabel.c_str(), fontBig, 37 + 25, 248 + scrY);
  // }
  // if (circleLabel.size() > 0)
  //   drawText(t, fontBig, 480 - tw - 40, 248 + scrY);
}

void Layer::drawMenu(string& title, string& triangleLabel, vector<MenuItem>& items) {
  drawMenu(title, triangleLabel, items, false);
}

void Layer::drawMenu(string& title, string& triangleLabel, vector<MenuItem>& items, string& upperBreadCrumb) {
  drawMenu(title, triangleLabel, items, false);
  #ifdef __vita__
    Screen::drawText(116, 71, RGBA8(255, 255, 255, 255), 1.0f, upperBreadCrumb.c_str());
  #endif
}

void Layer::drawMenu(string& title, string& triangleLabel, vector<MenuItem>& items, bool useUTFFont) {
  int maxItemNum = 8;
  int selPos = selItem - topItem;
  int scrY = 0;
  Font* itemFont;
  if(useUTFFont&&fontUTF){
    itemFont = fontUTF;
    maxItemNum = 9;
  }
  else{
    itemFont = fontBig;
    maxItemNum = 8;
    useUTFFont = false;
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

  string tl(triangleLabel);
  if (items[selItem].flags & BK_MENU_ITEM_OPTIONAL_TRIANGLE_LABEL) {
    tl = items[selItem].triangleLabel;
  }
  #ifdef DEBUG_RENDER
    printf("drawmenu\n");
  #endif
  drawDialogFrame(title, tl, items[selItem].circleLabel, items[selItem].flags);



  // texUI->bindForDisplay();
  // // folder icons
  #ifdef __vita__
    int ITEMHEIGHT = 128;
  #else
    int ITEMHEIGHT = 60;
  #endif
  /*
    if(useUTFFont)
      ITEMHEIGHT = 62;
    Screen::ambientColor(0xffffffff);
    for (int i = 0; i < maxItemNum; ++i) {
      if (i + topItem == selItem)
        continue;
      if ((ITEMHEIGHT + (i+1)*itemFont->getLineHeight()) > 250)
        break;
      if ((i + topItem) >= (int)(items.size()))
        break;
      if (items[i + topItem].flags & BK_MENU_ITEM_FOLDER) {
        //drawImage(40, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY, 20, 20, 84, 52);
        drawImage(40, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY, 20, 20, 58, 81);
      }
    }
  */
  // selected item
  #ifdef __vita__
    //Screen::drawText(116,    (ITEMHEIGHT + (i*15)) + scrY, RGBA8(255, 255, 255, 255), 1.0f, items[i + topItem].label.c_str());
    vita2d_draw_rectangle(106, (ITEMHEIGHT + ((selPos-1)*15) + scrY), 748, 15, RGBA8(170, 170, 170, 255));
  #elif defined(PSP)
    int wSelBox = scrollbar ? 480 - 46 - 10 - 24: 480 - 46 - 10;
    drawPill(25, ITEMHEIGHT - 3 + scrY + selPos*itemFont->getLineHeight(), wSelBox, 19, 6, 31, 1);
  #elif defined(MAC)

  #endif
  // if (items[selItem].flags & BK_MENU_ITEM_FOLDER) {
  // 	Screen::ambientColor(0xff000000);
  // 	//drawImage(40, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight(), 20, 20, 84, 52);
  // 	drawImage(40, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight(), BK_IMG_FOLDER_XSIZE, BK_IMG_FOLDER_YSIZE, BK_IMG_FOLDER_X, BK_IMG_FOLDER_Y);
  // }

  // // scrollbar
  // if (scrollbar) {
  // 	float barh = 1.0f * maxItemNum / float(items.size());
  // 	barh *= 173.0f;
  // 	if (barh < 15.0f)
  // 		barh = 15.0f;
  // 	float trel = float(topItem) / float(items.size());
  // 	trel *= 173.0f;
  // 	Screen::ambientColor(0xff555555);
  // 	drawPill(436, 57, 12, 173, 6, 31, 1);
  // 	Screen::ambientColor(0xffaaaaaa);
  // 	drawPill(436, 57 + int(trel), 12, int(barh), 6, 31, 1);
  // }

  // // color rects items
  // for (int i = 0; i < maxItemNum; ++i) {
  // 	if ((ITEMHEIGHT + (i+1)*itemFont->getLineHeight()) > 250)
  // 		break;
  // 	if ((i + topItem) >= (int)(items.size()))
  // 		break;
  // 	if (items[i + topItem].flags & BK_MENU_ITEM_COLOR_RECT) {
  // 		int tw = textW((char*)items[i + topItem].label.c_str(), itemFont);
  // 		Screen::ambientColor(items[i + topItem].bgcolor | 0xff000000);
  // 		drawRect(40 + 25 + tw + 10, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY, 30, 15, 6, 31, 1);
  // 		Screen::ambientColor(items[i + topItem].fgcolor | 0xff000000);
  // 		drawRect(40 + 25 + tw + 15, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY + 4, 30, 15, 6, 31, 1);
  // 	}
  // }

  // itemFont->bindForDisplay();

  // Screen::ambientColor(0xffffffff);
  // contents
  int yoff = 3;
  for (int i = 0; i < maxItemNum; ++i) {
    // if (i + topItem == selItem)
    // 	continue;
    // if ((ITEMHEIGHT + (i+1)*itemFont->getLineHeight()) > 250)
    // 	break;
    if ((i + topItem) >= (int)(items.size()))
      break;
    if(useUTFFont){
      // int tooLong = drawUTFMenuItem(&(items[i + topItem]), itemFont, 40 + 25, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY + yoff, 0, 350);
      // if(tooLong){
      //   //drawUTFText("...", itemFont, 416, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY + yoff, 0, 480);
      //   texUI->bindForDisplay();
      //   drawImage(415,ITEMHEIGHT + i*itemFont->getLineHeight() + scrY + yoff,12,12,7,112);
      // }
    }
    else {
      #ifdef __vita__
        Screen::drawText(116, (ITEMHEIGHT + (i*15)) + scrY, RGBA8(255, 255, 255, 255), 1.0f, items[i + topItem].label.c_str());
      #else
        //drawText((char*)items[i + topItem].label.c_str(), itemFont, 40 + 25, ITEMHEIGHT + i*itemFont->getLineHeight() + scrY);
      #endif
    }
  }
  // Screen::ambientColor(0xff000000);
  // if(useUTFFont){
  //   int tooLong;
  //   if(skipChars==0)
  //     tooLong = drawUTFMenuItem(&(items[selItem]), itemFont, 40 + 25, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff, 0, 350);
  //   else {
  //     //drawUTFText("...", itemFont, 40+25, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff, 0, 480);
  //     texUI->bindForDisplay();
  //     drawImage(40+25,ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff,12,12,7,112);
  //     tooLong = drawUTFMenuItem(&(items[selItem]), itemFont, 40 + 25 + 14, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff, skipChars*10+14, 336);
  //   }
  //   if(tooLong){
  //     texUI->bindForDisplay();
  //     drawImage(415, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight() + yoff, 12,12,7,112);
  //   }
  //   else{
  //     maxSkipChars = skipChars;
  //   }
  //   itemFont->doneUTFFont();
  // }
  // else
  //   drawText((char*)items[selItem].label.c_str(), itemFont, 40 + 25, ITEMHEIGHT + scrY + selPos*itemFont->getLineHeight());
}

void Layer::drawOutlinePrefix(string prefix, int x, int y, int w, int h, int ws){
  for (int i = 0;i<prefix.length();i++){
    texUI->bindForDisplay();
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
  if(useUTFFont&&fontUTF){
    itemFont = fontUTF;
    maxItemNum = 9;
  }
  else{
    itemFont = fontBig;
    maxItemNum = 8;
    useUTFFont = false;
  }

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
    texUI->bindForDisplay();
    Screen::ambientColor(0xffcccccc);
    drawImage(190, 248, BK_IMG_SQUARE_XSIZE, BK_IMG_SQUARE_YSIZE, BK_IMG_SQUARE_X, BK_IMG_SQUARE_Y);
    // fontBig->bindForDisplay();
    if (User::options.t_ignore_x)
      drawText(const_cast<char*>("Goto (ignore zoom&X)"), fontBig, 190+BK_IMG_SQUARE_XSIZE+8, 248);
    else
      drawText(const_cast<char*>("Goto (ignore zoom)"), fontBig, 190+BK_IMG_SQUARE_XSIZE+8, 248);
  }
  texUI->bindForDisplay();
  int ITEMHEIGHT = 60;
  if(useUTFFont)
    ITEMHEIGHT = 62;
  Screen::ambientColor(0xffffffff);

  // selected item
  int wSelBox = scrollbar ? 480 - 46 - 10 - 24: 480 - 46 - 10;
  //drawPill(25, ITEMHEIGHT - 3 + scrY + selPos*itemFont->getLineHeight(), wSelBox, 19, 6, 31, 1);
  if (items[selItem].flags & BK_MENU_ITEM_FOLDER) {
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
  // 	if (items[i + topItem].flags & BK_MENU_ITEM_COLOR_RECT) {
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
  //texUI->bindForDisplay();
  int l = countLines(text);
  #ifdef __vita__
    int h = 22 + (22*l);
    int y;
    if (h >= 544)
      y = 0;
    else
      y = (544 - h) / 2;
  #elif defined(PSP)
    int h = 25 + (25*l);
    int y = (272 - h) /2;
  #endif

  // back
  #ifdef __vita__
    vita2d_draw_rectangle(80, y, 960 - 156, h, bg1);
  #elif defined(PSP)
    Screen::ambientColor(bg1);
    drawPill(40, y, 480 - 86, h, 6, 31, 1);
  #endif

  // title
  #ifdef __vita__
    vita2d_draw_rectangle(90, 10 + y, 960 - 176, 30, bg2);
  #elif defined(PSP)
    Screen::ambientColor(bg2);
    drawPill(45, 5 + y, 480 - 96, 20, 6, 31, 1);
  #endif


  // // icons
  // Screen::ambientColor(bg1|0xff000000);
  // // drawImage(410, 9 + y, BK_IMG_CIRCLE_XSIZE, BK_IMG_CIRCLE_YSIZE, BK_IMG_CIRCLE_X, BK_IMG_CIRCLE_Y);
  // switch (User::controls.select) {
  // case FZ_REPS_CIRCLE:
  // 	drawImage(410, 9 + y, BK_IMG_CROSS_XSIZE, BK_IMG_CROSS_YSIZE, BK_IMG_CROSS_X, BK_IMG_CROSS_Y);
  // 	break;
  // case FZ_REPS_CROSS:
  // default:
  // 	drawImage(410, 9 + y, BK_IMG_CIRCLE_XSIZE, BK_IMG_CIRCLE_YSIZE, BK_IMG_CIRCLE_X, BK_IMG_CIRCLE_Y);
  // 	break;
  // }

  //fontBig->bindForDisplay();

  #ifdef __vita__
    Screen::drawText(102, y + 30, fg, 1.0f, title.c_str());
    Screen::drawText(102, y + 65, fg, 1.0f, text.c_str());
  #elif defined(PSP)
    Screen::ambientColor(fg);
    drawText((char*)title.c_str(), fontBig, 51, y + 9);
    drawText((char*)text.c_str(), fontBig, 51, y + 35);
  #endif
}

void Layer::drawClockAndBattery(string& extra) {
  texUI->bindForDisplay();
  Screen::ambientColor(0xffbbbbbb);
  drawImage(350, 226, BK_IMG_BATTERY_XSIZE, BK_IMG_BATTERY_YSIZE, BK_IMG_BATTERY_X, BK_IMG_BATTERY_Y);
  drawImage(405, 222, BK_IMG_CLOCK_XSIZE, BK_IMG_CLOCK_YSIZE, BK_IMG_CLOCK_X, BK_IMG_CLOCK_Y);
  drawImage(292, 224, BK_IMG_MEMORY_XSIZE, BK_IMG_MEMORY_YSIZE, BK_IMG_MEMORY_X, BK_IMG_MEMORY_Y);
  // fontSmall->bindForDisplay();
  Screen::ambientColor(0xffbbbbbb);
  int ew = textW((char*)extra.c_str(), fontSmall);
  drawText((char*)extra.c_str(), fontSmall, 480 - 30 - ew, 205);
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
  drawText(t1, fontSmall, 425, 224);
  drawText(t2, fontSmall, 370, 224);
  drawText(t3, fontSmall, 310, 224);
  drawText(t4, fontSmall, 240, 224);
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