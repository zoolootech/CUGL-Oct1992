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
FILENAME:       GLOB_CA.C;

WARNINGS:      "1. Assumes function definitions conform with
		ANSI standards and have prototype form. See
		also "Caveats and Restrictions" in README.CA.
		2. Assumes syntactically correct source files.
		3. Written and tested using Microsoft QuickC.
		4. Copyright retained.  See Copyright
		information in README.CA.";

SEE-ALSO:      EXIT_CA.C, GLOB_CA.C, FUNC_CA.C, IFDEF_CA.C, INTF_CA.C,
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

********************************************************************/

/*lint -esym(534,storage_class,process_vars)  */
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>

#define EXTERNAL
#define INCLUDEFILES_DEFINE
#define TYPEDEF_DEFINE
#define TDEF_EXTERNAL
#include "ca.h"
#include "ca_decl.h"
#include "keywords.h"

static void glob_ca_msgs(int Line, char * Mod, struct func *func_top );
/* externs */
extern struct includeFILE inclF;
extern struct func  *FirstFunction;
extern int function_no;
extern int reading_includeF;
extern int __mods[_MAX_MODS];
extern FILE *scrn_out;
extern int IfDefOFF;
extern char *CurrentMod;
/* globals */
struct globals_list *global = NULL, *globals_start = NULL;
char STRUNbuf[32];

#define SEMI_COLON (int) ';'
#define OPEN_PAREN (int) '('
#define CLOSE_PAREN (int) ')'
#define CLOSE_BRACKET (int) '}'
#define OPEN_BRACKET  '{'
#define PROTOTYPE OPEN_PAREN
#define COMPLEX_DECL CLOSE_PAREN

#define inSTRUCTUNION -1
#define TYPEDEF_OPEN TYPEDEF + 101
#define INCLUDEFILE 1

#define COMPLEX_INI  -1


#define conv_sign_type(s,t) {   s = (char)((t & 0xFF00) >> 8); \
				t = (t & 0x00FF);   }
#define is_outsideFunction(n) ((n <  func_top->line_no) \
				|| (n > func_top->endfunc_line ))

static char COMPLEXdeclbuf[60];
static int  COMPLExp = COMPLEX_INI;
static int in_structunion = 0;
static struct func * FN_ALPHAtop;
extern int In_MainLoop;
extern FILE **FP_ADDRESS;
extern unsigned int inclfLine;
extern unsigned int inclFini;

#define DEBUG_LINE 20
struct func * record_variables(FILE *fp, FILE *rptfp,
		    struct func *start, struct func **func_alphatop)
{
  char  storage, *p,  *end_statement, buffer[261];
  static struct func *func_top = NULL;
  static struct func *previous = NULL;

  unsigned line_num = 0, lineno;
  int inquotes = 0, intypedf = 0, t_pos = 0, delim = 0;
  int preproc_type, skip_ifdef = 0;
  int next_statement = 0, split_line = 0,  __type = NO_TYPE;
  FILE * hold_fp  = fp;
  char *in_comment = NULL;

  FP_ADDRESS = &fp;
  rewind(fp);   In_MainLoop = 0;
  FN_ALPHAtop = NULL;

  if(!func_top) func_top = FirstFunction;
    else func_top = start->next_function;

  if(func_top && func_top->fnptr) func_top =  skip_fnptrs(func_top);

  glob_ca_msgs( __LINE__, __FILE__, func_top );

  ini_if_def_stack();

start_GlobalsLoop:

