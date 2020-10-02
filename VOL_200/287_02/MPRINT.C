#include <stdio.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys\types.h>
#include <io.h>
#include <sys\stat.h>

#define unreadc(ch) ungetc(ch,infile)

#define MAXPARM 9       /* max. number of parameters per function */
#define MAXLPP 88       /* max. number of line per page */
#define TBUFSIZ 16384   /* Text buffer size */

#define Boolean int
#define TRUE 1
#define FALSE 0
#define ERROR (-1)

enum Ptype {            /* Parameter Type */
    UNDEFINED, INTG, STRG
};

struct funcdesc {       /* Function description entry */
    char *name;         /* name in ASCII */
    enum Ptype rettype; /* no return value or expected return type */
    int nuparm;         /* number of paramters for the functions */
    enum Ptype parmstype[MAXPARM];      /* definition parameter type */
};

/* The function names must arrange in ascending order */
/* and same as the order in EXECFUNC.ASM              */
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

/* total number of functions */
#define NFUNC (sizeof(FUNCTIONS) / sizeof(struct funcdesc))


/* Error message in C format string */
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
    "Idendifier Expected\n"
};

/* total number of error messges */
#define NUOFERROR (sizeof(ERRMSG) / sizeof(char *))

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

char LINE[160]; /* for variable line */

struct pval {   /* parameter record */
    enum Ptype parmtype;
    union {
        int u_int;
        char *u_strg;
    } v;
};

struct pval RETVAL, *expr();

/* Only first 12 characters of identifier is significant */
/* Maximum number of characters in string(s) per function is 512 */
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

/* configurable parameters */
int COL_OFFSET=0, ROW_OFFSET=0, LINE_HEIGHT=12, CHAR_WIDTH=12, LINEPERPAGE=66;
int FRAME_WIDTH=960, FRAME_HEIGHT=792, VERT_DEN=0, HORZ_DEN=4; 

jmp_buf EOF_JMP, SYNERR_JMP;

int EOF_FLAG=0, TEXT_EOF=0, REMAIN=0;
char TBUF[TBUFSIZ+1], *PAGE[MAXLPP], *PAGEPTR;

FILE *infile;   /* GRAD command file */
int textfile;   /* text file */

main(argc,argv)
int argc;
char *argv[];
{
    char ch;
    int ret, frame1;
    FILE *fopen();

    /* Interpret the options if any */

    argv++;
    while (argc-- > 1 && *(*argv)++=='-') {
        if (**argv) {
            ch= *(*argv)++;
            ret=atoi(*argv);
        } else {
            ch=0;
        }
        switch(ch) {
        case 'H':
            FRAME_HEIGHT=ret;
            break;
        case 'W':
            FRAME_WIDTH=ret;
            break;
        case 'c':
            COL_OFFSET=ret;
            break;
        case 'd':
            HORZ_DEN=ret & 0x0f; 
            break;
        case 'h':
            LINE_HEIGHT=ret;
            break;
        case 'p':
            if (ret > MAXLPP) {
                fprintf(stderr,"Max number of line per page is %d\n",MAXLPP);
                exit(1);
            }
            LINEPERPAGE=ret;
            break;
        case 'r':
            ROW_OFFSET=ret;
            break;
        case 'v':
            VERT_DEN=ret ? 0x10 : 0;
            break;
        case 'w':
            CHAR_WIDTH=ret;
            break;
        default:
            fprintf(stderr,"Unknow option -%c ** IGNORED **\n",ch);
            break;
        }
        argv++;
    }

    /* Any command line argument remain ? */
    if ((argc < 1) || (argc > 2)) {
        /* no more or too much */
        fprintf(stderr,"Usage: mprint [options] commandfile [textfile]\n");
        exit(1);
    }
    (*argv)--;

    /* open GRAD command file */
    if ((infile=fopen(argv[0], "r")) == (FILE *) NULL) {
        fprintf(stderr,"mprint: Command file not found\n");
        exit(1);
    }
    if (argc==2) {
        /* Open text file */
        if ((textfile=open(argv[1],O_RDONLY | O_BINARY)) == ERROR) {
            fprintf(stderr,"mprint: text file not found\n");
            exit(1);
        }
    } else {
        TEXT_EOF=TRUE;
    }
    GRADinit();         /* Initialize GRAD environemnt */
    initvars();         /* Initialize my varaibles */
    frame1=CreateFrame(FRAME_WIDTH,FRAME_HEIGHT);       /* Create Frame */
    SelectFrame(frame1);        /* use it as default output */

    if (setjmp(EOF_JMP) != 0) {
        /* End Of File is reached */
        fclose(infile);
        if (EOF_FLAG) { 
            printf("mprint: Unexpected End Of File !\n");
        } else {
            PrintAll();
        }
        close(textfile);
        cleanup(EOF_FLAG);
        exit(0);
    }
    setjmp(SYNERR_JMP);
    /* Back to here if any syntax error is found */
    for (;;) {
        EOF_FLAG=0;
        stmt();
    }
}

