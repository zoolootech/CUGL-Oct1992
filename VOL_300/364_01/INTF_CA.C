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
FILENAME:       INTF_CA.C;

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
#include <string.h>
#include <graph.h>
#include <malloc.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <direct.h>
#include <errno.h>
#define EXIT_CODES
#include "ca.h"
#include "ca_decl.h"

#define WHITE   0x7
#define INTENSE 0x8
#define HIGH_VIDEO  _settextcolor(WHITE | INTENSE)
#define LOW_VIDEO   _settextcolor(WHITE)

/* general externs and globals */
extern struct globals_list *globals_start;
char rpt_fname[_MAX_FNAME] = "\0\0";
char *CurrentMod = NULL;
struct rccoord locate;

/* Globals for Command Line Options */
int __Inclf_Limit = 5;
char NewInclude[128]="INCLUDE=\0\0";
FILE *scrn_out = (FILE *)stdout;
int declare_file = 0;
int user_hf_VAR = 0;
int user_hf_FUNC = 0;
int user_hfile_only = 0;
int verbose = 0;
size_t GLfpSsize = 15;
static SplitNumber_rpf = 0;

int IfDefOFF = 0;
int __Complex_Declarators = 1;
int __Complex_Prototypes = 1;

void ini_characterset(void);

/** NOTE: Command Line and Makefile filespecs are limited to 63 characters**/
/**This is set by FILENAME_MAX in STDIO.H**/

int switches_found = 0;
void main(int argc, char **argv)
{
 struct filespec *c_file_start = NULL;
 struct func *func_start;
 FILE *rptfp;
 char *file_string =  NULL;
 int prj = 0;

 _clearscreen(0);
 if (argc > 1) {
    file_string = argv_cat(argc, argv);
    if (*file_string == '@')   {
	 prj = 1;
	 file_string = readmakefile(file_string);
	 }
    if (file_string)
      c_file_start = extract_file_names(file_string);
      else {
	int c;
	printf("\nMake file not found. Continue [Y/N]?\n");
	c = getch();
	if (c != 'y' || c != 'Y') exit(0);
	}
    }
 if (!file_string || (!c_file_start && switches_found))
			   c_file_start = get_modules();
 if (!c_file_start)
      if (file_string && prj) exit_ca (BAD_PROJ, "");
 if (!c_file_start) exit_ca (NO_FILE_STR, "");
 rptfp = make_rptfile(c_file_start->path);

 ini_characterset();

 loop_through_modules (c_file_start, &func_start, rptfp);
 if (declare_file) { fcloseall(); exit(0); }

 print_ll(func_start, rptfp);

 if(SplitNumber_rpf == 3)
   rptfp = open_reportfile(FIRST_SPLIT);

 xrf(func_start, c_file_start, rptfp);

 if(SplitNumber_rpf == 2)
   rptfp = open_reportfile(FIRST_SPLIT);

 print_calls_from(func_start, rptfp);
 print_fnptrs(func_start,rptfp);

 if(SplitNumber_rpf == 3)
      rptfp = open_reportfile(SECOND_SPLIT);

 print_alphab_globals(globals_start, rptfp);

 fcloseall();

}

/*lint -epu */

struct filespec *get_modules(void)
{

  char *file_string, buffer[81];
  size_t strL;
  int ampersand;

 instructions();

 if ( !( file_string = (char *) malloc(sizeof (char) * 81 ) ) )return NULL;

 *buffer = '\0';
 *file_string = '\0';

 do
 {
 relocate();
 HIGH_VIDEO;
 _outtext("C Files:  ");
 LOW_VIDEO;

 fgets (buffer, 80, stdin);
 if (*buffer == '\n') exit(0);
 strupr( buffer );
 strL = strlen(buffer);
 strncat (file_string, buffer, strL - (size_t) 1);
 strL = strlen(file_string);
 ampersand = reallocate(file_string, strL);

  } while (ampersand);

  return ( extract_file_names(file_string));


 }