  while (  fgets (buffer, 160, fp) )
    {


	if (!reading_includeF)  lineno = ++line_num;
	    else lineno = ++inclfLine;

	p = buffer;
	if(line_num == DEBUG_LINE)
		line_num  = DEBUG_LINE;

	 if (in_comment) {
	     in_comment = is_closed_comment(p);
	     if (in_comment == (char *) scrn_out) {
		in_comment = comment_error(func_top, lineno, p);
		     }
	     }
	     else in_comment = is_comment(p);

  if(func_top)
  {
    if (is_outsideFunction(line_num) )
    {

     next_statement = is_nextstatement(&p);  /* if not blank line */

     while (next_statement)
      {


	   do {
	     if ( (inquotes = isliteral(p)) ) p++;
	     if (*p == '\n') { next_statement = 0; break; }
	   } while (inquotes );
	   while (iswhite(*p)) p++;

	   if ( (preproc_type = is_includeF(p, hold_fp)) == INCLUDEFILE)
								break;
	   if (preproc_type ==  PREPROCESSOR)
	       if (!IfDefOFF) skip_ifdef = is_else_if(p, skip_ifdef);
	   if (skip_ifdef) { next_statement = 0; break; }

	    COMPLExp = COMPLEX_INI;
	    if( !(delim == PROTOTYPE  || split_line) )
	    if (!intypedf && !isdelim(*p) )
	      if (in_structunion || (t_pos = data_type_(p, &__type) )) {

		if (__type != NO_TYPE ) {
		    char *complex_p = p;
		    if(!intypedf && !in_structunion) complex_p += t_pos;
		    delim = is_prototype(p);
		    if ( (delim != PROTOTYPE) ||
			(delim == PROTOTYPE &&
			(COMPLExp = is_complex_decl(complex_p))) ) {
		       t_pos = sign_or_ld(p, t_pos, &__type);
		       process_vars(p, t_pos, func_top, __type, lineno,
							storage=0,  rptfp);
		      }

		    }
		}
		else if (__type == TYPEDEF_OPEN) intypedf = 1;

	    if(!t_pos && __type != PREPROCESSOR) {
	      if (!intypedf && !in_structunion) {
	      if (!vol_const(p, func_top, lineno, &__type, rptfp))
		 delim = storage_class(p, func_top, lineno, &__type, rptfp);
	       }
	    }
	   if (next_statement)
	    {
	     next_statement = 0;
	     end_statement = strchr(p, SEMI_COLON);
	     if (end_statement || __type == PREPROCESSOR) {
		intypedf = 0;
		delim = 0;
		split_line = 0;
		if (end_statement)  {
		   p = end_statement + 1;
		   next_statement = is_nextstatement(&p);
		   }
		}
		else
		if (!intypedf && !in_structunion)
		    split_line = 1;

	    }


      }                  /* while (next_statement) */

    }                    /* if (is_outsideFunction(line_num)) */
     else
     storage_class(p, func_top, lineno,  &__type, rptfp);


    if (line_num >= func_top->endfunc_line) {
       previous = func_top;
       func_top = func_top->next_function;
       }

  }   /* if (func_top) */

    }     /* while (fgets. . .) */

    next_statement = 0;
    end_statement = NULL;
    intypedf = 0;
    delim = 0;
    split_line = 0;

  if (close_include(hold_fp, &fp, rptfp, CurrentMod))
			goto start_GlobalsLoop;

  free_if_def_stack();
  func_top = previous;
  if (FN_ALPHAtop) *func_alphatop = FN_ALPHAtop;
  return(previous);

}


