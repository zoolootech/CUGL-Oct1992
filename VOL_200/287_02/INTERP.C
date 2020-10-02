#include <stdio.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdarg.h>

/*==============================================================*
 *                                                              *
 *  This program is used to interpret GRAD functions in         *
 *      restricted C syntax of function call.                   *
 *  The method used is recursive descend.                       *
 *                                                              *
 *==============================================================*/

#define unreadc(ch) ungetc(ch,infile)

/* maximum number of parameter for a single function */
#define MAXPARM 9

#define Boolean int
#define TRUE 1
#define FALSE 0
#define ERROR (-1)

enum Ptype {
    UNDEFINED, INTG, STRG
};

struct funcdesc {
    char *name;
    enum Ptype rettype;
    int nuparm;
    enum Ptype parmstype[MAXPARM];
};

/* The function names must arrange in ascending order */
struct funcdesc FUNCTIONS[] =
    { { "Arc1",     UNDEFINED, 4, { INTG, INTG, INTG, INTG } } ,
      { "Arc2",     UNDEFINED, 5, { INTG, INTG, INTG, INTG, INTG } },
      { "ArcPoint", UNDEFINED, 4, { INTG, INTG, INTG, INTG } },
      { "BlockCopy",  INTG, 8,
                    { INTG, INTG, INTG, INTG, INTG, INTG, INTG, INTG } },
      { "BlockLoad",  INTG, 3, { INTG, INTG, STRG } },
      { "BlockSave",  UNDEFINED, 5, { INTG, INTG, STRG, INTG, INTG } },
      { "Box",      UNDEFINED, 6, { INTG, INTG, INTG, INTG, INTG, INTG } } ,
      { "Circle",   UNDEFINED, 3, { INTG, INTG, INTG } } ,
      { "CreateFrame", INTG, 2, { INTG, INTG } } ,
      { "Dot",      UNDEFINED, 2, { INTG, INTG } } ,
      { "Draw",     UNDEFINED, 3, { STRG, INTG, INTG } },
      { "Earc1",    UNDEFINED, 5, { INTG, INTG, INTG, INTG, INTG } } ,
      { "Earc2",    UNDEFINED, 6, { INTG, INTG, INTG, INTG, INTG, INTG } },
      { "Ellipse",  UNDEFINED, 4, { INTG, INTG, INTG, INTG } } ,
      { "EnvRsto",  UNDEFINED, 2, { INTG, INTG } },
      { "EnvSave",  INTG, 1, { INTG } },
      { "FillCircle", UNDEFINED, 3, { INTG, INTG, INTG } },
      { "FillEllipse",UNDEFINED, 4, { INTG, INTG, INTG, INTG } },
      { "HorzLine", UNDEFINED, 4, { INTG, INTG, INTG, INTG } } ,
      { "Line",     UNDEFINED, 4, { INTG, INTG, INTG, INTG } } ,
      { "LoadFont", INTG, 1, { STRG } } ,
      { "NextXY",   UNDEFINED, 2, { INTG, INTG } },
      { "PatternFill", UNDEFINED, 4, { INTG, INTG, STRG, INTG } },
      { "PlotType", INTG, 1, { INTG } } ,
      { "PrintFrame", UNDEFINED, 5, { INTG, STRG, INTG, INTG, INTG } } ,
      { "PrintPage",  UNDEFINED, 0 },
      { "ReadStr",    UNDEFINED, 7,
                          { STRG, INTG, INTG, INTG, INTG, INTG, INTG } },
      { "Rectangle",  UNDEFINED, 4, { INTG, INTG, INTG, INTG } } ,
      { "RelOrg",   UNDEFINED, 2, { INTG, INTG } } ,
      { "RemvFont", INTG, 1, { INTG } } ,
      { "RemvFrame", INTG, 1, { INTG } } ,
      { "ResetWin", UNDEFINED, 0 } ,
      { "SelectFont",  INTG, 1, { INTG } },
      { "SelectFrame", INTG, 1, { INTG } } ,
      { "SetOrg",   UNDEFINED, 2, { INTG, INTG } } ,
      { "SetStyle", UNDEFINED, 1, { INTG } } ,
      { "SetWin",   UNDEFINED, 4, { INTG, INTG, INTG, INTG } } ,
      { "SolidFill",   UNDEFINED, 2, { INTG, INTG } },
      { "VertLine", UNDEFINED, 4, { INTG, INTG, INTG, INTG } },
      { "WriteStr", UNDEFINED, 7, { INTG, INTG, INTG, INTG, STRG, INTG, INTG }},
      { "XHLine",   INTG, 5, { INTG, INTG, INTG, INTG, INTG } }
 };

