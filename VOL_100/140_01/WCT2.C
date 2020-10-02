/* 

word count program which also calculates column centimetres 
oæ magazinå (oò newspaper© copù foò would-bå contributors® 
Seô uð foò Helveticá Medium phototype which is the body copy 
used in Your Computer magazine, but can be easily changed 
for others. Original structure taken from `The C Programming 
Language', via BDSC users group (wc.c) and modified since 
then by YC staff....

Version YC.01, 4/6/82
*/


#include	<bdscio.h>

#define	YES	1
#define	NO	0
#define	EOF	0x1a
#define ERROR	-1
#define MASK	0x7f

char toupper();		

main(argc,argv)	/* count lines, words, chars in input file */
char	**argv;
{
	int c, nl, nw, nc, inword, fd, m;
	char	buf[BUFSIZ];

	if(argc != 2) {
		printf("Usage: wc filename\n");
		exit();
	}

	if((fd = fopen(argv[1],buf)) == ERROR) {
		printf("cannot open: %s\n",argv[1]);
		exit();
	}
	inword = NO;
	nl = nw = nc = 0;
	while ((c = (getc(buf) & MASK)) != EOF)
	{
		++nc;
		if (c == '\n')
			++nl;
		if (c == ' ' || c == '\n' || c == '\t' || c == 0x0d)
			inword = NO;
		else if (inword == NO) {
			inword = YES;
			++nw;
		}
	}
	menu();
	while (m != "5") {                 /* the m!=5 is irrelevant, but at
					      least sets up the loop */
	printf("\nWhich would you like? ");
	m = getchar();
		switch (toupper(m)) {

		case '1':
			printf("\n\n\n");
			printf("number of chars    = %d\n",nc);
			printf("number of lines    = %d\n",nl);
			printf("number of words    = %d\n",nw);
			printf("column centimetres = %d\n",nw/20);
			break;
		case '2':
			printf("\n\n\n");
			printf("number of chars    = %d\n",nc);
			printf("number of lines    = %d\n",nl);
			printf("number of words    = %d\n",nw);
			printf("column centimetres = %d\n",nw/24);
			break;
		case '3':
			printf("\n\n\n");
			printf("number of chars    = %d\n",nc);
			printf("number of lines    = %d\n",nl);
			printf("number of words    = %d\n",nw);
			printf("column centimetres = %d\n",nw/12);
			break;
		case '4':
			printf("\n\n\n");
			printf("number of chars = %d\n",nc);
			printf("number of lines = %d\n",nl);
			printf("number of words = %d\n",nw);
			printf("haven't worked it out yet!\n");
			break;
		case 'X':
			printf("\n\n\n");
			printf("number of chars = %d\n",nc);
			printf("number of lines = %d\n",nl);
			printf("number of words = %d\n",nw);
			printf("bibi\n");
			exit();
		case 'M':
			menu();
			break;
		default:
			putch(7);
			printf("\nEnter 1-4 for measure, M for menu");
			break;
			}
		}
}

menu()
{
	printf(CLEARS);		/* Hope you have screen-clear defined
				   in bdscio.h			    */ 

	printf("SELECT:\n");

	printf("<1> ---> 9/10 x 13 1/2 ems\n");   /*9point type, 1point
						    leading - the standard 
						    3-column type used in the
						    magazine                */

	printf("<2> --->  8/9 x 13 1/2 ems\n");   /*slightly smaller type over 
						    same column with - used for
						    copy with "pocket programs"*/

	printf("<3> ---> 9/10 x  9 1/2 ems\n");   /*normal type size, narrow
						    column width for news and
						    "clinic" pages */

	printf("<4> ---> 9/10 x 20 1/2 ems\n");   /* 2-column (1/2-page) width*/

	printf("<X> ---> ++ eXit ++\n");

}


