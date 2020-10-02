/**

	APPLY.H
	
	Header file for APPLY.C
	
**/

/* Constant Values */
#ifndef TRUE
#define TRUE		1
#endif
#ifndef FALSE
#define FALSE		0
#endif

#define MAXLINE		256

/* Data Types */

typedef int BOOL ;

typedef struct _eline
{
	int linenum ;		/* line number in file */
	char *text ;		/* text for the line */
	struct _eline *prev;	/* previous line of text */
	struct _eline *next;	/* next line of text */
}
ELINE ;
	

/**

	Function Declarations

**/

static ELINE *AddNode(ELINE *,char *,int);
static ELINE *DeleteNode(ELINE *);
static int ReadFile(FILE *,ELINE **,ELINE **);
static void DoDelete(int ,int ,ELINE **,ELINE **);
static void DoAdd(int ,ELINE **,ELINE **,ELINE **);
static void DoChange(int ,int ,ELINE **,ELINE **,ELINE **);
static void ParseDelta(ELINE **,ELINE **,ELINE **);
static void banner(void);
void main(int,char **) ;

