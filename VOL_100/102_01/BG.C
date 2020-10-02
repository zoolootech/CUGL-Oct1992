/*
	Backgammon...finally, some"one" else to play against than just
	my girlfriend, who hates to lose! (but then, so do I...and this
	program doesn't do too bad a job at keeping me honest)

	Modified for BDS C by Leor Zolman, 2/82
*/

#include <bdscio.h>

#define H19 1		/* true if to be run on H19 terminal */
#define NIL (-1)
#define MAXGMOV 10
#define MAXIMOVES 1000

char level;		/*'b'=beginner, 'i'=intermediate, 'e'=expert*/
int die1;
int die2;
int i;
int j;
int l;
int m;
int count;

int red[31];
int white[31];
int probability[13];

int imoves;
int goodmoves[MAXGMOV];
int probmoves[MAXGMOV];

struct {
	int pos[4];
	int mov[4];
} moves[MAXIMOVES];


exinit()
{
	int i;
	setmem(externs(), endext() - externs(), 0);
	red[1] = white[1] = 2;
	red[12] = white[12] = 5;
	red[17] = white[17] = 3;
	red[19] = white[19] = 5;
	probability[0] = 0;
	probability[1] = 11;
	probability[2] = 12;
	probability[3] = 13;
	probability[4] = 14;
	probability[5] = 15;
	probability[6] = 16;
	probability[7] = 6;
	probability[8] = 5;
	probability[9] = 4;
	probability[10] = 3;
	probability[11] = 2;
	probability[12] = 1;
}

