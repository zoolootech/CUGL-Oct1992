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
FILENAME:       TYPES_CA.C;

WARNINGS:      "1. Assumes function definitions conform with
		ANSI standards and have prototype form. See
		also "Caveats and Restrictions" in README.CA.
		2. Assumes syntactically correct source files.
		3. Written and tested using Microsoft QuickC.
		4. Copyright retained.  See Copyright
		information in README.CA.";

SEE-ALSO:      EXIT_CA.C, FUNC_CA.C, GLOB_CA.C, IFDEF_CA.C, INTF_CA.C,
	       LINKL_CA.C, PARSE_CA.C, TDEF_CA.C, UTIL_CA.C,
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <graph.h>
#include <malloc.h>
#include <ctype.h>

#define INCLUDEFILES_DEFINE
#define TYPEDEF_DEFINE
#define TDEF_EXTERNAL
#define TDEF_NUM
#define EXTERNAL
#include "keywords.h"
#include "ca.h"
#include "ca_decl.h"

#define INPARENTHESES -1

/* externs */
extern FILE *scrn_out;
extern int __mods[_MAX_MODS];
extern int declare_file ;    /* create a C declarations file: /UHF switch */
extern int In_Parentheses ;
extern int In_MainLoop ;

/******************     Begin Routines      ****************/

int data_type_ (char *test, int *__type)
{
  char *p, buffer[261], *token_ptr, structname[40], *end_token;
  int str_pos = 0, i, holdpos = 0;
  static typedef_open = 0;
  FILE * holdfp;
  size_t strL;

  *__type = NO_TYPE;
  if (In_Parentheses) return(INPARENTHESES);
  if(typedef_open) {
      typedef_open = get_typedef(test);
      return(0);
      }

  strcpy (buffer, test);
  p = buffer;
  while(iswhite(*p) ) { p++; holdpos++; }
  token_ptr = isolate_token(p, &end_token);
  if(!token_ptr) return(0);

  switch (*token_ptr)
  {

    case '#':

    if (!In_MainLoop) {
      *__type = PREPROCESSOR;   return(0);
    }

    token_ptr++;
    if( !(*token_ptr) ) {
      p = token_ptr + 1;
      token_ptr = isolate_token(p, &end_token);
      }

    if ( (str_pos = test_token(token_ptr, TYPES[DEFINE_])) )   {
		get_defines(token_ptr+str_pos);
		return(0);
		}
     if(is_include(token_ptr,holdfp)) return(0);
      break;

    case 'c':
    if ( (str_pos = test_token(token_ptr, TYPES[CHARACTER])) ) {
	     *__type = CHARACTER;
	     return (str_pos + holdpos);
	     }
    break;

    case 'd':
      if ( (str_pos = test_token(token_ptr, TYPES[DOUBLE])) ) {
	     *__type = DOUBLE;
	     return (str_pos + holdpos);
	     }
      break;

     case 'e':
      if ( (str_pos = test_token(token_ptr, TYPES[ENUM])) ) {
	     *__type =  ENUM;
	     return (str_pos);
	     }
      break;

     case 'F':
      if ( (str_pos = test_token(token_ptr, TYPES[file])) ) {
	     *__type =  file;
	     return (str_pos);
	     }
      break;


     case 'f':
       if ( (str_pos = test_token(token_ptr, TYPES[FLOAT])) ) {
	     *__type = FLOAT;
	     return (str_pos);
	     }
      break;

    case 'h':
     if ( (str_pos = test_token(token_ptr, TYPES[HUGE])) ) {
	     *__type = HUGE;
	     return (str_pos);
	     }
      break;

    case 'i':
    if ( (str_pos = test_token(token_ptr, TYPES[INTEGER])) ) {
	     *__type = INTEGER;
	     return (str_pos + holdpos);
	     }
      break;

     case 'l':
      if ( (str_pos = test_token(token_ptr, TYPES[LONG])) ) {
	    str_pos += data_type_(p + str_pos, __type);
	    *__type = LONG;
	     return (str_pos);
	     }
      break;

    case 's':
    if ( (str_pos = test_token(token_ptr, TYPES[SIGNED])) ) {
	     str_pos += data_type_(p + str_pos, __type);
	     *__type = SIGNED;
	     return (str_pos);
	     }
    if ( (str_pos = test_token(token_ptr, TYPES[SHORT])) ) {
	     *__type = SHORT;
	     return (str_pos);
	     }
    if ( (str_pos = test_token(token_ptr, TYPES[STRUCTURE])) )
	     {
	     strL = strlen(p);
	     p += strL;
	     p++;
	     while (iswhite(*p) && *p) { p++; str_pos++; }
	     while (isalnum( (int) *p ) ) { p++; str_pos++; }
	     while (iswhite(*p) && *p)
		{ p++; str_pos++; }
	     while (isalnum( (int) *p) || *p == '_')
		{ p++; str_pos++; }
	     while(is_in(*p, " *") && *p)
		{
		p++;
		str_pos++;
		}
	     *__type = STRUCTURE;
	     return (str_pos);
	     }

    if ( (str_pos = test_token(token_ptr, TYPES[SEGMENT])) ) {
	     *__type = SEGMENT;
	     return (str_pos);
	     }

       break;

     case 't':
       if ( (str_pos = test_token(token_ptr, TYPES[TYPEDEF])) ) {
	     typedef_open = get_typedef(test);
	     *__type = TYPEDEF + 100 + typedef_open;
	     return(0);
	     }
      break;

    case 'u':
    if ( (str_pos = test_token(token_ptr, TYPES[UNSIGNED])) ) {
	     str_pos += data_type_(p + str_pos, __type);
	     *__type = UNSIGNED;
	     return (str_pos);
	     }
    if ( (str_pos = test_token(token_ptr, TYPES[UNION])) ) {
	     str_pos += data_type_(p += str_pos, __type);
	     *__type = UNION;
	     return (str_pos);
	     }

    break;

    case 'v':
      if ( (str_pos = test_token(token_ptr, TYPES[VOID])) ) {
	     *__type = VOID;
	     return (str_pos);
	     }
      break;


     }

      if ( (str_pos = check_types(token_ptr, &__typenum))) {
	  if (( __typ_mod[__typenum] & 0xf) == _TYPE_) {
	     *__type = TYPEDEF;
	      return(str_pos);
	      }
	    }

     return(0);
}




