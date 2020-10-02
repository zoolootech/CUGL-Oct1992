/*
 * PACMAN  - written by Dave Nixon, AGS Computers Inc., July, 1981.
 *
*/

#include	"pacdefs.h"
 
main(argc, argv)
 char **argv;
{
	int tmp;		/* temp variables */
	int pac_cnt;
	int monstcnt;	/* monster number */
	struct pac *mptr;
	char gcnt[10];

	UpdSc = 0;		/* haven't updated score file yet */
	rawio();
	gscore();		/* read in score file */
	if (!setjmp(jbuf))
		instruct();	/* instructions first time only */
	init(argc, argv);		/* global init */
	for (pac_cnt = MAXPAC; pac_cnt > 0; pac_cnt--)
	{
redraw:
		clr();
		SPLOT(0, 45, "SCORE: ");
		SPLOT(21, 45, "gold left = ");
		POS(21, 57); printf("%6d", goldcnt);

		if (potion == TRUE)
		{
			SPLOT(3, 45, "COUNTDOWN: ");
		};
		pacsymb = PACMAN;
		killflg = FALSE;
		POS(22, 45); printf("delay = %6d", delay);

		/*
		 * PLOT maze
		 */
		for (tmp = 0; tmp < BRDY; tmp++)
		{
			SPLOT(tmp, 0, &(display[tmp][0]));
		};

		/* initialize a pacman */

		Pac.xpos = pacstart.xpos;	Pac.ypos = pacstart.ypos;
		Pac.dirn = pacstart.dirn;	Pac.speed = pacstart.speed;
		Pac.danger = pacstart.danger;	Pac.stat = pacstart.stat;

		PLOT(pacptr->ypos, pacptr->xpos, pacsymb);
		/* display remaining pacmen */
		for (tmp = 0; tmp < pac_cnt - 1; tmp++)
		{
			PLOT(23, (MAXPAC * tmp), PACMAN);
		};
		/*
		 * Init. monsters
	 	 */
		for (mptr = &monst[0], monstcnt = 0; monstcnt < MAXMONSTER; mptr++, monstcnt++)
		{
			mptr->xpos = MSTARTX + (2 * monstcnt);
			mptr->ypos = MSTARTY;
			mptr->speed = SLOW;
			mptr->dirn = DNULL;
			mptr->danger = FALSE;
			mptr->stat = START;
			PLOT(mptr->ypos, mptr->xpos, MONSTER);
		};
		rounds = 0;	/* timing mechanism */

		/* main game loop */
		do
		{
			if (rounds++ % MSTARTINTVL == 0)
			{
				startmonst();
			};
			pacman();
			if (killflg == TURKEY)
				break;
			for (monstcnt = 0; monstcnt < (MAXMONSTER / 2); monstcnt++)
			{
				monster(monstcnt);	/* next monster */
			};
			if (killflg == TURKEY)
				break;
			if (pacptr->speed == FAST)
			{
				pacman();
				if (killflg == TURKEY)
					break;
			};
			for (monstcnt = (MAXMONSTER / 2); monstcnt < MAXMONSTER; monstcnt++)
			{
				monster(monstcnt);	/* next monster */
			};
			if (killflg == TURKEY)
				break;
			if (potion == TRUE)
			{
				POS(3, 60); printf("%2d%c", potioncnt,
					((potioncnt < 5) ? BEEP : ' '));

				if (potioncnt-- < 0)
				{
					SPLOT(3, 45,
					  "                        ");
					potion = FALSE;
					pacptr->speed = SLOW;
					pacptr->danger = FALSE;
					for (monstcnt = 0; monstcnt < MAXMONSTER; monstcnt++)
					{
						monst[monstcnt].danger = TRUE;
					};
				};
			};
			update();	/* score display etc */
			if (goldcnt <= 0)
			{
				reinit();
				goto redraw;
			};
		} while (killflg != TURKEY);
		SPLOT(5, 45, "YOU ARE BEING EATEN");
		SPLOT(6, 45, "THIS TAKES ABOUT 2 SECONDS");
		sleep(2);
	};
	SPLOT(8, 45, "THE MONSTERS ALWAYS TRIUMPH");
	SPLOT(9, 45, "IN THE END!");
	over();
}

