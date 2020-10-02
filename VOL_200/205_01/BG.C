/*
HEADER:         CUG205.00;
TITLE:          BG.C - Backgammon II (for MS-DOS);
VERSION:        2.00;
DATE:           10/14/86;
DESCRIPTION:    "Leor Zolman's public domain backgammon
                for PC/MS-DOS.  Uses ANSI.SYS for screen control and colors.";
KEYWORDS:       games, backgammon;
SYSTEM:         MS-DOS;
FILENAME:       BG.C.;
WARNINGS:       "This program uses the ANSI color standard.  Include the
                 following line in your CONFIG.SYS file at system start.  
                      device = ansi.sys
                For further details on ANSI.SYS, refer to your DOS manual.
                The author claims the copyright to the MS-DOS version and
                authorizes non-commercial use only.";
SEE-ALSO:       CUG102, BGTALK.C, BGDISP.C;
AUTHORS:        Michael Yokoyama;
COMPILERS:      Microsoft v.3.00.;
*/

#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bgdisp.c"     /* Display subroutines              */
#include "bgtalk.c"     /* Conversation subroutines         */

#define LINT_ARGS 1     /* Enable strong lint-type checking */
#define NIL (-1)
#define NO 0
#define YES 1
#define MAXGMOV 10
#define MAXLINE 150
#define MAXIMOVES 1000

/*  Declare all functions which do not return integers */

char strips();

char c, level;
int i, j, l, m, count;
int die1, die2;               /* Dice                                   */
int mwin, ywin;               /* Rolling count of my wins and your wins */
int red[31];                  /* Array of the board's red pieces        */
int white[31];                /* Array of the board's white pieces      */
int prob[13];                 /* Probability?                           */
int goodmove[MAXGMOV];
int probmove[MAXGMOV];
int imoves;

struct {
  int pos[4];
  int mov[4];
} moves[MAXIMOVES];

