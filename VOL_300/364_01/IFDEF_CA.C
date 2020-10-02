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
FILENAME:       IFDEF_CA.C;

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
#include <malloc.h>


/* temporary storage of information on if/def defines for testing */
/* & analysis of truth values in ifdef_true() */
typedef struct if_def {
    char *def1;              /* definition name */
    char *def2;
    unsigned int boolean1;   /* truth value of keyword [ifdef=1, ifndef=0 */
    unsigned int boolean2;
    int op;                 /*  relational operator in if/def expresion: */
			    /*  either && or || */
} IF_DEF;

/* stack of if/def defines & all undefs */
typedef struct if_def_stackType {
    char *def_name;
    unsigned int def_status;   /* 0 = #undefined, 1 = #defined  */
    struct if_def_stackType *next;
} IF_DEF_STACK;
IF_DEF_STACK *start_ifdef_stack = NULL;

#define EXIT_CODES
#define IFDEFS_DEFINED

#include "ca.h"
#include "ca_decl.h"


#define EXCLUDE_IFDEF 1
#define INCLUDE_IFDEF 0
#define EXCLUDE_BLOCK -1

extern int verbose;
#define increment_levels() if_level++; elif_level = else_level = if_level

#define _AND  1
#define _OR   2

#define IFLEVEL_MAX  3
#define DEFINE_TYPES 9
enum DefineTYPES {  _DEFINED, _DEF, _IF,_ELSEIF, _ELSE,
			   _IFDEF, _IFNDEF, _UNDEF, _ENDIF };

int if_code[DEFINE_TYPES] = {    _DEFINED,  _DEF, _IF, _ELSEIF,
				 _ELSE, _IFDEF, _IFNDEF,
				 _UNDEF, _ENDIF };

char *elseif[DEFINE_TYPES] =  { "defined", "#define", "#if", "#elif",
				"#else", "#ifdef", "#ifndef",
				"#undef", "#endif"};

int is_else_if(char *p, int in_define)
{
  int def_type, found = 0, restore_str = 1;
  char *new_line = p;

 /* This function begins by testing the token created in is_includeF() */
 /* to see whether it is one of the #if/#def commands; we will */
 /* restore the original string, if a newline character is not found, by */
 /* replacing the '\0' inserted in the tokenizing function with a space. */
 /* If a newline is not found the string hasn't been altered. */

 /* First find the null or newline */
  while (*new_line != '\0' && *new_line != '\n') new_line++;
  if (*new_line == '\n') {
	   *new_line = '\0';
	   restore_str = 0;
	   }

  for (def_type = _DEF; def_type <= _ENDIF; def_type++) {
  if (found = test_token(p, elseif[def_type])) break;
  }
  if (found) {
     if (restore_str)
	     *new_line = ' ';
     if (if_code[def_type])
	  return(def_value(p, if_code[def_type]));
       else
       p = new_line++;
     }

  return(in_define);
}

static int def_value(char *p, int if_type)
{
  static int if_level = 0;
  static int elif_level = 0;
  static int else_level = 0;
  static int if_status[IFLEVEL_MAX] = { 0, 0, 0 };
  IF_DEF ifdefs;

	 switch (if_type)
	 {
	 case _IF: case _IFDEF: case _IFNDEF:
	    if (if_level == IFLEVEL_MAX) break;
	    if (if_status[if_level])
	    if(if_status[if_level - 1] == EXCLUDE_IFDEF)
		return(if_status[if_level] = EXCLUDE_BLOCK);
	 /* If_type converted in get_definition() to IFDEF or IFNDEF */
	    ifdefs = get_definition(p, if_type);
	    if (ifdefs.def1)
	      if(ifdef_true(ifdefs)) {
		  increment_levels();
		  return (if_status[if_level - 1] = INCLUDE_IFDEF);
		  }
	  increment_levels();
	  return (if_status[if_level - 1] = EXCLUDE_IFDEF);


	 case _ELSEIF:
	    if (elif_level != if_level) break;
	    elif_level--;  else_level--;
	    if (if_status[elif_level])
	    if(if_status[elif_level - 1] == EXCLUDE_BLOCK)
		  return (EXCLUDE_BLOCK);
	    if (if_status[elif_level] == INCLUDE_IFDEF)
		return (EXCLUDE_IFDEF);
	 /* If_type converted in get_definition() to IFDEF or IFNDEF */
	    ifdefs = get_definition(p, if_type);
	    if (ifdefs.def1)
	      if(ifdef_true(ifdefs))
		  return (if_status[elif_level] = INCLUDE_IFDEF);

	  return (if_status[elif_level] = EXCLUDE_IFDEF);

	 case _ELSE:
	    if (else_level != if_level) break;
	    elif_level--;  else_level--;
	    if(if_status[else_level])
	    if(if_status[else_level - 1] == EXCLUDE_BLOCK)
		  return (EXCLUDE_BLOCK);
	    if (if_status[else_level] == INCLUDE_IFDEF)
		return (EXCLUDE_IFDEF);
	    return (if_status[else_level] = INCLUDE_IFDEF);
	 case _ENDIF:
	    if_level--;
	    elif_level = else_level = if_level;
	    if_status[if_level] = INCLUDE_IFDEF;
	 break;

	 case _DEF:
	   ifdefs = get_definition(p, if_type);
	   if (ifdefs.def1) push_ifdef(ifdefs.def1, 1);
	 break;

	 case _UNDEF:
	   ifdefs = get_definition(p, if_type);
	   if (ifdefs.def1)
	      push_ifdef(ifdefs.def1, 0);
	 break;

	 }

  return(if_status[if_level]);

}
  /*                               0         1     2     3
int if_code[DEFINE_TYPES] = {    _DEFINED,  _DEF, _IF, _ELSEIF,
				   4      5        6
				 _ELSE, _IFDEF, _IFNDEF,
				    7      8
				 _UNDEF, _ENDIF };
    */

