

/*
TITLE:		Karel The Robot;
VERSION:	1.0;
DATE:		09/27/1984;
DESCRIPTION:
	"Header for syntax-directed editor, KED.";
KEYWORDS:	Globals, constants;
SYSTEM:		CP/M 2+;
FILENAME:	KED.H;
AUTHORS:	Linda Rising;
COMPILERS:	BDS C;
REFERENCES:
	AUTHOR: Richard E. Pattis;
	TITLE: "KAREL the Robot: A Gentle Introduction
		to the Art of Programming";
	CITATION: "";
ENDREF
*/


/* global constants */

#define MAXCOMM   30          /* number of valid edit commands */
#define MAXTEST   19          /* number of booleans for Karel */
#define MAXNAME   12          /* number of reserved commands */
#define NOMATCH   -1          /* search for valid command fails */
#define CONSOLE    1          /* console output */
#define FILE  struct _buf     /* label for file header */
#define TEST       2          /* frequently referenced commands */
#define POSINT     3     
#define NEWINSTR   4
#define BEG	   5
#define BEPGM	   6
#define BX         7
#define ND         8
#define NDX        9
#define NDC	  10
#define IFF	  11
#define THEN      12
#define ELS       13
#define DEF       16
#define MENU      18

/* global constants included in the standard library

BUFSIZ
CPMEOF
EOF
ERROR
FILE
MAXLINE
NULL  
 */

/* global variables */

char ibuf1[BUFSIZ];           /* input buffer for parse tree */
char ibuf2[BUFSIZ];           /* output buffer for text */
char *comm[MAXCOMM];          /* valid editor commands */
char *test[MAXTEST];          /* booleans for Karel's pgm */
char *iname[MAXNAME];         /* commands for Karel's pgm */
int insert;                   /* boolean for inserting new node */
int ins;		      /* final indent level */
struct tnode {                /* node for parse tree */
     int instr;               /* stored instruction */
     int indent;              /* used for pretty printing */
     int del;                 /* can node be deleted */
     int nfollow;             /* valid next follow statements */
     int sfollow;             /* valid component followers */
     int comp;                /* possible compound statement */
     int cons;                /* start or end of construct */
     int lastin;              /* last defined instruction */
     struct tnode *prev;      /* pointer to preceding node */
     struct tnode *prec;      /* pointer to preceding construct */
     struct tnode *next;      /* pointer to next construct */
     struct tnode *sub;       /* pointer to component instruction */
};
struct tnode *root;           /* points to parse tree */
struct tnode *p;              /* next node to be added */
struct tnode *pr;             /* parent of next node */
struct tnode *curr;           /* current line pointer reference */
struct tnode *temp;           /* reference for end construct */ 
struct tnode *endof;          /* last node added to tree */
struct tnode *insertnode;     /* place holder for insert mode */ 
struct tnode *tempend;        /* last node inserted in tree */ 
struct deftable {             /* symbol table for new instructions */
     char defname[MAXLINE];   /* name of new instruction */
     int nextin;              /* order of definition */
     struct tnode *loc[10];   /* pointers to instr in pgm */
} new[10];                    /* symbol table */
int first;                    /* first loc in symbol table */
int last;                     /* next available loc in table */
int reloc[10];                /* old table loc to new table loc */