static int xt_struct_union(char *p, int __type)
{
  char *ptr = NULL;
  char *initializing = NULL;
  static char *close_brackets = NULL;
  static char *open_brackets = NULL;
  static char *ending_semicol = NULL;
  static int identified = 0;
  size_t count;

      if (ending_semicol) {
	  in_structunion = 0;
	  open_brackets = NULL;
	  close_brackets = NULL;
	  ending_semicol = NULL;
	  }

      initializing = strchr(p, '=');
      if (initializing) {
	 *initializing = ';';
	 *(++initializing) = '\0';
	 }

      if(!open_brackets) {
	 open_brackets = strchr(p, (int) OPEN_BRACKET);
	 if(open_brackets) in_structunion = 1;
	 }

       if (open_brackets && !close_brackets)
	 close_brackets = strchr(p, CLOSE_BRACKET);

       if (close_brackets && !ending_semicol) {
	  ending_semicol = strchr(p, SEMI_COLON);
	  }


       if (in_structunion && !close_brackets)  {
	  if (!identified) { isolate_strun(p, __type); identified = 1; }
	  return(inSTRUCTUNION);
	  }

       if (in_structunion) {
	      if(ending_semicol) {
		 in_structunion = 0;
		 identified = 0;
		 }
	      return(close_brackets - p + 1);
	      }

	 count = isolate_strun(ptr = p, __type);
	 ptr += count;
	 ending_semicol = strchr(p, SEMI_COLON);
	 if (initializing)
		*initializing = ' ';
		identified = 1;
	 if (ending_semicol) return(count);

	 while(*ptr && iswhite(*ptr))  {  ptr++; count++; }
	 if (*ptr == '\n' || *ptr == '\0') {
	       in_structunion = 1;
	       return(inSTRUCTUNION);
	       }

	 while (*ptr && (isalnum(*ptr) || *ptr == '_') )
	      {
	      ptr++; count++;
	      }
	 return(count);


}

int infunclist(char *token_string);
static int process_vars(char *p, int token_pos, struct func *funcptr,
		    int __type, int line_num, char storage, FILE *rptfp)
{
   char *ptr = p, *token, *open_array = NULL, *white_in_token;
   int count, str_pos = 0;
   char token_string[41];
   int mods[_MAX_MODS];
   static char hold_storage = '\0';
   char *complex_decl = NULL, *complex_tokenstr = NULL;

   COMPLEXdeclbuf[0] = '\0';

   if (in_structunion) storage = hold_storage;
   if (__type == STRUCTURE || __type == UNION  || __type == ENUM) {
		token_pos =  xt_struct_union(p, __type);
		hold_storage = storage;
		if (token_pos == inSTRUCTUNION) return (0);
		}

   ptr = p + token_pos;
   while(*ptr && iswhite(*ptr)) ptr++;
   if (!*ptr) return(0);

   if (complex_decl = strchr(ptr, '('))
	*complex_decl = '\0';

  /* test string to locate sequences of modifiers, both user-defined */
  /* and C-language based */
   ptr = get_modifiers(FINAL_MODS_CHECK, ptr, mods);
   /*restore parenthesis deleted in get_modifiers() */
   if (complex_decl) *complex_decl = '(';

   if (*ptr == SEMI_COLON) return(SEMI_COLON);
     else if (*ptr == '\n' || *ptr == '\0' ) return (0);

     if (complex_decl) {
	while(iswhite(*ptr)) ptr++;
	complex_tokenstr = ptr;
	while(*ptr == '*' || iswhite(*ptr) ) ptr++;

	do
	{
	  while (*ptr == '(' || *ptr == '*') ptr++;
	  str_pos = _modifier(ptr, 0);
	  ptr += str_pos;
	}
	while (str_pos);
	while (!isalnum(*ptr) && *ptr != '_') ptr++;
       }

     open_array = strchr(ptr, '[');

     while ( (ptr && *ptr) && (*ptr != '\n' && *ptr != SEMI_COLON) ) {
	count = 0;
	token = ptr;
	while (ptr && *ptr == '*')  { ptr++; count++; }
	white_in_token = ptr;
	while(ptr && iswhite(*ptr)) { ptr++; count++; }
	if (white_in_token == ptr) white_in_token = NULL;
	while(*ptr && (isalnum(*ptr) || *ptr == '_' || *ptr == '*') ) {
		ptr++;
		count++;
	      }

       if (open_array) {
	     char *holdp = ptr;
	     int holdc = count;
	     if(!white_in_token) white_in_token = ptr;
	     while(ptr && iswhite(*ptr)) { ptr++; count++; }
	     if (white_in_token == ptr) white_in_token = NULL;
	     if (*ptr == '[' )
	       count = get_dimensions(ptr, count);
	       else { ptr = holdp; count = holdc; }
	     }
	if(count > 40) count = 40;
	strncpy(token_string, token, (size_t)count );
	token_string[count] = '\0';
	token = token_string;


	if(*token_string) {
	   if (is_variable(token_string)) {
	      if (white_in_token) white_out(token_string);

	      if(__typ_mod[__typenum] == _FNPTR_ ) {
		       strcpy(COMPLEXdeclbuf, token_string);
		       COMPLExp = COMPLEX_FN;
		       }

	      if (complex_decl || (COMPLExp == COMPLEX_FN) ) {
		if( __typ_mod[__typenum] != _FNPTR_)
		  str_pos = make_complex_decl(complex_tokenstr);
	       if (COMPLExp == COMPLEX_FN ) {
		  if(!infunclist(token_string))
		   FN_ALPHAtop =
		    fill_func_struct(token_string, line_num, funcptr->module);
		   Globfnptrpush(p, token_string);
		  }
	       }

	      globals_start = fill_globals_struct(token_string, funcptr,
				mods,  __type, line_num, storage, rptfp);
					 }
	    }
	    else return(0);

	 if (open_array) {
	    ptr = strchr(ptr, ']');
	    if (ptr) ptr++; else return(1);
	    }
	 while(*ptr && iswhite(*ptr)) ptr++;
	 if(*ptr == '=') {
	   ptr++;
	   while(*ptr && iswhite(*ptr)) ptr++;
	   if(*ptr == OPEN_BRACKET) return (1);
	   }

	if ( !(ptr = strchr(ptr, ','))) {
	    ptr = strchr(ptr, ';');
	    if (ptr) return(SEMI_COLON); else break;
	    }
	    else {
	     if (complex_decl) {
		       if (ptr < complex_tokenstr + str_pos) {
			    ptr = strchr(ptr, ';');
			    if (ptr) return(SEMI_COLON); else break;
			}
		    }
	     while(*++ptr && iswhite(*ptr));
	     }

       }
   return(1);
}