main()
{
  int a, i, k, n, t, go[6];
  int firstmove;                /* Flag == true on first move only       */
  int wrongmove;                /* Flag == true if move is incorrect     */
  long iths;                    /* Hundredths of secs for random seed    */
  char s[MAXLINE];

  mwin = ywin = 0;              /* Reset game score                      */
  time(&iths);                  /* Seed the random generator             */
  srand(iths);
  title();
  printf ("Do you want the instructions?\n");
  if ((c = getche()) == 'y')
    help();
  header();
  select();
newgame:
  init();
  firstmove = 1;
  go[5] = NIL;
  message0();
throw:  
  roll();
  printf ("My roll is: %c[37m%d%c[30m, your roll is: %c[31m%d%c[30m\n",27,die1,27,27,die2,27);
  if (die1 == die2) {
    printf ("Tied up, roll again:\n");
    sleep();
    goto throw;
  }
  printf ("\n%s will go first . . .\n\n", die1 > die2 ? "I" : "You");
  printf ("Press any key:\n");
  getch();
  cls();
  if (die2 > die1) 
    goto red;

white: 
  if (!firstmove) 
    roll();                 /* Skip roll on first move */
  firstmove = 0;            /* First roll not true anymore */
  order();
  show2();
  if (!canmove(white,red)) {
    message6();
    goto red;
  }
  message5();
  if (nextmove(white,red) == NIL) 
    goto red;
  if (piececount(white,0,24) == 0) {
    sleep();
    cls();
    printf ("\7I won!\n\n");
    ++mwin;
    score();
    if (mwin == ywin)
      printf ("All tied up!  Who's gonna pull ahead?\n");
    else if (mwin > 2 && !ywin)
      printf ("It's about time you managed to win a game!\n");
    else message1();
    goto again;
  }

red:
  dboard();
  if (!firstmove) 
    roll();                  /* Skip roll on first move             */
  firstmove = 0;             /* First roll not true anymore         */
  order();
  show1();
  if (!canmove(red,white)) {
    message7();
    getch();
    goto white;
  }
retry:
  message3();
  while (a = kbhit() != 0) {
    time(&iths);
    srand(iths);             /* Exercise the random generator while */
  }                          /* waiting for a move to be made . . . */
  gets(s);                   

  if (strips(s) == '?' || strips(s) == 'h') {
    helpscrn();
    printf ("\nDo you want to see all the instructions?\n");
    if ((c = getche()) == 'y')
      help();
    cls();
    dboard();
    show1();
    wrongmove = YES;
  }
  if (strips(s) == 'q') {     /* check for quit command */
    printf ("Aborting game:  Enter Y to confirm\n");
    if ((c = getche()) == 'y')
      goto quit;
    dboard();
    show1();
    wrongmove = YES;
  }
  if (strips(s) == 'b') {     /* check for print board command */
    cls();
    dboard();
    show1();
    wrongmove = YES;
  }
  if (strips(s) != '-' && !isdigit(strips(s))) {
    cls();
    dboard();
    printf ("\7You must move.  ");
    show1();
    wrongmove = YES;
  }
  /* convert commas to spaces */
  for (i = 0; s[i]; i++)
    if (s[i] == ',') 
      s[i] = ' ';

  n = sscanf(s,"%d%d%d%d%d",&go[0],&go[1],&go[2],&go[3],&go[4]);

  if ((die1  != die2 && n > 2) || n > 4) {
    cls();
    dboard();
    printf ("\7Too many moves.  ");
    show1();
    wrongmove = YES;
  }

  if (((die1  != die2) && n < 2) || ((die1 == die2) && n < 4)) {
    if (n == redcount()) 
      goto moveok;
    printf ("\7Ok to skip move(s)?");
    if ((c = getche()) != 'y') {
      cls();
      dboard();
      printf ("\7Enter your moves again:  ");
      show1();
      wrongmove = YES;
    }
    printf ("\n");
  }

moveok: 
  go[n] = NIL;
  if (*s == '-') {
    go[0] = -go[0];
    t = die1;
    die1 = die2;
    die2 = t;
  }
  for (k = 0; k<n; k++) {
    if (0 <= go[k] && go[k] <= 24)
      continue;
    else {
      cls();
      dboard();
      printf ("\7You can't move %d.  ",go[k]);
      show1();
      wrongmove = YES;
    }
  }
  if (play(red,white,go)) 
    wrongmove = YES;
  if (wrongmove == YES) {
    wrongmove = NO;
    goto retry;
  }
  if (piececount(red,0,24) == 0) {
    sleep();
    cls();
    printf ("\7You win . . .\n");
    ++ywin;
    score();
    if (mwin == ywin)
      printf ("\nLooks like now we're dead even . . . it's tiebreaker time!\n");
    else if (ywin > 2 && !mwin)
      printf ("\nYou're lucky I don't throw the dice. . . .\n");
    else message2();

again:    
    message4();
poll:
    c = getch();
    if (c == 'y') {
      cls();
      goto newgame;
    }
    else if (c != 'n') {
      printf("\7");
      goto poll;
    }
    else goto quit;
  }
  goto white;
quit:
  cls();
  exit(1);
}

/* Initialize the board and the probabilities */
init()
{
  int i;

  /* Clear off board and enter the starting positions of the pieces */
  for (i = 0; i <= 30; i++)
    red[i] = white[i] = 0;
  red[1] = white[1] = 2;
  red[12] = white[12] = 5;
  red[17] = white[17] = 3;
  red[19] = white[19] = 5;

  for (i = 0; i <= 30; i++)   /* Note:  Is this code necessary? */
    prob[i] = 0;

  prob[0] = 0;
  prob[1] = 11;
  prob[2] = 12;
  prob[3] = 13;
  prob[4] = 14;
  prob[5] = 15;
  prob[6] = 16;
  prob[7] =  6;
  prob[8] =  5;
  prob[9] =  4;
  prob[10] = 3;
  prob[11] = 2;
  prob[12] = 1;
}

/* STRATEGY SECTION.STRATEGY SECTION.STRATEGY SECTION.*/
/* STRATEGY SECTION.STRATEGY SECTION.STRATEGY SECTION.*/
/* STRATEGY SECTION.STRATEGY SECTION.STRATEGY SECTION.*/
/* STRATEGY SECTION.STRATEGY SECTION.STRATEGY SECTION.*/
/* STRATEGY SECTION.STRATEGY SECTION.STRATEGY SECTION.*/

