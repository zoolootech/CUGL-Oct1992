/*
        HEADER:         CUG236;
        TITLE:          Cross Reference Generator;
        DATE:           04/27/1988;
        DESCRIPTION:    "C-language program cross-referencer, modified for
                            Microsoft C5.0, with enhancements."
        VERSION:        1.1;
        KEYWORDS:       Cross Reference;
        FILENAME:       XC2.C;
        SEE-ALSO:       CUG126, CUG171;
        COMPILERS:      Vanilla;
        AUTHORS:        Phillip N. Hisley, David N. Smith, Fred C. Smith,
                        William C. Colley III;
*/
/**********************************************************

        XC - A 'C' Concordance Utility

        Version 1.0 January, 1982

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

        Changes Copyright (c) 1983 by David N. Smith

        PC Enhancements include:

            1)  Nested #INCLUDE statements
            2)  Single spaced cross-reference list
            3)  Removal of tabbing on output device
                (Since many printers don't support it)
            4)  #INCLUDE statements with both "--" and <-->
                syntax and with a full fileid in the quotes.
            5)  Multiple input filenames on command line.


        Converted to Microsoft C V5.0 with enhancements.
        November/December 1987.

                Fred C. Smith
                20 Whipple Ave.
                Stoneham, MA 02180

        Enhancements/modifications include:

        1)  Call to reargv() at beginning of main(). If you are running
            Allen Holub's Unix-like shell, reargv() will rebuild argc/argv
            to contain the 2kbyte command-line which the shell provides.
            If not using that shell, reargv is a no-op. (That shell is
            available from M&T Books for around $40, including source!)
        2)  Modify the include file lookups to use the same algorithm as
            used by the Microsoft C compiler, i.e., look first in the
            current directory for quote-delimited files. If a complete
            pathname is specified look there only then quit. Any remaining
            includes, including '<' or '"' delimited ones are then
            searched for in the place(s) (if any) specified in the
            optional parameter to the -i switch. If not found there,
            searches the pathname(s) specified in the INCLUDE environment
            variable.
        3)  Modify the -i switch so that an optional trailing argument
            specifies an alternate path for the includes, as in the -I
            switch in the Microsoft C compiler's "cl" command. Multiple
            -i switches may be given, each with a single pathname. These
            multiple pathnames are saved in the order given.
        4)  Make the default behavior to assume that comments are NOT
            nested. Some PC-based C compilers (e.g. Lattice) support
            nested comments, but Microsoft's does not. Also, those on
            Unix and other larger systems do not. The ANSI standard
            does not. For those who prefer to have support for nested
            comments, a new command-line switch has been added so that
            you can (optionally) have your cake, and eat it, too!
        5)  Add recognition of keywords recognized in ANSI C, i.e.,
            const, enum, signed, volatile, void. Remove recognition of
            entry as a keyword (not in ANSI C). Also, add optional
            recognition of Microsoft-specific keywords: cdecl, far,
            fortran, huge, near and pascal. Use the -m switch for this
            last group.
        6)  Fix the awful kludge which previously existed in xc, in
            which struct rf_blk.ref_cnt was declared as an int, but
            under certain circumstances was used to hold a pointer to
            a struct rf_blk. Replaced the declaration of ref_cnt as
            an int with an appropriately declared union of the two
            types needed.
        7)  Minor tweaks to proc_file to fix incorrect line numbers
            being generated around included files.
        8)  In fil_chr(), used ferror() and feof() to implement the
            original error-checking logic which had been commented
            out in previous versions of the code.

    Version 1.1 -- April 1988:

        Microsoft-isms removed by William C. Colley, III.  In particular
        the function access() is not available in many environments.  A
        reorganization of the code eliminates the use of this function.  Note,
        however, that the function open_include_file() does chop on file names
        which is an inherently non-portable operation, so that function will
        have to be cut to fit different operating systems' file naming
        conventions.  Also, the call to getenv() may have to be modified or
        removed.

        Also, added the -x option to remove the directories specified in the
        INCLUDE environment variable from the search path for #include files.
        WCC3.

    Abstract:

        'XC' is a cross-reference utility for 'C' programs.  Its has the
        ability to handle nested include files to a depth of 8 levels and
        properly processes nested comments as supported by BDS C. Option flags
        support the following features:

            - Routing of list output to disk
            - Cross-referencing of reserved words
            - Processing of nested include files
            - Generation of listing only

        Usage: xc <filename> <flag(s)>

        Flags:  -i [pathname]     = Enable file inclusion
                -l                = Generate listing only
                -m                = recognize Microsoft-specific keywords
                -n                = Allow nested comments
                -o <filename>     = Write output to named file
                -r                = Cross-ref reserved words
                -w <width>        = Maximum listing width
                -x                = Exclude default include file dir's

        Note:   For the -i option, the pathname is optional.  The -i flag may
                be given multiple times, each time with a pathname. These
                pathnames are saved, building a list of places to search for
                include files.  If no list is given the standard places will
                be used.  If the -i option flag is not used, includes will not
                be cross-referenced.

***********************************************************/

