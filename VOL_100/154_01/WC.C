/* wc.c:    word count */

/* From Kernighan/Ritchie C, by Chuck Allison */

#include <stdio.h>
#include <ctype.h>

#define yes 1
#define no 0
#define MAXFILES 150

int bytes = no, words = no, lines = no, files = no;

main(argc,argv)
int argc;
char *argv[];
{
    FILE *f;
    char *s, *xargv[MAXFILES];
    int dash = no, maxarg = MAXFILES, xargc, i;

	/* ..process switches.. */
	for ( ; *(s = *(argv+1)) == '-'; ++argv, --argc)
	{
		dash = yes;
		while (*++s)
			switch(tolower(*s)) {
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
					fprintf(stderr,"unknown switch: -%c\n",*s);
					exit(1);
			}
	}

	if (dash == no) bytes = words = lines = yes;

	/* ..process files.. */
	if (argc == 1)
		wc(stdin,"");
	else
	{
		/* ..expand filespecs (Mark Williams C only!).. */
		xargc = exargs("",argc,argv,xargv,maxarg);

		/* ..check file switch.. */
		if (xargc > 1) files = !files;

		for (i = 0; i < xargc; ++i)
			if ((f = fopen(xargv[i],"r")) != NULL)
			{
				wc(f,xargv[i]);
				fclose(f);
			}
			else
				fprintf(stderr,"can't open: %s\n",xargv[i]);
	}
}

wc(fp,s)
FILE *fp;
char *s;
{
    long c, kc, kl, kw;
	int inword;

	kc = kw = kl = 0L;
	inword = no;

    while ((c = fgetc(fp)) != EOF)
	{
        kc++;
        if (c == '\n')
            kl++;
        if (isspace(c))
            inword = no;
        else if (!inword)
		{
            inword = yes;
            kw++;
        }
    }
    fclose(fp);
	if (files) printf("%-20s :   ",s);
    if (bytes) printf("%8D ",kc);
    if (words) printf("%7D ",kw);
    if (lines) printf("%6D ",kl);
    putchar('\n');
}
