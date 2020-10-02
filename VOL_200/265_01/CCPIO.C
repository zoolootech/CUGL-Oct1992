/*
 *  CCPIO.C
 *
 *  An Adaption of the UNIX CPIO Archiving Program
 *
 *  Author:  Jon C. Snader
 *
 *
 *  useage: ccpio -a|x|l archive_file [file(s)_to_be archived]
 *      where
 *              -a causes files_to_be_archived to be added to an
 *                 existing archive or put in a new archive
 *
 *              -x extracts all the files from archive_file
 *
 *              -l lists all the files and their sizes in archive_file
 *
 */

/* Preprocessor Control */

#define MSC5	1		/* Microsoft C ver. 5.0 */
#define MSC4	0		/* Microsoft C ver. 4.0 */
#define LAT3	0		/* Lattice C ver. 3.21 */
#define TURBO	0		/* Turbo C ver. 1.50 */
#define ECOC	0		/* Eco C ver. 3.23 */


#include <stdio.h>
#include <ctype.h>

#if (MSC4 | MSC5 | TURBO)
	#include <stdarg.h>
	#include <io.h>
#endif

#if (ECOC)
	#include <varargs.h>
	#define access(x,y)	!fopen (x, "rb")	/* if x file exsits,
								return 0 */
#endif

#if (ECOC | MSC4 | LAT3)
	#define const
#endif

#if (ECOC | LAT3)
	#define SEEK_CUR 1
#endif

#if (MSC4 | MSC5 | LAT3 | TURBO)
	#define TRUE ( 1 )
	#define FALSE ( 0 )
	#define phead "%06o%053o%06o%011lo%s%c" /*format string to write a header*/
#else
	#if (ECOC)
		#define phead "%06o%053o%06o%011lo%s"
	#endif
#endif

#define NULLFILE ( FILE * )( NULL )
#define EXISTS 0                        /*for access*/
#define N '\0'                          /*a null byte*/
#define MAGIC 070707                    /*magic number indicating cpio arch*/
#define TRAILER_SIZE ( 87L )            /*size of the trailer record*/
#define shead "%6o%*59c%11lo"           /*format string to read a header*/


typedef int bool;

static const char useage[] = "useage: ccpio -a|x|l archive file(s)";
FILE *ar;

/*---------------------------------------------------------------------
 *  error  --  output an error message and die
 *---------------------------------------------------------------------
 */

#if (MSC4 | MSC5 | TURBO)

void error( fmt )

char *fmt;

{
    va_list args;

    va_start( args, fmt );
	vfprintf( stderr, fmt, args );
    exit( 1 );

}
#endif

#if (LAT3)

void error( fmt)

char *fmt;

{
_pf (fputc, stderr, fmt, (int)(&fmt)+2);
exit(1);
}
#endif

#if (ECOC)

void error (va_alist)

va_dcl

{
    char *fmt;
    va_list args;

    va_start(args);
    fmt = va_arg (args, char *);
    vfprintf (stderr, fmt, args);
    exit (1);
}
#endif



/*---------------------------------------------------------------------
 *  openRD  --  open the archive file for reading
 *---------------------------------------------------------------------
 */

void openRD( archive )

char *archive;

{

    int magicno;

	if ( ( ar = fopen( archive, "rb" ) ) == NULLFILE )
        error( "ccpio: can not open %s\n", archive );

    if ( fscanf( ar, "%6o", &magicno ) != 1 || magicno != MAGIC )
		error( "ccpio: %s is not an archive file\n", archive );

}

/*--------------------------------------------------------------------
 *  openWR  --  open the archive file for writing
 *--------------------------------------------------------------------
 */

void openWR( archive )

char *archive;

{

    int magicno;

    if ( access( archive, EXISTS ) == 0 )
    {
        if ( ( ar = fopen( archive, "r+b" ) ) == NULLFILE )
	       	error( "ccpio:  Can not open %s\n", archive );
        if ( fscanf( ar, "%6o", &magicno ) != 1 || magicno != MAGIC )
   			error( "ccpio: %s is not an archive file\n", archive );

        /* position file just before trailer record */

        find( "TRAILER!!!" );
  		fseek( ar, -TRAILER_SIZE, SEEK_CUR );
    }
    else
    {
   		if ( ( ar = fopen( archive, "wb" ) ) == NULLFILE )
            error( "ccpio: can not create %s\n", archive );
    }

}

/*---------------------------------------------------------------------
 *  fill  --  fill out the file size to an even (512) block
 *---------------------------------------------------------------------
 */

void fill()

{

    /*
     * We need to fill out the last block so that the UNIX cpio can
     * read the archive file without errors.
     *
     */

    long size;
    int residue;

	fflush( ar );                           /*get the full count*/
#if (ECOC)
	fseek (ar, 0L, 2);		/* go to the end of file */
	size = ftell (ar);
	fseek (ar, 0L, 1);		/* go back to the current pos */
#else
	size = filelength( fileno( ar ) );
#endif

printf ("file size : %ld\n",size);

    residue = 512 - ( int )( size % 512 );
    if ( residue == 512 )                   /*already at block boundary*/
        return;
    while ( residue-- )
        fputc( '$', ar );

}