main()
{
	int t,k,n,go[5];
	char c, s[MAXLINE];
	int i;
	int first;	/* true on first move only */
	int mywins, hiswins;

	printf("\n\nWelcome to BACKGAMMON!\n");

	nrand (0,"Do you need instructions? ");
	if(tolower(getchar())=='y')
		instructions();
	else puts("\n\n");

	mywins = hiswins = 0;

	printf( "Choose the level of your oppponent:\n");
	printf( "Type 'b' for beginner, or 'i' for intermediate,\n");
	printf( "or anything else to play an expert... ?");
	level='e';
	if((c = tolower(getchar()))=='b') level='b';
	else if(c=='i') level='i';

	printf( "\n\nI'll play white, and you'll play red.\n\n");

newgame:
	exinit();
	first = 1;
	go[5]=NIL;

	printf("Let's throw the dice to see who goes first:\n");

throw:	roll();
	printf("I throw: ");
	sleep(15);
	printf("%d, and you throw: ",die1);
	sleep(15);
	printf("%d ... ",die2);
	sleep(15);
	if (die1 == die2) {
		printf("Tied up, so let's try again:\n");
		goto throw;
	}
	printf("%s go first...\n\n", die1 > die2 ? "I" : "You");
	if (die2 > die1) goto nowhmove;
	
whitesmv:
	if (!first) roll();	/* skip roll on first move */
	order();
	first = 0;		/* after this, not first roll anymore */
#if H19
	printf( "\nWhite rolls \33p %d,%d \33q... ",die1,die2);
#else
	printf( "\nWhite rolls %d,%d ... ",die1,die2);
#endif
	if (!chkmov(white,red)) {
		printf("White has no moves. Foo.\n");
		goto nowhmove;
	}
	printf( "White's move is: ");
	if(nextmove(white,red)==NIL)goto nowhmove;
	if(piececount(white,0,24)==0) {
	  printf( "\7White wins!\n");
	  printf("\nGame score:   Me: %d,  You: %d\n",++mywins,hiswins);
	  if (mywins == hiswins)
		printf("All tied up! Who's gonna pull ahead?\n");
	  else if (mywins > 2 && !hiswins)
		printf ("It's about time you managed to win a game!\n");
	  else switch(nrand(1) % 5) {
		case 4:
		case 0:	printf("Aren't you ashamed? You've been beaten ");
			printf("by a computer...\n");
			break;
		case 1:	printf("Not bad for a dumb program, eh?\n");
			break;
		case 2:	printf("Better luck next time, sucker....\n");
			break;
		case 3:	printf("Maybe you'd better stick to checkers?\n");
			break;
	  }
	  goto again;
	}

nowhmove:
	prtbrd();
	if (!first) roll();		/* skip roll on first move */
	first = 0;			/* after this, not 1st roll anymore */

retry:
	order();

#if H19
	printf( "Your roll is \33p %d, %d \33q ... ",die1,die2);
#else
	printf( "Your roll is %d,%d ... ",die1,die2);
#endif
	if (!chkmov(red,white)) {
		printf("Sorry, you have no possible moves.\n");
		printf("Type any key to let me make my next move: ");
		getchar();
		goto whitesmv;
	}

	printf("your move? ");
	while (!kbhit()) nrand(1);   /* Exercize the random generator while */
	gets(s);		     /* waiting for a move to be made...    */

	if (ignore_sp(s) == '?' || ignore_sp(s) == 'h') {
	  puts("\nType either a list of moves, separated by spaces, or:\n");
	  puts("	q -- to quit the current game\n");
	  puts("	b -- to see the board\n");
	  puts("	? -- to see these options (h does the same thing)\n");
	  puts("Do you want to see the long instructions? ");
	  if (tolower(getchar()) == 'y') instructions();
	  puts("\n");
	  goto retry;
	}

	if (ignore_sp(s) == 'q') {	/* check for quit command */
		puts("Are you sure you want to quit? ");
		if (tolower(getchar()) == 'y') goto again;
		putchar('\n');
		goto retry;
	}

	if (ignore_sp(s) == 'b') {	/* check for print board command */
		prtbrd();
		goto retry;
	}
					/* check for null move */
	if( ignore_sp(s) != '-' && !isdigit(ignore_sp(s))) {
	    printf("You MUST make a move!\n");
	    goto retry;
	}

	for (i = 0; s[i]; i++)		/* convert commas to spaces */
	 if (s[i] == ',') s[i] = ' ';

	n=sscanf(s,"%d%d%d%d%d",&go[0],&go[1],&go[2],&go[3],&go[4]);

	if( (die1 != die2 && n > 2) || n > 4) {
	    printf( "You've made too many moves;\n");
	    goto retry;
	}

	if( ((die1 != die2) && n < 2) || ((die1 == die2) && n < 4)) {
	    if (n == cntred()) goto moveok;
	    printf("Are you sure you can't make all the moves? ");
	    if (tolower(getchar()) != 'y') {
		printf("\nOK, then let's try it again:\n");
		goto retry;
	    }
	    putchar('\n');
	}

moveok:	go[n]=NIL;
	if(*s=='-'){
	    go[0]= -go[0];
	    t=die1;
	    die1=die2;
	    die2=t;
	}
	for(k=0; k<n; k++) {
	    if(0<=go[k] && go[k]<=24)continue;
	    else{
		printf( "Sorry, move %d is illegal.\n",go[k]);
		goto retry;
	    }
	}
	if(play(red,white,go))goto retry;
	if(piececount(red,0,24)==0){
	    printf( "\n\7Red wins.\n");
	    printf("\nGame score:   Me: %d,  You: %d\n",mywins,++hiswins);
	    if (mywins == hiswins)
		printf("Looks like now we're dead even...tiebreaker time!\n");
	    else if (hiswins > 2 && !mywins)
		printf("See? I'm not THAT stupid after all!\n");
	    else switch(nrand(1) % 5) {
		case 0:	printf( "Congratulations! You've just defeated a ");
			printf( "dumb machine.\n");
			break;
		case 1:	printf("Sigh...at least I put up a good fight, eh?\n");
			break;
		case 2: printf("Darn....I'll get you next time, human!\n");
			break;
		case 3:	printf("Mumble...those unlucky rolls will do it ");
			printf("every time!\n");
			break;
		case 4:	printf("Not bad... for a dumb human!\n");
	    }
  again:    printf("\nWould you like to play another game? ");
	    if (tolower(getchar()) != 'y') exit();
	    printf("\n\n");
	    goto newgame;
	}
	goto whitesmv;
}