/* Compiler specific stuff */

#define LINT_ARGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <alloc.h>         /* was malloc.h dnh 7/21/89 */
#include <string.h>
#undef LINT_ARGS

/* end compiler specific section */

/* Function declarations for xc's routines. */

#include "xc2.h"

#ifndef TRUE
#define TRUE        1
#define FALSE       0
#endif

#define DUMMY       0       /* a dummy integer value */
#define ERROR       -1
#define MAX_REF     5       /* maximum refs per ref-block */
#define MAX_LEN     32      /* maximum identifier length */
                            /* was 20 dnh 7/21/89 - deeper error !! */
#define MAX_WRD     749     /* maximum number of identifiers */
#define MAX_ALPHA   53      /* maximum alpha chain heads */
#define REFS_PER_LINE 10    /* maximum refs per line */
#define LINES_PER_PAGE 60
#define MAXCOL      78      /* default max column number for listing line */
#define MINCOL      30      /* minimum value for -w option */
#define PATHLEN     128     /* maximum pathname length in chars. */
#define FF          0x0C    /* formfeed */

typedef union {
    int cnt;
    struct rf_blk *pnext;
} cnt;

struct rf_blk {
    int  ref_item[MAX_REF];
    cnt ref_cnt;
} onerf;

struct id_blk {
    char id_name[MAX_LEN];
    struct id_blk *alpha_lnk;
    struct rf_blk *top_lnk;
    struct rf_blk *lst_lnk;
} oneid;

struct id_blk *id_vector[MAX_WRD];

struct alpha_hdr {
    struct id_blk *alpha_top;
    struct id_blk *alpha_lst;
};

struct alpha_hdr alpha_vector[MAX_ALPHA];

int lin_refs = MAXCOL/7;/* max refs on a printed line */
int linum;              /* line number */
int edtnum = 0;         /* edit line number */
int fil_cnt = 0;        /* active file index */
int wrd_cnt = 0;        /* token count */
int pagno = 0;          /* page number */
int id_cnt = 0;         /* number of unique identifiers */
int rhsh_cnt = 0;       /* number of conflict hits */
int filevl = 0;         /* file level  */
int paglin = 0;         /* page line counter */
int maxcol = MAXCOL;    /* maximum right column for listing line */
int prt_ref = FALSE;
char act_fil[PATHLEN];
char lst_fil[PATHLEN];
char gbl_fil[PATHLEN];
char i_path[PATHLEN];
FILE *f_lst_fil;
int n_flg = FALSE;
int i_flg = FALSE;
int o_flg = FALSE;
int r_flg = FALSE;
int l_flg = FALSE;
int m_flg = FALSE;
int x_flg = FALSE;


/*************************************************************************/

