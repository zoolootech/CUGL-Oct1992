/*
	The game of "Hangman"

	Modified for BDS C by Leor Zolman, 2/82

	The dictionary is simply a text file of words and/or phrases,
	one word or phrase per line. Punctuation and spaces are OK; the
	program will display all punctuation and non-alphabetic characters
	of the string. You only have to guess the letters.

	Usage:
		A>hangman [dictname]

	If no dictionary name is given, the default HANGMAN.WDS is presumed.
*/

#include <bdscio.h>

#define MAXLEN 130
#define DICT "hangman.wds"
#define MAXERR 7
#define MINSCORE 0
#define MINLEN 7
#define stderr 4

char *dictfile;
int alive,lost;
char dict[BUFSIZ];
int dictlen;
char *wordlist[1000];
int errors, words;
char firstguess;

main(argc,argv) char **argv;
{
	int i;

	alive = lost = dictlen = errors = words = 0;

	if(argc==1) dictfile=DICT;
	else dictfile=argv[1];

	setup();
	printf("Read %d words from file %s\n\n",dictlen,dictfile);


	for(;;)
	{	startnew();
		while(alive>0)
		{	stateout();
			getguess();
		}
		words=words+1;
		if(lost) wordout();
		else youwon();
		pscore();
	}
}

setup()
{
	srand1("Ready to play hangman?");
	getchar(); putchar('\n');

	if((fopen(dictfile,dict))==ERROR) fatal("No dictionary");
	dictlen=readin();
	fclose(dict);
	words = 0;
}

int readin()		/* read in dictionary, count up words */
{
	int i;
	int count;
	char *wptr;
	char linebuf[MAXLINE];

	count = 0;	
	while (fgets(linebuf,dict))
	{
		if ((wptr = sbrk(strlen(linebuf)+1)) == ERROR)
			break;
		wordlist[count] = wptr;
		for (i = 0; linebuf[i] != '\n'; i++)
			wptr[i] = linebuf[i];
		wptr[i] = '\0';
		count++;
	}
	return count;
}

char word[MAXLEN], alph[MAXLEN], realword[MAXLEN];

startnew()
{	int i;
	for(i=0; i<MAXLEN; i++) word[i] = alph[i] = realword[i] = 0;
	getword();
	alive=MAXERR;
	lost=0;
	firstguess = 1;
}

stateout()
{	int i;
	char noneflag;

	noneflag = 1;
	for(i=0;i<26;i++)
		if(alph[i]!=0) {
			if (noneflag) {
				printf("\nGuesses: ");
				noneflag = 0;
			}
			putchar(alph[i]);
		}

	if (firstguess) {
		printf("\nNew word: %s ",word);
		firstguess = 0;
	}
	else
		printf("\nWord: %s ",word);

	printf("\nerrors: %d/%d\n",MAXERR-alive,MAXERR);
}

getguess()
{	char c;
	int ok,i;

	ok = 0;
loop:
	printf("Guess: ");

	
	c = tolower(getchar());

	if (c < 'a' || c > 'z') {
		puts("\nGuesses are letters between a and z.\n");
		goto loop;
	} else putchar('\n');

	if(alph[c-'a']!=0)
	{	printf("You guessed that letter already.\n");
		goto loop;
	}
	else alph[c-'a']=c;

	for(i=0;realword[i]!=0;i++)
		if(tolower(realword[i])==c)
		{	word[i]=realword[i];
			ok=1;
		}

	if(ok==0)
	{	alive--;
		errors=errors+1;
		if(alive<=0) lost=1;
		return;
	}
	for(i=0;word[i]!=0;i++)
		if(word[i]=='.') return;
	alive=0;
	lost=0;
	return;
}

wordout()
{
	errors=errors+2;
	printf("The answer was %s, you BLEW it!\n",realword);
}

youwon()
{
	printf("\7You win, the word is:  %s\n",realword);
}

fatal(s) char *s;
{
	fprintf(stderr,"%s\n",s);
	exit(1);
}

getword()
{	int i,j;
	char c;

	if (dictlen == 0) {
		printf("All available words have been used. Bye!\n");
		exit(0);
	}

	j = rand() % dictlen;
	strcpy(realword,wordlist[j]);
	wordlist[j] = wordlist[dictlen-1];
	dictlen--;		

	for(j=0; j<strlen(realword); j++)
		if ((c = tolower(realword[j])) < 'a' || c > 'z')
			if (c == '.')	word[j] = '.' + 0x80;
			else		word[j] = c;
		else
			word[j] = '.';
}

pscore()
{
	if(words!=0)
	    printf("You average number of errors for %d word%s is %d.%d\n",
		words,
		words == 1 ? "" : "s",
		errors/words,
		(errors * 100 / words) % 100);
}
