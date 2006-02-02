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
	Stripped down version of txt2pdbdoc for Bookr. You can find the original in:
	http://homepage.mac.com/pauljlucas/software/txt2pdbdoc/
*/

/* standard */
#ifdef PSP

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

/* types */
#ifdef	bool
#undef	bool
#endif
#define	bool		int

#ifdef	false
#undef	false
#endif
#define	false		0

#ifdef	true
#undef	true
#endif
#define	true		1

/* constants */
#define	BUFFER_SIZE	6000		/* big enough for uncompressed record */
#define	COMPRESSED	2
#define	COUNT_BITS	3		/* why this value?  I don't know */
#define	DISP_BITS	11		/* ditto */
#define	DOC_CREATOR	"REAd"
#define	DOC_TYPE	"TEXt"
#define	UNCOMPRESSED	1

#define	MOBI_CREATOR	"MOBI"
#define	MOBI_TYPE		"BOOK"

/* exit status codes */
enum {
	Exit_Success			= 0,
	Exit_Usage			= 1,
	Exit_No_Open_Source		= 2,
	Exit_No_Open_Dest		= 3,
	Exit_No_Read			= 4,
	Exit_No_Write			= 5,
	Exit_Not_Doc_File		= 6,
	Exit_Unknown_Compression	= 7
};

/* macros */
#define	NEW_BUFFER(b)	(b)->data = malloc( (b)->len = BUFFER_SIZE )

#define	GET_Word(f,n) \
	{ if ( fread( &n, 2, 1, f ) != 1 ) read_error(); n = ntohs(n); }

#define	GET_DWord(f,n) \
	{ if ( fread( &n, 4, 1, f ) != 1 ) read_error(); n = ntohl(n); }

#define	PUT_Word(f,n) \
	{  Word t = htons(n); if ( fwrite( &t, 2, 1, f ) != 1 ) write_error(); }

#define	PUT_DWord(f,n) \
	{ DWord t = htonl(n); if ( fwrite( &t, 4, 1, f ) != 1 ) write_error(); }

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

/*****************************************************************************
 *
 *	Globals
 *
 *****************************************************************************/

typedef struct {
	Byte	*data;
	unsigned len;
} buffer;

char const*	me;			/* executable name */

bool		binary_opt		= true;
bool		compress_opt		= true;
bool		no_check_doc_opt	= false;
bool		verbose_opt		= false;

void		compress( buffer* );
void		decode( char const *src_file_name, char const *dest_file_name );
void		encode( char const *document_name,
			char const *src_file_name, char const *dest_file_name
		);
Byte*		mem_find( Byte *t, int t_len, Byte *m, int m_len );
FILE*		open_src_file ( char const *file_name );
FILE*		open_dest_file( char const *file_name );
void		put_byte( buffer*, Byte, bool *space );
void		read_error();
void		remove_binary( buffer* );
void		uncompress( buffer* );
void		usage();
void		write_error();

#if 0
/*****************************************************************************
 *
 * SYNOPSIS
 */
	int main( int argc, char *argv[] )
/*
 * DESCRIPTION
 *
 *	Parse the command line, initialize, call other functions ... the
 *	usual things that are done in main().
 *
 * PARAMETERS
 *
 *	argc	The number of arguments.
 *
 *	argv	A vector of the arguments; argv[argc] is null.  Aside from
 *		the options below, the arguments are the names of the files.
 *
 * SEE ALSO
 *
 *	Brian W. Kernighan, Dennis M. Ritchie.  "The C Programming Language,
 *	2nd ed."  Addison-Wesley, Reading, MA.  pp. 114-118.
 *
 *****************************************************************************/
{
	extern char*	optarg;
	extern int	optind, opterr;

	bool		decode_opt = false;
	char const	opts[] = "bcdDvV";	/* command line options */
	int		opt;			/* option being processed */

	me = strrchr( argv[0], '/' );		/* determine base name... */
	me = me ? me + 1 : argv[0];		/* ...of executable */

	/********** Process command-line options *****************************/

	opterr = 1;
	while ( (opt = getopt( argc, argv, opts )) != EOF )
		switch ( opt ) {

			case 'b':
				binary_opt = false;
				break;

			case 'c':
				compress_opt = false;
				break;

			case 'd':
				decode_opt = true;
				break;

			case 'D':
				no_check_doc_opt = true;
				break;

			case 'v':
				verbose_opt = true;
				break;

			case 'V':
				printf( PACKAGE " " VERSION "\n" );
				exit( Exit_Success );

			case '?': /* Bad option. */
				usage();
		}
	argc -= optind, argv += optind;

	if ( decode_opt ) {
		if ( argc < 1 || argc > 2 ) usage();
		decode( argv[0], argc == 2 ? argv[1] : 0 );
	} else {
		if ( argc != 3 ) usage();
		encode( argv[0], argv[1], argv[2] );
	}

	exit( Exit_Success );
}
#endif

