/***************************************************************
 *                    Setup HP Laser Jet II                    *
 *                         Robert Ramey                        *
 *                         19 May 1987                         *
 *                                                             *
 *This program sets up Hewlett Packard laser printer.  It also *
 *demonstrates how to use a menu driven code system.           *
 ***************************************************************/

#include "stdio.h"
#include "menu.h"

/* indexes into tables for various variables */
#define PORTRAIT 0
#define LANDSCAPE 1
#define SIXLPI  0
#define EIGHTLPI 1
#define TWELVELPI 2
#define TENCPI  0
#define SIXTEENCPI 1
#define EXECUTIVE 0
#define LETTER 1
#define LEGAL 2
#define A4  3
#define MONARCH 4
#define BUSINESS 5
#define DL 6
#define C5 7
#define COURIER 0
#define LINEPRINTER 1

int l,w;    /* length and width in spaces */
int tdots,bdots,ldots,rdots ;/* margins in dots */
int p[11];   /* array of currently selected options */

/* names for each option selection */
#define orientation p[0]    /* Portrait(0) or Landscape(1) */
#define pagesize p[1]   /* Letter size is default */
#define font p[2]   /* default font is courier */
#define bold p[3]   /* default is not bold */
#define lpi p[4]    /* six lines per inch */
#define cpi p[5]    /* Ten characters per inch */
#define ncopies p[6]    /* number of copies */
#define top p[7]    /* number of lines */
#define bottom p[8]
#define left p[9]
#define right p[10]

/* Length and width of various page sizes in dots */
/* Taken from fig 2-2 and 2-3 of Laser Jet II Tech Ref */
int length[] = {3150,3300,4200,3507,2850,2250,2704,2598};
int width[] = {2175,2550,2550,2480,1237,1162,1913,1299};
int lmargin[] = {50,50,50,50,50,50,50,50};
int rmargin[] = {100,100,100,92,100,100,92,92};
int tmargin[] = {60,60,60,60,60,60,60,60};
int bmargin[] = {60,60,60,58,60,60,58,58};
int charwidth[] = {30,18};    /*char width in dots */
int linesperinch[] = {6,8,12};   /*characters per inch */

/* Codes for control of printer */
char *ocode[] = {"0o","1o"}; /* orientation codes */
char *pscode[] = {"2h1a","1h2a","2h3a","2h26a",
        "3h80a","3h81a","3h90a","3h91a"};
char *lpicode[] = {"6d","8d","0d"};
char *fcode[] = {"\33(s3t","\33(s0t"};
char *cpicode[] = {"10h","16.6h"};
char *bcode[] = {"0B","3B"};

