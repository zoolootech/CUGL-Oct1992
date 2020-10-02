/* b.c:    File Browse Facility */


/* NOTE: The function "getcnb()" is a MWC-supplied function that
         gets an unbuffered keypress from the standard input device.
         Scan codes (keypad, etc.) are not interpreted.
*/

/* (c) Chuck Allison, 1986. */

#include <stdio.h>
#include <ctype.h>
#include <setjmp.h>

#define MAXLINE 1024
#define MAXSIZE 5000
/* SCRSIZ = number of screen rows - 2 */
#define SCRSIZ 23
#define OVRLP 3
#define TABSPACE 8
#define HORIZ_SCROLL 20
#define TRIMLEN 30

int find(), prompt(), match(), browse(), n, screen_width=79,
    cur_line, cur_col, tabstops[MAXLINE];
char get_cmd(), *lines[MAXSIZE], fname[TRIMLEN];
char *getstr();
extern char *trim_fspec();
jmp_buf env;

main(argc,argv)
int argc;
char *argv[];
{
    register i, j;

    clrscr();
    settabs();

    if (argc == 1)
    {
        fputs("usage:  browse file1 [file2 ...]",stderr);
        exit(1);
    }

    /* ..Comment-out the following line if your compiler won't like it.. */
    freopen("con","w",stdout);

    i = 1;
    while (0 < i && i < argc)
        if (freopen(argv[i],"r",stdin) != NULL)
            i += browse(argv[i]);
        else
        {
            /* ..file not found.. */
            clrscr();
            printf("%s not available.\n",argv[i]);
            fputs("Hit any key...",stdout);
            fflush(stdout);
            getcnb();
            /* ..remove filename from list.. */
            for (j = i; j < argc-1; ++j)
                argv[j] = argv[j+1];
            --argc;
        }

    clrscr();
}   
            
