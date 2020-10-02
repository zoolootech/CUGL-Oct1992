/*
 *  letter      A short, savage, document processor designed to serve one
 *              function; format a one page letter.
 *
 *              I don't know about anybody else, but I write about 25 one-
 *              pagers a week and I like my letters to look pretty. This
 *              requires vertical centering and I've not found a word
 *              processor yet that can do it (although qnxs' document
 *              processor 'doc' did provide sufficient primitives to
 *              write a REALLY complex macro that did and UNIXs' nroff and
 *              a whole bunch of shell commands could do the job as well).
 *
 *              letter will vertically center a one page letter, will
 *              divert the address to a file (suitable for printing on
 *              an envelope by address) and support a small set of document
 *              processing functions. It is weak on error checking and
 *              semantic analysis (so don't push it!). See the separate
 *              document 'letter.doc' for more detail.
 *
 *              This program took less than six hours to write. It is
 *              probably far from perfect. I'm not offering this as a
 *              replacement for nroff (or even WordStar), but it does
 *              what I need acceptably.
 *
 *              This is thrown freely into the public domain and can be
 *              used, modified, and distributed by any person or agency
 *              that wants to. I would appreciate it if the original
 *              author credit remains and would also request that no
 *              compensation be charged by anyone passing this turkey
 *              around.
 *
 *      This program was written by Jon Simkins and tested using the DeSmet C
 *      compiler and MS-DOS 3.0.
 *
 *  Revision Notes
 *
 *      Date        Revision
 *      --------------------
 *      86.02.04    Added UL_ON and BF_ON control strings for printers that
 *                  directly support underlining and boldface (most if them
 *                  these days).
 *                  Changed setDate() function for Wizard C and recompiled. js.
 *      
 *      Copyright (c) 1985 SoftFocus
 *                         1343 Stanbury Dr.,
 *                         Oakville, Ontario, Canada
 *                         L6L 2J5
 *                         (416)825-0903
 *
 *                      Compuserve: 76566, 1172
 */
#include <stdio.h>
#include <bdos.h>      /* for getdate() structure                     */

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define MAXLINE 200     /* maximum input line. Assumed larger than     */
                        /* maximum print line.                         */
#define LPP     66      /* maximum lines per page.                     */
#define ERROR -1
#define HEAD 6       /* lines left at top of page when paper is loaded */

/*
 *  program defaults and printer controls.
 *  If your printer cannot backspace one character, don't use underline or
 *  boldface.
 */
#define DEFIN           0       /* default indent                          */
#define DEFRM           65      /* default right margin                    */
#define DEFPO           5      /* default page offset                     */
#define DEFJU           FALSE   /* justification off                       */
#define DEFFI           TRUE    /* fill on                                 */

#define FF              "\014"  /* string to formfeed the printer          */
#define BACKSPACE       8       /* character to backspace the printer      */
/*
 *  Printer Control Strings
 *  -----------------------
 *  If your printer supports such niceties as automatic underlining and
 *  boldface, then fill in the following defines in the logical manner.
 *  If they are left undefined, then letter will underline by backspacing
 *  and printing a '_' and boldface by doublestriking.
 *  The given examples are for a Brother HR-35 daisywheel. This is disabled
 *  in the distribution version.
 */
/* #define UL_ON    "\033E" *//* enable auto underline                  */
/* #define UL_OFF   "\033R" *//* disable auto underline                 */
/* #define BF_ON    "\033O" *//* enable auto boldface                   */
/* #define BF_OFF   "\033&" *//* disable auto boldface                  */

FILE *fd;               /* main input file */
FILE *fdAdd;            /* address file    */
FILE *printer;          /* printer device  */
int hardCopy;           /* true if printed */
#define PRINTER "LPT1"  /* hardcopy output */
#define CONSOLE "CON"   /* console output  */

/*
 *  letter array, line buffer, and counters
 */                  
char *lines[LPP];           /* build the page here                      */
int lineCount;
char pLine[MAXLINE];        /* build the current line here              */
int pCount;                                                              
int saveJu, saveFi;         

/*
 *  command array & defines
 */
#define COMCOUNT 13
char *commands[] = {". ", "in", "po", "ju", "rm", "ti", "br",
                    "as", "ae", "sp", "dt", "sg", "ce", "fi"};
#define COMMENT     0
#define INDENT      1
#define PAGEOFF     2
#define JUSTIFY     3
#define RM          4
#define TI          5
#define BR          6
#define AS          7
#define AE          8
#define SP          9
#define DT          10
#define SG          11
#define CENTER      12
#define FILL        13

/*
 *  margins, switches.
 *  There is currently no way to disable the interpretation of the characters
 *  listed below.
 */