static int storage_class(char *p, struct func *func_top, int line_num,
					    int *__type,  FILE *rptfp)
{
 int str_pos = 0,  strpos = 0;
 char *token_ptr ;
 int  mods[_MAX_MODS], hold_type = 0;
 char storage = 0, *hold_p;
 char *open_paren = NULL;
 char *truncation = NULL, trunc_char;

  while(*p && iswhite(*p)) p++;
  hold_p = p;
  token_ptr = p;
  while(*p && (isalnum(*p) || *p == '_') ) {
		  p++;
		  }

  truncation = p;
  trunc_char = *p;
  *p = '\0';
  str_pos = test_token(token_ptr, MODIFIERS[EXTERN]);
  if (str_pos) storage = EXTERN;
    else {
     if (str_pos = test_token(token_ptr, MODIFIERS[STATIC]))
	     storage = STATIC;
     }


  if (str_pos) {
    p++;

    while (iswhite(*p)) p++;
    if (*p == PROTOTYPE) {
	   str_pos =  PROTOTYPE;
	   goto Not_Variable ;
	    }
    open_paren = strchr(p, PROTOTYPE);

    /*test for FILE type */
    str_pos = data_type_(p, __type);
    if (*__type == file)
	      p += str_pos;
    p = get_modifiers(FIRST_MODS_CHECK, p, mods);
    if(*mods == TYPEDEF) hold_type = TYPEDEF;
    if (*__type != file) {
       strpos = data_type_(p, __type);
       if (hold_type == TYPEDEF)
	  * __type = TYPEDEF;
       else strpos = sign_or_ld(p,strpos, __type);
    }
    if (open_paren)
    if (*open_paren != PROTOTYPE) *open_paren = PROTOTYPE;
    if ( is_prototype(p) == PROTOTYPE ) {
	char *complex_p = p;
	if(!in_structunion) complex_p += strpos;
	if ( !(COMPLExp = is_complex_decl(complex_p)) ) {
	       str_pos =  PROTOTYPE;
	       goto Not_Variable ;
	      }
       }
    process_vars(p, strpos, func_top, *__type, line_num, storage, rptfp);
    }

Not_Variable:

	*truncation = trunc_char;
	p = hold_p;

  return(str_pos);
}


