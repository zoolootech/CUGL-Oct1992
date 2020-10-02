      /***********************************************************

      XCXREF  -  A 'C' Concordance Utility

      Version 1.0   xc     - January, 1982
      Version 1.0   xcxref - May,     1990

      Copyright (c) 1982 by Philip N. Hisley

              Philip N. Hisley
              548H Jamestown Court
              Edgewood, Maryland 21040
              (301) 679-4606

      Released for non-commercial distribution only

      Converted to IBM/PC CI/C86 by David N. Smith, May/June 1983
      with enhancements and Lattice compiler support in December 1983.

              David N. Smith
              44 Ole Musket Lane
              Danbury, CT 06810
              (203) 748-5934
              CompuServe: 73145,153

      Changes Copyright (c) 1983 by David N. Smith
      Permission granted to copy for non-commercial purposes.

      Version v1.0   XCXREF - May, 1990.
      Program functions extended, hence the name change.

              Martin D. Winnick
              5301 El Arbol Drive
              Carlsbad, CA 92008
              (619) 431-0485
              CompuServe: 71665,456
              May 1990

      Changes Copyright (c) 1990 by Martin D. Winnick
      Permission granted to copy for non-commercial use only.
      See the accompanying xcxref.doc file for program history and
      all change details.


      Abstract:

      'XCXREF' is a cross-reference utility for 'C' programs.
      Its will handle nested #include files and properly process
      nested comments.

      Option flags control the following features:

      Usage: xcxref <filename> <flag(s)>

      Flags: -e            = Write program data to log file
             -g            = Ignore missing files
             -i            = Enable #include processing
             -l            = Generate listing only - no xref
             -o <outfile>  = Write output to named file
             -p            = Write output to line printer LPT1
             -r            = Cross-reference 'C' reserved words
             -s            = Write output to video screen
             -w width      = Width of output page; default = 78
                                                   max     = 150
      Flags MUST FOLLOW all input file names

      ***********************************************************/

#include "bios.h"
#include "ctype.h"
#include "direct.h"
#include "dos.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

#ifndef  TRUE
#define  TRUE        1
#define  FALSE       0
#endif

#define  ERROR      -1
#define  FF         0x0C        /* formfeed                            */
#define  FOREVER    for(;;)
#define  LINES_PER_PAGE 60
#define  LPT1       0           /* defines LPT1                        */
#define  MAX_ALPHA  53          /* maximum alpha chain heads           */
#define  MAX_LEN    31          /* maximum identifier length           */
#define  MAX_REF     5          /* maximum refs per ref-block          */
#define  MAX_REFS_LINE  10      /* maximum refs per line               */
#define  MAX_WRD  5000          /* maximum number of identifiers (749) */
#define  MAXCOL     78          /* right margin for listing line       */
#define  MAXLINE   150          /* maximum print line length.          */
#define  MINCOL     30          /* minimum value for -w option         */

struct alpha_hdr
    {
    struct id_blk *alpha_top;
    struct id_blk *alpha_lst;
    };

struct  id_blk
    {
    char  id_name[MAX_LEN];
    struct id_blk *alpha_lnk;
    struct rf_blk *top_lnk;
    struct rf_blk *lst_lnk;
    };

struct  rf_blk
    {
    int  ref_item[MAX_REF];
    int  ref_cnt;
    struct rf_blk *next_rfb;
    };

struct alpha_hdr alpha_vector[MAX_ALPHA];
struct dosdate_t dt;
struct id_blk *id_vector[MAX_WRD];

int     al_count   = 0;       /* number of alpha links        */
int     edtnum     = 0;       /* edit line number             */
int     file_level = 0;       /* file level                   */
int     hash_hits  = 0;       /* number of conflict hits      */
int     id_cnt     = 0;       /* number of unique identifiers */
int     id_count   = 0;       /* number of id structs alloc.  */
int     linum      = 0;       /* line number                  */
int     paglin     = 0;       /* page line counter            */
int     pagno      = 0;       /* page number                  */
int     rf_count   = 0;       /* number of rf structs alloc.  */

short   base_row    = 0;             /* basic data display video row   */
short   maxcol      = MAXCOL;        /* right column for listing       */
short   maxrefs     = MAX_REFS_LINE; /* max references per line        */
short   vline       = 10;            /* work data display video column */

short   do_echo     = TRUE;
short   do_includes = FALSE;
short   do_lprint   = FALSE;
short   do_numbrs   = TRUE;
short   do_outfile  = FALSE;
short   do_res_wds  = FALSE;
short   do_screen   = FALSE;
short   file_queue  = FALSE;
short   ignore      = FALSE;
short   infl_open   = FALSE;
short   lf_open     = FALSE;
short   list_only   = FALSE;
short   log_on      = FALSE;
short   prt_ref     = FALSE;
short   qfl_open    = FALSE;
short   qfirst      = TRUE;

char    current_file[_MAX_PATH] = { '\0' };
char    glbl_file[_MAX_PATH]    = { '\0' };
char    list_file[_MAX_PATH]    = { '\0' };
char    log_file[]              = { "XCXREF.LOG" };
char    work_name[_MAX_PATH]    = { '\0' };

/* Working file name components */
char    wdrive[_MAX_DRIVE]      = { '\0' };
char    wdir[_MAX_DIR]          = { '\0' };
char    wfname[_MAX_FNAME]      = { '\0' };
char    wext[_MAX_EXT]          = { '\0' };

/* Current default directory components */
char    ddrive[_MAX_DRIVE]      = { '\0' };
char    ddir[_MAX_DIR]          = { '\0' };

/* Alternate path name components */
char    adrive1[_MAX_DRIVE]     = { '\0' };
char    adir1[_MAX_DIR]         = { '\0' };
char    adrive2[_MAX_DRIVE]     = { '\0' };
char    adir2[_MAX_DIR]         = { '\0' };
char    adrive3[_MAX_DRIVE]     = { '\0' };
char    adir3[_MAX_DIR]         = { '\0' };
char    adrive4[_MAX_DRIVE]     = { '\0' };
char    adir4[_MAX_DIR]         = { '\0' };
char    xfname[_MAX_FNAME]      = { '\0' };
char    xext[_MAX_EXT]          = { '\0' };

