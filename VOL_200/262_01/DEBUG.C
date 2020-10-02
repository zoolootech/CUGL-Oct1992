/***************************************************************
 *           An Interactive Debugger for Q/C                   *
 *                                                             *
 *                 by Robert Ramey                             *
 *                 October    1984                             *
 ***************************************************************/

#define MAXLINE 81  /*maximum line of data display format */
#define MAXARGS 12  /*maximum number of arguments of a function */
            /*to be displayed.  note:floats and longs   */
            /*count as two arguments                    */
#define MAXDEPTH 24 /*maximum number of breakpoints in stack   */
#define INSTWIDTH 3 /*width of "call debug" instruction */
#define MAXCOMDEPTH 4   /*maximum depth of inclusion of *.DBG files */
#define APRXSYMBOLS 100 /*approx number of symbols to be defined */
#define SYMSIZE 6   /*size of a symbol */

typedef int boolean;
typedef char *ADDRESS;  /*This machine addresses to the byte */

static boolean trace = 0;   /*trace toggle*/
#define dataseg (&trace) /*assumes trace is first data symbol */

#include <stdio.h>
#include "symdef.h" /*definitions for symbol table routines */

static
FILE *fp[MAXCOMDEPTH];  /*stack of included command files */
        /*file pointer for terminal input */
static
int bcnt = 0,   /*number of breakpoints to be skipped */
    depth = 0,  /*number of currently pending breakpoints */
    fpdepth = 0;    /*level of command file inclusion */
static
SYMBOLTABLE *symtbl;    /*table of symbols and data */

static
char *fptr, format[MAXLINE];    /*format line */

typedef struct symtbl_entry {
    struct symtbl_entry *slnxt, /*next symbol in set/reset list*/
        *slprv, /*previous symbol in set/reset list*/
        *slalpha; /*next symbol in alphabetic list */
    ADDRESS address; /*address of symbol according to *.sym */
    char *symbol,   /*pointer to symbol itself */
        *ptype, /*pointer to format of data or result */
        *arg_format, /*pointer to format of arguments */
            /*(null for data segment items */
        saved_inst[INSTWIDTH + sizeof(int *)],
        /*when break point is set, first instructions*/
        /*are stored here and replaced with call to trap */
        /*followed by address of this structure */
        state;
        /*bit 0 indicates if break point set or not */
        /*bit 1 indicates if break point pending or not*/
    } SYMTBL_ENTRY;
#define PENDING 2
#define SET 1

static
SYMTBL_ENTRY *entrylist = NULL, /*list of set breakpoints */
    *pendlist = NULL,   /*list of pending breakpoints */
    *alphafirst = NULL, /*alphabetic list of symbols */
    *alphalast = NULL;  /*last symbol loaded */

static
struct {        /* special stack for active breakpoints */
    ADDRESS return_address; /* address of calling function */
    ADDRESS *sp;
    SYMTBL_ENTRY *stdata;   /* currently active break point*/
    } bpstack[MAXDEPTH];

char *getfmt();
SYMTBL_ENTRY *dactall();
SYMBOLENTRY *getsym();
void resume();

