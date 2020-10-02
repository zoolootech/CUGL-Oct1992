
/* printer.c -- for burlap, 11/29/88, d.c.oshel */

#include "burlap.h"

extern FORM_RULES burlap[];
extern char vrec[ BUFSIZE ];

#define LABELWIDTH 3    /* mailing labels are 3.5'' wide by 15/16'' deep */ 

#define PICA 0
#define ELITE 1
#define CMPRX 2

static int pfofs[] = { 78, 102, 131 }; /* offsets into patch area */
static char printfont[] = {

      '[', 'P', 'r', 'i', 'n', 't', 'e', 'r', ' ', 'P',
      'a', 't', 'c', 'h', ' ', 'A', 'r', 'e', 'a', ' ',

      '(', 'S', 't', 'a', 'y', ' ', 'w', 'i', 't', 'h',
      'i', 'n', ' ', 'b', 'r', 'a', 'c', 'k', 'e', 't',

      's', ',', ' ', 'f', 'i', 'r', 's', 't', ' ', 'b',
      'y', 't', 'e', ' ', '=', ' ', 'l', 'e', 'n', 'g',

      't', 'h', ' ', 'o', 'f', ' ', 's', 't', 'r', 'i',
      'n', 'g', ':', 'P', 'i', 'c', 'a', '{', 

            7, '\x1b', 'U', '0', '\x1b', 'P', '\x1b', 'n',
            0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,
            '}',

      ':', 'E', 'l', 'i', 't', 'e', '{',

            7, '\x1b', 'U', '1', '\x1b', 'P', '\x1b', 'M',
            0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,
            '}',

      ':', 'C', 'o', 'm', 'p', 'r', 'e', 's', 's', 'e', 'd', '{',

            6, '\x1b', 'U', '1', '\x1b', 'P', '\x0f', 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,
            '}',

      ':', 'E', 'n', 'd', ' ', 'P', 'a', 't', 'c', 'h', ' ', 
      'A', 'r', 'e', 'a', ']'
      };

#define fontcmd(F) (&printfont[pfofs[F]])



static int limit;

static void prt_label_line( char *pline )
{
      char *p, *prntrcmd;
      int cnt, doublestrike;

      /* adjust label line width to actual length of data received;
      ** data too long is TRUNCATED on the label
      */

      doublestrike = 0;
      limit = 80;

      if ( strlen(pline) <= 10 * LABELWIDTH )
      {
          prntrcmd = fontcmd( PICA );
          limit = 10 * LABELWIDTH;
      }
      else if ( strlen(pline) <= 12 * LABELWIDTH )
      {
          prntrcmd = fontcmd( ELITE );
          limit = 12 * LABELWIDTH;
          doublestrike = 1;
      }
      else /* long line, default to compressed print */
      {
          prntrcmd = fontcmd( CMPRX );
          limit = 17 * LABELWIDTH;
          doublestrike = 1;
      }

      /* Uses WordStar style; i.e., first byte is length of cmd string, 
      ** to allow zero byte to be sent to printer.
      */

      for ( cnt = ((*prntrcmd++) & 0xff); cnt > 0; cnt--, prntrcmd++ )
          fputc( *prntrcmd, stdprn );


      /* send up to limit chars from p to file or stdprn
      */

dbl:  for ( p = pline, cnt = 0; isprint(*p) && cnt < limit; p++, cnt++ )
      {
            fputc( *p, stdprn );
      }
      /* doublestrike underemphasized printer fonts */
      if ( doublestrike )
      {
            fputc( '\r', stdprn );
            doublestrike = 0;
            goto dbl;
      }

      fputc( '\r', stdprn);
      fputc( '\n', stdprn);
      fflush( stdprn );
}




void print_labels( int index )  /* index determines sort order */
{
    int i,j,n;
    char *q;

	if ( choose_records() )
    {
        if ( ask("Printer ready?") ) {
        fullscreen();
        put_screen();
        gotoxy( 2,0 );
        wputs( "µ^2 Print Labels ^0Æ" );
        gotoxy( 62,18 );
        wputs( "^1Esc^0 halts!" );
	    for ( n = FRSREC( index,vrec ); n != 101 ; n = NXTREC( index,vrec ))
        {
            if ( n )
                ISAM_crash( "NXTREC" );

            if ( kbhit() && getch() == ESC )
                break;

            if ( REDVREC( datno, vrec, BUFSIZE ) )
                ISAM_crash("REDVREC");
            else
            {
        	    if ( load_form(),selected() )
                {
                    show_form();
            	    for ( j = 0,i = ISAM_field_range; j < 5; i++ )
                    {
                        q = strdup( *(burlap[i].fptr) );
                        strip_blanks( q );
                        if ( strlen(q) )
                        {
                	        prt_label_line( q );
                            j++;
                        }
                        free (q);
                    }
                    for ( ; j < 6; j++ )
                    {
                	    fputc( '\r', stdprn );
                	    fputc( '\n', stdprn );
                    }
                    fflush( stdprn );
                }
            }
        }}
    }
}

