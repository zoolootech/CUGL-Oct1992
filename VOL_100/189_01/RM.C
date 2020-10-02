/* rm.c: remove files
 *
 *   (c) Chuck Allison, 1985
 *
 */

#include <stdio.h>

#define NLOOK 22
#define MAXLINE 256
#define yes 1
#define no 0

FILE *screen;

main(argc,argv)
int argc;
char *argv[];
{
    register i;
    int go = no,
        delete = yes;
    unsigned k;

    screen = fopen("con","w");

    /* ..get optional "go" switch.. */
    if (argv[1][0] == '-' || argv[1][0] == '/')
    {
        if (tolower(argv[1][1]) == 'g')
            go = yes;
        else
        {
            fprintf(screen,"---> ERROR: bad option: -%c\n",argv[1][1]);
            fflush(screen);
        }
        argv++;
        argc--;
    }

    for (i = 1; i < argc; ++i)
    {
        if (!exist(argv[i]))
        {
            fprintf(screen,"---> %s not found\n",argv[i]);
            fflush(screen);
            continue;
        }
        delete = yes;

again:
        if (go == no)
        {
            fprintf(screen,"Delete \033[7m%s\033[0m (y,n,p,g,q)? ",argv[i]);
            fflush(screen);
            k = getcnb();
            switch (k)
            {
                case 'g':
                case 'G':
                    go = yes;
                case 'y':
                case 'Y':
                    delete = yes;
                    break;
                case 'n':
                case 'N':
                    delete = no;
                    putc('\n',screen);
                    break;
                case 'p':
                case 'P':
                    look(argv[i]);
                    goto again;
                case 'q':
                case 'Q':
                    exit(0);
                default :
                    putc('\n',screen);
                    goto again;
            }
        }
        if (delete)
            if (unlink(argv[i]) == 0)
                fprintf(screen,"%s deleted\n",argv[i]);
            else
                fprintf(screen,"---> %s NOT deleted -\n",argv[i]);
        fflush(screen);
    }
}

look(s)            /* ..review first page of file.. */
char *s;
{
    FILE *f;
    int i;
    char l[MAXLINE];

    putc('\n',screen);
    if ((f = fopen(s,"r")) == NULL) 
    {
        fprintf(screen,"---> ERROR: can't open %s\n",s);
        fflush(screen);
    }
    else
    {
        fputs("\033[2J",screen);
        for (i = 0; i < NLOOK && fgets(l,MAXLINE-1,f); ++i)
            fputs(l,screen);
        fclose(f);
        fputs("\n\033[7m-- hit any key --\033[0m",screen);
        fflush(screen);
        getcnb();
        fputs("\n\033[2J",screen);
        fflush(screen);
    }
}

int exist(s)
char *s;
{
    FILE *f;
    int found;

    if ((f = fopen(s,"r")) == NULL)
        found = no;
    else
    {
        found = yes;
        fclose(f);
    }

    return found;
}
[2J",screen);
        for (i = 0; i < NLOOK && fgets(l,MAXLINE-1,f);