int browse(file)
char *file;
{
    int factor, find_line, dir, column;
    register i;
    static char s[MAXLINE], t[MAXLINE];

    /* ..Read file.. */
    clrscr();
    setcur(SCRSIZ+1,1);
    rvs(); bold();
    printf("Reading %s ...",file); reset();
    putchar(' '); column = 14+strlen(file);
    for (n = 0; n < MAXSIZE-1 && fgets(s,MAXLINE-1,stdin) != NULL; ++n)
    {
        int sl;
        sl = strlen(s);
        if (s[sl-1] == '\n')
            s[sl-1] = '\0';        /* zap newline */
        detab(t,s);
        trim(t);
        lines[n] = (char *) malloc(strlen(t)+1);
        if (lines[n] == NULL)
        {
            fputs("\nFile truncated.\n\nHit any key...",stderr);
            getcnb();
            break;
        }
        strcpy(lines[n],t);
        setcur(SCRSIZ+1,column); printf("%4d",n);
        fflush(stdout);
    }

    if (n == 0)
    {
        fputs("\nFile is empty.\n\nHit any key... ",stdout);
        fflush(stdout);
        getcnb();
        return 1;
    }

    trim_fspec(fname,file,TRIMLEN-1);
    strcpy(s,"");
    cur_line = 0;
    cur_col = 0;
    setjmp(env);
    paint();
    
    /* ..main loop: repeat forever.. */
    for (;;)
        switch(tolower(get_cmd(&factor)))
        {
            case 'u':
                /* ..window up.. */
                cur_line = max(0,cur_line-factor*SCRSIZ+OVRLP);
                if (cur_line > n-1)
                   cur_line = 0;
                paint();
                break;
            case 'd':
                /* ..window down.. */
                cur_line = min(n-1,cur_line+factor*SCRSIZ-OVRLP);
                if (cur_line < 0)
                    cur_line = n-1;
                paint();
                break;
            case '\015':
            case '\004':
                /* ..window down one line.. */
                if (cur_line+SCRSIZ < n)
                {
                    ++cur_line;
                    setcur(SCRSIZ+2,1);
                    if (cur_col < strlen(lines[cur_line+SCRSIZ-1]))
                         printf("%-.*s\n",screen_width,
                                 lines[cur_line+SCRSIZ-1]+cur_col);
                    else
                        putchar('\n');
                    home();
                    clear_line();
                    rvs(); bold();
                    printf("\"%s\" Lines: %d-%d  Cols: %d-%d",
                      fname,cur_line+1,min(n,cur_line+SCRSIZ),cur_col+1,
                      cur_col+screen_width);
                    reset();
                    setcur(SCRSIZ+2,1);
                }
                else
                    beep();
                fflush(stdout);
                break; 
            case '\033':
            case '\025':
                /* ..window up one line.. */
                if (cur_line > 0)
                {
                    --cur_line;
                    paint();
                }
                else
                    beep();
                fflush(stdout);
                break;
            case 't':
                /* ..goto to top-of-file.. */
                cur_line = 0;
                paint();
                break;
            case 'b':
                /* ..goto end-of-file.. */
                cur_line = max(0,n-SCRSIZ+1);              
                paint();
                break;
            case 'r':
                /* ..scroll right.. */
                cur_col += factor*HORIZ_SCROLL;
                if (cur_col < 0)
                    cur_col = MAXLINE-screen_width;    /* overflow check */
                cur_col = min(cur_col,MAXLINE-screen_width);
                paint();
                break;
            case 'l':
                /* ..scroll left.. */
                cur_col -= factor*HORIZ_SCROLL;
                if (cur_col > MAXLINE-screen_width)
                    cur_col = 0;        /* underflow check */
                cur_col = max(cur_col,0);
                paint();
                break;
            case 'g':
                /* ..goto line number.. */
                if (factor >= 1 && factor <= n)
                {
                    cur_line = factor-1;
                    paint();
                }
                else
                    beep();
                fflush(stdout);
                break;
            case 'f':
                /* ..Find a string.. */
                dir = prompt(s);
                setcur(SCRSIZ+2,(screen_width-12)/2);
                bold(); blink();
                fputs("Searching...",stdout);
                reset();
                fflush(stdout);
                find_line = max(0,cur_line+dir);
                find_line = min(n-1,cur_line+dir);
                for (i = 1; i <= factor; ++i, find_line += dir)
                    if (!find(s,&find_line,dir))
                    {
                        beep();
                        fflush(stdout);
                        goto nofind;
                    }
                cur_line = max(0,find_line-dir);
                cur_line = min(n-1,find_line-dir);
nofind:         paint();
                break;
            case 's':
            case '\006':
                /* ..Step-search.. */
                dir = prompt(s);
                do
                {
                    setcur(SCRSIZ+2,(screen_width-12)/2);
                    bold(); blink();
                    fputs("Searching...",stdout);
                    reset();
                    fflush(stdout);
                    find_line = max(0,cur_line+dir);
                    find_line = min(n-1,cur_line+dir);
                    if (!find(s,&find_line,dir))
                    {
                        beep();
                        fflush(stdout);
                        break;
                    }
                    cur_line = max(0,find_line);
                    cur_line = min(n-1,find_line);
                    paint();
                    clear_line();
                    rvs(); bold();
                    fputs("Continue",stdout);
                    reset();
                    fputs(" (Y/N)? ",stdout);
                    fflush(stdout);
                } while(tolower(getcnb()) == 'y');
                setcur(SCRSIZ+2,1);
                clear_line(); home(); clear_line(); rvs(); bold();
                printf("\"%s\" Lines: %d-%d  Cols: %d-%d",
                  fname,cur_line+1,min(n,cur_line+SCRSIZ),cur_col+1,
                  cur_col+screen_width);
                setcur(SCRSIZ+2,1); reset();
                fflush(stdout);
                break;
            case 'h':
                help();
                paint();
                break;
            case 'n':
            case '\032':
                /* ..next file.. */
                clrscr();
                if (factor != 0)
                {
                    release();
                    return factor;
                }
            case 'p':
                /* ..previous file.. */
                clrscr();
                if (factor != 0)
                {
                    release();
                    return -factor;
                }
            case 'q':
            case 'x':
                /* ..quit.. */
                clrscr();
                exit();
            default:
                beep();
                fflush(stdout);
        }
}

