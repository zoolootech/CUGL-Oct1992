/*  dump.c  (10/83)  Debug style dump of a file from any starting position.


  Usage: dump filespec [block [page]] | [segment:[offset]] [count]
	 Where a block is 64K bytes and a page is 256 bytes.
	 Segment:offset are standard 8086 notation in hexadecimal.
	 Count is the number of bytes to dump in decimal.

   dump b:\calc\data.dbm 1 16
	Positions file to absolute offset of 1000:1000 or 69632.
   dump bigfile :4F0 256
	Positions file to absolute offset of 4F0H or 1264 and
	dumps 256 bytes.

     The original source for dump was found on Lynn Long's BBS in Tulsa.  I
modified it to include file positioning.  It has proven to be a great tool at
times when I have to work with binary files that often exceed several million
characters.
   I have been using the Lattice 'C' compiler for about a year now and have
been very pleased with it.  I have re-written the level 1 I/O routines to take
advantage of DOS 2.0 path names, etc. and modified the _main module to allow
me to load and execute programs from within 'c'.

			Ted Reuss     c/o South Texas Software, Inc.
		  Home: 713/961-3926	  4544 Post Oak Place, Suite 176
		  Offi: 713/877-8205	  Houston, Tx 77027

*/
#include <stdio.h>
#include <ctype.h>		/* Lattice character type macros */

#define FAIL	    1
#define SUCCESS     0
#define TRUE	  (-1)
#define FALSE	    0
#define NULL	    0		/* null pointer value */
#define FOREVER     for (;;)
#define PAUSE	    if (getch()=='\0') getch();
#define STDIN	    0
#define STDOUT	    1
#define STDERR	    2

#define BUFSIZE 512
typedef unsigned    ushort;
typedef int	    void;

int _fmode = 0x8000;		/* Lattice 'c' - forces binary I/O */
extern short errno;		/* DOS 2.0 error number */
long lseek();

char   buffer[BUFSIZE]; 	/* input buffer */
ushort block = 0;		/* block number ( 64k bytes/block ) */
ushort page = 0;		/* page number ( 256 bytes/page ) */
ushort segment = 0;
ushort offset = 0;
long   filpos = 0;		/* beginning file position */
long   count = 0x7FFFFFFFL;	/* number of bytes to dump */

main(argc,argv) 		/* DUMP ENTRY */
int  argc;
char *argv[];
{
   char c;
   ushort i, numin, tot, file, cfrom, flag = 0;

   if (argc < 2)
      abort( "Usage:dump filespec [block [page]] | [seg:[ofs]]", 0, 0 );

   if ((file = open( argv[1], 0 )) == -1)
      abort( "cannot open", argv[1], errno );

   if (argc > 2) {
      if ((flag = stpchr( argv[2], ':' )) != NULL) {
	 i = stch_i( argv[2], &segment );
	 stch_i( argv[2]+i+1, &offset );
      }
      if (sscanf( argv[2], "%d", &block ) != 1)
	 abort( "invalid block", argv[2], 0 );
   }
   if (argc > 3)
      if (sscanf( argv[3], "%d", &page ) != 1)
	 abort( "invalid page", argv[3], 0 );

   if ( flag ) {
      filpos = (long)segment*16L + (long)offset;
      tot = offset;
   }
   else {
      filpos = (block * 65536L) + (page * 256);
      tot = page * 256;
      segment = block * 4096;
   }

   if (lseek( file, filpos, 0 ) == -1L)
      abort( "positioning to", argv[2], errno );

   if (argc > 4)
      if (sscanf( argv[4], "%ld", &count ) != 1)
	 abort( "invalid count", argv[4], 0 );


   do { 				   /* read & dump BUFSIZE bytes */
      numin = read( file, buffer, BUFSIZE );
      if (numin == -1)
	 abort( "cannot read", argv[1], errno );
      cfrom=0;
      while (cfrom < numin) {

	 ohw(segment);			   /* print offset in hex */
	 putchar(':');
	 ohw(cfrom+tot);
	 putchar(' ');

	 for (i=0; i < 16; i++) {	   /* print 16 bytes in hex */
	    putchar(' ');
	    ohb(buffer[cfrom++]);
	 }

	 putchar(' '); putchar(' '); putchar(' ');

	 cfrom -= 16;
	 for (i=0; i < 16; i++) {	   /* print 16 bytes in ASCII */
	    c = buffer[cfrom] & 0x7f;
	    if ( isprint(c) )		   /* if printable character */
	       putchar(c);
	    else
	       putchar('.');               /* else print period */
	    cfrom++;
	 }

	 putchar('\r'); putchar('\n');     /* print CR/LF */

	 if ((count -= 16) <= 0)	     /* is count exhausted? */
	    exit(0);
      } 				   /* end of while */
      tot += numin;
      if ( tot == 0 )
	 segment += 4096;
   }					   /* end of do */
   while (numin == BUFSIZE);
}					   /* end of main */

void ohw(wrd)			     /*      print a word in hex     */
ushort wrd;
{
   ohb( wrd>>8 );
   ohb( wrd );
}

void ohb(byt)			     /*      print a byte in hex     */
char byt;
{
   onib( byt>>4 );
   onib( byt );
}

void onib(nib)			 /*	 print a nibble as a hex character   */
char nib;
{
   nib &= 15;
   putchar((nib >= 10) ? nib-10+'A': nib+'0');
}

void abort( msg1 ,msg2 ,errno)	   /*  print error msg1, msg2, and nbr */
char *msg1,*msg2;		   /*	Does not close files.  */
short errno;
{
   char stemp[10];

   write( STDERR, "ERR: ", 5 );
   if (msg1)
      write( STDERR, msg1, strlen(msg1) );
   if (msg2)
      write( STDERR, " ", 1 );
      write( STDERR, msg2, strlen(msg2) );
   if (errno)	{
      sprintf( stemp," #%d", errno );
      write( STDERR, stemp, strlen(stemp) );
   }
   write( STDERR, "\r\n", 2 );
   _exit( FAIL );
}
/** END DUMP **/
                                                                                                            