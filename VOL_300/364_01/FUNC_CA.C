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
FILENAME:       FUNC_CA.C;

WARNINGS:      "1. Assumes function definitions conform with
		ANSI standards and have prototype form. See
		also "Caveats and Restrictions" in README.CA.
		2. Assumes syntactically correct source files.
		3. Written and tested using Microsoft QuickC.
		4. Copyright retained.  See Copyright
		information in README.CA.";

SEE-ALSO:      EXIT_CA.C,  GLOB_CA.C, IFDEF_CA.C, INTF_CA.C,
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
#include <ctype.h>
#include <stdlib.h>
#include <graph.h>

#define TYPEDEF_DEFINE
#define TDEF_EXTERNAL
#define TDEF_NUM
#define INCLUDEFILES_DEFINE
#include "keywords.h"
#define TESTRUN
#include "ca.h"
#include "ca_decl.h"
#define FLAG_S static short
#define UINT unsigned

#define OPEN_CURLY '{'
extern FILE *scrn_out;
extern int declare_file ;    /* create a C declarations file: /UHF switch */
extern int __mods[_MAX_MODS];
extern int reading_includeF;
extern UINT int inclfLine;
extern UINT int inclFini;

int skipifdefn(char *p, int skipdef_status);
FLAG_S functionstart(char **hold_p, char **hold_tptr);
int func_def(char *p, char *module, struct func **top,
				   char *big_buf, FILE *rptfp);

static struct func *syntax(char *p, char * big_buf, char *module,
				 struct func *top,  FILE *rptfp);

FILE **FP_ADDRESS;
int function_no = 0;
int In_Parentheses = 0;
int In_MainLoop = 0;
struct func *function = NULL, *FirstFunction = NULL;
struct includeFILE inclF;

#define BIGBUFSIZE  350 + 261
#define mod_type_check_off()   __mods[0] = 0
#define INPARENTHESES -1


/*   Group of variables used by several functions in this module:  the     */
/*   gloabls eliminate need to pass excessive number of function arguments */
/*   for variables which are needed in and changed by several functions    */

static char *routine;
FLAG_S infunction = 0;
FLAG_S open_parenthesis = 0;   /* = possible function */
FLAG_S buffering = 0;   /* = more than one input line in possible function */
FLAG_S inner_paren= 0;  /* track of parentheses within main parenthetical  */
static int brackets = 0;
static UINT endfunction = 0;
static struct func *previous =  NULL;
static UINT bufring_line_held=0;  /* holds 1st lineno of funct'n def when */
				 /* definition takes up more than one line */
FLAG_S inliteral = 0;
static UINT int line_count = 0,  lineno;
static char *token_ptr;
static int t_pos = 0, m_pos = 0;
static char buffer[261];

static void ini_Globals(void);

#define DEBUG_LINE 13
char *debugptr;
   /**********************FUNCTIONS******************************/

  /* Main Loop for first pass */
struct func *main_loop(FILE *fp, FILE *rptfp, char *module)
{
 static struct func *top = NULL;
 char  *p, big_buf[BIGBUFSIZE];
 int  __type, skip_ifdef = 0;
 FILE * hold_fp = NULL;

 ini_Globals();
 FP_ADDRESS = &fp;
 hold_fp = fp;

 memset( big_buf, '\0', BIGBUFSIZE );
 ini_if_def_stack();

  _clearscreen(0);
  relocate();
  if (scrn_out != (FILE *)stdout) fprintf(scrn_out,
		       "\n**********\n\nFirst Pass: %s\n", module);
  printf("First Pass: %s\n", module);



/* Main loop looks for the first indicator of a function definition,    */
/* which is the type specifier.  It also handles comments and strings,  */
/* keeps count of curly braces, in order to determine end of             */
/* function body.                                                        */

startmainloop:

  while (  fgets (buffer, 160, fp) )
  {

      p = buffer;
      debugptr = p;
      if (!reading_includeF) line_count++;
	 else
	 inclfLine++;
	 if (line_count == DEBUG_LINE)
				   line_count = DEBUG_LINE;

	 if (inclfLine == DEBUG_LINE)
				   inclfLine = DEBUG_LINE;

      skip_ifdef = skipifdefn(p, skip_ifdef);
      if (skip_ifdef) goto startmainloop;


      /* count curly brackets to determine ending line of function body    */
      /* infunction is set to 1 in func_def(), when a function definition  */
      /* is found.                                                         */
      if (infunction && *p != '\n') {
	   brackets += countbrackets(buffer);
	   if (!brackets)
	       endfunction = line_count;
	   if (!brackets) infunction = 0;
	   }

      /* handle comments and strings */
      do {
       if ( (inliteral = isliteral(p)) ) p++;
       } while ( *p && inliteral );

      if (!inliteral)
	if(buffering && open_parenthesis) In_Parentheses = 1;
	while (iswhite(*p)) p++;

	 /* move through modifiers;  */
	if (!buffering) { m_pos = _modifier (p, 0); p += m_pos; }

	/*  if next element in line is a data type check for function def */
	if ( (t_pos = data_type_(p, &__type)) || open_parenthesis)
	    top = syntax(p, big_buf, module, top, rptfp);


   }

