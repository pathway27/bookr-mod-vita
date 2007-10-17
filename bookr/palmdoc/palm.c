/*
**	Text to Doc converter for Palm Pilots
**	txt2pdbdoc.c
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

/*
	Stripped down version of txt2pdbdoc for Bookr with added support
	for MobiPocket Reader type/creator. You can find the original at:
	http://homepage.mac.com/pauljlucas/software/txt2pdbdoc/
*/

#ifdef PSP
#define ntohs(x) __builtin_allegrex_wsbh(x)
#define ntohl(x) __builtin_allegrex_wsbw(x)
#else
#include <sys/types.h>			/* for FreeBSD */
#include <netinet/in.h>			/* for htonl, etc */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* local */
#include "palm.h"
#include "palmdoc.h"

/* constants */
#define	BUFFER_SIZE	6000		/* big enough for uncompressed record */
#define	COMPRESSED	2
#define	COUNT_BITS	3		/* why this value?  I don't know */
#define	DISP_BITS	11		/* ditto */
#define	UNCOMPRESSED	1

#define	DOC_TYPE		"TEXt"
#define	DOC_CREATOR		"REAd"
#define	MOBI_TYPE		"BOOK"
#define	MOBI_CREATOR	"MOBI"

/* macros */
#define	NEW_BUFFER(b)	(b)->data = malloc( (b)->len = BUFFER_SIZE )

#define	SEEK_REC_ENTRY(f,i) \
	fseek( f, DatabaseHdrSize + RecordEntrySize * (i), SEEK_SET )

/*****************************************************************************
 *
 * SYNOPSIS
 */
	struct doc_record0		/* 16 bytes total */
/*
 * DESCRIPTION
 *
 *	Record 0 of a Doc file contains information about the document as a
 *	whole.
 *
 *****************************************************************************/
{
	Word	version;		/* 1 = plain text, 2 = compressed */
	Word	reserved1;
	DWord	doc_size;		/* in bytes, when uncompressed */
	Word	num_records; 		/* PDB header numRecords - 1 */
	Word	rec_size;		/* usually RECORD_SIZE_MAX */
	DWord	reserved2;
} __attribute__ ((__packed__));
typedef struct doc_record0 doc_record0;

typedef struct {
	Byte	*data;
	unsigned len;
} buffer;

static void uncompress( buffer *b ) {
	Byte *const new_data = malloc( BUFFER_SIZE );
	int i, j;

	for ( i = j = 0; i < b->len; ) {
		unsigned c = b->data[ i++ ];

		if ( c >= 1 && c <= 8 )
			while ( c-- )			/* copy 'c' bytes */
				new_data[ j++ ] = b->data[ i++ ];

		else if ( c <= 0x7F )			/* 0,09-7F = self */
			new_data[ j++ ] = c;

		else if ( c >= 0xC0 )			/* space + ASCII char */
			new_data[ j++ ] = ' ', new_data[ j++ ] = c ^ 0x80;

		else {					/* 80-BF = sequences */
			int di, n;
			c = (c << 8) + b->data[ i++ ];
			di = (c & 0x3FFF) >> COUNT_BITS;
			for ( n = (c & ((1 << COUNT_BITS) - 1)) + 3; n--; ++j )
				new_data[ j ] = new_data[ j - di ];
		}
	}
	free( b->data );
	b->data = new_data;
	b->len = j;
}

int palmdoc_is_palmdoc(char const *src_file_name) {
	FILE		*fin;
	DatabaseHdrType	header;

	fin = fopen( src_file_name, "rb" );
	if (fin == NULL)
		return 0;

	if (fread( &header, DatabaseHdrSize, 1, fin ) != 1) {
		fclose(fin);
		return 0;
	}

	if (!(
	     strncmp( header.type,    DOC_TYPE,    sizeof header.type ) == 0 ||
	     strncmp( header.creator, DOC_CREATOR, sizeof header.creator ) == 0 ||
	     strncmp( header.type,    MOBI_TYPE,    sizeof header.type ) == 0 ||
	     strncmp( header.creator, MOBI_CREATOR, sizeof header.creator ) == 0
	)) {
		fclose(fin);
		return 0;
	}

	fclose(fin);
	return 1;
}

