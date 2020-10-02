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
FILENAME:       EXIT_CA.C;

WARNINGS:      "1. Assumes function definitions conform with
		ANSI standards and have prototype form. See
		also "Caveats and Restrictions" in README.CA.
		2. Assumes syntactically correct source files.
		3. Written and tested using Microsoft QuickC.
		4. Copyright retained.  See Copyright
		information in README.CA.";

SEE-ALSO:      EXIT_CA.C, FUNC_CA.C, GLOB_CA.C, IFDEF_CA.C, INTF_CA.C,
	       LINKL_CA.C, PARSE_CA.C, TDEF_CA.C, TYPES_CA, UTIL_CA.C,
	       UTLG_CA.C, XRF_CA.C, README.CA,
	       CA.H, CA.PRJ, CA.RPT, CDECL_CA.H, KEYWORDS.H;
AUTHORS:       Myron Turner;
COMPILERS:     Microsoft C;

*/

/* **************************  C-ACROSS  **************************
			       V. 1.02
		       Copyright (C) Myron Turner

			  333 Bartlet Ave.
			  Winnipeg, Manitoba
			  Canada R3L 0Z9
			  (204) 284-8387

 ******************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void nullptrxit(char *msg, char *filename, unsigned lineno);
static void prn_exitmsg(FILE *output, char const *function, char const *module,
			char const *exit_msg, char *exit_str, int exit_code);

static const char *exit_msg[] =
 {
   /* IFDEF_CA */
   "cannot inititiate ifdef stack",           /* 0 */
   "cannot continue ifdef stack",             /* 1 */

  /* UTLG_CA */
   "cannot initiate user type stack",         /* 2 */
   "cannot continue user type stack",         /* 3 */

  /* INTF_CA  */
   "unable to allocate file buffer",          /* 4 */ /* /get_new_path */
   "cannot allocate \"file_string\"",         /* 5 */  /* get_modules */

   "problem in command line arguments",       /* 6 */  /* argv_cat */
   "unable to open report file: ",            /* 7 */   /* make_rpt_file */
   "no source files found in project file",   /* 8 */   /* main */

  /* XRF_CA  */
   "unable to create binary search array",   /* 9 *//* create_bsearch_array*/
   "Null Pointer Error"                      /* 10 */
   };

static const char *FUNCTION[] = {
   "push_ifdef",           /* 0 */
   "push_ifdef",           /* 1 */
   "push_usertype",        /* 2 */
   "push_usertype",        /* 3 */
   "get_new_path",         /* 4 */
   "get_modules",          /* 5 */
   "argv_cat",             /* 6 */
   "make_rptfile",         /* 7 */
   "main",                 /* 8 */
   "create_bsearch_array", /* 9 */
   "xrf"                   /*10*/
   };


static const char *MODULE[] = {
   "IFDEF_CA",       /* 0 */
   "UTLG_CA",        /* 1 */
   "INTF_CA",        /* 2 */
   "XRF_CA"          /* 3 */
   };


enum {
	NO_IFDEF_STK, BAD_IFDEF_STK,         /* IFDEF_CA */   /* memory */

	NO_USER_TYP_STK, BAD_USER_TYP_STK,   /* UTLG_CA */    /* memory */

	BAD_FILE_BUF, NO_FILE_STR,           /* INTF_CA */    /* memory */
	BAD_ARGV,  BAD_FILE, BAD_PROJ,                        /* files  */

	NO_BSEARCH_ARRAY,                    /* XRF_CA  */    /* memory */
	NULL_POINTER
	};

extern FILE *scrn_out;
void exit_ca(int exit_code, char *exit_str)
{

   const char *module = NULL;
   char *ptr;
   size_t pos;

       switch (exit_code)
       {
	 case NO_IFDEF_STK: case  BAD_IFDEF_STK:
	 /* IFDEF_CA */
	   module =     MODULE[0];
	   break;

	 case NO_USER_TYP_STK: case BAD_USER_TYP_STK:
	   /* UTLG_CA */
	   module =     MODULE[1];
	   break;

	 case BAD_FILE_BUF: case NO_FILE_STR: case BAD_ARGV:
	 case BAD_FILE:  case BAD_PROJ:
	 /* INTF_CA */
	   module =     MODULE[2];
	   break;

	 case NO_BSEARCH_ARRAY:
	   /* XRF_CA */
	   module =     MODULE[3];
	   break;

	 case NULL_POINTER:
	   ptr = exit_str;
	   pos = strcspn(exit_str, ". ");
	   exit_str = strchr(exit_str, ' ');
	   *(ptr + pos) = '\0';
	   strupr(ptr);
	   module = ptr;
	   break;
       }
      printf ("\a");
      prn_exitmsg((FILE *)stdout, FUNCTION[exit_code], module,
	  exit_msg[exit_code], exit_str, exit_code);

      if(scrn_out != (FILE *)stdout)
       prn_exitmsg(scrn_out, FUNCTION[exit_code], module, exit_msg[exit_code],
	   exit_str, exit_code);

       exit(exit_code);

 }

static void prn_exitmsg(FILE *output, char const *function, char const *module,
			char const *xit_msg, char *exit_str, int exit_code)
{

     fprintf(output, "\n***");
     if (exit_code <= NO_FILE_STR || exit_code == NO_BSEARCH_ARRAY)
       fprintf(output, "Out of memory:\n  ");

     if (exit_code != BAD_FILE)
       fprintf(output,"%s\n%s   function: %s()   [module: %s.C]\n",
		 xit_msg, exit_str, function, module);
     else
     if (exit_code == BAD_FILE) {
       strupr (exit_str);
       fprintf(output,"%s\"%s\"\n    function: %s()  [module: %s.C]\n",
	      xit_msg, exit_str, function, module);
	      }
}

void nullptrxit(char *msg, char *filename, unsigned lineno)
{
    char buffer[81];

  sprintf(buffer, "%s %s:\n  Line: %d",  filename, msg, lineno);
  exit_ca(NULL_POINTER, buffer);
}