struct filespec  *extract_file_names(char *file_string)
 {
  char seps[] = " +";
  struct filespec  *c_file_ptr, *previous, *c_file_start = NULL;

   if ( !(
    c_file_ptr = (struct filespec *)
	   malloc ((size_t) sizeof (struct filespec) )
       ) ) return (NULL);
    if (!c_file_start) c_file_start = c_file_ptr;
    c_file_ptr->next = NULL;

    c_file_ptr->path = strtok( file_string, seps );     /* Find first token        */
    if (!c_file_ptr->path) return (NULL);
    if (c_file_ptr->path)
	if(*c_file_ptr->path == '&') return (NULL);


    while( c_file_ptr->path != NULL )
    {

   previous  = c_file_ptr;

   if ( !(
    c_file_ptr = (struct filespec *)
	   malloc ((size_t) sizeof (struct filespec) )
       ) ) return (NULL);

    c_file_ptr->next = NULL;
    previous->next = c_file_ptr;
    c_file_ptr->path = strtok( NULL, seps );   /* Find next token         */

  }

    if (c_file_start)  return c_file_start;
    return (NULL);
 }


void instructions (void)
{
 _clearscreen(0);
 HIGH_VIDEO;
 _outtext("\nInstructions:  \n");
 LOW_VIDEO;
 printf(
"Enter C files, separated by spaces or commas; if the files need more than\n"
"one line, append an & to end of line and continue on to next line (do the\n"
"same for each new line needed). The \'.C\' extension is assumed. [Note: it\n"
"is also assumed that each new function begins on a new line and that\n"
"function declarations conform to new ANSI style.]\n\n"
  );

  relocate();
}


static int reallocate(char *file_string, size_t strL)
{
 int count;
 static int line = 1;
 char *p;



 for (p = file_string + (int) strL - 1, count = (int) strL;
					      count;   count--, p++)
    if (*p != ' ') break;

 strL = count;
 count--;
 if (file_string[count] == '&')  {
     if (file_string[count - 1] != ' ') return(0);
				 }
				else return(0);

 file_string[count] = ' ';
 line++;
 file_string = realloc(file_string, (strL + 81) * sizeof(char));
 file_string[strL] = '\0';
 return (1);
}

void loop_through_modules (struct filespec *top, struct func **func_start,
								FILE *rptfp)
{
  char fname[_MAX_FNAME];
  FILE *fp;
  struct func *start = NULL, *current_func = NULL;

   if (!top) return;
   if (!declare_file) print_headings(0, rptfp );

  while (top->path)
  {

  do
  {
  strcpy(fname, top->path);
  fp = open_c_file(fname, rptfp);
  if (!fp)  get_new_path(top);
  }
  while (!fp);

  user_hfile_only =  user_hf_FUNC;
  start = main_loop(fp, rptfp, fname);
  CurrentMod = fname;
  user_hfile_only =  user_hf_VAR;
  if (!declare_file)
       current_func = record_variables(fp, rptfp, current_func, &start);
  fclose(fp);
  top = top->next;
  }

  *func_start = start;   /* func_start = start of alphabetical linked list */
}


int paginate(FILE *rptfp)
{
 static int line_count = 0;
 static int page = 2;
 char dash = '-';

 if(declare_file) return (0);

 line_count++;
 if (line_count > 59)
    {
      fprintf(rptfp, "\f\n%38c%d-\n\n",dash, page);
      line_count = 4;
      heading(rptfp, rpt_fname);
      page++;
      return(0);
    }

  return(line_count);
}

void formfeed(int line_count, FILE *rptfp)
{
  while(line_count )
     line_count = paginate(rptfp);
}


static char *argv_cat(int argc, char **argv)
{
  int makefile = 0;
  char *buffer;
  buffer = (char *)malloc((size_t)argc * (size_t) (FILENAME_MAX + 1));
  memset(buffer, 0, 14);

  *(argv++);
   argc--;
   if (!argc) exit_ca (BAD_ARGV, "");

    while (argc)
     {
      if(**argv == '@') {
	 strcpy(buffer, *(argv++));
	 makefile = 1;
	 argc--;
	 }
      else if(!makefile) {
       if(**argv != '/' && **argv != '-') {
	 strncat( buffer, *(argv++), (size_t)FILENAME_MAX );
	 argc--;
	 if (argc) strcat(buffer, "+");
	 }  else {
		 handle_switches(&argv, &argc);
		 argv++;
		 argc--;
		}
	 }
	 else {
	 handle_switches(&argv, &argc);
	 argv++;
	 argc--;
	}
     }


  strupr(buffer);
  _outtext(buffer);
  return(buffer);

}