#define BC '^'      /* boldface toggle  */
#define UC '_'      /* underline toggle */
#define HS '~'      /* 'hard' space (not spread by justification)          */

int rm, in, ti, ju, po, fi, br, sp, ce;
int addFlag;                    /* whether address redirection in effect   */
int ul, bf;                     /* underline, boldface currently selected  */


/*
 *  off and away
 */
main(argc, argv)
int argc;
char *argv[];
{
    char buff[MAXLINE], word[MAXLINE];
    int index, len;
    char *pr;
    register j;

    /*
     *  check argument count
     */
    if (argc > 3 || argc < 2)
        erExit("usage: letter [-p] <fName>");

    /*
     *  get command line option(s)
     */
    j=1;
    if (argv[j][0] == '-' && argv[j][1] == 'p') {
        pr = PRINTER;
        hardCopy = TRUE;
        j++;
    }
    else {
        pr = CONSOLE;
        hardCopy = FALSE;
    }

    /*
     *  open input file
     */
    if ( (fd=fopen(argv[j], "r")) == 0) {
        printf("Could not open '%s'\n", argv[j]);
        exit(ERROR);
    }

    /*
     *  set defaults
     */
    ul = bf = addFlag = FALSE;
    in = DEFIN;
    rm = DEFRM;
    ju = DEFJU;
    fi = DEFFI;
    if (hardCopy)
        po = DEFPO;
    else
        po = 0;

    ti = ce = lineCount = 0;

    /*
     *  input/process loop
     */
    pCount = 0;
    pLine[0] = '\0';
    while( fgets(buff, MAXLINE, fd) != NULL) {
        trim(buff);
        /*
         *  blank line?
         */
        if (buff[0] == '\0') {
            pFlush(FALSE);
            pCount=1;
            pFlush(FALSE);
            continue;
        }

        /*
         *  check for any command lines
         */
        if (buff[0] == '.') {
            dotComm(buff);
            continue;
        }

        /*
         *  if fill mode is off, just pass the line on through.
         */
        if ( !(fi)) {
            strcpy(pLine, buff);
            pCount = lenCal(pLine);
            pFlush(ju);
        }
        else {
        /*
         *  must be text; add it (word at a time) to the current print
         *  line.
         */
            index=0;
            do { 
                index = getWord(index, buff, word);
                if (index == 0)
                    continue;
                len = lenCal(word);
                /*
                 *  if there's still room, add word to current line
                 */
                if ( ( len + pCount) < (rm-in-ti) ) {
                    if (pCount != 0) {
                        strcat( pLine, " ");
                        pCount++;
                    }
                    strcat( pLine, word);
                    pCount += len;
                }
                /*
                 * 'print' current line and start new one.
                 */
                else {
                    pFlush(ju);
                    strcpy(pLine, word);
                    pCount=len;
                }
            } while (index);
        }
    }
    pFlush(FALSE);
    fclose(fd);

    /*
     *  all text is in internal text buffer nicely formatted.
     *  Now center it on the page and print it.
     */

    /*
     *  strip off any trailing blank lines
     */
    while ( trim(lines[lineCount][0]) == '\0')
        lineCount--;

    /*
     *  center the text vertically
     */
    printer = fopen(pr, "w");
    len = ( (LPP-lineCount) / 2) - HEAD + 1;
    if (hardCopy)
        for(j=0; j<=len; j++)
            lprint("\n");
    
    /*
     *  ... and print it all
     */
    for(j=0; j<= lineCount; j++) {
        lprint(lines[j]);
        lprint("\015\n");
        free(lines[j]);
    }
    if (hardCopy)
        lprint(FF);
    fclose(printer);
}

/*
 *  trim    Strip off all trailing white spaces
 */
trim(str)
char *str;
{
    register j;

    for( j=strlen(str); (str[j] <= ' ') && (j > -1); j--);
    str[++j] = '\0';
}

/*
 *  strip       Pull off all leading spaces.
 *              Stripped string is returned in place.
 */
strip(str)
char *str;
{
    register j, k;

    for(j=0; (str[j] <= ' ') && (str[j]); j++);
    if ( !(j) )
        return;
    k=0;
    do {
        str[k++] = str[j];
    } while (str[j++]);
}

/*
 *  erExit      Print an error message and die
 */
erExit(str)
char *str;
{
    printf("%s\n", str);
    exit(ERROR);
}

/*
 * getWord  Pull the next word off the passed string. Return an index into
 *          the source string pointing to the next word.
 */
getWord(index, buff, word)
int index;
char *buff, *word;
{
    register j = 0;
    
    while ( buff[index] <= ' ' ) {
        if (buff[index] == '\0') {
            word[0] = '\0';
            return 0;
        }
        index++;
    }
    while ( (buff[index] != ' ') && (buff[index] != '\0') )
        word[j++] = buff[index++];
    word[j++] = '\0';
    return index;
}