char get_cmd(k)
int *k;
{
    char c;
    
    /* ..get command with optional count prefix.. */
    for (*k = 0; isdigit(c = getcnb()); *k = 10*(*k) + c-'0')
        ;
    if (*k == 0)
        *k = 1;
    return c;
}

int prompt(s)
char *s;
{
    char temp[MAXLINE], c;
    
    /* ..Get search-string.. */
    home(); clear_line(); rvs(); bold();
    printf("Find:");
    reset();
    printf(": ");
    fflush(stdout);
    if (getstr(temp,50) != NULL)
        strcpy(s,temp);
    else
        fputs(s,stdout);
    setcur(1,59); rvs(); bold(); 
    fputs("F(orward)/B(ackward)",stdout);
    reset(); 
    fputs(": ",stdout);
    fflush(stdout);
    c = tolower(getcnb());
    if (iscntrl(c))
        longjmp(env,0);
    return (c == 'b') ? -1 : 1;
}

paint()
{
    register i;
    
    /* ..Print status line.. */
    clrscr(); rvs(); bold();
    printf("\"%s\"  Lines: %d-%d  Cols: %d-%d\n",fname,
      cur_line+1,min(cur_line+SCRSIZ,n),cur_col+1,cur_col+screen_width);
    reset();

    /* ..Paint screen with text lines.. */
    for (i = cur_line; i < n && i < cur_line+SCRSIZ; ++i)
        if (cur_col < strlen(lines[i]))
            printf("%-.*s\n",min(strlen(lines[i]+cur_col),screen_width),
              lines[i]+cur_col);
        else
            putchar('\n');

    if (i == n)
    {
        rvs();
        fputs("*** End-of-File ***",stdout);
        reset();
    }
    setcur(SCRSIZ+2,1);
    fflush(stdout);
}

int find(s,start,dir)
char *s;
int *start, dir;
{
    register i;
    int found;
    
    /* ..find next occurrence of search-string.. */
    found = 0;
    for (i = *start; i < n && i >= 0; i += dir)
        if ((found = match(lines[i],s)) == 1)
            break;
    *start = i;
    return found;
}


int match(line,s)
register char *line;
char *s;
{
    register char *p;
    
    /* ..Check if search-string is in this line.. */
    for (p = s; *line && *p; ++line)
        if (tolower(*line) == tolower(*p))    /* ..Ignore case.. */
            ++p;
        else
        {
            line -= (p - s);
            p = s;
        }

    return (*p == '\0');
}

detab(t,s)
char *s, *t;
{
    register i, j;

    /* ..expand tabs; filter-out other control characters.. */
    for (i = 0, j = 0; j < MAXLINE-1 && i < strlen(s); ++i)
    if (s[i] == '\t')
        do
            t[j++] = ' ';
        while (!tabstops[j]);
    else if (!iscntrl(s[i]))
        t[j++] = s[i];

    t[j] = '\0';
}

settabs()
{
    register i;

    tabstops[0] = 0;
    for (i = 1; i < MAXLINE; ++i)
        tabstops[i] = (i%TABSPACE == 0);
}

trim(s)
char *s;
{
    register i;

    for (i = strlen(s)-1; i >= 0 && s[i] == ' '; --i)
        ;
    s[i+1] = '\0';
}

