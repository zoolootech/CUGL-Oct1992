/*
 *   pr.c:  detabbing file printer -
 *
 *        usage:  pr [options] [file1 file2 ...]
 *        options:
 *               -b#     begin printing with page #
 *               -c#     print # copies
 *               -d      delete file after printing
 *               -e#     end printing with page #
 *               -hs     use title s instead of filename (NULL = no header)
 *               -l      print with line numbers
 *               -m#     print with a left margin of # spaces
 *               -n#     print # lines per page (header xtra)
 *               -s#     print with spacing of # lines
 *               -t#     set tabs every # spaces (default 4)
 *               -w#     define page width (default 256)
 *
 *       Other than tabs, all control characters are suppressed.
 *
 *       Adapted from Software Tools by Chuck Allison - May 1985
 */

#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <types.h>  /* MWC only */

#define inrange(x) (start <= x && x <= stop)
#define getnum(x) x = atoi(s+1); goto next_arg

#define MAXINT 32767
#define MAXLINE 256
#define yes 1
#define no 0

/* ..set default parameters.. */
int number = no,
    delete = no,
    print_lines_pp = 55,
    text_lines_pp = 55,
    margin = 3,
    spacing = 1,
    tabspace = 4,
    width = 256,
    headers = yes,
    copies = 1,
    start = 1,
    stop = MAXINT;

static tabstops[MAXLINE];
time_t time_val;
static char timestr[25], title[36] = "";
extern char *trim_fspec();

main(argc,argv)
int argc;
char *argv[];
{
    int hour;
    register char *s;

    /* ..process switches.. */
    while (--argc && (**++argv == '-'|| **argv == '/'))
    {
        for (s = *argv+1; *s; ++s)
            switch(tolower(*s))
            {
                case 'b':
                    getnum(start);
                case 'd':
                    delete = yes;
                    break;
                case 'e':
                    getnum(stop);
                case 'h':
                    if (s[1])
                    {
                        strcpy(title,++s);
                        goto next_arg;
                    }
                    else
                    {
                        headers = no;
                        break;
                    }
                case 'l':
                    number = yes;
                    break;
                case 'm':
                    getnum(margin);
                case 'n':
                    getnum(print_lines_pp);
                case 's':
                    getnum(spacing);
                case 't':
                    getnum(tabspace);
                case 'c':
                    getnum(copies);
                case 'w':
                    getnum(width);
                default :
                    fprintf(stderr,"---> ERROR: bad switch: -%c\n",*s);
                    exit(1);
            }
        next_arg: /* ..cycle on outer for().. */ ;
    }

    /* ..initialize tab settings.. */
    settabs();

    /* ..validate line spacing.. */
    if (spacing <= 0 || print_lines_pp <= spacing)
    {
        fprintf(stderr,"---> ERROR: Spacing invalid\n");
        exit(1);
    }
    text_lines_pp = 1 + (print_lines_pp - 1) / spacing;
    
    /* ..get date and time.. */
    time(&time_val);
    strcpy(timestr,ctime(&time_val));
    timestr[24] = '\0';   /* ..zap newline.. */
    /* ..Replace seconds with am/pm.. */
    timestr[16] = ' ';
    hour = 10*(timestr[11] - '0') + (timestr[12] - '0');
    if (hour >= 12)
    {
        timestr[17] = 'p';
        if (hour > 12)
        {
            hour -= 12;
            timestr[12] = hour%10 + '0';
            timestr[11] = (hour /= 10) ? hour + '0' : ' ';
        }
   }
    else
    {
        timestr[17] = 'a';
        if (hour == 0)
        {
            timestr[11] = '1';
            timestr[12] = '2';
        }
        else if (timestr[11] == '0')
            timestr[11] = ' ';
    }
    timestr[18] = 'm';

    /* ..process files.. */
    if (argc == 0)
        process(title);
    else
    {
        /* ..print each file.. */
        for (; *argv; ++argv)
            if (freopen(*argv,"r",stdin) != NULL)
            {
                process((strlen(title) == 0) ? *argv : title);
                if (delete)
                    if (unlink(*argv) != 0)
                        fprintf(stderr,"---> WARNING: can't delete %s\n",*argv);
                    else
                        fprintf(stderr,"---> %s deleted.\n",*argv);
            }
            else
                fprintf(stderr,"---> ERROR: can't process %s\n",*argv);
    }
}

process(f)
char *f;
{
    register i;

    for (i = 0; i < copies; ++i)
    {
         rewind(stdin);
         pr(f);
    }
}

pr(name)
char *name;
{
    register lineno, i;
    int pageno, offset;
    char line[MAXLINE];
    extern char *rindex();
    
    offset = 0;
    pageno = 1;
    lineno = 1;
    
    fprintf(stderr,"---> printing %s...\n",name);

    /* ..trim a long file name.. */
    trim_fspec(name,name,30);

    /* ..output initial header.. */
    if (inrange(pageno))
        if (headers)
            header(name,pageno);
        else
            printf("\n\n\n\n");

    while (fgets(line,MAXLINE-1,stdin) != NULL)
    {
        /* ..check page range.. */
        if (pageno > stop)
            return;
            
        if (lineno == 0)
        {
            lineno = 1;
            if (inrange(pageno))
                if (headers)
                    header(name,pageno);
                else
                    printf("\n\n\n\n");
        }

        if (pageno > stop)
            return;

        /* ..print line, with number, if requested.. */
        if (inrange(pageno))
        {
            do_margin();
            if (number)
                printf("%5d  ",offset+lineno);
            else
                printf("  ");
            detab(line);
            if (lineno+1 <= text_lines_pp)
                for (i = 1; i < spacing; ++i)
                    putchar('\n');
        }

        /* ..check for page break.. */
        if (++lineno > text_lines_pp)
        {
            if (inrange(pageno))
                putchar('\f');
            offset += text_lines_pp;
            ++pageno;
            lineno = 0;
        }
    }

    /* ..form-feed after last partial page.. */
    if (inrange(pageno))
        if (lineno > 0)
            putchar('\f');
}

header(file,page)
char *file;
int page;
{
    register i;

    putchar('\n');
    do_margin();
    for (i = 0; i < 75; ++i)
        putchar('-');
    putchar('\n');
    do_margin();
    printf("%-36.36s%-30.30sPage%5d",file,timestr,page);
    putchar('\n');
    do_margin();
    for (i = 0; i < 75; ++i)
        putchar('-');
    printf("\n\n\n");
}

detab(line)
char *line;
{
    register i, col;

    col = 0;

    /* ..note: line[] has a terminating '\n' per fgets().. */
    for (i = 0; col < width-margin-(number?7:2) && i < strlen(line); ++i)
        if (line[i] == '\t')
            do
            {
                /* ..tab.. */
                putchar(' ');
                ++col;
            } while (!tabstops[col]);
        else if (line[i] == '\n' || isprint(line[i]))
        {
            putchar(line[i]);
            ++col;
        }
    if (i < strlen(line))
        putchar('\n');
}

settabs()
{
    register i;

    tabstops[0] = 0;
    for (i = 1; i < MAXLINE; ++i)
        tabstops[i] = (i % tabspace == 0);
}

do_margin()
{
    register i;
    
    for (i = 0; i < margin; ++i)
        putchar(' ');
}
          putchar(' ');
                ++col;
            } while (!tabstops[col]);
        else if (line[