/*
 *  pFlush  Add the current print line to lines[].
 */
pFlush(just)
int just;
{
    register j;
    char *malloc();

    if (pCount == 0)
        return;
    if (lineCount == LPP)
        erExit("Page overflow! This utility only works for one page letters.");

    /*
     *  insert page offset, indent
     */
    trim(pLine);
    if (ce) {
        strip(pLine);
        center(pLine, rm-in-ti);
        ce--;
    }
    else
    if (just)
        justify(pLine, rm-in-ti);

    lines[lineCount] = malloc( MAXLINE );
    for(j=0; j<po+in+ti; j++)
        lines[lineCount][j] = ' ';
    ti=0;
    strcpy( &lines[lineCount++][j], pLine);     
    if (addFlag)
        fprintf(fdAdd, "%s\n", pLine);

    pCount = 0;
    pLine[0] = '\0';
}

/*
 *  justify     Right justify a line in place. This is a fairly sleazy
 *              algorithm, but I don't often need justification. Hackers
 *              note!
 */
justify(line, margin)
char *line;
int margin;
{
    int lp=0, rp=0, sp, len;

    len=strlen(line);
    margin += len - lenCal(line);
    sp = margin - len;
    lp=0; rp=len;

    while (sp) {

        /*
         *  find first space from current left position
         */
        while(line[lp] != ' ') {
            if (line[lp] == '\0') {
                lp=0;
                break;
            }
            lp++;
        }
        /*
         *  add a space and find next non-blank
         */
        if (lp) {
            addSpace(line, lp, 1);
            len++;
            sp--;
            while ( (line[lp] <=' ') && (line[lp]) )
                lp++;
        }
        if (sp) {
            while (line[rp] != ' ') {
                rp--;
                if (rp < 0) {
                    rp=len;
                    break;
                }
            }
            if (rp != len) {
                addSpace(line, rp, 1);
                len++;
                sp--;
                while (line[rp] <=' ' && (rp > -1) )
                    rp--;
                if (rp < 0)
                    rp=len;
            }
        }
    }
}

/*
 *  Center      Center the given string in place using the supplied
 *              line length.
 */
center(str, len)
char *str;
int len;
{
    len = (len - lenCal(str)) / 2;
    addSpace(str, 0, len);
}

/*
 *  addSpace    Add <count> spaces at the given position.
 */
addSpace(str, pos, count)
char *str;
int pos;
int count;
{
    register j;

    if (count <= 0)
        return;

    for ( j=strlen(str)+count; j>pos; j--)
        str[j] = str[j-count];
    for (j=0; j<count; j++)
        str[pos++]=' ';
}

/*
 *  dotComm     Process a command line
 */
dotComm(buff)
char *buff;
{
    char scratch[3];
    register j;
    int action = ERROR;
    int relative, value;

    for(j=1; j<3; j++)
        scratch[j-1] = buff[j];
    scratch[2] = '\0';
    for(j=0; j <= COMCOUNT; j++) {
        if (strcmp(commands[j], scratch) == 0) {
            action = j;
            break;
        }
    }
    if (action == ERROR)
        return;

    if (buff[4] == '+' || buff[4] == '-') {
        relative = TRUE;
        j=atoi(&buff[5]);
        if (buff[4] == '-')
            j *= -1;
    }
    else {
        relative = FALSE;
        j = atoi(&buff[4]);
    }

    switch (action) {

        case COMMENT:
            break;

        case INDENT:
            if (relative)
                value = in+j;
            else
                value = j;
            if ( !(value <0 || value >rm)) {
                pFlush(FALSE);
                in = value;
            }
            break;           
                
        case TI:
            if (relative)
                ti = j;
            else
                ti = in - j;
            pFlush(FALSE);
            break;           

        case PAGEOFF:
            if ( !(hardCopy) )
                break;
            if (relative)
                value = po+j;
            else
                value = j;
            if ( !(value<0 || value > 100)) {
                pFlush(FALSE);
                po = value;
            }
            break;

        case CENTER:
            pFlush(FALSE);
            strip(&buff[3]);
            ce = j;
            if (j == 0 && (buff[3] == '\0') )
                ce = 1;
            break;
        
        case JUSTIFY:
            pFlush(FALSE);
            strip(&buff[3]);
            trim( &buff[3]);
            if (strcmp( &buff[3], "on") )
                ju = FALSE;
            else
                ju = TRUE;
            break;

        case RM:
            if (relative)
                value = rm+j;
            else
                value = j;
            if ( !(value<10 || value >100) ) {
                pFlush(FALSE);
                rm=value;
            }
            break;

        case BR:
            pFlush(FALSE);
            break;

        case AS:
            if (addFlag)
                break;
            pFlush(FALSE);
            saveFi = fi;
            saveJu = ju;
            fi = FALSE;
            ju = FALSE;
            if ( (fdAdd = fopen("address", "w")) == NULL)
                erExit("Could not open address file.");
            addFlag = TRUE;
            break;

        case AE:
            if (addFlag != TRUE)
                erExit("Address redirection not in effect!");
            fclose(fdAdd);
            pFlush(FALSE);
            fi = saveFi;
            ju = saveJu;
            addFlag = FALSE;
            break;

        case FILL:
            pFlush(FALSE);
            strip(&buff[3]);
            trim( &buff[3]);
            if (strcmp( &buff[3], "on") )
                fi = FALSE;
            else
                fi = TRUE;
            break;

        case SP:
            pFlush(FALSE);
            value=0;
            do {
                pCount=1;
                pFlush(FALSE);
            } while (++value < j);
            break;

        case DT:
            pFlush(FALSE);
            setDate(TRUE);
            pFlush(FALSE);
            break;

        case SG:
            pFlush(FALSE);
            strcpy(pLine, "Sincerely yours,");
            pCount=1;
            pFlush(FALSE);
            pCount=1;
            pFlush(FALSE);
            pCount=1;
            pFlush(FALSE);
            pCount=1;
            pFlush(FALSE);
            trim(&buff[4]);
            strip(&buff[4]);
            strcpy(pLine, &buff[4]);
            pCount=1;
            pFlush(FALSE);
            break;
        
        default:
            break;
    }
}