/* two preprocessor statements on one line not accepted: #else #if defined()*/
static IF_DEF get_definition(char *p, int if_type)
{
  size_t strL = 0;
  char *ptr  = elseif[if_type];
  char *end_token, *boolean = NULL;
  int boolean_found = 0, AND = 1, OR = 1;
  IF_DEF ifdefs = { "", "", 0, 0, 0 };

  char temp[100], *temp_p = temp;
  strcpy(temp, p);

  while (*ptr) {  ptr++; strL++; }
  p += strL;
  while (iswhite(*p)) p++;

  ifdefs.def1 = NULL;
  ifdefs.def2 = NULL;
  /* reject a second preprocessor statement on same line */
  if (*p == '#') return (ifdefs);

  if (if_type == _IF || if_type == _ELSEIF) {
  if ( *p == '!') {
	       p++;  ifdefs.boolean1 = 0;
	       }
	 else {
	 if_type = _IFNDEF;
	 ifdefs.boolean1 = 1;
	 }
       }
      else if (if_type == _IFDEF) ifdefs.boolean1 = 1;
      else if (if_type == _IFNDEF) ifdefs.boolean1 = 0;

  while (iswhite(*p)) p++;

  ptr = NULL;

  p = isolate_token(p, &end_token);

  if (if_type == _DEF || if_type == _UNDEF) {
       if (*(end_token - 1) != '\n') {
	  end_token++;
	  while (iswhite (*end_token)) end_token++;
	  if (*end_token != '\n') return(ifdefs);
	  }
       *(end_token - 1) = '\0';
       ifdefs.def1 = p;
       return(ifdefs);
       }

  if(*(end_token  - 1) == '\n')
	*(--end_token) = '\0';
  if (test_token(p, elseif[_DEFINED])) {
	*end_token = ' ';  /*  replace null with space */

       do {

	boolean = NULL;
	while (iswhite(*p)) p++;
	if(boolean_found)
	   if (*p == '(') p++;
	if(boolean_found) end_token = p;
	ptr = strchr(p, ')');
	if(!boolean_found) {
	   while (iswhite(*end_token)) end_token++;
	   if (*end_token == '(') end_token++;
	   }
	p = isolate_token(end_token, &end_token);
	/* normally ptr is byte preceding end_token and = ')' */
	if (ptr)  *ptr = '\0';
	if(boolean_found) ifdefs.def2 = p;
	      else ifdefs.def1 = p;

	if (!boolean_found)
	if (*(end_token - 1) != '\n')  {

	/*
	   if there are extra spaces before the closed quot      */
	/* as in:  (TYPE_DEFINED    ) get past them:  this       */
	/* means keeping in mind that there are two null bytes,  */
	/* *ptr and *end_token,  which precede the spaces:       */
	/* unless this is done isolate_token() will return empty */
	/* since it stops creating a token when it reaches a     */
	/* space
	*/
	    while ((!*end_token  || iswhite(*end_token) )
			  && ptr >= end_token ) end_token++;
	    boolean = isolate_token(end_token + 1, &end_token);
	    if ( AND = strcmp(boolean, "&&"))
		      OR = strcmp(boolean, "||");
	    if (AND == 0 || OR == 0) {
	       if (!AND) ifdefs.op = _AND; else ifdefs.op = _OR;
	       boolean_found = 1;
	       p = end_token + 1;
	       while (iswhite(*p)) p++;
	       if ( *p == '!') { p++;  ifdefs.boolean2 = 0; }
				  else ifdefs.boolean2 = 1;
	       while (iswhite(*p)) p++;
	       p += 7;  /*(strlen of "defined") */
	       }
	     }

     } while (boolean);

     }
	 /* reject non-standard "NOT" */
       else
	if (if_type == _IF &&  if_type == _IFNDEF)
				return(ifdefs);
       else
	ifdefs.def1 = p;


  return(ifdefs);
}

