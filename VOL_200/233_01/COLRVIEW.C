/*
Colrview.c - views file on screen and optionally sends to JX-80 printer.
 
02/19/85 by Ward Christensen

  TEST VERSION - not really ready for prime time...

Uses \x type imbeds in a file to select colors.  A single space is printed
every time a \ is encountered.  The character following the \ picks the
color or function:

	n	normal (black)
	r	red
	b	blue
	g	green
	y	yellow
	p	purple
	o	orange

	m	turn off colors from here on
*/

#include "stdio.h"
#define MAXLEN 160

    int printer,parm,mono,i;
    FILE *fi,*pr;

main(argc,argv)
int argc;
char *argv[];
{

    char c, line[MAXLEN], *p;

    printer=0; mono=0;
    bwhite();green();
    printf("Color");blue();printf("view");yellow();
    printf(" Version 1.0, 2/16/85 by Ward Christensen\n");
    black();

    if (argc==1)
    {   fputs("Usage: colrview [-options] filename\n",		stderr);
	fputs("Options: (m)ono, (c)ondensed, (p)rinter\n",	stderr);
	fputs("         (s)uperscript [to even ribbon wear]\n",	stderr);
	fputs("     ex: colrview -pc test.txt\n",		stderr);
	fputs("         (option p must precede option c)\n",	stderr);
	fputs("Color display & ANSI.SYS req'd.\n",		stderr);
	exit(1);
    }

    parm=1;
    if(argv[1][0]=='-')
    {   ++parm;    /* so file open gets filename */
	for(i=1;i<strlen(argv[1]);++i)
	{   switch(argv[1][i])
	    {   case 'm':	/* monochrome */
		    mono=1;
		    break;
		case 'p':	/* printer */
		    ++printer;
		    if((pr = fopen("lpt1:", "w")) == NULL) 
		    {   printf("\tCan't open printer\n");
			exit();
		    }
		    break;
		case 'c':	/* condensed */
		    if(printer) pr_setup();
		    break;
		case 's':	/* superscript */
		    lprintf("\033S1");
		    break;
		default:
		    printf("Unknown switch: %c\n",argv[2][i]);
		    exit(1);
	    }
	}
    }

    if((fi = fopen(argv[parm], "r")) == NULL) 
    {   printf("\tCan't open todo.txt in root dir\n");
	exit();
    }

    p = line;
    while(p != NULL)
    {   p = fgets(line,MAXLEN,fi);
	colorview(line);
    }
    fclose(fi);
    puts("\n{DONE}");
}


/* printer setup */

/*    \033S0	superscript mode
    \033A\006    12 lpi
    \033M	Elite on
    \017	condensed on
    \033N\002    skip-over-perf to 2
*/

pr_setup()
{   lprintf("\033S0\033A\006\033M\017\033N\002");
}

colorview(line)
char *line;
{   int i,len;
    char c;
    len=strlen(line);
    if(len==0)
    {   puts(" ");
	return(0);
    }
    for(i=0;i<len;++i)
    {   c=line[i];
	switch(c)
	{   case '\\':
		++i;
		putchar(' ');
		if(printer) lputc(' ');
		c=line[i];
		    if(c=='n') black();
		else    if(c=='r') red();
		else    if(c=='b') blue();
		else    if(c=='g') green();
		else    if(c=='p') magenta();
		else    if(c=='o') orange();
		else    if(c=='m') 
			{	mono=1;	/* turn off color	*/
				black();/* kill curr color	*/
			}
		break;
 	    case 0x8d:    /* hi-bit c/r */
	    case 0x0d:
	    case 0x0a:
		break;
	    default: putchar(c);
	    if(printer) lputc(c);
	}
    }
    printf("\n");
    if(printer) lprintf("\n");
}



lputc(c)
char c;
{   putc(c,pr);
}



lprintf(line)
char *line;
{   fputs(line,pr);
}

/*
colors.h: color screen subroutines - for ansi.sys
GLOBAL references: 
	printer		>0 if output is to go to the printer
	mono		>1 if printer is in monochrome mode.

*/


nor()		{ printf("\033[0m"); } /* note resets background to black */

high()		{ printf("\033[1m"); }

revvid()	{ printf("\033[7m"); }

black()	
{	printf("\033[30m");
	if(printer)
		lprintf("\033r0");
}
red()	
{	if(mono) 
		return(0);
	printf("\033[31m");
	if(printer)
		lprintf("\033r1");
}
green()	
{	if(mono) 
		return(0);
	printf("\033[32m");
	if(printer)
		lprintf("\033r6");
}
yellow()
{	if(mono) 
		return(0);
	printf("\033[33m");
	if(printer)
		lprintf("\033r4");
}
blue()	
{	if(mono) 
		return(0);
	printf("\033[34m");
	if(printer)
		lprintf("\033r2");
}
magenta()
{	if(mono) 
		return(0);
	printf("\033[35m");
	if(printer)
		lprintf("\033r3");
}
cyan()	
{	if(mono) 
		return(0);
	printf("\033[36m");
	if(printer)
		lprintf("\033r2");
}
white()	
{	if(mono) 
		return(0);
	printf("\033[37m");
	if(printer)
		lprintf("\033r0");
}

orange()
{	if(mono) 
		return(0);
	nor();bwhite();
	printf("\033[33m");
	if(printer)
		lprintf("\033r5");
}

bblack()	{ printf("\033[40m"); }
bred()		{ printf("\033[41m"); }
bgreen()	{ printf("\033[42m"); }
byellow()	{ printf("\033[43m"); }
bblue()		{ printf("\033[44m"); }
bmagenta()	{ printf("\033[45m"); }
bcyan()		{ printf("\033[46m"); }
bwhite()	{ printf("\033[47m"); }