int main(p_argc, p_argv)
int p_argc;
char **p_argv;
{
    char *arg, **argv;
    int argc, i;
    FILE *f;

#ifdef REARGV
    reargv (&p_argc, &p_argv);
#endif

    argc = p_argc;  argv = p_argv;
    if (argc < 2) use_err();
    while (--argc) {
        if (*(arg = *++argv) == '-') {
            switch (*++arg) {
                case 'i':
                case 'I':
                    i_flg = TRUE;
                    if (argc > 1 && **(argv + 1) != '-' && **(argv + 1)) {
                        if (strlen(i_path)) {
                            strcat(i_path,";");  strcat(i_path,*++argv);
                        }
                        else strcpy(i_path,*++argv);
                    }
                    else i_path[0] = '\0';
                    if (i_path[0] == '-') use_err();
                    break;
                case 'r':
                case 'R':
                    r_flg++;
                    break;
                case 'l':
                case 'L':
                    l_flg++;
                    break;
                case 'n':
                case 'N':
                    n_flg++;
                    break;
                case 'm':
                case 'M':
                    m_flg++;
                    break;
                case 'o':
                case 'O':
                    o_flg++;
                    if (!--argc) use_err();
                    strcpy(lst_fil,*++argv);
                    if (lst_fil[0] == '-') use_err();
                    break;
                case 'w':
                case 'W':
                    if (!--argc) use_err();
                    if ((i = atoi(*++argv)) <= MINCOL || i >= 255) use_err();
                    maxcol = i;
                    lin_refs = maxcol/7;
                    break;
                case 'x':
                case 'X':
                    x_flg++;
                    break;
                default:
                    use_err();
                    break;
            }
        }
    }
    if (o_flg) {
        if (!(f_lst_fil = fopen(lst_fil,"w"))) {
            printf("ERROR: Unable to create list file - %s\n",lst_fil);
            exit(0);
        }
        printf("XC ... 'C' Concordance Utility  v1.0\n\n");
    }
    for (linum = 0;  linum < MAX_WRD;  ++linum) id_vector[linum] = NULL;
    for (linum = 0; linum < MAX_ALPHA; ++linum)
        alpha_vector[linum].alpha_top = alpha_vector[linum].alpha_lst = NULL;
    linum = 0;
    while (--p_argc && **++p_argv != '-') {
        strcpy(gbl_fil,*p_argv);  strcpy(act_fil,*p_argv);
        if (!(f = fopen(gbl_fil,"r")))
            printf("\nERROR: Unable to open input file: %s\n",gbl_fil);
        else proc_file(f,DUMMY);
    }
    if (!l_flg) {
        prnt_tbl();
        printf("\nAllowable Symbols: %d\n",MAX_WRD);
        printf("   Unique Symbols: %d\n",id_cnt);
    }
    if (o_flg) {
        echo(FF);  fclose(f_lst_fil);
    }
    return 0;
}

/*************************************************************************/

void lst_err() {
    printf("\nERROR: Write error on list output file - %s\n", lst_fil);
    exit(0);
}

/*************************************************************************/

void use_err() {
    printf("\nERROR: Invalid parameter specification\n\n");
    printf("Usage: xc <filename>... <flag(s)>\n\n");
    printf("Flags: -i <opt. path(s)> = enable file inclusion\n");
    printf("       -l                = Generate listing only\n");
    printf("       -m                = Recognize Microsoft specific keywords\n");
    printf("       -n                = Allow nested comments\n");
    printf("       -o <outfile>      = Write output to named file\n");
    printf("       -r                = Cross-reference reserved words\n");
    printf("       -w width          = Width of output page; default=78\n");
    printf("       -x                = Exclude default include file dir's\n");
    printf("Flags must follow all input file names");
    exit(0);
}

/*************************************************************************/

int proc_file(infile,incnum)
FILE *infile;
int incnum;         /* prev. included line number (return to caller) */
{
    char token[MAX_LEN];    /* token buffer */
    char cur_fil[PATHLEN];  /* name of file we are currently processing */
    int eof_flg;            /* end-of-file indicator */
    int tok_len;            /* token length */
    FILE *f, *get_include_file();

    edtnum=0;
    if (!filevl++) prt_hdr();
    else nl();
    eof_flg = FALSE;
    do {
        if (get_token(infile,token,&tok_len,&eof_flg,0)) {
            if (chk_token(token)) {
                if((!strcmp(token,"#include")) && i_flg) {
                    strcpy(cur_fil,act_fil);
                    if (f = get_include_file(infile))
                        edtnum = proc_file(f,edtnum);
                    else if (!i_flg)
                        printf("\nERROR: Unable to open input file: %s\n",
                            act_fil);
                    strcpy(act_fil,cur_fil);
                }
                else put_token(token,linum);
            }
        }
    } while (!eof_flg);

    --filevl;  fclose(infile);  return incnum;
}