static int ifdef_true(IF_DEF ifdefs)
{
   unsigned int status1, status2;

   status1 = pop_ifdef_status(ifdefs.def1);
   ifdefs.boolean1 =  (ifdefs.boolean1 == status1);
   if (ifdefs.def2) {
       status2 = pop_ifdef_status(ifdefs.def2);
       ifdefs.boolean2 =  (ifdefs.boolean2 == status2);
       }
     else  {
       return (ifdefs.boolean1 );
       }



   if (ifdefs.op ==  _AND)
      return ( (ifdefs.boolean1 & ifdefs.boolean2) );

   if (ifdefs.op == _OR)
      return (ifdefs.boolean1 | ifdefs.boolean2);

   return(0);
}
extern FILE *scrn_out;
static void push_ifdef(char *p, unsigned int status)
{

   IF_DEF_STACK  *ts, *top, *previous;

    if (!start_ifdef_stack) {
       ts = (IF_DEF_STACK *) calloc(1, sizeof(IF_DEF_STACK));
       if (!ts) exit_ca(NO_IFDEF_STK,"");
       ts->def_name = (char *) calloc(1, strlen(p) + 1);
       if (!ts->def_name) exit_ca(NO_IFDEF_STK,"");
       strcpy (ts->def_name, p);
       ts->def_status = status;
       ts->next = NULL;
       start_ifdef_stack = ts;
       return;
       }


    top = start_ifdef_stack;

    while(top)
    {
     if (!strcmp(top->def_name, p)) {
	top->def_status = status;
	return;
	}
     previous = top;
     top = top->next;
     }

    ts = (IF_DEF_STACK *) calloc(1, sizeof(IF_DEF_STACK));
    previous->next = ts;
    ts->next = NULL;
    ts->def_name = (char *) calloc(1, strlen(p) + 1);
    if (!ts->def_name) exit_ca(BAD_IFDEF_STK,"");
    strcpy (ts->def_name, p);
    ts->def_status = status;

}


static int pop_ifdef_status(char *p)
{
 IF_DEF_STACK  *top = start_ifdef_stack;

  if(verbose) fprintf(scrn_out, "\nSEARCHING FOR: %s\n", p);

   while (top)
    {
     if (!strcmp(p, top->def_name))  {
	if(verbose)
	   fprintf(scrn_out, "FOUND & POPPED: %s, %d\n", top->def_name,
					    top->def_status);
	   return (top->def_status);
	   }
     top = top->next;
     }

    if(verbose)
      fprintf(scrn_out, "Could not Find IF/DEF name: %s\n", p);

  if(verbose) fprintf(scrn_out, "\n");
   return (0);
}

void ini_if_def_stack(void)
{
  start_ifdef_stack = NULL;
}

void free_if_def_stack(void)
{
  IF_DEF_STACK  *current = start_ifdef_stack, *next;



   fprintf(scrn_out, "\n\n#UNDEFS & BOOLEAN DEFINES WITH TRUTH VALUES\n");
    while (current)
    {
     fprintf(scrn_out, "%s, %d\n", current->def_name, current->def_status);
     next = current->next;
     free(current);
     current = next;
     }

    fprintf(scrn_out, "\n");
}

int skipifdefn(char *p, int skipdef_status)
{
   char  buffer[160], *token_ptr= NULL, *end_token = NULL;
   extern IfDefOFF;

   while (iswhite (*p)) p++;
   if (*p != '#') return (skipdef_status);
   strncpy(buffer, p, 159);
   buffer[158] = '\0';
   token_ptr = isolate_token(buffer, &end_token);
   if(!token_ptr) return(skipdef_status);

   if (!IfDefOFF) skipdef_status = is_else_if(buffer, skipdef_status);
   return(skipdef_status);
}