int _modifier (char *test, int t_pos)
{
  char *p,  *token_ptr;
  int str_pos = t_pos, post_ptr_strpos;
  char _mod_buffer[261];
  int ptr_count = 0, mod_type = NO_TYPE, underscore = 0;

  strcpy (_mod_buffer, test + t_pos);
  p = _mod_buffer;


  post_ptr_strpos = check_pointers(p, &ptr_count);
  p += post_ptr_strpos;
  if (*p == '/'  && !In_MainLoop) return(0);
  str_pos += post_ptr_strpos;
  t_pos = str_pos;

  if (*p == '_') {  p++; underscore = 1; }
  token_ptr  = p;
  while ( *p != '\x20' && *p != '\x9' && *p != '(' && *p != '\0') p++;
  *p = '\0';      /* isolate first element in line  */

   switch (*token_ptr)
   {
    case 'b':
      str_pos += test_token(token_ptr, MODIFIERS[BASED]);
	    if(str_pos > t_pos) mod_type = BASED;
	     break;

    case 'c':
      str_pos += test_token(token_ptr, MODIFIERS[CDECL]);
	   if(str_pos > t_pos) {
	   mod_type = CDECL;
	   break;
	   }

      str_pos += test_token(token_ptr, MODIFIERS[CONSTANT]);
	   if(str_pos > t_pos)   mod_type = CONSTANT;
	     break;

    case 'e':
      str_pos += test_token(token_ptr, MODIFIERS[EXTERN]);
	   if(str_pos > t_pos) mod_type = EXTERN;
	   break;

    case 'f':
       str_pos += test_token(token_ptr, MODIFIERS[FAR]);
	   if(str_pos > t_pos)  mod_type = FAR;
	    break;
       str_pos += test_token(token_ptr, MODIFIERS[FORTRAN]);
	   if(str_pos > t_pos)   mod_type = FORTRAN;
	     break;

    case 'i':
      str_pos += test_token(token_ptr, MODIFIERS[INTERRUPT]);
	   if(str_pos > t_pos)   mod_type = INTERRUPT;
	     break;

    case 'l':
      str_pos += test_token(token_ptr, MODIFIERS[LOADDS]);
	   if(str_pos > t_pos)   mod_type = LOADDS;
	     break;

    case 'n':
      str_pos += test_token(token_ptr, MODIFIERS[NEAR]);
	   if(str_pos > t_pos)   mod_type = NEAR;
	     break;

    case 'p':
      str_pos += test_token(token_ptr, MODIFIERS[PASCAL]);
	   if(str_pos > t_pos)   mod_type = PASCAL;
	     break;

    case 's':
      str_pos += test_token(token_ptr, MODIFIERS[STATIC]);
	   if(str_pos > t_pos)   mod_type = STATIC;
	     break;
      str_pos += test_token(token_ptr, MODIFIERS[SEGNAME]);
	   if(str_pos > t_pos)   mod_type = SEGNAME;
	     break;

    case 'v':
      str_pos += test_token(token_ptr, MODIFIERS[VOLATILE]);
	   if(str_pos > t_pos)   mod_type = VOLATILE;
	     break;

   }

   if (__mods[0] && mod_type != NO_TYPE) {
      if (__mods[0] < _MAX_MODS) {
	    ptr_count <<= 8;
	  __mods[__mods[0]++] =  mod_type | ptr_count;
	  }
       }

  /* Check user defs if parsing functions; for globals this is */
  /* done in get_modifiers() in UTLG_CA */
  if (In_MainLoop) {
      int temp = 0;
      char *tp = token_ptr;
      if (underscore)
      --tp;
      if ( (temp = check_types(tp, &__typenum))) {
	  if (__typ_mod[__typenum] == _MOD_)
	      str_pos += temp;
	  }
       }

    /* check for additional modifiers */
  if (str_pos != t_pos )
     {
     p++; str_pos++;
     while (iswhite(*p) && *p) { p++; str_pos++; }

     if(!underscore) str_pos--;
     str_pos = _modifier (test,  str_pos);
     }

  if (ptr_count && mod_type == NO_TYPE)
       str_pos -= post_ptr_strpos;
  return(str_pos) ;
}


