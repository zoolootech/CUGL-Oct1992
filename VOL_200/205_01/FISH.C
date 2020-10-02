
/*
HEADER:                 CUG205.00;
TITLE:                  GOFISH;
DATE:                   09/24/86; 
DESCRIPTION:            "Children's card game, based
                        on CUG102 by Leor Zolman.";
KEYWORDS:               games, cards;
SYSTEM:                 MS-DOS;
FILENAME:               GOFISH.C;
WARNINGS:               "The author claims the copyright to this MS-DOS version
                        and authorizes non-commercial use only.";
AUTHORS:                 Michael M. Yokoyama;
COMPILERS:              Microsoft;
*/

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*  Throughout the program, `my' refers to the computer, `your' to the human */

#define CTYPE 13
#define CTSIZ (CTYPE+1)
#define DECK 52
#define NOMORE 0
#define DOUBTIT (-1);

/* data structures */
int debug;
void title();
char myhand[CTSIZ];
char yourhand[CTSIZ];
char deck[DECK];
int nextcd;
int proflag = 2;

/* utility and output programs */
shuffle()
{
  register i,j;
  long iths;
  time(&iths);
  srand(iths);
  for (i=0; i<DECK; ++i) deck[i] = (i%13)+1;  /* seed the deck */
  for (i=DECK; i>0; --i){ /* select the next card at random */
    deck[i-1] = choose(deck, i);
  }
  nextcd = 0;
}

choose(a, n) char a[];
{
  /* pick and return one at random from the n choices in a */
  /* The last one is moved to replace the one chosen */
  register j, t;
  if (n <= 0) error("Null choice");
  j = rand() % n;
  t = a[j];
  a[j] = a[n-1];
  return(t);
}

draw() {
  if (nextcd >= DECK) return(NOMORE);
  return(deck[nextcd++]);
}

error(s) char *s;
{
  fprintf(stderr, "Error:  ");
  fprintf(stderr, s);
  exit(1);
}

empty(h) char  h[CTSIZ];
{
  register i;
  for (i=1; i<=CTYPE; ++i){
    if (h[i] != 0 && h[i] != 4) return(0);
  }
  return(i);
}

mark(hand,cd) char hand[CTSIZ];
{
  if (cd != NOMORE){
    ++hand[cd];
    if (hand[cd] > 4){
      error("Mark overflow");
    }
  }
  return(cd);
}

deal(hand, n) char hand[CTSIZ];
{
  while (n--){
    if (mark(hand, draw()) == NOMORE) error("Deck exhausted");
  }
}

char *cname[14];
stats(){
  register i, ct, b;
  if (proflag) printf("Professional level\n");
  b = ct = 0;
  for (i=1; i<=CTYPE; ++i){
    if (myhand[i] == 4) ++b;
    else ct += myhand[i];
  }
  if (b){
    printf("My books:  ");
    for (i=1; i<=CTYPE; ++i){
      if (myhand[i] == 4) printf("%s ", cname[i]);
    }
    printf("\n");
  }
  printf("%d cards in my hand, %d in the pool\n", ct, DECK-nextcd);
  printf("You ask me for:  ");
}

phand(h) char h[CTSIZ];
{
  register i, j;
  j = 0;
  for (i = 1; i<= CTYPE; ++i){
    if (h[i] == 4) {
      ++j;
      continue;
    }
    if (h[i]){
      register k;
      k = h[i];
      while (k--) printf("%s ", cname[i]);
    }
  }
  if (j){
    printf("+ Books of ");
    for (i=1; i<=CTYPE; ++i){
      if (h[i] == 4) printf("%s ", cname[i]);
    }
  }
  printf("\n");
}

main(argc, argv) char * argv[];
{
  long iths;
  /* initialize shuffling, ask for instructions, play game, die */
  register c;
  cname[0]   = "Nomore!!!";
  cname[1]   = "A";
  cname[2]   = "2";
  cname[3]   = "3";
  cname[4]   = "4";
  cname[5]   = "5";
  cname[6]   = "6";
  cname[7]   = "7";
  cname[8]   = "8";
  cname[9]   = "9";
  cname[10]  ="10";
  cname[11]  = "J";
  cname[12]  = "Q";
  cname[13]  = "K";
  if (argc > 1 && argv[1][0] == '-') debug++;
  time(&iths);
  srand(iths);
  title();
  printf("Do you want the rules? (y/n)");
  if ((c=tolower(getche())) == 'y')
    instruct();
  printf("%c[2J",27);
start:
  game();
  exit(0);
}