#define RET_PATHVAR_ONLY  0x20
char Rptfpath[_MAX_PATH];
FILE *make_rptfile(char *path)
{
  char inpath[_MAX_PATH] ;
  char drive[_MAX_DRIVE], dir[_MAX_DIR];
  char fname[_MAX_FNAME], ext[_MAX_EXT] = "EXE";
  char newpath[_MAX_PATH];
  char *pathvar;
  char rpt_ext[] = "rpt";
  char cdecl_ext[] = "h$$";
  char *ext_ptr;
  size_t strL;
  int result;

    if (declare_file)
	ext_ptr = cdecl_ext;
	else
	ext_ptr = rpt_ext;

    _clearscreen(0);
    HIGH_VIDEO;

    relocate();
    pathvar = get_envpath(RET_PATHVAR_ONLY, "INCLUDE", inpath);
    if (!pathvar) {
       _outtext("INLUDE variable not found\n");
       _outtext("\nPress TAB to use Current Directory, ");
       }
      else {
	 _outtext("INCLUDE = ");
	 _outtext(pathvar);
	 _outtext("\nPress ENTER to Accept, ");
	 }
   _outtext("'Q' to Abort, ESC to Revise.\n");
    do
    {
      result = getch();
      if (result == 0 || result == 0xE0) getch();

      if (result == (int)'q' || result == (int)'Q') exit (0);
    } while (result != 27 && result != 13  && result != 9);
    if (result == 9 && !pathvar) {
	 if (getcwd(inpath, 127)) pathvar = makepathvar(inpath,pathvar);
	}
    else if(result == 27) {
       _outtext("\nNew Include variable: ");
       gets(inpath);
       if (*inpath) {
	  pathvar = makepathvar(inpath,pathvar);}
       }
    strncpy (inpath, path, _MAX_PATH);
    strL = strlen(inpath);
    inpath[strL] = '\0';
   _splitpath( inpath, drive, dir, fname, ext );
   _makepath( Rptfpath, drive, dir, fname, ext_ptr );

   _makepath( rpt_fname, "", "", fname, ext_ptr );

    relocate();
    if (declare_file)
     _outtext("\nHeader File [");
     else _outtext("\nReport File [");
    _outtext(Rptfpath);
    _outtext("]: ");
    LOW_VIDEO;

    fgets (newpath, _MAX_PATH - 1, stdin);
    if (*newpath != '\n') {
	strL = strlen(newpath);
	newpath[(int) --strL] = '\0';
       _splitpath( newpath, drive, dir, rpt_fname, ext );
       _makepath( Rptfpath, drive, dir, rpt_fname, rpt_ext );
       strcat(rpt_fname, ext);
		  }
     relocate();
     return(open_reportfile(START_RPT));
}


FILE *open_c_file(char *filepath, FILE *rptfp)
{
  char drive[_MAX_DRIVE], dir[_MAX_DIR];
  char fname[_MAX_FNAME], ext[_MAX_EXT] ;
  char c_ext[] = ".C";
  char newpath[_MAX_PATH];
  FILE *fp;

 _splitpath(filepath, drive, dir, fname, ext );
 if (!*ext) strcpy (ext, c_ext);
 _makepath(newpath, drive, dir, fname, ext);

  if (declare_file)
     fprintf (rptfp,"\n/*   %s%s   */\n",fname, ext );
   else
    fprintf (rptfp,"\n%s%s\n",fname, ext );

  paginate(rptfp);

  if (   (fp = fopen( newpath, "rt") ) == NULL )
       {
	   relocate();
	   printf ("cannot open file: %s\n", filepath);
	   fprintf (rptfp, "cannot open file: %s\n", filepath);
	   return (NULL);
       }
   strcpy(filepath, fname);
   return(fp);
  }



