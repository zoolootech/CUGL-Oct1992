/*	
** time.c	Time set/display Program	by F.A.Scacchitti  9/23/84
**		    Copyright 1984 		   25 Glenview Lane
**		  All rights reserved		   Roch, NY  14609
**
**		Written in Small-C  (Version 2.09)
**
**		Time Utility for the Xerox 820
**
**		time p   --->  prints current time and date
**		time s	 --->  enter time set routine
**
** This program is designed to be used with an auxiliary printf routine
** (prntf) to minimize program size. (printf may be substituted for prntf) 
**								 (fas)
**
*/

#include <stdio.h>

#define CLEAR 26
#define BACKSP 8
#define SPACE 32
#define VERTAB 11
#define LINSIZ 18
/*#define MEM 65369             pointer to first time byte for the 820-I   */
#define MEM 65366           /*  pointer to first time byte for the 820-II  */


char array[LINSIZ];
int num[6];
int lolim[] = { 1, 1, 84, 0, 0, 0};
int hilim[] = {31, 12, 99, 23, 59, 59};


main(argc,argv) int argc, *argv; {

char *entry;

argv++;		/* point to option */
entry = *argv;	/* set up 2nd indirect pointer   *entry = **argv */

   switch (*entry) {
      case 'P':
		if(!gudtim(gettim())) settim();
		if(gudtim()) prntim();
		break;
      case 'S':
		if(settim()) prntim();
		break;
      default:
		argc = 0;
		break;
   }
   if (argc != 2){
	prntf(" usage: time p <CR> prints time and date\n");
	prntf("        time s <CR> to set time and date\n");
   }
}

prntim(){	/* Routine to print time and date */

char *clkmem;

   putchar(VERTAB);

   clkmem = MEM;
   clkmem++;
	switch (*clkmem){
	case 1:
		prntf("January");
		break;
	case 2:
		prntf("February");
		break;
	case 3:
		prntf("March");
		break;
	case 4:
		prntf("April");
		break;
	case 5:
		prntf("May");
		break;
	case 6:
		prntf("June");
		break;
	case 7:
		prntf("July");
		break;
	case 8:
		prntf("August");
		break;
	case 9:
		prntf("September");
		break;
	case 10:
		prntf("October");
		break;
	case 11:
		prntf("November");
		break;
	case 12:
		prntf("December");
		break;
	default:
	abort('?');
	}

	clkmem--;
   prntf(" %d, 19%d  %02d:%02d:%02d",*clkmem++,*++clkmem,*++clkmem,
						*++clkmem,*++clkmem);

}

settim(){	/* Routine to set time and date */

int i;
char c, *clkmem;

	clkmem = MEM;

/* Display the time set header */

	putchar(CLEAR);
	prntf("Enter the time and date as dd/mm/yy hh:mm:ss\n");
	prntf("\n eg. for 1:45 P.M. on August 12, 1984\n");
	prntf("     Enter ---> 12/08/84 13:45:00\n");
	prntf("                dd/mm/yy hh:mm:ss");
	prntf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");

/* Get the input string */
	i = -1;
	while (((c = getchar()) != CR) && i++ < 17)
		if (c == BACKSP)
			if (i == 0){
				i = -1;
				putchar(SPACE);}
			else
				i = i - 2;
		else
			array[i] = c;

/* Convert the characters to integer values */

	for (i = 0; i <= 18; i = i + 3)
		num[i/3] = ((array[i] - '0') * 10) + (array[i+1] - '0');

	prntf("\n\n");

/* Store the values to memory if they're valid */

	if (gudtim()) {
		for (i = 0; i <=5; i++)
			*clkmem++ = num[i];
		return(TRUE);
	}else{ 
		prntf(" Incorrect data entered - time not updated");
		return(FALSE);
	}
}

gudtim(){

int i;

   for(i = 0; i <= 5;i++)
	if (num[i] < lolim[i] || num[i] > hilim[i]) return(FALSE);
   return(TRUE);
}

gettim(){  /* Routine to get time data from memory to an array  */

int i;
char *clkmem;

	clkmem = MEM;

	for (i = 0; i <=5; i++)
			num[i] = *clkmem++;
}

