/*
 * Bookr: document reader for the Sony PSP 
 * Copyright (C) 2015 Giorgos Tzampanakis (giorgos.tzampanakis@gmail.com)
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

#ifndef BKLOGGING_H
#define BKLOGGING_H

#include <stdio.h>

//#define LOGGING_ENABLED
#define LOG_LEVEL_THRESHOLD -100

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_ERROR 50

#define MAIN_LOG_PATH "logs/log.log"

extern FILE* BK_MAIN_LOG_FILE;

void initLogging();
void finalizeLogging();
void logDebug(char *fmt, ...);

#endif