char ignore_sp(s)	/* return first non-whitespace char at s: */
char *s;
{
	char c;
	while (isspace(c = *s++));
	return tolower(c);
}

play(player,playee,pos)
int *player,*playee,pos[];
{
	int k,n,die,ipos;
	for(k=0;k<player[0];k++){  /*blots on player[0] must be moved first*/
	    if(pos[k]==NIL)break;
	    if(pos[k]!=0){
		printf( "\nPiece on BAR (position 0) must be moved first\n");
		return(-1);
	    }
	}
	for(k=0;(ipos=pos[k])!=NIL;k++){
	    die=k?die2:die1;
	    n=25-ipos-die;
	    if(player[ipos]==0)goto badmove;
	    if(n>0 && playee[n]>=2)goto badmove;
	    if(n<=0){
		if(piececount(player,0,18)!=0)goto badmove;
		if((ipos+die)!=25 &&
		    piececount(player,19,24-die)!=0)goto badmove;
	    }
	    player[ipos]--;
	    player[ipos+die]++;
	}
	for(k=0;pos[k]!=NIL;k++){
	    die=k?die2:die1;
	    n=25-pos[k]-die;
	    if(n>0 && playee[n]==1){
		playee[n]=0;
		playee[0]++;
	    }
	}
	return(0);

badmove:
	printf( "Move %d is not legal. Enter all moves again:\n",ipos);
	while(k--){
	    die=k?die2:die1;
	    player[pos[k]]++;
	    player[pos[k]+die]--;
	}
	prtbrd();
	return(-1);
}

cntred()			/* count up number of red's pieces */
{
	int i, count;
	for (count = 0, i = 0; i < 25; i++) count += red[i];
	return count;
}

chkmov(player,playee)		/* return true if player has possible move */
int *player, *playee;
{
	int k;
	imoves=0;
	movegen(player,playee);
	if(die1!=die2) {
		k = die1; 	die1 = die2;	die2 = k;
		movegen(player,playee);
		k = die1;	die1 = die2;	die2 = k;
	}
	return imoves;
}

nextmove(player,playee)
int *player,*playee;
{
	int k;
	if(die1!=die2){
	k=die1;  die1=die2;  die2=k;
	}
	k=strategy(player,playee);		/*select kth possible move*/
	prtmov(k);
	update(player,playee,k);
	return(0);
}

prtmov(k)
int k;
{
	int commaf;
	int n;
	int i;
	commaf = 0;
	if(k==NIL)printf( " No move possible\n");
	else for(n=0;n<4;n++) {
	    if(moves[k].pos[n]==NIL)break;
	    if (commaf) putchar(',');
	    i = 25-moves[k].pos[n];
	    if (i == 0 || i == 25)
		printf( " BAR (%d)",moves[k].mov[n]);
	    else
		printf( " %d (%d)",i,moves[k].mov[n]);
	    commaf = 1;
	}
	putchar('\n');
}

update(player,playee,k)
int *player,*playee,k;
{
	int n,t;
	for(n=0;n<4;n++){
	    if(moves[k].pos[n]==NIL)break;
	    player[moves[k].pos[n]]--;
	    player[moves[k].pos[n]+moves[k].mov[n]]++;
	    t=25-moves[k].pos[n]-moves[k].mov[n];
	    if(t>0 && playee[t]==1){
		playee[0]++;
		playee[t]--;
	    }
	}
}

piececount(player,startrow,endrow)
int *player,startrow,endrow;
{
	int sum;
	sum=0;
	while(startrow<=endrow)
	sum+=player[startrow++];
	return(sum);
}

roll()
{
	int temp;
	die1=(nrand(1)>>8)%6+1;
	die2=(nrand(1)>>8)%6+1;
}

