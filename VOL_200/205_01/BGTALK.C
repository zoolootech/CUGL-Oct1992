
/*
HEADER:         CUG205.00;
TITLE:          BGTALK.C - Backgammon II (for MS-DOS);
VERSION:        2.00;
DATE:           10/10/86;
DESCRIPTION:    "Conversation module for Backgammon II.";
KEYWORDS:       games, backgammon;
SYSTEM:         MS-DOS;
FILENAME:       BG.C;
WARNINGS:       "The author claims the copyright to the MS-DOS version and
                authorizes non-commercial use only.";
SEE-ALSO:       CUG102, BG.C, BGDISP.C;
AUTHORS:        Michael Yokoyama;
COMPILERS:      Microsoft v. 3.00;
*/

extern char c, level;

help()
{
  cls();
  printf("This is a simulation of a backgammon game with no doubling cube.  The computer\n");
  printf("will play white, you will play red.  In this game, the backgammon board has\n");
  printf("numbers (from 1 to 24) to represent the different points (triangles) on the\n");
  printf("standard board, plus letters and numbers to show how many pieces, and of which\n");
  printf("color, are at each point.  The bar point is represented by the number 0.\n\n");
  printf("White's Home                         (ME)\n\n");
  printf("    1     2     3     4     5     6         7     8     9    10    11    12\n");
  printf(" =============================================================================\n");
  printf("    R                             W   ||          W                       R\n");
  printf("    2                             5   ||          3                       5\n");
  printf("                                      ||\n\n");
  printf("                                      ||\n");
  printf("                1     1           5   ||          3                       5\n");
  printf("                W     W           R   ||          R                       W\n");
  printf(" =============================================================================\n\n");
  printf("   24    23    22    21    20    19        18    17    16    15    14    13\n\n");
  printf("Red's Home                           (YOU)\n\n");
  printf("Press any key to continue instructions:\n");
  getch();
  cls();
  printf("PLAYING BACKGAMMON\n\n");
  printf("When it is your turn, the computer will roll the dice for you, and you will\n");
  printf("be asked to enter a move.  To move, type the numbers of the points (a number\n");
  printf("from 0 to 24) where the pieces are to be moved.\n");
  printf("\n");
  printf("For example, if your roll was '5,3', and you want to advance a piece on\n");
  printf("point 12 five points, and a piece on point 6 three points, you would\n");
  printf("enter '12,16' as your move.\n\n");
  printf("ORDERING THE DICE\n\n");
  printf("To simplify the bookkeeping, it is assumed that you will take the move\n");
  printf("corresponding to the higher number first.  Sometimes, however, that will make\n");
  printf("it impossible to move off the bar, or make a move.\n\n");
  printf("If you want to move in the opposite order (take the lower number first)\n");
  printf("type a minus as the first character.\n\n");
  printf("For example, in the above case where your roll was '5 3', and you wish to\n");
  printf("advance a piece on point 12 THREE points, and the piece on point 6 FIVE\n");
  printf("points, you would enter '-12 6'.\n\n");
  printf("Press any key to continue instructions:\n");
  getch();
  cls();
  printf("MULTIPLE MOVES\n\n");
  printf("To move a single piece more than once, enter the point over where it will pass.\n");
  printf("For example, if you had rolled '2 2' (double two's), and you want to move the\n");
  printf("piece on point 14 four times, enter '14 16 18 20' as your move.\n\n");
  printf("DOUBLES\n\n");
  printf("Instead of entering two numbers, you get to enter four numbers.\n\n");
  printf("SKIPPING MOVES\n\n");
  printf("You may skip your turn by typing a 'new-line' all by itself.\n\n");
  printf("THE BAR POINT\n\n");
  printf("If your opponent hits your piece removes it to the bar, you must bring your\n");
  printf("piece back into play before you make any other move.  In this game, the bar\n");
  printf("is represented by point 0.\n\n");
  printf("Press any key to continue instructions:\n");
  getch();
  cls();
  printf("ILLEGAL MOVES\n\n");
  printf("You cannot move to a point where your opponent has more than one piece;\n");
  printf("take too many moves; or remove pieces from your inner board until all your\n");
  printf("pieces are first there.\n\n");
  printf("OTHER COMMANDS\n\n");
  printf("    ? or h             Display this help screen\n");
  printf("      b                Review the board\n");
  printf("      q                Quit the current game\n\n");
  printf("Press any key to get back to the game:\n");
  getch();
  cls();
}

