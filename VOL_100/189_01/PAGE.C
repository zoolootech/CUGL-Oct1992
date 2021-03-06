/* page.c:	paginates text to screen */

#include <stdio.h>
#include <ctype.h>
#include <xdir.h>

#define SCR_SIZ 25
#define MAXLINE 256
#define TABSPACE 8
#define MAXFILES 150
#define cls() fputs("\033[2J\033[H",stderr)
#define clear_line() fputs("\033[2K",stderr)
#define setcur(row,col) fprintf(stderr,"\033[%d;%dH",row,col)
#define bold() fputs("\033[1m",stderr)
#define reset() fputs("\033[0m",stderr)
#define min(x,y) (x < y) ? x : y

extern char *trim_fspec();
int tabstops[MAXLINE];
char prompt();
FILE *screen;

main(argc,argv)
int argc;
char *argv[];
{
    int rc, nfiles;
    register i;
    char temp[MAXLINE], trim_temp[MAXLINE];
    struct file_info *xfiles[MAXFILES];

    settabs();
    screen = fopen("con","w");

    if (argc == 1)
    {
        page((struct file_info *) NULL);
        exit();
    }

    /* ..expand wildcard filespecs.. */
    nfiles = xdir(argc,argv,xfiles,MAXFILES,NORMAL);

    for (i = 0; i < nfiles; ++i)
        if (freopen(xfiles[i]->name,"r",stdin) != NULL)
        {
            rc = page(xfiles[i]);

            if (i < nfiles-1 && rc == EOF)
	    {
		strcpy(temp,xfiles[i]->name);
	        strcat(temp,"  (Next file: ");
		strcat(temp,trim_fspec(trim_temp,xfiles[i+1]->name,30));
		strcat(temp,")");
		prompt(temp,0);
	    }
        }
        else if (i < nfiles-1)
        {
            fprintf(screen,"can't open %s\nHit any key...",xfiles[i]->name);
            fflush(screen);
            getcnb();
        }
}

int page(f)
struct file_info *f;
{
    register count;
    long bytes_read, file_size;
    char line[MAXLINE], temp[MAXLINE], fname[MAXLINE];

    cls();
    if (f)
    {
        file_size = f->size;
        trim_fspec(fname,f->name,30);
    }
    else
        strcpy(fname,"(stdin)");

    bytes_read = 0;

    for (count = 1; fgets(temp,MAXLINE-1,stdin) != NULL; ++count)
    {
        bytes_read += strlen(temp) + 1;
        line[strlen(temp)-1] = '\0';	/* ..zap newline.. */
	detab(line,temp);
        fprintf(screen,"%-.*s\n",min(79,strlen(line)),line);
        fflush(screen);
        if (count % (SCR_SIZ-1) == 0)
        {
            switch(prompt(fname,(int) ((float) bytes_read/file_size * 100)))
            {
                case '\015':
                    --count;
		    break;
                case 'n':
                    return 0;
            }
        }
    }
    return EOF;
}

int prompt(fname,per_cent_read)
char *fname;
int per_cent_read;
{
    int c;

    setcur(SCR_SIZ,1);
    bold();
    fprintf(screen,"%s",fname);
    if (per_cent_read > 0)
        fprintf(screen," (%d%%)",per_cent_read);
    fputs(" ...\r",screen);
    fflush(screen);
    reset();
    c = tolower(getcnb());
    clear_line();
    if (c == 'q' || c == '\003')
	exit();
    return c;
}
    
detab(t,s)
char *s, *t;
{
    register i, j;

    for (i = 0, j = 0; j < MAXLINE-1 && i < strlen(s)-1; ++i)
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
E    C             �t�0�  FTRIM   C             ڐ} V  