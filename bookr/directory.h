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
#ifndef __DIRECTORY_H__
#define __DIRECTORY_H__
#define MAX_FILENAME_LENGTH 1024
typedef enum {
	FS_DIRECTORY = 0,
	FS_FILE
} file_type_enum;

typedef struct {
	const char * ext;
	file_type_enum filetype;
} file_type_ext_struct;

typedef struct {
	char shortname[MAX_FILENAME_LENGTH];
	char longname[MAX_FILENAME_LENGTH];
	u32 filesize;
	u16 cdate;
	u16 ctime;
	u16 mdate;
	u16 mtime;
	file_type_enum filetype;
} directory_item_struct;

#ifdef __cplusplus
extern "C" {
#endif

int open_ms0_directory(const char* dir, char* sdir, directory_item_struct** list); 

#ifdef __cplusplus
}
#endif

#endif