/*      print instructions */
instruct()
{
  int c;
  printf("%c[2J                                G O   F I S H\n\n\n",27);
  printf("     GO FISH is a card game for children.\n\n");
  printf("     The object is to accumulate `books' of cards, a `book' being a \n");
  printf("     set of four cards of a kind such as four Jacks, four Aces, etc.\n\n");
  printf("     Players alternate turns.  The game starts with each player being \n");
  printf("     dealt seven cards.  The rest of the cards are placed in a `pond'.\n\n");
  printf("     After looking at the cards in his hand, a player asks his opponent \n");
  printf("     for any cards of a particular number . . . For example, he might \n");
  printf("     ask \"Do you have any Fives?\"\n\n");
  printf("     If the opponent has one or more cards of the card requested, he \n");
  printf("     must give them to the first player.  If he does not have the card \n");
  printf("     requested, he says `Go fish'.\n\n");
  printf("     The first player then draws a card from the `pond' of undealt cards.  \n");
  printf("     If he draws the card he had last requested, he draws again.  If \n");
  printf("     the card is not the one requested, the next player takes a turn.\n\n\n");
  printf("Press any key to see the next screen . . .");
  c = getche();
  printf("%c[2J\n     Once a player makes a book, no further action takes place with that \n",27);
  printf("     face value.\n\n");
  printf("     PLAYING AGAINST THE COMPUTER:\n\n");
  printf("     To play GO FISH on the computer, simply make guesses by typing\n\n");
  printf("          a, 2, 3, 4, 5, 6, 7, 8, 9, 10, j, q, or k.\n\n");
  printf("     Hit return to get information about the size of my hand, my books,\n");
  printf("     and the size of the pool.\n\n");
  printf("          G O O D   L U C K !\n\n\n\n\n\n\n\n\n");
  printf("Press any key to start the game . . .\n");
  c = getche();
  printf("%c[2J",27);
}

/* play game */
game()
{
  int i,j;
  shuffle();
  for (i = 0; i < CTSIZ; i++) {
    myhand[i] = 0;
    yourhand[i] = 0;
  }
  deal(myhand, 7);
  deal(yourhand, 7);
  start(myhand);
  while (1) {
    register g;
    /* you make repeated guesses */
    while (1) {
      printf("\nYour hand is:  ");
      phand(yourhand);
      printf("You ask me for:  ");
      if (!move(yourhand, myhand, g=guess(), 0)) break;
      printf("Guess again\n");
    }
    /* I make repeated guesses */
    while (1) {
      if ((g=myguess()) != NOMORE){
        printf("I ask you for:  %s\n", cname[g]);
      }
      if (!move(myhand, yourhand, g, 1)) break;
      printf("I get another guess\n");
    }
  }
}

/*      reflect the effect of a move on the hands */
move(hs, ht, g, v) char hs[CTSIZ], ht[CTSIZ];
{
  /* hand hs has made a guess, g, directed towards ht */
  /* v on indicates that the guess was made by the machine */
  register d;
  char *sp, *tp;
  sp = tp = "I";
  if (v) tp = "You";
  else sp = "You";
  if (g == NOMORE){
    d = draw();
    if (d == NOMORE) score();
    else {
      printf("No Cards\n");
      if (!v) printf("You draw %s\n", cname[d]);
      mark(hs, d);
    }
    return(0);
  }
  if (!v) heguessed(g);
  if (hs[g] == 0){
    if (v) error("Cheat!!!  ");
    printf("You don't have any %s's\n", cname[g]);
    return(1);
  }
  if (ht[g]){ /* successful guess */
    printf("%s have ",tp);
    switch(ht[g]) {
    case 1 :
      printf("one ");
      break;
    case 2 :
      printf("two ");
      break;
    case 3 :
      printf("three ");
      break;
    case 4 :
      printf("four ");
      break;
    }
    printf("%s%s\n",cname[g], ht[g] > 1 ? "'s" : "");
    hs[g] += ht[g];
    ht[g] = 0;
    if (hs[g] == 4) madebook(g);
    return(1);
  }
  /* GO FISH! */
  printf("%s say \"Go fish!\"\n\n", tp);
newdraw:
  d = draw();
  if (d == NOMORE) {
    printf("No more cards\n");
    return(0);
  }
  mark(hs, d);
  if (!v) printf("You draw %s\n", cname[d]);
  if (hs[d] == 4) madebook(d);
  if (d == g){
    printf("%s drew the guess\n%s get another draw\n", sp,sp);
    if (!v) hedrew(d);
    goto newdraw;
  }
  return(0);
}