help()
{
    /* ..Print help screen.. */
    clrscr();
    fputs("\n                             ",stdout);
    bold(); rvs(); bold(); fputs("BROWSE UTILITY\n\n",stdout); reset();
    bold(); fputs("\t   Screen Down",stdout);
    reset(); fputs(" -        \t\t   D\n",stdout);
    bold();  fputs("\tScreen Up",stdout);
    reset(); fputs(" -          \t\t\U\n",stdout);
    bold();  fputs("\tScreen Right",stdout);
    reset(); fputs(" -       \t\tR\n",stdout);
    bold();  fputs("\tScreen Left",stdout);
    reset(); fputs(" -        \t\tL\n",stdout);
    bold();  fputs("\tLine Down",stdout);
    reset(); fputs(" -          \t\t^D, RETURN\n",stdout);
    bold();  fputs("\tLine Up",stdout);
    reset(); fputs(" -            \t\t^U, ESCAPE\n",stdout);
    bold();  fputs("\tBottom-of-File",stdout);
    reset(); fputs(" -     \t\tB\n",stdout);
    bold();  fputs("\tTop-of-File",stdout);
    reset(); fputs(" -        \t\tT\n",stdout);
    bold();  fputs("\tGoto Line #",stdout);
    reset(); fputs(" -        \t\t#G\n",stdout);
    bold();  fputs("\tSearch (Find)",stdout);
    reset(); fputs(" -      \t\tF\n",stdout);
    bold();  fputs("\tStep-Search",stdout);
    reset(); fputs(" -  \t\t\tS\n",stdout);
    bold();  fputs("\tHelp",stdout);
    reset(); fputs(" (this message) -\t\tH\n",stdout);
    bold();  fputs("\tNext-file",stdout);
    reset(); fputs(" -          \t\tN, ^Z\n",stdout);
    bold();  fputs("\tPrevious-file",stdout);
    reset(); fputs(" -      \t\tP\n",stdout);
    bold();  fputs("\tChoose another file",stdout);
    reset(); fputs(" -\t\t(Not available in this Version)\n",stdout);
    bold();  fputs("\tQuit",stdout);
    reset(); fputs(" -               \t\tQ, X\n",stdout);
    fputs("\nThe commands U, D, F, N, P, R, and L may be preceded ",stdout);
    fputs("by a repetition count.",stdout);
    setcur(SCRSIZ+2,1);
    bold(); fputs("...Hit any key to continue...",stdout); reset();
    fflush(stdout);
    getcnb();
}

min(x,y) 
int x, y;
{
    return (x <= y) ? x : y;
}

max(x,y)
int x, y;
{
    return (x >= y) ? x : y;
}


/* ..ANSI screen escape sequences.. */

setcur(row,col) 
int row, col;
{
    printf("\033[%d;%dH",row,col);
}

clrscr() 
{
    fputs("\033[2J",stdout);
}

beep() 
{
    putchar('\007');
}

home() 
{
    fputs("\033[H",stdout);
}

clear_line() 
{
    fputs("\033[2K",stdout);
}

clear_eol() 
{
    fputs("\033[K",stdout);
}

reset() 
{
    fputs("\033[0m",stdout);
}

bold() 
{
    fputs("\033[1m",stdout);
}

underl() 
{
    fputs("\033[4m",stdout);
}

blink() 
{
    fputs("\033[5m",stdout);
}

rvs() 
{
    fputs("\033[7m",stdout);
}


release()
{
    register i;

    for (i = 0; i < n; ++i)
        free(lines[i]);
}

char *getstr(s,maxstr)
char *s;
int maxstr;
{
    char c, *p;

    p = s;
    while (p-s < maxstr && (c = getcnb()) != '\r')
        if (!iscntrl(c))
        {
            *p++ = c;
            putchar(c);
            fflush(stdout);
        }
        else if (c == '\b')
            if (p > s)
            {
                --p;
                fputs("\b \b",stdout);
                fflush(stdout);
            }
            else
            {
                putchar('\007');
                fflush(stdout);
            }
        else
            longjmp(env,0);

    *p = '\0';
    return (p == s) ? NULL : s;
}    
        ≤õÅ M<  XDIR    H             ÄãH! ®  