pacman()
{
	int sqtype;
	int mcnt;
	int tmpx, tmpy;
	int deltat;
	char dir, dir1, dir2;
	struct pac *mptr;

	/* pause; wait for the player to hit a key */
	for (deltat = delay; deltat > 0; deltat--);

	/* get instructions from player, but don't wait */
	poll(0);

	/* remember current pacman position */
	tmpx = pacptr->xpos;
	tmpy = pacptr->ypos;

	/* "eat" any gold */
	/* update display array to reflect what is on terminal */
	display[tmpy][tmpx] = VACANT;


	/* what next? */
	dir1 = pacptr->dirn;	dir2 = pacptr->dirx;
again:	dir = dir1; dir1 = dir2; dir2 = DNULL;
	switch (dir)
	{
	case DUP:
		pacsymb = PUP;
		switch (sqtype = display[tmpy + UPINT][tmpx])
		{
		case GOLD:
		case VACANT:
		case CHOICE:
		case POTION:
		case TREASURE:
			pacptr->dirx = dir;	/* remember last dir */

			/* erase where the pacman went */
			PLOT(tmpy, tmpx, VACANT);
			pacptr->ypos += UPINT;
			break;

		default:
			goto again;
		};
		break;
	case DDOWN:
		pacsymb = PDOWN;
		switch (sqtype = display[tmpy + DOWNINT][tmpx])
		{
		case GOLD:
		case VACANT:
		case CHOICE:
		case POTION:
		case TREASURE:
			pacptr->dirx = dir;	/* remember last dir */

			/* erase where the pacman went */
			PLOT(tmpy, tmpx, VACANT);
			pacptr->ypos += DOWNINT;
			break;

		default:
			goto again;
		};
		break;
	case DLEFT:
		if(tmpx == 0)
		{
			/* erase where the pacman went */
			PLOT(tmpy, tmpx, VACANT);
			pacptr->xpos = XWRAP;
			sqtype = VACANT;
			break;
		};
		pacsymb = PLEFT;
		switch (sqtype = display[tmpy][tmpx + LEFTINT])
			{
		case GOLD:
		case VACANT:
		case CHOICE:
		case POTION:
		case TREASURE:

			pacptr->dirx = dir;	/* remember last dir */
			/* erase where the pacman went */
			PLOT(tmpy, tmpx, VACANT);
			pacptr->xpos += LEFTINT;
			break;
		
		default:
			goto again;
		};
		break;
	case DRIGHT:
		if(tmpx == XWRAP)
		{
			/* erase where the pacman went */
			PLOT(tmpy, tmpx, VACANT);
			pacptr->xpos = 0;
			sqtype = VACANT;
			break;
		};
		pacsymb = PRIGHT;
		switch (sqtype = display[tmpy][tmpx + RIGHTINT])
		{
		case GOLD:
		case VACANT:
		case CHOICE:
		case POTION:
		case TREASURE:

			pacptr->dirx = dir;	/* remember last dir */
			/* erase where the pacman went */
			PLOT(tmpy, tmpx, VACANT);
			pacptr->xpos += RIGHTINT;
			break;

		default:
			goto again;
		};
		break;
	};

	/* did the pacman get any points or eat a potion? */
	switch (sqtype)
	{
	case CHOICE:
	case GOLD:
		pscore++;
		goldcnt--;
		break;

	case TREASURE:
		pscore += TREASVAL;
		break;

	case POTION:
		SPLOT(3, 45, "COUNTDOWN: ");
		potion = TRUE;
		potioncnt = POTINTVL;
		pacptr->speed = FAST;
		pacptr->danger = TRUE;

		/* slow down monsters and make them harmless */
		mptr = &monst[0];
		for (mcnt = 0; mcnt < MAXMONSTER; mcnt++)
		{
			if (mptr->stat == RUN)
			{
				mptr->speed = SLOW;
				mptr->danger = FALSE;
			};
			mptr++;
		};
		break;
	};

	/* did the pacman run into a monster? */
	for (mptr = &monst[0], mcnt = 0; mcnt < MAXMONSTER; mptr++, mcnt++)
	{
		if ((mptr->xpos == pacptr->xpos) &&
			(mptr->ypos == pacptr->ypos))
		{

			killflg = dokill(mcnt);
		}
		else
		{
			killflg = FALSE;
		};
	};
	if (killflg != TURKEY)
	{
		PLOT(pacptr->ypos, pacptr->xpos, pacsymb);
	};
}