char    b22[]         = { "                      " };
char    *days[]       = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
char    emsgs[71]     = { '\0' };
char    pdate[]       = { "Thu 05/10/90" };
char    prt_line[256] = { '\0' };
char    version[]     = { "v1.0" };
char    xcx_name[]    = { "XCXREF ... " };
char    xname[]       = { "XCXREF.LST" };

FILE    *f_list_file;         /* list  file            */
FILE    *lgfile;              /* program data log file */
FILE    *ofile;               /* general open file     */

/* function prototypes */
struct rf_blk *add_rf(struct rf_blk *adr_ptr, int adr_ref);
struct id_blk *alloc_id(char *aid_token);
struct rf_blk *alloc_rf(int arf_ref);
void chain_alpha(struct id_blk *ca_ptr, char *ca_token);
int  check_for_resword(char *c_token);
void clr_scrn(short crow, short nrows);
void do_prints(int pparm);
void do_print_xrefs(void);
void echo(char c);
void echochar(char c);
int  get_file_char(FILE *cfile, int *f_eof);
int  get_fqueue(char *qfn);
void get_include_fileid(char *token, FILE *ifile, short parm);
int  get_token(FILE *gfile, char *g_token, int *g_toklen, int *g_eoflg, char x_chr);
void list_err(int parm);
void logit(short parm);
void lprintr(char *buffer);
void new_line(void);
int  openlist(char *lname);
FILE *open_rfile(void);
void pause(clock_t mcount);
void pexit(int xparm);
void print_header(void);
int  proc_file(int incnum);
void put_token(char *p_token, int p_ref);
char read_file_char(FILE *rfile, int *r_eoflg, char rd_flg);
void set_csr(short vrow, short vcol);
void use_err(void);

/*************************************************************************/

int main(int p_argc, char **p_argv)
{
    char *arg, **argv;
    int  argc, i, ret;

    argc = p_argc;
    argv = p_argv;
    if (argc < 2)
      {
      printf("\nThere are no option arguments.\n");
      use_err();
      }

    while (--argc != 0)
      {
      if (*(arg = *++argv) == '-')
        {
        switch(*++arg)
          {
          /* log program data to a disk log file. */
          case 'e':
          case 'E': if ((lgfile = fopen(log_file, "w")) != NULL)
                      {
                      log_on = TRUE;
                      break;
                      }
                    else
                      {
                      sprintf(emsgs, "\nCannot open XCXREF log file...%s\n", log_file);
                      pexit(0);
                      }

          /* Ignore missing files. */
          case 'g':
          case 'G': ignore = TRUE;
                    break;

          /* enable processing of #include */
          case 'i':
          case 'I': do_includes = TRUE;
                    break;

          /* xref reserved words also. */
          case 'r':
          case 'R': do_res_wds = TRUE;
                    break;

          /* Program listing only - no xref. */
          case 'l':
          case 'L': list_only = TRUE;
                    break;

          /* write the xref to the disk file named. */
          case 'o':
          case 'O': do_outfile = TRUE;
                    if ((--argc == 0) || (*++argv[0] == '-'))
                      {
                      printf("\nInvalid -o option argument.\n");
                      use_err();
                      }

                    if ((openlist(*++argv)) == 0)
                      {
                      printf("\n%s  %s\n", xcx_name, version);
                      vline += 2;
                      }
                    else
		      { pexit(1); }

                    break;

          /* write xref to the line printer. */
          case 'p':
          case 'P': do_lprint = TRUE;
                    break;

          /* write xref to the video screen.     */
          case 's':
          case 'S': do_screen = TRUE;
                    break;

          /* set page width. */
          case 'w':
          case 'W': if (--argc == 0)
                      {
                      ("\nInvalid -w option argument.\n");
                      use_err();
                      }

		    i = atoi(*++argv);
                    if (i <= MINCOL || i >= MAXLINE)
                      {
                      printf("\nInvalid -w option argument.\n");
                      use_err();
                      }

                    maxcol = i;
                    break;

          default:  printf("\nUnrecognizable option argument.\n");
                    use_err();
          }
        }
      }

    if (!do_outfile && !do_lprint && !do_screen)
      {
      printf("\nAt least one output destination must be selected.\n");
      use_err();
      }

    /* Get current system date from dos. */
    _dos_getdate(&dt);
    sprintf(pdate, "%s %u/%02u/%02u", days[dt.dayofweek],
                                 dt.month, dt.day, dt.year - 1900);

    /* Get currently logged directory. */
    getcwd(current_file, _MAX_DIR);
    strcat(current_file, "\\");
    _splitpath(current_file, ddrive, ddir, xfname, xext);

    /* display program header */
    clr_scrn(0, 25);
    set_csr(vline++, 0);
    printf("%s   %s\n", xcx_name, pdate);
    vline++;

    prt_ref = FALSE;
    for (linum = 0; linum < MAX_WRD; linum++)
      { id_vector[linum] = NULL; }

    for (linum = 0; linum < MAX_ALPHA; linum++)
      { alpha_vector[linum].alpha_top = alpha_vector[linum].alpha_lst = NULL; }

    linum = 0;

    argc = p_argc;
    argc--;
    argv = p_argv;
    while (argc--)
      {
      if (!file_queue)
        {
        /* Get program names from the command line. */
	strcpy(glbl_file, *++argv);

	/* Separate file name into components. */
	_splitpath(glbl_file, wdrive, wdir, wfname, wext);
        }
      else
        {
        /* Get the program names from the list file. */
	/* maintain arg count to preserve while loop */
	argc++;
	if ((ret = get_fqueue(glbl_file)) == -1)
	  { pexit(2); }

	/* If this is false then at eof of list file, */
	/* go back to command line.                   */
	if (!file_queue)
	  {
	  argc--;
	  continue;
	  }
        }

      /* If the first character of a file name is an @  */
      /* it is a reference to a list of file names in a */
      /* list file.  Get the program names, and paths   */
      /* from that file.                                */
      /* NOTE: The first character of any file name in  */
      /*       a list file CANNOT be an @.              */
      /* This @filename may be mixed with program names */
      /* on the command line.  When it is encountered   */
      /* XCXREF will get all program names from the     */
      /* file.  It will return to the command line on   */
      /* its eof; however, any alternate paths set will */
      /* be retained but may be overridden.             */
      if (glbl_file[0] == '@')
        {
        /* maintain arg count to preserve while loop */
	argc++;
        file_queue = TRUE;

        /* delete the leading '@' */
	strcpy(glbl_file, &glbl_file[1]);
        strcpy(wfname, &wfname[1]);
        continue;
        }

      /* No more program names; this is an option; */
      /* we are all done.                          */
      if (glbl_file[0] == '-')
        { break; }

      /* This is where all the work will be done. */
      if ((ret = proc_file(0)) == -1)
        {
        /* If this flag is TRUE then ignore a not-found */
        /* source file and just go to the next one.     */
        if (!ignore)
          { pexit(3); }
        }

      }  /* end while(argc...) */

    /* Process the cross reference.  */
    if (!list_only)
      {
      glbl_file[0] = '\0';
      do_print_xrefs();
      printf("\nAllowable Symbols: %d\n", MAX_WRD);
      printf("Unique    Symbols: %d\n", id_cnt);
      }

    if (do_outfile)
      {
      new_line();
      fprintf(f_list_file, "\nAllowable Symbols: %d\n", MAX_WRD);
      fprintf(f_list_file, "Unique    Symbols: %d\n", id_cnt);
      fclose(f_list_file);
      lf_open = FALSE;
      }

    if (log_on)
      {
      logit(999);
      fclose(lgfile);
      }

}  /* exit xcxref program. */