int play(player,opponent,pos)
int *player,*opponent,pos[];
{
  int k,n,die,ipos;

  /* Blots on player[0] must be moved first */
  for (k = 0;k<player[0];k++) {
    if (pos[k] == NIL)
      break;
    if (pos[k] != 0) {
      printf ("\nPlay your BAR piece (on point 0) first\n");
      return(-1);
    }
  }

  for (k = 0;(ipos = pos[k]) != NIL;k++) {
    die = k?die2:die1;
    n = 25-ipos-die;
    if (player[ipos] == 0) 
      goto illegal;
    if (n>0 && opponent[n] >= 2) 
      goto illegal;
    if (n <= 0) {
      if (piececount(player,0,18) != 0) 
        goto illegal;
      if ((ipos+die) != 25 && piececount(player,19,24-die) != 0) 
        goto illegal;
    }
    player[ipos]--;
    player[ipos+die]++;
  }
  for (k = 0;pos[k] != NIL;k++) {
    die = k?die2:die1;
    n = 25-pos[k]-die;
    if (n>0 && opponent[n] == 1) {
      opponent[n] = 0;
      opponent[0]++;
    }
  }
  return(0);

illegal:
  while(k--) {
    die = k?die2:die1;
    player[pos[k]]++;
    player[pos[k]+die]--;
  }
  cls();
  dboard();
  printf ("\7You can't move %d.  Try again:\n",ipos);
  show1();
  return(-1);
}

/* Return true if player can move */
int canmove(player,opponent)
int *player, *opponent;
{
  int k;
  imoves = 0;
  movegen(player,opponent);
  if (die1 != die2) {
    k = die1;   
    die1 = die2;        
    die2 = k;
    movegen(player,opponent);
    k = die1;   
    die1 = die2;        
    die2 = k;
  }
  return imoves;
}

int nextmove(player,opponent)
int *player,*opponent;
{
  int k;
  if (die1 != die2) {
    k = die1;  
    die1 = die2;  
    die2 = k;
  }

  /* Select the kth possible move */
  k = strategy(player,opponent);
  dmove(k);
  update(player,opponent,k);
  return(0);
}

dmove(k)
int k;
{
  int cflag;                           /* Comma flag       */
  int i, n;

  cflag = 0;
  if (k == NIL)
    printf ("No move possible\n");
  else for (n = 0; n < 4; n++) {
    if (moves[k].pos[n] == NIL)
      break;
    if (cflag)                         /* Is comma required? */ 
      printf (",");
    i = 25 - moves[k].pos[n];
    if (i == 0 || i == 25)
      printf (" BAR-%d",moves[k].mov[n]);
    else
      printf (" %d-%d",i,moves[k].mov[n]);
    cflag = 1;
  }
  printf (".");
}

update(player,opponent,k)
int *player,*opponent,k;
{
  int n,t;
  for (n = 0;n<4;n++) {
    if (moves[k].pos[n] == NIL)
      break;
    player[moves[k].pos[n]]--;
    player[moves[k].pos[n]+moves[k].mov[n]]++;
    t = 25-moves[k].pos[n]-moves[k].mov[n];
    if (t>0 && opponent[t] == 1) {
      opponent[0]++;
      opponent[t]--;
    }
  }
}

