/*
 * bkzoomin: zoom in to specified left and right borders.
 * Copyright (C) 2009
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
 
#include <string.h>
#include "fzscreen.h"

#include "bkzoomin.h"
#include "bkuser.h"

BKZoomIn::BKZoomIn(int l, int r,int re) : leftPos(l), rightPos(r) {
	if (leftPos < 0 ||leftPos > 479){
	  leftPos = 0;
	}
	if (rightPos < leftPos+1){
	  rightPos = leftPos+1;
	}
	if (rightPos > 479)
	  rightPos = 479;
	isLeftSelected = true;
	showUsage = false;
	ret = re;
}

BKZoomIn::~BKZoomIn() {
}

int BKZoomIn::getLeftPos(){
  return leftPos;
}

int BKZoomIn::getRightPos(){
  return rightPos;
}

int BKZoomIn::update(unsigned int buttons) {
	int* b = FZScreen::ctrlReps();
	if (b[BKUser::controls.menuLeft] == 1 || b[BKUser::controls.menuLeft] > 20) {
	  if(isLeftSelected){
	    if(leftPos > 0){
	      leftPos--;
	    }
	  }
	  else{
	    if(rightPos>leftPos+1){
	      rightPos--;
	    }
	  }
	  return BK_CMD_MARK_DIRTY;
	}
	if (b[BKUser::controls.menuRight] == 1 || b[BKUser::controls.menuRight] > 20) {
	  if(isLeftSelected){
	    if(leftPos+1 < rightPos){
	      leftPos++;
	    }
	  }
	  else{
	    if(rightPos < 479){
	      rightPos++;
	    }
	  }
	  return BK_CMD_MARK_DIRTY;
	}
	if (b[BKUser::controls.menuLTrigger] == 1 ) {
	  if(isLeftSelected){
	    leftPos -= 20;
	    if(leftPos < 0){
	      leftPos = 0;
	    }
	  }
	  else{
	    rightPos -= 20;
	    if(rightPos < leftPos+1){
	      rightPos = leftPos+1;
	    }
	  }
	  return BK_CMD_MARK_DIRTY;
	}
	if (b[BKUser::controls.menuRTrigger] == 1) {
	  if(isLeftSelected){
	    leftPos += 20;
	    if(leftPos+1 > rightPos){
	      leftPos = rightPos-1;
	    }
	  }
	  else{
	    rightPos += 20;
	    if(rightPos > 479){
	      rightPos = 479;
	    }
	  }
	  return BK_CMD_MARK_DIRTY;
	}
	
	if (b[BKUser::controls.alternate] == 1) {
	  isLeftSelected = ! isLeftSelected;
	  return BK_CMD_MARK_DIRTY;
	}
	
	if (b[BKUser::controls.select] == 1) {
	  return ret;
	}
	if (b[BKUser::controls.cancel] == 1) {
	  return BK_CMD_CLOSE_TOP_LAYER;
	}
	if (b[BKUser::controls.showToolbar] == 1) {
	  return BK_CMD_CLOSE_TOP_LAYER;
	}
	if (b[BKUser::controls.showMainMenu] == 1) {
	  return BK_CMD_INVOKE_MENU;
	}
	if (b[BKUser::controls.details] == 1) {
	  showUsage = ! showUsage;
	  return BK_CMD_MARK_DIRTY;
	}
	return 0;
}

void BKZoomIn::render() {

  texUI->bindForDisplay();
  FZScreen::ambientColor(0xbf7f7f7f);
  drawRect(0,0,leftPos,272,6,31,1);
  drawRect(rightPos,0,480-rightPos,272,6,31,1);

  texUI2->bindForDisplay();
  FZScreen::ambientColor(0xff0000ff);
  int apos = leftPos;
  if (!isLeftSelected){
    apos = rightPos;
  }
  drawImage(apos - BK_IMG_LARROW_XSIZE - 1, 130, BK_IMG_LARROW_XSIZE, BK_IMG_LARROW_YSIZE, BK_IMG_LARROW_X, BK_IMG_LARROW_Y);
  drawImage(apos + 1, 130, BK_IMG_RARROW_XSIZE, BK_IMG_RARROW_YSIZE, BK_IMG_RARROW_X, BK_IMG_RARROW_Y);    
  if(showUsage){
    int x1 = 5;
    int x2 = 142;
    int x3 = 340;
    int x4 = 430;

    texUI->bindForDisplay();
    // context label
    FZScreen::ambientColor(0xdf222222);
    drawTPill(2, 272 - 30, 480 - 10, 30, 6, 31, 1);
    // icons
    FZScreen::ambientColor(0xffffffff);
    drawImage(x1, 248, BK_IMG_LRARROWS_XSIZE, BK_IMG_LRARROWS_YSIZE, BK_IMG_LRARROWS_X, BK_IMG_LRARROWS_Y);
    drawImage(x2, 248, BK_IMG_TRIANGLE_XSIZE, BK_IMG_TRIANGLE_YSIZE, BK_IMG_TRIANGLE_X, BK_IMG_TRIANGLE_Y);
    
    
    switch (BKUser::controls.select) {
    case FZ_REPS_CROSS:
      drawImage(x4, 248, BK_IMG_CROSS_XSIZE, BK_IMG_CROSS_YSIZE, BK_IMG_CROSS_X, BK_IMG_CROSS_Y);
      drawImage(x3, 248, BK_IMG_CIRCLE_XSIZE, BK_IMG_CIRCLE_YSIZE, BK_IMG_CIRCLE_X, BK_IMG_CIRCLE_Y); 
      break;
    case FZ_REPS_CIRCLE:	
    default:
      drawImage(x4, 248, BK_IMG_CIRCLE_XSIZE, BK_IMG_CIRCLE_YSIZE, BK_IMG_CIRCLE_X, BK_IMG_CIRCLE_Y); 
      drawImage(x3, 248, BK_IMG_CROSS_XSIZE, BK_IMG_CROSS_YSIZE, BK_IMG_CROSS_X, BK_IMG_CROSS_Y);
      break;
    }
    
    fontBig->bindForDisplay();
    FZScreen::ambientColor(0xffdddddd);
    drawText("Move border", fontBig, x1 + 25, 248);
    drawText("Toggle left/right border", fontBig, x2 + 25, 248);
    drawText("Cancel", fontBig, x3 + 25, 248);
    drawText("OK", fontBig, x4 + 25, 248);
  }
}

BKZoomIn* BKZoomIn::create(int left, int right, int ret) {
  BKZoomIn* z = new BKZoomIn(left, right, ret);
  FZScreen::resetReps();
  return z;
}