struct globals_list *store_ll_globals( struct globals_list *i,
						struct globals_list *top);
struct globals_list *fill_globals_struct(char *token, struct func *funcptr,
   int mods[_MAX_MODS], int __type, int line_num, char storage, FILE * rptfp)
{
  size_t strL;
  int spaces = 6;
  static struct globals_list *top = NULL;
  int mod_count;
  char sign = '\0';

    if (inclFini ) {
       fprintf(rptfp, "<Include File: %s>\n", inclF.file[inclF.level - 1]);
       inclFini = 0;
       }

    global = (struct globals_list *)
	 malloc ( sizeof (struct globals_list) );

    strL = strlen(token) + 1;
    global->variable =  (char *) malloc(strL);
    strcpy(global->variable, token);
    global->module = funcptr->module;

    if(__type == NO_TYPE) __type = INTEGER;
    global->type = __type;

    if (__type > END_TYPES) conv_sign_type(sign, __type);
    if(__type == NO_TYPE) __type = INTEGER;

    if (__type == TYPEDEF) {
      global->type_ident = (char *) malloc(strlen(__type_def[__typenum]) + 1);
      strcpy (global->type_ident, __type_def[__typenum]);
      }
    if (__type == STRUCTURE || __type == UNION || __type == ENUM) {
      global->struct_ident = (char *) malloc(strlen(STRUNbuf) + 1);
      strcpy (global->struct_ident, STRUNbuf);
      }

   if(COMPLEXdeclbuf[0]) {
     char complex_id[2] = { (char )COMPLExp, '\0' };
     global->complex_decl = (char *) malloc(strlen(COMPLEXdeclbuf) + 3);
     strcpy(global->complex_decl, complex_id);
     strncat(global->complex_decl, COMPLEXdeclbuf, 60);
     } else global->complex_decl = NULL;

  if (mods[0]) {
     for (mod_count = 0; mod_count < mods[0]; mod_count++)
	 global->modifiers[mod_count] = mods[mod_count];
      }

  global->storage = (char)storage;
    top = store_ll_globals(global, top);
    if (reading_includeF) {
       spaces = 4;
       fprintf(rptfp, " <");
       }
    fprintf(rptfp, "%*d: ", spaces,  line_num);
    if (storage)
	fprintf(rptfp, "[%s] ", MODIFIERS[storage]);
    if (sign) fprintf(rptfp, "[%c]", sign);
    fprintf(rptfp, "[%s] %s\n",  TYPES[__type], token);

    if (mods[0] - 1) print_modifiers(0, mods, __type, rptfp);

    if (__type == TYPEDEF) {
	fprintf(rptfp, "        type: %s\n", global->type_ident);
	paginate(rptfp);
      }

   if (global->type == STRUCTURE || global->type == UNION
					|| global->type == ENUM) {
       fprintf(rptfp, "         %s %s\n",
				TYPES[global->type], global->struct_ident);
       paginate(rptfp);
       }

    if(COMPLEXdeclbuf[0]) {
       if(*global->complex_decl == (char)COMPLEX_FN)
		fprintf(rptfp, "        <Fn Ptr> ");
	   else fprintf(rptfp, "          ");
       fprintf(rptfp, "%s\n", global->complex_decl + 1);
       paginate(rptfp);
       }

    paginate(rptfp);
    return(top);
}