/*****************************************************************************
 *  Reads the include filename from the source file and saves it into a global
 *  variable, act_fil[].  It is assumed that the include statement gives the
 *  filename only, without any drive specifier or path, as in the previous
 *  version of XC.  Calls open_include_file() to flesh out the filename into a
 *  full pathname and actually get the file opened.
 */

FILE *get_include_file(infile)
FILE *infile;
{
    char c, term, *p;
    FILE *open_include_file();

    p = act_fil;

    while ((term = getc(infile)) == ' ') echo(term);
    echo(term);
    if (term == '<') term = '>';    /* terminator is > or " */
    if ((term != '>') && (term != '"')) {
        printf("Error scanning #INCLUDE fileid: %c\n",term);
        exit(1);
    }

    do {
        if ((c = getc(infile)) != ' ') *p++ = c;
        echo(c);
    } while (c != term);
    *--p = '\0';

    if (i_flg) return open_include_file(act_fil,term);
    else return NULL;
}

/*************************************************************************/

FILE *open_include_file(filenam,delim)
char *filenam, delim;
{
    char fullnam[PATHLEN];
    char includevar[4 * PATHLEN];
    char *temp1, *temp2;
    int include_len;
    FILE *f;

/*
 *  First, if delimited by quotes and file exists in current directory, return
 *  a file handle for the opened file.  Next, if a complete pathname
 *  specified, return a file handle for the opened file or NULL if the file
 *  cannot be found.
 */

    if (delim == '"' && (f = fopen(filenam,"r"))) return f;
    if (filenam[1] == ':' || filenam[0] == '\\' || filenam[0] == '/')
        return fopen(filenam,"r");

/*
 *  Next, if a non-null pathname was provided with the -i switch, check in
 *  that place.  Finally, check in the places specified by the INCLUDE
 *  environment variable unless the -x switch was used.
 */

    if (i_path[0] || (temp1 = getenv("INCLUDE"))) {
        strcpy(includevar,";");
        if (i_path[0]) { strcat(includevar,i_path);  strcat(includevar,";"); }
        if (!x_flg) strcat(includevar,temp1);
        include_len = strlen(temp2 = includevar);

/*
 *  Break up the semicolon-delimited list of directories into
 *  null-terminated strings and check each of them.
 */

        while (include_len > 0) {
            for (temp1 = temp2;  *temp2 != ';' && *temp2;  ++temp2)
                --include_len;
            if (*temp2 == ';') 
            {
               *temp2++ = '\0';
               --include_len;
            }
            strcpy(fullnam,temp1);  strcat(fullnam,"\\");
            strcat(fullnam,filenam);
            if (f = fopen(fullnam,"r")) return f;
        }
    }
    return NULL;
}

/*************************************************************************/

void echo(c)
char c;
{
    static int col = 11;
    int i;

    echochar(c);
    if (c == '\n') col = 11;
    else if (++col > maxcol) {
        col = 11;  ++paglin;
        echochar('\n');
        for (i = 1;  i <= 11;  ++i) echochar(' ');
    }
}
/************************************************************/
void echochar(c)
char c;
{
    if (o_flg) {
        if (fprintf(f_lst_fil,"%c",c) == ERROR) lst_err();
    }
    else printf("%c",c);
}

/*************************************************************************/