/*************************************************************************/

FILE *open_rfile()
/* The file_name is in the q????? component variables and  */
/* must be put together into the work_name variable.       */
/* IF                                                      */
/* wdir is already set then only that pathname will be     */
/* tried.  If not found then will exit without trying any  */
/* alternate pathnames.                                    */
/* ELSE ...                                                */
/* The drive and path in ddir, etc., will be tried.        */
/* If the file is not found there alternate path 1-4 will  */
/* be tried next. If not in any of those a not-found will  */
/* be returned to the caller.  Further disposition will be */
/* up to the caller.                                       */

{
  short i   = 0;               /* retry control         */
  int   ret = TRUE;            /* set return as 'found' */

  FOREVER
    {
    /* if wdir is already set then do not          */
    /* do any of the alternate directory searches. */
    if (wdir[0] != '\0')
      { i = 99; }

    switch (i++)
      {
      /* Default directory */
      case 0: strcpy(wdrive, ddrive);
              strcpy(wdir, ddir);
              break;

      /* Alternate 1 directory */
      case 1: if (adrive1[0] != '\0')
                {
                strcpy(wdrive, adrive1);
                strcpy(wdir, adir1);
                break;
                }
              else
                { continue; }

      /* Alternate 2 directory */
      case 2: if (adrive2[0] != '\0')
                {
                strcpy(wdrive, adrive2);
                strcpy(wdir, adir2);
                break;
                }
              else
                { continue; }

      /* Alternate 3 directory */
      case 3: if (adrive3[0] != '\0')
                {
                strcpy(wdrive, adrive3);
                strcpy(wdir, adir3);
                break;
                }
              else
                { continue; }

      /* Alternate 4 directory */
      case 4: if (adrive4[0] != '\0')
                {
                strcpy(wdrive, adrive4);
                strcpy(wdir, adir4);
                break;
                }
              else
                { continue; }

      /* Path already set */
      case 99: break;

      /* not found in any path */
      /* ignore error return.  */
      default: ret = FALSE;
               break;

      }  /* end switch (++path...)  */

    /* File not found exit.  */
    if (!ret)
      { break; }

    /* Put all file name components together. */
    _makepath(work_name, wdrive, wdir, wfname, wext);

    /* Try to open file.  If not able then try next path. */
    if ((ofile = fopen(work_name, "r")) != NULL)
      { break; }

    /* Clear so switch will work. */
    wdir[0] = '\0';

   }  /* end FOREVER... */

  /* Save only .c file names for page header. */
  if (strncmp(wext, ".h", 2) != 0)
    { strcpy(current_file, work_name); }

  return(ofile);
}

/*************************************************************************/

int proc_file(int incnum)
/* The file_name is in the q????? component variables and  */
/* must be put together into the work_name variable.       */
/* -  incnum;        prev. included line number (returned to caller) */
{
  FILE  *infile;        /* input file            */
  char  token[MAX_LEN]; /* token buffer          */
  int   eof_flg;        /* end-of-file indicator */
  int   tok_len;        /* token length          */
  int   i, ret, iret;

  /* Reset edit line counter. */
  edtnum = 0;

  if ((infile = open_rfile()) == NULL)
    {
    sprintf(emsgs, "\nUnable to open input file: %s\n", work_name);
    printf(emsgs);
    vline += 2;
    logit(101);
    return(-1);             /* caller may ignore error */
    }

  infl_open = TRUE;
  if (file_level++ == 0)
    { print_header(); }

  eof_flg = FALSE;
  do
    {
    if (get_token(infile, token, &tok_len, &eof_flg, 0x254))
      {
      if (check_for_resword(token))
        {
        if (strcmp(token, "#include") == 0)
          {
	  do_echo = FALSE;
	  get_include_fileid(token, infile, FALSE);
	  do_echo = TRUE;
          if (!do_includes)
            { continue; }
          else
            {
            /* Process #include file. */
	    new_line();

	    /* Separate #include name into components. */
            _splitpath(token, wdrive, wdir, wfname, wext);

            /* if file not found then ignore the #include file. */
	    if ((iret = proc_file(edtnum)) == -1)
	      { ; }
            }  /* end else ... */
          }  /* end if (strcmp...)  */

        put_token (token, linum);
        }  /* end if (check_for...)  */
      }  /* end if (get_token...)  */
    } while (!eof_flg);

  file_level -= 1;
  fclose(infile);
  infl_open = FALSE;
  return(incnum);
}

