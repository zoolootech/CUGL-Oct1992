/* merge.c:        merge input files to standard output
 *
 *         usage:  merge file1 file2 [file3 ...]
 *
 *              ..a file argument of "-" refers to standard input
 */

#include <stdio.h>
#define MAXLINE 256

main(argc,argv)
int argc;
char *argv[];
{
    FILE *f, *inf, *outf, *mustopen();
    register i;
    static char line[MAXLINE], *t, *inp = "temp1.$$$", *outp = "temp2.$$$";

    /* ..process files.. */
    if (argc < 2)
    {
        fprintf(stderr,"usage: merge f1 f2 [f3 ...]\n");
        exit(1);
    }

    /* ..Set up first file as current input; outp as current output.. */
    inf = (argv[1][0] == '-') ? stdin : mustopen(argv[1],"r");
    outf = mustopen(outp,"w");

    for (i = 2; i < argc; ++i)
    {
        f = (argv[i][0] == '-') ? stdin : mustopen(argv[i],"r");
        fprintf(stderr,"merging %s...\n",
          (argv[i][0] == '-') ? "stdin" : argv[i]);
        merge(f,inf,outf); 
        fclose(f); fclose(inf); fclose(outf);
        t = inp; inp = outp; outp = t;  /* ..swap filenames.. */
        inf = mustopen(inp,"r");
        outf = mustopen(outp,"w");
    }
    fclose(inf); fclose(outf);

    /* ..answer is in inp.. */
    f = mustopen(inp,"r");
    while (fgets(line,MAXLINE-1,f))
        fputs(line,stdout);
    fclose(f);
    unlink(inp); unlink(outp);
}

FILE *mustopen(s,mode)
char *s, *mode;
{
    FILE *f;

    /* ..open file or die.. */
    if ((f = fopen(s,mode)) == NULL)
    {
        fprintf(stderr,"---> ERROR: can't open %s\n",s);
        exit(1);
    }

    return(f);
}

merge(f1,f2,outf)
FILE *f1, *f2, *outf;
{
    char s1[MAXLINE], s2[MAXLINE];

    fgets(s1,MAXLINE-1,f1);
    fgets(s2,MAXLINE-1,f2);

    /* ..merge while both open.. */
    while (!feof(f1) && !feof(f2))
        if (strcmp(s1,s2) < 0)
        {
            fputs(s1,outf);
            fgets(s1,MAXLINE-1,f1);
        }
        else
        {
            fputs(s2,outf);
            fgets(s2,MAXLINE-1,f2);
        }

    /* ..dump remaining file.. */
    while (!feof(f1))
    {
        fputs(s1,outf);
        fgets(s1,MAXLINE-1,f1);
    }
    while (!feof(f2))
    {
        fputs(s2,outf);
        fgets(s2,MAXLINE-1,f2);
    }
}
while both open.. */
    while 