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
FILENAME:       LINKL_CA.C;

WARNINGS:      "1. Assumes function definitions conform with
		ANSI standards and have prototype form. See
		also "Caveats and Restrictions" in README.CA.
		2. Assumes syntactically correct source files.
		3. Written and tested using Microsoft QuickC.
		4. Copyright retained.  See Copyright
		information in README.CA.";

SEE-ALSO:      EXIT_CA.C, FUNC_CA.C, GLOB_CA.C, IFDEF_CA.C, INTF_CA.C,
	       PARSE_CA.C, TDEF_CA.C, TYPES_CA, UTIL_CA.C,
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
#include "ca.h"
#include "ca_decl.h"

struct GLoc_fnptr_stack
{
   char fnptr[32];
   char fn_name[32];
   char *caller;
};

extern struct GLoc_fnptr_stack *GLfptrstack;


void print_ll (struct func *top, FILE *rpt)
{
   char dots[] = ". . . . . . . . . . . . . . . . . .";
   int dot_len = 0, count = 0;

   print_headings(1, rpt );

   while (top){
    if (!top->fnptr) {
     dot_len = ( 36 - ((int) strlen(top->name) + (int) strlen(top->module)) );
     fprintf (rpt,"%.27s%.*s%s",top->name, dot_len, dots, top->module);
     if ( ++count % 2 == 0 ) {
	     fprintf (rpt,"\n");
	     paginate(rpt);
	     }
	     else fprintf (rpt,"    ");
     }
     top = top->next;
   }
}


struct func *store_linked_list( struct func *i, struct func *top)
{
 static struct func *last = 0;
 struct func *old, *start;

 start = top;

 if (!last) {
   i->next = NULL;
   last = i;
   return i;
	    }

 old = NULL;
 while (top) {
   if (strcmp(top->name, i->name) < 0) {
      old = top;
      top = top->next;
      }
      else {
	 if (old) {
	    old->next = i;
	    i->next = top;
	    return (start);
		  }
    i->next = top;
    return (i);
	  }
   }
   last->next = i;
   i->next = NULL;
   last = i;
   return (start);
}

#if !defined(_GLOBALS_LIST)
#define _MAX_MODS 6
struct globals_list
 {
  char *variable;
  char *module;
  int type;
  char *type_ident;    /* ptr to name of typedef type if type == TYPEDEF */
  char *struct_ident; /* point to name of structure or union */
  char *complex_decl;
  char storage;       /* type of storage */
  int modifiers[_MAX_MODS];     /* list of modifiers & qualfiers */
  struct globals_list *next;
 };
#endif
struct globals_list *store_ll_globals( struct globals_list *i,
						struct globals_list *top)
{
 static struct globals_list *last = 0;
 struct globals_list *old, *start;
 char *i_var, *top_var;

 start = top;

 if (!last) {
   i->next = NULL;
   last = i;
   return i;
	    }

 old = NULL;
 while (top) {
   top_var = top->variable;
   i_var = i->variable;
   while ( is_in(*top_var, "*") ) top_var++;
   while( is_in(*i_var, "*") ) i_var++;

   if (stricmp(top_var, i_var) < 0) {
      old = top;
      top = top->next;
      }
      else {
	 if (old) {
	    old->next = i;
	    i->next = top;
	    return (start);
		  }
    i->next = top;
    return (i);
	  }
   }
   last->next = i;
   i->next = NULL;
   last = i;
   return (start);
}
FILE *open_reportfile(int rpt_number);
static void prn_calls__fnptrs(struct func *func_start,
					int which, FILE *rptfp);
void print_calls_from(struct func *func_start, FILE *rptfp)
{

    print_headings(3, rptfp );
    fprintf(rptfp, "\n<Function>\n       <Calling Functions>\n"
		   "        . . . .\n"  );
    paginate(rptfp); paginate(rptfp);
    paginate(rptfp); paginate(rptfp);

    prn_calls__fnptrs(func_start, 0, rptfp);
}

void prn_all__fnptrs(FILE *rptfp);
void print_fnptrs(struct func *func_start, FILE *rptfp)
{

    print_headings(4, rptfp );
    fprintf(rptfp, "\n<Global Pointers>\n       <Functions Called From>\n"
		   "        . . . .\n"  );
    paginate(rptfp); paginate(rptfp);
    paginate(rptfp); paginate(rptfp);

    prn_calls__fnptrs(func_start, 1, rptfp);
    prn_all__fnptrs(rptfp);
}

static void prn_calls__fnptrs(struct func *func_start, int which, FILE *rptfp)
 {
  struct func *top;
  struct calls_from *ptr;
  static char *msg[2] = { "UNUSED FUNCTION", "UNUSED PTR TO FUNCTION" };

    top = func_start;
    while(top)
    {
     if(top->fnptr != which) goto getnext;
     fprintf(rptfp, "\n\n%-33s [%s]\n", top->name, top->module);
     paginate(rptfp); paginate(rptfp); paginate(rptfp);

     if (top->first_call) {
	ptr = top->first_call;
	  do {
	     fprintf(rptfp,"      %-35s (%s)\n",
				  ptr->caller->name, ptr->caller->module);
	     paginate(rptfp);
	     ptr = ptr->next;
	     }
	     while( ptr);
	}
	else if ( strcmp("main", top->name) ) {

	       fprintf(rptfp,"      %s\n", msg[which] );
	       paginate(rptfp);
	       }
getnext:
     top = top->next;
    }
}

