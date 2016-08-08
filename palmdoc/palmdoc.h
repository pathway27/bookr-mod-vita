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

#ifndef PALMDOC_H
#define PALMDOC_H

#ifdef __cplusplus
extern "C" {
#endif

extern char* palmdoc_decode(char const *src_file_name, int* length, int* isMobi, char* title);
extern int palmdoc_is_palmdoc(char const *src_file_name);
#define	dmDBNameLength	32		/* 31 chars + 1 null terminator */

#ifdef __cplusplus
}
#endif

#endif
