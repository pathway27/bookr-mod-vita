/*
 * Bookr: document reader for the Sony PSP 
 * Copyright (C) 2005 Carlos Carrasco Martinez (carloscm at gmail dot com)
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

#include "bkoutline.h"
#include "bkuser.h"
#include "bkdocument.h"
#include "mupdf.h"

BKOutline::BKOutline(const char* t, int r) : title(t), ret(r) {
  menuDirty = true;
  outlines = 0;
  selected_outline = 0;
}

BKOutline::~BKOutline() {
}

void* BKOutline::getSelectedOutline() {
  return selected_outline;
}

void BKOutline::setOutlines(int type, void* o){
  outlineType = type;
  if (type == OUTLINE_PDF)
    outlines = (pdf_outline*) o ;
}

int BKOutline::update(unsigned int buttons) {
	menuCursorUpdate(buttons, (int)items.size());
	selected_outline = items[selItem].outline;
	bool hasOutline = true;
	if (items.size() == 1 && items[0].circleLabel == "")
	  hasOutline = false;
	int* b = FZScreen::ctrlReps();
	if (b[BKUser::controls.select] == 1 && hasOutline) {
	  // todo: return ret if selected outline is valid. else display a popup windows.
	  ignoreZoom = false;
	  return ret;

	}
	if (b[BKUser::controls.details] == 1 && hasOutline) {
	  // todo: return ret if selected outline is valid. else display a popup windows.
	  ignoreZoom = true;
	  return ret;

	}
	if (b[BKUser::controls.alternate] == 1) {
	  // alter open/closed state of the selected outline item if it has any children outlines. otherwise keep its closed.
	  if (selected_outline){
	    if(outlineType == OUTLINE_PDF)
	      ((pdf_outline*)selected_outline)->isOpen = ((pdf_outline*)selected_outline)->isOpen?0:1;
	    menuDirty = true;
	    return BK_CMD_MARK_DIRTY;
	  }
	}
	if (b[BKUser::controls.cancel] == 1) {
		return BK_CMD_CLOSE_TOP_LAYER;
	}
	if (b[BKUser::controls.showMainMenu] == 1) {
	  return BK_CMD_INVOKE_MENU;
 	}
	if (b[BKUser::controls.menuLTrigger] == 1 || b[BKUser::controls.menuRTrigger] == 1){
	  BKUser::options.t_ignore_x = !BKUser::options.t_ignore_x;
	  return BK_CMD_MARK_DIRTY;
	}
	
	return 0;
}

void
BKOutline::buildOutlineMenus(void* outline, string prefix, bool topLevel){
  if (outlineType == OUTLINE_PDF){
    pdf_outline* current = (pdf_outline*)outline;
    string cl("Goto");
    while (current){
      string label = prefix;
      if(!topLevel){
	if(current->next)
	  label += "4";
	else 
	  label += "3";
      }
      if (current->child)
	label += current->isOpen?"2":"1";
      else 
	label += "0";

      //fprintf(stderr,"adding item: %s\n",label.c_str());
      items.push_back(BKOutlineItem(current->title, cl, (void*)current, label, current->child));
      
      if (current->isOpen && current->child){
	buildOutlineMenus(current->child, prefix + (topLevel?"":current->next?"5":"0"), false);
	//buildOutlineMenus(current->child, prefix + "  ");
      }
      
      current = current->next;
    }
  }
}


void BKOutline::render() {
  if(menuDirty){
    menuDirty = false;
    //todo: built menu items on fly.
    items.clear();
    buildOutlineMenus(outlines,"",true);
    if (items.size()==0){
      string cl = "";
      string prefix = "";
      items.push_back(BKOutlineItem("<No Outlines>", cl, (void*)0,prefix , false));
    }
  }
  string tl("Expand/Collapse");
  drawOutline(title, tl, items, true);
}

BKOutline* BKOutline::create(const char* t, int r, int top, int sel) {
	BKOutline* o = new BKOutline(t, r);
	o->topItem = top;
	o->selItem = sel;
	FZScreen::resetReps();
	return o;
}

void BKOutline::getSelection(int& top, int& sel){
  top = topItem;
  sel = selItem;
}