#define NFUNC (sizeof(FUNCTIONS) / sizeof(struct funcdesc))

char *ERRMSG[]= {
    "Undefined Error Number\n",
    "Variable/Function name expected\n",
    "Variable name %s not found\n",
    "Function name or expression expected\n",
    "Function name %s not found\n",
    "'(' expected after function name\n",
    "Type if parameter %d is different from definition\n",
    "')' expected after the parameters of a function\n",
    "Less parameter than expected\n",
    "End of string not detected before end of line\n",
    "',' expected after a parameter\n",
    "'C' or 'L' expected after '['\n",
    "']' expected after a line or column specification\n",
    "Identifier Expected\n"
};

#define NUOFERROR (sizeof(ERRMSG) / sizeof(char *))

char LINE[160];

int PATTERN[]={
    0x5555, 0xaaaa, 0x5555, 0xaaaa,
    0x5555, 0xaaaa, 0x5555, 0xaaaa,
    0x5555, 0xaaaa, 0x5555, 0xaaaa,
    0x5555, 0xaaaa, 0x5555, 0xaaaa,
    0x8888, 0x4444, 0x2222, 0x1111,
    0x8888, 0x4444, 0x2222, 0x1111,
    0x8888, 0x4444, 0x2222, 0x1111,
    0x8888, 0x4444, 0x2222, 0x1111,
    0x1111, 0x2222, 0x4444, 0x8888,
    0x1111, 0x2222, 0x4444, 0x8888,
    0x1111, 0x2222, 0x4444, 0x8888,
    0x1111, 0x2222, 0x4444, 0x8888
};

struct pval {
    enum Ptype parmtype;
    union {
        int u_int;
        char *u_strg;
    } v;
};

struct pval RETVAL, *expr();

char IDNAME[12], CHARS[512], *PSPTR;

#define NUOFVAR 21

int nuofvar=NUOFVAR;
char VARNAME[NUOFVAR][12]={
    "pattern1", "pattern2", "pattern3",
    "line",
    "font1", "font2", "font3", "font4",
    "frame1", "frame2", "frame3",
    "temp1", "temp2", "temp3",
    "x1", "x2", "y1", "y2",
    "env1", "env2", "env3"
};

struct pval VARTABLE[NUOFVAR];

int COL_OFFSET=0, ROW_OFFSET=0, LINE_HEIGHT=12, CHAR_WIDTH=12;

jmp_buf EOF_JMP, SYNERR_JMP;

int EOF_FLAG=0;

FILE *infile;

main(argc,argv)
int argc;
char *argv[];
{
    int ret;
    FILE *fopen();

    if (argc <= 1) {
        fprintf(stderr,"Usage: interp commandfile\n");
        exit(1);
    }
    if ((infile=fopen(*(++argv), "r")) == (FILE *) NULL) {
        fprintf(stderr,"interp: File not found\n");
        exit(1);
    }
    GRADinit();         /* GRAD initialization function */
    initvars();         /* initialize internal variables */
    setgraph();         /* set graphics mode */

    if (setjmp(EOF_JMP) != 0) {
        /* return here on End_Of_File */
        fclose(infile);
        if (EOF_FLAG) { 
            printf("interp: Unexpected End Of File !\n");
        }
        getch();
        settext();
        cleanup(EOF_FLAG);
        exit(0);
    }
    setjmp(SYNERR_JMP);
    /* return here on syntax error */
    for (;;) {
        EOF_FLAG=0;
        stmt();
    }
}

/*------------------------------------------------------*
 *    read a single character from input file.          *
 *    A string of comment is treated as a single space  *
 *    Comments can be nested.                           *
 *------------------------------------------------------*/
readc0()
{
    int ch;

    if ((ch=getc(infile)) == EOF) {
        longjmp(EOF_JMP,1);
    }
    if (ch != '/') return(ch);
    if ((ch=getc(infile)) == EOF) {
        longjmp(EOF_JMP,1);
    }
    if (ch != '*') {
        ungetc(ch,infile);
        return('/');
    }
    /* begining of comment */
    for (;;) {
        if ((ch=readc0()) == '*') {
            while((ch=readc0()) == '*') ;
            if (ch=='/') return(' ');
        }
    }
}

/* skip all control characters except tab (0x09) and return (0x0d) */
/* but they will be returned as space (0x20)                       */
readc1()
{
    int ch;

    do {
        ch=readc0();
        if (isspace(ch)) return(' ');
    } while (ch < 0x20);
    return(ch);
}

/* read first non-blank character from input file */
readc2()
{
    int ch;

    do {
        ch=readc1();
    } while (ch==' ');
    EOF_FLAG=1;
    return(ch);
}