title()
{
  cls();
  printf ("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
  printf ("=-=-=-=-=-=-=-=-=-=-=-=-=   Welcome to BACKGAMMON!        =-=-=-=-=-=-=-=-=-=-=\n");
  printf ("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");

  printf ("              Backgammon written by Leor Zolman, February, 1982\n");
  printf ("             PC/MS-DOS version by Michael Yokoyama, October, 1986\n");
  printf ("        Source code provided by The C User's Group, McPherson, Kansas\n\n");

  printf ("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");
}

helpscrn()
{
  printf ("\nBackgammon commands:\n");
  printf ("   ? or h           Display this help screen\n");
  printf ("   [n][ n]          Move piece at number n (for normal roll)\n");
  printf ("   [n][ n][ n][ n]  Move piece at number n (for doubles roll)\n");
  printf ("   b                Draw the board\n");
  printf ("   q                Quit the current game\n");
}

select()
{
  printf ("\n\n\n\n\n\n\nChoose the game level:\n\n");
  printf ("      N = Novice\n");
  printf ("      I = Intermediate\n");
  printf ("      E = Expert\n\n");
  level = 'e';
  if ((c = getche()) == 'n') {
    level = 'b';
    header();
    printf ("\n\nNovice\n\n");
  }
  else if (c == 'i') {
    header();
    printf ("\n\nIntermediate\n\n");
    level = 'i';
  }
  else {
    header();
    printf ("\n\nExpert\n\n");
  }
}

message0()
{
  switch(rand() % 5) {
  case 4:
  case 0:     
  printf ("\n\n\nLet's see who gets to go first:\n\n");
    break;
  case 1:     
    printf ("\n\n\nLet's roll for first turn:\n\n");
    break;
  case 2:     
    printf ("\n\n\nRoll one die for the first turn:\n\n");
    break;
  case 3:     
    printf ("\n\n\nOk, the higher die moves first:\n\n");
    break;
  }
}

message1()
{
  switch(rand() % 5) {
  case 4:
  case 0:     
    printf ("Maybe you'd better stick to Adventure games?\n");
    break;
  case 1:     
    printf ("Not bad for an AI experiment, eh?\n");
    break;
  case 2:     
    printf ("Hee hee hee.  Better luck next time. . .\n");
    break;
  case 3:     
    printf ("Aren't you ashamed to have been beaten by a computer?\n");
    break;
  }
}

message2()
{
  switch(rand() % 5) {
  case 0:     
    printf ("\nNext time I'll put lead in the dice . . . .\n");
    break;
  case 1:     
    printf ("\nHohum . . . next time get Babbage to program me.\n");
    break;
  case 2: 
    printf ("\nIf I lose too many times, I'll format your hard disk (joke).\n");
    break;
  case 3:     
    printf ("\nMumble . . . those unlucky rolls will do it every time!\n");
    break;
  case 4:     
    printf ("\nNot bad for a person who always gets the good rolls!\n");
  }
}


message3()
{
  switch(rand() % 5) {
  case 0:     
    printf ("Enter your move:\n");
    break;
  case 1:     
    printf ("Your move:\n");
    break;
  case 2: 
    printf ("Move, please:\n");
    break;
  case 3:     
    printf ("Your turn:\n");
    break;
  case 4:     
    printf ("Move:\n");
  }
}

message4()
{
  printf ("\n");
  switch(rand() % 5) {
  case 0:     
    printf ("Let's play another game, ok");
    break;
  case 1:     
    printf ("Will you have another round");
    break;
  case 2: 
    printf ("Try your luck again");
    break;
  case 3:     
    printf ("How about another one");
    break;
  case 4:     
    printf ("How about another game");
  }
  printf ("? (Y/N)\n\n\n");
}

message5()
{
  switch(rand() % 5) {
  case 0:     
    printf ("I moved: ");
    break;
  case 1:     
    printf ("My move was: ");
    break;
  case 2: 
    printf ("I went: ");
    break;
  case 3:     
    printf ("I played: ");
    break;
  case 4:     
    printf ("My move: ");
  }
}

message6()
{
  switch(rand() % 5) {
  case 0:     
    printf ("These dice must be fixed.  I pass.");
    break;
  case 1:     
    printf ("What lousy luck.  Hurry up and move!");
    break;
  case 2: 
    printf ("Out of the goodness of my heart I pass.");
    break;
  case 3:     
    printf ("I lose my turn.");
    break;
  case 4:     
    printf ("Oh, (expletive deleted), now I have to pass.");
  }
}

message7()
{
  switch(rand() % 5) {
  case 0:     
    printf ("Ha, ha, ha, you must pass.  Press space:\n");
    break;
  case 1:     
    printf ("You have to pass.  Press the space bar:\n");
    break;
  case 2: 
    printf ("I got you now!  Type a space:\n");
    break;
  case 3:     
    printf ("You lose your turn.  Press the space bar:\n");
    break;
  case 4:     
    printf ("Too bad we can't double . . . .  Press space\n");
  }
}