/*************************************************************************/
/*  Get the program names from the external list file. */
int  get_fqueue(char *qfn)
{
  char  token[MAX_LEN]; /* token buffer            */
  char  q_p;            /* work q_path number      */

  int   qeof_flg;       /* end-of-file indicator   */
  int   tok_len;        /* token length            */

  FILE  *qfile;         /* program name input file */

  /* On the first entry open the external list file. */
  /* Then on this and subsequent calls return a name */
  /* from the file or set file_queue to FALSE to     */
  /* indicate end of list and continue from command  */
  /* line.                                           */
  if (qfirst)
    {
    if ((qfile = fopen(qfn, "r")) == NULL)
      {
      sprintf(emsgs, "\nCannot open external program list file: %s\n", qfn);
      vline += 2;
      printf(emsgs);
      logit(102);
      return(-1);
      }

    qfl_open = TRUE;
    qeof_flg = qfirst = FALSE;
    } /* end if (qfirst.. ) */

  /* Now get names from the list file.                         */
  /* #qpath are optional paths, if any.  Save in path 1-4.     */
  /* Save only the drive and dir/path, throw away any fn.ext,  */
  /* each is expected as '#qpathn <dr:\path\path\>'; the 'n'   */
  /* may be 1 - 4.                                             */
  /* There must be a space before the '<' and the actual path  */
  /* must be enclosed in the "<>" brackets.                    */
  /* These may be anywhere in the file.  They will become      */
  /* effective when encountered.                               */
  /* To clear an alternate path enter as #qpathn <>            */
  do
    {
    do_echo = FALSE;
    if (get_token(qfile, token, &tok_len, &qeof_flg, '.'))
      {
      if (strncmp(token, "#qpath", 6) == 0)
        {
        /* Save the path number. */
        q_p = token[6];

        /* Now go get the path. */
	get_include_fileid(token, qfile, TRUE);
        switch(q_p)
          {
          /* qpath1 */
	  case 49: _splitpath(token, adrive1, adir1, xfname, xext);
                  break;

          /* qpath2 */
          case 50: _splitpath(token, adrive2, adir2, xfname, xext);
                  break;

          /* qpath3 */
          case 51: _splitpath(token, adrive3, adir3, xfname, xext);
                  break;

          /* qpath4 */
          case 52: _splitpath(token, adrive4, adir4, xfname, xext);
                  break;

          default: break;                 /* max of 4 alternate paths */
          }  /* end switch (path... ) */
        continue;

        }  /* end if(strncmp...)  */

      /* if the token is not a qpath and not eof then assume */
      /* it is a filename in the form of nnnnnnnn.ext        */
      /* If it has a full path it will override any now set. */
      /* - decompose into separate variables and exit.       */
      _splitpath(token, wdrive, wdir, wfname, wext);
      break;
      }  /* end if(get_token...)  */
    } while (!qeof_flg);

  if (qeof_flg)
    {
    fclose(qfile);
    qfl_open = file_queue = FALSE;
    }

  do_echo = TRUE;
  return(0);
}

/*************************************************************************/

void get_include_fileid(char *token, FILE *ifile, short parm)
{
   char c, term;

   while ((term = getc(ifile)) == ' ')
     { echo(term); }

   echo(term);
   if (term == '<' )
     { term = '>'; }

   /* Terminator is > or " */
   if ((term != '>') && (term != '"'))
      {
      sprintf(emsgs, "\nError scanning #INCLUDE fileid: %c\n", term);
      pexit(4);
      }

   do
     {
     if ((c = getc(ifile)) != ' ')
       {
       *token++ = c;
       echo(c);
       }
      else
	{ echo(c); }

     } while (c != term);

   *--token = '\0';

   /* necessary only for list file paths. */
   if (parm)
     {
     if (token[strlen(token)-1] != '\\')
       { strcat(token, "\\"); }
     }
}

/*************************************************************************/

/*
  'getoken' returns the next valid identifier or
  reserved word from a given file along with the
  character length of the token and an end-of-file
  indicator.
*/
int get_token(FILE *gfile, char *g_token, int *g_toklen, int *g_eoflg, char x_chr)
{
  int     c,
          ret = TRUE;
  char    *h_token;
  char    tmpchr;

  h_token = g_token;

  FOREVER
    {
    *g_toklen = 0;
    g_token   = h_token;

    /*
      Scan and discard characters until an alphabetic or
      '_' (underscore) character is encountered or an end-of-file
      condition occurs
    */
    while ((!isalpha(*g_token = read_file_char(gfile, g_eoflg, x_chr))) &&
	    !*g_eoflg                                                   &&
	     *g_token != '_'                                            &&
	     *g_token != '0'                                            &&
             *g_token != '#');

    if (*g_eoflg)
      {
      ret = FALSE;
      break;
      }

    *g_toklen += 1;

    /*
      Scan and collect identified alphanumeric token until
      a non-alphanumeric character is encountered or an
      end-of-file condition occurs
    */
    tmpchr = x_chr;

    while ((isalpha(c = read_file_char (gfile, g_eoflg, x_chr)) ||
	    isdigit(c)                                          ||
	    c == '_'                                            ||
	    c == tmpchr)                                        &&
            !*g_eoflg)
      {
      if (*g_toklen < MAX_LEN)
        {
        *++g_token = c;
        *g_toklen += 1;
        }
      }  /* end while ((isalpha... ) */

    /*
      Check to see if a numeric hex or octal constant has
      been encountered ... if so dump it and try again
    */
    if (*h_token == '0')
      { continue; }

    /* NULL terminate the token */
    *++g_token = NULL;

    /*  Screen out all #token strings except  #include */
    /*  and special #qpathn from qpath list.           */
    if (*h_token == '#')
      {
      if ((strcmp(h_token, "#include") == 0)    ||
	  (strncmp(h_token, "#qpath", 6)  == 0))
        { break; }
      else
        { continue; }
      }

    break;
    }

  return (TRUE);
}

/*************************************************************************/

int get_file_char(FILE *cfile, int *f_eof)
 {
  int fc;

  if ((fc = fgetc(cfile)) == EOF)
    {
    *f_eof = TRUE;
    fc = NULL;
    }
  else
    { *f_eof = FALSE; }

  return(fc);
}

/*************************************************************************/

char read_file_char(FILE *rfile, int *r_eoflg, char rd_flg)

  /*
    'read_file_char' returns the next valid character in a file
    and an end-of-file indicator. A valid character is defined
    as any which does not appear in either a commented or a
    quoted string ... 'read_file_char' will correctly handle
    comment tokens which appear within a quoted string.
  */

