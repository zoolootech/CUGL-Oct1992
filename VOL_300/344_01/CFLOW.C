/*
        HEADER:         CUG236;
        TITLE:          Function Abstractor;
        DATE:           04/19/1987;
        DESCRIPTION:    "Abstracts C function calls and declarations from a C
                        source and produces a listing of the program's calling
                        hierarchy."
        VERSION:        3.0;
        KEYWORDS:       Flow Analysis, Flow Analyzer;
        FILENAME:       CFLOW.C;
        SEE-ALSO:       CFLOW.DOC;
        COMPILERS:      vanilla;
        AUTHORS:        W. C. Colley III, Mark Ellington;
*/

/*
**   CFLOW.C : find module call structure of c program
**   refer to cflow.doc for how to use
**                                      Mark Ellington
**                                      05-27-84
**
**   Ported to portable C.  Required the following changes:
**
**      1)  Stripped BDS C hooks.
**      2)  Stripped C/80 hooks.
**      3)  Allowed for presence/absence of header files "ctype.h"
**              and "string.h".
**      4)  Allowed for possible pre-definition of constants TRUE,
**              FALSE, and EOF.
**      5)  Made variable fptr type FILE * instead of int.
**      6)  Added a #define for the max line length.
**      7)  Made preprocessor directive rejection logic smarter.
**      8)  Removed name conflict between our fgets() and the std
**          library fgets() by changing ours to get_source_line().
**                                      William C. Colley, III
**                                      04-19-87
*/

#include <stdio.h>

/*
 * Portability Note:  The AZTEC C compilers handle the binary/text file
 * dichotomy differently from most other compilers.  Uncomment the following
 * pair of #defines if you are running AZTEC C:
 */

/*
#define getc(f)         agetc(f)
#define putc(c,f)       aputc(c,f)
*/

/*  Portability Note:  If you do not have a header file "ctype.h",
    uncomment the following #define so that the program will look for
    library support.                                                    */

/* #define      NO_CTYPE_H                                              */

#ifdef  NO_CTYPE_H
extern int isalnum();
#else
#include <ctype.h>
#endif

/*  Portability Note:  If you do not have a header file "string.h",
    uncomment the following #define so that the program will look for
    library support.                                                    */

/* #define      NO_STRING_H                                             */

#ifdef  NO_STRING_H
extern int strcmp();
#else
#include <string.h>
#include <dos.h>
#include <dir.h>
#endif

/*  Portability Note:  A few compilers don't know the additional type
    void.  If yours is one of these, uncomment the following #define.   */

/* #define      void            int                                     */

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef EOF
#define EOF     -1
#endif

#define LINE_LENGTH     256     /* Max line length program can handle.  */
#define PAGE_LENGTH     55      /* lines per page                       */

FILE *fptr;                     /* input file pointer                   */
int level;                      /* keep track of level of open "{"s     */
char name[LINE_LENGTH];         /* module name buffer                   */
char ins[LINE_LENGTH];          /* source input line buffer             */
int curchar;                    /* current character in input line
                                        buffer array subscript          */
unsigned short int linenum;     /* current line number                  */
unsigned short int linecnt;     /* hidden line number                   */
int function = FALSE;           /* flag for level zero only             */
char filename[128];             /* holder for filename                  */
short int lpp;                  /* holder for lines per page            */
struct date today;              /* structure for todays date            */
struct time now;                /* structure for current time           */
short int pageno;               /* holder for page number               */
struct ffblk ffblk;             /* holder for file structure            */

/*  Fixed bug that makes _ characters lop off the beginning of function
    names.  WCC3.                                                       */

/**********************************************************************/

