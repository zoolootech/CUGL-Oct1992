/*
	"Go Fish"

	Modified for BDS C by Leor Zolman	2/82
*/

#include <bdscio.h>
#define stderr 4

/*	Through, `my' refers to the program, `your' to the player */

#define CTYPE 13
#define CTSIZ (CTYPE+1)
#define DECK 52
#define NOMORE 0
#define DOUBTIT (-1);

/* data structures */

int debug;

char myhand[CTSIZ];
char yourhand[CTSIZ];
char deck[DECK];
int nextcd;
int proflag;

/* utility and output programs */

shuffle(){
	/* shuffle the deck, and reset nextcd */
	/* uses the random number generator `rand' in the C library */
	/* assumes that `srand' has already been called */

	register i,j;

	for( i=0; i<DECK; ++i ) deck[i] = (i%13)+1;  /* seed the deck */

	for( i=DECK; i>0; --i ){ /* select the next card at random */
		deck[i-1] = choose( deck, i );
		}

	nextcd = 0;
	}

choose( a, n ) char a[]; {
	/* pick and return one at random from the n choices in a */
	/* The last one is moved to replace the one chosen */
	register j, t;

	if( n <= 0 ) error( "null choice" );

	j = rand() % n;
	t = a[j];
	a[j] = a[n-1];
	return(t);
	}

draw() {
	if( nextcd >= DECK ) return( NOMORE );
	return( deck[nextcd++] );
	}

error( s ) char *s; {
	fprintf( stderr, "error: " );
	fprintf( stderr, s );
	exit( 1 );
	}

empty( h ) char  h[CTSIZ]; {
	register i;

	for( i=1; i<=CTYPE; ++i ){
		if( h[i] != 0 && h[i] != 4 ) return( 0 );
		}
	return( i );
	}

mark( hand,cd ) char hand[CTSIZ]; {
	if( cd != NOMORE ){
		++hand[cd];
		if( hand[cd] > 4 ){
			error( "mark overflow" );
			}
		}
	return( cd );
	}

deal( hand, n ) char hand[CTSIZ]; {
	while( n-- ){
		if( mark( hand, draw() ) == NOMORE ) error( "deck exhausted" );
		}
	}

char *cname[14];

stats(){
	register i, ct, b;

	if( proflag ) printf( "Pro level\n" );
	b = ct = 0;

	for( i=1; i<=CTYPE; ++i ){
		if( myhand[i] == 4 ) ++b;
		else ct += myhand[i];
		}

	if( b ){
		printf( "My books: " );
		for( i=1; i<=CTYPE; ++i ){
			if( myhand[i] == 4 ) printf( "%s ", cname[i] );
			}
		printf( "\n" );
		}

	printf( "%d cards in my hand, %d in the pool\n", ct, DECK-nextcd );
	printf( "You ask me for: " );
	}

phand( h ) char h[CTSIZ]; {
	register i, j;

	j = 0;

	for( i = 1; i<= CTYPE; ++i ){
		if( h[i] == 4 ) {
			++j;
			continue;
			}
		if( h[i] ){
			register k;
			k = h[i];
			while( k-- ) printf( "%s ", cname[i] );
			}
		}

	if( j ){
		printf( "+ Books of " );
		for( i=1; i<=CTYPE; ++i ){
			if( h[i] == 4 ) printf( "%s ", cname[i] );
			}
		}

	printf( "\n" );
	}

main( argc, argv ) char * argv[]; { 
	/* initialize shuffling, ask for instructions, play game, die */
	register c;

	cname[0] = "NOMORE!!!";
	cname[1] = "A";
	cname[2] = "2";
	cname[3] = "3";
	cname[4] = "4";
	cname[5] = "5";
	cname[6] = "6";
	cname[7] = "7";
	cname[8] = "8";
	cname[9] = "9";
	cname[10] = "10";
	cname[11] = "J";
	cname[12] = "Q";
	cname[13] = "K";

	if( argc > 1 && argv[1][0] == '-') debug++;


	srand1("Instructions? ");

	if( (c=tolower(getchar())) != 'n' )
		instruct();
	putchar('\n');
	game();
	}