madebook(x){
  printf("Made a book of %s's\n", cname[x]);
}
score(){
  register my, your, i;
  my = your = 0;
  printf("\nGame over\n\nMy books:   ");
  for (i=1; i<=CTYPE;++i){
    if (myhand[i] == 4){
      ++my;
      printf("%s ", cname[i]);
    }
  }
  printf("\nYour books: ");
  for (i=1; i<=CTYPE;++i){
    if (yourhand[i] == 4){
      ++your;
      printf("%s ", cname[i]);
    }
  }
  printf("\n\nI have %d, you have %d\n", my, your);
  printf("\n%s win!!!\n", my>your?"I":"You");
}

#define G(x) { if (go) goto err;  else go = x; }
guess(){
  /* get the guess from the tty and return it... */
  int g, go;
  go = 0;
  while (1) {
    switch(tolower(g = getchar())){
    case '2' :
    case '3' :
    case '4' :
    case '5' :
    case '6' :
    case '7' :
    case '8' :
    case '9' :
      G(g-'0');
      continue;
    case 'a' :
    case 'A' :
      G(1);
      continue;
    case '1' :
      G(10);
      continue;
    case 'J' :
    case 'j' :
      G(11);
      continue;
    case 'Q' :
    case 'q' :
      G(12);
      continue;
    case 'K' :
    case 'k' :
      G(13);
      continue;
    case '\n' :
      if (empty(yourhand)) return(NOMORE);
      if (go == 0){
        stats();
        continue;
      }
      return(go);
    case 'x' :
      exit(0);
      break;
err:
    default :
      /*
                        while (g != '\n') g = getche();
*/
      printf("What?\n");
      continue;
    }
  }
}

/*      the program's strategy appears from here to the end */
char try[100];
char ntry;
char haveguessed[CTSIZ];
char hehas[CTSIZ];
start(h) char h[CTSIZ];
{
  ;
}

hedrew(d){
  ++hehas[d];
}

heguessed(d){
  ++hehas[d];
}

myguess(){
  register i, lg, t;
  if (empty(myhand)) return(NOMORE);
  /* make a list of those things which i have */
  /* leave off any which are books */
  /* if something is found that he has, guess it! */
  ntry = 0;
  for (i=1; i<=CTYPE; ++i){
    if (myhand[i] == 0 || myhand[i] == 4) continue;
    try[ntry++] = i;
  }
  if (!proflag) goto random;
  /* get ones he has, if any */
  for (i=0; i<ntry; ++i){
    if (hehas[try[i]]) {
      i = try[i];
      goto gotguess;
    }
  }
  /* is there one that has never been guessed; if so, guess it */
  lg = 101;
  for (i=0; i<ntry; ++i){
    if (haveguessed[try[i]] < lg) lg = haveguessed[try[i]];
  }
  /* remove all those not guessed longest ago */
  t = 0;
  for (i=0; i<ntry; ++i){
    if (haveguessed[try[i]] == lg) try[t++] = try[i];
  }
  ntry = t;
  if (t <= 0) error("Bad guessing loop");
random:
  i = choose(try, ntry);  /* make a random choice */
gotguess:  /* do bookkeeping */
  hehas[i] = 0;  /* he won't anymore! */
  for (t=1; t<=CTYPE; ++t){
    if (haveguessed[t]) --haveguessed[t];
  }
  haveguessed[i] = 100;  /* will have guessed it */
  return(i);
}

void title()
{
  printf("%c[2J",27);
  printf("\n                                            G O   F I S H\n\n");
  printf("                 *\n");
  printf("                / \\\n");
  printf("               /   \\\n");
  printf("       O      /     \\\n");
  printf("       -     /       \\\n");
  printf("     /\\ /\\  /         \\\n");
  printf("     \\ =  \\/           \\\n");
  printf("      / \\               \\\n");
  printf("     /   \\               \\\n");
  printf("    -     -               \\\n");
  printf("========================   \\\n");
  printf("   \|\|             \|\|        \\\n");
  printf("-----------------------------\\----------------------------------------------\n");
  printf("                              \\\n");
  printf("         >>>>O                 \\                       O<<<<\n");
  printf("                                \\\n");
  printf("                                 *   O<<<<\n");
  printf("                       >>>>O\n\n\n");
}