{
  int  c;
  int  double_quotes;      /* double quoted string flag */
  int  single_quotes;      /* single quoted string flag */
  int  comment_start;      /* comment start flag        */
  int  comment_end;        /* comment end flag          */
  int  nesting_level;      /* comment nesting level     */
  int  transparent;        /* transparency flag         */

  double_quotes = single_quotes = comment_start = FALSE;
  comment_end   = transparent   = FALSE;
  nesting_level = 0;

  FOREVER
    {
    /* Fetch character from file  */
    c = get_file_char(rfile, r_eoflg);

    if (*r_eoflg)
      { break; }              /* EOF encountered */

    if (c == '\n')
      { new_line(); }
    else
      { echo(c); }

    if (rd_flg == 0x254)
      { break; }

    if (transparent)
      {
      transparent = !transparent;
      continue;
      }

    if (c == '\\')
      {
      transparent = TRUE;
      continue;
      }

    /*
      If the character is not part of a quoted string
      check for and process commented strings...
      nested comments are handled correctly but unbalanced
      comments are not ... it is assumed that the syntax of
      the program being xref'd is correct.
    */
    if (!double_quotes && !single_quotes)
      {
      /* test for end of a comment block. */
      if (c == '*' && nesting_level && !comment_start)
        {
        comment_end = TRUE;
        continue;
        }

      /* end of a comment */
      if (c == '/' && comment_end)
        {
        nesting_level -= 1;
        comment_end = FALSE;
        continue;
        }

      comment_end = FALSE;
      if (c == '/')
        {
        comment_start = TRUE;
        continue;
        }

       if (c == '*' && comment_start)
         {
         nesting_level += 1;
         comment_start = FALSE;
         continue;
         }

       /* if nesting_level > 0 then we are in a comment block; */
       /* throw away characters until nesting_level is zero.   */
       comment_start = FALSE;
       if (nesting_level)
	 { continue; }
      }

    /* Check for and process quoted strings */
    if ( c == '"' && !single_quotes)
      {
      /* toggle quote flag */
      double_quotes =  !double_quotes;
      continue;
      }

    if (double_quotes)
      { continue; }

    if (c == '\'')
      {
      /* toggle quote flag */
      single_quotes = !single_quotes;
      continue;
      }

    if (single_quotes)
      { continue; }

    /* Valid character ... exit function. */
    break;
    }  /* end FOREVER */

  return (c);
}

/*************************************************************************/

int check_for_resword(char *c_token)
{
  char  u_token[MAX_LEN];
  int   ret,
        i = 0;

      if (do_res_wds)
	{ return(TRUE); }

      do
        {
        u_token[i] = toupper(c_token[i]);
        } while (c_token[i++] != NULL);

      ret = TRUE;
      switch(u_token[0])
        {
        case 'A': if (strcmp(u_token,"AUTO")      == 0)
		    { ret = FALSE; }
                  break;

        case 'B': if (strcmp(u_token,"BREAK")     == 0)
		    { ret = FALSE; }
                  break;

        case 'C': if ((strcmp(u_token,"CASE")     == 0)  ||
                      (strcmp(u_token,"CHAR")     == 0)  ||
                      (strcmp(u_token,"CONST")    == 0)  ||
                      (strcmp(u_token,"CONTINUE") == 0))
		    { ret = FALSE; }
                  break;

        case 'D': if ((strcmp(u_token,"DEFAULT")  == 0)  ||
                      (strcmp(u_token,"DO")       == 0)  ||
                      (strcmp(u_token,"DOUBLE")   == 0))
		    { ret = FALSE; }
                  break;

        case 'E': if ((strcmp(u_token,"ELSE")     == 0)  ||
                      (strcmp(u_token,"ENTRY")    == 0)  ||
                      (strcmp(u_token,"ENUM")     == 0)  ||
                      (strcmp(u_token,"EXTERN")   == 0))
		    { ret = FALSE; }
                  break;

        case 'F': if ((strcmp(u_token,"FLOAT")   == 0)   ||
                      (strcmp(u_token,"FOR")     == 0))
		    { ret = FALSE; }
                  break;

        case 'G': if (strcmp(u_token,"GOTO")     == 0)
		    { ret = FALSE; }
                  break;

        case 'I': if ((strcmp(u_token,"IF")      == 0)   ||
                      (strcmp(u_token,"INT")     == 0))
		    { ret = FALSE; }
                  break;

        case 'L': if (strcmp(u_token,"LONG")     == 0)
		    { ret = FALSE; }
                  break;

        case 'R': if ((strcmp(u_token,"REGISTER")== 0)   ||
                      (strcmp(u_token,"RETURN")  == 0))
		    { ret = FALSE; }
                  break;

        case 'S': if ((strcmp(u_token,"SHORT")   == 0)   ||
                      (strcmp(u_token,"SIGNED")  == 0)   ||
                      (strcmp(u_token,"SIZEOF")  == 0)   ||
                      (strcmp(u_token,"STATIC")  == 0)   ||
                      (strcmp(u_token,"STRUCT")  == 0)   ||
                      (strcmp(u_token,"SWITCH")  == 0))
		    { ret = FALSE; }
                  break;

        case 'T': if (strcmp(u_token,"TYPEDEF")  == 0)
		    { ret = FALSE; }
                  break;

        case 'U': if ((strcmp(u_token,"UNION")   == 0)   ||
                      (strcmp(u_token,"UNSIGNED")== 0))
		    { ret = FALSE; }
                  break;

        case 'V': if ((strcmp(u_token,"VOID")    == 0)   ||
                      (strcmp(u_token,"VOLATILE")== 0))
		    { ret = FALSE; }
                  break;

        case 'W': if (strcmp(u_token,"WHILE")    == 0)
		    { ret = FALSE; }
                  break;
        }

  return(ret);
}

/*************************************************************************/

/* Install parsed token and line reference in linked structure */