int palmdoc_is_palmdoc(char const *src_file_name) {
	FILE		*fin;
	DatabaseHdrType	header;

	fin = open_src_file( src_file_name );
	if (fin == NULL)
		return 0;

	if ( fread( &header, DatabaseHdrSize, 1, fin ) != 1 )
		read_error();
	if ( !no_check_doc_opt && (
	     strncmp( header.type,    DOC_TYPE,    sizeof header.type ) ||
	     strncmp( header.creator, DOC_CREATOR, sizeof header.creator )
	     //strncmp( header.type,    MOBI_TYPE,    sizeof header.type ) ||
	     //strncmp( header.creator, MOBI_CREATOR, sizeof header.creator )
	) ) {
		fclose(fin);
		return 0;
	}
	fclose(fin);
	return 1;
}

/*****************************************************************************
 *
 * SYNOPSIS
 */
	char* palmdoc_decode( char const *src_file_name, int* length)
/*
 * DESCRIPTION
 *
 *	Decode the source Doc file to a text file.
 *
 * PARAMETERS
 *
 *	src_file_name	The name of the Doc file.
 *
 *	dest_file_name	The name of the text file.  If null, text is sent to
 *			standard output.
 *
 *****************************************************************************/
{
	buffer		buf;
	int		compression, doc_size;
	DWord		file_size, offset, rec_size;
	FILE		*fin;//, *fout;
	DatabaseHdrType	header;
	int		num_records, rec_num;
	doc_record0	rec0;

	char *out, *p;

	/********** open files, read header, ensure source is a Doc file *****/

	fin = open_src_file( src_file_name );
	if (fin == NULL)
		return NULL;

	if ( fread( &header, DatabaseHdrSize, 1, fin ) != 1 )
		read_error();
	if ( !no_check_doc_opt && (
	     strncmp( header.type,    DOC_TYPE,    sizeof header.type ) ||
	     strncmp( header.creator, DOC_CREATOR, sizeof header.creator )
	     //strncmp( header.type,    MOBI_TYPE,    sizeof header.type ) ||
	     //strncmp( header.creator, MOBI_CREATOR, sizeof header.creator )

	) ) {
		printf("%s: %s is not a Doc file\n", me, src_file_name);
		fclose(fin);
		return NULL;
		//exit( Exit_Not_Doc_File );
	}

	num_records = ntohs( header.recordList.numRecords ) - 1; /* w/o rec 0 */

	//fout = dest_file_name ? open_dest_file( dest_file_name ) : stdout;

	/********** read record 0 ********************************************/

	SEEK_REC_ENTRY( fin, 0 );
	GET_DWord( fin, offset );		/* get offset of rec 0 */
	fseek( fin, offset, SEEK_SET );
	if ( fread( &rec0, sizeof rec0, 1, fin ) != 1 ) {
		fclose(fin);
		return NULL;
		//read_error();
	}

	compression = ntohs( rec0.version );
	if ( compression != COMPRESSED && compression != UNCOMPRESSED ) {
		printf("%s: error: unknown file compression type: %d\n", me, compression);
		fclose(fin);
		return NULL;
		//exit( Exit_Unknown_Compression );
	}
	doc_size = ntohl( rec0.doc_size );
	*length = doc_size;
	p = out = malloc(doc_size);

	/********* read Doc file record-by-record ****************************/

	fseek( fin, 0, SEEK_END );
	file_size = ftell( fin );

	if ( verbose_opt )
		fprintf( stderr, "%s: decoding \"%s\":", me, header.name );

	NEW_BUFFER( &buf );
	for ( rec_num = 1; rec_num <= num_records; ++rec_num ) {
		DWord next_offset;

		/* read the record offset */
		SEEK_REC_ENTRY( fin, rec_num );
		GET_DWord( fin, offset );

		/* read the next record offset to compute the record size */
		if ( rec_num < num_records ) {
			SEEK_REC_ENTRY( fin, rec_num + 1 );
			GET_DWord( fin, next_offset );
		} else
			next_offset = file_size;
		rec_size = next_offset - offset;

		/* read the record */
		fseek( fin, offset, SEEK_SET );
		buf.len = fread( buf.data, 1, rec_size, fin );
		if ( buf.len != rec_size )
			read_error();

		if ( compression == COMPRESSED )
			uncompress( &buf );
		memcpy(p, buf.data, buf.len);
		p += buf.len;

		//if ( fwrite( buf.data, buf.len, 1, fout ) != 1 )
		//	write_error();
		
		if ( verbose_opt )
			fprintf( stderr, " %d", num_records - rec_num );
	}
	free(buf.data);
	if ( verbose_opt )
		putc( '\n', stderr );

	fclose( fin );
	//fclose( fout );
	return out;
}

