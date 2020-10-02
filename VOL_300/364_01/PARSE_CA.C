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
FILENAME:       PARSE_CA.C;

WARNINGS:      "1. Assumes function definitions conform with
		ANSI standards and have prototype form. See
		also "Caveats and Restrictions" in README.CA.
		2. Assumes syntactically correct source files.
		3. Written and tested using Microsoft QuickC.
		4. Copyright retained.  See Copyright
		information in README.CA.";

SEE-ALSO:      EXIT_CA.C, FUNC_CA.C, GLOB_CA.C, IFDEF_CA.C, INTF_CA.C,
	       LINKL_CA.C, TDEF_CA.C, TYPES_CA, UTIL_CA.C,
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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "ca.h"
#include "ca_decl.h"
#define EXTERNAL
#include "keywords.h"

/* externs and globals */
char *prog_line;
char token[81];
extern char **bsearch_start;
extern int function_no;
static int fnp_assignment;
extern struct func *calling_function;

extern size_t GLfpSsize;
struct GLoc_fnptr_stack
{
   char fnptr[32];
   char fn_name[32];
   char *caller;
} *GLfptrstack;
size_t GLfptrstack_Ptr = 0;
static int ini_GLstack(void);
static struct GLoc_fnptr_stack pop_fn_name(char *token);

static void process_calls_from( struct func *function, FILE *rpt,
					char *token, int current_line);
/******BEGIN ROUTINES*********/
#define DEBUG_LINE 38
void parse(FILE *fp, FILE *rpt, unsigned end_func, unsigned next_function,
						    unsigned *current_line)
{
  char buffer[261];
  int token_type = NO_TOKEN;
  char *func_name;

  fnp_assignment = 0;
  memset( buffer, '\0', 261 );
  if (next_function == LAST_FUNCTION) end_func = LAST_FUNCTION;
  /* reads from file all lines up to first line of next fuction */
  /* next_function = first line of next function */
  while (  (fgets (buffer, 160, fp))   && *current_line < next_function )
    {
     prog_line = buffer;

     (*current_line)++;
     if (*current_line == DEBUG_LINE)
		*current_line = DEBUG_LINE;
     while (*prog_line)
      {
      if (*prog_line == '\n') break;
      if(  (token_type = get_token()) ) {
	int element;

	  element = ( binary_search( bsearch_start, function_no, token) );

	  if (element > - 1 && *current_line < end_func) {
	    if (token_type == IS_TOKEN) {
	      process_calls_from((struct func *) *(bsearch_start + element),
					  rpt, token, *current_line);
	    }
	    else if (token_type == PTR_ASSIGNED)
	      if (push_fnptr(prog_line, buffer, token))
		 GLfptrstack[GLfptrstack_Ptr - 1].caller
				     = calling_function->name;
	  }

	  /* if a function ptr has been assigned and not identified in the */
	  /* preceding search, it must be a local ptr:  pop the function   */
	  /* to which it points and process it:
	  */
	  else if (fnp_assignment)
	   if (token_type == IS_TOKEN && *current_line < end_func) {
	    if ( (func_name = (pop_fn_name(token)).fn_name) != NULL) {
	     element = (binary_search(bsearch_start, function_no, func_name));
	     if (element > - 1)
	       process_calls_from((struct func *) *(bsearch_start + element),
					   rpt, func_name, *current_line);
	       }
	     }

	}

      }
   }
   (*current_line)++;

}


