
/* LZWUNC - FILE UNCOMPRESSOR UTILITY                                   */
#include "stdio.h"
#include "fcntl.h"
#include "sgtty.h"
#include "debug.h"
#define FALSE    0
#define TRUE     !FALSE
#define TABSIZE  4096
#define STACKSIZE TABSIZE
#define NO_PRED  0xFFFF
#define EMPTY    0xF000
#define NOT_FND  0xFFFF
#define NO_UPDATE FALSE
#define UEOF ((unsigned)EOF)

struct entry {
  char used;
  unsigned int next;        /* hi bit is 'used' flag                        */
  unsigned int predecessor; /* 12 bit code                  */
  unsigned char follower;
} string_tab[TABSIZE];

char is_a_con = FALSE;  /* suppresses printing dots during operation if
                           outfd is the console                         */

/*   routines common to compress and decompress, contained in CommLZW.c */
unsigned hash();
unsigned unhash();
unsigned getcode();
putcode();
init_tab();
upd_tab();

main(argc,argv)
int argc; char *argv[];
{
  register unsigned int c, tempc, code, oldcode, incode, finchar,lastchar;
  char unknown = FALSE;
  int code_count = TABSIZE - 256;
  int infd, outfd;
  register struct entry *ep;

  if (3 != argc) {
    fprintf(stderr,"Usage : lzwunc squeezefilename newfilename\n");
    exit(0);
  }
  if ( -1 == (infd = open( *++argv, O_RDONLY )) ) {
    fprintf(stderr,"Cant open %s\n", *argv);
    exit(0);
  }
  if ( -1 == (outfd = creat(*++argv,0666)) ) {
    fprintf(stderr,"Cant create %s\n",*argv);
    exit(0);
  }

  is_a_con = isatty(outfd);             /* suppress dots if it is        */
  init_tab();                           /* set up atomic code definitions*/
  code = oldcode = getcode(infd);
  c = string_tab[code].follower;        /* first code always known      */
  writec(outfd,c);
  DEBUGGER(\
	   putc(c,stderr);\
	   fprintf(stderr,"\n%x\n",code);\
  )
  finchar = c;

  while ( UEOF != (code = incode = getcode(infd)) ) {
	DEBUGGER(fprintf(stderr,"\n%x\n",code);)
    ep = &string_tab[code];             /* initialize pointer           */
    if ( !ep->used ) 
	{                  /* if code isn't known          */
	  lastchar = finchar;
	  code = oldcode;
	  unknown = TRUE;
      ep = &string_tab[code];           /* re-initialize pointer        */
    }

    while (NO_PRED != ep->predecessor) 
	{
      push( ep->follower);              /* decode string backwards into */
                                        /* stack                        */
      code = ep->predecessor;
      ep = &string_tab[code];
    }

    finchar = ep->follower;
    /* above loop terminates, one way or another, with                  */
    /* string_tab[code].follower = first char in string                 */

    writec(outfd, finchar);
    DEBUGGER(fputc(finchar,stderr);)
    /* pop anything stacked during code parsing                         */
    while ( EMPTY != (tempc = pop()) ) 
	{
      writec(outfd,tempc);
	  DEBUGGER(\
    	putc(tempc,stderr);\
	  )
    }
    if ( unknown ) {                  /* if code isn't known          */
      writec(outfd,finchar = lastchar); /* the follower char of last    */
	  unknown = FALSE;
	  DEBUGGER(\
	  	putc(lastchar,stderr);\
		fprintf(stderr,"\nunknown code\n");\
	  )
	}
    if ( code_count ) 
	{
      upd_tab(oldcode,finchar);
      --code_count;
	  DEBUGGER(fprintf(stderr,"\nAdding %x,%c = %x\n",\
	  			oldcode,finchar,unhash(oldcode,finchar));)
    }
    oldcode = incode;
  }
  flushout(outfd);      /* write out fractional buffer, if any          */
  exit(0);              /* close all files and quit                     */
}

char stack[STACKSIZE];  /* stack for pushing and popping characters     */
int sp = 0;             /* current stack pointer                        */

push(c)
int c;
{
  stack[sp] = ((char) c);/* coerce passed integer into a character      */
  ++sp;
  if (sp >= STACKSIZE) {
    fprintf(stderr,"Stack overflow, aborting\n");
    exit(1);            /* non-0 exit parameter terminates batch job    */
  }
}

pop() 
{
  if (sp > 0) 
  {
    --sp;               /* push leaves sp pointing to next empty slot   */
    return ( (int) stack[sp] ); /* make sure pop returns char           */
  }
  else
    return EMPTY;
}
    