void get_new_path(struct filespec *top)
{
 char *new_file_buf, buffer[81];
 size_t strL;

 _outtext("\nEnter revised filespec or type Enter to Exit\n");
  HIGH_VIDEO;
  relocate();
 _outtext("\nC File: ");
  LOW_VIDEO;
 fgets (buffer, 80, stdin);
 relocate();

 if (*buffer == '\n') exit (2);

 strL = strlen(buffer);
 strL--;
 *(buffer + strL) = '\0';
 strupr(buffer);

 new_file_buf = (char *) calloc ((size_t) 81, sizeof(char));
 if (!new_file_buf) exit_ca (BAD_FILE_BUF, "");

 strcpy (new_file_buf, buffer);
 top->path = new_file_buf;

}

union REGS regs;

static void crt_grc(int *row, int *col)
{
   regs.h.ah = 3;
   regs.h.bh = 0;         /* page=0 */
   int86(0x10, &regs, &regs);   /* Use video int fctn = 6 */
   *row = regs.h.dh; *col = regs.h.dl;
}

void relocate(void)
{
  int row, col;

  crt_grc(&row, &col);
  if(row <= 1) _clearscreen(0);
  if (row < 24 ) row++;
	else _outtext("                      ");
  _settextposition((short)row, (short) 1);
}

void handle_switches(char ***cmdline_switch, int *argc)
{
   char *p, *__switch = NULL ;
   char no_equal[] = "Missing Equal Sign: Recommend Abort.";
   int c;

   switches_found = 1;   /* global for use in main */
   (**cmdline_switch)++;
   p = **cmdline_switch;
   while ( iswhite(*p) ) p++;
   strupr(p);

     switch (*p)
     {
      case 'C':
	 /*  CDECLARE: /CDE  */
	 /* Creates a 'C'  Declarations file from all modules */
	 if ( *(p + 1) == 'D' && *(p + 2) == 'E') {
	    declare_file = 1;
	    return;
	}
	break;

      case  'F':
	   /*  Set size of FUNCTION POINTER STACK  */
	   if ( *(p + 1) == 'P' && *(p + 2) == 'S') {
	     if( !(__switch =  parse_switch(cmdline_switch, argc, p) ) )
								  break;
	     GLfpSsize = atoi(__switch);
	     return;
	     }
	  break;


      case  'I':
	  /*INCLUDE: /INC */
	  /* Sets New INCLUDE directory */
	  if ( *(p + 1) == 'N' && *(p + 2) == 'C') {
	      if( !(__switch =  parse_switch(cmdline_switch, argc, p)))
								break;
	      strncat(NewInclude, __switch, 127);
	      if( putenv (NewInclude) == -1 ) break;
	      return;
	     }
	   if ( *(p + 1) == 'F' && *(p + 2) == 'D') {
	     IfDefOFF = 1;
	     return;
	     }
	     break;
      case 'N':
	  /*NOINCLUDE   /NOI */
	  if ( *(p + 1) == 'O' && *(p + 2) == 'I') {
	  __Inclf_Limit = 0;
	  return;
	  }
	  /* NO COMPLEX DECLARATORS:  /NCX */
	  if ( *(p + 1) == 'C' && *(p + 2) == 'X') {
	  __Complex_Declarators = 0;
	  return;
	  }

	  break;
      case 'L':
      /* LEVEL  /LEV */
      /* Sets number of include file levels which will be opened */
      /* Default is 5 */
	   if ( *(p + 1) == 'E' && *(p + 2) == 'V') {
	     if( !(__switch =  parse_switch(cmdline_switch, argc, p) ) )
								  break;
	     __Inclf_Limit = atoi(__switch);
	     if  (__Inclf_Limit > 8) __Inclf_Limit = 8;
	     return;
	     }
	  break;

      case 'R':
	 /* REDIRECT SCREEN OUTPUT TO FILE:  /RED */
	 if ( *(p + 1) == 'E' && *(p + 2) == 'D') {
	 if (!(scrn_out = get_redfp()) ) scrn_out = stdout;
	return;
	}
	break;

      case  'S':
	  /* SPLIT REPORT FILE INTO TWO OR THREE FILES: /SPF=n */
	   if ( *(p + 1) == 'R' && *(p + 2) == 'F') {
	     if( !(__switch =  parse_switch(cmdline_switch, argc, p) ) )
								  break;
	     SplitNumber_rpf = atoi(__switch);
	     if(SplitNumber_rpf > 3) SplitNumber_rpf = 3;
	     return;
	     }
	  break;

      case 'U':
	  /* USER HEADER FILES ONLY:  /UHF */
	 if ( *(p + 1) == 'H' && *(p + 2) == 'F') {

	    if(*(p + 3) == ':')  {
	      if(*(p + 4) == 'V')
		 user_hf_VAR = 1;
	      else if(*(p + 4) == 'F')
		 user_hf_FUNC = 1;
	      return;
	      }

	    user_hf_VAR = 1;
	    user_hf_FUNC = 1;
	    return;
	}
	break;

      case 'V':
	 /* VERBOSE=1 SWITCHES ON REPORTING OF WHETHER OR NOT DEFINES */
	 /* HAVE BEEN FOUND */
	if ( *(p + 1) == 'E' && *(p + 2) == 'R') {
	   verbose = 1;
	}
	break;
     }


 if (!__switch) p = no_equal;
 printf
 ("Unrecognized switch: %s\nESC to exit, Any other Key to Continue\n", p);
 c = getch();
 if (c == 27) exit(0);

}