int get_token(infile,g_token,g_toklen,g_eoflg,g_flg)
FILE *infile;
char *g_token;
int *g_toklen, *g_eoflg, g_flg;
{

/*
        'getoken' returns the next valid identifier or
        reserved word from a given file along with the
        character length of the token and an end-of-file
        indicator
*/

    int c;
    char *h_token, tmpchr;

    h_token = g_token;

    gtk:
    *g_toklen = 0;  g_token = h_token;

/*
        Scan and discard any characters until an alphabetic or
        '_' (underscore) character is encountered or an end-of-file
        condition occurs
*/

    while ((!isalpha(*g_token = rdchr(infile,g_eoflg,g_flg)))
        && !*g_eoflg && *g_token != '_' && *g_token != '0'
        && *g_token != '#');

    if (*g_eoflg) return FALSE;
    *g_toklen += 1;

/*
        Scan and collect identified alpanumeric token until
        a non-alphanumeric character is encountered or and
        end-of-file condition occurs
*/

    if (g_flg) tmpchr = '.';
    else tmpchr = '_';
    while ((isalpha(c = rdchr(infile,g_eoflg,g_flg))
        || isdigit(c) || c == '_' || c == tmpchr) && !*g_eoflg) {
        if (*g_toklen < MAX_LEN) { *++g_token = c;  ++*g_toklen; }
    }

/*
        Check to see if a numeric hex or octal constant has
        been encountered ... if so dump it and try again
*/

    if (*h_token == '0') goto gtk;

/*
        Tack a NUL character onto the end of the token
*/

    *++g_token = '\0';

/*
        Screen out all #token strings except #include
*/

    if (*h_token == '#' && strcmp(h_token,"#include")) goto gtk;

    return(TRUE);
}

/*************************************************************************/

int fil_chr(infile,f_eof)
FILE *infile;
int *f_eof;
{
    int fc;

    if ((fc = getc(infile)) == EOF) {
        if (ferror (infile)) {
            printf("\nERROR: Error while processing input file - %s\n",
                act_fil);
            exit(0);
        }
        else if (feof(infile)) {
            *f_eof = TRUE;  fc = NULL;
        }
    }
    return fc;
}

/*************************************************************************/

int rdchr(infile,r_eoflg,rd_flg)
FILE *infile;
int *r_eoflg, rd_flg;
{

/*
        'rdchr' returns the next valid character in a file
        and an end-of-file indicator. A valid character is
        defined as any which does not appear in either a
        commented or a quoted string ... 'rdchr' will correctly
        handle comment tokens which appear within a quoted
        string
*/

    int c;
    int q_flg;          /* double quoted string flag */
    int q1_flg;         /* single quoted string flag */
    int cs_flg;         /* comment start flag */
    int ce_flg;         /* comment end flag */
    int c_cnt;          /* comment nesting level */
    int t_flg;          /* transparency flag */

    q_flg = q1_flg = cs_flg = ce_flg = t_flg = FALSE;
    c_cnt  = 0;

    rch:

/*
        Fetch character from file
*/

    c = fil_chr(infile,r_eoflg);
    if (*r_eoflg) return c;        /* EOF encountered */
    if (c == '\n') nl();
    else echo(c);

    if (rd_flg) return c;

    if (t_flg) { t_flg = !t_flg;  goto rch; }
    if (c == '\\') { t_flg = TRUE;  goto rch; }

/*
        If the character is not part of a quoted string
        check for and process commented strings...
        nested comments are handled correctly but unbalanced
        comments are not ... the assumption is made that
        the syntax of the program being xref'd is correct.
  NOTE: Now nested comment support is optional. The source for XC
        formerly contained an un-matched begin comment. If nested
        comment support is used to cross-reference the xc source
        the program produces an incorrect cross-reference, yet
        the Microsoft compiler compiles it correctly, as it
        should. Nested comments are a non-standard extension to
        the language. Therefore, nested comment support has been made
        optional.
        See usage information in comment at top of file.
*/

    if (!q_flg && !q1_flg) {
        if (c == '*' && c_cnt && !cs_flg) { ce_flg = TRUE;  goto rch; }
        if (c == '/' && ce_flg) {
            c_cnt -= 1;  ce_flg = FALSE;  goto rch;
        }
        ce_flg = FALSE;
        if (c == '/') { cs_flg = TRUE;  goto rch; }
        if (c == '*' && cs_flg) {
            c_cnt = (n_flg) ? TRUE : c_cnt + 1; /* optional nested comments */
            cs_flg = FALSE;  goto rch;
        }
        cs_flg = FALSE;
        if (c_cnt) goto rch;
    }

/*
        Check for and process quoted strings
*/

    if (c == '"' && !q1_flg) {
        q_flg =  !q_flg;        /* toggle quote flag */
        goto rch;
    }
    if (q_flg) goto rch;

    if (c == '\'') {
        q1_flg = !q1_flg;       /* toggle quote flag */
        goto rch;
    }
    if (q1_flg) goto rch;

/*
        Valid character ... return to caller
*/

    return c;
}

