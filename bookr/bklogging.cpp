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

#include <ctime>
#include <cstdarg>
#include "bklogging.h"

FILE *BK_MAIN_LOG_FILE;
char TIME_BUFF[80];

void initLogging() {
#ifdef LOGGING_ENABLED
	BK_MAIN_LOG_FILE = fopen(MAIN_LOG_PATH, "a");
#endif
}

void log(int level, char *fmt, va_list args) {

#ifdef LOGGING_ENABLED
	if (level > LOG_LEVEL_THRESHOLD) {

		time_t currentTime;
		time(&currentTime);

		struct tm *currentTimeInfo = localtime(&currentTime);
		strftime(TIME_BUFF, 80, "%Y-%m-%dT%H:%M:%S%z", currentTimeInfo);

		if (level == LOG_LEVEL_DEBUG)
			fprintf(BK_MAIN_LOG_FILE, "DEBUG ");
		else if (level == LOG_LEVEL_ERROR)
			fprintf(BK_MAIN_LOG_FILE, "ERROR ");

		fprintf(BK_MAIN_LOG_FILE, TIME_BUFF);
		fprintf(BK_MAIN_LOG_FILE, ": ");
		vfprintf(BK_MAIN_LOG_FILE, fmt, args);
		fprintf(BK_MAIN_LOG_FILE, "\n");

		fflush(BK_MAIN_LOG_FILE);

	}
#endif
}

void logDebug(char *fmt, ...) {

#ifdef LOGGING_ENABLED
	if (LOG_LEVEL_DEBUG < LOG_LEVEL_THRESHOLD) {
		va_list args;
		va_start(args, fmt);
		log(LOG_LEVEL_DEBUG, fmt, args);
		va_end(args);
	}
#endif

}

void finalizeLogging() {
#ifdef LOGGING_ENABLED
	fclose(BK_MAIN_LOG_FILE);
#endif
}
