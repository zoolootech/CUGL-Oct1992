/*
HEADER:         ;
TITLE:          C-ACROSS;
VERSION         1.02

DESCRIPTION:   "Utility for multiple module programs. Produces
	  Six indexes of functions, prototypes, and globals that
	  enable user to 'see across' modules for use in checking
	  and comparison.  One of these is type of hierarchical
	  functions list, a listing by module of functions
	  and calls made FROM them; another is alphabetical list
	  of functions and calls made TO them. Globals listed
	  in schematic descriptors which record all modifiers
	  and qualifiers and enable checking of declarators
	  across modules. Creates, on request, header file
	  consisting of prototypes constructed from function
	  definitions. Can list user types #defined and some
	  preprocessor #defines. Full documentation in README.CA";

KEYWORDS:       Utility, Cross Reference, Deubgging;
SYSTEM:         MS-DOS;
FILENAME:       UTIL_CA.C;

WARNINGS:      "1. Assumes function definitions conform with
		ANSI standards and have prototype form. See
		also "Caveats and Restrictions" in README.CA.
		2. Assumes syntactically correct source files.
		3. Written and tested using Microsoft QuickC.
		4. Copyright retained.  See Copyright
		information in README.CA.";

SEE-ALSO:      EXIT_CA.C, FUNC_CA.C, GLOB_CA.C, IFDEF_CA.C, INTF_CA.C,
	       LINKL_CA.C, PARSE_CA.C, TDEF_CA.C, TYPES_CA,
	       UTLG_CA.C, XRF_CA.C, README.CA,
	       CA.H, CA.PRJ, CA.RPT, CDECL_CA.H, KEYWORDS.H;
AUTHORS:       Myron Turner;
COMPILERS:     Microsoft C;

*/

/***************************  C-ACROSS  ***************************
			       V. 1.02
		       Copyright (C) Myron Turner

			  333 Bartlet Ave.
			  Winnipeg, Manitoba
			  Canada R3L 0Z9
			  (204) 284-8387

*********************************************************************/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>


#define INCLUDEFILES_DEFINE
#include "ca.h"
#include "ca_decl.h"
#define EXTERNAL
#include "keywords.h"

#define QUOT  '\x22'
#define RET_FULLPATH 0x10
#define RET_PATHVAR_ONLY  0x20

/* externs and globals */
char SubstInclude[128];
extern int __Inclf_Limit;
extern int declare_file;
extern FILE *scrn_out;
extern int user_hfile_only;
extern FILE **FP_ADDRESS;
int reading_includeF = 0;
unsigned int inclfLine = 0;
unsigned int inclFini = 0;
struct includeFILE inclF;

/******** UTILITIES FOR HANDLING FILES, INCLUDE FILES, PATH *****/

char * get_envpath(int service, char *env_var, char *fname)
{
    char *pathvar, pathbuf[128], filebuf[128];
    int i, conver_fact = 32;
    size_t strL;

    if (!(pathvar = getenv("INCLUDE" )) ) {
      i = 0;
      if (env_var[i] > 'Z' ) conver_fact = -conver_fact;
      while(env_var[i]) {
	env_var[i++] += (char)conver_fact;
       }

       if ( !(pathvar = getenv(env_var)) ) {
	if (service == RET_PATHVAR_ONLY) return (NULL);
	 printf("\nPath not found for %s.\nEnter path [%s]: ",
						fname, SubstInclude);
	 gets(filebuf);

	 strL = strlen(filebuf);
	 if (strL) {
	 if (filebuf[strL] != '\\') {
	     filebuf[strL] = '\\';
	     filebuf[strL + 1] =  '\0';
	     }
	 if (!SubstInclude[0]) strcpy (SubstInclude, filebuf);
	 }
	 else {
	  if (SubstInclude[0]) strcpy(filebuf, SubstInclude);
	 }

	 strcat(filebuf, fname);
	 strcpy(fname, filebuf);
	 return(fname);
	 }
    }

    if (service == RET_FULLPATH) {
       _searchenv( fname, env_var, filebuf );
       return(strcpy(fname, filebuf));
       }
     else if (service == RET_PATHVAR_ONLY)
       return(strcpy(fname, pathvar));

}


/* Check whether include file is enclosed in brackets "<>" or in */
/* quotation marks. Isolate the filename in passed string and open */
/* file if it is less than or equal to the include file limit*/

FILE *setincludefile(char *includebuf)
{
  char *fname, *ptr, *endbuf = includebuf;
  char  filespec[128];
  size_t strL;
  FILE *fp;
  char delimiter = '>';


  strL = strlen(includebuf);
  endbuf += (strL -1);
  if ( !(fname = strchr(includebuf, '<')) )
      if ( (fname = strchr(includebuf, QUOT)) )
	      delimiter = QUOT;
  if (!fname || (fname >= endbuf)) return(NULL);
  if  (user_hfile_only) if (*fname == '<') return (NULL);
  while( (*(++fname) == ' ' || *fname == (char) 0x9) && *fname);
  ptr = strchr(fname, delimiter);
  if (!ptr || (ptr >= endbuf)) return (NULL);
  while(*(--ptr) == (char)0x9 || *ptr == (char)0x20);
  *(++ptr) = '\0';

  strcpy(filespec, fname);
  if  (inclF.level <= __Inclf_Limit)
    strcpy(inclF.file[inclF.level], fname);
    else return(NULL);

  if (delimiter == QUOT)
    if ((fp = open_includef(filespec))) return(fp);

  get_envpath(RET_FULLPATH, "INCLUDE", filespec);
  if (!(fp = open_includef(filespec)))
     fprintf (scrn_out,
		"Cannot open: %s. Possible INCLUDE variable error.\n", fname);


  return(fp);
}


