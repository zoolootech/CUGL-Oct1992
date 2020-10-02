/* Open and close files for bison,
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
 what you give them.   Help stamp out software-hoarding!  */

/*
    DrH. 1/4/89
        Post-mortem hacking on filename handling for MS/PC-DOS conventions.
        Removed obscurant PASCAL-isms in string manipulations.
        Filename strings changed to static arrays to avoid stack-whacking
        when constructing new names.  Why malloc strings on the fly if 
        they MUST persist for the entire runtime of the program, i.e.,
        are by definition static ?
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

#include "bison.h"
#include "files.h"
#include "new.h"
#include "gram.h"

FILE *finput = NULL;
FILE *foutput = NULL;
FILE *fdefines = NULL;
FILE *ftable = NULL;
FILE *fattrs = NULL;
FILE *fguard = NULL;
FILE *faction = NULL;
FILE *fparser = NULL;

/* File name specified with -o for the output file, or 0 if no -o.  */
char *spec_outfile;

char *infile;
char outfile[80] = "\0" ;
char defsfile[80] = "\0" ;
char tabfile[80] = "\0" ;
char attrsfile[ FNAMESIZE + 1] = "\0" ;
char guardfile[ FNAMESIZE + 1] = "\0" ;
char *actfile;
char *tmpattrsfile;
char *tmptabfile;

static FILE *tryopen( char *, char *) ;
static char *get_namebase( char *) ;
static char *name_base;

extern int verboseflag;
extern int definesflag;

int fixed_outfiles = 0;



/* JF this has been hacked to death.  Nowaday it sets up the file names for
   the output files, and opens the tmp files and the parser */

/*--------------------------------------------------------------------------
** OPENFILES
**      constructs output/temp filenames from whatever is handy.
**      and attempts to open them all.
*/
void openfiles()
{
  char *filename;

  if (*spec_outfile)             /* output file was specified via '-o' option */
      name_base = get_namebase( spec_outfile) ;
  else
  {                                /* base filename is "ytab" if -y flag used */
        if (fixed_outfiles)
            name_base = "ytab" ;
        else
            name_base = get_namebase( infile) ;
  }
 
  finput = tryopen(infile, "r") ;

  if (!(filename = getenv("BISON_SIMPLE")))      /* check if parser skeleton */
     filename = PFILE ;                           /* in environment variable  */

  fparser = tryopen( filename, "r") ;

  if (verboseflag)                                /* open parser stats file */
    {
      sprintf( &outfile[0], "%s%s", name_base, ".out") ;
      foutput = tryopen(outfile, "w") ;
    }

  if (definesflag)                               /* open definitions file */
    {
      sprintf( &defsfile[0], "%s%s", name_base, ".h") ;
      fdefines = tryopen(defsfile, "w") ;
    }
                                         /* open temporary files        */
  actfile = mktemp("acXXXXXX") ;         /* NOTE: temps were previously */
                                         /* unlinked after opening in the */
  faction = tryopen(actfile, "w+") ;     /* GNU release version (ca. 9/88) */
                                         
  tmpattrsfile = mktemp("atXXXXXX") ;    
  fattrs = tryopen(tmpattrsfile,"w+") ;  

  tmptabfile = mktemp("taXXXXXX") ;
  ftable = tryopen(tmptabfile, "w+") ;

    /* These are opened by `done' or `open_extra_files', if at all */
    /* N.B. tabfile is the filename of the generated parser (DrH)  */

  if (spec_outfile && *spec_outfile)
    strcpy( &tabfile[0], spec_outfile) ;
  else
    sprintf( &tabfile[0], "%s%s", name_base, ".c") ;

  sprintf( &attrsfile[0], "%s%s", name_base, ".atr") ;
  sprintf( &guardfile[0], "%s%s", name_base, ".grd") ;
}


/* open the output files needed only for the semantic parser.
This is done when %semantic_parser is seen in the declarations section.  */
void open_extra_files()
{
  FILE *ftmp;
  int c;
  char *filename;
        /* JF change open parser file */
  fclose( fparser) ;

  if (!(filename = getenv("BISON_HAIRY")))      /* check if parser skeleton */
     filename = PFILE1 ;                         /* in environment variable  */

  fparser = tryopen( filename, "r") ;

        /* JF change from inline attrs file to separate one */
  ftmp = tryopen( attrsfile, "w") ;
  rewind( fattrs) ;

  while ((c = getc( fattrs)) != EOF)    /* Thank god for buffering */
    putc(c,ftmp) ;

  fclose(fattrs) ;
  fattrs=ftmp;

  fguard = tryopen(guardfile, "w") ;

}

    /* JF to make file opening easier.  This func tries to open file
       NAME with mode MODE, and prints an error message if it fails. */
FILE *
tryopen(name, mode)
char *name;
char *mode;
{
 FILE   *ptr;

    ptr = fopen(name, mode) ;

    if (ptr == NULL)
    {
      fprintf(stderr, "bison file open: ") ;
      perror(name) ;
      done(2) ;
    }

    return ptr;
}

/*
    DrH 1/6/89
        change file copy to block mode
*/

void done(k)
int k;
{
 char *buff ;
 int bytesread = 0 ;

  if (faction)
  {
    fclose(faction) ;
    unlink(actfile) ;
  }

  if (fattrs)
  {
    fclose(fattrs) ;
    unlink(tmpattrsfile) ;
  }

  if (fguard)
    fclose(fguard) ;

  if (finput)
    fclose(finput) ;

  if (fparser)
    fclose(fparser) ;

  if (foutput)
    fclose(foutput) ;

    /* JF write out the output file */
  if (k == 0 && ftable)
    {
      FILE *ftmp;
      register int c;

      ftmp = tryopen( tabfile, "w") ;
      rewind(ftable) ;

      buff = NEW2( 4608, char) ;

      while ((bytesread = fread( buff, 1, 4608, ftable)) > 0)
        (void) fwrite( buff, 1, bytesread, ftmp) ;
      FREE( buff) ;

      fclose( ftmp) ;
      fclose( ftable) ;
      unlink(tmptabfile) ;
    }

  exit(k) ;
}

/*----------------------------------------------------------------------------
** GET_NAMEBASE
**      extracts base file name from an MS/PC-DOS path string.
**      makes naive assumptions about initial correctness of path string.
**  (DrH 1/5/88)
*/
static char *get_namebase( char *path)
{
 int i ;
 char *cp ;
 static char filename[ FNAMESIZE + 1 ] ;

    if (!*path)                                     /* if no input string...*/
        path = "ytab    .y  " ;

    for(i=0 ; i<FNAMESIZE ; i++)                    /* init string space */
        filename[i] = '\0' ;

    if (cp = strrchr( path, '\\'))           /* find rightmost path separator */
        ++cp ;
    else
        cp = path ;
                                                          /* copy  up to '.' */
    for(i = 0 ; ((i < 8) && *cp && (*cp != '.')) ; i++)
        filename[i] = *cp++ ;
    filename[i] = '\0' ;

    return ( &filename[0] ) ;
}