void put_token(char *p_token, int p_ref)
{
  int  hash_index, i,
       found = FALSE,
       j     = 0,
       d     = 1;

  struct id_blk *idptr;
  struct rf_blk *rfptr;

  if (list_only)
    { return; }

  /* Hashing algorithm is far from */
  /* optimal but is adequate for a */
  /* memory-bound index vector!    */
  for (i = 0; p_token[i] != NULL; i++)
    { j = j * 10 + p_token[i]; }

  hash_index = abs(j) % MAX_WRD;
  do
    {
    if ((idptr = id_vector[hash_index]) == NULL)
      {
      id_cnt++;
      idptr = id_vector[hash_index] = alloc_id(p_token);
      chain_alpha(idptr, p_token);
      idptr->top_lnk = idptr->lst_lnk = alloc_rf(p_ref);
      found = TRUE;
      }
    else
      if(strncmp(p_token, idptr->id_name, MAX_LEN) == 0)
        {
        idptr->lst_lnk = add_rf(idptr->lst_lnk, p_ref);
        found = TRUE;
        }
      else
        {
        hash_index += d;
        d += 2;
        hash_hits++;
        if (hash_index >= MAX_WRD)
          hash_index -= MAX_WRD;

	if (d >= MAX_WRD)
          {
          sprintf(emsgs, "\nSymbol table overflow\n");
          pexit(5);
          }
        }
    } while (!found);
}

/*************************************************************************/

void chain_alpha(struct id_blk *ca_ptr, char *ca_token)
{
  char  c;

  struct id_blk *cur_ptr;
  struct id_blk *lst_ptr;

  if ((c = ca_token[0]) == '_')
    { c = 0; }
  else
    if (isupper(c))
      { c = 1 + ((c-'A')*2); }
    else
      { c = 2 + ((c-'a')*2); }

  FOREVER
    {
    if (alpha_vector[c].alpha_top == NULL)
      {
      alpha_vector[c].alpha_top = alpha_vector[c].alpha_lst = ca_ptr;
      ca_ptr->alpha_lnk = NULL;
      break;
      }

    /*
    check to see if new id_blk should be inserted between
    the alpha_vector header block and the first id_blk in
    the current alpha chain
    */
    if (strncmp(alpha_vector[c].alpha_top->id_name, ca_token, MAX_LEN) > 0)
      {
      ca_ptr->alpha_lnk = alpha_vector[c].alpha_top;
      alpha_vector[c].alpha_top = ca_ptr;
      break;
      }

    if (strncmp(alpha_vector[c].alpha_lst->id_name, ca_token, MAX_LEN) < 0)
      {
      alpha_vector[c].alpha_lst->alpha_lnk = ca_ptr;
      ca_ptr->alpha_lnk = NULL;
      alpha_vector[c].alpha_lst = ca_ptr;
      break;
      }

    cur_ptr = alpha_vector[c].alpha_top;
    while (strncmp(cur_ptr->id_name, ca_token, MAX_LEN) < 0)
     {
     lst_ptr = cur_ptr;
     cur_ptr = lst_ptr->alpha_lnk;
     }

    lst_ptr->alpha_lnk = ca_ptr;
    ca_ptr->alpha_lnk  = cur_ptr;
    break;
    }

  al_count++;
}

/*************************************************************************/

struct id_blk *alloc_id(char *aid_token)
{
    int  i = 0;
    struct id_blk *aid_ptr;

    if ((aid_ptr = malloc(sizeof(struct id_blk))) == NULL)
      {
      sprintf(emsgs, "\nUnable to allocate identifier block\n");
      pexit(6);
      }

    do
      {
      aid_ptr->id_name[i] = aid_token[i];
      } while ((aid_token[i++] != NULL) && (i < MAX_LEN));

    id_count++;
    return(aid_ptr);
}

/*************************************************************************/

struct rf_blk *alloc_rf(int arf_ref)
{
    short  i;
    struct rf_blk *arf_ptr;

    if ((arf_ptr = malloc(sizeof(struct rf_blk))) == NULL)
      {
      sprintf(emsgs, "\nUnable to allocate reference block\n");
      pexit(7);
      }

    arf_ptr->ref_item[0] = arf_ref;
    arf_ptr->ref_cnt     = 1;
    arf_ptr->next_rfb    = NULL;

    for (i = 1; i < MAX_REF; i++)
      { arf_ptr->ref_item[i] = NULL; }

    rf_count++;
    return(arf_ptr);
  }

/*************************************************************************/

struct rf_blk *add_rf(struct rf_blk *adr_ptr, int adr_ref)
{
    struct rf_blk *tmp_ptr;

    tmp_ptr = adr_ptr;
    if (adr_ptr->ref_cnt == MAX_REF)
      { tmp_ptr = adr_ptr->next_rfb = alloc_rf(adr_ref); }
    else
      { adr_ptr->ref_item[adr_ptr->ref_cnt++] = adr_ref; }

    return (tmp_ptr);
}

/*************************************************************************/

/*
   build cross reference print lines.
   when a line is full then write to:
     the screen,
     a file,
     the line printer;
   depending on program options selected.
*/
void do_print_xrefs()
{
  int  prf_cnt, ptb_cnt, i, line_cnt;
  int  pref = TRUE;

  char wk_line[21] = { '\0' };
  char work[21]    = { '\0' };

  struct id_blk *pid_ptr;
  struct rf_blk *ptb_ptr;

  printf ("\nWriting Cross Reference Table... Please Wait...\n");
  do_numbrs = FALSE;
  print_header();

  for (i = 0; i < MAX_ALPHA; i++)
    {
    if ((pid_ptr = alpha_vector[i].alpha_top) != NULL)
      {
      do
        {
        /* Place reference variable in print line */
        prt_line[0] = '\0';
        sprintf(prt_line, "%-20.19s: ", pid_ptr->id_name);
        ptb_ptr = pid_ptr->top_lnk;
        line_cnt = prf_cnt = 0;

        do
          {
          if (prf_cnt == MAX_REF)
            {
            prf_cnt = 0;
            ptb_cnt = ptb_ptr->ref_cnt;
            ptb_ptr = ptb_ptr->next_rfb;
            }

          /* if (ptb_cnt > MAX_REF) */
          if (ptb_ptr != NULL)
            {
            if ((pref = ptb_ptr->ref_item[prf_cnt++]) != 0)
              {
              sprintf(wk_line, "%4d ", pref);
              strcat(prt_line, wk_line);

              /* if the number of references or the next reference */
              /* line number may exceed the print line capacity    */
              /* then print the line and start another.            */
              if ((strlen(prt_line) + (strlen(itoa(linum, work, 10))) > maxcol) ||
                  (++line_cnt == maxrefs))
                {
                /* print file line, screen line, etc. */
                do_prints(FALSE);
                new_line();
                sprintf(prt_line, b22);
                line_cnt = 0;
                }
              }  /* end if ((pref... ) */
            }  /* end if (ptb->ptr...) */
          else
            {
            pref = 0;
            /* print file line, screen line, etc. */
            do_prints(FALSE);
            }

          } while (pref);

        do_prints(FALSE);
        new_line();
        } while ((pid_ptr = pid_ptr->alpha_lnk) != NULL);
      }
   }  /* end for(i... */

   echo('\n');
   echo(FF);
}

