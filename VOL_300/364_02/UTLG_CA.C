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
FILENAME:       UTLG_CA.C;

WARNINGS:      "1. Assumes function definitions conform with
		ANSI standards and have prototype form. See
		also "Caveats and Restrictions" in README.CA.
		2. Assumes syntactically correct source files.
		3. Written and tested using Microsoft QuickC.
		4. Copyright retained.  See Copyright
		information in README.CA.";

SEE-ALSO:      EXIT_CA.C, FUNC_CA.C, GLOB_CA.C, IFDEF_CA.C, INTF_CA.C,
	       LINKL_CA.C, PARSE_CA.C, TDEF_CA.C, TYPES_CA, UTIL_CA.C,
	       XRF_CA.C, README.CA,
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

#define CLOSE_PAREN (int) ')'
#define COMPLEX_DECL CLOSE_PAREN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

#define TYPEDEF_DEFINE
#define TDEF_EXTERNAL
#define EXTERNAL
#define EXIT_CODES
#include "ca.h"
#include "ca_decl.h"
#include "keywords.h"

/* externs and globals */
int __mods[_MAX_MODS];
struct user_typeStack *start_user_stack = NULL;

/*********Utilities for GLOBL_CA and for handling GLOBALS******************/


int is_prototype(char *p)
{
  char *hold_p = p;


     while(iswhite(*p)) p++;

     while (*p == '*') p++;
     while(iswhite(*p)) p++;

     while (*p && !isdelim(*p) )
     {
     p++;
     while(iswhite(*p)) p++;
     while (*p == '*') p++;
     while(iswhite(*p)) p++;
     }


     return ((int)*p);
}


int get_dimensions(char *ptr, int count)
{
    int k = 0;
	while (*ptr && *ptr != ';') {
	while (*ptr && *ptr != ']') { ptr++; k++; }
	if (*ptr == ']') { ptr++; k++; }
	while (*ptr && iswhite(*ptr)) ptr++;
	if(*ptr != '[') break;
	}
	return(count + k);
}



char * white_out(char *token)
{
 char *temp_from = token, *temp_to = token;
 int i = 0;


  do
  {
   if (!iswhite(*temp_from) )
       *temp_to++
	   = *temp_from;
  }
   while(*temp_from++);

  *temp_to = '\0';

  return(token);
}

int is_variable(char *token)
{
 int i;

 while(*token == '*') token++;
 while(iswhite(*token)) token++;
 if (key_word(token) || _modifier (token, 0) || check_types(token, &i) )
			    return(0);

 return(1);
}


int check_pointers(char *p, int *ptr_count)
{
  int str_pos = 0;


  while(iswhite(*p))
  {
  p++;
  str_pos++;
  }

  while (*p == '*') {
   p++;
   (*ptr_count)++;
   str_pos++;
   }
  while(*p && iswhite(*p)) { p++; str_pos++; }

  return(str_pos);
}


  /* test variables string to locate sequences of modifiers, both */
  /* user-defined and C-language based */

char *get_modifiers(int final_check, char *ptr, int mods[_MAX_MODS])
{
  int ptr_count, str_pos = 0, mod_count = 0 ;
  char *pre_ptr_check, *ptr_to_null = ptr, *end_token;
  int user_type_found, stack_pos, type_id_number;

   if (!__mods[0]) __mods[0] = 1;

   while (*ptr_to_null)
   {
      ptr_to_null++;
    }

   do
   {
   pre_ptr_check = ptr;
   ptr_count = 0;
   user_type_found = 0;
   ptr += check_pointers(ptr, &ptr_count);
   /* create null terminated token to test in check_types() */
   ptr = isolate_token(ptr, &end_token);

   /* restore token to original string if not a user defined modifier*/
   if ( !(str_pos = check_types(ptr, &type_id_number)) ) {
     if (end_token != ptr_to_null)
	   *end_token = ' ';
     ptr =  pre_ptr_check;
     }
     else {
     /* if user defined type modifier found */
      user_type_found = 1;
      if((__typ_mod[type_id_number] & 0xf) != _TYPE_) {
	stack_pos = push_usertype(__type_def[type_id_number]);
	if (__mods[0] < _MAX_MODS)  {
	    ptr_count <<= 8;
	  __mods[__mods[0]++] =  (stack_pos | ptr_count) | 0x2000;
	  }
       }
       else {
	 if (!final_check) mods[0] = TYPEDEF;
	 /* restore token to original string if a type & not a modifier*/
	 if (end_token != ptr_to_null)
	     *end_token = ' ';
	 ptr =  pre_ptr_check;
	 }

      ptr += str_pos;
     }
   /* check for C modifier */
   mod_count = __mods[0];
   str_pos = _modifier(ptr, 0);
   if ((mod_count == __mods[0]) && str_pos) str_pos = 0;

   if (str_pos) ptr += str_pos;

   }
   while (str_pos || user_type_found);

  if (final_check != FINAL_MODS_CHECK)  {
     if (__mods[0] == 1) __mods[0] = 0;
     return(ptr);
     }

  for (mod_count = 0; mod_count < __mods[0]; mod_count++)  {
     mods[mod_count] = __mods[mod_count];
    }

  __mods[0] = 0;
  return(ptr);
}

/*struct user_typeStack start_user_stack;*/