readc0()        /* return next character but skip over comments */
{               /* comment is treated as a space.               */
    int ch;     /* comments can be nested                       */

    if ((ch=getc(infile)) == EOF) {
        longjmp(EOF_JMP,1);     /* EOF found */
    }
    if (ch != '/') return(ch);
    if ((ch=getc(infile)) == EOF) {
        longjmp(EOF_JMP,1);     /* EOF found */
    }
    if (ch != '*') {
        ungetc(ch,infile);
        return('/');            /* not a comment */
    }
    for (;;) {
        if ((ch=readc0()) == '*') {
            while((ch=readc0()) == '*') ;
            if (ch=='/') return(' ');   /* end of comment, return a space */
        }
    }
}

readc1()        /* return a single charater but skip over control characters */
{
    int ch;

    do {
        ch=readc0();
        if (isspace(ch)) return(' ');
    } while (ch < 0x20);
    return(ch);
}

readc2()        /* return a single character but skip over spaces */
{
    int ch;

    do {
        ch=readc1();
    } while (ch==' ');
    EOF_FLAG=1;
    return(ch);
}

/* processing of  statement, see also stmt() in interp.c */
stmt()
{
    int ch, varidx,funcidx, nuparmoffunc, nparm;
    struct pval *valptr, parmlist[MAXPARM];

    PSPTR=CHARS;
    varidx=-1;
    if (!identifier()) {
        synerr(1);
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
    execfunc(funcidx, FUNCTIONS[funcidx].nuparm, parmlist,
             FUNCTIONS[funcidx].rettype, &RETVAL);
    if (varidx>=0) {
        VARTABLE[varidx]=RETVAL;
    }
endofstmt:
    ch=readc2();
    if (ch!=';') {
        printf("Warning: Semicolon ';' expected. Assuming present\n");
        unreadc(ch);
    }
}

/* read the identifier on input if any */
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

/* find the variable given in name from the variable table */
findvar(name)
char *name;
{
    int loop;

    for (loop=0; loop<nuofvar; loop++) {
        if (strcmp(name,VARNAME[loop])==0) return(loop);
    }
    return(ERROR);
}

/* find the function given in name from the function table */
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

/* processing of expression, see also expr() in interp.c */
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
            *PSPTR++ = ch;
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
            VARTABLE[varidx].parmtype=INTG;
            RETVAL.parmtype=INTG;
            RETVAL.v.u_int=(int) &VARTABLE[varidx].v.u_int;
        }
    } else if ((ch==',') || (ch==')')) {
        unreadc(ch);
        RETVAL.parmtype=INTG;
        RETVAL.v.u_int=0;
    }
    return(&RETVAL);
}

/* processing of syntax error */
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

/*get an integer */
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

/*   *** FOR DEBUGGING ***
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

/* read next page of text data */
readpage()
{
    int temp1, linenu, temp2, inkline;

    inkline=0;
    if (!TEXT_EOF) {
        temp1=TBUFSIZ-REMAIN;
        if ((temp2=read(textfile, &TBUF[REMAIN], temp1)) != temp1) {
            TEXT_EOF=TRUE;
        }
        REMAIN+=temp2;
    }
    linenu=0;
    if (REMAIN != 0) {
        PAGEPTR=TBUF;
        for (; (linenu<LINEPERPAGE) && (REMAIN>0); ) {
            PAGE[linenu]=PAGEPTR;
            linenu++;
            for (; REMAIN>0 ;) {
                while (REMAIN-- > 0) {
                    if (*PAGEPTR==12) {         /* is it a FORM FEED ? */
                        *PAGEPTR++='\0';        /* yes, end of page */
                        return(linenu);
                    }
                    if (*PAGEPTR==0) {
                        *PAGEPTR++ = 0x80; /* change 0 in text file to 0x80 */
                        continue;
                    }
                    if (*PAGEPTR++ == '\r')     /* is it a carriage return ? */
                        break;                  /* yes, end of search */
                    inkline=linenu;
                }
                if ((REMAIN>0) && (*PAGEPTR=='\n')) {   /* new line ? */
                    *(PAGEPTR-1)='\0';  /* end of line */
                    *PAGEPTR++;
                    REMAIN--;
                    break;
                }
            }
        }
        if (REMAIN<=0) {        /* any data remain in the buffer ?*/
            linenu=inkline;     /* No more! */
            REMAIN=0;
            *PAGEPTR='\0';
        }
    }
    return(linenu);
}

/* Print text data with graphics */
PrintPage()
{
    int linenu, loop;
    char *ptr;
    
    linenu=readpage();
    PrintFrame(VERT_DEN+HORZ_DEN,PAGE,linenu,LINE_HEIGHT,0); 
    SetOrg(0,0);
    PlotType(1);
    HorzLine(0,0,960,792);
    PlotType(0);
    ptr=TBUF;
    for (loop=REMAIN; loop>0; loop--) {
        *ptr++ = *PAGEPTR++;    /* copy remaining data to begining of buffer */
    }
    PAGEPTR=TBUF;
}

/* print all the remaining text data */
PrintAll()
{
    int linenu, loop;
    char **tpage, *ptr;

    while ((linenu=readpage()) > 0) {
        loop=LINEPERPAGE-linenu;
        tpage=PAGE;
        while (linenu--) {
            ptext(*tpage++);
            skiplong(12);
        }
        while (loop-- > 0) {
            skiplong(12);
        }
        ptr=TBUF;
        for (loop=REMAIN; loop>0; loop--) {
            *ptr++ = *PAGEPTR++;
        }
        PAGEPTR=TBUF;
    }
}

/* Initialize one variable */
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

/* Initialize mprint variables */
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

