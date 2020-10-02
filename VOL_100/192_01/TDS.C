/*   (C) Copyright 1985, 1986 Neil V. Deasy                             
        Program Name: Terminal Digit Sorter
          Where Used: In hospital's Medical Records Department.  It speeds
			the task of pulling and re-filing records into
			terminal digit (non-numeric) order.
            Features: Program is driven by the key words Sort, Exit, Help.
			Anything else is checked upon input to meet the
			expected criteria of numbers of six digits length.
			Improper entry is rejected and an error message
			explaines why.  Record numbers are not sorted until
			specified, and if not specified, are saved in the
			file 'DATA.DAT'.  When the operator requests sorting
			all numbers are sorted, that includes ones previously
			stored as well as those which may have just been
			entered at the keyboard.  The Record numbers are output
			on the printer, with page headers and numbering, in
			their peculiar but proper order.  Beside each Record
			number is printed a line, so that patient names or
			other marginal notes can be made.

   ====================================================================  ;
;   This program sorts numbers into terminal digit order. The numbers    ;
;   are actually strings, since it is necessary to partition them for    ;
;   sorting.                                                             ;
;   Present limit on entries is 600, a considerable number.  Version 1.0 ;
;   lacked a 'help' screen. Version 1.3 fixed that and added video       ;
;   attributes.  Version 1.5 fixed an unnecessary 'beep' and provided    ;
;   nicely paginated output.                                             ;
;   Version 1.6 adds color and 'boxes' the help text.                    ;
;                                                                        ;
;    Eco-C88 is a copyright of Ecosoft, Inc., 1983 - 1986.             ;
; ===================================================================== */

/*  Uses functions available with the Ecosoft Eco-C88 Compiler  Ver 3.00  */
/*  Runs on the IBM and most generics, uses color or monochrome which  */
/*  includes the Hercules clones.  Printer is assumed to be Epson-code  */
/*  compatible, like the Gemini and other printers.  The routines which */
/*  bold, underline or reverse video the text assume ANSI.SYS is installed. */


#include "stdio.h"
#include "color.h"
#include "string.h"
#define BELL 7
#define ESC 27
int _colorflag = TRUE;   /* says you have color, but works if you don't */
char bigbuff[4200]; /* the size of bigbuff determines how many records  */
int strcmp();       /* can be brought into memory and sorted, in this   */
int cnt, flag, page; /* case the size is 4200/7 = 600 Record numbers,   */
unsigned row, col;   /* see msg4();, elsewhere in this program          */
FILE *fd;