  /* check to see whether we are in an include file: if so, go back to */
  /* start of the main_loop                                            */
  if (close_include(hold_fp, &fp, rptfp, module))
			goto startmainloop;

   print_userdefs(rptfp, module);

  if (previous) previous->endfunc_line = endfunction;
  free_if_def_stack();
  return (top);
}


static void make_function_token(char *token_ptr, char *function_name)
{
  char *p = token_ptr, *function_ptr = function_name;

  while (*p == '\x20' || *p == '\x9' || *p == '*') p++;

 while (  *p && *p != ' ' && *p != '('  )
     *function_ptr++ = *p++;
     *function_ptr = '\0';

}


static void print_userdefs(FILE *rptfp, char *module)
{

  int i, j = 0;
  char *tm[] = {"",  "Modifier", "Type" };

  fprintf(scrn_out, "\nUSER DEFINED TYPES AND MODIFIERS IN: %s ", module);
  for (i = 0; i < __typedef_count; i++) {
    j = (__typ_mod[i] & 0xf);
    fprintf(scrn_out,"\n%d: %s [ %s ]", i, __type_def[i], tm[j]);
    }

   fprintf(scrn_out, "\n");
   fprintf(scrn_out, "\n");

}

#define PTR_to_FN  !In_MainLoop
struct func *fill_func_struct(char *token_ptr, int lineno, char *module)
{
  static struct func *top = NULL;

	function = (struct func *)
		 malloc ( sizeof (struct func) );
	function->line_no = lineno;
	/* list functions in order of appearance in files */
	link_by_module(function);
	function->first_call = NULL;
	make_function_token(token_ptr, function->name);
	strcpy (function->module, module);
	if (PTR_to_FN) function->fnptr = 1;
		  else function->fnptr = 0;
	top = store_linked_list(function, top);
	function_no++;

	 if (!FirstFunction) FirstFunction = function;
  return(top);
}



/* This function does syntactical analysis of the input line.            */
/* If line includes possible function definition,  it is sent to         */
/* func_def() to determine whether it holds a function definition.       */

static struct func *syntax(char *p, char * big_buf, char *module,
					 struct func *top,  FILE *rptfp)
{
  char *hold_p = NULL, *hold_tptr = NULL;
  static int chars_to_fn_name = 0;


    /* if we are inside parentheses, we are using the big buffer for */
    /* a multiple line function definition: so we add chars_to_fn_name */
    /* to pointer to beginning of buffer (routine) to move forward the */
    /* token_ptr, which must point to the name of the function   */
    if (t_pos == INPARENTHESES) {
	     token_ptr = routine + chars_to_fn_name;
	     t_pos = 0;
	     } else chars_to_fn_name = 0;

    if (!buffering)  open_parenthesis = 0;
    /* if big-buffering and not inside a possible function's parentheses */
    if (!open_parenthesis)
    {

       hold_p = p + t_pos;  /* get past type spec */
       hold_p += _modifier (hold_p, 0);  /* get past modifiers */
       hold_tptr = hold_p;   /* save current position in line  */
			     /* for calculating chars_to_fn_name below */

      /* Look for first open parenthesis in functionstart() as sign of */
      /* possible function definition                                  */
       open_parenthesis = functionstart(&hold_p, &hold_tptr);

       if (open_parenthesis && !(isalnum(*hold_tptr) || *hold_tptr == '_') )
       {
	/* step back if space(s) separates function name from '(' or */
	/* from a preceding modifier */
	while(iswhite(*hold_tptr) && hold_tptr > p + t_pos) hold_tptr--;
	while( isalnum(*hold_tptr) || *hold_tptr == '_'
				     && hold_tptr > p + t_pos) hold_tptr--;
	if(*hold_tptr == ' ') hold_tptr++;
	/* makes sure we haven't stepped back into a modifier*/
	t_pos = _modifier (hold_tptr, 0);
	hold_tptr += t_pos;
       }

      }

    if (!chars_to_fn_name)
       chars_to_fn_name = hold_tptr - p;
		     /* number of chars from beginning of line to         */
		     /* possible function name for use above              */
		     /*  where, if big buffer is in use we can move       */
		     /*  pointer chars_to_fn_name characters from start   */
		     /*  of line to function name                         */