/*************************************************************************/

int chk_token(c_token)
char *c_token;
{
    char u_token[MAX_LEN];
    int i;

    if (r_flg) return TRUE;
    i = 0;
    do {
        u_token[i] = toupper(c_token[i]);
    } while (c_token[i++] != NULL);

/*
 *  Support for the Microsoft extended keywords.
 */

    if (m_flg) {
        switch (u_token[0]) {
            case 'C':
                if (!strcmp(u_token,"CDECL")) return FALSE;
                break;
            case 'F':
                if (!strcmp(u_token,"FORTRAN")) return FALSE;
                if (!strcmp(u_token,"FAR")) return FALSE;
                break;
            case 'H':
                if (!strcmp(u_token,"HUGE")) return FALSE;
                break;
            case 'N':
                if (!strcmp(u_token,"NEAR")) return FALSE;
                break;
            case 'P':
                if (!strcmp(u_token,"PASCAL")) return FALSE;
                break;
            case '_':
                if (!strcmp(u_token,"_CDECL")) return FALSE;
                if (!strcmp(u_token,"_FAR")) return FALSE;
                break;
        }
    }

/*
 *  Standard keyword support.
 */

    switch (u_token[0]) {
        case 'A':
            if (!strcmp(u_token,"AUTO")) return FALSE;
            break;
        case 'B':
            if (!strcmp(u_token,"BREAK")) return FALSE;
            break;
        case 'C':
            if (!strcmp(u_token,"CHAR")) return  FALSE;
            if (!strcmp(u_token,"CONTINUE")) return  FALSE;
            if (!strcmp(u_token,"CONST")) return  FALSE;
            if (!strcmp(u_token,"CASE")) return  FALSE;
            break;
        case 'D':
            if (!strcmp(u_token,"DOUBLE")) return FALSE;
            if (!strcmp(u_token,"DO")) return FALSE;
            if (!strcmp(u_token,"DEFAULT")) return FALSE;
            break;
        case 'E':
            if (!strcmp(u_token,"EXTERN")) return FALSE;
            if (!strcmp(u_token,"ELSE")) return FALSE;
            if (!strcmp(u_token,"ENUM")) return FALSE;
            break;
        case 'F':
            if (!strcmp(u_token,"FLOAT")) return FALSE;
            if (!strcmp(u_token,"FOR")) return FALSE;
            break;
        case 'G':
            if (!strcmp(u_token,"GOTO")) return FALSE;
            break;
        case 'I':
            if (!strcmp(u_token,"INT")) return FALSE;
            if (!strcmp(u_token,"IF")) return FALSE;
            break;
        case 'L':
            if (!strcmp(u_token,"LONG")) return FALSE;
            break;
        case 'R':
            if (!strcmp(u_token,"RETURN")) return FALSE;
            if (!strcmp(u_token,"REGISTER")) return FALSE;
            break;
        case 'S':
            if (!strcmp(u_token,"STRUCT")) return FALSE;
            if (!strcmp(u_token,"SHORT")) return FALSE;
            if (!strcmp(u_token,"STATIC")) return FALSE;
            if (!strcmp(u_token,"SIZEOF")) return FALSE;
            if (!strcmp(u_token,"SWITCH")) return FALSE;
            if (!strcmp(u_token,"SIGNED")) return FALSE;
            break;
        case 'T':
            if (!strcmp(u_token,"TYPEDEF")) return FALSE;
            break;
        case 'U':
            if (!strcmp(u_token,"UNION")) return FALSE;
            if (!strcmp(u_token,"UNSIGNED")) return FALSE;
            break;
        case 'V':
            if (!strcmp(u_token,"VOID")) return FALSE;
            if (!strcmp(u_token,"VOLATILE")) return FALSE;
            break;
        case 'W':
            if (!strcmp(u_token,"WHILE")) return FALSE;
            break;
    }
    return TRUE;
}