char* palmdoc_decode(char const *src_file_name, int* length, int* isMobi, char* title) {
	buffer		buf;
	int		compression, doc_size;
	DWord		file_size, offset, rec_size;
	FILE		*fin;
	DatabaseHdrType	header;
	int		num_records, rec_num;
	doc_record0	rec0;

	char *out, *p;

	/********** open files, read header, ensure source is a Doc file *****/

	fin = fopen( src_file_name, "rb" );
	if (fin == NULL)
		return NULL;

	if (fread( &header, DatabaseHdrSize, 1, fin ) != 1) {
		fclose(fin);
		return NULL;
	}

	if (!(
	     strncmp( header.type,    DOC_TYPE,    sizeof header.type ) == 0 ||
	     strncmp( header.creator, DOC_CREATOR, sizeof header.creator ) == 0 ||
	     strncmp( header.type,    MOBI_TYPE,    sizeof header.type ) == 0 ||
	     strncmp( header.creator, MOBI_CREATOR, sizeof header.creator ) == 0
	)) {
		printf("%s is not a Palm Doc file\n", src_file_name);
		fclose(fin);
		return NULL;
	}

	if (
		strncmp(header.type,    MOBI_TYPE,    sizeof header.type) == 0 ||
		strncmp(header.creator, MOBI_CREATOR, sizeof header.creator) == 0
	) {
		*isMobi = 1;
	} else {
		*isMobi = 0;
	}

	if( title )
		strncpy(title,header.name,dmDBNameLength);

	num_records = ntohs( header.recordList.numRecords ) - 1; /* w/o rec 0 */

	/********** read record 0 ********************************************/

	SEEK_REC_ENTRY( fin, 0 );
	if (fread(&offset, 4, 1, fin) != 1) { /* get offset of rec 0 */
		fclose(fin);
		return NULL;
	}
	offset = ntohl(offset);
	fseek( fin, offset, SEEK_SET );
	if ( fread( &rec0, sizeof rec0, 1, fin ) != 1 ) {
		fclose(fin);
		return NULL;
	}

	compression = ntohs( rec0.version );
	if ( compression != COMPRESSED && compression != UNCOMPRESSED ) {
		printf("error: unknown file compression type: %d\n", compression);
		fclose(fin);
		return NULL;
	}
	doc_size = ntohl( rec0.doc_size );
	*length = doc_size;
	p = out = malloc(doc_size);
	memset(out, 0, doc_size);

	/********* read Doc file record-by-record ****************************/

	fseek( fin, 0, SEEK_END );
	file_size = ftell( fin );

	NEW_BUFFER( &buf );
	int c = 0;
	for ( rec_num = 1; rec_num <= num_records; ++rec_num ) {
		DWord next_offset;

		/* read the record offset */
		SEEK_REC_ENTRY( fin, rec_num );
		if (fread(&offset, 4, 1, fin) != 1) {
			fclose(fin);
			free(out);
			return NULL;
		}
		offset = ntohl(offset);

		/* read the next record offset to compute the record size */
		if ( rec_num < num_records ) {
			SEEK_REC_ENTRY( fin, rec_num + 1 );
			if (fread(&next_offset, 4, 1, fin) != 1) {
				fclose(fin);
				free(out);
				return NULL;
			}
			next_offset = ntohl(next_offset);
		} else
			next_offset = file_size;
		rec_size = next_offset - offset;

		/* read the record */
		fseek( fin, offset, SEEK_SET );
		buf.len = fread( buf.data, 1, rec_size, fin );
		if ( buf.len != rec_size ) {
			fclose(fin);
			free(out);
			return NULL;
		}

		if ( compression == COMPRESSED )
			uncompress( &buf );
		c += buf.len;
		if (c <= doc_size) {
			memcpy(p, buf.data, buf.len);
		} else {
			break;
		}
		p += buf.len;
	}
	free(buf.data);
	fclose(fin);
	return out;
}