    if (!buffering)  {
		      routine = buffer;
		      token_ptr = hold_tptr;
		      bufring_line_held = 0;
		      inner_paren = 0;
		      In_Parentheses = 0;
		     }
		       else if (open_parenthesis)
			  In_Parentheses = 1;

	if (!buffering) p = hold_p;

	while(*p)
	{

	  if (!open_parenthesis)break;

	  /* if line definitely does not contain a function def, break; */
	  /*  otherwise continue analysis of line */
	  if ( *p++ == ')' && (inner_paren + 1) == 0 )
	    if (!strchr(p, '(') ) {
	     if (!func_def(p, module, &top, big_buf, rptfp)) break;
	     }


	  if (*p == ')' )
	      inner_paren--;
	  if (*p == '(')
	     inner_paren++;
	  if ( *p == '\n' && open_parenthesis) {
	  /* Multiple line declaration found.  Add each part line to    */
	  /* big_buf until final part is sent to func_def() where final */
	  /* will be added.                                             */
		if (strlen(big_buf) < 350) {
		     strncat(big_buf, buffer, 261);
		     big_buf[BIGBUFSIZE] = '\0';
		     }
		routine = big_buf;
		if (!bufring_line_held) bufring_line_held = line_count;
		buffering = 1;
		}
	   else if(*p == '=' ||  strchr(p, ';') ) {
		 if (buffering) memset( big_buf, '\0', BIGBUFSIZE );
		 buffering = 0;
		 open_parenthesis = 0;
		 In_Parentheses = 0;
		 break;
		 }
	}

   return(top);
}



static void ini_Globals(void)
{
  mod_type_check_off();
  open_parenthesis = 0;
  buffering = 0;
  inner_paren= 0;
  brackets = 0;
  infunction = 0;
  endfunction = 0;
  inliteral = 0;
  bufring_line_held = 0;
  line_count = 0;
  t_pos = 0;
  m_pos = 0;

  previous = NULL;

  In_MainLoop = 1;
  inclF.level = 0;


  memset ( __type_def, '\0', (__TYPEDEFSIZE * __TYPEDEFS) );
  memset ( __typ_mod, 0, (__TYPEDEFSIZE * sizeof(int)) );
  __typedef_count = 0;

}

FLAG_S functionstart(char **hold_p, char **hold_tptr)
{

      /* check for an open parenthesis, marking start of function */
      /* but stop at any other posssible delimiters  */
      while (**(hold_p) && **(hold_p) != '/'
		&& **(hold_p) != ',' && **(hold_p) != ';')
	 {
	 if (**hold_p == ' ')
	    *hold_tptr = *hold_p;
	 (*hold_p)++;
	 if (**hold_p == '(')
	    return(1);
	 }

       return(0);
}


/*  The major syntactical task of this function is to check for an ending */
/*  semi-colon.  If there is one, then this is not a function definition. */

int func_def(char *p, char *module, struct func **top,
				   char *big_buf, FILE *rptfp)
{
  /* char *ptr_to_curly = NULL; */
		    if (*p == ')')
				 p++;
		    if (buffering) {
		     paginate(rptfp);
		     if (strlen(big_buf) < 350) {
		       /* Add final part line to big_buf  */
			    strncat(big_buf, buffer, 261);
			    big_buf[BIGBUFSIZE] = '\0';
			    }
		    }
		    open_parenthesis = 0;
		    buffering = 0;
		    while (iswhite(*p) && *p)  p++;

		    /* if not function, exit but first re-set big_buf */
		    if (*p == ';') {
			memset( big_buf, '\0', BIGBUFSIZE );
			return(0);
			}


		    if (previous) previous->endfunc_line = endfunction;
		    if (!bufring_line_held) lineno = line_count;
		      else {
		      lineno = bufring_line_held;
		      token_ptr += m_pos;
		      }
		    *top = fill_func_struct(token_ptr, lineno, module);
		    previous = function;
		    /* once a function declaration is found we are inside */
		    /* the body of a function: now we can count brackets  */
		    infunction = 1;

		/* Eliminate curly bracket one same line as func def:     */
		/* This is just for formatting output and has             */
		/* no logical function                                    */
		    if (*p == OPEN_CURLY) {
			*p = ' ';
			brackets = 1;
			}

		    if (declare_file ) {
		      *(routine + (strlen(routine) - 1) )= ';';
		      fprintf (rptfp, "%s\n",  routine);
		      }
		    else fprintf (rptfp, "%s",  routine);
		    paginate(rptfp);


		    memset( big_buf, '\0', BIGBUFSIZE );

   return (1);
}