FILE *open_includef(char *filespec)
{
  FILE *fp;
  extern FILE *rptfp;

  if (   (fp = fopen( filespec, "rt") ) == NULL )
       {
	   return (NULL);
       }


   return(fp);


}

/*  File pointer for scrnout.rpt when /red switch is set */
FILE *get_redfp(void)
{

    return(fopen("scrnout.rpt", "w"));
}


int is_include(char *token_ptr, FILE *holdfp)
{
  int str_pos;

    if (inclF.level == __Inclf_Limit) return(0);
    holdfp = *FP_ADDRESS;
    if ( (str_pos = test_token(token_ptr, TYPES[INCLUDE_])) )   {
	  if( (*FP_ADDRESS = setincludefile(token_ptr+str_pos)) ) {
	     reading_includeF = 1;
	     inclF.address[inclF.level] = *FP_ADDRESS;
	     fprintf(scrn_out, "\nOpened Include File: %s (%d)\n",
		    inclF.file[inclF.level],
		    fileno(inclF.address[inclF.level]) );
	     inclF.level++;
	     inclfLine = 0;
	     inclFini = 1;

	     return (str_pos);
	     }
	     else *FP_ADDRESS = holdfp;
	   }
 return(0);
}

int close_include(FILE *hold_fp, FILE **fp, FILE *rptfp, char *module)
{
  inclFini = 0;
  if (reading_includeF) {
      if (inclF.level > 1 && inclF.level <= __Inclf_Limit) {
	inclF.level--;
	fprintf(scrn_out,"Closing Include: %s (%d). Returning to: %s (%d)\n",
	   inclF.file[inclF.level], fileno (inclF.address[inclF.level]),
	   inclF.file[inclF.level - 1], fileno (inclF.address[inclF.level - 1]) );
	fclose( inclF.address[inclF.level] );
	*fp = inclF.address[inclF.level - 1];
	}
	else {
	fprintf(scrn_out,
	   "Closing Include: %s (%d). Returning to Module: %s (%d)\n",
		inclF.file[0], fileno (inclF.address[0]),
		module, fileno (hold_fp) );
	inclF.level = 0;
	fclose( inclF.address[0] );
	reading_includeF = 0;
	*fp = hold_fp;

	}
     /* return 1 causes an exit from the main loop */
      return (1);
      }

 /*  cr/lf at end of module */
    fprintf(rptfp, "\n");
    paginate(rptfp);
    paginate(rptfp);

   return(0);
}


/************** Utility functions for FUNC_CA ************/


void date_time(FILE *rptfp )
{
 char tmpbuf[128];
     if (declare_file) return;

    _strdate( tmpbuf );
     fprintf(rptfp, "DOS date:\t\t\t\t%s\n", tmpbuf );
     paginate(rptfp);
     paginate(rptfp);
    _strtime( tmpbuf );
    fprintf( rptfp, "DOS time:\t\t\t\t%s\n\n", tmpbuf );
    paginate(rptfp);
    paginate(rptfp);
    paginate(rptfp);
}

void heading(FILE *rptfp, char *fname)
{
 int i;

 if (declare_file) return;

 fprintf(rptfp, "        \"C-ACROSS\" CROSS-REFERENCE LISTINGS FOR: %s\n\n",
								fname);
 for (i = 0; i < 79; i++) fprintf(rptfp, "%c", '\x5f');
 fprintf(rptfp,"\n\n");

 paginate(rptfp);
 paginate(rptfp);
 paginate(rptfp);
 paginate(rptfp);

}

int countbrackets(char *buffer)
{
  int brackets = 0;
  int ischar = 0;
  char *p = buffer;
   while(*buffer) {
     if(*buffer == '\'')  {
	 if (*++buffer == '{') brackets--;
	 if (*buffer == '}') brackets++;
      }
     if(*buffer == '{') brackets++;
     if(*buffer == '}') brackets--;
     buffer++;
     }

 return (brackets);
}


char *headings[] =    {
  "I.   FUNCTION DECLARATIONS AND GLOBAL VARIABLES",
  "II.  ALPHABETICAL INDEX OF FUNCTIONS & THEIR MODULES",
  "III. FUNCTIONS (LISTED BY MODULE) AND CALLS",
  "IV.  LIST OF FUNCTIONS AND CALLING FUNCTIONS",
  "V.   FUNCTION POINTERS AND CALLING FUNCTIONS",
  "VI.  GLOBAL VARIABLES LISTED ALPHABETICALLY"
};
void print_headings(int head_no, FILE *rptfp )
{
  int line_count;

   if(head_no != 0) {
   line_count = paginate(rptfp);
   formfeed(line_count, rptfp);
   }

   fprintf(rptfp,"\n%s\n\n", headings[head_no]);
   paginate(rptfp); paginate(rptfp); paginate(rptfp); paginate(rptfp);

}

/************** Utility functions for Tokens ************/

char * isolate_token(char * p, char **end_token)
{
  char *token_ptr;

		       /* eliminate initial white space */
  while (*p == '\x20' || *p == '\x9') p++;
  token_ptr  = p;
  while ( *p != '\x20' && *p != '\x9' && *p != '(' && *p != '\0' ) p++;
  *p = '\0';      /* isolate first element in line as possible token */
  *end_token = p;
   return (token_ptr);
}


int test_token( char *token, char *type)
{
  int count = 0, i;
  char *tptr = token;

  if(*token == '\0' || *token == '\n') return(0);

     do
       {
	type++;
	token++;
	count++;
	}
     while ( (*token == *type) && *type );

     if (!(*token) && !(*type))
     return (++count);

     return (0);
}

