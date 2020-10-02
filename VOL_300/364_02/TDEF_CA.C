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
FILENAME:       TDEF_CA.C;

WARNINGS:      "1. Assumes function definitions conform with
		ANSI standards and have prototype form. See
		also "Caveats and Restrictions" in README.CA.
		2. Assumes syntactically correct source files.
		3. Written and tested using Microsoft QuickC.
		4. Copyright retained.  See Copyright
		information in README.CA.";

SEE-ALSO:      EXIT_CA.C, FUNC_CA.C, FUNC_CA.C, IFDEF_CA.C, INTF_CA.C,
	       LINKL_CA.C, PARSE_CA.C, TYPES_CA, UTIL_CA.C,
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


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define TYPEDEF_DEFINE
#define TDEF_CA
#include "ca.h"
#include "ca_decl.h"

int get_typedef(char *test)
{
   char *open_brace = NULL;
   char *token;
   static int tdef_open = 0;


   if (!tdef_open)
      if ( !(open_brace =  strchr(test, '{')) )
		tdef_open = tdeftest(test, tdef_open);
		else tdef_open = 1;

    if (open_brace || tdef_open)
       if ( strchr(test, '}') )
	       tdef_open = tdeftest(test, tdef_open);


   return(tdef_open);
}


int tdeftest(char *test, int tdef_open)
{
   char *semi_colon = NULL, hold_char = '\0';
   char *token, *comma = NULL;
   int i, str_pos, complex = 0;
   char *paren = NULL;
   char *temp = test;

      if ( (semi_colon = strchr(test, ';')) ) {
	  if (!tdef_open) {
	    test += 7;    /* skip over 'typedef' key word */
	    while(iswhite(*test)) test++;
	    test += data_type_ (test, &i);


	    do
	      {
	       while(iswhite(*test) || *test == '*' || *test == '(' ) {
		     if (*test == '(' ) if (!paren) paren = test;
		     test++;
		     }
	       str_pos = _modifier(test, 0);
	       test += str_pos;
	      }
	      while (str_pos);
	    }
	  do
	  {
	   complex = NO_COMPLEX;
	   if (paren)
	     complex   =  complex_value(paren);
	   while(!isalpha(*test) && *test != '_') test++;
	   token = test;
	   while(isalnum(*test) || *test == '_') test++;
	   hold_char = *test;
	   *test = '\0';
	   if ( (i =insert_type(token)) != -1 )
	    __typ_mod[i] = _TYPE_ | complex;
	    *test = hold_char;
	    if(comma = strchr(test, ',') ) {
	       test = comma + 1;
	       paren = strchr(test, '(');
	       }
	       else break;
	   }
	   while(test != semi_colon);
	   return(0);
	   }

    return(1);
 }


int check_types(char *token_ptr, int *typenum)
{
  int i, str_pos = 0;

       if(!*token_ptr) return(0);

       for  (i = 0; i < __typedef_count; i++){
	 if(*token_ptr == __type_def[i][0])
	  if ((str_pos = test_token(token_ptr, __type_def[i]))) {
	    *typenum = i;
	    return(str_pos);
	    }
	    }

   return(0);
 }


int intypeslist(char *token)
{
  int i = 0;

  if(!*token) return(1);
  for (; i < __typedef_count; i++)
     if ( __type_def[i][0] == *token )
       if (!strcmp(__type_def[i], token)) return (1);

  return(0);
}


int get_defines(char *test)
{
   char token[32], *end_token, *p ;
   size_t strL;
   int __type;
   int i;

   p = token;

   strL = strlen(test);
  *(test + strL -1)  = '\0';
   while( iswhite( *test) ) test++;
   strncpy(token, test, 31);
   end_token = token;

   while( !(iswhite( *test)) && *test != '\n' && *test ) {
	 test++; end_token++;
	 }

   if (!*test || *test == '\n') return(0);

   *(end_token) = '\0';
   while( iswhite( *test) || *test == '*' )             {
		    test++;
		    }

   if (data_type_(test, &__type)) {
	 if ( (i =insert_type(token)) != -1 ) {
	     __typ_mod[i] = _TYPE_;
	  return(i);
	  }
     }

   if(_modifier (test, 0)) {
	if ( (i =insert_type(token)) != -1 ) {
	  __typ_mod[i] = _MOD_;
	  return(i);
	  }
      }

   return(0);
}

int insert_type(char *token)
{
     if (token && __typedef_count < __TYPEDEFS) {
	if(!intypeslist(token)) {
	   strncpy(__type_def[__typedef_count], token, 31);
	  return(__typedef_count++);
	 }
	 }   else printf ("\aOut of typedef space: %s\n", token);

  return(-1);
}
