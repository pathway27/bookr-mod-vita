/*
**	Text to Doc converter for Palm Pilots
**	palm.h
**
**	Copyright (C) 1998  Paul J. Lucas
**
**	This program is free software; you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation; either version 2 of the License, or
**	(at your option) any later version.
** 
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
** 
**	You should have received a copy of the GNU General Public License
**	along with this program; if not, write to the Free Software
**	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef	palm_H
#define	palm_H

/* standard */
#include <time.h>

/*****************************************************************************
 *
 *	Define integral type Byte, Word, and DWord to match those on the
 *	Pilot being 8, 16, and 32 bits, respectively.
 *
 *****************************************************************************/

typedef unsigned char Byte;

#define SIZEOF_UNSIGNED_SHORT 2
#define SIZEOF_UNSIGNED_INT 4

#if	SIZEOF_UNSIGNED_SHORT == 2
typedef unsigned short Word;
#else
#error machine does not seem to support a 16-bit integral type
#endif

#if	SIZEOF_UNSIGNED_INT == 4
typedef unsigned int DWord;
#elif	SIZE_OF_UNSIGNED_LONG == 4
typedef unsigned long DWord;
#else
#error machine does not seem to support a 32-bit integral type
#endif

/********** Other stuff ******************************************************/

#define	dmDBNameLength	32		/* 31 chars + 1 null terminator */
#define RECORD_SIZE_MAX	4096		/* Pilots have a max 4K record size */

#ifdef	HAVE_TIME_H
#define	palm_date()	(DWord)(time(0) + 2082844800ul)
#else
#define	palm_date()	0
#endif

/*****************************************************************************
 *
 * SYNOPSIS
 */
	struct RecordEntryType 
/*
 * DESCRIPTION
 *
 *	Every record has one of these headers.
 *
 * SEE ALSO
 *
 *	Christopher Bey and Kathleen Dupre.  "Palm File Format Specification,"
 *	Document Number 3008-003, Palm, Inc., May 16, 2000.
 *
 *****************************************************************************/
{
	DWord	localChunkID;		/* offset to where record starts */
	struct {
		unsigned delete   : 1;
		unsigned dirty    : 1;
		unsigned busy     : 1;
		unsigned secret   : 1;
		unsigned category : 4;
	} attributes;
	Byte	uniqueID[3];
} __attribute__ ((__packed__));
typedef struct RecordEntryType RecordEntryType;

/*
** Some compilers pad structures out to DWord boundaries so using sizeof()
** doesn't give the right result.
*/
#define	RecordEntrySize		8

/*****************************************************************************
 *
 * SYNOPSIS
 */
	struct RecordListType			/* 6 bytes total */
/*
 * DESCRIPTION
 *
 *	This is a PDB database header as currently defined by Palm, Inc.
 *
 * SEE ALSO
 *
 *	Ibid.
 *
 *****************************************************************************/
{
	DWord	nextRecordListID;
	Word	numRecords;
} __attribute__ ((__packed__));
typedef struct RecordListType RecordListType;

#define	RecordListSize		6

/*****************************************************************************
 *
 * SYNOPSIS
 */
	struct DatabaseHdrType	/* 78 bytes total */
/*
 * DESCRIPTION
 *
 *	This is a PDB database header as currently defined by Palm, Inc.
 *
 *****************************************************************************/
{
	char		name[ dmDBNameLength ];
	Word		attributes;
	Word		version;
	DWord		creationDate;
	DWord		modificationDate;
	DWord		lastBackupDate;
	DWord		modificationNumber;
	DWord		appInfoID;
	DWord		sortInfoID;
	char		type[4];
	char		creator[4];
	DWord		uniqueIDSeed;
	RecordListType	recordList;
} __attribute__ ((__packed__));
typedef struct DatabaseHdrType DatabaseHdrType;

#define DatabaseHdrSize		78

#endif	/* palm_H */