/*
 *  setDate     Fill in the current system date in pLine.
 *              The DeSmet C function dates() is used here to snag
 *              the system date; other compilers will need other functions.
 */
setDate(right)
int right;
{
    int month, day, year;
    struct date db;
    char dt[25];
    register j;
    static char *mTable[] = {"January", "February", "March",
                             "April", "May", "June",
                             "July", "August", "September",
                             "October", "November", "December"};
    getdate(&db);
    month = db.da_mon;
    day = db.da_day;
    year = db.da_year;
    sprintf(dt, "%s %d, %d", mTable[month-1], day, year);
    if (right) {
        for(j=0; j<rm; j++)
            pLine[j] = ' ';
        strcpy( &pLine[ rm - strlen(dt) ], dt);
    }
    else
        strcpy(pLine, dt);
    pCount = strlen(pLine);
}

/*
 *  lenCal      Calculate the length of a string disregarding boldface
 *              and underline command characters.
 */
lenCal(str)
char *str;
{
    register j, len;

    for(j=0, len=0; str[j] != '\0'; j++)
        if ( (str[j] != UC) && (str[j] != BC) )
            len++;

    return len;
}

/*
 *  lprint  Print the string; enable/disable necessary print controls
 */
lprint(str)
char *str;
{
    register j;

    /*
     *  some printers disable all print attributes at the beginning
     *  of each new line. To handle this, boldface and underline are
     *  reasserted at the beginning of each print line if they're
     *  currently enabled.
     */
#ifdef UL_ON
    if (ul)
        pstr(UL_ON);
#endif

#ifdef BF_ON
    if (bf)
        pstr(BF_ON);
#endif

    /*
     *  print the line
     */
    for(j=0; str[j]; j++) {

        switch (str[j]) {

            case UC:
                if (ul) {
#ifdef UL_ON
                    pstr(UL_OFF);
#endif
                    ul=FALSE;
                }
                else {
#ifdef UL_ON
                    pstr(UL_ON);
#endif
                    ul=TRUE;
                }
                break;

            case BC:
                if (bf) {
#ifdef BF_ON
                    pstr(BF_OFF);
#endif
                    bf = FALSE;
                }
                else {
#ifdef BF_ON
                    pstr(BF_ON);
#endif

                    bf = TRUE;
                }
                break;

            case HS:
                putc(' ', printer);
                break;
            
            default:
                if ( (str[j] > ' ') ) {
#ifndef BF_ON
                    if (bf) {
                        putc(str[j], printer);
                        putc(BACKSPACE, printer);
                    }
#endif
#ifndef UL_ON
                    if (ul) {
                        putc('_', printer);
                        putc(BACKSPACE, printer);
                    }
#endif
                }
                putc(str[j], printer);
                break;
        }
    }
}

/*
 *  pstr    Print the given string to the hardcopy device or not at all
 *          if we're hitting the console.
 */
pstr(str)
char *str;
{
    if ( !(hardCopy) )
        return;

    while (*str)
        putc(*str++, printer);
}
        