movegen(mover,movee)
int *mover,*movee;
{
  int k;
  for (i = 0;i <= 24;i++) {
    count = 0;
    if (mover[i] == 0)
      continue;
    if ((k = 25-i-die1)>0 && movee[k] >= 2)
      if (mover[0]>0)
        break;
      else 
        continue;
    if (k <= 0) {
      if (piececount(mover,0,18) != 0)
        break;
      if ((i+die1) != 25 && piececount(mover,19,24-die1) != 0)
        break;
    }
    mover[i]--;
    mover[i+die1]++;
    count = 1;
    for (j = 0;j <= 24;j++) {
      if (mover[j] == 0)
        continue;
      if ((k = 25-j-die2)>0 && movee[k] >= 2)
        if (mover[0]>0)
          break;
        else 
          continue;
      if (k <= 0) {
        if (piececount(mover,0,18) != 0)
          break;
        if ((j+die2) != 25 && piececount(mover,19,24-die2) != 0)
          break;
      }
      mover[j]--;
      mover[j+die2]++;
      count = 2;
      if (die1 != die2) {
        moverec(mover);
        if (mover[0]>0)
          break;
        else continue;
      }
      for (l = 0;l <= 24;l++) {
        if (mover[l] == 0)
          continue;
        if ((k = 25-l-die1)>0 && movee[k] >= 2)
          if (mover[0]>0)
            break;
        else continue;
        if (k <= 0) {
          if (piececount(mover,0,18) != 0)
            break;
          if ((l+die2) != 25 && piececount(mover,19,24-die1) != 0)
            break;
        }
        mover[l]--;
        mover[l+die1]++;
        count = 3;
        for (m = 0;m <= 24;m++) {
          if (mover[m] == 0)
            continue;
          if ((k = 25-m-die1) >= 0 && movee[k] >= 2)
            if (mover[0]>0)
              break;
          else
            continue;
          if (k <= 0) {
            if (piececount(mover,0,18) != 0)
              break;
            if ((m+die2) != 25 && piececount(mover,19,24-die1) != 0)
              break;
          }
          count = 4;
          moverec(mover);
          if (mover[0]>0)
            break;
        }
        if (count == 3)
          moverec(mover);
        else {
          mover[l]++;
          mover[l+die1]--;
        }
        if (mover[0]>0)
          break;
      }
      if (count == 2)
        moverec(mover);
      else {
        mover[j]++;
        mover[j+die1]--;
      }
      if (mover[0]>0)
        break;
    }
    if (count == 1)
      moverec(mover);
    else {
      mover[i]++;
      mover[i+die1]--;
    }
    if (mover[0]>0)
      break;
  }
}

moverec(mover)
int *mover;
{
  int t;
  if (imoves >= MAXIMOVES) 
    goto undo;
  ;
  for (t = 0;t <= 3;t++)
    moves[imoves].pos[t] = NIL;
  switch(count) {
  case 4:
    moves[imoves].pos[3] = m;
    moves[imoves].mov[3] = die1;
  case 3:
    moves[imoves].pos[2] = l;
    moves[imoves].mov[2] = die1;
  case 2:
    moves[imoves].pos[1] = j;
    moves[imoves].mov[1] = die2;
  case 1:
    moves[imoves].pos[0] = i;
    moves[imoves].mov[0] = die1;
    imoves++;
  }
undo:
  switch(count) {
  case 4:
    break;
  case 3:
    mover[l]++;
    mover[l+die1]--;
    break;
  case 2:
    mover[j]++;
    mover[j+die2]--;
    break;
  case 1:
    mover[i]++;
    mover[i+die1]--;
  }
}

strategy(player,opponent)
int *player,*opponent;
{
  int k,n,nn,bestval,moveval,prob;
  n = 0;
  if (imoves == 0)
    return(NIL);
  goodmove[0] = NIL;
  bestval = -32000;
  for (k = 0;k<imoves;k++) {
    if ((moveval = eval(player,opponent,k,&prob))<bestval)
      continue;
    if (moveval>bestval) {
      bestval = moveval;
      n = 0;
    }
    if (n<MAXGMOV) {
      goodmove[n] = k;
      probmove[n++] = prob;
    }
  }
  if (level == 'e' && n>1) {
    nn = n;
    n = 0;
    prob = 32000;
    for (k = 0;k<nn;k++) {
      if ((moveval = probmove[k])>prob)
        continue;
      if (moveval<prob) {
        prob = moveval;
        n = 0;
      }
      goodmove[n] = goodmove[k];
      probmove[n++] = probmove[k];
    }
  }
  return(goodmove[(rand()%n)]);
}