void print_alphab_globals(struct globals_list *top, FILE *rptfp)
{

  char *c1, *c2, temp[6];
  int newinitletter = 0;
  int spaces;
  char sign;
  struct globals_list *hold = top;

  print_headings(5, rptfp );

  while(top) {
   if (newinitletter) {
       newinitletter = 0;
       fprintf(rptfp, "\n");
       paginate(rptfp);
       }

   if (top->storage)
	fprintf(rptfp, "%s ", MODIFIERS[(int)top->storage]);
	else fprintf(rptfp, "       ");

   sign = '\0';
   spaces = 17;
   if (top->type > END_TYPES)conv_sign_type(sign , top->type);
   if(top->type == NO_TYPE) top->type = INTEGER;
   if (sign)  fprintf(rptfp, "[%c]", sign); else fprintf(rptfp, "   ");

   fprintf(rptfp, "%-7s %-40s %s\n",
			TYPES[top->type], top->variable, top->module);

   if (top->modifiers[0] - 1) {
	print_modifiers(1, top->modifiers, top->type, rptfp);
      /*  spaces = 2;*/
	}

   if (top->type == TYPEDEF) {
       fprintf(rptfp, "%*c[type:  %s]\n",spaces, ' ', top->type_ident);
       paginate(rptfp);
       }

   if (top->type == STRUCTURE || top->type == UNION || top->type == ENUM) {
       fprintf(rptfp, " %*c[%s:  %s]\n",
				spaces, ' ',
				TYPES[top->type], top->struct_ident);
       paginate(rptfp);
       }

    if(top->complex_decl) {
       if(*top->complex_decl == (char)COMPLEX_FN) {
	   fprintf(rptfp, " %*c<Fn Ptr>\n%*c ", spaces, ' ', spaces, ' ');
	   paginate(rptfp);
	   }
	 else
	   fprintf(rptfp, " %*c", spaces, ' ');
       fprintf(rptfp, "%s\n",top->complex_decl + 1);
       paginate(rptfp);
       }

   c1 = top->variable;

   top = top->next;

   if(top) {
     strncpy(temp, top->variable, 5);
     temp[5] = '\0';
     c2 = temp;
     strupr(c1);
     strupr(c2);
     while(*c1 == '_' || *c1 == '*') c1++;
     while(*c2 == '_' || *c2 == '*') c2++;
     if(*c1 != *c2) newinitletter = 1;
   }
   paginate(rptfp);

   }

  strupr(c1);
  top = hold;
  fprintf(scrn_out,
  "\n\n*******\n*******\n"
   "ALPHABETICAL LIST OF GLOBAL SYMBOLS IN ALL MODULES\n\n");
  while (top) {
    if (top && hold)
    if( (strcmp(top->variable, hold->variable)))
	fprintf(scrn_out, "%s\n", top->variable);
    hold = top;
    top = top->next;
    }
   fprintf(scrn_out, "\n\n\nEND SCRNOUT\n");
}


static size_t isolate_strun(char *ptr, int __type)
{
 char *token;
 size_t count = 0, token_count = 0;

	 while(*ptr && iswhite(*ptr)) {  ptr++; count++; }
	 ptr += 6; count += 6;   /* 6 = size of keyword 'struct' */
	 if(__type == UNION || __type == ENUM) { ptr--; count--; }
	 while(*ptr && iswhite(*ptr))  {  ptr++; count++; }

		 /* get past name of structure or union*/
	 token = ptr;
	 while(*ptr && (isalnum(*ptr) || *ptr == '_') ) {
			  ptr++;
			  count++;
			  token_count++;
			  }
	 if (token_count > 31) token_count = 31;
	 strncpy(STRUNbuf, token, token_count);
	 STRUNbuf[token_count] = '\0';

       return(count);
}


int is_nextstatement(char **p)
{
    while(iswhite(**p)) (*p)++;
    if(**p != '\n' && **p != '\0') return (1);

    return (0);
}

int is_includeF(char *p, FILE * hold_fp)
{
   char *token_ptr, *temp = p, *end_token;

   if(*p != '#') return (0);

   token_ptr = isolate_token(++(p), &end_token);


   if ( !is_include(token_ptr, hold_fp)) {
	   p = temp;
	   return(PREPROCESSOR);
	   }
   return(INCLUDEFILE);
}