/***************************************************************
setup - this subroutine should be called at the beginning of
program execution.  It permits initial use of commands to load
symbol tables and set breakpoints.
***************************************************************/
setup()
{
    /* setup symbol table */
    if(!(symtbl = symmk(sizeof(SYMTBL_ENTRY), APRXSYMBOLS))){
        fputs("Not enough memory for symbol table\n", stderr);
        exit(1);
    }
    /* open file to be used for debugger commands */
    fp[0] = fopen("con:", "r");

    ldtbl("DEBUG.SYM"); /*load symbol file */
    comand();       /*accept initial debugger commands */

    actall();/* activate entry point traps and continue */

}
/**************************************************************
trap - when a subroutine is entered this function is called.
Here is where program flow is traced and if a breakpoint encounterd
execution is suspended in order to process debugger commands
**************************************************************/
static void
trap(caller)
ADDRESS caller;
{
    ADDRESS entrypt;
    SYMTBL_ENTRY *stdata;
    boolean all;

    /* retieve entry point address.  assumes that the subroutine */
    /* entry point has been replaced with a "call trap" instruc- */
    /* -tion there leaving on the stack a return address just */
    /* beyond the entry point */
    entrypt = *(&caller - 1) - INSTWIDTH;

    /* following the "call trap" instruction at the subroutine */
    /* entry the address of the symbol data structure is stored */
    stdata = *(int *)(entrypt + INSTWIDTH);

    if(depth == MAXDEPTH)
        fputs("Too many pending breakpoints\n",stderr);
    else{
        /* store information on encounterd breakpoint on */
        /* stack of currently pending subroutines */
        bpstack[depth].stdata = stdata;
        bpstack[depth].return_address = caller;
        bpstack[depth].sp = &caller + 1;

        /* restore original contents to the entry point to */
        /* permit normal execution of subroutine to proceed */
        /* note: this means the tracing will not show */
        /* recurrsive subroutine calls */
        dactbp(stdata);
        /* remove entry point from list to be activated */
        relink(stdata, &pendlist);
        stdata->state ^= PENDING;

        /* replace address of caller with address of resume */
        /* so that resume will gain control when subroutine */
        /* returns.  This will permit resume to display */
        /* return value of subroutine */
        caller = resume; /* return for interrupted routine */

        /* increment index into stack of pending entry points*/
        ++depth;
    }

    if((!bcnt && stdata->state & SET) || trace){
        /* deactivate all entry point traps to permit */
        /* normal use of library subroutines by the debugger */
        dactall();

        /* display subroutine name and its arguments */
        /* assumes that arguments have been pushed on to */
        /* stack before calling trapped subroutine */
        dentry(&caller+1,stdata);
    }
    else
        all = FALSE;

    if(stdata->state & SET) /*if is a a break point */
        if(!bcnt)
            comand();
        else
            --bcnt;

    /* if necessary reactivate all entry point traps */
    if(all) actall();
    
    /* continue execution by loading entry point on top of */
    /* current return address */
    *(&caller - 1) = entrypt;   /* return to break point */
}
/****************************************************************
resume - recieves control when a trapped subroutine returns.
The object of this is to undo the changes done in trap and
permit display of value returned.
*****************************************************************/
static void
resume(){
#asm
    push    af  ;save z flag
    push    hl  ;save return value on stack
#endasm
    if(trace){
        /* display value returned by subroutine */
        dactall();
        dexit(bpstack[depth-1].stdata);
        actall();
    }
    /* reactivate trapping of this subrouine */
    actbp(bpstack[--depth].stdata);
    relink(bpstack[depth].stdata, &entrylist);
    bpstack[depth].stdata->state ^= PENDING;
    bpstack[depth].return_address++; /*leave return address in HL*/
#asm
    pop de  ;recover subroutine return value
    pop af  ;recover flags
    push    de
    ex  (sp),hl ;leave returned original returned value in HL
            ;leave original return address on stack
#endasm
}
/****************************************************************
actall - activate trapping of all subroutine calls for all entry
points in the entry point list.
*****************************************************************/
static void
actall()
{
    SYMTBL_ENTRY *stdata;
    for(stdata = entrylist;stdata;stdata = stdata->slnxt)
        actbp(stdata);
}
/*****************************************************************
dactall - dactivate trapping of all subroutines in the list of
entry points.  This is necessary in order to permit the debugger
program to use library routines without trapping itself.
******************************************************************/
static
SYMTBL_ENTRY *
dactall(entrypt)
ADDRESS entrypt;
{
    SYMTBL_ENTRY *stdata;
    for(stdata = entrylist;stdata;stdata = stdata->slnxt){
        dactbp(stdata);
    }
}
/******************************************************************
dentry - display entry point with arguments on entering function 
******************************************************************/
static void
dentry(stackpt, stdata)
ADDRESS stackpt;
SYMTBL_ENTRY *stdata;
{
    char *p;
    /* set up string for format of arguments */
    if(!stdata->ptype)
        /* if no format string has been specified */
        p = "";
    else
        p = stdata->arg_format;
    ddata(">%s", stackpt, p, stdata->symbol, 2);
}
/***************************************************************
dexit - display breakpoint with return value on exit of function
****************************************************************/
static void
dexit(stdata,retval)
SYMTBL_ENTRY *stdata;
int retval;
{
    char c, *a, *p, *t;
    long longval;
    int *aptr;

    /* isolate first part of format string to be used to display */
    /* argument. */
    if(!stdata->ptype){
        p = "";
        a = &c;
    }
    else{
        p = stdata->ptype;
        a = stdata->arg_format;
    }
    c = *a;
    *a = '\0';

    /* if a long is returned, get long value from special stack */
    /* check to see if prototype is long or float */
    for(t = p;*t != '%' && *t != NULL;++t);
    if(*t){
        switch(*++t){
        default:
            aptr = &retval;
            break;
        case 'l':
        case 'L':
        case 'f':
        case 'F':
            /* leave address of long value in HL */
            aptr = &longval;
#asm
            call    ?save32;/* move TOS to longval */
#endasm
        }
    }
    ddata("<%s = ", aptr, p, stdata->symbol, 2);
    *a = c;
}
/*************************************************************
ddata - display data from memory
**************************************************************/
static void
ddata(idstr, data_address, ptype, symbol, charsize)
char    *idstr, /*address of string identifying symbol */
    *symbol, /*address of symbol to be displayed */
    *ptype; /*points to format of display */
