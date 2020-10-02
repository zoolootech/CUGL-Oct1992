/*
TITLE:		Karel The Robot;
VERSION:	1.0;
DATE:		09/27/1984;
DESCRIPTION:
	"Header and source for Karel Simulator.";
KEYWORDS:	Robot, Simulator;
SYSTEM:		CP/M 2+;
FILENAME:	KAREL.H;
AUTHORS:	Linda Rising;
COMPILERS:	BDS C;
REFERENCES:
	AUTHOR: Richard E. Pattis;
	TITLE: "KAREL the Robot: A Gentle Introduction
		to the Art of Programming";
	CITATION: "";
ENDREF
*/


#include <bdscio.h>             /* standard header */

/* global constants */

#define CURPOS  "\033="		/* cursor control ESC = */
#define X_POS     32		/* cursor control for x */
#define Y_POS     32		/* cursor control for y */
#define FILE  struct _buf	/* label for file header */

/* global variables */

char wworld[23][75];             /* array for Karel's world */
int kst;		   	 /* Karel's street */
int kav;			 /* Karel's avenue */
char kdir;			 /* Karel's direction */
int nnorth;	      		
int ssouth;
int eeast;
int wwest;
int ccorner;
int bbag;
int ffront;
int rright;
int lleft;

set_up(argv)   /* prepare for execution of KAREL's program */
char **argv;
{

	  char ibuf1[BUFSIZ];

	  fopen(argv[1],ibuf1);
	  readworld(ibuf1);
	  printworld();
	  setbooleans();
	  wwait();  wwait();
}
	     
readworld(buf)   /* read values stored in world file */
FILE *buf;
{
	  int i,j;

	  fscanf(buf,"%d %d %d",&kst,&kav,&bbag);
	  kdir = getc(buf);
	  for (i = 0; i < 23; i++)
	    for (j = 0; j < 75; j++)
	      wworld[i][j] = getc(buf);
	  fclose(buf);
}


printworld()   /* output values stored in world array */
{
          int i,j;

          for (i = 0; i < 23; i++) {
            for (j = 0; j < 75; j++)
              printf("%c",wworld[i][j]);
            printf("\n");
	 
          } 
}


setbooleans()     /* evaluate tests for KAREL's program */
{
	  char k;

	  k = wworld[kst][kav];
	  if (k == '.' || k == '^' || k == 'V' ||
			  k == '>' || k == '<')
	     ccorner = 0;
	  else ccorner = 1;

	  nnorth = (kdir == '^');
	  ssouth = (kdir == 'V');
	  eeast = (kdir == '>');
	  wwest = (kdir == '<');

	  switch(kdir) {
	  case '^':
	     ffront = (wworld[kst - 1][kav] == ' ');
	     rright = (wworld[kst][kav + 2] == ' ');
	     lleft = (wworld[kst][kav - 2] == ' ');
	     break;
	  case 'V':
	     ffront = (wworld[kst + 1][kav] == ' ');
	     rright = (wworld[kst][kav - 2] == ' ');
	     lleft = (wworld[kst][kav + 2] == ' ');
	     break;
	  case '>':
	     ffront = (wworld[kst][kav + 2] == ' ');
	     rright = (wworld[kst + 1][kav] == ' ');
	     lleft = (wworld[kst - 1][kav] == ' ');
	     break;
	  case'<':
	     ffront = (wworld[kst][kav - 2] == ' ');
	     rright = (wworld[kst - 1][kav] == ' ');
	     lleft = (wworld[kst + 1][kav] == ' ');
	  }
}