/*---------------------------------------------------------------------
 *  find  --  find a file in the archive
 *---------------------------------------------------------------------
 */

bool find( filen )

char *filen;

{
	char filespec[ 128 ];               /*same as UNIX cpio*/
	long cnt;                           /*size of the file*/
	char *fs;
	int magicno;

    rewind( ar );
	for ( ;; )
	{
		/* get the file size and name */

		if ( fscanf( ar, shead, &magicno, &cnt ) != 2 || magicno != MAGIC )
			error( "ccpio: corrupted header on archive file\n" );

		fs = filespec;
		do
			*fs = getc( ar );
		while ( *fs++ );

        if ( strcmp( filespec, filen ) == 0 )
            return TRUE;

        /*
         * even if the trailer record is missing, the header check will
         * stop the loop
         *
         */

		if ( strcmp( filespec, "TRAILER!!!" ) == 0 )
			return FALSE;
		fseek( ar, cnt, SEEK_CUR );
    }

}

/*---------------------------------------------------------------------
 *  list --  list the files in the archive
 *---------------------------------------------------------------------
 */

void list()

{

	char filespec[ 128 ];               /*same as UNIX cpio*/
	long cnt;                           /*size of the file*/
	char *fs;
	int magicno;

	rewind( ar );
	for ( ;; )
	{
		/* get the file size and name */

		if ( fscanf( ar, shead, &magicno, &cnt ) != 2 || magicno != MAGIC )
			error( "ccpio: corrupted header on archive file\n" );

		fs = filespec;
		do
			*fs = getc( ar );
		while ( *fs++ );

		if ( strcmp( filespec, "TRAILER!!!" ) == 0 )
			break;

		fprintf( stderr, "%s (%lu)\n", filespec, cnt );
		fseek( ar, cnt, SEEK_CUR );
	}

}

/*---------------------------------------------------------------------
 *  archive  --  add a file to the archive
 *---------------------------------------------------------------------
 */

void archive( filespec )

char *filespec;

{

	FILE *f;
	int c;
	long size;

    if ( ( f = fopen( filespec, "rb" ) ) == NULLFILE )
        error( "ccpio:  Can't open %s\n", filespec );

	/*put out the file heading*/

#if (ECOC)
	fseek (f, 0L, 2);		/* go to the end of file */
	size = ftell (f);
	fseek (f, 0L, 0);		/* go back to the current pos */
#else
	size = filelength( fileno( f ) );
#endif

printf ("file size : %ld\n", size);

	fprintf( ar, phead, MAGIC, 0, strlen( filespec ) + 1, size, filespec, N);
#if (ECOC)
	putc (N, ar);
#endif
	/*put out the file*/

	while ( ( c = getc( f ) ) != EOF )
		putc( c, ar);
	fclose( f );
}

/*---------------------------------------------------------------------
 *  extract  --  extract files from the archive
 *---------------------------------------------------------------------
 */

void extract()

{

	char filespec[ 128 ];               /*same as UNIX cpio*/
	long cnt;                           /*size of the file*/
	char *fs;
	int c;
	int magicno;
	FILE *f;

	rewind( ar );
	for ( ;; )
	{
		/* get the file size and name */

		if ( fscanf( ar, shead, &magicno, &cnt ) != 2 || magicno != MAGIC )
			error( "ccpio: corrupted header on archive file\n" );

		fs = filespec;
		do
			*fs = getc( ar );
		while ( *fs++ );

		if ( strcmp( filespec, "TRAILER!!!" ) == 0 )
			break;

		fprintf( stderr, "ccpio: extracting %s (%lu)\n", filespec, cnt );

		if ( ( f = fopen( filespec, "wb" ) ) == NULLFILE )
			error( "ccpio: can not create %s\n", filespec );

		while ( cnt-- && ( c = getc( ar ) ) != EOF  )
			 putc( c, f );

		fclose( f );

		if ( c == EOF )
			error( "ccpio: premature EOF on archive file\n" );

	}

}

/*---------------------------------------------------------------------
 *  main
 *---------------------------------------------------------------------
 */

void main( argc, argv )

int argc;
char **argv;

{

	int option;
    unsigned magicno;

	if ( **++argv != '-' )
		error( "ccpio: missing option %s\n%s\n", **argv, useage );
	option = ( *argv )[ 1 ];
	argc -= 2;



	switch( toupper( option ) )
	{
		case 'A' :                      /*archive*/
                    openWR( *++argv );
                    while ( --argc )
		    archive( *++argv );
		    fprintf( ar, phead, MAGIC, 0, 11, 0L, "TRAILER!!!", N );
#if (ECOC)
		    putc (N, ar);
#endif
                    fill();
		    break;

		case 'X' :                      /*extract*/
                    openRD( *++argv );
		    extract();
		    break;

		case 'L' :                      /*list*/
                    openRD( *++argv );
		    list();
		    break;

		default  :
		    error( "ccpio: Illegal option %c\n", option );
	}
	fclose( ar );
}
