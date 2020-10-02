/* PUNCTUATION ERROR FINDER PROGRAM */
/* r p sarna                     */
/* 11 feb 84                      */

/* FINDS:                                       */
/* periods, etc., proceeded by spaces           */
/* periods, etc., not followed by spaces        */
/* number of '(' and ')' and "                  */
/* Sentences not beginning with capital letters */

#define VERSION "1.11"

#define COMMA ','
#define PERIOD '.'
#define LPAREN '('
#define RPAREN ')'
#define ON 1
#define OFF 0

#include <bdscio.h>

main(argc,argv)
char **argv;
{
	char ibuf[BUFSIZ];
	char end_of_file;
	char curr;
	int icurr;
	char prev;
	int linefeed;
	int leftparen;
	int rightparen;
	unsigned num_of_chars;
	int line_num;
	char period_flag;
	int quotes;

	if ((strcmp(argv[1], "help") == 0) || (strcmp(argv[1],"HELP") == 0)) {
		printf("\n%cPUNCTUATION ",0x1A);
		printf("     r p sarna   Version %s\n", VERSION);
		printf("\nThis program checks for improper punctuation.\n");
		printf("\nThe program checks for:");
		printf("\n     periods preceeded by a space");
		printf("\n     commas preceeded by a space");
		printf("\n     exclamation marks preceeded by a space");
		printf("\n     question marks preceeded by a space");
		printf("\n     periods not followed by a space");
		printf("\n     commas not followed by a space");
		printf("\n     exclamation points not followed by a space");
		printf("\n     question marks not followed by a space");
		printf("\n     unequal number of left and right parentheses");
		printf("\n     odd number of quotes marks\n");
		printf("\n\n     Note: characters are counted only up to 65,535 -- ");
                printf(  "\n           the count then starts over");
		printf("\n\nUsage: A>punct d:filename.typ\n\n");
		exit();
	}

	if (argc != 2) {
		printf("\nUsage: A>punct d:filename.typ \n");
		printf("\n    or A>punct help\n\n");
		putchar(7);
		exit();
	}

	if (fopen(argv[1], ibuf) == ERROR) {
		printf("\n***** ERROR: can't find %s\n", argv[1]);
		putchar(7);
		exit();
	}
	
	printf("\n\nPUNCTUATION CHECK PROGRAM  --  r p sarna  -- Ver. %s\n", VERSION);

	prev = ' ';
	end_of_file = NO;
	leftparen = 0;
	rightparen = 0;
	quotes = 0;
	line_num = 1;
	num_of_chars = 1;
	period_flag = ON;
	end_of_file = NO;

	while (end_of_file == NO) {

		if ((icurr = getc(ibuf)) == ERROR || (icurr == CPMEOF)) { 
			end_of_file = YES;
			continue;
		}

		curr = icurr;

		if ((icurr == 0x0D) || (icurr == 0x8D)) {
			line_num++;
			prev = ' ';
			curr = ' ';
			continue;
		}

		if (curr == 0x0A) continue;

		curr = curr & 0x7F;

		if (curr == '\t') curr = ' ';
                                                   
		num_of_chars++; 

		if ((prev == PERIOD) && (curr != ' '))
			printf("\n*** ERROR-- no space after period in line number %d", line_num); 
		;

		if ((prev == '!') && (curr != ' '))
			printf("\n*** ERROR-- no space after exclamation point in line number %d", line_num); 
		;

		if ((prev == '?') && (curr != ' '))
			printf("\n*** ERROR-- no space after question mark in line number %d", line_num); 
		;

		if ((prev == ',') && (curr != ' '))
			printf("\n*** ERROR-- no space after comma in line number %d", line_num); 
		;

		if ((curr == PERIOD) && (prev == ' '))
			printf("\n*** ERROR-- space before period in line number %d", line_num );
		;	

		if ((curr == '?') && (prev == ' '))
			printf("\n*** ERROR-- space before question mark in line number %d", line_num );
		;	

		if ((curr == '!') && (prev == ' '))
			printf("\n*** ERROR-- space before exclamation point in line number %d", line_num );
		;	

		if ((curr == COMMA) && (prev == ' ')) 
			printf("\n*** ERROR-- space before comma in line number %d", line_num);
		;

		if ((period_flag == ON) && (curr != ' ') && (curr > 0x5A))
 			printf("\n*** ERROR-- First letter of sentence not capitalized in line %d", line_num);
		;

		if ((period_flag == ON) && (curr > '!')) period_flag = OFF;

		if ((curr == PERIOD) || (curr == '!') || (curr == '?')) period_flag = ON;

		if (curr == LPAREN) leftparen++;

		if (curr == RPAREN) rightparen++;

		if (curr == '"' ) quotes++;

		prev = curr;


	}


	close(ibuf);

	printf("\n\nThere were %d left parentheses and %d right parentheses in '%s'", leftparen, rightparen, argv[1]);

	if (leftparen != rightparen) printf("\n**** NOTE: '%s' has unbalanced parentheses!!", argv[1]);

	printf("\n\nThere were %d quotes marks (%c) in '%s'", quotes, 0x22, argv[1]);

	if (quotes & 0x01) printf("\n**** NOTE: odd number of quotes used.");

	printf("\n\nThere were %d characters and %d lines in '%s'\n\n\n", num_of_chars, (line_num - 1), argv[1]);

}