move()        /* move KAREL one unit in direction faced */
{

	  if (ffront) {               /* if front is clear */
	     if (!ccorner) {   /* and no beepers on corner */
	        set_curs(kst,kav,1,".");     /* put a "." */
		wwait();
	     }
	     switch(kdir) {     /* determine new location */
	     case '^':
		kst -= 2;
		break;
	     case 'V':
		kst += 2;
		break;
	     case '<':
		kav -= 4;
		break;
	     case '>':
		kav  += 4;
	     }
	     setbooleans();         /* re-evaluate booleans */
	     if (!ccorner) {     /* if no beepers on corner */
		wworld[kst][kav] = kdir;     /* print KAREL */
	        set_curs(kst,kav,1,"");
	        printf("%c",kdir);
		wwait();
	     } else {
		  set_curs(kst,kav,1,"");
		  printf("%c",kdir);
		  wwait();
		  set_curs(kst,kav,1,"");
		  printf("%c",wworld[kst][kav]);
		  wwait();
	       }
	  } else turnoff(1);  /* error if front is blocked */
}


turnleft()     /* rotate KAREL 90 degrees counterclockwise */
{
	  switch(kdir) {
	  case '^':
	     kdir = '<';
	     break;
	  case 'V':
	     kdir = '>';
	     break;
	  case '<':
	     kdir = 'V';
	     break;
	  case '>':
	     kdir = '^';
	  }
	  if (!ccorner) {    /* if no beepers on corner */
	      wworld[kst][kav] = kdir;   /* print KAREL */
	      set_curs(kst,kav,1,"");
	      printf("%c",kdir);
	      wwait();
	  } else {
	       set_curs(kst,kav,1,"");
	       printf("%c",kdir);
	       wwait();
	       set_curs(kst,kav,1,"");
	       printf("%c",wworld[kst][kav]);
	       wwait();
	    } 
	  setbooleans();
}


pickbeeper()    /* pick up a beeper from KAREL's corner */
{
	  char k;
	  
	  k = wworld[kst][kav];
	  if (!ccorner)
	     turnoff(3);
 	  else { if (k == '2')
		     k = '@';
	     else if (k != '@')
		     --k;
	     else {
		k = kdir;
		ccorner = 0;
	     }
	     wworld[kst][kav] = k; 
	     set_curs(kst,kav,1,"");
	     printf("%c",k);
	     wwait();
	     ++bbag;
	  }
}

putbeeper()     /* place a beeper on KAREL's corner */
{
	  char k;
	 
	  k = wworld[kst][kav];
	  if (!bbag)
	     turnoff(2);
	  else { 
	     if (!ccorner) { 
	        k = '@';
	        ccorner = 1;
	     } else if (k == '@')
		       k = '2';
	       else if (k == '9')
		       set_curs(23,0,75,"ONLY 9 BEEPERS ALLOWED.");
	       else ++k;
	     wworld[kst][kav] = k;
	     set_curs(kst,kav,1,"");
	     printf("%c",k);
	     wwait();
	     --bbag; 
	  }
}


turnoff(code)    /* stop execution of KAREL's program */
int code;
{
	  if (code == 0) 
	     set_curs(22,0,45,"  END OF EXECUTION.");
	  else { 
	       set_curs(22,0,45,"  ERROR SHUTOFF.");
	       switch (code) {
	       case 1:
	          set_curs(22,17,0,"FRONT IS BLOCKED.");
	          break;
	       case 2:
	          set_curs(22,17,0,"NO BEEPERS IN BAG.");
	          break;
	       case 3:
	          set_curs(22,17,0,"NO BEEPERS ON CORNER.");
	       case 4:
		  set_curs(22,17,0,"NO TURNOFF EXECUTED.");
	       }
	    }
	  set_curs(23,37,0,"");
	  kst = (21 - kst)/2;
	  kav = (kav - 4)/4;
	  printf("KAREL'S ST= %d  AVE= %d  DIR= %c BAG= %d",
					    kst,kav,kdir,bbag);
	  set_curs(20,0,0,"");
	  exit();
}


set_curs(row,col,blanks,mess)    /* set cursor */
int row,col,blanks;
char *mess;
{
  	  int j;

	  printf("%s%c%c", CURPOS, row + X_POS, col + Y_POS);
	  for (j = 1; j <= blanks; j++)
	    printf(" ");
	  printf("%s%c%c", CURPOS, row + X_POS, col + Y_POS);
	  printf("%s", mess);
}


wwait()      /* slow KAREL so execution can be seen */
{
	  int j;

	  for (j = 0; j < 5000; j++)
	    ;
}