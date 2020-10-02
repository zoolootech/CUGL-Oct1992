
/* HEADER:       (cat #);
   TITLE:        Mastermind - number guessing game with skill levels;
   DATE:         06/08/1989;
   DESCRIPTION:  "Try to guess a random number of a specified number of digits.
                 The computer tells you how many digits you have correct, each
                 time, and how many of them are in the correct position.";
   KEYWORDS:     mastermind, game.;
   SYSTEM:       MS-DOS;
   FILENAME:     MIND.C;
   AUTHOR:       Eric Horner;
   COMPILERS:    Turbo C 2.0;
*/



#include    <stdio.h>
#include    <stdlib.h>
#include    <conio.h>
#include    <time.h>

#define	    BSpace	'\x8'

static  char    rnd_num[8],
	rnd_flag[] = {'N','N','N','N','N','N','N','N'},
	in_flag[]  = {'N','N','N','N','N','N','N','N'},
	in_num[8], char_in;
int     loops, result_1, result_2, inner,
	num_digs, attempts;
char    reply, max_dig;

void menu()
{
    clrscr();
    printf("                 ษอออออออออออออออออออออออออออออออออออออออป\n");
    printf("                 บ              PC MASTERMIND            บ\n");
    printf("                 วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ\n");
    printf("                 บ Try to guess the number. The computer บ\n");
    printf("                 บ will give you hints if you guess well บ\n");
    printf("                 บ Press Q to QUIT any time you need to! บ\n");
    printf("                 บ Written by Eric Horner       Apr 1989 บ\n");
    printf("                 ศอออออออออออออออออออออออออออออออออออออออผ\n\n");

    printf("                  1: JERK   (3 Figure number - 0 to 5)\n\n");

    printf("                  2: NOVICE (3 Figure number - 0 to 9)\n\n");

    printf("                  3: TRYER  (4 Figure number - 0 to 5)\n\n");

    printf("                  4: NORMAL (4 Figure number - 0 to 9)\n\n");

    printf("                  5: BRAINY (5 Figure number - 0 to 9)\n\n");

    printf("                  S: SADIST (Set someone a number)\n\n");

    printf("                  Q: QUITTER\n\n");

    printf("                  Enter your SKILL LEVEL (1 to 5, S or Q)\n");

    do          /* until valid character (1 to 7) */
    {
        reply = getch();
        if (((reply <= '0') || (reply > '7'))
            && (reply != 'Q') && (reply != 'q')
            && (reply != 'S') && (reply != 's')) printf("\07");

    } while (((reply <= '0') || (reply > '7'))
            && (reply != 'Q') && (reply != 'q')
            && (reply != 'S') && (reply != 's'));

    switch (reply)
    {
	case '1': {
		    num_digs = 3;
		    max_dig  = '5';
                    break;
                  }
	case '2': {
		    num_digs = 3;
		    max_dig  = '9';
                    break;
                  }
	case '3': {
		    num_digs = 4;
		    max_dig  = '5';
                    break;
                  }
	case '4': {
		    num_digs = 4;
		    max_dig  = '9';
                    break;
                  }
	case '5': {
		    num_digs = 5;
		    max_dig  = '9';
                    break;
                  }
        case 'Q':
        case 'q': {
                    exit(0);            /* Quit program */
                    break;
                  }
        case 'S':
	case 's': {
		    num_digs = 0;	/* This means user selected */
		    max_dig  = '9';
                    break;
                  }
        default : {
                    break;
                  }
    } /* End of switch */

} /* End of menu() */


main()
{
    randomize();            /* set up rnd gen       */
    do
    {
        clrscr();
	menu();                             /* Get players skill level */
	clrscr();

	if (num_digs == 0)
	{
	    printf("%s\n\n",
		   "TELL YOUR OPPONENT NOT TO LOOK, THEN ENTER YOUR NUMBER");
	    /* get user selected number - up to 7 digits */
	    for (loops = 0; loops < 7 && num_digs == 0; ++loops)
	    {
		do
		{
		    char_in = getche();
                    if ((char_in == 'Q') || (char_in == 'q')) exit(0);
		    if (((char_in < '0') || (char_in > '9')) &&
			 (char_in != BSpace) &&
			 (char_in != '\r'))
		    {
			 printf("\07");
			 putch(BSpace);
		    }

		    if ((char_in == BSpace) && (loops > 0))
			--loops;
		} while (((char_in < '0') || (char_in > '9'))
			&& (char_in != '\r'));
		if (char_in != '\r') rnd_num[loops] = char_in;
		else num_digs = loops;
		if (loops == 6) num_digs = loops + 1;
	    }
	}
	else
	{
	    /* get random number (in ASCII) */
	    for (loops = 0; loops < num_digs; ++loops)
		rnd_num[loops] = random(max_dig - 0x30 + 1) + 0x30;
	}
	clrscr();
	printf("ENTER YOUR GUESS NOW (USE BACKSPACE TO CORRECT ERRORS)\n\n");
	attempts = 0;

	do
        {
	    ++attempts;			/* Keep score */
	    result_1 = 0;
            result_2 = 0;

	    for (loops = 0; loops < 8; ++loops)
	    {
		in_flag[loops]  = 'N';
		rnd_flag[loops] = 'N';
	    }

	    /* Get next guess */
	    for (loops = 0; loops < num_digs; ++loops)
            {
                do
                {
                    char_in = getche();
                    if ((char_in == 'Q') || (char_in == 'q')) exit(0);
		    if (((char_in < '0') || (char_in > max_dig)) &&
			 (char_in != BSpace))
		    {
			 printf("\07");
			 putch(BSpace);
		    }
		    if ((char_in == BSpace) && (loops > 0))
			--loops;
		} while ((char_in < '0') || (char_in > max_dig));
                in_num[loops] = char_in;
            }




	    /* How many right and in the right place? */
	    for (loops = 0; loops < num_digs; ++loops)
            {
                if (rnd_num[loops] == in_num[loops])
                {
                    ++result_1;     /* right number, right place    */
                    rnd_flag[loops] = 'Y';
                    in_flag[loops]  = 'Y';
                }
	    }
	    /* How many right but in the wrong place? */
	    for (loops = 0; loops < num_digs; ++loops)
            {
		for (inner = 0; inner < num_digs; ++inner)
                {
                    if ((rnd_num[loops] == in_num[inner])
                        && (rnd_flag[loops] != 'Y')
                        && (in_flag[inner]  != 'Y'))
                    {
                        rnd_flag[loops] = 'Y';
                        in_flag[inner]  = 'Y';
                        ++result_2;     /* right number, wrong place    */
                    }
                }
            }
            printf("\t%s%d\t%s%d\n",
                   "Right number, right place : ", result_1,
                   "Right number, wrong place : ", result_2);
	} while (result_1 < num_digs);

	if (attempts == 1)
	    printf("\07\nLUCKY GUESS!!!");
	else printf("\07\n%s%d%s",
		    "WELL DONE! You got it all right in ",
		     attempts, " attempts!");
	printf("\n\nPress <Q> to quit, any other key to play again!");
        char_in = getche();

    } while ((char_in != 'Q') && (char_in != 'q'));
}
