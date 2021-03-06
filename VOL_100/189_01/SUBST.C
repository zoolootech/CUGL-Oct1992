/* change.c:    replace strings in text files */

#include <makearg.h>

#define MAXLINE 256
#define MAXFILES 512
int count = 0,
    sensitive = 1;

main()
{
    char from[MAXLINE], to[MAXLINE];
    
    makeargv();     /* ..Must be first - builds args.. */

    if (argv[1][0] == '-' || argv[1][0] == '/')
        if (tolower(argv[1][1]) == 'i')
        {
            sensitive = !sensitive;
            --argc;
            ++argv;
        }
        else
        {
            fprintf(stderr,"invalid option: %c\n",argv[1][1]);
            exit(1);
        }
    
    if (argc < 3)
    {
        fprintf(stderr,"usage:  subst [-i] from to [file1 file2 ...]\n");
        exit(1);
    }

    strcpy(from,argv[1]);
    strcpy(to,argv[2]);
    argc -= 2;
    argv += 2;
    
    if (argc == 1)
        change(from,to);
    else
    {
        /* ..process each file.. */
        while (*++argv)
            if (freopen(*argv,"r",stdin) != NULL)
                change(from,to);
            else
                fprintf(stderr,"can't open: %s\n",*argv);
    }
    fprintf(stderr,"\n%d substitutions.\n",count);
}

change(from,to)
char *from, *to;
{
    char s[MAXLINE], *subst();
    
    while (gets(s) != NULL)
        puts(subst(s,from,to));
}

char *subst(s,from,to)
char *s, *from ,*to;
{
    int delta;
    register char *p, *ptr;
    char *q, *match();

    for (p = s; p = match(p,from); ++count)
    {
        q = p + strlen(from);
        delta = strlen(from) - strlen(to);

        if (delta < 0)
            for (ptr = p + strlen(p); ptr >= q; --ptr)
                *(ptr - delta) = *ptr;
        else if (delta > 0)
        {
            ptr = q;
            do
                *(ptr - delta) = *ptr;
            while (*ptr++);
        }

        ptr = to;
        while (*ptr)
            *p++ = *ptr++;
    }
    return s;
}

char *match(s,pat)
char *s, *pat;
{
    register char *p;

    for (p = s; *p; ++p)
        if (compare(p,pat,strlen(pat)) == 0)
            return p;
    return NULL;
}

int compare(s,t,n)
char *s, *t;
int n;
{
    char s1[MAXLINE], t1[MAXLINE];

    if (sensitive)
        return strncmp(s,t,n);
    else
        return strncmp(strlower(s1,s),strlower(t1,t),n);
}
       PE~y d$  SUBST   C             E~ 	  PAGE    EXE           B�}U 0  READ    ME            �>�$ �  