/*****************************************************************************
 *
 * SYNOPSIS
 */
	void put_byte( register buffer *b, Byte c, bool *space )
/*
 * DESCRIPTION
 *
 *	Put a byte into a buffer.
 *
 * PARAMETERS
 *
 *	b	The buffer to be affected.
 *
 *	c	The byte.
 *
 *	space	Is it a space?
 *
 *****************************************************************************/
{
	if ( *space ) {
		*space = false;
		/*
		** There is an outstanding space char: see if we can squeeze it
		** in with an ASCII char.
		*/
		if ( c >= 0x40 && c <= 0x7F ) {
			b->data[ b->len++ ] = c ^ 0x80;
			return;
		}
		b->data[ b->len++ ] = ' ';	/* couldn't squeeze it in */
	} else if ( c == ' ' ) {
		*space = true;
		return;
	}

	if ( (c >= 1 && c <= 8) || c >= 0x80 )
		b->data[ b->len++ ] = '\1';

	b->data[ b->len++ ] = c;
}

/*****************************************************************************
 *
 * SYNOPSIS
 */
	void remove_binary( register buffer *b )
/*
 * DESCRIPTION
 *
 *	Replace the given buffer with one that has had ASCII characters (0-8)
 *	removed and carriage-returns and form-feeds converted to newlines.
 *
 * PARAMETERS
 *
 *	b	The buffer to be affected.
 *
 *****************************************************************************/
{
	register Byte *const new_data = malloc( b->len );
	int i, j;

	for ( i = j = 0; i < b->len; ++i ) {
		if ( b->data[ i ] < 9 )		/* discard really low ASCII */
			continue;
		switch ( b->data[ i ] ) {

			case '\r':
				if ( i < b->len - 1 && b->data[ i+1 ] == '\n' )
					continue;	/* CR+LF -> LF */
				/* no break; */

			case '\f':
				new_data[ j ] = '\n';
				break;

			default:
				new_data[ j ] = b->data[ i ];
		}
		++j;
	}
	free( b->data );
	b->data = new_data;
	b->len = j;
}

/*****************************************************************************
 *
 * SYNOPSIS
 */
	void uncompress( register buffer *b )
/*
 * DESCRIPTION
 *
 *	Replace the given buffer with an uncompressed version of itself.
 *
 * PARAMETERS
 *
 *	b	The buffer to be uncompressed.
 *
 *****************************************************************************/
{
	Byte *const new_data = malloc( BUFFER_SIZE );
	int i, j;

	for ( i = j = 0; i < b->len; ) {
		register unsigned c = b->data[ i++ ];

		if ( c >= 1 && c <= 8 )
			while ( c-- )			/* copy 'c' bytes */
				new_data[ j++ ] = b->data[ i++ ];

		else if ( c <= 0x7F )			/* 0,09-7F = self */
			new_data[ j++ ] = c;

		else if ( c >= 0xC0 )			/* space + ASCII char */
			new_data[ j++ ] = ' ', new_data[ j++ ] = c ^ 0x80;

		else {					/* 80-BF = sequences */
			register int di, n;
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

/*****************************************************************************
 *
 *	Miscellaneous function(s)
 *
 *****************************************************************************/

/* replacement for strstr() that deals with 0's in the data */
Byte* mem_find( register Byte *t, int t_len, register Byte *m, int m_len ) {
	register int i;
	for ( i = t_len - m_len + 1; i > 0; --i, ++t )
		if ( *t == *m && !memcmp( t, m, m_len ) )
			return t;
	return 0;
}

FILE* open_src_file( char const *file_name ) {
	FILE *f = fopen( file_name, "rb" );
	if ( f ) return f;
	fprintf( stderr, "%s: can not open %s for input\n", me, file_name );
	//exit( Exit_No_Open_Source );
	return NULL;
}

FILE* open_dest_file( char const *file_name ) {
	FILE *f = fopen( file_name, "wb" );
	if ( f ) return f;
	fprintf( stderr, "%s: can not open %s for output\n", me, file_name );
	exit( Exit_No_Open_Dest );
}

void read_error() {
	fprintf( stderr, "FIX CALLS TO THIS!!! ----------------- %s: reading failed\n", me );
	exit( Exit_No_Read );
}

void write_error() {
	fprintf( stderr, "%s: writing failed\n", me );
	exit( Exit_No_Write );
}
