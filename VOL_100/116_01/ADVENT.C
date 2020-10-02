
/*  program ADVENT.C    modified by LCC for V 1.43 by:
	altering buffer sizes
*/
#include "advent.h"

main(argc,argv)
char **argv;
{
	int rflag;			/* user restore request option */

	rflag = 0;
	while (--argc > 0) {
		++argv;
		if (**argv != '-')
			break;
		switch(tolower(argv[0][1])) {
		case 'r':
			++rflag;
			continue;
		case 'd':
			++dbgflg;
			continue;
		default:
			printf("unknown flag: %c\n",argv[0][1]);
			continue;
		}
	}
	init();
	if (rflag)
		restore();
	exec("eadvent");
}

/*
	Initialization
*/
init()
{
	int c;
	int i,j,offset;
	char *wptr, *wordptr;
	char word[WORDSIZE];
	int wval;

	turns=0;

	/* initialize location status array */
	setmem(cond,MAXLOC,0);
	initb(cond,"0,5,1,5,5,1,1,5,17,1,1");
	initb(&cond[11],"0,0,32,0,0,2,0,0,64,2");
	initb(&cond[21],"2,2,0,6,0,2,0,0,0,0");
	initb(&cond[31],"2,2,0,0,0,0,0,4,0,2");
	initb(&cond[41],"0,128,128,128,128,136,136,136,128,128");
	initb(&cond[51],"128,128,136,128,136,0,8,0,2");
	initb(&cond[80],"128,128,136,0,0,8,136,128,0,2,2");
	initb(&cond[91],"0,0,0,0,4,0,0,0,0,1");
	initb(&cond[111],"0,0,4,0,1,1,0,0,0,0");
	initb(&cond[121],"0,8,8,8,8,8,8,8,8,8");
	cond[79]=2;

	/* initialize object locations */
	setmem(place,2*MAXOBJ,0);
	initw(&place[1],"3,3,8,10,11,0,14,13,94,96");
	initw(&place[11],"19,17,101,103,0,106,0,0,3,3");
	initw(&place[21],"0,0,109,25,23,111,35,0,97,0");
	initw(&place[31],"119,117,117,0,130,0,126,140,0,96");
	initw(&place[50],"18,27,28,29,30,0,92,95,97,100,101,0,119,127,130");

	/* initialize second (fixed) locations */
	setmem(fixed,2*MAXOBJ,0);
	initw(&fixed[1],"0,0,9,0,0,0,15,0,-1");
	initw(&fixed[11],"-1,27,-1,0,0,0,-1");
	initw(&fixed[21],"0,0,-1,-1,67,-1,110,0,-1,-1");
	initw(&fixed[31],"121,122,122,0,-1,-1,-1,-1,0,-1");
	fixed[62]=121;
	fixed[64]=-1;

	/* initialize default verb messages */
	initb(actmsg,"0,24,29,0,33,0,33,38,38,42,14");
	initb(&actmsg[11],"43,110,29,110,73,75,29,13,59,59");
	initb(&actmsg[21],"174,109,67,13,147,155,195,146,110,13,13");

	/* initialize various flags and other variables */
	setmem(visited,MAXLOC,0);
	setmem(prop,2*MAXOBJ,0);
	setmem(&prop[50],2*(MAXOBJ-50),0xff);
	wzdark=closed=closing=holding=detail=0;
	limit=100;
	tally=15;
	tally2=0;
	newloc=3;
	loc=oldloc=oldloc2=1;
	knfloc=0;
	chloc=114;
	chloc2=140;
	initw(dloc,"0,19,27,33,44,64");
	initw(odloc,"0,0,0,0,0,0,0");
	dloc[DWARFMAX-1]=chloc;
	dkill=0;
	initb(dseen,"0,0,0,0,0,0,0");
	clock=30;
	clock2=50;
	panic=0;
	bonus = 0;
	numdie = 0;
	daltloc = 18;
	lmwarn = 0;
	foobar = 0;
	dflag = 0;
	gaveup = 0;
	saveflg = 0;
	dbgflg = 0;
	printf("Go read a book while I get my act together...\n");
	/*
	   initialize disk index arrays
	*/
	for(i=0;i<MAXLOC/10;++i)
		idx1[i]=idx2[i]=idx3[i]=0;
	for(i=0;i<30;++i)
		idx6[i]=0;
	fd[1]=fopen("advent1.dat",dbuff);
	fd[2]=fopen("advent2.dat",dbuff);
	fd[3]=fopen("advent3.dat",dbuff);
	fd[4]=fopen("advent4.dat",dbuff);
	fd[6]=fopen("advent6.dat",dbuff);
	for(i=1;i<=3;++i) {
		swchfd(i);
		if(fseek(dbuff,0,0)==-1)
			bug(42);
		offset=0;
		for(j=0;j<141;++j) {
			while((c=getc(dbuff))!='#') {
				if(c==-1 || c==EOF)
					bug(40);
				++offset;
			}
			if(j%10 == 0) {
				switch(i) {
				case 1:
					idx1[j/10]=offset;
					break;
				case 2:
					idx2[j/10]=offset;
					break;
				case 3:
					idx3[j/10]=offset;
					break;
				default:
					break;
				}
			}
		++offset;
		}
	}
	offset=0;
	swchfd(6);
	if(fseek(dbuff,0,0)==-1)
		bug(43);
	for(j=0;j<201;++j) {
		while((c=getc(dbuff))!='#') {
			if(c==-1 || c==EOF)
				bug(41);
			++offset;
		}
		if(j%10 == 0) {
			idx6[j/10]=offset;
		}
		++offset;
	}
	/*
		initialize fast verb arrays
	*/
	swchfd(4);
	if (fseek(dbuff,0,0) == -1)
		bug(46);
	fastvseek = 0;
	wptr = fastverb;
	wval = 0;
	while(wptr < &fastverb[MAXVFAST-WORDSIZE] &&
	      wval < MAXVVAL) {
		while((c=getc(dbuff)) != ',') {
			if (c == -1 || c == EOF)
				bug(44);
			++fastvseek;
			*wptr++ = c;
		}
		++fastvseek;
		*wptr++ = 0;
		wordptr = word;
		while((c=getc(dbuff)) != '\n') {
			if (c == -1 || c == EOF)
				bug(45);
			++fastvseek;
			*wordptr++ = c;
		}
		++fastvseek;
		*wordptr++ = 0;
		fastvval[wval++] = atoi(word);
	}
 }

/*
	restore saved game handler
*/
restore()
{
	char username[13];
	int restfd,c;
	char *sptr;

	printf("What is your saved game name? ");
	scanf("%s",username);
	strcat(username,".adv");
	if ((restfd=fopen(username,dbuff)) == -1) {
		printf("sorry, can''t open save file...\n");
		exit();
	}
	for (sptr=&turns; sptr<&lastglobal; sptr++) {
		if ((c=getc(dbuff)) == -1) {
			printf("can''t read save file...\n");
			exit();
		}
		*sptr = c;
	}
	if (close(restfd) == -1) {
		printf("warning -- can''t close save file...\n");
	}
	saveflg = 0;
}
