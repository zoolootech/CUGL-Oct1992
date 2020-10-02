static char HELPTEXT[] =

" makehx.c --- This program makes the help file index \n"
"              For use with the WTWG user interface system \n"
" \n"
"  PARAMETER:  FILENAME without extension.  \n"
"		the input  file is named FILENAME.hlp \n"
"		the output file is named FILENAME.hx \n"
" \n"
"  INPUT: ASCII file with help text.\n"
"       New topics are identified by a '@' in column 1\n"
"       ... followed immediately by the topic name, then <CR>\n"
"	The text for that topic follows. Limit is 50 columns, 8 lines\n"
"       The next topic begins with another @ in column 1\n"
" \n"
" OUTPUT: binary file containing help index file FILENAME.HX, which \n"
"       can be read by the whelp() routine to give context-sensitive help\n"
" \n"
" by D Blum 1989 \n";

/* To compile this program under TurboC (NOTE large memory model)
 *		tcc -ml makehx.c
 *
 *	To compile under microsoft C
 *		cl /AL /D__LARGE__  makehx.c
 */

#include <stdlib.h>
#ifdef __TURBOC__
	#include <alloc.h>
#else
	/* MICROSOFT version */
	#include "msc.h"
	#include <malloc.h>
#endif		/* MICROSOFT */	

#include <string.h>
#include <stdio.h>

#include "wtwg.h"		/* allows wsys.h to be included */
#include "wsys.h"		/* for NORMALIZE macro */
#include "whelp.h"


#ifndef __LARGE__
#error Program must be built in the large model. MICROSOFT: define __LARGE__
#endif	/* LARGE */


/* function to trim strings of terminal spaces, \r \n
 */
static void near strim ( char * );


main ( int argc, char **argv )
	{
	FILE *infile, *outfile;
	char inf_name[13],  outf_name[13];
	char buff[256];

	HX   *hx;

	int  entry;
	long lastpos;

	if ( argc == 1
	|| strlen (argv[1]) > 8
	|| strchr (argv[1], '.') )
		{
		puts (HELPTEXT);
		exit (0);
		}

	strcpy ( inf_name, argv[1] );
	strcat ( inf_name, ".hlp" );
	strcpy ( outf_name, argv[1] );
	strcat ( outf_name, ".hx" );

	infile = fopen (inf_name, "rb");
	if ( ! infile )
		{
		printf ( "error - file %s not found\n", inf_name );
		puts (HELPTEXT);
		exit (1);
		}

	hx = malloc ( MAX_TOPICS * sizeof (HX) );
	if ( ! hx )
		{
		perror ("Not enough memory");
		exit (1);
		}
	_NORMALIZE (hx);
	memset ( hx, 0, MAX_TOPICS * sizeof (HX) );

	lastpos = entry = 0;

	while ( fgets( buff, sizeof(buff), infile ) )	/* read thru text file */
		{
		if ( *buff == '@' )
			{
			/* new topic coming,
			 * first have to cleanup last topic entry, set size of last topic,
			 */
			hx[entry].hxsize = lastpos - hx[entry].hxpos;

			/* start new entry
			 */
			++entry;
			if ( entry == MAX_TOPICS )
				{
				puts ("Too many topics");
				exit ( 99 );
				}
			strim (buff);   /* clean up new topic name */

			memcpy ( hx[entry].hxtopic, buff+1, TOPIC_SIZE );

			hx[entry].hxpos = ftell ( infile );

			printf ("Topic: %s\n", buff+1);
			}
		lastpos = ftell ( infile );

		}

	if ( entry == 0 )
		{
		puts ("No topics found");
		exit (1);
		}

	hx[entry].hxsize = lastpos - hx[entry].hxpos;	/* size of last entry */


	/* finished constructing help index - now write it to disk */


	outfile = fopen (outf_name, "wb");
	if  ( ! outfile )
		{
		printf ("Could not open output file %s\n",outf_name);
		exit (1);
		}


	/* The first thing found in the help index is the number of entries
	 */
	if ( 1 != fwrite ( &entry, sizeof (entry), 1, outfile ) )
		{
		printf ("Error writing header to output file %s\n", outf_name);
		}

	/* Now write the entire table of entries to the index file
	 * NOTE the first entry is a dummy (see the loop that constructed
	 *      the entries.
	 */
	if ( entry != fwrite ( hx+1,   sizeof (HX),  entry, outfile ) )
		{
z		printf ("Error writing data to output file %s\n", outf_name);
		}

	fclose (outfile);

	puts ("Sucessful Completion");


     return (0);	/* main */
     }



static void near strim (char *s)
	{
	int n;
	char *p;

	_NORMALIZE (s);		/* large model */
	n =  strlen (s);

	while ( n-- )
		{
		p = s+n;
		if ( *p == '\r' || *p == '\n' || *p == ' ' )
			{
			*p = 0;
			}
		else
			{
			break;
			}
		}

	return; /* strim() */


	}
/*--------------------- end of MAKEHX.C --------------------------*/