order()			/* place the two dice in descending order */
{
	int temp;
	if (die1 < die2) {		/* first die always bigger */
		temp = die1;
		die1 = die2;
		die2 = temp;
	}
}


movegen(mover,movee)
int *mover,*movee;
{
	int k;
	for(i=0;i<=24;i++){
		count=0;
		if(mover[i]==0)continue;
		if((k=25-i-die1)>0 && movee[k]>=2)
		    if(mover[0]>0)break;
		    else continue;
		if(k<=0){
		    if(piececount(mover,0,18)!=0)break;
		    if((i+die1)!=25 &&
		        piececount(mover,19,24-die1)!=0)break;
		}
		mover[i]--;
		mover[i+die1]++;
		count=1;
		for(j=0;j<=24;j++){
			if(mover[j]==0)continue;
			if((k=25-j-die2)>0 && movee[k]>=2)
			    if(mover[0]>0)break;
			    else continue;
			if(k<=0){
			    if(piececount(mover,0,18)!=0)break;
			    if((j+die2)!=25 &&
			        piececount(mover,19,24-die2)!=0)break;
			}
			mover[j]--;
			mover[j+die2]++;
			count=2;
			if(die1!=die2){
			    moverecord(mover);
			    if(mover[0]>0)break;
			    else continue;
			}
			for(l=0;l<=24;l++){
			    if(mover[l]==0)continue;
			    if((k=25-l-die1)>0 && movee[k]>=2)
				if(mover[0]>0)break;
				else continue;
			    if(k<=0){
				if(piececount(mover,0,18)!=0)break;
				if((l+die2)!=25 &&
				    piececount(mover,19,24-die1)!=0)break;
			    }
			    mover[l]--;
			    mover[l+die1]++;
			    count=3;
			    for(m=0;m<=24;m++){
				if(mover[m]==0)continue;
				if((k=25-m-die1)>=0 && movee[k]>=2)
				    if(mover[0]>0)break;
				    else continue;
				if(k<=0){
				    if(piececount(mover,0,18)!=0)break;
				    if( (m+die2) != 25 &&
				        piececount(mover,19,24-die1)!=0)break;
				}
				count=4;
				moverecord(mover);
				if(mover[0]>0)break;
			    }
			    if(count==3)moverecord(mover);
			    else{
				mover[l]++;
				mover[l+die1]--;
			    }
			    if(mover[0]>0)break;
			}
			if(count==2)moverecord(mover);
			else{
			    mover[j]++;
			    mover[j+die1]--;
			}
			if(mover[0]>0)break;
		}
		if(count==1)moverecord(mover);
		else{
		    mover[i]++;
		    mover[i+die1]--;
		}
		if(mover[0]>0)break;
	}
}

