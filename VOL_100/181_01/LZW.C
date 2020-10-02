/* LZWCOM - FILE COMPRESSOR UTILITY					*/
#include "sgtty.h"
#include "stdio.h"
#include "fcntl.h"
#define FALSE    0
#define TRUE     !FALSE
#define TABSIZE  4096
#define NO_PRED  -1
#define EMPTY    -1
#define NOT_FND  -1
struct entry {
  char used;
  int next;		/* hi bit is 'used' flag			*/
  int predecessor;			/* 12 bit code			*/
  char follower;
} string_tab[TABSIZE];

char is_a_con = FALSE;		/* flag to suppress 'dots' in writec		*/

/*   routines common to compress and decompress, contained in CommLZW.c	*/
hash();
unhash();
getcode();
putcode();
init_tab();
upd_tab();


main(argc,argv)
int argc; char *argv[];
{
  int c, code, localcode;
  int code_count = TABSIZE - 256;
  int infd, outfd;
  if (3 != argc) {
    printf("Usage : lzwcom oldfilename squeezefilename\n");
    exit(0);
  }
  if ( -1 == (infd = open( *++argv, O_RDONLY )) ) {
    printf("Cant open %s\n", *argv);
    exit(0);
  }
  if ( -1 == (outfd = creat(*++argv,0666)) ) {
    printf("Cant create %s\n",*argv);
    exit(0);
  }
  init_tab();				/* initialize code table	*/
  c = readc(infd);
  code = unhash(NO_PRED,c);		/* initial code for table 	*/
#ifdef DEBUG
  putchar(c);
  printf( "\n%x\n",code);
#endif
  while ( EOF != (c = readc(infd)) ) {
#ifdef DEBUG
  putchar(c);
#endif
    if ( NOT_FND != (localcode = unhash(code,c)) ) {
      code = localcode;
      continue;
    }
/* when the above clause comes false, you have found the last known code */
    putcode(outfd,code);	/* only update table if table isn't full */
#ifdef DEBUG
  printf( "\n%x\n",code);
#endif
    if ( code_count ) {
      upd_tab(code,c);
      --code_count;
    }
/* start loop again with the char that didn't fit into last string	*/
    code = unhash(NO_PRED,c);
  }
  putcode(outfd,code);			/* once EOF reached, always 	*/
					/* one code left unsent		*/
  flushout(outfd);			/* make sure everything's written */
  exit(0);				/* make sure everything gets closed */
}