/* procdure to interpret a statement */
/* { } means optional, | means or

   STMT := [ VARIABLE = ] FUNCTION
   VARIABLE := identifier
   FUNCTION := identifier ( PARAMETERS ) ;
   PARAMETERS := PARAMETER , PARAMETERS | PARAMETER
   PARAMETER := EXPRESSION
*/
stmt()
{
    int ch, varidx,funcidx, nuparmoffunc, nparm;
    struct pval *valptr, parmlist[MAXPARM];

    PSPTR=CHARS;
    varidx=-1;    /* init */
    if (!identifier()) {   /* is first token an identifier */
        synerr(1);         /* syntax error 1 */
    }
    ch=readc2();
    if (ch=='=') {
        if ((varidx=findvar(IDNAME))==ERROR) {
            synerr(2,IDNAME);
        }
        if (!identifier()) {
            valptr=expr();
            if (valptr->parmtype == UNDEFINED) {
                synerr(3);
            } else {
                VARTABLE[varidx]=*valptr;
                goto endofstmt;
            }
        }
        ch=readc2();
    }
    if ((funcidx=findfunc(IDNAME))==ERROR) {
        synerr(4,IDNAME);
    }
    if (ch!='(') {
        synerr(5);
    }
    if ((nuparmoffunc=FUNCTIONS[funcidx].nuparm) == 0) {
        ch=readc2();
    } else {
        ch=',';
    }
    for (nparm=0; nparm<nuparmoffunc; nparm++) {
        if (ch != ',') {
            synerr(10);
        }
        valptr=expr();
        if (valptr->parmtype == UNDEFINED) {
            synerr(8);
        }
        if (valptr->parmtype != FUNCTIONS[funcidx].parmstype[nparm]) {
            synerr(6,nparm+1);
        }
        parmlist[nparm]=*valptr;
        ch=readc2();
    }
    if (ch != ')') {
        synerr(7);
    }
    /* execute the functions by pushing the parameters onto stack */
    /* and then call indirectly using funcidx                     */
    execfunc(funcidx, FUNCTIONS[funcidx].nuparm, parmlist,
             FUNCTIONS[funcidx].rettype, &RETVAL);
    if (varidx>=0) {
        VARTABLE[varidx]=RETVAL; /* assign the retugn value to the
                                    variable */
    }
endofstmt:
    ch=readc2();
    if (ch!=';') {
        printf("Warning: Semicolon ';' expected. Assuming present\n");
        unreadc(ch);
    }
}

/* try read an identifier and put the name in IDNAME */
/* if identifier not found, return FALSE else return TRUE */
Boolean identifier()
{
    int ch, count;

    if (!isalpha(ch=readc2())) {
        IDNAME[0]='\0';
        unreadc(ch);
        return(FALSE);
    }
    count=0;
    do {
        IDNAME[count++]=ch;
        ch=readc1();
    } while (isalnum(ch));
    unreadc(ch);
    IDNAME[count]='\0';
    return(TRUE);
}

/* find the variable in the variable table and return the index if found */
/* else return ERROR */
findvar(name)
char *name;
{
    int loop;

    for (loop=0; loop<nuofvar; loop++) {
        if (strcmp(name,VARNAME[loop])==0) return(loop);
    }
    return(ERROR);
}

/* find the function in the function table, return index if found */
/* else return ERROR */
findfunc(name)
char *name;
{
    int start, end, current, ret;

    for (start=0, end=NFUNC; start != end ; ) {
        current=(start+end)>>1;
        if ((ret=strcmp(name,FUNCTIONS[current].name)) == 0) {
            return(current);
        } else if (ret > 0) {
            start=current+1;
        } else {
            end=current;
        }
    }
    return(ERROR);
}


/* { } means optional, | means or

   EXPRESSION := NUMBER | STRING | VARIABLE | & VARIABLE | COLUMN | LINE
   NUMBER := +NATURAL | -NATURAL
   NATURAL := 0xHHHH | decimal digits
   STRINGS := "character string"
   H := decimal digits | A | B | C | D | E | F | a | b | c | d | e | f
   COLUMN := [ C NATURAL { , NATURAL } ]
   LINE := [ L NATURAL { , NATURAL } ]
*/
struct pval *expr()
{
    int ch, number, sign, varidx, addrflag;