moverecord(mover)
int *mover;
{
	int t;
	if(imoves>=MAXIMOVES)goto undo;;
	for(t=0;t<=3;t++)
	    moves[imoves].pos[t]= NIL;
	switch(count){
case 4:
	    moves[imoves].pos[3]=m;
	    moves[imoves].mov[3]=die1;
case 3:
	    moves[imoves].pos[2]=l;
	    moves[imoves].mov[2]=die1;
case 2:
	    moves[imoves].pos[1]=j;
	    moves[imoves].mov[1]=die2;
case 1:
	    moves[imoves].pos[0]=i;
	    moves[imoves].mov[0]=die1;
	    imoves++;
	}
undo:
	switch(count){
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


strategy(player,playee)
int *player,*playee;
{
	int k,n,nn,bestval,moveval,prob;
	n=0;
	if(imoves==0)return(NIL);
	goodmoves[0]=NIL;
	bestval= -32000;
	for(k=0;k<imoves;k++){
	    if((moveval=eval(player,playee,k,&prob))<bestval)continue;
	    if(moveval>bestval){
		bestval=moveval;
		n=0;
	    }
	    if(n<MAXGMOV){
		goodmoves[n]=k;
		probmoves[n++]=prob;
	    }
	}
	if(level=='e' && n>1){
	    nn=n;
	    n=0;
	    prob=32000;
	    for(k=0;k<nn;k++){
		if((moveval=probmoves[k])>prob)continue;
		if(moveval<prob){
		    prob=moveval;
		    n=0;
		}
		goodmoves[n]=goodmoves[k];
		probmoves[n++]=probmoves[k];
	    }
	}
	return(goodmoves[(nrand(1)>>4)%n]);
}

eval(player,playee,k,prob)
int *player,*playee,k,*prob;
{
	int newtry[31],newother[31],*r,*q,*p,n,sum,first;
	int ii,lastwhite,lastred;
	*prob=sum=0;
	r=player+25;
	p=newtry;
	q=newother;
	while(player<r){
	    *p++= *player++;
	    *q++= *playee++;
	}
	q=newtry+31;
	for(p=newtry+25;p<q;) *p++ = 0;	/*zero out spaces for hit pieces*/
	for(n=0;n<4;n++){
	    if(moves[k].pos[n]==NIL)break;
	    newtry[moves[k].pos[n]]--;
	    newtry[ii=moves[k].pos[n]+moves[k].mov[n]]++;
	    if(ii<25 && newother[25-ii]==1){
		newother[25-ii]=0;
		newother[0]++;
		if(ii<=15 && level=='e')sum++;	/*hit if near other's home*/
	    }
	}
	for(lastred=0;newother[lastred]==0;lastred++);
	for(lastwhite=0;newtry[lastwhite]==0;lastwhite++);
	lastwhite=25-lastwhite;
	if(lastwhite<=6 && lastwhite<lastred)sum=1000;
	if(lastwhite<lastred && level=='e'
	    && lastwhite>6){			/*expert's running game.
						  First priority to get all
						  pieces into white's home*/
	    for(sum=1000;lastwhite>6;lastwhite--)
		sum=sum-lastwhite*newtry[25-lastwhite];
	}
	for(first=0;first<25;first++)
	    if(newother[first]!=0)break;	/*find other's first piece*/
	q=newtry+25;
	for(p=newtry+1;p<q;)if(*p++ > 1)sum++;	/*blocked points are good*/
	if(first>5){	/*only stress removing pieces if homeboard
			  cannot be hit
			*/
	    q=newtry+31;
	    p=newtry+25;
	    for(n=6;p<q;n--)
		sum += *p++ * n;	/*remove pieces, but just barely*/
	}
	if(level!='b'){
	    r=newtry+25-first;	/*singles past this point can't be hit*/
	    for(p=newtry+7;p<r;)
		if(*p++ == 1)sum--;	/*singles are bad after 1st 6 points
					  if they can be hit*/
	    q=newtry+3;
	    for(p=newtry;p<q;)sum-= *p++;  /*bad to be on 1st three points*/
	}

	for(n=1;n<=4;n++)
	    *prob += n*getprob(newtry,newother,6*n-5,6*n);
	return(sum);
}

instructions()
{
	printf( "\n\n");
	printf( "To play backgammon, type the numbers of the points\n");
	printf( "from which pieces are to be moved. Thus, if the\n");
	printf( "roll is '3,5', typing '2 6' will move a piece\n");
	printf( "from point 2 three spaces to point 5,\n");
	printf( "and a piece from point 6 forward to\n");
	printf( "point 11.  If the moves must be made in the\n");
	printf( "opposite order, the first character typed must\n");
	printf( "be a minus ('-'). Thus, typing\n");
	printf( "'-2 6' moves the piece on point 2\n");
	printf( "by 5, and the piece on point 6 by 3.\n");
	printf( "If you want to move a single piece several times,\n");
	printf( "the sequence of points from which it is to be\n");
	printf( "moved must be typed. Thus '14 17' will move\n");
	printf( "a piece from point 14 to point 17 and thence to\n");
	printf( "to point 22.\n");
	printf( "Instead of entering a move, you can instead type:\n");
	printf( "	q	to quit the game, or\n");
	printf( "	b	to print the board again.\n\n");

	printf( "\nType any key to continue instructions: "); getchar();

	printf( "\nIf a double is rolled, you should type four numbers.\n");
	printf( "Red pieces that have been removed from the board by\n");
	printf( "being hit by white are on point 0 and\n");
	printf( "must be brought in before any other move can be made.\n");
	printf( "White pieces that are hit are removed to point 25.\n");
	printf( "You will not be allowed to make an illegal move, or\n");
	printf( "to make too many moves. However, if you make too\n");
	printf( "few moves, the program does not care. In particular\n");
	printf( "you may skip your turn by typing a 'new-line'\n");
	printf( "all by itself.\n\n");
}

getprob(player,playee,start,finish)
int *player,*playee,start,finish;
{			/*returns the probability (times 102) that any
			  pieces belonging to 'player' and lying between
			  his points 'start' and 'finish' will be hit
			  by a piece belonging to playee
			*/
	int k,n,sum;
	sum=0;
	for(;start<=finish;start++){
	    if(player[start]==1){
		for(k=1;k<=12;k++){
		    if((n=25-start-k)<0)break;
		    if(playee[n]!=0)sum += probability[k];
		}
	    }
	}
	return(sum);
}

prtbrd()
{
	int k;
	printf( "\nWhite's Home (mine)\n");		/* first line */

	for(k=1;k<=6;k++)				/* top label line */
	    printf( "%4d",k);
	printf( "    ");
	for(k=7;k<=12;k++)printf( "%4d",k);
	putchar('\n' );

	for(k=1;k<=54;k++)putchar('_' );		/* delimiter line */
	putchar('\n' );

	numline(red,white,1,6);				/* numbers line */
	printf( "    ");
	numline(red,white,7,12);
	putchar('\n' );

	colorline(red,'R',white,'W',1,6);		/* letters line */
	printf( "    ");
	colorline(red,'R',white,'W',7,12);
	putchar('\n' );

	if(white[0]!=0)printf( "%28dW\n",white[0]);	/* white's bar */
	else putchar('\n' );

	if(red[0]!=0)printf( "%28dR\n",red[0]);		/* red's bar */
	else putchar('\n' );

	colorline(white,'W',red,'R',1,6);		/* letters line */
	printf( "    ");
	colorline(white,'W',red,'R',7,12);
	putchar('\n' );

	numline(white,red,1,6);				/* numbers line */
	printf( "    ");
	numline(white,red,7,12);
	putchar('\n' );

	for(k=1;k<=54;k++)putchar('_' );		/* delimiter line */
	putchar('\n' );

	for(k=24;k>=19;k--)printf( "%4d",k);		/* bottom label line */
	printf( "    ");
	for(k=18;k>=13;k--)printf( "%4d",k);

	printf( "\nRed's Home (yours)\n\n");		/* last line */
}

numline(upcol,downcol,start,fin)
int *upcol,*downcol,start,fin;
{
	int k,n;
	for(k=start;k<=fin;k++){
	    if((n=upcol[k])!=0 || (n=downcol[25-k])!=0)

#if H19
		if ((upcol[k] && upcol == red) ||
		    (downcol[25-k] && downcol == red))
			if (n < 10)
				printf("   \33p%1d\33q",n);
			else
				printf("  \33p%2d\33q",n);
		else
			printf("%4d",n);
#else
		printf("%4d",n);
#endif

	    else printf( "    ");
	}
}

colorline(upcol,c1,downcol,c2,start,fin)
int *upcol,*downcol,start,fin;
char c1,c2;
{
	int k;
	char c;
	for(k=start;k<=fin;k++){
	    c=' ';
	    if(upcol[k]!=0)c=c1;
	    if(downcol[25-k]!=0)c=c2;
#if H19
	if (c == 'R')
		printf("   \33p%c\33q",c);	/* make 'R' reverse video */
	else
#endif
	 	printf("   %c",c);
	}
}