/*	print instructions */

instruct(){
printf("\n`Go Fish' is a children's card game. The object is to accumulate\n");
printf("`books' of 4 cards with the same face value. Players alternate\n");
printf("turns; each turn begins with one player selecting a card from his\n");
printf("hand, and asking the other player for all cards of that value.\n");
printf("If the other player has one or more cards of that face value\n");
printf("in his hand, he gives them to the first player, and the first\n");
printf(" player makes another request.\n");
printf("Eventually, the first player asks for a card which is not in the\n");
printf("second player's hand: he replies `GO FISH!'\n");
printf("The first player then draws a card from the pool of undealt cards.\n");
printf("If this is the card he had last requested, he draws again.\n");
printf("When a book is made, either through drawing or requesting, the\n");
printf("cards are laid down and no further action takes place with that\n");
printf("face value.\n");
printf("To play the computer, simply make guesses by typing\n");
printf("a, 2, 3, 4, 5, 6, 7, 8, 9, 10, j, q, or k when asked.\n");
printf("Hitting return gives you information about the size of\n");
printf("my hand and the pool, and tells you about my books.\n");
printf("Saying `p' as a first guess puts you into `pro' level;\n");
printf("The default is pretty dumb!   G O O D   L U C K!\n");
}

/* play game */

game(){
	int i,j;
	shuffle();

	for (i = 0; i < CTSIZ; i++) {
		myhand[i] = 0;
		yourhand[i] = 0;
	}
		
	deal( myhand, 7 );
	deal( yourhand, 7 );

	start( myhand );

	for(;;){

		register g;


		/* you make repeated guesses */

		for(;;) {
			printf( "your hand is: " );
			phand( yourhand );
			printf( "you ask me for: " );
			if( !move( yourhand, myhand, g=guess(), 0 ) ) break;
			printf( "Guess again\n" );
			}

		/* I make repeated guesses */

		for(;;) {
			if( (g=myguess()) != NOMORE ){
				printf( "I ask you for: %s\n", cname[g] );
				}
			if( !move( myhand, yourhand, g, 1 ) ) break;
			printf( "I get another guess\n" );
			}
		}
	}

/*	reflect the effect of a move on the hands */

move( hs, ht, g, v ) char hs[CTSIZ], ht[CTSIZ]; {
	/* hand hs has made a guess, g, directed towards ht */
	/* v on indicates that the guess was made by the machine */
	register d;
	char *sp, *tp;

	sp = tp = "I";
	if( v ) tp = "You";
	else sp = "You";

	if( g == NOMORE ){
		d = draw();
		if( d == NOMORE ) score();
		else {

			printf( "Empty Hand\n" );
			if( !v ) printf( "You draw %s\n", cname[d] );
			mark( hs, d );
			}
		return( 0 );
		}

	if( !v ) heguessed( g );

	if( hs[g] == 0 ){
		if( v ) error( "Rotten Guess" );
		printf( "You don't have any %s's\n", cname[g] );
		return(1);
		}

	if( ht[g] ){ /* successful guess */

		printf("%s have ",tp);
		switch(ht[g]) {
			case 1: printf("one "); break;
			case 2:	printf("two "); break;
			case 3: printf("three "); break;
			case 4: printf("four "); break;
		}
		printf("%s%s\n",cname[g], ht[g] > 1 ? "'s" : "");

		hs[g] += ht[g];
		ht[g] = 0;
		if( hs[g] == 4 ) madebook(g);
		return(1);
		}

	/* GO FISH! */

	printf( "%s say \"GO FISH!\"\n", tp );

	newdraw:
	d = draw();
	if( d == NOMORE ) {
		printf( "No more cards\n" );
		return(0);
		}
	mark( hs, d );
	if( !v ) printf( "You draw %s\n", cname[d] );
	if( hs[d] == 4 ) madebook(d);
	if( d == g ){
		printf( "%s drew the guess, so draw again\n", sp );
		if( !v ) hedrew( d );
		goto newdraw;
		}
	return( 0 );
	}