int main(argc,argv)
int argc;
char *argv[];
{
   void modules(), prt_hdr();
   int i, done, filenum;
   unsigned long int ttllines;

   if (argc < 2) {
      fprintf(stderr,"\nUsage: cflow infilename.ext [-f] ");
      return TRUE;
   }

   if (done = findfirst(argv[1], &ffblk, 0)) {
      fprintf(stderr,"\nCan't open %s\n",argv[1]);
      return TRUE;
   }

   if (argc > 2)
      if (argv[2][0] == '-' && (argv[2][1] == 'f' || argv[2][1] == 'F'))
         function = TRUE;
      else {
         fprintf(stderr,"Second parameter is either '-f' or '-F'.");
         return TRUE;
      }

   getdate(&today);
   gettime(&now);
   filenum = 0;
   ttllines = 0;

   while (!done) {
      for (i = 0; ffblk.ff_name[i]; i++)
         filename[i] = toupper(ffblk.ff_name[i]);
      filename[i] = NULL;

      if (!(fptr = fopen(filename,"r")))
         break;

      if (!function) {
         pageno = 0;
         prt_hdr();
      } 
      else
         fprintf(stderr,"File: %s\n", filename);

      if (filenum++ > 1)
         ttllines += linenum;
      linenum = linecnt = 0;

      modules();

      fclose(fptr);

      done = findnext(&ffblk);
   }
   if (filenum > 1)
      fprintf(stderr, "Processed %d files, %u lines\n", filenum, ttllines);

   return FALSE;

}

/**********************************************************************/

void modules()          /* find function declarations and calls         */
{
   char c;
   int i, dquote, defcont;
   int incom;      /* comment flag                                 */
   int decl;       /* module declaration line flag                 */
   int quoted;     /* within " quotes "                            */
   int header;     /* within function header (before 1st '{')      */
   int lookbak();
   void comout(), modname(), prt_hdr();

   incom = quoted = header = defcont = dquote = FALSE;
   level = 0;

   while (fgets(ins, LINE_LENGTH, fptr)) { /* read a line of source */
      linenum++;
      decl = FALSE;           /* assume nothing               */
      curchar = 0;

      while (ins[curchar]) {  /* read for significant characters */

comment:
         if (ins[curchar] == '/' && (!quoted)) /* comments ? */
            if (incom) {
               if (curchar && (ins[curchar-1] == '*')) {
                  incom = FALSE;
                  curchar++;
               }
            } 
            else
               if (ins[curchar+1] == '*') {
                  incom = TRUE;
                  curchar += 2;
                  if (ins[curchar] == '/')
                     curchar++;
               }

         if (!incom) {
            if (defcont) { /* delete preprocessor continued lines */
def_cont:
               defcont = TRUE;
               for (i = curchar; ins[i]; i++)
                  switch (ins[i]) {

                  case '\\':
                     i++;
                     break;

                  case '\'':
                     if (!dquote) {
                        if (ins[++i] == '\\')
                           i++;
                        i++;
                     }
                     break;

                  case '"':
                     dquote = !dquote;
                     break;

                  case '/':
                     if (!dquote)
                        if (ins[i+1] == '*') {
                           curchar = i;
                           goto comment;
                        }
                     break;

                  } /* end switch */

               if (ins[i-2] != '\\' || ins[i-1] != '\n')
                  defcont = dquote = FALSE;
               break;
            }

            if (ins[curchar] == '\\') {
               curchar += 2;
               if (!ins[curchar])
                  break;
               continue;
            }

            /* skip double quoted strings */
            if ((c  = ins[curchar]) == '"')
               quoted = !quoted;

            if (!quoted) {
               if (ins[curchar] == '#') {
                  for (i = curchar - 1; i >= 0; i-- )
                     if (!isspace(ins[i]))
                        break;
                  if (i < 0)
                     goto def_cont;
               }

               switch(c) {

               case '\'':
                  if (ins[++curchar] == '\\')
                     curchar++;
                  curchar++;
                  break;

               case '{':  
                  level++;
                  header = FALSE;
                  break;

               case '}':  
                  level--;
                  break;

                  /* "(" always follows function call */
                  /* or declaration */

               case '(':
                  if ((function && level) || header || !lookbak(curchar)) {
                     linenum += linecnt;
                     linecnt = 0;
                     break;
                  }
                  modname();
                  decl = TRUE;
                  header = !level;
                  linenum += linecnt;
                  linecnt = 0;
                  break;

               } /* End switch */
            } /* End if */
         } /* End if */
         curchar++;
      } /* End while */

      if (defcont && (!incom))
         continue;

      /* display argument declarations */

      if (header && !decl && !function) {
         comout(ins);
         printf("%s",ins);
         if ((lpp++) >= PAGE_LENGTH)
            prt_hdr();
      }
   }  /* end while */
   if (!function)
      printf("\n\n >>> End CFLOW of %s <<<\f", filename);
}

/**********************************************************************/

/* look back from position n in string.  called with n indicating '('.
   determine function name                                              */