static int sign_or_ld(char *p , int t_pos, int *__type)
{
   int type = *__type;
   int str_pos = 0;

   if (type == LONG) {
	  str_pos = 5;
	  str_pos += data_type_(p + str_pos, __type);
	  if (*__type == DOUBLE) {
	     *__type |= LONG_DOUBLE;
	     return(str_pos);
	     }
	  return (t_pos);
	  }

   if(*__type == SIGNED) t_pos = 7;
   else if(*__type == UNSIGNED) t_pos = 9;
   str_pos = t_pos;

   if ( *__type == SIGNED || *__type == UNSIGNED  ) {
      t_pos = data_type_(p + t_pos, __type);
      if  (t_pos)  {
	t_pos += str_pos;
	if (type == UNSIGNED) *__type |= UNSIGNED_VAR;
	  else if (type == SIGNED) *__type |= SIGNED_VAR;
	}
	  else if (str_pos == (t_pos += str_pos) )
	    *__type = type | INTEGER;
   }

   return(t_pos);
}

void print_modifiers(int alphab, int mods[_MAX_MODS], int __type, FILE *rptfp)
{
  int mod_count, nmber_ptrs = 0, type, p_count, userdef = 0;
  char *type_ptr;

  if (alphab) fprintf(rptfp, "                  ");
     else fprintf(rptfp, "         ");
  fprintf(rptfp, "< ");

  for (mod_count = 1; mod_count < mods[0]; mod_count++) {
       if (mods[mod_count] >= ( 1 << 8 ) ) {

	   type = mods[mod_count];
	   if (type >= 0x2000) {
	      type &= 0x1fff;
	      userdef = 1;
	      }
	   nmber_ptrs =  (type & 0xff0) >> 8;
	   if (nmber_ptrs < 8)
	     for (p_count = 0; p_count < nmber_ptrs; p_count++)
		 fprintf(rptfp, "* ");
	   type &= 0x0f;
	   if (userdef) {
	     if (!(type_ptr = pop_usertype(type))) return;
	     }
	    else
	     type_ptr = MODIFIERS[ type ];

	  }
	  else type_ptr = MODIFIERS[mods[mod_count]];
       fprintf(rptfp, "%s  ", type_ptr);
       }

   fseek(rptfp, (-1L), SEEK_CUR);
   fprintf(rptfp, ">");
  if ( !( __type == STRUCTURE || __type == UNION || __type == ENUM
		    || __type == TYPEDEF) || alphab ) {
	fprintf(rptfp, "\n"); paginate(rptfp);
	}

}


int vol_const(char *p, struct func *func_top, int line_num,
					int *__type, FILE *rptfp)
{
 int str_pos = 0,  strpos = 0;
 char *token_ptr, *hold_ptr, save_char;
 int storage = 0;
 int *mods_dummy = NULL;

  while(*p && iswhite(*p)) p++;
  hold_ptr = p;
  token_ptr = p;
  while(*p && (isalnum(*p) || *p == '_') ) {
		  p++;
		  }
  save_char = *p;
  *p = '\0';


    if ((str_pos = test_token(token_ptr, MODIFIERS[VOLATILE]))) {
      __mods[0] = 1;
      *p = save_char;
      p = hold_ptr;
      if (str_pos = _modifier (p, 0)) p += str_pos;
      }
      else
      if (str_pos = test_token(token_ptr, MODIFIERS[CONSTANT])) {
	__mods[0] = 1;
	*p = save_char;
	p = hold_ptr;
	if (str_pos = _modifier (p, 0)) p += str_pos;
	}

  if (str_pos) {

    while (iswhite(*p)) p++;
    if (*p == PROTOTYPE) return(PROTOTYPE);

    p = get_modifiers(FIRST_MODS_CHECK, p, mods_dummy);
    if (storage =  storage_class(p, func_top, line_num,  __type, rptfp))
						return(storage);
    strpos = data_type_(p, __type);
    strpos = sign_or_ld(p,strpos, __type);
    if ( (is_prototype(p)) == PROTOTYPE ) {
	char *complex_p = p;
	if(!in_structunion) complex_p += strpos;
	if ( !(COMPLExp = is_complex_decl(complex_p)) ) return(PROTOTYPE);
	}
    process_vars(p, strpos, func_top, *__type, line_num, (char)0, rptfp);
    }
    else {
    *p = save_char;
    p = hold_ptr;
    __mods[0] = 0;
    }
  return(-str_pos);
}