int eval(player,opponent,k,prob)
int *player,*opponent,k,*prob;
{
  int newtry[31],newother[31],*r,*q,*p,n,sum,rear;
  int ii,lastwhite,lastred;
  *prob = sum = 0;
  r = player+25;
  p = newtry;
  q = newother;
  while(player<r) {
    *p++= *player++;
    *q++= *opponent++;
  }
  q = newtry+31;

  for (p = newtry+25;p<q;) *p++= 0;     /* Zero out spaces for hit pieces */
  for (n = 0;n<4;n++) {
    if (moves[k].pos[n] == NIL)
      break;
    newtry[moves[k].pos[n]]--;
    newtry[ii = moves[k].pos[n]+moves[k].mov[n]]++;
    if (ii<25 && newother[25-ii] == 1) {
      newother[25-ii] = 0;
      newother[0]++;
      if (ii <= 15 && level == 'e')
        sum++;  /*hit if near other's home*/
    }
  }
  for (lastred = 0;newother[lastred] == 0;lastred++);
  for (lastwhite = 0;newtry[lastwhite] == 0;lastwhite++);
  lastwhite = 25-lastwhite;
  if (lastwhite <= 6 && lastwhite<lastred)
    sum = 1000;

  /* Running game for expert level */
  if (lastwhite<lastred && level == 'e' && lastwhite > 6) {
    for (sum = 1000;lastwhite>6;lastwhite--)
      sum = sum-lastwhite*newtry[25-lastwhite];
  }

  /* Find opponent's rear piece */
  for (rear = 0;rear<25;rear++)
    if (newother[rear] != 0)
      break;
  q = newtry+25;

  /* Closed points are ok */
  for (p = newtry+1;p<q;)
    if (*p++ > 1)
      sum++;

  /* As much as possible, bear off pieces only if the inner board is safe */
  if (rear>5) { 
    q = newtry+31;
    p = newtry+25;

    /* Use one dice number for each piece so as to bear off economically  */
    for (n = 6;p<q;n--)
      sum += *p++ * n;
  }

  /* Blots past this point can't be hit */
  if (level != 'b') {
    r = newtry+25-rear;

    /* Blots are bad after the first six points   */
    for (p = newtry+7;p<r;)
      if (*p++== 1)
        sum--;

    /* Try not to be on first three points */
    q = newtry+3;
    for (p = newtry;p<q;)
      sum-= *p++;
  }

  for (n = 1;n <= 4;n++)
    *prob += n*getprob(newtry,newother,6*n-5,6*n);
  return(sum);
}

/* Return the probability (times 102) that pieces belonging to 'player' 
   between the player's 'start' and 'finish' points will be hit by a 
   piece belonging to 'opponent' */
int getprob(player,opponent,start,finish)
int *player,*opponent,start,finish;
{
  int k, n, sum;
  sum = 0;
  for ( ; start <= finish; start++) {
    if (player[start] == 1) {
      for (k = 1; k <= 12; k++) {
        if ((n = 25 - start - k) < 0)
          break;
        if (opponent[n] != 0)
          sum += prob[k];
      }
    }
  }
  return(sum);
}

/* Count pieces belonging to 'player' */
int piececount(player,startrow,endrow)
int *player,startrow,endrow;
{
  int sum;
  sum = 0;
  while(startrow <= endrow)
    sum+= player[startrow++];
  return(sum);
}

/* Count red pieces */
int redcount()
{
  int i, count;
  for (count = 0, i = 0; i < 25; i++) count += red[i];
  return count;
}

/* BOOKKEEPING.BOOKKEEPING.BOOKKEEPING.BOOKKEEPING.   */
/* BOOKKEEPING.BOOKKEEPING.BOOKKEEPING.BOOKKEEPING.   */
/* BOOKKEEPING.BOOKKEEPING.BOOKKEEPING.BOOKKEEPING.   */
/* BOOKKEEPING.BOOKKEEPING.BOOKKEEPING.BOOKKEEPING.   */

/* Generate two numbers, die1 and die2, for the dice */
roll()
{
  long iths;
  time(&iths);
  srand(iths);

  die1 = ((((rand()%6)+12)%6)+1);
  die2 = ((((rand()%6)+12)%6)+1);
}

/* Order the dice, larger die first */
order()
{
  int temp;
  if (die1 < die2) {
    temp = die1;
    die1 = die2;
    die2 = temp;
  }
}

/* Return first non-whitespace character in string s: */
char strips(s)
char *s;
{
  char u;
  while (isspace(u = *s++));
  return tolower(u);
}