/*************************************************************************/
void do_prints(int pparm)
{
  if (prt_line[0] != '\0')
    {
    /* print file line, screen line, etc. */
    if (do_outfile)
      {
      if ((fprintf(f_list_file, prt_line)) == ERROR)
        { list_err(3); }
      }

    if (do_screen)
      { printf(prt_line); }
    else
      {
      if (pparm)
        {
	if (linum % 60 == 1)            /* sixty dots per line. */
          {
	  if (vline >= 24)              /* if at bottom line of screen; */
            {
	    clr_scrn(base_row+1, 24);   /* clear screen and position at line 2. */
            vline++;
            }
          set_csr(vline++, 0);
	  printf("\n<%d> ", linum);
          }
	printf(".");
        fflush(stdout);
        }
      }

    if (do_lprint)
      { lprintr(prt_line); }

    /* Clear the print line. */
    prt_line[0] = '\0';
    }
}

/*************************************************************************/

void print_header()
{
  if (pagno++ != 0)
    {
    echo('\n');
    echo(FF);
    }

  /* _makepath(current_file, wdrive, wdir, wfname, wext); */
  sprintf(prt_line, "%s  %s  %s    %s  Page %d", xcx_name, glbl_file, current_file, pdate, pagno);

  /* print file line, screen line, etc. */
  if (do_screen)
    {
    clr_scrn(base_row, 25);
    set_csr(vline++, 0);
    }

  do_prints(FALSE);
  echo('\n');
  paglin = 3;
  new_line();            /* number first line on next page. */
}

/*************************************************************************/

void new_line()
{
  echo('\n');
  if (++paglin >= LINES_PER_PAGE)
    { print_header(); }
  else
    {
    /* no line numbers when doing xref table. */
    if (do_numbrs)
      {
      if (!prt_ref)
        {
        sprintf(prt_line, "%-4d %4d: ", ++linum, ++edtnum);
        if (do_screen)
          {
          if (vline >= 24)
            {
	    clr_scrn(base_row+1, 24);
            vline++;
            }
          set_csr(vline++, 0);
          }

	do_prints(TRUE);
        }  /* end if (!prt_ref...) */
      }  /*  end if (nparm...) */
    }  /* end else ... */
}

/*************************************************************************/

void echo(char c)
{
   static int col = 11;
   short i;

   if(do_echo)
     {
     echochar(c);

     if (c == '\n' )
       { col = 11; }
     else if (++col > maxcol )
       {
       col = 11;
       paglin++;
       echochar('\n');
       for (i = 1; i <= 11; i++)
          { echochar(' '); }
       }
     }  /* end if(do_echo ... */
}

void echochar(char c)
{
   if (do_outfile)
     {
     if ((fprintf(f_list_file, "%c", c)) == ERROR)
       { list_err(1); }
     }

   if (do_screen)
     { printf("%c", c); }

   if (do_lprint)
     {
     prt_line[0] = c;
     prt_line[1] = '\0';
     lprintr(prt_line);
     }
}

/*************************************************************************/

/* Print using direct call to the system BIOS, */
void lprintr(char *buffer)
{
    char *p;
    int  key;
    union REGS inregs, outregs;

    /*
    Check for printer available and ready.
    Fail if any error bit is on or if a selected operation bit is off.
    */
    FOREVER
      {
      inregs.h.ah = 0x2;              /* get printer status. */
      inregs.x.dx = LPT1;
      int86( 0x17, &inregs, &outregs );
      if ((outregs.h.ah & 0x29)   ||  /* 1 = out-of-paper, i/o-error, time-out */
          !(outregs.h.ah & 0x80)  ||  /* 1 = not-busy                          */
          !(outregs.h.ah & 0x10) )    /* 1 = selected                          */
        {
        /* Compensate for slow printer response...     */
        /* Wait one second and then try printer again. */
        pause((clock_t) 1000);
        inregs.h.ah = 0x2;
        inregs.x.dx = LPT1;
        int86( 0x17, &inregs, &outregs );
        if ((outregs.h.ah & 0x29)    ||
             !(outregs.h.ah & 0x80)  ||
             !(outregs.h.ah & 0x10))
          {
          fflush(stdin);                    /* Clear keyboard buffer. */
          printf("\nPrinter not ready...\n");
          printf("\nPress 'Q' to quit - XREF will not be printed\n");

          /* Cannot redirect if a list file already assigned. */
          if (list_file[0] == '\0')
            { printf("              'D' to redirect to 'XCXREF.LST' disk file\n"); }

          printf("               'R' to retry the printer.\n");
          key = getche();
          switch(toupper(key))
            {
            /* Redirect printer output to XCXREF.LST file */
            /* and close printer.                       */
            case 'D': if (list_file[0] == '\0')
                        {
                        if ((openlist(xname)) == 0)
                          {
                          do_lprint = FALSE;
                          return;
                          }
                        else
                          { logit(103); }
                        }
                      break;

            /* Quit - close printer. */
            case 'Q': do_lprint = FALSE;
                      return;

            /* Retry printer. */
            case 'R': break;

            /* Retry printer. */
            default:  break;
            }  /* end switch ... */
          }  /* end if ((outregs... ) */
        }

      /* Printer is ready. */
      break;

      }  /* end FOREVER... */

    /* Output a string to the printer using DOS function 0x5. */
    for (p = buffer; *p != 0; p++)
      { bdos(0x05, *p, 0); }

    /* Clear print line buffer. */
    buffer[0] = '\0';
}

