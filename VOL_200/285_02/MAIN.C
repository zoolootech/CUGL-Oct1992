/*
**	file:		main.c
**	purpose:	Top level entry point of bison

Copyright (C) 1984, 1986 Bob Corbett and Free Software Foundation, Inc.

BISON is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the BISON General Public License for full details.

Everyone is granted permission to copy, modify and redistribute BISON,
but only under the conditions described in the BISON General Public
License.  A copy of this license is supposed to have been given to you
along with BISON so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

 In other words, you are welcome to use, share and improve this program.
 You are forbidden to forbid anyone else to use, share and improve
 what you give them.   Help stamp out software-hoarding!
*/

#include <stdio.h>
#include <stdlib.h>

#include "machine.h"    /* JF for MAXSHORT */
#include "getargs.h"
#include "files.h"
#include "symtab.h"
#include "state.h"
#include "reader.h"
#include "nullable.h"
#include "lalr.h"
#include "lr0.h"
#include "print.h"
#include "conflict.h"
#include "derives.h"
#include "output.h"
#include "bison.h"

extern  int lineno;
extern  int verboseflag;

static void usage( void) ;

/* Nonzero means failure has been detected; don't write a parser file.  */
int failure;

int main(argc, argv)
int argc;
char *argv[];
{
  failure = 0;
  lineno = 0;

  if (argc < 2)
    usage() ;

  getargs(argc, argv);
  openfiles();

  /* read the input.  Copy some parts of it to fguard, faction, ftable and fattrs.
     In file reader.
     The other parts are recorded in the grammar; see gram.h.  */
  reader();

  /* record other info about the grammar.  In files derives and nullable.  */
  set_derives();
  set_nullable();

  /* convert to nondeterministic finite state machine.  In file LR0.
     See state.h for more info.  */
  generate_states();

  /* make it deterministic.  In file lalr.  */
  lalr();

  /* Find and record any conflicts: places where one token of lookahead is not
     enough to disambiguate the parsing.  In file conflicts.
     Currently this does not do anything to resolve them;
     the trivial form of conflict resolution that exists is done in output.  */
  initialize_conflicts();

  /* print information about results, if requested.  In file print. */
  if (verboseflag)
    verbose();
  else
    terse();

  /* output the tables and the parser to ftable.  In file output. */
  output();

  done(failure);
  return 0 ;
}

/* functions to report errors which prevent a parser from being generated */

void fatal(s)
char *s;
{
  extern char *infile;

  if (infile == 0)
    fprintf(stderr, "fatal error: %s\n", s);
  else
    fprintf(stderr, "\"%s\", line %d: %s\n", infile, lineno, s);
  done(1);
}


/* JF changed to accept/deal with variable args.  Is a real kludge since
   we don't support _doprnt calls */
/*VARARGS1*/
void fatals(fmt,x1,x2,x3,x4,x5,x6,x7,x8)
char *fmt;
int x1,x2,x3,x4,x5,x6,x7,x8 ;
{
  char buffer[200];

  sprintf(buffer, fmt, x1,x2,x3,x4,x5,x6,x7,x8);
  fatal(buffer);
}



void toomany(s)
char *s;
{
  char buffer[200];

    /* JF new msg */
  sprintf(buffer, "limit of %d exceeded, too many %s", MAXSHORT, s);
  fatal(buffer);
}



void berror(s)
char *s;
{
  fprintf(stderr, "internal error, %s\n", s);
  abort();
}

static void usage( void)
{
 int i ;
 static char *s[9] = {
    "BISON\t\t\t\t(Jan. 7, 1989)",
    "\tusage:\tbison -{dlvty} <infile.Y> [-o <outfile.ext>]",
    "\t-d\tgenerate token definition file",
    "\t-l\tfilter #line directives from output file",
    "\t-v\tVerbose.  generate state machine statistics file",
    "\t-t\tdebugging Trace enabled",
    "\t-y\tYacc compatibility. produce files named YTAB.?",
    "\t-o\tOutput filename",
    "\0"
    } ;

    for (i=0 ; i<8 ; i++)
        fprintf( stderr,"%s\n", s[i]) ;

    exit(0) ;
}