int get_token(void)
{
 char *temp;
 int __type;

  temp = token;

  while ( iswhite(*prog_line)) {
	if (*prog_line == '\n' || !*prog_line) return (NO_TOKEN);
	prog_line++;
			       }

  while ( isliteral(prog_line)) {
	if (*prog_line == '\n' || !*prog_line) return (NO_TOKEN);
	prog_line++;            }
  while (iswhite(*prog_line)) prog_line++;

  if ( isalpha( (int) *prog_line ) || *prog_line == '_')   {
     while ( !isdelim(*prog_line) ) {
	   if (*prog_line == '\n' || !*prog_line) return (NO_TOKEN);
	   *temp++ = *prog_line++;
				    }
     while ( iswhite(*prog_line) )
	 {
	 if (*prog_line == '\n' || !*prog_line) return (NO_TOKEN);
	  prog_line++;
	 }
     if (*prog_line == '(' || *prog_line == ';')
       {
       *temp = '\0';
       if (*prog_line == ';') return (PTR_ASSIGNED);
       return (IS_TOKEN);
       }
       else {
       *temp = '\0';
       return (NO_TOKEN);
       }

					}
      else if (*prog_line) prog_line++;   /* not alpha, so move one byte */
					  /* forward and return */
 return (0);

}

/* buffer holds the entire current line: p points to end of expression */
int push_fnptr(char *p, char *buffer, char *func_name)
{
  char token[32], *end_token = NULL, *equal_sign;
  char *function_name = NULL;
  size_t strL = 0;
  static ini = 0;
  int strpos, __type, GL_decl = 0;
  extern FILE *scrn_out;

   if (!ini)
      if ( !(ini = ini_GLstack()) ) return(0);
   if (GLfptrstack_Ptr >= GLfpSsize) {
     if (scrn_out != (FILE *)stdout) fprintf(scrn_out,
	  "Function Pointer Stack at Limit: %d\n", GLfpSsize);
     printf("Function Pointer Stack at Limit: %d\n\a", GLfpSsize);
     return(0);
     }

   if (strpos = data_type_ (buffer, &__type)) {
      char *ptr;
      buffer += strpos;
      while (*buffer != '(' && *buffer) buffer++;
      if(*buffer) {
	while (! (isalnum(*buffer) || *buffer == '_') ) buffer++;
	ptr = buffer;
	while ( isalnum(*ptr) || *ptr == '_') {
	   ptr++;
	   strL++;
	   }
	if(strL > 31) strL = 31;
	strncpy(token, buffer, strL);
	token[strL] = '\0';
	GL_decl = 1;
	}
      }

   while (p > buffer) {
   if ( *p == '=') {

      equal_sign = p;    /* save place */

      if (!GL_decl) {
	p--;
	while (isspace(*p)) p--;
	end_token = p;
	while ( isalnum(*p) || *p == '_') p--;
	p++;
	if (p < buffer) p = buffer;     /* precautionary */
	strL = (end_token - p) + 1;
	if(strL > 31) strL = 31;
	strncpy(token, p, strL);
	token[strL] = '\0';
	}
      strcpy(GLfptrstack[GLfptrstack_Ptr].fnptr , token);

      equal_sign++;
      while (isspace(*equal_sign)) equal_sign++;
      strL = 0;
      function_name = token;
      strcpy(function_name, equal_sign);

      while ( (++strL < 32)  &&
	      (isalnum(*function_name) || *function_name == '_') )
				       function_name++;
      *function_name = '\0';
      strcpy(GLfptrstack[GLfptrstack_Ptr].fn_name , token);
      GLfptrstack_Ptr++;
      fnp_assignment = 1;
      break;
      }
   p--;
   }

   return(1);
}

/*  pops name of function associated with function pointer */
static struct GLoc_fnptr_stack pop_fn_name(char *token)
{
  size_t i, element = - 1;
  struct GLoc_fnptr_stack temp = { "NULL", "NULL", '\0' } ;

  for (i = 0; i <  GLfptrstack_Ptr; i++)
      if ( !(strcmp(GLfptrstack[i].fnptr, token )) )
		  element = i;

      if (element != -1) return(GLfptrstack[element]);
      return (temp);
}


int iswhite(char c)
{
 if (c == ' ' || c == '\x9') return (1);
 return(0);
}

int isliteral(char *pgmline)
{
static int String = 0;
static int Comment = 0;

if (!Comment)
if (*pgmline == '\"' && *(pgmline - 1) != '\\') {
    if (!String) String = 1;
      else String = 0;
    return (String);
    }

if(!String)
{
if (!Comment) {
   if (*pgmline == '/' && *(pgmline + 1) == '*')
     return(Comment = 1);
   }
   else if (*pgmline == '*' && *(pgmline + 1) == '/') {
    *pgmline = ' ';   *(pgmline + 1) = ' ';
    return (Comment = 0);
    }
}

 return (String | Comment);

}