static int make_complex_decl(char *p)
{
   char *end_complex = NULL, *paren = NULL, *ptr;
   size_t difference = 0;
   ptr =  COMPLEXdeclbuf;

    ptr[60] = '\0';   /* keep string in bounds */
    strcpy(ptr, p);

    end_complex =  strchr(ptr, ',');
    if(end_complex) {
       paren = strchr(end_complex, ')');
       if (  !(strchr(end_complex, '(') > paren) )
			  end_complex = NULL;
	}

    if (!end_complex)
	end_complex =  strchr(ptr, ';');
    if (!end_complex)
	end_complex =  strchr(ptr, '\n');
    *end_complex = '\0';

   difference = end_complex - ptr;
   return(difference);
}

static char *is_comment(char *p)
{
 char *start_comment = NULL, *end_comment = NULL;
 char *start_quot = NULL, *end_quot = NULL;
 int in_quot = 0;


   start_comment = strchr(p, '/');
   if (start_comment && *(start_comment + 1) == '*') {

	/* make sure we're not in a quotation */
	start_quot = strchr(p, '\"');
	if (start_quot) {
	 if (start_comment > start_quot) {
	    end_quot = start_quot + 1;
	    while (!(*end_quot == '\"')) {
	    if (*end_quot == '\\') end_quot++;
	     if (*end_quot == '\n' || *end_quot == '\0') {
		in_quot = 1;
		break;
		}
	     end_quot++;
	    }
	   if (!in_quot)
	     if(*end_quot == '\"')
	       if (start_comment < end_quot) return (NULL);
	   }
	 }

	*start_comment = '\0';
	end_comment = start_comment + 2;
	/* if comment closed on current line return false: not in_comment */
	while(*end_comment) {
	    if (*end_comment == '/')
	    if ( *(end_comment - 1) == '*' ) return(NULL);
	    end_comment++;
	    }
    /* if comment not closed on current line return true: in_comment */
	return (start_comment);
      }

    return(NULL);
}

static char * is_closed_comment(char * p)
{
    char *truncate = p;
    while(*p) {
	if (*p == '/') {
	if ( *(p - 1) == '*' ) { *truncate = '\0'; return(NULL); }
	if ( *(p + 1) == '*' ) return((char *)scrn_out);
	}
	p++;
       }

    /* truncate line */
    *truncate = '\0';
    return(p);

}

static char *comment_error(struct func *func_top, unsigned lineno, char *p)
{

   fprintf(scrn_out, "\n***"
      "Possible comment error at line %u in module: %s\n   %s",
       lineno, func_top->module, p);

   return(p);
}

static void glob_ca_msgs(int Line, char * Mod, struct func *func_top )
{

  if (!func_top) {
       printf("\a");
       if (scrn_out != (FILE *)stdout)
       fprintf(scrn_out,
	    "\nSecond Pass: NULL Pointer Found in Table of Functions:"
	    "\n   function: record_variables()  module[%s]  Line: %d."
	    "\n   Report File may prove unreliable.\n",  Mod, Line);
	return(NULL);
	}

  if (scrn_out != (FILE *)stdout) fprintf(scrn_out,
	  "\nSecond Pass: %s\n", CurrentMod);
  printf("Second Pass: %s\n", CurrentMod);

}