madebook( x ){
	printf( "Made a book of %s's\n", cname[x] );
	}

score(){
	register my, your, i;

	my = your = 0;

	printf( "The game is over.\nMy books: " );

	for( i=1; i<=CTYPE;++i ){
		if( myhand[i] == 4 ){
			++my;
			printf( "%s ", cname[i] );
			}
		}

	printf( "\nYour books: " );

	for( i=1; i<=CTYPE;++i ){
		if( yourhand[i] == 4 ){
			++your;
			printf( "%s ", cname[i] );
			}
		}

	printf( "\n\nI have %d, you have %d\n", my, your );

	printf( "\n%s win!!!\n", my>your?"I":"You" );
	exit(0);
	}

#define G(x) { if(go) goto err;  else go = x; }

guess(){
	/* get the guess from the tty and return it... */
	register g, go;

	go = 0;

	for(;;) {
		switch( g = getchar() ){

		case 'p':
		case 'P':
			++proflag;
			continue;

		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			G(g-'0');
			continue;

		case 'a':
		case 'A':
			G(1);
			continue;

		case '1':
			G(10);
			continue;

		case '0':
			if( go != 10 ) goto err;
			continue;

		case 'J':
		case 'j':
			G(11);
			continue;

		case 'Q':
		case 'q':
			G(12);
			continue;

		case 'K':
		case 'k':
			G(13);
			continue;

		case '\n':
			if( empty( yourhand ) ) return( NOMORE );
			if( go == 0 ){
				stats();
				continue;
				}
			return( go );

		case ' ':
		case '\t':
			continue;

		err:
		default:
			while( g != '\n' ) g = getchar();
			printf( "what?\n" );
			continue;
			}
		}
	}

/*	the program's strategy appears from here to the end */

char try[100];
char ntry;
char haveguessed[CTSIZ];

char hehas[CTSIZ];

start( h ) char h[CTSIZ]; {
	;
	}

hedrew( d ){
	++hehas[d];
	}

heguessed( d ){
	++hehas[d];
	}

myguess(){

	register i, lg, t;

	if( empty( myhand ) ) return( NOMORE );

	/* make a list of those things which i have */
	/* leave off any which are books */
	/* if something is found that he has, guess it! */

	ntry = 0;
	for( i=1; i<=CTYPE; ++i ){
		if( myhand[i] == 0 || myhand[i] == 4 ) continue;
		try[ntry++] = i;
		}

	if( !proflag ) goto random;

	/* get ones he has, if any */

	for( i=0; i<ntry; ++i ){
		if( hehas[try[i]] ) {
			i = try[i];
			goto gotguess;
			}
		}

	/* is there one that has never been guessed; if so, guess it */
	lg = 101;
	for( i=0; i<ntry; ++i ){
		if( haveguessed[try[i]] < lg ) lg = haveguessed[try[i]];
		}
	/* remove all those not guessed longest ago */

	t = 0;
	for( i=0; i<ntry; ++i ){
		if( haveguessed[try[i]] == lg ) try[t++] = try[i];
		}
	ntry = t;
	if( t <= 0 ) error( "bad guessing loop" );

	random:
	i = choose( try, ntry );  /* make a random choice */

	gotguess:  /* do bookkeeping */

	hehas[i] = 0;  /* he won't anymore! */
	for( t=1; t<=CTYPE; ++t ){
		if( haveguessed[t] ) --haveguessed[t];
		}
	haveguessed[i] = 100;  /* will have guessed it */
	return(i);

	}