int data_address[],
        /*points to first memory location to be displayed */
    charsize;   /*size of character variable */
        /*1 if in memory, 2 if on a stack */
{
    int arg[MAXARGS];   /*arguments to be displayed */
        /*note: must be first for fprint (below) to function */
    int i;
    
    /*indent display according to depth of call stack */
    for(i = 0;i < depth;++i) format[i] = ' ';
    fptr = strmov(format+depth,idstr);

    /*first item displayed is the symbol it self */
    arg[0] = symbol;

    /*fill rest of arg array */
    farg(arg + 1, data_address, &ptype, charsize);

    fptr = strmov(fptr, "\n");
    fprintf(stderr,format); /*format is format string. */
        /*arguments are on top of stack already */
        /*because arg[] is first local variable */
}
/***************************************************************
farg - fill in array of arguments to be passed to fprintf.
Leave modifyed format string in static variable format.
**************************************************************/
static void
farg(arg, data_address, ptype, charsize)
int arg[],  /* array into which arguments are to be loaded */
    data_address[], /* array which currently holds arguments */
    charsize; /* size of objects to be displayed with %c */
        /* These are 1 if character is in memory, */
        /* 2 if character has been pushed onto stack */
char    **ptype; /* pointer to address of format string to be used */

{
    int icount, *targ;
    char c, state;
    state = ' ';
    icount = 0;
    while(c = *(*ptype)++){
        switch(state){
        case ' ':
            switch(c){
            default:
                icount = 0;
                break;
            case '%':
                state = '%';
                break;
            case '*':
                state = '*';
                icount = 1;
                break;
            case '(':
                *fptr++ = c;
                farg(arg,data_address,ptype,charsize);
                c = ')';
                break;
            case ')':
                return;
            case '\"':
                state = '\"';
                break;
            }
            break;
        case '\"':
            if(c == '\"')
                state = ' ';
            break;
        case '*':
            switch(c){
            case '*':
                ++icount;
                break;
            case '%':
                state = '%';
                break;
            case '(':
                fptr -= icount;
                targ = data_address++;
                while(icount--)
                    targ = *targ;
                *fptr++ = '(';
                farg(arg,targ,ptype,1);
                c = ')';
            default:
                state = ' ';
                icount = 0;
                break;
            }
            break;
        case '%':
            state = ' ';
            if(c == '%' || c == ')' || c == '('){
                fptr -= icount;
                break;
            }
            if(c == 'c' && charsize == 1 && icount == 0){
                targ = (char *)data_address;
                data_address =
                (int *)(1 + (char *)data_address);
            }
            else{
                targ = data_address++;
                while(icount--)
                    targ = *targ;
            }
            if(c == 'c' && charsize == 1)
                *arg++ = *(char *)targ;
            else
            if(c == 'f' || c == 'l'){
                /* longs and floats use two words */
                *arg++ = *targ++;
                *arg++ = *targ;
            }
            else
                /* everything else uses one word */
                *arg++ = *targ;
        }
        *fptr++ = c;
    }
}
/*****************************************************************
comand - read and process command from keyboard or file
*****************************************************************/
static void
comand()
{
    SYMBOLENTRY *ste;
    SYMTBL_ENTRY *stdata;
    char c, *fstart, *fend;
    int addr, i;
    FILE *fl;

    for(;;){    /* main command loop */

    putc('*',stderr);   /* prompt screen for input */

    /* get line from current input file */
    while(!fgets(format,MAXLINE - 1,fp[fpdepth])){
        if(!fpdepth) return;
        --fpdepth;
        continue;
    }
    if(!isatty(fileno(fp[fpdepth])))/*if input not from keyboard */
        fputs(format,stderr);   /*echo input to screen */

    fstart = format;
    while(isspace(*++fstart));  /*skip leading spaces */
    switch(tolower(format[0])){
    case 'b':/*toggle breakpoint */
        /*b<breakpoint symbol> [<format string>]*/
        /*b without arg toggles all breakpoints */
        if(!*fstart){   /*no arguments*/
            for(stdata = pendlist;stdata;
                stdata = stdata->slnxt)
                stdata->state ^= SET;
            for(stdata = entrylist;stdata;
                stdata = stdata->slnxt)
                stdata->state ^= SET;
            break;
        }

        /* with a symbol */
        if(!(ste = getsym(&fstart))){
            serr();
            break;
        }
        stdata = (SYMTBL_ENTRY *)symdat(symtbl,ste);
        if(stdata->address < dataseg){
            stdata->state ^= SET;
            getfmt(stdata,fstart);  /* reset format */
        }
        else
            fputs("Symbol not in code segment\n",stderr);
        break;
    case 'f':/*specify format string for subroutine arguments or */
        /*external variable */
        /*f <symbol> <format string> */
        if(!(ste = getsym(&fstart))){
            serr();
            break;
        }
        stdata = (SYMTBL_ENTRY *)symdat(symtbl,ste);
        if(!*fstart){
            /* return space used by current format string*/
            if(stdata->ptype){
                free(stdata->ptype);
                stdata->ptype = NULL;
            }
        }
        else
            /* set new format string from rest of command*/
            getfmt(stdata, fstart);
        break;
    case 'g':/*continue execution skipping n breakpoints*/
        /*g [<number of break points to be displayed>] */
        i = sscanf(fstart, "%d", &bcnt);
        if(i == EOF) bcnt = 0;
        return;
    case 't':/*toggle trace mode */
        trace = 1 - trace;
        fputs("trace o", stderr);
        fputs(trace ? "n\n" : "ff\n", stderr);
        break;
    case 'c':/*process debug commands from a disk file */
        /*c <filename> */
        if(++fpdepth == MAXCOMDEPTH){   /* nested too deep? */
            --fpdepth;
            fputs("Comand file nested to deeply\n",stderr);
            break;
        }
        /* get filename from rest of command line */
        fname(fstart, ".DBG");
        if(!(fp[fpdepth] = fopen(fstart, "r"))){
            fprintf(stderr,
            "Could not open file %s\n",fstart);
            --fpdepth;
        }
        break;
    case 'l':/*load file of symbols generated by link */
        /*l <filename> */
        /* get file name from rest of command line */
        fname(fstart, ".SYM");
        ldtbl(fstart);
        break;
    case 'd':/*display global variabk */
        /*d <symbol name> */
        /*d without args displays all external variables in*/
        /* the data segment */
        if(!*fstart){
            for(stdata = alphafirst; stdata;
                 stdata = stdata->slalpha)
                ddata("%s ",stdata->address,
                    stdata->ptype, ste, 1);
            break;
        }
        if(!(ste = getsym(&fstart))){
            serr();
            break;
        }
        stdata = (SYMTBL_ENTRY *)symdat(symtbl, ste);
        ddata("%s=",stdata->address,
        getfmt(stdata, fstart),ste,1);
        break;
    case 'w':/*display argument stack */
        i = depth;
        for(depth = 0;depth < i;++depth){
        dentry(bpstack[depth].sp,bpstack[depth].stdata);
        }
        depth = i;
        break;
    case 's':/*display or set symbol table entry */
        /*s without arguments display all symbols */
        /*s <symbol> displays symbol and address */
        /*s <symbol> <hex> creates symbol with given address*/
        if(!*fstart){
            for(stdata = alphafirst; stdata;
                 stdata = stdata->slalpha)
                dsymtbl(stdata);
            break;
        }
        fend = fstart;
        ste = getsym(&fend);
        i = sscanf(fend, "%4x", &addr);
        if(i == EOF) i = 0;
        if(!i && ste)
            dsymtbl((SYMTBL_ENTRY *)symdat(symtbl,ste));
        else if(!i && !ste)
            serr();
        else if(i && ste){
            stdata = (SYMTBL_ENTRY *)symdat(symtbl, ste);
            stdata->address = addr;
            dsymtbl(stdata);
        }
        else
            loadsym(fstart,addr);
        break;
    default:
        fputs("invalid debugger command\n", stderr);
    case '\n':;
    /*null command*/
    }
    }
}
/*************************************************************
dsymtbl - display data for one symbol table entry
**************************************************************/
static void
dsymtbl(stdata)
SYMTBL_ENTRY *stdata;
{
    char c;
    c = (stdata->state & SET) ? 'B' : ' ';
    fprintf(stderr,
        stdata->ptype ?
             "%c %x\t%s\t%s\n" : "%c %x\t%s\n",
        c, stdata->address, stdata->symbol, stdata->ptype);
}
/**************************************************************
relink - remove entry point from its current list and link on 
to head of a new list.
**************************************************************/
static void
relink(stdata, list)
SYMTBL_ENTRY *stdata, **list;
{
    /*delink from current breakpoint list */
    if(stdata->slnxt)
        stdata->slnxt->slprv = stdata->slprv;
    stdata->slprv->slnxt = stdata->slnxt;

    /*link into requested breakpoint list*/
    if(*list)
        (*list)->slprv = stdata;
    stdata->slnxt = *list;
    stdata->slprv = list;
    *list = stdata;
}
/***************************************************************
actbp - activate trapping of a given entry point.
***************************************************************/
static void
actbp(stdata)
SYMTBL_ENTRY *stdata;
{
    char *j;
    /*load with "call trap" instruction */
    j = stdata->address;
    j[0] = 0xCD;    /* Z-80 call */
    j[1] = trap;        /* "trap" address */
    j[2] = trap >> 8;

    /*followed by address of corresponding symbol table entry */
    *((SYMTBL_ENTRY **)(j + INSTWIDTH)) = stdata;
}
/****************************************************************
dactbp - deactivate trapping of a given entry point.
*****************************************************************/
/* dactbp - deactivate a breakpoint */
static void
dactbp(stdata)
SYMTBL_ENTRY *stdata;
{
    /* restore original contents of breakpoint address */
    memcpy(stdata->address,stdata->saved_inst,
        INSTWIDTH + sizeof(SYMTBL_ENTRY *));
}
/****************************************************************
fname - check and build name of file
****************************************************************/
static void
fname(prfix,sufix)
char *prfix, *sufix;
{
    char c;
    --prfix;
    while((c = *++prfix) == ':' || isalnum(c));
    if(*prfix != '.')
        strmov(prfix, sufix);
    else{
        while(isalnum(*++prfix));
        *prfix = '\0';
    }
}
/*****************************************************************
getsym - get symbol from string. return pointer to next non
blank character.
*****************************************************************/
static SYMBOLENTRY *
getsym(text)
char **text;
{
    char *i, *j;
    int k;
    if(!*text){
        fputs("Comand needs symbol\n",stderr);
        return NULL;
    }
    for(j = *text,k = 0;
        *j && (!isspace(*j)) && (k < SYMSIZE);
        ++j,++k);
    i = j;
    while(!isspace(*j)) ++j;    /*skip rest of current word */
    while(isspace(*j)) ++j;     /*skip to next word */

    if(*(i - 1) == '?') --i;    /*strip off trailing '?'*/
    *i = '\0';
    strlwr(*text);
    i = *text;
    *text = j;
    return symlkup(symtbl, i);
}
/****************************************************************
serr - Symbol not defined message
*****************************************************************/
static void
serr()
{
    fputs("Symbol not defined\n", stderr);
}
/****************************************************************
getfmt - get and/or store format to be displayed
*****************************************************************/
static char *
getfmt(stdata, fstart)
SYMTBL_ENTRY *stdata;
char *fstart;
{
    char *p;
    if(p = strchr(fstart, '\n'))
        *p = NULL;
    if(*fstart) {
        p = stdata->ptype;
        if(p)
            free(p);
        stdata->ptype =
        p = malloc(1 + strlen(fstart));
        strmov(p,fstart);
        if(stdata->address < dataseg)
            initp(stdata);
    }
    else if(!(p = stdata->ptype))
        p = " %x";
    return p;
}
/****************************************************************
initp - initialize format pointers for a code segment symbol
*****************************************************************/
static void
initp(stdata)
SYMTBL_ENTRY *stdata;
{
    int arg, level;
    char *p;
    p = stdata->ptype;
    /*separate first % as format for return value */
    arg = FALSE;
    level = 0;
    for(;;){
        switch(*p++){
        case NULL:
        case '\n':
            break;
        case '(':
            ++level;
            continue;
        case ')':
            --level;
            continue;
        case '%':
            if(arg && !level)
                break;
            else
                arg = TRUE;
            continue;
        case '\t':
        case ' ':
            if(arg && !level){
                --p;
                break;
            }
            continue;
        default:
            continue;
        }
        break;
    }
    stdata->arg_format = p;
}
/************************************************************
ldtbl - load a symbol file from disk given its name
************************************************************/
static void
ldtbl(fname)
char *fname;
{
    FILE *fl;
    ADDRESS addr;
    SYMBOLENTRY *ste;
    if(!(fl = fopen(fname, "r"))){
        fprintf(stderr,
        "Could not open file %s\n",fname);
        return;
    }
    while(fscanf(fl,"%x %s",&addr,format) == 2){
        /* symbols starting with ? are generally*/
        /* not interesting in programs compiled with */
        /* QC.  I have chosen to exclude them in */
        /* to reduce size of symbol table */
        if(format[0] == '?')
            continue; /*skip internal symbols */
        /* QC changes initial _ to @ to avoid */
        /* error messages from linker */
        if(format[0] == '@')
            format[0] = '_';
        fptr = format;
        if(ste = getsym(&fptr)){
            fprintf(stderr,
                "%s already loaded\n",ste);
            continue;
        }
        if(!loadsym(format, addr))
            break;
    }
    fclose(fl);
}
/****************************************************************
loadsym - add a new symbol to symbol table
*****************************************************************/
static int
loadsym(symbol, addr)
char *symbol;
ADDRESS addr;
{
    SYMBOLENTRY *ste;
    SYMTBL_ENTRY *stdata;

    if(!(ste = symadd(symtbl,symbol))){
        fputs("Can't load any more symbols\n",
        stderr);
        return FALSE;
    }
    stdata = (SYMTBL_ENTRY *)symdat(symtbl, ste);
    stdata->address = addr;
    stdata->state =
    stdata->ptype = NULL;
    stdata->symbol = (char *)ste;
    
    /*if in code segment add to entrylist */
    if(addr < dataseg){
        /*store in symbol table original contents of entry pt*/
        memcpy(stdata->saved_inst,
            addr,
            INSTWIDTH + sizeof(SYMTBL_ENTRY *));
        /*link into list of entry points*/
        if(entrylist)
            entrylist->slprv = stdata;
        stdata->slnxt = entrylist;
        entrylist = stdata;
        stdata->slprv = &entrylist;
    }
    
    /*add to alphabetic list */
    if(!alphafirst)
        alphalast =
        alphafirst = stdata;
    else
        alphalast->slalpha = stdata;
    alphalast = stdata;
    stdata->slalpha = NULL;
    return TRUE;
}
/*******************************************************************
memcpy - copy n characters to destination from source
*********************************************************************/
static void
memcpy(dest, source, count)
char *dest, *source;
int count;
{
    while(count--) *dest++ = *source++;
}
if(!alphafirst)
        alphalast =
        alphafirst = stdata;
    e