main(){
    /* specify default selections */

    /* since Q/C does't permit initialization of unions */
    /* do the job here */
    orientation = PORTRAIT;
    pagesize = LETTER;
    font = COURIER;
    bold = FALSE;
    lpi = SIXLPI;
    cpi = TENCPI;
    ncopies = 1;
    top = bottom = -1;
    left = right = 0;
    menu(&m);
    exit(0);
}
/*****************************************************************
action - this function recieves control when there is no menu
or other action routine specified.  It recieves a pointer to a
string containing the sequence of responses so far which brought
us to this point in dialog.
******************************************************************/
action(responses)
char *responses;
{
    /* In this example,  the first character of the argument */
    /* should contain a '2' */
    /* The second character of the argument contains the option */
    /* to be specified. */
    /* The third character of the argument contains the */
    /* selection made. */
    /* Update to chosen option with the given selection */
    p[response[1] - '1'] = response[2] - '1';
    return 1;
}
/*************************************************************
display - displays the current selection for each printer option
This is unusual in that the text for the display is taken from
the menus themselves.  This will not always be possible but is
very convenient in cases like this one.
**************************************************************/
display(){
    int i, charheigth, lineslost;
    MENU *home;
    MLINE *q, *a;

    /* if invalid combination of selections chosen simply return */
    if(chksel() == 0) return 0;

    /* get address of second menu */
    home = (MENU *)(m.selection[1].parameter);
    fprintf(stderr, "\n");

    /* for each line in the second menu */
    for(i = 0; i < 3;++i){
        /* get text of line describing option */
        q = home->selection + i;
        /* get text of line describing selection */
        a = ((MENU *)(q->parameter))->selection + p[i];
        /* display "<option> is <selection>" */
        /* not including numbers on menus */
        fprintf(stderr,"%s is %s\n",q->answer+3,a->answer+3);
    }

    /* display other information not amenable to the above */
    /* approach */
    fprintf(stderr,"Number of copies is %d\n", ncopies);
    fprintf(stderr,"Top margin is %d lines\n", top);
    fprintf(stderr,"Text space is %d lines long ",l-top-bottom);
    fprintf(stderr,"at %d lines per inch\n",linesperinch[lpi]);
    fprintf(stderr,"Bottom margin is %d lines\n", bottom);
    charheight = 300 / linesperinch[lpi];
    lineslost = (tdots - 1 + charheight) / charheight;
    if(lineslost > top)
        fprint(stderr,
        "    NOTE: %d lines will be lost at top margin\n",
        lineslost - top);
    lineslost = (bdots - 1 + charheight) / charheight;
    if(lineslost > bottom)
        fprintf(stderr,
        "    NOTE: %d lines will be lost at bottom margin\n",
        lineslost - bottom);
    fprintf(stderr,
    "Pages are %d characters wide at %d characters per inch\n"
    ,w - right - left, 300/charwidth[cpi]);
    if(left)
        fprintf(stderr,"    with a left margin of %d\n", left);
    fprintf(stderr,"\n");
    return 0;
}
/****************************************************************
copies - Ask the operator how many copies he wants and store the
answer.
*****************************************************************/
copies(){
    ncopies = getn("\nHow many copies do you want?:");
    return 0;
}
/***************************************************************
margin - set margins
****************************************************************/
margin(i)
int i;
{
    static char *prompt[] = {
        "How many rows do you want for the top margin:",
        "How many rows for the bottom margin:",
        "How many columns for the left margin:",
        "How many columns for the right margin:"};
    p[i+7] = getn(prompt[i]);
    return 0;
}
/***************************************************************
getn - Get a number from the operator.
****************************************************************/
getn(prompt)
char *prompt;
{
    char s[10];
    fprintf(stderr, prompt);
    return atoi(gets(s));
}
/**************************************************************
finish - now that the operator has finally decide on which
selections he wants for each selection, write the appropriate
control codes to the output file
***************************************************************/
finish(){
    FILE *fp;

    if(!chksel()) return 0; /* check selections */

    /* emit printer control codes */
    printf("\33E\33&l");    /* reset printer */
    printf("%dx", ncopies);
    printf(pscode[pagesize]);
    printf(ocode[orientation]);
    printf(lpicode[lpi]);
    printf("%de",top);
    printf("%df1L",l - top - bottom);
    printf("\33&a%dla%dM",left,w - right);
    printf(fcode[font]);
    printf(cpicode[cpi]);
    printf(bcode[bold]);

    /* create format skeleton file */
    /* to be included in format documents */
    fp = fopen("slptr.fmt", "w");
    fprintf(fp, ".pl %d\n", l - top - bottom);
    fprintf(fp, ".m1 0\n.m2 0\n");
    fprintf(fp, ".rm %d\n", w - 8);
    fprintf(fp, ".in 8\n");
    fclose(fp);
    return 99;
}
/****************************************************************
chksel - check to see if any invalid combination of choices has
be made.
*****************************************************************/
chksel(){
    int i;

    switch(font){
    case 0: break;
    case 1:
        bold = TRUE;
        break;
    case 2:
        font = LINEPRINTER;
        cpi = SIXTEENCPI;
        lpi = EIGHTLPI;
        break;
    }
    l = length[pagesize];
    w = width[pagesize];
    tdots = tmargin[pagesize];
    bdots = bmargin[pagesize];
    ldots = lmargin[pagesize];
    rdots = rmargin[pagesize];

    if(pagesize > A4)
        orientation = LANDSCAPE; /* envelopes are landscape */
    if(orientation == LANDSCAPE){
        i = l;l = w;w = i;    /* swap length and width */
        i = tdots;tdots = ldots;ldots = i; /*top and left margin */
        i = bdots;bdots = rdots;rdots = i; /*bottom and right margin */
    }
    l = l * linesperinch[lpi] / 300;    /* number of lines */
    w = (w - rdots - ldots) / charwidth[cpi];   /* number of columns */
    if(bottom == -1)   /* default half inch for top and bottom */
        bottom = linesperinch[lpi] / 2;
    if(top == -1)
        top = linesperinch[lpi] / 2;

    return 1;
}
w;w = i;    /* swa