int lookbak(n)
int n;
{
   int i, j, parens;
   char lins[LINE_LENGTH];
   void comout();

   while (isspace(ins[--n]));
   if (!(isalnum(ins[n]) || (ins[n] == '_')))
      return FALSE;

   /* find leading blank */
   while ((isalnum(ins[n-1]) || (ins[n-1] == '_')) && n)
      --n;

   /* save name */
   /* include variable declarations if module declaration */


   if (!level) {                /* full line if declaration     */
      for (j = n, parens = 1; ins[j++] != '(';);

      while (ins[j] != ';' && ins[j] && parens) {
         if (ins[j] == '(')
            parens++;
         if (ins[j] == ')')
            parens--;
         j++;
      }

      while (isspace(ins[j]))
         j++;

      if (parens && ins[j] != ';')           /* parens are unbalanced */
         /* get following lines to make determination */
         while (fgets(lins, LINE_LENGTH, fptr)) {
            linecnt++;
            i = 0;
            while (lins[i] != ';' && lins[i] && parens) {
               if (lins[i] == '(')
                  parens++;
               if (lins[i] == ')')
                  parens--;
               i++;
            }     
            if (!parens || lins[i] == ';') {
               while (isspace(lins[i]))
                  i++;
               break;
            }
         }

      if (parens)
         return FALSE;

      if (!linecnt) {
         if (ins[j] == ',' || ins[j] == ';')        /* extern or prototype */
            return FALSE;
      } 
      else
         if (lins[i] == ',' || lins[i] == ';')     /* extern or prototype */
            return FALSE;

      i = 0;    
      while (ins[n])
         name[i++] = ins[n++];
   } 
   else {
      i = 0;                     /* function call within function */
      while (isalnum(ins[n]) || (ins[n] == '_'))
         name[i++] = ins[n++];
   }
   name[i] = NULL;
   comout(name);   /* remove comment from name string */
   if (strcmp(name,"if") && strcmp(name,"for") &&
       strcmp(name,"while") && strcmp(name,"switch") &&
       strcmp(name,"return"))
      return TRUE;

   return FALSE;
}

/**********************************************************************/

/* terminate string at comment */

void comout(s)
char *s;
{
   char c;

   while (c = *s++)
      if (c == '/')
         if (*s == '*') {
            *(s - 1) = '\n';
            *s = NULL;
            break;
         }
}

/**********************************************************************/

/* display module name with indentation according to { level */

void modname()
{
   int i, j, k;
   char temps[255], final[255];
   char sep[] =
       "********************************************************************";
   void comout(), prt_hdr();

   if (!level) {
      comout(ins);
      if (function) {
         sprintf(temps, "%-12s :%4u: %-s", filename, linenum, ins);
         for (i = 0; i < 20; i++)
            final[i] = temps[i];
         j = i;
         while (isspace(temps[i]))
            i++;
         k = i;
         while (temps[k] && temps[k] != '(' && temps[k] != '*') {
            while (temps[k])
               if (!isspace(temps[k]) && (temps [k] != '('))
                  k++;
               else
                  break;
            while (isspace(temps[k]))
               k++;
            if (temps[k] != '(' )
               i = k;
         }
         if (temps[k] == '*')
            j--;
         while (temps[i])
            final[j++] =temps[i++];
         final[j] = NULL;
         printf("%s",final);
      } 
      else {
         printf("\n\n");
         if ((lpp += 2) >= PAGE_LENGTH)
            prt_hdr();
         printf("%s\n\n %4u: %s", sep, linenum, ins);
         if ((lpp += 3) >= PAGE_LENGTH)
            prt_hdr();
      }
   } 
   else {
      printf("\n %4u: ", linenum);
      for (j=0; j < level; ++j)
         printf("|  ");
      printf("%s()",name);
      if ((lpp++) >= PAGE_LENGTH)
         prt_hdr();
   }

}

/**********************************************************************/

/* print page header */

void prt_hdr(void)

{
   if (pageno++)
      printf("\f");
   printf("CFLOW of: %s                    ", filename);
   printf(" Date:%d/%d/%02d %02d:%02d:%02d", today.da_mon, today.da_day,
   today.da_year, now.ti_hour, now.ti_min, now.ti_sec);
   printf(" Page: %d\n\n", pageno);
   lpp = 2;
}

/**********************************************************************/
