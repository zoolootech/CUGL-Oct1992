/* 
HEADER:         CUG205.00;
TITLE:          BGDISP.C - Backgammon II (for MS-DOS);
VERSION:        2.00;
DATE:           10/10/86;
DESCRIPTION:    "Display subroutines for Backgammon II.";
KEYWORDS:       games, backgammon;
SYSTEM:         MS-DOS;
FILENAME:       BGDISP.C;
WARNINGS:       "The author claims the copyright to the MS-DOS version and
                authorizes non-commercial use only.";
SEE-ALSO:       CUG102, BG.C, BGHELP.C;
AUTHORS:        Michael Yokoyama;
COMPILERS:      Microsoft v.3.00;
*/

extern int die1, die2;
extern int mwin, ywin;
extern int red[31], white[31];

header()
{
  cls();
  printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n"); 
  printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=    BACKGAMMON     =-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
  printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
}

score()
{
    printf ("\n\n\n              ----------------------------------\n");
    printf ("              \|                                \|\n");
    printf ("              \|           Scorecard            \|\n");
    printf ("              \|                                \|\n");
    printf ("              \|                                \|\n");
    printf ("              \|    White (Me)  Red (You)       \|\n");
    printf ("              \|    ----------  ----------      \|\n");
    printf ("              \|        %2d         %2d           \|\n",mwin,ywin);
    printf ("              \|                                \|\n");
    printf ("              \|                                \|\n");
    printf ("              ----------------------------------\n\n\n\n");
}

cls()
{
  printf("%c[2J",27);
}

dboard()
{
  int k;

  /* first line */
  printf ("%c[37m\n\nWhite's Home                         (ME)%c[30m\n\n",27,27);

  /* top label line */
  for (k = 1;k <= 6;k++)
    printf (" %4d ",k);
  printf ("    ");
  for (k = 7;k <= 12;k++)
    printf (" %4d ",k);
  printf ("\n" );

  /* delimiter */ 
  for (k = 1;k <= 78;k++)
    printf ("=" );
  printf ("\n" );

  /* letters line */
  printf ("|" );
  dcol(red,'R',white,'W',1,6);
  printf (" || ");
  dcol(red,'R',white,'W',7,12);
  printf ("|\n" );

  /* numbers line */
  printf ("|" );
  dnum(red,white,1,6);
  printf (" || ");
  dnum(red,white,7,12);
  printf ("|\n");

  printf ("|%37s||%37s|\n"," "," ");

  /* white's bar */
  if (white[0] != 0)
    printf ("|%c[37m%38dW%c[30m%37s|\n",27,white[0],27," ");
  else 
    printf ("|%37s||%37s|\n"," "," ");

  /* red's bar */
  if (red[0] != 0)
    printf ("|%c[31m%38dR%c[30m%37s|\n",27,red[0],27," "); 
  else 
    printf ("|%37s||%37s|\n"," "," ");

  printf ("|%37s||%37s|\n"," "," ");

  /* numbers line */
  printf ("|" );
  dnum(white,red,1,6);
  printf (" || ");
  dnum(white,red,7,12);
  printf ("|\n");

  /* letters line */
  printf ("|" );
  dcol(white,'W',red,'R',1,6);
  printf (" || ");
  dcol(white,'W',red,'R',7,12);
  printf ("|\n" );

  /* delimiter */ 
  for (k = 1;k <= 78;k++)
    printf ("=" );
  printf ("\n" );

  /* bottom label line */
  for (k = 24;k >= 19;k--)
    printf (" %4d ",k);
  printf ("    ");
  for (k = 18;k >= 13;k--)
    printf (" %4d ",k);

  /* last line */
  printf ("%c[31m\n\nRed's Home                           (YOU)\n\n%c[30m",27,27);
}

dnum(upcol,downcol,start,fin)
int *upcol,*downcol,start,fin;
{
  int k,n;
  for (k = start;k <= fin;k++) {
    if ((n = upcol[k]) != 0 || (n = downcol[25-k]) != 0)

#if MSDOS
      if ((upcol[k] && upcol == red) || (downcol[25-k] && downcol == red)) {
        if (n < 10)
          printf ("   %c[31m%1d%c[30m  ",27,n,27);
        else
          printf ("  %c[31m%2d%c[30m  ",27,n,27);
      }
    else {
      if (n < 10)
        printf ("   %c[37m%1d%c[30m  ",27,n,27);
      else
        printf ("  %c[37m%2d%c[30m  ",27,n,27);
    }
#else
    printf (" %4d ",n);
#endif
    else printf ("      ");
  }
}

dcol(upcol,c1,downcol,c2,start,fin)
int *upcol,*downcol,start,fin;
char c1,c2;
{
  int k;
  char c;
  for (k = start;k <= fin;k++) {
    c = ' ';
    if (upcol[k] != 0)
      c = c1;
    if (downcol[25-k] != 0)
      c = c2;
    switch (c) {
    case 'R':
#if MSDOS
      printf ("  %c[0;41;33m %c %c[0;43;30m ",27,c,27);  
#else
      printf ("   %c %c ",c);  
#endif
      break;
    case 'W':
#if MSDOS
      printf ("  %c[0;47;33m %c %c[0;43;30m ",27,c,27);
#else
      printf ("   %c %c ",c);  
#endif
      break;
    default:
      printf ("      ",c);
      break;
    }
  }
}

show1()
{
  switch(rand() % 5) {
  case 0:     
    printf ("You rolled ");
    break;
  case 1:     
    printf ("Your roll is ");
    break;
  case 2: 
    printf ("Your roll:  ");
    break;
  case 3:     
    printf ("Your dice show: ");
    break;
  case 4:     
    printf ("You rolled a ");
  }
#if MSDOS
  printf ("%c[31m%d %d%c[30m  ",27,die1,die2,27);
#else
  printf ("%d-%d.  ",die1,die2);
#endif
}

show2()
{
#if MSDOS
  cls();
#else
  printf ("\n");
#endif
  switch(rand() % 5) {
  case 0:     
    printf ("I rolled");
    break;
  case 1:     
    printf ("My roll was");
    break;
  case 2: 
    printf ("I threw: ");
    break;
  case 3:     
    printf ("My roll: ");
    break;
  case 4:     
    printf ("I tossed");
  }
#if MSDOS
  printf (" %c[37m%d %d%c[30m  ",27,die1,die2,27);
#else
  printf (" %d, %d, ",die1,die2);
#endif
}

sleep()
{
  int delay,delay1;

  for (delay = 1; delay <= 500; delay++)
    for (delay1 = 1; delay1 <= 500; delay1++)
      ;
}
