/* global constants */

#define MAXCOMM   30          /* number of valid edit commands */
#define MAXTEST   19          /* number of booleans for Karel */
#define MAXNAME   12          /* number of reserved commands */
#define MAXLINE   150         /* number of lines */
#define NOMATCH   -1          /* search for valid command fails */
#define TEST       2          /* frequently referenced commands */
#define POSINT     3     
#define NEWINSTR   4
#define BEG        5
#define BEPGM      6
#define BX         7
#define ND         8
#define NDX        9
#define NDC       10
#define IFF       11
#define THEN      12
#define ELS       13
#define DEF       16
#define MENU      18
#define ERROR     -1
#define CPMEOF   0X1A

/* global constants included in the standard library

BUFSIZ
EOF
FILE
NULL  

*/

/* global variables */

FILE *fptr1;                  /* input file pointer for sparse tree */
FILE *fptr2;                  /* output file ptp for text */
char *comm[MAXCOMM];          /* valid editor commands */
char *test[MAXTEST];          /* booleans for Karel's pgm */
char *iname[MAXNAME];         /* commands for Karel's pgm */
int insert;                   /* boolean for inserting new node */
int ins;                      /* final indent level */
struct tnode {                /* node for parse tree */
     int instr;               /* stored instruction */
     int indent;              /* used for pretty printing */
     int del;                 /* can node be deleted */
     int nfollow;             /* valid next follow statements */
     int sfollow;             /* valid component followers */
     int comp;
     int cons;
     int lastin;
     struct tnode *prev;
     struct tnode *prec;
     struct tnode *next;
     struct tnode *sub;
};
struct tnode *root;
struct tnode *p;
struct tnode *pr;
struct tnode *curr;
struct tnode *temp;
struct tnode *endof;
struct tnode *insertnode;
struct tnode *tempend;
struct deftable {
     char defname[MAXLINE];
     int nextin;
     struct tnode *loc[15];
} new[15];
int first;
int last;
int reloc[15];
