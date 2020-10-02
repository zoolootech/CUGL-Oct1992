/*	This game involves guessing the digits in a 
	three by two digit multiplication.  It is a
	simple game, but absorbing.  You are likely
	to find that it is relatively easy to guess
	the right answer within three to five tries
	once you are proficient at the game.

	The original was written in BASIC and first
	appeared in KILOBAUD at about 1979.  I have
	lost the original reference.

	This version was written as an exercise for
	my own edification in learning the  Digital
	Research version of C on the IBM-PC.

	Peter G. Wohlmut                         */

#include <stdio.h>
#include <ctype.h>


#define ESC	putchar(27)         	    /* ANSI escape code */
#define CLS	ESC,printf("[2J")           /* ANSI Clear Screen */
#define LOC(r,c)  ESC,printf("[%d;%dH",r,c) /* ANSI position cursor */
#define BEEP	printf("%c",'\007')	/*ASCII bell */
#define SMODE(n) ESC,printf("[=%dh",n)	/* ANSI set screen type */



/*Haven't figured out how to get at the system time under PC-DOS yet
  so have settled on calling HANG with a numeric argument to set up
  the random number seed. */

main(argc,argv)

	int argc;
	char *argv[];

{

	int width;		/* width =0 is 40 column 
					 =2 is 80 column */
	int c,d;		/* c is column #, d is guessed digit */
	int ans;			/* keyboard responses */
	int g;				/* game number */
	int t;				/* total missed guesses */
	int n1;				/* # misses in game */
	int n5;				/* good guesses in game */
	int f[10][6];			/* flags for which digits have
                                 been guesses (f=1) */
	long int a[6][6];		/* value of each digit */
	int i,j,m,n9;			/* general indices */
	int v;				/* average # misses/game */
	int q;				/* performance rating index */
	long int n[6];			/* power series for digit set up */
	static char *r[] = { 	         /*Set up ratings */
		 	"GREAT",
		     	"EXCELLENT",
		     	"VERY GOOD",
		     	"GOOD",
		     	"ABOVE AVERAGE",
		     	"AVERAGE",
		     	"BELOW AVERAGE",
		     	"FAIR",
		        "POOR"
			};
 
	static char *p[] = {		/* set up masked array size */
		     	"* * * * * ",
		     	"* * * * * ",
		     	"* * * * * ",
		     	"* * * * * ",
		     	"* * * *   ",
		     	"* * * * * "
				};

		width= 0;               /* set screen to 40 column */
		SMODE(width);
		CLS;			/* clear screen */

		ans=0; i=argc;
	while (--i>0) ans+=atoi(argv[i]);
		ans=srand(ans); 		/*set up random seed */


/* Print title page */

		CLS; LOC(3,2);
		printf("%s\n","HANGMATH - a game of luck and logic");
		LOC(5,2);
		printf("%s\n","Play at least 3 games to get a score");

		LOC(9,10);
		printf("%s","Instructions (y/n)");
 
		while((ans=getchar())!='\n')
		if (ans=='Y'||ans=='y' ) instruct(ans);

		g=1;				/* first game */
		t=0;

      while (g)     {			/* main loop */

	/* reset masked digits each game */

		CLS;
		c=d=0;
		for (i=1;i<6;i++) {
			for (j=0;j<5;j++)
				*(p[i]+2*j)='*';
				  }
			*(p[4]+8)=' ';  /* fourth is left shifted */


/* initialize masked number set */

		for (i=1;i<6;i++) {
			for (j=1;j<6;j++) {
			a[i][j]=-1;
			n[i]=0;
				     }
				}
 
/* initialize flags for entered numbers */

		for (i=0;i<10;i++) {
			for (j=1;j<6;j++) {
				f[i][j]=0;
                                            }
				   }

/* generate random digits for multiplication */

	a[1][1]=rand()%10; a[2][1]=rand()%10; a[3][1]=rand()%10;
	a[1][2]=rand()%10; a[2][2]=rand()%10; 


/* do the multiplication */

		n[1]=100*a[3][1]+10*a[2][1]+a[1][1];
		n[2]=10*a[2][2]+a[1][2];
		n[3]=a[1][2]*nC1WW!HkEEEjua[2][2]*n[1]*10;
		n[5]=n[1]*n[2];

		for (i=5;i>0;i--) {
			m = power(10,i-1);

			for (j=3;j<6;j++) 
				a[i][j]=(n[j]/m+0.001);
 
			for (j=3;j<6;j++) {
			n[j]=(n[j]-a[i][j]*m+0.9);
					  }
			           }
		n1=0;
		n5=0;

	/* eliminate superfluous digits */

		a[1][4]=a[4][1]=a[5][1]=a[3][2]=a[4][2]=a[5][2]=-1;
		a[5][3]=-1;

	CLS;

/* start game loop */

		while (n5<30) {


	LOC(1,21);printf("%s",(p[1]+4));
	LOC(3,23);printf("%s",(p[2]+6));
/*	LOC(4,16);printf("%s",());*/
	LOC(5,19);printf("%s",(p[3]+2));
	LOC(6,17);printf("%s",p[4]);
	LOC(7,16);printf("%s","-----------");
	LOC(8,17);printf("%s",p[5]);
 
	LOC(12,1);printf("%s %1d","# misses: ",n1);
	LOC(14,1);printf("%s","PREVIOUS GUESSES BY COLUMN");

		for (i=1;i<6;i++) {
			LOC(15+i,1);printf("%s %d %s","COL ",i,":");
 
		for (j=0;j<10;j++) 
			if (f[j][i]) printf("%d %s",j," ");
				  }

		if (n5>=18) break;

		LOC(10,1); printf("ENTER COLUMN #");
	        c=interrupt(7);           /*DOS function 7 - keyboard 
					    poll with no echo */
		LOC(10,15); printf("%c %s",c," ");


		LOC(10,20); printf("ENTER DIGIT ");
	        d=interrupt(7);
		LOC(10,33); printf("%c %s",d," ");
 
	c=c-'0';			/*set c,d to integers */
	d=d-'0';

	if ((c<1||c>5)||(d<0||d>9)) continue;  /*check for bounds */

		if (f[d][c]) {
			LOC(24,1);BEEP;BEEP;
		printf("YOU ALREADY GUESSED THAT!");
				}
	else {
		LOC(24,1);printf("                          ");
		m=1;
		i=0;

	while (m<6)
		{
		if ((a[c][m]==d) && (!f[d][c])) {

		j=8-2*(c-1);
		*(p[m]+j)=d+'0';     /*replace * with digit */
			++n5;
			i=m;
					}

			++m;
		}
		if (!i)
			++n1;		/* tally wrong guess */

		f[d][c]=1;

	}
			}
	LOC(21,1);
	BEEP;BEEP;BEEP;
	 printf("%s\n","YOU GOT IT !!!!");
		t+=n1; v=t/g;       /*tally total wrong, and average */
	LOC(22,1);
	printf("%s %d %s %d\n","Average # of misses after ",g," games ",v);
	LOC(23,1);
	printf("%s","Another Game???? (y/n) ");

		if((ans=interrupt(7))=='N'||ans=='n') break;
		++g;

  }
	q=v/2;
		if(q<0 ) q=0;
		if(q>8 ) q=8;

		width = 2;	        /* set to 80 column B&W */
		SMODE(width);


	printf("%s %s %s\n","Your performance rating was ",r[q],". BYE");
 
}

instruct(i)
{

		CLS;
		printf("%s\n","This game sets up a three digit by");
		printf("%s\n","two digit multiplication and displays");
		printf("%s\n","the details with all digits masked by");
		printf("%s\n","a * for each digit. Type the column");
		printf("%s\n","and then the number you chose as");
		printf("%s\n","a number between 0 and 9.  All");
		printf("%s\n","occurences of that digit in that ");
		printf("%s\n","will then be displayed.  Play until");
		printf("%s\n","you have replace,$a`l *s with digits.");
		printf("%s\n","Columns are numbered from right to");
		printf("%s\n","left as #1 to #5");

		LOC(20,5);
		printf("%s","Touch return key to continue");
		getchar();

		return;
}
/* calculate i to the jth power */
power(i,j)
{
	int l;
	int m;

		m=1;
		for (l=1;l<j+1;l++) 
			m=m*i;

		return(m);
}
/* IBM DOS interrupt calls */
interrupt(i)
{
	int c;

	c=__IBMDOS(i);

	return(c);

}