/*************************************************************************/

/*
   Install parsed token and line reference in linked structure
*/

void put_token(p_token,p_ref)
char *p_token;
int p_ref;
{
    int hsh_index, i, j, d, found;
    struct id_blk *idptr;
    struct id_blk *alloc_id();
    struct rf_blk *alloc_rf(), *add_rf();

    if (l_flg) return;
    j=0;
    for (i = 0;  p_token[i] != NULL;  ++i) {    /* Hashing algorithm is far */
        j = j * 10 + p_token[i];                /* from optimal but is      */
    }                                           /* adequate for a memory-   */
    hsh_index = abs(j) % MAX_WRD;               /* bound index vector!      */
    found = FALSE;  d = 1;
    do {
        idptr = id_vector[hsh_index];
        if (idptr == NULL) {
            id_cnt++;
            idptr = id_vector[hsh_index] = alloc_id(p_token);
            chain_alpha(idptr,p_token);
            idptr->top_lnk = idptr->lst_lnk = alloc_rf(p_ref);
            found = TRUE;
        }
        else if (!strcmp(p_token,idptr->id_name)) {
            idptr->lst_lnk = add_rf(idptr->lst_lnk,p_ref);
            found = TRUE;
        }
        else {
            hsh_index += d;  d += 2;  rhsh_cnt++;
            if (hsh_index >= MAX_WRD) hsh_index -= MAX_WRD;
            if (d == MAX_WRD) {
                printf("\nERROR: Symbol table overflow\n");
                exit(0);
            }
        }
    } while (!found);
}

/*************************************************************************/

void chain_alpha(ca_ptr,ca_token)
struct id_blk *ca_ptr;
char  *ca_token;
{
    char c;
    struct id_blk *cur_ptr;
    struct id_blk *lst_ptr;

    if ((c = ca_token[0]) == '_') c = 0;
    else if (isupper(c)) c = 1 + ((c - 'A') * 2);
    else c = 2 + ((c - 'a') * 2);

    if (!alpha_vector[c].alpha_top) {
        alpha_vector[c].alpha_top = alpha_vector[c].alpha_lst = ca_ptr;
        ca_ptr->alpha_lnk = NULL;  return;
    }

/*
    check to see if new id_blk should be inserted between
    the alpha_vector header block and the first id_blk in
    the current alpha chain
*/

    if (strcmp(alpha_vector[c].alpha_top->id_name,ca_token) > 0) {
        ca_ptr->alpha_lnk=alpha_vector[c].alpha_top;
        alpha_vector[c].alpha_top=ca_ptr;  return;
    }
    if (strcmp(alpha_vector[c].alpha_lst->id_name,ca_token) < 0) {
        alpha_vector[c].alpha_lst->alpha_lnk = ca_ptr;
        ca_ptr->alpha_lnk = NULL;  alpha_vector[c].alpha_lst=ca_ptr;  return;
    }
    cur_ptr = alpha_vector[c].alpha_top;
    while (strcmp(cur_ptr->id_name,ca_token) < 0) {
        lst_ptr = cur_ptr;  cur_ptr = lst_ptr->alpha_lnk;
    }
    lst_ptr->alpha_lnk = ca_ptr;  ca_ptr->alpha_lnk = cur_ptr;  return;
}

/*************************************************************************/

struct id_blk *alloc_id(aid_token)
char *aid_token;
{
    int ai;
    struct id_blk *aid_ptr;

    if (!(aid_ptr = (struct id_blk *)malloc(sizeof(struct id_blk)))) {
        printf("\nERROR: Unable to allocate identifier block\n");
        exit(0);
    }
    ai = 0;
    do
        aid_ptr->id_name[ai] = aid_token[ai];
    while (aid_token[ai++] != NULL);
    return aid_ptr;
}

/*************************************************************************/