int isdelim(char c)
{
   if (is_in(c, " \\+-/*^%();:\'\"<>=|!&,") || c == '\x9' || c == '\r')
	 return (1);
   return(0);
}

int is_in(char c, char *s)
{
  while(*s) if (*s++ == c) return (1);
  return (0);
}


void store_calls_from(struct func *function)
{
  struct calls_from *previous;
  static struct calls_from *current = NULL;
  /* the calling functions are identified in xrf() */



     if (!function->first_call)
       {

       function->first_call = (struct calls_from *)
			     calloc ((size_t) 1, sizeof(struct calls_from) );
       function->first_call->caller = calling_function;
       function->first_call->next = NULL;
       return;
       }


     current =
     (struct calls_from *)
     calloc ((size_t) 1, sizeof(struct calls_from) );

    previous = function->first_call;

    while (previous->next) previous = previous->next;

    previous->next = current;
    current->caller = calling_function;
    current->next = NULL;

 }

int key_word(char *token)
{
  int i;

   for (i = 0; i < nKEY_WORDS; i++) {
    if (!strcmp(token, KEYWORDS[i])) return(1);
    }

   return(0);
}

struct func * pop_fn_address(char *func_name);
static void process_calls_from( struct func * fn, FILE *rpt,
					char *token, int current_line)
{
   char *func_name;
   int element = -1;

    fprintf(rpt,"%15u: %s\n", current_line, token);

    if (fn->fnptr) {
      fprintf(rpt,"                < Fn Ptr = ");
      if ( (func_name = (pop_fn_name(token)).fn_name) != NULL)
       element = ( binary_search( bsearch_start, function_no, func_name) );
      if (element != -1) {
	store_calls_from(fn);
	fn = (struct func *) *(bsearch_start + element);
	fprintf(rpt,"%s >\n", fn->name);
	}
	else fprintf(rpt,"Fn Name not accessible >\n");
      paginate(rpt);
     }

   store_calls_from(fn);

   paginate(rpt);

}

static int ini_GLstack(void)
{
  static ini = 0;

  if (ini == - 1)
      return(0);  /* no stack space available */

  if (!ini) {

     if ( GLfptrstack = (struct GLoc_fnptr_stack *)
	     calloc(GLfpSsize, sizeof(struct GLoc_fnptr_stack)) ) ini = 1;

      else {

       ini = -1;
       if (scrn_out != (FILE *)stdout) fprintf(scrn_out,
	 "Function Pointer Stack not available at Limit: %d\n", GLfpSsize);
       printf("Function Pointer Stack not available at Limit: %d\n\a",
								   GLfpSsize);

       return(0);
       }
     }

   return(1);
}


void Globfnptrpush(char *p, char *token)
{
  char *holdp = p;

  while (*p != '=') {
     if (*p == '\n' || !*p) return;
     p++;
     }

  GLfptrstack[GLfptrstack_Ptr - 1].caller = calling_function->name;
  push_fnptr(p, holdp, token);

}

void prn_all__fnptrs(FILE *rptfp)
{
  size_t i;

  fprintf(rptfp,"\n\n<All Function Pointers>\n. . . . . .\n"
  "Pointer Name             Function Pointed To        [Calling Function]\n");
  paginate(rptfp); paginate(rptfp);
  paginate(rptfp); paginate(rptfp);

  for (i = 0; i < GLfptrstack_Ptr; i++) {
    fprintf(rptfp,"\n%-25s %-25s  [%s]",
       GLfptrstack[i].fnptr, GLfptrstack[i].fn_name,
       GLfptrstack[i].caller ? GLfptrstack[i].caller : "NOT CALLED");
       paginate(rptfp);
   }

  fprintf(rptfp,"\n");
  paginate(rptfp);
}