    RETVAL.parmtype=UNDEFINED;
    number=sign=addrflag=0;
    ch=readc2();
    if (isdigit(ch)) {
getnum:
        if (ch=='0') {
            ch=readc1();
            if (toupper(ch)=='X') {
                ch=readc1();
                while(isxdigit(ch)) {
                    number=(number<<4) | ((ch-(isalpha(ch) ? 7:0)) & 0x0f);
                    ch=readc1();
                }
            } else {
                number=getint(&ch);
            }
        } else {
            number=getint(&ch);
        }
        unreadc(ch);
        RETVAL.parmtype=INTG;
        RETVAL.v.u_int=sign ? -number:number;
    } else if (ch=='+') {
        ch=readc1();
        goto getnum;
    } else if (ch=='-') {
        sign=1;
        ch=readc1();
        goto getnum;
    } else if (ch=='"') {
        RETVAL.parmtype=STRG;
        RETVAL.v.u_strg=PSPTR;
        for (;;) {
        if ((ch=getc(infile)) == EOF) {
            longjmp(EOF_JMP,1);
        }
            if (ch=='\n') {
                synerr(9);
            }
            if (ch=='"') break;
            *PSPTR++ = ch; /* put the string into string pool */
        }
        *PSPTR++ = '\0';
    } else if (ch=='&') {
        addrflag=1;
        goto ident;
    } else if (ch=='[') {
        ch=readc2();
        if (ch=='L' || ch=='l') {
            ch=readc2();
            number=getint(&ch);
            if (number) number--;
            number=number*LINE_HEIGHT+ROW_OFFSET;
        } else if (ch=='C' || ch=='c') {
            ch=readc2();
            number=getint(&ch);
            if (number) number--;
            number=number*CHAR_WIDTH+COL_OFFSET;
        } else {
            synerr(11);
        }
        if (ch==',') {
            ch=readc2();
            number+=getint(&ch);
        }
        if (ch!=']') {
            synerr(12);
        }
        RETVAL.parmtype=INTG;
        RETVAL.v.u_int=number;  
    } else if (isalpha(ch)) {
        unreadc(ch);
ident:
        if (!identifier()) synerr(13);
        if ((varidx=findvar(IDNAME)) == ERROR) {
            synerr(2,IDNAME);
        }
        RETVAL=VARTABLE[varidx];
        if (addrflag) {
            RETVAL.parmtype=VARTABLE[varidx].parmtype=INTG;
            RETVAL.v.u_int=(int) &VARTABLE[varidx].v.u_int;
        }
    } else if ((ch==',') || (ch==')')) {
        /* empty expression, assume equal to 0 */
        unreadc(ch);
        RETVAL.parmtype=INTG;
        RETVAL.v.u_int=0;
    }
    return(&RETVAL);
}

/* syntax error handling */
synerr(errno)
int errno;
{
    va_list arg_ptr;
    int ch;

    va_start(arg_ptr, errno);
    if ((errno >= NUOFERROR) || (errno<0)) {
        errno=0;
    }
    vprintf(ERRMSG[errno], arg_ptr);
    va_end(arg_ptr);
    /* skip until semicolon ';' is found */
    for(;;) {
        ch=readc2();
        if (ch=='"') {
            do {
               if ((ch=getc(infile)) == EOF) {
                       longjmp(EOF_JMP,1);
                   }
            } while (ch!='"');
        }
        if (ch==';') {
            longjmp(SYNERR_JMP,1);
        }
    }
}

getint(ch)
int *ch;
{
    int number;

    number=0;
    while (isdigit(*ch)) {
        number=number*10 + (*ch & 0x0f);
        *ch=readc1();
    }
    return(number);
}

/* function for testing only
execfunc(current, nparm, parmlist, retype, ret)
int current, nparm;
struct pval *parmlist, *ret;
{
    int loop;

    printf("%s(",FUNCTIONS[current].name);
    for (loop=0; loop<nparm; loop++) {
        switch (parmlist->parmtype) {
        case INTG:
            printf("%d,",parmlist->v.u_int);
            break;
        case STRG:
            printf("\"%s\",",parmlist->v.u_strg);
            break;
        }
        parmlist++;
    }
    printf(")\n");
}
*/

/* dummy function, to be compatible with MPrint */
PrintPage()
{
    getch();   /* push any key to continue */
    setgraph();
}

int initvar(name,type)
char *name;
enum Ptype type;
{
    int varidx;

    if ((varidx=findvar(name)) == ERROR) {
        printf("What happened! I can't find '%s'",name);
        cleanup(1);
    }
    VARTABLE[varidx].parmtype=type;
    return(varidx);
}

initvars()
{
    int varidx;

    varidx=initvar("pattern1", STRG);
    VARTABLE[varidx].v.u_strg=(char *) &PATTERN[0];
    varidx=initvar("pattern2", STRG);
    VARTABLE[varidx].v.u_strg=(char *) &PATTERN[16];
    varidx=initvar("pattern3", STRG);
    VARTABLE[varidx].v.u_strg=(char *) &PATTERN[32];
    varidx=initvar("line", STRG);
    VARTABLE[varidx].v.u_strg=LINE;
}