struct rf_blk *alloc_rf(arf_ref)
int arf_ref;
{
    int ri;
    struct rf_blk *arf_ptr;

    if (!(arf_ptr = (struct rf_blk *)malloc(sizeof(struct rf_blk)))) {
        printf("\nERROR: Unable to allocate reference block\n");
        exit(0);
    }
    arf_ptr->ref_item[0] = arf_ref;  arf_ptr->ref_cnt.cnt = 1;
    for (ri = 1;  ri < MAX_REF;  ++ri) arf_ptr->ref_item[ri] = NULL;
    return arf_ptr;
}

/*************************************************************************/

struct rf_blk *add_rf(adr_ptr,adr_ref)
struct rf_blk *adr_ptr;
int adr_ref;
{
    struct rf_blk *tmp_ptr;

    tmp_ptr = adr_ptr;
    if (adr_ptr->ref_cnt.cnt == MAX_REF) {
        tmp_ptr = alloc_rf(adr_ref);
        adr_ptr->ref_cnt.pnext = tmp_ptr;
    }
    else adr_ptr->ref_item[adr_ptr->ref_cnt.cnt++] = adr_ref;
    return tmp_ptr;
}

/*************************************************************************/

void prnt_tbl()
{
    int prf_cnt, pti, pref, lin_cnt;
    struct id_blk *pid_ptr;
    struct rf_blk *ptb_ptr;

    prt_ref = TRUE;  prt_hdr();
    for (pti = 0;  pti < MAX_ALPHA;  ++pti) {
        if (pid_ptr = alpha_vector[pti].alpha_top) {
            do {
                if (o_flg) {
                    if (fprintf(f_lst_fil,"     %-20.19s: ",pid_ptr->id_name)
                        == ERROR) lst_err();
                }
                else printf("%-20.19s: ",pid_ptr->id_name);
                ptb_ptr = pid_ptr->top_lnk;  lin_cnt = -1;  prf_cnt = 0;
                do {
                    if (prf_cnt == MAX_REF) {
                        prf_cnt = 0;
                        if ((ptb_ptr->ref_cnt.cnt > MAX_REF) ||
                            (ptb_ptr->ref_cnt.cnt < 0) &&
                            (ptb_ptr->ref_cnt.cnt != NULL))
                           ptb_ptr = (struct rf_blk *)ptb_ptr->ref_cnt.pnext;
                        else
                           ptb_ptr = NULL;
                    }
                    if (ptb_ptr) {
                        if (pref = ptb_ptr->ref_item[prf_cnt++]) {
                            if (++lin_cnt == lin_refs) {   /* was REFS_PER_LINE dnh 7/21/89 */
                                nl();
                                if (o_flg) {
                                    if (fprintf(f_lst_fil,
                                        "                           ") == ERROR)
                                        lst_err();
                                }
                                else printf("                      ");
                                lin_cnt=0;
                            }
                            if (o_flg) {
                                if (fprintf(f_lst_fil,"%4d ",pref) == ERROR)
                                    lst_err();
                            }
                            else printf("%4d ",pref);
                        }
                    }
                    else pref=0;
                } while (pref);
                nl();
            } while ((pid_ptr=pid_ptr->alpha_lnk) != NULL);
        }
    }
/*for*/

    echo( '\n' );
}

/*************************************************************************/

void prt_hdr()
{
    if (pagno++) { echo('\n'); echo(FF); }
    if (o_flg) {
        if (fprintf(f_lst_fil,
            "XC ... 'C' Concordance Utility   %-20s       Page %d",
            gbl_fil,pagno) == ERROR) lst_err();
        }
    else printf("XC ... 'C' Concordance Utility   %-20s       Page %d",
        gbl_fil,pagno);
    echo('\n');  paglin = 3;  nl();
}

/*************************************************************************/

void nl()
{
    echo('\n');
    if (++paglin >= LINES_PER_PAGE) prt_hdr();
    else if (!prt_ref) {
        if (o_flg) {
            if (fprintf(f_lst_fil,"     ") == ERROR)
                lst_err();
            ++linum;
            ++edtnum;
        }
        else printf("%-4d %4d: ",++linum,++edtnum);
        if (o_flg) {
            if (linum % 60 == 1) printf("\n<%d> ",linum);
            else {
                printf(".");
                fflush(stdout);
            }
        }
    }
    return;
}


/***************************************/