int push_usertype(char * user_type)
{
   int count = 0;
   struct user_typeStack *ts, *top, *previous;

    if (!start_user_stack) {

       ts = (struct user_typeStack *)
				calloc(1, sizeof(struct user_typeStack));
       if (!ts) exit_ca(NO_USER_TYP_STK, "");
       ts->type = (char *) calloc(1, strlen(user_type) + 1);
       if (!ts->type) exit_ca(NO_USER_TYP_STK, "");
       strcpy (ts->type, user_type);
       ts->next = NULL;
       start_user_stack = ts;
       return (count);
       }


    top = start_user_stack;

    while(top)
    {
     if (!strcmp(top->type, user_type)) return (count);
     previous = top;
     top = top->next;
     count++;
     }

    ts =  (struct user_typeStack *) calloc(1, strlen(user_type) + 1);
    previous->next = ts;
    ts->next = NULL;
    ts->type = (char *) calloc(1, strlen(user_type) + 1);
    if (!ts->type) exit_ca(BAD_USER_TYP_STK, "");
    strcpy (ts->type, user_type);
    return (count);

}


char *pop_usertype(int type_number)
{
 struct user_typeStack  *top = start_user_stack;
 int count = 0;

    while (top)
    {
     if (type_number == count) return (top->type);
     top = top->next;
     count++;
     }

   return (NULL);
}


/* checks string immediately after type specifier */
/* makes assumption that open parenthesis following type specifier */
/*and its modifiers will be a complex declaration */

extern int __Complex_Declarators;
extern int __Complex_Prototypes;

int complex_value(char *p);
int is_identifier(char **p);
int is_complex_decl(char *p)
{
  char *closed_paren = NULL;
  int str_pos = 0;

  if(!__Complex_Declarators) return (NO_COMPLEX);

  while(iswhite(*p) || *p == '*' || *p == '}' ) p++;


  if (*p != '(') {
      str_pos =_modifier (p, 0);
      if (str_pos) {
	  p += str_pos;
	  while(iswhite(*p) || *p == '*') p++;
	  }
     }

  if (*p != '(') return(NO_COMPLEX);

  return(complex_value(p));
}

static char character[128];
#define c_ident  1
void ini_characterset(void)
{
 char *p =  character;
 memset (p, 127, 128);
 p += 48;
 memset (p, c_ident, 10 );
 p += 17;
 memset (p, c_ident, 26 );
 p += 32;
 memset (p, c_ident, 26 );

 character['_'] =   c_ident;
 character['*'] =  '*';
 character['['] =  '[';
 character[']'] =  ']';
 character['('] =  '(';
 character[')'] =  ')';
 character['('] =  '(';
 character[' '] =  ' ';
 character['\t'] =  '\t';
 character[';'] =  ';';
 character['\0'] =  '\0';

}

int complex_value(char *p)
{
 int ptrs = 0, paren = 0, identifier = 0, array = 0;
 int pointer_decl = 0, inner_paren = 0, ptr_mods = 0;
 int function_decl = 0, nesting_levels = 0, array_levels = 0;
 int hold_levels = 0;
 char *holdp = p;


    do
    {

       switch (character[*p])
       {
       case '*':
	 ptrs++;
	 ptr_mods++;
	 break;

       case  '(':
	if(function_decl && !pointer_decl)
		      return (NO_COMPLEX);
	if (paren) inner_paren++;
	 paren++;
	 if (paren > inner_paren && identifier)
	     function_decl = 1;
	 ptrs = 0;
	 nesting_levels++;
	 break;

       case '[':
	 if ( ( nesting_levels> hold_levels ) ||
	      ( nesting_levels  && !inner_paren) )
						array_levels++;
	 array++;
	 hold_levels = nesting_levels;
	 if (array_levels) break;
	 if(function_decl && !pointer_decl) return (NO_COMPLEX);
	 break;


       case  c_ident:
	 if ((pointer_decl || identifier) ) break;
	 identifier = is_identifier(&p);
	 break;

       case ']':
       case ' ': case '\t':
	 break;

       case '\0': case ';':
	 /*  "A 'complex' declarator is an identifier qualified by more */
	 /*  than one array, pointer or function modifier."             */
	 /*                             --Microsoft Language Reference  */
	 if(function_decl && pointer_decl) return (COMPLEX_FN);
	 if (ptr_mods < 2 && array == 0) return (NO_COMPLEX);
	 if (pointer_decl) return (COMPLEX_VAR);
	 /* accept array declarations treated as complex */
	 if (array_levels) return (COMPLEX_VAR);
	 return (NO_COMPLEX);

       case ')':
	 paren--;
	 if (inner_paren) inner_paren--;

	/* let fall through */

      default:
	       if (!inner_paren && !pointer_decl) {
		    if (ptrs && identifier) pointer_decl = 1;
		    }


      break;
     }
    }
     while (*(++p));


    return (NO_COMPLEX);
}

int is_identifier(char **p)
{
  int count = 0, __type = 0;
  char token[32];
  char *token_ptr = token;

   while ( **p && count <32) {
	 if (iswhite(**p) || **p == ')' || **p == '[' || **p == '(') break;
	 *token_ptr++ = **p;
	 (*p)++;
	 count++;
	}
   (*p)--;  /* back-up so that current char is not lost when while loop */
	    /* increments pointer p in complex_value()    */
   *token_ptr = '\0';
   token_ptr = token;

   if (is_variable(token) ) {
     data_type_ (token, &__type);
     if (__type == NO_TYPE) return (1);
     }

    return(0);
}
