/* wc.c:    word count */

#include <stdio.h>
#include <ctype.h>

#define yes 1
#define no 0
#define BUFSIZE 8192

char buf[BUFSIZE];

/* ..set default options.. */
int bytes = no,
    words = no,
    lines = no,
    files = no;

long tkc, tkl, tkw;

main(argc,argv)
int argc;
char *argv[];
{
    char *s;
    int fd;

    /* ..process switches.. */
    while (--argc > 0 && (**++argv == '-' || **argv == '/'))
    {
        for (s = *argv+1; *s; ++s)
            switch(tolower(*s))
            {
                case 'l':
                    lines = !lines;
                    break;
                case 'w':
                    words = !words;
                    break;
                case 'c':
                    bytes = !bytes;
                    break;
                case 'f':
                    files = !files;
                    break;
                default :
                    fprintf(stderr,"---> ERROR: bad option: -%c\n",*s);
                    exit();
            }
    }

    /* ..set all three options by default.. */
    if (!(bytes || words || lines))
        bytes = words = lines = yes;

    /* ..initialize totals.. */
    tkc = tkl = tkw = 0;

    /* ..process files.. */
    if (argc == 0)
        wc(0,"");
    else
    {
        /* ..check file switch.. */
        if (argc > 1)
            files = !files;

        for (; *argv; ++argv)
            if ((fd = open(*argv,0)) >= 0)
            {
                wc(fd,*argv);
                close(fd);
            }
            else
                fprintf(stderr,"---> ERROR: can't open %s\n",*argv);
    }

    /* ..print totals.. */
    if (files)
    {
        if (bytes)
            printf("-------- ");
        if (words) 
            printf("------- ");
        if (lines) 
            printf("------ ");
        putchar('\n');
        if (bytes)
            printf("%8D ",tkc);
        if (words) 
            printf("%7D ",tkw);
        if (lines) 
            printf("%6D ",tkl);
        putchar('\n');
    }
}

wc(fd,f)
int fd;
char *f;
{
    long kc, kl, kw;
    int inword, count;
    register i;

    kc = kw = kl = 0L;
    inword = no;

    while ((count = read(fd,buf,BUFSIZE)) > 0)
    {
        kc += count;
        inword = no;
        for (i = 0; i < count; ++i)
            if (isspace(buf[i]))
            {
                inword = no;
                if (buf[i] == '\n')
                    ++kl;
            }
            else if (!inword)
            {
                inword = yes;
                kw++;
            }
    }
    if (bytes)
        printf("%8D ",kc);
    if (words) 
        printf("%7D ",kw);
    if (lines) 
        printf("%6D ",kl);
    if (files) 
        puts(f);
    else
        putchar('\n');

    /* ..accumulate totals.. */
    tkc += kc;
    tkw += kw;
    tkl += kl;
}
      inword = no;
                if (buf[i] == '\n')
                    ++kl;
            }
            else i