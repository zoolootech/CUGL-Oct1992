/*
 * cpmfile.c - a monolithic program to access vertual CP/M floppy
 */

#include <stdio.h>
#include <fcntl.h>

extern long lseek ();
extern char * memcpy ();
extern char * memset ();

#define DISK		"CPMDAT.DSK"

#define NOTUSED		0xE5
#define DIRMAX		64
#define MINBLOCK	2
#define MAXBLOCK	243

#define BINRD		"rb"
#define ASCRD		"rt"
#define BINWR		"wb"
#define ASCWR		"wt"

#define DSKREAD		( O_RDONLY | O_BINARY )
#define DSKWRIT		( O_WRONLY | O_BINARY )

#define MESSAGE(s,t)	fprintf( stderr, (s), (t) )
#define USGOUT()	fprintf( stderr, usfmt, usage ); return( 1 )
#define ERROUT(s,t)	fprintf( stderr, (s), (t) ); return( 1 )

#define TRUE		1
#define FALSE		0

char usage [] = "[-webvl][-d disk][-u user][-o file] filename ...";
char usfmt [] = "usage: cpmfile %s\n";

char dirbuf [ DIRMAX ][ 32 ];
char recbuf [ 128 ];
char block  [ MAXBLOCK ];

main ( argc, argv )
    int argc;
    char * argv [];
{
    int binary, verbos, wrmode, ermode, lister;
    char * disk, * file, * user;
    int uid;
    int argpos;
    FILE * fp;
    int dfd;
    char * dirptr;
    int reel, ext;
    int exist;
    int state;
    int i, n, c;
    char * u, * v, ** p;
    int r;
    char fcb [ 13 ];

    /* set options to default */
    binary = verbos = wrmode = ermode = lister = FALSE;
    file = NULL;
    disk = DISK;
    user = "";

    /* fetch options and record them */
    argpos = 1;
    while ( argpos < argc && argv[ argpos ][ 0 ] == '-' ) {
	switch ( argv[ argpos ][ 1 ] ) {
	case 'b': binary = TRUE; argpos++; continue;
	case 'v': verbos = TRUE; argpos++; continue;
	case 'w': wrmode = TRUE; argpos++; continue;
	case 'e': ermode = TRUE; argpos++; continue;
	case 'l': lister = TRUE; argpos++; continue;
	case 'd': p = &disk; break;
	case 'o': p = &file; break;
	case 'u': p = &user; break;
	default:
	    USGOUT();
	}
	if ( argv[ argpos ][ 2 ] != '\0' ) {
	    *p = &argv[ argpos ][ 2 ];
	    argpos++;
	} else if ( argpos + 1 < argc ) {
	    *p = argv[ argpos + 1 ];
	    argpos += 2;
	} else {
	    USGOUT();
	}
    }

    /* check consistency of options */
    if ( ( wrmode | ermode ) + lister + ( file != NULL ) > 1 ) {
	ERROUT( "-w, -e, -l and -o are excluseve (except -w and -e)\n", NULL );
    }

    /* get user id */
    uid = atoi( user );

    /* at least one file name must be present */
    if ( argpos >= argc && !lister ) {
	USGOUT();
    }

    /* open common output file when -o flag is present */
    if ( file != NULL ) {
	if ( strcmp( file, "-" ) == 0 ) {
	    fp = stdout;
	} else {
	    fp = fopen( file, binary ? BINWR : ASCWR );
	    if ( fp == NULL ) {
		ERROUT( "cannot open %s for output\n", file );
	    }
	}
    }

    /* open CP/M disk */
    dfd = open( disk, DSKREAD );
    if ( dfd < 0 ) {
	ERROUT( "cannot open disk file %s for input\n", disk );
    }

    /* load CP/M directory */
    if ( read( dfd, ( char * )dirbuf, 32 * DIRMAX ) < 32 * DIRMAX ) {
	ERROUT( "disk read error on directory\n", NULL );
    }

    /* display file names when listing */
    if ( lister ) {
	for ( i = 0; i < DIRMAX; i++ ) {
	    u = dirbuf[ i ];
	    if ( ( u[ 0 ] & 0xFF ) != NOTUSED && u[ 0 ] == 0 ) {
		if ( user[ 0 ] != '\0' && ( u[ 0 ] & 0xFF ) != uid ) continue;
		printf( "%2d: %8.8s.%3.3s\n", u[ 0 ] & 0xFF, u + 1, u + 9 );
	    }
	}
	return( 0 );
    }

    /* change access mode of disk when writing or erasing */
    if ( wrmode || ermode ) {
	close( dfd );
	dfd = open( disk, DSKWRIT );
	if ( dfd < 0 ) {
	    ERROUT( "cannot open disk file %s for output\n", disk );
	}
    }

    /* build used block table */
    for ( i = 0; i < DIRMAX; i++ ) {
	if ( ( dirbuf[ i ][ 0 ] & 0xFF ) != NOTUSED ) {
	    for ( n = 16; n < 32; n++ ) {
		block[ dirbuf[ i ][ n ] & 0xFF ] = TRUE;
	    }
	}
    }

    /* process each file */
    for ( ; argpos < argc; argpos++ ) {

	/* echo file name when -v flag is present */
	if ( verbos ) MESSAGE( "%s\n", argv[ argpos ] );

	/* fill CP/M FCB with given name */
	u = argv[ argpos ];
	v = fcb;
	/* set user id */
	*v++ = uid;
	/* set primary name */
	while ( v < fcb + 9 ) {
	    if ( *u != '\0' && *u != '.' ) {
		*v++ = toupper( *u++ );
	    } else {
		*v++ = ' ';
	    }
	}
	/* skip extra characters */
	while ( *u != '\0' && *u != '.' ) u++;
	if ( *u == '.' ) u++;
	/* set extention name */
	while ( v < fcb + 12 ) {
	    if ( *u != '\0' ) {
		*v++ = toupper( *u++ );
	    } else {
		*v++ = ' ';
	    }
	}

	/* check existance of the file */
	exist = FALSE;
	for ( i = 0; i < DIRMAX; i++ ) {
	    if ( memcmp( dirbuf[ i ], fcb, 12 ) == 0 ) {
		exist = TRUE;
		break;
	    }
	}

	/* warn exist or non-exist cases */
	if ( exist ) {
	    if ( wrmode && !ermode ) {
		MESSAGE( "CP/M file %s exist\n", argv[ argpos ] );
		continue;
	    }
	} else {
	    if ( !wrmode ) {
		MESSAGE( "CP/M file %s not exist\n", argv[ argpos ] );
		continue;
	    }
	}

	/* open file if nessesary */
	if ( wrmode ) {
	    fp = fopen( argv[ argpos ], binary ? BINRD : ASCRD );
	    if ( fp == NULL ) {
		ERROUT( "cannot open %s for input\n", argv[ argpos ] );
	    }
	} else if ( file == NULL && !ermode ) {
	    fp = fopen( argv[ argpos ], binary ? BINWR : ASCWR );
	    if ( fp == NULL ) {
		ERROUT( "cannot open %s for output\n", argv[ argpos ] );
	    }
	}

	/* erase the file if required */
	if ( ermode ) {
	    for ( i = 0; i < DIRMAX; i++ ) {
		if ( memcmp( dirbuf[ i ], fcb, 12 ) == 0 ) {
		    dirbuf[ i ][ 0 ] = NOTUSED;
		    for ( n = 16; n < 32; n++ ) {
			block[ dirbuf[ i ][ n ] & 0xFF ] = FALSE;
		    }
		}
	    }
	}

	/* read the file if required */
	if ( !wrmode && !ermode ) {

	    /* read from extent 0 */
	    ext = 0;
	    for (;;) {

		/* search directory entry for ext'th extent */
		fcb[ 12 ] = ext;
		for ( i = 0; i < DIRMAX; i++ ) {
		    if ( memcmp( dirbuf[ i ], fcb, 13 ) == 0 ) break;
		}

		/* if there's no entry, it means EOF */
		if ( i == DIRMAX ) break;

		/* set up parameters of current extent */
		reel = dirbuf[ i ][ 15 ] & 0xFF;
		dirptr = &dirbuf[ i ][ 16 ];

		/* read all records in the extent */
		for ( i = 0; i < reel; i++ ) {

		    /* get current block number */
		    r = dirptr[ i / 8 ] & 0xFF;

		    /* seek to current record position */
		    if ( lseek( dfd, r * 1024L + ( i & 7 ) * 128L, 0 ) < 0 ) {
			ERROUT( "disk seek error in %s\n", argv[ argpos ] );
		    }

		    /* read a record */
		    if ( read( dfd, recbuf, 128 ) < 128 ) {
			ERROUT( "disk read error in %s\n", argv[ argpos ] );
		    }

		    /* write a record, converting if needed */
		    for ( u = recbuf; u < recbuf + 128; u++ ) {
			if ( binary ) {
			    putc( *u, fp );
			} else {
			    switch ( *u ) {
			    case 0x0A: putc( '\n', fp ); break;
			    case 0x0D: break;
			    case 0x1A: goto ReadEOF;
			    default:   putc( *u, fp ); break;
			    }
			}
		    }

		}

		/* if this extent is last one, end reading */
		if ( reel < 128 ) break;

		/* succeed extent number */
		ext++;
	    }

	ReadEOF:;
	}

	/* write file if required */
	if ( wrmode ) {

	    /* write from extent 0, state 0 */
	    ext = 0;
	    state = 0;
	    for (;;) {

		/* get free directory entry */
		for ( i = 0; i < DIRMAX; i++ ) {
		    if ( ( dirbuf[ i ][ 0 ] & 0xFF ) == NOTUSED ) break;
		}
		if ( i == DIRMAX ) {
		    ERROUT( "directory full on %s\n", argv[ argpos ] );
		}

		/* fill the directory entry */
		memset( dirbuf[ i ], 0x00, 32 );
		memcpy( dirbuf[ i ], fcb, 12 );
		dirbuf[ i ][ 12 ] = ext;

		/* set block list pointer */
		dirptr = &dirbuf[ i ][ 16 ];

		/* fill blocks in the extent */
		for ( i = 0; i < 128; i++ ) {

		    /* stop looping if input is exhousted */
		    if ( state == 2 ) break;
		    if ( binary ) {
			c = getc( fp );
			if ( c == EOF ) break;
			ungetc( c, fp );
		    }

		    /* read 128 bytes data from the file */
		    for ( u = recbuf; u < recbuf + 128; u++ ) {
			switch ( state ) {
			case 0:
			    c = getc( fp );
			    if ( c == EOF ) {
				state = 2;
				*u = binary ? 0x00 : 0x1A;
			    } else if ( !binary && c == '\n' ) {
				state = 1;
				*u = 0x0D;
			    } else {
				*u = c;
			    }
			    break;
			case 1:
			    state = 0;
			    *u = 0x0A;
			    break;
			case 2:
			    *u = binary ? 0x00 : 0x1A;
			    break;
			}
		    }

		    /* get current block number */
		    r = dirptr[ i / 8 ] & 0xFF;

		    /* if position enters a new block, allocate it */
		    if ( r == 0 ) {
			for ( r = MINBLOCK; r < MAXBLOCK; r++ ) {
			    if ( ! block[ r ] ) break;
			}
			if ( r == MAXBLOCK ) {
			    ERROUT( "disk is full\n", NULL );
			}
			block[ r ] = TRUE;
			dirptr[ i / 8 ] = r;
		    }

		    /* write a record into virtual disk */
		    if ( lseek( dfd, r * 1024L + ( i & 7 ) * 128L, 0 ) < 0 ) {
			ERROUT( "disk seek error in %s\n", argv[ argpos ] );
		    }
		    if ( write( dfd, recbuf, 128 ) < 128 ) {
			ERROUT( "disk write error in %s\n", argv[ argpos ] );
		    }

		}

		/* store reel count */
		dirptr[ -1 ] = i;

		/* if reel is not full count, it means EOF */
		if ( i < 128 ) break;

		/* succeed extent number */
		ext++;
	    }

	}

	/* close output file when -o flag is omitted */
	if ( file == NULL ) {
	    fclose( fp );
	}

    }

    /* write back directory if nessesary */
    if ( wrmode || ermode ) {
	if ( lseek( dfd, 0L, 0 ) < 0 ) {
	    ERROUT( "disk seek error on directory\n", NULL );
	}
	if ( write( dfd, ( char * )dirbuf, 32 * DIRMAX ) < 32 * DIRMAX ) {
	    ERROUT( "disk write error on directory\n", NULL );
	}
    }

    /* close CP/M disk */
    close( dfd );

    /* close common output file when -o flag is present */
    if ( file != NULL && fp != stdout ) {
	fclose( fp );
    }

    return( 0 );
}