/*************************************************************************/

/* Position the cursor using direct call to the system BIOS, */
/* vrow 0, vcol 0 == upper left.                             */
void set_csr(short vrow, short vcol)
{
    union REGS inregs, outregs;

    inregs.h.dh = vrow;               /* set row register. */
    inregs.h.dl = vcol;               /* set col register. */
    inregs.x.bx = 0;                  /* page number       */
    inregs.h.ah = 2;                  /* bios position cursor */
    int86(0x10, &inregs, &outregs);
}

/*************************************************************************/

/* Clear the screen using direct call to the system BIOS, */
/* Be CAREFUL, no validation of row/col is done.          */
/* Use crow  = 0          and                             */
/*     nrows = 25 to clear the entire screen.             */
void clr_scrn(short crow, short nrows)
{
    union REGS inregs, outregs;

    inregs.h.ch = crow;               /* upper left  corner row  */
    inregs.h.cl = 0;                  /*                    col  */
    inregs.h.dh = crow + nrows;       /* lower right corner row  */
    inregs.h.dl = 79;                 /*                    col  */
    if (crow == 0 && nrows == 25)
      { inregs.h.al = 0; }            /* indicates entire screen */
    else
      { inregs.h.al = nrows; }

    inregs.h.ah = 6;                  /* scroll page up          */
    inregs.x.bx = 0;                  /* page number             */
    int86(0x10, &inregs, &outregs);
    vline = base_row;
}

/*************************************************************************/

/* Pause for a specified number of microseconds. */
void pause(clock_t mcount)
{
    clock_t mtime;

    mtime = mcount + clock();
    while (mtime > clock());
}

/*************************************************************************/

int openlist(char *lname)
{
  if ((f_list_file = fopen(lname, "w")) == NULL)
    {
    sprintf(emsgs, "\nUnable to create XCXREF list file - %s\n", lname);
    printf(emsgs);
    vline += 2;
    return(-1);
    }

  lf_open = TRUE;
  strcpy(list_file, lname);
  return(0);
}

/*************************************************************************/

void list_err(int parm)
{
  sprintf(emsgs, "\nWrite error <%d> on XCXREF list output file - %s\n", parm, list_file);
  pexit(8);
}

/*************************************************************************/

void use_err()
{
  printf("%s  %s\n\n", xcx_name, version);
  printf("\nXCXREF: Invalid parameter specification\n\n");
  printf("Usage: xcxref <filename>... <flag(s)>\n\n");
  printf("Flags: -e            = Write program data to log file\n");
  printf("       -g            = Ignore missing files\n");
  printf("       -i            = Enable #include processing\n");
  printf("       -l            = Generate listing only - no xref\n");
  printf("       -o <outfile>  = Write output to named file\n");
  printf("       -p            = Write output to line printer LPT1\n");
  printf("       -r            = Cross-reference reserved words\n");
  printf("       -s            = Write output to video screen\n");
  printf("       -w width      = Width of output page; default = 78\n");
  printf("                                             maximum = 150\n");
  printf("Flags MUST FOLLOW all input file names");
  pexit(99);
}

/*************************************************************************/

/* Display program data only if flag is on. */

void logit(short parm)
{
  if (log_on)
    {
    fprintf(lgfile, emsgs);
    emsgs[0] = '\0';
    fprintf(lgfile, "XCXREF program data...ID# %d\n", parm);
    fprintf(lgfile, "sizesof  id_blk = %d; rf_blk = %d; alpha_hdr = %d\n",
             (sizeof(struct id_blk)), (sizeof(struct rf_blk)), (sizeof(struct alpha_hdr)));
    fprintf(lgfile, "id_blk count - %d; rf_blk count %d\n", id_count, rf_count);
    fprintf(lgfile, "Line nbr - %d; Edit line nbr - %d\n", linum, edtnum);
    fprintf(lgfile, "Page nbr - %d; Unique id cnt - %d; Alpha cnt - %d\n", pagno, id_cnt, al_count);
    fprintf(lgfile, "Nbr of conflict hits - %d; File level - %d\n", hash_hits, file_level);
    fprintf(lgfile, "Page line counter - %d; Right col margin - %d\n", paglin, maxcol);
    fprintf(lgfile, "Active file - %s\n", current_file);
    fprintf(lgfile, "List file - %s\n", list_file);
    fprintf(lgfile, "Gbl file - %s\n", glbl_file);

    fprintf(lgfile, "wdrive:dir:fname.ext %s%s%s%s\n", wdrive, wdir, wfname, wext);
    fprintf(lgfile, "adrive1:dir1 %s%s\n", adrive1, adir1);
    fprintf(lgfile, "adrive2:dir2 %s%s\n", adrive2, adir2);
    fprintf(lgfile, "adrive3:dir3 %s%s\n", adrive3, adir3);
    fprintf(lgfile, "adrive4:dir4 %s%s\n", adrive4, adir4);

    fprintf(lgfile,"qfl_open %d; lf_open %d; infl_open %d; do_res_wds %d; ignore %d\n",
			qfl_open, lf_open, infl_open, do_res_wds, ignore);
    fprintf(lgfile,"do_outfile %d; do_screen %d; list_only %d; do_includes %d; log_on %d\n",
                        do_outfile, do_screen, list_only, do_includes, log_on);
    fprintf(lgfile,"do_lprint %d; do_numbrs %d; qfirst %d; file_queue %d\n",
			do_lprint, do_numbrs, qfirst, file_queue);
    }
}

/*************************************************************************/

/* Error exit from program */

void pexit(int xparm)
{
  printf(emsgs);

  /* Write console message and log data. */
  if (xparm < 100)
    { logit(xparm); }

  /* if log on print any partial xref table. */
  if (log_on)
    {
    do_print_xrefs();
    logit(100);
    fclose(lgfile);
    }

/*
  if (infl_open)
    { fclose(infile); }
*/

  if (lf_open)
    { fclose(f_list_file); }

  exit(xparm);
}
