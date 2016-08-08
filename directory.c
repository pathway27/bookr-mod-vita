/* 
 *	Copyright (C) 2006 cooleyes
 *	eyes.cooleyes@gmail.com 
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pspkernel.h>
#include "fat.h"
#include "directory.h"

int open_ms0_directory(const char* dir, char* sdir, directory_item_struct** list) {
	int item_count;
	p_fat_info info;

	if ( strncmp(dir,"ms0:", 4) != 0 ){
	  /* only 'ms0:/...' file/dir is supported */
	  return -1;
	}
	if(*list != NULL){
		free((void *)(*list));
		*list = NULL;
	}
	u32 count = fat_readdir(dir, sdir, &info);
	if(count == INVALID)
		return 0;
	u32 i, cur_count = 0;
	for(i = 0; i < count; i ++) {
		if(cur_count % 256 == 0){
		  if(cur_count == 0){
		    if(*list == NULL){
		      *list = (directory_item_struct*)malloc(sizeof(directory_item_struct) * 256);
		    }
		  }
		  else{
		    *list = (directory_item_struct*)realloc(*list, sizeof(directory_item_struct) * (cur_count + 256));
		  }
		  if(*list == NULL){
		    free((void *)info);
		    return 0;
		  }
		}
		if(info[i].attr & FAT_FILEATTR_DIRECTORY){
			(*list)[cur_count].filetype = FS_DIRECTORY;
			strcpy((*list)[cur_count].shortname, info[i].filename);
			strcpy((*list)[cur_count].longname, info[i].longname);
		}
		else{
			if(info[i].filesize == 0)
				continue;
			(*list)[cur_count].filetype = FS_FILE;
			strcpy((*list)[cur_count].shortname, info[i].filename);
			strcpy((*list)[cur_count].longname, info[i].longname);
			(*list)[cur_count].filesize = info[i].filesize;
			(*list)[cur_count].cdate = info[i].cdate;
			(*list)[cur_count].ctime = info[i].ctime;
			(*list)[cur_count].mdate = info[i].mdate;
			(*list)[cur_count].mtime = info[i].mtime;
		}
		cur_count ++;
	}
	free((void *)info);
	item_count = cur_count;
	return item_count;
}
