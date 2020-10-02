   /*  HEADER:CUG 226;
       TITLE:"ART-CEE";
       VERSION:1.4;
       DESCRIPTION:"Small artificial intelligence inference engine.

            ART-CEE is a rule-based general purpose driver capable of working
            with heirarchical and/or propositional information.";
       KEYWORDS:artificial intelligence, inference engines, database;
       SYSTEM:MS-DOS;
       FILENAME:WORKUP1.H;
       SEE-ALSO:WORKUP0.C, WORKUP2.C, WORKUP3.C, WORKUP4.C, MANUAL.TXT,
            WORKUP0.H, HELP1.AIH, HELP2.AIH, HELP3.AIH, HELP4.AIH, ART-CEE.COM;
       AUTHOR:Dan Cheatham;
       COMPILER:MIX-C;
   */

 /*  This file defines all global variables for all but the first source */
   /*  file (WORKUP4.C).                                                   */
#define     MAXFILES    20
#define     BUFSIZ      512
#define     EOF         -1
#define     NULL        0
#define     stdin       _iob[0]
#define     stdout      _iob[1]
#define     stderr      _iob[2]
#define     getchar()   getc(stdin)
#define     putchar(c)  putc(c,stdout)

typedef struct {
    char    file[32];
    int     fd;
} FILE;

extern      FILE        *_iob[MAXFILES];

typedef struct {
    int     length;
    char    string[80];
} STRING;

/***************************************************************/
    extern    float    dfwd, drev, ff1, ft[MAX][MAX];
    extern    char     indff[80], subjs[MAX][77],   filebuf [81], currfile[32];
    extern    char       menu_sw, oride_sw;
    extern    int      x1, x2, filestat, srchdpth, assumpno;