main()
{
char *gets();
char s[7];       /* holds 6 digits plus the \0 string terminator */
char *strcpy();
int c, p;
flag = cnt = 0;
row = 4;
col = 3;
	fd = fopen("data.dat","a");     /* opening and/or creating this file */
	if((c = ferror(fd)) != NULL)    /* should produce no problem, however*/
	{                               /* this message is a warning   */
		puts("A file error has occured...");
		exit();
	}
	clrscr();
	cursor(0,29);
	bld();            
	mascr(2,FRED,BOLD);
	puts("TERMINAL DIGIT SORTER");
	unbld();
	cursor(22,27);
	revon();
	mascr(1,FGREEN);
	puts("enter Exit or Sort or Help");
	revoff();
	cursor(23,21);
	bld();
	mascr(2,FBLUE,BOLD);
	puts("(C) Copyright 1985, 1986 Neil V. Deasy");
	unbld();
	mascr(1,FWHITE);
loop:	cursor(row,col);
while(TRUE)	{
	printf("Enter number or selection  ");
	gets(s);
	if(flag != 0)	{   /*  flag is set by error routines */
		eraeol(cursor(24,0));  /* so at this point in the program */
		flag = 0;   /* we erase the error message and reset the */
		}                          /*    flag   */
	cursor(++row,col);
	eraeol(row,col);
	if(tolower(s[0]) == 'e' || tolower(s[0]) == 's' || tolower(s[0])=='h')
	{
		switch(tolower(s[0]))
		{                           /*  here are your choices  */
			case 'e':
				clrscr();
				fclose(fd);
				exit();
			case 's':
				fclose(fd);
				append1();
				exit();
			case 'h':
				help();
				printf("\nEnter <CR> to continue. . . .");
				gets(s);
				eraeol(cursor(21,0)); /* erase the help msg */
				for(row = 20; row > 4; row--) /* by backing */
					eraeol(cursor(row,0));/* up 15 lines*/
				row = 4;
				col = 0;
				eraeol(cursor(row,col));
				goto loop;
		}
	}      /*  input MUST be 6 characters  */
	if((p = strlen(s)) < 6)  /* this input is less than 6  */
		msg1();
	if((p = strlen(s)) > 6)  /* this input is more than 6  */
		msg2();
	if((p = strlen(s)) ==6)  /* this is exactly 6, BUT  */
	{
		int w;
		for(w = 0; w < 6; w++)   /* let's check each to make */
		if((c = isdigit(s[w])) == 0) /* sure there are NO letters */
		{
			msg3();  /*  it isn't a digit!  */
		}
	}
	if(flag == 0)
		{                           /* since we got here with no */
		fprintf(fd,"%6s\n",&s[0]);  /* errors - write it to the file */
			cnt += 1;
		}
	if(cnt > 14)   /* screen display area is now at max of 15 lines  */
	{
		for(row = 4; row < 20; row++)  /* so clear screen and  */
			eraeol(cursor(row,0));
		cnt = 0; row = 4; col = 3;     /* start over again  */
		cursor(row,col);
		}
	}
}
msg1()   /* messages follow a similar format, see below  */
{
	flag = 1;                    /*  set error flag */
	cursor(--row,col);           /* adjust row count location */
	cursor(24,0);                /* bottom of screen for error message */
	bld();                       /*  attribute on  */
	puts("Error: too FEW digits (6 needed) "); /* what U did wrong */
	unbld();                     /* reset attribute  */
	eraeol(cursor(row,30));      /*  erase prompting line  */
	cursor(row,col);             /* put cursor at start of prompt line  */
putchar(BELL);                       /* audible indication of error  */
	return(flag);                /* says it all */
}
msg2()
{
	flag = 1;
	cursor(--row,col);
	cursor(24,0);
	bld();
	puts("Error: too MANY digits (6 needed)");
	unbld();
	eraeol(cursor(row,30));
	cursor(row,col);
putchar(BELL);
	return(flag);
}
msg3()
{
	flag = 1;
	cursor(--row,col);
	cursor(24,0);
	bld();
	puts("Error: input must be NUMBERS     ");
	unbld();
	eraeol(cursor(row,30));
	cursor(row,col);
putchar(BELL);
	return(flag);
}
msg4()
{
	clrscr();
	puts("TDS will not sort more than 600 numbers.");
	puts("Delete some entries from file 'DATA.DAT' and re-run TDS.");
	exit();
}
append1()
{
	char temp[2];   /* a 2 character swap buffer */
	int x, y;
	x = y = 0;

	fd = fopen("data.dat","r");
	clrscr();
	bld();
	puts("READING DATA FILE");
	unbld();
	if((y = fgetc(fd)) == EOF)   /* see if file is empty to start */
	{
		puts("You made no entries to sort.        Program ends.");
		exit();              /* it was, so exit  */
	}
	ungetc(y,fd);                /* it wasn't, so put back a character */
	while((y = feof(fd))==FALSE)	{  /* read file till EOF detected */
		fscanf(fd,"%6s\n",&bigbuff[x]);
		strncpy(&temp[0],&bigbuff[x+4],2);    /* put last 2 char's */
		strncpy(&bigbuff[x+4],&bigbuff[x+2],2); /* up front and */
		strncpy(&bigbuff[x+2],&bigbuff[x],2); /* slide the others  */
		strncpy(&bigbuff[x],&temp[0],2);      /* over to align */
		x += 7;   /* bump count by 7 to include the '\0'  */
	}
	fclose(fd);
	if((x/7) > 600)
		msg4();
	printf("sorting %d entries\t\t",x/7);  /* a visual marker of progress*/
qsort(&bigbuff,(x/7),7,strcmp);     /* a library sort, using string compare  */
bld();
puts("Sorted!");
unbld();
header(&bigbuff,x);     /*  print page header */
fflush(stdlst);
unlink("data.dat");     /* discard the file now that it's printed */
}
header(buf,x)
int x;
{
	int  y, loop, page;
	char temp[2];
	y = loop = 0;
	page = 1;
	dump(page); /* this function prints page header info only */
	for(y = 0; y < x; y++)
	{  /* print sorted Record Numbers  */
		strncpy(&temp[0],&bigbuff[y],2);        /* now that they */
		strncpy(&bigbuff[y],&bigbuff[y+2],2);   /* are sorted put */
		strncpy(&bigbuff[y+2],&bigbuff[y+4],2); /* them back in the */
		strncpy(&bigbuff[y+4],&temp[0],2);      /* proper format and */
fprintf(stdlst,"\t%.2s-",&bigbuff[y]);                  /* send to printer */
fprintf(stdlst,"%.2s-",&bigbuff[y+2]);
fprintf(stdlst,"%.2s",&bigbuff[y+4]);
fprintf(stdlst," ___________\n\n");
		y += 6;
		++loop;
	if(loop > 25)    /* 26 Records per page, double spaced  */
	{
		loop = 0;
		page += 1;
		putc(12,stdlst);  /*  a form-feed  */
		putc(27,stdlst); putc(87,stdlst); putc(0,stdlst);
		fflush(stdlst);
		dump(page);
		}
	}
fprintf(stdlst,"Total Records: %d",x/7);
putc(12,stdlst);
putc(27,stdlst); putc(87,stdlst); putc(0,stdlst); /* set printer to     */
}                                                 /* normal becaues the */
dump(d)                                           /* program ends here. */
int d;
{
putc(27,stdlst);  putc(69,stdlst);   /* set to print dark  */
fprintf(stdlst,"------------------------------------------------------");
fprintf(stdlst,"---------------------------\n");
clk(); /* current time */
fprintf(stdlst,"\t\t\tTERMINAL DIGIT SORTER\t\t  ");
dte(); /* current date */
fprintf(stdlst,"\nVersion 1.60\t\t\t\t\t\t   Page %d",d);
fprintf(stdlst,"\n------------------------------------------------------");
fprintf(stdlst,"---------------------------\n\n");
putc(27,stdlst);  putc(70,stdlst); /* reset from darkend printing  */
fprintf(stdlst,"\n\t\t  Record Number\t     Last Name\n\n");
putc(27,stdlst);
putc(87,stdlst); /* set printer for double-width characters  */
putc(1,stdlst);
return;
}
help()
{
int wide, deep, r, column;
wide = 71;
deep = 15;
r = 5;
column = 1;
cursor(6,2);
mascr(1,BMAGEN);
puts("You are prompted to enter either a number or a selection which will  ");
puts(" then determine what happens next. Your selections are: Sort, Exit and");
puts(" Help.                                                                ");
puts("----------------------------------------------------------------------");
puts(" HELP - will display this screen.                                     ");
puts(" SORT - will sort the numbers just entered as well as those previously");
puts("        saved in the data file.                                       ");
puts(" EXIT - is the normal ending place after you have entered numbers but ");
puts("        do not want to sort yet. Once you select sort, all numbers are");
puts("        selected and printed.  The number file is then deleted.       ");
puts(" Numbers is the expected (or default) entry if none of the above 3 are");
puts(" selected.  The program will accept no more or no less than 6 digits  ");
puts(" and if you accidently enter letters (other than H,E or S) the program");
puts(" will prompt you for the right kind of input.                         ");
mascr(1,BBLACK);
bld();
mascr(2,FCYAN,BOLD);
box(wide,deep,r,column); /* draw pretty box around help text */
unbld();
mascr(1,FWHITE);
return;
}
bld()     /*  turn bold video on  */
{
	fprintf(stderr,"%c[1m",ESC);
}
unbld()   /*  turn bold video off  */
{
	fprintf(stderr,"%c[0m",ESC);
}
clk()
{
struct time	{
	unsigned int hour;
	unsigned int min;
	unsigned int sec;
};
	struct time *clk;
	clk = 0;
gettime(clk);
	fprintf(stdlst,"%.2u:%.2u:%.2u",clk->hour,clk->min,clk->sec);
}
dte()
{
struct date	{
	unsigned int month;
	unsigned int day;
	unsigned int year;
};
	struct date *dte;
	dte = 0;
getdate(dte);
	fprintf(stdlst,"%.2u/%.2u/%.4u",dte->month,dte->day,dte->year);
}
revon()   /*  turn on reverse video */
{
	fprintf(stderr,"%c[7m",ESC);
}
revoff()  /*  turn off reverse video  */
{
	fprintf(stderr,"%c[0m",ESC);
}
