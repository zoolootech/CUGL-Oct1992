/* darken.c:  overstrike text lines for darker output -
 *
 *
 *   (c) Chuck Allison, 1985
 *
 */

#include <stdio.h>
#include <ctype.h>

#define MAXLINE 256

int strikes = 3;              /* ..default over-print count.. */

main(argc,argv)
int argc;
char *argv[];
{
    register i;

    /* ..process switches.. */
    if (argv[1][0] == '-' || argv[1][0] == '/')
    {
        strikes = atoi(argv[1]+1);
        ++argv;
        --argc;
    }

    /* ..process files.. */
    if (argc == 1)
        dk();
    else
    {
        /* ..print each file.. */
        for (i = 1; i < argc; ++i)
            if (freopen(argv[i],"r",stdin) != NULL)
                dk();
            else
                fprintf(stderr,"---> ERROR: can't open %s\n",argv[i]);
    }
}

dk()
{
    register i;
    char *line, *index();

    line = (char *) malloc(MAXLINE);

    while (fgets(line,MAXLINE-1,stdin) != NULL)

        /* ..check for form-feed in line.. */
        if (index(line,'\f') != NULL)

            /* ..output character-at-a-time.. */
            for (; *line; ++line)
                if (iscntrl(*line) || isspace(*line))
                    /* ..don't repeat special characters.. */
                    putchar(*line);
                else
                {
                    for (i = 1; i < strikes; ++i)
                    {
                        putchar(*line);
                        putchar('\b');
                    }
                    putchar(*line);
                }

        else if (strlen(line) == 0)
            /* ..output blank line.. */
            putchar('\n');

        else
        {
            /* ..output line-at-a-time.. */
            line[strlen(line)-1] = '\0';    /* zap \n */

            for (i = 0; i < strikes; ++i)
                printf("%s\r",line);

            putchar('\n');
        }

    free(line);
}
ine);
                        putchar('\b');
                    }
                    putchar(*line);
     