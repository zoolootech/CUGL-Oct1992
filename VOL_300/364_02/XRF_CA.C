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
FILENAME:       XRF_CA.C;

WARNINGS:      "1. Assumes function definitions conform with
		ANSI standards and have prototype form. See
		also "Caveats and Restrictions" in README.CA.
		2. Assumes syntactically correct source files.
		3. Written and tested using Microsoft QuickC.
		4. Copyright retained.  See Copyright
		information in README.CA.";

SEE-ALSO:      EXIT_CA.C, FUNC_CA.C, GLOB_CA.C, IFDEF_CA.C, INTF_CA.C,
	       LINKL_CA.C, PARSE_CA.C, TDEF_CA.C, TYPES_CA,
	       UTIL_CA.C, UTLG_CA.C, README.CA,
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define EXIT_CODES
#include "ca.h"
#include "ca_decl.h"


/* externs and globals */
extern int function_no;
char **bsearch_start;
struct func *top_mod = NULL;     /* initialized in link_by_module() */
struct func *calling_function;   /* used in store_calls_from() in PARSE_CA */
				 /* and as argument to parse() */


/******************BEGIN ROUTINES***********/

void xrf(struct func *func_start, struct filespec *c_file_start, FILE *rptfp)
{
 struct func *func_top;
 struct filespec * files_top;
 FILE *fp;
 unsigned line_count = 0, next_function ;
 char buffer[81], filename[_MAX_FNAME];
 short last_func_in_mod;
 struct func *hold_func_start = func_start;

 if (!top_mod  || !c_file_start)  return(NULL);
 func_top = top_mod;
 files_top = c_file_start;

 bsearch_start = create_bsearch_array(func_start);

 print_headings(2, rptfp );

 fp = ini_cfile(c_file_start->path, rptfp,  func_top->line_no,  &line_count);


 while(func_top && files_top) {

    last_func_in_mod = 0;

      fprintf(rptfp,"\n%6u: %-25s (%s)\n", func_top->line_no,
					 func_top->name,
					 func_top->module);
       paginate(rptfp); paginate(rptfp);
       calling_function = func_top;
       func_top = func_top->next_function;

    /*since func_top is one function ahead, and calling_function         */
    /* is current function:  when a new module comes up func_top->module */
    /*  will point to it>  Also, there will not be a next function       */
    /* in current module:  therefore, line number of 'next_function'     */
    /* LAST_FUNCTION                                                     */

      if (files_top->path)
	if (files_match(files_top->path, func_top->module)
					 && !func_top->fnptr)
	     next_function =  func_top->line_no;
	     else {
	       next_function = LAST_FUNCTION;
	       last_func_in_mod = 1;
	       if (func_top)
		 if(func_top->fnptr) func_top =  skip_fnptrs(func_top);
	       }

      parse(fp, rptfp, calling_function->endfunc_line,
				next_function, &line_count);

     if (last_func_in_mod) {
	     fclose(fp);
	     if (files_top->next->path) {
		     files_top = files_top->next;
		     if (!files_top || !func_top)
			nullptrxit("Gap in array", __FILE__, __LINE__);
		     line_count = 0;
		     fp = ini_cfile(files_top->path, rptfp,
					    func_top->line_no,  &line_count);
					}
					else files_top = NULL;
			  }
	     }



}

/* creates list of functions in order of appearance in files:  */
/* uses 'next_function'                                         */
void link_by_module(struct func *function)
{
 static struct func *previous = NULL;

 if (!top_mod) {
     top_mod = function;
     previous = function;
     return;    }

 function->next_function = NULL;
 previous->next_function = function;
 previous = function;
}

static int files_match(char *path, char *module)
{
   char drive[_MAX_DRIVE], dir[_MAX_DIR];
  char fname[_MAX_FNAME], ext[_MAX_EXT] ;

   _splitpath(path, drive, dir, fname, ext );
   if ( strcmp(fname, module) ) return (0);

   return (1);


}

/* Open source file and move to first function [*count < func_line]; */
/* count is line count and func_line the line number of first function. */
static FILE *ini_cfile(char *path, FILE *rptfp, unsigned func_line,
							unsigned *count)
{
 char buffer[261];
 FILE *fp;

 printf ("Third Pass: %s\n", path);
 fp = open_c_file(path, rptfp);
 while (  (fgets (buffer, 160, fp)) && ++(*count) < func_line) ;

 return (fp);
}



int binary_search(char **item, int count, char *key)
{
   int low, high, mid;
   int result = 0;

  low = 0;
  high = count - 1;
  while (low <= high)
   {
    mid = (low + high) / 2;
    result =  strcmp(key, *(item + mid));
    if (result < 0) high = mid - 1;
      else if ( result > 0) low = mid + 1;
      else
      return (mid);
   }

   return (-1);
}

 /*create array of ptrs to function names, aligned in alphabetical order,
 from linked list in which alphabeticization is by links, not by alignment
 in memory */
static char **create_bsearch_array(struct func *top)
{

   char **ptr =  NULL, **ptr_start = NULL;
     if ( !( ptr = (char **) malloc ( sizeof(char **) * function_no ) ) )
		    exit_ca (NO_BSEARCH_ARRAY, "");
     ptr_start = ptr;

    while(top)
    {
     *ptr++ = top->name;
     top = top->next;
    }

  return (ptr_start);
 }


struct func *skip_fnptrs(struct func *func_top)
{

  while (func_top &&  func_top->fnptr)
	func_top = func_top->next_function;

  return(func_top);
}


int infunclist(char *token_string)
{
 struct func *top = top_mod;

 if (!top) return (0);

 while (top)
 {
 if (!strcmp( top->name, token_string) ) return (1);
 top = top->next_function;
 }

 return (0);
}
