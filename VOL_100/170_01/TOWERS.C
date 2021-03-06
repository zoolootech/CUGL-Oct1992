#include <stdio.h>

/*
 *                 IBM RBBS-PC Tulsa, OK  
 *                 Switching totally to the "C" Language
 *                 24 Hour operation 300/1200 baud XMODEM
 *                 918-664-8737
 *                 SYSOP LYNN LONG
 *
 *                  Towers of Hanoi
 *
 * The object of the game is to transfer the disks from
 * the leftmost tower to the rightmost tower obeying the
 * following set of rules:
 * 
 * 1) Only the top disk of any tower may be moved at a time
 * 2) At no time must a larger disk be placed on a smaller disk
 *
 */

#define FORMFEED		0x0c
#define ESC				0x1b

#define POST			0xba
#define POST_BASE		0xca
#define BASE			0xcd
#define RING			0xdc

#define SCREEN_WIDTH	80
#define SCREEN_HEIGHT	25

#define RING_WIDTH		((((SCREEN_WIDTH - 2)/3) & 0xfe)-1)
#define LEFT_POST		(RING_WIDTH/2+1)
#define CENTER_POST		(LEFT_POST+RING_WIDTH)
#define RIGHT_POST		(LEFT_POST+2*RING_WIDTH)

#define MOVING_ROW		2
#define BASE_ROW		15
#define POST_HEIGHT		11

char top[3] { BASE_ROW-1, BASE_ROW-1, BASE_ROW-1 };
int pause 0;


main(argc, argv)
int argc;
char *argv[];
	{
	int nrings;

	if(argc < 1  ||  argc > 3)
		abort("Use: hanoi [rings [delay]]\n");

	nrings = argc > 1 ? atoi(argv[1]) : 7;
	pause = argc > 2 ? atoi(argv[2]) : 1;

	setup(nrings);
	hanoi(nrings, 0, 2, 1);

	curse(0, SCREEN_HEIGHT-1);
	}


hanoi(n, a, b, c)
char n, a, b, c;
	{
	if(n == 0)
		return;

	hanoi(n-1, a, c, b);
	movering(n, a, b);
	hanoi(n-1, c, b, a);
	}


setup(n)
char n;
	{
	char i;

	putchar(FORMFEED);

	for(i = MOVING_ROW+2; i < BASE_ROW; ++i) {
		cput(LEFT_POST, i, POST);
		cput(CENTER_POST, i, POST);
		cput(RIGHT_POST, i, POST);
		}

	curse(0, BASE_ROW);

	for(i = 1; i < SCREEN_WIDTH; ++i)
		putchar(BASE);

	cput(LEFT_POST, BASE_ROW, POST_BASE);
	cput(CENTER_POST, BASE_ROW, POST_BASE);
	cput(RIGHT_POST, BASE_ROW, POST_BASE);

	for(i = n; i > 0; --i)
		draw(i, LEFT_POST, top[0]--, RING);
	}


curse(x, y)
char x, y;
	{

	wait();

	putchar(ESC);
	putchar('=');
	putchar(y + ' ');
	putchar(x + ' ');
	}


cput(x, y, ch)
char ch;
int x,y;
	{
	putchar(ESC);
	putchar('=');
	putchar(y + ' ');
	putchar(x + ' ');
	putchar(ch);
	wait();
	}


draw(ring, centre, y, ch)
char ring, centre, y, ch;
	{
	char i;

	curse(centre-ring, y);

	for(i=0; i<ring; ++i)
		putchar(ch);

	curse(centre+1, y);

	for(i=0; i<ring; ++i)
		putchar(ch);
	}


movering(ring, from, to)
char ring, from, to;
	{
	char fromc, toc;
	char fromy, toy;

	fromc = LEFT_POST + from * RING_WIDTH;
	toc = LEFT_POST + to * RING_WIDTH;
	fromy = ++top[from];
	toy = top[to]--;

	while(fromy != MOVING_ROW) {
		draw(ring, fromc, fromy, ' ');
		draw(ring, fromc, --fromy, RING);
		}

	if(fromc < toc)
		while(fromc != toc) {
			cput(fromc-ring, fromy, ' ');
			cput(fromc, fromy, RING);
			cput(fromc+1, fromy, ' ');
			cput(fromc+ring+1, fromy, RING);
			++fromc;
			}
	else if (fromc > toc)
		while(fromc != toc) {
			cput(fromc+ring, fromy, ' ');
			cput(fromc, fromy, RING);
			cput(fromc-1, fromy, ' ');
			cput(fromc-ring-1, fromy, RING);
			--fromc;
			}

	while(fromy != toy) {
		draw(ring, fromc, fromy, ' ');
		draw(ring, fromc, ++fromy, RING);
		}
	}


wait() {
	int i;

	i = 1 << pause;

	while(--i)
		i = i;
	}

                               