char *parse_switch(char ***cmdline_switch, int *argc, char *p)
{
 char *equal_to = NULL;

	if ( !(equal_to = strchr(p, '=') )) {
	    equal_to = *((*cmdline_switch + 1));
	    if (equal_to) {
		  if ( !(equal_to = strchr(p, '=') )) return (equal_to);
		  *argc-=2;
		  return (*(*cmdline_switch += 2));
		  }
		  }
		  else {
		  *(++equal_to);
		  if (!*equal_to) {
		    equal_to = *((*cmdline_switch + 1));
		    if (equal_to) {
			  *argc-=1;
			  return (*(*cmdline_switch += 1));
			  }
			  }
		  }

      return(equal_to);
}

char *readmakefile(char *file_string)
{
 char makefile[FILENAME_MAX + 1];
 FILE *fp;
 char buffer[160], *ptr, *newline;
 size_t strL = 0;

 strncpy (makefile, (file_string + 1), FILENAME_MAX);
 makefile[FILENAME_MAX] = '\0';
 if( (fp = fopen( makefile, "rt" )) == NULL ) return(NULL);

 while (  fgets (buffer, 160, fp) ) strL += (strlen(buffer) + 1);

 file_string = realloc(file_string, strL);
 memset(file_string, 0, strL);


 rewind(fp);
 while (  fgets (buffer, 160, fp) ) {
   ptr = buffer;
   while (*ptr && is_in(*ptr, " \n")) ptr++;
   if (*ptr) {
	newline = strchr(ptr, '\n');
	if (newline) *newline = '\0';
	newline = strchr(ptr, '&');
	if (newline) *newline = '\0';
	strupr(ptr);
	strcat(file_string, ptr);
	strcat(file_string, " ");
	}
   }


 fclose(fp);
 return(file_string);
}


char *makepathvar(char *inpath, char *pathvar)
{
  size_t strL;

       pathvar =  strchr(NewInclude, '=');
       strL = strlen(inpath);
       if(strL > 137) strL = 137;
       strncat(++pathvar, inpath, strL);
      *(pathvar + strL) = '\0';
      if( (putenv(NewInclude)) == -1) {
	_outtext("\nError in setting Include variable\n");
	return (NULL);
	}
      return(pathvar);
}


static FILE * open_reportfile(int rpt_number)
{
    static FILE *rptfp;
    static char *revrpt[] =  { "PT2.RPT", "PT3.RPT" };


    if (rpt_number != START_RPT) {
      size_t ext;
      char *p = Rptfpath;
      fclose(rptfp);
      ext = strcspn(Rptfpath, ".");
      ext = (ext > 3) ? (ext - 3) : 1;
      *(p + ext) = '\0';
      strcat(Rptfpath, revrpt[rpt_number]);
    }

    if (   (rptfp = fopen( Rptfpath, "wt") ) == NULL )
	  {
	   exit_ca (BAD_FILE, Rptfpath);
	 }

   if (rpt_number == START_RPT) {
       date_time(rptfp);
       heading(rptfp, rpt_fname);
    }

    return(rptfp);
}

/*lint +epu */
